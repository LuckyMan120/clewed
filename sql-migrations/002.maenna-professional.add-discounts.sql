CREATE TABLE `insight_discount` (
  `discount_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `insight_id` int(10) unsigned NOT NULL,
  `code` char(7) NOT NULL,
  `rate` tinyint(3) unsigned NOT NULL,
  `approved` bit(1) NOT NULL DEFAULT b'0',
  PRIMARY KEY (`discount_id`),
  UNIQUE KEY `insight_id` (`insight_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
