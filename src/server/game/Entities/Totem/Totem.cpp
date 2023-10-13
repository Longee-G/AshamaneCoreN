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

#include "Totem.h"
#include "Group.h"
#include "Map.h"
#include "Player.h"
#include "SpellHistory.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "TotemPackets.h"

Totem::Totem(SummonPropertiesEntry const* properties, Unit* owner) : Minion(properties, owner, false)
{
    _unitTypeMask |= UNIT_MASK_TOTEM;
    _duration = 0;
    _type = TOTEM_PASSIVE;
}

void Totem::Update(uint32 time)
{
    if (!GetOwner()->IsAlive() || !IsAlive())
    {
        UnSummon();                                         // remove self
        return;
    }

    if (_duration <= time)
    {
        UnSummon();                                         // remove self
        return;
    }
    else
        _duration -= time;

    Creature::Update(time);
}

void Totem::InitStats(uint32 duration)
{
    // client requires SMSG_TOTEM_CREATED to be sent before adding to world and before removing old totem
    if (Player* owner = GetOwner()->ToPlayer())
    {
        if (_properties->Slot >= SUMMON_SLOT_TOTEM && _properties->Slot < MAX_TOTEM_SLOT)
        {
            WorldPackets::Totem::TotemCreated data;
            data.Totem = GetGUID();
            data.Slot = _properties->Slot - SUMMON_SLOT_TOTEM;
            data.Duration = duration;
            data.SpellID = GetUInt32Value(UNIT_CREATED_BY_SPELL);
            owner->SendDirectMessage(data.Write());
        }

        // set display id depending on caster's race
        if (uint32 totemDisplayId = sSpellMgr->GetModelForTotem(GetUInt32Value(UNIT_CREATED_BY_SPELL), owner->getRace()))
            SetDisplayId(totemDisplayId);
    }

    Minion::InitStats(duration);

    // Get spell cast by totem
    if (SpellInfo const* totemSpell = sSpellMgr->GetSpellInfo(GetSpell()))
        if (totemSpell->CalcCastTime(getLevel()))   // If spell has cast time -> its an active totem
            _type = TOTEM_ACTIVE;

    _duration = duration;

    SetLevel(GetOwner()->getLevel());
}

void Totem::InitSummon(Spell const* /*summonSpell*/ /*= nullptr*/)
{
    SetControlled(true, UNIT_STATE_ROOT);

    if (_type == TOTEM_PASSIVE && GetSpell())
        CastSpell(this, GetSpell(), true);

    // Some totems can have both instant effect and passive spell
    if (GetSpell(1))
        CastSpell(this, GetSpell(1), true);
}

void Totem::UnSummon(uint32 msTime)
{
    if (msTime)
    {
        _events.AddEvent(new ForcedUnsummonDelayEvent(*this), _events.CalculateTime(msTime));
        return;
    }

    CombatStop();
    RemoveAurasDueToSpell(GetSpell(), GetGUID());

    // clear owner's totem slot
    for (uint8 i = SUMMON_SLOT_TOTEM; i < MAX_TOTEM_SLOT; ++i)
    {
        if (GetOwner()->_summonSlots[i] == GetGUID())
        {
            GetOwner()->_summonSlots[i].Clear();
            break;
        }
    }

    GetOwner()->RemoveAurasDueToSpell(GetSpell(), GetGUID());

    // remove aura all party members too
    if (Player* owner = GetOwner()->ToPlayer())
    {
        owner->SendAutoRepeatCancel(this);

        if (SpellInfo const* spell = sSpellMgr->GetSpellInfo(GetUInt32Value(UNIT_CREATED_BY_SPELL)))
            GetSpellHistory()->SendCooldownEvent(spell, 0, nullptr, false);

        if (Group* group = owner->GetGroup())
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* target = itr->GetSource();
                if (target && group->SameSubGroup(owner, target))
                    target->RemoveAurasDueToSpell(GetSpell(), GetGUID());
            }
        }
    }

    AddObjectToRemoveList();
}

bool Totem::IsImmunedToSpellEffect(SpellInfo const* spellInfo, uint32 index, Unit* caster) const
{
    /// @todo possibly all negative auras immune?
    if (GetEntry() == 5925)
        return false;
    if (SpellEffectInfo const* effect = spellInfo->GetEffect(GetMap()->GetDifficultyID(), index))
    {
        switch (effect->ApplyAuraName)
        {
        case SPELL_AURA_PERIODIC_DAMAGE:
        case SPELL_AURA_PERIODIC_LEECH:
        case SPELL_AURA_MOD_FEAR:
        case SPELL_AURA_MOD_FEAR_2:
        case SPELL_AURA_TRANSFORM:
            return true;
        default:
            break;
        }
    }
    else
        return true;

    return Creature::IsImmunedToSpellEffect(spellInfo, index, caster);
}
