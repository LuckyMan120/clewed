<?php

include_once 'php-ofc-library/open_flash_chart_object.php';
open_flash_chart_object( 500, 250, 'http://'. $_SERVER['SERVER_NAME'] .'/php-ofc-library/gallery-data-31.php', false );
?>