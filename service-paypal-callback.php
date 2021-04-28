<?php require 'lib/init.php';

error_reporting(0);

$data = $_POST;
$validStatus = array('Completed', 'Pending');
if (ENVIRONMENT === PROD)
    $validStatus = array('Completed');

if (valid()) {

    if ($data['receiver_email'] === PAYPAL_ACCOUNT) {

        if (!in_array($data['payment_status'], $validStatus, true)) {
            error_log(sprintf('Payment status was "%s"', $data['payment_status']));
            return true;
        }

        if (hasTransaction($data['txn_id'])) {
            error_log('Duplicate transaction ' . $data['txn_id']);
            return false;
        }

        $custom = unserialize(base64_decode($data['custom']));
        $companyService = new \Clewed\Company\Service();
        return $companyService->processServicePayment(array(
            'txn_id' => $data['txn_id'],
            'user_id' => $custom['user_id'],
            'service_id' => $custom['service_id'],
            'amount' => $data['mc_gross'],
            'datetime' => date('Y-m-d H:i:s'),
            'method' => 'paypal'
        ));

    } else {
        error_log(
            sprintf(
                'Receiver not valid. Expected "%s" got "%s"',
                PAYPAL_ACCOUNT,
                $data['receiver_email']
            )
        );
    }

} else {
    error_log('Did not validate IPN');
    error_log(print_r($data, true));
}

function valid()
{
    header($_SERVER['SERVER_PROTOCOL'] . ' 200 OK');
    $url = PAYPAL_URL . '?cmd=_notify-validate';
    foreach ($_POST as $key => $value)
        $url .= sprintf('&%s=%s', $key, urlencode(stripslashes($value)));

    $c = curl_init($url);
    curl_setopt_array(
        $c,
        array(
            CURLOPT_HEADER => false,
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_FOLLOWLOCATION => true,
            CURLOPT_MAXREDIRS => 5,
            CURLOPT_TIMEOUT => 30,
            CURLOPT_CONNECTTIMEOUT => 10,
            CURLOPT_DNS_CACHE_TIMEOUT => 86400,
            CURLOPT_SSL_VERIFYPEER => false,
        )
    );
    $response = curl_exec($c);
    $httpCode = curl_getinfo($c, CURLINFO_HTTP_CODE);

    if(200 != $httpCode)
        error_log(print_r(array(
            'url' => $url,
            'response' => $response,
            'code' => $httpCode,
            'error' => curl_error($c)
        ), true));

    return $httpCode == 200 && trim($response) === 'VERIFIED';
}

function hasTransaction($txnId)
{
    $companyService = new \Clewed\Company\Service();
    $payment = $companyService->getServicePayment($txnId);

    return !empty($payment);
}