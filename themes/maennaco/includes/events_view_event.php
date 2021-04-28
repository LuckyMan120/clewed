<?php
error_reporting(0);
global $base_url;
global $user;
$user_id = $user->uid;
include('dbcon.php');
// find out the domain:
$domain = $_SERVER['HTTP_HOST'];
// find out the path to the current file:
$path = $_SERVER['SCRIPT_NAME'];
// find out the QueryString:
$queryString = $_SERVER['QUERY_STRING'];
// put it all together:
$paymenturl1 = "http://" . $domain . $path . "?" . $queryString;
//echo "The current URL is: " . $url . "";

// An alternative way is to use REQUEST_URI instead of both
// SCRIPT_NAME and QUERY_STRING, if you don't need them seperate:
$paymentur2 = "http://" . $domain . $_SERVER['REQUEST_URI'];
//echo "The alternative way: " . $url2;
$id = $_REQUEST['id'];

function n2br($text) {
    return preg_replace("|\n|", "<br>", $text);
}

$uname = $user->name;

$user_id = $user->uid;

if ($_POST['post_comment'] != '') {
    mysql_query("INSERT INTO pro_wall_posts_comments (d_id,post_id,user_id,comment,username,datecreated) VALUES('" . $_REQUEST['dis_id'] . "','" . $_REQUEST['post_id'] . "','" . $user_id . "','" . checkValues($_REQUEST['post_comment']) . "','" . $uname . "','" . strtotime(date("Y-m-d H:i:s")) . "')");
    $ins_id = mysql_insert_id();
    $url = $base_url . "/account?tab=professionals&page=pro_detail&id=" . $id . "&section=pro_industry_view&type=details&tabs=2&pro_id=" . $_REQUEST['dis_id'] . "#com" . $ins_id . "";
    header("location:" . $url . "");
    exit;
}

if ($_POST['dis_posts'] != '' && !empty($_GET['editMinutes'])) {

    mysql_query("UPDATE pro_wall_posts SET post = '" . checkValues($_REQUEST['dis_posts'])."' WHERE pid = ".$_GET['editMinutes']);
    $ins_id = $_GET['editMinutes'];
    $url = $base_url . "/account?tab=companies&page=company_detail&id=" . $id . "&mtab=advice&tabs=2&view_id=" . $_REQUEST['view_id'] . "&post_id=" . $ins_id . "#dis_post" . $ins_id . "";
    header("location:" . $url . "");
    exit;
}

if ($_POST['dis_posts'] != '' && !empty($_GET['editComments'])) {

    mysql_query("UPDATE pro_wall_posts_comments SET comment = '" . checkValues($_REQUEST['dis_posts'])."' WHERE cid = ".$_GET['editComments']);
    $ins_id = $_GET['editComments'];
    $url = $base_url . "/account?tab=companies&page=company_detail&id=" . $id . "&mtab=advice&tabs=2&view_id=" . $_REQUEST['view_id'] . "#com" . $ins_id . "";
    header("location:" . $url . "");
    exit;
}

if ($_POST['dis_posts'] != '') {
    global $user;
    mysql_query("INSERT INTO pro_wall_posts (pro_id,post,user,tags,flag,date_created) VALUES('" . $_REQUEST['view_id'] . "','" . checkValues($_REQUEST['dis_posts']) . "','" . $user->uid . "','".$_POST['tags']."','".$_POST['flag']."','" . strtotime(date("Y-m-d H:i:s")) . "')");
    $ins_id = mysql_insert_id();
    $url = $base_url . "/account?tab=companies&page=company_detail&id=" . $id . "&mtab=advice&tabs=2&view_id=" . $_REQUEST['view_id'] . "&post_id=" . $ins_id . "#dis_post" . $ins_id . "";
    header("location:" . $url . "");
    exit;
}

if ($_POST['deleteMinutes'] != '') {
    mysql_query("DELETE FROM pro_wall_posts WHERE pid = ".$_POST['deleteMinutes']);
    mysql_query ("DELETE FROM pro_wall_posts_comments WHERE post_id = ".$_POST['deleteMinutes']);
    // $ins_id = mysql_insert_id();
    // $url = $base_url . "/account?tab=companies&page=company_detail&id=" . $id . "&mtab=advice&tabs=2&view_id=" . $_REQUEST['view_id'];
    // header("location:" . $url . "");
    print 'OK';
    exit;
}

if ($_POST['payment_status'] == "Completed") {
    mysql_query("INSERT INTO maenna_professional_payments (pro_id,transaction_id,user_id,amount,status,date_created) VALUES('" . $_REQUEST['pro_id'] . "','" . $_POST['txn_id'] . "','" . $user_id . "','" . $_POST['mc_gross'] . "','1','" . strtotime(date("Y-m-d H:i:s")) . "')");

    $insightId = (int) $_REQUEST['pro_id'];
    $userId = (int) $user_id;
    if(!empty($insightId) && !empty($userId)) {
        $offerRepository = new \Clewed\Insights\InsightRepository();
        $offer = $offerRepository->findById($insightId);
        if(!empty($offer) && $offer->type == \Clewed\Insights\InsightEntity::TYPE_GROUP_INSIGHT) {
            $notificationService = new \Clewed\Notifications\NotificationService();
            $notificationService->registerEvent(
                'insight_review_requested',
                $insightId,
                $userId
            );
        }
    }

    header("location:" . $paymenturl1 . "");
    exit;
}


function checkValues($value) {
    // Use this function on all those values where you want to check for both sql injection and cross site scripting
    //Trim the value
    $value = trim($value);
    // Stripslashes
    if (get_magic_quotes_gpc()) {
        $value = stripslashes($value);
    }
    // Convert all &lt;, &gt; etc. to normal html and then strip these
    $value = strtr($value, array_flip(get_html_translation_table(HTML_ENTITIES)));
    // Strip HTML Tags
    $value = strip_tags($value);
    // Quote the value
    $value = mysql_real_escape_string($value);
	// '&' (ampersand) becomes '&amp;' in PHP 5.4<
    //$value = htmlspecialchars($value);
    return $value;
}

date_default_timezone_set('EST');
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

<script type="text/javascript">

<?php if($_REQUEST['tabs'] == 2) { ?>
$(document).ready(function () {
    $('#tab2').trigger('click');
});
<?php } ?>

<?php if($_REQUEST['rsvp'] == 1) { ?>
$(document).ready(function () {
    openRSVPDialog();
});
<?php } ?>

function openRSVPDialog() {

    $("#discussion_popup").dialog({
        autoOpen: true,
        width: 400,
        title: 'CONFIRMATION REQUEST',
//					height:200,
        buttons: {
            "Save": function () {
                status = $('input[name=attending]:checked', '#rsvp-form').val();
                uid = '<?=$user->uid;?>';
                eventid = <?=$_REQUEST['view_id']?>;
                $.post("/themes/maennaco/includes/posts.php?type=confirmAtt&status=" + status + "&uid=" + uid + "&eventid=" + eventid, {

                }, function (response) {

                    if (response.trim() == 'overlap') {
                        alert("You are already attending event at that time!");
                        return;
                    }

                    var $popup = $("#discussion_popup");
                    $popup.find('.result').html(response);
                    $popup.find('.result').show();
                    $popup.closest('.ui-dialog').find('.ui-dialog-buttonpane').hide();
                    setTimeout(function(){
                        return window.location = window.location.toString().replace(/&rsvp=1/, "");
                    }, 1000);
                });
            },
            "Cancel": function () {
                $(this).dialog("close");
            }
        }, closeOnEscape: true,
        modal: true
    }).show();
}

$("#dialog").dialog({
    autoOpen: false
});

$("#join").livequery("click", function (e) {

    e.preventDefault();

    //var createaccount = $("#createaccount").show();

    $("#createaccount").dialog({
        autoOpen: true,
        width: 450,
        title: 'CREATE AN ACCOUNT',
        height: 300,
        closeOnEscape: true,
        modal: true
    }).html();

});

function n2br(text) {
    return text.replace(/\r?\n/g, '<br>');
}

$("#question_post").livequery("click", function (event) {

    event.preventDefault();
    text = $("#dis_posts_question").val();
    dissid = $(this).attr('dissid');
    editor = '<?=$user_id;?>';
    pro_profile = '<?=$_REQUEST['id'];?>';
    m = $(this).attr('m');
    tags = $("#tags_question :selected").val();
    sel_obj = $(this);

    if ($('#dis_posts_question').val() == '') {
        alert('Please type your question.');
        return false;
    } else if ($('#tags_question').val() == '') {
        alert('Categories field is required');
        return false;
    }

    sel_obj.attr("disabled", "disabled");

    $.post("/themes/maennaco/includes/add_comment.php?type=pro_dis_comment", {dissid: dissid, text: text, editor: editor, m: m, tags: tags, pro_profile: pro_profile

    }, function (response) {

        if (response.status == 'success') {

            $(".comts").after(response.display).show();
            sel_obj.removeAttr("disabled");
            $("#dis_posts_question").val('');
            $("#tags_question :selected").removeAttr("selected");
            $('#question').hide();

        }
        else {
            (alert("Your request din`t go through. Please try again!"));
        }

    }, "json");

});

$('input[id^="post_com"]').livequery("click", function (event) {

    event.preventDefault();
    post_id = $(this).attr('id').replace("post_com", "");
    text = $(this).siblings('textarea[name="post_comment"]').val();
    editor = '<?=$user_id;?>';
    pro_profile = '<?=$_REQUEST['id'];?>';
    m = $(this).attr('m');
    sel_obj = $(this);

    if (text == '') {
        alert('Please type your comment.');
        return false;
    }

    sel_obj.attr("disabled", "disabled");

    $.post("/themes/maennaco/includes/add_comment.php?type=company_service_post_reply", {post_id: post_id, text: text, editor: editor, m: m, pro_profile: pro_profile

    }, function (response) {

        if (response.status == 'success') {

            sel_obj.parent().parent().before(response.display);
            sel_obj.removeAttr("disabled");
            sel_obj.siblings('textarea[name="post_comment"]').val('');
            formHide(post_id);

        }
        else {
            (alert("Your request din`t go through. Please try again!"));
        }

    }, "json");

});

$("#showFile").livequery("click", function () {

    pathArray = window.location.href.split('/');
    host = pathArray[2];
    sel_obj = $(this);

    proba = '<form action="" method="post" name="postsForm"><div class="UIComposer_Box"><span class="w"><textarea class="input" id="watermark" name="watermark" style="height:30px" cols="64"></textarea></span><br clear="all" /><div id="submitDiv" align="right" style="display:none; height:80px; "><a id="closeButton" class="tool">Close</a><a  </div><div style="clear: both;"></div></div></form>';

    sel_obj.parent().after('<iframe src="https://docs.google.com/viewer?url=http://' + host + '/sites/default/files/events_tmp/' + sel_obj.attr('file') + '&embedded=true" width="600" height="670" style="border: none;"></iframe>' + proba);

});

$('[id^=shareButton]').livequery("click", function () {

    tarea = $('textarea[proid=' + $(this).attr('textref') + ']');
    var a = encodeURIComponent(tarea.val());
    var m = tarea.attr("alt");
    var u = tarea.attr("name");
    var prof_id = $("#pro_id").val();
    var uid = '<?=$user->uid;?>';
    var proid = tarea.attr('proid');

    if (a != "Discuss a topic or ask a question on this file ...") {

        $.post("<?php echo $base_url; ?>/themes/maennaco/includes/pro_comments_list.php?type=pro_comments&prof_id=" + prof_id + "&u=" + u + "&m=" + m + "&value=" + a + "&proid=" + proid + "&uid=" + uid, {

        }, function (response) {

            tarea.parent().parent().parent().parent().after($(response).show());
            tarea.val("Share ideas on this topic");

            $('textarea').elastic();
            $(".commentMark").Watermark("Got advice / question on this topic?");
            tarea.Watermark("watermark", "#369");

            $(".commentMark").Watermark("watermark", "#EEEEEE");

        });
    }
});
$('textarea').elastic();

function to_join(ownInsight, oldInsight) {

    if (ownInsight === true) {
        alert('You can not join your own Insight');
        return false;
    }

    if (oldInsight === true) {
        alert('This Insight was in the past. Please join upcoming insights.');
        return false;
    }

    $.post("/themes/maennaco/includes/fetch.php?type=checkCompleted", {uid: <?=$user->uid;?>, utype: '<?=$AccessObj->user_type;?>'}, function (response) {

        if (response == 'true') {
            $("#payment").dialog({
                autoOpen: true,
                width: 450,
                title: 'PROCEED WITH PAYMENT',
                height: 300,
                buttons: {
                    "Cancel": function () {
                        $(this).dialog("close");
                    }
                }, closeOnEscape: true,
                modal: true
            }).show;
        }

        else {
            alert(response);
            return false;
        }

    });

}

function showsubmit(id) {

    $('#post_com' + id).show();

}
function submitcomment(id) {
    $.post("<?php echo $base_url; ?>/themes/maennaco/includes/pro_comments_list.php?type=pro_comments&prof_id=" + prof_id + "&u=" + u + "&m=" + m + "&value=" + a + "&proid=" + proid + "&uid=" + uid, {

    }, function (response) {

    });
}

<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

$('.deletepost').livequery("click", function (e) {

    if (confirm('Are you sure you want to delete this Post?') == false) {
        return false;
    }

    e.preventDefault();

    var temp = $(this).attr('id').replace('deletepost', '');

    $.ajax({

        type: 'get',

        url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
            'type=professional_post&' +
            'id=' + temp+ "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",

        data: '',

        beforeSend: function () {

        },

        success: function () {
            $('#dis_post' + temp).fadeOut(200, function() {
                $(this).remove();
            });
        }

    });
    return true;

});

$('.delete_comment').livequery("click", function (e) {

    if (confirm('Are you sure you want to delete this Post?') == false) {
        return false;
    }

    e.preventDefault();

    var clicked = $(this);

    var temp = $(this).attr('id').replace('delete_comment', ''),
        u = $(this).attr('u'),
        m = $(this).attr('m');

    $.ajax({

        type: 'get',

        url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
            'type=project-service-comment&' +
            'id=' + temp + "&" +
            "u=" + u + "&" +
            "m=" + m,

        data: '',

        beforeSend: function () {

        },
        success: function () {

            //location.reload();
            clicked.parent().parent().parent().fadeOut(200,function(){

                clicked.parent().parent().parent().remove();

             });

        }

    });
    return true;

});

<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

function like_discussion(type, prof_id, userid) {

    if (type == 'like') {
        var status = 1;
    }
    else {
        var status = 0;
    }

    $.ajax({

        type: 'get',

        url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
            'type=like_discussion&' +
            'prof_id=' + prof_id + '&' +
            'userid=' + userid + '&' +
            'status=' + status + "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",

        data: '',

        beforeSend: function () {

        },

        success: function () {

            if (type == 'like') {
                $('#likepost').html("<a style='cursor:pointer;' onclick='like_discussion(\"unlike\", " + prof_id + "," + userid + ");'>Unlike</a>");
            }
            else {
                $('#likepost').html("<a style='cursor:pointer;' onclick='like_discussion(\"like\", " + prof_id + "," + userid + ");'>Like</a>");
            }

        }

    });
}

<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

function like_posts(type, prof_id, post_id, userid) {
    if (type == 'like') {
        var status = 1;
    }
    else {
        var status = 0;
    }

    $.ajax({

        type: 'get',

        url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
            'type=like_posts&' +
            'prof_id=' + prof_id + '&' +
            'post_id=' + post_id + '&' +
            'userid=' + userid + '&' +
            'status=' + status + "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",

        data: '',

        beforeSend: function () {

        },

        success: function () {

            if (type == 'like') {
                $('#likepost1' + post_id).html("<a style='cursor:pointer;' onclick='like_posts(\"unlike\", " + prof_id + ", " + post_id + "," + userid + ");'>Unlike</a>");
                //location.reload();
            }
            else {
                $('#likepost1' + post_id).html("<a style='cursor:pointer;' onclick='like_posts(\"like\", " + prof_id + ", " + post_id + "," + userid + ");'>Like</a>");
                //location.reload();
            }

        }

    });
}

function joinnow2(pro_id, id, ownInsight, oldInsight) {

    if (ownInsight === true) {
        alert('You can not join your own Insight');
        return false;
    }

    if (oldInsight === true) {
        alert('This Insight was in the past. Please join upcoming insights.');
        return false;
    }

    $.post("/themes/maennaco/includes/fetch.php?type=checkCompleted", {uid: <?=$user->uid;?>, utype: '<?=$AccessObj->user_type;?>'}, function (response) {

        if (response == 'true')

            <?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

            $.post("<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?"+
                "type=join&" +
                "prof_id=" + pro_id + "&" +
                "uid=" + id + "&" +
                "u=<?php echo $u; ?>&" +
                "m=<?php echo $m; ?>",
                {},
                function (response) {
                    location.reload();
                });

        else {
            alert(response);
            return false;
        }

    });

}

function validate_minutes() {

    var editorContent = tinyMCE.get('dis_posts').getContent();
    // var editorContent = $('#dis_posts').val();
    if (editorContent == '' || editorContent == null || editorContent == '<div>&nbsp;</div>') {
      alert('Content field is required');
      return false;
    }
    else if ($('#tags').val() == '') {
        alert('Categories field is required');
        return false;
    }
}

function validate_questions() {
    if ($('#dis_posts_question').val() == '') {
        alert('Please type your question.');
        return false;
    } else if ($('#tags_question').val() == '') {
        alert('Categories field is required');
        return false;
    }
}
var old_formaction = '';

function show_minutes() {
    $('#question').hide();
    $('#minutes').show();
    if (old_formaction == '') {
      old_formaction = $('#minutes form#comments').attr('action');
    } else {
      $('#minutes form#comments').attr('action', old_formaction);
    }
    if(tinyMCE.get('dis_posts'))
        tinyMCE.get('dis_posts').setContent('');
    $('#dis_posts').val()
}

function ask_question() {
    $('#minutes').hide();
    $('#question').show();
}

function formDisplay(id) {
    $('#form_id' + id).show();
    $('#post_comment' + id).focus();
    $('#commentMark-' + id).focus();
}

function fileFormDisplay(id) {
    $('#record-' + id).find('.commentBox').show();
    $('#record-' + id).find('textarea').focus();
}

function fileFormHide(id) {
    $('#record-' + id).find('.commentBox').hide();
}

function formHide(id) {
    $('#form_id' + id).hide();
}

function submitEditor(postId, commentId) {

    var $messageContainer = $('.company-service-post-reply[data-post-id=' + postId + ']');
    if(commentId)
        $messageContainer = $('.company-service-post-reply[data-comment-id=' + commentId + ']');

    var $message = $messageContainer.find('.comment_text'),
        text = $messageContainer.find('textarea').val(),
        m = $messageContainer.attr('m'),
        u = $messageContainer.attr('u'),
        t = $messageContainer.attr('t'),
        $submitButton = $messageContainer.find('.comment_editor_controls a.save-post');

    $submitButton.attr("onclick", "return false;");

    if (text == '') {
        alert('Please type your comment.');
        return false;
    }

    $.post("/themes/maennaco/includes/add_comment.php?type=edit_company_service_post", {
        id: commentId ? commentId : postId,
        ctype: commentId ? 'comment' : 'post',
        text: text,
        u: u,
        t: t,
        m: m

    }, function (response) {

        if (response.status == 'success') {
            $submitButton.attr("onclick", "submitEditor(" + postId + ");");
            if(commentId)
                $submitButton.attr("onclick", "submitEditor(" + postId + "," + commentId + ");");
            $message.html(n2br(text));
            hideEditor(postId, commentId);
        }
        else {
            alert("Please refresh the page and try again!");
        }

    }, "json");
}

function submitFilePostEditor(postId, commentId) {

    var $messageContainer = $('.service-file-post[data-post-id=' + postId + ']');
    if(commentId)
        $messageContainer = $('.service-file-post[data-comment-id=' + commentId + ']');

    var $message = $messageContainer.find('.comment_text'),
        text = $messageContainer.find('textarea').val(),
        m = $messageContainer.attr('m'),
        u = $messageContainer.attr('u'),
        t = $messageContainer.attr('t'),
        $submitButton = $messageContainer.find('.comment_editor_controls a.save-post');

    $submitButton.attr("onclick", "return false;");

    if (text == '') {
        alert('Please type your comment.');
        return false;
    }

    $.post("/themes/maennaco/includes/add_comment.php?type=edit_service_file_post", {
        id: commentId ? commentId : postId,
        ctype: commentId ? 'comment' : 'post',
        text: text,
        u: u,
        t: t,
        m: m
    }, function (response) {

        if (response.status == 'success') {
            $submitButton.attr("onclick", "submitFilePostEditor(" + postId + ");");
            if(commentId)
                $submitButton.attr("onclick", "submitFilePostEditor(" + postId + "," + commentId + ");");
            $message.html(n2br(text));
            hideFilePostEditor(postId, commentId);
        }
        else {
            alert("Please refresh the page and try again!");
        }

    }, "json");
}

function addPost() {

    var $container = $('#minutes'),
        text = $container.find("textarea").val(),
        $submitButton = $container.find('a.save-post'),
        dissid = $submitButton.attr('dissid'),
        u = $submitButton.attr('u'),
        t = $submitButton.attr('t'),
        m = $submitButton.attr('m'),
        editor = '<?=$user_id;?>',
        pro_profile = '<?=$_REQUEST['id'];?>';

    if (text.trim().length > 0) {

        $submitButton.attr("onclick", "return false;");
        $.post("./themes/maennaco/includes/add_comment.php?type=project-service-post", {
            dissid: dissid,
            text: text,
            editor: editor,
            m: m,
            u: u,
            t: t,
            view_id: '<?= $_REQUEST['view_id']?>',
            pro_profile: pro_profile
        }, function (response) {

            if (response.status == 'success') {
                $container.hide();
                $(".comts").after(response.display).show();
                $submitButton.attr("onclick", "addPost()");
                $container.find("textarea").val('');
            }
            else
                return alert("Please refresh the page and try again!");

        }, "json");
    }
    else return alert('Please type your comment!');

}

function showEditor(postId, commentId) {

    var $messageContainer = $('.company-service-post-reply[data-post-id=' + postId + ']');
    if(commentId)
        $messageContainer = $('.company-service-post-reply[data-comment-id=' + commentId + ']');

    var $controls = $messageContainer.find('.comment_anchor'),
        $text = $messageContainer.find('.comment_text'),
        $editor = $messageContainer.find('.comment_editor'),
        $editorControls = $messageContainer.find('.comment_editor_controls');

    formHide(postId);

    $controls.hide();
    $text.hide();
    $editor.show();
    $editorControls.show();
    $editor.find('textarea').focus();
}

function showFilePostEditor(postId, commentId) {

    var $messageContainer = $('.service-file-post[data-post-id=' + postId + ']');
    if(commentId)
        $messageContainer = $('.service-file-post[data-comment-id=' + commentId + ']');

    var $controls = $messageContainer.find('.comment_anchor'),
        $text = $messageContainer.find('.comment_text'),
        $editor = $messageContainer.find('.comment_editor'),
        $editorControls = $messageContainer.find('.comment_editor_controls');

    fileFormHide(postId);

    $controls.hide();
    $text.hide();
    $editor.show();
    $editorControls.show();
    $editor.find('textarea').focus();
}

function hideFilePostEditor(postId, commentId) {

    var $messageContainer = $('.service-file-post[data-post-id=' + postId + ']');
    if(commentId)
        $messageContainer = $('.service-file-post[data-comment-id=' + commentId + ']');

    var $controls = $messageContainer.find('.comment_anchor'),
        $text = $messageContainer.find('.comment_text'),
        $editor = $messageContainer.find('.comment_editor'),
        $editorControls = $messageContainer.find('.comment_editor_controls');

    $controls.show();
    $text.show();
    $editor.hide();
    $editorControls.hide();
}

function hideEditor(postId, commentId) {

    var $messageContainer = $('.company-service-post-reply[data-post-id=' + postId + ']');
    if(commentId)
        $messageContainer = $('.company-service-post-reply[data-comment-id=' + commentId + ']');

    var $controls = $messageContainer.find('.comment_anchor'),
        $text = $messageContainer.find('.comment_text'),
        $editor = $messageContainer.find('.comment_editor'),
        $editorControls = $messageContainer.find('.comment_editor_controls');

    $controls.show();
    $text.show();
    $editor.hide();
    $editorControls.hide();
}

function submitFilePost(el) {

    var a = $("#watermark").val();
    var m = '<?= md5($user_id.$_REQUEST['fid']."kyarata75")?>';
    var u = '<?=$user_id?>';
    var fid = '<?=$_REQUEST['fid'];?>';
    var flag = 'af';
    var bEdit = false;
    var pid = 0;

    if (a != "") {

        $(el).attr('onclick', 'return false;');
        $.post("/themes/maennaco/includes/pro_comments_posts.php", {
            value: a,
            m: m,
            uid: u,
            fid: fid,
            flag: flag,
            bEdit: bEdit,
            pid: pid
        }, function (response) {

            $('#posting').prepend($(response).hide().fadeIn());
            $("#watermark").val("");
            $('#fShareIdea').hide();
            $(el).attr('onclick', 'submitFilePost(this);');

        });
    } else return alert('Please type your message');
}

function submitFileComment(postId, el) {

    var a = $("#commentMark-" + postId).val(),
        m = $(this).attr("m"),
        u = $(this).attr("uid"),
        sel_obj = $(this),
        cid = 0;

    if (a != "") {

        $(el).attr('onclick', 'return false;');
        $.post("/themes/maennaco/includes/add_comment.php?type=pro_file_comment", {

            value: a,
            m: m,
            uid: u,
            pid: postId,
            bEdit: 0,
            cid: cid

        }, function (response) {
            sel_obj.parent().parent().prev().append(($(response).hide().fadeIn()));
            $("#commentMark-" + postId).val("");
            $(el).attr('onclick', 'submitFileComment(' + postId + ', this);');
            fileFormHide(postId);
        });
    } else alert("Please type your message");
}

function deleteMinutesConfirm(id) {
  if (confirm('Are you sure you want to delete this post?')) {
    var postUrl = '<?php echo $base_url; ?>/account?tab=companies&page=company_detail&id=<?php echo $id; ?>&mtab=advice&view_id=<?= $_REQUEST['view_id'] ?>';
    $.post(postUrl, {
      deleteMinutes: id
    }).done(function(data){
      //window.location.href = postUrl + '&tabs=2';
        $("#dis_post"+id).fadeOut(200, function(){
            $(this).remove();
        });
    });
  }
}
function edit_file_subcomments(cid,pid) {
    var text = $('#subcomment-' + cid).html();
    $('#commentMark-' + pid).val(text);
    $('#commentMark-' + pid).attr('stype','edit');
    $('#commentMark-' + pid).attr('rel',cid);
    $('#commentMark-' + pid).focus();
}


function edit_file_comments(pid) {
    var text = $('#record-' + pid).find('div.comment_text').html();
    $('#watermark').val(text);
    $('#watermark').attr('stype','edit');
    $('#watermark').attr('rel',pid);
    $("#watermark").focus();
}

function edit_minutes(pid) {	
	cancel_min_com();
  var text = $('#edit_minutes_'+pid).html();
  // tinymce.EditorManager.execCommand('mceRemoveControl',true, 'dis_posts');
  $('#dis_posts').val(text);
  // tinymce.EditorManager.execCommand('mceAddControl',true, 'dis_posts');
  tinyMCE.get('dis_posts').setContent(text);

  if (old_formaction == '') {
    old_formaction = $('#minutes form#comments').attr('action');
  }

  $('#minutes form#comments').attr('action', old_formaction + '&editMinutes=' + pid);
  edit_show_minutes(pid);
}

function show_hidden_minutes(){
	var divId = $('#min_id').val();
	//$('#dis_post'+divId).height($('#min_position').val());	
	$('#dis_post'+divId).html($('#tmp_minutes').html());
}

function clear_minutes_prop(){
	$('#min_id').val('');
	$('#min_position').val('')
	$('#tmp_minutes').html('');
}

function edit_show_minutes(pid) { 
	var position = $('#dis_post'+pid).offset();
	var height = $('#dis_post'+pid).height();
	   
	$('#tmp_minutes').html($('#dis_post'+pid).html());
	$('#min_id').val(pid);
	$('#min_position').val(height)
	$('#dis_post'+pid).html('');
	
	if(height<287)
		$('#dis_post'+pid).height(287);
	else
		$('#dis_post'+pid).height(height);
	$( ".comts" ).offset({ top: (position.top-370), left: position.left });
	$( ".comts" ).css('height', '1px');
    $('#minutes').show();
	
}

$( "#cancel" ).livequery("click", function (e) {	
	cancel_min_com();
});

function cancel_min_com(){
	show_hidden_minutes();
	show_hidden_comments();
	$( ".comts" ).removeAttr( 'style' );
	var divId = $('#min_id').val();
	$('#dis_post'+divId).removeAttr( 'style' );
	var comId = $('#com_id').val();
	$('#comments_'+comId).removeAttr( 'style' );	
	clear_comments_prop();
	clear_minutes_prop();
}

function edit_comments(pid,min_id) {
	cancel_min_com();
    var text = $('#com'+pid).html();
    // tinymce.EditorManager.execCommand('mceRemoveControl',true, 'dis_posts');
    $('#dis_posts').val(text);
    // tinymce.EditorManager.execCommand('mceAddControl',true, 'dis_posts');
    tinyMCE.get('dis_posts').setContent(text);

    if (old_formaction == '') {
        old_formaction = $('#minutes form#comments').attr('action');
    }

    $('#minutes form#comments').attr('action', old_formaction + '&editComments=' + pid);
	edit_show_comments(min_id);
}

function show_hidden_comments(){
	var comId = $('#com_id').val();	
	//$('#comments_'+comId).height($('#com_position').val());		
	$('#comments_'+comId).html($('#tmp_comments').html());
}

function clear_comments_prop(){
	$('#comments_'+$('#com_id').val()).attr( 'style' , $('#tmp_com_style').html() );
	show_hidden_comments();
	$('#com_id').val('');
	$('#com_position').val('')
	$('#tmp_comments').html('');
}

function edit_show_comments(min_id) { 	
	var position = $('#comments_'+min_id).offset();
	var height = $('#comments_'+min_id).height();
	
	$('#tmp_com_style').html($('#comments_'+min_id).attr('style'));   
	$('#comments_'+min_id).removeAttr( 'style' );
	$('#tmp_comments').html($('#comments_'+min_id).html());
	$('#com_id').val(min_id);
	$('#com_position').val(height)
	$('#comments_'+min_id).html('');
	
	if(height<287)
		$('#comments_'+min_id).height(287);
	else
		$('#comments_'+min_id).height(height);
	$( ".comts" ).offset({ top: (position.top-370), left: position.left });
	$( ".comts" ).css('height', '1px');
    $('#minutes').show();
}


function show_files(id, proid, file) {
    //window.location.href="./account?tab=professionals&page=pro_detail&id="+id+"&section=pro_industry_view&type=details&tabs=1&pro_id="+proid+"&files="+file;
}

<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

function like_post_comments(type, comment_id, user_id) {
    if (type == 'like')
        var status = 1;
    else
        var status = 0;

    $.ajax({
        type: 'get',
        url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
            'type=like_post_comments&' +
            'comment_id=' + comment_id + '&' +
            'user_id=' + user_id + '&' +
            'status=' + status + "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",
        data: '',
        beforeSend: function () {
        },
        success: function () {
            if (type == 'like') {
                $('#likepostcomment' + comment_id).html("<a style='cursor:pointer;' onclick='like_post_comments(\"unlike\", " + comment_id + "," + user_id + ");'>Unlike</a>");
            }
            else {
                $('#likepostcomment' + comment_id).html("<a style='cursor:pointer;' onclick='like_post_comments(\"like\", " + comment_id + "," + user_id + ");'>Like</a>");
            }
        }
    });
}

$(function(){
    if('#notes' == location.hash)
        $('#tab2.TabbedPanelsTab').trigger('click');

    var availableTags = {items: [
        //Get the advisors and connected users for the autocomplete feature; $companyid was gotten in the earlier phase in new_company_detail_left.php
        <?php
        $Conns = Connections::Com_conns($companyid);
        foreach($Conns['Advisor'] as $Pro) {
            $pro_uid = $Pro->assignee_uid;
            $pro_maeid = getProId($pro_uid);
            echo '{value: "'.$pro_uid.'", name: "'.$pro_maeid.'", type: "advisor"},';
        }
        foreach($Conns['Client'] as $Pro) {
            $pro_uid = $Pro->assignee_uid;
            $pro_maeid = getProId($pro_uid);
            echo '{value: "'.$pro_uid.'", name: "'.$pro_maeid.'", type: "client"},';
        }
        ?>
    ]};

    if(availableTags.items.length <= 0)
        $('.add-service-form').addClass('no-users-available');
    else
        $('.add-service-form').removeClass('no-users-available');

    $('[id^=rmFile]').livequery("click", function () {
        if (confirm("Removing files may cause disruption and delays. Are you sure you want to remove this file?")) {
            var fileid = $(this).attr('id').replace("rmFile", "");
            $.post(
                "/themes/maennaco/includes/posts.php?type=removeFile&fileid=" + fileid,
                function (response) {
                    $("#file" + fileid).remove();
                }
            );
        }
    });
    $('[id^=unDelFile]').livequery("click", function () {
        deleted = $(this);
        if (confirm("Are you sure you want to undelete this file. It will again be visible to members of service?")) {
            var fileid = $(this).attr('id').replace("unDelFile", "");
            $.post(
                "/themes/maennaco/includes/posts.php?type=unDeleteFile&fileid=" + fileid,
                function (response) {
                    deleted.remove();
                }
            );
        }
    });

    $(".service-update-deliverables-btn").click(function (e) {
        e.preventDefault();
        var eventid = $(this).data('id'),
            uid = $(this).data('uid'),
            time = $(this).data('time'),
            hash = $(this).data('hash'),
            filterColleagues = $(this).data('filter-colleagues');

        $.post("/themes/maennaco/includes/posts.php?type=eventEdit&display=true&eventid=" + eventid, {
                uid: uid,
                time: time,
                hash: hash
            },
            function (response) {
                $("#service-update-deliverables-dlg").dialog({
                    autoOpen: true,
                    width: 650,
                    title: 'Service editor',
                    height: 600,
                    buttons: {
                        "Save": {
                            text: "Save",
                            'class': "form-submit1",
                            click: function () {
                                var editEventForm = $("#editEventForm");
                                var $form = editEventForm;
                                var budget = $form.find('input[name=budget]').val();
                                var total = $form.find('input[name=total]').val();
                                var fee = $form.find('input[name=clewed_fee]').val();
                                var title = editEventForm.children("#eventType").val();
                                var datetime = editEventForm.children("#eventDateEdit").val();
                                var loc = editEventForm.children("#eventLoc").val();
                                var agenda = editEventForm.children("textarea[name=eventDesc]").val();
                                var adjustments = editEventForm.children("textarea[name=subsequent_adjustments]").val();
                                var uname = '<?=$user->name;?>';
                                var cid = <?=$_REQUEST['id'];?>;
                                var uid = <?=$user->uid;?>;
                                var time = <?php echo $time = time();?>;
                                var hash = '<?php echo md5($user->uid . ':' . $time . ':kyarata75');?>';
                                var mainExpert = $form.find('.main-expert-container .as-values').val();
                                var invitedExperts = $form.find('.invited-experts-container .as-values').val();

                                var notif = 'true';
                                if ($("#chkNot").is(":checked")) notif = 'true'; else notif = 'false';

                                var milestones = [];
                                editEventForm
                                    .find('.milestones-container .milestone')
                                    .each(function(i, m){
                                        var id = $(m).data('id'),
                                            dueDate = $(m).find('.due-date').val(),
                                            description = $(m).find('.description').val();
                                        if(dueDate !== void 0 && dueDate.length > 0) {
                                            var splitted = dueDate.split('/'),
                                                year = '20' + splitted[2],
                                                month = splitted[0] - 1,
                                                day = splitted[1];
                                            milestones.push({
                                                id: id,
                                                due_date: new Date(year, month, day, 12).getTime() / 1000,
                                                description: description
                                            });
                                        }
                                    });

                                $.post(
                                    "/themes/maennaco/includes/posts.php?type=eventEdit",
                                    {
                                        eventid: eventid,
                                        title: title,
                                        datetime: datetime,
                                        loc: loc,
                                        agenda: agenda,
                                        invitees: invitedExperts,
                                        u: uname,
                                        fee: fee,
                                        budget: budget,
                                        total: total,
                                        time: time,
                                        executor_id: mainExpert,
                                        hash: hash,
                                        cid: cid,
                                        uid: uid,
                                        files: [],
                                        notif: notif,
                                        subsequent_adjustments: adjustments,
                                        milestones: milestones
                                    },
                                    function () {
                                        location.reload();
                                    });
                            }},
                        "Close": {
                            'class': "form-submit2",
                            text: "Close",
                            click: function () {
                                $(this).dialog("close");
                            }
                        }
                    },
                    closeOnEscape: true,
                    modal: true
                }).html(response);
                var invited = $(response).find(".invited-experts-container input").val().split(',');

                if(availableTags.items.length <= 0) {
                    $('.edit-service-form').addClass('no-users-available');
                    $(".edit-service-form input[name=executor_id]").val('');
                }
                else
                    $('.edit-service-form').removeClass('no-users-available');

                var preFill = availableTags.items.filter(function(item){
                    return invited.indexOf(item.value) >= 0;
                });

                var activeConnections = availableTags.items.filter(function(item) {

                    return item.type != 'client';

                });

                var executorId = $(response).find("input[name=executor_id]").val();
                $("#service-update-deliverables-dlg input[name=executor_id]").autoSuggest(activeConnections, {
                    preFill: availableTags.items.filter(function(el){
                        return el.value == executorId;
                    }),
                    selectionLimit: 1,
                    selectedItemProp: "name",
                    searchObjProps: "name"
                });

                if(filterColleagues)
                    availableTags.items = availableTags.items.filter(function(item){
                        return 'client' == item.type || invited.indexOf(item.value) >= 0;
                    });

                $("#eventInv").autoSuggest(availableTags.items, {
                    preFill: preFill,
                    selectedItemProp: "name",
                    searchObjProps: "name"
                });

                //$("#eventDesc").elastic();
                $(".edit-service-form .as-selections").width(290);
                $(".datepicker").datetimepicker(
                    {
                        dateFormat: 'yy-mm-dd',
                        timeFormat: 'hh:mm'
                    }
                );
            });
    });

    $(".service-add-files-btn").click(function (e) {
        e.preventDefault();
        var $btn = $(this),
            $container = $("#service-file-upload-dlg"),
            eventId = $btn.data('id'),
            uid = $btn.data('uid'),
            time = $btn.data('time'),
            hash = $btn.data('hash');

        $.post("/themes/maennaco/includes/manage-service-files.php?type=edit&display=true&eventid=" + eventId, {
                uid: uid,
                time: time,
                hash: hash
            },
            function (response) {
                $container.dialog({
                    autoOpen: true,
                    width: 650,
                    title: 'Service files editor',
                    buttons: {
                        "Save": {
                            text: "Save",
                            'class': "form-submit1",
                            click: function () {
                                var files = [];
                                $container.find(".fileInfo").each(function () {
                                    var tmp = {'path': $(this).attr('timestamp') + '_' + $(this).attr('path'), 'title': $(this).attr('filetitle'), 'timestamp': $(this).attr('timestamp')};
                                    files.push(tmp);
                                });

                                $.post(
                                    "/themes/maennaco/includes/manage-service-files.php?type=edit&eventid=" + eventId,
                                    {
                                        uid: uid,
                                        time: time,
                                        hash: hash,
                                        files: files
                                    },
                                    function (response) {
                                        location.reload();
                                    }
                                );
                            }},
                        "Close": {
                            'class': "form-submit2",
                            text: "Close",
                            click: function () {
                                $(this).dialog("close");
                            }
                        }
                    },
                    closeOnEscape: true,
                    modal: true
                }).html(response);

                var uploader1 = new qq.FileUploader({
                    // pass the dom node (ex. $(selector)[0] for jQuery users)
                    element: $("#file-uploader1")[0],
                    // path to server-side upload script
                    action: '/themes/maennaco/includes/file_upload.php',
                    onComplete: function (id, fileName, responseJSON) {
                        if (responseJSON['success']) {
                            fileName = responseJSON['name'].replace(responseJSON['timestamp'] + '_', '');
                            $container.append('<input type="hidden" class="fileInfo" path="' + fileName + '" filetitle="' + $("#fileTitleEdit").val() + '" timestamp = "' + responseJSON['timestamp'] + '">');
                            $("#fileTitleEdit").val('');
                        }
                    }
                });
            });
    });
});

<?php if(isset($_REQUEST['files'])) : ?>

$(document).ready(function () {

    $("#watermark").focus(function () {

            $("#submitDiv").show();

        }

    );

    $("textarea[name='commentMark']").focus(function () {

            $(this).parent().parent().children('a').show();

        }

    );

    $('#comm_shareButton').click(function (event) {

        event.preventDefault();

        var a = $("#watermark").val();
        var m = '<?=md5($user_id.$_REQUEST['fid']."kyarata75")?>';
        var u = '<?=$user_id?>';
        var fid = '<?=$_REQUEST['fid'];?>';
        var flag = 'af';
        var bEdit = false;
        var pid = 0;

        if ($('#watermark').attr('stype') == 'edit') {

            bEdit = true;
            pid = $("#watermark").attr('rel');
        }



        if (a != "") {

            $.post("/themes/maennaco/includes/pro_comments_posts.php", {

                value: a,
                m: m,
                uid: u,
                fid: fid,
                flag: flag,
                bEdit: bEdit,
                pid: pid

            }, function (response) {

                $('#posting').prepend($(response).hide().fadeIn());
                $("#watermark").val("");
                if (bEdit) {
                    $("#watermark").attr("stype", '');
                    $("#watermark").attr("rel", '');
                    $('#record-' + pid).find('div.comment_text').html();
                    $('#record-' + pid).find('div.comment_text').html(a);

                }
                else {
                    $('#fShareIdea').hide();
                }
            });
        }
    });

    $(".SubmitSubComment").livequery('click', function (event) {
        event.preventDefault();

        pid = $(this).attr("pid");
        a = $("#commentMark-" + pid).val();
        m = $(this).attr("m");
        u = $(this).attr("uid");
        sel_obj = $(this);
        var bEdit = false;
        var cid = 0;

        if ($('#commentMark-' + pid).attr('stype') == 'edit') {

            bEdit = true;
            cid = $('#commentMark-' + pid).attr('rel');
        }

        if (a != "") {

            $.post("/themes/maennaco/includes/add_comment.php?type=pro_file_comment", {

                value: a,
                m: m,
                uid: u,
                pid: pid,
                bEdit: bEdit,
                cid: cid

            }, function (response) {

                //alert(sel_obj.parent().parent().attr('id'));
                sel_obj.parent().parent().prev().append(($(response).show()));
                $("#commentMark-" + pid).val("");
                if (bEdit) {
                    $("#commentMark-" + pid).attr("stype",'');
                    $("#commentMark-" + pid).attr("rel",'');
                    $('#comment-' + cid).find('div.subcomment_text').html(a);
                    $('#subcomment-' + cid).html(a);

                }


            });
        }

    });

    $("a[id^='remove_id']").livequery('click', function () {
        var m = $(this).attr('m'),
            pid = $(this).attr('pid');

        if (false == confirm('Are you sure you want to delete this Post?'))
            return false;

        $.post("/themes/maennaco/includes/delete_comment.php?type=pro_file_post", {
            pid: pid,
            m: m
        }, function (response) {
            if (response.status == 'success') {
                $("#record-" + pid).fadeOut(200, function() {
                    $(this).hide();
                });
            }
            else alert(response.display);
        }, "json");

    });
    $("a[id^='cid-']").livequery('click', function (event) {

        event.preventDefault();

        var m = $(this).attr('m'),
            cid = $(this).attr('cid');

        if (false == confirm('Are you sure you want to delete this Post?'))
            return false;

        $.post("/themes/maennaco/includes/delete_comment.php?type=pro_file_comment", {
            cid: cid,
            m: m
        }, function (response) {
            if (response.status == 'success') {
                $("#comment-" + cid).fadeOut(200, function(){
                    $(this).hide();
                });
            }
            else alert(response.display);
        }, "json");

    });

});

<?php endif; ?>

</script>

<script>
function DownloadFile(uri, name) 
{
    var link = document.createElement("a");
    link.download = name;
    link.href = uri;
    link.click();
};

</script>

<style type="text/css">
    div.content_box .box_title {
        margin-top: 14px;
    }

    .text_button {
        border: none !important;
        background-color: transparent !important;
        color: #0fabc4 !important;
        cursor: pointer;
        font-family: 'LatoRegular' !important;
        font-size: 14px !important;
        font-style: normal !important;
    }

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
function nameToId($name) {
    $q = mysql_query("SELECT uid FROM users WHERE name = '" . $name . "' LIMIT 1") or die(mysql_error());
    $r = mysql_fetch_array($q);
    return $r['uid'];
}

function getUserType($uid) {
    $q = mysql_query("SELECT pid FROM maenna_people WHERE pid = '" . ((int) $uid) . "' ");
    if (mysql_num_rows($q) > 0) return 'people';
    else {
        $q1 = mysql_query("SELECT companyid FROM maenna_company WHERE companyid = '" . ((int) $uid) . "' ");
        if (mysql_num_rows($q1) > 0) return 'company';
        else return 'admin';
    }
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

$page = sget($_REQUEST, 'page');
$type = sget($_REQUEST, 'type');
$id = sget($_REQUEST, 'id');
$pro_id = sget($_REQUEST, 'pro_id');
//$result1 = mysql_query ("SELECT *
//					     FROM  `maenna_professional`
//					     WHERE  id = '".$pro_id."' ORDER BY id DESC ");
$row1 = $event; //mysql_fetch_array($result1);


$result3 = mysql_query(
    "SELECT *
                             FROM  `like_discussion_professional`
                             WHERE  prof_id = '" . ((int) $pro_id) . "' and user_id = '" . ((int) $id) . "'"
);

$likepost = mysql_num_rows($result3);
$row3 = mysql_fetch_array($result3);

$resPayment = mysql_query("SELECT * FROM  `maenna_professional_payments` WHERE  pro_id = '" . ((int) $pro_id) . "' and user_id = '" . ((int) $user_id) . "' ");
$checkUserCount = mysql_num_rows($resPayment);
$checkUser = mysql_fetch_array($resPayment);


$sql_expertise = mysql_query("SELECT * FROM  `maenna_people` WHERE `pid` = '" . $row1['postedby'] . "'");
$sql_exp_result = mysql_fetch_array($sql_expertise);
if ($sql_exp_result['username_type'] == 1) $P_username = ucfirst($sql_exp_result['firstname']);
else $P_username = ucfirst($sql_exp_result['firstname']) . ' ' . ucfirst($sql_exp_result['lastname']);

$cost = $row1['cost'];
$disc_title = $row1['title'];

$disc_date = date("m/d/Y", $row1['datetime']);
$popupDate = date("l, M j, Y g:i A T ", $row1['datetime']);

$userService = new Clewed\User\Service();
$executor = array();
$executorId = $row1['executor_id'];
if(!empty($executorId))
    $executor = reset($userService->get(array($executorId)));

$popupDescription = $row1['title'];
if(!empty($executor) && $executorId !== $user->uid) {
    $popupDescription = "
        You are a participant to a service with the topic <b>{$row1['title']}</b> 
        scheduled for the above reference delivery date. <br/><br/> The lead expert for this service is <b>{$executor['full_name']}</b>
    ";
}
elseif(!empty($executor) && $executorId === $user->uid){
    $popupDescription = "
        You are a lead expert to a service with the topic <b>{$row1['title']}</b>
        scheduled for the above reference delivery date. 
    ";
}
else {
    $popupDescription = "
        You are a participant to a service with the topic <b>{$row1['title']}</b>
        scheduled for the above reference delivery date. 
    ";
}

$companyService = new \Clewed\Company\Service();
$fees = $companyService->getServicePaymentMethodFees();

$paypalFee = $fees['paypal'];
$wireFee = $fees['wire'];

if(1 == $_REQUEST['review'] && !empty($row1['executor_id'])) {
    include_once __DIR__ . '/../blocks/profile/expert_review_form.php';
    echo displayExpertReviewForm($row1['executor_id'], true, null, $row1['eventid']);
}

?>
<div id="pro_popup"></div>
<div id="service-update-deliverables-dlg"></div>
<div id="service-file-upload-dlg"></div>
<div id='docprev' style="width:610px;">
<div class="table-panel-wrapper">
<div class="join">

    <div class="jleft">

        <?php if(!empty($row1['datetime']) && '1970' < date('Y', $row1['datetime'])):?>
            <a class='discussion_link'>Deadline: <?= date("l, M j, Y g:i A T ", $row1['datetime'])."Created By: ".prepareUserName($row1['postedby'],$row1['companyid']); ?></a>
        <?php endif;?>

        <div style="clear:both"></div>

        <?php $expertList = array();

            if(!empty($row1['executor_id'])):
                $expertId = (int) $row1['executor_id'];
                $userName = getProId($expertId);
                $userType = getUserType($expertId);
                $status = empty($row1['executor_status']) ? 'invited' : $row1['executor_status'];
                if ('company' == $userType) {
                    $userName = getProjectName($expertId);
                    $expertList[$status][$expertId] = "<a class=\"tool\" style=\"text-transform: uppercase;\" target=\"_blank\" href=\"/account?tab=companies&page=company_detail&id=" . $expertId . "&mtab=about\"><b>" . $userName . "</b></a> (Lead)";
                } else {
                    $expertList[$status][$expertId] = "<a class=\"tool pro_popup\" rel=\"".$expertId."\" target=\"_blank\" href=\"/account?tab=professionals&page=pro_detail&id=" . $expertId . "&closebtn=1\"><b>" . $userName . "</b></a> (Lead)";
                }
            endif;

            $invitationsResourse = mysql_query("
                SELECT *
                FROM maenna_company_events_inv
                WHERE eventid = '" . (int) $row1['eventid'] . "'
                ORDER BY status DESC"
            );

            while(false !== ($row = mysql_fetch_assoc($invitationsResourse))) {
                $expertId = (int) $row['uid'];
                $userName = getProId($expertId);
                $userType = getUserType($expertId);
                $status = 'sent' == $row['status'] ? 'invited' : $row['status'];
                if ('company' == $userType) {
                    $userName = getProjectName($expertId);
                    $expertList[$status][$expertId] = "<a class=\"tool\" style=\"text-transform: uppercase;\" target=\"_blank\" href=\"/account?tab=companies&page=company_detail&id=" . $expertId . "&mtab=about\"><b>" . $userName . "</b></a>";
                } else {
                    $expertList[$status][$expertId] = "<a class=\"tool pro_popup\" rel=\"".$expertId."\" target=\"_blank\" href=\"/account?tab=professionals&page=pro_detail&id=" . $expertId . "&closebtn=1\"><b>" . $userName . "</b></a>";
                }
            }

        ?>

        <div style="margin-top:8px;margin-bottom: 20px;">

            <?php foreach($expertList ?:array() as $status => $experts):
                echo '<span>' . ucfirst($status) . ': ' . implode(', ', $experts) . '</span><br/>';?>
            <?php endforeach;?>

        </div>
    </div>
</div>

<div class="jrght">

    <?php $isAdmin = in_array($AccessObj->user_type, array('super', 'admin'));?>
    <?php $isConfirmedLead = 'confirmed' == $row1['executor_status'] && $row1['executor_id'] == $user->uid;?>
    <?php $isCompany = ($row1['companyid'] == $user->uid || in_array($user->uid,$companyService->getColleagueIds($row1['companyid'])));?>
    <?php $isCompleted = !empty($row1['delivery_date']) && (time() - strtotime($row1['delivery_date']) >= 3 * 24 * 60 * 60);?>
    <?php $isApproved = $row1['approved'];?>
    <?php $isApprovedByClient = 'wire' == $row1['payment_method'];?>
    <?php $isFunded = !empty($row1['payment_id']);?>
    <?php $isStarted = !empty($row1['start_date']);?>

    <?php $cost = 0;?>
    <?php if($isCompany || $isAdmin): ?>
        <?php $cost = number_format((float)$row1['budget'] * (100 + (float)$row1['clewed_fee']) / 100, 0);?>
        <?php if($row1['budget'] == '250' || $cost == 0):?>
            <div style="font-size: 14px;position: absolute;right:10px;font-style:italic; color:#00a1be;">
               <?php if ($cost == 0) { echo("Free of charge");}
               else { ?>
                <a style="
                   cursor:pointer;
                   float: right;
                   padding: 10px 0;
                   width: 137px;
                   margin-right: -10px;
                   margin-bottom:10px;
                   color: #00a2bf;
                   text-decoration: none;
                   border: 1px solid #00a2bf;">$500</a>
                <span data-tooltip="Minimum engagement takes 45-60 minutes for an average cost $500 including expert preparation time. Price may change based on deliverable requirements and expert cost." style="display: block;width:100%;text-align: center;line-height: 8px;margin-bottom:10px;">Pricing in progress</span>
                <?php } if(!$isApproved):?>
                    <span style="display: block;width:100%;text-align: center;line-height: 8px;">Pending Approval</span>
                <?php elseif($isCompleted):?>
                    <span style="display: block;width:100%;text-align: center;line-height: 8px;">Completed</span>
                <?php endif;?>
            </div>
        <?php elseif($isApproved && !$isFunded):?>
            <?php

            $userService = new \Clewed\User\Service();
            $users = $userService->get(array($user->uid));
            if(empty($users[$user->uid]))
                die;

            $u = $users[$user->uid];

            $notifyUrl = PROTO . HOST . '/service-paypal-callback.php';
            $returnUrl = PROTO . HOST . '/account?tab=companies&page=company_detail&id=' . $row1['companyid'] . '&mtab=advice&view_id=' . $row1['eventid'];
            $cancelUrl = $returnUrl;

            $custom = base64_encode(serialize(array(
                'user_id' => $user->uid,
                'service_id' => $row1['eventid']
            )));

            ?>

            <script type="text/javascript">

                function openPaymentPopup() {
                    $('.project-service-payment-popup').dialog({
                        modal: true,
                        autoOpen: true,
                        resizable: false,
                        title: 'Place Your Order',
                        width: 500
                    });
                }

                function showPaymentSummary(selector) {
                    $('.project-service-payment-popup .summary').hide();
                    $('.' + selector + '-summary' ).show();
                }

                function centerPaymentPopup() {
                    $('.project-service-payment-popup').dialog('option', 'position', 'center');
                }

                function confirmWireTransfer(el) {

                    var serviceId = '<?= $row1['eventid']; ?>',
                        $btn = $(el);

                    $btn.attr('onclick', 'return false;');
                    $.post('/wrapper.php?controller=company&action=confirm-wire-transfer', {
                        id: serviceId,
                        method: 'wire',
                        time: <?= $t = time(); ?>,
                        uid: <?= $uid = $user->uid; ?>,
                        hash: '<?= md5("{$row1['eventid']}:wire:{$t}:{$uid}:kyarata75"); ?>'
                    }, function(r) {

                        $btn.attr('onclick', 'confirmWireTransfer(this);');
                        if(!r.success && r.error && r.error.message)
                            return alert(r.error.message);

                        if(r.success) {
                            $btn.val('Please, wait...');
                            alert('Thank you! Your service will begin shortly. Please return to this area to confirm your wire');
                            location.reload();
                        }

                    }, 'json');
                }

                function confirmPaypal(el) {
                    var $btn = $(el);
                    $btn.attr('onclick', 'return false;');
                    $btn.val('Redirecting You To Paypal...');
                    $btn.closest('form').submit();
                }

                $(function(){
                    $('.service-charge-btn').attr('onclick', 'openPaymentPopup();');
                    showPaymentSummary('<?= $row1['payment_method'];?>');
                });

            </script>

            <div class="project-service-pay-button" style="font-size: 14px;position: absolute;right:0px;">
                <?php if(!$isApprovedByClient):?>
                    <input style="width:140px;padding-bottom:35px;padding-left:20px;text-align: center;"
                           id="project-service-pay-btn"
                           class="join service-charge-btn"
                           type="submit"
                           onclick="return false;"
                           value="$ <?php echo $cost; ?>">
                    <label for="project-service-pay-btn" style="cursor:pointer;display:block;margin-top: -40px;color:white;font-style: italic;">Order Service Now</label>
                <?php else:?>
                    <script type="text/javascript">
                        $(function(){
                            if('#wire-transfer-details' == location.hash)
                                openPaymentPopup();
                        });
                    </script>
                    <input style="width:180px;padding-bottom:35px;padding-left:20px;text-align: center;"
                           id="project-service-pay-btn"
                           class="join service-charge-btn"
                           type="submit"
                           onclick="return false;"
                           value="$ <?php echo $cost; ?>">
                    <?php if(empty($row1['wire_reference'])):?>
                        <label for="project-service-pay-btn" style="cursor:pointer;display:block;margin-top: -40px;color:white;font-style: italic;">Confirm/Process Payment</label>
                    <?php else:?>
                        <label for="project-service-pay-btn" style="cursor:pointer;display:block;margin-top: -40px;color:white;font-style: italic;font-size: 13px;">Admin to confirm receipt</label>
                    <?php endif;?>
                <?php endif;?>
            </div>

            <div class="project-service-payment-popup hidden">
                <div class="project-service-payment-popup-container">
                    <div style="padding-top:20px;text-align: left;margin-left: 20px;">
                        Please fund your service or agree to payment terms to place your order!
                    </div>
                    <div class="selector">
                        <div class="option">
                            <input name="payment-option"
                                   onclick="showPaymentSummary('paypal');centerPaymentPopup();"
                                   type="radio"
                                   id="project-service-payment-paypal-option" />
                            <label for="project-service-payment-paypal-option">Paypal</label>
                        </div>
                        <div class="option">
                            <input name="payment-option"
                                   onclick="showPaymentSummary('wire');centerPaymentPopup();"
                                   type="radio"
                                   <?php if($isApprovedByClient):?>
                                       checked="checked"
                                   <?php endif;?>
                                   id="project-service-payment-wire-option" />
                            <label for="project-service-payment-wire-option">Wire or ACH <span style="font-size: 14px;color: #929497;">(Reduces your processing fee)</span></label>
                        </div>
                    </div>
                    <div class="paypal-summary summary hidden">
                        <?php $cost = (float) str_replace(',', '', $cost);?>
                        <div class="content">
                            <div class="row title">
                                <div class="column">Service fee</div>
                                <div class="column">$ <?php echo number_format($cost, 2); ?></div>
                            </div>
                            <div class="row">
                                <div class="column">Processing fee</div>
                                <div class="column">$ <?php echo number_format($cost * $paypalFee / 100, 2); ?></div>
                            </div>
                            <div class="row total">
                                <div class="column">Total</div>
                                <div class="column">$ <?php echo number_format($cost * (100 + $paypalFee) / 100, 2); ?></div>
                            </div>
                            <form action="<?= PAYPAL_URL ?>" method="post">

                                <input style="width:300px;text-align: center;margin: 20px 0;"
                                       class="join"
                                       type="submit"
                                       onclick="confirmPaypal(this);return false;"
                                       value="Proceed With Payment">

                                <input type="hidden" name="cmd" value="_xclick"/>
                                <input type="hidden" name="business" value="<?= PAYPAL_ACCOUNT ?>"/>
                                <input type="hidden" name="no_note" value="1"/>
                                <input type="hidden" name="no_shipping" value="1"/>
                                <input type="hidden" name="bn" value="Clewed_Project_Service_<?= $row1['eventid'] ?>_US"/>
                                <input type="hidden" name="hosted_button_id" value="58JBKVTL2D9JW"/>

                                <input type="hidden" name="notify_url" value="<?= $notifyUrl;?>"/>
                                <input type="hidden" name="return" value="<?= $returnUrl ?>"/>
                                <input type="hidden" name="cancel_return" value="<?= $cancelUrl ?>"/>

                                <input type="hidden" name="email" value="<?= $u['email'] ?>"/>
                                <input type="hidden" name="first_name" value="<?= $u['firstname'] ?>"/>
                                <input type="hidden" name="last_name" value="<?= $u['lastname'] ?>"/>

                                <input type="hidden" name="item_name" value="<?= $row1['title'] ?>"/>
                                <input type="hidden" name="item_number" value="<?= $row1['eventid'] ?>"/>
                                <input type="hidden" name="amount" value="<?= number_format($cost * (100 + $paypalFee) / 100, 2); ?>"/>
                                <input type="hidden" name="currency_code" value="USD"/>
                                <input type="hidden" name="lc" value="US"/>
                                <input type="hidden" name="custom" value="<?php echo $custom;?>"/>
                            </form>
                        </div>
                    </div>
                    <div class="wire-summary summary hidden">
                        <div class="content">
                            <?php if(empty($row1['wire_reference'])):?>
                                <div class="instruction-message">
                                    <?php if('wire' !== $row1['payment_method']):?>
                                        Please click <b>Payment is on its way</b> button below to agree for service to start in good faith while you process payment. <br>
                                    <?php endif;?>
                                    You will need to provide reference # for wire with in 3 days!
                                </div>
                            <?php else:?>
                                <br><br>
                            <?php endif;?>
                            <div class="instruction">
                                <b>Bank of America</b> <br>
                                <b>New York, NY ABA</b> <br>
                                <b>Routing number for Wire:</b> 026-009-593 <br>
                                <b>Routing number for ACH:</b> 021-000-322 <br>
                                <b>Account name:</b> MAENNA & Company LLC <br>
                                <b>Account #:</b> 48303711 6597
                            </div>
                            <div class="row title">
                                <div class="column">Service fee</div>
                                <div class="column">$ <?php echo number_format($cost, 2); ?></div>
                            </div>
                            <div class="row">
                                <div class="column">Processing fee</div>
                                <div class="column">$ <?php echo number_format($cost * $wireFee / 100, 2); ?></div>
                            </div>
                            <div class="row total">
                                <div class="column">Total</div>
                                <div class="column">$ <?php echo number_format($cost * (100 + $wireFee) / 100, 2); ?></div>
                            </div>
                            <?php if('wire' !== $row1['payment_method']):?>
                                <input style="width:300px;text-align: center;margin: 20px 0"
                                       class="join"
                                       type="submit"
                                       onclick="confirmWireTransfer(this);return false;"
                                       value="Payment is on its way">
                            <?php else:?>
                                <script type="text/javascript">

                                    function confirmWireReference(el) {

                                        var serviceId = '<?= $row1['eventid']; ?>',
                                            reference = $('.wire-transfer-reference').val(),
                                            $btn = $(el);

                                        $btn.attr('onclick', 'return false;');
                                        $.post('/wrapper.php?controller=company&action=confirm-wire-transfer-reference&ref=' + reference, {
                                            id: serviceId,
                                            time: <?= $t = time(); ?>,
                                            uid: <?= $uid = $user->uid; ?>,
                                            hash: '<?= md5("{$row1['eventid']}:{$t}:{$uid}:kyarata75"); ?>'
                                        }, function(r) {

                                            $btn.attr('onclick', 'confirmWireReference(this);return false;');
                                            if(!r.success && r.error && r.error.message)
                                                return alert(r.error.message);

                                            if(r.success) {
                                                alert('Wire transfer reference has been updated');
                                                return location.reload();
                                            }

                                        }, 'json');
                                    }

                                    $(function(){
                                        $('.wire-transfer-reference-confirm-btn').attr('onclick', 'confirmWireReference(this);return false;');
                                    });
                                </script>
                                <?php if(!empty($row1['wire_reference'])):?>
                                    <div style="text-align: left;margin-left: 20px;">Wire transfer reference #:</div>
                                <?php endif;?>
                                <input style="outline: none;
                                              height: 42px;
                                            <?php if(empty($row1['wire_reference'])):?>
                                              border:1px solid #ADD;
                                            <?php endif;?>
                                              text-align: center;
                                              width:340px;
                                              margin-top: 3px;"
                                       class="wire-transfer-reference"
                                       placeholder="Provide wire amount, date and confirmation"
                                       value="<?= $row1['wire_reference']; ?>"/>
                                <input style="width:100px;text-align: center;margin: 0 0 20px 10px;padding-left: 20px;"
                                       class="join wire-transfer-reference-confirm-btn"
                                       type="submit"
                                    <?php if(empty($row1['wire_reference'])):?>
                                       value="Confirm"
                                    <?php else:?>
                                       value="Update"
                                    <?php endif;?>
                                >
                            <?php endif;?>
                        </div>
                    </div>
                </div>
            </div>


        <?php elseif(!empty($row1['payment_id'])):?>
            <div style="font-size: 14px;position: absolute;right:10px;font-style:italic; color:#00a1be;">
                $ <?php echo $cost; ?>
                <?php if(!$isCompleted):?>
                    <span style="display: block;width:100%;text-align: center;line-height: 8px;">Funded</span>
                <?php else:?>
                    <span style="display: block;width:100%;text-align: center;line-height: 8px;">Completed</span>
                <?php endif;?>
            </div>
        <?php else:?>
            <div style="font-size: 14px;position: absolute;right:10px;font-style:italic; color:#00a1be;">
                $ <?php echo $cost; ?>
                <?php if(!$row1['approved']):?>
                    <span style="display: block;width:100%;text-align: center;line-height: 8px;">Pending Approval</span>
                <?php elseif($isCompleted):?>
                    <span style="display: block;width:100%;text-align: center;line-height: 8px;">Completed</span>
                <?php endif;?>
            </div>
        <?php endif;?>
    <?php elseif(!empty($row1['executor_id']) && $row1['executor_id'] == $user_id): ?>
        <?php $cost = number_format((float)$row1['budget'], 0);?>
        <?php if('0' == $cost):?>
            <div style="font-size: 14px;font-style:italic; color:#00a1be;position: absolute;right:10px;">
                Free of charge
                <?php if(!$isApproved):?>
                    <span style="display: block;width:100%;text-align: center;line-height: 8px;">Pending Approval</span>
                <?php elseif($isCompleted):?>
                    <span style="display: block;width:100%;text-align: center;line-height: 8px;">Completed</span>
                <?php endif;?>
            </div>
        <?php elseif($isFunded):?>
            <div style="font-size: 14px;position: absolute;right:10px;font-style:italic; color:#00a1be;">
                $ <?php echo $cost; ?>
                <?php if(!$isCompleted):?>
                    <span style="display: block;width:100%;text-align: center;line-height: 8px;">Funded</span>
                <?php else:?>
                    <span style="display: block;width:100%;text-align: center;line-height: 8px;">Completed</span>
                <?php endif;?>
            </div>
        <?php elseif($isApprovedByClient):?>
            <div style="font-size: 14px;position: absolute;right:10px;font-style:italic; color:#00a1be;">
                $ <?php echo $cost; ?>
                <span style="display: block;width:100%;text-align: center;line-height: 8px;">Approved by client</span>
            </div>
        <?php else:?>
            <div style="font-size: 14px;position: absolute;right:10px;font-style:italic; color:#00a1be;">
                $ <?php echo $cost; ?>
                <?php if(!$isApproved):?>
                    <span style="display: block;width:100%;text-align: center;line-height: 8px;">Pending Approval</span>
                <?php elseif($isCompleted):?>
                    <span style="display: block;width:100%;text-align: center;line-height: 8px;">Completed</span>
                <?php endif;?>
            </div>
        <?php endif; ?>
    <?php endif; ?>

    <?php if($isAdmin && !$isFunded):?>

        <script id="project-service-mark-paid-script" type="text/javascript">
            function markPaid(el) {
                var $btn = $(el),
                    paymentId = prompt('Please introduce manual payment identifier');

                if(null === paymentId)
                    return false;

                if(!paymentId || paymentId.trim().length <= 0)
                    return alert('Payment identifier can\'t be empty!');

                if(!confirm("You are about to mark funded a service. Transaction reference number is: " + paymentId + ". Confirm to proceed."))
                    return false;

                $btn.attr('onclick', 'return false;');
                $btn.html('Processing...');
                $.post("/themes/maennaco/includes/payment.php?action=mark-service-paid", {
                    id: '<?= $s = $row1['eventid'] ?>',
                    p: paymentId,
                    u: '<?= $u = $user->uid ?>',
                    n: '<?= $n = uniqid(); ?>',
                    t: '<?= $t = time(); ?>',
                    m: '<?= md5("mark-service-paid:$s:$u:$n:$t:kyarata75"); ?>'
                }, function(r) {

                    if(r.success) {
                        return location.reload();
                    }
                    else if(!r.success && r.msg && r.msg.length > 0)
                        alert(r.msg);
                    else
                        alert('Please, refresh the page and try again!');

                    $btn.html('Mark Funded');
                    $btn.attr('onclick', 'markPaid(this);');

                }, 'json');
            }
            $(function(){
                $('.project-service-mark-paid a').attr('onclick', 'markPaid(this);');
            });
        </script>

        <?php $title =
            "This tool allows admin to create a manual payment record if the selected payment option "
            . "does not have online status confirmation mechanism or if an error occurred during payment process. "
            . "Please make sure that funds are transferred before marking a service funded and enter the corresponding "
            . "transaction reference number";?>

        <div class="project-service-mark-paid">
            <a data-tooltip="<?= $title;?>"
               style="
                   cursor:pointer;
                   float: right;
                   padding: 10px 0;
                   width: <?= 'wire' === $row1['payment_method'] ? 177 : 137; ?>px;
                   margin-top: 85px;
                   color: #00a2bf;
                   text-decoration: none;
                   <?php if(!$isApproved):?>
                       display: none;
                   <?php endif;?>
                   border: 1px solid #00a2bf;">Mark funded</a>
        </div>
    <?php endif; ?>

    <?php if(!$isApproved && in_array($AccessObj->user_type, array('super'))):?>

        <script id="project-service-approve-script" type="text/javascript">
            function approve(el) {

                var $btn = $(el);
                if(!confirm('Approving a service will notify all members of the service to take action. Are you sure you want to approve?'))
                    return false;

                $btn.attr('onclick', 'return false;');
                $btn.html('Processing...');
                $.post('/wrapper.php?controller=company&action=toggle-service-approval', {
                    id: '<?php echo $row1['eventid'] ?>',
                    time: '<?php echo $time = time();?>',
                    uid: '<?php echo $user->uid; ?>',
                    hash: '<?php echo md5($row1['eventid'] . ':' . $time . ':' . $user->uid . ":kyarata75") ?>'
                }, function(r) {

                    if(r.success) {
                        return location.reload();
                    }
                    else if(!r.success && r.msg && r.msg.length > 0)
                        alert(r.msg);
                    else
                        alert('Please, refresh the page and try again!');

                    $btn.html('Approve');
                    $btn.attr('onclick', 'approve(this);');

                }, 'json');
            }
            $(function(){
                $('.project-service-approve-btn a').attr('onclick', 'approve(this);');
            });
        </script>

        <div class="project-service-approve-btn">
            <a style="
                   cursor:pointer;
                   float: right;
                   padding: 10px 0;
                   width: 137px;
                   margin-top: 95px;
                   color: #00a2bf;
                   text-decoration: none;
                   border: 1px solid #00a2bf;">Approve</a>
        </div>
    <?php endif; ?>

    <?php if(($isStarted || (time() >= $row1['datetime'])) && empty($row1['delivery_date']) && ($isAdmin || $isConfirmedLead)):?>

        <script id="project-service-mark-complete-script" type="text/javascript">
            function markComplete(el) {

                var $btn = $(el);
                if(!confirm('Marking a service complete will inform the project owner that you are done with the project and they need to review. Are you sure you have delivered this service?'))
                    return false;

                $btn.attr('onclick', 'return false;');
                $btn.html('Processing...');
                $.post('/wrapper.php?controller=company&action=mark-service-complete', {
                    id: '<?php echo $row1['eventid'] ?>',
                    time: '<?php echo $time = time();?>',
                    uid: '<?php echo $user->uid; ?>',
                    hash: '<?php echo md5($row1['eventid'] . ':' . $time . ':' . $user->uid . ":kyarata75") ?>'
                }, function(r) {

                    if(r.success) {
                        return location.reload();
                    }
                    else if(!r.success && r.msg && r.msg.length > 0)
                        alert(r.msg);
                    else
                        alert('Please, refresh the page and try again!');

                    $btn.attr('onclick', 'markComplete(this);');
                    $btn.html('Mark Complete');

                }, 'json');
            }
            $(function(){
                $('.project-service-mark-complete-btn a').attr('onclick', 'markComplete(this);');
            });
        </script>

        <div class="project-service-mark-complete-btn">
            <a style="
                    cursor:pointer;
                    float: right;
                    padding: 10px 0;
                <?php if(!$isApproved):?>
                    display: none;
                <?php elseif(!$isFunded && $isApprovedByClient && ($isAdmin || $isCompany)):?>
                    width: 177px;
                    margin-right: -179px;
                    margin-top: 140px;
                <?php elseif(!$isFunded && !$isApprovedByClient && ($isAdmin || $isCompany) && !$isConfirmedLead):?>
                    width: 137px;
                    margin-right: -140px;
                    margin-top: 140px;
                <?php elseif(!$isFunded && !$isApprovedByClient && ($isAdmin || $isCompany) && $isConfirmedLead):?>
                    width: 137px;
                    margin-top: 80px;
                <?php else:?>
                    width: 137px;
                    margin-top: 50px;
                <?php endif;?>
                    color: #00a2bf;
                    text-decoration: none;
                    border: 1px solid #00a2bf;">Mark Complete</a>
        </div>
    <?php endif; ?>

<br/><br/><br/>
<div id="TabbedPanels1" class="TabbedPanels" style="position:relative; margin-top:160px;">

<ul class="TabbedPanelsTabGroup">
    <li class="TabbedPanelsTab" id="tab1" tabindex="0">Deliverables & Files</li>
    <?php
    global $AccessObj;

    if ($AccessObj->Com_sections['advice']['sections']['advice_minutes']['access'] != 'hide')
        echo '<li class="TabbedPanelsTab" id="tab2" tabindex="0">Service Discussions</li>';
    ?>
    <?php if (isset($_REQUEST['files'])) :
        list($time_stamp, $file_name) = explode('_', rawurldecode($_REQUEST['files']));
        list($name, $ext) = explode('.', $file_name);
        ?>
        <li class="TabbedPanelsTab" id="tab3"
            tabindex="0"><?= strlen($name) > 10 ? substr($name, 0, 10) . '...' : $name; ?></li>
    <?php endif; ?>
</ul>

<div class="TabbedPanelsContentGroup">

<!--- start about tab content -->

<div class="TabbedPanelsContent">
    <div style="float: left;padding-top: 15px;width:569px;">
        <?php if ($row1['postedby'] == $user->uid || $AccessObj->Com_sections['advice']['sections']['maenna_events']['access'] == 'write'):?>
        <?php
            $userService = new Clewed\User\Service();
            $isProjectOwner = $user->uid == $row1['companyid'];
            $isColleague = $userService->isColleague($user->uid, $row1['companyid']);
        ?>

        <div class="m-buttons">
        <a  data-id="<?= $eventId = $row1['eventid']?>"
            data-uid="<?= $uid = $user->uid;?>"
            data-time="<?= $time = time();?>"
            data-hash="<?= md5($uid . ':' . $time . ':kyarata75')?>"
            data-filter-colleagues="<?= (($isProjectOwner || $isColleague) && !empty($row1['start_date'])) ? 1 : 0;?>"
            class="tool service-update-deliverables-btn">Edit Service</a>

            <a  data-id="<?= $eventId = $row1['eventid']?>"
                data-uid="<?= $uid = $user->uid;?>"
                data-time="<?= $time = time();?>"
                data-hash="<?= md5($eventId . ':' . $uid . ':' . $time . ':kyarata75')?>"
                class="tool service-add-files-btn">Edit files</a>
        </div>
        <?php endif;?>
        <strong class="part-title" >Delivery format</strong>

        <p style="padding-bottom: 20px"><?= nl2br($row1['location']); ?></p>
        <!--<strong style="font-style:normal; font-weight:bold; color:#686b70;text-transform: uppercase;">Why Attend?</strong>-->
        <!--<p><?php // =$row1['whyattend'];?></p>-->
    </div>

    <div>
        <strong class="part-title" >Service description</strong>
        <p style="padding-bottom: 20px;"><?php print $row1['description']; ?></p>
    </div>

    <?php $companyService = new Clewed\Company\Service();
    $milestones = $companyService->getServiceMilestones($row1['eventid']);
    if(!empty($milestones)):?>
        <div>
            <strong class="part-title" >Milestones</strong>
            <div class="project-about-tab-milestones-container">
            <?php foreach($milestones as $milestone):?>
                <div class="milestone">
                    <div class="due-date"><?= date("m/d/y", strtotime($milestone['due_date']))?></div>
                    <div class="description"><?= $milestone['description']?></div>
                </div>
            <?php endforeach;?>
            </div>
        </div>
    <?php endif;?>

    <?php if(!empty($row1['subsequent_adjustments'])):?>
        <div>
            <strong class="part-title">Subsequent adjustments</strong>
            <p style="padding-bottom: 20px;"><?php print $row1['subsequent_adjustments']; ?></p>
        </div>
    <?php endif;?>

    <div class="materials" style="height:18px;">
        <?php if ($row1['postedby'] == $user->uid || $AccessObj->Com_sections['advice']['sections']['maenna_events']['access'] == 'write'):?>
            <?php
            $userService = new Clewed\User\Service();
            $isProjectOwner = $user->uid == $row1['companyid'];
            $isColleague = $userService->isColleague($user->uid, $row1['companyid']);
            ?>

            <div class="m-buttons">
                <a  data-id="<?= $eventId = $row1['eventid']?>"
                    data-uid="<?= $uid = $user->uid;?>"
                    data-time="<?= $time = time();?>"
                    data-hash="<?= md5($uid . ':' . $time . ':kyarata75')?>"
                    data-filter-colleagues="<?= (($isProjectOwner || $isColleague) && !empty($row1['start_date'])) ? 1 : 0;?>"
                    class="tool service-update-deliverables-btn">Edit Service</a>

                <a  data-id="<?= $eventId = $row1['eventid']?>"
                    data-uid="<?= $uid = $user->uid;?>"
                    data-time="<?= $time = time();?>"
                    data-hash="<?= md5($eventId . ':' . $uid . ':' . $time . ':kyarata75')?>"
                    class="tool service-add-files-btn">Edit files</a>
            </div>
        <?php endif;?>
        <strong class="part-title">Service Files</strong><br/>
    </div>
        <!--			<strong>FILES :</strong>
        -->            <?php
        //Getting images and links of event.
        $files = mysql_query("
            SELECT 
                mcd.*,
                CASE mp.firstname IS NULL WHEN TRUE THEN mc.projname ELSE mp.firstname END as uploaded_by,
                CASE mp.pid IS NULL AND mc.companyid IS NULL WHEN TRUE THEN 1 ELSE 0 END as uploaded_by_clewed
            FROM maenna_company_data mcd
            LEFT JOIN maenna_people mp ON mp.pid = mcd.editorid 
            LEFT JOIN maenna_company mc ON mc.companyid = mcd.editorid 
            WHERE mcd.data_value6 = '" . $row1['eventid'] . "' 
            AND mcd.data_type='service-file' 
            AND mcd.deleted = 0 
            ORDER BY mcd.dataid DESC");

        echo "<ul style=\"margin-top:10px;\">";

        if(mysql_num_rows($files) > 0)
            echo
                "<li style=\"text-transform: uppercase!important;font-family: Lato Bold,serif;text-align: center;padding-left:10px;\">
                    <div style=\"text-align:left;width:350px;float: left;\">Name</div>
                    <div style=\"width:130px;float: left;\">Uploaded by</div>
                    <div style=\"float: left;width:70px;\">Date</div>
                </li>";

        while ($images = mysql_fetch_assoc($files)) {
            if ($images['data_value2'] != '') {
                $name = preg_replace('/^\d+_/', '', $images['data_value2']);
                $uploadedOn = date('m/d/y', $images['access']);
                $uploadedBy = $images['uploaded_by'];
                if($images['uploaded_by_clewed'])
                    $uploadedBy = 'Clewed';

                if ($checkUser['status'] == 1 || $row1['postedby'] == $user_id || true) {
                    $id = $_REQUEST['id']; //
                    $file_name = $images['data_value2'];
                    echo '<li style="padding-left:10px;">'; ?>
                    <a style="width:350px;float:left;" href="account?tab=companies&page=company_detail&id=<?= $_REQUEST['id']; ?>&section=pro_industry_view&type=details&mtab=advice&view_id=<?= $_REQUEST['view_id']; ?>&files=<?= rawurlencode($file_name); ?>&fid=<?= $images['dataid']; ?>"><?php echo $name; ?></a>
                    <div style="width:130px;float:left;text-align: center;"><?=$uploadedBy;?></div>
                    <div style="width:70px;float:left;text-align: center;"><?=$uploadedOn;?></div>
                    <?php echo '</li>';
                } else {
                    echo '<li style="padding-left:10px;">
                        <div style="width:350px;float:left;">' . $name . '</div>
                        <div style="width:130px;float:left;text-align: center;"></div>
                        <div style="width:70px;float:left;text-align: center;"></div>
                    </li>';
                }
            }
        }
        echo "</ul>";?>

        <!--			<strong>LINKS :</strong>
        -->            <?php
        //Getting images and links of event.
        $sql_links = mysql_query("SELECT * FROM  maenna_professional_links WHERE professional_id = '" . ((int) $_REQUEST['pro_id']) . "'");

        while ($links = mysql_fetch_assoc($sql_links)) {
            //echo $row1['cost'] ."!=". 0;
            if ($checkUser['status'] == 1 OR $row1['postedby'] == $user_id) {
                echo "<ul>";
                if ($links['links'] != '') {
                    $st = strpos($links['links'], "://");
                    if ($st == 0) {
                        $resc = "http://" . trim($links['links']);
                    } else {
                        $resc = $links['links'];
                    }
                    echo '<li><a target="_blank" href="' . $resc . '">' . $links['name'] . '</a></li>';
                }
                echo "</ul>";
            } else {
                echo "<ul>";
                echo '<li>' . $links['name'] . '</li>';
                echo "</ul>";
            }
        }
        ?>
    </div>

<!--- end about tab content -->

<!--- start conversation tab content -->
<?php

global $AccessObj;
$op_minutes = $AccessObj->Com_sections['advice']['sections']['advice_minutes']['access'];
$op_comm_minutes = $AccessObj->Com_sections['advice']['sections']['advice_minutes_comments']['access'];
//print "<pre>".print_r($op_comm_minutes,true)."</pre>";
// if(!empty($checkUser) && ($checkUser['status'] == 1 || $checkUser['amount'] == 0) || $row1['postedby'] == $user_id )
if ((!empty($checkUser) && $checkUser['status'] == 1) || $row1['postedby'] == $user_id || $usertype == 'super' || $usertype == 'admin' || in_array($op_minutes,array('write','read','edit')))
{
?>
<div class="TabbedPanelsContent">
		 <span style="margin-right:0px" class="add_minutes_span">
		<?php

        if ($row1['postedby'] == $user_id || in_array($op_minutes,array('edit','write'))) { ?>
             <a class="new_tool" href="javascript:void(0);" onClick="$(this).addClass('active');cancel_min_com(); show_minutes();">Start a new discussion</a>
		 <?php } ?><!--<a href="javascript:void(0);" onClick="ask_question();">Ask a question</a>--></span>

         <div style="clear:both"></div>
		 <div id="tmp_minutes" style="display:none;"></div>		 
		 <input type="hidden" name="min_position" id="min_position">
		 <input type="hidden" name="min_id" id="min_id">

		 <div id="tmp_comments" style="display:none;"></div>
		 <div id="tmp_com_style" style="display:none;"></div>
		 <input type="hidden" name="com_position" id="com_position">
		 <input type="hidden" name="com_id" id="com_id">
		
         <div class="tabtags">
         <div class="comts">
          <style>
          .tabtags div a.mceButton {
            padding:0;
          }
          .tabtags span.mceIcon {
            padding:0;
          }
          .tabtags span.mceEditor {
            padding:0;
          }

          .conversations_forms select {
            min-height: inherit;
            margin: 8px 0;
          }

          #comments {
              padding: 0 0 0 22px;
          }

          .minutes_post_comment span {
            float:none;
          }

          .minutes_post_comment p {
            line-height: 18px;
          }

          </style>
             <div id="minutes" class="conversations_forms" style="display:none;">
                 <div id="error"></div>
                 <form method="post"
                       action="<?php echo $base_url; ?>/account?tab=companies&page=company_detail&id=<?php echo $id; ?>&mtab=advice&view_id=<?= $row1['eventid'] ?>"
                       id="comments">
                     <input type="hidden" name="prof_id" id="prof_id" value="<?php echo $user_id; ?>"/>
                     <input type="hidden" name="dis_id" id="dis_id" value="<?php echo $id; ?>"/>
                     <input type="hidden" name="flag" id="flag" value="am"/>
                     <table width="100%" id="minutues">
                         <tr>
                             <td>
                                 <div class="w">
                                    <textarea class="mceNoEditor" name="dis_posts" placeholder="Add a note or start a discussion" id="dis_posts"
                                              rows="7" style="line-height:15px!important;width:97%!important;margin:-20px 10px 0 10px!important;height:100px"></textarea>
                                 </div></td>
                         </tr>

                         <tr>
                             <td style="float:right;margin-top: 0;">
                                 <a style="padding-right: 5px!important;"
                                    name="dis_post"
                                    onClick="addPost();"
                                    u="<?php echo $user_id;?>"
                                    t="<?php echo $time = time();?>"
                                    m="<?php echo md5("add-project-service-note:{$user_id}:{$time}:kyarata75")?>"
                                    class="tool save-post">Submit</a>&nbsp;|&nbsp;
                                 <a name="dis_post"
                                    id="cancel"
                                    onClick="$('#minutes').hide();return false;"
                                    class="tool">Cancel</a></td>
                         </tr>
                     </table>
                 </form>
             </div>
             <div id="question" class="conversations_forms" style="display:none;">
                 <form method="post"
                       action="<?php echo $base_url; ?>/account?tab=professionals&page=pro_detail&id=<?php echo $id; ?>&section=pro_industry_view&type=details&pro_id=<?= $row1['id'] ?>"
                       id="comments">
                     <input type="hidden" name="prof_id" id="prof_id" value="<?php echo $row1['id']; ?>"/>
                     <input type="hidden" name="dis_id" id="dis_id" value="<?php echo $id; ?>"/>
                     <input type="hidden" name="flag" id="flag" value="q"/>
                     <table width="100%">
                         <tr>
                             <td><textarea name="dis_posts" class="input" id="dis_posts_question"
                                           placeholder="Type Your Question" cols="52" rows="3"></textarea></td>
                         </tr>
                         <tr>
                             <td>
                                 <select id="tags_question" name="tags">
                                     <option value="">Choose a Category</option>
                                     <?php

                                     echo OPTION_TAGS(_categories());

                                     ?>
                                 </select>
                             </td>
                         </tr>
                         <tr>
                             <td>
                                 <input type="submit" name="dis_post" id="question_post" dissid= <?= $row1['id']; ?> m
                                 = <?= md5($row1['id'] . "kyarata75"); ?>  value="Submit"  class="tool" />
                                 <input type="submit" name="dis_post" id="cancel" value="Cancel"
                                        onClick="javascript:$('#question').hide();return false;" class="tool"/></td>
                         </tr>
                     </table>
                 </form>
             </div>

             <div style="clear:both"></div>
         </div>

         <?php
         $result = mysql_query(
             "SELECT *,
                         UNIX_TIMESTAMP() - date_created AS TimeSpent FROM pro_wall_posts where pro_id = " . $row1['eventid'] . " and flag = 'am' order by pid desc"
         );

         $userIds = array();
         while ($row1 = mysql_fetch_array($result))
             $userIds[$userId] = $userId = (int) $row1['user'];

         $userService = new \Clewed\User\Service();
         $users = !empty($userIds) ? $userService->get(array_keys($userIds)) : array();
         mysql_data_seek($result, 0);
         while ($row1 = mysql_fetch_array($result)) {
             $crId = $row1['user'];
             $uid = $crId;
             $uType = getUserType($crId);
             /*$result3 = mysql_query ("SELECT *
                                      FROM  `like_discussion_posts`
                                      WHERE  prof_id = '".$pro_id."' and post_id = '".$row1['p_id']."' and user_id = '".$uid."'");*/
             $result3 = mysql_query(
                 "SELECT *
                                              FROM  `like_discussion_posts`
                                              WHERE  prof_id = '" . ((int) $pro_id) . "' and post_id = '" . $row1['pid'] . "'"
             );
             $row3 = mysql_fetch_array($result3);
             $likepost1 = mysql_num_rows($result3);
             $avatar = $users[$crId]['avatar'];?>
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
                             <div class="asktitle"><?php echo ((getUserById($row1['user']) == '' || getUserById($row1['user']) == 'invalid id') ? $row1['f_name'] : getUserById($row1['user'])); ?>&nbsp;
                                 <?php if ($row1['flag'] == 'q') {
                                     echo '<strong>asks a question:</strong>';
                                 } else {
                                     echo '<strong>starts a new discussion on ' . date('m/d/y', $row1['date_created']) . '</strong>';
                                 }?></div>
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

                                     <?php if ($op_comm_minutes == 'write' || $op_comm_minutes == 'edit'): ?>
                                         &nbsp;|&nbsp;
                                         <a onclick='formDisplay("<?= $row1['pid'] ?>");'>Reply</a>
                                     <?php endif; ?>

                                     <?php if ($op_comm_minutes == 'write' || ($op_comm_minutes == 'edit' && $row1['user'] == $user->uid)): ?>
                                         &nbsp;|&nbsp;
                                         <a onclick="showEditor(<?= $row1['pid'] ?>)">Edit</a>
                                         <div style="display: none;" id='edit_minutes_<?= $row1['pid']?>'>
                                           <?= $row1['post'] ?>
                                         </div>
                                     <?php endif; ?>

                                     <?php if ($op_comm_minutes == 'write' || ($op_comm_minutes == 'edit' && $row1['user'] == $user->uid)): ?>
                                         &nbsp;|&nbsp;
                                         <a onclick='deleteMinutesConfirm("<?= $row1['pid'] ?>");'>Delete</a>
                                     <?php endif; ?>
                                 </div>
                                 <!--<div style='float:right;margin:7px 0px 0px 0px;color:#76787f'>Topic: <?php echo $row1['tags']; ?></div>-->
                                 <!--<a href="#">Invite</a>-->
                             </div>

                         </div>
                     </div>
						 	 <div class="askright" id="comments_<?= $row1['pid'] ?>"
                          style="width:<?php echo $width; ?> !important; float: left;">

                         <!-- <?php if ($row1['flag'] != 'q') {
                             echo '<h5>' . $row1['tags'] . '</h5>';
                         } ?></p> -->
                         <div style="clear:both"></div>

                         <?php
                         $result2 = mysql_query(
                             "SELECT *,
                                         UNIX_TIMESTAMP() - datecreated AS TimeSpent FROM pro_wall_posts_comments where post_id=" . $row1['pid'] . " order by cid asc"
                         );
                         $comments_count = mysql_num_rows($result2);

                         $commentUserIds = array();
                         while ($row2 = mysql_fetch_array($result2))
                             $commentUserIds[$commentUserId] = $commentUserId = (int) $row2['user_id'];

                         $commentUsers = !empty($commentUserIds) ? $userService->get(array_keys($commentUserIds)) : array();
                         mysql_data_seek($result2, 0);

                         while ($row2 = mysql_fetch_array($result2)) {
//    print "<pre>".print_r($row2,true)."</pre>";
                             ?>
                             <?php

                             $crId = nameToId($row2['user_id']);
                             $uType = getUserType($crId);
                             $avatar = $commentUsers[$row2['user_id']]['avatar'];

                             $comment_result = mysql_query(
                                 "SELECT *
                                                          FROM  `like_discussion_posts_comments`
                                                          WHERE  comment_id = '" . $row2['cid'] . "' and user_id = '" . $user->uid . "'"
                             );
                             $likepost_comment = mysql_num_rows($comment_result);
                             ?>
                             <div class="aucomnts company-service-post-reply" u="<?= $user_id;?>" t="<?= $time = time();?>" m="<?= md5($user_id . ':' . $row2['cid'] . ':' . $time .":kyarata75");?>" data-comment-id="<?= $row2['cid'];?>">
                                 <div class="aucpic">
                                     <?php echo "<img src=" . $avatar . " style=\"float:left; margin-top:13px; margin-right:5px; width:45px; height:45px;\">&nbsp;"; ?>
                                 </div>
                                 <div class="aucdisc">
                                     <h5 class="comm-username"><?php echo ((getUserById($row2['user_id']) == '' || getUserById($row2['user_id']) == 'invalid id') ? $row2['f_name'] : getUserById($row2['user_id'])); ?></h5>
                                     <br/>

                                     <div class="comment_text" style="padding:7px 20px 7px 0!important;width:465px"
                                        id="com<?php echo $row2['cid']; ?>"><?php echo n2br(htmlspecialchars($row2['comment'])); ?></div>
                                     <div class="w comment_editor hidden" style="margin-left:0!important;">
                                         <textarea
                                             style="line-height:15px!important;width:92%; margin:0!important; height:100px; "
                                             class="input mceNoEditor"><?php echo htmlspecialchars($row2['comment']);?></textarea>
                                     </div>
                                     <div class="comment_editor_controls hidden" style="float:right;margin: 2px 14px 10px;">
                                         <a class="save-post" style="cursor: pointer;padding-right: 0;" onclick="submitEditor(<?= $row1['pid'];?>,<?= $row2['cid'] ?>);">Submit</a>&nbsp;|&nbsp;
                                         <a style="cursor: pointer" onclick="hideEditor(<?= $row1['pid'];?>,<?= $row2['cid'] ?>);">Cancel</a>
                                     </div>
                                     <div class='comment_anchor'>
                                         <div style='float:left;margin:0px;padding:0px;font-size:12px;'>
                                             <?= date("D, M j, Y g:i A T ", $row2['datecreated']); ?>
                                         </div>
                                         <?php if ($op_comm_minutes == 'write' || $op_comm_minutes == 'edit'): ?>
                                             &nbsp;|&nbsp;
                                             <a onclick='formDisplay("<?= $row1['pid'] ?>");'>Reply</a>
                                         <?php endif; ?>
                                             <?php /*?><p>Tags : <?php echo $row1['tags']; ?></p><?php */ ?>
                                             <?php /*?><p><?php echo time() - $row1['date_created']; ?>&nbsp;few seconds ago<?php */ ?>  <?php
                                             if ($op_comm_minutes == 'write' || ($op_comm_minutes == 'edit' && $row2['user_id'] == $user->uid)){
                                             ?>
                                             <!-- <a style="cursor:pointer;" href="javascript:void(0);" id="deletepost<?= $row1['pid'] ?>" class="deletepost">Delete</a>-->
                                             &nbsp;|&nbsp;<a style="cursor:pointer;" onclick="showEditor(<?= $row1['pid'];?>,<?= $row2['cid'] ?>);"
                                                             id="edit_comment<?= $row2['cid'] ?>"
                                                             class="edit_comment">Edit</a> &nbsp;|&nbsp;<a style="cursor:pointer;" href="javascript:void(0);"
                                                             id="delete_comment<?= $row2['cid'] ?>"
                                                             u="<?php echo $u = time();?>"
                                                             m="<?php echo md5('delete.php:' . $row2['cid'] . ':' . $u . ':kyarata75');?>"
                                                             class="delete_comment">Delete</a>
                                             <?php } ?></p>
                                 </div>
                                 </div>
                                 <div style="clear:both"></div>
                                 <!--<div class="aucpic"></div>
                                 <div class="aucdisc">
                                     <h5>Aseg Amin</h5>
                                 <p>Lorem Ipsum is that it has a more-or-less normal distribution of letters, as opposed to using 'Content here, content here', making it look like </p>
                                 </div><div style="clear:both"></div>--></div>

                         <?php
                         }
                         if ($comments_count == 0) {
                             $comment_style = "style='width:511px;display:none;margin:21px 0px 0px 0px; padding:0px 0px 0px 20px; background:#f4f8fa!important;'";
                         } else {
                             $comment_style = "style='width:511px;margin:21px 0px 0px 0px; padding:0px 0px 0px 20px; background:#f4f8fa!important;'";
                         }
                         ?>
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
                                 }

                                 if ($op_comm_minutes == 'write' || $op_comm_minutes == 'edit') {

                                 ?>
                                 <textarea name="post_comment"
                                           id="post_comment<?php echo $row1['pid']; ?>"
                                           class="input watermark mceNoEditor"
                                           style="line-height:15px!important;width:<?php echo $width; ?>;  margin: 20px 0px 0px 42px!important; height:100px; "
                                           onFocus="showsubmit('<?php echo $row1['pid']; ?>');"></textarea>

                                 <input type="submit" id="post_com<?php echo $row1['pid']; ?>"
                                        m= <?= md5($row1['pid'] . "kyarata75"); ?>  value="Submit" class="text_button"
                                        style="margin-bottom:20px;display:none;vertical-align: top;color:#00A3BF!important;margin-top: 0px;margin-left:<?php echo $marginleft; ?>;"/>
                                 <?php } ?>
                             </form>
                             <!-- <textarea id="post_comments" style="width:85%; margin-left: 82px!important; margin-top: 10px!important;" ></textarea>-->
                         </div>

                     </div>
                 </div>

                 <div style="clear:both"></div>

             </div>
         <?php } ?>
         </div>
         <?php } ?>
</div>

<!--- end conversation tab content -->


<?php if (isset($_REQUEST['files'])) : ?>
    <!-- start file preview -->
    <div class="TabbedPanelsContent" style="width:600px;">		
		<?php 
            $filePath = (isset($_SERVER["HTTPS"]) ? 'https' : 'http')."://".$_SERVER['HTTP_HOST']."/sites/default/files/".(empty($_REQUEST['root']) ? rawurlencode($_REQUEST['files']) : $_REQUEST['root'] . '/' . rawurlencode($_REQUEST['files']));
			if(pathinfo($filePath, PATHINFO_EXTENSION)=="pdf"){
				$viewerTypeUrl = (isset($_SERVER["HTTPS"]) ? 'https' : 'http')."://".$_SERVER['HTTP_HOST']."/themes/maennaco/pdf.js-master/web/viewer.html?file=";
			}else{
				$viewerTypeUrl = "https://docs.google.com/viewer?url=";
			}
            $name = rawurldecode($_REQUEST['files']);
            $name = preg_replace("/^\d+\_/uims", "", $name);
            $name = preg_replace("/\.[^\.]+$/uims", "", $name);
		?>
		<div class="box_title shaded_title"><?php echo $name;?>
			<div style="margin-right:18px;float:right;">                                   
                            <button style="background-color: #003241;color: white; border: 0;" onclick='DownloadFile("/download.php?tab=file&file_name=<?= rawurlencode($_REQUEST['files']);?>","<?= rawurlencode($_REQUEST['files']);?>");'>Download file</button>&nbsp;&nbsp;
                            <a href="/download.php?tab=file&file_name=<?= rawurlencode($_REQUEST['files']);?>" style="display:inline-block;margin-right: 5px;"><img src="/themes/maennaco/images/download_file_icon.jpg"></a>
            </div>
		</div>
        <iframe src="<?= $viewerTypeUrl ?><?= $filePath ?>&embedded=true" width="600" height="600" style="border: none;"></iframe>

<span style="margin-right:0px;" class="add_minutes_span">
        <a style="cursor:pointer;" onclick=" $('#fShareIdea').toggle();">Add comment / feedback</a>
    </span>

        <div style="clear:both;"></div>

        <div style="position: relative;margin-bottom:10px;" id="fShareIdea">
            <form action="" method="post" name="postsForm">

                <div class="UIComposer_Box">

                    <span class="w" style="margin-bottom: -11px;">
                        <textarea
                            style="line-height:18px!important;color:#96979c!important;font-size:12px;font-family:'Lato Italic';border: 1px solid #CCCCCC !important;height:100px; margin-left:5px;width:590px;"
                            class="mceNoEditor" id="watermark" placeholder='Enter your comment here' name="watermark"></textarea>
                    </span>

                    <div style="clear:both"></div>

                    <div id="submitDiv" align="right" style="margin-top:10px;">
                        <a style="cursor:pointer;color: #0fabc4!important;"
                           class="text-button"
                           onclick="submitFilePost(this);">Submit</a>
                    </div>
                </div>

            </form>
            <div style="clear: both;">&nbsp;</div>
            <!--<div style="position: relative;border-bottom:dotted 1px #ccc;bottom: 4px;z-index:-1">
                <div class="discussions">
                    Discussions
                </div>
             </div>-->

        </div>

        <div class="tabtags cmtloop" id="posting" align="center">

            <?php
            $op_comments = 'edit';
            $op_subcomments = 'edit'; // Advice tab file commenting is not part of permission table so permission access is hard-coded (currently you are able to edit and delete your content unless super or admin)
            include_once('pro_comments_posts.php');

            ?>

        </div>

    </div>
    <!-- end file preview -->
<?php endif; ?>
</div>
<div id="createaccount" style="display:none">

    <!--<form method="post" name="joinform" id="joinform">
    First name: <input type="text" name="firstname" id="firstname" value=""  /><br />
    Last name: <input type="text" name="lastname" id="lastname" value=""  /><br />
    E-mail Address: <input type="text" name="emailid" id="emailid" value=""  /><br />
    Password: <input type="password" name="password" id="password" value=""  /><br />
    <input type="submit" name="submit" id="submit" value="SIGN UP"  /><br />
    </form>-->
    <form method="post" name="joinform" id="joinform">
        <?php /*?>
First name: <input type="text" name="firstname" id="firstname" value=""  /><br />
Last name: <input type="text" name="lastname" id="lastname" value=""  /><br />
E-mail Address: <input type="text" name="emailid" id="emailid" value=""  /><br />
Password: <input type="password" name="password" id="password" value=""  /><br />
<input type="submit" name="submit" id="submit" value="SIGN UP"  /><br /><?php */
        ?>
        <table width="100%" border="0" cellpadding="0" cellspacing="0" class="popup">
            <tr>
                <td>If you have an account, <a href="#">SIGN IN HERE</a></td>
            </tr>
            <tr>
                <td>
                    <table width="100%" border="0">
                        <tr>
                            <td class="poptit">First name:</td>
                            <td class="poptit">Last Name</td>
                        </tr>
                        <tr>
                            <td><input type="text" name="firstname" id="firstname" value=""/></td>
                            <td><input type="text" name="lastname" id="lastname" value=""/></td>
                        </tr>
                    </table>
                </td>
            </tr>

            <tr>

                <td class="poptit">E-mail Address:</td>
            </tr>
            <tr>
                <td><input type="text" name="emailid" id="emailid" value="" style="width:435px;"/></td>
            </tr>
            <tr>
                <td>
                    <table width="100%" border="0">
                        <tr>
                            <td class="poptit">Password</td>
                            <td class="subtit">Minimum 5 Characters</td>
                        </tr>
                        <tr>
                            <td colspan="2"><input type="text" name="firstname2" id="firstname2" value=""
                                                   style="width:435px;"/></td>
                        </tr>
                    </table>
                </td>
            </tr>
            <tr>
                <td><input type="submit" name="button" id="button" value="Submit"/></td>
            </tr>
        </table>
    </form>

</div>

<script type="text/javascript">
    var TabbedPanels1 = new Spry.Widget.TabbedPanels("TabbedPanels1"<?php if(!empty($_REQUEST['files'])) echo ', {"defaultTab" : 2}' ?>);
//    $(".discussion_link").livequery("click", function (e) {
//        e.preventDefault();
//        openRSVPDialog();
//    });
</script>
<div id='discussion_popup' style='display:none;width:400px!important;'>
    <form method='post' action='' id="rsvp-form">
        <?= $popupDate ?>
        <p><?= $popupDescription; ?></p>
        Accept?
        <input type='radio' name='attending' value="confirmed">YES
        <input type='radio' name='attending' value="declined">NO
        <input type='radio' name='attending' value="maybe">MAYBE
    </form>
    <div class="result" style="display:none; border-top: 1px solid #d6d6d8; padding-top: 10px; margin-top: 10px;"></div>
</div>
<div id="payment" style="display:none">
    <table width="100%" border="0" cellpadding="0" cellspacing="0">
        <tr>
            <td>To Join, Proceed with Payment</td>
        </tr>
        <tr>
            <td>
                <table width="100%" border="0">
                    <tr>
                        <td class="poptit">Cost:</td>
                        <td class="poptit">$<?php echo $cost; ?></td>
                    </tr>
                    <tr>
                        <td></td>
                        <td>
                            <!-- <form action="https://www.sandbox.paypal.com/cgi-bin/webscr" method="post"> -->
                            <form action="https://www.paypal.com/cgi-bin/webscr" method="post">
                                <input type="hidden" name="cmd" value="_xclick">
                                <input type="hidden" name="business" value="clewed@clewed.com">
                                <!-- <input type="hidden" name="business" value="test.business@gmail.com" /> -->
                                <input type="hidden" name="item_name"
                                       value="<?php echo $P_username . ". " . $disc_date . " discussion: " . $disc_title; ?>">
                                <!--<input type="hidden" name="item_number" value="<?php echo "'" . $disc_title . "', " . $disc_date; ?>">
	 <input type="hidden" name="item_name" value="<?php echo $disc_date; ?>">
   <input type="hidden" name="item_number" value="1">-->
                                <input type="hidden" name="amount" value="<?php echo str_replace('$', '', $cost); ?>">
                                <input type="hidden" name="no_shipping" value="0">
                                <input type="hidden" name="no_note" value="1">
                                <input type="hidden" name="currency_code" value="USD">
                                <input type="hidden" name="lc" value="AU">
                                <INPUT TYPE="hidden" NAME="return" value="<?php echo $paymenturl1; ?>">
                                <input type="hidden" name="custom_SauceColor">
                                <input type="hidden" name="custom_HowHot">
                                <!--<input type="hidden" name="bn" value="PP-BuyNowBF">-->
                                <!--<input type="hidden" name="hosted_button_id" value="58JBKVTL2D9JW">-->
                                <input type="hidden" name="hosted_button_id" value="58JBKVTL2D9JW">
                                <!--<input type="image" src="https://www.paypalobjects.com/en_US/i/btn/btn_buynowCC_LG.gif" border="0" name="submit" alt="PayPal - The safer, easier way to pay online!">-->
                                <img alt="" border="0" src="https://www.paypalobjects.com/en_US/i/scr/pixel.gif"
                                     width="1" height="1">

                                <input type="hidden" name="rm" value="2">
                                <!-- <input type="image" src="https://www.paypal.com/en_AU/i/btn/btn_buynow_LG.gif" border="0" name="submit" alt="PayPal - The safer, easier way to pay online.">-->
                                <div class="diss">
                                    <input type="submit" id="payment" class="small button"
                                           style="width: 212px !important;" value="Proceed With Payment">
                                </div>
                                <img alt="" border="0" src="https://www.paypalobjects.com/en_US/i/scr/pixel.gif"
                                     width="1" height="1">
                            </form>
                        </td>
                    </tr>
                </table>
            </td>
        </tr>
    </table>

</div>
