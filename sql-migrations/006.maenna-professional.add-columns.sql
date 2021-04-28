ALTER TABLE `maenna_professional` ADD `type` TINYINT NOT NULL DEFAULT '0' COMMENT '0 - group insight, 1 - private template, 2 - private attending' AFTER `id`;
ALTER TABLE `maenna_professional` ADD `hours` FLOAT NOT NULL AFTER `cost`, ADD `hourlyrate` FLOAT NOT NULL AFTER `hours`;
ALTER TABLE `maenna_professional` ADD `template_insight_id` INT NOT NULL AFTER `views`, ADD INDEX (`template_insight_id`);