/*
 * Copyright (C) 2017-2018 AshamaneProject <https://github.com/AshamaneProject>
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
#include "InstanceScript.h"
#include "broken_shore_scenario.h"
#include "GameObject.h"
#include "Vehicle.h"

/* The Battle for Broken Shore
   Stage 1: The Broken Shore
   Stage 2: Storm The Beach
   Stage 3: Defeat the Commander
   Stage 4: Find The Others[Horde]/Find Varian [Alliance]
   Stage 5: Destroy the Portal
   Stage 6: Raze the Black City
   Stage 7: The Highlord
   Stage 8: Krosus
   Final Stage: Hold the Ridge[Horde]/Stop Gul'dan[Alliance]
   - Cinematic ...
*/  

// Script for Instance(Dungeon/Scenario) Map..
class instance_broken_shore_scenario : public InstanceMapScript
{
    enum _Enums
    {
        _DT_SCENARIA_TEAM = 0,      // data type
        _CIMEMATIC_ID = 999,
    };

public:
    instance_broken_shore_scenario() : InstanceMapScript("scenario_broken_shore_7.0", 1460) {}

    // Script for Instance(Dungeon/Scenario) ...
    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new broken_shore_scenario_InstanceScript(map);
    }

    struct broken_shore_scenario_InstanceScript : public InstanceScript
    {
        broken_shore_scenario_InstanceScript(InstanceMap* map) : InstanceScript(map) {}

        std::map<uint32, ObjectGuid> _objects;

        uint32 _team = 0;
        int32 _waveTotal = 0;               // 总共几波怪..
        int32 _waveCurr = 0;                // 当前是第几波怪
        int32 _spiresDestroyCount = 0;      // Spires of Woe destroyed
        WorldLocation _graveyardPoint;


        void Initialize() override
        {

        }

        // Check If all parts of Unit have been destroyed
        bool IsAllPartsDestroyed(Unit* unit, int8 count)
        {
            if (count <= 1) return true;
            int32 dc = 0;
            if (auto owner = unit->GetOwner())
                if (auto vehi = owner->ToCreature()->GetVehicleKit())
                    for (int8 i = 0; i < count; ++i)
                        if (!vehi->GetPassenger(i))
                            dc++;
            return (dc >= count);
        }

        void OnPlayerEnter(Player* player) override
        {
            if (!_team)
                _team = player->GetTeam();




        }
        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
                case _DT_SCENARIA_TEAM: return _team;
                default: return 0;
            }
        }
        void SetData(uint32 type, uint32 data) override
        {

        }
        ObjectGuid GetGuidData(uint32 type) const override
        {
            ObjectGuid guid = ObjectGuid::Empty;
            auto itr = _objects.find(type);
            if (itr != _objects.end())
                guid = itr->second;
            return guid;
        }
        void OnUnitDeath(Unit* unit) override
        {
            switch (unit->GetEntry())
            {                
                case 90686:
                case 109591:
                case 109592:
                case 109604:
                    // Achievement: Demons slain
                    UpdateCriteria(44095);      // Alliance  44095=>[Criteria.db2].Asset
                    UpdateCriteria(54116);      // Horde
                    break;
                
                case 91588:
                case 109586:
                case 109587:
                    // Achievement: Fel Lords slain
                    UpdateCriteria(52643);      // Alliance
                    break;
                case 113036:
                case 113037:
                case 113038:
                    // Achievement: Fel Lords slain
                    UpdateCriteria(54117);      // Horde
                    break;
                case 91704:
                case 110618:
                {
                    // Achievement: Spires of Woe destroyed
                    if (IsAllPartsDestroyed(unit, 3))
                    {
                        // Step2 ?
                        UpdateCriteria(44077);  // Alliance
                        UpdateCriteria(54114);  // Horde

                        // Step6?  部落和联盟的步骤不一样吗？
                        UpdateCriteria(44384);  // Alliance

                        // Spire of Woe: 240194
                        // Active building when all of it's Guard Unit destroyed
                        if (GameObject* go = unit->FindNearestGameObject(240194, 20.0f))
                        {
                            go->SetGoState(GO_STATE_ACTIVE);
                            // set phaseMask=2
                            // TODO:
                            // go->GetPhaseShift();
                        }

                        // 为什么要删除owner ？
                        if (auto c = unit->GetOwner()->ToCreature())
                            c->DespawnOrUnsummon();
                    }
                    break;
                }
                // Stage 3 - Defeat the Commander
                case 90705: // Dread Commander Arganoth                    
                    UpdateCriteria(45131);      // Alliance
                    break;
                case 93719: // Fel Commander Azgalor
                    UpdateCriteria(54109);      // Horde
                    break;
                // Stage 5 - Destroy the Portal
                case 101667:
                    UpdateCriteria(45288);  // Alliance
                    UpdateCriteria(54141);  // Horde
                    break;
                // Stage 6 - Raze the Black City
                case 102698:
                case 102701:
                case 110614:
                case 102704:
                case 102705:
                case 113059:
                case 100621:
                case 110616:
                case 102703:
                case 91967:
                case 94190:
                case 110617:
                case 113053:
                case 113058:
                case 113054:
                case 113055:
                case 97510:
                case 113056:
                case 113057:
                    // Increase progress by 10% When unit be killed
                    UpdateCriteria(53064);
                    break;
                case 110615:
                case 102696:
                case 102706:
                case 94189:
                case 90525:
                    // Increase progress by 5% When unit be killed
                    UpdateCriteria(53063);
                    break;
                case 100959:
                case 91970:
                case 90506:
                case 102702:
                case 94191:
                    // Increase progress by 1% When unit be killed
                    UpdateCriteria(44384);
                    break;
                case 105199:
                case 111074:
                case 105200:
                case 105206:
                case 105205:
                case 111175:
                case 111174:
                case 111173:
                case 111167:
                case 111171:
                case 111156:
                case 111157:
                case 111155:
                case 105164:
                case 105165:
                case 105183:
                case 105189:
                case 105175:
                case 105174:
                case 111085:
                case 111088:
                case 111087:
                case 105197:
                case 105196:
                case 105167:
                case 111165:
                case 111149:
                case 105182:
                case 111152:
                case 111153:
                case 105188:
                case 105190:
                case 105192:
                case 111148:
                case 105185:
                case 105186:
                case 105187:
                case 105181:
                case 105179:
                case 105180:
                case 92558:
                case 111154:
                case 105163:
                case 111089:
                case 105166:
                case 91902:
                case 105176:
                case 105171:
                case 111079:
                case 105168:
                case 90688:
                case 105169:
                case 105170:
                    --_waveTotal;
                    // TODO:

                    break;
                default:
                    break;
            }
        }
        void OnCreatureCreate(Creature* creautre) override
        {

        }
        void OnGameObjectCreate(GameObject* go) override
        {

        }
        // update Criteria for players in this dungeon..
        void UpdateCriteria(uint32 objective)
        {
            Map::PlayerList const& playerList = instance->GetPlayers();
            if (playerList.isEmpty()) return;

            // Spires of Woe destroyed 
            if (objective == 44077 || objective == 54114) 
            {
                ++_spiresDestroyCount;
                
                if (objective == 44077)  
                {
                    // Genn Greymane [Alliance]
                    if (Creature* npc = instance->GetCreature(GetGuidData(90717)))
                        npc->CastSpell(npc, (1 == _spiresDestroyCount ? 181978 : (2 == _spiresDestroyCount ? 199675 : 199676)));
                }
                else if (objective == 54114)
                {
                    // Vol'jin [Horde]
                    if (Creature* npc = instance->GetCreature(GetGuidData(90708)))
                        npc->CastSpell(npc, (1 == _spiresDestroyCount ? 224901 : (2 == _spiresDestroyCount ? 224904 : 224905)));
                }
            }

            for (auto itr = playerList.begin(); itr != playerList.end(); ++itr)
            {
                if (Player* player = itr->GetSource())
                    player->UpdateCriteria(CRITERIA_TYPE_SEND_EVENT_SCENARIO, objective);
            }
        }
    };
};


void AddSC_instance_broken_shore_scenario()
{
    new instance_broken_shore_scenario();

    //RegisterInstanceScript()
}
