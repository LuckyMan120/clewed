<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.autosuggest.js"></script>
<link href="/themes/maennaco/jui/comments/css/autosuggest.css" type="text/css" rel="stylesheet"/>
<!--<link href="/themes/maennaco/jui/comments/css/screen.css?as" type="text/css" rel="stylesheet"/>-->
<script src="/themes/maennaco/jui/comments/js/jquery.elastic.js" type="text/javascript" charset="utf-8"></script>
<script src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js" type="text/javascript"></script>
<link href="/themes/maennaco/jui/comments/css/SpryTabbedPanels.css" type="text/css" rel="stylesheet"/>

<div id='docprev'>
<script type="text/javascript">

function formDisplay(id) {
    $('#commentMark-' + id).focus();
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

function like_posts(type,prof_id, post_id, userid) {
    if (type == 'like') {
        var status = 1;
    } else {
        var status = 0;
    }
    $.ajax({
        type: 'get',
        url: '/themes/maennaco/includes/delete.php?' +
            'type=like_posts&' +
            'post_id=' + post_id + '&' +
            'userid=' + userid + '&' +
            'status=' + status + "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",
        success: function () {
            if (type == 'like') {
                $('#likepost1' + post_id).html("<a style='cursor:pointer;' onclick='like_posts(\"unlike\", " + post_id + "," + userid + ");'>Unlike</a>");
            } else {
                $('#likepost1' + post_id).html("<a style='cursor:pointer;' onclick='like_posts(\"like\", " + post_id + "," + userid + ");'>Like</a>");
            }
        }
    });
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

$(function () {
    var availableTags = {items: [
        //Get the advisors and connected users for the autocomplete feature; $companyid was gotten in the earlier phase in new_company_detail_left.php
        <?php
        $Conns = Connections::Com_conns($companyid);
            foreach($Conns['Advisor'] as $Pro) {
                $pro_uid = $Pro->assignee_uid;
                $pro_maeid = getProId($pro_uid);
                echo '{value: "'.$pro_uid.'", name: "'.$pro_maeid.'"},';
            }
            foreach($Conns['Client'] as $Pro) {
                $pro_uid = $Pro->assignee_uid;
                $pro_maeid = getProId($pro_uid);
                echo '{value: "'.$pro_uid.'", name: "'.$pro_maeid.'"},';
            }
            $q = mysql_query("SELECT projname FROM maenna_company WHERE companyid = '".((int) $_REQUEST['id'])."'");
            $q = mysql_fetch_array($q);
            echo '{value: "'.$_REQUEST['id'].'", name: "'.$q['projname'].' "}';
        ?>
    ]};
    $("#commentsInv").autoSuggest(availableTags.items, {startText: "Invite user", selectedItemProp: "name", searchObjProps: "name"});
    $("#watermark").focus(function () {
        $("#submitDiv").show('fast');
    });
    $('#closeButton').click(function () {
        $("#submitDiv").hide('fast');
    });
    $('#comm_shareButton').click(function (event) {

        event.preventDefault();

        var a = $("#watermark").val();
        var m = '<?=md5($userid.$f_id."kyarata75")?>';
        var u = '<?=$userid?>';
        var fid = '<?=$f_id;?>';
        var bEdit = false;
        var pid = 0;

        if ($('#watermark').attr('stype') == 'edit') {

            bEdit = true;
            pid = $("#watermark").attr('rel');
        }

        if (a != "") {

            var cid = '<?=$_REQUEST['id']?>';
            var name = '<?=$_REQUEST['name']?>';
            var filename = '<?=$_REQUEST['file']?>';
            var invitees = $(".as-values").val();
            $.post("/themes/maennaco/includes/posts.php?type=commInv", { cid: cid, name: name, filename: filename, invitees: invitees});

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
 /*   $('#shareButton').click(function () {
        var a = encodeURIComponent($("#watermark").val());
        var doc = $("#documenturl").attr("title");
        var m = $("#documenturl").attr("alt");
        var u = $("#documenturl").attr("name");
        var uid = '<?=$user->uid?>';
        if (a != "Discuss a topic or ask a question on this file ...") {
            var cid = '<?=$_REQUEST['id']?>';
            var name = '<?=$_REQUEST['name']?>';
            var filename = '<?=$_REQUEST['file']?>';
            var invitees = $(".as-values").val();
            $.post("/themes/maennaco/includes/posts.php?type=commInv", { cid: cid, name: name, filename: filename, invitees: invitees});
            $.post("/themes/maennaco/includes/posts.php?doc=" + doc + "&u=" + u + "&m=" + m + "&value=" + a + "&uid=" + uid + "&type=commAdd", {
            }, function (response) {
                $('#posting').prepend(response);
                $("#watermark").val("Discuss a topic or ask a question on this file ...");
                $('textarea').elastic();
                $(".commentMark").Watermark("Got advice / question on this topic?");
                $("#watermark").Watermark("watermark", "#369");
                $(".commentMark").Watermark("watermark", "#EEEEEE");
                $(".as-selection-item").each(function () {
                    $(this).remove()
                });
                $(".as-values").val("");
            });
        }
    });*/

    $('.commentMark').livequery("focus", function (e) {
        var parent = $('.commentMark').parent();
        $(".commentBox").children(".CommentImg").hide();
        var getID = parent.attr('id').replace('record-', '');
        $("#commentBox-" + getID).children("a#SubmitComment").show();
        $("#commentBox-" + getID).children(".CommentImg").show();
    });

    //showCommentBox
    $('a.showCommentBox').livequery("click", function (e) {
        var getpID = $(this).attr('id').replace('post_id', '');
        $("#commentBox-" + getpID).css('display', '');
        $("#commentMark-" + getpID).focus();
        $("#commentBox-" + getpID).children("CommentImg").show();
        $("#commentBox-" + getpID).children("a#SubmitComment").show();
    });

    //SubmitComment
    $('a.comment').livequery("click", function (e) {
        var pid = getpID = $(this).attr('id').replace('SubmitComment-', '');
        var comment_text = encodeURIComponent($("#commentMark-" + getpID).val());
        var m = $(this).attr("m");
        var uid = '<?=$user->uid?>';
        var bEdit = false;
        var cid = 0;

        if ($('#commentMark-' + pid).attr('stype') == 'edit') {

            bEdit = true;
            cid = $('#commentMark-' + pid).attr('rel');
        }
        if (comment_text != "Got advice / question on this topic?") {
            $.post("/themes/maennaco/includes/add_comment.php?type=pro_file_comment&value=" + comment_text + "&pid=" + getpID + "&m=" + m + "&uid=" + uid + "&bEdit=" + bEdit + "&cid=" + cid, {
            }, function (response) {
                $('#CommentPosted' + getpID).append($(response).show());
                $("#commentMark-" + getpID).val("Got advice / question on this topic?");
                if (bEdit) {
                    $("#commentMark-" + pid).attr("stype",'');
                    $("#commentMark-" + pid).attr("rel",'');
                    $('#comment-' + cid).find('div.subcomment_text').html(decodeURIComponent(comment_text));
                    $('#subcomment-' + cid).html(decodeURIComponent(comment_text));

                }
            });
        }
    });

    //more records show
    $('a.more_records').livequery("click", function (e) {
        var next = $('a.more_records').attr('id').replace('more_', '');
        $.post("/themes/maennaco/includes/posts.php?show_more_post=" + next, {
        }, function (response) {
            $('#bottomMoreButton').remove();
            $('#posting').append($(response).show());
        });
    });

    //deleteComment
    $("a[id^='remove_id']").livequery('click', function () {


        m = $(this).attr('m');
        pid = $(this).attr('pid');

        $.post("/themes/maennaco/includes/delete_comment.php?type=pro_file_post", {

            pid: pid,
            m: m

        }, function (response) {

            if (response.status == 'success') {

                alert(response.display);
                $("#record-" + pid).hide();

            }
            else alert(response.display);
        }, "json");

    });
    //delete SubComment
    $("a[id^='cid-']").livequery('click', function (event) {

        event.preventDefault();

        m = $(this).attr('m');
        cid = $(this).attr('cid');

        $.post("/themes/maennaco/includes/delete_comment.php?type=pro_file_comment", {

            cid: cid,
            m: m

        }, function (response) {

            if (response.status == 'success') {

                alert(response.display);
                $("#comment-" + cid).hide();

            }
            else alert(response.display);
        }, "json");

    });
/*    $('a.c_delete').livequery("click", function (e) {
        if (confirm('Are you sure you want to delete this comment?') == false) {
            return false;
        }
        e.preventDefault();
        var c_id = $(this).attr('id').replace('CID-', '');
        var u = $("#documenturl").attr("name");
        var m = $(this).attr("alt");
        $.ajax({
            type: 'get',
            url: '/themes/maennaco/includes/delete_comment.php?c_id=' + c_id + "&u=" + u + "&m=" + m,
            success: function () {
                $('#comment-' + c_id).remove();
            }
        });
    });*/
    /// hover show remove button
    $('.friends_area').livequery("mouseenter", function (e) {
        $(this).children("label.name").children("a.delete").show();
    });
    $('.friends_area').children("label.name").livequery("mouseleave", function (e) {
        $('a.delete').hide();
    });
    /// hover show remove button
    $('a.delete').livequery("click", function (e) {
        if (confirm('Are you sure you want to delete this post?') == false) {
            return false;
        }
        e.preventDefault();
        var parent = $('a.delete').parent();
        var temp = $(this).attr('id').replace('remove_id', '');
        var u = $(this).attr("name");
        var m = $(this).attr("alt");
        var main_tr = $('#' + temp).parent();
        $.ajax({
            type: 'get',
            url: '/themes/maennaco/includes/delete.php?id=' + temp + "&u=" + u + "&m=" + m,
            success: function () {
                $('#record-' + temp).remove();
            }
        });
    });
    $('textarea').elastic();
    jQuery(function ($) {
        $("#watermark").Watermark("Discuss a topic or ask a question on this file ...");
        $(".commentMark").Watermark("Got advice / question on this topic?");
    });
    jQuery(function ($) {
        $("#watermark").Watermark("watermark", "#369");
        $(".commentMark").Watermark("watermark", "#EEEEEE");
    });
    function UseData() {
        $.Watermark.HideAll();
        $.Watermark.ShowAll();
    }



});
</script>
<div align="center">
    <div id="documenturl" title="<?php echo basename($url) ?>" name="<?php echo $editorname ?>"
         alt="<?php echo md5($editorname . "kyarata75") ?>" class="UIComposer_Box" align="center" width="0" height="0">
    </div>
    <?php if (strlen($url) > strlen('http://www.clewed.com')) { ?>        
		<?php
			$filePath = (isset($_SERVER["HTTPS"]) ? preg_replace("/^http:/i", "https:", urldecode($url)) : urldecode($url));
			if(pathinfo($filePath, PATHINFO_EXTENSION)=="pdf"){
				$viewerTypeUrl = (isset($_SERVER["HTTPS"]) ? 'https' : 'http')."://".$_SERVER['HTTP_HOST']."/themes/maennaco/pdf.js-master/web/viewer.html?file=";
			}else{
				$viewerTypeUrl = "https://docs.google.com/viewer?url=";?>
				<div style="position: absolute; display: block; height: auto; left: 572px; top: 33px; width: 30px; z-index: 1000;background: #F5F5F5; height: 30px;"></div>
		<?php
			}
		?>
        <iframe src="<?= $viewerTypeUrl ?><?= $filePath ?>&embedded=true" width="600"
                height="670" style="border: none;"></iframe>
        <br>
        <br><br>
        <div style="position: relative;margin-bottom:-22px; ">
            <form action="" method="post" name="postsForm">
                <div class="UIComposer_Box">
                    <span class="w">
                        <textarea class="input" id="watermark" name="watermark"
                                  style="outline:none; color:#9e9fa4!important;height:25px;line-height:25px;border: 1px solid #CCCCCC !important;"
                                  cols="64"></textarea>
                    </span>
                    <br clear="all"/>
                    <div id="submitDiv" align="right" style="display:none; height:80px; ">
                        <input type='text' id="commentsInv" name="commentsInv" style="color:333;"/>
                        <a id="comm_shareButton" class="tool button" style="padding: 5px 17px; background:#43a0c1!important">Submit</a>
                        <a id="closeButton" class="tool button" style="padding: 5px 17px;">Close</a>
                    </div>
                    <div style="clear: both;"></div>
                </div>
            </form>
            <div style="clear: both;">&nbsp;</div>
        </div>
        <div class="tabtags cmtloop" id="posting" align="center">
            <?php
            include('dbcon.php');
            $check_res = mysql_query("SELECT * FROM wall_documents where document_name = '" . basename($url) . "'");
            $check_result = mysql_num_rows(@$check_res);
            if ($check_result == 0) {
                mysql_query("INSERT INTO wall_documents (document_name) VALUES('" . basename($url) . "')");
            }
            $op_comments = 'edit';
            $op_subcomments = 'edit'; // File tab file commenting is not part of permission table so permission access is hard-coded (currently you are able to edit and delete your content unless super or admin)
            include_once('pro_comments_posts.php');
            //include_once('posts.php');
            ?>
        </div>
    <?php
    } else {
        echo '<div style="margin-top:50px;">Please select a file to view here.</div>';
    } ?>
</div>
</div>
