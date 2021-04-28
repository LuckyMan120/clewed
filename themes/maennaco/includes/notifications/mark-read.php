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

$notificationId = (int) $_REQUEST['id'];
$service = new NotificationService();
$notification = $service->markUserNotificationRead($notificationId);
$counter = $service->getNewUserNotificationsCount($userId);

die(json_encode(array(
    'success' => true,
    'data' => array(
        'notification' => $notification,
        'counter' => $counter
    )
)));

function fail()
{
    die(json_encode(array(
        'success' => false
    )));
}