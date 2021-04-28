<?php
/**
 * Created by Ayen Designs.
 */
/**
 * This file contains the Get Started progress bars block.
 * It can later be moved to a Drupal block and an admin setting
 */
global $user;
?>

<script>
    $(document).ready(function() {

        $(".client_proj_name").click(function(event) {

            event.preventDefault();
            $("#proj_name_form").show();

        });
        $(".referral_code").click(function(event) {

            event.preventDefault();
            $("#referral_form").show();

        });

        var availableTags = [
<?php

            $q = mysql_query ("SELECT DISTINCT projname FROM maenna_company where projname is not null") or die(mysql_error());
            while ($row = mysql_fetch_array($q)) {

                echo '"'.strtolower($row['projname']).'",';
            }
?>
        ];
        $( "#proj_name" ).autocomplete({
            source: availableTags
        });

        $("#proj_name_submit").click(function() {

            var proj_name = $("#proj_name").val();
            var uid = '<?=$_REQUEST['id'];?>';
            var editor = '<?=$user->uid;?>';
            if(proj_name == null || proj_name == ''){

                alert('Insert project name!');
                return;
            }

            if ($.inArray(proj_name.toLowerCase(), availableTags) == -1) {

                alert('There is no company with inserted Project name!');
                return;

            }

            $.post("/themes/maennaco/includes/collaborator.php?type=clientRequest",{proj_name: proj_name, uid: uid,editor:editor},

                function(response){
                    if (response != 'success') {alert('Your request did not succeed.Please try again later!'); $("#proj_name_form").hide(); return;}
                    else { $("#proj_name_form").hide();  return; }

            });





        });
        $("#referral_submit").click(function() {

            var ref_code = $("#referral_code").val();
            var uid = '<?=$_REQUEST['id'];?>';
            var utype = '<?=$_REQUEST['tab'];?>';
            if(ref_code == null || ref_code == ''){

                alert('Insert Invitation Code!');
                return;
            }

            $.post("/themes/maennaco/includes/posts.php?type=add_invitation_code",{ref_code: ref_code, uid: uid, utype: utype},

                function(response){
                    if (response != 'success') {alert('Your request did not succeed.Please try again later!'); $("#referral_form").hide(); return;}
                    else {
                        $("#referral_form").hide();
                        obj = $("a.referral_code").find("div#get-started-bar-wrapper");
                        obj.find("div.get-started-progress").css("width","100%");
                        obj.parent().find("div.get-started-check-box").removeClass('no-check-mark');
                        obj.parent().find("div.get-started-check-box").addClass('check-mark');
                        alert("Successfully added referral code");
                        return; }

                });





        });

    });
</script>
    <?php

function get_started() {
    global $user,$AccessObj;
    //if ($_REQUEST['id'] != $user->uid) return '';
  // this array contains the label and a callback function for each progress bar
    if ($_REQUEST['tab'] == 'companies'){

        $get_started_events = array(
            array(
              'label' => 'Change company photo',
              'callback' => 'get_profile_status',
              'link' => 'account?tab=companies&page=company_detail&mtab=about&id='.$_REQUEST['id'].'&view=edit&field=project'
            ),
            array(
              'label' => 'Select a project name',
              'callback' => 'get_project_name_status',
              'link' => 'account?tab=companies&page=company_detail&id='.$_REQUEST['id'].'&section=company_name&panel=edit_company_info'
            ),
            array(
              'label' => 'Add company data',
              'callback' => 'get_basic_info_status',
                'link' => 'account?tab=companies&page=company_detail&id='.$_REQUEST['id'].'&section=company_name&panel=edit_company_info'
            ),
            array(
              'label' => 'Complete diligence Q&A',
              'callback' => 'get_questionnaire_status',
              'link' => 'account?tab=companies&page=company_detail&id='.$_REQUEST['id'].'&mtab=questionnaire'
            ),
/*            array(
                'label' => 'Add Invitation Code',
                'callback' => 'get_referral_status',
                'link' => '#'
            ),*/
        );

        if(is_projectname_selected()) unset($get_started_events[1]);

   }elseif ($_REQUEST['tab'] == 'professionals') {
       $rid = getRoleId($_REQUEST['id']);
       $get_started_events = array(
           array(
               'label' => 'Edit your profile summary',
               'callback' => 'get_pro_form_status',
               'link' => 'account?tab=professionals&page=pro_detail&id='.$_REQUEST['id'].'&panel=pro_profile_panel&view=edit'
           ),
           array(
               'label' => 'Edit your expertise',
               'callback' => 'get_pro_expertise_status',
               'tooltip_callback' => 'get_pro_expertise_status_tooltip',
               'link' => 'account?tab=professionals&page=pro_detail&id='.$_REQUEST['id'].'&panel=profilecontent&view=edit&section=pro_experties&box=experties'
           ),
/*           array(
               'label' => 'Add invitation code',
               'callback' => 'get_referral_status',
               'link' => '#'
           ),*/
/*           array(
               'label' => 'Finish your form questions',
               'callback' => 'get_pro_form_status',
               'link' => 'account?tab=questionnaire&panel=pro_profile_panel&view=edit'
           ),*/
/*           array(
               'label' => 'Upload files that profile your work ',
               'callback' => 'get_pro_files_status',
               'link' => 'account?tab=professionals&page=pro_detail&id=314&panel=file_and_links&view=add'
           )*/
       );
    }
    if ($rid == 11) array_unshift($get_started_events,
        array(
            'label' => 'Confirm accreditation',
            'callback' => 'get_pro_accreditation_status',
            'link' => 'account?tab=questionnaire&id='.$_REQUEST["id"].'&type=accreditation&view=edit'
        )

    );

/*    if ($rid == 12) array_unshift($get_started_events,
        array(
            'label' => 'Submit project name',
            'callback' => 'get_pro_project_name_status',
            'link' => ''
        )

    );*/

  $output = render_get_started_events($get_started_events);
  return $output;
}

function get_pro_expertise_status() {

    $pro_sql = "select data_value3 as status,data_value2 as expertise from maenna_people_data where pid = %d and data_attr = 'experties' order by access desc LIMIT 1";
    $pro_result = db_query($pro_sql, array($_REQUEST['id']));
    $pro_req = db_fetch_object($pro_result);

    if ($pro_req->expertise == '' && $pro_req->status != 'approved') return 0;
    elseif ($pro_req->expertise != '' && $pro_req->status == 'rejected') return 10;
    elseif ($pro_req->expertise != '' && $pro_req->status == 'saved') return 30;
    elseif ($pro_req->expertise != '' && $pro_req->status == 'submitted') return 50;
    elseif ($pro_req->expertise != '' && $pro_req->status == 'approved') return 100;

}

function get_pro_expertise_status_tooltip($perc) {

    if($perc == 100) {
        return 'Your expertise is approved and visible to companies.';
    } elseif($perc == 50) {
        return 'Your expertise is under admin review.';
    } elseif($perc == 10) {
        return 'Your expertise submission is rejected';
    } elseif($perc == 30) {
        return 'Your expertise is saved but not submitted';
    }
    elseif($perc == 0) {
        return 'Edit your expertise.';
    }


}

function get_referral_status () {

    if ($_REQUEST['tab'] == 'companies')
    {$table = 'maenna_company';$id = 'companyid';}
    else {$table = 'maenna_people';$id = 'pid';}

    $pro_sql = "select referral_code from $table where $id = %d";
    $pro_result = db_query($pro_sql, array($_REQUEST['id']));
    $pro_req = db_fetch_object($pro_result);
    if ($pro_req ->referral_code != '') return 100; else return 0;

}


function get_pro_project_name_status() {

    $pro_sql = "select * from maenna_connections where assignee_uid = %d and conntype= '%s' and status <> '%s' ";
    $pro_result = db_query($pro_sql, array($_REQUEST['id'],'client','deactivated'));
    $pro_req = mysql_num_rows($pro_result);
    if ($pro_req != 0) return 100; else return 0;
}

function is_projectname_selected() {

    $pro_sql = "select * from maenna_company where companyid = %d";
    $pro_result = db_query($pro_sql, array($_REQUEST['id']));
    $pro_Row = db_fetch_object($pro_result);

    return $pro_Row && !empty($pro_Row->projname);
}

function get_pro_form_status() {

    $selectedOptions = getSelectedProPage2 ($_REQUEST['id']);

    $ignore_keys = array('firstname','lastname','email','referral_code','protype','profile','company','plres','discussion_tool');

    $total = 0;//21;

    $completed = 0;

    //test_array($selectedOptions);

    foreach ($selectedOptions as $key => $value) {

        if (!in_array($key,$ignore_keys)) {

            if ($value != '') $completed++;
            $total++;

        }


    }
    //echo $completed.$total;
    //test_array($selectedOptions);
    return $completed*100/$total;
    //die(test_array($selectedOptions));

}
function get_pro_files_status() {

    $sql = "select * from maenna_people_data where pid = %d and data_type = 'proselectwork'";
    $result = db_query($sql,array($_REQUEST['id']));

    $files = mysql_num_rows($result);


   if ($files == 0) return 0;
    else return 100;
}
function get_profile_status() {
    $sql = "select * from maenna_about where project_id = %d";
    $result = db_query($sql,array($_REQUEST['id']));

    $row = db_fetch_array($result);
    $total = 2; // was 5
    $answered = 0;

    foreach ($row as $key => $value) {
      if( ($key == 'project' || $key == 'mission') && !empty($value) ) {
        $answered++;
      }
    }
    $perc = $answered*100/$total;
    //Hardcode - currently image is loaded by default so we always have 100% result and check mark
    return 100;
    return $perc;
}

function get_professional_profile_status() {
    $submitted = 0;
    $approved = 0;
    $rejected = 0;
    $saved = 0;

    $sql_access = "select COUNT(*) as count from maenna_people_data where data_attr IN ('experties', 'industryview', 'mgmtview') and pid = %d and data_value3 = 'approved' and data_type = 'addinfo'";

    $result1 = db_query($sql_access,array($_REQUEST['id']));
    $Row1 = db_fetch_object($result1);

    $sql = "select mp.data_value3 as status from maenna_people_data mp inner join (select * FROM maenna_people_data WHERE pid = %d AND data_type = 'addinfo' AND data_attr IN ('experties', 'industryview', 'mgmtview')  order by access desc) a on mp.dataid = a.dataid group by mp.data_attr";

    $result = db_query($sql,array($_REQUEST['id']));

    while ($row = db_fetch_object($result)) {
        $status = $row->status;
        if($status == 'submitted') {
            $submitted++;
        } elseif($status == 'rejected') {
            $rejected++;
        } elseif($status == 'approved') {
            $approved++;
        }
        elseif($status == 'saved') {
            $saved++;
        }

    }

    $perc = 0;
    if($submitted > 0) $perc = 50;
    if($approved > 0 && $submitted == 0 && $rejected == 0 && $saved == 0 ) $perc = 100;
    if($rejected >0) $perc = 0.1;
    if($saved >0) $perc = 0.2;
    if ($Row1->count >= 3 && ($submitted != 0 || $rejected != 0 || $saved != 0)) $perc = 100.1;
    return $perc;
}

function get_professional_profile_status_tooltip($perc) {
    if($perc == 100) {
        return 'Profile is approved and visible to companies.';
    } elseif($perc == 50) {
        return 'Profile is under admin review.';
    } elseif($perc == 0.1) {
        return 'Your profile submission is rejected';
    } elseif($perc == 0.2) {
        return 'Your profile is saved but not submitted';
    } elseif($perc == 100.1) {
        $sql = "select access as updated,data_value3 as status from maenna_people_data where dataid = (select dataid as updated from maenna_people_data where pid = %d and data_type = 'addinfo' AND data_attr IN ('experties', 'industryview', 'mgmtview') and data_value3 <> 'approved' order by access desc  LIMIT 1)";
        $result = db_query($sql,array($_REQUEST['id']));
        $Row = db_fetch_object($result);

        return 'Update '.$Row->status.' on '.date("F d Y @ h:i:s A",$Row->updated);
    }
    else {
        return 'Please complete your profile page.';
    }
}

function get_project_name_status() {
  $sql = "select projname from maenna_company where companyid = %d";
  $result = db_query($sql,array($_REQUEST['id']));
  $row = db_fetch_object($result);

  if ($row->projname == '') return 0;
  else return 100;

}
function get_basic_info_status() {
    $selectedOptions = getSelectedCompanyPage2($_REQUEST['id']);
    //die(test_array($selectedOptions));

    $total = 23;
    $completed = 0;
    foreach ($selectedOptions as $key => $value) {
        if ($key == '0' || $key == '1') {
            foreach($value as $key1 => $value1) {
                if ($key1 == 'data_value' || $key1 == 'data_value2' || $key1 == 'data_value3'){
                    if (!empty($value1)) $completed++;
              }
            }

        }
        else {
            if (!empty($value)) $completed++;
        }
    }

    return $completed*100/$total;

}
function get_questionnaire_status() {


    if ($_REQUEST['tab'] == 'companies') {
    //Get membership of the company whose questionaire is viewed

    $q = "SELECT membership FROM maenna_company WHERE companyid = %d";
    $cmp_type_tmp = db_query($q,array($_REQUEST['id']));
    $cmp_type = db_fetch_object($cmp_type_tmp);
    $cmp_type = $cmp_type->membership;

        $sql = "select  *
                from    maenna_questionair
                where   status = 1 and
                        target='company' and usertype='%s'
                        and parentid is null
                order by recordid asc";
        $result = db_query($sql,array($cmp_type));
        $Row = db_fetch_object($result);
    }
    else {
        $query = "SELECT rid FROM users_roles WHERE uid = %d ";
        $result = db_query($query,array($_REQUEST['id']));
        $row1 = db_fetch_object($result);

        $user_type = $row1->rid;

        $sql = "select  *
                from    maenna_questionair
                where   status = 1 and
                        target='pro' and usertype= %d and
                        parentid is null
                order by recordid asc";
        $result = db_query($sql,array($user_type));
        $Row = db_fetch_object($result);

    }
       $questionnaireid = $Row->recordid;

    $sql = "select count(*) as total FROM maenna_questionair where parentid = %d and (type = 'question' or type = 'fileuploader')";
    $result = db_query($sql,array($questionnaireid));


    $row1 = db_fetch_object($result);
    $total = $row1->total;
    $sql = "select Q.*, A.answer from maenna_questionair as Q
                    left join maenna_qa as A
                        on Q.recordid = A.qid and A.uid = %d
                    where parentid = %d and ((type = 'question') or (type = 'fileuploader'))";
    $result = db_query($sql, array($_REQUEST['id'], $questionnaireid));
    $completed = 0;
    while($Row = db_fetch_object($result))
    {
        if(!empty($Row->answer)) $completed++;
    }

    if (!$total) {
        return 0;
    }

    return $completed*100/$total;
  }
function render_get_started_events($events) {
    global $user, $AccessObj;

    $sql = "select public FROM maenna_company where companyid = %d";
    $result = db_query($sql,array($_REQUEST['id']));
    $Row = db_fetch_array($result);

    if('people' == $AccessObj->user_type && $user->uid != $_REQUEST['id'])
        return '';

    if (call_user_func('get_pro_form_status') >= 100)
        return '';

    if ($Row['public'] != '1') {
        if ($_REQUEST['tab'] == "companies")
            $block_title = 'Start service or fundraising <a data-tooltip="Click to expand/collapse" class="collapse-icon" style="margin-top:12px; padding-left: 3px; float:right; line-height: 20px;" data-for="#get-started-wrapper" style="float: right">+</a>';
        else if ($_REQUEST['tab'] == "professionals")
            $block_title = 'Getting Started <a data-tooltip="Click to expand/collapse" class="collapse-icon" style="margin-top:12px; padding-left: 3px; float:right; line-height: 20px;" data-for="#get-started-wrapper" style="float: right">+</a>';
        $gettingStartedTooltip = "Click on each topic below to complete your information to get started.";
        $output = '<div class="sidebar_box started_sidebar_box"><div class="wrapper"><div class="box_title"><span data-tooltip="' . $gettingStartedTooltip . '" style="font-size:14px;text-transform:capitalize !important;cursor: pointer;">' . $block_title . '</span></div></div><div style="display: none;" id="get-started-wrapper">';

        if (!$events || !is_array($events) || sizeof($events) == 0) return FALSE;
        foreach ($events as $event) {
            if ($event['callback'] == 'get_pro_project_name_status') $js_fun = 'class = "client_proj_name"'; else $js_fun = '';
            if ($event['callback'] == 'get_referral_status') $js_fun = 'class = "referral_code"'; else $js_fun = '';
            $value = call_user_func($event['callback']);

            $tooltip_message = '';
            if ($event['tooltip_callback']) {
                $tooltip_message = call_user_func($event['tooltip_callback'], $value);
            }

            $output .= '<a ' . $js_fun . ' href="' . $event['link'] . '"' . ($tooltip_message ? " data-tooltip='$tooltip_message'" : '') . '><div class="get-started-event">' . $event['label'] . '</div>';
            $output .= '<div id="get-started-bar-wrapper"><div class="get-started-bar"></div>';
            $output .= '<div class="get-started-progress" style="width: ' . $value . '%"></div></div>';
            $output .= '<div class="get-started-check-box ' . check_mark($value) . '"></div><div class="clear-fix"></div>';
            $output .= '</a>';

            if ($event['callback'] == 'get_pro_project_name_status') {

                $output .= '<div style="display: none;" id="proj_name_form"><br><input placeholder="Project name of company related" style="width:220px !important;" id="proj_name" type="text">
        <input id="proj_name_submit" style="margin-top:4px; float:right;" type="submit" name="submit" value="Submit" class="button_add"><br></div>';

            }
            if ($event['callback'] == 'get_referral_status') {

                $output .= '<div style="display: none;" id="referral_form"><br><input placeholder="Add Invitation Code" style="width:220px !important;" id="referral_code" type="text">
        <input id="referral_submit" style="margin-top:4px; float:right;" type="submit" name="submit" value="Submit" class="button_add"><br></div>';

            }
        }

        $output .= '</div></div>';
    }
  return $output;
}
function check_mark($value=0) {
  return ($value == 100 || $value == 100.1) ? 'check-mark' : 'no-check-mark';
}
