--
-- Kayn Sunfury Wellcome SAI fixes
UPDATE `smart_scripts` SET `action_param2`=6000, `action_param3`=0, `action_param4`=20540931, `target_type`=1, `target_param1`=0, `target_param2`=0
	WHERE `entryorguid`=93011 AND `source_type`=0 AND `id`=2 AND `event_type`=52;

-- Kor'vas Bloodthorn SAI fixes
-- 20540931 or 20540919
UPDATE `smart_scripts` SET  `target_type`=1, `target_param1`=0, `target_param2`=0 WHERE `entryorguid`=98292 AND `source_type`=0 AND `id`=0 AND `event_type`=52;
UPDATE `creature_text` SET `Type`=6, `Emote`=15 WHERE `CreatureID` = 98292;	-- Yells


DELETE FROM `waypoint_data` WHERE `id` IN (10267107, 10267108, 10267109, 10267110, 10267111, 10267112);
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_type`, `action`, `action_chance`, `wpguid`) VALUES
(10267107,1,1178.73,3203.94,51.5596,0,0,1,0,100,0),
(10267107,2,1169.01,3206.02,51.7205,0,0,1,0,100,0),
(10267107,3,1157.38,3202.05,48.5555,0,0,1,0,100,0),
(10267107,4,1142.81,3197.18,43.5603,0,0,1,0,100,0),
(10267107,5,1131.93,3194.71,43.2108,0,0,1,0,100,0),
(10267107,6,1117.65,3191.72,35.4776,0,0,1,0,100,0),
(10267107,7,1101.23,3188.03,31.3267,0,0,1,0,100,0),
(10267107,8,1085.24,3181.91,26.0593,0,0,1,0,100,0),
(10267107,9,1066.22,3172.38,19.1902,0,0,1,0,100,0),
(10267107,10,1057.33,3165.81,17.8223,0,0,1,0,100,0),
(10267108,1,1180.78,3205.39,51.9965,0,0,1,0,100,0),
(10267108,2,1173.74,3209.55,52.7726,0,0,1,0,100,0),
(10267108,3,1159.42,3212.35,50.6096,0,0,1,0,100,0),
(10267108,4,1141.28,3207.38,44.871,0,0,1,0,100,0),
(10267108,5,1131.1,3204.25,44.9873,0,0,1,0,100,0),
(10267108,6,1119.6,3202.27,37.9456,0,0,1,0,100,0),
(10267108,7,1112.31,3200.29,35.7262,0,0,1,0,100,0),
(10267108,8,1097.17,3194.71,31.4578,0,0,1,0,100,0),
(10267108,9,1078.7,3185.25,25.1685,0,0,1,0,100,0),
(10267108,10,1061.93,3176.11,19.0967,0,0,1,0,100,0),
(10267109,1,1174.98,3204.27,51.5042,0,0,1,0,100,0),
(10267109,2,1166.16,3208.31,51.9857,0,0,1,0,100,0),
(10267109,3,1151.73,3207.11,48.0207,0,0,1,0,100,0),
(10267109,4,1139.73,3204.31,43.8143,0,0,1,0,100,0),
(10267109,5,1131.66,3202.43,43.384,0,0,1,0,100,0),
(10267109,6,1118.68,3198.24,36.7279,0,0,1,0,100,0),
(10267109,7,1103.64,3191.97,32.619,0,0,1,0,100,0),
(10267109,8,1092.45,3187.91,28.7512,0,0,1,0,100,0),
(10267109,9,1079.56,3183.88,25.159,0,0,1,0,100,0),
(10267109,10,1063.99,3174.41,19.0846,0,0,1,0,100,0),
(10267109,11,1059.64,3170.84,18.4537,0,0,1,0,100,0),
(10267110,1,1170.24,3207.99,52.2548,0,0,1,0,100,0),
(10267110,2,1156.65,3204.88,49.0171,0,0,1,0,100,0),
(10267110,3,1149.89,3202.98,46.5824,0,0,1,0,100,0),
(10267110,4,1139.25,3199.53,42.8941,0,0,1,0,100,0),
(10267110,5,1131.05,3197,42.6448,0,0,1,0,100,0),
(10267110,6,1119.06,3192.96,35.9639,0,0,1,0,100,0),
(10267110,7,1103.81,3189.32,32.2262,0,0,1,0,100,0),
(10267110,8,1087.39,3185.91,26.9575,0,0,1,0,100,0),
(10267110,9,1073.64,3182.91,23.6075,0,0,1,0,100,0),
(10267110,10,1054.45,3174.69,18.728,0,0,1,0,100,0),
(10267111,1,1168.07,3203.88,51.1538,0,0,1,0,100,0),
(10267111,2,1155.24,3201.22,47.6943,0,0,1,0,100,0),
(10267111,3,1140.44,3196.55,42.6629,0,0,1,0,100,0),
(10267111,4,1131.84,3194.26,43.748,0,0,1,0,100,0),
(10267111,5,1121.43,3191.73,36.3668,0,0,1,0,100,0),
(10267111,6,1112.27,3190.34,34.0611,0,0,1,0,100,0),
(10267111,7,1095.81,3186.08,29.4127,0,0,1,0,100,0),
(10267111,8,1081.67,3179.01,24.9956,0,0,1,0,100,0),
(10267111,9,1070.42,3172.81,20.7183,0,0,1,0,100,0),
(10267111,10,1057.83,3165.26,17.7868,0,0,1,0,100,0),
(10267112,1,1167.82,3202.48,50.798,0,0,1,0,100,0),
(10267112,2,1151.47,3197.79,45.3514,0,0,1,0,100,0),
(10267112,3,1138.01,3195.36,41.5363,0,0,1,0,100,0),
(10267112,4,1130.64,3193.69,41.6789,0,0,1,0,100,0),
(10267112,5,1120.36,3190.48,35.9676,0,0,1,0,100,0),
(10267112,6,1100.82,3185.39,30.8637,0,0,1,0,100,0),
(10267112,7,1081.18,3181.05,25.1364,0,0,1,0,100,0),
(10267112,8,1063.56,3173.27,18.7396,0,0,1,0,100,0),
(10267112,9,1057.57,3168.61,18.2155,0,0,1,0,100,0);


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

