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
#include "BattlePayMgr.h"
#include "BattlePayDataStore.h"
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

auto SendStartPurchaseResponse = [](WorldSession* session, BattlePay::Purchase const& purchase, BattlePay::Error const& result) -> void
{
    WorldPackets::BattlePay::StartPurchaseResponse response;
    response.PurchaseID = purchase.PurchaseID;
    response.ClientToken = purchase.ClientToken;        // 这个token从哪来的...
    response.PurchaseResult = result;
    session->SendPacket(response.Write());
};


auto SendPurchaseUpdate = [](WorldSession* session, BattlePay::Purchase const& purchase, uint32 result) -> void
{
    WorldPackets::BattlePay::PurchaseUpdate packet;
    WorldPackets::BattlePay::BattlePayPurchase data;
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


auto MakePurchase = [](ObjectGuid targetCharacter, uint32 clientToken, uint32 productID, WorldSession* session) -> void
{
    if (!session || !session->GetBattlepayMgr()->IsAvailable())
        return;

    auto mgr = session->GetBattlepayMgr();

    auto player = session->GetPlayer();
    if (!player)
        return;

    auto accountID = session->GetAccountId();

    BattlePay::Purchase purchase;
    purchase.ProductID = productID;
    purchase.ClientToken = clientToken;
    purchase.TargetCharacter = targetCharacter;
    purchase.Status = BattlePay::UpdateStatus::Loading;
    purchase.DistributionId = mgr->GenerateNewDistributionId();

    auto characterInfo = sWorld->GetCharacterInfo(targetCharacter);
    if (!characterInfo)
    {
        SendStartPurchaseResponse(session, purchase, BattlePay::Error::PurchaseDenied);
        return;
    }

    if (characterInfo->AccountId != accountID)
    {
        SendStartPurchaseResponse(session, purchase, BattlePay::Error::PurchaseDenied);
        return;
    }

    if (!sBattlePayDataStore->ProductExist(productID))
    {
        SendStartPurchaseResponse(session, purchase, BattlePay::Error::PurchaseDenied);
        return;
    }

    BattlePay::ProductGroup* group = sBattlePayDataStore->GetProductGroupForProductId(purchase.ProductID);
    if (!group)
    {
        SendStartPurchaseResponse(session, purchase, BattlePay::Error::PurchaseDenied);
        return;
    }

    auto const& product = sBattlePayDataStore->GetProduct(purchase.ProductID);
    purchase.CurrentPrice = product.CurrentPriceFixedPoint;

    mgr->RegisterStartPurchase(purchase);

    auto accountBalance = mgr->GetBalance(group->TokenType);

    auto purchaseData = mgr->GetPurchase();

    if (!accountBalance)
    {
        SendStartPurchaseResponse(session, *purchaseData, BattlePay::Error::InsufficientBalance);
        return;
    }

    if (accountBalance < purchaseData->CurrentPrice)
    {
        SendStartPurchaseResponse(session, *purchaseData, BattlePay::Error::InsufficientBalance);
        return;
    }

    if (!product.Items.empty())
    {
        if (product.Items.size() > GetBagsFreeSlots(player))
        {
            std::ostringstream data;
            data << sObjectMgr->GetTrinityString(BattlePay::String::NotEnoughFreeBagSlots, session->GetSessionDbLocaleIndex());
            player->SendCustomMessage("Store purchase failed ", data);
            SendStartPurchaseResponse(session, *purchaseData, BattlePay::Error::PurchaseDenied);
            return;
        }
    }

    if (!product.ScriptName.empty())
    {
        std::string reason;
        if (!sScriptMgr->BattlePayCanBuy(session, product, reason))
        {
            std::ostringstream data;
            data << reason;
            player->SendCustomMessage("Store purchase failed ", data);
            SendStartPurchaseResponse(session, *purchaseData, BattlePay::Error::PurchaseDenied);
            return;
        }
    }

    for (auto itr : product.Items)
    {
        if (mgr->AlreadyOwnProduct(itr.ItemID))
        {
            std::ostringstream data;
            data << sObjectMgr->GetTrinityString(BattlePay::String::YouAlreadyOwnThat, session->GetSessionDbLocaleIndex());;
            player->SendCustomMessage("Store purchase failed ", data);
            SendStartPurchaseResponse(session, *purchaseData, BattlePay::Error::PurchaseDenied);
            return;
        }
    }

    purchaseData->PurchaseID = mgr->GenerateNewPurchaseID();
    purchaseData->ServerToken = urand(0, 0xFFFFFFF);
    //purchaseData->Status = BattlePay::UpdateStatus::Ready; ?

    SendStartPurchaseResponse(session, *purchaseData, BattlePay::Error::Ok);
    SendPurchaseUpdate(session, *purchaseData, BattlePay::Error::Ok);

    WorldPackets::BattlePay::ConfirmPurchase confirmPurchase;
    confirmPurchase.PurchaseID = purchaseData->PurchaseID;
    confirmPurchase.ServerToken = purchaseData->ServerToken;
    session->SendPacket(confirmPurchase.Write());
};

// 给客户端返回货品列表...货品列表要从哪里获取？
void WorldSession::HandleGetPurchaseListQuery(WorldPackets::BattlePay::GetPurchaseListQuery & packet)
{
    WorldPackets::BattlePay::PurchaseListResponse resp;
    // TODO:
    resp.Result = 0;

    SendPacket(resp.Write());
}

void WorldSession::HandleBattlePayQueryClassTrialResult(WorldPackets::BattlePay::BattlePayQueryClassTrialResult & packet)
{
    // TODO:
}

void WorldSession::HandleBattlePayTrialBoostCharacter(WorldPackets::BattlePay::BattlePayTrialBoostCharacter & packet)
{
    // TODO:
}

void WorldSession::HandleBattlePayPurchaseUnkResponse(WorldPackets::BattlePay::BattlePayPurchaseUnkResponse & packet)
{
    // TODO:
}

void WorldSession::HandleBattlePayPurchaseDetailsResponse(WorldPackets::BattlePay::BattlePayPurchaseDetailsResponse & packet)
{
    // TODO:
}

void WorldSession::HandleUpdateVasPurchaseStates(WorldPackets::BattlePay::UpdateVasPurchaseStates & packet)
{
    // TODO:
}

void WorldSession::HandleGetProductList(WorldPackets::BattlePay::GetProductList & packet)
{
    if (!GetBattlepayMgr()->IsAvailable())
        return;

    GetBattlepayMgr()->SendProductList();
    GetBattlepayMgr()->SendPointsBalance();
}

void WorldSession::HandleBattlePayDistributionAssign(WorldPackets::BattlePay::DistributionAssignToTarget & packet)
{
    if (!GetBattlepayMgr()->IsAvailable())
        return;
    GetBattlepayMgr()->AssignDistributionToCharacter(packet.TargetCharacter, packet.DistributionID,
        packet.ProductID, packet.SpecializationID, packet.ChoiceID);
}

void WorldSession::HandleBattlePayStartPurchase(WorldPackets::BattlePay::StartPurchase & packet)
{
    MakePurchase(packet.TargetCharacter, packet.ClientToken, packet.ProductID, this);
}

// TODO: 为什么和 StartPurchase 一样处理？
void WorldSession::HandleBattlePayPurchaseProduct(WorldPackets::BattlePay::PurchaseProduct & packet)
{
    MakePurchase(packet.TargetCharacter, packet.ClientToken, packet.ProductID, this);
}

void WorldSession::HandleBattlePayConfirmPurchase(WorldPackets::BattlePay::ConfirmPurchaseResponse & packet)
{
    // TODO:
}

void WorldSession::HandleBattlePayAckFailedResponse(WorldPackets::BattlePay::BattlePayAckFailedResponse & packet)
{
    // TODO:
}

// Promotion 推广消息...
void WorldSession::SendDisplayPromo(int32 promotionID)
{
    // TODO:
}
