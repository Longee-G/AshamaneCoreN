/*
 * Copyright (C) 2008-2018 TrinityCore <https://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "WorldSession.h"
#include "Bag.h"
#include "BattlepayMgr.h"
#include "BattlepayDataStore.h"
#include "ScriptMgr.h"

// <WIP>....

// 这个定义的是一个什么？delegate？
// 获取player的背包还有多少可用的空间...
auto GetBagsFreeSlots = [](Player* player) -> uint32
{
    uint32 freeBagSlots = 0;
    // 背包格子的定义没看明白需要进行分析...
    // 这个是放`背包`的格子的索引... 每个player可以有4个放`背包`的格子...
    for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
    {
        if (auto bag = player->GetBagByPos(i))
            freeBagSlots += bag->GetFreeSlots();
        // 计算缺省的背包（就是一开始固定的背包，16格的那个，后期可以扩展）
        uint8 inventoryEnd = INVENTORY_SLOT_ITEM_START + player->GetInventorySlotCount();
        for (int i = INVENTORY_SLOT_ITEM_START; i < inventoryEnd; i++)
            if (!player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                ++freeBagSlots;
    }

    return freeBagSlots;
};

// 定义c++的delegate需要操作符`[]`吗？ 这个
// 为什么要用这种方式来定义函数... 直接定义函数它不香吗？

auto SendStartPurchaseResponse = [](WorldSession* session, Battlepay::Purchase const& purchase, Battlepay::Error const& result) -> void
{
    WorldPackets::Battlepay::StartPurchaseResponse response;
    response.PurchaseID = purchase.PurchaseID;
    response.ClientToken = purchase.ClientToken;        // 这个token从哪来的...
    response.PurchaseResult = result;
    session->SendPacket(response.Write());
};


auto SendPurchaseUpdate = [](WorldSession* session, Battlepay::Purchase const& purchase, uint32 result) -> void
{
    WorldPackets::Battlepay::PurchaseUpdate packet;
    WorldPackets::Battlepay::BattlepayPurchase data;
    data.PurchaseID = purchase.PurchaseID;
    data.UnkLong = 0;
    data.UnkLong2 = 0;
    data.Status = purchase.Status;
    data.ResultCode = result;
    data.ProductID = purchase.ProductID;
    data.UnkInt = purchase.ServerToken;
    data.WalletName = session->GetBattlepayMgr()->GetDefaultWalletName();
    packet.Purchase.emplace_back(data);
    session->SendPacket(packet.Write());
};

// 给客户端返回货品列表...货品列表要从哪里获取？
void WorldSession::HandleGetPurchaseListQuery(WorldPackets::Battlepay::GetPurchaseListQuery & packet)
{
    WorldPackets::Battlepay::PurchaseListResponse resp;
    // TODO:
    // resp.Result = 0;
    SendPacket(resp.Write());
}

void WorldSession::HandleUpdateVasPurchaseStates(WorldPackets::Battlepay::UpdateVasPurchaseStates & packet)
{
    // TODO: `SMSG_ENUM_VAS_PURCHASE_STATES_RESPONSE`
    //
    //WorldPackets::Battlepay::BattlepayVasPurchaseList resp;
    //SendPacket(resp.Write());
}

void WorldSession::HandleBattlepayDistributionAssign(WorldPackets::Battlepay::DistributionAssignToTarget & packet)
{
    GetBattlepayMgr()->AssignDistributionToCharacter(packet.TargetCharacter, packet.DistributionID,
        packet.ProductID, packet.SpecializationID, packet.ChoiceID);
}

void WorldSession::HandleGetProductList(WorldPackets::Battlepay::GetProductList & packet)
{
    GetBattlepayMgr()->SendProductList();
    GetBattlepayMgr()->SendPointsBalance();
}

auto MakePurchase = [](ObjectGuid targetCharacter, uint32 clientToken, uint32 productID, WorldSession* session) -> void
{
    if (!session || !session->GetBattlepayMgr()->IsAvailable())
        return;

    auto mgr = session->GetBattlepayMgr();

    auto player = session->GetPlayer();
    if (!player)
        return;

    auto accountID = session->GetAccountId();

    Battlepay::Purchase purchase;
    purchase.ProductID = productID;
    purchase.ClientToken = clientToken;
    purchase.TargetCharacter = targetCharacter;
    purchase.Status = Battlepay::UpdateStatus::Loading;
    purchase.DistributionId = mgr->GenerateNewDistributionId();

    auto characterInfo = sWorld->GetCharacterInfo(targetCharacter);
    if (!characterInfo)
    {
        SendStartPurchaseResponse(session, purchase, Battlepay::Error::PurchaseDenied);
        return;
    }

    if (characterInfo->AccountId != accountID)
    {
        SendStartPurchaseResponse(session, purchase, Battlepay::Error::PurchaseDenied);
        return;
    }

    if (!sBattlepayDataStore->ProductExist(productID))
    {
        SendStartPurchaseResponse(session, purchase, Battlepay::Error::PurchaseDenied);
        return;
    }

    Battlepay::ProductGroup* group = sBattlepayDataStore->GetProductGroupForProductId(purchase.ProductID);
    if (!group)
    {
        SendStartPurchaseResponse(session, purchase, Battlepay::Error::PurchaseDenied);
        return;
    }

    auto const& product = sBattlepayDataStore->GetProduct(purchase.ProductID);
    purchase.CurrentPrice = product.CurrentPriceFixedPoint;

    mgr->RegisterStartPurchase(purchase);

    auto accountBalance = mgr->GetBalance(group->TokenType);

    auto purchaseData = mgr->GetPurchase();

    if (!accountBalance)
    {
        SendStartPurchaseResponse(session, *purchaseData, Battlepay::Error::InsufficientBalance);
        return;
    }

    if (accountBalance < purchaseData->CurrentPrice)
    {
        SendStartPurchaseResponse(session, *purchaseData, Battlepay::Error::InsufficientBalance);
        return;
    }

    if (!product.Items.empty())
    {
        if (product.Items.size() > GetBagsFreeSlots(player))
        {
            std::ostringstream data;
            data << sObjectMgr->GetTrinityString(Battlepay::String::NotEnoughFreeBagSlots, session->GetSessionDbLocaleIndex());
            player->SendCustomMessage("Store purchase failed ", data);
            SendStartPurchaseResponse(session, *purchaseData, Battlepay::Error::PurchaseDenied);
            return;
        }
    }

    if (!product.ScriptName.empty())
    {
        std::string reason;
        if (!sScriptMgr->BattlepayCanBuy(session, product, reason))
        {
            std::ostringstream data;
            data << reason;
            player->SendCustomMessage("Store purchase failed ", data);
            SendStartPurchaseResponse(session, *purchaseData, Battlepay::Error::PurchaseDenied);
            return;
        }
    }

    for (auto itr : product.Items)
    {
        if (mgr->AlreadyOwnProduct(itr.ItemID))
        {
            std::ostringstream data;
            data << sObjectMgr->GetTrinityString(Battlepay::String::YouAlreadyOwnThat, session->GetSessionDbLocaleIndex());;
            player->SendCustomMessage("Store purchase failed ", data);
            SendStartPurchaseResponse(session, *purchaseData, Battlepay::Error::PurchaseDenied);
            return;
        }
    }

    purchaseData->PurchaseID = mgr->GenerateNewPurchaseID();
    purchaseData->ServerToken = urand(0, 0xFFFFFFF);
    //purchaseData->Status = Battlepay::UpdateStatus::Ready; ?

    SendStartPurchaseResponse(session, *purchaseData, Battlepay::Error::Ok);
    SendPurchaseUpdate(session, *purchaseData, Battlepay::Error::Ok);

    WorldPackets::Battlepay::ConfirmPurchase confirmPurchase;
    confirmPurchase.PurchaseID = purchaseData->PurchaseID;
    confirmPurchase.ServerToken = purchaseData->ServerToken;
    session->SendPacket(confirmPurchase.Write());
};

void WorldSession::HandleBattlepayStartPurchase(WorldPackets::Battlepay::StartPurchase & packet)
{
    MakePurchase(packet.TargetCharacter, packet.ClientToken, packet.ProductID, this);
}

// TODO: 为什么和 StartPurchase 一样处理？
void WorldSession::HandleBattlepayPurchaseProduct(WorldPackets::Battlepay::PurchaseProduct & packet)
{
    MakePurchase(packet.TargetCharacter, packet.ClientToken, packet.ProductID, this);
}

// 确认购买
void WorldSession::HandleBattlepayConfirmPurchase(WorldPackets::Battlepay::ConfirmPurchaseResponse & packet)
{
    if (!GetBattlepayMgr()->IsAvailable())
        return;
    packet.ClientCurrentPriceFixedPoint /= Battlepay::g_CurrencyPrecision;
    auto purchase = GetBattlepayMgr()->GetPurchase();
    if (!purchase)
        return;

    if (purchase->Lock)
    {
        SendPurchaseUpdate(this, *purchase, Battlepay::Error::PurchaseDenied);
        return;
    }

    if (purchase->ServerToken != packet.ServerToken || !packet.ConfirmPurchase || purchase->CurrentPrice != packet.ClientCurrentPriceFixedPoint)
    {
        SendPurchaseUpdate(this, *purchase, Battlepay::Error::PurchaseDenied);
        return;
    }

    auto player = GetPlayer();
    if (!player)
    {
        SendPurchaseUpdate(this, *purchase, Battlepay::Error::PurchaseDenied);
        return;
    }

    Battlepay::ProductGroup* group = sBattlepayDataStore->GetProductGroupForProductId(purchase->ProductID);
    if (!group)
    {
        SendPurchaseUpdate(this, *purchase, Battlepay::Error::PurchaseDenied);
        return;
    }

    if (player->GetTokenBalance(group->TokenType) < static_cast<int64>(purchase->CurrentPrice))
    {
        SendPurchaseUpdate(this, *purchase, Battlepay::Error::PurchaseDenied);
        return;
    }

    purchase->Lock = true;
    purchase->Status = Battlepay::UpdateStatus::Finish;

    auto const& product = sBattlepayDataStore->GetProduct(purchase->ProductID);
    if (!product.ScriptName.empty())
    {
        std::string reason;
        if (!sScriptMgr->BattlepayCanBuy(this, product, reason))
        {
            std::ostringstream data;
            data << reason;
            player->SendCustomMessage("Store purchase failed ", data);
            SendPurchaseUpdate(this, *purchase, Battlepay::Error::PaymentFailed);
            return;
        }
    }

    if (!product.Items.empty())
    {
        if (product.Items.size() > GetBagsFreeSlots(player))
        {
            std::ostringstream data;
            data << sObjectMgr->GetTrinityString(Battlepay::String::NotEnoughFreeBagSlots, GetSessionDbLocaleIndex());
            player->SendCustomMessage("Store purchase failed ", data);
            SendStartPurchaseResponse(this, *purchase, Battlepay::Error::PurchaseDenied);
            return;
        }
    }

    for (auto itr : product.Items)
    {
        if (GetBattlepayMgr()->AlreadyOwnProduct(itr.ItemID))
        {
            std::ostringstream data;
            data << sObjectMgr->GetTrinityString(Battlepay::String::YouAlreadyOwnThat, GetSessionDbLocaleIndex());
            player->SendCustomMessage("Store purchase failed ", data);
            SendStartPurchaseResponse(this, *purchase, Battlepay::Error::PurchaseDenied);
            return;
        }
    }

    SendPurchaseUpdate(this, *purchase, Battlepay::Error::Other);

    if (player->ChangeTokenCount(group->TokenType, -int64(purchase->CurrentPrice), Battlepay::BattlepayCustomType::BattlepayShop, purchase->ProductID))
        GetBattlepayMgr()->ProcessDelivery(purchase);
}

void WorldSession::HandleBattlepayAckFailedResponse(WorldPackets::Battlepay::BattlepayAckFailedResponse & packet)
{
    if (!GetBattlepayMgr()->IsAvailable())
        return;
    // TODO:
}


// 有可能是用来购买角色升级服务的...
void WorldSession::HandleBattlepayQueryClassTrialResult(WorldPackets::Battlepay::BattlepayQueryClassTrialResult & packet)
{
    if (!GetBattlepayMgr()->IsAvailable())
        return;

    // TODO:
}

// 开始进行角色升级...
void WorldSession::HandleBattlepayTrialBoostCharacter(WorldPackets::Battlepay::BattlepayTrialBoostCharacter & packet)
{
    if (!GetBattlepayMgr()->IsAvailable())
        return;
    // TODO:
}

// 
void WorldSession::HandleBattlepayPurchaseDetailsResponse(WorldPackets::Battlepay::BattlepayPurchaseDetailsResponse & packet)
{
    WorldPackets::Battlepay::BattlepayPurchaseUnk resp;
    resp.UnkInt = 0;
    resp.Key = "";
    resp.UnkByte = packet.UnkByte;
    SendPacket(resp.Write());
}

void WorldSession::HandleBattlepayPurchaseUnkResponse(WorldPackets::Battlepay::BattlepayPurchaseUnkResponse & packet)
{
    auto purchase = GetBattlepayMgr()->GetPurchase();
    SendPurchaseUpdate(this, *purchase, Battlepay::Error::Ok);
}

// Promotion 推广消息... 这个消息和Bpay的商店相关，没有这个消息，商店打开会一直出现loading
void WorldSession::SendDisplayPromo(int32 promotionID)
{
    SendPacket(WorldPackets::Battlepay::DisplayPromotion(promotionID).Write());

    if (!GetBattlepayMgr()->IsAvailable())
        return;

    // 有可能是这个消息没有应答导致无法打开    
    WorldPackets::Battlepay::DistributionListResponse packet;
    SendPacket(packet.Write());
    /*
   auto player = GetPlayer();
   auto const& product = sBattlepayDataStore->GetProduct(109);
   WorldPackets::Battlepay::DistributionListResponse packet;
   packet.Result = Battlepay::Error::Ok;

   WorldPackets::Battlepay::BattlepayDistributionObject data;
   data.TargetPlayer;
   data.DistributionID = GetBattlepayMgr()->GenerateNewDistributionId();
   data.PurchaseID = GetBattlepayMgr()->GenerateNewPurchaseID();
   data.Status = Battlepay::DistributionStatus::BATTLE_PAY_DIST_STATUS_AVAILABLE;
   data.ProductID = 109;
   data.TargetVirtualRealm = 0;
   data.TargetNativeRealm = 0;
   data.Revoked = false;

   WorldPackets::Battlepay::BattlepayProductItem pProduct;
   pProduct.ProductID = product.ProductID;
   pProduct.Flags = product.Flags;
   pProduct.Type = product.Type;
   //pProduct.UnkBits Optional<uint16> ;
   //pProduct.UnkInt1 = 0;
   //pProduct.DisplayId = 0;
   //pProduct.ItemId = 0;
   //pProduct.UnkInt4 = 0;
   //pProduct.UnkInt5 = 0;
   //pProduct.UnkString = "";
   //pProduct.UnkBit = false;

   for (auto& itr : product.Items)
   {
       WorldPackets::Battlepay::ProductItem pItem;
       pItem.ID = itr.ID;
       pItem.ItemID = product.Items.size() > 1 ? 0 : itr.ItemID; ///< Disable tooltip for packs (client handle only one tooltip).
       pItem.Quantity = itr.Quantity;
       //pItem.UnkInt1 = 0;
       //pItem.UnkInt2 = 0;
       //pItem.UnkByte = 0;
       pItem.HasPet = GetBattlepayMgr()->AlreadyOwnProduct(itr.ItemID);
       pItem.PetResult = itr.PetResult;

       auto dataP = GetBattlepayMgr()->WriteDisplayInfo(itr.DisplayInfoID, GetSessionDbLocaleIndex());
       if (std::get<0>(dataP))
       {
           pItem.DisplayInfo = boost::in_place();
           pItem.DisplayInfo = std::get<1>(dataP);
       }

       pProduct.Items.emplace_back(pItem);
   }

   auto dataP = GetBattlepayMgr()->WriteDisplayInfo(product.DisplayInfoID, GetSessionDbLocaleIndex());
   if (std::get<0>(dataP))
   {
       pProduct.DisplayInfo = boost::in_place();
       pProduct.DisplayInfo = std::get<1>(dataP);
   }

   data.Product = boost::in_place();
   data.Product = pProduct;

   packet.DistributionObject.emplace_back(data);

   SendPacket(packet.Write());
   */
}

// 请求代币信息？
void WorldSession::HandleRequestConsumptionConversionInfo(WorldPackets::Battlepay::RequestConsumptionConversionInfo & packet)
{
    // TODO: response SMSG_CONSUMPTION_CONVERSION_INFO_RESPONSE or SMSG_CONSUMPTION_CONVERSION_RESULT


}
