<?php
/**
 * NOTE:
 * THIS FUNCTIONALITY IS SHARED WITH
 *  - COMPANY COMMENTS BELOW FILES ATTACHED TO A DISCUSSION
 *  - READING/IDEAS SECTION,INSIGHTS SECTION IN PRO PROFILE
 *  - FILES COMMENTS IN FILE TAB
 */
global $AccessObj;
global $user;
if (!isset($user->uid)) {
    $user_id = $_REQUEST ['uid'];
}

include_once __DIR__ . '/safe_functions.inc';

$fid = $_REQUEST['fid'];
if (isset($_REQUEST['flag'])) $flag = $_REQUEST['flag']; else $flag = 'f'; //Regular files stored in wall_documents db table

if (isset($_REQUEST['mtab'])) {

    if ($_REQUEST['mtab'] == 'file') {

        if (isset($url) && $url) {
            $res = mysql_query("SELECT d_id FROM wall_documents where document_name = '" . basename($url) . "'");
            $red = mysql_fetch_array($res);
            $fid = $red['d_id'];
        }

    } else if ($_REQUEST['tab'] == 'companies' && $_REQUEST['mtab'] == 'advice') {
        $flag = 'af';
    } //Advice files
}

if (isset($AccessObj->uid))
    $editor = $AccessObj->uid;
else
    $editor = $_REQUEST['uid'];

if ($_REQUEST['value']) {

    include('dbcon.php');

    error_reporting(0);

    if (md5($_REQUEST['uid'] . $_REQUEST['fid'] . "kyarata75") === $_REQUEST['m']) {

        //Check if post is only edited
        $message = strip_tags($_REQUEST['value']);
        if (isset($_REQUEST['bEdit']) && $_REQUEST['bEdit'] == "true") {

            mysql_query("UPDATE pro_wall_posts SET post = '" . mysql_real_escape_string($message) . "',user = '" . mysql_real_escape_string($_REQUEST['uid']) . "',date_created = '" . time() . "' WHERE pid = '" . mysql_real_escape_string($_REQUEST['pid']) . "' ") or die(mysql_error());

            exit(1);

        }

        $editor = $_REQUEST['uid'];
        $editorname = getUserById($editor);

        mysql_query("INSERT INTO pro_wall_posts (pro_id,post,f_name,user,tags,flag,date_created) VALUES('" . checkValues($_REQUEST['fid']) . "','" . mysql_real_escape_string($message) . "','" . $editorname . "','" . $editor . "','','" . $flag . "','" . time() . "')")
        or die('Database connection problem!' . mysql_error());

        $db = \Clewed\Db::get_instance();
        $fileId = (int) $_REQUEST['fid'];
        $file = $db->get_row("SELECT * FROM maenna_company_data WHERE dataid = ?", array($fileId));
        if(!empty($file) && 'service-file' == $file['data_type']) {
            $notificationService = new \Clewed\Notifications\NotificationService();
            $notificationService->registerEvent(
                'company_service_file_commented',
                (int) $file['data_value6'],
                (int) $editor,
                array(
                    'id' => $fileId,
                    'fileName' => $file['data_value2'],
                    'authorId' => $file['editorid'],
                    'message' => $message
                )
            );
        }

        $pid = mysql_insert_id();

        $result = mysql_query("SELECT *, UNIX_TIMESTAMP() - date_created AS TimeSpent FROM pro_wall_posts WHERE pid = '" . $pid . "' limit 1")
        or die('Database connection problem!' . mysql_error());
    }
    else
        die('Authentication problem!');
}
elseif($_REQUEST['show_more_post']) // more posting paging
{
    $next_records = $_REQUEST['show_more_post'] + 10;

    $fid = $_REQUEST['fid'];

    $result = mysql_query("SELECT *, UNIX_TIMESTAMP() - date_created AS TimeSpent FROM pro_wall_posts WHERE pro_id = " . $fid . " order by p_id desc limit " . $_REQUEST['show_more_post'] . ", 10");

    $check_res = mysql_query("SELECT * FROM pro_wall_posts WHERE pro_id = " . $fid . " order by p_id desc limit " . $next_records . ", 10");

    $show_more_button = 0; // button in the end

    $check_result = mysql_num_rows(@$check_res);
    if ($check_result > 0) {
        $show_more_button = 1;
    }
} else {
//    echo "<style>";
//    require_once('themes/maennaco/jui/comments/css/screen.css');
//    echo "</style>";
    $show_more_button = 1;
    $result = mysql_query("SELECT *, UNIX_TIMESTAMP() - date_created AS TimeSpent FROM pro_wall_posts WHERE pro_id = " . $fid . " and flag = '" . $flag . "' order by pid desc limit 0,10") or die(mysql_error());
}
if ($result) {

    $postUserIds = array();
    while ($row = mysql_fetch_array($result))
        $postUserIds[$row['user']] = $row['user'];

    $userService = new \Clewed\User\Service();
    $postUsers = !empty($postUserIds) ? $userService->get(array_keys($postUserIds)) : array();

    mysql_data_seek($result, 0);

    while ($row = mysql_fetch_array($result)) {
        $comments = mysql_query("SELECT *, UNIX_TIMESTAMP() - datecreated AS CommentTimeSpent FROM pro_wall_posts_comments where post_id = " . $row['pid'] . " order by cid asc") or die(mysql_error());

        $result_like = mysql_query("SELECT * FROM  like_discussion_posts WHERE post_id = '" . $row['pid'] . "' and user_id = '" . $user_id . "'");
        $likepost = mysql_num_rows($result_like);

        ?>
        <div style="margin-top:15px;" class="friends_area ask" id="record-<?php echo $row['pid'] ?>">
            <?php
            $crId = $row['user'];
            $avatar = $postUsers[$crId]['avatar'];
            ?>
            <?php //echo "<img src=".$avatar." style=\"float:left; margin-top:13px; margin-right:5px; width:50px; height:50px;\">";
            ?>
            <div class="service-file-post"
                 data-post-id="<?= $row['pid']?>"
                 u="<?= $editor;?>"
                 t="<?= $time = time();?>"
                 m="<?= md5($editor. ":" . $row['pid'] . ':' . $time . ":kyarata75");?>">
                <div style="float:left">
                    <?php echo "<img src=" . $avatar . " style=\"margin-right:5px; width:35px; height:35px;\">"; ?>
                </div>
                <div style="float:left;width:88%;">
                    <div style="color:#4169AF;float:left">
                        <div
                            style="text-transform:uppercase;float:left;font-weight: bold;color:#2db6de!important;"><?php echo getUserById($crId); ?></div>
                    </div>
                    <div style="clear:both"></div>
                    <div class="comment_text"
                         style="padding-right:0;padding-bottom:5px;float:left;text-align:left;line-height: 16px;"><?php echo nl2br(htmlspecialchars($row['post'])); ?></div>
                    <div style="clear:both"></div>
                    <div class="w comment_editor hidden" style="margin-left:0!important;">
                         <textarea
                             style="line-height:15px!important;width:98%; margin:0!important; height:100px; "
                             class="input mceNoEditor"><?php echo htmlspecialchars($row['post']);?></textarea>
                    </div>
                    <div class="comment_editor_controls hidden" style="float:right;margin: 2px 0 10px;padding-right: 0;">
                        <a class="save-post" style="cursor: pointer;padding-right: 0;" onclick="submitFilePostEditor(<?= $row['pid'];?>);">Submit</a>&nbsp;|&nbsp;
                        <a style="cursor: pointer" onclick="hideFilePostEditor(<?= $row['pid'];?>);">Cancel</a>
                    </div>
                    <div class="comment_anchor">
                        <div style="float:left;padding:0;"><?php echo date("D, M j, Y g:i A T ", $row['date_created']); ?></div>
                        <div style="float:left;">


                            &nbsp;|&nbsp;<a style="padding:0;cursor:pointer;" onclick='fileFormDisplay("<?= $row['pid'] ?>");'>Reply</a>
                            <?php if (
                                isset($pid) ||
                                $AccessObj->user_type == 'super' ||
                                $AccessObj->user_type == 'admin' ||
                                $op_comments == 'write' ||
                                ($op_comments == 'edit' && $row['user'] == $AccessObj->uid)) { ?>

                                &nbsp;|&nbsp;<a style="padding:0;cursor:pointer;" onclick='showFilePostEditor("<?= $row['pid'] ?>");'>Edit</a>
                                &nbsp;|&nbsp;<a id="remove_id<?php echo $row['pid'] ?>"
                                                       pid="<?php echo $row['pid'] ?>"
                                                       style="cursor:pointer;padding:0;"
                                                       m="<?php echo md5($row['pid'] . "kyarata75") ?>">Delete</a>
                            <?php } ?>
                        </div>
                        <div style="clear:both"></div>
                    </div>
                    <div style="clear:both"></div>
                </div>
                <div style="clear:both;"></div>
            </div>
            <div id="CommentPosted<?php echo $row['pid'] ?>" style="margin-left:46px">
                <?php

                $commentUserIds = array();
                while ($rows = mysql_fetch_array($comments))
                    $commentUserIds[$rows['user_id']] = $rows['user_id'];

                $userService = new \Clewed\User\Service();
                $commentUsers = !empty($commentUserIds) ? $userService->get(array_keys($commentUserIds)) : array();

                mysql_data_seek($comments, 0);

                $comment_num_row = mysql_num_rows(@$comments);
                if ($comment_num_row > 0) {
                    while ($rows = mysql_fetch_array($comments)) {
                        $days2 = floor($rows['date_created'] / (60 * 60 * 24));
                        $remainder = $rows['date_created'] % (60 * 60 * 24);
                        $hours = floor($remainder / (60 * 60));
                        $remainder = $remainder % (60 * 60);
                        $minutes = floor($remainder / 60);
                        $seconds = $remainder % 60;

                        $result_like_comment = db_query("SELECT count(*) as like_cnt FROM like_discussion_posts_comments WHERE comment_id = %d and user_id = %d", array($rows['cid'], $user_id));
                        $like_comment = db_fetch_object($result_like_comment);

                        ?>
                        <div class="commentPanel service-file-post"
                             data-comment-id="<?= $rows['cid']; ?>"
                             id="comment-<?= $rows['cid']; ?>"
                             u="<?= $editor;?>"
                             t="<?= $time = time();?>"
                             m="<?= md5($editor. ":" . $rows['cid'] . ':' . $time . ":kyarata75");?>"
                             align="left"
                             style='width:513px; padding:6px 4px;border-bottom:solid 1px #fff;background-color:#f4f8fa!important;'>
                            <div style="float:left;margin-left: 8px;">
                                <?php
                                $crId = $rows['user_id'];
                                $avatar = $commentUsers[$crId]['avatar'];
                                echo "<img src=\"" . $avatar . "\" style=\"float:left; margin-top:5px;margin-right:5px; width:35px; height:35px;\">";
                                ?>
                            </div>
                            <div style="width:85%;float:left;" class="postedComments">
                                <div style="color:#2db6de!important;float:left">
                                    <div
                                        style="text-transform:uppercase;float:left;color:#2db6de!important;"><?= $rows['username'] ?></div>
                                </div>
                                <div style="clear:both"></div>
                                <div class="subcomment_text comment_text" style="padding-right:0;padding-bottom:5px;float:left;text-align:left;line-height: 16px;">
                                    <?php echo nl2br(htmlspecialchars($rows['comment'])); ?>
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
                                    <div style="float:left;padding:0;">
                                        <?php echo date("D, M j, Y g:i A T ", $rows['datecreated']); ?>
                                    </div>

                                    &nbsp;|&nbsp;<a style="padding:0;cursor:pointer;" onclick='fileFormDisplay("<?= $row['pid'] ?>");'>Reply</a>
                                    <?php if ($AccessObj->user_type == 'super' || $AccessObj->user_type == 'admin' || $op_subcomments == 'write' || ($op_subcomments == 'edit' && $rows['user_id'] == $AccessObj->uid)) { ?>
                                        &nbsp;|&nbsp;<a style="padding:0;cursor:pointer;" onclick="showFilePostEditor(<?= $row['pid'] ?>, <?= $rows['cid'] ?>)">Edit</a>
                                        &nbsp;|&nbsp;<a
                                            style="padding:0;"
                                            href="#"
                                            id="cid-<?php echo $rows['cid']; ?>"
                                            cid="<?= $rows['cid']; ?>"
                                            m="<?php echo md5($rows['cid'] . "kyarata75") ?>"
                                            class="c_delete tool">Delete</a>
                                    <?php } ?>

                                </div>
                            </div>
                            <div style="clear:both"></div>
                        </div>
                        <?php
                    } ?>
                    <?php
                } ?>
            </div>
            <div style="margin-left:47px;width: 510px;background-color: #f4f8fa!important;" class="commentBox hidden w"
                 id="commentBox-<?php echo $row['pid']; ?>"
                <?php //echo (($comment_num_row) ? '' :'style=""')
                ?>
                 name="<?php echo $editorname; ?>"
                 alt="<?php echo md5($editorname . "kyarata75") ?>">
                <div id="record-<?php echo $row['pid']; ?>" style="padding-top:0;">
                <textarea placeholder="Enter your reply here" class="mceNoEditor commentMark"
                      rowid="<?php echo $row['pid']; ?>"
                      id="commentMark-<?php echo $row['pid']; ?>"
                      name="commentMark" cols="120"
                      style="line-height:18px!important;width: 87%;margin-top: 20px!important;margin-left: 49px!important;height:100px;border:solid 1px #d0d2d3;"></textarea>
                </div>
                <div style="clear:both;"></div>
                <div style="float:right;margin-right: 4px;margin-top: 7px;margin-bottom: 15px;">
                    <a style="padding-right:0px;"
                       onclick="submitFileComment(<?php echo $row['pid']?>, this);"
                       class="SubmitSubComment tool comment  comentboxBtns-<?php echo $row['pid']; ?>"
                       uid="<?= $editor; ?>" pid="<?= $row['pid'] ?>"
                       m="<?= md5($editor . $row['pid'] . "kyarata75"); ?>">Submit</a>
                </div>
                <div style="clear:both;"></div>
            </div>
        </div>
        <div style="clear:both;"></div>
        <?php
    }
}
