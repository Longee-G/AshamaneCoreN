/*
 * Copyright (C) 2008-2018 TrinityCore <https://www.trinitycore.org/>
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

#ifndef TRINITYCORE_CREATURE_H
#define TRINITYCORE_CREATURE_H

#include "Unit.h"
#include "Common.h"
#include "CreatureData.h"
#include "DatabaseEnvFwd.h"
#include "Duration.h"
#include "Loot.h"
#include "MapObject.h"

#include <list>

class CreatureAI;
class CreatureGroup;
class Group;
class Quest;
class Player;
class SpellInfo;
class WildBattlePet;
class WorldSession;
enum MovementGeneratorType : uint8;

struct ScriptParam;

struct VendorItemCount
{
    VendorItemCount(uint32 _item, uint32 _count)
        : itemId(_item), count(_count), lastIncrementTime(time(NULL)) { }

    uint32 itemId;
    uint32 count;
    time_t lastIncrementTime;
};

typedef std::list<Creature*> CreatureList;
typedef std::list<VendorItemCount> VendorItemCounts;

// max different by z coordinate for creature aggro reaction
#define CREATURE_Z_ATTACK_RANGE 3

#define MAX_VENDOR_ITEMS 150                                // Limitation in 4.x.x item count in SMSG_LIST_INVENTORY

//used for handling non-repeatable random texts
typedef std::vector<uint8> CreatureTextRepeatIds;
typedef std::unordered_map<uint8, CreatureTextRepeatIds> CreatureTextRepeatGroup;

class TC_GAME_API Creature : public Unit, public GridObject<Creature>, public MapObject
{
    public:
        explicit Creature(bool isWorldObject = false);
        virtual ~Creature();

        void AddToWorld() override;
        void RemoveFromWorld() override;

        void SetObjectScale(float scale) override;
        void SetDisplayId(uint32 modelId) override;

        void DisappearAndDie();

        bool Create(ObjectGuid::LowType guidlow, Map* map, uint32 entry, float x, float y, float z, float ang, CreatureData const* data, uint32 vehId);

        static Creature* CreateCreature(uint32 entry, Map* map, Position const& pos, uint32 vehId = 0);
        static Creature* CreateCreatureFromDB(ObjectGuid::LowType spawnId, Map* map, bool addToMap = true, bool allowDuplicate = false);

        bool LoadCreaturesAddon();
        void SelectLevel();
        void UpdateLevelDependantStats();
        void LoadEquipment(int8 id = 1, bool force = false);
        void SetSpawnHealth();
        void SetBaseHealth(uint64 health);

		void ReLoad(bool skipDB);

        ObjectGuid::LowType GetSpawnId() const { return _spawnId; }

        void Update(uint32 time) override;                         // overwrited Unit::Update
        void GetRespawnPosition(float &x, float &y, float &z, float* ori = nullptr, float* dist =nullptr) const;

        void SetCorpseDelay(uint32 delay) { _corpseDelay = delay; }
        uint32 GetCorpseDelay() const { return _corpseDelay; }
        bool IsRacialLeader() const { return GetCreatureTemplate()->RacialLeader; }
        bool IsCivilian() const { return (GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_CIVILIAN) != 0; }
        bool IsTrigger() const { return (GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_TRIGGER) != 0; }
        bool IsGuard() const { return (GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_GUARD) != 0; }
        bool CanWalk() const { return (GetCreatureTemplate()->InhabitType & INHABIT_GROUND) != 0; }
        bool CanSwim() const override { return (GetCreatureTemplate()->InhabitType & INHABIT_WATER) != 0 || IsPet(); }
        bool CanFly()  const override { return (GetCreatureTemplate()->InhabitType & INHABIT_AIR) != 0; }

        void SetReactState(ReactStates st) { _reactState = st; }
        ReactStates GetReactState() const { return _reactState; }
        bool HasReactState(ReactStates state) const { return (_reactState == state); }
        void InitializeReactState();

        /// @todo Rename these properly
        bool isCanInteractWithBattleMaster(Player* player, bool msg) const;
        bool CanResetTalents(Player* player) const;
        bool CanCreatureAttack(Unit const* victim, bool force = true) const;
        bool IsImmunedToSpell(SpellInfo const* spellInfo, Unit* caster) const override;
        bool IsImmunedToSpellEffect(SpellInfo const* spellInfo, uint32 index, Unit* caster) const override;
        bool isElite() const;
        bool isWorldBoss() const;

        bool IsDungeonBoss() const;

        bool HasScalableLevels() const;
        uint8 GetLevelForTarget(WorldObject const* target) const override;

        uint64 GetMaxHealthByLevel(uint8 level) const;
        float GetHealthMultiplierForTarget(WorldObject const* target) const override;

        float GetBaseDamageForLevel(uint8 level) const;
        float GetDamageMultiplierForTarget(WorldObject const* target) const override;

        float GetBaseArmorForLevel(uint8 level) const;
        float GetArmorMultiplierForTarget(WorldObject const* target) const override;

        bool IsInEvadeMode() const { return HasUnitState(UNIT_STATE_EVADE); }
        bool IsEvadingAttacks() const { return IsInEvadeMode() || CanNotReachTarget(); }

        bool AIM_Destroy();
        bool AIM_Create(CreatureAI* ai = nullptr);
        void AI_InitializeAndEnable();
        bool AIM_Initialize(CreatureAI* ai = nullptr);
        void Motion_Initialize();

        CreatureAI* AI() const { return reinterpret_cast<CreatureAI*>(_AI); }

        SpellSchoolMask GetMeleeDamageSchoolMask() const override { return _meleeDamageSchoolMask; }
        void SetMeleeDamageSchool(SpellSchools school) { _meleeDamageSchoolMask = SpellSchoolMask(1 << school); }

        bool HasSpell(uint32 spellID) const override;

        bool UpdateEntry(uint32 entry, CreatureData const* data = nullptr, bool updateLevel = true, bool updateScript = false);

        void UpdateMovementFlags();

        bool UpdateStats(Stats stat) override;
        bool UpdateAllStats() override;
        void UpdateResistances(uint32 school) override;
        void UpdateArmor() override;
        void UpdateMaxHealth() override;
        void UpdateMaxPower(Powers power) override;
        uint32 GetPowerIndex(Powers power) const override;
        void UpdateAttackPowerAndDamage(bool ranged = false) override;
        void CalculateMinMaxDamage(WeaponAttackType attType, bool normalized, bool addTotalPct, float& minDamage, float& maxDamage) override;

        void SetCanDualWield(bool value) override;
        int8 GetOriginalEquipmentId() const { return _originalEquipmentId; }
        uint8 GetCurrentEquipmentId() const { return _equipmentId; }
        void SetCurrentEquipmentId(uint8 id) { _equipmentId = id; }

        float GetSpellDamageMod(int32 Rank) const;

        VendorItemData const* GetVendorItems() const;
        uint32 GetVendorItemCurrentCount(VendorItem const* vItem);
        uint32 UpdateVendorItemCurrentCount(VendorItem const* vItem, uint32 used_count);

        TrainerSpellData const* GetTrainerSpells() const;

        CreatureTemplate const* GetCreatureTemplate() const { return _creatureInfo; }
        CreatureData const* GetCreatureData() const { return _creatureData; }
        CreatureAddon const* GetCreatureAddon() const;

        std::string GetAIName() const;
        std::string GetScriptName() const;
        uint32 GetScriptId() const;
        ScriptParam GetScriptParam(uint8 index) const;

        // override WorldObject function for proper name localization
        std::string const& GetNameForLocaleIdx(LocaleConstant locale_idx) const override;
        // override virtual Unit::setDeathState
        void setDeathState(DeathState s) override;                   

        bool LoadFromDB(ObjectGuid::LowType spawnId, Map* map) { return LoadCreatureFromDB(spawnId, map, false, false); }
    private:
        bool LoadCreatureFromDB(ObjectGuid::LowType spawnId, Map* map, bool addToMap, bool allowDuplicate);
    public:
        void SaveToDB();
        // overriden in Pet                                                            
        virtual void SaveToDB(uint32 mapid, DifficultyVector const& spawnDifficulties);
        // overriden in Pet
        virtual void DeleteFromDB();

        Loot loot;
        void StartPickPocketRefillTimer();
        void ResetPickPocketRefillTimer() { _pickpocketLootRestore = 0; }
        bool CanGeneratePickPocketLoot() const { return _pickpocketLootRestore <= time(NULL); }
        void SetSkinner(ObjectGuid guid) { _skinner = guid; }
        // Returns the player who skinned this creature
        ObjectGuid GetSkinner() const { return _skinner; } 
        Player* GetLootRecipient() const;
        Group* GetLootRecipientGroup() const;
        bool hasLootRecipient() const { return !_lootRecipient.IsEmpty() || !_lootRecipientGroup.IsEmpty(); }
        // return true if the creature is tapped by the player or a member of his party.
        bool isTappedBy(Player const* player) const;           

        void SetLootRecipient (Unit* unit);
        // Call this when all things loot from corpse.
        void AllLootRemovedFromCorpse();

        uint16 GetLootMode() const { return _lootMode; }
        bool HasLootMode(uint16 lootMode) { return (_lootMode & lootMode) != 0; }
        void SetLootMode(uint16 lootMode) { _lootMode = lootMode; }
        void AddLootMode(uint16 lootMode) { _lootMode |= lootMode; }
        void RemoveLootMode(uint16 lootMode) { _lootMode &= ~lootMode; }
        void ResetLootMode() { _lootMode = LOOT_MODE_DEFAULT; }

        SpellInfo const* reachWithSpellAttack(Unit* victim);
        SpellInfo const* reachWithSpellCure(Unit* victim);

        uint32 _spells[MAX_CREATURE_SPELLS];

        bool CanStartAttack(Unit const* u, bool force) const;
        float GetAttackDistance(Unit const* player) const;
        float GetAggroRange(Unit const* target) const;

        void SendAIReaction(AiReaction reactionType);

        Unit* SelectNearestTarget(float dist = 0, bool playerOnly = false) const;
        Unit* SelectNearestTargetInAttackDistance(float dist = 0) const;
        Unit* SelectNearestHostileUnitInAggroRange(bool useLOS = false) const;

        void DoFleeToGetAssistance();
        void CallForHelp(float fRadius);
        void CallAssistance();
        void SetNoCallAssistance(bool val) { _alreadyCallAssistance = val; }
        void SetNoSearchAssistance(bool val) { _alreadySearchedAssistance = val; }
        bool HasSearchedAssistance() const { return _alreadySearchedAssistance; }
        bool CanAssistTo(const Unit* u, const Unit* enemy, bool checkfaction = true) const;
        bool _IsTargetAcceptable(const Unit* target) const;

        MovementGeneratorType GetDefaultMovementType() const { return _defaultMovementType; }
        void SetDefaultMovementType(MovementGeneratorType mgt) { _defaultMovementType = mgt; }

        void RemoveCorpse(bool setSpawnTime = true);

        void DespawnOrUnsummon(uint32 msTimeToDespawn = 0, Seconds const& forceRespawnTime = Seconds(0));
        void DespawnOrUnsummon(Milliseconds const& time, Seconds const& forceRespawnTime = Seconds(0)) { DespawnOrUnsummon(uint32(time.count()), forceRespawnTime); }
        void DespawnCreaturesInArea(uint32 entry, float range = 125.0f);

        time_t const& GetRespawnTime() const { return _respawnTime; }
        time_t GetRespawnTimeEx() const;
        void SetRespawnTime(uint32 respawn) { _respawnTime = respawn ? time(NULL) + respawn : 0; }
        void Respawn(bool force = false);
        void SaveRespawnTime() override;

        uint32 GetRespawnDelay() const { return _respawnDelay; }
        void SetRespawnDelay(uint32 delay) { _respawnDelay = delay; }

        float GetRespawnRadius() const { return _respawnRadius; }
        void SetRespawnRadius(float dist) { _respawnRadius = dist; }

        void DoImmediateBoundaryCheck() { _boundaryCheckTime = 0; }
        uint32 GetCombatPulseDelay() const { return _combatPulseDelay; }
        // (secs) interval at which the creature pulses the entire zone into combat (only works in dungeons)
        void SetCombatPulseDelay(uint32 delay) 
        {
            _combatPulseDelay = delay;
            if (_combatPulseTime == 0 || _combatPulseTime > delay)
                _combatPulseTime = delay;
        }
        // (msecs)timer used for group loot
        uint32 _groupLootTimer;
        // used to find group which is looting corpse
        ObjectGuid lootingGroupLowGUID;            

        void SendZoneUnderAttackMessage(Player* attacker);

        void SetInCombatWithZone();

        bool hasQuest(uint32 quest_id) const override;
        bool hasInvolvedQuest(uint32 quest_id)  const override;

        bool isRegeneratingHealth() { return _regenHealth; }
        void setRegeneratingHealth(bool regenHealth) { _regenHealth = regenHealth; }
        virtual uint8 GetPetAutoSpellSize() const { return MAX_SPELL_CHARM; }
        virtual uint32 GetPetAutoSpellOnPos(uint8 pos) const;
        float GetPetChaseDistance() const;

        void SetCannotReachTarget(bool cannotReach) { if (cannotReach == _cannotReachTarget) return; _cannotReachTarget = cannotReach; _cannotReachTimer = 0; }
        bool CanNotReachTarget() const { return _cannotReachTarget; }

        void SetPosition(float x, float y, float z, float o);
        void SetPosition(const Position &pos) { SetPosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation()); }

        void SetHomePosition(float x, float y, float z, float o) { _homePosition.Relocate(x, y, z, o); }
        void SetHomePosition(const Position &pos) { _homePosition.Relocate(pos); }
        void GetHomePosition(float& x, float& y, float& z, float& ori) const { _homePosition.GetPosition(x, y, z, ori); }
        Position const& GetHomePosition() const { return _homePosition; }

        void SetTransportHomePosition(float x, float y, float z, float o) { _transportHomePosition.Relocate(x, y, z, o); }
        void SetTransportHomePosition(const Position &pos) { _transportHomePosition.Relocate(pos); }
        void GetTransportHomePosition(float& x, float& y, float& z, float& ori) const { _transportHomePosition.GetPosition(x, y, z, ori); }
        Position const& GetTransportHomePosition() const { return _transportHomePosition; }

        uint32 GetWaypointPath() const { return _pathId; }
        void LoadPath(uint32 pathid) { _pathId = pathid; }

        uint32 GetCurrentWaypointID() const { return _waypointID; }
        void UpdateWaypointID(uint32 wpID) { _waypointID = wpID; }

        void SearchFormation();
        CreatureGroup* GetFormation() { return _formation; }
        void SetFormation(CreatureGroup* formation) { _formation = formation; }

        Unit* SelectVictim();

        void SetDisableReputationGain(bool disable) { _disableReputationGain = disable; }
        bool IsReputationGainDisabled() const { return _disableReputationGain; }
        bool IsDamageEnoughForLootingAndReward() const
        {
            return (_creatureInfo->flags_extra & CREATURE_FLAG_EXTRA_NO_PLAYER_DAMAGE_REQ) || (_playerDamageReq == 0);
        }
        void LowerPlayerDamageReq(uint64 unDamage);
        void ResetPlayerDamageReq() { _playerDamageReq = GetHealth() / 2; }

        uint64 _playerDamageReq;

        uint32 GetOriginalEntry() const { return _originalEntry; }
        void SetOriginalEntry(uint32 entry) { _originalEntry = entry; }

        static float _GetDamageMod(int32 Rank);

        float _sightDistance;
        // melee attack range
        float _combatDistance;
        //true when possessed
        bool _isTempWorldObject; 

        // Handling caster facing during spellcast
        void SetTarget(ObjectGuid const& guid) override;
        // flags the Creature for forced (client displayed) target reacquisition in the next ::Update call
        void MustReacquireTarget() { _shouldReacquireTarget = true; } 
        void DoNotReacquireTarget() { _shouldReacquireTarget = false; _suppressedTarget = ObjectGuid::Empty; _suppressedOrientation = 0.0f; }
        void FocusTarget(Spell const* focusSpell, WorldObject const* target);
        bool IsFocusing(Spell const* focusSpell = nullptr, bool withDelay = false);
        void ReleaseFocus(Spell const* focusSpell = nullptr, bool withDelay = true);

        CreatureTextRepeatIds GetTextRepeatGroup(uint8 textGroup);
        void SetTextRepeatId(uint8 textGroup, uint8 id);
        void ClearTextRepeatGroup(uint8 textGroup);

        bool CanGiveExperience() const;

        void ForcedDespawn(uint32 timeMSToDespawn = 0, Seconds const& forceRespawnTimer = Seconds(0));

        WildBattlePet* GetWildBattlePet() { return _wildBattlePet; }

        void DisableHealthRegen() { _disableHealthRegen = true; }
        void ReenableHealthRegen() { _disableHealthRegen = false; }
        bool HealthRegenDisabled() const { return _disableHealthRegen; }

        bool IsCorpseGatherable() const;

    protected:
        bool CreateFromProto(ObjectGuid::LowType guidlow, uint32 entry, CreatureData const* data = nullptr, uint32 vehId = 0);
        bool InitEntry(uint32 entry, CreatureData const* data = nullptr);

        // vendor items
        VendorItemCounts _vendorItemCounts;

        static float _GetHealthMod(int32 Rank);
        // the mail receiver GUID
        ObjectGuid _lootRecipient;
        ObjectGuid _lootRecipientGroup;
        // guid of skinner who skin this creature
        ObjectGuid _skinner;

        /// Timers
        time_t _pickpocketLootRestore;
        // (secs)timer for death or corpse disappearance
        time_t _corpseRemoveTime;
        // (secs) time of next respawn
        time_t _respawnTime;
        // (secs) delay between corpse disappearance and respawning
        uint32 _respawnDelay;
        // (secs) delay between death and corpse disappearance
        uint32 _corpseDelay;                               
        float _respawnRadius;
        // (msecs) remaining time for next evade boundary check
        uint32 _boundaryCheckTime;
        // (msecs) remaining time for next zone-in-combat pulse
        uint32 _combatPulseTime;
        // (secs) how often the creature puts the entire zone in combat (only works in dungeons)
        uint32 _combatPulseDelay;
        // for AI, not charmInfo
        ReactStates _reactState;

        // recover mana points
        void RegenerateMana();
        void RegenerateHealth();
        void Regenerate(Powers power);

        MovementGeneratorType _defaultMovementType;
        // For new or temporary creatures is 0 for saved it is lowguid
        ObjectGuid::LowType _spawnId;                               
        uint8 _equipmentId;
        // can be -1
        int8 _originalEquipmentId; 

        bool _alreadyCallAssistance;
        bool _alreadySearchedAssistance;
        // enable regenerate health???
        bool _regenHealth;
        bool _cannotReachTarget;
        uint32 _cannotReachTimer;
        bool _isAILocked;

        SpellSchoolMask _meleeDamageSchoolMask;
        uint32 _originalEntry;

        Position _homePosition;
        Position _transportHomePosition;

        bool _disableReputationGain;
        // Can differ from sObjectMgr->GetCreatureTemplate(GetEntry()) in difficulty mode > 0
        CreatureTemplate const* _creatureInfo;                 
        CreatureData const* _creatureData;
        // Bitmask (default: LOOT_MODE_DEFAULT) that determines what loot will be lootable
        uint16 _lootMode;                                  

        bool IsInvisibleDueToDespawn() const override;
        bool CanAlwaysSee(WorldObject const* obj) const override;

    private:
        // No aggro from gray creatures
        bool CheckNoGrayAggroConfig(uint32 playerLevel, uint32 creatureLevel) const; 

        //WaypointMovementGenerator vars
        uint32 _waypointID;
        uint32 _pathId;

        //Formation var
        CreatureGroup* _formation;
        bool _triggerJustRespawned;

        /* Spell focus system */
        // Locks the target during spell cast for proper facing
        Spell const* _focusSpell;
        uint32 _focusDelay;
        bool _shouldReacquireTarget;
        // Stores the creature's "real" target while casting
        ObjectGuid _suppressedTarget;
        // Stores the creature's "real" orientation while casting
        float _suppressedOrientation; 

        CreatureTextRepeatGroup _textRepeat;

        WildBattlePet* _wildBattlePet;

        bool _disableHealthRegen;
};

class TC_GAME_API AssistDelayEvent : public BasicEvent
{
    public:
        AssistDelayEvent(ObjectGuid victim, Unit& owner) : BasicEvent(), _victim(victim), _owner(owner) { }

        bool Execute(uint64 e_time, uint32 p_time) override;
        void AddAssistant(ObjectGuid guid) { _assistants.push_back(guid); }
    private:
        AssistDelayEvent();

        ObjectGuid        _victim;
        GuidList          _assistants;
        Unit&             _owner;
};

class TC_GAME_API ForcedDespawnDelayEvent : public BasicEvent
{
    public:
        ForcedDespawnDelayEvent(Creature& owner, Seconds const& respawnTimer) : BasicEvent(), _owner(owner), _respawnTimer(respawnTimer) { }
        bool Execute(uint64 e_time, uint32 p_time) override;

    private:
        Creature& _owner;
        Seconds const _respawnTimer;
};

#endif
