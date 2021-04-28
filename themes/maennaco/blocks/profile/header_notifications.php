<?php defined('ROOT') || die;

global $user;

use Clewed\Notifications\NotificationService;
use Clewed\User\Service as UserService;

$service = new NotificationService();
$userService = new UserService();

$notifications = array();
$counter = 0;
if (!empty($user->uid)) {
    $notifications = $service->getUserNotifications($user->uid);
    $counter = $service->getNewUserNotificationsCount($user->uid);
}

$templatesEngine = new Twig_Environment(new Twig_Loader_Filesystem(ROOT . '/templates'));

$firstNotification = $notifications[0];
$lastNotification = end($notifications);

echo $templatesEngine->render('/header/notification-list.twig', array(
    'lastId' => $lastNotification['id'] ?: '',
    'firstId' => $firstNotification['id'] ?: '',
    'notifications' => $notifications,
    'counter' => $counter,
    'user' => array(
        'id' => $userId = (int) $user->uid,
        'type' => $userService->getUserType($userId)
    ),
    'time' => $time = time(),
    'hash' => md5('notifications:' . $userId . ':' . $time . ':kyarata75'),
    'notificationTemplate' => escapeHeaderNotificationTemplate(file_get_contents(ROOT . '/templates/header/notification.twig')),
    'request' => $_SERVER["REQUEST_URI"]
));

function escapeHeaderNotificationTemplate($string) {
    return str_replace("\n", '\n', str_replace('"', '\"', addcslashes(str_replace("\r", '', (string) $string), "\0..\37'\\")));
}


