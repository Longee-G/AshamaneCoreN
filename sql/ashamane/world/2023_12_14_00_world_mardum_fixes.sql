--
-- Kayn Sunfury Wellcome SAI fixes
UPDATE `smart_scripts` SET `action_param2`=6000, `action_param3`=0, `action_param4`=20540931, `target_type`=1, `target_param1`=0, `target_param2`=0
	WHERE `entryorguid`=93011 AND `source_type`=0 AND `id`=2 AND `event_type`=52;

-- Kor'vas Bloodthorn SAI fixes
-- 20540931 or 20540919
UPDATE `smart_scripts` SET  `target_type`=1, `target_param1`=0, `target_param2`=0 WHERE `entryorguid`=98292 AND `source_type`=0 AND `id`=0 AND `event_type`=52;
UPDATE `creature_text` SET `Type`=6, `Emote`=15 WHERE `CreatureID` = 98292;	-- Yells

-- Remove invalid [aura:195821]
UPDATE `creature_addon` SET `auras`='' WHERE `auras`='195821' AND `guid` IN (20541317, 20541333, 20541320);

-- update GameObject:250560 `Legion Banner`
SET @GOID := 250560;
UPDATE `gameobject_template` SET `ScriptName`='go_mardum_legion_banner_1', `Data10`=40077 WHERE `entry`=@GOID;	-- Legion Banner
UPDATE `gameobject_template_addon` SET `flags`=4 WHERE `entry`=@GOID;	-- update flags: 262144(0x40000) to 4

--
-- quest-40378 Enter the Illidari: Ashtongue
--
UPDATE `gameobject_template` SET `ScriptName`='go_mardum_portal_ashtongue' WHERE `entry`=241751;
UPDATE `gameobject_template` SET `Data10`=0 WHERE `entry`=241751; -- spell: 184561  Data10  NOT found

-- update gameobject script
UPDATE `gameobject_template` SET `ScriptName`='go_mardum_cage_belath' WHERE `entry`=242989;
UPDATE `gameobject_template` SET `ScriptName`='go_mardum_cage_cyana' WHERE `entry`=244916;
UPDATE `gameobject_template` SET `ScriptName`='go_mardum_cage_izal' WHERE `entry`=242987;
UPDATE `gameobject_template` SET `ScriptName`='go_mardum_cage_mannethrel' WHERE `entry`=242990;
-- update creature script
-- UPDATE `creature_template` SET `ScriptName`='npc_mardum_allari' WHERE `entry`=94410;

-- go_mardum_portal_coilskar
UPDATE `gameobject_template` SET `ScriptName`='go_mardum_portal_coilskar' WHERE `entry`=241756;
UPDATE `gameobject_template` SET `ScriptName`='go_meeting_with_queen_ritual' WHERE `entry`=243335;
UPDATE `gameobject_template` SET `ScriptName`='go_mardum_portal_shivarra' WHERE `entry`=241757;
UPDATE `gameobject_template` SET `ScriptName`='go_mardum_illidari_banner' WHERE `entry` IN (243968, 243967, 243965);
UPDATE `gameobject_template` SET `ScriptName`='go_mardum_the_keystone' WHERE `entry`=245728; -- Sargerite Keystone

-- npc 100982 Sevis Brightflame
SET @NPC:=100982;
SET @PATH:=@NPC*10;
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_type`, `action`, `action_chance`, `wpguid`) VALUES
(@PATH, 1, 827.307, 2760.44, -30.6905, 0, 0, 1, 0, 100, 0),
(@PATH, 2, 825.709, 2765.81, -30.7838, 0, 0, 1, 0, 100, 0),
(@PATH, 3, 822.158, 2770.88, -30.7162, 0, 0, 1, 0, 100, 0),
(@PATH, 4, 814.814, 2774.30, -31.5780, 0, 0, 1, 0, 100, 0),
(@PATH, 5, 808.102, 2772.58, -32.7075, 0, 0, 1, 0, 100, 0),
(@PATH, 6, 797.479, 2765.08, -34.7338, 0, 0, 1, 0, 100, 0),
(@PATH, 7, 787.555, 2757.01, -37.4998, 0, 0, 1, 0, 100, 0),
(@PATH, 8, 777.271, 2746.22, -41.4613, 0, 0, 1, 0, 100, 0),
(@PATH, 9, 770.216, 2737.83, -44.0264, 0, 0, 1, 0, 100, 0);
-- DELETE FROM `smart_scripts` WHERE `entryorguid`=@NPC;
UPDATE `creature_template` SET `ScriptName`='npc_sevis_enter_coilskar' WHERE entry=@NPC;  -- `AIName`=''
UPDATE `creature` SET `PhaseId`=173 WHERE `guid`=20541160 AND `id`=@NPC;

-- delete duplicate NPC
DELETE FROM `creature` WHERE `guid`=20541303; 
UPDATE `creature_template` SET `ScriptName`='npc_mardum_sevis_99917' WHERE entry=99917; 
-- UPDATE `smart_scripts` SET `event_flags`=0 WHERE `entryorguid`=99917 AND `event_type`=10 AND `source_type`=0;	-- 1 --> 0
UPDATE `creature_text` SET `BroadcastTextId`=101659 WHERE `CreatureID`=99917 AND `GroupID`=1;
UPDATE `creature_text` SET `BroadcastTextId`=101660, `Text`="This Ashtongue mystic will willingly sacrifice his soul for the cause if you\'re the one to do it." WHERE `CreatureID`=99917 AND `GroupID`=2;


-- 99914 - Ashtongue Mystic
UPDATE `creature` SET `PhaseId`=180 WHERE `id`=99914 AND `map`=1481; 

-- Ashtongue Mystic SAI
SET @ASHTONGUE_MYSTIC := 99914;
UPDATE `creature_template` SET `AIName`="" WHERE `entry`=@ASHTONGUE_MYSTIC;		-- SmartAI
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ASHTONGUE_MYSTIC AND `source_type`=0;
/* 
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ASHTONGUE_MYSTIC,0,0,1,62,0,100,0,19015,0,0,0,33,@ASHTONGUE_MYSTIC ,0,0,0,0,0,7,0,0,0,0,0,0,0,"Ashtongue Mystic - On Gossip Option 0 Selected - Quest Credit ''"),
(@ASHTONGUE_MYSTIC,0,1,2,61,0,100,0,0,0,0,0,1,0,5000,0,0,0,0,1,0,0,0,0,0,0,0,"Ashtongue Mystic - On Gossip Option 0 Selected - Say Line 0"),
(@ASHTONGUE_MYSTIC,0,2,3,61,0,100,0,0,0,0,0,85,196724,0,0,0,0,0,1,0,0,0,0,0,0,0,"Ashtongue Mystic - On Gossip Option 0 Selected - Invoker Cast ' Mystic's Soul'"),
(@ASHTONGUE_MYSTIC,0,3,4,61,0,100,0,0,0,0,0,37,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Ashtongue Mystic - On Gossip Option 0 Selected - Kill Self"),
(@ASHTONGUE_MYSTIC,0,4,5,61,0,100,0,0,0,0,0,41,0,0,0,0,0,0,11,99917,20,0,0,0,0,0,"Ashtongue Mystic - On Gossip Option 0 Selected - Despawn Instant"),
(@ASHTONGUE_MYSTIC,0,5,0,61,0,100,0,0,0,0,0,12,9991700,6,0,0,0,0,8,0,0,0,756.767,2401.41,-60.9137,1.067820,"Ashtongue Mystic - On Gossip Option 0 Selected - Summon Creature 'Sevis Brightflame'");
*/
-- Ashtongue Mystic Text
DELETE FROM `creature_text` WHERE `CreatureID`=@ASHTONGUE_MYSTIC ;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES 
(@ASHTONGUE_MYSTIC , '0', '0', 'I am as good as dead. Do... what must... be done.', '12', '0', '100', '0', '0', '0', '0', '0', 'Ashtongue Mystic');
