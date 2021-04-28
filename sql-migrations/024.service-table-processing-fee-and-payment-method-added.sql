ALTER TABLE `maenna_company_events`
ADD `processing_fee` DECIMAL( 10, 2 ) NOT NULL AFTER `clewed_fee` ,
ADD `payment_method` VARCHAR( 32 ) NOT NULL AFTER `processing_fee` ,
ADD `wire_reference` VARCHAR ( 64 ) NOT NULL AFTER `payment_method` ;