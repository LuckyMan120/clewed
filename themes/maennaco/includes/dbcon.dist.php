<?php
if (!defined('ENVIRONMENT')) {
    require dirname(__FILE__) . '/../../../lib/init.php';
}
if (ENVIRONMENT === DEV) {
    $env = 'dev';
} else {
    $env = 'prod';
}
$credentials = array(
    'prod' => array(
        'host' => 'host',
        'user' => 'user',
        'pass' => 'pass',
        'name' => 'database',
    ),
    'dev'  => array(
        'host' => 'host',
        'user' => 'user',
        'pass' => 'pass',
        'name' => 'database',
    ),
);
$conn = @mysql_connect(
    $credentials[$env]['host'],
    $credentials[$env]['user'],
    $credentials[$env]['pass']
);
if (!$conn) {
    die('Could not connect: ' . mysql_error());
}
mysql_select_db($credentials[$env]['name'], $conn);
mysql_query('SET NAMES utf8 COLLATE utf8_general_ci');
