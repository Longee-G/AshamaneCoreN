DELETE FROM rbac_permissions WHERE id = 775;
INSERT INTO rbac_permissions VALUES
(775, "Command: modify currency");

DELETE FROM `rbac_linked_permissions` WHERE linkedid = 775;
INSERT INTO `rbac_linked_permissions` VALUES
(192, 775);
