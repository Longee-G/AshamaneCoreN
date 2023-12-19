INSERT IGNORE INTO `rbac_permissions` (`id`, `name`) VALUES ('1000', 'Command: .debug play svk');

INSERT IGNORE INTO `rbac_linked_permissions` VALUES (192, 1000);
