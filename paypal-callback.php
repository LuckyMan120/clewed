<?php
/**
 * @author Dmitro Vovk <dmitry.vovk@gmail.com>
 */
require 'lib/init.php';

$pp = new \Clewed\PaypalIpnProcessor;
if ($pp->process($_POST)) {
    // Success
} else {
    // Failure
}
