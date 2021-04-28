ALTER TABLE `maenna_company` 
    ADD `service_fee` DECIMAL(10,2) NULL DEFAULT NULL, 
    ADD `paypal_client_id` VARCHAR(255) NULL DEFAULT NULL, 
    ADD `paypal_client_secret` VARCHAR(255) NULL DEFAULT NULL; 
ALTER TABLE `maenna_professional_investment`
    ADD `service_fee` DECIMAL(10,2) NULL DEFAULT NULL; 
