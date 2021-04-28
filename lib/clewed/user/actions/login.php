<?php

require_once '../../../init.php';
$data = $_POST;
$model = new \Clewed\User\Model(new \Clewed\User\User());
$isCredentialsPresent = $model->validate($data);
if (isset($_SERVER['HTTP_X_REQUESTED_WITH'])) {
    header('content-type: application/json');
    die(json_encode(array('success' => $isCredentialsPresent)));
}

