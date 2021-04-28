ALTER TABLE maenna_company_events
ADD executor_id INT NULL ,
ADD executor_status VARCHAR( 32 ) NULL ,
ADD payment_id VARCHAR( 64 ) NULL DEFAULT NULL ,
ADD payment_amount DECIMAL ( 10, 2 ) NULL ,
ADD payment_date DATETIME NULL ,
ADD budget DECIMAL( 10, 2 ) NOT NULL,
ADD clewed_fee DECIMAL( 10, 2 ) NOT NULL DEFAULT 20,
ADD approved TINYINT( 1 ) NOT NULL DEFAULT '0';