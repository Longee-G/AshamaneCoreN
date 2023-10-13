-- add miss record to `gameobject_template`
DELETE FROM gameobject_template WHERE entry in (280650, 235794);
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `size`, `Data0`, `Data1`, `Data2`, `Data3`, `Data4`, `Data5`, `Data6`, `Data7`, `Data8`, `Data9`, `Data10`, `Data11`, `Data12`, `Data13`, `Data14`, `Data15`, `Data16`, `Data17`, `Data18`, `Data19`, `Data20`, `Data21`, `Data22`, `Data23`, `Data24`, `Data25`, `Data26`, `Data27`, `Data28`, `Data29`, `Data30`, `Data31`, `Data32`, `RequiredLevel`, `AIName`, `ScriptName`, `VerifiedBuild`) VALUES 
(235794, 45, 20508, 'Order for engineering workshop', '', '', '', 1, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 26972),
(280650, 19, 37393, 'Mailbox', '', '', '', 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 26972);
-- fixed creature<29264> deprecated spell
UPDATE `creature_template` SET `spell1`=0 WHERE (`entry`=29264); 
-- foxed creature_equip_template
UPDATE `creature_equip_template` SET `ItemID2`=0 WHERE (`CreatureID`=132659 AND `ID`=1);
-- fixed SPELL_AURA_CONTROL_VEHICLE
UPDATE `creature_template_addon` SET `auras`='' WHERE entry=41366;
UPDATE `creature_template_addon` SET `auras`='' WHERE entry=42010;
UPDATE `creature_template_addon` SET `auras`='' WHERE entry=43273;
UPDATE `creature_template_addon` SET `auras`='80855 16245' WHERE entry=43279;
-- fixed creature spawndist
UPDATE `creature` SET spawndist=0 WHERE `guid` IN (183746, 183806);
-- fixed gameobject spawnMask
UPDATE `gameobject` SET spawnMask=1 WHERE `guid`=20000097;
-- fixed quest deprecated spell
UPDATE `quest_template` SET RewardSpell=0 WHERE `id`=24974;
-- fixed table `spell_script_names` deprecated spell
-- DELETE FROM `spell_script_names` WHERE `spell_id` IN (59725, 94009);
DELETE FROM `areatrigger_scripts` WHERE `entry` IN 
(11353,11420,12802,1489,149959,151582,1520,2200,2763,314,3691,
373,5555,5559,6022,7214,7455,7625,7630,9677,983);