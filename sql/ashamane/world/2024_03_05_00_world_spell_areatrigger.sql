-- 如果type=3需要在`areatrigger_template_polygon_vertices`中定义数据
-- 这个表的Id是自己定义的还是来源于哪里？ 

INSERT IGNORE INTO `areatrigger_template` (`Id`, `Type`, `Flags`, `Data0`, `Data1`, `Data2`, `Data3`, `Data4`, `Data5`, `VerifiedBuild`) VALUES
(10801, 4, 0, 8, 8, 4, 4, 0.3, 0.3, 22423);



-- 添加丢失的数据，可能不正确，areatrigger需要关联 areatrigger_template 中的triggerId ...
-- 通过area_trigger触发关联到areatrigger_template的ScriptName上 ...
INSERT IGNORE INTO `spell_areatrigger` (`SpellMiscId`, `AreaTriggerId`, `MoveCurveId`, `ScaleCurveId`, `MorphCurveId`, `FacingCurveId`, `DecalPropertiesId`, `TimeToTarget`, `TimeToTargetScale`, `VerifiedBuild`) VALUES
(5218, 10801, 0, 0, 0, 0, 0, 0, 8000, 26972), -- SpellId : 195834