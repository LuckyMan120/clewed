<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.autosuggest.js"></script>
<link href="/themes/maennaco/jui/comments/css/autosuggest.css" type="text/css" rel="stylesheet"/>
<script src="/themes/maennaco/jui/comments/js/jquery.elastic.js" type="text/javascript" charset="utf-8"></script>
<script src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js" type="text/javascript"></script>
<link href="/themes/maennaco/jui/comments/css/SpryTabbedPanels.css" type="text/css" rel="stylesheet"/>

<div id='docprev'>
    <script type="text/javascript">

        function n2br(text) {
            return text.replace(/\r?\n/g, '<br>');
        }

        function fileFormDisplay(id) {
            var $record = $('#record-' + id);
            $record.find('.commentBox').show();
            $record.find('textarea').focus();
        }

        function fileFormHide(id) {
            $('#record-' + id).find('.commentBox').hide();
        }

        function submitFilePost(el) {

            var $input = $(el).closest("form").find('.file-post-input');
            var a = $input.val();
            var u = '<?= $u = $userid ?>';
            var fid = '<?= $fid = $f_id;?>';
            var m = '<?= md5($u . $fid . "kyarata75")?>';
            var bEdit = false;
            var pid = 0;

            if (a != "") {

                $(el).attr('onclick', 'return false;');
                $.post("/themes/maennaco/includes/pro_comments_posts.php", {
                    value: a,
                    m: m,
                    uid: u,
                    fid: fid,
                    bEdit: bEdit,
                    pid: pid
                }, function (response) {

                    $('#posting').prepend($(response).hide().fadeIn());
                    $input.val("");
                    $(el).attr('onclick', 'submitFilePost(this);');

                });
            } else return alert('Please type your message');
        }

        function submitFileComment(postId, el) {

            var a = $("#commentMark-" + postId).val(),
                m = $(el).attr("m"),
                u = $(el).attr("uid"),
                sel_obj = $(el),
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

        function showFilePostEditor(postId, commentId) {

            var $messageContainer = $('.service-file-post[data-post-id=' + postId + ']');
            if (commentId)
                $messageContainer = $('.service-file-post[data-comment-id=' + commentId + ']');

            var $controls = $messageContainer.find('.comment_anchor'),
                $text = $messageContainer.find('.comment_text'),
                $editor = $messageContainer.find('.comment_editor'),
                $editorControls = $messageContainer.find('.comment_editor_controls');

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

        function submitFilePostEditor(postId, commentId) {

            var $messageContainer = $('.service-file-post[data-post-id=' + postId + ']');
            if (commentId)
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
                    if (commentId)
                        $submitButton.attr("onclick", "submitFilePostEditor(" + postId + "," + commentId + ");");
                    $message.html(n2br(text));
                    hideFilePostEditor(postId, commentId);
                }
                else {
                    alert("Please refresh the page and try again!");
                }

            }, "json");
        }

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


        <?php

        $tags = array();
        $connections = Connections::Com_conns($companyid);
        $connections = array_merge(
            !empty($connections['Advisor']) ? $connections['Advisor'] : array(),
            !empty($connections['Client']) ? $connections['Client'] : array()
        );

        foreach ($connections as $connection) {
            $uid = $connection->assignee_uid;
            $name = getProId($uid);
            $tags[] = "{value: \"{$uid}\", name: \"{$name}\"}";
        }

        $userService = new Clewed\User\Service();
        $companyId = (int) $_REQUEST['id'];
        $companies = $userService->get(array($companyId));
        $company = $companies[$companyId];
        if (!empty($company))
            $tags[] = "{value: \"{$companyId}\", name: \"{$company['full_name']}\"}";
        ?>

        $(function () {
            var tags = [<?php echo implode(',', $tags);?>];
            $(".file-invited-experts-container input").autoSuggest(tags, {
                selectedItemProp: "name",
                searchObjProps: "name"
            });

            $(".file-post-input").focus(function () {
                $(".file-invited-experts-container").show('fast');
            });

            $('.file-invited-experts-container .submit-button').attr('onclick', 'submitFilePost(this);');

            $('.file-invited-experts-container .close-button').click(function () {
                $(".file-invited-experts-container").hide('fast');
            });
        });
    </script>
    <div align="center">
        <div id="documenturl" title="<?php echo basename($url) ?>" name="<?php echo $editorname ?>"
             alt="<?php echo md5($editorname . "kyarata75") ?>" class="UIComposer_Box" align="center" width="0"
             height="0">
        </div>
        <?php if (strlen($url) > strlen('http://www.clewed.com')) { ?>
            <?php
            $filePath = (isset($_SERVER["HTTPS"]) ? preg_replace("/^http:/i", "https:", urldecode($url)) : urldecode($url));
            if (pathinfo($filePath, PATHINFO_EXTENSION) == "pdf") {
                $viewerTypeUrl = (isset($_SERVER["HTTPS"]) ? 'https' : 'http') . "://" . $_SERVER['HTTP_HOST'] . "/themes/maennaco/pdf.js-master/web/viewer.html?file=";
            } else {
                $viewerTypeUrl = "https://docs.google.com/viewer?url="; ?>
                <div
                    style="position: absolute; display: block; height: auto; left: 572px; top: 33px; width: 30px; z-index: 1000;background: #F5F5F5; height: 30px;"></div>
                <?php
            }

            $fileParts = explode('/', $filePath);
            $fileName = array_pop($fileParts);
            array_push($fileParts, rawurlencode($fileName));
            $filePath = implode('/', $fileParts);

            ?>
            <iframe src="<?= $viewerTypeUrl ?><?= $filePath ?>&embedded=true" width="600"
                    height="670" style="border: none;"></iframe>
            <br>
            <br><br>
            <div style="position: relative;margin-bottom:-22px; ">
                <form action="" method="post" name="postsForm">
                    <div class="UIComposer_Box">
                    <span class="w">
                        <textarea
                            placeholder="Add a comment"
                            class="input file-post-input"
                            style="width:98%;padding-left:10px;outline:none; color:#666666!important;height:70px;line-height:25px;border: 1px solid #CCCCCC !important;"
                            cols="64"
                            rows="7"></textarea>
                    </span>
                        <br clear="all"/>
                        <div class="file-invited-experts-container" style="display:none; height:80px; ">
                            <input type='text' placeholder="Invite users" style="color:#333;"/>
                            <a class="tool button submit-button" style="padding: 5px 17px; background:#43a0c1!important">Submit</a>
                            <a class="tool button close-button" style="padding: 5px 17px;">Close</a>
                        </div>
                        <div style="clear: both;"></div>
                    </div>
                </form>
                <div style="clear: both;">&nbsp;</div>
            </div>
            <div class="tabtags cmtloop" id="posting" align="center" style="margin-top: 15px;">
                <?php
                include('dbcon.php');
                $check_res = mysql_query("SELECT * FROM wall_documents where document_name = '" . basename($url) . "'");
                $check_result = mysql_num_rows(@$check_res);
                if ($check_result == 0) {
                    mysql_query("INSERT INTO wall_documents (document_name) VALUES('" . basename($url) . "')");
                }
                $op_comments = 'edit';
                $op_subcomments = 'edit'; // File tab file commenting is not part of permission table so permission access is hard-coded (currently you are able to edit and delete your content unless super or admin)
                include_once('file_comments_posts.php');
                ?>
            </div>
            <?php
        } else {
            echo '<div style="margin-top:50px;">Please select a file to view here.</div>';
        } ?>
    </div>
</div>
