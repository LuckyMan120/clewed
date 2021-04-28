CREATE TABLE IF NOT EXISTS `maenna_service_payments` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `txn_id` varchar(64) NOT NULL,
  `user_id` bigint(20) NOT NULL,
  `service_id` bigint(20) NOT NULL,
  `amount` decimal(10,2) NOT NULL,
  `status` int(11) NOT NULL DEFAULT '0',
  `datetime` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `txn_id` (`txn_id`),
  KEY `user_id` (`user_id`),
  KEY `service_id` (`service_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;