<?php
require_once("./includes/bootstrap.inc");
require_once("./includes/common.inc");
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

var_dump($_SESSION);
$_SESSION['add'] = array("DSAddd");
sess_write('sdsa', 'dsadsa');
?>