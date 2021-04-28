ALTER TABLE `maenna_people` ADD COLUMN `investor_type` varchar(255) NULL COMMENT '';
ALTER TABLE `maenna_people` ADD COLUMN `previous_appruved_investor_type` varchar(255) NULL COMMENT '';
ALTER TABLE `maenna_people` ADD COLUMN `investor_type_appruved_flag` boolean NOT NULL DEFAULT '0' COMMENT '';
ALTER TABLE `maenna_people` ADD COLUMN `investor_type_last_appruved_datetime` int(11) NULL DEFAULT NULL COMMENT '';
