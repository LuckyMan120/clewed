<?php

error_reporting(0);
global $base_url;
global $user;
global $AccessObj;
$editorid = $user->uid;
$companyid = $_REQUEST['id'];
$time = time();
$data_type = 'share_knowledge';
$id = $_REQUEST['id'];
$analysis_id = (int) $_REQUEST['data_id'];
$uname = $user->name;

$user_id = $user->uid;

$opCom = $AccessObj->Com_sections['share_knowledge']['sections']['share_knowledge_commenting']['access'];
//die("op=".$op."<br>opcom=".$opCom);

function n2br($text) {
    return preg_replace("|\n|", "<br>", $text);
}

include('dbcon.php');
// find out the domain:
$domain = $_SERVER['HTTP_HOST'];
// find out the path to the current file:
$path = $_SERVER['SCRIPT_NAME'];
$result = mysql_query("select * from maenna_company_data where data_type = '" . mysql_real_escape_string($data_type) . "'  and deleted != 1 and dataid= '$analysis_id' order by dataid");

$likepost = mysql_num_rows($result);

$row = mysql_fetch_array($result);

date_default_timezone_set('EST');

$db = \Clewed\Db::get_instance();


$companyService = new \Clewed\Company\Service();
$invitedUserIds = $companyService->getDiscussionInvitedExpertIds($analysis_id);

$isInvited = in_array($editorid,$invitedUserIds);

$isPrivate = !empty($invitedUserIds);

$editorType = getUserType($editorid);
$isAdmin = in_array($editorType, array('admin', 'super_admin'));

$isPoster = $editorid == $row['editorid'];
$isCompany = $editorid == $companyid;

$colleagueIds = $companyService->getColleagueIds($companyid);
$isColleague = in_array($editorid, $colleagueIds);

$authorType = getUserType($row['editorid']);
$authorIsAdmin = in_array($authorType, array('admin', 'super_admin'));
$authorIsCompany = $row['editorid'] == $companyid;

$isBroadcast = !$isPrivate && ($authorIsAdmin || $authorIsCompany);

if(!$isAdmin && !$isPoster && !$isBroadcast && !($isPrivate && $isInvited) && !(($isCompany || $isColleague) && !$isPrivate))
    header("Location: " . $base_url . "/account?tab=companies&page=company_detail&id=$companyid&mtab=share_knowledge");

?>
<script type="text/javascript"
        src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
<script type="text/javascript"
        src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery-ui-timepicker-addon.js"></script>
<script type="text/javascript"
        src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.autosuggest.js"></script>
<link href="<?php echo $base_url; ?>/themes/maennaco/jui/comments/css/screen.css?as" type="text/css" rel="stylesheet"/>
<link href="<?php echo $base_url; ?>/themes/maennaco/jui/comments/css/autosuggest.css" type="text/css"
      rel="stylesheet"/>
<link href="<?php echo $base_url; ?>/themes/maennaco/jui/comments/css/fileuploader.css" type="text/css"
      rel="stylesheet"/>
<link href="<?php echo $base_url; ?>/themes/maennaco/jui/comments/css/SpryTabbedPanels.css" type="text/css"
      rel="stylesheet"/>
<script src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.elastic.js" type="text/javascript"
        charset="utf-8"></script>
<script src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/fileuploader.js" type="text/javascript"
        charset="utf-8"></script>
<script src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.watermarkinput.js"
        type="text/javascript"></script>
<script src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/SpryTabbedPanels.js"
        type="text/javascript"></script>
<style type="text/css">
    .comments_box {
        display: block !important;
    }

    .text_button {
        background-color: transparent !important;
        border: medium none !important;
        color: #0fabc4 !important;
        cursor: pointer;
        font-family: 'LatoRegular' !important;
        font-size: 14px !important;
        font-style: normal !important;
    }

    .conversations_forms textarea {
        border: 1px solid #CCCCCC !important;
        margin-top: 10px;
        min-height: 26px;
        padding: 6px;
        width: 320px !important;
        /*display: block!important;*/
    }

    .defaultSkin table.mceLayout {
        border: 0;
        border-left: 1px solid #CCC;
        border-right: 1px solid #CCC;
    }

</style>
<script type="text/javascript">

    function showEditor(postId, commentId) {

        var $messageContainer = $('.project-discussion-post[data-post-id=' + postId + ']');
        if(commentId)
            $messageContainer = $('.project-discussion-comment[data-comment-id=' + commentId + ']');

        var $controls = $messageContainer.find('.comment_anchor'),
            $text = $messageContainer.find('.comment_text'),
            $editor = $messageContainer.find('.comment_editor'),
            $editorControls = $messageContainer.find('.comment_editor_controls');

        hideCommentForm(postId);

        $controls.hide();
        $text.hide();
        $editor.show();
        $editorControls.show();
        $editor.find('textarea').focus();
    }

    function hideEditor(postId, commentId) {

        var $messageContainer = $('.project-discussion-post[data-post-id=' + postId + ']');
        if(commentId)
            $messageContainer = $('.project-discussion-comment[data-comment-id=' + commentId + ']');

        var $controls = $messageContainer.find('.comment_anchor'),
            $text = $messageContainer.find('.comment_text'),
            $editor = $messageContainer.find('.comment_editor'),
            $editorControls = $messageContainer.find('.comment_editor_controls');

        $controls.show();
        $text.show();
        $editor.hide();
        $editorControls.hide();
    }

    function n2br(text) {
        return text.replace(/\r?\n/g, '<br>');
    }

    function submitEditor(postId, commentId) {

        var $messageContainer = $('.project-discussion-post[data-post-id=' + postId + ']');
        if(commentId)
            $messageContainer = $('.project-discussion-comment[data-comment-id=' + commentId + ']');

        var $message = $messageContainer.find('.comment_text'),
            text = $messageContainer.find('textarea').val(),
            m = $messageContainer.attr('m'),
            u = $messageContainer.attr('u'),
            t = $messageContainer.attr('t'),
            $submitButton = $messageContainer.find('.comment_editor_controls a.save-post');

        $submitButton.attr("disabled", "disabled");

        if (text == '') {
            alert('Please type your comment.');
            return false;
        }

        $.post("/themes/maennaco/includes/add_comment.php?type=edit_project_discussion_post", {
            id: commentId ? commentId : postId,
            ctype: commentId ? 'comment' : 'post',
            text: text,
            u: u,
            t: t,
            m: m

        }, function (response) {

            if (response.status == 'success') {
                $submitButton.removeAttr('disabled');
                $message.html(n2br(text));
                hideEditor(postId, commentId);
            }
            else {
                alert("Please refresh the page and try again!");
            }

        }, "json");
    }

    function showCommentForm(postId) {
        var $form = $('#question');
        if(postId)
            $form = $('#form_id' + postId);

        $form.show();
        $form.find('textarea').focus();
    }

    function hideCommentForm(postId) {
        if(!postId)
            return $('#question').hide();

        $('#form_id' + postId).hide();
    }

    function submitCommentForm(postId) {

        var $container,
            $submitButton,
            text = "",
            dissid = 0,
            m = "",
            editor = '<?=$user_id;?>',
            pro_profile = '<?=$_REQUEST['id'];?>';

        if(!postId) {
            $container = $('#question');
            text = $container.find("textarea").val();
            $submitButton = $container.find('a.save-post');
            if (text.trim().length > 0) {
                dissid = $submitButton.attr('dissid');
                m = $submitButton.attr('m');

                $submitButton.attr("onclick", "return false;");
                $.post("./themes/maennaco/includes/add_comment.php?type=project_discussion_comment", {
                    dissid: dissid, text: text, editor: editor, m: m, pro_profile: pro_profile
                }, function (response) {

                    if (response.status == 'success') {
                        hideCommentForm();
                        $(".comts").after(response.display).show();
                        $submitButton.attr("onclick", "submitCommentForm()");
                        $container.find("textarea").val('');
                    }
                    else
                        return alert("Please refresh the page and try again!");

                }, "json");
            }
            else return alert('Please type your comment!');
        }
        else {
            $container = $('.project-discussion-post[data-post-id=' + postId + ']').parent().find('.post-submit-container');
            text = $container.find("textarea").val();
            $submitButton = $container.find('a.save-comment');
            if (text.trim().length > 0) {
                m = $submitButton.attr('m');
                $submitButton.attr("onclick", "return false;");
                $.post("./themes/maennaco/includes/add_comment.php?type=project_discussion_comment_reply", {
                    post_id: postId, text: text, editor: editor, m: m, pro_profile: pro_profile
                }, function (response) {

                    if (response.status == 'success') {
                        hideCommentForm(postId);
                        $container.parent().find('.askright').append(response.display).show();
                        $submitButton.attr("onclick", "submitCommentForm(" + postId + ");");
                        $container.find("textarea").val('');
                    }
                    else
                        return alert("Please refresh the page and try again!");

                }, "json");
            }
            else return alert('Please type your comment!');
        }
    }

    $('.delete_comment').live("click", function (e) {

        if (confirm('Are you sure you want to delete this Post?') == false)
            return false;
        e.preventDefault();
        var temp = $(this).data('id'),
            u = $(this).attr('u'),
            m = $(this).attr('m');
        $.ajax({
            type: 'get',
            url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
            'type=analysis_comments&' +
            'id=' + temp + "&" +
            'u=' + u + "&" +
            'm=' + m,
            data: '',
            beforeSend: function () {
            },
            success: function (response) {
                if('error' != response)
                    $('#aucomnts' + temp).fadeOut(200, function(){
                        $(this).remove();
                    });
                else
                    alert('Please refresh the page and try again!');
            }
        });
        return true;
    });

    $('.delete_post').live("click", function (e) {

        if (confirm('Are you sure you want to delete this post?') == false)
            return false;
        e.preventDefault();
        var post_id = $(this).data('id');
        <?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>
        $.ajax({
            type: 'get',
            url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
            'type=analysis_posts&id=' + post_id + '&' +
            'u=<?php echo $u; ?>&' +
            'm=<?php echo $m; ?>',
            data: '',
            beforeSend: function () {
            },
            success: function (response) {
                if (response != 'success') {
                    alert('Action did not succeed.Please try again.');
                }
                else {
                    to_remove = "dis_post" + post_id;
                    $("#" + to_remove).fadeOut(200, function(){
                        $(this).remove();
                    });

                }
            }
        });
        return true;
    });

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
<style type="text/css">
    div.content_box .box_title {
        margin-top: 14px;
    }

    /*.text_button { border: none !important; background-color: transparent!important; color:#8f9094!important; text-decoration: underline!important; /* if desired
       color: #000000; cursor: pointer; font-family: 'LatoRegular'!important; font-size: 14px!important; font-weight: bold!important;
        font-style:normal !important; }*/

    div.content_box .box_content {
        /*padding-bottom:15px;*/
        padding-top: 2px;
        font-size: 14px;
        text-align: left;
        font-family: 'LatoRegular';
        float: left !important;
    }

    div.content_box {
        padding-left: 10px;
        position: relative;
        width: 100%;
        padding-bottom: 10px;
        float: left !important;
    }
</style>
<?php

$usertype = $AccessObj->user_type;
$userService = new \Clewed\User\Service();

function nameToId($name)
{
    $q = mysql_query("SELECT uid FROM users WHERE name = '" . mysql_real_escape_string($name) . "' LIMIT 1") or die(mysql_error());
    $r = mysql_fetch_array($q);
    return $r['uid'];
}

function getUserType($uid)
{
    $q = mysql_query("SELECT pid FROM maenna_people WHERE pid = '" . ((int) $uid) . "' ");
    if (mysql_num_rows($q) > 0) return 'people';
    else {
        $q1 = mysql_query("SELECT companyid FROM maenna_company WHERE companyid = '" . ((int) $uid) . "' ");
        if (mysql_num_rows($q1) > 0) return 'company';
        else return 'admin';
    }
}

?>
<div id="pro_popup" class="hidden"></div>
<div class="tabtags" style="padding: 0 0 0 10px; width:610px;">
    <div class="rt-title" style="margin-right: 0px;"><?php echo  substr(strtoupper($row['data_value']),0,55); ?></div>
    <?php

    $P_username = getUserById($row['editorid']);
    $result3 = mysql_query("SELECT * FROM  `like_analysis_posts` WHERE post_id = '" . $row['dataid'] . "' and user_id = '" . $user->uid . "'");
    $row3 = mysql_fetch_array($result3);
    $likepost1 = mysql_num_rows($result3);

    $show_like_unlike_sql = mysql_query("SELECT * FROM  `like_analysis` WHERE prof_id = '" . $row['dataid'] . "' and user_id = '" . $user->uid . "'");

    $show_like_unlike_rows = mysql_fetch_array($show_like_unlike_sql);
    $show_like_unlike_count = mysql_num_rows($show_like_unlike_sql);

    $count_result3 = mysql_query("SELECT * FROM  `like_analysis` WHERE prof_id = '" . $row['dataid'] . "'");
    $count_row3 = mysql_fetch_array($count_result3);
    $count_likepost1 = mysql_num_rows($count_result3);
    if ($row['status'] == 0) {
        $savedby = 'Saved by';
    } else {
        $savedby = 'Published by';
    }

    foreach ($invitedUserIds as $uid) {
        $userType = getUserTypeById($uid);
        $uname = getProId($uid);
        if ('company' == $userType) {
            $proId = getProjectName($uid);
            $invitedExperts[] = "<a class=\"tool\" target=\"_blank\" href=\"/account?tab=companies&page=company_detail&id=" . $uid . "&mtab=about\"><b>" . $uname . "</b></a>";
        } else
            $invitedExperts[] = "<a class=\"tool\" onclick=\"showExpertInfo({$uid});\"><b>" . $uname . "</b></a>";
    }

    if (!empty($invitedExperts)) {
        $invitedLabel = "<div style=\"margin-top:8px;margin-bottom: 20px;\"><span class='scope_invited' style='padding:0px !important;'>Invited: ".implode(', ',$invitedExperts)."</span></div>";
    }

    else $invitedLabel = '';
    ?>

    <div style="margin-top: -7px; " class="rt-date">
        <?php echo $savedby; ?> <strong><?php echo $P_username; ?></strong>
        on <span class="rt-date" style="padding:0 !important;"><?= date("M j, Y ", $row['access']); ?></span>
    </div>
    <div class="analysis_details_post">
        <?php echo _filter_autop(utf8_decode($row['data_value2'])); ?>
    </div>
    <?=$invitedLabel;?>
    <div class='comment_anchor' style="width:610px;float:left;margin-top:0px;margin-bottom:10px;padding: 0;">
        <?php if ($opCom == 'write' || (($opCom == 'write_own' || $opCom == 'read_all_write_own') && ($isColleague || $isInvited || $editorid == $row['editorid'] || $companyid == $row['editorid'] || $authorIsAdmin)) || ($opCom == 'edit' && $editorid == $row['editorid'])):?>
            <?php $posterType = getUserType($row['editorid'])?>
            <a onclick='showCommentForm();'
               data-tooltip="Discussing pricing or sharing personal contact creates various business challenges and is prohibited"
            >Reply</a>
        <?php endif;?>

        <?php $uType = getUserType($user->cid);
        if ($op == 'write' || (($op == 'write_own' || $op == 'read_all_write_own') && $editorid == $row['editorid']) || ($op == 'edit' && $editorid == $row['editorid'])) { ?>
            |&nbsp;<a href="<?php echo $base_url; ?>/account?tab=companies&page=company_detail&mtab=share_knowledge&id=<?php echo $companyid; ?>&section=maenna_share_knowledge&panel=multi&view=edit&dataid=<?php echo $row['dataid']; ?>">Edit</a>
            |&nbsp;<a href="<?php echo $base_url; ?>/account?tab=companies&page=company_detail&mtab=share_knowledge&id=<?php echo $companyid; ?>&section=maenna_share_knowledge&panel=multi&view=edit&do=remove&dataid=<?php echo $row['dataid']; ?>">Delete</a>
        <?php } ?>
        <?php if ($row['tags']):?>
            <span style="float:right">Category: <?php echo $row['tags']; ?></span>
        <?php endif; ?>
    </div>
    <br/>

    <div class="comts" style="margin:0;">
        <div id="question" class="conversations_forms" style="display:none;">
            <form method="post" id="comments">
                <input type="hidden" name="prof_id" id="prof_id" value="<?php echo $row['dataid']; ?>"/>
                <input type="hidden" name="flag" id="flag" value="q"/>
                <div class="w" style="padding: 0!important;">
                    <textarea
                        style="display:inline; width:98%!important; margin-left: 14px; height: 100px;padding:0 5px;line-height: 15px!important;"
                        name="dis_posts"
                        id="dis_posts_question"
                        placeholder="Start a discussion"
                        class="mceNoEditor"></textarea>
                    <div class="controls" style="float:right;padding:0!important;">
                        <a style="cursor: pointer; margin: 0;padding: 0;"
                           class="save-post"
                           name="dis_post"
                           id="question_post"
                           dissid="<?= $row['dataid']; ?>"
                           onclick="submitCommentForm();"
                           m="<?= md5($row['dataid'] . "kyarata75"); ?>">Submit</a>
                        |&nbsp;<a style="cursor: pointer;margin-left: 3px;padding: 0;"
                                  onclick="hideCommentForm();">Cancel</a>
                    </div>
                </div>
            </form>
        </div>
        <div style="clear:both"></div>
    </div>
    <?php

    if ($opCom != 'hide') {

        $result = mysql_query("SELECT *,UNIX_TIMESTAMP() - date_created AS TimeSpent FROM pro_analysis_posts where pro_id = " . $row['dataid'] . " order by pid desc");

        $postUserIds = array();
        while ($row1 = mysql_fetch_array($result))
            $postUserIds[$row1['user']] = $row1['user'];

        $postUsers = !empty($postUserIds) ? $userService->get(array_keys($postUserIds)) : array();
        mysql_data_seek($result, 0);

        while ($row1 = mysql_fetch_array($result)) {
            $crId = $row1['user'];
            $uid = $crId;
            $uType = getUserType($crId);

            $avatar = $postUsers[$crId]['avatar'];

            ?>
            <div class="cmtloop" id="dis_post<?php echo $row1['pid']; ?>">
                <div class="ask" style="margin-left:14px;">
                    <div class="project-discussion-post" u="<?= $user_id;?>" t="<?= $time = time();?>" m="<?= md5($user_id . ':' . $row1['pid'] . ':' . $time .":kyarata75");?>" data-post-id="<?= $row1['pid'];?>">
                        <div class="askpic"
                             style="width: 35px;"> <?php echo "<img src='" . $avatar . "' style=\"float:left; margin-top:13px; margin-right:5px; width:35px; height:35px;\">&nbsp;"; ?> </div>
                        <div class="asktitle"><?php echo $row1['f_name']; ?></div>
                        <p class="comment_text" style="padding: 0 0 0px 49px; ">
                            <?= n2br(htmlspecialchars($row1['post'])) ?>
                        </p>

                        <div class="w comment_editor hidden" style="margin-left:0!important;">
                             <textarea
                                 style="line-height:15px!important;width:90%; margin:-20px 0 0 49px!important; height:100px; "
                                 class="input mceNoEditor"><?php echo htmlspecialchars($row1['post']);?></textarea>
                        </div>
                        <div class="comment_editor_controls hidden" style="float:right;margin: 2px 14px 10px;">
                            <a class="save-post" style="cursor: pointer;padding-right: 0;" onclick="submitEditor(<?= $row1['pid'];?>);">Submit</a>&nbsp;|&nbsp;
                            <a style="cursor: pointer" onclick="hideEditor(<?= $row1['pid'];?>);">Cancel</a>
                        </div>

                        <div style="margin:0px 0px -12px 49px; padding:0px 0px 2px 0px;">
                            <div style="height:30px; width: 535px;">
                                <div class='comment_anchor' style="width:350px;float:left;margin-top:-5px;">
                                    <div style='float:left;padding:0;font-size:12px;'>
                                        <?= date("D, M j, Y g:i A T ", $row1['date_created']); ?>
                                        &nbsp;|&nbsp;
                                    </div>
                                    <?php if ($opCom == 'write' || $opCom == 'write_own' || $opCom == 'read_all_write_own' || ($opCom == 'edit' && $editorid == $row['editorid'])):?>
                                        <a onclick='showCommentForm("<?= $row1['pid'] ?>");' data-tooltip="Discussing pricing or sharing personal contact creates various business challenges and is prohibited">Reply</a>
                                    <?php endif; ?>
                                    <?php if ($opCom == 'write' || (($opCom == 'write_own' || $opCom == 'read_all_write_own') && $editorid == $crId) || ($opCom == 'edit' && $editorid == $row1['user'])):?>
                                        &nbsp;|&nbsp;<a style="cursor:pointer;"
                                                        onclick="showEditor(<?= $row1['pid'] ?>);">Edit</a>
                                        &nbsp;|&nbsp;<a style="cursor:pointer;"
                                                        data-id="<?= $row1['pid'] ?>"
                                                        class="delete_post">Delete</a>
                                    <?php endif; ?>
                                </div>
                            </div>
                        </div>
                    </div>

                    <div class="askright" style="margin-top:0; width: 95%!important;">

                        <?php
                        $result2 = db_query("SELECT *, UNIX_TIMESTAMP() - datecreated AS TimeSpent FROM analysis_wall_post_comments where post_id=" . $row1['pid'] . " order by cid asc");
                        $comments_count = 0;

                        $commentUserIds = array();
                        while ($row2 = mysql_fetch_array($result2))
                            $commentUserIds[$row2['user_id']] = $row2['user_id'];

                        $commentUsers = !empty($commentUserIds) ? $userService->get(array_keys($commentUserIds)) : array();
                        mysql_data_seek($result2, 0);

                        while ($row2 = db_fetch_array($result2)) { ?>
                            <?php
                            $comments_count++;
                            $crId = ($row2['user_id']);
                            $uType = getUserType($crId);

                            $avatar = $commentUsers[$crId]['avatar'];
                            $comment_result = mysql_query("SELECT * FROM  `analysis_comments` WHERE  comment_id = '" . $row2['cid'] . "' and post_id ='" . $row1['pid'] . "'  and user_id = '" . $user->uid . "'");
                            $likepost_comment = mysql_num_rows($comment_result);
                            ?>
                            <div class="aucomnts project-discussion-comment"
                                 data-comment-id="<?= $row2['cid']?>"
                                 id="aucomnts<?php echo $row2['cid']; ?>"
                                 u="<?= $user_id;?>"
                                 t="<?= $time = time();?>"
                                 m="<?= md5($user_id . ':' . $row2['cid'] . ':' . $time .":kyarata75");?>"
                                 style="margin-left: 49px!important;">
                                <div
                                    class="aucpic"> <?php echo "<img src=" . $avatar . " style=\"float:left; margin-top:13px; margin-right:5px; width:45px; height:45px;\">&nbsp;"; ?>
                                </div>
                                <div class="aucdisc" style="width: 91% !important;">
                                    <h5 style="float: none !important; margin-top: 0;color:#2db6de!important">
                                        <?php echo ($row2['username'] != '') ? $row2['username'] : 'User_' . $row2['user_id']; ?>
                                    </h5>

                                    <p class="comment_text" style="padding: 0 !important; width: 95%;line-height: 14px;"
                                       id="com<?php echo $row2['cid']; ?>"><?php echo n2br(htmlspecialchars($row2['comment'])); ?></p>

                                    <div class="w comment_editor hidden" style="margin-left:0!important;">
                                         <textarea
                                             style="line-height:15px!important;width:92%; margin:0!important; height:100px; "
                                             class="input mceNoEditor"><?php echo htmlspecialchars($row2['comment']);?></textarea>
                                    </div>
                                    <div class="comment_editor_controls hidden" style="float:right;margin: 2px 14px 10px;">
                                        <a class="save-post" style="cursor: pointer;padding-right: 0;" onclick="submitEditor(<?= $row1['pid'];?>,<?= $row2['cid'] ?>);">Submit</a>&nbsp;|&nbsp;
                                        <a style="cursor: pointer" onclick="hideEditor(<?= $row1['pid'];?>,<?= $row2['cid'] ?>);">Cancel</a>
                                    </div>

                                    <div class='comment_anchor' style="margin-top:-4px;margin-bottom: 10px;">
                                        <div style='float:left;padding:0;font-size:12px;'>
                                            <?= date("D, M j, Y g:i A T ", $row2['datecreated']); ?>
                                            &nbsp;|&nbsp;
                                        </div>
                                        <?php if ($opCom == 'write' || $opCom == 'write_own' || $opCom == 'read_all_write_own' || ($opCom == 'edit' && $editorid == $row['editorid'])):?>
                                            <a onclick='showCommentForm("<?= $row1['pid'] ?>");' data-tooltip="Discussing pricing or sharing personal contact creates various business challenges and is prohibited">Reply</a>
                                        <?php endif; ?>
                                        <?php if ($opCom == 'write' || (($opCom == 'write_own' || $opCom == 'read_all_write_own') && $editorid == $crId) || ($opCom == 'edit' && $editorid == $row1['user'])):?>
                                            &nbsp;|&nbsp;<a style="cursor:pointer;"
                                                            onclick="showEditor(<?= $row1['pid'] ?>,<?= $row2['cid'];?>);">Edit</a>
                                            &nbsp;|&nbsp;<a style="cursor:pointer;"
                                                            data-id="<?= $row2['cid'] ?>"
                                                            u="<?= $u = time();?>"
                                                            m="<?= md5('delete.php:' . $row2['cid'] . ':' . $u . ':kyarata75');?>"
                                                            class="delete_comment">Delete</a>
                                        <?php endif; ?>
                                    </div>
                                </div>
                            </div>
                            <div style="clear:both"></div>

                        <?php } ?>
                    </div>
                    <?php if ($opCom == 'write' || $opCom == 'write_own'  || $opCom == 'read_all_write_own' || ($opCom == 'edit' && $editorid == $row['editorid'])) { ?>
                        <div class="w post-submit-container" style="float:left; display:none;margin:0px 10px 0px 49px; padding:21px 0px 0 0px;background:#f4f8fa !important;" id="form_id<?php echo $row1['pid']; ?>">
                            <form method="post" action="" id="comments" style="position: inherit;width:536px;">
                                <textarea name="post_comment"
                                          class="comments_box mceNoEditor"
                                          id="post_comment<?php echo $row1['pid']; ?>"
                                          style="line-height:18px!important;width:83%; margin: 20px 0px 0px 55px!important; height:100px;"></textarea>
                                <a class="save-comment"
                                   style="margin:7px 25px 20px;float:right;cursor: pointer;"
                                   id="post_com<?php echo $row1['pid']; ?>"
                                   dissid="<?= $row1['pid'] ?>"
                                   onclick="submitCommentForm(<?php echo $row1['pid']; ?>)"
                                   m="<?= md5($row1['pid'] . "kyarata75"); ?>">Submit</a>
                            </form>
                            <div style="clear:both"></div>

                        </div> <?php } ?>
                    <div style="clear:both"></div>
                </div>
            </div>
            <div id="paa" style="clear:both"></div>

        <?php } ?>
    <?php } ?>
</div>