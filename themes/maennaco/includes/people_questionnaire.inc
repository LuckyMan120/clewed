<?php

function questionnaire_list($op = null){
    global $user;
    $editorid = $user->uid;
    $roleid = userRoleId($editorid);
    $time = time();

    $Block['title'] = '';
    $content = '';

    if($op == null || $op == 'view')
    {
        $Q = array();
        $sql = "select * from maenna_questionair where target='pro' and parentid is null and status = 1";
        $result = db_query($sql);
        while($Row = db_fetch_object($result))
        {
            $title = htmlentities($Row->content, ENT_QUOTES);
            $questionid = $Row->recordid;
            $Q[] .= "\n <a href='/account?a=pro&panel=questionnaire_panel&id=$questionid'>$title</a>";
        }
        if(count($Q) > 0)$content = implode("<br>", $Q);
    }

    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}
function questionnaire($op = null)
{
    global $user;
    $uid = $user->uid;
    $rid = userRoleId($uid);
    $time = time();

    $Block['title'] = '';
    $content = '';

    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_questionair where target='pro' and status = 1 and parentid is null order by recordid";
        $result = db_query($sql);
        $Row = db_fetch_object($result);
        if($Row)
        {
            $parentid = $Row->recordid;
            $Block['title'] = htmlentities(strtoupper($Row->content), ENT_QUOTES);

            $submit = sget($_REQUEST, 'submit');


            if( answered_all($parentid, $uid, $rid)  )
            {
                if($submit){ // user finished submit
                    $content .= "<div style='font:14px arial'>Thank you for complete the questionnaire.</div>";
                }else{ // user browse action
                    $content .= "<div style='font:14px arial'>You have completed the questionnaire. Thank you.</div>";
                }
            }else{
                // Check how many user input: questions and file uploaders

                $sql = "select Q.*,A.qaid as answerid,  A.answer from maenna_questionair as Q
                        left join maenna_qa as A
                            on Q.recordid = A.qid and A.uid = %d
                        where parentid = %d and (usertype = %d or usertype = 0 ) order by weight, recordid";
                $result = db_query($sql, array($uid, $parentid, $rid));
                $morePage = false;

                $pageCnt = 1;
                $page  = sget($_REQUEST, 'page');
                if(empty($page))$page = 1;
                $preType = '';
                $counter = 1;
                while($Row = db_fetch_object($result)){

                    $type = $Row->type;
                    $questionid = $Row->recordid;

                    if($type == 'pagedivider'){
                        if($preType != 'pagedivider')$pageCnt++;

                    }elseif($type == 'title'){
                        if($pageCnt == $page)
                        {
                            $questionid = $Row->recordid;
                            $subtitle = htmlentities($Row->content);
                            $top = "<a href='#top' class='tool'>Top</a>";
                            if($topCount == 1)$top = '';
                            $topCount++;
                            $content .= "\n<a name='t_$questionid'></a><div class='subtitle'>$subtitle
                                        <div class='editbtn' style='top:6px;border:none;text-align:right'>$top</div>
                                    </div>";

                        }
                    }elseif($type == 'question')
                    {
                        if($pageCnt == $page)
                        {
                            $answerid = $Row->answerid;
                            $question = htmlentities($Row->content, ENT_QUOTES);
                            $answer = $Row->answer;
                            $content .= "\n<div class='entry'><a name='q_$questionid' style='display:hidden'></a>";
                            $content .= "\n<div class='entry-title'>$counter. $question</div>";
                            $content .= "\n<div class='entry-content'><textarea name='q_$questionid' class='entry-textarea'>$answer</textarea></div>";
                            $content .= "<div style='height:12px;position:relative;text-align:right'>
                                            <div class='tool' style='position:absolute;left:0;top:0;text-align:left;width:400px;font-style:italic;' >&nbsp;</div>
                                            <a href='#' class='tool review-btn'>REVIEW</a> &nbsp;&nbsp;&nbsp;
                                            <a href='#' class='tool save-btn' answerid='$answerid' qid='$questionid'>SAVE</a>
                                        </div>";
                            $content .= "\n</div>";
                        }
                        $counter++;
                    }elseif($type == 'instruction'){
                        if($pageCnt == $page)
                        {
                            $instruction = htmlentities($Row->content, ENT_QUOTES);
                            $content .= "\n<div style='margin-top:8px;font: 13px Signika'>$instruction</div>";
                        }
                    }elseif($type == 'fileuploader'){
                        if($pageCnt == $page && 0)
                        {
                            $loaderTitle = htmlentities($Row->content, ENT_QUOTES);
                            $filename = $Row->answer;
                            if(empty($filename)) $file = "";
                            else {
                                $path =  "./" . file_directory_path() . "/" . $filename;
                                $file = "<a href='$path' target='_blank'>$filename</a>";
                            }
                            $questionid = $Row->recordid;
                            $loaderTitle = htmlentities($Row->content, ENT_QUOTES);
                            $filename = $Row->answer;
                            if(empty($filename)) $file = "";
                            else {
                                $path =  "./" . file_directory_path() . "/" . $filename;
                                $file = "<a href='$path' target='_blank'>$filename</a>";
                            }
                            $content .= "\n<div style='margin-top:20px;'><a name='q_$questionid' style='display:hidden'></a>";
                            $content .= "\n<div style='font: 13px Signika;'>$counter. $loaderTitle</div>";
                            $content .= "\n<div style='margin-top:8px;padding:5px;border:solid 1px #f3f3f3'><input type=file name='f_$questionid' /><br>$file</div>";
                            $content .= "\n</div>";
                        }
                        $counter++;
                    }
                    $preType = $type;
                }

                $prePage = $nextPage = $submit = '';
                if($page > 1 && $pageCnt > 1){
                    $pre= $page -1;
                    $prePage = "<a href='/account?a=pro&panel=questionnaire_panel&id=$parentid&page=$pre' class=button>previous page</a> &nbsp;&nbsp;&nbsp;";
                }
                if($pageCnt > 1 && $page < $pageCnt ){
                    $next = $page + 1;
                    $nextPage = "<a href='/account?a=pro&panel=questionnaire_panel&id=$parentid&page=$next' class=button>next page</a>&nbsp;&nbsp;&nbsp;";
                }
                if($pageCnt == $page){
                    $submit = "<input type=submit name=submit value='Submit' class=button />";
                }else{
                    $submit = "<input type=submit name=submit value='save and continue' class=button />";
                }

                $content = <<< END
                <form method='post' action='/account' enctype='multipart/form-data'>
                    <div style='position:relative;width:100%;'>
                    $content
                    <input type=hidden name=a value=questionnaire />

                    <input type=hidden name=parentid value='$parentid' />
                    <input type=hidden name=update_section value='questionnaire' />
                    <input type='hidden' name='page' value='$next' />
                        <div style='margin-top:15px'>
                        $prePage $nextPage $submit
                        <div>
                    </div>
                </form>
                <div id="dialog" title="Basic dialog">
                    This is the default dialog which is useful for displaying information. The dialog window can be moved, resized and closed with the 'x' icon.
                </div>
                <script>
                    var ajaxobj = {};
                    var save_btn_obj = {};
                    function hide_ajax_obj(res)
                    {
                        if(res.status == 'ok'){
                            ajaxobj.html(res.content);
                        }else if(res.status == 'new'){
                            ajaxobj.html(res.content);
                            if(res.answerid == '')
                            {
                                ajaxobj.html("Invalid operation. failed to save content");
                            }else{
                                save_btn_obj.attr('answerid', res.answerid);
                            }

                        }else{
                            ajaxobj.html('');
                        }
                    }
                    function openDialog(boxtitle, boxcontent){

                                $( "#dialog" ).dialog({
                                        autoOpen: true,
                                        width: 800,
                                        title: boxtitle,
                                        height:500,
                                        buttons: { "Ok": function() { $(this).dialog("close"); }},
                                        closeOnEscape: true,
                                        modal:true
                                    }).html(boxcontent);

                    }
                    $(document).ready(function(){

                            $( "#dialog" ).dialog({
                                        autoOpen: false
                                    });
                            $(".review-btn").each(function(){
                                $(this).bind('click', function(evt){
                                    evt.preventDefault();
                                    boxtitle = $(this).parent().prev().prev().text();
                                    boxcontent =  $(this).parent().prev().children(':first-child').val() ;
                                    if(boxcontent != '') boxcontent = boxcontent.replace(/\\n/g,'<br />');

                                    openDialog(boxtitle, boxcontent);

                                })
                            });
                            $(".save-btn").each(function(){
                                $(this).bind('click', function(evt){
                                    evt.preventDefault();
                                    save_btn_obj = $(this);
                                    var content = $(this).parent().prev().children(':first-child').val() ;
                                    ajaxobj = $(this).parent().children(':first-child');
                                    ajaxobj.html("<img src='/ajax-loader.gif' />");

                                    answerid = $(this).attr('answerid');
                                    questionid = $(this).attr('qid');
                                    if(questionid != '')
                                    {
                                        POSTDATA = "a=maennaco_ajax&action=ajaxTrigger&table=update_qa&answerid="+answerid +"&qid=" + questionid + "&text=" + encodeURIComponent(content);
                                        ajax_update(POSTDATA, hide_ajax_obj);
                                    }
                                })
                            });

                    });
                </script>
END;
            }
        }
    }elseif($op == 'update'){
        $submit = sget($_REQUEST, 'submit');
        $parentid = sget($_REQUEST, 'parentid');
        $success = false;
        $message = "Nothing to update";
        $post_data = serialize($_POST);
        backup_copy($post_data, 'questionnaire_pro');
        foreach($_POST as $key => $val)
        {
            if(substr($key, 0 ,2 ) == 'q_'){
                $qid = str_replace('q_', '', $key);
                if((intval($qid) > 0) && (! empty($val)))
                {
                    $sql_new = "insert into maenna_qa (qid, qtype, uid, answer, created) values (%d, 'question', %d, '%s', '%s' ) on duplicate key update answer = '%s', created = '%s'";

                    $DBValues = array($qid, $uid, $val, $time, $val, $time);
                    db_query($sql_new, $DBValues);
                    $success = true;


                }
            }
        }

        foreach($_FILES as $key => $FILE)
        {
            if( (substr($key, 0, 2) == 'f_') && ($FILE['error'] == 0)){
                $qid = str_replace('f_', '', $key);
                if((intval($qid) > 0) && (! empty($FILE)))
                {

                    $name = $FILE['name'];
                    $type = $FILE['type'];
                    $filename = time() . '_' . clearString($name);
                    $path = "./" . file_directory_path() . "/" . $filename;
                    if(! move_uploaded_file($FILE['tmp_name'], $path)){
                        drupal_set_message("Error: the file $name wasn't uploaded", 'error');
                    }else{
                        $sql_new = "insert into maenna_qa (qid, qtype, uid, answer, created) values(%d, '%s', %d, '%s', '%s') on duplicate key update answer = '%s' , created = '%s', qtype = '%s'";

                        $DBValues = array($qid, $type, $uid, $filename, $time, $filename, $time,$type);
                        if( !db_query($sql_new, $DBValues)){
                            drupal_set_message("Failed to upload file", 'error');
                            return;
                        }else{
                            $success = true;
                        }

                    }
                }
            }
        }
        if($success){
            drupal_set_message("Questionnaire record updated");
        }else{
            drupal_set_message("Nothing to update", 'error');
        }
        return '';

    }

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function answered_all($questionnaireid, $uid, $roleid)
{
    $sql = "select Q.*, A.answer from maenna_questionair as Q
                    left join maenna_qa as A
                        on Q.recordid = A.qid and A.uid = %d
                    where parentid = %d and (usertype = %d or usertype = 0 ) and ((type = 'question') or (type = 'fileuploader'))";
    $result = db_query($sql, array($uid, $questionnaireid, $roleid));
    $completed = true;
    while($Row = db_fetch_object($result))
    {
        if(empty($Row->answer)) $completed = false;
    }
    return $completed;
}

/* EOF */
