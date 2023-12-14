--
-- Kayn Sunfury Wellcome SAI fixes
UPDATE `smart_scripts` SET `action_param2`=6000, `action_param3`=0, `action_param4`=20540931, `target_type`=1, `target_param1`=0, `target_param2`=0
	WHERE `entryorguid`=93011 AND `source_type`=0 AND `id`=2 AND `event_type`=52;

-- Kor'vas Bloodthorn SAI fixes
-- 20540931 or 20540919
UPDATE `smart_scripts` SET  `target_type`=1, `target_param1`=0, `target_param2`=0 WHERE `entryorguid`=98292 AND `source_type`=0 AND `id`=0 AND `event_type`=52;

