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

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battlepay::ProductDisplayInfo const& displayInfo)
{
    // 经过测试，正确的顺序应该是先IconId，再ModelId，之后的数据也是这个顺序 ...
    data.WriteBit(displayInfo.IconFileID.is_initialized());
    data.WriteBit(displayInfo.CreatureDisplayInfoID.is_initialized());
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

    if (displayInfo.CreatureDisplayInfoID)
        data << *displayInfo.CreatureDisplayInfoID;
        //data << test;

    data.WriteString(displayInfo.Name1);
    data.WriteString(displayInfo.Name2);
    data.WriteString(displayInfo.Name3);
    data.WriteString(displayInfo.Name4);

    if (displayInfo.Flags)
        data << *displayInfo.Flags;     // 这个位置是正确的，客户端读取了

    if (displayInfo.UnkInt1)
        data << *displayInfo.UnkInt1;

    if (displayInfo.UnkInt2)
        data << *displayInfo.UnkInt2;

    if (displayInfo.UnkInt3)
        data << *displayInfo.UnkInt3;

    // 有没有一种可能，模型的显示是在这个数据上的？

    // 这个数据是可以多份的，在预览窗口中通过左右箭头图标来切换显示 ...
    // 并且Name字段已经生效了，显示在左右箭头图标的上面...
    // 当只有1份数据的时候，预览窗口的两个按钮是转向按钮
    // 当有超过一份的时候，预览窗口的两个按钮是左右切换按钮 ..
    for (auto const& itr : displayInfo.Visuals)
    {
        data.WriteBits(itr.ProductName.length(), 10);
        data.FlushBits();
        data << itr.DisplayId;
        data << itr.VisualId;
        data.WriteString(itr.ProductName);
    }
    return data;
}

// product info 的数据来自哪里？ 和product数据有什么不同？
// 来自 `battlepay_product`表

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battlepay::ProductInfoStruct const& info)
{
    uint32 cnt = 0;
    uint32 cnt2 = 0;

    data << info.ProductID;
    data << info.NormalPriceFixedPoint;
    data << info.CurrentPriceFixedPoint;

    data << uint32(info.ProductIDs.size());         // 确定了，这个是一个count，需要在之后输出count 个 uint32 ...
    data << info.UnkInt2;                           // 47 ？ Flags ？ 填 0 会导致显示不出商城内容 ..
    data << uint32(info.UnkInts.size());            // info.UnkInts      也确定了这个需要输出count 个 uint32 
    

    // 输出的这个值用在 角色界面，当鼠标移动到卡片的Icon上面的时候显示的Tips
    // 如果填写了非0值，会导致在游戏中，无法显示tooltip ..
    // 如果填写了ItemId，会导致无法显示 tooltips, 不管是填0还是itemId，都会导致无法显示tooltips...
    for (auto z1 : info.ProductIDs)  // 这个数据来自哪里？    
        data << uint32(z1);

    // 这个Id不明白是什么
    for (auto z2 : info.UnkInts)
        data << uint32(z2);


    // 前面的数据是确定了... 
    data.WriteBits(info.ChoiceType, 7);             // 这个位置是正确的？ 写入的值是 2
    data.WriteBit(info.DisplayInfo.is_initialized());
    data.FlushBits();
    if (info.DisplayInfo)
        data << *info.DisplayInfo;

    return data;
}


// 这个实际上应该是 ProductItem 的数据 ...
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battlepay::BattlepayProductItem const& product)
{
    data << product.ProductID;
    data << product.Type;
    data << product.UnkInt0;              // 如果设置了具体的itemId，当鼠标移动到图标上的时候，会显示物品的Tips, 设置成0，就只显示product的tip
    data << product.UnkInt1;
    data << product.DisplayId;
    data << product.UnkInt3;
    data << product.UnkInt4;            // Maybe CreatureID?
    data << product.UnkInt5;

    data.WriteBits(product.UnkString.size(), 8);        // 这个长度不确定是什么，需要在后面输出size()个字节数据，最长255
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

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battlepay::BattlepayDistributionObject const& object)
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

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battlepay::BattlepayPurchase const& purchase)
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

// 购买清单
WorldPacket const* WorldPackets::Battlepay::PurchaseListResponse::Write()
{
    _worldPacket << Result;
    _worldPacket << static_cast<uint32>(Purchase.size());
    for (auto const& purchaseData : Purchase)
        _worldPacket << purchaseData;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlepay::DistributionListResponse::Write()
{
    _worldPacket << Result;
    _worldPacket.WriteBits(DistributionObject.size(), 11);
    for (BattlepayDistributionObject const& objectData : DistributionObject)
        _worldPacket << objectData;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlepay::DistributionUpdate::Write()
{
    _worldPacket << DistributionObject;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlepay::ProductListResponse::Write()
{
    _worldPacket << Result;
    _worldPacket << ProductList.CurrencyID;

    _worldPacket << static_cast<uint32>(ProductList.ProductInfo.size());
    _worldPacket << static_cast<uint32>(ProductList.Product.size());
    _worldPacket << static_cast<uint32>(ProductList.ProductGroup.size());
    _worldPacket << static_cast<uint32>(ProductList.Shop.size());

    // product info ... 
    for (auto const& v : ProductList.ProductInfo)
        _worldPacket << v;

    // product Item ...
    for (auto const& productData : ProductList.Product)
        _worldPacket << productData;

    // product group 
    for (auto const& productGroupData : ProductList.ProductGroup)
    {
        _worldPacket << productGroupData.GroupID;
        _worldPacket << productGroupData.IconFileDataID;
        _worldPacket << productGroupData.DisplayType;
        _worldPacket << productGroupData.Ordering;
        _worldPacket << productGroupData.Flags;

        _worldPacket.WriteBits(productGroupData.Name.length(), 8);
        _worldPacket.WriteBits(productGroupData.IsAvailableDescription.length() + 1, 24);
        _worldPacket.WriteString(productGroupData.Name);
        if (!productGroupData.IsAvailableDescription.empty())
            _worldPacket.WriteString(productGroupData.IsAvailableDescription);
    }

    // shop entry ...
    for (BattlepayShopEntry const& shopData : ProductList.Shop)
    {
        _worldPacket << shopData.EntryID;
        _worldPacket << shopData.GroupID;
        _worldPacket << shopData.ProductID;
        _worldPacket << shopData.Ordering;
        _worldPacket << shopData.VasServiceType;
        _worldPacket << shopData.StoreDeliveryType;

        // 这个数据是错误的，或者是displayInfo 的结构是错误的 ...
        if (_worldPacket.WriteBit(shopData.DisplayInfo.is_initialized()))
        {
            _worldPacket.FlushBits();

            _worldPacket << *shopData.DisplayInfo;
        }
    }

    return &_worldPacket;
}

void WorldPackets::Battlepay::StartPurchase::Read()
{
    _worldPacket >> ClientToken;
    _worldPacket >> ProductID;
    _worldPacket >> TargetCharacter;
}

void WorldPackets::Battlepay::PurchaseProduct::Read()
{
    _worldPacket >> ClientToken;
    _worldPacket >> ProductID;
    _worldPacket >> TargetCharacter;

    uint32 strlen1 = _worldPacket.ReadBits(6);
    uint32 strlen2 = _worldPacket.ReadBits(12);
    WowSytem = _worldPacket.ReadString(strlen1);
    PublicKey = _worldPacket.ReadString(strlen2);
}

WorldPacket const* WorldPackets::Battlepay::StartPurchaseResponse::Write()
{
    _worldPacket << PurchaseID;
    _worldPacket << PurchaseResult;
    _worldPacket << ClientToken;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlepay::BattlepayAckFailed::Write()
{
    _worldPacket << PurchaseID;
    _worldPacket << PurchaseResult;
    _worldPacket << ClientToken;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlepay::PurchaseUpdate::Write()
{
    _worldPacket << static_cast<uint32>(Purchase.size());
    for (auto const& purchaseData : Purchase)
        _worldPacket << purchaseData;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlepay::ConfirmPurchase::Write()
{
    _worldPacket << PurchaseID;
    _worldPacket << ServerToken;

    return &_worldPacket;
}

void WorldPackets::Battlepay::ConfirmPurchaseResponse::Read()
{
    ConfirmPurchase = _worldPacket.ReadBit();
    _worldPacket >> ServerToken;
    _worldPacket >> ClientCurrentPriceFixedPoint;
}

void WorldPackets::Battlepay::BattlepayAckFailedResponse::Read()
{
    _worldPacket >> ServerToken;
}

WorldPacket const* WorldPackets::Battlepay::DeliveryEnded::Write()
{
    _worldPacket << DistributionID;

    _worldPacket << static_cast<int32>(item.size());
    for (Item::ItemInstance const& itemData : item)
        _worldPacket << itemData;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlepay::BattlepayDeliveryStarted::Write()
{
    _worldPacket << DistributionID;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlepay::UpgradeStarted::Write()
{
    _worldPacket << CharacterGUID;

    return &_worldPacket;
}

void WorldPackets::Battlepay::DistributionAssignToTarget::Read()
{
    _worldPacket >> ProductID;
    _worldPacket >> DistributionID;
    _worldPacket >> TargetCharacter;
    _worldPacket >> SpecializationID;
    _worldPacket >> ChoiceID;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battlepay::VasPurchaseData const& purchase)
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

WorldPacket const* WorldPackets::Battlepay::BattlepayVasPurchaseStarted::Write()
{
    _worldPacket << UnkInt;
    _worldPacket << VasPurchase;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlepay::CharacterClassTrialCreate::Write()
{
    _worldPacket << Result;
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlepay::BattlepayCharacterUpgradeQueued::Write()
{
    _worldPacket << Character;
    _worldPacket << static_cast<uint32>(EquipmentItems.size());
    for (auto const& item : EquipmentItems)
        _worldPacket << item;

    return &_worldPacket;
}

void WorldPackets::Battlepay::BattlepayTrialBoostCharacter::Read()
{
    _worldPacket >> Character;
    _worldPacket >> SpecializationID;
}

WorldPacket const* WorldPackets::Battlepay::BattlepayVasPurchaseList::Write()
{
    _worldPacket.WriteBits(VasPurchase.size(), 6);
    _worldPacket.FlushBits();
    for (auto const& itr : VasPurchase)
        _worldPacket << itr;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlepay::BattlepayPurchaseDetails::Write()
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

WorldPacket const* WorldPackets::Battlepay::BattlepayPurchaseUnk::Write()
{
    _worldPacket << UnkByte;
    _worldPacket << UnkInt;

    _worldPacket.WriteBits(Key.length(), 7);
    _worldPacket.WriteString(Key);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlepay::BattlepayBattlePetDelivered::Write()
{
    _worldPacket << DisplayID;
    _worldPacket << BattlePetGuid;

    return &_worldPacket;
}

void WorldPackets::Battlepay::BattlepayPurchaseDetailsResponse::Read()
{
    _worldPacket >> UnkByte;
}

void WorldPackets::Battlepay::BattlepayPurchaseUnkResponse::Read()
{
    auto keyLen = _worldPacket.ReadBits(6);
    auto key2Len = _worldPacket.ReadBits(7);
    Key = _worldPacket.ReadString(keyLen);
    Key2 = _worldPacket.ReadString(key2Len);
}

WorldPacket const* WorldPackets::Battlepay::DisplayPromotion::Write()
{
    _worldPacket << PromotionID;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlepay::BattlepayStartDistributionAssignToTargetResponse::Write()
{
    _worldPacket << DistributionID;
    _worldPacket << unkint1;
    _worldPacket << unkint2;

    return &_worldPacket;
}


void WorldPackets::Battlepay::RequestConsumptionConversionInfo::Read()
{
    _worldPacket >> ID;
}
