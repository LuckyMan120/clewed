<pre>
<?php
/**
 * @author Dmytro Vovk <dmitry.vovk@gmail.com>
 */

require '../lib/init.php';

session_start();

$pp = new Clewed\Paypal;
$result = $pp->buyItem('Insight', '125.40', 'ins-3');

if ($result) {
    var_dump($pp);
    //header('Location: ' . $pp->redirectUrl);
} else {
    var_dump($pp->error->getData());
}
