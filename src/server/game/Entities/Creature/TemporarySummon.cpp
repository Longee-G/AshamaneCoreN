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

#include "TemporarySummon.h"
#include "CreatureAI.h"
#include "DB2Structure.h"
#include "Log.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "Pet.h"
#include "Player.h"

TempSummon::TempSummon(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject) :
    Creature(isWorldObject), _properties(properties), _type(TEMPSUMMON_MANUAL_DESPAWN),
    _timer(0), _lifetime(0), _visibleBySummonerOnly(false)
{
    if (owner)
        _summonerGUID = owner->GetGUID();

    _unitTypeMask |= UNIT_MASK_SUMMON;
}

Unit* TempSummon::GetSummoner() const
{
    return !_summonerGUID.IsEmpty() ? ObjectAccessor::GetUnit(*this, _summonerGUID) : NULL;
}

Creature* TempSummon::GetSummonerCreatureBase() const
{
    return !_summonerGUID.IsEmpty() ? ObjectAccessor::GetCreature(*this, _summonerGUID) : NULL;
}

void TempSummon::Update(uint32 diff)
{
    Creature::Update(diff);

    if (_deathState == DEAD)
    {
        UnSummon();
        return;
    }
    switch (_type)
    {
        case TEMPSUMMON_MANUAL_DESPAWN:
            break;
        case TEMPSUMMON_TIMED_DESPAWN:
        {
            if (_timer <= diff)
            {
                UnSummon();
                return;
            }

            _timer -= diff;
            break;
        }
        case TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT:
        {
            if (!IsInCombat())
            {
                if (_timer <= diff)
                {
                    UnSummon();
                    return;
                }

                _timer -= diff;
            }
            else if (_timer != _lifetime)
                _timer = _lifetime;

            break;
        }

        case TEMPSUMMON_CORPSE_TIMED_DESPAWN:
        {
            if (_deathState == CORPSE)
            {
                if (_timer <= diff)
                {
                    UnSummon();
                    return;
                }

                _timer -= diff;
            }
            break;
        }
        case TEMPSUMMON_CORPSE_DESPAWN:
        {
            // if _deathState is DEAD, CORPSE was skipped
            if (_deathState == CORPSE || _deathState == DEAD)
            {
                UnSummon();
                return;
            }

            break;
        }
        case TEMPSUMMON_DEAD_DESPAWN:
        {
            if (_deathState == DEAD)
            {
                UnSummon();
                return;
            }
            break;
        }
        case TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN:
        {
            // if _deathState is DEAD, CORPSE was skipped
            if (_deathState == CORPSE || _deathState == DEAD)
            {
                UnSummon();
                return;
            }

            if (!IsInCombat())
            {
                if (_timer <= diff)
                {
                    UnSummon();
                    return;
                }
                else
                    _timer -= diff;
            }
            else if (_timer != _lifetime)
                _timer = _lifetime;
            break;
        }
        case TEMPSUMMON_TIMED_OR_DEAD_DESPAWN:
        {
            // if _deathState is DEAD, CORPSE was skipped
            if (_deathState == DEAD)
            {
                UnSummon();
                return;
            }

            if (!IsInCombat() && IsAlive())
            {
                if (_timer <= diff)
                {
                    UnSummon();
                    return;
                }
                else
                    _timer -= diff;
            }
            else if (_timer != _lifetime)
                _timer = _lifetime;
            break;
        }
        default:
            UnSummon();
            TC_LOG_ERROR("entities.unit", "Temporary summoned creature (entry: %u) have unknown type %u of ", GetEntry(), _type);
            break;
    }
}

void TempSummon::InitStats(uint32 duration)
{
    ASSERT(!IsPet());

    _timer = duration;
    _lifetime = duration;

    if (_type == TEMPSUMMON_MANUAL_DESPAWN)
        _type = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;

    Unit* owner = GetSummoner();

    if (owner && IsTrigger() && _spells[0])
    {
        setFaction(owner->getFaction());
        SetLevel(owner->getLevel());
        if (owner->GetTypeId() == TYPEID_PLAYER)
            _isControlledByPlayer = true;
    }

    if (!_properties)
        return;

    if (owner)
    {
        int32 slot = _properties->Slot;
        if (slot > 0)
        {
            if (!owner->_summonSlots[slot].IsEmpty() && owner->_summonSlots[slot] != GetGUID())
            {
                Creature* oldSummon = GetMap()->GetCreature(owner->_summonSlots[slot]);
                if (oldSummon && oldSummon->IsSummon())
                    oldSummon->ToTempSummon()->UnSummon();
            }
            owner->_summonSlots[slot] = GetGUID();
        }
    }

    if (_properties->Faction)
        setFaction(_properties->Faction);
    else if (IsVehicle() && owner) // properties should be vehicle
        setFaction(owner->getFaction());
}

void TempSummon::InitSummon(Spell const* summonSpell /*= nullptr*/)
{
    Unit* owner = GetSummoner();
    if (owner)
    {
        owner->AddSummonedCreature(GetGUID(), GetEntry());

        if (owner->GetTypeId() == TYPEID_UNIT && owner->ToCreature()->IsAIEnabled)
            owner->ToCreature()->AI()->JustSummoned(this);
        if (IsAIEnabled)
        {
            AI()->IsSummonedBy(owner);

            if (summonSpell)
                AI()->IsSummonedBy(summonSpell);
        }
    }
}

void TempSummon::UpdateObjectVisibilityOnCreate()
{
    WorldObject::UpdateObjectVisibility(true);
}

void TempSummon::SetTempSummonType(TempSummonType summonType)
{
    _type = summonType;
}

void TempSummon::UnSummon(uint32 msTime)
{
    if (msTime)
    {
        ForcedUnsummonDelayEvent* pEvent = new ForcedUnsummonDelayEvent(*this);

        _events.AddEvent(pEvent, _events.CalculateTime(msTime));
        return;
    }

    //ASSERT(!IsPet());
    if (IsPet())
    {
        ToPet()->Remove(PET_SAVE_DISMISS);
        ASSERT(!IsInWorld());
        return;
    }

    if (Unit* owner = GetSummoner())
    {
        owner->RemoveSummonedCreature(GetGUID());

        if (owner->IsCreature() && owner->ToCreature()->IsAIEnabled)
            owner->ToCreature()->AI()->SummonedCreatureDespawn(this);
    }

    AddObjectToRemoveList();
}

bool ForcedUnsummonDelayEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    _owner.UnSummon();
    return true;
}

void TempSummon::RemoveFromWorld()
{
    if (!IsInWorld())
        return;

    if (_properties)
    {
        int32 slot = _properties->Slot;
        if (slot > 0)
            if (Unit* owner = GetSummoner())
                if (owner->_summonSlots[slot] == GetGUID())
                    owner->_summonSlots[slot].Clear();
    }

    //if (GetOwnerGUID())
    //    TC_LOG_ERROR("entities.unit", "Unit %u has owner guid when removed from world", GetEntry());

    Creature::RemoveFromWorld();
}

Minion::Minion(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject)
    : TempSummon(properties, owner, isWorldObject), _owner(owner)
{
    ASSERT(_owner);
    _unitTypeMask |= UNIT_MASK_MINION;
    _followAngle = PET_FOLLOW_ANGLE;
    /// @todo: Find correct way
    //InitCharmInfo();
}

void Minion::InitStats(uint32 duration)
{
    TempSummon::InitStats(duration);

    SetReactState(REACT_PASSIVE);

    SetCreatorGUID(GetOwner()->GetGUID());
    setFaction(GetOwner()->getFaction());

    GetOwner()->SetMinion(this, true);
}

void Minion::RemoveFromWorld()
{
    if (!IsInWorld())
        return;

    GetOwner()->SetMinion(this, false);
    TempSummon::RemoveFromWorld();
}

bool Minion::IsGuardianPet() const
{
    return IsPet() || (_properties && _properties->Control == SUMMON_CATEGORY_PET);
}

bool Minion::IsWarlockMinion() const
{
    switch (GetEntry())
    {
        case ENTRY_IMP:
        case ENTRY_VOIDWALKER:
        case ENTRY_SUCCUBUS:
        case ENTRY_FELHUNTER:
        case ENTRY_FELGUARD:
        case ENTRY_DOOMGUARD:
        case ENTRY_DOOMGUARD_PET:
        case ENTRY_INFERNAL:
        case ENTRY_INFERNAL_LORD_OF_FLAMES:
        case ENTRY_INFERNAL_PET:
        case ENTRY_WILD_IMP:
        case ENTRY_WILD_IMP_DREADSTALKER:
        case ENTRY_DREADSTALKER:
        case ENTRY_DARKGLARE:
        case ENTRY_CHAOS_TEAR:
        case ENTRY_UNSTABLE_TEAR:
        case ENTRY_SHADOWY_TEAR:
            return true;
        default:
            return false;
    }
}

// WL & mage pets/minions get 100% of owners SP
// need some more proofs which are affected and which arent
bool Minion::HasSameSpellPowerAsOwner() const
{
    return IsWarlockMinion() || GetEntry() == ENTRY_WATER_ELEMENTAL;
}

Guardian::Guardian(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject) : Minion(properties, owner, isWorldObject)
, _bonusSpellDamage(0)
{
    memset(_statFromOwner, 0, sizeof(float)*MAX_STATS);
    _unitTypeMask |= UNIT_MASK_GUARDIAN;
    if (properties && (properties->Title == SUMMON_TYPE_PET || properties->Control == SUMMON_CATEGORY_PET))
    {
        _unitTypeMask |= UNIT_MASK_CONTROLABLE_GUARDIAN;
        InitCharmInfo();
    }
}

void Guardian::InitStats(uint32 duration)
{
    Minion::InitStats(duration);

    InitStatsForLevel(GetOwner()->getLevel());

    if (GetOwner()->GetTypeId() == TYPEID_PLAYER && HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
        _charmInfo->InitCharmCreateSpells();

    SetReactState(REACT_AGGRESSIVE);
}

void Guardian::InitSummon(Spell const* summonSpell /*= nullptr*/)
{
    TempSummon::InitSummon(summonSpell);

    if (GetOwner()->GetTypeId() == TYPEID_PLAYER
            && GetOwner()->GetMinionGUID() == GetGUID()
            && !GetOwner()->GetCharmGUID())
    {
        GetOwner()->ToPlayer()->CharmSpellInitialize();
    }
}

Puppet::Puppet(SummonPropertiesEntry const* properties, Unit* owner)
    : Minion(properties, owner, false) //maybe true?
{
    ASSERT(_owner->GetTypeId() == TYPEID_PLAYER);
    _unitTypeMask |= UNIT_MASK_PUPPET;
}

void Puppet::InitStats(uint32 duration)
{
    Minion::InitStats(duration);
    SetLevel(GetOwner()->getLevel());
    SetReactState(REACT_PASSIVE);
}

void Puppet::InitSummon(Spell const* summonSpell /*= nullptr*/)
{
    Minion::InitSummon(summonSpell);
    if (!SetCharmedBy(GetOwner(), CHARM_TYPE_POSSESS))
        ABORT();
}

void Puppet::Update(uint32 time)
{
    Minion::Update(time);
    //check if caster is channelling?
    if (IsInWorld())
    {
        if (!IsAlive())
        {
            UnSummon();
            /// @todo why long distance .die does not remove it
        }
    }
}

void Puppet::RemoveFromWorld()
{
    if (!IsInWorld())
        return;

    RemoveCharmedBy(NULL);
    Minion::RemoveFromWorld();
}
