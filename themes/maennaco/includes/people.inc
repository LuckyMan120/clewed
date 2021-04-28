<?php
defined("__ACCOUNT__") or die("no direct access allowed");
global $maenna_page;
$maenna_page = array();
$maenna_page['left'] = array();
$maenna_page['content'] = array('working_on', 'my_collaborations', 'interested_in');
$maenna_page['right']= array('invite_companies','pro_questionnaire', 'invite_people',  'pro_statistics', 'my_referrals', 'my_research','my_schedule');

$maenna_page['Tabs'] = array(
                            // 'Activities' =>    '/account?a=activities',
                             'questionnaire' => '/account?a=questionnaire',
                             //'People' =>        '/account?a=people',
                             //'Information' =>   '/account?a=information',
                             );

include ("." . base_path() . path_to_theme() . "/includes/people_rightbar.inc");
include ("." . base_path() . path_to_theme() . "/includes/people_leftbar.inc");

$a = sget($_REQUEST, 'a');
if(empty($a)) $a = 'questionnaire';
if($a == 'questionnaire') 
{
    mybreadcrumb(2, "Questionnaire", "/account?a=questionnaire");
    include ("." . base_path() . path_to_theme() . "/includes/people_questionnaire.inc");
    $maenna_page['tab']= 'questionnaire';
    $maenna_page['left'] = array( 'people_info'); //, 'diligence');
    $maenna_page['right'] = array('questionnaire_fileuploader');
    $maenna_page['content'] = array('questionnaire');

}elseif($a == 'activities'){
    mybreadcrumb(2, "Activities", "/account?a=activities");
    $maenna_page['tab']= 'Activities';
    $maenna_page['left'] =  array();
    $maenna_page['right'] = array();
    $maenna_page['content'] = array();
}elseif($a == 'people'){
    mybreadcrumb(2, "People", "/account?a=people");
    $maenna_page['tab']= 'Activities';
    $maenna_page['left'] =  array();
    $maenna_page['right'] = array();
    $maenna_page['content'] = array();
}elseif($a == 'information'){
    mybreadcrumb(2, "Information", "/account?a=information");
    $maenna_page['tab']= 'Information';
    $maenna_page['left'] =  array();
    $maenna_page['right'] = array();
    $maenna_page['content'] = array();
}

$panel = sget($_REQUEST, 'panel');
if($panel && function_exists($panel)){ // check later: function is allowed
    $maenna_page['content'] = array($panel);    
}


//////////////////////////
function edit_people_info($op = null)
{
    global $user;
    $uid = $user->uid;
    $rid = userRoleId($uid);
    $time = time();
    $Block['title'] = 'Edit Information';
    $Block['title'] .= '<div class="ui-state-default ui-corner-all" title=".ui-icon-close" style="position:absolute;top:9px;right:0;width:17px;border:none;"><a href="javascript:history.go(-1)" ><span class="ui-icon ui-icon-close"></span></a></div>';
    $content = '';
    $a = sget($_REQUEST, 'a');
    if(empty($a))$a = 'questionnaire';
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_people where pid = %d limit 1";
        $result = db_query($sql, array($uid));
        $Row = db_fetch_object($result);
        
        if($Row){
            $yearwork = $Row->yearwork;
            $industry = $Row->industry;
            $expertise = $Row->experties ;
            $pic = $Row->pic;
            $phone = $Row->phone;
            $intro = $Row->brief_intro;
            $city = $Row->city;
            $state = $Row->state;
            $zip = $Row->zip;
            
            $protype = Options_proType($rid, 1);
            $option_protype = Options_proType($rid);
            $option_industry = Options_industry($industry);
            $option_expertise = Options_experties($expertise);
            $option_state = Options_state($state);
            
            
            
            $content = <<< END
                <form method=post action='/account'>
                    <table class='edit_table no-border'>
                        <tr><td style='width:140px;'>Type:</td><td>$protype</td></tr>
                        <tr><td>Industry:</td><td><select name=industry style='width:260px;'><option></option>$option_industry</select></td></tr>
                        <tr><td>Expertise:</td><td><select name=experties  style='width:260px;'><option></option>$option_expertise</select></td></tr>
                        <tr><td>Year started working in the industry :</td><td><input type=text name='yearwork' value='$yearwork' /></td></tr>
                        <tr><td>Introduction:</td><td><textarea name='brief_intro' style='width:400px;height:130px;'>$intro</textarea></td></tr>
                        <tr><td colspan=2>&nbsp;</td></tr>
                        <tr><td>City:</td><td><input type=text name=city value='$city'   /></td></tr>
                        <tr><td>State:</td><td><select name=state >$option_state</select></td></tr>
                        <tr><td>Zip Code:</td><td><input type=text name=zip value='$zip' style='width:60px' /></td></tr>
                        <tr><td colspan=2>&nbsp;</td></tr>
                        <tr><td>Phone:</td><td><input type=text name=phone value='$phone' /></td></tr>
                        <tr><td colspan=2>&nbsp;</td></tr>
                        <tr><td></td><td><input type=submit name=submit value='Submit' class='button' /></td></tr>
                    </table>
                    <input type=hidden name=a value='$a' />
                    <input type=hidden name=panel value='edit_people_info' />
                    <input type=hidden name=update_section value='edit_people_info' />
                </form>
END;
        }
    }elseif($op == 'update')
    {
        $DBKeys = array('phone',
                        'city',
                        'state',
                        'zip',
                        'industry',
                        'experties',
                        'yearwork',
                        'brief_intro');
        $DBValues = $SQL_STR = array();
        foreach($DBKeys as $key)
        {
            $DBValues["$key"] = sget($_REQUEST, $key);
            $SQL_STR["$key"] = "$key = '%s'";
        }
        
        $sql = "update maenna_people set ". implode(',', $SQL_STR) ." where pid = $uid limit 1";
        if(db_query($sql, $DBValues)){
            drupal_set_message("Information is updated");
        }else{
            drupal_set_message("Failed to update information", 'error');
        }
        
        return '';
        
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function pro_statistics($op  = null)
{
    $Block['title'] = "My Statistics";
    if($op == 'view' || $op == '')  $Block['body'] =  sidebar_box($Block['title'], "DSADSA");
    
    return $Block;
}
function working_on($op = null)
{
    global $maenna_page;
    $Block['title'] = 'Working on';
    if($op == 'view'){
        $content = "";
        $html = <<< END
        <table class='data-table'>
            <thead>
            <tr>
                <th>Start Date</th>
                <th>Company</th>
                <th>Sector</th>
                <th>Revenues</th>
                <th>Type</th>
                <th>Status</th>
                <th>Detail</th>
            </tr>
            </thead>
            <tbody>
                $content
            </tbody>
        </table>
END;
        $Block['body'] = content_box($Block['title'], $html);
    }else{
        $Block['body'] = "DSADSA";
        $maenna_page['right'] = array("invite_companies");
    }
    
    return $Block;
}

function my_collaborations($op = null)
{
    $Block['title'] = 'Collaborations';
    if($op == 'view'){
        $content = "DSA";
        $Block['body'] = content_box($Block['title'], $content);
    }
    
    return $Block;
}

function interested_in($op = null)
{
    $Block['title'] = 'Interested In';
    if($op == 'view'){
        $content = "DSA";
        $Block['body'] = content_box($Block['title'], $content);
    }
    
    return $Block;
}


/* EOF */