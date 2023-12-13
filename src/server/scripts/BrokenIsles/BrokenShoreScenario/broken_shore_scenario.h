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
#ifndef __BROKEN_SHORE_SCENARIO_H__


enum eCreature
{
    // Alliance's NPCs
    NPC_GennGreymane = 90717,       // King of Gilneas
    NPC_Varian = 90713,             // King Varian Wrynn (King of Stormwind)
    NPC_Jaina = 90714,              // Lady Jaina Proudmoore
    NPC_Tirion = 91951,             // Highlord Tirion Fordring

    // Horde's NPCs
    NPC_Voljin = 90708,             // Vol'jin
    NPC_Baine = 90710,              // Baine Bloodhoof
    NPC_Sylvanas = 90709,           // Lady Sylvanas Windrunner (Banshee Queen)

    // The Legion's NPCs
    NPC_Jaraxxus = 105179,          // Lord Jaraxxus
    NPC_Tichondrius = 90688,        // Tichondrius the Darkener
    NPC_Brutallus = 91902,    
    NPC_Guldan = 94276,

    // others
    NPC_Kross = 90544,
};


enum eEnums
{
    _DT_SCENARIA_TEAM = 0,      // data type
    _CIMEMATIC_ID = 999,

    _EVENT_TeleportAlliance = 1,    // teleport alliance at stage 1 beginning
    _EVENT_TeleportHorde = 2,


    // Stage 2     
    TowerDestroyAlliance = 44077,   // Criteria objective ==> [Criteria.db2].Asset
    TowerDestroyHorde = 54114,

    // Stage 3
    DefeatCommanderAlliance = 45131,
    DefeatCommanderHorde = 54109,

    // Stage 5
    DestroyPortalAlliance = 45288,
    DestroyPortalHorde = 54141,

    // Stage 6
    BlackCityRazed_1p = 44384,   // stage progress Increase by 1%
    BlackCityRazed_5p = 53063,
    BlackCityRazed_10p = 53064,

    // Gameobjets 
    GO_ShipAlliance = 251604,
    GO_ShipHorde = 255203,

    GO_TransportAlliance = 251513,      // Alliance Battleship
    GO_TransportHorde = 254124,         // Horde Battleship
    
    GO_SpiresOfWoe = 240194,  // The Tower
};



#endif // !__BROKEN_SHORE_SCENARIO_H__
