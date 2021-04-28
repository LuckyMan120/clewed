<?php

include_once __DIR__ . '/dbcon.php';
include_once __DIR__ . '/safe_functions.inc';
include_once __DIR__ . '/../blocks/insights/comment.php';
include_once __DIR__ . '/../blocks/insights/post.php';

error_reporting (E_ALL ^ E_NOTICE);
date_default_timezone_set('EST');
global $base_url;

function n2br($text) {
    return preg_replace("|\n|", "<br>", $text);
}

function getNewInsightPostHtml($post_id, $ownerUid, $ownerName, $requesterUid, $content, $flag, $dissid, $tags, $pro_profile, $created_timestamp) {

    $canRequesterEdit = isDiscussionAdmin($dissid, $requesterUid) || $ownerUid == $requesterUid;
    $isPrivate = $flag == 'pm' || $flag == 'pq';

    $post = array(
        'id'            => $post_id,
        'discussion_id' => $dissid,
        'insight_id'    => (int)$pro_profile,
        'flag'          => $flag,
        'is_private'    => $isPrivate,
        'author_id'     => $ownerUid,
        'author_name'   => $ownerName,
        'author_avatar' => getAvatarUrl($ownerUid),
        'post'          => $content,
        'tags'          => $tags,
        'is_liked'      => false,
        'likes_cnt'     => 0,
        'created_at_timestamp' => $created_timestamp,
    );

    ob_start(); //todo: find better solution
    displayPost($post, array(), $requesterUid, $isPrivate, $canRequesterEdit);
    return ob_get_clean();
}

function getNewInsightCommentHtml($commentId, $ownerUid, $requesterUid, $postId, $flag, $discussionId, $comment, $created_timestamp)
{
    $canRequesterEdit = isDiscussionAdmin($discussionId, $requesterUid) || $ownerUid == $requesterUid;
    $isPrivate = $flag == 'pm' || $flag == 'pq';

    $comment = array(
        'id'            => $commentId,
        'post_id'       => $postId,
        'author_id'     => $ownerUid,
        'author_avatar' => getAvatarUrl($ownerUid),
        'author_name'   => getUserById($ownerUid),
        'is_private'    => $isPrivate,
        'is_liked'      => false,
        'comment'       => $comment,
        'created_at_timestamp' => $created_timestamp,
    );

    ob_start(); //todo: find better solution
    displayComment($comment, $requesterUid, $canRequesterEdit);

    return ob_get_clean();
}

function isDiscussionAdmin($discussionId,  $checkingUid)
{
    $checkingUserType = getUserTypeById($checkingUid);
    $ifAdmin = $checkingUserType == 'super' || ifAdmin($checkingUid) || ifDiscussionModerator($discussionId, $checkingUid);
    return $ifAdmin;
}

$_REQUEST['text'] = preg_replace('#<\s*a[^>]+href\s*=\s*\"?\'?([^\s\'\">]+)\'?\"?[^>]*>[^<]*<\/a>#uims', '$1', $_REQUEST['text']);
$_REQUEST['text'] = preg_replace('#<\s*img[^>]>#uims', '', $_REQUEST['text']);
$_REQUEST['text'] = preg_replace('#<\s*iframe.*?\/iframe\s*>#uims', '', $_REQUEST['text']);
$_REQUEST['text'] = preg_replace('#(https?\:\/\/[^\s]+\.[^\s\.<]+)#uims', '<a class="comment-inline" href="$1" target="_blank">$1</a>', $_REQUEST['text']);

if ($_REQUEST['type'] == 'analysis_comment') {

    if ($_REQUEST['m'] == md5($_REQUEST['post_id']."kyarata75")) {

        $uid = (int) $_REQUEST['editor'];
        $uname = getUserById($uid);

        $now = time();

        mysql_query("INSERT INTO analysis_wall_post_comments (d_id,post_id,user_id,comment,username,datecreated) VALUES('".mysql_real_escape_string($_REQUEST['dis_id'])."', '".mysql_real_escape_string($_REQUEST['post_id'])."','".$uid."','".checkValues($_REQUEST['text'])."','".$uname."','".$now."')");

        $comm_id = mysql_insert_id();

        $avatar = getAvatarUrl($uid);

        $html = '
            <div class="aucomnts" id="aucomnts' .$comm_id . '">
                <div class="aucpic">
                    <img src="'.$avatar.'" style="float:left; margin-top:13px; margin-right:5px; width:45px; height:45px;">&nbsp;
                </div>
                <div class="aucdisc" style="width: 91% !important;">
                    <h5 style="text-transform:uppercase;float: none !important; margin-top: 0;color:#2db6de!important">'.$uname.'</h5>
                    <p style="padding: 0 !important; width: 95%;line-height: 14px;" id="com'.$comm_id.'">'.$_REQUEST['text'].'</p>
                    <div class="comment_anchor" style="margin-top:-4px;">
                      <div id="likepostcomment'.$comm_id.'" style="float:left;margin:0px;padding:0px 0px 0px 0px;">
                        <a href="javascript:void(0);" style="cursor:pointer;" onClick="like_post_comments(\'like\',  \''.$comm_id.'\', \''.$uid.'\');">Like</a>
                      </div>
                        <div style="float:left">
                            &nbsp;|&nbsp;<a style="cursor:pointer;" href="javascript:void(0);" id="delete_comment'.$comm_id.'" class="delete_comment">Delete</a>
                        </div>
                    </div>
                </div>
                <div style="clear:both">
            </div>';

        $return = array("status" => "success", "display" => $html);
        die(json_encode($return));
    }
    else (die("Authentication problem"));

}

if ($_REQUEST['type'] == 'project_discussion_comment_reply') {

    if ($_REQUEST['m'] == md5($_REQUEST['post_id']."kyarata75")) {

        $uid = (int) $_REQUEST['editor'];
        $uname = getUserById($uid);
        $now = time();
        $message = strip_tags($_REQUEST['text']);

        $db = \Clewed\Db::get_instance();

        $db->run("
            INSERT INTO analysis_wall_post_comments (d_id,post_id,user_id,comment,username,datecreated)
            VALUES(?, ?, ?, ?, ?, ?)",
            array(
                (int) $_REQUEST['dis_id'],
                (int) $_REQUEST['post_id'],
                $uid,
                $message,
                $uname,
                $now
            )
        );

        $comm_id = $db->lastInsertId();
        $postId = (int) $_REQUEST['post_id'];

        $post = $db->get_row("
            SELECT *
            FROM pro_analysis_posts
            WHERE pid = ?
            LIMIT 1",
            array($postId)
        );

        if(empty($post))
            die('Invalid request data');

        $discussionId = (int) $post['pro_id'];
        $options = array(
            'projectId' => (int) $_REQUEST['pro_profile'],
            'postAuthorId' => (int) $post['user'],
            'message' => $message
        );

        $notificationService = new \Clewed\Notifications\NotificationService();
        $notificationService->registerEvent(
            'project_discussion_comment_replied',
            $discussionId,
            $uid,
            $options
        );

        $userService = new \Clewed\User\Service();
        $users = $userService->get(array($uid));
        $user = $users[$uid];

        $avatar = $user['avatar'];

        $html = '
            <div class="aucomnts project-discussion-comment"
                style="margin-left: 49px!important;"
                id="aucomnts' .$comm_id . '"
                u="' . $uid . '"
                t="' . ($time = time()) . '"
                m="' . md5($uid . ':' . $comm_id . ':' . $time . ":kyarata75") . '"
                data-comment-id="' .$comm_id . '">
                <div class="aucpic">
                    <img src="'.$avatar.'" style="float:left; margin-top:13px; margin-right:5px; width:45px; height:45px;">&nbsp;
                </div>
                <div class="aucdisc" style="width: 91% !important;">
                    <h5 style="text-transform:uppercase;float: none !important; margin-top: 0;color:#2db6de!important">'.$uname.'</h5>
                    <p class="comment_text" style="padding: 0 !important; width: 95%;line-height: 14px;" id="com'.$comm_id.'">'. n2br(htmlspecialchars($_REQUEST['text'])).'</p>
                    <div class="w comment_editor hidden" style="margin-left:0!important;">
                         <textarea
                             style="line-height:15px!important;width:92%; margin:0!important; height:100px; "
                             class="input mceNoEditor">' . htmlspecialchars($_REQUEST['text']) . '</textarea>
                     </div>
                     <div class="comment_editor_controls hidden" style="float:right;margin: 2px 14px 10px;">
                         <a class="save-post" style="cursor: pointer;padding-right: 0;" onclick="submitEditor('.$postId.',' . $comm_id. ');">Submit</a>&nbsp;|&nbsp;
                         <a style="cursor: pointer" onclick="hideEditor('.$postId.',' . $comm_id. ');">Cancel</a>
                     </div>
                    <div class="comment_anchor" style="margin-top:-4px;margin-bottom: 10px;">
                                    <div style="float:left;padding:0;font-size:12px;">
                                        '.date("D, M j, Y g:i A T ", time()).'
                                        &nbsp;|&nbsp;
                                    </div>
                        <a onclick="showCommentForm(' . $postId . ');" 
                           data-tooltip="Discussing pricing or sharing personal contact creates various business challenges and is prohibited">Reply</a>&nbsp;|&nbsp;
                        <a style="cursor:pointer;"
                           onclick="showEditor('.$postId.',' . $comm_id. ');">Edit</a>&nbsp;|&nbsp;
                        <a style="cursor:pointer;"
                           data-id="' . $comm_id. '"
                           u="' . ($u = time()) .'"
                           m="' . md5('delete.php:' . $comm_id . ':' . $u . ':kyarata75') . '"
                           class="delete_comment">Delete</a>
                    </div>
                </div>
                <div style="clear:both">
            </div>';

        $return = array("status" => "success", "display" => $html);
        die(json_encode($return));
    }
    else (die("Authentication problem"));

}

if ($_REQUEST['type'] == 'pro_file_comment') {

    if ($_REQUEST['m'] == md5($_REQUEST['uid'].$_REQUEST['pid']."kyarata75")) {

        $text = strip_tags($_REQUEST['value']);
        if (isset($_REQUEST['bEdit']) && $_REQUEST['bEdit'] == 'true') {

            mysql_query("UPDATE pro_wall_posts_comments SET comment = '".mysql_real_escape_string($text)."',user_id = ".mysql_real_escape_string($_REQUEST['uid']).", datecreated = '".time()."' WHERE cid = ".mysql_real_escape_string($_REQUEST['cid'])) or die(mysql_error());

            exit(1);
        }

        $editor = (int) $_REQUEST['uid'];
        $editorname = getUserById($editor);

        mysql_query("INSERT INTO pro_wall_posts_comments (post_id,comment,username,user_id,datecreated) VALUES('".mysql_real_escape_string($_REQUEST['pid'])."','".mysql_real_escape_string($text)."','".$editorname."','".$editor."','".time()."')")  or die(mysql_error());

        $db = \Clewed\Db::get_instance();
        $file = $db->get_row("
            SELECT *
            FROM pro_wall_posts p
            INNER JOIN maenna_company_data f ON f.dataid = p.pro_id
            WHERE p.pid = ?",
            array((int) $_REQUEST['pid'])
        );

        if(!empty($file) && 'service-file' == $file['data_type']) {
            $notificationService = new \Clewed\Notifications\NotificationService();
            $notificationService->registerEvent(
                'company_service_file_commented',
                (int) $file['data_value6'],
                (int) $editor,
                array(
                    'id' => $file['pro_id'],
                    'fileName' => $file['data_value2'],
                    'authorId' => $file['editorid'],
                    'message' => $text
                )
            );
        }

        $result = mysql_query("SELECT *,
    UNIX_TIMESTAMP() - datecreated AS CommentTimeSpent FROM pro_wall_posts_comments order by cid desc limit 1")  or die(mysql_error());

        $userIds = array();
        while($rows = mysql_fetch_array($result))
            $userIds[$rows['user_id']] = $rows['user_id'];

        $userService = new \Clewed\User\Service();
        $users = !empty($userIds) ? $userService->get(array_keys($userIds)) : array();

        mysql_data_seek($result, 0);
        $row['pid'] = (int) $_REQUEST['pid'] ;
        while ($rows = mysql_fetch_array($result))
        {
            $days2 = floor($rows['date_created'] / (60 * 60 * 24));
            $remainder = $rows['date_created'] % (60 * 60 * 24);
            $hours = floor($remainder / (60 * 60));
            $remainder = $remainder % (60 * 60);
            $minutes = floor($remainder / 60);
            $seconds = $remainder % 60;
            ?>
            <div class="commentPanel service-file-post"
                 data-comment-id="<?= $rows['cid']; ?>"
                 id="comment-<?php  echo $rows['cid'];?>"
                 u="<?= $editor;?>"
                 t="<?= $time = time();?>"
                 m="<?= md5($editor. ':' . $rows['cid'] . ':' . $time . ":kyarata75");?>"
                 align="left" style='width:501px; padding:6px 10px;border-bottom:solid 1px #fff;background-color:#f4f8fa!important;'>
                <div style="float:left">
                    <?php
                    $crId = $editor;
                    $avatar = $users[$crId]['avatar'];
                    echo "<img src=\"".$avatar."\" style=\"float:left; margin-top:5px;margin-right:5px; width:35px; height:35px;\">";
                    ?>
                </div>
                <div style="width:85%;float:left;"  class="postedComments">
                    <div style="color:#2db6de!important;float:left">
                        <div style="text-transform:uppercase;float:left;color:#2db6de!important;"><?=$rows['username']?></div>
                    </div>
                    <div style="clear:both"></div>
                    <div class="subcomment_text comment_text" style="padding-right:0;padding-bottom:5px;float:left;text-align:left;line-height: 16px;">
                        <?=nl2br(htmlspecialchars($rows['comment']))?>
                    </div>
                    <div style="clear:both"></div>
                    <div class="w comment_editor hidden" style="margin-left:0!important;">
                     <textarea
                         style="line-height:15px!important;width:98%; margin:0!important; height:100px; "
                         class="input mceNoEditor"><?php echo htmlspecialchars($rows['comment']);?></textarea>
                    </div>
                    <div class="comment_editor_controls hidden" style="float:right;margin: 2px 0 10px;padding-right: 0;">
                        <a class="save-post" style="cursor: pointer;padding-right: 0;" onclick="submitFilePostEditor(<?= $row['pid'];?>,<?= $rows['cid'] ?>);">Submit</a>&nbsp;|&nbsp;
                        <a style="cursor: pointer" onclick="hideFilePostEditor(<?= $row['pid'];?>,<?= $rows['cid'] ?>);">Cancel</a>
                    </div>
                    <div class="comment_anchor">
                        <div style="float:left; padding:0;">
                            <?php echo  date("D, M j, Y g:i A T ",$rows['datecreated']); ?>
                        </div>
                        <div style="float:left">
                            &nbsp;|&nbsp;<a style="padding:0;cursor:pointer;" onclick='fileFormDisplay(<?= $row['pid'] ?>);'>Reply</a>
                            &nbsp;|&nbsp;<a style="padding:0;cursor:pointer;" onclick="showFilePostEditor(<?= $row['pid'] ?>, <?= $rows['cid'] ?>)">Edit</a>
                            &nbsp;|&nbsp;<a style="padding:0;" href="#" id="cid-<?php  echo $rows['cid'];?>" cid="<?=$rows['cid'];?>" m="<?php  echo md5($rows['cid']."kyarata75") ?>" class="c_delete tool">Delete</a>
                        </div>
                    </div>
                </div>
                <div style="clear:both"></div>
            </div>
            <?php
        }

    } else die('Authentication problem');
}

if ($_REQUEST['type'] == 'pro_dis_comment') {

    if ($_REQUEST['m'] == md5($_REQUEST['dissid']."kyarata75")) {

        if (isset($_REQUEST['action']) && $_REQUEST['action'] == 'get_new_posts')
        {
            $lastPostId = (isset($_REQUEST['last_pid']) && intval($_REQUEST['last_pid']) > 0) ? intval($_REQUEST['last_pid']) : 0;
            $dissid = intval($_REQUEST['dissid']);
            $requesterId = intval($_REQUEST['requester']);

            $result = mysql_query("SELECT * FROM pro_wall_posts WHERE flag IN ('m', 'q', 'pm', 'pq')  AND pro_id = '" . $dissid . "' AND pid > " . $lastPostId );

            $posts = array();
            while ($row = mysql_fetch_array($result)) {

                $isAdmin = isDiscussionAdmin($dissid, $requesterId);
                $isInsightOwner = ifInsightOwner($dissid, $requesterId);
                $isPostOwner = ($row['user'] == $requesterId);
                $isPrivate = ($row['flag'] == 'pm' || $row['flag'] == 'pq');
                $canUserSee = !$isPrivate || $isAdmin || $isInsightOwner || $isPostOwner;

                if ($canUserSee) {
                    $postHtml = getNewInsightPostHtml($row['pid'], $row['user'], $row['f_name'], $requesterId, $row['post'], $row['flag'], $dissid, $row['tags'], $_REQUEST['pro_profile'], $row['date_created']);
                    $posts[] = array(
                        'pid' => $row['pid'],
                        'flag' => $row['flag'],
                        'html' => $postHtml,
                    );
                }
            }

            $return = array("status" => "success", "posts" => $posts);
            die(json_encode($return));

        } else {

            $uid = (int) $_REQUEST['editor'];
            $dissid = (int) $_REQUEST['dissid'];
            $uname = getUserById($uid);
            if ($_REQUEST['flag']) $flag = $_REQUEST['flag']; else $flag = 'q';
            $tags = mysql_real_escape_string($_REQUEST['tags']);
            $postMsg = mysql_real_escape_string($_REQUEST['text']);
            $pid = (int)$_REQUEST['pid'];

            $dbResource = mysql_query("SELECT pid,`user` FROM pro_wall_posts WHERE pid = " . $pid);

            if($dbResource === false) {
                //            $return = array("status" => "error", "display" => $html, 'pid' => $pid);
                //            die(json_encode($return));
            }
            $sql = '';

            $update = mysql_num_rows($dbResource) == 1;
            if ($update) {
                $_data = mysql_fetch_assoc($dbResource);
                $uid = $_data['user'];
                $sql = "UPDATE `pro_wall_posts` SET " .
                    "pro_id = '" . $dissid . "'" .
                    ", post = '" . $postMsg . "'" .
                    ", tags = '" . $tags . "'" .
                    ", flag = '" . $flag . "'" .
                    ", date_created=" . time() .
                    " WHERE pid = '" . $pid . "'";
            } else {
                $sql = "INSERT INTO `pro_wall_posts` SET ".
                    "pro_id = '" . $dissid . "'" .
                    ", post = '" . $postMsg . "'" .
                    ", f_name = '" . $uname . "'" .
                    ", `user`= '" . $uid . "'" .
                    ", tags = '" . $tags . "'" .
                    ", flag = '" . $flag . "'" .
                    ", date_created=" . time();
            }

            $result = mysql_query($sql);

            $post_id = $update ? $pid : mysql_insert_id();

            $notificationEvent = 'pq' == $flag ? 'private_message_added' : 'comment_added';
            $options = serialize(array(
                'message' => $postMsg
            ));

            mysql_query("INSERT INTO notifications (action, state, author, item, created, options) VALUES ('$notificationEvent', 1, ".((int) $_POST['editor']).", {$dissid}, NOW(), '{$options}')");


            $html = getNewInsightPostHtml($post_id, $uid, $uname, $uid, trim($_REQUEST['text']), $flag, $dissid, $_REQUEST['tags'], $_REQUEST['pro_profile'], time());

            $return = array("status" => "success", "display" => $html, 'pid' => $post_id, 'flag' => $flag);
            die(json_encode($return));
        }
    }
    else (die("Authentication problem"));


}

if ($_REQUEST['type'] == 'project_discussion_comment') {

    if ($_REQUEST['m'] == md5($_REQUEST['dissid']."kyarata75")) {

        $uid = (int) $_REQUEST['editor'];
        $dissid = (int) $_REQUEST['dissid'];
        $uname = getUserById($uid);
        $message = strip_tags($_REQUEST['text']);
        $db = \Clewed\Db::get_instance();

        $db->run("
            INSERT INTO pro_analysis_posts (pro_id, post, f_name, user, date_created)
            VALUES(?, ?, ?, ?, ?)",
            array(
                $dissid,
                $message,
                $uname,
                $uid,
                time()
            )
        );

        $post_id = $db->lastInsertId();

        $userService = new \Clewed\User\Service();
        $users = $userService->get(array($uid));
        $user = $users[$uid];

        $avatar = $user['avatar'];

        $discussion = $db->get_row("
            SELECT *
            FROM maenna_company_data
            WHERE dataid = ?
            LIMIT 1",
            array($dissid)
        );

        if(empty($discussion))
            die('Invalid request data provided');

        $authorId = (int) $discussion['editorid'];
        $userService = new Clewed\User\Service();
        $author = $userService->get(array($authorId));
        $author = $author[$authorId];
        if(empty($author))
            die('Invalid request data provided');

        $projectId = (int) $_REQUEST['pro_profile'];
        $options = serialize(array(
            'projectId' => $projectId,
            'authorId' => $authorId,
            'message' => $message
        ));

        $companyService = new \Clewed\Company\Service();
        $invitedUserIds = $companyService->getDiscussionInvitedExpertIds($dissid);
        $isPrivate = !empty($invitedUserIds);

        if(!$isPrivate) {
            $eventType = 'project_proposal_commented';
            if($authorId == $projectId || $author['is_admin'])
                $eventType = 'project_broadcast_commented';

            $res = $db->run("
                INSERT INTO notifications (action, state, item, author, created, options)
                VALUES ('{$eventType}', 1, {$dissid}, {$uid}, NOW(), '{$options}')"
            );
        }

        $html = '
            <div class="cmtloop" id="dis_post'.$post_id.'">
                <div class="ask" style="margin-left:14px;">
                    <div class="project-discussion-post" u="' .$uid.'" t="' . ($time = time()). '" m="' . md5($uid . ':' . $post_id . ':' . $time .":kyarata75") . '" data-post-id="'. $post_id .'">
                        <div class="askpic" style="width: 35px;">
                            <img src="'.$avatar.'" style="float:left; margin-top:13px; margin-right:5px; width:35px; height:35px;">&nbsp;
                        </div>
                        <div class="asktitle">'.$uname.'&nbsp;</div>
                        <p class="comment_text" style="padding: 0 0 0px 49px;">'. n2br(htmlspecialchars($_REQUEST['text'])).' </p>

                        <div class="w comment_editor hidden" style="margin-left:0!important;">
                            <textarea
                                 style="line-height:15px!important;width:90%; margin:-20px 0 0 49px!important; height:100px; "
                                 class="input mceNoEditor">'.htmlspecialchars($_REQUEST['text']).'</textarea>
                        </div>
                        <div class="comment_editor_controls hidden" style="float:right;margin: 2px 14px 10px;">
                            <a class="save-post" style="cursor: pointer;padding-right: 0;" onclick="submitEditor(' . $post_id . ');">Submit</a>&nbsp;|&nbsp;
                            <a style="cursor: pointer" onclick="hideEditor(' . $post_id . ');">Cancel</a>
                        </div>
                        <div style="margin:0px 0px 5px 49px; padding:0px 0px 2px 0px;">
                            <div style="height:30px; width: 535px;">
                                <div class="comment_anchor" style="width:350px;float:left;margin-top:-5px;">
                                <div style="float:left;padding:0;font-size:12px;">
                                        '.date("D, M j, Y g:i A T ", time()).'
                                        &nbsp;|&nbsp;
                                    </div>
                                    <a onclick="showCommentForm(' . $post_id . ');"
                                       data-tooltip="Discussing pricing or sharing personal contact creates various business challenges and is prohibited">Reply</a>
                                    |&nbsp;<a onclick="showEditor(' . $post_id . ');">Edit</a>
                                    |&nbsp;<a class="delete_post" data-id="' . $post_id . '" onclick="deleteComment(' . $post_id . ');">Delete</a>
                                </div>
                            </div>
                        </div>
                    </div>
                     <div class="askright" style="margin-top:0; width: 95%!important;"></div>
                    <div class="w post-submit-container" style="float:left; display:none;margin:0px 10px 0px 49px; padding:21px 0px 0 0px;background:#f4f8fa !important;" id="form_id'.$post_id.'">
                        <form method="post" action="" id="comments" style="position: inherit;width:536px;">
                            <textarea name="post_comment"
                                      class="comments_box mceNoEditor"
                                      style="line-height:18px!important;width:83%; margin: 20px 0px 0px 55px!important; height:100px;"></textarea>
                            <a class="analysis_subcomment save-comment"
                               style="margin:7px 25px 20px;float:right;cursor: pointer;"
                               dissid="'.$post_id.'"
                               onclick="submitCommentForm('.$post_id.')"
                               m="' . md5($post_id . "kyarata75") . '">Submit</a>
                        </form>
                        <div style="clear:both"></div>
                    </div>
                </div>
            </div>';
        $return = array("status" => "success", "display" => $html);
        die(json_encode($return));
    }
    else (die("Authentication problem"));

}

if ($_REQUEST['type'] == 'pro_analysis_comment') {

    if ($_REQUEST['m'] == md5($_REQUEST['dissid']."kyarata75")) {

        $uid = (int) $_REQUEST['editor'];
        $dissid = (int) $_REQUEST['dissid'];
        $uname = getUserById($uid);


        mysql_query("INSERT INTO pro_analysis_posts (pro_id,post,f_name,user,date_created) VALUES('".$dissid."','".checkValues(urldecode($_REQUEST['text']))."','".$uname."','".$uid."','".time()."')");
        $post_id = mysql_insert_id();
        $avatar = getAvatarUrl($uid);

        $html = '<div class="cmtloop" id="dis_post'.$post_id.'">
                <div class="ask" style="margin-left:14px;">
                  <div class="askpic" style="width: 35px;">
                    <img src="'.$avatar.'" style="float:left; margin-top:13px; margin-right:5px; width:35px; height:35px;">&nbsp;
                  </div>
                  <div class="asktitle">'.$uname.'&nbsp;</div>
                  <p style="padding: 0 0 8px 50px;">'.urldecode($_REQUEST['text']).' </p>
                  <div class="askright" style="margin-top: -8px; width:561px !important; float: left; margin-left: 50px;">
                    <div style="margin:0px 0px 5px 0px; padding:0px 0px 2px 0px;">
                      <div style="height:30px; width: 535px;">
                        <div class="comment_anchor" style="width:130px;float:left;margin-top:5px;">
                          <span style="margin:0px 0px 0px 0px;padding:0px 0px 0px 0px;" id="likepost1'.$dissid.'" >
                            <a href="javascript:void(0);" style="cursor:pointer;" onClick="like_posts(\'like\', \''.$uid.'\', \''.$post_id.'\', \''.$_REQUEST['pro_profile'].'\');">Like</a>
                          </span>
                          &nbsp;|&nbsp;<a onclick="formDisplay(\''.$post_id.'\');">Comment</a>
                        </div>
                      </div>
                    </div>
                  </div>';
        $html .= '<div class="w" style="display:none;float:left; margin:0px 10px 0px 49px; padding:21px 0px 0 0px;background:#f4f8fa!important" id="form_id'.$post_id.'">
                    <form style="position: inherit;width: 536px;" method="post" action="/account?tab=professionals&page=pro_detail&id='.$_REQUEST['pro_profile'].'&section=pro_industry_view&type=details&pro_id='.$dissid.'" id="comments">
                      <input type="hidden" name="post_id" id="post_id" value="'.$post_id.'"  />
                      <input type="hidden" name="dis_id" id="dis_id" value="'.$dissid.'"  />
                      <textarea name="post_comment" id="post_comment'.$post_id.'" class=" input watermark"
                        style="width: 506px;margin: 5px 0px 0px 10px!important;height: 25px;font-style: italic;font-family: \'Lato Light\';color: #929497;" onFocus="showsubmit(\''.$post_id.'\');"></textarea>
                      <a class="analysis_subcomment" style="display:none;vertical-align: top;" type="submit"  id="post_com'.$post_id.'" dissid="'.$dissid.'" m="'.md5($post_id."kyarata75").'" >Submit</a>
                    </form>
                  </div>
                </div></div>
                <div id="paa" style="clear:both"></div>';
        $return = array("status" => "success", "display" => $html);
        die(json_encode($return));
    }
    else (die("Authentication problem"));

}


elseif ($_REQUEST['type'] == 'pro_post_comment') {

    if ($_REQUEST['m'] == md5($_REQUEST['dissid']."kyarata75") && isset($_REQUEST['action']) && $_REQUEST['action'] == 'get_new_comments') {

        $lastCommentId = (isset($_REQUEST['last_comment_id']) && intval($_REQUEST['last_comment_id']) > 0) ? intval($_REQUEST['last_comment_id']) : 0;
        $dissid = intval($_REQUEST['dissid']);
        $result = mysql_query("SELECT c.*, p.flag FROM pro_wall_posts_comments AS c JOIN pro_wall_posts AS p ON (p.pid=c.post_id) WHERE p.pro_id = '" . $dissid . "' AND c.cid > " . $lastCommentId);

        $comments = array();
        while ($row = mysql_fetch_array($result)) {
            $html = getNewInsightCommentHtml($row['cid'], $row['user_id'], (int)$_REQUEST['requester'], $row['post_id'], $row['flag'], $dissid, $row['comment'], $row['datecreated']);
            $comments[] = array(
                'cid' => $row['cid'],
                'pid' => $row['post_id'],
                'html' => $html,
            );
        }

        $return = array("status" => "success", "comments" => $comments);
        die(json_encode($return));

    } elseif ($_REQUEST['m'] == md5($_REQUEST['post_id']."kyarata75")) {

        $uid = (int) $_REQUEST['editor'];
        $uname = getUserById($uid);
        $dissid = intval($_REQUEST['dissid']);

        $result = mysql_query("SELECT * FROM pro_wall_posts WHERE pid='" . (int)$_REQUEST['post_id'] . "'");
        $postRow = mysql_fetch_array($result);

        $comm_id = (int) $_REQUEST['comment_id'];
        if ($comm_id) {
            mysql_query("UPDATE pro_wall_posts_comments SET comment='".checkValues($_REQUEST['text'])."' WHERE post_id='".mysql_real_escape_string($_REQUEST['post_id'])."' AND cid='".mysql_real_escape_string($comm_id)."'");
        } else {
            mysql_query("INSERT INTO pro_wall_posts_comments
                            (d_id, post_id, user_id, comment, username, datecreated)
                        VALUES (
                            '0',
                            '".mysql_real_escape_string($_REQUEST['post_id'])."',
                            '".$uid."',
                            '".checkValues($_REQUEST['text'])."',
                            '".mysql_real_escape_string($uname)."',
                            " . time() .
                ")");
            $comm_id = mysql_insert_id();

            $notificationEvent = $_REQUEST['is_private'] ? 'private_message_replied' : 'comment_added';
            $item = $_REQUEST['is_private'] ? (int)$_REQUEST['post_id'] : (int) $postRow['pro_id'];
            $options = serialize(array(
                'message' => checkValues($_REQUEST['text'])
            ));

            mysql_query("INSERT INTO notifications (action, state, author, item, created, options) VALUES ('$notificationEvent', 1, ".((int) $_REQUEST['editor']).", ".($item).", NOW(), '{$options}')");
        }

        $html = getNewInsightCommentHtml($comm_id, $uid, $uid, (int)$_REQUEST['post_id'], $postRow['flag'], $dissid, $_REQUEST['text'], time());

        $return = array("status" => "success", "display" => $html, 'cid' => $comm_id);
        die(json_encode($return));
    }
    else (die("Authentication problem"));

}

elseif($_REQUEST['type'] == 'project-service-post') {

    $u = (int) $_REQUEST['u'];
    $t = (int) $_REQUEST['t'];
    $m = $_REQUEST['m'];
    $calculatedM = md5("add-project-service-note:{$u}:{$t}:kyarata75");
    if($m !== $calculatedM || time() - $t > 3 * 60 * 60)
        die('Authentication problem');

    $text = strip_tags($_REQUEST['text']);
    $authorId = (int) $_REQUEST['editor'];
    $serviceId = (int) $_REQUEST['view_id'];
    $uName = getUserById($authorId);

    $db = \Clewed\Db::get_instance();
    $db->run("
        INSERT INTO pro_wall_posts
        SET
            pro_id = ?,
            post = ?,
            f_name = ?,
            user = ?,
            flag = 'am',
            date_created = ?",
        array(
            $serviceId,
            $text,
            $uName,
            $authorId,
            time()
        )
    );

    $postId = $db->lastInsertId();
    if(!empty($postId)) {
        $notificationService = new \Clewed\Notifications\NotificationService();
        $options = array(
            'message' => $text
        );
        $notificationService->registerEvent(
            'company_service_notes_added',
            (int) $serviceId,
            (int) $authorId,
            $options
        );
    }

    $userService = new \Clewed\User\Service();
    $users = $userService->get(array($authorId));
    $avatar = $users[$authorId]['avatar'];

    $row1 = $db->get_row("
        SELECT *
        FROM pro_wall_posts
        WHERE pid = ?",
        array($postId)
    );

    $user_id = $authorId;

    ob_start();?>

    <div class="cmtloop" id="dis_post<?php echo $row1['pid']; ?>">

        <div class="ask">
            <div class="company-service-post-reply" u="<?= $user_id;?>" t="<?= $time = time();?>" m="<?= md5($user_id . ':' . $row1['pid'] . ':' . $time .":kyarata75");?>" data-post-id="<?= $row1['pid'];?>" >
                <?php if ($row1['flag'] != 'm') { ?>
                    <div class="askpic">
                        <?php echo "<img src=" . $avatar . " style=\"float:left; margin-top:13px; margin-right:5px; width:35px; height:35px;\">&nbsp;"; ?>
                    </div>
                    <?php if ($row1['flag'] != 'm') { ?>

                        <?php
                        if (strpos($row1['f_name'], 'SID_') !== false) {
                            list($garb, $qcompid) = explode('_', $row1['f_name']);
                            $projres = db_query("SELECT projname FROM {maenna_company} WHERE companyid = %d", $qcompid);
                            $row1fname = db_fetch_array($projres);
                            $row1['f_name'] = $row1fname['projname'];
                        }
                        ?>
                        <div class="asktitle"><?php echo ((getUserById($row1['user']) == '' || getUserById($row1['user']) == 'invalid id') ? $row1['f_name'] : getUserById($row1['user'])); ?>&nbsp;<?php if ($row1['flag'] == 'q') {
                                echo '<strong>asks a question:</strong>';
                            } ?></div>
                    <?php } ?>

                <?php } ?>
                <div class="minutes_post_comment comment_text" style="margin:5px 0px 0px 65px;"><?= n2br(htmlspecialchars($row1['post'])) ?> </div>
                <?php if ($row1['flag'] != 'm') {
                    $width = "525px";
                } else {
                    $width = "592px";
                } ?>

                <div class="w comment_editor hidden" style="margin-left:0!important;">
                             <textarea
                                 style="line-height:15px!important;width:83%; margin:-15px 0 0 63px!important; height:100px; "
                                 class="input mceNoEditor"><?php echo htmlspecialchars($row1['post']);?></textarea>
                </div>
                <div class="comment_editor_controls hidden" style="float:right;margin: 2px 14px 10px;">
                    <a class="save-post" style="cursor: pointer;padding-right: 0;" onclick="submitEditor(<?= $row1['pid'];?>);">Submit</a>&nbsp;|&nbsp;
                    <a style="cursor: pointer" onclick="hideEditor(<?= $row1['pid'];?>);">Cancel</a>
                </div>

                <div style="margin:0px 0px 5px 0px; padding:0px 0px 2px 0px;">
                    <div class='comment_anchor' style="height:30px; width: 535px;margin-left: 65px">
                        <div style="float:left;padding-right: 0;"><?php echo  date("D, M j, Y g:i A T ",$row1['date_created']); ?></div>
                        <div style="width:200px;float:left;">

                            &nbsp;|&nbsp;<a onclick='formDisplay("<?= $row1['pid'] ?>");'>Reply</a>
                            &nbsp;|&nbsp;<a onclick="showEditor(<?= $row1['pid'] ?>)">Edit</a>
                            &nbsp;|&nbsp;<a onclick='deleteMinutesConfirm("<?= $row1['pid'] ?>");'>Delete</a>
                        </div>
                        <!--<div style='float:right;margin:7px 0px 0px 0px;color:#76787f'>Topic: <?php echo $row1['tags']; ?></div>-->
                        <!--<a href="#">Invite</a>-->
                    </div>

                </div>
            </div>
            <div class="askright" id="comments_<?= $row1['pid'] ?>"
                 style="width:<?php echo $width; ?> !important; float: left;">
                <div style="clear:both"></div>

                <?php $comment_style = "style='width:511px;margin:21px 0px 0px 0px; padding:0px 0px 0px 20px; background:#f4f8fa!important;'"; ?>

                <div class="w hidden" <?php echo $comment_style; ?> id='form_id<?= $row1['pid'] ?>'>
                    <form style="position:static;" method="post"
                          action="<?php echo $base_url; ?>/account?tab=professionals&page=pro_detail&id=<?php echo $id; ?>&section=pro_industry_view&type=details&pro_id=<?php echo $_REQUEST['pro_id']; ?>"
                          id="comments">
                        <input type="hidden" name="post_id" id="post_id" value="<?php echo $row1['pid']; ?>"/>
                        <input type="hidden" name="dis_id" id="dis_id"
                               value="<?php echo $_REQUEST['pro_id']; ?>"/>

                        <?php
                        if ($row1['flag'] != 'm') {
                            $marginleft = "81%";
                            $width = "80%";
                        } else {
                            $marginleft = "79%";
                            $width = "79%";
                        }   ?>

                        <textarea name="post_comment"
                                  id="post_comment<?php echo $row1['pid']; ?>"
                                  class="input watermark mceNoEditor"
                                  style="line-height:15px!important;width:<?php echo $width; ?>;  margin: 20px 0px 0px 42px!important; height:100px; "
                                  onFocus="showsubmit('<?php echo $row1['pid']; ?>');"></textarea>

                        <input type="submit" id="post_com<?php echo $row1['pid']; ?>"
                               m= <?= md5($row1['pid'] . "kyarata75"); ?>  value="Submit" class="text_button"
                               style="margin-bottom:20px;display:none;vertical-align: top;color:#00A3BF!important;margin-top: 0px;margin-left:<?php echo $marginleft; ?>;"/>
                    </form>
                    <!-- <textarea id="post_comments" style="width:85%; margin-left: 82px!important; margin-top: 10px!important;" ></textarea>-->
                </div>

            </div>
        </div>

        <div style="clear:both"></div>
    </div>

    <?php

    $content = ob_get_contents();
    ob_end_clean();

    die(json_encode(array(
        'status' => 'success',
        'display' => $content
    )));
}

elseif ($_REQUEST['type'] == 'com_post_comment') {

    if ($_REQUEST['m'] == md5($_REQUEST['post_id']."kyarata75")) {

        $uid = (int) $_REQUEST['editor'];
        $uname = getUserById($uid);

        $now = time();

        mysql_query("INSERT INTO pro_wall_posts_comments (post_id,user_id,comment,datecreated) VALUES('".mysql_real_escape_string($_REQUEST['post_id'])."','".$uid."','".checkValues($_REQUEST['text'])."','".$now."')") or die(mysql_error());

        $comm_id = mysql_insert_id();

        $avatar = getAvatarUrl($uid);

        $html = '<div class="aucomnts">
						<div class="aucpic">
						<img src="'.$avatar.'" style="float:left; margin-top:13px; margin-right:5px; width:45px; height:45px;">&nbsp;
						</div>
						<div class="aucdisc">
                            <h5 style="text-transform:uppercase;">'.$uname.'</h5>
                            <br />
                            <div class="comment_text" style="padding-left:0px!important;width:465px" id="com'.$comm_id.'">'.$_REQUEST['text'].'</div>
                            <div class="comment_anchor">
                                <div style="float:left;margin:0px;padding:0px;font-size:12px;">
                                    '.date("D, M j, Y g:i A T ",$now).'
                                    &nbsp;|&nbsp;
                                </div>
                                <div id="likepostcomment'.$comm_id.'" style="float:left;padding:0px 0px 0px 0px;">

                                        <a href="javascript:void(0);" style="cursor:pointer;" onClick="like_post_comments(\'like\',  \''.$comm_id.'\', \''.$uid.'\');">Like</a>
                                </div>
                                &nbsp;|&nbsp;<a style="cursor:pointer;" onclick="show_minutes(); edit_comments('.$comm_id.');"
                                                             id="edit_comment'.$comm_id.'"
                                                             class="edit_comment">Edit</a>
                                        &nbsp;|&nbsp;<a style="cursor:pointer;" href="javascript:void(0);" id="delete_comment'.$comm_id.'" class="delete_comment">Delete</a>
                                       </p></div>
                        </div><div style="clear:both"></div>
	</div>';

        $return = array("status" => "success", "display" => $html);
        die(json_encode($return));
    }
    else (die("Authentication problem"));

}

elseif ($_REQUEST['type'] == 'edit_company_service_post') {

    if ($_REQUEST['m'] == md5($_REQUEST['u']. ":" . $_REQUEST['id'] . ':' . $_REQUEST['t'] . ":kyarata75")) {

        if(time() - $_REQUEST['t'] > 1 * 60 * 60)
            die('Invalid timestamp');

        if('comment' == $_REQUEST['ctype']) {
            \Clewed\Db::get_instance()->run("
                UPDATE pro_wall_posts_comments
                SET comment = ?
                WHERE cid = ?",
                array(
                    strip_tags($_REQUEST['text']),
                    $_REQUEST['id']
                )
            );
        }
        elseif('post' == $_REQUEST['ctype']) {
            \Clewed\Db::get_instance()->run("
                UPDATE pro_wall_posts
                SET post= ?
                WHERE pid = ?",
                array(
                    strip_tags($_REQUEST['text']),
                    $_REQUEST['id']
                )
            );
        }

        die(json_encode(array("status" => "success")));
    }
    else
        die("Authentication problem");
}

elseif ($_REQUEST['type'] == 'edit_project_discussion_post') {

    if ($_REQUEST['m'] == md5($_REQUEST['u']. ":" . $_REQUEST['id'] . ':' . $_REQUEST['t'] . ":kyarata75")) {

        if(time() - $_REQUEST['t'] > 1 * 60 * 60)
            die('Invalid timestamp');

        if('comment' == $_REQUEST['ctype']) {
            \Clewed\Db::get_instance()->run("
                UPDATE analysis_wall_post_comments
                SET comment = ?
                WHERE cid = ?",
                array(
                    strip_tags($_REQUEST['text']),
                    $_REQUEST['id']
                )
            );
        }
        elseif('post' == $_REQUEST['ctype']) {
            \Clewed\Db::get_instance()->run("
                UPDATE pro_analysis_posts
                SET post = ?
                WHERE pid = ?",
                array(
                    strip_tags($_REQUEST['text']),
                    $_REQUEST['id']
                )
            );
        }

        die(json_encode(array("status" => "success")));
    }
    else
        die("Authentication problem");
}

elseif ($_REQUEST['type'] == 'edit_service_file_post') {

    if ($_REQUEST['m'] == md5($_REQUEST['u']. ":" . $_REQUEST['id'] . ':' . $_REQUEST['t'] . ":kyarata75")) {

        if(time() - $_REQUEST['t'] > 1 * 60 * 60)
            die('Invalid timestamp');

        if('comment' == $_REQUEST['ctype']) {
            \Clewed\Db::get_instance()->run("
                UPDATE pro_wall_posts_comments
                SET comment = ?
                WHERE cid = ?",
                array(
                    strip_tags($_REQUEST['text']),
                    $_REQUEST['id']
                )
            );
        }
        elseif('post' == $_REQUEST['ctype']) {
            \Clewed\Db::get_instance()->run("
                UPDATE pro_wall_posts
                SET post = ?
                WHERE pid = ?",
                array(
                    strip_tags($_REQUEST['text']),
                    $_REQUEST['id']
                )
            );
        }

        die(json_encode(array("status" => "success")));
    }
    else
        die("Authentication problem");
}


elseif ($_REQUEST['type'] == 'company_service_post_reply') {

    if ($_REQUEST['m'] == md5($_REQUEST['post_id']."kyarata75")) {

        $uid = (int) $_REQUEST['editor'];
        $uname = getUserById($uid);
        $now = time();
        $text = strip_tags($_REQUEST['text']);

        $db = \Clewed\Db::get_instance();
        $db->run("
            INSERT INTO pro_wall_posts_comments (post_id,user_id,comment,datecreated)
            VALUES(?, ?, ?, ?)",
            array(
                (int) $_REQUEST['post_id'],
                $uid,
                $text,
                $now
            )
        );

        $comm_id = $db->lastInsertId();
        if(!empty($comm_id)) {
            $post = $db->get_row("
                SELECT *
                FROM pro_wall_posts
                WHERE pid = ?",
                array((int) $_REQUEST['post_id'])
            );
            if(!empty($post)) {
                $notificationService = new \Clewed\Notifications\NotificationService();
                $options = array(
                    'message' => $text
                );
                $notificationService->registerEvent(
                    'company_service_notes_added',
                    (int) $post['pro_id'],
                    (int) $uid,
                    $options
                );
            }
        }

        $userService = new Clewed\User\Service();
        $users = $userService->get(array($uid));
        $user = $users[$uid];
        $avatar = $user['avatar'];

        $html = '<div class="aucomnts company-service-post-reply" u="' . $_REQUEST['editor'] . '" t="' . ($time = time()) . '" m="' . md5($_REQUEST['editor'] . ':' . $comm_id . ':' . $time .":kyarata75") . '" data-comment-id="' . $comm_id . '">
						<div class="aucpic">
						<img src="' . $avatar . '" style="float:left; margin-top:13px; margin-right:5px; width:45px; height:45px;">&nbsp;
						</div>
						<div class="aucdisc">
                            <h5 style="text-transform:uppercase;">'.$uname.'</h5>
                            <br />
                            <div class="comment_text" style="padding:7px 20px 7px 0!important;width:465px" id="com'.$comm_id.'">'.n2br(htmlspecialchars($_REQUEST['text'])).'</div>
                             <div class="w comment_editor hidden" style="margin-left:0!important;">
                                 <textarea
                                     style="line-height:15px!important;width:92%; margin:0!important; height:100px; "
                                     class="input mceNoEditor">' . htmlspecialchars($_REQUEST['text']) . '</textarea>
                             </div>
                             <div class="comment_editor_controls hidden" style="float:right;margin: 2px 14px 10px;">
                                 <a class="save-post" style="cursor: pointer;padding-right: 0;" onclick="submitEditor(' . $_REQUEST['post_id'] . ',' . $comm_id . ');">Submit</a>&nbsp;|&nbsp;
                                 <a style="cursor: pointer" onclick="hideEditor(' . $_REQUEST['post_id'] . ',' . $comm_id . ');">Cancel</a>
                             </div>
                            <div class="comment_anchor">
                                <div style="float:left;margin:0px;padding:0px;font-size:12px;">
                                    '.date("D, M j, Y g:i A T ",$now).'
                                    &nbsp;|&nbsp;
                                </div>
                                 <a onclick="formDisplay(' . $_REQUEST['post_id'] . ');">Reply</a>&nbsp;|&nbsp;
                                 <a style="cursor:pointer;"
                                    onclick="showEditor(' . $_REQUEST['post_id'] . ',' . $comm_id .');"
                                    id="edit_comment'.$comm_id.'"
                                    class="edit_comment">Edit</a>&nbsp;|&nbsp;
                                 <a style="cursor:pointer;"
                                    href="javascript:void(0);"
                                    id="delete_comment'.$comm_id.'"
                                    u="' . ($u = time()) . '"
                                    m="' . md5('delete.php:' . $comm_id . ':' . $u . ':kyarata75') . '"
                                    class="delete_comment">Delete</a>
                                       </p>
                           </div>
                        </div><div style="clear:both"></div>
	</div>';

        $return = array("status" => "success", "display" => $html);
        die(json_encode($return));
    }
    else (die("Authentication problem"));

}


if($_REQUEST['comment_text'] && $_REQUEST['post_id'])
{
    $editorname = getUserById($_REQUEST['uid']);

    if (md5($_REQUEST['u']."kyarata75") === $_REQUEST['m'])
    {
        mysql_query("INSERT INTO wall_posts_comments (post_id,comments,f_name,user,date_created) VALUES('".mysql_real_escape_string($_REQUEST['post_id'])."','".mysql_real_escape_string($_REQUEST['comment_text'])."','".$editorname."','".mysql_real_escape_string($_REQUEST['uid'])."','".strtotime(date("Y-m-d H:i:s"))."')")  or die(mysql_error());
    }
    $result = mysql_query("SELECT *, UNIX_TIMESTAMP() - date_created AS CommentTimeSpent FROM wall_posts_comments order by c_id desc limit 1")  or die(mysql_error());
}

while ($rows = mysql_fetch_array($result))
{
    $days2 = floor($rows['CommentTimeSpent'] / (60 * 60 * 24));
    $remainder = $rows['CommentTimeSpent'] % (60 * 60 * 24);
    $hours = floor($remainder / (60 * 60));
    $remainder = $remainder % (60 * 60);
    $minutes = floor($remainder / 60);
    $seconds = $remainder % 60;	?>
    <div style="padding-top:6px;border-bottom:solid 1px #fff;width:540px;background-color: #f4f8fa!important" class="commentPanel" id="comment-<?php  echo $rows['c_id'];?>" align="left">

        <?php

        $crId = $_REQUEST['uid'];
        $avatar = getAvatarUrl($crId);

        echo "<img src=".$avatar." style=\"float:left;margin-top:5px; margin-right:5px; width:35px; height:35px;\">";

        ?>
        <label style="width:85%;color:#8f9094!important;" class="postedComments">
				<span style="color:#00a2bf"><b>
                <?php if (ifAdmin($rows['user'])) {
                    echo "clewed";

                } else
                    echo $rows['f_name']; echo "</b><span style='color:#8f9094;font-size:12px;font-style:italic;'>&nbsp;continues discussion:</span><br />";?>
                </span>
            <?php  echo nl2br($rows['comments']);?>

            <br><span style="display:inline-block;width:90px; color:#666666; font-size:11px">
			<?php
            if($days2 > 0)
                echo  date("l, M j, Y g:i A T ", $rows['date_created']);
            elseif($days2 == 0 && $hours == 0 && $minutes == 0)
                echo "few seconds ago";
            elseif($days2 == 0 && $hours == 0)
                echo $minutes.' minutes ago';
            else
                echo "few seconds ago";
            ?>
			</span>

            <?php

            if($rows['user'] == $_REQUEST['u']){?>
                &nbsp;&nbsp;<a href="#" id="CID-<?php  echo $rows['c_id'];?>" alt="<?php  echo md5($rows['c_id'].$rows['user']."kyarata75") ?>" name="<?=$rows['user'];?>" class="c_delete tool">Delete</a>
                <?php
            }?>
    </div></label>
    <?php
}?>
