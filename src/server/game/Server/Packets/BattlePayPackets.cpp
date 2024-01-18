/*
* Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
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

#include "BattlepayPackets.h"

using namespace WorldPackets;

ByteBuffer& operator<<(ByteBuffer& data, Battlepay::ProductDisplayInfo const& displayInfo)
{
    data.WriteBit(displayInfo.IconFileID.is_initialized());
    data.WriteBit(displayInfo.ModelSceneID.is_initialized());
    data.WriteBits(displayInfo.Name1.length(), 10);
    data.WriteBits(displayInfo.Name2.length(), 10);
    data.WriteBits(displayInfo.Name3.length(), 13);
    data.WriteBits(displayInfo.Name4.length(), 13);
    data.WriteBit(displayInfo.Flags.is_initialized());
    data.WriteBit(displayInfo.UnkInt1.is_initialized());
    data.WriteBit(displayInfo.UnkInt2.is_initialized());
    data.WriteBit(displayInfo.UnkInt3.is_initialized());
    data.FlushBits();

    data << static_cast<uint32>(displayInfo.Visuals.size());

    if (displayInfo.IconFileID)
        data << *displayInfo.IconFileID;

    if (displayInfo.ModelSceneID)
        data << *displayInfo.ModelSceneID;

    data.WriteString(displayInfo.Name1);
    data.WriteString(displayInfo.Name2);
    data.WriteString(displayInfo.Name3);
    data.WriteString(displayInfo.Name4);

    if (displayInfo.Flags)
        data << *displayInfo.Flags;
    
    if (displayInfo.UnkInt1)
        data << *displayInfo.UnkInt1;

    if (displayInfo.UnkInt2)
        data << *displayInfo.UnkInt2;

    if (displayInfo.UnkInt3)
        data << *displayInfo.UnkInt3;

    for (auto const& itr : displayInfo.Visuals)
    {
        data.WriteBits(itr.Title.length(), 10);
        data.FlushBits();
        data << itr.DisplayID;
        data << itr.ModelSceneID;
        data.WriteString(itr.Title);
    }
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, Battlepay::BattlepayProduct const& info)
{
    data << info.ProductID;
    data << info.NormalPriceFixedPoint;
    data << info.CurrentPriceFixedPoint;
    data << uint32(info.ProductIDs.size());
    data << info.Flags;
    data << uint32(info.UnkInts.size());

    for (auto z1 : info.ProductIDs)
        data << uint32(z1);

    for (auto z2 : info.UnkInts)
        data << uint32(z2);

    data.WriteBits(info.ChoiceType, 7);
    data.WriteBit(info.DisplayInfo.is_initialized());
    data.FlushBits();
    if (info.DisplayInfo)
        data << *info.DisplayInfo;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, Battlepay::BattlepayProductItem const& product)
{
    data << product.ProductID;
    data << product.Type;
    data << product.ItemID;
    data << product.UnkInt1;
    data << product.UnkInt2;
    data << product.UnkInt3;
    data << product.UnkInt4;
    data << product.UnkInt5;

    data.WriteBits(product.UnkString.size(), 8);
    data.WriteBit(product.UnkBit);
    data.WriteBit(product.UnkBits.is_initialized());
    data.WriteBits(product.Items.size(), 7);
    data.WriteBit(product.DisplayInfo.is_initialized());
    if (product.UnkBits)
        data.WriteBits(*product.UnkBits, 4);

    data.FlushBits();


    for (auto const& productItem : product.Items)
    {
        data << productItem.Entry;
        data << productItem.UnkByte;
        data << productItem.ItemID;
        data << productItem.Quantity;
        data << productItem.UnkInt1;
        data << productItem.UnkInt2;

        data.WriteBit(productItem.HasPet);
        data.WriteBit(productItem.PetResult.is_initialized());
        data.WriteBit(productItem.DisplayInfo.is_initialized());

        if (productItem.PetResult)
            data.WriteBits(*productItem.PetResult, 4);

        data.FlushBits();

        if (productItem.DisplayInfo)
            data << *productItem.DisplayInfo;
    }


    data.WriteString(product.UnkString);

    if (product.DisplayInfo)
        data << *product.DisplayInfo;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, Battlepay::BattlepayDistributionObject const& object)
{
    data << object.DistributionID;

    data << object.Status;
    data << object.ProductID;

    data << object.TargetPlayer;
    data << object.TargetVirtualRealm;
    data << object.TargetNativeRealm;

    data << object.PurchaseID;
    data.WriteBit(object.Product.is_initialized());
    data.WriteBit(object.Revoked);
    data.FlushBits();

    if (object.Product)
        data << *object.Product;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, Battlepay::BattlepayPurchase const& purchase)
{
    data << purchase.PurchaseID;
    data << purchase.Status;
    data << purchase.ResultCode;
    data << purchase.ProductID;
    data << purchase.UnkLong;
    data << purchase.UnkLong2;
    data << purchase.UnkInt;
    
    data.WriteBits(purchase.WalletName.length(), 8);
    data.WriteString(purchase.WalletName);

    return data;
}

WorldPacket const* Battlepay::PurchaseListResponse::Write()
{
    _worldPacket << Result;
    _worldPacket << static_cast<uint32>(Purchase.size());
    for (auto const& purchaseData : Purchase)
        _worldPacket << purchaseData;

    return &_worldPacket;
}

WorldPacket const* Battlepay::DistributionListResponse::Write()
{
    _worldPacket << Result;
    _worldPacket.WriteBits(DistributionObject.size(), 11);
    for (BattlepayDistributionObject const& objectData : DistributionObject)
        _worldPacket << objectData;

    return &_worldPacket;
}

WorldPacket const* Battlepay::DistributionUpdate::Write()
{
    _worldPacket << DistributionObject;

    return &_worldPacket;
}

WorldPacket const* Battlepay::ProductListResponse::Write()
{
    _worldPacket << Result;
    _worldPacket << ProductList.CurrencyID;

    _worldPacket << static_cast<uint32>(ProductList.Products.size());
    _worldPacket << static_cast<uint32>(ProductList.Items.size());
    _worldPacket << static_cast<uint32>(ProductList.Groups.size());
    _worldPacket << static_cast<uint32>(ProductList.Shop.size());

    // product ... 
    for (auto const& product : ProductList.Products)
        _worldPacket << product;

    // product Item ...
    for (auto const& item : ProductList.Items)
        _worldPacket << item;

    // product group 
    for (auto const& group : ProductList.Groups)
    {
        _worldPacket << group.GroupID;
        _worldPacket << group.IconFileDataID;
        _worldPacket << group.DisplayType;
        _worldPacket << group.Ordering;
        _worldPacket << group.Flags;

        _worldPacket.WriteBits(group.Name.length(), 8);
        _worldPacket.WriteBits(group.IsAvailableDescription.length() , 24);
        _worldPacket.WriteString(group.Name);

        if (!group.IsAvailableDescription.empty())
            _worldPacket.WriteString(group.IsAvailableDescription);
    }

    // shop product ...
    for (BattlepayShopEntry const& shopData : ProductList.Shop)
    {
        _worldPacket << shopData.EntryID;
        _worldPacket << shopData.GroupID;
        _worldPacket << shopData.ProductID;
        _worldPacket << shopData.Ordering;
        _worldPacket << shopData.VasServiceType;
        _worldPacket << shopData.StoreDeliveryType;
        if (_worldPacket.WriteBit(shopData.DisplayInfo.is_initialized()))
        {
            _worldPacket.FlushBits();
            _worldPacket << *shopData.DisplayInfo;
        }
    }

    return &_worldPacket;
}

void Battlepay::StartPurchase::Read()
{
    _worldPacket >> ClientToken;
    _worldPacket >> ProductID;
    _worldPacket >> TargetCharacter;
}

void Battlepay::PurchaseProduct::Read()
{
    _worldPacket >> ClientToken;
    _worldPacket >> ProductID;
    _worldPacket >> TargetCharacter;

    uint32 strlen1 = _worldPacket.ReadBits(6);
    uint32 strlen2 = _worldPacket.ReadBits(12);
    WowSytem = _worldPacket.ReadString(strlen1);
    PublicKey = _worldPacket.ReadString(strlen2);
}

WorldPacket const* Battlepay::StartPurchaseResponse::Write()
{
    _worldPacket << PurchaseID;
    _worldPacket << PurchaseResult;
    _worldPacket << ClientToken;

    return &_worldPacket;
}

WorldPacket const* Battlepay::BattlepayAckFailed::Write()
{
    _worldPacket << PurchaseID;
    _worldPacket << PurchaseResult;
    _worldPacket << ClientToken;

    return &_worldPacket;
}

WorldPacket const* Battlepay::PurchaseUpdate::Write()
{
    _worldPacket << static_cast<uint32>(Purchase.size());
    for (auto const& purchaseData : Purchase)
        _worldPacket << purchaseData;

    return &_worldPacket;
}

WorldPacket const* Battlepay::ConfirmPurchase::Write()
{
    _worldPacket << PurchaseID;
    _worldPacket << ServerToken;

    return &_worldPacket;
}

void Battlepay::ConfirmPurchaseResponse::Read()
{
    ConfirmPurchase = _worldPacket.ReadBit();
    _worldPacket >> ServerToken;
    _worldPacket >> ClientCurrentPriceFixedPoint;
}

void Battlepay::BattlepayAckFailedResponse::Read()
{
    _worldPacket >> ServerToken;
}

WorldPacket const* Battlepay::DeliveryEnded::Write()
{
    _worldPacket << DistributionID;

    _worldPacket << static_cast<int32>(item.size());
    for (Item::ItemInstance const& itemData : item)
        _worldPacket << itemData;

    return &_worldPacket;
}

WorldPacket const* Battlepay::BattlepayDeliveryStarted::Write()
{
    _worldPacket << DistributionID;

    return &_worldPacket;
}

WorldPacket const* Battlepay::UpgradeStarted::Write()
{
    _worldPacket << CharacterGUID;

    return &_worldPacket;
}

void Battlepay::DistributionAssignToTarget::Read()
{
    _worldPacket >> ProductID;
    _worldPacket >> DistributionID;
    _worldPacket >> TargetCharacter;
    _worldPacket >> SpecializationID;
    _worldPacket >> ChoiceID;
}

ByteBuffer& operator<<(ByteBuffer& data, Battlepay::VasPurchaseData const& purchase)
{
    data << purchase.PlayerGuid;
    data << purchase.UnkInt;
    data << purchase.UnkInt2;
    data << purchase.UnkLong;
    data.WriteBits(purchase.ItemIDs.size(), 2);
    data.FlushBits();

    for (auto const& itemID : purchase.ItemIDs)
        data << itemID;

    return data;
}

WorldPacket const* Battlepay::BattlepayVasPurchaseStarted::Write()
{
    _worldPacket << UnkInt;
    _worldPacket << VasPurchase;

    return &_worldPacket;
}

WorldPacket const* Battlepay::CharacterClassTrialCreate::Write()
{
    _worldPacket << Result;
    return &_worldPacket;
}

WorldPacket const* Battlepay::BattlepayCharacterUpgradeQueued::Write()
{
    _worldPacket << Character;
    _worldPacket << static_cast<uint32>(EquipmentItems.size());
    for (auto const& item : EquipmentItems)
        _worldPacket << item;

    return &_worldPacket;
}

void Battlepay::BattlepayTrialBoostCharacter::Read()
{
    _worldPacket >> Character;
    _worldPacket >> SpecializationID;
}

WorldPacket const* Battlepay::BattlepayVasPurchaseList::Write()
{
    _worldPacket.WriteBits(VasPurchase.size(), 6);
    _worldPacket.FlushBits();
    for (auto const& itr : VasPurchase)
        _worldPacket << itr;

    return &_worldPacket;
}

WorldPacket const* Battlepay::BattlepayPurchaseDetails::Write()
{
    _worldPacket << UnkInt;
    _worldPacket << VasPurchaseProgress;
    _worldPacket << UnkLong;

    _worldPacket.WriteBits(Key.length(), 6);
    _worldPacket.WriteBits(Key2.length(), 6);
    _worldPacket.WriteString(Key);
    _worldPacket.WriteString(Key2);

    return &_worldPacket;
}

WorldPacket const* Battlepay::BattlepayPurchaseUnk::Write()
{
    _worldPacket << UnkByte;
    _worldPacket << UnkInt;

    _worldPacket.WriteBits(Key.length(), 7);
    _worldPacket.WriteString(Key);

    return &_worldPacket;
}

WorldPacket const* Battlepay::BattlepayBattlePetDelivered::Write()
{
    _worldPacket << DisplayID;
    _worldPacket << BattlePetGuid;

    return &_worldPacket;
}

void Battlepay::BattlepayPurchaseDetailsResponse::Read()
{
    _worldPacket >> UnkByte;
}

void Battlepay::BattlepayPurchaseUnkResponse::Read()
{
    auto keyLen = _worldPacket.ReadBits(6);
    auto key2Len = _worldPacket.ReadBits(7);
    Key = _worldPacket.ReadString(keyLen);
    Key2 = _worldPacket.ReadString(key2Len);
}

WorldPacket const* Battlepay::DisplayPromotion::Write()
{
    _worldPacket << PromotionID;

    return &_worldPacket;
}

WorldPacket const* Battlepay::BattlepayStartDistributionAssignToTargetResponse::Write()
{
    _worldPacket << DistributionID;
    _worldPacket << unkint1;
    _worldPacket << unkint2;

    return &_worldPacket;
}


void Battlepay::RequestConsumptionConversionInfo::Read()
{
    _worldPacket >> ID;
}
