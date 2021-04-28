<?php

use Clewed\Insights\InsightModel;
use Clewed\Insights\InsightRepository;
use Clewed\Notifications\NotificationService;

    error_reporting(0);
    include 'dbcon.php';

    $hashLifeTime = 3 * 3600;

    if(isset($_GET['type']) && 'feature-insight' === $_GET['type']) {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        $insightId = (int) $_GET['id'];
        if(empty($insightId))
            die('error');

        mysql_query('UPDATE maenna_professional SET featured = ABS(featured - 1) WHERE id=' . $insightId);
        $res = mysql_query('
            SELECT COUNT(*) as count
            FROM maenna_professional mp
            INNER JOIN maenna_people p ON p.pid = mp.postedby
            LEFT JOIN (
                SELECT
                    pro_id,
                    COUNT(DISTINCT(user_id)) as oined
                FROM maenna_professional_payments
                GROUP BY pro_id
            ) j ON j.pro_id = mp.id
            WHERE mp.featured = 1
            AND mp.approve_status = 1
            AND (j.oined IS NULL OR j.oined < mp.capacity )'
        );
        $count = mysql_fetch_assoc($res);
        $count = $count['count'];
        die($count);
    }

    if(isset($_GET['type']) && 'insight-audio-preview' === $_GET['type']) {

        require_once __DIR__ . '/../../../lib/init.php';

        $insightId = (int) $_GET['id'];
        if(empty($insightId))
            die('error');

        $m = $_GET['m'];
        $hash = md5($insightId . ':kyarata75');
        if($m !== $hash)
            die('error');

        $fileName = $insightId . '.mp3';
        if(is_readable(ROOT . 'sites/default/files/events_tmp/insights/audio-previews/' . $fileName))
            unlink(ROOT . 'sites/default/files/events_tmp/insights/audio-previews/' . $fileName);

        die('ok');
    }

if(isset($_GET['type']) && 'discussion-audio-preview' === $_GET['type']) {

    require_once __DIR__ . '/../../../lib/init.php';

    $id = (int) $_GET['id'];
    $companyId = $_GET['companyId'];
    $fileName = $_GET['file_name'];

    if(empty($id)|| empty($companyId) || empty($fileName))
        die('error');

    $m = $_GET['m'];
    $hash = md5($id . ':_token');
    if($m !== $hash)
        die('error');


    if(is_readable(ROOT . 'sites/default/files/events_tmp/discussion/audio-previews/' . $fileName)){
        unlink(ROOT . 'sites/default/files/events_tmp/discussion/audio-previews/' . $fileName);

        $db = \Clewed\DB::get_instance();
        $db->run('delete from discussion_audio where name = :fileName and company_id = :companyId ',
            array(
                ':fileName'=> $fileName,
                ':companyId'=> $companyId,
            )
        );
        die('success');
    }

    die('error');

}


    if(isset($_POST['type']) && 'feature-company' === $_POST['type']) {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        $companyIds = $_POST['ids'];
        if(!is_array($companyIds) || empty($companyIds))
            die('Invalid company ids');

        foreach($companyIds as $i => $companyId){
            $sql2 = "SELECT `mission` FROM `maenna_about` WHERE `project_id` = $companyId";
            $result2 = mysql_query($sql2);
            $row2 = mysql_fetch_object($result2);
            if (!$row2 || !strlen(strip_tags($row2->mission))) {
                    echo("This company is not ready to be visible. Please complete the story in the About tab first.");
                    return;
            } else{
                $companyRealIds[$i] = (int) $companyId;
            }
        }        
        require_once __DIR__ . '/../../../lib/init.php';
        $service = new Clewed\Company\Service();

        $companiesActiveState = $service->areCompaniesApproved($companyRealIds);
        if(empty($companiesActiveState))
            die('No companies found');

        $activeCompanies = array();
        foreach($companiesActiveState as $companyId => $active)
            if($active)
                $activeCompanies[] = $companyId;

        if(empty($activeCompanies))
            die('All the companies provided are inactive so can not be marked as featured');

        $service->toggleCompaniesFeaturedState($activeCompanies);

        $msg = "";
        if(count($activeCompanies) == count($companyIds))
            $msg .= "Featured state was toggled for all the provided companies. ";
        else
            $msg .= "Featured state was toggled only for " . count($activeCompanies) . ' companies. ';

        $featuredCount = $service->getFeaturedCount();
        if(1 == $featuredCount)
            $msg .= "1 company is now marked as featured.";
        else
            $msg .= $featuredCount . " companies are now marked as featured";

        die($msg);
    }

    if(isset($_POST['type']) && 'shareable-company' === $_POST['type']) {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        $companyIds = $_POST['ids'];
        if(!is_array($companyIds) || empty($companyIds))
            die('Invalid company ids');

        foreach($companyIds as $i => $companyId)
            $companyIds[$i] = (int) $companyId;

        require_once __DIR__ . '/../../../lib/init.php';
        $service = new Clewed\Company\Service();

        $service->toggleCompaniesShareableState($companyIds);

        die("Shareable state for companies toggled");
    }

    if(isset($_POST['type']) && 'shareable-open' === $_POST['type']){
        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        $companyIds = $_POST['ids'];
        if(!is_array($companyIds) || empty($companyIds))
            die('Invalid company ids');

        foreach($companyIds as $i => $companyId)
            $companyIds[$i] = (int) $companyId;

        require_once __DIR__ . '/../../../lib/init.php';
        $service = new Clewed\Company\Service();

        $service->toggleCompaniesOpenState($companyIds);
        die("Change Status Successed!");
    }

    if(isset($_POST['type']) && 'shareable-coming' === $_POST['type']){
        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        $companyIds = $_POST['ids'];
        if(!is_array($companyIds) || empty($companyIds))
            die('Invalid company ids');

        foreach($companyIds as $i => $companyId)
            $companyIds[$i] = (int) $companyId;

        require_once __DIR__ . '/../../../lib/init.php';
        $service = new Clewed\Company\Service();

        $service->toggleCompaniesComingState($companyIds);

        die("Change Status Successed!");
    }

    if(isset($_POST['type']) && 'shareable-past' === $_POST['type']){
        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        $companyIds = $_POST['ids'];
        if(!is_array($companyIds) || empty($companyIds))
            die('Invalid company ids');

        foreach($companyIds as $i => $companyId)
            $companyIds[$i] = (int) $companyId;

        require_once __DIR__ . '/../../../lib/init.php';
        $service = new Clewed\Company\Service();

        $service->toggleCompaniesPastState($companyIds);

        die("Change Status Successed!");
    }

    if ($_REQUEST['type'] === 'analysis_posts') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        mysql_query("delete from pro_analysis_posts where pid = '" . ((int)$_REQUEST['id']) . "'") or die(mysql_error());
        mysql_query("delete from analysis_wall_post_comments where post_id = '" . ((int)$_REQUEST['id']) . "'") or die(mysql_error());
        die('success');
    }
    if ($_REQUEST['type'] === 'professional_images') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        $sql   = "delete from wall_documents where ref_id = '" . mysql_real_escape_string($_POST['event_id']) . "' AND d_id = '" . mysql_real_escape_string($_REQUEST['image_id']) . "'";
        $query = mysql_query($sql);
        if ($query) {
            echo 1;
        }
    }
    if ($_REQUEST['type'] === 'professional_links') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        $sql   = "delete from maenna_professional_links where professional_id = '" . mysql_real_escape_string($_POST['event_id']) . "' AND id = '" . mysql_real_escape_string($_REQUEST['link_id']) . "'";
        $query = mysql_query($sql);
        if ($query) {
            echo 1;
        }
    }
    if ($_REQUEST['type'] === 'followdis') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        if ($_REQUEST['status'] == 1) {
            mysql_query("INSERT INTO maennaco_discussion_follow (prof_id, user_id) VALUES('" . ((int)$_REQUEST['prof_id']) . "','" . ((int)$_REQUEST['user_id']) . "')");
        } else {
            mysql_query("DELETE FROM maennaco_discussion_follow where prof_id = '" . ((int)$_REQUEST['prof_id']) . "' and user_id = '" . ((int)$_REQUEST['user_id']) . "' ");
        }
    }
    if ($_REQUEST['type'] === 'approvedis') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        if((int) $_REQUEST['status']) {

            require_once __DIR__ . '/../../../lib/init.php';

            $offerId = (int) $_REQUEST['prof_id'];

            $db = \Clewed\DB::get_instance();
            $offer = $db->get_array("
                SELECT
                    type,
                    approve_status
                FROM maenna_professional
                WHERE id = :id
                LIMIT 1",
                array(':id' => $offerId)
            );

            $isApproved = !!$offer[0]['approve_status'];
            $type = (int) $offer[0]['type'];

            if(2 !== $type && !$isApproved) {
                $notificationService = new NotificationService();
                $notificationService->registerEvent('offer_approved', (int) $_REQUEST['prof_id'], 0);
            }
        }

        mysql_query("UPDATE maenna_professional SET approve_status = '" . mysql_real_escape_string($_REQUEST['status']) . "' WHERE id='" . ((int)$_REQUEST['prof_id']) . "'");
        if(0 === (int) $_REQUEST['status'])
            mysql_query('UPDATE maenna_professional SET featured = 0 WHERE id=' . ((int) $_REQUEST['prof_id']));

    }
    if ($_REQUEST['type'] === 'like_discussion') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        if ($_REQUEST['status'] == 1) {
            mysql_query("INSERT INTO like_discussion_professional (prof_id,user_id) VALUES('" . ((int)$_REQUEST['prof_id']) . "','" . ((int)$_REQUEST['userid']) . "')");
            $sql_likes        = mysql_query("SELECT * FROM  `like_discussion_professional` WHERE `prof_id` = '" . ((int)$_REQUEST['prof_id']) . "' ORDER BY id DESC");
            $sql_likes_result = mysql_num_rows($sql_likes);
            echo $sql_likes_result;
            return $sql_likes_result;
        } else {
            mysql_query("delete from like_discussion_professional where prof_id = '" . ((int)$_REQUEST['prof_id']) . "' and user_id = '" . ((int)$_REQUEST['userid']) . "' ");
            $sql_likes        = mysql_query("SELECT * FROM  `like_discussion_professional` WHERE `prof_id` = '" . ((int)$_REQUEST['prof_id']) . "' ORDER BY id DESC");
            $sql_likes_result = mysql_num_rows($sql_likes);
            echo $sql_likes_result;
            return $sql_likes_result;
        }
    }
    if (isset($_REQUEST['id']) && isset($_REQUEST['u']) && isset($_REQUEST['m']) && md5($_REQUEST['id'] . $_REQUEST['u'] . "kyarata75") === $_REQUEST['m']) {
        if ($_REQUEST['type'] == 'event') {
            $event = mysql_query("SELECT * FROM maenna_company_events WHERE eventid = '" . ((int)$_REQUEST['id']) . "'");
            $event = mysql_fetch_array($event);
            $inv   = mysql_query("SELECT mail FROM maenna_company_events_inv mcei JOIN users u ON mcei.uid = u.uid WHERE eventid = '" . ((int)$_REQUEST['id']) . "' AND mcei.status = 'confirmed'");
            mysql_query("delete from maenna_company_events where eventid = '" . ((int)$_REQUEST['id']) . "'");
            mysql_query("delete from maenna_company_events_inv where eventid = '" . ((int)$_REQUEST['id']) . "'");
            mysql_query("delete from wall_posts where p_id ='" . ((int)$_REQUEST['id']) . "' AND user ='" . $_REQUEST['u'] . "'");
            mysql_query("delete from wall_posts_comments where post_id ='" . ((int)$_REQUEST['id']) . "'");
            mysql_query("delete from maenna_company_data where data_type = 'events' AND data_value6 ='" . ((int)$_REQUEST['id']) . "'");
            while ($invm = mysql_fetch_array($inv)) {
                $to      = $invm['mail'];
                $subject = '[Event canceled] ' . strtoupper($event['title']);
                $message .= 'Event: ' . strtoupper($event['title']) . '<br><br>';
                $message .= 'Unfortunately, this event has been canceled!!!<br><br>';
                $message .= 'Best regards<br>';
                $message .= 'CLEWED TEAM';
                $headers = "From: admin@maennaco.com \r\n";
                $headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";
                mail($to, $subject, $message, $headers) or die('neuspeh');
            }
        } else {
            mysql_query("delete from wall_posts where p_id ='" . ((int)$_REQUEST['id']) . "' AND user ='" . $_REQUEST['u'] . "'");
            mysql_query("delete from wall_posts_comments where post_id ='" . ((int)$_REQUEST['id']) . "'");
        }
    }
    if ($_REQUEST['type'] == 'professional') {

        $id = (int)$_REQUEST['id'];
        $u = $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5($id . $u . 'kyarata75'))
            die('error');

        mysql_query("delete from maenna_professional where id = '{$id}'");
    }
    if ($_REQUEST['type'] == 'professional_post') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        mysql_query("delete from pro_wall_posts where pid = '" . ((int)$_REQUEST['id']) . "'");
        mysql_query("delete from pro_wall_posts_comments where post_id = '" . ((int)$_REQUEST['id']) . "'");
    }
    if ($_REQUEST['type'] == 'professional_comments') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        mysql_query("delete from pro_wall_posts_comments where cid = '" . ((int)$_REQUEST['id']) . "'");
    }
    if ($_REQUEST['type'] == 'minutes_comments') {

        $id = (int)$_REQUEST['id'];
        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $id . ':' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        mysql_query("delete from wall_posts_comments where c_id = '{$id}'");
    }
    if ($_REQUEST['type'] == 'project-service-comment') {

        $id = (int)$_REQUEST['id'];
        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $id . ':' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        mysql_query("delete from pro_wall_posts_comments where cid = '{$id}'");
    }
    if ($_REQUEST['type'] == 'analysis_comments') {

        $id = (int)$_REQUEST['id'];
        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $id . ':' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        mysql_query("delete from analysis_wall_post_comments where cid = '{$id}'");
    }
    if ($_REQUEST['type'] == 'like_posts') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        if ($_REQUEST['status'] == 1) {
            mysql_query("INSERT INTO like_discussion_posts (prof_id,post_id,user_id) VALUES('" . ((int)$_REQUEST['prof_id']) . "','" . ((int)$_REQUEST['post_id']) . "','" . ((int)$_REQUEST['userid']) . "')");
        } else {
            mysql_query("delete from like_discussion_posts where prof_id = '" . ((int)$_REQUEST['prof_id']) . "' and post_id = '" . ((int)$_REQUEST['post_id']) . "' and user_id = '" . ((int)$_REQUEST['userid']) . "' ");
        }
    }
    if ($_REQUEST['type'] == 'analysis_like_posts') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        if ($_REQUEST['status'] == 1) {
            mysql_query("INSERT INTO like_analysis_posts (prof_id, post_id,user_id) VALUES('" . ((int)$_REQUEST['prof_id']) . "', '" . ((int)$_REQUEST['post_id']) . "','" . ((int)$_REQUEST['userid']) . "')");
        } else {
            mysql_query("delete from like_analysis_posts where post_id = '" . ((int)$_REQUEST['post_id']) . "' and prof_id = '" . ((int)$_REQUEST['prof_id']) . "' and user_id = '" . ((int)$_REQUEST['userid']) . "' ");
        }
    }
    if ($_REQUEST['type'] == 'like_analysis') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        if ($_REQUEST['status'] == 1) {
            mysql_query("INSERT INTO like_analysis (prof_id,user_id) VALUES('" . ((int)$_REQUEST['prof_id']) . "','" . ((int)$_REQUEST['userid']) . "')");
        } else {
            mysql_query("delete from like_analysis where prof_id = '" . ((int)$_REQUEST['prof_id']) . "' and user_id = '" . ((int)$_REQUEST['userid']) . "' ");
        }
        $count_result3   = mysql_query("SELECT * FROM  `like_analysis` WHERE prof_id = '" . ((int)$_REQUEST['prof_id']) . "'");
        $count_row3      = mysql_fetch_array($count_result3);
        $count_likepost1 = mysql_num_rows($count_result3);
        echo $count_likepost1;
        return $count_likepost1;
    }
    if ($_REQUEST['type'] == 'like_company') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        if ($_REQUEST['status'] == 1) {
            mysql_query("INSERT INTO like_company (user_id,project_id) VALUES('" . ((int)$_REQUEST['user_id']) . "','" . ((int)$_REQUEST['project_id']) . "')");
        } else {
            mysql_query("delete from like_company where project_id = '" . ((int)$_REQUEST['project_id']) . "' and user_id = '" . ((int)$_REQUEST['user_id']) . "' ");
        }
    }
    if ($_REQUEST['type'] == 'follow') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        if ($_REQUEST['status'] == 1) {
            mysql_query("INSERT INTO maenna_followers (cid,uid) VALUES('" . ((int)$_REQUEST['user_id']) . "','" . ((int)$_REQUEST['id']) . "')");
        } else {
            mysql_query("delete from maenna_followers where cid = '" . ((int)$_REQUEST['user_id']) . "' and uid = '" . ((int)$_REQUEST['id']) . "' ");
        }
    }
    if ($_REQUEST['type'] == 'join') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        //@todo: add payment checking
        $insightRepository = new \Clewed\Insights\InsightRepository();
        $insightService = new \Clewed\Insights\InsightService();

        $insight = $insightRepository->findById($_REQUEST['prof_id']);
        if ($insight) {
            $insightService->attend((int)$_REQUEST['uid'], $insight, $insertFakePayment = true);
        }
    }
    if ($_REQUEST['type'] == 'like_post_comments') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        if ($_REQUEST['status'] == 1) {
            mysql_query("INSERT INTO like_discussion_posts_comments (user_id, comment_id) VALUES('" . ((int)$_REQUEST['user_id']) . "','" . ((int)$_REQUEST['comment_id']) . "')");
        } else {
            mysql_query("DELETE FROM like_discussion_posts_comments where user_id = '" . ((int)$_REQUEST['user_id']) . "' and comment_id = '" . ((int)$_REQUEST['comment_id']) . "' ");
        }
    }
    if ($_REQUEST['type'] == 'analysis_like_post_comments') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        if ($_REQUEST['status'] == 1) {
            mysql_query("INSERT INTO analysis_comments (user_id,post_id, comment_id) VALUES('" . ((int)$_REQUEST['user_id']) . "','" . ((int)$_REQUEST['post_id']) . "','" . ((int)$_REQUEST['comment_id']) . "')");
        } else {
            mysql_query("DELETE FROM analysis_comments where user_id = '" . ((int)$_REQUEST['user_id']) . "' and comment_id = '" . ((int)$_REQUEST['comment_id']) . "' and post_id = '" . ((int)$_REQUEST['post_id']) . "' ");
        }
        return true;
    }
    if ($_REQUEST['type'] == 'discussion_status') {

        $u = (int) $_REQUEST['u'];
        $m = $_REQUEST['m'];
        if(empty($u) || empty($m) || $m !== md5('delete.php:' . $u . ':kyarata75') || (time() - $u) > $hashLifeTime)
            die('error');

        if (isset($_REQUEST['status']) && $_REQUEST['status'] == 1) {
            $status = 0;
        } else {
            $status = 1;
        }
        mysql_query("UPDATE maenna_people SET discussion_status = '" . $status . "' WHERE pid='" . ((int)$_REQUEST['id']) . "'");
        echo 1;
    }

    if (isset($_REQUEST['id'], $_REQUEST['type'], $_REQUEST['m']) && $_REQUEST['type'] === 'deliver-insight' && md5($_REQUEST['id'] . "kyarata75") === $_REQUEST['m']) {

        $insightId = (int)$_REQUEST['id'];

        $notificationService = new NotificationService();
        $repository = new InsightRepository;
        $model = new InsightModel;

        $insight = $repository->findById($insightId);
        $insight->delivered = 1;
        $model->save($insight);

        $notificationService->registerEvent('service_delivered', $insightId, 0);

        exit('success');
    }
