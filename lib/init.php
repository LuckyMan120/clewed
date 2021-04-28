<?php
/**
 * Include this file to get class library accessible
 *
 * @author Dmitry Vovk <dmitry.vovk@gmail.com>
 */
define('PROD', 'prod');
define('DEV', 'dev');
define('HOST', isset($_SERVER['HTTP_HOST']) ? $_SERVER['HTTP_HOST'] : '');
define('PROTO', array_key_exists('HTTPS', $_SERVER) ? 'https://' : 'http://');

$prodEnv = array(
    'newprod.clewed.com',
    'www.newprod.clewed.com',
    'clewed.com',
    'www.clewed.com'
);

define('ENVIRONMENT', in_array(HOST, $prodEnv) ? PROD : DEV);

if (ENVIRONMENT === PROD) {
    error_reporting(0);
    ini_set('display_errors', false);
} else {
    error_reporting(E_ALL);
    ini_set('display_errors', true);
}

if (ENVIRONMENT === PROD) {
    define('PAYPAL_URL', 'https://www.paypal.com/cgi-bin/webscr');
} else {
    define('PAYPAL_URL', 'https://www.sandbox.paypal.com/cgi-bin/webscr');
}

define('PAYPAL_ACCOUNT', 'clewed@clewed.com');


define('ROOT', dirname(__DIR__) . '/');
require ROOT . 'sites/default/settings.php';
require ROOT . 'lib/clewed/class-loader.php';
require ROOT . 'vendor/autoload.php';
ClewedClassLoader::register(ROOT . '/lib');
\Clewed\Db::init($db_url);
