<?php die('disabled');
///**
// * Created by PhpStorm.
// * User: vladimir
// * Date: 06.10.15
// * Time: 20:33
// *
// * @see clewed-409
// */
//
//use Clewed\Insights\InsightRepository;
//use Clewed\Notifications\NotificationService;
//
//require __DIR__ . '/../lib/init.php';
//require __DIR__ . '/../themes/maennaco/includes/dbcon.php';
//
//
//$notificationService = new NotificationService();
//$insightRepository = new InsightRepository();
//
//$attendedAfterTime = strtotime('-4 days');
//$attendedBeforeTime = strtotime('-3 days');
//
//$attendees = mysql_query(
//    'SELECT mp.id, u.mail
//     FROM maenna_professional_payments mpp
//      JOIN maenna_professional mp ON(mp.id=mpp.pro_id)
//      JOIN users u ON mpp.user_id = u.uid
//     WHERE  date_created > ' . $attendedAfterTime . '
//        AND date_created < ' . $attendedBeforeTime . '
//        AND mp.`datetime` < UNIX_TIMESTAMP()'
//);
//
//$insights = array();
//while ($attendee = mysql_fetch_array($attendees)) {
//
//    $insightId = $attendee['id'];
//    if (!isset($insights[$insightId])) {
//        $insights[$insightId] = $insightRepository->findById($insightId);
//    }
//
//    $notificationService->sendInsightDeliveredNotification($insights[$insightId], $attendee['mail']);
//}
