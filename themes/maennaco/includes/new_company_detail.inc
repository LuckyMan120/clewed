<?php

use Clewed\Notifications\NotificationService;
use Clewed\User\Service as UserService;

include 'about.php';

function company_not_visible($op = null)
{
    echo '<style>
    .account-section-tabs {border-bottom:none !important;}
    .right-td {border-left:none !important;}
    </style>
    <div class="page-content"><span style="font-size:14px;font-family:LatoRegular,sans-serif;color:#839095;margin-left:110px;">This user is not yet visible. Please try later.</span></div>';
}

function questionnaire_old($op = null)
{
    global $user;
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "QUESTIONNAIRE";
    $content = '';
    if (($op != null) && ($op != 'update')) {
        // choose the most recent and active questionnaire entry
        $sql = "select * from maenna_questionair where status = 1 and target='company' and parentid is null order by recordid desc";
        $result = db_query($sql);
        $Row = db_fetch_object($result);
        if ($Row) {
            $Block['title'] = htmlentities(ucwords($Row->content), ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $parentid = $Row->recordid;
            $sql = "select Q.*, A.answer from maenna_questionair as Q
                    left join maenna_qa as A on
                        Q.recordid = A.qid and
                        A.uid = %d
                    where Q.parentid = %d order by weight, recordid";
            $DBValues = array($companyid, $parentid);
            $result = db_query($sql, $DBValues);
            $qpage = sget($_REQUEST, 'qpage');
            if (empty($qpage)) $qpage = 1;
            $pageCnt = 1;
            $preType = '';
            $counter = 1;
            while ($Row = db_fetch_object($result)) {
                $type = $Row->type;
                $questionid = $Row->recordid;
                if ($type == 'pagedivider') {
                    if ($preType != 'pagedivider') $pageCnt++;
                } elseif ($type == 'title') {
                    if ($pageCnt == $qpage) {
                        $subtitle = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                        $content .= "\n<a name='t_$questionid'></a><div class='subtitle dilligence-subtitle'>$subtitle</div>";
                    }
                } elseif ($type == 'question') {
                    if ($pageCnt == $qpage) {
                        $question = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                        if ($Row->answer) {
                            $answer = nl2br(htmlentities($Row->answer, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                        } else $answer = "N/A";
                        $content .= "\n<div class='entry'>";
                        $content .= "\n<div class='entry-title'>$counter. $question</div>";
                        $content .= "\n<div class='entry-content'><span class='blue'>Answer:</span> $answer</div>";
                        $content .= "\n</div>";
                        $counter++;
                    }
                } elseif ($type == 'instruction') {
                    if ($pageCnt == $qpage) {
                        $instruction = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                        $content .= "\n<div style='margin-top:8px;font:italic 12px arial'>$instruction</div>";
                    }
                } elseif ($type == 'fileuploader' && 0) {
                    if ($pageCnt == $qpage) {
                        $loaderTitle = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                        $filename = $Row->answer;
                        if (empty($filename)) {
                            $file = "N/A";
                        } else {
                            $path = "./" . file_directory_path() . "/" . $filename;
                            $file = "<a href='$path' target='_blank'>$filename</a>";
                        }
                        $content .= "\n<div class='entry'>";
                        $content .= "\n<div class='entry-title'>$counter. $loaderTitle</div>";
                        $content .= "\n<div class='entry-content' style='width:400px;overflow:hidden'><span class='blue'>File1:</span> $file</div>";
                        $content .= "\n</div>";
                        $counter++;
                    }
                }
                $preType = $type;
            }
            // Page navigation by divider
            $prePage = $nextPage = $submit = '';
            if ($qpage > 1 && $pageCnt > 1) {
                $pre = $qpage - 1;
                $prePage = "<a href='/account?a=information&tab=questionnaire&panel=preview&id=$parentid&page=$pre&companyid=$companyid' class=button>previous page</a> &nbsp;&nbsp;&nbsp;";
            }
            if ($pageCnt > 1 && $qpage < $pageCnt) {
                $next = $qpage + 1;
                $nextPage = "<a href='/account?a=information&tab=questionnaire&panel=preview&id=$parentid&page=$next&companyid=$companyid' class=button>next page</a>&nbsp;&nbsp;&nbsp;";
            }
            // Show view as userType for Pro
            $content = <<< END
            <form method='post' action='/account' enctype='multipart/form-data'>
                <div style='position:relative;width:100%;'>
                $content
                <input type=hidden name=a value=information />
                <input type=hidden name=tab value=questionnaire />
                <input type=hidden name=panel value='preview' />
                <input type=hidden name=id value='$parentid' />
                <input type='hidden' name='page' value='$next' />
                    <div style='margin-top:15px;'>$prePage $nextPage</div>
                </div>
            </form>
END;
        }
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function about($op = null)
{
    global $user;
    global $AccessObj;
    $editorid = $user->uid; // the currently logged in user, print_r this object to see all data available
    $companyid = sget($_REQUEST, 'id');
    $content = '';
    global $redirect;
    $data_type = 'discussions';
    $panel = 'multi';
    $section = __FUNCTION__;
    if (sget($_REQUEST, 'name')) {
        $Block['title'] = sget($_REQUEST, 'name');
        $fname = sget($_REQUEST, 'name');
    }
    ob_start();
    $url = "http://" . $_SERVER['HTTP_HOST'];
    $editorname = $user->name;
    require_once 'about.php';
    $content = ob_get_contents();
    ob_end_clean();
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function questionnaire($op = null)
{
    global $AccessObj;
    $time = time();
    $Block['title'] = 'DILIGENCE QUESTIONS';
    $content = '';
    $redirect = rebuild_url(array('tab', 'page', 'id', 'mtab'));
    //Get membership of the company whose questionaire is viewed
    $q = mysql_query("SELECT membership FROM maenna_company WHERE companyid = '" . (int) $_REQUEST['id'] . "'");
    $cmp_type_tmp = mysql_fetch_array($q);
    $cmp_type = $cmp_type_tmp['membership'];
    if ($AccessObj->Com_sections['questionnaire']['sections']['questionnaire']['access'] == 'write')
        $Block['title'] .=
            "<div style='left:495px;top:10px;height:15px;line-height:15px;width:120px;' class=editbtn>
                    <a style='float:left;padding-right: 2px;padding-left:18px;margin-right:3px;'
                    href='${redirect}&type=edit'
                    data-tooltip='Answer diligence questions. Save each answer as you go. You can edit until you`re done.'
                    class='tool'>Add Answers</a>
        </div>";
    if ($op && $op != 'update') {
        $highlight = isset($_REQUEST['update_section']) && $_REQUEST['update_section'] == 'questionnaire';
        $sql = "select  *
                from    maenna_questionair
                where   status = 1 and
                        target='company' and usertype='%s'
                        and parentid is null
                order by recordid asc";
        $result = db_query($sql, array($cmp_type));
        $Row = db_fetch_object($result);
        if ($Row) {
            $parentid = $Row->recordid;
            if (answered_all($parentid, $AccessObj->uid)) {
                if ($submit) { // user finished submit
                    $content .= "<div style='font:14px arial'>Thank you for complete the questionnaire.</div>";
                }
            }
            if (true) {
                $sql = "select  Q.*,
                                A.answer,
                                A.qaid as answerid
                        from    maenna_questionair as Q
                        left join maenna_qa as A on
                                Q.recordid = A.qid and
                                A.uid = %d
                        where   Q.parentid = %d
                        order by weight, recordid";
                $DBValues = array($_REQUEST['id'], $parentid);
                $result = db_query($sql, $DBValues);
                $_page = sget($_REQUEST, '_page');
                if (empty($_page)) $_page = 1;
                $morePage = false;
                $pageCnt = 1;
                $curPage = $_page;
                $preType = '';
                $counter = 1;
                $topCount = 1;
                while ($Row = db_fetch_object($result)) {
                    $type = $Row->type;
                    $questionid = $Row->recordid;
                    if ($type == 'pagedivider') {
                        if ($preType != 'pagedivider') $pageCnt++;
                    } elseif ($type == 'title') {
                        if ($pageCnt == $_page) {
                            $questionid = $Row->recordid;
                            $subtitle = htmlentities($Row->content);
                            $top = "<a href='#top' class='tool'>Top</a>";
                            if ($topCount == 1) $top = '';
                            $topCount++;
                            $content .= "\n<a name='t_$questionid'></a><div class='subtitle dilligence-subtitle'>$subtitle
                                        <div class='editbtn' style='top:6px;border:none;text-align:right'>$top</div>
                                    </div>";
                        }
                    } elseif ($type == 'question') {
                        if ($pageCnt == $_page) {
                            $answerid = $Row->answerid;
                            $question = htmlentities($Row->content, ENT_QUOTES);
                            $entry_class = "";
                            if ($Row->answer) {
                                $answer = $Row->answer;
                            } else {
                                $answer = 'N/A';
                                if ($highlight) {
                                    $entry_class = 'question-highlight';
                                }
                            }
                            $content .= "\n<div class='entry $entry_class'><a name='q_$questionid' style='display:hidden'></a>";
                            $content .= "\n<div class='entry-title dilligence'>$counter. $question</div>";
                            if ($AccessObj->Com_sections['questionnaire']['sections']['questionnaire']['access'] == 'write' && $_REQUEST['type'] == 'edit') {
                                $content .= "\n<div class='entry-content'><textarea name='q_$questionid' class='entry-textarea'>$answer</textarea></div>";
                                $content .= "<div style='height:12px;position:relative;text-align:right'>
                                            <div class='tool' style='position:absolute;left:0;top:0;text-align:left;width:400px;font-style:italic;' >&nbsp;</div>
                                            <a href='#' class='tool review-btn'>Review</a> &nbsp;&nbsp;&nbsp;
                                            <a href='#' class='tool save-btn' answerid='$answerid'  qid='$questionid'>Save</a>
                                        </div>";
                            } else {
                                $content .= "\n<div class='entry-content'><span class='blue'>Answer:</span> $answer</div>";
                            }
                            $content .= "\n</div>";
                        }
                        $counter++;
                    } elseif ($type == 'instruction') {
                        if ($pageCnt == $_page) {
                            $instruction = htmlentities($Row->content, ENT_QUOTES);
                            $content .= "\n<div style='margin-top:8px;font-size: 14.5px; font-family: LatoRegular;'>$instruction</div>";
                        }
                    } elseif ($type == 'fileuploader') {
                        if ($pageCnt == $_page && 0) {
                            $questionid = $Row->recordid;
                            $loaderTitle = htmlentities($Row->content, ENT_QUOTES);
                            $filename = $Row->answer;
                            if (empty($filename)) {
                                $file = "";
                            } else {
                                $path = "./" . file_directory_path() . "/" . $filename;
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
                // Page navigation by divider
                $prePage = $nextPage = $submit = '';
                if ($_page > 1 && $pageCnt > 1) {
                    $pre = $_page - 1;
                    $prePage = "<a href='$redirect&panel=questionnaire&id=$parentid&_page=$pre' class=button>previous page</a> &nbsp;&nbsp;&nbsp;";
                }
                if ($pageCnt > 1 && $_page < $pageCnt) {
                    $next = $_page + 1;
                    $nextPage = "<a href='$redirect&panel=questionnaire&id=$parentid&_page=$next' class=button>next page</a>&nbsp;&nbsp;&nbsp;";
                }
                $submit = '';
                if ($AccessObj->Com_sections['questionnaire']['sections']['questionnaire']['access'] == 'write' && $AccessObj->user_type == 'company') {
                    if ($pageCnt == $_page) {
                        $submit = "<input type=submit name=submit value='Submit' class=button />";
                    } else {
                        $submit = "<input type=submit name=submit value='save and continue' class=button />";
                    }
                }
                // Show view as userType for Pro
                $HV = hidden_post_values(array('tab', 'page'));
            }
            $content = <<< END

                <form method='post' id="company-questions" action='/account?tab=companies&page=company_detail&id={$_REQUEST['id']}&mtab=questionnaire'>
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
                                        modal:true,
                                        resizable: false,
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
                                    cid = $('#cnf_uid').attr('cid');
                                    if (questionid != '') {
                                        POSTDATA = "a=maennaco_ajax&action=ajaxTrigger&table=update_qa&answerid="+ answerid + "&qid=" + questionid + "&cid=" + cid + "&text=" + encodeURIComponent(content);
                                        ajax_update(POSTDATA, hide_ajax_obj);
                                    }
                                })
                            });
                    });
                </script>

END;
        }
    } elseif ($op == 'update') {
        $submit = sget($_REQUEST, 'submit');
        $parentid = sget($_REQUEST, 'parentid');
        $success = false;
        $message = "Nothing to update";
        $post_data = serialize($_POST);
        backup_copy($post_data, 'questionnaire_com');
        $allAnsweredBeforeThisUpdate = answered_all($parentid, $AccessObj->uid);
        foreach ($_POST as $key => $val) {
            if ($val == 'N/A') continue;
            if (substr($key, 0, 2) == 'q_') {
                $qid = str_replace('q_', '', $key);
                if ((intval($qid) > 0)) {
                    $sql_new = "insert into     maenna_qa (qid, qtype, uid, answer, created)
                                values (%d, 'question', %d, '%s', '%s' )
                                on duplicate key update answer = '%s', created = '%s'";
                    $DBValues = array($qid, $AccessObj->uid, $val, $time, $val, $time);
                    if (!db_query($sql_new, $DBValues)) {
                        $message = "";
                        drupal_set_message("Failed to update questionnaire record", 'error');
                        return;
                    } else {
                        $success = true;
                    }
                }
            }
        }
        if ($success) {
            drupal_set_message("Your information is updated.");
        } else {
            drupal_set_message("Nothing to update", 'error');
        }

        if ($submit && !$allAnsweredBeforeThisUpdate && answered_all($parentid, $AccessObj->uid)) {
            $notificationService = new NotificationService();
            $notificationService->registerEvent('company_due_diligence_completed', (int) $_REQUEST['id'], $AccessObj->uid);
        }

        return;
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function answered_all($questionnaireid, $uid)
{
    $sql = "select Q.*, A.answer from maenna_questionair as Q
                    left join maenna_qa as A
                        on Q.recordid = A.qid and A.uid = %d
                    where parentid = %d and (type = 'question' or type = 'fileuploader')";
    $result = db_query($sql, array($uid, $questionnaireid));
    $completed = true;
    while ($Row = db_fetch_object($result)) {
        if (empty($Row->answer)) $completed = false;
    }
    return $completed;
}

function edit_company_info($op = null)
{
    global $user;
    global $AccessObj;
    global $redirect;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Edit Company Information";
    if ($op && ($op != 'update')) {
        $sql = "select * from maenna_company where companyid = %d";
        $result = db_query($sql, array($companyid));
        $Row = db_fetch_object($result);
        if ($Row) {
            $company_id = $companyid;
            $company = $Row->company;
            $firstname = $Row->firstname;
            $lastname = $Row->lastname;
            $role = $Row->role;
            $email = $Row->email;
            $company_type = $Row->company_type;
            $revenue_this = $Row->revenue_this;
            $profits_this = $Row->profits_this;
            $earnings_this = $Row->earnings_this;
            $revenue_last = $Row->revenue_last;
            $profits_last = $Row->profits_last;
            $earnings_last = $Row->earnings_last;
            $reference_code = $Row->reference_code;
            $hearaboutus = $Row->hearaboutus;
            $projName = $Row->projname;
            if (empty($projName)) $projName = getProjectName($companyid);
            $address1 = $Row->address1;
            $address2 = $Row->address2;
            $city = $Row->city;
            $state = $Row->state;
            $option_state = Options_state($state);
            $zip = $Row->zip;
            $foundedYear = $Row->founded;
            $empnum = $Row->empnum;
            $description = $Row->description;
            $web = $Row->web;
            $phone = $Row->phone;
            $industry = $Row->sector;
            $option_industry = Options_industry($industry);
            $HV = hidden_post_values(array('tab', 'page', 'id', 'panel', 'section'));
            $content .= "<script type='text/javascript'>
            $(document).ready(function(){
               init_changeProjName();

               $('#follow_cmp_exp').click(function() {
               clickObj = $(this);
               clickObj.prop('disabled', true);
               if ($('#edit-expertise-match').val() === '') {

               alert ('Please select one Expertise from the list.');
               clickObj.prop('disabled', false);
               return false;
               }

               else  {
               if (confirm('Continue to follow all the professionals with the selected Expertise?')) {


                var uid = " . $_REQUEST['id'] . ";
                var m ='" . md5($_REQUEST['id'] . "kyarata75") . "';
                var exp = $('#edit-expertise-match').val();

                $.post('themes/maennaco/includes/follower.php?type=batchFollow&stype=follow_cmp_exp', {uid: uid,m:m,exp:exp}, function(response){
                    alert(response);
                    clickObj.prop('disabled', false);
                });


               }

               else { clickObj.prop('disabled', false); return false; }
               }

               });

                $('#maenna-forms-company-signup-form-page2').submit(function(e) {
                                 var self = this;
                                 e.preventDefault();
                                    if(validate()){
                                 self.submit();
                             }
                             else (alert('You cannot leave empty fields. Please submit empty fields.'));
                           });

                            function validate() {

                            var status = true;

                           $('#maenna-forms-company-signup-form-page2 :input').each(function() {

                            $(this).attr('style','background-color:none;');



                          if ($(this).attr('id') == 'edit-firstname' || $(this).attr('id') == 'edit-cmp-name' || $(this).attr('id') == 'edit-email' || $(this).attr('id') == 'edit-sector' ) {

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
            </script>";
            $content .= "<font face='Lato Regular'>" . drupal_get_form('maenna_forms_company_signup_form_page2') . "</font>";
        }
    }
    elseif ($op == 'update') {
        $DBKeys = array(
            'firstname',
            'lastname',
            'company',
            'role',
            'email',
            'address1',
            'address2',
            'city',
            'state',
            'zip',
            'phone',
            'web',
            'founded',
            'empnum',
            'description',
            'sector',
            'projname',
            'logo',
            'paypal_instructions',
            'ach_wire_bank_name',
            'ach_wire_bank_addr',
            'ach_wire_account_name',
            'ach_wire_ach_aba_no',
            'ach_wire_wire_aba_no',
            'ach_wire_account_no',
        );
        $DBValues = $SQL_STR = array();
        foreach ($DBKeys as $key) {
            $DBValues["$key"] = sget($_POST, $key);
            $SQL_STR[] = "$key = '%s'";
        }
        if (isset($_FILES['logo']['name']) && ($_FILES['logo']['error'] == 0)) {
            $filename = $_FILES['logo']['name'];
            $filename = $time . '_' . clearString($filename);
            $path = "./" . file_directory_path() . "/" . $filename;
            if (move_uploaded_file($_FILES['logo']['tmp_name'], $path)) {
                $DBValues['logo'] = $filename;
            } else {
                drupal_set_message('Failed to upload logo', 'error');
                return;
            }
        }
        $DBValues[] = $companyid;
        $sql = "update maenna_company set " . implode(',', $SQL_STR) . " where companyid = %d limit 1";
        if (db_query($sql, $DBValues)) {
            drupal_set_message("Company information is updated");
        } else {
            drupal_set_message("Failed to update company information", 'error');
        }
        return '';
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function prof_explore_investment($op = null)
{
    global $user;
    global $AccessObj;
    global $redirect;
    $section = 'company_name';
    $panel = __FUNCTION__;
    $userid = $user->uid;
    $companyid = sget($_REQUEST, 'id');

    //Add check for investor type (admin approved || self-reported)


    if ($_REQUEST['mode'] == 'add') {
        $Block['title'] = "Explore Investment";
        $content = '<input type="hidden" id="userId" value="' . $AccessObj->uid . '">';
        $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
        $sql = "select * from maenna_professional_investment where prof_id=%d and company_id=%d and deleted!=1 and status = 0 order by date desc limit 1"; //TODO::check expired
        $result = db_query($sql, array($AccessObj->uid, $companyid));
        $Row = db_fetch_object($result);
        $do = 'new';
        $data_id = '';
        $sql = "SELECT investor_type,previous_appruved_investor_type, investor_type_appruved_flag FROM maenna_people WHERE pid = %d LIMIT 1";
        $result = db_query($sql, array($AccessObj->uid));
        $user_data = db_fetch_object($result);
        $sql_company_data = "select * from maenna_company_data WHERE deleted != 1 and companyid = %d order by dataid desc limit 1";
        $company_result = db_query($sql_company_data, array($companyid));
        $company_data_row = db_fetch_array($company_result);
        $investorType = INVESTOR_TYPE();
        $minimum_amount_style = '$' . number_format($company_data_row['min_per_investor']);
        $maxium_amount_style = '$' . number_format($company_data_row['max_per_investor']);
        if (!empty($user_data->investor_type_appruved_flag)) {
            $investorType[$user_data->investor_type] = V_INVESTOR_TYPE()[$user_data->investor_type];
        } elseif (!empty($user_data->previous_appruved_investor_type)) {
            $investorType[$user_data->previous_appruved_investor_type] = V_INVESTOR_TYPE()[$user_data->previous_appruved_investor_type];
        }

        if ($Row) {
            $amount = $Row->ioi;
            $amount_style = '$' . number_format($Row->ioi);
            $inv_type = $Row->type;
            $do = 'edit';
            $data_id = $Row->id;
        } else {
            if (!empty($user_data->investor_type) && !empty($user_data->investor_type_appruved_flag)) {
                $inv_type = $user_data->investor_type;
            } elseif (!empty($user_data->previous_appruved_investor_type)) {
                $inv_type = $user_data->previous_appruved_investor_type;
            }
        }
        $content .= "<div id='box1' style='margin-bottom:15px;'>
                        <input type='hidden' id='maximum_amount' value='" . $company_data_row['max_per_investor'] . "'>
                        <input type='hidden' id='minimum_amount' value='" . $company_data_row['min_per_investor'] . "'>
                        <form id='addInvest' method=post action='/account' enctype='multipart/form-data' onsumbit=\"return validateFileForm();\">
                            <select id='investor_type' class='investor-type-select' name='investor_type'><option value=''>Investor Type:</option>";
        foreach ($investorType as $key => $value) {
            if ($key == $inv_type) $selected = "selected";
            else $selected = "";
            $content .= "<option " . $selected . " value='" . $key . "'>" . $value . "</option>";
        }
        $content .= "</select>";
        $content .= '<span class="questmark-investor-type"><a href="#" id="get-help-investor-type-btn">
					<img src="/themes/maennaco/images/questionmark.png" style="padding-top:2px;"></a></span>';
        $content .= "<p style='line-height:normal !important;margin-top:30px;margin-bottom:30px;'>Enter amount you expect to invest after reviewing project information</p>";
        $content .= "<input style='width:197px!important;height:22px;' id='amount' type=text name='amount' placeholder='Amount' value='" . $amount_style . "' />";
        $content .= "<br/><label style='margin-bottom: 10px;color: #929497;font-family:Lato Bold Italic;font-size: 12px;line-height: 1.5em;'>Enter range: ".$minimum_amount_style."~".$maxium_amount_style."</label>";
        $content .= "<div id='agree' style='margin-top:30px;'><input style='margin-left:10px; margin-top:7px;' type='checkbox' id='cmp-agree'><label style='margin-bottom: 10px;' for='cmp-agree'>I agree with <a class='show_confidentiality' type='terms' target='_blank' style='font-family:Lato Regular;font-style: italic;color:#00a1be;'>confidentiality</a> to access fundraising information </label></div>";
        $content .= "<input type=submit name=submit value='Submit' class='button' /> &nbsp;
                <a href='' class='hidebox button' boxid=box1>Close</a>
        </div>
        $HPV
        <input type='hidden' name=section value='prof_invest' />
        <input type='hidden' name=update_section value='prof_invest' />
        <input type='hidden' id='do' name=do value=$do />
        <input type='hidden' id='dataid' name=dataid value=$data_id />
        </form>";

        include __DIR__ . '/html_blocks/investor_type_text.php';

        $content .= "<script type='text/javascript' src='/themes/maennaco/jui/comments/js/jquery.livequery.js'></script>
            <script type='text/javascript'>
                function add_invest() {
                    $('#investor_type').val('');
                    $('#amount').val('');
                    $('#do').val('new');
                    $('#dataid').val('-1');

                    $('#agree').attr('style', 'display: inline');
                }
                function edit_invest(id, type, amount) {
                    $('#investor_type').val(type);
                    $('#amount').val(amount);
                    $('#do').val('edit');
                    $('#dataid').val(id);

                    $('#box1').attr('style', 'display: inline');
                    $('#agree').attr('style', 'display: none');
                }
                $(document).ready(function(){
                    $('#policy').html('Under the Securities & Exchange Commissions\'s Regulation D, a non-accredited individual investor is one who has a net worth of less than $1 million (including spouse) and who earned less than $200,000 annually ($300,000 with spouse) in the last two years.<br><br>For crowd funding, If you make less than $100,000 per year or your net worth is below that amount, you can invest up to either the greater of a) $2,000 or <br>b) the lesser of 5% of your income or net worth.<br><br>If your annual income and your net worth exceed $100,000, you can invest up to 10% of your income or net worth, whichever is less, up to a total limit of $100,000.');
                    // setTimeout(function() {
                    //     $('#policy').dialog('open');
                    // }, 1000);

                    $('.account-section-tabs').attr('style', 'display: none');
                    $('#squeeze').attr('style', 'margin-top: -40px');
                    $('.left-td-wrapper').attr('style', 'margin-top: -20px');

                    init_openbox();
                    init_hidebox();
                    init_sortPanel();

                    $('#amount')
                    .focusout(function() {
                        $(this).trigger('keyup');
                        var amount = $('#amount').val();
                        var split_amount = amount.split('$');
                        amount = split_amount[1].split(',');
                        amount = parseInt(amount.join(''));
                        var max_amount = parseInt($('#maximum_amount').val());
                        var min_amount = parseInt($('#minimum_amount').val());
                        if (amount > max_amount || amount < min_amount){
                            alert('You must enter an amount within this range $' + min_amount + '~$' + max_amount);
                            $('#amount').val('$');
                        }
                    })
                    .keyup(function() {
                        if($('#amount').val() == '' && $('#amount').val() == ''){
                            $('#amount').val('$');
                        }
                        var amount = $('#amount').val();
                        var split_amount = amount.split('$');
                        amount = split_amount[1].split(',');
                        amount = parseInt(amount.join(''));
                        var max_amount = parseInt($('#maximum_amount').val());
                        var min_amount = parseInt($('#minimum_amount').val());
                        if (amount > max_amount || amount < min_amount)
                            $(this).attr('style','color:red;');
                        else $(this).attr('style','color:#929497;');
                    })
                    .mouseover(function(){
                        $(this).trigger('keyup');
                        if($('#amount').val() == '' && $('#amount').val() == ''){
                            $('#amount').val('$');
                        }
                    })
                    $('#addInvest').submit(function(event) {

                        if ($('#investor_type').val() == '') {

                           $('#investor_type').addClass('inerror');
                           event.preventDefault();

                        }

                        else $('#investor_type').removeClass('inerror');


                        if ($('#amount').val() == '') {

                           $('#amount').addClass('inerror');
                           event.preventDefault();
                           return;

                        }

                        else $('#amount').removeClass('inerror');

                        if ($('#do').val() == 'new' || $('#do').val() == 'edit') {
                            if ($('#cmp-agree').is(':checked') == false) {
                                alert('You have to read confidentiality!');
                                event.preventDefault();
                                return;
                            }
                            $('a.blue_button div.main-text').html('LOI Submitted');
                        }

                    });
                });
            </script>";
    }
    //    else if($_REQUEST['mode'] == 'edit') {
    //        $uid = isset($_REQUEST[id])?$_REQUEST[invest_id]:'-1';
    //        $type = isset($_REQUEST[type])?$_REQUEST[type]:'';
    //        $amount = isset($_REQUEST[amount])?$_REQUEST[amount]:'';
    //        if (isset($_REQUEST[type]))
    //            $op = 'edit';
    //        else
    //            $op = 'new';
    //
    //        $Block['title'] = "Input - EDIT";
    //        $content = '<input type="hidden" id="userId" value="' . $AccessObj->uid . '">';
    //        $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
    //        $content .= "<div id='box1' style='margin-bottom:15px;'>
    //                        <form id='addInvest' method=post action='/account' enctype='multipart/form-data' onsumbit=\"return validateFileForm();\">
    //                            <select id='investor_type' style='width:207px;height:28px;margin-top:10px; margin-bottom:10px;' name='investor_type'><option value=''>Investor Type:</option>";
    //        foreach (INVESTOR_TYPE() as $key => $value)
    //            if ($key != $type)
    //                $content .= "<option value='" . $key . "'>" . $value . "</option>";
    //            else
    //                $content .= "<option value='" . $key . "' selected>" . $value . "</option>";
    //        $content .= "</select>";
    //        $content .= "<br><input style='width:197px!important;height:22px;margin-bottom:10px;' id='amount' type=text name='amount' placeholder='Amount' value='$amount' />";
    //        $content .= "<div id='agree'><br><input style='margin-left:10px; margin-top:7px;' type='checkbox' id='cmp-agree'><label style='margin-bottom: 10px;' for='cmp-agree'>I agree with <a class='show_confidentiality' type='terms' target='_blank' style='font-family:Lato Regular;font-style: italic;color:#00a1be;'>confidentiality</a> to access fundraising information </label></div>";
    //        $content .= "<br><input type=submit name=submit value='Submit' class='button' /> &nbsp;
    //                <a href='' class='hidebox button' boxid=box1>Close</a>
    //        </div>
    //        $HPV
    //        <input type='hidden' name=section value='prof_invest' />
    //        <input type='hidden' name=update_section value='prof_invest' />
    //        <input type='hidden' id='do' name=do value=$op />
    //        <input type='hidden' id='dataid' name=dataid value=$uid />
    //        </form>";
    //
    //        $content .= "<script type='text/javascript' src='/themes/maennaco/jui/comments/js/jquery.livequery.js'></script>
    //    <script type='text/javascript'>
    //        function add_invest() {
    //            $('#investor_type').val('');
    //            $('#amount').val('');
    //            $('#do').val('new');
    //            $('#dataid').val('-1');
    //
    //            $('#agree').attr('style', 'display: inline');
    //        }
    //        function edit_invest(id, type, amount) {
    //            $('#investor_type').val(type);
    //            $('#amount').val(amount);
    //            $('#do').val('edit');
    //            $('#dataid').val(id);
    //
    //            $('#box1').attr('style', 'display: inline');
    //            $('#agree').attr('style', 'display: none');
    //        }
    //        $(document).ready(function(){
    //            $('.account-section-tabs').attr('style', 'display: none');
    //            $('#squeeze').attr('style', 'margin-top: -40px');
    //            $('.left-td-wrapper').attr('style', 'margin-top: -20px');
    //
    //            init_openbox();
    //            init_hidebox();
    //            init_sortPanel();
    //
    //            $('#addInvest').submit(function(event) {
    //
    //                if ($('#investor_type').val() == '') {
    //
    //                   $('#investor_type').addClass('inerror');
    //                   event.preventDefault();
    //
    //                }
    //
    //                else $('#investor_type').removeClass('inerror');
    //
    //
    //                if ($('#amount').val() == '') {
    //
    //                   $('#amount').addClass('inerror');
    //                   event.preventDefault();
    //                   return;
    //
    //                }
    //
    //                else $('#amount').removeClass('inerror');
    //
    ////                if ($('#do').val() == 'new') {
    //                    if ($('#cmp-agree').is(':checked') == false) {
    //                        alert('You have to read confidentiality!');
    //                        event.preventDefault();
    //                    }
    //                    $('a.blue_button div.main-text').html('LOI Submitted');
    ////                }
    //
    //            });
    //        });
    //    </script>";
    //    }
    //    else {
    //        if (isset($_REQUEST['sortby'])) $sortby = $_REQUEST['sortby'];
    //        $Block['title'] = "INDICATION OF INTEREST ($ in 000)";
    //        $File_Perm = $AccessObj->Com_sections['file']['sections'];
    //        $_Tags = PROF_INVEST_SORT();
    //        $tag_no = count($_Tags);
    //        $company_id = sget($_REQUEST, 'id');
    //        $i = 0;
    //
    //        $sortLinks = '';
    //        foreach ($_Tags as $key => $value) {
    //            if ($sortby == $key) $font_style = 'color:#00a2bf;'; else $font_style = 'font-style:italic;';
    //            $sortLinks .= "<a href='${redirect}&section=$section&panel=$panel&sortby=$key' style='$font_style font-size:12px;cursor:pointer;";
    //
    //            if ($i == 0)
    //                $sortLinks .= "margin-right:10px;'>{$value}</a>";
    //            else
    //                $sortLinks .= "margin-left:10px;margin-right:10px;'>{$value}</a>";
    //
    //            if (++$i != $tag_no)
    //                $sortLinks .= '|';
    //        }
    //        if(!empty($sortLinks))
    //            $sortLinks = rtrim($sortLinks, '|');
    //
    //        $status = array('<span class="color-swatch" data-tooltip="Pending" style="background-color:#FFE940; border:1px solid #FFE940; padding-right: 10px;"></span>',
    //            '<span class="color-swatch" data-tooltip="Accepted" style="background-color:#00A2BF; border:1px solid #00A2BF; padding-right: 10px;"></span>',
    //            '<span class="color-swatch" data-tooltip="Declined" style="background-color:#DF4000; border:1px solid #DF4000; padding-right: 10px;"></span>',
    //            '<span class="color-swatch" data-tooltip="Funded" style="background-color:#6ddf45; border:1px solid #6ddf45; padding-right: 10px;"></span>');
    ////        $userService = new UserService();
    ////        $tooltip = "File tab allows experts to share files with client and Clewed. Files tagged Deliverable Requirements define expected deliverables. Files tagged Expert Screening are private used only for vetting purposes. All files you upload in the File tab should be sanitized to exclude your brand name or logo. All files associated with a service you are involved with should be uploaded by editing that service, not in this tab.";
    ////        if(
    ////            $userService->isColleague($userid, $companyid) ||
    ////            $userService->isCompany($userid) && $userid == $companyid ||
    ////            $userService->isAdmin($userid, $companyid) ||
    ////            $userService->isSuperAdmin($userid)
    ////        ) {
    ////            $tooltip = "Files tagged Deliverable Requirements are visible to experts connected to this project. All other files are visible selectively on a need to know and NDA basis. All files you upload should be sanitized to exclude any brand name or logo. Files tagged Expert Screening are private for the purposes of vetting expert's only.";
    ////        }
    //
    ////    $Block['title'] .= '<a class= "openbox" boxid="box1" data-tooltip="' . $tooltip . '" style="color:#fff;cursor:pointer;display:inline-block;margin-left:120px; margin-top:5px;height:20px;line-height:22px;border-left:thin solid #fff;padding-left:10px;font-family:LatoRegular; font-size:13px; text-transform: none;" onclick="add_invest()">Add</a>';
    //
    ////        $Block['title'] .= '<a class= "openbox" boxid="box2" data-tooltip="' . $tooltip . '" style="color:#fff;cursor:pointer;display:inline-block;margin-left:10px; margin-top:5px;height:20px;line-height:22px;border-left:thin solid #fff;padding-left:10px;font-family:LatoRegular; font-size:13px; text-transform: none;">Edit</a>';
    //
    ////        $Block['title'] .= '<span class="color-swatch" data-tooltip="Accepted" style="background-color:#00A2BF; border:1px solid #00A2BF; margin-left: 20px; padding-left: 4px; padding-right: 13px;"></span>';
    ////        $Block['title'] .= '<span class="color-swatch" data-tooltip="Pending" style="background-color:#FFE940; border:1px solid #FFE940; margin-left: 10px; padding-left: 4px; padding-right: 13px;"></span>';
    ////        $Block['title'] .= '<span class="color-swatch" data-tooltip="Declined" style="background-color:#DF4000; border:1px solid #DF4000; margin-left: 10px; padding-left: 4px; padding-right: 13px;"></span>';
    //
    //        $Block['title'] .= "<span style='margin-top:5px;height:20px;line-height:22px;border-left:thin solid #fff;padding-left:10px; float:right;display:inline-block;font-weight:bold; font-family:LatoRegular; font-size:13px; text-transform:none;'>Sort By <img class='openSortPanel' panelid='file_sort' style='width:20px; margin-left:20px;' src='themes/maennaco/images/arrow_down.png'></span>";
    //
    //        $content = '<input type="hidden" id="userId" value="'. $AccessObj->uid . '"><input type="hidden" id="redirect" value="' . ${redirect} . '">';
    //        //Set this page styles
    //        $content .= "<style>
    //                        .shaded_title {
    //
    //                            background:#ffffff;
    //							font-size: 16px;
    //                            height:30px;
    //                            line-height:30px;
    //                        }
    //                        .main_content {
    //                            padding-top:0 !important;
    //                        }
    //            </style>";
    //        $content .= '<div id="file_sort" style="display:none;line-height:30px;height:30px;width:100%; background-color:#f2f2f2"><span style="margin-top:0; display:inline-block;vertical-align:middle;margin-left:15px;">';
    //        $content .= $sortLinks . '</span></div>';
    //        $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
    //        $content .= "<div id='box1' style='margin-bottom:15px;display:none;'>
    //                            <form id='addInvest' method=post action='/account' enctype='multipart/form-data' onsumbit=\"return validateFileForm();\">
    //                                <select id='investor_type' style='width:207px;height:28px;margin-top:10px; margin-bottom:10px;' name='investor_type'><option value=''>Investor Type:</option>";
    //        foreach (INVESTOR_TYPE() as $key => $value)
    //            $content .= "<option value='" . $key . "'>" . $value . "</option>";
    //        $content .= "</select>";
    //        $content .= "<br><input style='width:197px!important;height:22px;margin-bottom:10px;' id='amount' type=text name='amount' placeholder='Amount' />";
    //        $content .= "<div id='agree'><br><input style='margin-left:10px; margin-top:7px;' type='checkbox' id='cmp-agree'><label style='margin-bottom: 10px;' for='cmp-agree'>I agree with <a class='show_confidentiality' type='terms' target='_blank' style='font-family:Lato Regular;font-style: italic;color:#00a1be;'>confidentiality</a> to access fundraising information </label></div>";
    //        $content .= "<br><input type=submit name=submit value='Submit' class='button' /> &nbsp;
    //                    <a href='#' class='hidebox button' boxid=box1>Close</a>
    //            </div>
    //            $HPV
    //            <input type='hidden' name=section value='prof_invest' />
    //            <input type='hidden' name=update_section value='prof_invest' />
    //            <input type='hidden' id='do' name=do value='new' />
    //            <input type='hidden' id='dataid' name=dataid value='-1' />
    //            </form>";
    //
    //        if ($sortby == '')
    //            $sql = "select a.*, p.firstname name from maenna_professional_investment a join maenna_people p on a.prof_id=p.pid where a.prof_id=%d AND a.deleted!=1 order by a.created desc";
    //        else
    //            $sql = "select a.*, p.firstname name from maenna_professional_investment a join maenna_people p on a.prof_id=p.pid where a.prof_id=%d AND a.deleted!=1 order by " . $sortby . " desc";
    //        $result = db_query($sql, array($userid));
    //        while ($Row = db_fetch_object($result)) {
    //            $id = $Row->id;
    //            $tmpRow = '';
    //            $tmpRow .= "\n<tr style='height: 32px;'>
    //                <td style='padding-left:15px;padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:left;height:32px;'><span>$Row->proj_name</span></div></td>
    //                <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:left;height:32px;'>". strtoupper($Row->type) . "</div></td>
    //                <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:left;height:32px;'>$" . number_format($Row->amount) . "</div></td>
    //                <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:left;height:32px;'>" . date("d/m/Y", $Row->created) . "</div></td>
    //                <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:left;height:32px;'>" . $status[$Row->status] . "</div></td>";
    //            $amount_style = '$' . number_format($Row->amount);
    //            //If file file author is currently logged in user
    ////            if ($userid == $editorid || $AccessObj->user_type == 'super') {
    //            $tmpRow .= "<td style='padding-top:5px;font-size:11px;'>
    //                    <a href='account?tab=companies&page=company_detail&id=$company_id&section=company_name&panel=prof_explore_investment&mode=edit&invest_id=$Row->id&type=$Row->type&amount=$Row->amount' style='display: inline-block;border-left: 1px solid;line-height: 1.1em;padding-left: 5px;' class='tool'>Edit</a>
    //                    <a href='.$redirect&update_section=prof_invest&do=del&dataid=" . $id . "' onclick='return confirm(\"Continue to delete this file?\")' style='margin-left: 5px;display: inline-block;border-left: 1px solid;line-height: 1.1em;padding-left: 5px;' class='tool'>Del</a></td>";
    ////            } else {
    ////                if ($File_Perm[$cat . "_2"]['access'] == 'hide') {
    ////                    $tmpRow .= "<td style='padding-top:5px;font-size:11px;'></td>";
    ////                } elseif ($File_Perm[$cat . "_2"]['access'] == 'view') {
    ////                    $tmpRow .= "<td style='padding-top:5px;font-size:11px;'>
    ////                                    <a style='margin-left:5px;display:inline-block;margin-right: 5px;' href = '/account?tab=companies&page=company_detail&eid=$Row->editorid&dataid=$Row->dataid&datatype=$Row->data_type&id=$companyid&mtab=file&name=" . rawurlencode($filename) . "&view=true' title = 'View'>
    ////                                    <img src = '/themes/maennaco/images/view_file_icon.png'>
    ////                                    </a></td>";
    ////                } else {
    ////                    $tmpRow .= "<td style='padding-top:5px;font-size:11px;'>
    ////                                    <a style='margin-left:5px;display:inline-block;margin-right: 5px;' href = '/account?tab=companies&page=company_detail&eid=$Row->editorid&dataid=$Row->dataid&datatype=$Row->data_type&id=$companyid&mtab=file&name=" . rawurlencode($filename) . "&view=true' title = 'View'>
    ////                                    <img src = '/themes/maennaco/images/view_file_icon.png'>
    ////                                    </a>
    ////                                    <a style='display:inline-block;margin-right: 5px;' href='$path'><img src = '/themes/maennaco/images/download_file_icon.png'></a>
    ////                                    </td>";
    ////                }
    ////            }
    //            $tmpRow .= "</tr>";
    //            $fileList[] = $tmpRow;
    //        }
    //        $content .= '<table style="margin:0; cellspacing="0" cellpadding="0" border="0" width="100%" class="left-align">
    //                    <thead style="background:#F9FAFC;">
    //                        <tr>
    //                            <td style="font-weight:normal;width:25%; padding-left:15px;">PROJECT NAME</td>
    //                            <td style="font-weight:normal;width:20%">TYPE</td>
    //                            <td style="font-weight:normal;width:12%">AMOUNT</td>
    //                            <td style="font-weight:normal;width:12%">DATE</td>
    //                            <td style="font-weight:normal;width:10%">STATUS</td>
    //                            <td style="font-weight:normal;width:*">ACTION</td>
    //
    //            </tr></thead>';
    ////        if ($tag != 'other') {
    //        foreach ($fileList as $key => $value) {
    ////                if (count($fileList[$key]) > 0 && ($File_Perm[$key . "_1"]['access'] != 'hide' || $File_Perm[$key . "_2"]['access'] != 'hide')) {
    ////                    foreach ($fileList[$key] as $value1) {
    //            $content .= $value;
    ////                    }
    ////                }
    //        }
    ////        }
    ////        if (!isset($tag) || $tag == 'other') {
    //////            if (count($fileList['Other']) > 0 && ($File_Perm["other_1"]['access'] != 'hide' || $File_Perm["other_2"]['access'] != 'hide')) {
    ////                $content .= "<tr><td style='padding-left:15px; background:#f2f2f2;' colspan = '6'>Other</td></tr>";
    //////                foreach ($fileList['Other'] as $value) {
    ////                    $content .= $value;
    //////                }
    //////            }
    ////        }
    //        $content .= "</table>
    //
    //        <div id='uploadDlg' style='display:none;'></div>
    //
    //        <script type='text/javascript' src='/themes/maennaco/jui/comments/js/jquery.livequery.js'></script>
    //        <script type='text/javascript'>
    ////            function add_invest() {
    ////                $('#investor_type').val('');
    ////                $('#amount').val('');
    ////                $('#do').val('new');
    ////                $('#dataid').val('-1');
    ////
    ////                $('#agree').attr('style', 'display: inline');
    ////            }
    //            function edit_invest(id, type, amount) {
    ////                $('#investor_type').val(type);
    ////                $('#amount').val(amount);
    ////                $('#do').val('edit');
    ////                $('#dataid').val(id);
    //
    ////                $('#box1').attr('style', 'display: inline');
    ////                $('#agree').attr('style', 'display: none');
    ////                window.location.href = $('#redirect').val()+'&section=company_name&panel=prof_explore_investment&id='+id+'&type='+type+'&amount='+amount;
    //            }
    //            $(document).ready(function(){
    //                $('.account-section-tabs').attr('style', 'display: none');
    //                $('#squeeze').attr('style', 'margin-top: -40px');
    //                $('.left-td-wrapper').attr('style', 'margin-top: -20px');
    //
    //                init_openbox();
    //                init_hidebox();
    //                init_sortPanel();
    //
    //                $('#addInvest').submit(function(event) {
    //
    //                    if ($('#investor_type').val() == '') {
    //
    //                       $('#investor_type').addClass('inerror');
    //                       event.preventDefault();
    //
    //                    }
    //
    //                    else $('#investor_type').removeClass('inerror');
    //
    //
    //                    if ($('#amount').val() == '') {
    //
    //                       $('#amount').addClass('inerror');
    //                       event.preventDefault();
    //                       return;
    //
    //                    }
    //
    //                    else $('#amount').removeClass('inerror');
    //
    //                    if ($('#do').val() == 'new') {
    //                        if ($('#cmp-agree').is(':checked') == false) {
    //                            alert('You have to read confidentiality!');
    //                            event.preventDefault();
    //                        }
    //                        $('a.blue_button div.main-text').html('LOI Submitted');
    //                    }
    //
    //                });
    //
    //                setInterval(function () {
    //                    var path = window.location.href, company_id, userId;
    //
    //                    userId = $('#userId').val();
    //                    company_id = path.substring(path.indexOf('company_detail&id=') + 18, path.indexOf('&section'));
    //                    sel_obj = $('a.blue_button div.main-text');
    //                    POSTDATA = 'action=check_invest_status&userId='+userId+'&companyId='+company_id;
    //                    ajax_update(POSTDATA, admin_invest_loi_approved);
    //                }, 5000);
    //            });
    //        </script>";
    //
    ////    else {
    ////        $Block['title'] = "File View";
    ////        if (sget($_REQUEST, 'name')) {
    ////            $Block['title'] = sget($_REQUEST, 'name');
    ////            $fname = sget($_REQUEST, 'name');
    ////        }
    ////        $file_array = pathinfo(sget($_REQUEST, 'name'));
    ////        $f_extension = $file_array['extension'];
    ////        if (in_array($f_extension, array('doc', 'xls', 'pdf', 'ppt', 'tiff'))) {
    ////            $arr = explode("_", sget($_REQUEST, 'datatype'), 2);
    ////        }
    ////        $cat = $arr[0];
    ////        $catName = getFileCategoryName($cat);
    ////        $tmpRow = '<div style="margin-right:18px;float:right;">';
    ////        $sql = "select * from maenna_company_data WHERE deleted != 1 and dataid = %d";
    ////        $result = db_query($sql, array(sget($_REQUEST, 'dataid')));
    ////        if ($Row = db_fetch_object($result)) {
    ////            $url = "http://" . $_SERVER['HTTP_HOST'] . "/" . file_directory_path() . "/" . $Row->data_value2;
    ////            $res  = mysql_query("SELECT d_id FROM wall_documents where document_name = '" . basename($url) . "'");
    ////            $red  = mysql_fetch_array($res);
    ////            $f_id = $red['d_id'];
    ////        }
    ////        $editorname = $user->name;
    ////        $editorid = $Row->editorid;
    ////        //If file file author is currently logged in user
    ////        if ($userid == $editorid || $AccessObj->user_type == 'super') {
    ////            $tmpRow .= "
    ////
    ////                                    <a data-tooltip='Replace file' title='REPLACE' class= 'openbox fileReplace' boxid='box2' style='display:inline-block;margin-right: 5px;' ref='" . sget($_REQUEST, 'dataid') . "' fileName = '" . sget($_REQUEST, 'name') . "' href = '#'><img src = '/themes/maennaco/images/replace_file_icon.png'></a>
    ////                                    <a data-tooltip='Download file' title='DOWNLOAD' href = '/download.php?tab=file&file_name=" . rawurlencode($Row->data_value2) . "' style='display:inline-block;margin-right: 5px;'><img src = '/themes/maennaco/images/download_file_icon.png'></a>
    ////                                    <a data-tooltip='Delete file' title='DELETE' href='.$redirect&update_section=files&do=del&dataid=" . sget($_REQUEST, 'dataid') . "'
    ////                                                    onclick='return confirm(\"Continue to delete this file?\")' style='display: inline-block;border-left: 1px solid;line-height: 1.1em;padding-left: 5px;' class='tool'>Delete</a>
    ////                                    ";
    ////        } else {
    ////            if ($File_Perm[$cat . "_2"]['access'] == 'hide') {
    ////                $tmpRow .= "";
    ////            } elseif ($File_Perm[$cat . "_2"]['access'] == 'view') {
    ////                $tmpRow .= "";
    ////            } else {
    ////                $tmpRow .= "<a  style='display:inline-block;margin-right: 5px;' href='/download.php?tab=file&file_name=" . $Row->data_value2 . "'><img src = '/themes/maennaco/images/download_file_icon.png'></a>
    ////                                    ";
    ////            }
    ////        }
    ////        $tmpRow .= '</div>';
    ////        $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
    ////        $Block['title'] .= $tmpRow;
    ////        $content .= "<div id='box2' style='margin-bottom:15px;display:none;'>
    ////                        <form id='fileReplace' method=post action='/account' enctype='multipart/form-data'>
    ////                            <br><input style='width:155px;height:22px;margin-bottom:4px;' type=text name=name value='' />
    ////                            <br><input type=file name='file' /></br><br><input style='padding-top:3px;width:75px; height:28px;' type=submit name=submit value='submit'  class=buttonReplace /> &nbsp;
    ////                                    <a href='#' style='width: 53px;height: 20px;display: inline-block;text-align: center;' class='hidebox buttonReplace' boxid=box2>Close</a>
    ////                            </div>
    ////                            $HPV
    ////                            <input type='hidden' name=section value='files' />
    ////                            <input type='hidden' name=update_section value='files' />
    ////                            <input type='hidden' name=do value='update' />
    ////                            <input type='hidden' name='dataid' value='".$f_id."'/>
    ////                        </form>
    ////                    ";
    ////        ob_start();
    ////        require_once("file_comments.php");
    ////        $content .= ob_get_contents();
    ////        $content .= "
    ////                <script type='text/javascript'>
    ////$(document).ready(function(){
    ////    init_openbox();
    ////    init_hidebox();
    ////
    ////    $('.fileReplace').click(function() {
    ////
    ////    $(\"#fileReplace input[name='name']\").val($(this).attr('fileName'));
    ////    $(\"input[name='dataid']\").val($(this).attr('ref'));
    ////
    ////    });
    ////});
    ////</script>
    ////
    ////        <style>
    ////
    ////                        .buttonReplace {
    ////                            font-family: 'Lato Regular' !important;
    ////                            padding: 2px 9px;
    ////                            background: #41454F;
    ////                            color: white !important;
    ////                            border: 2px outset buttonface;
    ////                            text-transform: capitalize;
    ////                            display:inline-block;
    ////                            text-align:center;
    ////                            font-size:12px;
    ////                        }
    ////                        .buttonReplace:hover {
    ////                            background: #00a2bf;
    ////                            text-decoration: none;
    ////                            padding: 2px 9px;
    ////                        }
    ////                        .shaded_title {
    ////
    ////                            background:#ffffff;
    ////                            height:30px;
    ////                            line-height:30px;
    ////                            padding-left:15px;
    ////
    ////                        }
    ////
    ////                        .main_content {
    ////
    ////                            padding-top:0 !important;
    ////
    ////                        }
    ////
    ////                    </style>";
    ////        ob_end_clean();
    ////    }
    //    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function invest_now($op = null)
{
    global $user;
    global $AccessObj;
    global $redirect;
    $section = 'company_name';
    $panel = __FUNCTION__;
    $userid = $user->uid;
    $user_data_sql = "SELECT * FROM users_bank_info WHERE uid = '" . $userid . "' and status = 1";
    $user_data_res = db_query($user_data_sql, array($userid));
    $user_data = mysql_fetch_object($user_data_res);
    $customerid = $user_data->customerid;
    $companyid = sget($_REQUEST, 'id');
    if ($customerid == null) {
        //drupal_goto('account', 'tab=linkbank&companyid=' . $companyid . '');
    }
    $sql = "select * from maenna_company_data WHERE deleted != 1 and companyid = %d order by dataid desc limit 1";
    $result = db_query($sql, array($companyid));
    $row = db_fetch_array($result);

    $sql = "select * from maenna_company WHERE companyid = %d";
    $result = db_query($sql, array($companyid));
    $row1 = db_fetch_array($result);

    $get_fee_sql = "SELECT clewed_fee,card_fee,paypal_fee,ach_fee FROM maenna_company WHERE companyid = %d";
    $fee_result = db_query($get_fee_sql, array($companyid));
    $fee_row = db_fetch_array($fee_result);
    /* if ($_REQUEST['mode'] == 'add')*/ {
        $Block['title'] = "Invest";
        $content = '<script type="text/javascript" src="/js/paypal-checkout.js"></script>';
        $content .= '<link type="text/css" rel="stylesheet" media="all" href="/bank_card.css">';
        $content .='<input type="hidden" id="userId" value="' . $AccessObj->uid . '">
                    <input type="hidden" id="maximum_amount" value="' . $row['max_per_investor'] . '">
                    <input type="hidden" id="minimum_amount" value="' . $row['min_per_investor'] . '">
                    <input type="hidden" id="paypal_link" value="' . $row1['paypal_instructions'] . '">
                    <input type="hidden" id="event" maxlength="7" value="' . $companyid . '"/>
                    <div id="pp-form-container"></div>';
        $minimum_amount_style = '$' . number_format($row['min_per_investor']);
        $maxium_amount_style = '$' . number_format($row['max_per_investor']);
        $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
        $content .= "<div id='box1' style='margin-bottom:15px;'>
                        <form id='addInvest' method=post action='/account' enctype='multipart/form-data' onsumbit=\"return validateFileForm();\"><br>";
        $content .= "<span style='display:inline-block;width:125px'>Investment Amount</span>
                        <input style='width:300px!important;height:22px;margin-bottom:10px; margin-left: 10px;' id='amount' type=text name='amount' placeholder='Amount' />
                    <div style='margin-top: -10px;margin-bottom:8px;'>
                        <span style='margin-left: 135px; font-size: 12px; color: #8f9095'>Enter Range:".$minimum_amount_style." ~ ".$maxium_amount_style."</span>
                    </div>";
        $content .= "<span style='display:inline-block;width:125px'>Clewed Service Fee</span>";
        $content .= "<input style='width:300px!important;height:22px;margin-bottom:10px; margin-left: 10px;' type=text id=clewedfee name='clewedfee' readonly/><br/>";
        $content .= "<span style='display:inline-block;width:125px'>Processing Fee</span>";
        $content .= "<input style='width:300px!important;height:22px;margin-bottom:10px; margin-left: 10px;' type=text id=processingfee name='processingfee' readonly/>";
        $content .= "<br><span style='display:inline-block;width:125px'>Total</span>";
        $content .= "<input style='width:300px!important;height:22px;margin-bottom:20px; margin-left: 10px;' id='total' name=total type=text placeholder='Total' readonly />";
        $content .= "<br><span>Bank Info</span>";
        $bank_sql = "SELECT * FROM users_bank_info WHERE uid = '" . $userid . "'";
        $bank_data_res = mysql_query($bank_sql);
        $content .= '<div class="bank_div">';
        while($data = mysql_fetch_object($bank_data_res)){
            $status = $data->status;
            if($status == 1) $class = 'is_active';
            else $class = null;
            $content .= "<div class='card-wrap ".$class."' style='margin-bottom: 35px !important;}'>";
            $content .= "<div class='card'>
                            <div class='bank_name'> $data->bankname •••• $data->banknumber </div>
                        </div>";
            $content .= "</div>";
        }
        $content .= '</div>';
        $content .= "<br><span style='display:inline-block;width:125px'>Payment method</span>";
        if($user_data->bankname != null) $value='linked_bank';
        $content .= "<select id='payment_method' name='paymentmethod' style='width:310px;height:28px;margin-top:10px; margin-bottom:20px; margin-left: 10px;' name='paymentmethod' value='".$value."'>
                        <option value='linked_bank'>Linked Bank(ACH)</option>";
                        foreach (PAYMENT_METHOD() as $key => $value)
                        $content .= "<option value='" . $key . "'>" . $value . "</option>";
        $content .= "</select>";
        $content .= "<br><div id='paypal' style='display: none'><table style='margin-left: 20px;'>
                    <tr><td>Paypal Link</td><td>" . $row1['paypal_instructions'] . "</td></tr></table></div>";
        $content .= "<div id='ach_wire' style='display: none'><table style='margin-left: 20px;'>
                    <tr><td width='115px'>Bank Name</td><td>" . $row1['ach_wire_bank_name'] . "</td></tr>
                    <tr><td>Account Name</td><td>" . $row1['ach_wire_account_name'] . "</td></tr>
                    <tr><td>Bank Address</td><td>" . $row1['ach_wire_bank_addr'] . "</td></tr>
                    <tr><td>Wire ABA#</td><td>" . $row1['ach_wire_wire_aba_no'] . "</td></tr>
                    <tr><td>ACH ABA#</td><td>" . $row1['ach_wire_ach_aba_no'] . "</td></tr>
                    <tr><td>Account#</td><td>" . $row1['ach_wire_account_no'] . "</td></tr></table></div>";
                $content .= "<br><div id='paypal' style='display: none'><table style='margin-left: 20px;'>
            <tr><td>Paypal Link</td><td>" . $row1['paypal_instructions'] . "</td></tr></table></div>";
        $content .= "<div><span style='display:inline-block;width:125px'>Payment Reference</span><input style='width:300px!important;height:22px; margin-left: 10px;' type='text' placeholder='Add date payment is sent'></div>";
        $content .= "<br><div style='margin-left: 135px;'><input style='width:300px!important;height:22px;margin-bottom:10px;' type='text' placeholder='Add payment reference if any'></div>";
        $content .= "<br><input type=submit id='invest_now' name=submit value='Submit' class='button' /> &nbsp;
                <a href='' class='hidebox button' boxid=box1>Close</a>
        </div>
        $HPV
        <input type='hidden' name=section value='prof_invest_now' />
        <input type='hidden' name=section value='prof_invest_now' />
        <input type='hidden' id=customerid name=customerid  value='".$customerid."' />
        <input type='hidden' name=update_section value='prof_invest_now' />
        </form>";

        $content .= "<script type='text/javascript' src='/themes/maennaco/jui/comments/js/jquery.livequery.js'></script>
            <script type='text/javascript'>
                $(document).ready(function() {
                    $('.card-wrap').click(function(){
                        var bank_name = $(this).find('.bank_name').html();
                        $('.bank_div').find('.card-wrap').removeClass('is_active');
                        $(this).addClass('is_active');
                        userid = '$userid'
                        $.post( '/setbank.php', {bank_name,userid}, function( data ) {                        
                            if (data != null || data != ''){
                                $('#customerid').val(data);
                                alert('Bank was reselected!');
                            }else{
                                alert('Failed');
                            }
                        });
                    });
                    $('.account-section-tabs').attr('style', 'display: none');
                    $('#squeeze').attr('style', 'margin-top: -40px');
                    $('.left-td-wrapper').attr('style', 'margin-top: -20px');

                    init_openbox();
                    init_hidebox();
                    init_sortPanel();

                    $('#amount')
                    .focusout(function() {
                        $(this).trigger('keyup');
                        getAmount();
                    })
                    .keyup(function() {
                        var amount = $('#amount').val();
                        var max_amount = parseInt($('#maximum_amount').val());
                        var min_amount = parseInt($('#minimum_amount').val());
                        if (amount > max_amount || amount < min_amount)
                            $(this).attr('style','color:red;width:300px!important;height:22px;margin-bottom:10px; margin-left: 10px;');
                        else $(this).attr('style','color:#929497;width:300px!important;height:22px;margin-bottom:10px; margin-left: 10px;');
                        amount = amount ? localStringToNumber(amount) : '';
                    }).keydown(function(){
                        $(this).trigger('keyup');
                    }).keypress(function(){
                        $(this).trigger('keyup');
                    }).change(function(){
                        $(this).trigger('keyup');
                    }).click(function(){
                        $(this).trigger('keyup');
                    }).mouseenter(function(){
                        $(this).trigger('keyup');
                    }).mousedown(function(){
                        $(this).trigger('keyup');
                    }).mouseup(function(){
                        $(this).trigger('keyup');
                    });

                    $('#invest_now').submit(function(event) {
                        if ($('#amount').val() == '') {
                           $('#amount').addClass('inerror');
                           event.preventDefault();
                           return;
                        }
                    });
                });
                $('#payment_method').change(function() {
                    $('#paypal').attr('style', 'display: none;');
                    $('#ach_wire').attr('style', 'display: none;');
                    if ($(this).val() != '' && $('#amount').val() == '') {
                        $('#payment_method').val('');
                        alert('Please input an amount');
                        return;
                    }
                    if($(this).val() == 'linked_bank'){
                        var amount = $('#amount').val();
                        var options = {
                            maximumFractionDigits : 2,
                            currency              : 'USD',
                            style                 : 'currency',
                            currencyDisplay       : 'symbol'
                        }
                        var split_amount = amount.split('$');
                        amount = split_amount[1].split(',');
                        amount = parseInt(amount.join(''));
                        var clewedfee = amount * ('".$fee_row['clewed_fee']."'/100);
                        var tempClewedFee = clewedfee;
                        clewedfee = clewedfee 
                        ? localStringToNumber(clewedfee).toLocaleString(undefined, options)
                        : ''
                        var profee_percent = '".$fee_row['ach_fee']."'/100;
                        var processingfee = amount * ('".$fee_row['ach_fee']."'/100);
                        var tempProcessingFee = processingfee;
                        var tempProcessingFee = processingfee;
                        processingfee = processingfee 
                        ? localStringToNumber(processingfee).toLocaleString(undefined, options)
                        : ''
                        $('#processingfee').val(processingfee);
                        var total = parseInt(amount) + parseInt(tempClewedFee) + parseInt(tempProcessingFee);
                        total = total 
                        ? localStringToNumber(total).toLocaleString(undefined, options)
                        : ''
                        $('#total').val(total);
                    }
                    if ($(this).val() == 'paypal') {
                        $('#paypal').attr('style', 'display: inline;');
                        var amount = $('#amount').val();
                        var options = {
                            maximumFractionDigits : 2,
                            currency              : 'USD',
                            style                 : 'currency',
                            currencyDisplay       : 'symbol'
                        }
                        var split_amount = amount.split('$');
                        amount = split_amount[1].split(',');
                        amount = parseInt(amount.join(''));
                        $('#ach_wire').attr('style', 'display: inline;');
                        var clewedfee = amount * ('".$fee_row['clewed_fee']."'/100);
                        var tempClewedFee = clewedfee;
                        clewedfee = clewedfee 
                        ? localStringToNumber(clewedfee).toLocaleString(undefined, options)
                        : ''
                        var profee_percent = '".$fee_row['paypal_fee']."'/100;
                        var processingfee = amount * ('".$fee_row['paypal_fee']."'/100);
                        console.log(processingfee,'wire');
                        var tempProcessingFee = processingfee;
                        var tempProcessingFee = processingfee;
                        processingfee = processingfee 
                        ? localStringToNumber(processingfee).toLocaleString(undefined, options)
                        : ''
                        $('#processingfee').val(processingfee);
                        var total = parseInt(amount) + parseInt(tempClewedFee) + parseInt(tempProcessingFee);
                        total = total 
                        ? localStringToNumber(total).toLocaleString(undefined, options)
                        : ''
                        $('#total').val(total);
                    }
                    if ($(this).val() == 'ach_wire'){
                        var amount = $('#amount').val();
                        var options = {
                            maximumFractionDigits : 2,
                            currency              : 'USD',
                            style                 : 'currency',
                            currencyDisplay       : 'symbol'
                        }
                        var split_amount = amount.split('$');
                        amount = split_amount[1].split(',');
                        amount = parseInt(amount.join(''));
                        $('#ach_wire').attr('style', 'display: inline;');
                        var clewedfee = amount * ('".$fee_row['clewed_fee']."'/100);
                        var tempClewedFee = clewedfee;
                        clewedfee = clewedfee 
                        ? localStringToNumber(clewedfee).toLocaleString(undefined, options)
                        : ''
                        var profee_percent = '".$fee_row['card_fee']."'/100;
                        var processingfee = amount * ('".$fee_row['card_fee']."'/100);
                        console.log(processingfee,'wire');
                        var tempProcessingFee = processingfee;
                        var tempProcessingFee = processingfee;
                        processingfee = processingfee 
                        ? localStringToNumber(processingfee).toLocaleString(undefined, options)
                        : ''
                        $('#processingfee').val(processingfee);
                        var total = parseInt(amount) + parseInt(tempClewedFee) + parseInt(tempProcessingFee);
                        total = total 
                        ? localStringToNumber(total).toLocaleString(undefined, options)
                        : ''
                        $('#total').val(total);
                    }
                });
                function getAmount(){
                    var amount = $('#amount').val();
                    var options = {
                        maximumFractionDigits : 2,
                        currency              : 'USD',
                        style                 : 'currency',
                        currencyDisplay       : 'symbol'
                    }
                    var tempAmount = amount;
                    amount = amount 
                        ? localStringToNumber(amount).toLocaleString(undefined, options)
                        : ''
                    $('#amount').val(amount);
                    var split_amount = amount.split('$');
                    amount = split_amount[1].split(',');
                    amount = parseInt(amount.join(''));
                    var max_amount = parseInt($('#maximum_amount').val());
                    var min_amount = parseInt($('#minimum_amount').val());
                    if (amount > max_amount || amount < min_amount){
                        alert('You must enter an amount within this range $' + min_amount + '~$' + max_amount);
                        $('#amount').val(null);
                        $('#clewedfee').val(null);
                        $('#processingfee').val(null);
                        $('#total').val(null);
                    } else {
                        var clewedfee = amount * ('".$fee_row['clewed_fee']."'/100);
                        var tempClewedFee = clewedfee;
                        clewedfee = clewedfee 
                        ? localStringToNumber(clewedfee).toLocaleString(undefined, options)
                        : ''
                        if($('#payment_method') == 'linked_bank') var profee_percent = '".$fee_row['ach_fee']."'/100;
                        if($('#payment_method') == 'paypal') var profee_percent = '".$fee_row['paypal_fee']."'/100;
                        if($('#payment_method') == 'ach_wire') var profee_percent = '".$fee_row['card_fee']."'/100;
                        var processingfee = amount * ('".$fee_row['ach_fee']."'/100);
                        var tempProcessingFee = processingfee;
                        processingfee = processingfee 
                        ? localStringToNumber(processingfee).toLocaleString(undefined, options)
                        : ''
                        $('#clewedfee').val(clewedfee);
                        $('#processingfee').val(processingfee);
                        var total = parseInt(tempAmount) + parseInt(tempClewedFee) + parseInt(tempProcessingFee);
                        total = total 
                        ? localStringToNumber(total).toLocaleString(undefined, options)
                        : ''
                        $('#total').val(total);
                    }
                }
                function localStringToNumber(string){
                    return Number(String(string).replace(/[^0-9.-]+/g,''));
                }
            </script>";
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function edit_capital_raising_info_step1($op = null)
{
    $companyid = sget($_REQUEST, 'id');

    $content .= drupal_get_form('maenna_forms_company_explore_capital_raising_step1_form');

    $title = "Step 1: Explore Capital Raising";

    return [
        'title' => $title,
        'body' => content_box($title, $content, [
            'close_link' => '/account?tab=companies&page=company_detail&id=' . $companyid . '&mtab=about'
        ])
    ];
}

function edit_capital_raising_info_step2($op = null)
{
    $companyid = sget($_REQUEST, 'id');

    $content .= drupal_get_form('maenna_forms_company_explore_capital_raising_step2_form');

    $title = "Step 2: Submit Capital Raising Related Information";

    return [
        'title' => $title,
        'body' => content_box($title, $content, [
            'close_link' => '/account?tab=companies&page=company_detail&id=' . $companyid . '&mtab=about'
        ])
    ];
}

function maenna_file($op = null)
{
    global $user;
    global $AccessObj;
    global $redirect;
    $section = __FUNCTION__;
    $userid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $File_Perm = $AccessObj->Com_sections['file']['sections'];
    if (!isset($_REQUEST['view'])) {
        if (isset($_REQUEST['tag'])) $tag = $_REQUEST['tag'];
        $data_type = $tag . '_files';
        $Block['title'] = "File Listing ";
        $File_Perm = $AccessObj->Com_sections['file']['sections'];
        $_Tags = _TAGS();
        $tag_no = count($_Tags);
        $i = 0;
        $canUploadAnyTag = 0;

        $sortLinks = '';
        $shortTags = _SHORT_TAGS();
        foreach (_TAGS() as $key => $value) {
            if ($tag == $key) $font_style = 'color:#00a2bf;';
            else $font_style = 'font-style:italic;';
            if ($File_Perm[$key . "_1"]['access'] == 'all' || $File_Perm[$key . "_1"]['access'] == 'write') $canUploadAnyTag = 1;
            if ($key == 'collaborator') {
                $key = 'other';
                $value = 'Other files';
            }

            if (
                $File_Perm[$key . "_2"]['access'] != 'hide'   ||
                $File_Perm[$key . "_1"]['access'] != 'hide'
            ) {
                $sortLinks .= "<a href='${redirect}&section=$section&tag=$key' style='$font_style font-size:12px;cursor:pointer;";

                if ($i == 0)
                    $sortLinks .= "margin-right:10px;'>{$shortTags[$key]}</a>";
                else
                    $sortLinks .= "margin-left:10px;margin-right:10px;'>{$shortTags[$key]}</a>";

                if (++$i != $tag_no)
                    $sortLinks .= '|';
            }
        }
        if (!empty($sortLinks))
            $sortLinks = rtrim($sortLinks, '|');

        $userService = new UserService();
        $tooltip = "File tab allows experts to share files with client and Clewed. Files tagged Deliverable Requirements define expected deliverables. Files tagged Expert Screening are private used only for vetting purposes. All files you upload in the File tab should be sanitized to exclude your brand name or logo. All files associated with a service you are involved with should be uploaded by editing that service, not in this tab.";
        if (
            $userService->isColleague($userid, $companyid) ||
            $userService->isCompany($userid) && $userid == $companyid ||
            $userService->isAdmin($userid, $companyid) ||
            $userService->isSuperAdmin($userid)
        ) {
            $tooltip = "Files you upload are private for project members and are shared as their roles require. Please sanitize files you upload to remove brand names and tag them according to purpose. Files related to specific project in the Workspace tab should be uploaded there.";
        }

        if ($canUploadAnyTag == 1 || $AccesObj->user_type == 'super') {
            $Block['title'] .= '<a data-tooltip="' . $tooltip . '" style="color:#fff;cursor:pointer;display:inline-block;margin-left:330px;
        margin-top:5px;height:20px;line-height:22px;border-left:thin solid #fff;padding-left:10px;font-family:LatoRegular; font-size:13px; text-transform: none;"
        class= "openbox" boxid="box1">Add<!--<img src="./themes/maennaco/images/upload_file_icon.png">--></a>';
        }
        $Block['title'] .= "<span style='margin-top:5px;height:20px;line-height:22px;border-left:thin solid #fff;padding-left:10px; float:right;display:inline-block;font-weight:bold; font-family:LatoRegular; font-size:13px; text-transform:none;'>Sort By <img class='openSortPanel' panelid='file_sort' style='width:20px; margin-left:20px;' src='themes/maennaco/images/arrow_down.png'></span>";
        $content = '';
        if (isset($tag) && $tag != 'other') {
            $dataTypeSql = " where data_type = '" . $data_type . "' ";
        } else $dataTypeSql = " where data_type LIKE '%_files'";
        $sql = "select * from maenna_company_data " . $dataTypeSql . " and companyid = %d and deleted != 1 order by data_type,access desc";
        $result = db_query($sql, array($companyid));
        //Set this page styles
        $content .= "<style>
                        .shaded_title {

                            background:#ffffff;
							font-size: 16px;
                            height:30px;
                            line-height:30px;
                        }
                        .main_content {
                            padding-top:0 !important;
                        }
                    </style>";
        $content .= '<div id="file_sort" style="display:none;line-height:30px;height:30px;width:100%; background-color:#f2f2f2"><span style="margin-top:0; display:inline-block;vertical-align:middle;margin-left:15px;">';
        $content .= $sortLinks . '</span></div>';
        $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
        $content .= "<div id='box1' style='margin-bottom:15px;display:none;'>
                        <form id='fileUpl' method=post action='/account' enctype='multipart/form-data' onsumbit=\"return validateFileForm();\">
                            <br><input style='width:197px!important;height:22px;margin-bottom:10px;' id='filename' type=text name=name placeholder='NAME' /><br>
                            <select id='tags' style='width:207px;height:28px;margin-bottom:10px;' name='tag'><option value='0'>Tag:</option>";
        foreach (_TAGS() as $key => $value) {
            if ($File_Perm[$key . "_1"]['access'] == 'all' || $File_Perm[$key . "_1"]['access'] == 'write') {
                $content .= "<option value='" . $key . "'>" . $value . "</option>";
            }
        }
        $content .= "</select><br><input style='margin-bottom: 10px;' type=file name='file' /></br>
          <input type=submit name=submit value='Submit'  class='button' /> &nbsp;
                <a href='#' class='hidebox button' boxid=box1>Close</a>
        </div>
        $HPV
        <input type='hidden' name=section value='files' />
        <input type='hidden' name=update_section value='files' />
        <input type='hidden' name=do value='new' />
      </form>
                    ";
        $content .= "<div id='box2' style='margin-bottom:15px;display:none;'>
                        <form id='fileReplace' method=post action='/account' enctype='multipart/form-data'>
                            <br><input style='width:155px;height:22px;margin-bottom:4px;' type=text name=name value='' />
                            <br><input type=file name='file' /></br><br><input style='width:75px; height:28px;' type=submit name=submit value='submit'  class=button /> &nbsp;
                                    <a href='#' style='padding: 5px 17px;' class='hidebox button' boxid=box2>Close</a>
                            </div>
                            $HPV
                            <input type='hidden' name=section value='files' />
                            <input type='hidden' name=update_section value='files' />
                            <input type='hidden' name=do value='update' />
                            <input type='hidden' name='dataid'/>
                        </form>
                    ";
        while ($Row = db_fetch_object($result)) {
            $nameLong = htmlentities(ucwords($Row->data_value), ENT_QUOTES);
            if (strlen($nameLong) > 30) $name = substr($nameLong, 0, 30) . "...";
            else $name = $nameLong;
            $filename = $Row->data_value2;
            $dataid = $Row->dataid;
            $path = "/" . file_directory_path() . "/" . $filename;
            if (!file_exists(dirname(__FILE__) . '/../../..' . $path)) {
                continue;
            }
            $ext = pathinfo($filename, PATHINFO_EXTENSION);
            $editorid = $Row->editorid;
            $access = $Row->access;
            if (in_array($ext, array('doc', 'xls', 'pdf', 'ppt', 'tiff', 'odt', 'txt'))) {
                $type_icon = '<img width="20px" height="20px" src="./themes/maennaco/images/' . $ext . '_icon.png">';
            }
            $arr = explode("_", $Row->data_type, 2);
            $cat = $arr[0];
            $catName = getFileCategoryName($cat);
            $tmpRow = '';
            $tmpRow .= "\n<tr>
                        <td style='padding-left:15px;padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:left;height:25px;' title='$name'>
                         $type_icon <span title = '" . date('d/m/Y', $Row->access) . " - " . getUserById($Row->editorid) . "'>$name</span>
                        </div></td>
                            <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:left;height:25px;' title='$ext'>$ext</div></td>
                            <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:left;height:25px;' title='$editorid'>" . str_replace('PROJECT ', '', getUserById($editorid)) . "</div></td>
                            <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:left;height:25px;' title='$access'>" . showTime($access) . "</div></td>";
            //If file file author is currently logged in user
            if ($userid == $editorid || $AccessObj->user_type == 'super') {
                $tmpRow .= "<td style='padding-top:5px;font-size:11px;'>
                                    <a style='margin-left:5px;display:inline-block;margin-right: 5px;' href = '/account?tab=companies&page=company_detail&eid=$Row->editorid&dataid=$Row->dataid&datatype=$Row->data_type&id=$companyid&mtab=file&name=" . rawurlencode($filename) . "&view=true' title = 'View'>
                                    <img src = '/themes/maennaco/images/view_file_icon.png'>
                                    </a>
                                    <a title='Replace' class= 'openbox fileReplace' boxid='box2' style='cursor:pointer;display:inline-block;margin-right: 5px;' ref='" . $dataid . "' fileName = '" . $nameLong . "'><img src = '/themes/maennaco/images/replace_file_icon.png'></a>
                                    <a title='Download' href = '" . $path . "' style='display:inline-block;margin-right: 5px;' ><img src = '/themes/maennaco/images/download_file_icon.png'></a>
                                    <a title='Delete' href='.$redirect&update_section=files&do=del&dataid=" . $dataid . "'
                                                    onclick='return confirm(\"Continue to delete this file?\")' style='display: inline-block;border-left: 1px solid;line-height: 1.1em;padding-left: 5px;' class='tool'>Delete</a>
                                    </td>";
            } else {
                if ($File_Perm[$cat . "_2"]['access'] == 'hide') {
                    $tmpRow .= "<td style='padding-top:5px;font-size:11px;'></td>";
                } elseif ($File_Perm[$cat . "_2"]['access'] == 'view') {
                    $tmpRow .= "<td style='padding-top:5px;font-size:11px;'>
                                    <a style='margin-left:5px;display:inline-block;margin-right: 5px;' href = '/account?tab=companies&page=company_detail&eid=$Row->editorid&dataid=$Row->dataid&datatype=$Row->data_type&id=$companyid&mtab=file&name=" . rawurlencode($filename) . "&view=true' title = 'View'>
                                    <img src = '/themes/maennaco/images/view_file_icon.png'>
                                    </a></td>";
                } else {
                    $tmpRow .= "<td style='padding-top:5px;font-size:11px;'>
                                    <a style='margin-left:5px;display:inline-block;margin-right: 5px;' href = '/account?tab=companies&page=company_detail&eid=$Row->editorid&dataid=$Row->dataid&datatype=$Row->data_type&id=$companyid&mtab=file&name=" . rawurlencode($filename) . "&view=true' title = 'View'>
                                    <img src = '/themes/maennaco/images/view_file_icon.png'>
                                    </a>
                                    <a style='display:inline-block;margin-right: 5px;' href='$path'><img src = '/themes/maennaco/images/download_file_icon.png'></a>
                                    </td>";
                }
            }
            $tmpRow .= "</tr>";
            if ($catName == 'Other') $cat = 'Other';
            $fileList[$cat][] = $tmpRow;
        }
        $content .= '<table style="margin:0; cellspacing="0" cellpadding="0" border="0" width="100%" class="left-align">
                    <thead style="background:#F9FAFC;">
                        <tr>
                            <td style="font-weight:normal;width:40%; padding-left:15px;">NAME</td>
                            <td style="font-weight:normal;width:8%">TYPE</td>
                            <td style="font-weight:normal;width:20%">UPLOADED BY</td>
                            <td style="font-weight:normal;width:10%">DATE</td>
                            <td style="font-weight:normal;width:22%">ACTIONS</td>

                        </tr></thead>';
        //test_array($fileList);
        if ($tag != 'other') {
            foreach (_TAGS() as $key => $value) {
                if (count($fileList[$key]) > 0 && ($File_Perm[$key . "_1"]['access'] != 'hide' || $File_Perm[$key . "_2"]['access'] != 'hide')) {
                    $content .= "<tr><td style='font-style:italic;padding-left:15px; background:#f2f2f2;' colspan = '5'>$value</td></tr>";
                    foreach ($fileList[$key] as $value1) {
                        $content .= $value1;
                    }
                }
            }
        }
        if (!isset($tag) || $tag == 'other') {
            if (count($fileList['Other']) > 0 && ($File_Perm["other_1"]['access'] != 'hide' || $File_Perm["other_2"]['access'] != 'hide')) {
                $content .= "<tr><td style='padding-left:15px; background:#f2f2f2;' colspan = '5'>Other</td></tr>";
                foreach ($fileList['Other'] as $value) {
                    $content .= $value;
                }
            }
        }
        $content .= "</table>

        <div id='uploadDlg' style='display:none;'></div>

        <script type='text/javascript' src='/themes/maennaco/jui/comments/js/jquery.livequery.js'></script>
        <script type='text/javascript'>
$(document).ready(function(){

    $('.fileReplace').click(function() {

    $(\"#fileReplace input[name='name']\").val($(this).attr('fileName'));
    $(\"input[name='dataid']\").val($(this).attr('ref'));

    });
    init_openbox();
    init_hidebox();
    init_sortPanel();

    $('#fileUpl').submit(function(event) {



   if ($('#filename').val() == '') {


        $('#filename').addClass('inerror');
        event.preventDefault();

   }
   else $('#filename').removeClass('inerror');


   if ($(\"#tags\").val() == '0') {

   $('#tags').addClass('inerror');
   event.preventDefault();

   }

   else $('#tags').removeClass('inerror');

    });


    });
</script>";
    } else {
        $Block['title'] = "File View";
        if (sget($_REQUEST, 'name')) {
            $Block['title'] = sget($_REQUEST, 'name');
            $fname = sget($_REQUEST, 'name');
        }
        $file_array = pathinfo(sget($_REQUEST, 'name'));
        $f_extension = $file_array['extension'];
        if (in_array($f_extension, array('doc', 'xls', 'pdf', 'ppt', 'tiff'))) {
            $arr = explode("_", sget($_REQUEST, 'datatype'), 2);
        }
        $cat = $arr[0];
        $catName = getFileCategoryName($cat);
        $tmpRow = '<div style="margin-right:18px;float:right;">';
        $sql = "select * from maenna_company_data WHERE deleted != 1 and dataid = %d";
        $result = db_query($sql, array(sget($_REQUEST, 'dataid')));
        if ($Row = db_fetch_object($result)) {
            $url = "http://" . $_SERVER['HTTP_HOST'] . "/" . file_directory_path() . "/" . $Row->data_value2;
            $res  = mysql_query("SELECT d_id FROM wall_documents where document_name = '" . basename($url) . "'");
            $red  = mysql_fetch_array($res);
            $f_id = $red['d_id'];
        }
        $editorname = $user->name;
        $editorid = $Row->editorid;
        //If file file author is currently logged in user
        if ($userid == $editorid || $AccessObj->user_type == 'super') {
            $tmpRow .= "

                                    <a data-tooltip='Replace file' title='REPLACE' class= 'openbox fileReplace' boxid='box2' style='display:inline-block;margin-right: 5px;' ref='" . sget($_REQUEST, 'dataid') . "' fileName = '" . sget($_REQUEST, 'name') . "' href = '#'><img src = '/themes/maennaco/images/replace_file_icon.png'></a>
                                    <a data-tooltip='Download file' title='DOWNLOAD' href = '/download.php?tab=file&file_name=" . rawurlencode($Row->data_value2) . "' style='display:inline-block;margin-right: 5px;'><img src = '/themes/maennaco/images/download_file_icon.png'></a>
                                    <a data-tooltip='Delete file' title='DELETE' href='.$redirect&update_section=files&do=del&dataid=" . sget($_REQUEST, 'dataid') . "'
                                                    onclick='return confirm(\"Continue to delete this file?\")' style='display: inline-block;border-left: 1px solid;line-height: 1.1em;padding-left: 5px;' class='tool'>Delete</a>
                                    ";
        } else {
            if ($File_Perm[$cat . "_2"]['access'] == 'hide') {
                $tmpRow .= "";
            } elseif ($File_Perm[$cat . "_2"]['access'] == 'view') {
                $tmpRow .= "";
            } else {
                $tmpRow .= "<a  style='display:inline-block;margin-right: 5px;' href='/download.php?tab=file&file_name=" . $Row->data_value2 . "'><img src = '/themes/maennaco/images/download_file_icon.png'></a>
                                    ";
            }
        }
        $tmpRow .= '</div>';
        $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
        $Block['title'] .= $tmpRow;
        $content .= "<div id='box2' style='margin-bottom:15px;display:none;'>
                        <form id='fileReplace' method=post action='/account' enctype='multipart/form-data'>
                            <br><input style='width:155px;height:22px;margin-bottom:4px;' type=text name=name value='' />
                            <br><input type=file name='file' /></br><br><input style='padding-top:3px;width:75px; height:28px;' type=submit name=submit value='submit'  class=buttonReplace /> &nbsp;
                                    <a href='#' style='width: 53px;height: 20px;display: inline-block;text-align: center;' class='hidebox buttonReplace' boxid=box2>Close</a>
                            </div>
                            $HPV
                            <input type='hidden' name=section value='files' />
                            <input type='hidden' name=update_section value='files' />
                            <input type='hidden' name=do value='update' />
                            <input type='hidden' name='dataid' value='" . $f_id . "'/>
                        </form>
                    ";
        ob_start();
        require_once("file_comments.php");
        $content .= ob_get_contents();
        $content .= "
                <script type='text/javascript'>
$(document).ready(function(){
    init_openbox();
    init_hidebox();

    $('.fileReplace').click(function() {

    $(\"#fileReplace input[name='name']\").val($(this).attr('fileName'));
    $(\"input[name='dataid']\").val($(this).attr('ref'));

    });
});
</script>

        <style>

                        .buttonReplace {
                            font-family: 'Lato Regular' !important;
                            padding: 2px 9px;
                            background: #41454F;
                            color: white !important;
                            border: 2px outset buttonface;
                            text-transform: capitalize;
                            display:inline-block;
                            text-align:center;
                            font-size:12px;
                        }
                        .buttonReplace:hover {
                            background: #00a2bf;
                            text-decoration: none;
                            padding: 2px 9px;
                        }
                        .shaded_title {

                            background:#ffffff;
                            height:30px;
                            line-height:30px;
                            padding-left:15px;

                        }

                        .main_content {

                            padding-top:0 !important;

                        }

                    </style>";
        ob_end_clean();
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function maenna_events($op = null)
{
    global $user, $baseUrl;
    global $AccessObj;
    $editorid = $user->uid; // the currently logged in user, print_r this object to see all data available
    $companyid = sget($_REQUEST, 'id');
    $content = '';
    global $redirect;
    $data_type = 'discussions';
    $panel = 'multi';
    $section = __FUNCTION__;
    $Block['title'] = 'SERVICES';

    $companyService = new Clewed\Company\Service();

    $createServiceToolCaption = 'Create service or meeting';
    if ('company' == $AccessObj->user_type && $companyid == $editorid || 'super' == $AccessObj->user_type || 'admin' == $AccessObj->user_type)
        $createServiceToolCaption = 'Order service';

    if ($AccessObj->Com_sections['advice']['sections']['nav_menu_right']['access'] == 'write') {
        $Block['title'] .= '<div class="schedule"><div class="discmnts"></div><div class="sched"><a id="showEventForm"><span data-tooltip="Do not add confidential information including company name until service is marked started and expert invitation tool is locked." style="text-transform: none;">' . $createServiceToolCaption . '</span></a></div></div>';
    }
    if (sget($_REQUEST, 'name')) {
        $Block['title'] = sget($_REQUEST, 'name');
        $fname = sget($_REQUEST, 'name');
    }
    ob_start();
    $url = "http://" . $_SERVER['HTTP_HOST'];
    $editorname = $user->name;
    $view_id = sget($_REQUEST, 'view_id');

    echo '<script type="text/javascript">
            $(document).ready(function(){
                $(".pro_popup").livequery("click", function(e) {


        e.preventDefault();
        var pro_id = $(this).attr("rel");
        uid = "' . $user->uid . '";
        inv = "";
        $.post("/themes/maennaco/includes/pro_posts.php?type=profileInfo&display=true&pro_id=" + pro_id + "&uid=" + uid + inv, {
        }, function(response) {

          $("#pro_popup").dialog({
            autoOpen: true,
            width: 650,
            title: "Profile",
            resizable: false,
            draggable: false,
            height: 400,
            closeText: "hide",
            buttons: {
              /*"Close": function() { $(this).dialog("close"); }*/

            }, closeOnEscape: true,
            modal: true
          }).html(response);
        },"html");
      });
            })
            </script>';
    if ($view_id) {
        $event_q = db_query("SELECT * FROM maenna_company_events WHERE eventid = %d", $view_id);
        $event = db_fetch_array($event_q);

        $Block['title'] = '';
        $eventShouldBeVisible = false;

        $db = \Clewed\Db::get_instance();
        $rows = $db->get_array(
            "
            SELECT uid
            FROM maenna_company_events_inv
            WHERE eventid = ?
            AND status <> 'declined'",
            array((int) $view_id)
        );

        $invitedExpertIds = array();
        foreach ($rows as $row)
            $invitedExpertIds[$row['uid']] = $row['uid'];

        $invitedExpertIds = array_values($invitedExpertIds);
        if (in_array($user->uid, $invitedExpertIds)) {
            if (in_array($user->uid, $companyService->getColleagueIds($event['companyid'])))
                if ($event['approved'])
                    $eventShouldBeVisible = true;
                else
                    $eventShouldBeVisible = false;

            else $eventShouldBeVisible = true;
        }
        //var_dump($AccessObj->user_type , $event, $user);
        if ($event['approved'] && $event['companyid'] == $user->uid)
            $eventShouldBeVisible = true;

        if ($event['approved'] && $event['executor_id'] == $user->uid && 'declined' != $event['executor_status'])
            $eventShouldBeVisible = true;

        if ($event['postedby'] == $user->uid) // && $AccessObj->user_type != 'people')
            $eventShouldBeVisible = true;

        if ('super' == $AccessObj->user_type || 'admin' == $AccessObj->user_type)
            $eventShouldBeVisible = true;

        /*
        Turning off permission for colleagues to see approved or owners services with commenting section
        if (in_array($AccessObj->uid,$companyService->getColleagueIds($event['companyid'])) && ($event['approved'] || $event['postedby'] == $event['companyid'])) {
            $eventShouldBeVisible = true;
        }*/

        if ($eventShouldBeVisible) {
            $Block['title'] = $event['title'];
            require_once('events_view_event.php');
        } else
            echo 'You are not allowed to view this content';
    } else {
        require_once("events_comments.php");
    }
    $content = ob_get_contents();
    ob_end_clean();
    //$content = sget($_REQUEST, 'file');
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function maenna_analysis($op = null)
{
    global $user;
    global $base_url;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Analysis";
    $content = '';
    global $redirect;
    $data_type = 'analysis';
    $panel = 'multi';
    $section = __FUNCTION__;
    if ($op) {
        $limit = 3;
        $_page = sget($_REQUEST, '_page');
        if (empty($_page)) $_page = 1;
        $start = ($_page - 1) * $limit;
        if ($_REQUEST['sort'] != '') {
            $sql = "select count(*) as total from maenna_company_data where companyid = %d and data_type = '$data_type' and deleted !=1 AND tags LIKE '%" . $_REQUEST['sort'] . "%'";
        } elseif ($_REQUEST['sortmonth'] != '') {
            $sql = "select count(*) as total from maenna_company_data where companyid = %d and data_type = '$data_type' and deleted !=1 AND MONTH(FROM_UNIXTIME(access)) = " . $_REQUEST['sortmonth'] . " ";
        } elseif ($_REQUEST['sortby'] != '') {
            $sql = "select count(*) as count_likes,prof_id, com.* from like_analysis, maenna_company_data com WHERE com.companyid = %d and com.data_type = '$data_type' and com.deleted != 1 and com.dataid = like_analysis.prof_id group by prof_id order by count(*) desc";
        } else {
            $sql = "select count(*) as total from maenna_company_data where companyid = %d and data_type = '$data_type' and deleted !=1 ";
        }
        $result = db_query($sql, array($companyid));
        $Row = db_fetch_object($result);
        $total = $Row->total;
        if ($_REQUEST['sort'] != '') {
            $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type' and deleted != 1 AND tags LIKE '%" . $_REQUEST['sort'] . "%' order by dataid";
        } elseif ($_REQUEST['sortmonth'] != '') {
            $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type' and deleted != 1 AND MONTH(FROM_UNIXTIME(access)) = " . $_REQUEST['sortmonth'] . " order by dataid";
        } elseif ($_REQUEST['sortby'] != '') {
            $sql = "select count(*) as count_likes,prof_id, com.* from like_analysis, maenna_company_data com WHERE companyid = %d and com.data_type = '$data_type' and com.deleted != 1 and com.dataid = like_analysis.prof_id group by prof_id order by count(*) desc";
        } else {
            $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and deleted != 1 order by dataid";
        }
        if ($op == 'write' || $op == 'edit') $Block['title'] .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=add&&section=$section' class='tool' >Add</a></div>";
        $result = db_query($sql, array($companyid, $data_type));
        while (($Row = db_fetch_object($result))) {
            $P_username = getUserById($Row->editorid);
            $access = date('m/d/Y', $Row->access);
            $title = strtoupper($Row->data_value);
            if ($title) {
                $title = htmlentities($title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            } else $title = "&nbsp;";

            $dataid = $Row->dataid;
            $company_id = $Row->companyid;
            $tags = $Row->tags;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid&section=$section";
            $content .= "<div class=\"rt-block\">";
            date_default_timezone_set('EST');
            $date_cre = date('l, M j, Y g:i A T ', $Row->access);
            if ($Row->status == 0) {
                $savedby = 'Saved by';
            } else {
                $savedby = 'Published by';
            }
            if ($Row->short_description == '') {
                $text1 = $Row->data_value2;
                $text = substr($text1, 0, 1800);
                /*                $a = explode(' ', $text);
                                array_pop($a);
                                $text = join(' ', $a);*/
                if (strlen($text1) > strlen($text)) $text .= "&nbsp;<a href='$base_url/account?tab=companies&page=company_detail&id=" . $companyid . "&mtab=summary&type=details&analysis_id=" . $dataid . "'>...more</a>";
            } else {
                $text = $Row->short_description;
            }
            $content .= "<div class=\"rt-title\">";
            $content .= "<a href='$base_url/account?tab=companies&page=company_detail&id=" . $companyid . "&mtab=summary&type=details&analysis_id=" . $dataid . "'>$title</a>";
            $content .= "</div>";
            $content .= "<div class=\"rt-date\" style='margin-left: 4px; margin-top: -5px; '> $savedby";
            $content .= "<strong> $P_username </strong> on <span class='rt-date'>$date_cre</span>";
            $content .= "<div style='float: right; color: rgb(118, 120, 127); margin: -7px 0pt 0pt;'>" . ($tags ? "Category: $tags" : '') . "</div>";
            $content .= "</div>";
            $content .= "<div style='clear:both'></div>";
            $content .= "<div>" . _filter_autop($text) . "</div>";
            $content .= "</div>
"; // closing .rt-block
        }
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function maenna_share_knowledge($op = null)
{
    global $user;
    global $base_url;
    $editorid = $user->uid;
    $companyid = (int) sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Scope Discussion";
    $content = '';
    global $redirect;
    $data_type = 'share_knowledge';
    $panel = 'multi';
    $section = __FUNCTION__;
    if ($op) {
        $limit = 3;
        $_page = sget($_REQUEST, '_page');
        if (empty($_page)) $_page = 1;
        $start = ($_page - 1) * $limit;

        $authorIsCurrentUser = " editorid='{$user->uid}' ";
        $authorIsCompany = " editorid='{$companyid}' ";
        $authorIsAdmin = " editorid IN(SELECT DISTINCT(uid) FROM users_roles WHERE rid IN(6, 10)) ";
        $noInvites = " (SELECT COUNT(*) FROM discussion_invites WHERE discussion_id = dataid) <= 0 ";
        $isBroadcast = " ({$authorIsCompany} OR {$authorIsAdmin}) AND {$noInvites} ";
        $isAdmin = " (SELECT uid FROM users_roles WHERE rid IN(6, 10) AND uid={$user->uid} LIMIT 1) IS NOT NULL ";
        $isInvited = " (SELECT user_id FROM discussion_invites WHERE discussion_id = dataid AND user_id='{$user->uid}') IS NOT NULL ";

        $companyService = new \Clewed\Company\Service();

        $isCompany = $companyid == $user->uid;
        $isColleague = false;
        $connections = Connections::Com_conns($companyid);
        foreach ($connections['Client'] as $connection)
            if ($connection->assignee_uid == $user->uid)
                $isColleague = true;

        $isCompanyStaff = ' 2=1 ';
        if ($isCompany || $isColleague)
            $isCompanyStaff = " {$noInvites} ";

        $contentReducer = " AND (
            {$isAdmin} OR
            {$authorIsCurrentUser} OR
            {$isBroadcast} OR
            {$isInvited} OR
            {$isCompanyStaff}
        )";

        if ($_REQUEST['sort'] != '') {
            $sql = "select count(*) as total from maenna_company_data where companyid = %d $contentReducer and data_type = '$data_type' and deleted !=1 AND tags LIKE '%" . $_REQUEST['sort'] . "%'";
        } elseif ($_REQUEST['sortmonth'] != '') {
            $sql = "select count(*) as total from maenna_company_data where companyid = %d $contentReducer and data_type = '$data_type' and deleted !=1 AND MONTH(FROM_UNIXTIME(access)) = " . $_REQUEST['sortmonth'] . " ";
        } elseif ($_REQUEST['sortby'] != '') {
            $sql = "select count(*) as count_likes,prof_id, com.* from like_analysis, maenna_company_data com WHERE com.companyid = %d $contentReducer and com.data_type = '$data_type' and com.deleted != 1 and com.dataid = like_analysis.prof_id group by prof_id order by count(*) desc";
        } else {
            $sql = "select count(*) as total from maenna_company_data where companyid = %d $contentReducer and data_type = '$data_type' and deleted !=1 ";
        }

        $result = db_query($sql, array($companyid));
        $Row = db_fetch_object($result);
        $total = $Row->total;

        $sql = "select m.*, count(distinct p.pid) as posts_num, count(distinct a.cid) as comments_num
            from maenna_company_data as m
            left join pro_analysis_posts as p
             on p.pro_id = m.dataid
            left join analysis_wall_post_comments a
             on a.post_id = p.pid";
        $conditions = " where
            companyid = %d $contentReducer
            and m.data_type = '%s'
            and m.deleted != 1";
        $group = " group by m.dataid
              order by m.dataid desc";

        if ($_REQUEST['sort'] != '') {
            $conditions .= " AND tags LIKE '%" . $_REQUEST['sort'] . "%'";
        } elseif ($_REQUEST['sortmonth'] != '') {
            $conditions .= " AND MONTH(FROM_UNIXTIME(access)) = " . $_REQUEST['sortmonth'];
        } elseif ($_REQUEST['sortby'] != '') {
            $sql = "select count(*) as count_likes,prof_id, com.* from like_analysis, maenna_company_data com WHERE companyid = %d $contentReducer and com.data_type = '$data_type' and com.deleted != 1 and com.dataid = like_analysis.prof_id group by prof_id order by count(*) desc";
            $conditions = $group = '';
        }

        if ($op == 'write' || $op == 'write_own' || $op == 'read_all_write_own' || $op == 'edit')
            $Block['title'] .=
                "<div style='left:495px;top:10px;height:15px;line-height:15px;width:120px;' class=editbtn>
                    <a style='float:left;padding-right: 2px;padding-left:3px;margin-right:3px;'
                    href='${redirect}&panel=${panel}&view=add&&section=$section'
                    data-tooltip='Use this tool to add content about project scope, expertise clarification and expert collaboration topics. Selected experts will be able to create services with clear deliverables and price for approval after this stage. Discussing contact information is prohibited.'
                    class='tool'>Add discussion</a>
                    <a class='company-discussions-add-tooltip' style=\"cursor:pointer;\">
                        <img src=\"/themes/maennaco/images/questionmark_white.png\">
                    </a>
                    <div class='company-discussions-add-tooltip-dialog hidden'>
                        This tab is for connected experts to communicate their interest with project administrator
                        for consideration. Your response to a requirement posting or your message to indicate interest
                        and discuss qualification for this project is reviewed by the project administrator privately.
                    </div>
                    <script>
                        $('.company-discussions-add-tooltip').click(function () {
                            $('.company-discussions-add-tooltip-dialog').dialog({
                                modal: true,
                                height: \"240\",
                                autoOpen: true,
                                title: 'Discussions help',
                                resizable: false,
//                                buttons: {
//                                    Close: function () {
//                                        $(this).dialog(\"close\");
//                                    }
//                                },
                                open: function (event, ui) {
                                    $(this).scrollTop(0);
                                }
                            });
                        });
                       function showExpertInfo(expertId) {
                            var uid = \"" . $user->uid . "\";
                            $.post(\"/themes/maennaco/includes/pro_posts.php?type=profileInfo&display=true&pro_id=\" + expertId + \"&uid=\" + uid, function (response) {

                                $(\"#pro_popup\").dialog({
                                    autoOpen: true,
                                    width: 650,
                                    title: \"Profile\",
                                    resizable: false,
                                    draggable: false,
                                    height: 400,
                                    closeText: \"hide\",
                                    buttons: {},
                                    closeOnEscape: true,
                                    modal: true
                                }).html(response);

                            }, \"html\");
                        }

                    </script>
                </div>";

        $result = db_query($sql . $conditions . $group, array($companyid, $data_type));
        while (($Row = db_fetch_object($result))) {

            $P_username = getUserById($Row->editorid);
            $access = date('m/d/Y', $Row->access);
            $title = strtoupper($Row->data_value);
            if ($title) {
                $title = htmlentities($title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            } else $title = "&nbsp;";

            $dataid = $Row->dataid;
            $company_id = $Row->companyid;
            $tags = $Row->tags;
            $more_link = $base_url . "/account?tab=companies&page=company_detail&id=" . $companyid . "&mtab=share_knowledge_details&type=details&data_id=" . $dataid;

            $invitedUserIds = $companyService->getDiscussionInvitedExpertIds($dataid);

            foreach ($invitedUserIds as $uid) {
                $userType = getUserTypeById($uid);
                $uname = getProId($uid);
                if ('company' == $userType) {
                    $proId = getProjectName($uid);
                    $invitedExperts[] = "<a class=\"tool\" style=\"font-style:italic;\" target=\"_blank\" href=\"/account?tab=companies&page=company_detail&id=" . $uid . "&mtab=about\"><b>" . $uname . "</b></a>";
                } else
                    $invitedExperts[] = "<a class=\"tool\" onclick=\"showExpertInfo({$uid});\" style=\"font-style:italic;\"><b>" . $uname . "</b></a>";
            }

            if (!empty($invitedExperts)) {
                $invitedLabel = "<span class=\"profile_info service-list-item-experts\" style=\"float:left;margin:10px 0 6px 0;\">Invited: " . implode(', ', $invitedExperts) . "</span>";
            } else $invitedLabel = '';

            $content .= "<div id=\"pro_popup\" class=\"hidden\"></div><div style='cursor:pointer;' onclick=\"location.href='$base_url/account?tab=companies&page=company_detail&id=" . $companyid . "&mtab=share_knowledge_details&type=details&data_id=" . $dataid . "';\" class=\"rt-block\">";

            date_default_timezone_set('EST');
            $date_cre = date('l, M j, Y g:i A T ', $Row->access);
            if ($Row->status == 0) {
                $savedby = 'Saved by';
            } else {
                $savedby = 'Published by';
            }
            if ($Row->short_description == '') {
                $text1 = $Row->data_value2;
                $text = substr($text1, 0, 1800);
                /*                $a = explode(' ', $text);
                                array_pop($a);
                                $text = join(' ', $a);*/
                if (strlen($text1) > strlen($text)) $text .= "&nbsp;<a class='rt-date' href='" . $more_link . "'>...more</a>";
            } else {
                $text = $Row->short_description;
            }
            $content .= "<div class=\"rt-title\" style='margin-right: -7px;'>";
            $content .= "<a href='$base_url/account?tab=companies&page=company_detail&id=" . $companyid . "&mtab=share_knowledge_details&type=details&data_id=" . $dataid . "'>" . substr($title, 0, 52) . "</a>";
            $content .= "</div>";
            $content .= "<div class=\"rt-date\" style='margin-left: 4px; margin-top: -5px; '> $savedby";
            $content .= "<strong> $P_username </strong> on <span class='rt-date'>$date_cre</span>";
            $content .= "<div style='float: right; color: rgb(118, 120, 127); margin: -16px 0pt 0pt;'><div>" . ($tags ? "Category: $tags" : '') . "</div>";
            $content .= "<div style='margin-top: -7px;'><a class='rt-date' href='" . $more_link . "'>" . $Row->posts_num . " posts, " . $Row->comments_num . " comments" . "</a></div></div>";
            $content .= "</div>";
            $content .= $invitedLabel;
            $content .= "<div style='clear:both'></div>";
            $content .= "<div class='share-knowledge-content'>" . _filter_autop($text) . "</div>";
            $content .= "</div>
"; // closing .rt-block
            unset($invitedExperts);
        }
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function share_knowledge_details($op = null)
{
    global $user;
    global $AccessObj;
    $editorid = $user->uid; // the currently logged in user, print_r this object to see all data available
    $companyid = sget($_REQUEST, 'id');
    $content = '';
    global $redirect;
    $data_type = 'discussions';
    $panel = 'multi';
    $section = __FUNCTION__;

    $Block['title'] = '';
    $content = "<div style='left:490px;top:5px;height:15px;line-height:15px;width:120px; border-left: 0px;' class='tabtags editbtn'>
                    <a style='float: right; padding-right: 2px; padding-left:3px; margin-right:3px; font-family: \"LatoRegular\"; font-size: 14px;'
                    href='account?tab=companies&page=company_detail&id=${companyid}&mtab=share_knowledge&panel=multi&view=add&&section=maenna_share_knowledge'
                    data-tooltip='Use this tool to add content about project scope, expertise clarification and expert collaboration topics. Selected experts will be able to create services with clear deliverables and price for approval after this stage. Discussing contact information is prohibited.'
                    class='tool'>Add discussion</a>
                    </div>";

    ob_start();
    $url = "http://" . $_SERVER['HTTP_HOST'];
    $editorname = $user->name;
    require_once("share_knowledge_details.php");
    $content .= ob_get_contents();
    ob_end_clean();
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function analysis_details($op = null)
{
    global $user;
    global $AccessObj;
    $editorid = $user->uid; // the currently logged in user, print_r this object to see all data available
    $companyid = sget($_REQUEST, 'id');
    $content = '';
    global $redirect;
    $data_type = 'discussions';
    $panel = 'multi';
    $section = __FUNCTION__;
    ob_start();
    $url = "http://" . $_SERVER['HTTP_HOST'];
    $editorname = $user->name;
    require_once("analysis_details.php");
    $content = ob_get_contents();
    ob_end_clean();
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function maenna_plan($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Performance";
    $content = '';
    $section = __FUNCTION__;
    global $redirect;
    $data_type = 'plan';
    $panel = 'multi';
    if ($op) {
        $limit = 3;
        $_page = sget($_REQUEST, '_page');
        if (empty($_page)) $_page = 1;
        $start = ($_page - 1) * $limit;
        $sql = "select count(*) as total from maenna_company_data where data_type = '$data_type' and companyid = %d and deleted != 1";
        $result = db_query($sql, array($companyid));
        $Row = db_fetch_object($result);
        $total = $Row->total;
        $sql = "select * from maenna_company_data where data_type = '$data_type' and companyid = %d and deleted != 1 order by dataid limit $start, $limit";
        $result = db_query($sql, array($companyid));
        while (($Row = db_fetch_object($result)) !== false) {
            $access = date('m/d/Y', $Row->access);
            $title = ucwords($Row->data_value);
            if ($title) {
                $title = htmlentities($title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            } else $title = "&nbsp;";
            $text = nl2br(htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
            $content .= "<div class=entry>";
            if ($op == 'write' || ($op == 'edit' && $user->uid == $_REQUEST['id'])) $content .= "<div class=editbtn><a href='${redirect}&section=$section&panel=${panel}&view=edit&dataid=$dataid' class='tool' >Edit</a></div>";
            $content .= "<a href='" . $more_link . "'><div style='border-bottom:solid 1px #d0d2d3;' class='performance-title'>$title</div></a><div class='entry-content'>" . _filter_autop(html_entity_decode($text)) . "</div></div><hr class='line' />";
        }
        $Pagenation = array(
            'total'        => $total,
            'limit'        => $limit,
            'baseurl'      => '/account?',
            'num_of_links' => 8,
        );
        $content .= pagination($Pagenation);
    }
    if ($op == 'write' || ($op == 'edit' && $user->uid == $_REQUEST['id'])) $Block['title'] .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=add&section=" . __FUNCTION__ . "' class='tool' >Add</a></div>";
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function superadmin_comprogress($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = 'Progress (super admin only) ';
    $content = '';
    $time = time();
    $content = '';
    if ($op == null || $op == 'view') {
        if ($companyid) {
            $sql = "select * from maenna_company_data where data_type = 'progress' and companyid = %d and deleted != 1 order by dataid desc limit 3";
            $result = db_query($sql, array($companyid));
            while (($Row = db_fetch_object($result)) !== false) {
                $date = date('m/d/Y', $Row->access);
                $title = htmlentities($Row->data_value, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                $text = htmlentities(contentExcerpt($Row->data_value2), ENT_QUOTES | ENT_IGNORE, "UTF-8");
                $followup = $Row->data_attr;
                if ($followup) $followup = "(follwup: " . date('m/d/Y', $followup) . ")";
                $access = date('m/d/Y', $Row->access);
                $content .= "\n<div class=row><b>$access - $title $followup</b><p>$text</p></div>";
            }
        }
    }
    $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>" .
        "<a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&view=add&companyid=$companyid'>add</a>&nbsp;" .
        "<a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&companyid=$companyid'>more</a></div>";
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function superadmin_comprogress_panel($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = 'Progress (super admin only)';
    $content = '';
    $time = time();
    if ($op == null || $op == 'view') {
        $view = sget($_REQUEST, 'view');
        if (empty($view)) {
            $sql = "select * from maenna_company_data where companyid = %d and data_type = 'progress' and deleted != 1 order by dataid desc";
            $result = db_query($sql, array($companyid));
            while (($Row = db_fetch_object($result)) !== false) {
                $subject = htmlentities($Row->data_value, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                $text = nl2br(htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                $dataid = $Row->dataid;
                $followup = $Row->data_attr;
                if ($followup) $followup = "(follwup: " . date('m/d/Y', $followup) . ")";
                $access = date('m/d/Y', $Row->access);
                $content .= "\n<div style='position:relative;border-bottom:solid 1px #ebebeb;'>" .
                    "<div style='position:absolute;height:20px;width:50px;top:8px;right:10px'>" .
                    "\n<a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&view=edit&dataid=$dataid&companyid=$companyid' class=edit-icon>edit</a>" .
                    "\n<a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&do=delete&update_section=superadmin_comprogress_panel&dataid=$dataid&companyid=$companyid' class='delete-icon' onclick='return confirm(\"Continue to remove the record\")'>delete</a>" .
                    "</div>" .
                    "\n<label><b>$access - $subject $followup </b></labe><p>$text</p></div>";
            }
        } elseif ($view == 'add' || $view == 'edit') {
            $subject = $detail = $followup = '';
            $dataid = sget($_REQUEST, 'dataid');
            if ($dataid) {
                $sql = "select * from maenna_company_data where dataid = %d and data_type = 'progress' and deleted != 1 limit 1";
                $result = db_query($sql, array($dataid));
                $Row = db_fetch_object($result);
                if ($Row) {
                    $detail = $Row->data_value2;
                    $subject = $Row->data_value;
                    $followup = date('m/d/Y', $Row->access);
                }
            }
            $content .= <<< END
            <form method='post' action='/account' onsubmit='return check_input();'>
                <table class='edit_table'>
                    <tr><td>Subject</td><td><input type=text name=subject value='$subject' style='width:500px;'> </td></tr>
                    <tr><td>Detail</td><td><textarea name='detail' style='width:500px;height:240px;'>$detail</textarea></td></tr>
                    <tr><td>Followup</td><td><input type='text' name=followup value='$followup' class='datepicker' /></td></tr>
                    <tr><td></td><td><input type=submit value='submit' class=button ></td></tr>
                </table>
                <input type=hidden name=a  value=companies >
                <input type=hidden name=type  value=company_detail >
                <input type=hidden name=panel  value=superadmin_comprogress_panel >
                <input type=hidden name=update_section  value='superadmin_comprogress_panel' >
                <input type=hidden name=companyid  value='$companyid'>
                <input type=hidden name=dataid  value='$dataid'>
            </form>
            <script type='text/javascript'>
            $(document).ready(function(){
                init_datepicker();
            })
            </script>
END;
        }
    } elseif ($op == 'update') {
        $dataid = sget($_REQUEST, 'dataid');
        $subject = sget($_REQUEST, 'subject');
        $detail = sget($_REQUEST, 'detail');
        $followup = sget($_REQUEST, 'followup');
        if ($followup) $followup = strtotime($followup);
        $do = sget($_REQUEST, 'do');
        if (empty($do)) {
            if ($subject) {
                if ($dataid) {
                    $sql = "update maenna_company_data set data_value = '%s',data_value2 = '%s',access='%s',data_attr='%s', editorid=%d where dataid = %d and data_type = 'progress' limit 1";
                    $DBValues = array($subject, $detail, $time, $followup, $editorid, $dataid);
                } else {
                    $sql = "insert into maenna_company_data (companyid, access, data_type,data_attr, data_value,data_value2, editorid) values(%d,'%s','%s','%s','%s','%s',%d)";
                    $DBValues = array($companyid, $time, 'progress', $followup, $subject, $detail, $editorid);
                }
                if (db_query($sql, $DBValues)) {
                    drupal_set_message("the record is updated.");
                } else drupal_set_message("failed to save changes.", 'error');
            }
        } elseif ($do == 'delete') {
            if ($dataid) {
                $sql = "delete from maenna_company_data where data_type = 'progress' and dataid = %d limit 1";
                if (db_query($sql, array($dataid))) {
                    drupal_set_message("the record is removed.");
                } else drupal_set_message("failed to remove record.", 'error');
            }
        }
    }
    $content .= "<div class='align-right edit-link'><a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&view=add&companyid=$companyid'>add</a></div>";
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function minutes($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "MINUTES";
    $content = '';
    global $redirect;
    $data_type = 'minutes';
    $panel = 'multi';
    $section = __FUNCTION__;
    if ($op) {
        /*        $limit = 3;
        $_page = sget($_REQUEST, '_page');
        if (empty($_page)) $_page = 1;
        $start = ($_page - 1) * $limit;*/
        $sql = "select count(*) as total from maenna_company_data where data_type = '$data_type' and companyid = %d and deleted != 1";
        $result = db_query($sql, array($companyid));
        $Row = db_fetch_object($result);
        $total = $Row->total;

        $sql = "select * from maenna_company_data where data_type = '$data_type' and companyid = %d and deleted != 1 order by dataid desc";
        $result = db_query($sql, array($companyid));
        while ($Row = db_fetch_object($result)) {
            $access = date('m/d/Y', $Row->access);
            $title = htmlentities(ucwords($Row->data_value), ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $text = nl2br(htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid&section=$section";
            $content .= "<div class=entry>";
            if ($op == 'write' || ($op == 'edit' && $user->uid == $_REQUEST['id'])) $content .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid&section=$section' class='tool' >Edit</a></div>";
            $content .= "<a href='" . $more_link . "'><div class='entry-title'>$title &nbsp;</div></a><div class='entry-content'>" . _filter_autop(html_entity_decode($text)) . "</div></div><hr class='line' />";
        }
        /*$Pagenation = array(
            'total'        => $total,
            'limit'        => $limit,
            'baseurl'      => '/account?',
            'num_of_links' => 8,
        );
        $content .= pagination($Pagenation);*/
    }
    if ($op == 'write' || ($op == 'edit' && $user->uid == $_REQUEST['id'])) $Block['title'] .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=add&datatype=${data_type}&section=$section' class='tool' >Add</a></div>";
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function mgmt_analysis($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "MANAGEMENT DISCUSSION & ANALYSIS";
    $panel = "multi";
    $data_type = 'mgmtanalysis';
    global $redirect;
    $content = '';
    if ($op) {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type' and deleted != 1 order by dataid desc limit 2";
        $result = db_query($sql, array($companyid));
        $counter = 0;
        while (($Row = db_fetch_object($result)) !== false) {
            $counter++;
            $title = htmlentities(ucwords($Row->data_value), ENT_QUOTES | ENT_IGNORE, "UTF-8");
            if (empty($title)) $title = "&nbsp;";
            $text = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
            $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
            $last = '';
            $content .= "\n<div class='entry'>
                            \n<div class=entry-title>$title" .
                " </div>
                 \n<div class=entry-content>" . _filter_autop(html_entity_decode($text)) . "</div>
                        \n</div>";
        }
        $add_link = $redirect . "&panel=${panel}&view=add&section=" . __FUNCTION__;
        $viewall_link = $redirect . "&panel=${panel}&view=listview&section=" . __FUNCTION__;
        if ($op == 'write' || ($op == 'edit' && $user->uid == $_REQUEST['id'])) $Block['title'] .= "<div class=editbtn><a href='$add_link' class=tool>Add</a></div>";
        $content .= "\n<div class=\"viewallbtn morebtn-content\"><a href='$viewall_link' class=tool>More</a></div>";
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function financial_data($op = null)
{
    global $user, $AccessObj;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "FINANCIAL DATA";
    $tab = sget($_REQUEST, 'tab');
    $panel = 'financial_data_panel';
    $data_type = 'financial';
    $section = __FUNCTION__;
    global $redirect;
    $content = '';
    // db reference: revenue, earing, equity, working_capt, capex, dc
    if ($op && $op != 'update') {
        $sql = "SELECT * FROM `maenna_company_data` WHERE `companyid` = $companyid AND `data_type` = '$data_type' AND `deleted` != 1 ORDER BY `data_attr` DESC";
        $result = db_query($sql);
        $Data = array();
        $Chart_data = array();
        $content .= '<div id="chart" style="width: 570px; height: 240px"></div>';
        $content .= '<div style="position:relative">
            <div style="position:relative;height:12px;">
                <div class="editbtn morebtn"><a class="tool openclose" boxid="chart_table">More</a></div>
            </div>
            <div style="margin-top:6px;display:none" id="chart_table">
                <table cellspacing="0" cellpadding="0" class="blue-border">
        ';

        while ($Row = db_fetch_array($result)) {
            $year = sget($Row, 'data_attr');
            $revenue = sget($Row, 'data_value', 'int');
            $earning = sget($Row, 'data_value2', 'int');
            $equity = sget($Row, 'data_value3', 'int');
            $working_capt = sget($Row, 'data_value4', 'int');
            $capex = sget($Row, 'data_value5', 'int');
            $dc = sget($Row, 'data_value6');
            if ($earning && $revenue) {
                $earning_p = number_format((($earning / $revenue) * 100), 1) . '%';
            } else {
                $earning_p = '';
            }
            $Data[] = array(
                'year'         => $year,
                'earning_p'    => $earning_p,
                'revenue'      => $revenue,
                'equity'       => ($equity) ? '$' . number_format($equity) : '',
                'earning'      => $earning,
                'working_capt' => ($working_capt) ? '$' . number_format($working_capt) : '',
                'capex'        => ($capex) ? '$' . number_format($capex) : '',
                'dc'           => $dc
            );
        }
        $thisYear = date('Y');
        $Table_rows = array(
            'year'            => array(),
            'Earnings %'      => array(),
            'Revenue Growth'  => array(),
            'Earnings Growth' => array(),
            'Equity'          => array(),
            'Working Cap.'    => array(),
            'Capex'           => array(),
            'D/C'             => array()
        );
        if (count($Data) == 0) {
            for ($i = 0; $i < 5; $i++) {
                $Table_rows['year'][$i] = $thisYear - $i;
            }
        } else {
            // column by column,
            $This_year = array_shift($Data);
            while (count($Data) > 0) {
                $Past_year = array_shift($Data);
                $Rows = array();
                $Rows[0] = $this_year = $This_year['year'];
                $Rows[1] = $This_year['earning_p'];
                $this_revenue = $This_year['revenue'];
                $this_earning = $This_year['earning'];
                $past_revenue = $Past_year['revenue'];
                $past_earning = $Past_year['earning'];
                $Chart_data[] = array("'$this_year'", $this_revenue, $this_earning);
                if ($this_revenue && $past_revenue) {
                    $Rows[2] = number_format((($this_revenue - $past_revenue) / $past_revenue) * 100, 1) . '%';
                } else {
                    $Rows[2] = '';
                }
                if ($past_earning && $this_earning) {
                    $Rows[3] = number_format((($this_earning - $past_earning) / $past_earning) * 100, 1) . '%';
                } else {
                    $Rows[3] = '';
                }
                $Rows[4] = $This_year['equity'];
                $Rows[5] = $This_year['working_capt'];
                $Rows[6] = $This_year['capex'];
                $Rows[7] = $This_year['dc'];
                $m = 0;
                foreach ($Table_rows as &$Column) {
                    array_push($Column, $Rows[$m]);
                    $m++;
                }
                //loop
                $This_year = $Past_year;
            }
            $m = 0;
            $Rows = array();
            $Rows[0] = $this_year = $This_year['year'];
            $Rows[1] = $Rows[2] = $Row[3] = '-';
            $Rows[1] = $This_year['earning_p'];
            $Rows[4] = $This_year['equity'];
            $Rows[5] = $This_year['working_capt'];
            $Rows[6] = $This_year['capex'];
            $Rows[7] = $This_year['dc'];
            $Chart_data[] = array("'$this_year'", $This_year['revenue'], $This_year['earning']);
            foreach ($Table_rows as &$Column) {
                array_push($Column, $Rows[$m]);
                $m++;
            }
        }
        foreach ($Table_rows as $title => &$Row) {
            if ($title == 'year') {
                $class = 'align-center bold';
                $title = '';
            } else {
                $class = 'align-right';
            }
            $_has_value = false;
            for ($i = 0; $i < count($Row); $i++) {
                if ((!empty($Row[$i])) && $Row[$i] != '-') {
                    $_has_value = true;
                    break;
                }
            }
            if (!$_has_value) continue;
            $content .= "\n<tr>
                            <td>$title</td>
                            <td style='width:90px;font-size:11px;' class='$class'>" . $Row[0] . "</td>
                            <td style='width:90px;font-size:11px;' class='$class'>" . $Row[1] . "</td>
                            <td style='width:90px;font-size:11px;' class='$class'>" . $Row[2] . "</td>
                            <td style='width:90px;font-size:11px;' class='$class'>" . $Row[3] . "</td>
                            <td style='width:90px;font-size:11px;' class='$class'>" . $Row[4] . "</td>
                        </tr>";
        }
        $content .= "\n</table></div></div>";
        foreach ($Chart_data as &$M) {
            foreach ($M as &$i) {
                if (empty($i)) $i = 0;
            }
            $M = '[' . implode(',', $M) . ']';
        }
        $Chart_data = array_reverse($Chart_data, true);
        $str_chart_data = implode(',', $Chart_data);
        if ($str_chart_data) {
            $AccessObj->loadGoogleChart = true; // load chart api at end of html. coded at end of page-account.tpl.php
            $AccessObj->str_chart_data = $str_chart_data;
        }
    }
    $content .= '<script>$(function () { init_openbox(); });</script>';
    $edit_link = $redirect . "&panel=${panel}&section=$section";
    if ($op == 'write') $Block['title'] .= "<div class=editbtn><a href='$edit_link' class=tool>Edit</a></div>";
    $Block['body'] = content_box($Block['title'], $content);
    //return $Block;
    return financial_data_table($op); // FIXME Dirty hack to get it working
}

function financial_data_table($op = null)
{
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "FINANCIAL DATA";
    $panel = "financial_data_panel";
    $data_type = 'financial';
    $section = __FUNCTION__;
    global $redirect;
    $content = '';
    // db reference: revenue, earing, equity, working_capt, capex, dc
    if ($op && $op != 'update') {
        $sql = "select * from maenna_company_data where companyid = $companyid and data_type = '$data_type' and deleted != 1 order by data_attr desc";
        $result = db_query($sql);
        $Data = array();
        $content .= '
            <div id="chart" style="width: 570px; height: 240px;"></div>
            <div style="position: relative">
            <div style="position:relative;height:12px;">
                <div class="editbtn morebtn"><a class="tool openclose" boxid="chart_table">More</a></div>
            </div>
            <div style="margin-top: 6px; display: none;" id="chart_table">';
        while (($Row = db_fetch_array($result)) !== false) {
            $year = sget($Row, 'data_attr');
            $revenue = sget($Row, 'data_value', 'int');
            $earning = sget($Row, 'data_value2', 'int');
            $equity = sget($Row, 'data_value3', 'int');
            $working_capt = sget($Row, 'data_value4', 'int');
            $capex = sget($Row, 'data_value5', 'int');
            $dc = sget($Row, 'data_value6');

            // New fields
            $sga = sget($Row, 'data_value9');
            $opassets = sget($Row, 'data_value10');

            $Data[] = array(
                'year'         => $year,
                'revenue'      => $revenue,
                'equity'       => $equity,
                'earning'      => $earning,
                'working_capt' => $working_capt,
                'capex'        => $capex,
                'dc'           => $dc,
                'sga'          => $sga,
                'opassets'     => $opassets,
            );
        }
        $columns = array(
            'EBIT %'      => 'earning_p',
            'Rev G%'      => 'rev_g',
            'EBIT G%'     => 'ebit_g',
            'D/E'         => 'd_e', // Debt / Equity
            'ROA'         => 'roa',
            'ROE'         => 'roe',
            'SGA %'       => 'sga_p',
        );

        $content .= '<table cellspacing="0" cellpadding="0" class="blue-border monitoring-values">
        <thead><tr><th></th>';
        foreach ($columns as $column_name => $column) {
            $content .= "<th width='13%'>$column_name</th>\n";
        }
        $content .= '</tr></thead><tbody>';
        $sorted_data = array();
        foreach ($Data as $val) {
            $sorted_data[$val['year']] = $val;
        }
        krsort($sorted_data);

        foreach ($sorted_data as $year => $year_data) {
            $prev_year = $year - 1;
            $prev_year_data = false;
            if (isset($sorted_data[$prev_year])) {
                $prev_year_data = $sorted_data[$prev_year];
            }
            $content .= '<tr>';
            $content .= "<td>$year</td>";
            foreach ($columns as $idx) {
                $year_data[$idx] = '';

                switch ($idx) {
                    case 'earning_p':
                        if ($year_data['revenue'] > 0) {
                            $year_data[$idx] = number_format($year_data['earning'] / $year_data['revenue'] * 100, 1) . '%';
                        }
                        break;

                    case 'rev_g':
                        if ($prev_year_data) {
                            $year_data[$idx] = number_format((($year_data['revenue'] - $prev_year_data['revenue']) / $prev_year_data['revenue']) * 100, 1) . '%';
                        }
                        break;

                    case 'ebit_g':
                        if ($prev_year_data) {
                            $year_data[$idx] = number_format((($year_data['earning'] - $prev_year_data['earning']) / $prev_year_data['earning']) * 100, 1) . '%';
                        }
                        break;

                        // Debt / Equity
                    case 'd_e':
                        if ($year_data['equity'] > 0) {
                            $year_data[$idx] = number_format($year_data['dc'] / $year_data['equity'] * 100, 1) . '%';
                        }
                        break;

                        // EBIT / Op.Assets from last year
                    case 'roa':
                        if ($prev_year_data['opassets'] > 0) {
                            $year_data[$idx] = number_format($year_data['earning'] / $prev_year_data['opassets'] * 100, 1) . '%';
                        }
                        break;

                        // EBIT / Equity last year
                    case 'roe':
                        if ($prev_year_data['equity'] > 0) {
                            $year_data[$idx] = number_format($year_data['earning'] / $prev_year_data['equity'] * 100, 1) . '%';
                        }
                        break;

                        // SGA / Rev
                    case 'sga_p':
                        if ($year_data['revenue'] > 0) {
                            $year_data[$idx] = number_format($year_data['sga'] / $year_data['revenue'] * 100, 1) . '%';
                        }
                        break;

                    case 'revenue':
                    case 'equity':
                        $year_data[$idx] = str_replace(array(',', ' ', '$'), '', $year_data[$idx]);
                        $year_data[$idx] = '$' . number_format($year_data[$idx] / 1000000, 2);
                        break;

                    default:
                }
                $content .= '<td>' . $year_data[$idx] . '</td>';
            }
            $content .= '</tr>';
        }
        $content .= '</tbody>';
        $content .= '</table>';
        $content .= '</div></div>';
    }
    $content .= '<script type="text/javascript">$(function () { init_openbox(); });</script>';
    $edit_link = $redirect . "&panel=$panel&section=$section";
    if ($op == 'write') {
        $Block['title'] .= '<div class="editbtn"><a href="' . $edit_link . '" class="tool">Edit</a></div>';
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function financial_data_panel($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "FINANCIAL DATA ($ in 000)";
    $tab = sget($_REQUEST, 'tab');
    $panel = "financial_data_panel";
    $data_type = 'financial';
    global $redirect;
    $redirect .= "&section=financial_data";
    $HV = hidden_post_values(array('tab', 'page', 'mtab', 'id', 'section'));
    $content = '';

    if ($op == 'title') {
        return $Block['title'];
    }

    if ($op && $op != 'update') {
        $view = sget($_REQUEST, 'view');
        if (empty($view)) {
            // List all the existing data in Table View

            // Add link
            $url_add = $redirect . "&panel=${panel}&view=add";
            $Block['title'] .= "<div class='editbtn'><a href='${url_add}' class='tool'>Add</a></div>";
            $sql = "SELECT * FROM maenna_company_data where data_type = '%s' and companyid = %d and deleted != 1 order by data_attr desc";
            $result = db_query($sql, array($data_type, $companyid));
            $content .= "<table class='report report-monitoring with-border'>
            <thead>
            <tr style='background:#ebebeb'>
                <th width='40'>Year</th>
                <th width='60'>Rev</th>
                <th width='60'>EBIT</th>
                <th width='60'>Equity</th>
                <th width='60'>WC</th>
                <th width='60'>Capex</th>
                <th width='60'>Debt</th>
                <th style=''></th>
            </tr></thead>";

            $content .= '<tbody>';
            while ($Row = db_fetch_array($result)) {
                $year = sget($Row, 'data_attr');

                $dataid = sget($Row, 'dataid');

                // Edit link
                $url_edit = $redirect . "&panel=${panel}&view=edit&dataid=${dataid}";

                // Delete link
                $url_del = $redirect . "&panel=${panel}&do=delete&dataid=${dataid}&update_section=${panel}&section=financial_data' onclick='return confirm(\"Continute to remove the record\")";

                // Revenue
                if ($revenue = sget($Row, 'data_value', 'int')) {
                    $revenue = number_format($revenue);
                }

                // Earnings (named EBIT later)
                if ($earning = sget($Row, 'data_value2', 'int')) {
                    $earning = number_format($earning);
                }

                // Equity
                if ($equity = sget($Row, 'data_value3', 'int')) {
                    $equity = number_format($equity);
                }

                // Capital
                if ($wc = sget($Row, 'data_value4', 'int')) {
                    $wc = number_format($wc);
                }

                // Capex
                if ($capex = sget($Row, 'data_value5', 'int')) {
                    $capex = number_format($capex);
                }

                // D/C (named Debt later)
                if ($debt = sget($Row, 'data_value6', 'int')) {
                    $debt = number_format($debt);
                }

                // NI
                if ($ni = sget($Row, 'data_value7', 'int')) {
                    $ni = number_format($ni);
                }

                // D&A
                if ($da = sget($Row, 'data_value8', 'int')) {
                    $da = number_format($da);
                }

                // SGA
                if ($sga = sget($Row, 'data_value9', 'int')) {
                    $sga = number_format($sga);
                }

                // Op. Assets
                if ($opassets = sget($Row, 'data_value10', 'int')) {
                    $opassets = number_format($opassets);
                }

                $content .= "<tr>
                                <td>$year</td>
                                <td>$revenue</td>
                                <td>$earning</td>
                                <td>$equity</td>
                                <td>$wc</td>
                                <td>$capex</td>
                                <td>$debt</td>
                                <td class='action'><a href='${url_edit}'>edit</a>&nbsp;&nbsp;
                                    <a href='${url_del}'>del</a>
                                </td>
                            </tr>";
            }

            $content .= '</tbody>';
            $content .= "</table>";
        } elseif ($view == 'edit' || $view == 'add') {
            $dataid = sget($_REQUEST, 'dataid');

            $existing_years = [];

            if ($dataid) {
                $sql = "select * from maenna_company_data where dataid = %d and deleted != 1";
                $result = db_query($sql, array($dataid));
                $Row = db_fetch_array($result);
            } else {
                /*
                $sql = "SELECT GROUP_CONCAT(data_attr) as years FROM maenna_company_data where data_type = '%s' and companyid = %d and deleted != 1 order by data_attr desc";
                $result = db_query($sql, array($data_type, $companyid));
                $existing_years_row = db_fetch_array($result);
                $existing_years = explode(',', $existing_years_row['years']);
                */
            }

            if (empty($Row)) {
                $Row = array();
            }

            $year = sget($Row, 'data_attr');
            $revenue = sget($Row, 'data_value', 'int');
            $earning = sget($Row, 'data_value2', 'int');
            $equity = sget($Row, 'data_value3', 'int');
            $capital = sget($Row, 'data_value4', 'int');
            $capex = sget($Row, 'data_value5', 'int');
            $dc = sget($Row, 'data_value6', 'int');
            $ni = sget($Row, 'data_value7', 'int');
            $da = sget($Row, 'data_value8', 'int');
            $sga = sget($Row, 'data_value9', 'int');
            $opassets = sget($Row, 'data_value10', 'int');

            $this_year = date("Y");
            for ($i = $this_year; $i >= ($this_year - 70); $i--) {
                if (!in_array($i, $existing_years)) {
                    $Years["$i"] = $i;
                }
            }

            $option_year = option_code($Years, $year);

            $target_link = request_uri();
            $target_link = str_replace('&view=add', '', $target_link);

            $content .= "
                <form method='POST' action='" . $target_link . "'>
                    <table class='report with-border'>
                        <tr>
                            <td style='width:160px;'>YEAR</td>
                            <td><select name=year><option></option>$option_year</select></td>
                        </tr>
                        <tr>
                            <td style=''>REV($)</td>
                            <td><input type=text name=revenue value='$revenue'></td>
                        </tr>
                        <tr>
                            <td style=''>EBIT($)</td>
                            <td><input type=text name=earning value='$earning' ></td>
                        </tr>
                        <tr>
                            <td style=''>NI($)</td>
                            <td><input type=text name='ni' value='$ni' ></td>
                        </tr>
                        <tr>
                            <td style=''>D&A($)</td>
                            <td><input type=text name='da' value='$da'></td>
                        </tr>
                        <tr>
                            <td style=''>SGA($)</td>
                            <td><input type=text name='sga' value='$sga'></td>
                        </tr>
                        <tr>
                            <td style=''>WORKING CAPITAL($)</td>
                            <td><input type=text name=capital value='$capital'></td>
                        </tr>
                        <tr>
                            <td style=''>CAPEX($)</td>
                            <td><input type=text name=capex value='$capex'></td>
                        </tr>
                        <tr>
                            <td style=''>OP.ASEETS($)</td>
                            <td><input type=text name='opassets' value='$opassets'></td>
                        </tr>
                        <tr>
                            <td style=''>DEBT ($)</td>
                            <td> <input type=text name='dc' value='$dc' ></td>
                        </tr>
                        <tr>
                            <td style=''>EQUITY ($)</td>
                            <td> <input type=text name='equity' value='$equity' ></td>
                        </tr>
                    </table>

                    <div class='row' style='border:none;margin-top:9px;'>
                        <input type='submit' name='submit' value='submit' class='button' />
                        <a href='$redirect&panel=${panel}' class=button>Cancel</a>
                    </div>
                    $HV
                    <input type='hidden' name=panel value='$panel' />
                    <input type='hidden' name=section value='financial_data' />
                    <input type='hidden' name=update_section value='$panel' />
                    <input type=hidden name=do value='update' />
                    <input type=hidden name=dataid value='$dataid' />
                </form>
            ";
        }
    } elseif ($op == 'update') {
        $Correct = false;
        $do = sget($_REQUEST, 'do');
        if ($do == 'update') {
            $DBKeys = array(
                'year'    => 'data_attr',
                'revenue' => 'data_value',
                'earning' => 'data_value2',
                'equity'  => 'data_value3',
                'capital' => 'data_value4',
                'capex'   => 'data_value5',
                'dc'      => 'data_value6',
                'ni'      => 'data_value7',
                'da'      => 'data_value8',
                'sga'      => 'data_value9',
                'opassets' => 'data_value10'
            );
            $DBValues = $SQL_STR_NEW = $SQL_UPDATE = array();
            foreach ($DBKeys as $post_key => $dbkey) {
                $DBValues["$dbkey"] = sget($_REQUEST, $post_key);
                $SQL_STR_NEW[] = "'%s'";
                $SQL_STR_UPDATE[] = "$dbkey = '%s'";
            }
            if (sget($_REQUEST, 'year') == '') {
                drupal_set_message("Please specify a year", 'error');
                return;
            }
            $dataid = sget($_REQUEST, 'dataid');
            if (empty($dataid)) {
                $sql = "INSERT INTO maenna_company_data (companyid, data_type, access, editorid, " . implode(',', $DBKeys) . ") VALUES (%d, '%s', '%s',%d ," . implode(',', $SQL_STR_NEW) . ")";
                array_unshift($DBValues, $companyid, $data_type, $time, $editorid);
            } else {
                $sql = "UPDATE maenna_company_data SET  " . implode(',', $SQL_STR_UPDATE) . " , editorid = %d, access = '%s' where dataid = %d limit 1";
                array_push($DBValues, $editorid, $time, $dataid);
            }
            $Correct = db_query($sql, $DBValues);
        } elseif ($do = 'delete') {
            $dataid = sget($_REQUEST, 'dataid');
            if ($dataid) {
                $sql = "UPDATE maenna_company_data SET deleted = 1 WHERE dataid = %d limit 1";
                $DBValues = array($dataid);
                $Correct = db_query($sql, $DBValues);
            }
        }
        if ($Correct) {
            drupal_set_message("Financial information is updated");
        } else {
            drupal_set_message("Operation failed", 'error');
        }
        return;
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function council_advice($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "COUNCIL ADVICE AND ANALYSIS";
    $panel = "multi";
    $data_type = 'advice';
    global $redirect;
    $content = '';
    if ($op) {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type' and deleted != 1 order by dataid desc limit 2";
        $result = db_query($sql, array($companyid));
        $counter = 0;
        while (($Row = db_fetch_object($result)) !== false) {
            $counter++;
            $title = htmlentities(ucwords($Row->data_value), ENT_QUOTES | ENT_IGNORE, "UTF-8");
            if (empty($title)) $title = "&nbsp;";
            $text = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid&section=" . __FUNCTION__;
            $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid&section=" . __FUNCTION__;
            $text = contentExcerpt($text, '', 1500);
            $last = '';
            $content .= "\n<div class='entry' >
                            \n<div class=entry-title>$title" .
                "</div>
                            \n<div class=entry-content>$text</div>
                        \n</div>";
        }
        $add_link = $redirect . "&panel=${panel}&view=add&section=" . __FUNCTION__;
        $viewall_link = $redirect . "&panel=${panel}&view=listview&section=" . __FUNCTION__;
        if ($op == 'write') {
            $Block['title'] .= "<div class=editbtn>
                                <a href='$add_link' class=tool>Add</a>
                            </div>";
        }
        $content .= "\n<div class=viewallbtn><a href='$viewall_link' class=tool>More</a></div>";
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

/**
 * @param null $op
 * @return mixed
 * highlights nav content for about page
 */
function highlights_content($op= null){
    $db = \Clewed\Db::get_instance();
    $project_id = (int) sget($_REQUEST, 'id');

    $highlightsData = $db->get_array(
        "SELECT cd.dataid as id , cd.data_value as title ,cd.data_value2 as content
                FROM maenna_company_data as cd where cd.data_type = :data_type and cd.companyid = :companyid",
        array(
            ':data_type' => 'highlights',
            ':companyid' => $project_id,
        )
    );
    $count = count($highlightsData);

    $content = "";
    if ($count>0){
        foreach ($highlightsData as $key =>$highlights){
            $active = "";
            if ($key==0){
                $active= 'display:block';
            }
            $contentStyle = '';
            if (strlen($highlights['content']) < 200 && strlen($highlights['content']) > 150){
                $contentStyle = 'align-items: center;justify-content: center;';
            }

            $content .= '
                <div id="tab_'.$highlights["id"].'" class="tabcontent"  style="'.$active.'"  >
                    <div class="abtloop about-highlights">
                        <div class="title-box">
                            <h4>'.$highlights['title'].'</h4>
                        </div>
                        <div class="text-content" style="'.$contentStyle.'">
                          '.$highlights['content'].'
                        </div>
                    </div>
                </div>
            ';
        }
    }
//    Our company is a 30-year old, New York based designer, manufacturer and installer of solid surface solutions. Our products include washroom fixtures (sinks and showers), wall systems (wall claddings), and cabinetry (countertops) for residential and commercial clients. We have built a premium brand from thousands of high profile custom projects we completed for leading institutions and art houses in New York. Recently, the Company has focused on products for healthcare and educational facilities that benefit from the health and environmental attributes of surface materials. Our healthcare furniture have now won design awards for our large buyer. We are seeking to raise working capital to fill orders.
    $Block['body'] = content_box('', $content);
    return $Block;
}

/**
 * @param $company_id
 * @param string $table
 * @return array
 * decides permission for 3 sections (management_discussion ,key_finance,use_of_funds)
 */
function access_information_tables($company_id , $table = '' ){
    global $AccessObj;
    $access = [
        "write"=>false,
        "read"=>false,
    ];

    $userId = sget($_REQUEST,'userid');
    $company = getCompanyByID($company_id);
    if ($company){


        if (($AccessObj->user_type == 'super' && !$userId)){
            $access["write"]= true;
            $access["read"]= true;
        }elseif (!$userId && $AccessObj->user_type == 'company' &&
                  $company->started_fundraising && $company->started_fundraising == "1" &&
                (
                    ($table == 'key_finance' && $company->access_key_finance == "company_edit") ||
                    ($table == 'use_of_funds' && $company->access_use_of_funds == "company_edit") ||
                    ($table == 'management_discussion' && $company->access_management_discussion == "company_edit")
                )
        ){
            $access["write"]= true;
            $access["read"]= true;
        }elseif (
                (
                    (!$userId && $AccessObj->user_type == 'company') ||
                    (($userId || $AccessObj->user_type == 'people') && $company->fundraising && $company->fundraising == "1")
                ) &&
                $company->started_fundraising && $company->started_fundraising == "1" &&
                (
                    ($table == 'key_finance' && $company->access_key_finance == "live") ||
                    ($table == 'use_of_funds' && $company->access_use_of_funds == "live") ||
                    ($table == 'management_discussion' && $company->access_management_discussion == "live")
                )
        ){
            $access["read"]= true;
        }

    }

    return $access;

}