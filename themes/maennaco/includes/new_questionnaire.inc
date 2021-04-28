<?php

function questionnaire($op = null)
{
    global $AccessObj;

    $time = time();
    $content = '';
    $redirect = rebuild_url(array('tab'));

    if($AccessObj->user_type == 'people')
    {
        $target = 'pro';
    }elseif($AccessObj->user_type == 'company')
    {
        $target = 'company';
    }else
    {
        return '';
    }

    if($op && $op != 'update'){
        $highlight = isset($_REQUEST['update_section']) && $_REQUEST['update_section'] == 'questionnaire';

        //Hard code for Authors and Consultants to use questionnaire of other experts questionnaire,needs further instructions on this
        if ($AccessObj->role_id == 13 || $AccessObj->role_id == 14) {

            $pro_type = 7;
        }

        else $pro_type = $AccessObj->role_id;

        $sql = "select  *
                from    maenna_questionair
                where   status = 1 and
                        target='%s' and usertype= %d and
                        parentid is null
                order by recordid asc";
        $result = db_query($sql,array($target,$pro_type));
         $Row = db_fetch_object($result);

        if ($Row)
        {
            $parentid = $Row->recordid;
            $Block['title'] = $Row->content;

            $status = answered_all($parentid, $AccessObj->uid);

            if($status)
            {

                if($submit){ // user finished submit
                    $content .= "<div style='font:14px arial'>Thank you for complete the questionnaire.</div>";
                }else{ // user browse action
                    $content .= "<div style='font:14px arial'>You have completed the questionnaire on ".date('Y/m/d',$status).". Thank you.</div>";
                }
            }else{
                if($AccessObj->user_type == 'people')
                {
                    $sql =  "select     Q.*,A.qaid as answerid,
                                        A.answer
                            from        maenna_questionair as Q
                            left join   maenna_qa as A
                            on          Q.recordid = A.qid and
                                        A.uid = %d
                            where       parentid = %d
                            order by weight, recordid";
                    $DBValues =  array($AccessObj->uid, $parentid, $AccessObj->role_id);
                    $Block['title'] = strtoupper(Options_proType($AccessObj->role_id, 1)) . ' QUESTIONNAIRE';
                }else
                {
			// This query is used to get the logged in company type
			$cmpqry = "SELECT membership FROM maenna_company WHERE companyid = '%s'";
			$cmp_type_res = db_query($cmpqry, array($AccessObj->uid));
			$cmp_type = db_fetch_object($cmp_type_res);
			//end of helping query

                    $sql = "select  Q.*,
                                A.answer,
                                A.qaid as answerid
                        from    maenna_questionair as Q
                        left join maenna_qa as A on
                                Q.recordid = A.qid and
                                A.uid = %d
                        where   Q.parentid = %d and (usertype = '%s')
                        order by weight, recordid";
                    $DBValues = array($AccessObj->uid, $parentid,$cmp_type->membership);
                }

                $result = db_query($sql, $DBValues);

                $_page = sget($_REQUEST, '_page');
                if(empty($_page)) $_page = 1;
                $morePage = false;
                $pageCnt = 1;
                $curPage = $_page;
                $preType = '';
                $counter = 1;
                $topCount = 1;
                while($Row = db_fetch_object($result)){
                    $type = $Row->type;

                    $questionid = $Row->recordid;
                    if($type == 'pagedivider'){
                        if($preType != 'pagedivider')$pageCnt++;

                    }elseif($type == 'title'){
                        if($pageCnt == $_page)
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
                        if($pageCnt == $_page)
                        {
                            $answerid = $Row->answerid;
                            $question = htmlentities($Row->content, ENT_QUOTES);
                            $answer = $Row->answer;

                            $entry_class = "";
                            if($highlight && empty($answer)) {
                              $entry_class = 'question-highlight';
                            }

                            $content .= "\n<div class='entry ".$entry_class."'><a name='q_$questionid' style='display:hidden'></a>";
                            $content .= "\n<div class='entry-title'>$counter. $question</div>";
                            $content .= "\n<div class='entry-content'><textarea name='q_$questionid' class='entry-textarea'>$answer</textarea></div>";
                            $content .= "<div style='height:12px;position:relative;text-align:right'>
                                            <div class='tool' style='position:absolute;left:0;top:0;text-align:left;width:400px;font-style:italic;' >&nbsp;</div>
                                            <a href='#' class='tool review-btn'>Review</a> &nbsp;&nbsp;&nbsp;
                                            <a href='#' class='tool save-btn' answerid='$answerid'  qid='$questionid'>Save</a>
                                        </div>";
                            $content .= "\n</div>";

                        }
                        $counter++;
                    }elseif($type == 'instruction'){
                        if($pageCnt == $_page)
                        {
                            $instruction = htmlentities($Row->content, ENT_QUOTES);
                            $content .= "\n<div style='margin-top:8px;font: 12px LatoRegular'>$instruction</div>";
                        }
                    }elseif($type == 'fileuploader')
                    {
                        if($pageCnt == $_page && 0)
                        {
                            $questionid = $Row->recordid;
                            $loaderTitle = htmlentities($Row->content, ENT_QUOTES);
                            $filename = $Row->answer;
                            if(empty($filename)) $file = "";
                            else {
                                $path =  "./" . file_directory_path() . "/" . $filename;
                                $file = "<a href='$path' target='_blank'>$filename</a>";
                            }
                            $content .= "\n<div style='margin-top:20px;'><a name='q_$questionid' style='display:hidden'></a>";
                            $content .= "\n<div style='font: 12px LatoRegular;'>$counter. $loaderTitle</div>";
                            $content .= "\n<div style='margin-top:8px;padding:5px;border:solid 1px #f3f3f3'><input type=file name='f_$questionid' /><br>$file</div>";
                            $content .= "\n</div>";

                        }
                        $counter++;
                    }
                    $preType = $type;
                }
                // Page navigation by divider
                $prePage = $nextPage = $submit = '';
                if($_page > 1 && $pageCnt > 1){
                    $pre= $_page -1;
                    $prePage = "<a href='$redirect&panel=questionnaire&id=$parentid&_page=$pre' class=button>previous page</a> &nbsp;&nbsp;&nbsp;";
                }
                if($pageCnt > 1 && $_page < $pageCnt ){
                    $next = $_page + 1;
                    $nextPage = "<a href='$redirect&panel=questionnaire&id=$parentid&_page=$next' class=button>next page</a>&nbsp;&nbsp;&nbsp;";
                }
                if($pageCnt == $_page){
                    $submit = "<input type=submit name=submit value='Submit' class=button />";
                }else{
                    $submit = "<input type=submit name=submit value='save and continue' class=button />";
                }
                // Show view as userType for Pro

                $HV = hidden_post_values(array('tab'));
}
                $content = <<< END

                <form method='post' id='pro-questionnaire' onsubmit='return confirm("Submitting will close access to the questionnaire. Please confirm you want to close the questionnaire by clicking Ok. Otherwise, click Cancel and use the Save button under each question to save.");' action='/account'>
                    <div style='position:relative;width:100%;'>
                    $content
                    $HV
                    <input type=hidden name=parentid value='$parentid' />
                    <input type=hidden name=update_section value='questionnaire' />
                    <input type='hidden' name='_page' value='$next' />
                        <div style='margin-top:15px;'>$prePage $nextPage $submit</div>
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
                                        POSTDATA = "a=maennaco_ajax&action=ajaxTrigger&table=update_qa&answerid="+ answerid + "&qid=" + questionid + "&text=" + encodeURIComponent(content);
					ajax_update(POSTDATA, hide_ajax_obj);
                                    }
                                })
                            });

                    });
                </script>

                <style>

                table.account-table td.right-td {
                border-left:none !important;
                }
                </style>

END;
            }

    }elseif($op == 'update')
    {
       //die($op);
        $submit = sget($_REQUEST, 'submit');
        $parentid = sget($_REQUEST, 'parentid');
        $success = false;
        $message = "Nothing to update";
        $post_data = serialize($_POST);
        backup_copy($post_data, 'questionnaire_com');
        foreach($_POST as $key => $val)
        {
            if(substr($key, 0 ,2 ) == 'q_'){
                $qid = str_replace('q_', '', $key);
                if((intval($qid) > 0))
                {


                    $sql_new = "insert into     maenna_qa (qid, qtype, uid, answer, created)
                                values (%d, 'question', %d, '%s', '%s' )
                                on duplicate key update answer = '%s', created = '%s'";

                    $DBValues = array($qid, $AccessObj->uid, $val, $time, $val, $time);
                    if(! db_query($sql_new, $DBValues))
                    {
                        $message = "";
                        drupal_set_message("Failed to update questionnaire record", 'error');
                        return;
                    }else{
                        $success = true;
                    }

                }
            }
        }

        if( answered_all($parentid, $AccessObj->uid)  )
        {
            drupal_set_message("Thank you for completing your questionaire.");

            $to      =  'clewed@clewed.com';
            $subject = 'New Questionnaire completed';
            $message.=  'Professional '.ucwords(getUserById($AccessObj->uid)).' has completed his/her questionnaire.<br>Please approve';
            $headers = "From:clewed@clewed.com \r\n";
            $headers .= "Content-type: text/html; charset=iso-8859-1 \r\n";


            mail($to, $subject, $message, $headers) or die("Message couldn`t be send. Please try again!");
            return;
        } else {
            drupal_set_message("Your information is updated. Please complete remaining sections and submit your information to begin.");
        }

        if(!$success){
            drupal_set_message("Nothing to update", 'error');
        }
        return;
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function pro_profile_panel($op = null)
{

    global $AccessObj;

    $editorid = $AccessObj->uid;
    $id = $AccessObj->uid;
    $time = time();
    $Block['title'] = "PROFESSIONAL PROFILE";

    $section = __FUNCTION__;
    $panel = __FUNCTION__;
    $content = '';
    $redirect = rebuild_url(array('tab'));
    if(empty($op)) return '';
    elseif($op && $op != 'update')
    {
        $mgmt_level = $experties = $industry = $industry2 = $school = $degree = '';
        $sql = "select * from maenna_people where pid = %d limit 1";
        $result = db_query($sql, array($id));
        $Maenna_people = db_fetch_array($result);
        $yearwork = sget($Maenna_people, 'yearwork');
        $industry = sget($Maenna_people,'industry');
        $industry2 = sget($Maenna_people, 'industry2');
        $profile = sget($Maenna_people, 'profile');
        $sql  = "select * from maenna_people_data
                where data_type = '%s' and pid = %d order by access desc limit 1";
        $result = db_query($sql, array('employment', $id));
        $Row = db_fetch_object($result);
        if($Row)
        {
            if($Row->data_value3)
            {
                //management level
                $mgmt_level = $Row->data_value3;
            }
            if( $Row->data_value2)
            {
                //experties
               $experties = $Row->data_value2;
            }

        }

        $sql  = "select * from maenna_people_data
                where data_type = '%s' and pid = %d order by access desc limit 1";
        $result = db_query($sql, array('education', $id));
        $Row = db_fetch_object($result);
        if($Row)
        {

            $school = $Row->data_value;
            $degree = $Row->data_value2;
        }
         global $base_url;

        $content .= "
        <script type='text/javascript'>

        $(document).ready(function() {

        $('#maenna-forms-pro-form-page2').submit(function(e) {
         var self = this;
         e.preventDefault();
            if(validate()){
         self.submit();
     }
     else (alert('You cannot leave empty fields. Please sumbit empty fields.'));
   });

    function validate() {

    var status = true;

   $('#maenna-forms-pro-form-page2 :input').each(function() {

    $(this).attr('style','background-color:none;');



   if ($(this).attr('id') == 'edit-pro-type' || $(this).attr('id') == 'edit-first-name' || $(this).attr('id') == 'edit-last-name' || $(this).attr('id') == 'edit-experties' ) {

        if ($(this).val() == '') {

             $(this).attr('style','background-color:rgb(255, 228, 228) !important');
             //alert($(this).attr('id'));
             status = false;

        }

   }



   });
   return status;

}
});

        </script>
        ";

    	 $content .= drupal_get_form('maenna_forms_pro_form_page2');

    }elseif($op == 'update')
    {

        //$rid = sget($_REQUEST, 'rid');
        $yearwork =     sget($_REQUEST,'year');
        $industry =     sget($_REQUEST, 'industry');
        $industry2 =    sget($_REQUEST, 'industry2');
        $experties =    sget($_REQUEST, 'experties');
        $mlvl =         sget($_REQUEST, 'managementlevel');
        $school =       sget($_REQUEST, 'school');
        $degree =       sget($_REQUEST, 'degree');
        $id =           $AccessObj->uid;

	    // resize_and_upload_image($_FILES['profile'],$id);

        if(empty($id)) return '';
        var_dump($id);die;
        $sql = "update maenna_people set    yearwork    = '%s',
                                            industry    = '%s',
                                            industry2   = '%s',
                                            experties  = '%s'
                where pid = %d limit 1";

        if(! db_query($sql, array($yearwork, $industry, $industry2,$experties, $id)))
        {
            drupal_set_message('Failed to update record - 001','error');
            return;
        }
        // experties = data_value2, mlevel = data_value3
        $sql = "update maenna_people_data set   data_value2 = '%s',
                                                data_value3 = '%s'
                where data_type = 'employment' and pid = %d limit 1";
        if(! db_query($sql, array($experties, $mlvl, $id)))
        {
            drupal_set_message('Failed to update record - 002','error');
            return;
        }
        $sql = "update maenna_people_data set   data_value = '%s',
                                                data_value2 = '%s'
                where data_type = 'education' and pid = %d limit 1";
        if(! db_query($sql, array($school, $degree, $id)))
        {
            drupal_set_message('Failed to update record - 003','error');
            return;
        }
        drupal_set_message('Record updated successfully');
        return '';
    }

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function answered_all($questionnaireid, $uid)
{
    $sql = "select Q.*, A.answer,A.created as antime from maenna_questionair as Q
                    left join maenna_qa as A
                        on Q.recordid = A.qid and A.uid = %d
                    where parentid = %d and ((type = 'question') or (type = 'fileuploader'))";
    $result = db_query($sql, array($uid, $questionnaireid));
    $completed = true;
    $last_answer = 0;
    while($Row = db_fetch_object($result))
    {
        if(empty($Row->answer)) $completed = false;
        else {

            if ($Row->antime > $last_answer) $last_answer = $Row->antime;

        }
    }
    if ($completed) $completed = $last_answer;
    return $completed;
}

function pro_accreditation($op = null)
{
    //die("op=".$op);
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "CHECK THE BOXES TO CONFIRM YOUR ACCREDITATION";
    $view = sget($_REQUEST, 'view');

    $acc_data = getAccreditationData($pid);


    if($op && $op != 'update')
    {
        $content = ' <p style="font-family: Lato Light; font-size: 14pt; margin-left:15px;padding-top:30px;margin-top:0px !important;">Please confirm you are an accredited investor by checking the box below:</p> ';

        $HV = hidden_post_values(array('tab','type','id'));

        $content .= '

       <form method="post" id="pro-accreditation" action="/account" enctype="multipart/form-data">
        '.$HV.'
        <input type="hidden" name=update_section value="pro_accreditation" />
        <sectionxmlns="http://www.w3.org/1999/html">

        <div class="chkbox">
                <input type="checkbox" value="1" id="chk1" name="chk[1]" '.$acc_data[1].' />
                <label for="chk1"></label>
            </div>
             <p style="font-family: Lato Italic; font-size:14pt;">I made $200,000 or more in each of the two most recent years and believe I will make at least that much this year.</p>
        </section>
        <sectionxmlns="http://www.w3.org/1999/html">

        <div class="chkbox">
                <input type="checkbox" value="1" id="chk2" name="chk[2]" '.$acc_data[2].'/>
                <label for="chk2"></label>
            </div>
             <p style="font-family: Lato Italic; font-size:14pt;">I have a joint income with my spouse that has exceeded $300,000 for each of the last 2 years and I expect it will exceed that again this year.</p>
        </section>
        <sectionxmlns="http://www.w3.org/1999/html">

        <div class="chkbox">
                <input type="checkbox" value="1" id="chk3" name="chk[3]" '.$acc_data[3].' />
                <label for="chk3"></label>
            </div>
             <p style="font-family: Lato Italic; font-size:14pt;">I have an individual net worth, or joint net worth with my spouse, that exceeds $1,000,000 today excluding my primary residence.</p>
        </section>
        <sectionxmlns="http://www.w3.org/1999/html">

        <div class="chkbox">
                <input type="checkbox" value="1" id="chk4" name="chk[4]" '.$acc_data[4].' />
                <label for="chk4"></label>
            </div>
             <p style="font-family: Lato Italic; font-size:14pt;">I am a representative of a bank, insurance company, registered investment company, business development company, or small business investment company interested in investment opportunities on the Clewed site.</p>
        </section>
       <div class="backbtn">
                                <input type="submit" name="submit" value="Submit" class="button_add" onclick="return checkbox();">&nbsp;&nbsp;
									<a href="/account" class="tool"><input type="button" name="submit" value="Cancel" class="button_add"></a>
       </div>
       </form>
        <style>

       div.content_box, .box_content {

        background-color: #ECEDED;
        padding-left:0px !important;
        padding-top:0px !important;
        }

        .backbtn {

        margin-top:32px !important;
        margin-bottom:28px !important;
        margin-left:auto !important;
        margin-right:auto !important;
        width:215px !important;
        padding-bottom:28px;

        }

        .shaded_title {

        background-color:#94C9DA;
        color: white !important;
        padding-left:15px;
        height:32px;
        line-height: 32px;
        }

        .chkbox {
            width: 25px;
            margin: 10px 20px;
            position: relative;
            float:left;
        }
        .chkbox label {
            cursor: pointer;
            position: absolute;
            width: 15px;
            height: 15px;
            top: 0;
            left: 0;
            background: white;
            border:1px solid white;
            border-radius:2px;
        }
        .chkbox label:after {
            opacity: 0;
            content: "";
            position: absolute;
            width: 9px;
            height: 5px;
            background: transparent;
            top: 2px;
            left: 2px;
            border: 3px solid #333;
            border-top: none;
            border-right: none;
            -webkit-transform: rotate(-45deg);
            -moz-transform: rotate(-45deg);
            -o-transform: rotate(-45deg);
            -ms-transform: rotate(-45deg);
            transform: rotate(-45deg);
        }
        .chkbox label:hover::after {
            opacity: 0.5;
        }
        .chkbox input[type=checkbox]:checked + label:after {
            opacity: 1;
        }

        input.button_add {

             cursor:pointer;

        }

        input.button_add:hover {

             background: none repeat scroll 0 0 #013241 !important


        }

        </style>';
    }

    elseif ($op == 'update') {

        global $user;

        $insert = '';

        $pro_id = $user->uid;

        for($i =1;$i<5;$i++) {

            if ($_REQUEST[chk][$i] == 1)

                $insert .= "($pro_id,".strval(time()).",'accreditation','$i','1'),";

        }


        mysql_query ("DELETE FROM maenna_people_data WHERE pid = ".$pro_id." AND data_type='accreditation'") or die(mysql_error());

        if ($insert != '')
        {
            $insert = substr_replace($insert ,"",-1);

            mysql_query("INSERT INTO maenna_people_data (pid,access,data_type,data_attr,data_value) VALUES
        ".$insert."
        ") or die(mysql_error());

            header("Location: account?tab=professionals&page=pro_detail&id=".$pro_id);
            drupal_set_message('Operation successful');


          // header("Location: account?tab=professionals&page=pro_detail&id=".$pro_id);
            //$content = "<META http-equiv=\"refresh\" content=\"0;URL=/account?tab=professionals&page=pro_detail&id=".$_REQUEST['id']."\"";
        }

    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

/* EOF */
