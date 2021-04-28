<?php
/**
 * @author Dmytro Vovk <dmitry.vovk@gmail.com>
 */

if ($_REQUEST['type'] == 'calculateRate') {
    // Prepare variables
    $target = (int) $_REQUEST['target_uid'];

// Get DB connection
    include 'dbcon.php';
    $db = \Clewed\Db::get_instance();
    // Process Rate
    $Row = $db->get_row(
        'select
                  count(*) total,
                  sum(case when rate_type = 1 then 1 else 0 end) positive,
                  sum(case when rate_type = 0 then 1 else 0 end) negative
                from user_rating
                where target_uid = :target ',
        array('target' => $target)
    );
    $perc = ($Row['positive']*100)/$Row['total'];
    die (json_encode(array('total'=>$Row['total'],'positive'=>$Row['positive'],'negative'=>$Row['negative'], 'rate' => (int) $perc)));
}

if ($_REQUEST['type'] == 'update_comment_rate_user') {

    // Prepare variables
    $rid = $_REQUEST['rid'];
    $m = $_REQUEST['m'];
    $text = $_REQUEST['text'];

    //Check authentication

    if (md5($rid."kyarata75") == $m) {
// Get DB connection
    include 'dbcon.php';
    $db = \Clewed\Db::get_instance();
    // Process Rate
    $success = $db->run(
        'UPDATE `user_rating` SET comment = :text WHERE id = :rid',
        array(':text' => $text,'rid' => $rid)
    );

        if ($success) die('success');

    }
    else {
        die('false');
    }
}

if ($_REQUEST['type'] == 'rate_user') {
// Prepare variables
    $rate = $_REQUEST['rate_str'];
    $rate_overall = $_REQUEST['rate_overall'];
    $target = (int) $_REQUEST['target_uid'];
    $targetInsightId = isset($_REQUEST['target_insight_id']) ? (int)$_REQUEST['target_insight_id'] : null;
    $targetProjectServiceId = (int) $_REQUEST['target_project_service_id'];
    $editor = (int) $_REQUEST['editor_uid'];
    $comment = $_REQUEST['comment'];
    $if_admin = $_REQUEST['if_admin'];
    $time = time();
// Get DB connection
    include 'dbcon.php';
    $db = \Clewed\Db::get_instance();
    // Process Rate
    $success = $db->run(
        'INSERT INTO `user_rating` (`rate_individual_string`,`rate_overall`, `target_uid`, `editor_uid`, `admin`,`comment`, `created`, `insight_id`, `service_id`) VALUES (:rate,:rate_overall, :target, :editor,:if_admin, :comment, :created, :insight_id, :service_id)
',
        array('rate' => $rate,'rate_overall' => $rate_overall, 'target' => $target, 'editor' => $editor, 'if_admin' => $if_admin,'comment' => $comment, 'created' => $time, 'insight_id' => $targetInsightId, 'service_id' => $targetProjectServiceId)
    );

    if($targetProjectServiceId && $success) {
        $notificationService = new \Clewed\Notifications\NotificationService();
        $notificationService->registerEvent(
            'project_service_expert_review_added',
            $targetProjectServiceId,
            $editor,
            array('expertId' => $target)
        );
    }

    die("success");
}

// Check input params validity
if (empty($_REQUEST['a'])) {
    die;
}
if (!in_array($_REQUEST['a'], array('Like', 'Unlike'))) {
    die;
}
if (!is_numeric($_REQUEST['p'])) {
    die;
}
if (!is_numeric($_REQUEST['u'])) {
    die;
}
// Prepare variables
$action = $_REQUEST['a'];
$pid = (int) $_REQUEST['p'];
$uid = (int) $_REQUEST['u'];
// Get DB connection
include 'dbcon.php';
$db = \Clewed\Db::get_instance();
if ($action === 'Like') {
    // Process Like
    $db->run(
        'INSERT INTO `like_discussion_professional` (`prof_id`, `user_id`) VALUES (:prof_id, :user_id)',
        array(':prof_id' => $pid, ':user_id' => $uid)
    );
} elseif ($action === 'Unlike') {
    // Process Unlike
    $db->run(
        'DELETE FROM `like_discussion_professional` WHERE `prof_id` = :prof_id AND `user_id` = :user_id',
        array(':prof_id' => $pid, ':user_id' => $uid)
    );
}
// Return likes count
$likesCount = (int) $db->get_column(
    'SELECT COUNT(*) FROM `like_discussion_professional` WHERE `prof_id` = :prof_id',
    array(':prof_id' => $pid)
);
echo $likesCount;
