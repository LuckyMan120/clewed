CREATE TABLE IF NOT EXISTS `audio_files` (
  `id`      INT(11)      NOT NULL AUTO_INCREMENT,
  `hash`    CHAR(40)     NOT NULL,
  `file`    VARCHAR(300) NOT NULL,
  `created` TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `hash` (`hash`),
  KEY `created` (`created`)
)
  ENGINE = InnoDB
  DEFAULT CHARSET = utf8;
