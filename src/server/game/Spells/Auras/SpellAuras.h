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

#ifndef TRINITY_SPELLAURAS_H
#define TRINITY_SPELLAURAS_H

#include "SpellAuraDefines.h"
#include "SpellInfo.h"
#include "Unit.h"

class SpellInfo;
struct SpellModifier;
struct ProcTriggerSpell;
struct SpellProcEntry;

namespace WorldPackets
{
    namespace Spells
    {
        struct AuraInfo;
    }
}

// forward decl
class AuraEffect;
class Aura;
class DynamicObject;
class AuraScript;
class ProcInfo;
class ChargeDropEvent;

// update aura target map every 500 ms instead of every update - reduce amount of grid searcher calls
#define UPDATE_TARGET_MAP_INTERVAL 500

class TC_GAME_API AuraApplication
{
    friend void Unit::_ApplyAura(AuraApplication * aurApp, uint32 effMask);
    friend void Unit::_UnapplyAura(AuraApplicationMap::iterator &i, AuraRemoveMode removeMode);
    friend void Unit::_ApplyAuraEffect(Aura* aura, uint8 effIndex);
    friend void Unit::RemoveAura(AuraApplication * aurApp, AuraRemoveMode mode);
    friend AuraApplication * Unit::_CreateAuraApplication(Aura* aura, uint32 effMask);
    private:
        Unit* const _target;
        Aura* const _base;
        AuraRemoveMode _removeMode:8;                  // Store info for know remove aura reason
        uint8 _slot;                                   // Aura slot on unit
        uint8 _flags;                                  // Aura info flag
        uint32 _effectsToApply;                         // Used only at spell hit to determine which effect should be applied
        bool _needClientUpdate:1;
        uint32 _effectMask;

        explicit AuraApplication(Unit* target, Unit* caster, Aura* base, uint32 effMask);
        void _Remove();
    private:
        void _InitFlags(Unit* caster, uint32 effMask);
        void _HandleEffect(uint8 effIndex, bool apply);
    public:

        Unit* GetTarget() const { return _target; }
        Aura* GetBase() const { return _base; }

        uint8 GetSlot() const { return _slot; }
        uint8 GetFlags() const { return _flags; }
        uint32 GetEffectMask() const { return _effectMask; }
        bool HasEffect(uint8 effect) const { ASSERT(effect < MAX_SPELL_EFFECTS);  return (_effectMask & (1 << effect)) != 0; }
        bool IsPositive() const { return (_flags & AFLAG_POSITIVE) != 0; }
        bool IsSelfcast() const { return (_flags & AFLAG_NOCASTER) != 0; }
        uint32 GetEffectsToApply() const { return _effectsToApply; }

        void SetRemoveMode(AuraRemoveMode mode) { _removeMode = mode; }
        AuraRemoveMode GetRemoveMode() const { return _removeMode; }

        void SetNeedClientUpdate();
        bool IsNeedClientUpdate() const { return _needClientUpdate; }
        void BuildUpdatePacket(WorldPackets::Spells::AuraInfo& auraInfo, bool remove);
        void SendFakeAuraUpdate(uint32 auraId, bool remove);
        void ClientUpdate(bool remove = false);
};

#pragma pack(push, 1)
// Structure representing database aura primary key fields
struct AuraKey
{
    ObjectGuid Caster;
    ObjectGuid Item;
    uint32 SpellId;
    uint32 EffectMask;

    bool operator<(AuraKey const& right) const
    {
        return memcmp(this, &right, sizeof(*this)) < 0;
    }
};

struct AuraLoadEffectInfo
{
    std::array<int32, MAX_SPELL_EFFECTS> Amounts;
    std::array<int32, MAX_SPELL_EFFECTS> BaseAmounts;
};
#pragma pack(pop)

class TC_GAME_API Aura
{
    friend Aura* Unit::_TryStackingOrRefreshingExistingAura(SpellInfo const* newAura, uint32 effMask, Unit* caster, int32 *baseAmount, Item* castItem, ObjectGuid casterGUID, bool resetPeriodicTimer, ObjectGuid castItemGuid, int32 castItemLevel);
    public:
        Ashamane::AnyData Variables;

        typedef std::map<ObjectGuid, AuraApplication*> ApplicationMap;

        static uint32 BuildEffectMaskForOwner(SpellInfo const* spellProto, uint32 availableEffectMask, WorldObject* owner);
        static Aura* TryRefreshStackOrCreate(SpellInfo const* spellproto, ObjectGuid castId, uint32 tryEffMask, WorldObject* owner, Unit* caster, int32* baseAmount = nullptr, Item* castItem = nullptr, ObjectGuid casterGUID = ObjectGuid::Empty, bool* refresh = nullptr, bool resetPeriodicTimer = true, ObjectGuid castItemGuid = ObjectGuid::Empty, int32 castItemLevel = -1);
        static Aura* TryCreate(SpellInfo const* spellproto, ObjectGuid castId, uint32 tryEffMask, WorldObject* owner, Unit* caster, int32* baseAmount = nullptr, Item* castItem = nullptr, ObjectGuid casterGUID = ObjectGuid::Empty, ObjectGuid castItemGuid = ObjectGuid::Empty, int32 castItemLevel = -1);
        static Aura* Create(SpellInfo const* spellproto, ObjectGuid castId, uint32 effMask, WorldObject* owner, Unit* caster, int32* baseAmount, Item* castItem, ObjectGuid casterGUID, ObjectGuid castItemGuid, int32 castItemLevel);
        Aura(SpellInfo const* spellproto, ObjectGuid castId, WorldObject* owner, Unit* caster, Item* castItem, ObjectGuid casterGUID, ObjectGuid castItemGuid, int32 castItemLevel);
        void _InitEffects(uint32 effMask, Unit* caster, int32 *baseAmount);
        virtual ~Aura();

        SpellInfo const* GetSpellInfo() const { return _spellInfo; }
        uint32 GetId() const{ return GetSpellInfo()->Id; }

        ObjectGuid GetCastGUID() const { return _castGuid; }
        ObjectGuid GetCasterGUID() const { return _casterGuid; }
        ObjectGuid GetCastItemGUID() const { return _castItemGuid; }
        int32 GetCastItemLevel() const { return _castItemLevel; }
        uint32 GetSpellXSpellVisualId() const { return _spellXSpellVisualId; }
        void SetSpellXSpellVisualId(uint32 visual) { _spellXSpellVisualId = visual; }

        Unit* GetCaster() const;
        WorldObject* GetOwner() const { return _owner; }
        Unit* GetUnitOwner() const { ASSERT(GetType() == UNIT_AURA_TYPE); return (Unit*)_owner; }
        DynamicObject* GetDynobjOwner() const { ASSERT(GetType() == DYNOBJ_AURA_TYPE); return (DynamicObject*)_owner; }

        AuraObjectType GetType() const;

        virtual void _ApplyForTarget(Unit* target, Unit* caster, AuraApplication * auraApp);
        virtual void _UnapplyForTarget(Unit* target, Unit* caster, AuraApplication * auraApp);
        void _Remove(AuraRemoveMode removeMode);
        virtual void Remove(AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT) = 0;

        virtual void FillTargetMap(std::unordered_map<Unit*, uint32>& targets, Unit* caster) = 0;
        void UpdateTargetMap(Unit* caster, bool apply = true);

        void _RegisterForTargets() {Unit* caster = GetCaster(); UpdateTargetMap(caster, false);}
        void ApplyForTargets() {Unit* caster = GetCaster(); UpdateTargetMap(caster, true);}
        void _ApplyEffectForTargets(uint8 effIndex);

        void UpdateOwner(uint32 diff, WorldObject* owner);
        void Update(uint32 diff, Unit* caster);

        time_t GetApplyTime() const { return _applyTime; }
        int32 GetMaxDuration() const { return _maxDuration; }
        void SetMaxDuration(int32 duration) { _maxDuration = duration; }
        int32 CalcMaxDuration() const { return CalcMaxDuration(GetCaster()); }
        int32 CalcMaxDuration(Unit* caster) const;
        int32 GetDuration() const { return _duration; }
        void SetDuration(int32 duration, bool withMods = false);
        void ModDuration(int32 duration, bool withMods = false) { SetDuration(GetDuration() + duration, withMods); }
        void RefreshDuration(bool withMods = false);
        void RefreshTimers(bool resetPeriodicTimer);
        bool IsExpired() const { return !GetDuration() && !_dropEvent; }
        bool IsPermanent() const { return GetMaxDuration() == -1; }
        
        

        uint8 GetCharges() const { return _procCharges; }
        void SetCharges(uint8 charges);
        uint8 CalcMaxCharges(Unit* caster) const;
        uint8 CalcMaxCharges() const { return CalcMaxCharges(GetCaster()); }
        bool ModCharges(int32 num, AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT);
        bool DropCharge(AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT) { return ModCharges(-1, removeMode); }
        void ModChargesDelayed(int32 num, AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT);
        void DropChargeDelayed(uint32 delay, AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT);

        uint8 GetStackAmount() const { return _stackAmount; }
        uint32 GetMaxStackAmount() const;
        void SetStackAmount(uint8 num);
        bool ModStackAmount(int32 num, AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT, bool resetPeriodicTimer = true, bool refresh = true);

        uint8 GetCasterLevel() const { return _casterLevel; }

        bool HasMoreThanOneEffectForType(AuraType auraType) const;
        bool IsArea() const;
        bool IsPassive() const;
        bool IsDeathPersistent() const;

        bool IsRemovedOnShapeLost(Unit* target) const
        {
            return GetCasterGUID() == target->GetGUID()
                    && _spellInfo->Stances
                    && !_spellInfo->HasAttribute(SPELL_ATTR2_NOT_NEED_SHAPESHIFT)
                    && !_spellInfo->HasAttribute(SPELL_ATTR0_NOT_SHAPESHIFT);
        }

        bool CanBeSaved() const;
        bool IsRemoved() const { return _isRemoved; }
        // Single cast aura helpers
        bool IsSingleTarget() const {return _isSingleTarget; }
        bool IsSingleTargetWith(Aura const* aura) const;
        void SetIsSingleTarget(bool val) { _isSingleTarget = val; }
        void UnregisterSingleTarget();
        int32 CalcDispelChance(Unit const* auraTarget, bool offensive) const;

        /**
        * @fn AuraKey Aura::GenerateKey(uint32& recalculateMask) const
        *
        * @brief Fills a helper structure containing aura primary key for `character_aura`, `character_aura_effect`, `pet_aura`, `pet_aura_effect` tables.
        *
        * @param [out] recalculateMask Mask of effects that can be recalculated to store in database - not part of aura key.
        *
        * @return Aura key.
        */
        AuraKey GenerateKey(uint32& recalculateMask) const;
        void SetLoadedState(int32 maxDuration, int32 duration, int32 charges, uint8 stackAmount, uint32 recalculateMask, int32* amount);

        // helpers for aura effects
        bool HasEffect(uint8 effIndex) const { return GetEffect(effIndex) != NULL; }
        bool HasEffectType(AuraType type) const;
        AuraEffect* GetEffect(uint32 index) const;
        uint32 GetEffectMask() const;
        void RecalculateAmountOfEffects();
        void HandleAllEffects(AuraApplication * aurApp, uint8 mode, bool apply);

        // Helpers for targets
        ApplicationMap const& GetApplicationMap() { return _applications; }
        void GetApplicationList(Unit::AuraApplicationList& applicationList) const;
        const AuraApplication* GetApplicationOfTarget(ObjectGuid guid) const { ApplicationMap::const_iterator itr = _applications.find(guid); if (itr != _applications.end()) return itr->second; return NULL; }
        AuraApplication* GetApplicationOfTarget(ObjectGuid guid) { ApplicationMap::iterator itr = _applications.find(guid); if (itr != _applications.end()) return itr->second; return NULL; }
        bool IsAppliedOnTarget(ObjectGuid guid) const { return _applications.find(guid) != _applications.end(); }

        void SetNeedClientUpdateForTargets() const;
        void HandleAuraSpecificMods(AuraApplication const* aurApp, Unit* caster, bool apply, bool onReapply);
        void HandleAuraSpecificPeriodics(AuraApplication const* aurApp, Unit* caster);
        bool CanBeAppliedOn(Unit* target);
        bool CheckAreaTarget(Unit* target);
        bool CanStackWith(Aura const* existingAura) const;

        bool IsProcOnCooldown(std::chrono::steady_clock::time_point now) const;
        void AddProcCooldown(std::chrono::steady_clock::time_point cooldownEnd);
        bool IsUsingCharges() const { return _isUsingCharges; }
        void SetUsingCharges(bool val) { _isUsingCharges = val; }
        void PrepareProcToTrigger(AuraApplication* aurApp, ProcEventInfo& eventInfo, std::chrono::steady_clock::time_point now);
        uint32 IsProcTriggeredOnEvent(AuraApplication* aurApp, ProcEventInfo& eventInfo, std::chrono::steady_clock::time_point now) const;
        float CalcProcChance(SpellProcEntry const& procEntry, ProcEventInfo& eventInfo) const;
        void TriggerProcOnEvent(uint32 procEffectMask, AuraApplication* aurApp, ProcEventInfo& eventInfo);
        float CalcPPMProcChance(Unit* actor) const;
        void SetLastProcAttemptTime(std::chrono::steady_clock::time_point lastProcAttemptTime) { _lastProcAttemptTime = lastProcAttemptTime; }
        void SetLastProcSuccessTime(std::chrono::steady_clock::time_point lastProcSuccessTime) { _lastProcSuccessTime = lastProcSuccessTime; }

        // AuraScript
        void LoadScripts();
        bool CallScriptCheckAreaTargetHandlers(Unit* target);
        void CallScriptDispel(DispelInfo* dispelInfo);
        void CallScriptAfterDispel(DispelInfo* dispelInfo);
        bool CallScriptEffectApplyHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, AuraEffectHandleModes mode);
        bool CallScriptEffectRemoveHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, AuraEffectHandleModes mode);
        void CallScriptAfterEffectApplyHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, AuraEffectHandleModes mode);
        void CallScriptAfterEffectRemoveHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, AuraEffectHandleModes mode);
        bool CallScriptEffectPeriodicHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp);
        void CallScriptEffectUpdatePeriodicHandlers(AuraEffect* aurEff);
        void CallScriptAuraUpdateHandlers(uint32 diff);
        void CallScriptEffectCalcAmountHandlers(AuraEffect const* aurEff, int32 & amount, bool & canBeRecalculated);
        void CallScriptEffectCalcPeriodicHandlers(AuraEffect const* aurEff, bool & isPeriodic, int32 & amplitude);
        void CallScriptEffectCalcSpellModHandlers(AuraEffect const* aurEff, SpellModifier* & spellMod);
        void CallScriptEffectAbsorbHandlers(AuraEffect* aurEff, AuraApplication const* aurApp, DamageInfo & dmgInfo, uint32 & absorbAmount, bool & defaultPrevented);
        void CallScriptEffectAfterAbsorbHandlers(AuraEffect* aurEff, AuraApplication const* aurApp, DamageInfo & dmgInfo, uint32 & absorbAmount);
        void CallScriptEffectManaShieldHandlers(AuraEffect* aurEff, AuraApplication const* aurApp, DamageInfo & dmgInfo, uint32 & absorbAmount, bool & defaultPrevented);
        void CallScriptEffectAfterManaShieldHandlers(AuraEffect* aurEff, AuraApplication const* aurApp, DamageInfo & dmgInfo, uint32 & absorbAmount);
        void CallScriptEffectSplitHandlers(AuraEffect* aurEff, AuraApplication const* aurApp, DamageInfo & dmgInfo, uint32 & splitAmount);
        void CallScriptEffectCalcCritChanceHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, Unit* victim, float& chance);
        // Spell Proc Hooks
        bool CallScriptCheckProcHandlers(AuraApplication const* aurApp, ProcEventInfo& eventInfo);
        bool CallScriptCheckEffectProcHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, ProcEventInfo& eventInfo);
        bool CallScriptPrepareProcHandlers(AuraApplication const* aurApp, ProcEventInfo& eventInfo);
        bool CallScriptProcHandlers(AuraApplication const* aurApp, ProcEventInfo& eventInfo);
        void CallScriptAfterProcHandlers(AuraApplication const* aurApp, ProcEventInfo& eventInfo);
        bool CallScriptEffectProcHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, ProcEventInfo& eventInfo);
        void CallScriptAfterEffectProcHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, ProcEventInfo& eventInfo);

        template<class Script>
        Script* GetScript(std::string const& scriptName) const
        {
            return dynamic_cast<Script*>(GetScriptByName(scriptName));
        }

        std::vector<AuraScript*> m_loadedScripts;

        AuraEffectVector GetAuraEffects() const { return _effects; }

        SpellEffectInfoVector GetSpellEffectInfos() const { return _spelEffectInfos; }
        SpellEffectInfo const* GetSpellEffectInfo(uint32 index) const;

        AuraScript* GetScriptByName(std::string const& scriptName) const;
    private:
        void _DeleteRemovedApplications();

    protected:
        SpellInfo const* const _spellInfo;
        ObjectGuid const _castGuid;
        ObjectGuid const _casterGuid;
        // it is NOT safe to keep a pointer to the item because it may get deleted
        ObjectGuid const _castItemGuid;                    
        int32 _castItemLevel;
        // how to use this id ???
        uint32 _spellXSpellVisualId;                       
        time_t const _applyTime;
        WorldObject* const _owner;

        // Max aura duration (secs or msecs?)
        int32 _maxDuration;
        // Current time
        int32 _duration;
        // Timer for power per sec calcultion
        int32 _timeCla;
        // Periodic costs
        std::vector<SpellPowerEntry const*> _periodicCosts;
        // Timer for UpdateTargetMapOfEffect
        int32 _updateTargetMapInterval;                    
        // Aura level (store caster level for correct show level dep amount)
        uint8 const _casterLevel;
        // Aura charges (0 for infinite)
        uint8 _procCharges;
        // Aura stack amount
        uint8 _stackAmount;                    

        ApplicationMap _applications;

        bool _isRemoved;
        // true if it's a single target spell and registered at caster - can change at spell steal for example
        bool _isSingleTarget;                              
        bool _isUsingCharges;

        ChargeDropEvent* _dropEvent;

        std::chrono::steady_clock::time_point _procCooldown;
        std::chrono::steady_clock::time_point _lastProcAttemptTime;
        std::chrono::steady_clock::time_point _lastProcSuccessTime;

    private:
        Unit::AuraApplicationList _removedApplications;

        AuraEffectVector _effects;
        SpellEffectInfoVector _spelEffectInfos;
};

class TC_GAME_API UnitAura : public Aura
{
    friend Aura* Aura::Create(SpellInfo const* spellproto, ObjectGuid castId, uint32 effMask, WorldObject* owner, Unit* caster, int32 *baseAmount, Item* castItem, ObjectGuid casterGUID, ObjectGuid castItemGuid, int32 castItemLevel);
    public:
        UnitAura(SpellInfo const* spellproto, ObjectGuid castId, uint32 effMask, WorldObject* owner, Unit* caster, int32 *baseAmount, Item* castItem, ObjectGuid casterGUID, ObjectGuid castItemGuid, int32 castItemLevel);

        void _ApplyForTarget(Unit* target, Unit* caster, AuraApplication * aurApp) override;
        void _UnapplyForTarget(Unit* target, Unit* caster, AuraApplication * aurApp) override;

        void Remove(AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT) override;

        void FillTargetMap(std::unordered_map<Unit*, uint32>& targets, Unit* caster) override;

        // Allow Apply Aura Handler to modify and access m_AuraDRGroup
        void SetDiminishGroup(DiminishingGroup group) { m_AuraDRGroup = group; }
        DiminishingGroup GetDiminishGroup() const { return m_AuraDRGroup; }

    private:
        DiminishingGroup m_AuraDRGroup;                 // Diminishing
};

class TC_GAME_API DynObjAura : public Aura
{
    friend Aura* Aura::Create(SpellInfo const* spellproto, ObjectGuid castId, uint32 effMask, WorldObject* owner, Unit* caster, int32 *baseAmount, Item* castItem, ObjectGuid casterGUID, ObjectGuid castItemGuid, int32 castItemLevel);
    public:
        DynObjAura(SpellInfo const* spellproto, ObjectGuid castId, uint32 effMask, WorldObject* owner, Unit* caster, int32 *baseAmount, Item* castItem, ObjectGuid casterGUID, ObjectGuid castItemGuid, int32 castItemLevel);

        void Remove(AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT) override;

        void FillTargetMap(std::unordered_map<Unit*, uint32>& targets, Unit* caster) override;
};

class TC_GAME_API ChargeDropEvent : public BasicEvent
{
    friend class Aura;
    protected:
        ChargeDropEvent(Aura* base, AuraRemoveMode mode) : _base(base), _mode(mode) { }
        bool Execute(uint64 /*e_time*/, uint32 /*p_time*/) override;

    private:
        Aura* _base;
        AuraRemoveMode _mode;
};

#endif
