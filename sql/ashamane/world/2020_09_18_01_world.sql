SET @QUEST_ID := 44281;
-- BEGIN quest `To Be Prepared` (44281)  Horde

-- recreate gameobject's info
DELETE FROM `gameobject_template` WHERE `entry` IN (251195, 251250, 251251, 251252, 251253, 251254, 251255, 255930, 255931, 251233, 251234, 251235);
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `size`, `Data0`, `Data1`, `Data2`, `Data3`, `Data4`, `Data5`, `Data6`, `Data7`, `Data8`, `Data9`, `Data10`, `Data11`, `Data12`, `Data13`, `Data14`, `Data15`, `Data16`, `Data17`, `Data18`, `Data19`, `Data20`, `Data21`, `Data22`, `Data23`, `Data24`, `Data25`, `Data26`, `Data27`, `Data28`, `Data29`, `Data30`, `Data31`, `Data32`, `RequiredLevel`, `AIName`, `ScriptName`, `VerifiedBuild`) VALUES
(251195, 10, 14894, 'Keg of Armor Polish', '', 'Polishing', '', 0.75, 93, 0, 0, 1, 0, 0, 1, 0, 0, 0, 215387, 0, 0, 0, 94406, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 22522),
(251250, 10, 12775, 'Baked Fish', '', 'Eating', '', 1, 93, 0, 0, 1, 0, 0, 1, 0, 0, 0, 215607, 0, 0, 1, 92973, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 22566),
(251251, 10, 30219, 'Baked Fowl', '', 'Eating', '', 1, 93, 0, 0, 1, 0, 0, 1, 0, 0, 0, 215607, 0, 0, 1, 92973, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 22566),
(251252, 10, 34807, 'Dumplings', '', 'Eating', '', 1, 93, 0, 0, 1, 0, 0, 1, 0, 0, 0, 215607, 0, 0, 1, 92973, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 22566),
(251253, 10, 34808, 'Fried Rice', '', 'Eating', '', 1, 93, 0, 0, 1, 0, 0, 1, 0, 0, 0, 215607, 0, 0, 1, 92973, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 22566),
(251254, 10, 26597, 'Grilled Fish', '', 'Eating', '', 1, 93, 0, 0, 1, 0, 0, 1, 0, 0, 0, 215607, 0, 0, 1, 92973, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 22566),
(251255, 10, 13595, 'Ribs', '', 'Eating', '', 1, 93, 0, 0, 1, 0, 0, 1, 0, 0, 0, 215607, 0, 0, 1, 92973, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 22566),
(255930, 10, 37165, 'Sun Sphere', '', 'Enchanting', '', 0.03, 93, 0, 0, 1, 0, 0, 1, 0, 0, 0, 215598, 0, 0, 1, 3006, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 22566),
(255931, 3, 10513, 'Cauldron of Mojo', '', 'Opening', '', 1, 57, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 21400, 0, 0, 44153, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 69040, 0, 0, 98, '', '', 22566),
(251233, 10, 15025, 'Light-Infused Crystals', '', 'Enchanting', '', 1, 93, 0, 0, 1, 0, 0, 1, 0, 0, 0, 215598, 0, 0, 1, 3006, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 22522),
(251234, 10, 17266, 'Light-Infused Crystals', '', 'Enchanting', '', 1, 93, 0, 0, 1, 0, 0, 1, 0, 0, 0, 215598, 0, 0, 1, 3006, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 22522),
(251235, 10, 17265, 'Light-Infused Crystals', '', 'Enchanting', '', 1, 93, 0, 0, 1, 0, 0, 1, 0, 0, 0, 215598, 0, 0, 1, 3006, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 22522);

DELETE FROM `gameobject_template_addon` WHERE `entry` IN (251195, 251253, 251250, 251255, 251251, 251254, 251252, 251235, 251234, 251233);
INSERT INTO `gameobject_template_addon` (`entry`, `faction`, `flags`) VALUES
(251195, 0, 262144), -- Keg of Armor Polish
(251253, 0, 262176), -- Fried Rice
(251250, 0, 262176), -- Baked Fish
(251255, 0, 262176), -- Ribs
(251251, 0, 262176), -- Baked Fowl
(251254, 0, 262176), -- Grilled Fish
(251252, 0, 262176), -- Dumplings
(251235, 0, 262176), -- Light-Infused Crystals
(251234, 0, 262176), -- Light-Infused Crystals
(251233, 0, 262176); -- Light-Infused Crystals

-- create `gameobject` associated with quest(44281)
DELETE FROM `gameobject` WHERE `guid` BETWEEN 51014379 AND 51014397;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseUseFlags`, `PhaseId`, `PhaseGroup`, `terrainSwapMap`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `VerifiedBuild`) VALUES
(51014379,255930,1,'0','0','1','0','0','532','-1','1380.75','-4681.56','29.5159','4.21016','-0','-0','-0.860635','0.509223','300','255','1','0','','0'),
(51014380,255930,1,'0','0','1','0','0','532','-1','1377.3','-4677.98','29.2534','3.30302','-0','-0','-0.996744','0.0806262','300','255','1','0','','0'),
(51014381,255930,1,'0','0','1','0','0','532','-1','1375.98','-4674.05','29.0099','3.18128','-0','-0','-0.999803','0.0198426','300','255','1','0','','0'),
(51014382,266793,1,'0','0','1','0','0','532','-1','1380.68','-4681.51','28.2927','4.02166','-0','-0','-0.904738','0.425969','300','255','1','0','','0'),
(51014383,266793,1,'0','0','1','0','0','532','-1','1376.01','-4673.89','27.786','3.13023','-0','-0','-0.999984','-0.00568347','300','255','1','0','','0'),
(51014384,266793,1,'0','0','1','0','0','532','-1','1377.38','-4677.92','28.031','3.83708','-0','-0','-0.940144','0.340779','300','255','1','0','','0'),
(51014385,255931,1,'0','0','1','0','0','532','-1','1321.766','-4612.412','23.98223','4.679367','0','0','0','1','300','255','1','0','','0'),
(51014386,251251,1,'0','0','1','0','0','532','-1','1335.48','-4487.15','26.9135','4.76387','-0','-0','-0.688672','0.725073','300','255','1','0','','0'),
(51014387,251250,1,'0','0','1','0','0','532','-1','1335.76','-4485.73','26.9135','4.72068','-0','-0','-0.704169','0.710032','300','255','1','0','','0'),
(51014388,251253,1,'0','0','1','0','0','532','-1','1333.82','-4481.21','26.6056','4.65785','-0','-0','-0.726125','0.687563','300','255','1','0','','0'),
(51014389,251254,1,'0','0','1','0','0','532','-1','1334.01','-4479.51','26.6056','4.6932','-0','-0','-0.713859','0.700289','300','255','1','0','','0'),
(51014390,251252,1,'0','0','1','0','0','532','-1','1335.98','-4475.77','26.515','4.61466','-0','-0','-0.740801','0.671725','300','255','1','0','','0'),
(51014391,251255,1,'0','0','1','0','0','532','-1','1336.08','-4474.17','26.515','4.71283','-0','-0','-0.70695','0.707263','300','255','1','0','','0'),
(51014392,266789,1,'0','0','1','0','0','532','-1','1335.57','-4486.36','25.7876','2.96139','-0','-0','-0.995943','-0.089981','300','255','1','0','','0'),
(51014393,266789,1,'0','0','1','0','0','532','-1','1336.02','-4474.8','25.389','3.01636','-0','-0','-0.99804','-0.0625741','300','255','1','0','','0'),
(51014394,266789,1,'0','0','1','0','0','532','-1','1333.8','-4480.24','25.4795','3.06741','-0','-0','-0.999312','-0.037082','300','255','1','0','','0'),
(51014395,251195,1,'0','0','1','0','0','532','-1','1304.16','-4582.48','24.3188','0.908225','-0','-0','-0.438665','-0.898651','300','255','1','0','','0'),
(51014396,266789,1,'0','0','1','0','0','532','-1','1304.39','-4583.46','23.1929','0.629424','-0','-0','-0.309543','-0.950886','300','255','1','0','','0'),
(51014397,251195,1,'0','0','1','0','0','532','-1','1305.04','-4583.94','24.3191','0.82576','-0','-0','-0.401249','-0.915969','300','255','1','0','','0');

-- create `Npc` associated with quest(44281)
DELETE FROM `creature` WHERE `guid` BETWEEN 280000478 AND 280000497;
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseUseFlags`, `PhaseId`, `PhaseGroup`, `terrainSwapMap`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `ScriptName`, `VerifiedBuild`) VALUES
(280000478,113550,1,14,4982,1,0,0,532,-1,0,0,1319.85,-4612.89,24.0486,0.0687671,300,0,0,3100,0,0,0,0,0,0,0,'',0),
(280000479,113549,1,14,4982,1,0,0,532,-1,0,0,1322.8,-4613.59,24.0823,2.2642,300,0,0,3100,0,0,0,0,0,0,0,'',0),
(280000480,113539,1,14,4982,1,0,0,532,-1,0,0,1302.22,-4584.89,22.997,0.696165,300,0,0,336523,12082,0,0,0,0,0,0,'',0),
(280000481,113540,1,14,4982,1,0,0,532,-1,0,0,1336.74,-4480.63,25.5877,2.94634,300,0,0,7263,12082,0,0,0,0,0,0,'',0),
(280000482,113541,1,14,4982,1,0,0,532,-1,0,0,1379.03,-4677.16,28.0534,3.46794,300,0,0,7452,12082,0,0,0,0,0,0,'',0),
(280000483,113548,1,14,4982,1,0,0,532,-1,0,0,1323.02,-4610.5,24.0635,4.28279,300,0,0,18630,12082,0,0,0,0,0,0,'',0),
(280000484,113542,1,14,4982,1,0,0,532,-1,0,0,1406.5,-4745.87,28.7449,5.22097,300,0,0,18630,12082,0,0,0,0,0,0,'',0),
(280000485,113544,1,14,4982,1,0,0,532,-1,0,0,1402.65,-4746.58,28.6723,5.67885,300,0,0,18630,12082,0,0,0,0,0,0,'',0),
(280000486,113545,1,14,4982,1,0,0,532,-1,0,0,1415.02,-4750.15,28.5781,4.18818,300,0,0,18630,12082,0,0,0,0,0,0,'',0),
(280000487,113546,1,14,4982,1,0,0,532,-1,0,0,1398.36,-4749.91,28.5535,5.83594,300,0,0,18630,12082,0,0,0,0,0,0,'',0),
(280000488,113947,1,14,4982,1,0,0,532,-1,0,0,1415.67,-4751.92,28.5304,3.86224,300,0,0,18630,12082,0,0,0,0,0,0,'',0),
(280000489,113948,1,14,4982,1,0,0,532,-1,0,0,1408.56,-4746.18,28.7385,4.67513,300,0,0,18630,12082,0,0,0,0,0,0,'',0),
(280000490,113950,1,14,4982,1,0,0,532,-1,0,0,1400.41,-4747.98,28.58,5.58068,300,0,0,18630,12082,0,0,0,0,0,0,'',0),
(280000491,113951,1,14,4982,1,0,0,532,-1,0,0,1415.91,-4754.89,28.4378,3.53237,300,0,0,18630,12082,0,0,0,0,0,0,'',0),
(280000492,113952,1,14,4982,1,0,0,532,-1,0,0,1410.44,-4747.1,28.6775,4.53376,300,0,0,18630,12082,0,0,0,0,0,0,'',0),
(280000493,113954,1,14,4982,1,0,0,532,-1,0,0,1404.55,-4746.02,28.7154,5.56262,300,0,0,18630,12082,0,0,0,0,0,0,'',0),
(280000494,113955,1,14,4982,1,0,0,532,-1,0,0,1415.57,-4757.55,28.3386,3.65804,300,0,0,18630,12082,0,0,0,0,0,0,'',0),
(280000495,113956,1,14,4982,1,0,0,532,-1,0,0,1412.32,-4747.92,28.644,4.65942,300,0,0,18630,12082,0,0,0,0,0,0,'',0),
(280000496,113547,1,14,4982,1,0,0,532,-1,0,0,1419.14,-4904.96,11.3406,1.94271,300,0,0,3225369,0,0,0,0,0,0,0,'',0);	-- Stone Guard Mukar

-- set mount for `Stone Guard Mukar`
INSERT IGNORE INTO `creature_addon` (`guid`, `mount`) VALUES (280000496, 29260);
UPDATE `creature_addon` SET `mount`=29260, `bytes2`=256 WHERE `guid`=280000496;


-- 113540 		Liu-So
-- 113539		Orgek Ironhand
-- 113541		Seleria Dawncaller

-- Creature Texts 
DELETE FROM `creature_text` WHERE `CreatureID` IN (113540, 113539, 113541);
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(113540, 0, 0, 'Sit, friend, and enjoy this meal. You cannot fight on an ampty stomach.', 12, 0, 100, 0, 0, 0, 0, 0, 'Liu-So to Player'),
(113539, 0, 0, 'A demon blade can find even the smallest crack in your armor, $p. Repair it, or you might end up with your head on a spike.', 12, 0, 100, 0, 0, 0, 0, 0, 'Orgek Ironhand to Player'),
(113541, 0, 0, 'Silvermoon stands behind you, $p. Allow us to bless your weapon with holy might.', 12, 0, 100, 0, 0, 0, 0, 0, 'Seleria Dawncaller to Player');

-- Smart Scripts 
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry` IN (113540, 113539, 113541);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (113540, 113539, 113541) AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(113540,0,0,1,10,0,100,1,1,15,25000,25000,1,0,10000,0,0,0,0,18,15,0,0,0,0,0,0,"Liu So - Within 1-15 Range Out of Combat LoS - Say Line 0 (No Repeat)"),
(113539,0,0,1,10,0,100,1,1,15,25000,25000,1,0,10000,0,0,0,0,18,15,0,0,0,0,0,0,"Orgek Ironhaned - Within 1-15 Range Out of Combat LoS - Say Line 0 (No Repeat)"),
(113541,0,0,1,10,0,100,1,1,15,25000,25000,1,0,10000,0,0,0,0,18,15,0,0,0,0,0,0,"Seleria Dawncaller - Within 1-15 Range Out of Combat LoS - Say Line 0 (No Repeat)");

-- 给要与之决斗的npc设置装备...
DELETE FROM `creature_equip_template` WHERE `CreatureID` IN (113955, 113951, 113947, 113545, 113961, 113956, 113952, 113948, 113542, 113954, 113544, 113950, 113546);
INSERT INTO `creature_equip_template` (`CreatureID`, `ID`, `ItemID1`, `AppearanceModID1`, `ItemVisual1`, `ItemID2`, `AppearanceModID2`, `ItemVisual2`, `ItemID3`, `AppearanceModID3`, `ItemVisual3`, `VerifiedBuild`) VALUES
(113955,1,47509,0,0,39716,0,0,0,0,0,25549),
(113951,1,65052,0,0,63680,0,0,0,0,0,25549),
(113947,1,21492,0,0,0,0,0,0,0,0,25549),
(113545,1,0,0,0,0,0,0,0,0,0,25549),
(113961,1,42274,0,0,42277,0,0,0,0,0,25549),
(113956,1,22799,0,0,0,0,0,0,0,0,25549),
(113952,1,42277,0,0,19335,0,0,0,0,0,25549),
(113948,1,50428,0,0,40350,0,0,0,0,0,25549),
(113542,1,0,0,0,0,0,0,0,0,0,25549),
(113954,1,38618,0,0,0,0,0,0,0,0,25549),
(113544,1,67098,0,0,0,0,0,0,0,0,25549),
(113950,1,34988,0,0,34996,0,0,0,0,0,25549),
(113546,1,11932,0,0,0,0,0,0,0,0,25549);

-- 设置`To Be Prepared` 任务的npc交互，这几个npc可以通过和他们交互进入决斗
-- for quest (To Be Prepared)' objective `Warmed up with a duel`
SET @MENU_ID := 19861;
-- gossip menu for quest(44281)'s objective `Warmed up with a duel`
DELETE FROM `gossip_menu` WHERE `MenuId` = @MENU_ID;
INSERT INTO `gossip_menu` (`MenuId`, `TextId`, `VerifiedBuild`) VALUES
(@MENU_ID, 29496, 27843); -- 108750 (Eunna Young);

-- 添加npc对话的菜单选项...
DELETE FROM `gossip_menu_option` WHERE `MenuId` = @MENU_ID;
INSERT INTO `gossip_menu_option` (`MenuId`, `OptionIndex`, `OptionIcon`, `OptionText`, `OptionBroadcastTextId`, `VerifiedBuild`) VALUES
(@MENU_ID, 0, 0, "Let\'s duel.", 114449, 27843);
-- OptionType ： 1 (GOSSIP_OPTION_GOSSIP)
UPDATE `gossip_menu_option` SET `OptionType` = 1, `OptionNpcFlag` = 1 WHERE `MenuId` =  @MENU_ID;


-- 修复npc和quest进行关联的数据...关联和npc对话后开启决斗的脚本
-- 这里设置了duel对象的npc的`ScriptName`，当玩家和npc进行交互后，将由脚本`npc_durotar_duelist`来完成交互
UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_durotar_duelist', `DamageModifier`=2.0, `ArmorModifier`=5.1, `ManaModifier`=3.56, `HealthModifier`=5.2 WHERE `entry` IN (113955, 113951, 113947, 113545, 113961, 113956, 113952, 113948, 113542, 113954, 113544, 113950, 113546);
UPDATE `creature_template` SET `npcflag` = 1, `gossip_menu_id` = @MENU_ID, `minlevel` = 98, `maxlevel` = 100 WHERE `entry` IN (113955, 113951, 113947, 113545, 113961, 113956, 113952, 113948, 113542, 113954, 113544, 113950, 113546);
UPDATE `creature_template` SET `minlevel` = 98, `maxlevel` = 100 WHERE `entry` IN (113540, 113539, 113541);

-- 创建areaId=4982(Dranoshar)关联的相位信息
-- area:374 -- 刃拳海湾
-- area:817 -- 骷髅石
-- area:4982 -- 
DELETE FROM `phase_area` WHERE `AreaId` IN (374, 817, 4982) AND `PhaseId` IN (169, 313, 315, 324);
INSERT INTO `phase_area` (`AreaId`, `PhaseId`, `Comment`) VALUES
(4982, 169, 'Dranoshar Blockade - See All'),
(4982, 313, 'Dranoshar Blockade - Phase 313 After Quest 44281 Taken and Before Quest 44281 Rewarded'),
(4982, 315, 'Dranoshar Blockade - Phase 315 After Quest 44281 Rewarded and Before Quest 40518 Rewarded'),
(4982, 324, 'Dranoshar Blockade - Phase 324 After Quest 44281 Rewarded and Before Quest 40518 Rewarded'),
(374, 313, 'Dranoshar Blockade - See All'),
(817, 313, 'Dranoshar Blockade - See All');

-- 创建相位(phase)关联的条件，type=26是进入相位的条件类型
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 26) AND (`SourceGroup` IN (313, 315, 324)) AND (`SourceEntry` =4982) AND (`ConditionValue1` IN (44281, 40518)); 
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
('26', '313', '4982', '0', '0', '47', '0', '44281', '8', '0', '0', '0', '0', '', 'Dranoshar Blockade - Phase 313 when quest 44281 taken'),
('26', '315', '4982', '0', '0', '47', '0', '44281', '66', '0', '0', '0', '0', '', 'Dranoshar Blockade - Phase 315 when quest 44281 complete'),
('26', '324', '4982', '0', '0', '47', '0', '40518', '8', '0', '0', '0', '0', '', 'Dranoshar Blockade - Phase 324 when quest 40518 taken');



-- END quest `To Be Prepared` (44281) 


--
-- fixed quest `40518`
--
-- from TDB `2018_10_11_04_world.sql`
INSERT IGNORE INTO `npc_text` (`ID`, `Probability0`, `Probability1`, `Probability2`, `Probability3`, `Probability4`, `Probability5`, `Probability6`, `Probability7`, `BroadcastTextId0`, `BroadcastTextId1`, `BroadcastTextId2`, `BroadcastTextId3`, `BroadcastTextId4`, `BroadcastTextId5`, `BroadcastTextId6`, `BroadcastTextId7`, `VerifiedBuild`) VALUES
(29515, 1, 0, 0, 0, 0, 0, 0, 0, 114580, 0, 0, 0, 0, 0, 0, 0, 27843); -- 29515

INSERT IGNORE INTO `gossip_menu` (`MenuId`, `TextId`, `VerifiedBuild`) VALUES
(19870, 29515, 27843); -- 113118 (Captain Russo)   29515 npc_text

INSERT IGNORE INTO `gossip_menu_option` (`MenuId`, `OptionIndex`, `OptionIcon`, `OptionText`, `OptionBroadcastTextId`, `VerifiedBuild`) VALUES
(19870, 0, 0, 'I am ready to face the Legion.', 114581, 27843);
-- (19870, 2, 0, `I\'ve heard this tale before... <Skip the scenario and begin your next mission.>`, 162120, 27843);
UPDATE `gossip_menu_option` SET `OptionType` = 1, `OptionNpcFlag` = 1 WHERE `MenuId` =  19870; -- OptionType ： 1 (GOSSIP_OPTION_GOSSIP)

UPDATE `creature_template` SET `gossip_menu_id`=19870, `ScriptName`='npc_captain_russo', `npcflag`=`npcflag`|1 WHERE `entry`=113118; -- Captain Russo, Horde
-- UPDATE `creature_template` SET `gossip_menu_id`=19870 WHERE `entry`=108920; -- Captain Angelica, Alliance

-- 给cutscene绑定脚本
UPDATE `scene_template` SET `ScriptName`='scene_face_the_legion' WHERE SceneId=1439; -- 7.0 Pre-Launch - Orgrimmar - Client Scene
-- UPDATE `scene_template` SET `ScriptName`='scene_face_the_legion' WHERE SceneId=1335; -- 7.0 Pre-Launch - Stormwind - Client Scene

-- dungeon teleport coordiates update
DELETE FROM `lfg_dungeon_template` WHERE `dungeonId`=908; -- The Battle for Broken Shore
INSERT INTO `lfg_dungeon_template` (`dungeonId`, `name`, `position_x`, `position_y`, `position_z`, `orientation`, `requiredItemLevel`, `VerifiedBuild`) VALUES
(908, 'The Battle for Broken Shore', 2.39286, 1.69455, 5.20573, 3.155920, 0, 26972);
