-- (bugfixed:#6)
DELETE FROM `item_effect` WHERE `ID` IN(10785,24389,24391);

INSERT INTO `item_effect`
(`ID`, `SpellID`, `CoolDownMSec`, `CategoryCoolDownMSec`, `Charges`, `SpellCategoryID`, `ChrSpecializationID`, `LegacySlotIndex`, `TriggerType`, `ParentItemID`, `VerifiedBuild`) VALUES
(10785,55884,-1,-1,-1,0,0,0,0,46894,26972),			-- #46894(魔化碧玉)
(24389,55884,-1,-1,-1,0,0,0,0,49362,26972),			-- #49362(奥妮克希亚龙宝宝)
(24391,55884,-1,-1,-1,0,0,0,0,44819,26972); 		-- #44819(暴雪熊宝宝)