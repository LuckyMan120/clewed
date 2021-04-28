<?php

require_once __DIR__ . '/../../../../lib/init.php';

use Clewed\Notifications\NotificationService;

$userId = (int) $_REQUEST['userId'];
if (empty($userId))
    fail();

$time = (int) $_REQUEST['time'];
if (time() - $time > 60)
    fail();

$hash = $_REQUEST['hash'];
$computedHash = md5('notifications:' . $userId . ':' . $time . ':kyarata75');
if ($hash !== $computedHash)
    fail();

if('new' == $_REQUEST['type']) {

    $service = new NotificationService();

    $firstId = (int) $_REQUEST['firstId'];
    $notifications = $service->getUserNotifications($userId, false, $firstId);
    $counter = $service->getNewUserNotificationsCount($userId);

    die(json_encode(array(
        'success' => true,
        'data' => array(
            'notifications' => $notifications,
            'counter' => $counter,
            'firstId' => !empty($notifications) ? $notifications[0]['id'] : ''
        ),
        'time' => $time = time(),
        'hash' => md5('notifications:' . $userId . ':' . $time . ':kyarata75')
    )));
}
elseif('past' == $_REQUEST['type']) {

    $service = new NotificationService();

    $lastId = (int) $_REQUEST['lastId'];
    $notifications = $service->getUserNotifications($userId, 20, null, $lastId);

    die(json_encode(array(
        'success' => true,
        'data' => array(
            'notifications' => $notifications,
            'lastId' => !empty($notifications) ? $notifications[count($notifications) - 1]['id'] : ''
        )
    )));
}

function fail() {
    die(json_encode(array(
        'success' => false
    )));
}