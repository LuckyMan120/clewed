ALTER TABLE `users` CHANGE `mail` `mail` VARCHAR( 64 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ;
ALTER TABLE `users` DROP INDEX `mail`, ADD UNIQUE INDEX `mail` (`mail`) ;