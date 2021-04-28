<?php
error_reporting(E_ALL ^ E_NOTICE);
date_default_timezone_set('EST');

$userService = new Clewed\User\Service();
$companyService = new Clewed\Company\Service();

function nameToId($name)
{
    $q = mysql_query("SELECT uid FROM users WHERE name = '" . $name . "' LIMIT 1") or die(mysql_error());
    $r = mysql_fetch_array($q);
    return $r['uid'];
}

function getUserType($uid)
{
    $q = mysql_query("SELECT pid FROM maenna_people WHERE pid = '" . $uid . "' ");
    if (mysql_num_rows($q) > 0)
        return 'people';
    else {
        $q1 = mysql_query("SELECT companyid FROM maenna_company WHERE companyid = '" . $uid . "' ");
        if (mysql_num_rows($q1) > 0)
            return 'company';
        else
            return 'admin';
    }
}

function ifAdmin($uname)
{
    $result = mysql_query("SELECT *  FROM users u WHERE u.name = '" . $uname . "' AND EXISTS (SELECT * FROM users_roles WHERE uid = u.uid AND rid IN (SELECT rid FROM role WHERE name = 'Super admin' OR name = 'Maennaco admin'))");
    if (mysql_num_rows($result) > 0) {
        return true;
    } else
        return false;
}

function printComments($resultSet) {
while ($row = mysql_fetch_array($resultSet))
{
    global $user;
    $comments = mysql_query("
      SELECT *, UNIX_TIMESTAMP() - date_created AS CommentTimeSpent
	  FROM wall_posts_comments
	  where post_id = " . $row['p_id'] . "
	  order by c_id asc"
    );
?>
<div class="friends_area" name="<?= $row['user']; ?>" alt="<?php echo md5($row['user'] . "kyarata75") ?>"
     id="record-<?php echo $row['p_id'] ?>">
    <?php

    $crId = nameToId($row['user']);
    $uType = getUserType($crId);

    if ($uType == 'people' || $uType == 'admin') {

        //Get user gender
        $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
        $gender_tmp = mysql_fetch_array($q1);
        $gender = $gender_tmp['gender'];

        if (file_exists('sites/default/images/profiles/50x50/' . $crId . '.jpg')) {
            $avatar = 'sites/default/images/profiles/50x50/' . $crId . '.jpg';
        } else {
            if ($gender == 'm' || $gender == '') {
                $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
            } else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
        }
    } else if ($uType == 'company') {

//Get cmp_role		
        $q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $crId") or die(mysql_error());
        $cmp_role_tmp = mysql_fetch_array($q1);
        $cmp_role = $cmp_role_tmp['company_type'];
        //Check if user have a profile picture
        if (file_exists('sites/default/images/company/50x50/' . $crId . '.jpg')) {
            $avatar = 'sites/default/images/company/50x50/' . $crId . '.jpg';
        } else
            if ($cmp_role == 'service') $avatar = ' /themes/maennaco/images/cmp-avatar-service.png';
            else $avatar = ' /themes/maennaco/images/cmp-avatar-product.png';

    }

    echo "<img src=" . $avatar . " style=\"float:left; margin-top:5px; margin-right:5px; width:40px; height:40px;\">";

    ?>

    <label style="width:90%; float:left" class="name">

		   <span style="color:#006274;"><b><?php if (ifAdmin($row['f_name'])) {
                   echo "Clewed";
               } else echo $row['f_name'];
               echo "</b>
		   <span style='color:#666;font-style:italic'>&nbsp;shares a new idea:</span>"; ?></span><?php

        if ($row['user'] == $user->name || ifAdmin($user->name)) { ?>
            <a href="#" style="float:none;" id="remove_id<?php echo $row['p_id'] ?>"
               alt="<?php echo md5($row['p_id'] . $row['user'] . "kyarata75") ?>" name="<?= $row['user']; ?>"
               class="delete tool"> Remove</a>

        <?php } ?>

        <br>
        <em><?php echo nl2br(replace_email($row['post'])); ?></em>
        <br clear="all"/>
		
		   <span>
		   <?php

           /* echo strtotime($row['date_created'],"Y-m-d H:i:s");

           $days = floor($row['date_created'] / (60 * 60 * 24));
           $remainder = $row['date_created'] % (60 * 60 * 24);
           $hours = floor($remainder / (60 * 60));
           $remainder = $remainder % (60 * 60);
           $minutes = floor($remainder / 60);
           $seconds = $remainder % 60;

           if($days > 0)
           echo date('F d Y H:i:s', $row['date_created']);
           elseif($days == 0 && $hours == 0 && $minutes == 0)
           echo "few seconds ago";
           elseif($days == 0 && $hours == 0)
           echo $minutes.' minutes ago';
           else
           echo "few seconds ago";
           */
           echo ago($row['date_created']);

           ?>
		   </span></label>
    <!-- <a href="javascript: void(0)" id="post_id<?php echo $row['p_id'] ?>" class="showCommentBox">Comments</a>
-->


    <br clear="all"/>

    <div id="CommentPosted<?php echo $row['p_id'] ?>">
        <?php
        $comment_num_row = mysql_num_rows(@$comments);
        if ($comment_num_row > 0) {
            while ($rows = mysql_fetch_array($comments)) {
                $days2 = floor($rows['date_created'] / (60 * 60 * 24));
                $remainder = $rows['date_created'] % (60 * 60 * 24);
                $hours = floor($remainder / (60 * 60));
                $remainder = $remainder % (60 * 60);
                $minutes = floor($remainder / 60);
                $seconds = $remainder % 60;
                ?>
                <div class="commentPanel" id="comment-<?php echo $rows['c_id']; ?>" align="left">
                    <?php
                    $crId = nameToId($rows['user']);
                    $uType = getUserType($crId);

                    if ($uType == 'people' || $uType == 'admin') {

                        //Get user gender
                        $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
                        $gender_tmp = mysql_fetch_array($q1);
                        $gender = $gender_tmp['gender'];

                        if (file_exists('sites/default/images/profiles/50x50/' . $crId . '.jpg')) {
                            $avatar = 'sites/default/images/profiles/50x50/' . $crId . '.jpg';
                        } else {
                            if ($gender == 'm' || $gender == '') {
                                $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                            } else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                        }
                    } else if ($uType == 'company') {

//Get cmp_role		
                        $q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $crId") or die(mysql_error());
                        $cmp_role_tmp = mysql_fetch_array($q1);
                        $cmp_role = $cmp_role_tmp['company_type'];
                        //Check if user have a profile picture
                        if (file_exists('sites/default/images/company/50x50/' . $crId . '.jpg')) {
                            $avatar = 'sites/default/images/company/50x50/' . $crId . '.jpg';
                        } else
                            if ($cmp_role == 'service') $avatar = ' /themes/maennaco/images/cmp-avatar-service.png';
                            else $avatar = ' /themes/maennaco/images/cmp-avatar-product.png';

                    }

                    echo "<img src=" . $avatar . " style=\"float:left;continues discussion margin-top:5px;margin-right:5px; width:35px; height:35px;\">";

                    ?>
                    <label style="width:85%;" class="postedComments">
							<span><b><?php if (ifAdmin($rows['user'])) {
                                    echo "Clewed";
                                } else  echo $rows['f_name'];
                                echo "</b><span style='color:#666;font-style:italic;font-size:11px;'>&nbsp;continues discussion:</span>"; ?></span>

                        <?php echo nl2br(replace_email($rows['comments'])); ?>

                        <br>
						<span style="display:inline-block;width:90px;margin-left:0px; color:#666666; font-size:11px">
						<?php
                        /*
                        if($days2 > 0)
                        echo date('F d Y H:i:s', $rows['date_created']);
                        elseif($days2 == 0 && $hours == 0 && $minutes == 0)
                        echo "few seconds ago";
                        elseif($days2 == 0 && $hours == 0)
                        echo $minutes.' minutes ago';
                        else
            echo "few seconds ago";
            */
                        echo ago($rows['date_created']);
                        ?>
						</span>
                        <?php

                        if ($rows['user'] == $user->name || ifAdmin($user->name)){
                        ?>
                        &nbsp;&nbsp;<a href="#" id="CID-<?php echo $rows['c_id']; ?>"
                                       alt="<?php echo md5($rows['c_id'] . $rows['user'] . "kyarata75") ?>"
                                       name="<?= $rows['user']; ?>" class="c_delete tool">Delete</a>
                    </label><?php
                } ?>
                </div>
                <?php
            } ?>
            <?php
        } ?>
    </div>
    <div class="commentBox" align="right"
         id="commentBox-<?php echo $row['p_id']; ?>" <?php echo(($comment_num_row) ? '' : 'style=""') ?>
         name="<?php echo $user->name; ?>" alt="<?php echo md5($user->name . "kyarata75") ?>">
        <label id="record-<?php echo $row['p_id']; ?>">
            <textarea style="width:540px;" class="commentMark" id="commentMark-<?php echo $row['p_id']; ?>"
                      name="commentMark" cols="120"></textarea>
        </label>
        <br clear="all"/>
        <a id="SubmitComment" style="cursor:pointer;" class="tool comment" comBox="commentBox-<?= $row['p_id']; ?>">
            REPLY</a>
    </div>
    <?php
    }
    }


    function checkValues($value)
    {
        $value = trim($value);

        if (get_magic_quotes_gpc()) {
            $value = stripslashes($value);
        }

        $value = strtr($value, array_flip(get_html_translation_table(HTML_ENTITIES)));

        $value = strip_tags($value);
        $value = mysql_real_escape_string($value);
        $value = htmlspecialchars($value);
        return $value;

    }

    function clickable_link($text = '')
    {
        $text = preg_replace('#(script|about|applet|activex|chrome):#is', "\\1:", $text);
        $ret = ' ' . $text;
        $ret = preg_replace("#(^|[\n ])([\w]+?://[\w\#$%&~/.\-;:=,?@\[\]+]*)#is", "\\1<a href=\"\\2\" target=\"_blank\">\\2</a>", $ret);

        $ret = preg_replace("#(^|[\n ])((www|ftp)\.[\w\#$%&~/.\-;:=,?@\[\]+]*)#is", "\\1<a href=\"http://\\2\" target=\"_blank\">\\2</a>", $ret);
        $ret = preg_replace("#(^|[\n ])([a-z0-9&\-_.]+?)@([\w\-]+\.([\w\-\.]+\.)*[\w]+)#i", "\\1<a href=\"mailto:\\2@\\3\">\\2@\\3</a>", $ret);
        $ret = substr($ret, 1);
        return $ret;
    }

    function ago($time)
    {
        $periods = array("second", "minute", "hour", "day", "week", "month", "year", "decade");
        $lengths = array("60", "60", "24", "7", "4.35", "12", "10");

        $now = time();

        $difference = $now - $time;
        $tense = "ago";

        for ($j = 0; $difference >= $lengths[$j] && $j < count($lengths) - 1; $j++) {
            $difference /= $lengths[$j];
        }

        $difference = round($difference);

        if ($difference != 1) {
            $periods[$j] .= "s";
        }

        return "$difference $periods[$j] ago ";
    }

    function prepareUserName ($uid,$companyid) {

        $companyService = new Clewed\Company\Service();

        $utype = getUserType($uid);
        $uname = getProId($uid);

        if ($utype == 'company') return $uname;
        elseif ($utype == 'people') {
            if (in_array($uid,$companyService->getColleagueIds($companyid))) return "Colleague ".$uname;
            else return "Expert ".$uname;
        }
        else return $uname;

    }

    $next_records = 10;
    $show_more_button = 0;

    if (checkValues($_REQUEST['value'])) {

        $editorname = $_REQUEST['u'];
        $d_id = $_REQUEST['eventId'];

        if (md5($_REQUEST['u'] . "kyarata75") === $_REQUEST['m']) {
            mysql_query("INSERT INTO wall_posts (post,f_name,user,date_created,document_id) VALUES('" . checkValues($_REQUEST['value']) . "','" . $editorname . "','" . $editorname . "','" . strtotime(date("Y-m-d H:i:s")) . "','" . $d_id . "')");
        }

        $result = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS TimeSpent FROM wall_posts WHERE document_id = " . $d_id . " order by p_id desc limit 1");

        die(printComments($result));

    } elseif ($_REQUEST['show_more_post']) // more posting paging
    {
        $next_records = $_REQUEST['show_more_post'] + 10;

        $result = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS TimeSpent FROM wall_posts WHERE document_id = " . $d_id . " order by p_id desc limit " . $_REQUEST['show_more_post'] . ", 10");

        $check_res = mysql_query("SELECT * FROM wall_posts WHERE document_id = " . $d_id . " order by p_id desc limit " . $next_records . ", 10");

        $show_more_button = 0; // button in the end

        $check_result = mysql_num_rows(@$check_res);
        if ($check_result > 0) {
            $show_more_button = 1;
        }
    } else {

        if (ifAdmin($user->name))
            $result1 = mysql_query("
                SELECT *
                FROM maenna_company_events mce
                WHERE  companyid = '" . (int) $_REQUEST['id'] . "'
                ORDER BY datetime DESC"
            );
        else
            $result1 = mysql_query("
                SELECT *
				FROM  `maenna_company_events` mce
				WHERE (
				    " . (int) $user->uid . " IN (
                        SELECT uid
                        FROM maenna_company_events_inv
                        WHERE eventid = mce.eventid
                        AND status <> 'declined'
                    )
                    AND 1 = CASE
                                    WHEN ('" . (int)$user->uid . "' IN (" . implode(',', $companyService->getColleagueIds($_REQUEST['id'])) . ") AND mce.approved = 0) THEN 0
                                    ELSE 1
                            END
                    AND companyid = '" . (int) $_REQUEST['id'] . "'
                )
                OR (
                    postedby = '" . (int) $user->uid . "'
                    AND companyid = '" . (int) $_REQUEST['id'] . "'
                )
                OR (
                    executor_id = '" . (int) $user->uid . "'
                    AND (executor_status <> 'declined' OR executor_status IS NULL)
                    AND companyid = '" . (int) $_REQUEST['id'] . "'
                    AND approved = 1
                )
                OR (
                    companyid = '" . (int) $_REQUEST['id'] . "'
                    AND companyid = '" . (int) $user->uid . "'
                    AND approved = 1
                )              
                ORDER BY datetime DESC
            ");

 /*
Turning off permission for colleagues to see approved or owners services with commenting section
       OR (
        '" . (int)$user->uid . "' IN (" . implode(',', $companyService->getColleagueIds($_REQUEST['id'])) . ")
                    AND (companyid = '" . (int)$_REQUEST['id'] . "' and mce.approved = 1)
                    or mce.postedby = '" . (int)$_REQUEST['id'] . "'
                )*/

        while ($row1 = mysql_fetch_array($result1)) {

            $resuid = mysql_query("SELECT name FROM users WHERE uid = '" . $row1['postedby'] . "'");
            $postedUname = mysql_fetch_array($resuid);

//		  $invres = mysql_query("SELECT status FROM maenna_company_events_inv WHERE uid = '".$user->uid."' AND eventid = '".$row1['eventid']."'");
//		  $invstatus = mysql_fetch_array($invres);

            $attres = mysql_query("SELECT uid FROM maenna_company_events_inv WHERE eventid = '" . $row1['eventid'] . "' AND status = 'confirmed' ");
            $invAll = mysql_query("SELECT uid FROM maenna_company_events_inv WHERE eventid = '" . $row1['eventid'] . "' AND status = 'sent'");

/*            $invitedExpertIds = array();
            while (false !== $expert = mysql_fetch_assoc($invAll))
                $invitedExpertIds[] = $expert['uid'];
*/

//		  $files = mysql_query ("SELECT * FROM maenna_company_data WHERE data_value6 = '".$row1['eventid']."' AND data_type='events' AND deleted = 0 ORDER BY dataid DESC");
            $confirmedTeamMemberIds = $companyService->getConfirmedInvitedExpertIds((int) $row1['eventid']);
            $teamMemberIds = $companyService->getInvitedExpertIds((int) $row1['eventid']);
            $isAdmin = $userService->isSuperAdmin($user->uid) || $userService->isAdmin($user->uid, $row1['eventid']);
            $isLead = $row1['executor_id'] == $user->uid;
            $isProjectOwner = $user->uid == $row1['companyid'];
            $isColleague = $userService->isColleague($user->uid, $row1['companyid']);
            $isTeamMember = !$isLead && in_array($user->uid, array_merge($teamMemberIds, $confirmedTeamMemberIds));

            ?>
            <div class="event" id="event<?= $row1['eventid']; ?>">

            <div id="clear" style="clear:both"></div>
            <div style="float:left;" class="calendar">
                    <span style='font-size: 14px;color:#686b70;margin-top:17px;font-family: "Lato Black"'
                          class="day month">
                        <?php if (date('Y', $row1['datetime']) > 1970): ?>
                            <?= (date("M", $row1['datetime'])); ?> <?= date("d", $row1['datetime']); ?>
                        <?php endif; ?>
                    </span>
            </div>

            <div class="event-info">
            <div style="width:535px;margin-left:10px; float:left;">
            <span class="eventNewTitle" style="float:left; cursor:pointer; text-decoration:none;">
                   <a style="color:#686b70;"
                      href="<?= $base_url . "/account?tab=companies&page=company_detail&id=" . $_REQUEST['id'] . "&mtab=advice&view_id=" . $row1['eventid'] ?>"><strong><?= replace_email(strtoupper($row1['title'])); ?></strong></a>
               </span>

            <div class="clear" style="clear:both"></div>
            <a href="<?= $base_url . "/account?tab=companies&page=company_detail&id=" . $_REQUEST['id'] . "&mtab=advice&view_id=" . $row1['eventid'] ?>">
                        <span class="prodatetime" style="float:left;margin:0;">
                            <?php if (!empty($row1['datetime']) && '1970' < date('Y', $row1['datetime'])): ?>
                                <?= ($row1['type'] == 'meeting' ? 'Date: ' : 'Deadline: ') . date("l, M j, Y g:i A T ", $row1['datetime']) . " Created By: " . prepareUserName($row1['postedby'], $_REQUEST['id']); ?>
                            <?php endif; ?>
                        </span>
            </a>
            <div class="clear" style="clear:both"></div>
            <div style="margin-top: 10px;">
            <?php
            $reviewExpert = '';

            $confirmedLabel = '';
            $confirmedExperts = array();
            if ($row1['executor_id'] && 'confirmed' == $row1['executor_status'] || mysql_num_rows($attres) > 0)
                $confirmedLabel .= '<span class="profile_info service-list-item-experts" style="float:left;margin:10px 0 6px 0;">Confirmed: ';

            if ($row1['executor_id'] && 'confirmed' == $row1['executor_status']) {
                $proId = getProId($row1['executor_id']);
                $userType = getUserType($row1['executor_id']);
                if ('company' == $userType) {
                    $proId = getProjectName($row1['executor_id']);
                    $confirmedExperts[] = "<a class=\"tool\" style=\"font-style:italic;\" target=\"_blank\" href=\"/account?tab=companies&page=company_detail&id=" . $row1['executor_id'] . "&mtab=about\"><b>" . $proId . "</b></a> (Lead)";
                } else
                    $confirmedExperts[] = "<a class=\"tool\" onclick=\"showExpertInfo({$row1['executor_id']});\" style=\"font-style:italic;\"><b>" . $proId . "</b></a> (Lead)";
               $reviewExpert[$row1['executor_id']] = $proId." (Lead)";
            }

            if (mysql_num_rows($attres) > 0) {
                while ($att = mysql_fetch_array($attres)) {
                    $proId = getProId($att['uid']);
                    $userType = getUserType($att['uid']);
                    if ('company' == $userType) {
                        $proId = getProjectName($att['uid']);
                        $confirmedExperts[] = "<a class=\"tool\" style=\"font-style:italic;\" target=\"_blank\" href=\"/account?tab=companies&page=company_detail&id=" . $att['uid'] . "&mtab=about\"><b>" . $proId . "</b></a>";
                    } else
                        $confirmedExperts[] = "<a class=\"tool\" onclick=\"showExpertInfo({$att['uid']});\" style=\"font-style:italic;\"><b>" . $proId . "</b></a>";
                    $reviewExpert[$att['uid']] = $proId;
                }
            }

            if ($row1['executor_id'] && 'confirmed' == $row1['executor_status'] || mysql_num_rows($attres) > 0)
                echo $confirmedLabel . implode(', ', $confirmedExperts) . '</span><br/>';

            $invitedLabel = '';
            $invitedExperts = array();

            if ($row1['executor_id'] && empty($row1['executor_status']) || mysql_num_rows($invAll) > 0)
                $invitedLabel .= '<span class="profile_info service-list-item-experts" style="float:left;margin:10px 0 6px 0;">Invited: ';
            if ($row1['executor_id'] && empty($row1['executor_status'])){
                $proId = getProId($row1['executor_id']);
                $userType = getUserType($row1['executor_id']);
                if ($userType == 'company') {
                    $proId = getProjectName($row1['executor_id']);
                    $invitedExperts[] = "<a class=\"tool\" style=\"font-style:italic;\" target=\"_blank\" href=\"/account?tab=companies&page=company_detail&id=" . $row1['executor_id'] . "&mtab=about\"><b>" . $proId . "</b></a> (Lead)";
                } else
                    $invitedExperts[] = "<a class=\"tool\" onclick=\"showExpertInfo({$row1['executor_id']});\" style=\"font-style:italic;\"><b>" . $proId . "</b></a> (Lead)";

                $reviewExpert[$row1['executor_id']] = $proId." (Lead)";
            }

                        if (mysql_num_rows($invAll) > 0) {
                            while ($att = mysql_fetch_array($invAll)) {
                                $proId = getProId($att['uid']);
                                $userType = getUserType($att['uid']);
                                if ('company' == $userType) {
                                    $proId = getProjectName($att['uid']);
                                    $invitedExperts[] = "<a class=\"tool\" style=\"font-style:italic;\" target=\"_blank\" href=\"/account?tab=companies&page=company_detail&id=" . $att['uid'] . "&mtab=about\"><b>" . $proId . "</b></a>";
                                } else
                                    $invitedExperts[] =  "<a class=\"tool\" onclick=\"showExpertInfo({$att['uid']});\" style=\"font-style:italic;\"><b>" . $proId . "</b></a>";
                                $reviewExpert[$att['uid']] = $proId;
                            }
                        }

                        if(!empty($invitedExperts))
                            echo $invitedLabel . implode(', ', $invitedExperts) . '</span><br/>';

            if($row1['approved'] && ($isAdmin || $isColleague || $isProjectOwner || $isTeamMember)) {
                if (!empty($row1['delivery_date'])) {
                    $reviewLabel = array();
                    foreach ($reviewExpert as $uid => $name) {

                        if ($user->uid == $uid) continue;

                        if ($userService->isColleague($uid, $row1['companyid'])) continue;

                        if (ifUserRatedForService('service',$user->uid,$uid,$row1['eventid'])) $reviewLabel[] =  $name." (Review Added)";
                        else {
                            $reviewLabel [] = "<a class=\"tool reviewUser\" data-service=\"$row1[eventid]\" data-editor=\"$user->uid\" data-target=\"$uid\" data-hash='".md5($uid.'kyarata75')."' style=\"font-style:italic;\"><b>Review " . $name . "</b></a>";
                        }

                    }
                    if ($reviewLabel) {
                        $reviewLabel = '<span class="profile_info service-list-item-experts" style="float:left;margin:10px 0 6px 0;">Add Reviews: ' . implode(', ', $reviewLabel);
                        echo $reviewLabel . '</span><br/>';
                    }
                }

            }

                        ?>
                        </div>

                        <div id="clear" style="clear:both"></div>
                        <a  style="color:#686b70;" href="<?= $base_url . "/account?tab=companies&page=company_detail&id=" . $_REQUEST['id'] . "&mtab=advice&view_id=" . $row1['eventid'] ?>">
                            <span class="project-service-list-description" style="float:left;max-height:110px;overflow: hidden"><?= strlen($row1['description']) >= 315 ? substr($row1['description'], 0, 315) . '...' : $row1['description']; ?></span>
                        </a>

                        <div class="clear" style="clear:both"></div>
                    </div>

                    <span class="attFiles" style="float:left;font-size:10px;">
				    <?php
                    if (mysql_num_rows($files) > 0) {
                        echo "FILES:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                        while ($filesres = mysql_fetch_array($files)) {
                            echo "<a target='_blank' class= \"tool\" style=\"color:#00a2bf;\" href='/account?tab=companies&page=company_detail&id=" . $filesres['companyid'] . "&mtab=file&file=" . urlencode('/sites/default/files/' . $filesres[data_value2]) . "&name=" . urlencode($filesres[data_value]) . "'>" . strtoupper($filesres[data_value]) . "</a> &nbsp;&nbsp; ";
                        }
                    }

                    ?>
			   
			   </span>
                </div>
                <div id="clear" style="clear:both"></div>

                <div style="float:left;padding-left: 73px;" class="service-list-controls">
                    <?php

                    echo '<span style="text-transform: capitalize" class="service-list-control">'.(($row1["type"] == "") ? "Service" : $row1["type"]).'</span>';

                    ?>

                    <?php $isNotConfirmedLead = $isLead && 'confirmed' !== $row1['executor_status'] && 'declined' !== $row1['executor_status']; ?>
                    <?php if($isAdmin || $isNotConfirmedLead || in_array($user->uid, $teamMemberIds) && !in_array($user->uid, $confirmedTeamMemberIds)):?>
                        <a href="/account?tab=companies&page=company_detail&id=<?= $_REQUEST['id'] ?>&mtab=advice&view_id=<?= $row1['eventid'] ?>&rsvp=1"
                           class="tool service-list-control">Confirm</a>
                    <?php endif;?>

                    <?php $isConfirmedLead = $isLead && 'confirmed' == $row1['executor_status'];?>
                    <?php if($isConfirmedLead || !in_array($user->uid, $teamMemberIds) && in_array($user->uid, $confirmedTeamMemberIds )):?>
                        <span class="service-list-control">Confirmed</span>
                    <?php endif;?>

                    <?php if ($row1['postedby'] == $user->uid || $AccessObj->Com_sections['advice']['sections']['maenna_events']['access'] == 'write'):?>
                        <?php if(!$row1['approved']):?>
                            <a href="#" id="remove_id<?php echo $row1['eventid'] ?>"
                               alt="<?php echo md5($row1['eventid'] . $user->name . "kyarata75") ?>"
                               name="<?= $user->name; ?>"
                               delType='event'
                               class="tool evdelete service-list-control">Delete</a>
                        <?php endif;?>

                        <a id="edit_id<?php echo $row1['eventid'] ?>"
                           alt="<?php echo md5($row1['eventid'] . $user->name . "kyarata75") ?>"
                           name="<?= $user->name; ?>"
                           delType='event'
                           data-filter-colleagues="<?= (($isProjectOwner || $isColleague) && !empty($row1['start_date'])) ? 1 : 0;?>"
                           class="tool evedit service-list-control">Edit Service</a>

                        <a data-id="<?= $eventId = (int) $row1['eventid'];?>"
                           data-uid="<?= $uid = $user->uid;?>"
                           data-time="<?= $time = time();?>"
                           data-hash="<?= md5($eventId . ':' . $uid . ':' . $time . ':kyarata75') ?>"
                           class="tool service-add-files-btn service-list-control">Add files</a>
                    <?php endif; ?>

                    <?php if($row1['approved'] && empty($row1['start_date']) && $isAdmin):?>
                        <a style="cursor:pointer;"
                           data-time="<?php echo $time = time();?>"
                           data-id="<?php echo $row1['eventid'] ?>"
                           data-hash="<?php echo md5($row1['eventid'] . ':' . $time . ':' . $user->uid . ":kyarata75") ?>"
                           data-uid="<?php echo $user->uid; ?>"
                           class="tool project-service-start-btn service-list-control">Lock Deliverables</a>
                    <?php elseif(!empty($row1['start_date']) && $isAdmin):?>
                        <span class="service-list-control">Deliverables Locked <?= date("n/d/y", strtotime($row1['start_date'])) ?></span>
                    <?php endif;?>

                    <?php if($isAdmin):?>
                        <a style="cursor:pointer;"
                           data-time="<?php echo $time = time();?>"
                           data-id="<?php echo $row1['eventid'] ?>"
                           data-hash="<?php echo md5($row1['eventid'] . ':' . $time . ':' . $user->uid . ":kyarata75") ?>"
                           data-uid="<?php echo $user->uid; ?>"
                           data-approved="<?php echo $row1['approved'] ?>"
                           class="tool project-service-approve-btn service-list-control"><?php echo $row1['approved'] ? 'Disapprove' : 'Approve' ;?></a>
                    <?php else:?>
                        <?php if(!empty($row1['start_date']) && ($isProjectOwner || $isColleague || $isLead || $isTeamMember)):?>
                            <span class="service-list-control">Deliverables Locked <?= date("n/d/y", strtotime($row1['start_date'])) ?></span>
                        <?php else:?>
                            <span class="service-list-control"><?= $row1['approved'] ? 'Approved' : '<span style="color:#00a2bf;font-size:14px!important;">Pending approval</span>'; ?></span>
                        <?php endif;?>
                    <?php endif;?>

                    <?php if(!empty($row1['delivery_date'])):?>
                        <span class="service-list-control">
                            <?php
                            $deliveryTime = strtotime($row1['delivery_date']);
                            $rest = ceil(($deliveryTime + 3 * 24 * 60 * 60 - time()) / 24 / 60 / 60);
                            if($rest > 0)
                                echo "Delivered " . date('n/d/y', $deliveryTime);
                            else
                                echo "Completed " . date('n/d/y', $deliveryTime);?>
                        </span>
                    <?php endif;?>

                    <?php if($row1['approved'] && empty($row1['delivery_date']) && ($isAdmin || $isLead)):?>
                        <a style="cursor:pointer;"
                           data-time="<?php echo $time = time();?>"
                           data-id="<?php echo $row1['eventid'] ?>"
                           data-hash="<?php echo md5($row1['eventid'] . ':' . $time . ':' . $user->uid . ":kyarata75") ?>"
                           data-uid="<?php echo $user->uid; ?>"
                           class="tool project-service-deliver-btn service-list-control">Mark complete</a>
                    <?php endif;?>

<!--                    <?php /*if($row1['approved'] && ($isAdmin || $isColleague || $isProjectOwner || $isTeamMember)):*/?>
                        <?php /*if(!empty($row1['delivery_date']) && !empty($row1['executor_id'])):*/?>
                            <?php /*if (ifUserRatedForService('service', $user->uid, $row1['executor_id'], $row1['eventid'])):*/?>
                                <span class="service-list-control">Review added</span>
                            <?php /*else:*/?>
                                <a class="tool service-list-control"
                                   href="/account?tab=companies&page=company_detail&id=<?/*= $_REQUEST['id'] */?>&mtab=advice&view_id=<?/*= $row1['eventid'] */?>&review=1"
                                   style="text-transform:none;cursor:pointer;">
                                    Add expert review
                                </a>
                            <?php /*endif;*/?>
                        <?php /*endif;*/?>
                    --><?php /*endif;*/?>

                    <!--		  <span style="float:right;color:#231F20; margin-left:1px;" class = "tool">

          <?= "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" . strtoupper(date("M j ", $row1['created'])); ?>
          </span>-->

                </div>
                <div id="clear" style="clear:both"></div>
            </div>

            <?php

        }
    }

    ?>
    <div id="pro_popup" class="hidden"></div>
    <div id="rating" class="hidden"></div>
    <script type="text/javascript">
        $( document ).ajaxComplete(function( event, xhr, settings ) {
            if ( settings.url === "themes/maennaco/blocks/profile/expert_review_form.php" ) {
                $(".rateit").rateit();
            }
        });
        $(function(){

            $('.reviewUser').click(function() {


                var $this = $(this);
                data = $this.data();
                if (!$this.hasClass('reviewUser')) return false;
                $this.addClass('waitingRate');

                $.post('themes/maennaco/blocks/profile/expert_review_form.php', {
                    'type': 'getReviewForm',
                    'targetId': data.target,
                    'editorId': data.editor,
                    'serviceId': data.service,
                    'hash': data.hash,
                    'utype': '<?=$AccessObj->user_type;?>',
                }, function (response) {

                    if (response.data.success == 'false') alert(response.data.error);
                    else $("#rating").html(response.data.html);


                },"json");
            });

            $('.project-service-approve-btn').livequery('click', function(){
                var $this = $(this);
                    data = $this.data();

                if('1' == data.approved && !confirm('Disapproving an already approved service can cause disruption to work and trigger error notifications. Are you sure you want to proceed with this action?'))
                    return false;

                if('0' == data.approved && !confirm('Approving a service will notify all members of the service to take action. Are you sure you want to approve?'))
                    return false;

                $.post('/wrapper.php?controller=company&action=toggle-service-approval', {
                    'id': data.id,
                    'time': data.time,
                    'uid': data.uid,
                    'hash': data.hash
                }, function(r) {

                    if(!r.success && r.error && r.error.message)
                        return alert(r.error.message);

                    if(!r.data[data.id])
                        return alert('Failed to load data');

                    $this.data('approved', r.data[data.id].approved);
                    if('1' == r.data[data.id].approved)
                        $this.html('Disapprove');
                    else
                        $this.html('Approve');
                }, 'json');
            });

            $('.project-service-deliver-btn').attr('onclick', 'markServiceComplete(this);');
            $('.project-service-start-btn').attr('onclick', 'markServiceStarted(this);');
        });

        function markServiceComplete(el) {

            var $btn = $(el);
            if(!confirm('Marking a service complete will notify the client that you have completed delivering all milestones and scope of service. Click OK to confirm, otherwise cancel.'))
                return false;

            $btn.attr('onclick', 'return false;');
            $.post('/wrapper.php?controller=company&action=mark-service-complete', {
                id: $btn.data('id'),
                time: $btn.data('time'),
                uid: $btn.data('uid'),
                hash: $btn.data('hash')
            }, function(r) {

                $btn.attr('onclick', 'markServiceComplete(this);');
                if(!r.success && r.error && r.error.message)
                    return alert(r.error.message);

                var date = $.datepicker.formatDate("m/d/y", new Date());
                if(r.success)
                    $btn.replaceWith('<span class="service-list-control">Delivered ' + date + '</span>');

            },'json');
        }

        function markServiceStarted(el) {

            var $btn = $(el);
            if(!confirm('This tool confirms the parties agreement to the service deliverables.'))
                return false;

            $btn.attr('onclick', 'return false;');
            $.post('/wrapper.php?controller=company&action=mark-service-started', {
                id: $btn.data('id'),
                time: $btn.data('time'),
                uid: $btn.data('uid'),
                hash: $btn.data('hash')
            }, function(r) {

                $btn.attr('onclick', 'markServiceStarted(this);');
                if(!r.success && r.error && r.error.message)
                    return alert(r.error.message);

                var date = $.datepicker.formatDate("m/d/y", new Date());
                if(r.success)
                    $btn.replaceWith('<span class="service-list-control">Deliverables Locked ' + date + '</span>');

            },'json');
        }

        function showExpertInfo(expertId) {
            var uid = "<?= $user->uid;?>";
            $.post("/themes/maennaco/includes/pro_posts.php?type=profileInfo&display=true&pro_id=" + expertId + "&uid=" + uid, function (response) {

                $("#pro_popup").dialog({
                    autoOpen: true,
                    width: 650,
                    title: "Profile",
                    resizable: false,
                    draggable: false,
                    height: 400,
                    closeText: "hide",
                    buttons: {},
                    closeOnEscape: true,
                    modal: true
                }).html(response);

            }, "html");
        }

    </script>