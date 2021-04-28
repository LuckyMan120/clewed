CREATE TABLE IF NOT EXISTS `discussion_invites` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `discussion_id` bigint(20) NOT NULL,
  `user_id` bigint(20) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `discussion_id` (`discussion_id`,`user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;