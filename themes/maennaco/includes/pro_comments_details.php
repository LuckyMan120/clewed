<?php
use Clewed\Insights\InsightHelper;
use Clewed\Insights\InsightService;

error_reporting (E_ALL ^ E_NOTICE);
    date_default_timezone_set('EST');

    global $base_url;
    global $user;
    global $AccessObj;

    $user_id = $user->uid;
    include_once __DIR__ . '/dbcon.php';
    include_once __DIR__ . '/safe_functions.inc';
    include_once __DIR__ . '/../blocks/profile/expert_review_form.php';

    // find out the domain:
    $domain = $_SERVER['HTTP_HOST'];
    // find out the path to the current file:
    $path = $_SERVER['SCRIPT_NAME'];
    // find out the QueryString:
    $queryString = $_SERVER['QUERY_STRING'];
    $queryString = str_replace('&payment_status=Completed', '', $queryString);
    $queryString = htmlentities($queryString);
    // put it all together:
    $paymenturl1 = "http://" . $domain . $path . "?" . $queryString;
    // An alternative way is to use REQUEST_URI instead of both
    // SCRIPT_NAME and QUERY_STRING, if you don't need them seperate:
    $paymentur2 = "http://" . $domain . $_SERVER['REQUEST_URI'];
    //echo "The alternative way: " . $url2;
    $id = $_REQUEST['id'];
    $uname = $user->name;

    $page = sget($_REQUEST, 'page');
    $type = sget($_REQUEST, 'type');
    $id = sget($_REQUEST, 'id');
    $pro_id = (int)sget($_REQUEST, 'pro_id');

    if (!InsightHelper::seenInsight($pro_id)) {
        $iService = new InsightService;
        $iService->addView($pro_id);
    }
    $result1 = mysql_query("SELECT * FROM `maenna_professional` WHERE id = '" . $pro_id . "' ORDER BY id DESC ");
    $row1 = mysql_fetch_array($result1);

    $result3 = mysql_query("SELECT * FROM `like_discussion_professional` WHERE  prof_id = '" . (int)$pro_id . "' and user_id = '" . (int)$id . "'");

    $likepost = mysql_num_rows($result3);
    $row3 = mysql_fetch_array($result3);

    $resPayment = mysql_query("SELECT * FROM `maenna_professional_payments` WHERE pro_id = '" . $pro_id . "' and user_id = '" . $user_id . "' ") or die(mysql_error());
    $checkUserCount = mysql_num_rows($resPayment);
    $checkUser = mysql_fetch_array($resPayment);

    $sql_expertise = mysql_query("SELECT * FROM `maenna_people` WHERE `pid` = '" . $row1['postedby'] . "'");
    $sql_exp_result = mysql_fetch_array($sql_expertise);

    $utype = getUserTypeById($row1['postedby']);
    if ($utype == 'admin') {
        $P_username = 'Admin';
    } else if ($utype == 'super_admin') {
        $P_username = 'Clewed';
    } else if ($sql_exp_result['username_type'] == 1) {
        $P_username = ucfirst($sql_exp_result['firstname']);
    } else {
        $P_username = ucfirst($sql_exp_result['firstname']) . ' ' . ucfirst($sql_exp_result['lastname']);
    }

    $cost = $row1['cost'];
    $disc_title = $row1['title'];
    $disc_date = date("m/d/Y", $row1['datetime']);

    if (!defined('POST_TYPE_PUBLIC_MINUTE')) {
        define('POST_TYPE_PUBLIC_MINUTE', 'm');
    }
    if (!defined('POST_TYPE_PRIVATE_MINUTE')) {
        define('POST_TYPE_PRIVATE_MINUTE', 'pm');
    }
    if (!defined('POST_TYPE_PUBLIC_QUESTION')) {
        define('POST_TYPE_PUBLIC_QUESTION', 'q');
    }
    if (!defined('POST_TYPE_PRIVATE_QUESTION')) {
        define('POST_TYPE_PRIVATE_QUESTION', 'pq');
    }

    $ifAdmin = ifAdmin($user_id);

    $ifAttendee = ($checkUser['status'] == 1);
    $ifInsightOwner = ($row1['postedby'] == $user_id);
    $ifInsightAdmin = ($AccessObj->user_type == 'super' || $ifAdmin || ifDiscussionModerator($pro_id, $user->uid));
    $accessAllowed = $ifAttendee || $ifInsightOwner || $ifInsightAdmin;


    //Redirect user returned from payment to his attended private insight
    $insightRepository = new \Clewed\Insights\InsightRepository();
    $insight = $insightRepository->findById($pro_id);
    if ($insight && $insight->isPrivateTemplateInsight() &&
        (isset($_REQUEST['payer_id'], $_REQUEST['payer_status']) || isset($_REQUEST['attended']))) { //if user returned from payment
        $attendedInsight = $insightRepository->findAttendedByTemplateInsight($insight->id, $user_id);
        if ($attendedInsight) {
            echo '<script type="text/javascript">
                    window.location.href="/account?tab=professionals&page=pro_detail&id=' . $id . '&section=pro_industry_view&type=details&pro_id=' . $attendedInsight->id . '"
                 </script>';
        }
    }

    $isService = false;
    if ($insight->type == 1 || $insight->type == 2) {
        $isService = true;
    }

    if ($_POST['post_comment'] != '') {
        mysql_query("INSERT INTO pro_wall_posts_comments (d_id,post_id,user_id,comment,username,datecreated) VALUES('" . $_REQUEST['dis_id'] . "','" . $_REQUEST['post_id'] . "','" . $user_id . "','" . checkValues($_REQUEST['post_comment']) . "','" . $uname . "','" . strtotime(date("Y-m-d H:i:s")) . "')");
        $ins_id = mysql_insert_id();
        $url    = "/account?tab=professionals&page=pro_detail&id=" . $id . "&section=pro_industry_view&type=details&tabs=2&pro_id=" . $_REQUEST['dis_id'] . "#com" . $ins_id . "";
        header("location:" . $url . "");
        exit;
    }

    if ($_POST['dis_posts'] != '' && !empty($_GET['editComments'])) {

        mysql_query("UPDATE pro_wall_posts_comments SET comment = '" . ($_REQUEST['dis_posts'])."' WHERE cid = ".$_GET['editComments']);
        $ins_id = $_GET['editComments'];
        $url = "/account?tab=professionals&page=pro_detail&id=" . $id . "&section=pro_industry_view&type=details&tabs=2&pro_id=" . $_REQUEST['prof_id'] . "#com" . $ins_id . "";
        header("location:" . $url . "");
        exit;
    }

    if ($_POST['dis_posts'] != '' && !empty($_GET['editMinutes'])) {

        mysql_query("UPDATE pro_wall_posts SET post = '" . (mysql_real_escape_string($_REQUEST['dis_posts']))."',tags = '".mysql_real_escape_string($_REQUEST['tags'])."' WHERE pid = ".$_GET['editMinutes']);
        $ins_id = $_GET['editMinutes'];
        $url = "/account?tab=professionals&page=pro_detail&id=" . $id . "&section=pro_industry_view&type=details&tabs=2&pro_id=" . $_REQUEST['prof_id'] . "&post_id=" . $ins_id . "#dis_post" . $ins_id . "";
        header("location:" . $url . "");
        exit;
    }

    if ($_POST['dis_posts'] != '' && empty($_GET['editMinutes'])) {
        mysql_query("INSERT INTO pro_wall_posts (pro_id,post,f_name,user,tags,flag,date_created) VALUES('" . $_POST['prof_id'] . "','" . checkValues($_REQUEST['dis_posts']) . "','" . $uname . "','" . $user_id . "','" . $_REQUEST['tags'] . "','" . $_REQUEST['flag'] . "','" . strtotime(date("Y-m-d H:i:s")) . "')");
        $ins_id = mysql_insert_id();
        mysql_query("INSERT INTO notifications (action, state, author, item, created) VALUES ('minutes_new', 1, " . ((int)$_POST['dis_id']) . ", " . ((int)$_POST['prof_id']) . ", NOW())");
        $url = "/account?tab=professionals&page=pro_detail&id=" . $id . "&section=pro_industry_view&type=details&tabs=2&pro_id=" . $_REQUEST['prof_id'] . "&post_id=" . $ins_id . "#dis_post" . $ins_id . "";
        header("location:" . $url . "");
        exit;
    }

?>
<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery-ui-timepicker-addon.js"></script>
<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.autosuggest.js"></script>
<link href="/themes/maennaco/jui/comments/css/screen.css?as" type="text/css" rel="stylesheet"/>
<link href="/themes/maennaco/jui/comments/css/autosuggest.css" type="text/css" rel="stylesheet"/>
<link href="/themes/maennaco/jui/comments/css/fileuploader.css" type="text/css" rel="stylesheet"/>
<link href="/themes/maennaco/jui/comments/css/SpryTabbedPanels.css" type="text/css" rel="stylesheet"/>
<script src="/themes/maennaco/jui/comments/js/jquery.elastic.js" type="text/javascript" charset="utf-8"></script>
<script src="/themes/maennaco/jui/comments/js/fileuploader.js" type="text/javascript" charset="utf-8"></script>
<script src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js" type="text/javascript"></script>
<script src="/themes/maennaco/jui/comments/js/SpryTabbedPanels.js" type="text/javascript"></script>

<script type="text/javascript">
$(document).ready(function () {
    init_follow();
    init_rsvp_invite();

    if('#pm' == location.hash) {
        location.hash = '';
        $('#tab4').trigger('click');
    }

    if('#msg' == location.hash) {
        location.hash = '';
        $('#tab3').trigger('click');
    }

    $(".profile_details").click(function (e) {
        e.preventDefault();
        var eventid = '';
        if ($(this).attr('ref') == 'pro_id') {
            eventid = 'pro_id=' + $(this).attr('id').replace('pro_id', '');
        } else {
            eventid = 'eventid=' + $(this).attr('id').replace('edit_id', '');
        }
        uid = <?= $user->uid; ?>;
        $.post("/themes/maennaco/includes/pro_posts.php?type=profileInfo&display=true&" + eventid + "&uid=" + uid + "&base_url=<?php echo $base_url; ?>", {
        }, function (response) {
            $("#eveditdlg").dialog({
                autoOpen: true,
                width: 650,
                title: 'PROFILE',
                resizable: false,
                draggable: false,
                height: 400,
                closeText: 'hide',
                buttons: {
                }, closeOnEscape: true,
                modal: true
            }).html(response);
        });
    });
    <?php if($_REQUEST['tabs'] == 2) { ?>
    $('#tab3').trigger('click');
    <?php } ?>

    <?php if ($accessAllowed) { ?>
        var interval_id;
        $(window).focus(function() {
            if (!interval_id) {
                interval_id = setInterval(pullNewPosts, 5000);
            }
        });

        $(window).blur(function() {
            clearInterval(interval_id);
            interval_id = 0;
        });
    <?php } ?>

    /***** requirements tab *****/
    // Add question
    $('#addQuestionButton').click(function () {
        $(this).parent().hide();
        $('#newQuestion').show();
    });
    $('#saveQuestionButton').click(function () {
        var questionText = $('#newQuestionText').val();
        if (questionText.length > 0) {
            $.post(window.location.href + '&panel=add_question', {
                questionText: questionText
            }, function (response) {
                alert("Question created.");
                window.location.replace(window.location.origin + '<?= rebuild_url(array('tab', 'page', 'id', 'section', 'type', 'pro_id')); ?>' + '&view_tab=requirements');
            });
        } else {
            alert("Please type your question.");
        }
    });
    // Add answer
    $('#addAnswerButton').click(function () {
        window.location.replace(window.location.origin + '<?= rebuild_url(array('tab', 'page', 'id', 'section', 'type', 'pro_id')); ?>' + '&view_tab=requirements&view_type=edit_answers');
    });
    // Save answer
    $('#submitAnswersButton').click(function () {
        var answers = [];
        $('.new_answer_text').each(function () {
            var answerId = parseInt($(this).attr('answer-id'));
            var questionId = parseInt($(this).attr('question-id'));
            var answerText = $(this).val();
            answers.push({
                answer_id: answerId ? answerId : 0,
                question_id: questionId ? questionId : 0,
                answer_text: answerText ? answerText : ''
            });
        });
        if (answers.length) {
            $.post(window.location.origin + '<?= rebuild_url(array('tab', 'page', 'id', 'section', 'type', 'pro_id')); ?>' + '&panel=save_answers', {
                answers: answers
            }, function (response) {
                alert("Answers saved successfully.");
                window.location.replace(window.location.origin + '<?= rebuild_url(array('tab', 'page', 'id', 'section', 'type', 'pro_id')); ?>' + '&view_tab=requirements');
            });
        } else {
            alert('There are no answers to save.');
        }
    });
    /***** deliverables tab *****/
    $('#edit_milestones_action').click(function () {
        $.post('/themes/maennaco/includes/posts.php?type=eventMilestonesEdit&display=true&eventid=<?= $insight->id ?>', {
            // request data
        },
        function (response) {
            $('#service-update-milestones-dlg').dialog({
                autoOpen: true,
                width: 650,
                title: 'MILESTONES EDITOR',
                height: 600,
                buttons: {
                    "Save": {
                        text: "Save",
                        'class': "form-submit1",
                        click: function () {
                            var milestones = [];
                            // getting milestones data
                            $('.milestone-data').each(function() {
                                var id = $(this).attr('id'),
                                    duration = parseInt($(this).find('select').val()),
                                    desc = $(this).find('input').val();
                                var currentObject = {
                                    description: desc,
                                    duration: duration ? duration : ''
                                }
                                if (id) {
                                    currentObject.id = parseInt(id.replace('ms_id', ''));
                                }
                                milestones.push(currentObject);
                            });
                            $.post(window.location.origin + '<?= rebuild_url(array('tab', 'page', 'id', 'section', 'type', 'pro_id')); ?>' + '&panel=save_milestone', {
                                milestones: milestones
                            }, function (response) {
                                // refresh
                                alert('Milestones saved.');
                                window.location.replace(window.location.origin + '<?= rebuild_url(array('tab', 'page', 'id', 'section', 'type', 'pro_id')); ?>' + '&view_tab=deliverables');
                            });
                        }
                    },
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
        });
    });
    $('#edit_approved_milestones_action').click(function () {
        alert('Milestones are approved․ You can\'t change them.');
    });

    $('#approve_milestones_action').click(function () {
        var today = new Date();
        var dd = String(today.getDate()).padStart(2, '0');
        var mm = String(today.getMonth() + 1).padStart(2, '0');
        var yyyy = today.getFullYear();
        today = mm + '/' + dd + '/' + yyyy;
        // show message
        var result = confirm("This action will lock the milestones as of " + today + " and will start the milestone duration tracking to start.");
        if (!result) {
            return;
        }
        // check milestones exists
        if (!$('.singleMilestone').length) {
            alert('There are no milestones to approve.');
            return;
        }
        $.post(window.location.origin + '<?= rebuild_url(array('tab', 'page', 'id', 'section', 'type', 'pro_id')); ?>' + '&panel=approve_milestones', {
            // no params needed
        }, function (response) {
            alert('Milestones approved.');
            window.location.replace(window.location.origin + '<?= rebuild_url(array('tab', 'page', 'id', 'section', 'type', 'pro_id')); ?>' + '&view_tab=deliverables');
        });
    });
    $('#approved_milestones_action').click(function () {
        alert('The milestones have already been approved.');
    });
    var viewTab = "<?= $_REQUEST['view_tab']; ?>";
    if (viewTab == 'requirements') {
        $('#tab2').click();
    } else if (viewTab == 'deliverables') {
        $('#tab4').click();
    } else if ("<?= $_REQUEST['file'] ?>") {
        $('#tab5').click();
    }
});

$("#dialog").dialog({
    autoOpen: false
});

$("#join").livequery("click", function (e) {
    e.preventDefault();
    $("#createaccount").dialog({
        autoOpen: true,
        width: 450,
        title: 'CREATE AN ACCOUNT',
        height: 300,
        closeOnEscape: true,
        modal: true
    }).html();
});

$("li.pop_qs_").livequery("click", function (e) {

    TabbedPanels1.showPanel(1);

    var rel = $(this).attr('rel');
    $('html, body').animate({
        scrollTop: $("#dis_post"+rel).offset().top
    }, 500);
});

function postNewEntry(postId, flag, entryContent)
{
    var container = (flag == 'pm' || flag == 'pq') ? '.tabtags.private-tab' : '.tabtags.public-tab';
    container = $(container);
    container.prepend($(entryContent).show());

    postId = parseInt(postId);
    if (postId > last_post_id) {
        last_post_id = postId;
    }
    return container;
}

function pullNewPosts()
{
    var dissid = <?=json_encode($_REQUEST['pro_id']);?>;
    var m = '<?=md5($_REQUEST['pro_id'].'kyarata75');?>';
    var pro_profile = <?=json_encode($_REQUEST['id']);?>;
    var requester = <?=json_encode($user->uid);?>;

    $.post("/themes/maennaco/includes/add_comment.php?type=pro_dis_comment&action=get_new_posts",
        {
            dissid: dissid, m: m, pro_profile: pro_profile, last_pid: last_post_id, requester: requester
        },
        function (response) {
            if (response.status == 'success') {

                for(i in response.posts) {
                    var post = response.posts[i];
                    postNewEntry(post.pid, post.flag, $(post.html).fadeIn());
                }
                pullNewComments();
            }
        }, "json"
    );
}

function pullNewComments()
{
    var dissid = <?=json_encode($_REQUEST['pro_id']);?>;
    var m = '<?=md5($_REQUEST['pro_id'].'kyarata75');?>';
    var pro_profile = <?=json_encode($_REQUEST['id']);?>;
    var requester = <?=json_encode($user->uid);?>;

    $.post("/themes/maennaco/includes/add_comment.php?type=pro_post_comment&action=get_new_comments",
        {
            dissid: dissid, m: m, pro_profile: pro_profile, last_comment_id: last_comment_id, requester: requester
        },
        function (response) {
            if (response.status == 'success') {

                for(i in response.comments) {
                    var comment = response.comments[i];
                    $('#comments_' + comment.pid).append($(comment.html).fadeIn());
                    if (parseInt(comment.cid) > last_comment_id) {
                        last_comment_id = parseInt(comment.cid);
                    }
                }
            }
        }, "json"
    );
}


$("#question_post").livequery("click", function (event) {
    event.preventDefault();
    var text = tinyMCE.get('dis_posts_question').getContent();
    var dissid = <?=json_encode($_REQUEST['pro_id']);?>;
    var editor = '<?=$user_id;?>';
    var pro_profile = <?=json_encode($_REQUEST['id']);?>;
    var m = $(this).attr('m');
    var flag = $(this).attr('flag');
    var tags = $("#tags_question :selected").val();
    var sel_obj = $(this);

    if(!validate_questions()){
        return;
    }
    sel_obj.attr("disabled", "disabled");

    $.post("/themes/maennaco/includes/add_comment.php?type=pro_dis_comment",
        {
            dissid: dissid, text: text, editor: editor, m: m, tags: tags, pro_profile: pro_profile, flag: flag
        },
        function (response) {
            sel_obj.removeAttr("disabled");
            if (response.status == 'success') {

                var newPid = response.pid;
                $("#dis_posts_question").val('');
                tinyMCE.get('dis_posts_question').setContent('');
                $("#tags_question :selected").removeAttr("selected");
                $('#question').hide();
                $('.new_tool').removeClass('active');

                postNewEntry(newPid, response.flag, response.display);

            } else {
                (alert("Your request din`t go through. Please try again!"));
            }
        }, "json");
});

$("#minutes_post").livequery("click", function (event) {

    event.preventDefault();
    var text = tinyMCE.get('dis_posts').getContent();
    var pid = $('#min_id').val();
    var dissid = <?=json_encode($_REQUEST['pro_id']);?>;
    var editor = '<?=$user_id;?>';
    var pro_profile = <?=json_encode($_REQUEST['id']);?>;
    var m = $(this).attr('m');
    var flag = $(this).attr('flag');
    var tags = $("#tags :selected").val();
    var sel_obj = $(this);

    if(!validate_minutes()) {
        return;
    }

    sel_obj.attr("disabled", "disabled");
    $.post("/themes/maennaco/includes/add_comment.php?type=pro_dis_comment",
        {
            dissid: dissid, text: text, editor: editor, m: m, tags: tags, pro_profile: pro_profile, flag: flag, pid: pid
        },
        function (response) {
            sel_obj.removeAttr("disabled");
            if (response.status == 'success') {
                var newPid = response.pid,
                    container = $("#dis_post" + newPid);
                var postContent = $(response.display).find('#edit_minutes_' + newPid).html();

                if (container.html()) {
                    container.find('.post_content').html(postContent)
                } else {
                    container = postNewEntry(newPid, response.flag, response.display);
                }
                container.show();

                $("#dis_posts").val('');
                $("#tags :selected").removeAttr("selected");
                $('#minutes').hide();
                $('.new_tool').removeClass('active');
                tinyMCE.get('dis_posts').setContent('');
                show_hidden_minutes();
            } else {
                (alert("Your request din`t go through. Please try again!"));
            }
        }, "json");
});

$('input[id^="post_com"]').livequery("click", function (event) {
    event.preventDefault();
    var form = $(this).parents('form#comments');
    var post_id = form.find('#post_id').val();
    var text = $(this).siblings('textarea[name="post_comment"]').val();
    var editor = '<?=$user_id;?>';
    var pro_profile = '<?=$_REQUEST['id'];?>';
    var m = $(this).attr('m');
    var comment_id = form.find('#comment_id').val();
    var isPrivate = 'private' == form.find('input[name=comment_type]').val();
    var sel_obj = $(this);
    if (text == '') {
        alert('Please type your comment.');
        return false;
    }
    sel_obj.attr("disabled", "disabled");
    $.post("/themes/maennaco/includes/add_comment.php?type=pro_post_comment",
        {
            post_id: post_id,
            text: text,
            editor: editor,
            m: m,
            pro_profile: pro_profile,
            comment_id: comment_id,
            is_private: isPrivate ? 1 : 0
        },
        function (response) {

            sel_obj.removeAttr('disabled');
            if (response.status == 'success') {

                if (comment_id) {
                    var content = $(response.display).find('#com' + comment_id).html();
                    $('#com' + comment_id).html(content);
                } else {
                    $('#comments_' + post_id).append($(response.display).fadeIn());
                    sel_obj.siblings('textarea[name="post_comment"]').val('');
                    if (parseInt(response.cid) > last_comment_id) {
                        last_comment_id = parseInt(response.cid);
                    }
                }
                $(sel_obj).parents('.w').hide();
            } else {
                (alert("Your request din`t go through. Please try again!"));
            }
        }, "json");
});

$("#showFile").livequery("click", function () {
    var pathArray = window.location.href.split('/');
    var host = pathArray[2];
    var sel_obj = $(this);
    var proba = '<form action="" method="post" name="postsForm"><div class="UIComposer_Box"><span class="w"><textarea class="input" id="watermark" name="watermark" style="height:30px" cols="64"></textarea></span><br clear="all" /><div id="submitDiv" align="right" style="display:none; height:80px; "><a id="closeButton" class="tool">Close</a><a  </div><div style="clear: both;"></div></div></form>';
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

    $.post("/themes/maennaco/includes/fetch.php?type=checkCompleted",
        {
            uid: <?=$user->uid;?>,
            utype: '<?=$AccessObj->user_type;?>'
        },
        function (response) {
            if (response == 'true') {
                $("#payment").dialog({
                    autoOpen: true,
                    width: 450,
                    title: 'PROCEED WITH PAYMENT',
                    height: 310,
                    buttons:[{
                        text: "Join",
                        click: function () {
                        },
                        "class": "payment-submit"
                    },
                        {
                        text: "Cancel",
                        click: function () {
                            $(this).dialog("close");
                        },
                        "class": "payment-cancel"
                    }
                        ], closeOnEscape: true,
                    modal: true
                }).show();
            } else {
                alert(response);
                return false;
            }
        }
    );
}

function showsubmit(id) {
    $('#post_com' + id).show();
}

function submitcomment(id) {
    $.post("/themes/maennaco/includes/pro_comments_list.php?type=pro_comments&prof_id=" + prof_id + "&u=" + u + "&m=" + m + "&value=" + a + "&proid=" + proid + "&uid=" + uid);
}

<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

$("a.upload_file_action").livequery("click", function (e) {
    e.preventDefault();
    eventid = $(this).attr('id').replace('editmat_id', '');
    $.post("/themes/maennaco/includes/pro_posts_files.php?type=attachFile&reason=edit",
        function (response) {
            $("#editmatdlg").dialog({
                autoOpen: true,
                width: 500,
                title: 'File attachment',
                resizable: false,
                draggable: false,
                height: 310,
                buttons: {"Save": function () {
                        $.post("/themes/maennaco/includes/pro_posts_files.php?type=editInsight", {eventid: eventid, str: $(editEventForm).serialize(), username: '<?= $user->name; ?>', user_id: '<?= $user->uid; ?>', user_fname: '<?= getUserById($user->uid); ?>'
                        }, function (response) {
                            alert(response);
                        });
                        $(this).dialog("close");
                    },
                    "Cancel": function () {
                        $(this).dialog("close");
                    }
                }
            }).html(response);
            var uploader2 = new qq.FileUploader({
                // pass the dom node (ex. $(selector)[0] for jQuery users)
                element: $("#file-uploader2")[0],
                dataType: 'json',
                params: {
                    pro_id: eventid
                },
                // path to server-side upload script
                action: '/themes/maennaco/includes/file_upload.php',
                onComplete: function (id, fileName, responseJSON) {
                    if (responseJSON['success']) {
                        $("#editEventForm").append('<input name="fileupl" type="hidden" value="' + responseJSON['timestamp'] + "_" + fileName + '"  class="fileInfo" path="' + responseJSON['timestamp'] + "_" + fileName + '" filetitle="' + $("#fileTitleEdit").val() + '" timestamp = "' + responseJSON['timestamp'] + '">');
                    }
                }
            });
        });
});
$("a.custom_upload_file_action").livequery("click", function (e) {
    e.preventDefault();
    eventid = $(this).attr('id').replace('editmat_id', '');
    $.post("/themes/maennaco/includes/pro_posts_files.php?type=attachFile&reason=edit",
        function (response) {
            $("#editmatdlg").dialog({
                autoOpen: true,
                width: 500,
                title: 'File attachment',
                resizable: false,
                draggable: false,
                height: 310,
                buttons: {
                    "Save": function () {
                        var formData = new FormData();
                        formData.append('file', $('#service_file').prop('files')[0]);
                        formData.append('eventid', eventid);
                        formData.append('str', '');
                        formData.append('file_name', $('#file_name').val());
                        formData.append('username', '<?= $user->name; ?>');
                        formData.append('user_id', '<?= $user->uid; ?>');
                        formData.append('user_fname', '<?= getUserById($user->uid); ?>');
                        $.ajax({
                            type: 'POST',
                            enctype: 'multipart/form-data',
                            url: '/themes/maennaco/includes/pro_posts_files.php?type=editInsight',
                            data: formData,
                            processData: false,
                            contentType: false,
                            cache: false,
                            success: function (response) {
                                alert(response);
                            },
                            error: function (error) {
                                console.log('ERROR: ', error);
                            }
                        });
                        $(this).dialog("close");
                    },
                    "Cancel": function () {
                        $(this).dialog("close");
                    }
                }
            }).html(response);
            $('#evFileEdit').replaceWith(`<div style='margin-bottom:15px;display:inline-block;'>
                <br><input style='width:197px!important;height:22px;margin-bottom:10px;' id='file_name' type='text' name='file_name' placeholder='NAME' />
                <br><input style='margin-bottom: 10px;' id='service_file' type='file' name='service_file' />
            </div>`);
        });
});
$(".evedit").livequery("click", function (e) {
    e.preventDefault();
    var eventid = $(this).attr('id').replace('edit_id', '');
    $.post("/themes/maennaco/includes/pro_posts.php?type=eventEdit&display=true&eventid=" + eventid,
        function (response) {
            $("#eveditdlg").dialog({
                autoOpen: true,
                width: 500,
                title: 'Event edit',
                resizable: false,
                draggable: false,
                open: function () {
                    $(this).closest(".ui-dialog").find(".ui-button:first").next().find(".ui-button-text").addClass("uicancel");
                },
                height: 600,
                buttons: {
                    "Save": function () {
                        links = '';
                        name = '';
                        title = $("#editEventForm").children("#eventType").val();
                        var name = new Array();
                        var links = new Array();
                        $("#text input[name=name]").each(function () {
                            name.push($(this).val());
                        });
                        var taskArray = new Array();
                        $("#text input[name=link]").each(function () {
                            links.push($(this).val());
                        });
                        names = name;
                        datetime = $("#editEventForm").children("#date").val();
                        loc = $("#editEventForm").children("#eventLoc").val();
                        desc = $("#editEventForm").children("#eventDesc").val();
                        whyattend = $("#editEventForm").children("#whyattend").val();
                        eventcost = $("#editEventForm").children("#eventCost").val();
                        eventcapacity = $("#editEventForm").children("#eventCapacity").val();
                        eventtags = $("#editEventForm").children("#eventTags").val();
                        eventindustry = $("#editEventForm").children("#eventIndustry").val();
                        buyer_requirement = $("#editEventForm").children("#buyer_requirement").val();
                        uname = '<?= $user->name; ?>';
                        <?php
                        if ($_REQUEST['id'] == '') {
                            $user_id = $user->uid;
                        } else {
                            $user_id = $_REQUEST['id'];
                        }
                        ?>
                        cid = <?= $user_id; ?>;
                        uid = <?= $user->uid; ?>;
                        filesEdit = [];
                        if ($("#chkNot").is(":checked")) {
                            notif = 'true';
                        } else {
                            notif = false;
                        }
                        invitees = $("#editEventForm").children(".as-selections").children(".as-original").children(".as-values").val();
                        $(".fileInfo").each(function () {
                            var tmpArr1 = {'path': $(this).attr('path'), 'title': $(this).attr('filetitle'), 'timestamp': $(this).attr('timestamp')};
                            filesEdit.push(tmpArr1);
                        });
                        $.post(
                            '/themes/maennaco/includes/pro_posts.php?type=eventEdit',
                            {
                                eventid: eventid,
                                title: title,
                                loc: loc,
                                whyattend: whyattend,
                                desc: desc,
                                date: datetime,
                                buyer_requirement: buyer_requirement,
                                cost: eventcost,
                                capacity: eventcapacity,
                                tags: eventtags,
                                industry: eventindustry,
                                u: uname,
                                cid: cid,
                                uid: uid,
                                files: filesEdit,
                                notif: notif,
                                links: links,
                                names: names,
                                approve_discount: $('input[name="approve_discount"]').prop('checked') ? 1 : 0,
                                discount_enabled: $('input[name="use_discount"]').prop('checked') ? 1 : 0,
                                discount_rate: $('input[name="rate"]').val()
                            },
                            function (response) {
                                if (response.replace(/\s/g, "") == 'true') {
                                    alert('Your update was sucessfully saved');
                                    location.reload();
                                } else {
                                    alert('Something went wrong. Some of your data may not be saved.');
                                }
                            }
                        );
                    },
                    "Close": function () {
                        if (confirm('Are you sure you want to close without saving?')) $(this).dialog("close");
                    }
                },
                closeOnEscape: true,
                modal: true
            }).html(response);

            $("#eventDesc").elastic();
            $(".as-selections").width(600);
            var uploader1 = new qq.FileUploader({
                // pass the dom node (ex. $(selector)[0] for jQuery users)
                element: $("#file-uploader1")[0],
                dataType: 'json',
                params: {
                    pro_id: eventid
                },
                // path to server-side upload script
                action: '/themes/maennaco/includes/file_upload.php',
                onComplete: function (id, fileName, responseJSON) {
                    if (responseJSON['success']) {
                        $("#eventType").before('<input type="hidden" class="fileInfo" path="' + fileName + '" filetitle="' + $("#fileTitleEdit").val() + '" timestamp = "' + responseJSON['timestamp'] + '">');
                        $("#fileTitleEdit").val('');
                    }
                }
            });
        }
    );
});

/********* Attach Files  *********/
$(".file").livequery("click", function (e) {
    e.preventDefault();
    fileid = $(this).attr('id').replace('file_id', '');
    $.post("/themes/maennaco/includes/pro_posts_files.php?type=attachFile",
        function (response) {
            $("#eveditdlg").dialog({
                autoOpen: true,
                width: 500,
                title: 'File Attachment',
                height: 320,
                buttons: {
                    "Save": function () {
                        title = $("#editEventForm #event").children("#eventType").val();
                        var name = new Array();
                        var links = new Array();
                        $("#text input[name=name]").each(function () {
                            name.push($(this).val());
                        });
                        var taskArray = new Array();
                        $("#text input[name=link]").each(function () {
                            links.push($(this).val());
                        });
                        datetime = $("#editEventForm").children("#eventDate").val();
                        loc = $("#editEventForm").children("#eventLoc").val();
                        agenda = $("#editEventForm").children("#eventDesc").val();
                        uname = '<?= $user->name; ?>';
                        <?php
                        if ($_REQUEST['id'] == '') {
                            $user_id = $user->uid;
                        } else {
                            $user_id = $_REQUEST['id'];
                        }
                        ?>
                        cid = <?= $user_id; ?>;
                        uid = <?= $user->uid; ?>;
                        filesEdit = [];
                        if ($("#chkNot").is(":checked")) {
                            notif = 'true';
                        } else {
                            notif = false;
                        }
                        invitees = $("#editEventForm").children(".as-selections").children(".as-original").children(".as-values").val();
                        $(".fileInfo").each(function () {
                            var tmpArr1 = {'path': $(this).attr('path'), 'title': $(this).attr('filetitle'), 'timestamp': $(this).attr('timestamp')};
                            filesEdit.push(tmpArr1);
                        });

                        $.post("/themes/maennaco/includes/pro_posts_files.php?type=attachFile", {title: title, datetime: datetime, loc: loc, agenda: agenda, invitees: invitees, u: uname, cid: cid, uid: uid, files: filesEdit, notif: notif, names: name, links: links},
                            function (response) {
                                $('#eveditdlg').dialog("close");
                            });
                    },
                    "Close": function () {
                        if (confirm('Are you sure you want to close without saving?')) $(this).dialog("close");
                    }
                },
                closeOnEscape: true,
                modal: true
            }).html(response);
            $("#eventDesc").elastic();
            $(".as-selections").width(600);
            var uploader1 = new qq.FileUploader({
                // pass the dom node (ex. $(selector)[0] for jQuery users)
                element: $("#file-uploader1")[0],
                params: {
                    pro_id: $("#file-uploader1").data('pro-id')
                },
                // path to server-side upload script
                action: '/themes/maennaco/includes/file_upload.php',
                onComplete: function (id, fileName, responseJSON) {
                    if (responseJSON['success']) {
                        $("#eventType").before('<input type="hidden" class="fileInfo" path="' + fileName + '" filetitle="' + $("#fileTitleEdit").val() + '" timestamp = "' + responseJSON['timestamp'] + '">');
                        $("#fileTitleEdit").val('');
                    }
                }
            });
        });
});
/******** End Attach Files **********/

$('.deletepost').livequery("click", function (e) {
    var clicked = $(this).parents('.cmtloop');
    if (confirm('Are you sure you want to delete this Post?') == false) {
        return false;
    }
    e.preventDefault();
    var temp = $(this).attr('id').replace('deletepost', '');
    $.ajax({
        type: 'get',
        url: '/themes/maennaco/includes/delete.php?' +
            'type=professional_post&' +
            'id=' + temp+ "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",
        success: function () {
//            $('#dis_post' + temp).remove();

            clicked.fadeOut(200, function () {
                clicked.remove();
            });
        }
    });
    return true;
});

<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

$('.delete_comment').livequery("click", function (e) {
    var clicked = $(this).parent().parent().parent().parent();
    if (confirm('Are you sure you want to delete this Post?') == false) {
        return false;
    }
    e.preventDefault();
    var temp = $(this).attr('id').replace('delete_comment', '');
    $.ajax({
        type: 'get',
        url: '/themes/maennaco/includes/delete.php?' +
            'type=professional_comments&' +
            'id=' + temp + "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",
        success: function () {
            clicked.fadeOut(200, function () {
                clicked.remove();
            });
        }
    });
    return true;
});

function like_discussion(type, prof_id, userid) {
    if (type == 'like') {
        var status = 1;
    } else {
        var status = 0;
    }

    <?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

    $.ajax({
        type: 'get',
        url: '/themes/maennaco/includes/delete.php?' +
            'type=like_discussion&' +
            'prof_id=' + prof_id + '&' +
            'userid=' + userid + '&' +
            'status=' + status + "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",
        success: function () {
            if (type == 'like') {
                $('#likepost').html("<a style='cursor:pointer;' onclick='like_discussion(\"unlike\", " + prof_id + "," + userid + ");'>Unlike</a>");
            } else {
                $('#likepost').html("<a style='cursor:pointer;' onclick='like_discussion(\"like\", " + prof_id + "," + userid + ");'>Like</a>");
            }
        }
    });
}

<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

function like_posts(type, prof_id, post_id, userid) {
    if (type == 'like') {
        var status = 1;
    } else {
        var status = 0;
    }
    $.ajax({
        type: 'get',
        url: '/themes/maennaco/includes/delete.php?' +
            'type=like_posts&' +
            'prof_id=' + prof_id + '&' +
            'post_id=' + post_id + '&' +
            'userid=' + userid + '&' +
            'status=' + status + "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",
        success: function () {
            if (type == 'like') {
                $('#likepost1' + post_id).find('a').replaceWith("<a style='cursor:pointer;' onclick='like_posts(\"unlike\", " + prof_id + ", " + post_id + "," + userid + ");'>Unlike</a>");
                $('#likepost1' + post_id).find('.like_cnt').html( parseInt($('#likepost1' + post_id).find('.like_cnt').text())+1);
            } else {
                $('#likepost1' + post_id).find('a').replaceWith("<a style='cursor:pointer;' onclick='like_posts(\"like\", " + prof_id + ", " + post_id + "," + userid + ");'>Like</a>");
                $('#likepost1' + post_id).find('.like_cnt').html( parseInt($('#likepost1' + post_id).find('.like_cnt').text())-1);
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
        if (response == 'true') {

            <?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

            $.post("/themes/maennaco/includes/delete.php?" +
                "type=join&" +
                "prof_id=" + pro_id + "&" +
                "uid=" + id + "&" +
                "u=<?php echo $u; ?>&" +
                "m=<?php echo $m; ?>",
                function () {
                    var url = window.location.toString();
                    window.location = url.replace('#', '') + '&view_tab=requirements&attended=1';
                }
            );
        } else {
            alert(response);
            return false;
        }
    });
}


function validate_questions() {

    var editorContent = tinyMCE.get('dis_posts_question').getContent();
    // var editorContent = $('#dis_posts').val();
    if (editorContent == '' || editorContent == null || editorContent == '<div>&nbsp;</div>') {
        alert('Please type your question');
        return false;
    }
    if ($('#tags_question').is(':visible') && $('#tags_question').val() == '') {
        alert('Categories field is required');
        return false;
    }
    if (old_formaction == '') {
        old_formaction = $('#question form#comments').attr('action');
    } else {
        $('#question form#comments').attr('action', old_formaction);
    }
    return true;
}

var old_formaction = '';

$( "#cancel" ).livequery("click", function (e) {
    cancel_min_com();
});

function cancel_min_com(){
    show_hidden_minutes();
    show_hidden_comments();
    //$(".comts").hide();
}
function show_hidden_comments(){
    $('.aucomnts').show()
}

function show_hidden_minutes(){
    $('.cmtloop').show()
}

function show_minutes(ifPrivate) {
    cancel_min_com();
    $('#question').hide();
    $('#minutes').show();
    $('#dis_posts').val('').show();
    $("#tags").val('').show();
    $('#min_id').val('');

    $('#tags_question').toggle(!ifPrivate);
    $('#tags').toggle(!ifPrivate);

    if (old_formaction == '') {
        old_formaction = $('#minutes form#comments').attr('action');
    } else {
        $('#minutes form#comments').attr('action', old_formaction);
    }

    $('#minutes_post').attr('flag', ifPrivate ? 'pm' : 'm');

    tinyMCE.get('dis_posts').setContent('');
    moveEntryEditor('minutes', 'dis_posts', function() {
        var containerSelector = ifPrivate ? '.tabtags.private-tab' : '.tabtags.public-tab';
        $('.comts').prependTo(containerSelector).show();
    });
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

function n2br(text) {
    return text.replace(/\r?\n/g, '<br>');
}

function fileFormDisplay(id) {
    $('#record-' + id).find('.commentBox').show();
    $('#record-' + id).find('textarea').focus();
}

function fileFormHide(id) {
    $('#record-' + id).find('.commentBox').hide();
}

function submitFilePost(el) {

    var a = $("#watermark").val();
    var m = '<?= md5(((string)$user->uid).$_REQUEST['fid']."kyarata75")?>';
    var u = '<?= $user->uid; ?>';
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


function edit_minutes(pid) {

    cancel_min_com();

    $('#min_id').val(pid);
    var text = $('#edit_minutes_'+pid).html();
    var topic = $('#topic_'+pid).html();

    $('#dis_post'+pid).hide();
    $('#question').hide();
    $('#minutes').show();
    $('#dis_posts').val(text).show();
    $("#tags").val(topic).show();

    if (old_formaction == '') {
        old_formaction = $('#minutes form#comments').attr('action');
    }
    $('#minutes form#comments').attr('action', old_formaction + '&editMinutes=' + pid);


    tinyMCE.get('dis_posts').setContent(text);
    moveEntryEditor('minutes', 'dis_posts', function() {
        $(".comts").insertAfter($('#dis_post'+pid)).show()
    });
}


function validate_minutes() {

    var editorContent = tinyMCE.get('dis_posts').getContent();
    // var editorContent = $('#dis_posts').val();
    if (editorContent == '' || editorContent == null || editorContent == '<div>&nbsp;</div>') {
        alert('Content field is required');
        return false;
    }
    else if ($("select#tags").is(":visible") && $('select#tags').val() == '') {
        alert('Categories field is required');
        return false;
    }
    return true;
}

function ask_question(ifPrivate) {

    cancel_min_com();
    $('#minutes').hide();
    $('#question').show();
    $('#tags').val('');
    $('#min_id').val('');
    $('#dis_posts_question').val('').show();

    $('#tags_question').toggle(!ifPrivate);
    $('#tags').toggle(!ifPrivate);

    $('#question_post').attr('flag', ifPrivate ? 'pq' : 'q');

    tinyMCE.get('dis_posts_question').setContent('');
    moveEntryEditor('question', 'dis_posts_question', function() {
        var containerSelector = ifPrivate ? '.tabtags.private-tab' : '.tabtags.public-tab';
        $('.comts').prependTo(containerSelector).show();
    })
}

function moveEntryEditor(entryContainerId, entryContentId, movingCallback) {
    var oInstance = tinyMCE.getInstanceById(entryContentId);
    if (oInstance) {
        if (oInstance.isHidden()) tinyMCE.remove(oInstance);
        tinyMCE.execCommand('mceRemoveControl', true, entryContentId);
    }

    movingCallback();

    tinyMCE.execCommand('mceAddControl', true, entryContentId);
    $("#" + entryContainerId + " #" + entryContentId + "_parent:not(:first)").remove(); //clear tinyMCE's shit
}


function edit_comments(cid, min_id) {
    cancel_min_com();

    var text = $('#com'+cid).html();

    var commentBlock = $('#com' + cid).parents('.aucomnts');
    var commentForm = $('#form_id' + min_id);
    commentForm.insertAfter(commentBlock).show();
    commentForm.find('#post_comment' + min_id).val(text).focus();
    commentForm.find('#min_id').val(min_id);
    commentForm.find('#post_id').val(min_id);
    commentForm.find('#comment_id').val(cid);
}

function commentFormDisplay(id) {
    $('#comments_' + id).append($('#form_id' + id).fadeIn());
    $('#post_comment' + id).val('').focus();
    $('#comments_' + id + ' #comment_id').val('');
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

<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

function like_post_comments(type, comment_id, user_id) {
    if (type == 'like') {
        var status = 1;
    } else {
        var status = 0;
    }
    $.ajax({
        type: 'get',
        url: '/themes/maennaco/includes/delete.php?' +
            'type=like_post_comments&' +
            'comment_id=' + comment_id + '&' +
            'user_id=' + user_id + '&' +
            'status=' + status + "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",
        success: function () {
            link = $('#likepostcomment' + comment_id);
            if (type == 'like') {
                link.text('Unlike');
                link.attr("onclick", 'like_post_comments(\"unlike\", ' + comment_id + ',' + user_id + ');');
            } else {
                link.text('Like');
                link.attr("onclick", 'like_post_comments(\"like\", ' + comment_id + ',' + user_id + ');');
            }
        }
    });
}

function duplicateReviewCallback() {

    var text = $('#rate_comment').val();
    var dissid = <?=json_encode($_REQUEST['pro_id']);?>;
    var editor = '<?=$user_id;?>';
    var pro_profile = <?=json_encode($_REQUEST['id']);?>;
    var m = "<?= md5($_REQUEST['pro_id']."kyarata75");?>";
    var flag = "pq";
    var tags = null;

    $.post("/themes/maennaco/includes/add_comment.php?type=pro_dis_comment",
        {
            dissid: dissid, text: text, editor: editor, m: m, tags: tags, pro_profile: pro_profile, flag: flag
        },
        function (response) {
            if (response.status == 'success') {

            } else {
                alert("Your request din`t go through. Please try again!");
            }
            location.href = location.href.replace('&review=1', '');
        },
        "json"
    );
}

<?php if(isset($_REQUEST['file'])) : ?>

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
        var m = '<?=md5(((string)$user->uid).$_REQUEST['fid']."kyarata75")?>';
        var u = '<?= $user->uid; ?>';
        var fid = '<?=$_REQUEST['fid'];?>';
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
                bEdit: bEdit,
                pid: pid

            }, function (response) {
                $('#posting').prepend($(response).show());
                $("#watermark").val("");
                if (bEdit) {
                    $("#watermark").attr("stype", '');
                    $("#watermark").attr("rel", '');
                    $('#record-' + pid).find('div.comment_text').html();
                    $('#record-' + pid).find('div.comment_text').html(a);

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
                sel_obj.parent().parent().prev().append($(response).show());
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
        var m = $(this).attr('m');
        var pid = $(this).attr('pid');
        $.post("/themes/maennaco/includes/delete_comment.php?type=pro_file_post", {
            pid: pid,
            m: m
        }, function (response) {
            if (response.status == 'success') {
                alert(response.display);
                $("#record-" + pid).hide();
            } else {
                alert(response.display);
            }
        }, "json");
    });
    $("a[id^='cid-']").livequery('click', function (event) {
        event.preventDefault();
        var m = $(this).attr('m');
        var cid = $(this).attr('cid');
        $.post("/themes/maennaco/includes/delete_comment.php?type=pro_file_comment", {
            cid: cid,
            m: m
        }, function (response) {
            if (response.status == 'success') {
                alert(response.display);
                $("#comment-" + cid).hide();
            } else {
                alert(response.display);
            }
        }, "json");
    });
    $(".review-link").click(function() {

    });
});

<?php endif; ?>

</script>

<style type="text/css">

    .like_cnt {
        float:none !important;
        color:#00A3BF!important;
    }

    .new_tool {
        color: gray !important;
    }

    .new_tool.active {
        color: #00a2bf !important;
    }

    div.content_box .box_title {
        margin-top: 14px;
    }

    .tabtags span.mceEditor {
        padding:0;
    }

    .add_minutes_span {margin-bottom:30px;}
    a.review-link {
        font-size: 13px;
        cursor: pointer;
    }
    a.review-link:hover {
        text-decoration: none;
        color: #00a2bf !important;
        cursor:pointer;
    }
    .tabtags span.mceEditor {
        padding:0;
    }
    .tabtags span {padding:0 !important; }
     td.mceToolbar span {padding:0 0 0 10px !important;}
    .tabtags span.mceIcon {padding:0 !important;}
    .tabtags a.mceButton {padding:0 !important;}
    .text_button {
        border: none !important;
        background-color: transparent !important;
        color: #0fabc4 !important;
        cursor: pointer;
        font-family: 'LatoRegular' !important;
        font-size: 14px !important;
        font-style: normal !important;
    }

    .conversations_forms select {
        min-height: inherit;
        margin: 8px 0 0 0;
    }
    div.content_box .box_content {
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
        padding-bottom: 10px; /*float:left !important;*/
    }
    .payment-cancel .ui-button-text {
        height: 30px;
        background-color: #D0D2D2 !important;
        border-radius: 4px;
        -webkit-border-radius: 4px;
        -moz-border-radius: 4px;
        -khtml-border-radius: 4px;
        border: none;
        padding-right: 12px;
        padding-left: 12px;
    }
    .payment-cancel {
        font-family: 'Lato Bold' !important;
        font-size: 17px !important;
        width: 95px;
        height: 30px;
        border: 0 none;
        background-color: #D0D2D2;
        color: white;
        text-align: center;
        cursor: pointer;
        margin-right:39px !important;
        margin: 2em 0.5em 2em 0.5em; /* LTR */ border-radius: 4px; -webkit-border-radius: 4px; -moz-border-radius: 4px; -khtml-border-radius: 4px; outline: none; border: none; cursor: pointer; }
    .payment-submit .ui-button-text {
        height: 30px;
        background-color: #43A0C1 !important;
        border-radius: 4px;
        -webkit-border-radius: 4px;
        -moz-border-radius: 4px;
        -khtml-border-radius: 4px;
        border: none;
        width:auto !important;
        min-width: 71px !important;
        padding-right: 12px;
        padding-left: 12px;

    }
    .payment-submit {
        font-family: 'Lato Bold' !important;
        font-size: 17px !important;
        width:auto !important;
        min-width: 95px !important;
        height: 30px;
        border: 0 none;
        background-color: #43A0C1;
        color: white;
        display:none;
        text-align: center;
        cursor: pointer;
        margin: 2em 0.5em 2em 0.5em; /* LTR */ border-radius: 4px; -webkit-border-radius: 4px; -moz-border-radius: 4px; -khtml-border-radius: 4px; outline: none; border: none; cursor: pointer; }

    audio::-webkit-media-controls {
        overflow: hidden !important
    }
    audio::-webkit-media-controls-enclosure {
        width: calc(100% + 32px);
        margin-left: auto;
    }
</style>

<div id='docprev' style="width:610px;">
<div>
<div class="join">
    <div class="jleft" style="width: auto">

        <div style="clear:both"></div>Host:
        <a href="#" id="edit_id<?=intval($_REQUEST['pro_id']);?>"
           class="profile_details detail"
           style="font-size: 14px; font-style: normal; font-weight: bold;"
           name="<?= $P_username ?>">
            <?= $P_username ?>,
            <?= ucfirst(preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $sql_exp_result['experties'])); ?>
        </a>
        <?php
            $res = db_query("SELECT mdm.*,IF (mp.username_type = 1,mp.firstname,CONCAT(mp.firstname,' ', mp.lastname)) as firstname,mp.protype as protype,mp.experties FROM maenna_discussion_moderator mdm join maenna_people mp ON mdm.user_id = mp.pid WHERE discussion_id = %d", $row1['id'] . ' LIMIT 1');
            $moderator = db_fetch_array($res);
                if ($moderator['status'] === 'invited') {
                    $status = '<span class="inv_status">Invited: </span>';
                } else {
                    $status = '<span class="inv_status">Collaborator: </span>';
                }
                echo '<a style="font-size: 14px;font-style:normal; font-weight: bold;margin-left:20px;" href="#" id="pro_id' . $moderator['user_id'] . '" ref="pro_id"class="profile_details detail">' . $status . trim(ucfirst($moderator['firstname'])) . ", " . ucfirst($moderator['experties']) . "</a> ";
                if ($moderator['status'] === 'invited' && $moderator['user_id'] == $user->uid) {
                    echo "<span class='rsvp_inv'><a style='margin-left:10px;cursor:pointer;' iid='" . $moderator['id'] . "' rel='accept' class='invitation'>Accept<a> / <a iid='" . $moderator['id'] . "' style='cursor:pointer;' rel='reject' class='invitation'>Reject</a></span>";
                }
            $ifActiveModerator = ($moderator['status'] == 'active' && $moderator['user_id'] == $user->uid);
        ?>
    </div>
    <span style="float:right;margin-top:23px;">Category: <?= $row1['tags'] ?></span>
</div>
<br/>
<br/>
<br/>
<?php

    $tooltip = $accessAllowed ? '' : 'Private for participants';
?>

<div id="TabbedPanels1" class="TabbedPanels">

<?php
function accessToTabsContent($insight, $user) {
    if ($insight->postedby == $user->uid) {
        return 'seller';
    }
    if ($insight->template_insight_id > 0) {
        $query = "SELECT * FROM `maenna_professional_payments` WHERE `pro_id`={$insight->id}";
        $res = db_query($query);
        $result = db_fetch_array($res);
        if ($result && $result['user_id'] == $user->uid) {
            return 'owner';
        }
    }
    return false;
}
$accessToTabsContent = accessToTabsContent($insight, $user);

?>

<ul class="TabbedPanelsTabGroup">
    <li class="TabbedPanelsTab" id="tab1" tabindex="0" style="margin: 0;">About</li>
    <?php if ($isService) : ?>
        <li data-tooltip="<?= $tooltip; ?>" class="TabbedPanelsTab" id="tab2" tabindex="0" style="margin: 0;">Requirements</li>
    <?php endif; ?>
    <li data-tooltip="<?= $tooltip; ?>" class="TabbedPanelsTab" id="tab3" tabindex="0" style="margin: 0;">Conversations</li>
    <li data-tooltip="<?= $tooltip; ?>" class="TabbedPanelsTab" id="tab4" tabindex="0" style="margin: 0;"><?= $isService ? 'Deliverables' : 'Private Message'; ?></li>
    <?php if (isset($_REQUEST['file'])) : ?>
        <li class="TabbedPanelsTab" id="tab5"
            tabindex="0" style="margin: 0;">Files</li>
    <?php endif; ?>
</ul>

<div class="TabbedPanelsContentGroup">

<!--- start about tab content -->
<div class="TabbedPanelsContent">
    <div style="display:none;" id="editmatdlg"></div>
    <?php if (in_array('Maennaco admin', $user->roles) || in_array('Super admin', $user->roles) || $accessToTabsContent == 'seller' || $ifAdmin || ($ifActiveModerator || $ifInsightOwner || $attendeeId == $user->uid)) { ?>
    <style>.new_tool:hover {
            text-decoration: none !important;
        }</style>
    <div style="height: 32px;background-color: #fafbfb;font-family: 'Lato Light';">
        <a style="cursor:pointer;float:right;padding:0 8px;border-left:1px solid #76787f;line-height:12px;margin-top:8px;"
           id="editmat_id<?= $insight->template_insight_id > 0 ? $insight->template_insight_id : $insight->id; ?>" alt="<?= md5($row1['id'] . "kyarata75") ?>"
           name="<?= $user->name; ?>" delType='event' class="new_tool upload_file_action">
            <strong style="font-weight:normal!important;">Add Files</strong>
        </a>
        <a style="float:right;margin-right:8px;line-height:12px;margin-top:8px;"
           href="#" id="edit_id<?= $row1['id'] ?>"
           alt="<?= md5($row1['id'] . $user->name . "kyarata75") ?>" name="<?= $user->name; ?>"
           delType='event' class="new_tool evedit">
            <strong style="font-weight:normal!important;">Edit</strong>
        </a>
    </div>
    <?php } ?>
    <div style="float: left; padding: 15px; width: 569px;">
        <?php if (!$isService) : ?>
        <strong style="font-style: normal; font-weight: bold; color: #686b70; text-transform: uppercase;">
            Target audience
        </strong>

        <p style="padding-bottom: 20px"><?= nl2br(htmlspecialchars($insight->description)); ?></p>
        <?php endif; ?>
        <strong style="font-style: normal; font-weight: bold; color: #686b70; text-transform: uppercase;">
            <?= 0 == $insight->type ? 'Insight ':'Service ' ?> description
        </strong>

        <p style="padding-bottom: 20px"><?= nl2br(htmlspecialchars($insight->whyattend)); ?></p>

        <?php
        if ($row1['buyer_requirement']) {
        ?>
            <strong style="font-style: normal; font-weight: bold; color: #686b70; text-transform: uppercase;">
                Buyer Requirement
            </strong>

            <p style="padding-bottom: 20px"><?= nl2br(htmlspecialchars($row1['buyer_requirement'])); ?></p>

        <?php
        }
        if ($row1['datetime'] > time()) { ?>

            <strong style="font-style: normal; font-weight: bold; color: #686b70; text-transform: uppercase;">
                LIVE DISCUSSION DATE
            </strong>
            <p style="padding-bottom: 20px"><?= date("l, M j, Y g:i A T ", $row1['datetime']); ?></p>


            <strong style="font-style:normal; font-weight:bold; color:#686b70;text-transform: uppercase;">Where</strong>
            <p style="padding-bottom: 20px">
                Materials and Q&A online. Call
                in <?= nl2br((time() >= ($row1['datetime'] - (7 * 24 * 60 * 60))) ? '<font style="color:#01a7bf;">' . $row1['location'] . '</font>' : '<font style="color:#01a7bf;">TBD</font>'); ?>
                for private analyst moderated Insight.</p>
        <?php } ?>
    </div>
    <?php
    if ($isService) :
    /**
     * Get milestones of service
     * @param $service_id
     * @return array
     */
    function getServiceMilestones($service_id) {
        $company_service = new Clewed\Company\Service();
        $milestones = $company_service->getServiceMilestones((int)$service_id);
        if (!$milestones) {
            $milestones = array();
        }
        return $milestones;
    }
    $milestones = getServiceMilestones($insight->id);
    ?>

    <div style="display: table;">
        <!-- Milestones -->
        <div style="height: 32px;margin-top: 18px;background-color: #e8e9ea;font-family: 'Lato Light';color: #686b70;display: flex;">
            <div style="text-transform: uppercase;margin: 4px 10px;">Milestones</div>
        </div>
        <div id="milestones_list">
            <ul style="margin: 0 !important;padding-left:10px;">
                <?php foreach ($milestones as $milestone) : ?>
                    <li style="height: 32px !important;border-bottom: solid 1px #d0d2d3;display: table-row;font-family: 'Lato Light';">
                        <div style="width:92px;float:left;">In  <?= $milestone['duration']; ?> days</div>
                        <div style="width:500px;float:left;text-align: justify;"><?= $milestone['description'] ?></div>
                    </li>
                <?php endforeach; ?>
            </ul>
        </div>
    </div>
    <?php endif; ?>
    <div <?= $isService ? '' : 'style="display: table;"'; ?>>
        <ul style="background-color: #fafbfb; margin: 0;">
            <li style="height: 32px;margin: 62px 0 0 0;text-transform: uppercase !important;font-family: 'Lato Bold';text-align: center;padding-left:10px;background-color: #e8e9ea !important;color: #ffffff;">
                <div style="text-align:left;width:592px;float: left;margin: 5px 0;">Files</div>
            </li>
        </ul>
    </div>
    <div class="materials" data-tooltip="<?= ($accessAllowed ? '' : 'Private files'); ?>" style="width: auto;">
        <ul>
            <?php
            //Getting images and links of event.
            $s_id = $insight->template_insight_id ? $insight->template_insight_id : $insight->id;
            $sql_images = mysql_query("SELECT * FROM wall_documents WHERE ref_id = '" . mysql_real_escape_string($s_id) . "'");
            $allowedExtensions = array("txt", "xml", "doc", "docx", "xls", "xlsx", "rtf", "ppt", "pdf");
            $playerNo = 1;
            while ($images = mysql_fetch_assoc($sql_images)) {
                if ($images['document_name'] != '') {
                    list($time_stamp, $file_name) = explode('_', $images['document_name'], 2);
                    list($name, $ext) = explode('.', $file_name);
                    $id        = $_REQUEST['id'];
                    $pro_id    = $s_id;
                    $file_name = $images['document_name'];
                    echo "<li style=\"font-family: 'Lato Light'\">";
                    if ($accessAllowed) {
                        $extension = end(explode('.', $file_name));
                        if (in_array($extension, $allowedExtensions)) {
                            ?>
                            <a href="/account?tab=professionals&page=pro_detail&id=<?= $_REQUEST['id']; ?>&section=pro_industry_view&type=details&mtab=advice&view_id=<?= $_REQUEST['view_id'] ?>&file=<?= $file_name; ?>&fid=<?= $images['d_id'] ?>&pro_id=<?= $_REQUEST['pro_id'] ?>"><?= $name ?></a>
                        <?php } elseif ($extension === 'mp3') {
                            $db = \Clewed\Db::get_instance();
                            // TODO remove after deploy
                            $db->run(
                                'CREATE TABLE IF NOT EXISTS `audio_files` (`id` INT(11) NOT NULL AUTO_INCREMENT, `hash` CHAR(40) NOT NULL, `file` VARCHAR(300) NOT NULL, `created` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, PRIMARY KEY (`id`), UNIQUE KEY `hash` (`hash`), KEY `created` (`created`)) ENGINE = InnoDB DEFAULT CHARSET = utf8;'
                            );
                            $hash = sha1('hash' . time() . $file_name);
                            // Delete old hashes
                            $db->run('DELETE FROM `audio_files` WHERE DATE_ADD(`created`, INTERVAL 1 DAY) < NOW()');
                            $db->run('INSERT INTO `audio_files` (`hash`, `file`) VALUES (:hash, :file)', array(':hash' => $hash, ':file' => $file_name));
                            $audioFile = '/' . $hash . '.mp3';
                            ?>
                            <audio controls preload type="audio/mpeg" style="width: 570px" src="<?=$audioFile;?>">
                                <script type="text/javascript" src="/js/swfobject.js"></script>
                                <script type="text/javascript">
                                    swfobject.registerObject("player","9.0.0");
                                    var flashvars = {
                                        height: "20",
                                        width: "200",
                                        file: "<?= $audioFile ?>",
                                        backcolor: "0x999999",
                                        frontcolor: "0xFFFFFF",
                                        overstretch: "none",
                                        usefullscreen: "false",
                                        enablejs: "true",
                                        javascriptid: "player"
                                    };
                                    var params = { };
                                    var attributes = { };
                                    swfobject.embedSWF("/mediaplayer.swf","audio_player<?= $playerNo ?>","200","20","9.0.0","",flashvars,params,attributes);
                                </script>
                                <div id="audio_player<?= $playerNo++ ?>"></div>
                            </audio>
                        <?php } else { ?>
                            <a href="/download.php?file_name=<?= $file_name; ?>"><?= $name ?></a>
                            <?php
                        }
                    } else {

                        $extension = end(explode('.', $file_name));
                        if('mp3' == $extension):?>
                            <div onmousedown="alert('This section is for people attending this insight. Please join to access information.')">
                                <!--                                    <script type="text/javascript" src="/js/swfobject.js"></script>
                                    <script type="text/javascript">
                                        swfobject.registerObject("player","9.0.0");
                                        var flashvars = {
                                            height: "20",
                                            width: "200",
                                            backcolor: "0x999999",
                                            frontcolor: "0xFFFFFF",
                                            overstretch: "none",
                                            usefullscreen: "false",
                                            enablejs: "true",
                                            javascriptid: "player"
                                        };
                                        var params = {
                                            wmode: "transparent"
                                        };
                                        var attributes = { };
                                        swfobject.embedSWF("/mediaplayer.swf","audio_player<?php echo md5($file_name)?>","200","20","9.0.0","",flashvars,params,attributes);
                                    </script>
                                    <div id="audio_player<?php echo md5($file_name)?>"></div>-->
                                <audio controls preload>Sorry, your browser is too old and is not supported anymore. Please update it and try again.</audio>
                            </div>
                        <?php else:?>
                            <a style="cursor:pointer;"
                               onclick="alert('This section is for people attending this insight. Please join to access information.')"><?= $name ?></a>

                        <?php endif;
                    }
                    echo '</li>';
                }
            }
            ?>
        </ul>
        <?php if ($_REQUEST['files'] != '') { ?>
            <?php
            list($time_stamp, $file_name) = explode('_', $_REQUEST['files']);
            list($name, $ext) = explode('.', $file_name);
            echo '<p style="text-align:center !important;margin-right: 128px;">' . $name . '</p>';
            ?>
            <iframe
                src="https://docs.google.com/viewer?url=<?= $base_url ?>/sites/default/files/events_tmp/<?= $_REQUEST['files'] ?>&embedded=true"
                width="600" height="670" style="border: none;"></iframe>
        <?php } ?>
        <?php
            //Getting images and links of event.
            $sql_links = mysql_query("SELECT * FROM maenna_professional_links WHERE professional_id = '" . mysql_real_escape_string($_REQUEST['pro_id']) . "'");
            while ($links = mysql_fetch_assoc($sql_links)) {
                if ($accessAllowed) {
                    echo "<ul>";
                    if ($links['links'] != '') {
                        $links['links'] = urldecode($links['links']);
                        $st             = strpos($links['links'], "://");
                        if ($st == 0) {
                            $resc = "http://" . trim($links['links']);
                        } else {
                            $resc = $links['links'];
                        }
                        echo '<li><a target="_blank" href="' . $resc . '">' . $links['name'] . '</a></li>';
                    }
                    echo '</ul>';
                } else {
                    echo '<ul><li>' . $links['name'] . '</li></ul>';
                }
            }
        ?>
    </div>

    <?php
        $openReviewDialog = isset($_REQUEST['review']) && $_REQUEST['review'];
        echo displayExpertReviewForm($_REQUEST['id'], $openReviewDialog, $insight);
    ?>
</div>
<!--- end about tab content -->

<?php if ($isService) : ?>
<!-- start requirements tab content -->
<div class="TabbedPanelsContent">
    <?php
    $seller_id = null;
    $buyer_id = null;
    if ($accessToTabsContent == 'seller') {
        $seller_id = (int)$user->uid;
    } else {
        $buyer_id = (int)$user->uid;
    }
    $service_id = (int)$insight->id;

    if ($seller_id || in_array('Maennaco admin', $user->roles) || in_array('Super admin', $user->roles)) {
        ?>
    <div style="height: 32px;background-color: #fafbfb;font-family: 'Lato Light';">
        <a style="cursor:pointer;float:right;padding:0 8px;line-height:12px;margin-top:8px;"
           id="addQuestionButton" class="new_tool">
            <strong style="font-weight:normal!important;">Add Question</strong>
        </a>
    </div>
        <?php
    }
    if ($buyer_id) {
        if ($_REQUEST['view_type'] != 'edit_answers') {
            ?>
        <div style="height: 32px;background-color: #fafbfb;font-family: 'Lato Light';">
            <a style="cursor:pointer;float:right;padding:0 8px;line-height:12px;margin-top:8px;"
               id="addAnswerButton" class="new_tool">
                <strong style="font-weight:normal!important;">Add Answer</strong>
            </a>
        </div>
            <?php
        }
    }
    if ($seller_id) {
        ?>
        <div id="newQuestion" style="display: none;">
            <textarea id="newQuestionText" class="input" placeholder="Question" style="height: 50px;width: 565px !important; font: 15px/160% Verdana, sans-serif"></textarea>
            <span id="saveQuestionButton" style="cursor: pointer;font-size: 14px; font-style: normal; font-weight: bold;float: right;">Save</span>
        </div>
        <?php
    }
    $query = "SELECT Q.*,
        A.answer,
        A.qaid AS answer_id 
        FROM maenna_questionair AS Q 
        LEFT JOIN maenna_qa AS A ON Q.recordid=A.qid
        WHERE Q.target='service' AND Q.type='question' AND Q.parentid={$service_id}
        ORDER BY recordid";
    $result = db_query($query, array($service_id, $seller_id ? $seller_id : $buyer_id));

    $counter = 0;

    if ($seller_id) {
        while($row = db_fetch_object($result)) {
            $question = htmlentities($row->content, ENT_QUOTES);
            if ($row->answer) {
                $answer = htmlentities($row->answer, ENT_QUOTES);
            } else {
                $answer = "N/A";
            }
            ++$counter;
            ?>
            <div class="entry" style="margin-top: 32px;">
                <div id="q_<?= $row->recordid; ?>" class="entry-title dilligence" style="border: none;">
                    <?= $counter; ?>. <?= $question; ?>
                </div>
                <?php if ($insight->template_insight_id): ?>
                <div class="entry-content" style="margin-top: 32px;">
                    <span style="all: initial;color: #00a2bf;font-size: 14.5px !important;line-height: 24px !important;margin-top: 6px !important;position: relative !important;font-family: 'LatoRegular' !important;">Answer:</span>
                    <?= $answer; ?>
                </div>
                <?php endif; ?>
            </div>
            <?php
        }
        if (!$counter) {
            ?>
            <div class="entry-content" style="margin-top: 32px; font-family: 'Lato Light'; font-size: 18px;">
                You must add at least one question to clarify expectations and avoid misunderstanding  of deliverables.
            </div>
            <?php
        }
    } else {
        if ($_REQUEST['view_type'] == 'edit_answers') {
            while($row = db_fetch_object($result)) {
                $question = htmlentities($row->content, ENT_QUOTES);
                if ($row->answer) {
                    $answer = htmlentities($row->answer, ENT_QUOTES);
                    $answer_id = (int)$row->answer_id;
                } else {
                    $answer = "";
                    $answer_id = null;
                }
                ++$counter;
                ?>
                <div class="entry-title dilligence" style="border: none;margin-top: 32px;">
                    <?= $counter; ?>. <?= $question; ?>
                </div>
                <div style="margin-top: 32px;">
                    <textarea answer-id="<?= $answer_id; ?>" question-id="<?= $row->recordid; ?>" class="input new_answer_text" placeholder="Answer" style="height: 70px;width: 598px !important; font: 15px/160% Verdana, sans-serif"><?= $answer; ?></textarea>
                </div>
                <?php
            }
            ?>
            <div>
                <span id="submitAnswersButton" style="cursor: pointer;font-size: 14px !important; font-style: normal; font-weight: bold;float: left;">Submit</span>
            </div>
            <?php
        } else {
            while($row = db_fetch_object($result)) {
                $question = htmlentities($row->content, ENT_QUOTES);
                if ($row->answer) {
                    $answer = htmlentities($row->answer, ENT_QUOTES);
                } else {
                    $answer = "N/A";
                }
                ++$counter;
                ?>
                <div class="entry" style="margin-top: 32px;">
                    <div id="q_<?= $row->recordid; ?>" class="entry-title dilligence" style="border: none;">
                        <?= $counter; ?>. <?= $question; ?>
                    </div>
                    <div class="entry-content" style="margin-top: 32px;">
                        <span style="all: initial;color: #00a2bf;font-size: 14.5px !important;line-height: 24px !important;margin-top: 6px !important;position: relative !important;font-family: 'LatoRegular' !important;">Answer:</span>
                        <?= $answer; ?>
                    </div>
                </div>
                <?php
            }
        }
    }
    ?>
</div>
<!-- end requirements tab content -->
<?php endif; ?>

<?php

include_once __DIR__ . '/../blocks/insights/comment.php';
include_once __DIR__ . '/../blocks/insights/post.php';

function getPosts($insightId, $discussionId, $currentUserId)
{
    $posts = array();
    $result = mysql_query(
        "SELECT *, UNIX_TIMESTAMP() - date_created AS TimeSpent
         FROM pro_wall_posts
         WHERE pro_id = " . $discussionId . "
           AND flag IN ('m', 'q', 'pm', 'pq')
         ORDER BY pid DESC"
    );

    while ($row1 = mysql_fetch_array($result)) {

        $result3   = mysql_query("SELECT * FROM `like_discussion_posts` WHERE  user_id = '" . $currentUserId . "' and post_id = '" . $row1['pid'] . "'");
        $likepost1 = mysql_num_rows($result3);
        $result4   = mysql_query("select count(*) as like_cnt from like_discussion_posts WHERE post_id = '" . $row1['pid'] . "'");
        $row4      = mysql_fetch_array($result4);



        $posts[] = array(
            'id'            => $row1['pid'],
            'discussion_id' => $discussionId,
            'insight_id'    => $insightId,
            'flag'          => $row1['flag'],
            'is_private'    => ($row1['flag'] == 'pm' || $row1['flag'] == 'pq'),
            'author_id'     => $row1['user'],
            'author_name'   => getUserById($row1['user']),
            'author_avatar' => getPostAvatarUrl($row1['user']),
            'post'          => $row1['post'],
            'tags'          => $row1['tags'],
            'is_liked'      => ($likepost1 > 0),
            'likes_cnt'     => $row4['like_cnt'],
            'created_at_timestamp' => $row1['date_created'],
        );
    }
    return $posts;
}

function getPostAvatarUrl($uid) {
    $usrType = getUserTypeById($uid);
    if ($usrType == 'people' || $usrType == 'admin' || $usrType == 'super_admin') {
        return getAvatarUrl($uid);

    } elseif ($usrType = 'company') {

        $query = "
            SELECT *
            FROM maenna_company mc
            LEFT JOIN maenna_about ma ON ma.project_id = mc.companyid
            WHERE companyid = %d";
        $result = db_query($query, array($uid));
        $result = db_fetch_object($result);
        $cmp_role = $result->company_type;
        $image = $result->project;

        $avatar = '/themes/maennaco/images/cmp-avatar-product.png';
        if (is_readable('sites/default/images/company/50x50/' . $uid . '.jpg'))
            $avatar = '/sites/default/images/company/50x50/' . $uid . '.jpg';

        elseif (!empty($image) && is_readable('themes/maennaco/images/project/' . urlencode($image)))
            $avatar = '/themes/maennaco/images/project/' . urlencode($image);

        elseif ('service' == $cmp_role)
            $avatar = '/themes/maennaco/images/cmp-avatar-service.png';

        return $avatar;
    }
}

function getAllComments($postIds, $currentUserId)
{
    $comments = array();
    if (empty($postIds)) {
        return array();
    }
    $postIds = implode(', ', $postIds);
    $result = mysql_query(
        "SELECT c.*, UNIX_TIMESTAMP() - c.datecreated AS TimeSpent, l.comment_id AS liked
         FROM pro_wall_posts_comments AS c
            LEFT JOIN like_discussion_posts_comments AS l ON(l.comment_id=c.cid AND l.user_id='" . $currentUserId . "')
         WHERE post_id IN (" . $postIds . ")
         ORDER BY cid ASC"
    );

    while ($row = mysql_fetch_array($result)) {

        $postId = $row['post_id'];
        if (!isset($comments[$postId])) {
            $comments[$postId] = array();
        }
        $comments[$postId][] = array(
            'id'            => $row['cid'],
            'post_id'       => $postId,
            'author_id'     => $row['user_id'],
            'author_avatar' => getPostAvatarUrl($row['user_id']),
            'author_name'   => getUserById($row['user_id']),
            'is_liked'      => (bool)$row['liked'],
            'comment'       => $row['comment'],
            'created_at_timestamp' => $row['datecreated'],
        );
    }
    return $comments;
}

function displayAddPostBlock($ifInsightOwner, $ifInsightAdmin, $ifPrivate,$moderator = null)
{
    global $AccessObj;
    global $isService;
    ?>
    <div style="height: 32px;background-color: #fafbfb;font-family: 'Lato Light';">
    <?php
    if (canRateUser($_REQUEST['id'], $AccessObj->uid)) {
        if (ifUserRatedForService('insight',$AccessObj->uid,$_REQUEST['id'],$_REQUEST['pro_id']))
        {
            ?>
            <div style="float:left;margin-bottom:15px;">
                <a class="new_tool review-link"/>Host review added</a>
            </div>
            <?php
        }
        else { ?>
        <div style="float:left;margin-bottom:15px;">
            <a class="new_tool review-link" data-id="<?=$_REQUEST['id'];?>" onclick="$('#rate_user_dialog').find('#target_uid').val('<?=$_REQUEST['id'];?>');$('#rate_user_dialog').dialog('open'); return false;">Review Host</a>
        </div>
        <?php }

    }
    if (($moderator && canRateUser($moderator, $AccessObj->uid)) || $ifInsightOwner ) {
        if (canRateUser($_REQUEST['id'], $AccessObj->uid)) {
            ?>
            <div style="float:left;margin-left:4px;margin-right:4px;">|</div>
            <?php
        }
        if (ifUserRatedForService('insight',$AccessObj->uid,$moderator,$_REQUEST['pro_id']))
        {
            ?>
            <div style="float:left;margin-bottom:15px;">
                <a class="new_tool review-link">Guest Expert review added</a>
            </div>
            <?php
        } else { ?>
            <div style="float:left;margin-bottom:15px;">
                <a class="new_tool review-link" data-id="<?=$moderator;?>" onclick="$('#rate_user_dialog').find('#target_uid').val('<?=$moderator;?>');$('#rate_user_dialog').dialog('open'); return false;">Review Guest Expert</a>
            </div>
        <?php }

    }
    ?>

    <span style="margin-right:0; clear:none;" class="add_minutes_span">
    <?php
//        if (!$ifInsightOwner) {
    ?>
        <a class="new_tool" href="javascript:void(0);"
             onClick="$('.new_tool').removeClass('active'); $(this).addClass('active'); ask_question(<?= (bool)$ifPrivate;?>);">
            <?= ($ifPrivate ? 'Message the expert' :'Add Q&A or Comments'); ?>
        </a>

        | <a class="new_tool" href="/account?tab=professionals&page=pro_detail&id=<?=sget($_REQUEST, 'id')?>&section=pro_industry_view&type=discussion<?= $isService ? '' : ('&add=guest_expert_' . $moderator); ?>">
           <?= $isService ? 'See other listings' : 'Order private help'; ?>
        </a>
    <?php //} else { echo '&nbsp;';} ?>

    </span>
    </div>
    <div style="clear:both"></div>
<?php
}

$lastPostId = 0;
$lastCommentId = 0;

if ($accessAllowed) {

    $posts = getPosts($id, $pro_id, $user_id);

    $postIds = array_map(function($post) {
            return $post['id'];
        }, $posts);

    $allComments = getAllComments($postIds, $user_id);

    foreach ($posts as $post) {

        if ($post['id'] > $lastPostId) {
            $lastPostId = $post['id'];
        }
        $comments = isset($allComments[$post['id']]) ? $allComments[$post['id']] : array();
        foreach ($comments as $i => $comment) {
            if ($comment['id'] > $lastCommentId) {
                $lastCommentId = $comment['id'];
            }
            $allComments[$post['id']][$i]['is_private'] = $post['is_private'];
        }
    }

    ?>

    <!--- start Q&A tab content -->
    <div class="TabbedPanelsContent">
        <?php displayAddPostBlock($ifInsightOwner, $ifInsightAdmin, false,$moderator['user_id']); ?>

        <div class="tabtags public-tab">

            <?php
            foreach ($posts as $post) {
                $comments = isset($allComments[$post['id']]) ? $allComments[$post['id']] : array();

                if (!$post['is_private']) {
                    displayPost($post, $comments, $user_id, false, $ifInsightAdmin);
                }
            }
            ?>

        </div>

    </div>
    <!--- end Q&A tab content -->

    <?php if ($isService) { ?>
    <!--- start of Deliverables tab content -->
    <div class="TabbedPanelsContent" style="font-family: 'Lato Light' !important;">
        <?php
        /**
         * Check if milestones are approved
         * @param $milestones
         * @return bool
         */
        function milestonesApproved($milestones) {
            foreach ($milestones as $milestone) {
                if (!$milestone['approve_status']) {
                    return false;
                }
            }
            return true;
        }
        $milestonesApproved = milestonesApproved($milestones);
        ?>
        <div>
            <div style="height: 32px;background-color: #fafbfb;font-family: 'Lato Light';">
                <a style="cursor:pointer;float:right;padding:0 8px;border-left:1px solid #76787f;line-height:12px;margin-top:8px;"
                   id="editmat_id<?= $row1['id'] ?>" alt="<?= md5($row1['id'] . "kyarata75") ?>"
                   name="<?= $user->name; ?>" delType='event' class="new_tool custom_upload_file_action">
                    <strong style="font-weight:normal!important;">Add Files</strong>
                </a>
                <a style="cursor:pointer;float:right;padding:0 8px;border-left:1px solid #76787f;line-height:12px;margin-top:8px;"
                   id="<?= $milestonesApproved ? 'approved_milestones_action' : 'approve_milestones_action'; ?>" class="new_tool">
                    <strong style="font-weight:normal!important;">Approve Milestones</strong>
                </a>
                <a style="cursor:pointer;float:right;padding:0 8px;line-height:12px;margin-top:8px;"
                   id="<?= $milestonesApproved ? 'edit_approved_milestones_action' : 'edit_milestones_action'; ?>" class="new_tool">
                    <strong style="font-weight:normal!important;">Edit Milestones</strong>
                </a>
            </div>
            <div>
                <!-- Milestones -->
                <div style="height: 32px;margin-top: 18px;background-color: #e8e9ea;font-family: 'Lato Light';color: #686b70;display: flex;">
                    <div style="text-transform: uppercase;margin: 4px 10px;">Milestones</div>
                </div>
                <div id="milestones_list">
                    <ul style="margin: 0 !important;padding-left:10px;">
                    <?php foreach ($milestones as $milestone) : ?>
                        <li style="height: 32px !important;border-bottom: solid 1px #d0d2d3;display: table-row;font-family: 'Lato Light';">
                            <div style="width:92px;float:left;">In  <?= $milestone['duration']; ?> days</div>
                            <div style="width:500px;float:left;text-align: justify;"><?= $milestone['description'] ?></div>
                        </li>
                    <?php endforeach; ?>
                    </ul>
                </div>
            </div>
        </div>
        <ul style="background-color: #fafbfb; margin: 0;">
            <?php
            // Getting images and links of event.
            $sql_images = mysql_query("SELECT wd.d_id, wd.document_name, wp.f_name, wp.date_created FROM wall_documents as wd 
                LEFT JOIN wall_posts as wp ON wd.d_id = wp.document_id WHERE wd.ref_id = {$insight->id} ");

            if ($sql_images) {
                echo
                "<li style=\"height: 32px;margin: 62px 0 0 0;text-transform: uppercase !important;font-family: 'Lato Bold';text-align: center;padding-left:10px;background-color: #e8e9ea !important;color: #ffffff;\">
                    <div style='text-align:left;width:365px;float: left;margin: 5px 0;'>File Name</div>
                    <div style='width:120px;float: left;margin: 5px 0;'>Uploaded by</div>
                    <div style='float: left;width:107px;margin: 5px 0;'>Date</div>
                </li>";
            }
            while ($images = mysql_fetch_assoc($sql_images)) {
                if ($images['document_name'] != '') {
                    list($time_stamp, $file_name) = explode('_', $images['document_name'], 2);
                    echo '<li style="height: 32px;margin: 0;padding-left:10px;font-family: \'Lato Light\';">
                    <a style="width:365px;float:left;" target="_new" href="sites/default/files/events_tmp/' . $images['document_name'] . '">' . $file_name . '</a>
                    <div style="width:120px;float:left;text-align: center;">' . ($images['f_name'] ? $images['f_name'] : 'unknown') . '</div>
                    <div style="width:107px;float:left;text-align: center;">' . ($images['date_created'] ? date('m/d/Y', $images['date_created']) : '-') . '</div>
                    </li>';
                }
            }
            ?>
        </ul>
    </div>
    <!--- end of Deliverable  tab content -->
    <?php } else { ?>
    <!--- start of Private conversation tab content -->
    <div class="TabbedPanelsContent">

        <?php displayAddPostBlock($ifInsightOwner, $ifInsightAdmin, true,$moderator['user_id']); ?>

        <div class="tabtags private-tab">

            <?php
            foreach ($posts as $post) {
                $comments = isset($allComments[$post['id']]) ? $allComments[$post['id']] : array();

                $ifPostOwner = $post['author_id'] == $user_id;
                if ($post['is_private'] && ($ifInsightAdmin || $ifInsightOwner || $ifPostOwner)) {
                    displayPost($post, $comments, $user_id, true, $ifInsightAdmin);
                }
            }
            ?>

        </div>

    </div>
    <!--- end of Private conversation  tab content -->
    <?php } ?>
<?php } ?>

<?php if (isset($_REQUEST['file'])) : ?>
    <!-- start file preview -->
    <div class="TabbedPanelsContent">
		<?php
			$filePath = (isset($_SERVER["HTTPS"]) ? 'https' : 'http')."://".$_SERVER['HTTP_HOST']."/sites/default/files/".(empty($_REQUEST['root']) ? 'events_tmp/' . $_REQUEST['file'] : $_REQUEST['root'] . '/' . $_REQUEST['file']);
			if(pathinfo($filePath, PATHINFO_EXTENSION)=="pdf"){
				$viewerTypeUrl = (isset($_SERVER["HTTPS"]) ? 'https' : 'http')."://".$_SERVER['HTTP_HOST']."/themes/maennaco/pdf.js-master/web/viewer.html?file=";
			}else{
				$viewerTypeUrl = "https://docs.google.com/viewer?url=";
			}
		?>
        <div style="font-size:14px;height:32px;line-height: 32px;color: #fff;background-color: #94c9da;padding-left: 15px;"><?php echo $name;?>
            <div style="margin-right:18px;float:right;">
                <a data-tooltip="Download file" href="/download.php?tab=file1&file_name=<?= $_REQUEST['file'];?>" style="display:inline-block;margin-right: 5px;"><img src="/themes/maennaco/images/download_file_icon.png"></a>
            </div>
        </div>
        <iframe
            src="<?= $viewerTypeUrl ?><?= $filePath ?>&embedded=true"
            width="600" height="600" style="border: none;"></iframe>
        <span style="margin-right: 0;" class="add_minutes_span">
            <a href="javascript:void();" onclick=" $('#fShareIdea').toggle();">Share an idea</a>
        </span>

        <div style="clear:both;"></div>
        <div style="position: relative;margin-bottom:10px;" id="fShareIdea">
            <form action="" method="post" name="postsForm">
                <div class="UIComposer_Box">
                <span class="w" style="margin-bottom: -11px;">
                    <textarea
                        style="font-size:14px;font-family:'LatoRegular';border: 1px solid #CCCCCC !important;height:25px; margin-left:5px;width:593px;"
                        class="input mceNoEditor" id="watermark" placeholder='Enter your comment here' name="watermark"></textarea>
                </span>

                    <div style="clear:both"></div>
                    <div id="submitDiv" align="right" style="display:none; margin-top:5px">
                        <a style="cursor:pointer;color: #0fabc4!important;" id="comm_shareButton" class="text-button">
                            Submit
                        </a>
                    </div>
                </div>
            </form>
            <div style="clear: both;">&nbsp;</div>
        </div>
        <div class="tabtags cmtloop" id="posting" align="center" style="margin-top: -10px;">
            <?php

            $op_comments = 'edit';
            $op_subcomments = 'edit'; //File commenting is not part of permission table so permission access is hard-coded (currently you are able to edit and delete your content unless super or admin)
            include_once 'pro_comments_posts.php';
            ?>
        </div>
    </div>
    <!-- end file preview -->
<?php endif; ?>

</div>



<!-- form's container outside of .TabbedPanelsContentGroup -->
<div style="display:none">

    <script>
        var last_post_id = <?=$lastPostId;?>;
        var last_comment_id = <?=$lastCommentId;?>;
    </script>

    <div class="comts">

        <div id="minutes" class="conversations_forms" style="display:none;">
            <div id="error"></div>
            <form method="post"
                  action="/account?tab=professionals&page=pro_detail&id=<?php echo $id; ?>&section=pro_industry_view&type=details&pro_id=<?= $row1['id'] ?>"
                  id="comments">
                <input type="hidden" name="prof_id" id="prof_id" value="<?php echo $row1['id']; ?>"/>
                <input type="hidden" name="dis_id" id="dis_id" value="<?php echo $id; ?>"/>
                <input type="hidden" name="flag" id="flag" value="m"/>
                <table  id="minutues" style="width:600px !important;">
                    <tr>
                        <td><textarea class="minutes_tinymce_editor" name="dis_posts" placeholder="Content" id="dis_posts" cols="52"
                                      rows="7" style="height:250px"></textarea></td>
                    </tr>
                    <tr>
                        <td>
                            <select id="tags" name="tags">
                                <option selected value="">-- Choose a Category --</option>
                                <?= OPTION_TAGS(_categories()) ?>
                            </select>
                        </td>
                        <td align="right"></td>
                    </tr>
                    <tr>
                        <td style="padding-top: 8px;">
                            <input type="submit" name="dis_post" id="minutes_post"
                                   onClick="$('.new_tool').removeClass('active');"
                                   dissid="<?=$_REQUEST['pro_id'];?>" m="<?=md5($_REQUEST['pro_id']."kyarata75");?>"
                                   value="Submit" class="tool"/>

                            <input type="submit" name="dis_post" id="cancel" value="Cancel"
                                   onClick="javascript:$('.new_tool').removeClass('active');$('#minutes').hide();return false;"
                                   class="tool"/>
                        </td>
                    </tr>
                </table>
            </form>
        </div>

        <div id="question" class="conversations_forms" style="display:none;">
            <form method="post"
                  action="/account?tab=professionals&page=pro_detail&id=<?php echo $id; ?>&section=pro_industry_view&type=details&pro_id=<?= $row1['id'] ?>"
                  id="comments">
                <input type="hidden" name="prof_id" id="prof_id" value="<?php echo $row1['id']; ?>"/>
                <input type="hidden" name="dis_id" id="dis_id" value="<?php echo $id; ?>"/>
                <input type="hidden" name="flag" id="flag" value="q"/>
                <table width="100%">
                    <tr>
                        <td><textarea name="dis_posts" class="input own_text_editor" id="dis_posts_question"
                                      placeholder="Type Your Question" cols="52" rows="3"></textarea></td>
                    </tr>
                    <tr style="display: none;">
                        <td>
                            <select id="tags_question" name="tags">
                                <option selected value="">-- Choose a Category --</option>
                                <?= OPTION_TAGS(_categories()) ?>
                            </select>
                        </td>
                    </tr>
                    <tr>
                        <td style="padding-top: 8px;">
                            <input type="submit" id ="question_post" name="dis_post" dissid="<?=$_REQUEST['pro_id'];?>" m="<?=md5($_REQUEST['pro_id']."kyarata75");?>"
                                   onClick="$('.new_tool').removeClass('active');"
                                   value="Submit" class="tool"/>

                            <input type="submit" name="dis_post" id="cancel" value="Cancel"
                                   onClick="javascript:$('.new_tool').removeClass('active');$('#question').hide();return false;"
                                   class="tool"/>
                        </td>
                    </tr>
                </table>
            </form>
        </div>

        <div style="clear:both"></div>
    </div>

</div>



<?php require ROOT . '/themes/maennaco/dialogs/create-account.php' ?>

<script type="text/javascript">

    var TabbedPanels1 = new Spry.Widget.TabbedPanels("TabbedPanels1"<?php if(!empty($_REQUEST['file'])) echo ', {"defaultTab" : 3}' ?>);

    <?php if (!$accessAllowed): ?>
    $('.TabbedPanelsTabGroup #tab3').unbind().bind("click", function (e) {
        e.preventDefault();
        TabbedPanels1.showPanel(0);
        alert('This section is for people attending this insight. Please join to access information.');
        return false;
    });
    $('.TabbedPanelsTabGroup #tab2').unbind().bind("click", function (e) {
        e.preventDefault();
        TabbedPanels1.showPanel(0);
        alert('This section is for people attending this insight. Please join to access information.');
        return false;
    });
    <?php endif;?>

    //Remove class active from top navigation so users don't get confused (fixing Liliane's work)
    $('ul.menu li').each(function (i) {
        $(this).find('a').removeClass('active');
    });
    $('.discussion_link').livequery('click', function (e) {
        e.preventDefault();
        $('#discussion_popup').dialog({
            autoOpen: true,
            width: 400,
            title: 'DISCUSSION TITLE',
            height: 400,
            buttons: {
                'Save': function () {
                },
                'Cancel': function () {
                    $(this).dialog('close');
                }
            },
            closeOnEscape: true,
            modal: true
        }).show();
    });
</script>

<div id='discussion_popup' style='display: none; width: 400px !important;'>
    <form method='post' action=''>
        <?= date("l, M j, Y g:i A T ", $row1['datetime']); ?>
        <p><?= $row1['description']; ?></p>
        Attending?
        <input type='radio' name='attending' id=''>YES
        <input type='radio' name='attending' id=''>NO
        <input type='radio' name='attending' id=''>MAY BE
    </form>
</div>
<div id="service-update-milestones-dlg"></div>
<?php require ROOT . '/themes/maennaco/dialogs/payment.php' ?>
