-- 7.3.5 starup quest 联盟任务


-- update warrior's artifact startup quest 
UPDATE `quest_template_addon` SET `PrevQuestID`=0, `NextQuestID`=42815 WHERE (ID=42814);
UPDATE `quest_template_addon` SET `PrevQuestID`=42814, `NextQuestID`=39654 WHERE (ID=42815);
UPDATE `quest_template_addon` SET `PrevQuestID`=42815, `NextQuestID`=0 WHERE (ID=39654);

UPDATE `quest_template_addon` SET `AllowableClasses`=1, `NextQuestID`=38904 WHERE (ID=41052);
-- paladin(2)
UPDATE `quest_template_addon` SET `AllowableClasses`=2, `NextQuestID`=40408 WHERE (ID=38710);
-- hunter(3)
UPDATE `quest_template_addon` SET `AllowableClasses`=4, `NextQuestID`=41415 WHERE (ID=40384);
-- rogue(4) build artifact quest chain...
UPDATE `quest_template_addon` SET `AllowableClasses`=8, `NextQuestID`=40839 WHERE (ID=40832);
UPDATE `quest_template_addon` SET `AllowableClasses`=8, `PrevQuestID`=40832, `NextQuestID`=40840 WHERE (ID=40839);
UPDATE `quest_template_addon` SET `AllowableClasses`=8, `PrevQuestID`=0, `NextQuestID`=0 WHERE (ID=40840);
UPDATE `quest_template_addon` SET `AllowableClasses`=8, `PrevQuestID`=0, `NextQuestID`=40847 WHERE (ID=40843);
UPDATE `quest_template_addon` SET `AllowableClasses`=8, `PrevQuestID`=40843, `NextQuestID`=40849 WHERE (ID=40847);
UPDATE `quest_template_addon` SET `AllowableClasses`=8, `PrevQuestID`=40847, `NextQuestID`=40950 WHERE (ID=40849);
UPDATE `quest_template_addon` SET `AllowableClasses`=8, `PrevQuestID`=0, `NextQuestID`=0 WHERE (ID=40950);
-- priest(5)
UPDATE `quest_template_addon` SET `AllowableClasses`=16, `NextQuestID`=40706 WHERE (ID=40705);
-- death knight(6)
-- UPDATE `quest_template_addon` SET `AllowableClasses`=32, `NextQuestID`= WHERE (ID=);
-- shaman(7)
UPDATE `quest_template_addon` SET `AllowableClasses`=64, `NextQuestID`=41335 WHERE (ID=39746);
-- mage(8)
-- UPDATE `quest_template_addon` SET `AllowableClasses`=128, `NextQuestID`= WHERE (ID=);
-- warlock(9)
UPDATE `quest_template_addon` SET `AllowableClasses`=256, `NextQuestID`=0 WHERE (ID=40716);
-- monk(10)
-- UPDATE `quest_template_addon` SET `AllowableClasses`=512, `NextQuestID`= WHERE (ID=);
-- druid(11)
UPDATE `quest_template_addon` SET `AllowableClasses`=1024, `NextQuestID`=41106 WHERE (ID=40643);
-- demon hunter(12)
UPDATE `quest_template_addon` SET `AllowableClasses`=2048, `NextQuestID`=0 WHERE (ID=39047);