/*
* Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
* Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include "Common.h"
#include "ObjectMgr.h"
#include "BattlepayMgr.h"
#include "WorldSession.h"
#include "Player.h"
#include "BattlepayDataStore.h"
#include "DatabaseEnv.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "PetBattle.h"
#include "CharacterService.h"
#include "CollectionMgr.h"
#include "Chat.h"
#include "WorldQuestMgr.h"
#include "SpellMgr.h"
#include "SpellInfo.h"

using namespace Battlepay;

namespace Tools
{
    uint32 GetItemIconFileDataID(uint32 itemID, uint32 appearanceModId = 0)
    {
        if (auto appearance1 = sDB2Manager.GetItemModifiedAppearance(itemID, appearanceModId))
        {
            if (auto appearance2 = sItemAppearanceStore.LookupEntry(appearance1->ItemAppearanceID))
                return appearance2->DefaultIconFileDataID;
        }

        return 0;
    }
}


BattlepayManager::BattlepayManager(WorldSession* session)
{
    _session = session;
    _walletName = "Donation points";
    _purchaseIDCount = 0;
    _distributionIDCount = 0;
}

BattlepayManager::~BattlepayManager() = default;

void BattlepayManager::RegisterStartPurchase(Purchase purchase)
{
    _actualTransaction = purchase;
}

uint64 BattlepayManager::GenerateNewPurchaseID()
{
    return uint64(0x1E77800000000000 | ++_purchaseIDCount);
}

uint64 BattlepayManager::GenerateNewDistributionId()
{
    return uint64(0x1E77800000000000 | ++_distributionIDCount);
}

Purchase* BattlepayManager::GetPurchase()
{
    return &_actualTransaction;
}

std::string const& BattlepayManager::GetDefaultWalletName() const
{
    return _walletName;
}

BattlepayCurrency BattlepayManager::GetShopCurrency() const
{
    /// @TODO: Move that to config files
    return Usd;
}

bool BattlepayManager::IsAvailable() const
{
    if (AccountMgr::IsModeratorAccount(_session->GetSecurity()))
        return true;

    return sWorld->getBoolConfig(CONFIG_FEATURE_SYSTEM_BPAY_STORE_ENABLED);
}

std::string Product::Serialize() const
{
    std::string res;
    res += "Expansion: " + std::to_string(CURRENT_EXPANSION);
    res += ", Type: " + std::to_string(WebsiteType);
    res += ", Quantity: " + std::to_string(1);
    res += ", IngameShop: " + std::to_string(1);
    res += ", CustomData: " + sScriptMgr->BattlepayGetCustomData(*this);

    uint32 idx = 0;
    for (auto const& itr : Items)
    {
        std::string iconName;
        if (auto itemTemplate = sObjectMgr->GetItemTemplate(itr.ItemID))
        {
            if (auto fileDataId = Tools::GetItemIconFileDataID(itr.ItemID))
                iconName = std::to_string(fileDataId);

            switch (WebsiteType)
            {
            case Item:
                res += ", Item(ItemID: " + std::to_string(itr.ItemID) + ", Quality: " + std::to_string(itemTemplate->GetQuality()) + ", Icon: " + iconName + ")";
                break;
            case PackItems:
                res += ", Pack(Icon: " + iconName + ", ItemsEntry: " + std::to_string(itr.ItemID) + ", Num: " + std::to_string(idx) + ")";
                break;
            default:
                break;
            }
        }

        idx++;
    }

    return res;
}

void BattlepayManager::ProcessDelivery(Purchase* purchase)
{
    // _existProducts.insert
    auto player = _session->GetPlayer(); // atm only ingame shop -_-

    auto const& product = sBattlepayDataStore->GetProduct(purchase->ProductID);
    switch (product.WebsiteType)
    {
    case Battlepay::Item:
        for (auto const& itr : product.Items)
            if (player)
                player->AddItem(itr.ItemID, itr.Quantity);
        break;
    case Battlepay::BattlePet:
        if (player)
            for (auto const& itr : product.Items)
                player->AddBattlePetByCreatureId(itr.ItemID, true, true);
        break;
    case Rename:
        if (player)
            sCharacterService->SetRename(player);
        break;
    case Faction:
        if (player)
            sCharacterService->ChangeFaction(player);
        break;
    case DeletedCharacter:
        sCharacterService->RestoreDeletedCharacter(_session);
        break;
    case Customization:
        if (player)
            sCharacterService->Customize(player);
        break;
    case Race:
        if (player)
            sCharacterService->ChangeRace(player);
        break;
    case CharacterBoost:    // level up to 90?
    {
        //if (_session->HasAuthFlag(AT_AUTH_FLAG_90_LVL_UP)) //@send error?
        //    break;

        //SendBattlepayDistribution(purchase->ProductID, DistributionStatus::BATTLE_PAY_DIST_STATUS_AVAILABLE, 1);

        //if (player)
        //    sCharacterService->Boost(player);
        break;
    }

    //case Category:
    //    break;
    //case Battlepay::Spell:
    //    break;
    //case Currency:
    //    break;
    //case GuildRename:
    //    break;
    //case Gold:
    //    break;
    //case Level:
    //    break;
    //case PremadeCharacter:
    //    break;
    //case RealmTransfer:
    //    break;
    //case ExpansionTransfer:
    //    break;
    //case Premium:
    //    break;
    //case PackItems:
    //    break;
    //case ItemProfession:
    //    break;
    //case Transmogrification:
    //    break;
    //case CategoryProfession:
    //    break;
    //case CategoryPremade:
    //    break;
    //case ItemMount:
    //    break;
    //case CategoryCharacterManagement:
    //    break;
    //case CategoryRealmTransfer:
    //    break;
    //case CategoryExpansionTransfer:
    //    break;
    //case CategoryGold:
    //    break;
    default:
        break;
    }

    if (!product.ScriptName.empty())
        sScriptMgr->OnBattlepayProductDelivery(_session, product);
}

bool BattlepayManager::AlreadyOwnProduct(uint32 itemId) const
{
    //if (_existProducts.find(productId) != _existProducts.end())
    //    return turue;

    auto const& player = _session->GetPlayer();
    if (player)
    {
        auto itemTemplate = sObjectMgr->GetItemTemplate(itemId);
        if (!itemTemplate)
            return true;

        for (auto itr : itemTemplate->Effects)
            if (itr->TriggerType == ITEM_SPELLTRIGGER_LEARN_SPELL_ID && player->HasSpell(itr->SpellID))
                return true;


        if (_session->GetCollectionMgr()->HasToy(itemId))
            return true;
    }

    return false;
}

auto BattlepayManager::ProductFilter(Product product) -> bool
{
    auto player = _session->GetPlayer();
    if (!player)
    {
        switch (product.WebsiteType)
        {
        case BattlePet:
        case Rename:
        case Faction:
        case DeletedCharacter:
        case Customization:
        case Race:
        case CharacterBoost:        
            //case Category:
            //    break;
            //case Battlepay::Spell:
            //    break;
            //case Currency:
            //    break;
            //case GuildRename:
            //    break;
            //case Gold:
            //    break;
            //case Level:
            //    break;
        case PremadeCharacter:
            //    break;
            //case RealmTransfer:
            //    break;
            //case ExpansionTransfer:
            //    break;
        case Premium:
            //    break;
            //case PackItems:
            //    break;
            //case ItemProfession:
            //    break;
            //case Transmogrification:
            //    break;
            //case CategoryProfession:
            //    break;
            //case CategoryPremade:
            //    break;
        case ItemMount:
            //    break;
            //case CategoryCharacterManagement:
            //    break;
            //case CategoryRealmTransfer:
            //    break;
            //case CategoryExpansionTransfer:
            //    break;
            //case CategoryGold:
            //    break;
        case Battlepay::Item:
            return true;
        default:
            return false;
        }
    }

    if (product.ClassMask && (player->getClassMask() & product.ClassMask) == 0)
        return false;

    for (auto& itr : product.Items)
    {
        if (AlreadyOwnProduct(itr.ItemID))
            return false;

        if (auto itemTemplate = sObjectMgr->GetItemTemplate(itr.ItemID))
        {
            if (itemTemplate->GetAllowableClass() && (itemTemplate->GetAllowableClass() & player->getClassMask()) == 0)
                return false;

            if (itemTemplate->GetAllowableRace() && (itemTemplate->GetAllowableRace() & player->getRaceMask()) == 0)
                return false;

            // TODO: 
            if (itemTemplate->GetRequiredReputationFaction() &&
                uint32(player->GetReputationRank(itemTemplate->GetRequiredReputationFaction())) < itemTemplate->GetRequiredReputationRank())
                return false;

            for (auto effectData : itemTemplate->Effects)
            {
                if (effectData->SpellID != 0 && effectData->TriggerType == ITEM_SPELLTRIGGER_LEARN_SPELL_ID)
                {
                    if (auto spellInfo = sSpellMgr->GetSpellInfo(effectData->SpellID))
                    {
                        if (spellInfo->HasAttribute(SPELL_ATTR7_HORDE_ONLY) && (player->getRaceMask() & RACEMASK_HORDE) == 0)
                            return false;

                        if (spellInfo->HasAttribute(SPELL_ATTR7_ALLIANCE_ONLY) && (player->getRaceMask() & RACEMASK_ALLIANCE) == 0)
                            return false;
                    }
                }
            }
        }
    }

    return true;
};

void BattlepayManager::SendProductList()
{
    WorldPackets::Battlepay::ProductListResponse response;
    if (!IsAvailable())
    {
        response.Result = ProductListResult::LockUnk1;
        _session->SendPacket(response.Write());
        return;
    }

    auto const& player = _session->GetPlayer();
    auto const& localeIndex = _session->GetSessionDbLocaleIndex();
    bool hasPlayer = player != nullptr;

    response.Result = ProductListResult::Available;
    response.ProductList.CurrencyID = GetShopCurrency();

    // group list
    for (auto& itr : sBattlepayDataStore->GetProductGroups())
    {
        if (!player && itr.IngameOnly)
            continue;

        if (itr.OwnsTokensOnly  && GetBalance(itr.TokenType) <= 0)
            continue;

        WorldPackets::Battlepay::BattlepayProductGroup group;
        group.GroupID = itr.GroupID;
        group.IconFileDataID = itr.IconFileDataID;
        group.Ordering = itr.Ordering;
        group.Flags = itr.Flags;
        group.DisplayType = itr.DisplayType;
        group.IsAvailableDescription = "Description";

        auto name = itr.Name;
        if (auto productLocale = sBattlepayDataStore->GetProductGroupLocale(itr.GroupID))
            ObjectMgr::GetLocaleString(productLocale->Name, localeIndex, name);
        group.Name = name;
        response.ProductList.Groups.emplace_back(group);
    }

    // shopEntry list ...
    for (auto const& itr : sBattlepayDataStore->GetShopEntries())
    {
        Battlepay::ProductGroup* productGroup = sBattlepayDataStore->GetProductGroup(itr.GroupID);
        if (!productGroup)
            continue;

        if (!player && productGroup->IngameOnly)
            continue;

        if (productGroup->OwnsTokensOnly && GetBalance(productGroup->TokenType) <= 0)
            continue;

        WorldPackets::Battlepay::BattlepayShopEntry sEntry;
        sEntry.EntryID = itr.EntryID;
        sEntry.GroupID = itr.GroupID;
        sEntry.ProductID = itr.ProductID;
        sEntry.Ordering = itr.Ordering;
        sEntry.VasServiceType = itr.VasServiceType;
        sEntry.StoreDeliveryType = itr.StoreDeliveryType;

        // DisplayInfo With Shop Product
        // When it is assigned, it will override BattlepayProduct.DisplayInfo.
        auto data = WriteDisplayInfo(itr.DisplayInfoID, localeIndex);
        if (std::get<0>(data))
        {
            sEntry.DisplayInfo = boost::in_place();
            sEntry.DisplayInfo = std::get<1>(data);
        }
        
        response.ProductList.Shop.emplace_back(sEntry);
    }

    for (auto const& itr : sBattlepayDataStore->GetProducts())
    {
        auto const& productEntry = itr.second;
        if (!ProductFilter(productEntry))
            continue;

        Battlepay::ProductGroup* group = sBattlepayDataStore->GetProductGroupForProductId(productEntry.ProductID);
        if (!group)
            continue;

        if (!player && group->IngameOnly)
            continue;

        uint64 tokenBalance = GetBalance(group->TokenType);
        if (group->OwnsTokensOnly && tokenBalance <= 0)
            continue;

        WorldPackets::Battlepay::BattlepayProduct product;
        product.NormalPriceFixedPoint = productEntry.NormalPriceFixedPoint * g_CurrencyPrecision;
        product.CurrentPriceFixedPoint = productEntry.CurrentPriceFixedPoint * g_CurrencyPrecision;
        product.ProductID = productEntry.ProductID;
        product.ChoiceType = productEntry.ChoiceType;


        if (hasPlayer)
        {
            // use for Item Tooltips, CAN NOT use in GLUE Screen
            product.ProductIDs.emplace_back(productEntry.ProductID);
        }

        //product.UnkInts.emplace_back(0);


        product.Flags = productEntry.Flags;


        // primary display info of Card 
        auto dataPI = WriteDisplayInfo(productEntry.DisplayInfoID, localeIndex);
        if (std::get<0>(dataPI))
        {
            product.DisplayInfo = boost::in_place();
            product.DisplayInfo = std::get<1>(dataPI);
        }

        bool hideProductPrice = false;
        bool hasEnoughTokens = false;

        if (product.DisplayInfo.is_initialized() && product.DisplayInfo->Flags.is_initialized())
            hideProductPrice = product.DisplayInfo->Flags.get() & BattlepayDisplayInfoFlag::HidePrice;
        hasEnoughTokens = tokenBalance >= productEntry.CurrentPriceFixedPoint;

        response.ProductList.Products.emplace_back(product);

        WorldPackets::Battlepay::BattlepayProductItem productItem;
        productItem.ProductID = productEntry.ProductID;
        productItem.UnkBit = false;

        productItem.Type = productEntry.Type;           // 0 - default, 1 - Boost 
        productItem.ItemID = productEntry.Items.size() > 0 ? productEntry.Items[0].ItemID : 0;
        productItem.UnkInt1 = 0;
        productItem.UnkInt2 = 0;
        productItem.UnkInt3 = 0;
        productItem.UnkInt4 = 0;
        productItem.UnkInt5 = 0;
        productItem.UnkString = "";
        
        for (auto& item : productEntry.Items)
        {
            WorldPackets::Battlepay::ProductItem pItem;

            pItem.Entry = item.Entry;
            pItem.ItemID = item.ItemID;         ///< Disable tooltip for packs (client handle only one tooltip).
            pItem.Quantity = item.Quantity;


            // if the productEntry is already owned disable the buy button
            // also disable the button if we don't show the price for a productEntry
            // and the player does not have enough tokens to pay for the productEntry
            pItem.HasPet = AlreadyOwnProduct(item.ItemID) || (hideProductPrice && !hasEnoughTokens);
            pItem.PetResult = item.PetResult;

            // Display Info With Item ...
            auto dataP = WriteDisplayInfo(item.DisplayInfoID, localeIndex);
            if (std::get<0>(dataP))
            {
                pItem.DisplayInfo = boost::in_place();
                pItem.DisplayInfo = std::get<1>(dataP);
            }

            productItem.Items.emplace_back(pItem);
        }

        // DisplayInfo With Product ...
        auto dataP = WriteDisplayInfo(productEntry.DisplayInfoID, localeIndex);
        if (std::get<0>(dataP))
        {
            productItem.DisplayInfo = boost::in_place();
            productItem.DisplayInfo = std::get<1>(dataP);
        }

        response.ProductList.Items.emplace_back(productItem);
    }

    _session->SendPacket(response.Write());
}

std::tuple<bool, WorldPackets::Battlepay::ProductDisplayInfo> BattlepayManager::WriteDisplayInfo(uint32 displayInfoID, LocaleConstant localeIndex, uint32 productId /*= 0*/)
{
    auto GeneratePackDescription = [localeIndex](Product const& product) -> std::string
    {
        auto getQualityColor = [](uint32 quality) -> std::string
        {
            switch (quality)
            {
                case ITEM_QUALITY_POOR:
                    return "|cff9d9d9d";
                case ITEM_QUALITY_NORMAL:
                    return "|cffffffff";
                case ITEM_QUALITY_UNCOMMON:
                    return "|cff1eff00";
                case ITEM_QUALITY_RARE:
                    return "|cff0070dd";
                case ITEM_QUALITY_EPIC:
                    return "|cffa335ee";
                case ITEM_QUALITY_LEGENDARY:
                    return "|cffff8000";
                case ITEM_QUALITY_ARTIFACT:
                    return "|cffe5cc80";
                case ITEM_QUALITY_HEIRLOOM:
                    return "|cffe5cc80";
                default:
                    return "|cffe5cc80";
            }
        };

        std::string res;
        for (auto itr : product.Items)
            if (auto itemTemplate = sObjectMgr->GetItemTemplate(itr.ItemID))
                res += getQualityColor(itemTemplate->GetQuality()) + itemTemplate->GetName(localeIndex) + "\n";
        return res;
    };

    auto info = WorldPackets::Battlepay::ProductDisplayInfo();
    if (!displayInfoID)
        return std::make_tuple(false, info);


    auto displayInfo = sBattlepayDataStore->GetDisplayInfo(displayInfoID);
    if (!displayInfo)
        return std::make_tuple(false, info);

    auto displayLocale = sBattlepayDataStore->GetDisplayInfoLocale(displayInfoID);

    info.Name1 = displayInfo->Name1;
    if (displayLocale)
        ObjectMgr::GetLocaleString(displayLocale->Name1, localeIndex, info.Name1);

    info.Name2 = displayInfo->Name2;
    if (displayLocale)
        ObjectMgr::GetLocaleString(displayLocale->Name2, localeIndex, info.Name2);

    info.Name3 = displayInfo->Name3;
    if (productId)
    {
        auto product = sBattlepayDataStore->GetProduct(productId);
        if (!product.Items.empty())
            info.Name3 = GeneratePackDescription(product);
    }
    else if (displayLocale)
        ObjectMgr::GetLocaleString(displayLocale->Name3, localeIndex, info.Name3);

    info.Name4 = displayInfo->Name4;
    if (displayLocale)
        ObjectMgr::GetLocaleString(displayLocale->Name4, localeIndex, info.Name4);

    if(displayInfo->IconFileID)
        info.IconFileID = displayInfo->IconFileID;

    if (displayInfo->CreatureDisplayID)
    {
        info.ModelSceneID = MODEL_SCENE_ID;

        // use to show model in Card ...
        WorldPackets::Battlepay::ProductDisplayVisualData visual;
        visual.DisplayID = displayInfo->CreatureDisplayID;
        visual.ModelSceneID = MODEL_SCENE_ID;
        visual.Title = "";
        info.Visuals.emplace_back(visual);
    }

    if (displayInfo->Flags)
        info.Flags = displayInfo->Flags;        // BattlepayDisplayFlag
      
    return std::make_tuple(true, info);
}

void BattlepayManager::SendPointsBalance()
{
    ChatHandler chatHandler(_session);
    if (!_session->GetPlayer())
        return;

    chatHandler.PSendSysMessage("Account name: %s", _session->GetAccountName());

    for (auto& tokenType : sBattlepayDataStore->GetTokenTypes())
    {
        int64 balance = GetBalance(tokenType.first);
        if (balance || tokenType.second.listIfNone)
            chatHandler.PSendSysMessage("%s: %d", tokenType.second.name, balance);
    }
}

void BattlepayManager::SendBattlepayDistribution(uint32 productId, uint8 status, uint64 distributionId, ObjectGuid targetGuid)
{
    WorldPackets::Battlepay::DistributionUpdate distributionBattlepay;
    auto product = sBattlepayDataStore->GetProduct(productId);
    if (!product.ProductID)
        return;

    auto const& localeIndex = _session->GetSessionDbLocaleIndex();
    distributionBattlepay.DistributionObject.DistributionID = distributionId;
    distributionBattlepay.DistributionObject.Status = status;
    distributionBattlepay.DistributionObject.ProductID = productId;
    distributionBattlepay.DistributionObject.Revoked = false; // not needed for us

    if (!targetGuid.IsEmpty())
    {
        distributionBattlepay.DistributionObject.TargetPlayer = targetGuid;
        distributionBattlepay.DistributionObject.TargetVirtualRealm = GetVirtualRealmAddress();
        distributionBattlepay.DistributionObject.TargetNativeRealm = GetVirtualRealmAddress();
    }

    WorldPackets::Battlepay::BattlepayProductItem productData;

    for (auto const& item : product.Items)
    {
        WorldPackets::Battlepay::ProductItem productItem;

        auto dataP = WriteDisplayInfo(item.DisplayInfoID, localeIndex);
        if (std::get<0>(dataP))
        {
            productItem.DisplayInfo = boost::in_place();
            productItem.DisplayInfo = std::get<1>(dataP);
        }

        productItem.PetResult = item.PetResult;
        productItem.Entry = item.Entry;
        productItem.ItemID = item.ItemID;
        productItem.Quantity = item.Quantity;
        productItem.UnkInt1 = item.DisplayInfoID;
        productItem.UnkInt2 = 0;
        productItem.PetResult = 0;
        productItem.HasPet = item.HasPet;
        productData.Items.emplace_back(productItem);
    }

    auto dataP = WriteDisplayInfo(product.DisplayInfoID, localeIndex);
    if (std::get<0>(dataP))
    {
        productData.DisplayInfo = boost::in_place();
        productData.DisplayInfo = std::get<1>(dataP);
    }

    //productData.UnkBits       Optional<uint16> ;
    productData.ProductID = product.ProductID;
    productData.Type = 0;               
    productData.ItemID = product.Items.size()>0 ? product.Items[0].ItemID : 0;

    productData.UnkInt1 = 0;   
    productData.UnkInt2 = product.DisplayInfoID;
    productData.UnkInt3 = 0;
    productData.UnkInt4 = 0;
    productData.UnkInt5 = 0;
    productData.UnkString = "";    
    productData.UnkBit = false;

    distributionBattlepay.DistributionObject.Product = std::move(productData);
    _session->SendPacket(distributionBattlepay.Write());
}

void BattlepayManager::AssignDistributionToCharacter(ObjectGuid const& targetCharGuid, uint64 distributionId, uint32 productId, uint16 specId, uint16 choiceId)
{
    WorldPackets::Battlepay::UpgradeStarted upgrade;
    upgrade.CharacterGUID = targetCharGuid;
    _session->SendPacket(upgrade.Write());

    WorldPackets::Battlepay::BattlepayStartDistributionAssignToTargetResponse assignResponse;
    assignResponse.DistributionID = distributionId;
    assignResponse.unkint1 = 0;
    assignResponse.unkint2 = 0;
    _session->SendPacket(upgrade.Write());

    auto purchase = GetPurchase();
    purchase->Status = DistributionStatus::BATTLE_PAY_DIST_STATUS_ADD_TO_PROCESS;

    SendBattlepayDistribution(productId, purchase->Status, distributionId, targetCharGuid);
}



void BattlepayManager::Update(uint32 diff)
{
    auto& data = _actualTransaction;
    auto& product = sBattlepayDataStore->GetProduct(data.ProductID);

    switch (data.Status)
    {
    case DistributionStatus::BATTLE_PAY_DIST_STATUS_ADD_TO_PROCESS:
    {
        switch (product.WebsiteType)
        {
        case CharacterBoost:
        {
            // TODO:
            auto const& player = ObjectAccessor::FindPlayer(data.TargetCharacter);
            if (!player)
                break;

            WorldPackets::Battlepay::BattlepayCharacterUpgradeQueued responseQueued;
            //responseQueued.EquipmentItems = sDB2Manager.GetItemLoadOutItemsByClassID(player->getClass(), 3)[0];

            responseQueued.Character = data.TargetCharacter;
            _session->SendPacket(responseQueued.Write());

            data.Status = DistributionStatus::BATTLE_PAY_DIST_STATUS_PROCESS_COMPLETE;
            SendBattlepayDistribution(data.ProductID, data.Status, data.DistributionId, data.TargetCharacter);
            break;
        }
        default:
            break;
        }
        break;
    }
    case DistributionStatus::BATTLE_PAY_DIST_STATUS_PROCESS_COMPLETE: //send SMSG_BATTLE_PAY_VAS_PURCHASE_STARTED
    {
        switch (product.WebsiteType)
        {
        case CharacterBoost:
        {
            data.Status = DistributionStatus::BATTLE_PAY_DIST_STATUS_FINISHED;
            SendBattlepayDistribution(data.ProductID, data.Status, data.DistributionId, data.TargetCharacter);
            break;
        }
        default:
            break;
        }
        break;
    }
    case DistributionStatus::BATTLE_PAY_DIST_STATUS_FINISHED:
    {
        switch (product.WebsiteType)
        {
        case CharacterBoost:
            SendBattlepayDistribution(data.ProductID, data.Status, data.DistributionId, data.TargetCharacter);
            break;
        default:
            break;
        }
        break;
    }
    case DistributionStatus::BATTLE_PAY_DIST_STATUS_AVAILABLE:
    case DistributionStatus::BATTLE_PAY_DIST_STATUS_NONE:
    default:
        break;
    }
}

uint64 BattlepayManager::GetBalance(uint8 tokenType)
{
    // TODO: 
    return 100000000;
}

void BattlepayManager::GetLoadoutItems(Player * player, std::vector<uint32>& itemList, int8 purpose)
{
    std::set<uint32> idSet;

    for (auto const* entry : sCharacterLoadoutStore)
    {
        if (entry && entry->ChrClassID == player->getClass())   // class test pass
        {
            if (entry->Purpose != purpose)
                continue;

            if (entry->RaceMask == 0 || (entry->RaceMask & player->getRaceMask()) != 0) // race test pass
                idSet.insert(entry->ID);
        }
    }

    for (auto const* entry : sCharacterLoadoutItemStore)
        if (entry && (idSet.count(entry->CharacterLoadoutID) > 0))
            itemList.push_back(entry->ItemID);
}
