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
#include "ScriptMgr.h"
#include "Player.h"
#include "BattlepayMgr.h"
#include "BattlepayDataStore.h"
#include "DB2Stores.h"
#include "DatabaseEnv.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Mail.h"
#include "Item.h"

#define ITEM_HEARTHSTONE 6948

template<uint32 t_Level>
class Battlepay_Level : BattlepayProductScript
{
public:
    explicit Battlepay_Level(std::string scriptName) : BattlepayProductScript(scriptName) {}

    void OnProductDelivery(WorldSession* session, Battlepay::Product const& /*product*/) override
    {
        auto player = session->GetPlayer();
        if (!player)
            return;

        player->GiveLevel(t_Level);
        player->CastSpell(player, 34092, true);
        player->CastSpell(player, 54198, true);
        player->ModifyMoney(5000000);           // 增加500金币，这个应该是测试代码了...

        // only support providing gear when the level boost to level 90 or higher
        if (t_Level >= 90)
        {
            std::vector<Item*> toBeMailedCurrentEquipment;
            for (int es = EquipmentSlots::EQUIPMENT_SLOT_START; es < EquipmentSlots::EQUIPMENT_SLOT_END; es++)
            {
                if (Item* currentEquiped = player->GetItemByPos(INVENTORY_SLOT_BAG_0, es))
                {
                    ItemPosCountVec off_dest;
                    if (player->CanStoreItem(NULL_BAG, NULL_SLOT, off_dest, currentEquiped, false) == EQUIP_ERR_OK)
                    {
                        player->RemoveItem(INVENTORY_SLOT_BAG_0, es, true);
                        player->StoreItem(off_dest, currentEquiped, true);
                    }
                    else
                        toBeMailedCurrentEquipment.push_back(currentEquiped);
                }
            }

            if (!toBeMailedCurrentEquipment.empty())
            {
                SQLTransaction trans = CharacterDatabase.BeginTransaction();
                MailDraft draft("Inventory Full: Old Equipment.",
                    "To equip your new level boost gear, your old gear had to be unequiped. "
                    "You did not have enough free bag space, the items that could not be added to your bag you can find in this mail.");

                for (Item* currentEquiped : toBeMailedCurrentEquipment)
                {
                    player->MoveItemFromInventory(currentEquiped, true);
                    // deletes item from character's inventory
                    currentEquiped->DeleteFromInventoryDB(trans);
                    // recursive and not have transaction guard into self, item not in inventory and can be save standalone
                    currentEquiped->SaveToDB(trans);
                    draft.AddItem(currentEquiped);
                }
                
                draft.SendMailTo(trans, player, MailSender(player, MAIL_STATIONERY_GM), MailCheckMask(MAIL_CHECK_MASK_COPIED | MAIL_CHECK_MASK_RETURNED));
                CharacterDatabase.CommitTransaction(trans);
            }

            // 构建要邮寄的物品列表...
            std::vector<uint32> toBeMailedNewItems;

            // 根据player的职业来获取物品？
            // 从`CharacterLoadout.db2`和`CharacterLoadoutItem.db2`两张表中得到当前的职业（class）要
            // 加载的物品列表...

            uint64 raceMask = player->getRaceMask();
            uint8 purpose = t_Level < 100 ? 3 : 6;

            // 最小的CharacterLoadoutID，为什么要取最小的呢，我也不知到,<LEGION-CORE>这样写的...
            int32 lowestLoadoutID = 0;      

            for (auto const* entry : sCharacterLoadoutStore)
            {
                // 种族匹配，职业匹配
                if (entry && (entry->RaceMask != 0 || entry->RaceMask & raceMask) && entry->ChrClassID == player->getClass())
                {
                    // purpose到底是什么不是很明白...
                    if (entry->Purpose == purpose)
                    {
                        if (lowestLoadoutID == 0)
                            lowestLoadoutID = entry->ID;
                        else
                            lowestLoadoutID = (entry->ID < lowestLoadoutID) ? entry->ID : lowestLoadoutID;
                    }                     
                }
            }

            
            for (auto const* entry : sCharacterLoadoutItemStore)
            {
                if (entry && entry->CharacterLoadoutID == lowestLoadoutID)
                {
                    // 尝试将物品直接给player（装备到身上或者放到背包中），失败则通过邮寄的方式给player
                    if (!player->StoreNewItemInBestSlots(entry->ItemID, 1))
                    {
                        // 如果物品是`炉石`，那么需要检查身上和银行中是否有，没有才邮寄给player
                        if (entry->ItemID != ITEM_HEARTHSTONE || !player->HasItemCount(ITEM_HEARTHSTONE, 1, true))
                            toBeMailedNewItems.push_back(entry->ItemID);
                    }
                }
            }
            

            if (!toBeMailedNewItems.empty())
            {
                SQLTransaction trans = CharacterDatabase.BeginTransaction();
                MailDraft draft("Inventory Full: Level Boost Items.",
                    "You did not have enough free bag space to add all your complementary level boost items to your bags, those that did not fit you can find in this mail.");

                for (const uint32 item : toBeMailedNewItems)
                {
                    if (Item* pItem = Item::CreateItem(item, 1, player))
                    {
                        pItem->SaveToDB(trans);
                        draft.AddItem(pItem);
                    }
                }

                draft.SendMailTo(trans, player, MailSender(player, MAIL_STATIONERY_GM), MailCheckMask(MAIL_CHECK_MASK_COPIED | MAIL_CHECK_MASK_RETURNED));
                CharacterDatabase.CommitTransaction(trans);
            }
        }

        player->SaveToDB();
    }

    // 检查player是否可以购买Battlepay的东西...
    bool CanBuy(WorldSession* session, Battlepay::Product const& /*product*/, std::string& reason) override
    {
        auto player = session->GetPlayer();
        if (!player)
        {
            reason = sObjectMgr->GetTrinityString(Battlepay::String::NeedToBeInGame, session->GetSessionDbLocaleIndex());
            return false;
        }

        if (t_Level <= player->getLevel())
        {
            reason = sObjectMgr->GetTrinityString(Battlepay::String::TooHighLevel, session->GetSessionDbLocaleIndex());
            return false;
        }

        return true;
    }
};

// AI脚本是通过脚本名来关联的，数据库中存放的脚本名关联到脚本代码..
// TODO:  class name 必须和 scriptName 一样吗？
class playerScriptTokensAvailable : public PlayerScript
{
public:
    playerScriptTokensAvailable() : PlayerScript("playerScriptTokensAvailable") { }

    // Called when a player logs in.
    virtual void OnLogin(Player* player, bool firstLogin) override
    {
        if (player)
        {
            // 整个token 有什么作用？player的token存储在什么地方的？
            // 只在内存中的一个信息... 怎么保证不同的player不会串号呢？
            // TODO: 整个目的只是给客户端发消息， 没有实际的作用

            //for (auto tokenType : sBattlepayDataStore->GetTokenTypes())
            //    if (tokenType.second.hasLoginMessage && player->GetSession()->GetTokenBalance(tokenType.first))
            //        player->SendMessageToPlayer(tokenType.second.loginMessage.c_str());
        }
            
    }
};

// TODO: 整个脚本是用来处理worldserver.conf新增加的属性的
class reachedRefererThreshold : public PlayerScript
{
public:
    reachedRefererThreshold() : PlayerScript("reachedRefererThreshold") { }

    void OnLevelChanged(Player* player, uint8 oldLevel) override
    {
        if (player == NULL)
            return;

        uint32 trackerToken = sWorld->getIntConfig(CONFIG_REFERRAL_TRACKER_TOKEN_TYPE);
        if (trackerToken <= 0)
            return;

        if (sWorld->getIntConfig(CONFIG_REFERRAL_TRACKER_LEVEL_THRESHOLD) != oldLevel + 1)
            return;
#if 0
        if (!player->GetSession() || player->GetSession()->GetReferer() == 0)
            return;

        // check that this account has not already counted towards the total referral count
        // `SELECT COUNT(guid) FROM characters WHERE account = ? AND level >= ?`
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_NUM_ACCOUNT_CHARS_REACHED_LEVEL);
        stmt->setUInt32(0, player->GetSession()->GetAccountId());
        stmt->setUInt8(1, sWorld->getIntConfig(CONFIG_REFERRAL_TRACKER_LEVEL_THRESHOLD));
        PreparedQueryResult result = CharacterDatabase.Query(stmt);

        uint64 charsReachedThreshold = result ? (*result)[0].GetUInt64() : 0;
        if (charsReachedThreshold >= 1)
            return;

        uint32 referer = player->GetSession()->GetReferer();

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_TOKEN);
        stmt->setUInt32(0, referer);
        stmt->setUInt8(1, trackerToken);
        result = LoginDatabase.Query(stmt);

        int64 oldTokenAmount = (result) ? (*result)[0].GetInt64() : 0;

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_OR_UPD_TOKEN);
        stmt->setUInt32(0, referer);
        stmt->setUInt8(1, trackerToken);
        stmt->setInt64(2, 1);
        stmt->setInt64(3, 1);
        LoginDatabase.Execute(stmt);

        uint8 numReferralThresholdReached = 0;
        uint8 newTokenAmount = oldTokenAmount + 1;
        switch (newTokenAmount)
        {
        case 1:
            numReferralThresholdReached = 1;
            break;
        case 2:
            numReferralThresholdReached = 2;
            break;
        case 5:
            numReferralThresholdReached = 3;
            break;
        case 10:
            numReferralThresholdReached = 4;
            break;
        case 15:
            numReferralThresholdReached = 5;
            break;
        case 25:
            numReferralThresholdReached = 6;
            break;
        default:
            break;
        }

        if (numReferralThresholdReached == 0)
            return;

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_OR_UPD_TOKEN);
        stmt->setUInt32(0, referer);
        stmt->setUInt8(1, trackerToken + numReferralThresholdReached);
        stmt->setInt64(2, 1);
        stmt->setInt64(3, 1);
        LoginDatabase.Execute(stmt);
#endif // TODO:
    }
};

template <uint32 t_AccountServiceFlag> class Battlepay_AccountService : BattlepayProductScript
{
public:
    explicit Battlepay_AccountService(std::string scriptName) : BattlepayProductScript(scriptName) {}

    void OnProductDelivery(WorldSession* /*session*/, Battlepay::Product const& /*product*/) override
    {
        //session->SetServiceFlags(t_AccountServiceFlag);
    }

    bool CanBuy(WorldSession* /*session*/, Battlepay::Product const& /*product*/, std::string& reason) override
    {

        return true;
    }
};

// what is `SC`
void AddSC_Battlepay_Services()
{
    new Battlepay_Level<90>("battlepay_service_level90");
    new Battlepay_Level<100>("battlepay_service_level100");
    new playerScriptTokensAvailable();
    new reachedRefererThreshold();
    //new Battlepay_AccountService<ServiceFlags::PremadePve>("battlepay_service_premade");
}
