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

-- update GameObjects
UPDATE `gameobject_template` SET `ScriptName`='go_mardum_legion_banner_1' WHERE `entry`=250560;	-- Legion Banner