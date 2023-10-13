--
-- fixed spell script 
--
DELETE FROM spell_script_names WHERE spell_id = 65219;
-- fixed spell: 100 binds script
DELETE FROM spell_script_names WHERE spell_id = 100;
INSERT INTO spell_script_names (spell_id, ScriptName) VALUES(100, 'spell_warr_charge_check_cast');

DELETE FROM spell_script_names WHERE spell_id = 103958;

DELETE FROM spell_script_names WHERE spell_id = 527;
INSERT INTO spell_script_names (`spell_id`, `ScriptName`) VALUES(527, 'spell_pri_purify');
-- `spell_gen_trigger_exclude_caster_aura_spell` script delete
DELETE FROM spell_script_names WHERE spell_id IN (172, 1454, 77606, 100780, 100784, 119996, 121471, 131632, 143395, 145158, 146739, 159607, 178338, 194834, 200153, 205523, 212284, 213398, 213831, 219779);


-- fixed spell: 100 binds script
DELETE FROM spell_script_names WHERE spell_id = 100;
INSERT INTO spell_script_names (spell_id, ScriptName) VALUES(100, 'spell_warr_charge');
INSERT INTO spell_script_names (spell_id, ScriptName) VALUES(100, 'spell_gen_trigger_exclude_target_aura_spell');
