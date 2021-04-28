<?php

/**
 * Created by PhpStorm.
 * User: vladimir
 * Date: 04.10.15
 * Time: 18:21
 */
namespace Clewed\Notifications;

use Clewed\Db;
use Clewed\Insights\InsightEntity;

class NotificationService
{

    /** @var Db */
    protected $db;

    public function __construct()
    {
        $this->db = Db::get_instance();
    }

    /**
     * @param InsightEntity $insight
     * @param string|array $to
     */
    public function sendInsightDeliveredNotification(InsightEntity $insight, $to)
    {
        $insightOwner = mysql_query(
            'SELECT mp.firstname, CONCAT(mp.firstname," ", mp.lastname) as firstname
             FROM maenna_professional i
             LEFT JOIN maenna_people mp ON mp.pid = i.postedby
             WHERE i.id = ' . $insight->id
        );
        $insightOwner = mysql_fetch_array($insightOwner);
        $insightOwner = trim(ucfirst($insightOwner['firstname']));

        $subject = 'Insight ' . strtoupper($insight->title) . ' is delivered';

        $message = 'Your order from ' . htmlspecialchars($insightOwner) . ' is delivered and waiting for your review. To review your order click ';

        $message .= '<a style="color: #00A2BF !important;" href="https://www.clewed.com/account?tab=professionals&id=' . $insight->postedby . '&page=pro_detail&type=details&pro_id=' . $insight->id . '&review=1">here</a>.';
        $message .= '<br><br>
                Thanks<br>
                The Clewed Team!';

        $to = is_array($to) ? $to : array($to);
        foreach ($to as $email) {
            $this->sendEmailNotification($email, $subject, $message);
        }
    }

    /**
     * @param string $email
     * @param string $subject
     * @param string $message
     */
    private function sendEmailNotification($email, $subject, $message)
    {
        $headers = "From: admin@maennaco.com \r\n";
        $headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";
        mail($email, $subject, $message, $headers);
    }

    /**
     * Register new event in db
     *
     * @param $eventType
     * @param $entityId
     * @param $authorId
     * @param array $options
     */
    public function registerEvent($eventType, $entityId, $authorId, $options = array())
    {
        $this->db->run("
            INSERT INTO notifications (`action`, `state`, `item`, `author`, `created`, `options`)
            VALUES (:action, '1', :item, :author, :created, :options)",
            array(
                ':action' => $eventType,
                ':item' => $entityId,
                ':author' => $authorId,
                ':created' => date('Y-m-d H:i:s'),
                ':options' => empty($options) ? null : serialize($options)
            )
        );
    }

    /**
     * Retrieves user notifications
     *
     * @param $userId
     * @param int $limit
     * @param null $firstId
     * @param null $lastId
     * @return array
     */
    public function getUserNotifications($userId, $limit = 20, $firstId = null, $lastId = null)
    {
        $params = array(
            ':user_id' => $userId
        );

        $limitCondition = '';
        if (false !== $limit)
            $limitCondition = 'LIMIT ' . $limit;

        $firstIdCondition = '';
        if (!empty($firstId)) {
            $firstIdCondition = 'AND id > :first_id';
            $params[':first_id'] = $firstId;
        }

        $lastIdCondition = '';
        if (!empty($lastId)) {
            $lastIdCondition = 'AND id < :last_id';
            $params[':last_id'] = $lastId;
        }

        $notifications = $this->db->get_array("
            SELECT *
            FROM user_notifications
            WHERE user_id = :user_id
            {$firstIdCondition}
            {$lastIdCondition}
            ORDER BY id DESC
            {$limitCondition}",
            $params
        );

        foreach($notifications as $i => $notification)
            $notifications[$i]['elapsed_time'] = $this->buildElapsedTimeString($notification['event_time']);

        return $notifications;
    }

    /**
     * Helper to build elapsed time
     *
     * @param $date
     */
    protected function buildElapsedTimeString($date)
    {
        $currentDate = new \DateTime();
        $pastDate = new \DateTime($date);
        $diff = date_diff($currentDate, $pastDate);

        $months = $diff->format('%m');
        if($months > 12)
            return '> 1y';
        elseif($months > 0)
            return $months . 'M';

        $weeks = floor($diff->format('%d') / 7);
        if($weeks > 0)
            return $weeks . 'w';

        $days = $diff->format('%d');
        if($days > 0)
            return $days . 'd';

        $hours = $diff->format('%h');
        if($hours > 0)
            return $hours . 'h';

        $minutes = $diff->format('%i');
        if($minutes > 0)
            return $minutes . 'm';

        $seconds = $diff->format('%s');
        if($seconds > 0)
            return $seconds . 's';

        return '';
    }

    /**
     * Retrieve a number of new notifications
     *
     * @param $userId
     * @return int
     */
    public function getNewUserNotificationsCount($userId)
    {
        $row = $this->db->get_row("
            SELECT COUNT(*) as count
            FROM user_notifications
            WHERE user_id = :user_id
            AND is_new = 1",
            array(
                ':user_id' => $userId
            ));

        return $row['count'];
    }

    /**
     * Marks notification as read
     *
     * @param $notificationId
     */
    public function markUserNotificationRead($notificationId)
    {
        $this->db->run("
            UPDATE user_notifications
            SET is_new = 0
            WHERE id = :id",
            array(
                ':id' => (int) $notificationId
            ));

        $row = $this->db->get_row("
            SELECT *
            FROM user_notifications
            WHERE id = :id",
            array(
                ':id' => (int) $notificationId
            ));

        $row['elapsed_time'] = $this->buildElapsedTimeString($row['event_time']);

        return $row;
    }
}
