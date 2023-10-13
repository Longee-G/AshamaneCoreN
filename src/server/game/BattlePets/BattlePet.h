/*
 * Copyright (C) 2017-2018 AshamaneProject <https://github.com/AshamaneProject>
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

#include "BattlePetPackets.h"

#ifndef BattlePet_h__
#define BattlePet_h__

enum BattlePetSaveInfo
{
    BATTLE_PET_UNCHANGED = 0,
    BATTLE_PET_CHANGED   = 1,
    BATTLE_PET_NEW       = 2,
    BATTLE_PET_REMOVED   = 3
};

struct BattlePetOwnerInfo
{
    ObjectGuid Guid;
    uint32 PlayerVirtualRealm = 0;
    uint32 PlayerNativeRealm = 0;
};

// 这个pet用于`宠物对战系统`
class BattlePet
{
public:
    void CalculateStats();
    BattlePetSpeciesEntry const* GetSpecies();

    BattlePetSaveInfo SaveInfo;

    uint32 AccountID;               // Owner account ID
    ObjectGuid Guid;                // journal ID?
    int32 Slot;                     // Battle slot
    std::string Name;               // Pet's Name
    uint32 NameTimeStamp;           // Name timestamp
    uint32 Species = 0;             // pet kinds
    uint32 DisplayModelID;          // Display id for Pet?
    uint16 Breed = 0;               // Breed quality (factor for some states)
    uint8 Quality = 0;              // Pet quality (factor for some states)
    uint32 Abilities[3];            // avialable abilities in battle...
    uint16 Level = 0;               // Pet Level
    uint16 Exp = 0;                 // Pet Exp
    uint16 Flags = 0;               

    uint32 CreatureID = 0;
    uint32 CollarID = 0;            // 
    
    
    uint32 Health = 0;              // Current health
    uint32 MaxHealth = 0;
    uint32 Power = 0;
    uint32 Speed = 0;
    int32 PetGender;                // pet's gender
    
    Optional<BattlePetOwnerInfo> OwnerInfo;

public:
    ObjectGuid::LowType AddToPlayer(Player* player);
    
};

#endif // BattlePet_h__
