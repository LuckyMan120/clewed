<?php require_once dirname(dirname(dirname(dirname(__FILE__)))) . '/lib/init.php';

error_reporting(0);

if ('mark-service-paid' == $_GET['action']) {

    $paymentId = preg_replace('#[^0-9A-Za-z\-]#', '', $_POST['p']);
    $serviceId = $s = (int) $_POST['id'];
    $userId = $u = (int) $_POST['u'];
    $n = $_POST['n'];
    $t = (int) $_POST['t'];
    $m = $_POST['m'];

    $calculatedM = md5("mark-service-paid:$s:$u:$n:$t:kyarata75");
    if ($m !== $calculatedM)
        jsonError('Bad request');

    if (time() - $t > 1 * 60 * 60)
        jsonError('Please, refresh the page and try again');

    $userService = new \Clewed\User\Service();
    $userType = $userService->getUserType($userId);
    if ('admin' !== $userType)
        jsonError('Access denied');

    $companyService = new \Clewed\Company\Service();
    $services = $companyService->getServices(array($serviceId));
    $service = $services[$serviceId];
    if (empty($service))
        jsonError('Invalid data provided');

    if (!empty($service['payment_id']))
        jsonError('Already funded');

    $processed = $companyService->processServicePayment(array(
        'txn_id' => $paymentId,
        'user_id' => $userId,
        'service_id' => $serviceId,
        'amount' => $amount = number_format($service['budget'] * (100 + $service['clewed_fee']) / 100, 2, '.', ''),
        'datetime' => date('Y-m-d H:i:s')
    ));

    if (!$processed)
        jsonError('Failed to process payment data');

    jsonSuccess('Success! Service is marked funded.', array(
        'amount' => number_format($amount, 0)
    ));
}

/**
 * Success
 *
 * @param $msg
 * @param array $data
 */
function jsonSuccess($msg = '', $data = array())
{
    $params = array('success' => true);
    if (!empty($msg))
        $params['msg'] = $msg;

    if (!empty($data))
        $params['data'] = $data;

    die(json_encode($params));
}

/**
 * Failure
 *
 * @param string $msg
 */
function jsonError($msg = '')
{
    $params = array('success' => false);
    if (!empty($msg))
        $params['msg'] = $msg;

    die(json_encode($params));
}

jsonError('Invalid action provided');

