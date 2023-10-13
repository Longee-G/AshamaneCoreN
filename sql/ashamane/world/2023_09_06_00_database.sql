-- fixed npc_vendor goods
UPDATE `npc_vendor` SET `incrtime`=3600 WHERE `maxcount`=2 AND `entry`=111903;
-- fixed spell_script_names
UPDATE `spell_script_names` SET ScriptName='' WHERE spell_id IN(59262,59271,81229,89104);
UPDATE `spell_script_names` SET ScriptName='spell_monk_gift_of_the_ox' WHERE spell_id=124502;
DELETE FROM `spell_script_names` WHERE spell_id=152962;
UPDATE `spell_script_names` SET ScriptName='spell_shadowmoon_burial_grounds_soul_steal' WHERE spell_id=152962;
DELETE FROM `spell_script_names` WHERE spell_id=203179 AND ScriptName='';
DELETE FROM `spell_script_names` WHERE spell_id=845;
INSERT INTO `spell_script_names` (spell_id, ScriptName) VALUES(845, 'spell_warr_cleave');
DELETE FROM `spell_script_names` WHERE spell_id=1680;
INSERT INTO `spell_script_names` (spell_id, ScriptName) VALUES(1680, 'spell_warr_whirlwind');