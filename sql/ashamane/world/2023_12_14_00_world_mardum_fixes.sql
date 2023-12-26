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