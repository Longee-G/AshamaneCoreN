-- 如果type=3需要在`areatrigger_template_polygon_vertices`中定义数据
-- 这个表的Id是自己定义的还是来源于哪里？ 
SET @TEMP_AT_ID:=99999;		-- 临时的编号

INSERT IGNORE INTO `areatrigger_template` (`Id`, `Type`, `Flags`, `Data0`, `Data1`, `Data2`, `Data3`, `Data4`, `Data5`, `VerifiedBuild`) VALUES
(@TEMP_AT_ID, 0, 2, 0, 2, 0, 0, 0, 0, 26972);


-- 添加丢失的数据，可能不正确，areatrigger需要关联 areatrigger_template 中的triggerId ...
-- 通过area_trigger触发关联到areatrigger_template的ScriptName上 ...
-- 还不确定这个triggerId是否要和AreaTrigger.db2中的ID关联 ...
-- Q: 不同的SpellMiscId是否可以关联到同一个triggerId上？
-- A: 可以的 ... 缺省是不是关联到0上 ... 例如at6038就用了很多次 ...
INSERT IGNORE INTO `spell_areatrigger` (`SpellMiscId`, `AreaTriggerId`, `MoveCurveId`, `ScaleCurveId`, `MorphCurveId`, `FacingCurveId`, `DecalPropertiesId`, `TimeToTarget`, `TimeToTargetScale`, `VerifiedBuild`) VALUES
(5218, @TEMP_AT_ID, 0, 0, 0, 0, 0, 0, 8000, 26972); -- SpellId : 195834