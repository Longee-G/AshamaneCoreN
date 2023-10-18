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

#ifndef TRINITYCORE_PET_H
#define TRINITYCORE_PET_H

#include "PetDefines.h"
#include "TemporarySummon.h"

#define PET_FOCUS_REGEN_INTERVAL 4 * IN_MILLISECONDS

enum StableResultCode
{
    STABLE_ERR_MONEY        = 0x01,                         // "you don't have enough money"
    STABLE_ERR_INVALID_SLOT = 0x03,                         // "That slot is locked"
    STABLE_SUCCESS_STABLE   = 0x08,                         // stable success
    STABLE_SUCCESS_UNSTABLE = 0x09,                         // unstable/swap success
    STABLE_SUCCESS_BUY_SLOT = 0x0A,                         // buy slot success
    STABLE_ERR_EXOTIC       = 0x0B,                         // "you are unable to control exotic creatures"
    STABLE_ERR_STABLE       = 0x0C                          // "Internal pet error"
};

struct PetSpell
{
    ActiveStates active;
    PetSpellState state;
    PetSpellType type;
};

enum PetStableinfo
{
    PET_STABLE_ACTIVE = 1,
    PET_STABLE_INACTIVE = 2
};

typedef std::unordered_map<uint32, PetSpell> PetSpellMap;
typedef std::vector<uint32> AutoSpellList;

class Player;

class TC_GAME_API Pet : public Guardian
{
    public:
        explicit Pet(Player* owner, PetType type = MAX_PET_TYPE);
        virtual ~Pet();

        void AddToWorld() override;
        void RemoveFromWorld() override;

        void SetDisplayId(uint32 modelId) override;

        PetType getPetType() const { return _petType; }
        void setPetType(PetType type) { _petType = type; }
        bool isControlled() const { return getPetType() == SUMMON_PET || getPetType() == HUNTER_PET; }
        bool isTemporarySummoned() const { return _duration > 0; }

        bool IsPermanentPetFor(Player* owner) const;        // pet have tab in character windows and set UNIT_FIELD_PETNUMBER

        bool Create(ObjectGuid::LowType guidlow, Map* map, uint32 Entry);
        bool CreateBaseAtCreature(Creature* creature);
        bool CreateBaseAtCreatureInfo(CreatureTemplate const* cinfo, Unit* owner);
        bool CreateBaseAtTamed(CreatureTemplate const* cinfo, Map* map);
        bool LoadPetData(Player* owner, uint32 petentry = 0, uint32 petnumber = 0, bool current = false);
        bool IsLoading() const override { return _isLoading;}
        void SavePetToDB(PetSaveMode mode);
        void Remove(PetSaveMode mode, bool returnreagent = false);
        static void DeleteFromDB(uint32 guidlow);

        void setDeathState(DeathState s) override;                   // overwrite virtual Creature::setDeathState and Unit::setDeathState
        void Update(uint32 diff) override;                           // overwrite virtual Creature::Update and Unit::Update

        uint8 GetPetAutoSpellSize() const override { return uint8(_autoSpells.size()); }
        uint32 GetPetAutoSpellOnPos(uint8 pos) const override
        {
            if (pos >= _autoSpells.size())
                return 0;
            else
                return _autoSpells[pos];
        }

        void GivePetXP(uint32 xp);
        void GivePetLevel(uint8 level);
        void SynchronizeLevelWithOwner();
        bool HaveInDiet(ItemTemplate const* item) const;
        uint32 GetCurrentFoodBenefitLevel(uint32 itemlevel) const;
        void SetDuration(int32 dur) { _duration = dur; }
        int32 GetDuration() const { return _duration; }

        /*
        bool UpdateStats(Stats stat);
        bool UpdateAllStats();
        void UpdateResistances(uint32 school);
        void UpdateArmor();
        void UpdateMaxHealth();
        void UpdateMaxPower(Powers power);
        void UpdateAttackPowerAndDamage(bool ranged = false);
        void UpdateDamagePhysical(WeaponAttackType attType) override;
        */

        void ToggleAutocast(SpellInfo const* spellInfo, bool apply);

        bool HasSpell(uint32 spell) const override;

        void LearnPetPassives();
        void CastPetAuras(bool current);
        void CastPetAura(PetAura const* aura);
        bool IsPetAura(Aura const* aura);

        void _LoadSpellCooldowns();
        void _LoadAuras(uint32 timediff);
        void _SaveAuras(SQLTransaction& trans);
        void _LoadSpells();
        void _SaveSpells(SQLTransaction& trans);

        bool addSpell(uint32 spellId, ActiveStates active = ACT_DECIDE, PetSpellState state = PETSPELL_NEW, PetSpellType type = PETSPELL_NORMAL);
        bool learnSpell(uint32 spell_id);
        void learnSpells(std::vector<uint32> const& spellIds);
        void learnSpellHighRank(uint32 spellid);
        void InitLevelupSpellsForLevel();
        bool unlearnSpell(uint32 spell_id, bool learn_prev, bool clear_ab = true);
        void unlearnSpells(std::vector<uint32> const& spellIds, bool learn_prev, bool clear_ab = true);
        bool removeSpell(uint32 spell_id, bool learn_prev, bool clear_ab = true);
        void CleanupActionBar();
        std::string GenerateActionBarData() const;

        PetSpellMap     _spells;
        AutoSpellList   _autoSpells;

        void InitPetCreateSpells();

        uint16 GetSpecialization() { return _petSpec; }
        void SetSpecialization(uint16 spec);
        void LearnSpecializationSpells();
        void RemoveSpecializationSpells(bool clearActionBar);

        uint32 GetGroupUpdateFlag() const { return _groupUpdateMask; }
        void SetGroupUpdateFlag(uint32 flag);
        void ResetGroupUpdateFlag();

        DeclinedName const* GetDeclinedNames() const { return _declinedName; }
        // prevent overwrite pet state in DB at next Pet::Update if pet already removed(saved)
        bool    _isRemoved;

        Player* GetOwner() const;

        uint32 GetSlot() { return _petSlot; }
        void SetSlot(uint32 newPetSlot) { _petSlot = newPetSlot; } // use only together with DB update

        bool IsActive() { return _isPetActive; }
        void SetActive(bool active) { _isPetActive = active; }

    protected:
        PetType _petType;
        // time until unsummon (used mostly for summoned guardians and not used for controlled pets)
        int32   _duration;                                 
        bool    _isLoading;
        uint32  _focusRegenTimer;
        uint32  _groupUpdateMask;

        DeclinedName *_declinedName;
        // pet specialization id
        uint16 _petSpec;
        uint32 _petSlot;
        bool _isPetActive = false;

    private:
        // override of Creature::SaveToDB     - must not be called
        void SaveToDB(uint32, DifficultyVector const&) override              
        {
            ABORT();
        }
        // override of Creature::DeleteFromDB - must not be called
        void DeleteFromDB() override                                
        {
            ABORT();
        }
};
#endif
