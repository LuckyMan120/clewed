<?php
defined("__ACCOUNT__") or die("no direct access allowed");
global $maenna_page;

// some functions will fail if these are empty
if (empty($_REQUEST['a'])) $_REQUEST['a'] = 'account';
global $user;
if (empty($_REQUEST['companyid'])) $_REQUEST['companyid'] = $user->uid;

$maenna_page = array();
$maenna_page['left'] = array();
$maenna_page['content'] = array('companyInfo', 'companyToWatch', 'professional_interested_in');
$maenna_page['right']= array('postAPorject','referCompanies', 'inviteProfessionals');
$maenna_page['Tabs'] = array(
                             'My Account' =>    '/account',
                             'Messages' =>      '/account?a=messages',
                             'Professionals' => '/account?a=professionals',
                             'Information' =>   '/account?a=information',
                             );

$a = sget($_REQUEST, 'a');
if ($a == 'account')
{
  $maenna_page['tab']= 'My Account';
  // copied from super.inc company_detail
  // these are modifying $maenna_page
  $maenna_page['section_tab'] ='company_section_tabs';
  include ("." . base_path() . path_to_theme() . "/includes/shared_functions.inc");

  // these have colliding function names with the company files, and do different things
  // so copy functions that don't exist to the corresponding company files
  // include ("." . base_path() . path_to_theme() . "/includes/super_comdetail_leftbar.inc");
  // include ("." . base_path() . path_to_theme() . "/includes/super_comdetail_rightbar.inc");

  include ("." . base_path() . path_to_theme() . "/includes/company_rightbar.inc");
  include ("." . base_path() . path_to_theme() . "/includes/company_leftbar.inc");

  // make a modified copy of this file
  // include ("." . base_path() . path_to_theme() . "/includes/super_comdetail.inc");
  include ("." . base_path() . path_to_theme() . "/includes/company_detail.inc");

}
elseif($a == 'questionnaire') 
{
  // i think this is moot now?
    mybreadcrumb(2, "Questionnaire", "/account?a=questionnaire");
    include ("." . base_path() . path_to_theme() . "/includes/company_questionnaire.inc");
    $maenna_page['tab']= 'questionnaire';
    $maenna_page['left'] = array('company_name', 'diligence');
    $maenna_page['right'] = array('questionnaire_fileuploader');
    $maenna_page['content'] = array('questionnaire');

}elseif($a == 'activities'){
    mybreadcrumb(2, "Activities", "/account?a=activities");
    $maenna_page['tab']= 'Activities';
    $maenna_page['left'] =  array();
    $maenna_page['right'] = array();
    $maenna_page['content'] = array();
}elseif($a == 'professionals'){
    mybreadcrumb(2, "Professionals", "/account?a=professionals");
    $maenna_page['tab']= 'Professionals';
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

function edit_company_info($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    if (!$write) return;

    $companyid = $user->uid;
    $time = time();
    
    $Block['title'] = "Edit Company Information";
    $Block['title'] .= '<div class="ui-state-default ui-corner-all" title=".ui-icon-close" style="position:absolute;top:9px;right:0;width:17px;border:none;"><a href="javascript:history.go(-1)" ><span class="ui-icon ui-icon-close"></span></a></div>';
    
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company where companyid = %d";
        $result = db_query($sql, array($companyid));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $company = $Row->company;
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
            
            $a = sget($_REQUEST, 'a');
            if (empty($a)) $a = 'questionnaire';
            
            $content = <<< END
            <form method=post action='/account'>
                <table class='edit_table no-border'>
                    <!--tr><td style='width:140px;'>Company:</td><td><input type=text name=company value='$company' style='width:300px;' /></td></tr-->
                    <tr><td>Industry:</td><td><select name=sector><option></option>$option_industry</select></td></tr>
                    <tr><td>Year Founded:</td><td><input type=text name='founded' value='$foundedYear' /></td></tr>
                    <tr><td>Number of Employees:</td><td><input type=text name=empnum value='$empnum' /></td></tr>
                    <tr><td>Introduction:</td><td><textarea name='description' style='width:400px;height:130px;'>$description</textarea></td></tr>
                    <tr><td colspan=2>&nbsp;</td></tr>
                    <tr><td>Address:</td><td><input type=text name=address1 value='$address1' /></td></tr>
                    <tr><td>Suite/Room #:</td><td><input type=text name=address2 value='$address2' /> </td></tr>
                    <tr><td>City:</td><td><input type=text name=city value='$city' /></td></tr>
                    <tr><td>State:</td><td><select name=state >$option_state</select></td></tr>
                    <tr><td>Zip Code:</td><td><input type=text name=zip value='$zip' style='width:60px' /></td></tr>
                    <tr><td colspan=2>&nbsp;</td></tr>
                    <tr><td>Phone:</td><td><input type=text name=phone value='$phone' /></td></tr>
                    <tr><td>Website:</td><td><input type=text name=web value='$web' /></td></tr>
                    <tr><td colspan=2>&nbsp;</td></tr>
                    <tr><td></td><td><input type=submit name=submit value='Submit' class='button' /></td></tr>
                </table>
                <input type=hidden name=a value='$a' />
                <input type=hidden name=panel value='edit_company_info' />
                <input type=hidden name=update_section value='edit_company_info' />
            </form>
END;
        }
    }elseif($op == 'update')
    {
        $DBKeys = array(
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
                        'sector'
                        );
        $DBValues = $SQL_STR = array();
        foreach($DBKeys as $key)
        {
            $DBValues["$key"] = sget($_POST, $key);
            $SQL_STR[] = "$key = '%s'";
        }
        $DBValues[] = $companyid;
        $sql = "update maenna_company set ".implode(',', $SQL_STR)." where companyid = %d limit 1";
        if(db_query($sql, $DBValues)){
            drupal_set_message("Company information is updated");
        }else{
            drupal_set_message("Failed to update company information", 'error');
        }
        
        return '';
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

//////////////////////////////////////////////////////////////////////////////////////
/*

if(isset($f) && $f == 'cactivities')
    {
        $page = sget($_REQUEST, 'p');
        if($page == 'edit_company_detail')
        {
            mybreadcrumb(2, "Activity", "/account");
            mybreadcrumb(3, "Company Detail", "/account?a=cactivities&o=edit_company_detail");
            $maenna_page['left'] = array('company_logo', 'company_brief_info', 'company_management', 'company_market','company_competitors', 'company_milestone');
            $maenna_page['content'] = array('company_name', 'company_financial','management_analysis','company_analyst','company_articles');
           // $maenna_page['content'] = array('company_name',);
            $maenna_page['right']= array('company_follower', 'related_companies', 'company_priorities', 'comapny_expansion', 'company_leaders', 'company_resources', 'company_events', 'company_servicehours');
        }
        
        $maenna_page['tab']= 'activities';
    }
*/
function companyToWatch($op = null)
{
    $Block['title'] = 'Companies to watch';
    if($op == 'view' || $op == null){
        $content = "";
        
        $result = 
        
        $table =<<< END
        <div style='position:absolute;right:5px;top:10px;'>Search Companies</div>
    <table class=' withborder report'>
        <tr class='header'>
            <td >Name</td>
            <td style=''>Industry</td>
            <td style=''>Revenues</td>
            <td style=''>Growth</td>
            <td style=''>Earnings</td>
            <td style=''>Growth</td>
            <td>Files</td>
            <td>edit</td>
        </tr>
    </table>
END;
        
        $Block['body'] = content_box($Block['title'], $table);
    }
    
    return $Block;
}
function companyInfo($op = null)
{
    global $user;
    $uid = $user->uid;
    
    if(($result = get_company_row($uid)) !== false)
    {
        $Row = db_fetch_array($result);
        $companyName = sget($Row, 'company');
        $companyId = sget($Row, 'companyid');
        if(($result = getCompanyFianacial($uid)) !== false)
        {
            $Financials = sortFinancials($result);
            $Newer = array();
            $Older = array();
            foreach($Financials as $Item)
            {
                $Older = $Newer;
                $Newer = $Item;
            }
            $revenue1 = sget($Newer,'revenue');
            $earning1 = sget($Newer, 'earning');
            $year1 = sget($Newer, 'year');
            
            $revenue2 = sget($Older, 'revenue');
            $earning2 = sget($Older, 'earning');
            $year2 = sget($Older, 'year');
            if(empty($revenue1) || empty($revenue2))
            {
                $revenueGrowth = 'n/a';
            }else
            {
                $revenueGrowth = number_format(($revenue1 / $revenue2) * 100);
            }
            if(empty($earning1) || empty($earning2))
            {
                $earningGrowth = 'n/a';
            }else
            $earningGrowth = number_format(($earning1 / $earning2) * 100);
            
            $revenue1 = '$' . number_format($revenue1);
            $earning1 = '$' . number_format($earning1);
            $table =<<< END
    <table class=' withborder report'>
        <tr class='header'>
            <td ></td>
            <td style='width:100px;'>Revenues ($year1)</td>
            <td style='width:100px;'>Growth</td>
            <td style='width:100px;'>Earnings ($year1)</td>
            <td style='width:100px;'>Growth</td>
            <td style='width:100px;'>ROE</td>
        </tr>
        <tr>
            <td >$companyName</td>
            <td >$revenue1</td>
            <td >$revenueGrowth%</td>
            <td >$earning1</td>
            <td >$earningGrowth%</td>
            <td>ROE</td>
        </tr>
    </table>
END;
        }else{
            echo __METHOD__ .": operation failed";
        return;
        }
               
    }else{
        echo __METHOD__ .": operation failed";
        return;
    }
    
    $Block['title'] = "<h2 style='color:#00a2bf;font-size:24px;'>Company - Activity</h2>";
    $content = "<h3>". strtoupper($companyName) . "</h3>";
    $content .= $table;
    if($op == 'view' || $op == null){
        
        $Block['body'] = content_box($Block['title'], $content);
    }
    
    return $Block;
}

// DIVIDER
function company_priorities($op = '')
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $uid = $user->uid;
    $Block['title'] =  'Priorities'; 
    if(!$write || $op == 'view' || $op == '') {
        $content = '';
        $sql = "select * from users_data where uid = $uid and data_type = 'priority' order by data_id";
        $result = db_query($sql);
        for($i = 1; $i <= 3; $i++)
        {
            $p = '';
            if(($data = db_fetch_object($result))  !== false)
            {
                $p = check_plain($data->data_value);
            }
            $content .= "${i}. $p<br>";
        }
        
       // $content .= "<div style='text-align:right'><a href='#' class='edit_wnd' rel='edit_company_priorities'>edit</a></div>";
        $Block['body']  =  sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        for($i = 1; $i <= 3; $i++)
        {
            $p = sget($_POST, 'priority_'. $i);
            $recid = (int)sget($_POST, 'recid_'. $i);
            
            if($recid) $sql = "update users_data set data_value = '%s' where uid = $uid and data_id = $recid limit 1";
            else $sql = "insert into users_data (uid, data_type, data_value) values($uid, 'priority', '%s')";
            $sql_data = array($p);
            if(! db_query($sql, $sql_data))
            {
                drupal_set_message("Failed to update company priorities. Please try again later",'error');
                return ;
            }
            
        }
        drupal_set_message("Company Priority info is updated");
    }
    
    return $Block;
}
function company_resources($op = '')
{
    $Block['title'] =  'Advisory Council'; 
    if($op == 'view' || $op == ''){
        $content = '';
        
        $content .= "<b>Monitoring</b><br>".
                    "" .
                    "<b>Sector Research</b><br>".
                    "" .
                    "<b>Management Advice</b><br>".
                    "" ;
        $Block['body']  =  sidebar_box($Block['title'], $content);
    }
    
    return $Block;
}
function company_events($op = '')
{
    $Block['title'] = 'Schedule/Events'; 
    if($op == 'view' || $op == '') {
        $Block['body']  =  sidebar_box($Block['title'], "");
        
    }
    
    return $Block;
}
function company_statistics($op = '')
{
    $Block['title'] = "Statistics";
    if($op == 'view' || $op == '')  {
        $content = '';
        $content = "Professional Requests 10 <br>Company Referral 10<br>people referral 5";
        $Block['body'] =  sidebar_box($Block['title'], $content);
    }
    
    return $Block;
}
function company_servicehours($op = '')
{
    $Block['title'] = "Service Hours";
    if($op == 'view' || $op == '')  {
        $content = '';
        $content = "Total service hours <br>monitoring -10, sector research 4, management advice <br>Remaining hours 35";
        $Block['body'] =  sidebar_box($Block['title'],$content);
    }
    
    return $Block;
}




function professional_interested_in($op = null)
{
    $Block['title'] = 'Professionals To Follow';
    if($op == 'view' || $op == null){
        $content = "";
        $html = <<< END
            <table>
                <thead><tr><th style='width:45%'>Description</th>
                            <th>Sector</th>
                            <th>Yrs</th>
                            <th>Position</th>
                            <th>Detail</th>
                </tr></thead>
            </table>


END;
        $Block['body'] = content_box($Block['title'], $html);
    }
    
    return $Block;
}

function edit_company_detail($op = null)
{   
    $Block['title'] = 'Company Detail';
    if($op == 'view' || $op == null){
        $content = "DSA";
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
/*
function company_name($op = null)
{
    global $user;
    $uid = $user->uid;
    
    $sql = "select company_name,description from users_extend where uid = $uid limit 1";
    $result = db_query($sql);
    $data = db_fetch_object($result);
    $company_name = ucwords($data->company_name);
    $Block['title'] = "<span style='color:#00a2bf;font-size:24px;'>" . check_plain($company_name) . "</span>";
    $desc = ($data->description) ? $data->description : "n/a";
    if($op == 'view' || $op == null)
    {
        $content = "" . check_plain($desc);
        $content .= "<div class='edit-link' style='margin-top:6px;'><a href='#' class='edit_wnd' rel='edit_company_description'>edit</a></div>";
        $Block['body'] = content_box($Block['title'], $content, 'first');
    }elseif($op == 'update')
    {
        $desc = sget($_POST,'description');
        $sql = "update users_extend set description = '%s' where uid = %d limit 1";
        $sql_data = array($desc, $uid);
        if(db_query($sql, $sql_data)) drupal_set_message("Your company description is updated");
        else drupal_set_message("Failed to update information. Please try again later", 'error');
    }
    return $Block;
}
*/

function company_competitors($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $uid = $user->uid;
    
    $Block['title'] = 'Competitors';
    if($op == 'view' || $op == null){
        $content = "DSA";
        if ($write) {
          $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
        }
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
     return $Block;
}
function comapny_expansion($op = null)
{
    global $user;
    $uid = $user->uid;
    
    $Block['title'] = 'Expansion Markets';
    if($op == 'view' || $op == null){
        $content = "DSA";
        //$content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
     return $Block;
}
function company_taggedinformation($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $uid = $user->uid;
    
    $Block['title'] = 'Tagged Information';
    if($op == 'view' || $op == null){
        $content = "links tagged from info tab";
        if ($write) {
          $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
        }
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
     return $Block;
}

function company_leaders($op = null)
{
    global $user;
    $uid = $user->uid;
    
    $Block['title'] = 'Leaders';
    if($op == 'view' || $op == null){
        $content = "company leaders";
        //$content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
     return $Block;
}




function company_articles($op = null){
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $uid = $user->uid;
    
    $Block['title'] = 'Relevant Information and Articles';
    if(!$write || $op == 'view' || $op == null){
        
        
        
        $content = "DSA";
        if ($write) {
          $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
        }
        $Block['body'] = content_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        $thisYear = date("Y");
        $Years = array();
        for($i = 0; $i < 5 ; $i++)
        {
            $theYear = $thisYear - $i;
            $revenue = sget($_POST,'revenue_'. $theYear,'decimal');
            $earnings = sget($_POST,'earnings_'. $theYear,'decimal');
            $recid = (int)sget($_POST,'recid_'.$theYear);
            if($recid > 0){
                $sql = "update users_data set data_value = '%s', data_value2 = '%s' where uid = $uid and data_type = 'financial' and data_attr = '$theYear'";
                $sql_data = array($revenue, $earnings);
            }else{
                $sql = "insert into users_data (uid, data_type, data_attr, data_value, data_value2) values ($uid, 'financial', '$theYear', '%s','%s')";
                $sql_data = array($revenue, $earnings);
            }
            if(db_query($sql, $sql_data) === false)
            {
                //echo $sql,
                //test_array($sql_data);
                //echo mysql_error();
                drupal_set_message("Faild to update company finacial information. Please try again later",'error');
                return;
            }
        }
        drupal_set_message("Your company financial information is updated");
    }
    return $Block;
}
function company_financial($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $uid = $user->uid;
    
    $Block['title'] = 'Company Financials';
    if(!$write || $op == 'view' || $op == null){
        
        
        
        
        include_once '/var/www/vhosts/clewed.com/httpdocs/php-ofc-library/open_flash_chart_object.php';
        $content = "<div style='position:relative;z-index:2;' id='financial_chart'>";
        $content .= open_flash_chart_object( 500, 250, 'http://'. $_SERVER['SERVER_NAME'] .'/open_chart?companyid=' . $uid , false );
        $content .= "</div>";
        if ($write) {
          $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
        }
        $Block['body'] = content_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        $thisYear = date("Y");
        $Years = array();
        for($i = 0; $i < 5 ; $i++)
        {
            $theYear = $thisYear - $i;
            $revenue = sget($_POST,'revenue_'. $theYear,'decimal');
            $earnings = sget($_POST,'earnings_'. $theYear,'decimal');
            $recid = (int)sget($_POST,'recid_'.$theYear);
            if($recid > 0){
                $sql = "update users_data set data_value = '%s', data_value2 = '%s' where uid = $uid and data_type = 'financial' and data_attr = '$theYear'";
                $sql_data = array($revenue, $earnings);
            }else{
                $sql = "insert into users_data (uid, data_type, data_attr, data_value, data_value2) values ($uid, 'financial', '$theYear', '%s','%s')";
                $sql_data = array($revenue, $earnings);
            }
            if(db_query($sql, $sql_data) === false)
            {
                //echo $sql,
                //test_array($sql_data);
                //echo mysql_error();
                drupal_set_message("Faild to update company finacial information. Please try again later",'error');
                return;
            }
        }
        drupal_set_message("Your company financial information is updated");
    }
    return $Block;
}

function company_risks($op = null)
{
    $Block['title'] = 'Company Risks';
    if($op == 'view' || $op == null){
        $content = "DSA";
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function company_weakness($op = null)
{
    $Block['title'] = 'Company Weakness';
    if($op == 'view' || $op == null){
        $content = "DSA";
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function company_analyst($op = null)
{
    $Block['title'] = 'Advisory Council Discussion';
    if($op == 'view' || $op == null){
        $content = "";
        
        
        
        $content .= "<b>Strengths</b><br>" .
                "" .
                "<b>Weaknesses</b><br>" .
                "" .
                "<b>Opportunities</b><br>" .
                "" .
                "<b>Risks</b><br>" .
                "" 
               ;
        
        
        $Block['body'] = content_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        // no update for company users
        global $user, $msg;
        $uid = $user->uid;
        test_array($_POST);
    }
    return $Block;
}


function company_logo($op = null)
{
    global $user, $msg;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $uid = $user->uid;
    $Block['title'] = 'Logo';
    if(!$write || $op == 'view' || $op == null){
        $sql = "select * from users_data where uid = $uid and data_type = 'logo'";
        $result = db_query( $sql );
        if( db_affected_rows($result) > 0 )
        {
            $Data = db_fetch_object($result);
            $img = $Data->data_value;
            $src = "/" . file_directory_path() . "/" . $img;
            //echo $src;
            $content = "<div class='logo'><img src='$src' style='width:130px;'></div>";
            if ($write) {
              $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='update_company_logo'>edit</a></div>";
            }
        }else{
            if ($write) {
              $content = "<div class='edit-link'><a href='#' class='edit_wnd' rel='company_logo'>upload a logo</a></div>";
            }
        }
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
       
            if ($_FILES["logo"]["error"] > 0)
            {
                drupal_set_message( $_FILES["logo"]["error"]);
            }
            else
            {
                $path = "./" . file_directory_path() . "/";
                $filename = "";
                if($ext = get_filetype($_FILES["logo"]["name"]))
                {
                    $filename = ranStr() . "." . $ext;
                    if(move_uploaded_file($_FILES["logo"]["tmp_name"], $path . $filename))
                    {
                        drupal_set_message("Your new logo has been submitted.");
                        $recid = sget($_REQUEST,'recid');
                        if($recid)
                        {
                            $sql = "update users_data set data_value = '$filename' where data_id=$recid and uid=$uid";
                            db_query($sql);
                        }else{
                            $sql = "insert into users_data (uid, data_type, data_value) values ($uid, 'logo', '$filename')";
                            db_query($sql);
                        }
                    }else{
                        drupal_set_message("Failed to upload new file",'error');
                    }
                }else{
                    drupal_set_message("The file type is not allowed",'error');
                }
            }
    }
    return $Block;
}
function company_brief_info($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $uid = $user->uid;
    $Block['title'] = 'Company Info';
    if(!$write || $op == 'view' || $op == null){
        if ($write) {
          $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_breif_info'>edit</a></div>";
        }
        
        
        $sql = "select * from users_extend where uid = $uid";
        $result = db_query($sql);
        $Data = db_fetch_object($result);
   
        $phone = ! empty($Data->phone) ? $Data->phone : 'n/a';
        $street1 =  ! empty($Data->street1) ? $Data->street1 : '';
        $street2 =  ! empty($Data->street2) ? ", " . $Data->street2 : '';
        $city =  ! empty($Data->city) ? "<br>" . $Data->city : 'n/a';
        $state =  ! empty($Data->state) ? ", " . $Data->state : 'n/a';
        $website =  ! empty($Data->website) ? $Data->website : 'n/a';
        
        
        $founded = 'n/a';$foundedid ="";
        $numOfEmp = 'n/a';$numOfEmpid = "";
        
        $sql = "select * from users_data where uid = $uid and data_type = 'numofemp'";
        $result = db_query($sql);
        $Data = db_fetch_object($result);
        if($Data){
            $numOfEmp = $Data->data_value;
            if($numOfEmp)$numOfEmp = number_format($numOfEmp);
            $numOfEmpid = $Data->data_id;
        }
        
        $sql = "select * from users_data where uid = $uid and data_type = 'founded'";
        $result = db_query($sql);
        $Data = db_fetch_object($result);
        if($Data){
            $founded = $Data->data_value;
            $foundedid = $Data->data_id;
        }
$content .= <<< END
    <div class=row>Founded Year: $founded</div>
    <div class=row>Employees: $numOfEmp</div>
    <div class=row>$street1 $street2 $city $state</div>
    <div class=row>Phone: $phone</div>
    <div class='row last'>Website: $website</div>
    
END;

        
        
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        $founded = sget($_POST,'founded');
        $numOfEmp = sget($_POST,'numofemp');
        $street1 = sget($_POST,'street1');
        $street2 = sget($_POST,'street2');
        $city = sget($_POST,'city');
        $state = sget($_POST,'state');
        $website = sget($_POST,'website');
        $phone = sget($_POST,'phone');
        
        if($numOfEmp) $numOfEmp = preg_replace("/[^0-9]/",'', $numOfEmp);
        if($phone) $phone = preg_replace("/[^0-9]/",'', $phone);
        
        $sql = "update users_extend set phone = '%s', website = '%s', street1='%s', street2 = '%s', city='%s', state='%s' where uid = $uid limit 1";
        db_query($sql, array($phone, $website, $street1, $street2, $city, $state));
        
        $fid = sget($_POST,'fid');
        $sql_data = array($founded, $uid, 'founded');
        if($fid) $sql = "update users_data set data_value = '%s' where uid = %d and data_type = '%s' limit 1";
        else $sql = "insert into users_data (data_value, uid, data_type ) values('%s', %d,  '%s')";
        db_query($sql, $sql_data);
        
        $noeid = sget($_POST,'noeid');
        $sql_data = array($numOfEmp, $uid, 'numofemp');
        if($noeid) $sql = "update users_data set data_value = '%s' where uid = %d and data_type = '%s' limit 1";
        else $sql = "insert into users_data (data_value, uid, data_type ) values('%s',%d,  '%s')";
        db_query($sql, $sql_data);
        drupal_set_message("Your company information is updated");
        //$_SESSION['messages']['status'] = 'Cour company information is updated';
        
        //drupal_goto("account","a=edit_company_detail");
    }
    return $Block;
}
function company_follower($op = null)
{
    $Block['title'] = 'Followers';
    if($op == 'view' || $op == null){
        $content = "n/a";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}
function management_analysis($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $uid = $user->uid;
    $Block['title'] = 'Management Discussion and Analysis';
    if(!$write || $op == 'view' || $op == null){
        $content = "<ul class=''>";
        
        $sql = "select * from users_data where uid = $uid and data_type = 'management' order by data_attr";
        $result = db_query($sql);

        for($i = 1; $i <= 3; $i++)
        {
            $row = db_fetch_object($result);
            $position = '';
            $name = '';
            if($row !== false)
            {
                $name = ucwords(check_plain($row->data_value));
                $position = ucwords(check_plain($row->data_value2));
            }
            $content .= "<li>$name <span class='note'>($position)</span></li>";
        }
        $content .= "</ul>";
        if ($write) {
          $content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_company_management'>edit</a></div>";
        }
        $Block['body'] = content_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        
        if($flag) drupal_set_message("Company management information is updated.");
        else{drupal_set_message("Failed to update information. Please try again later", 'error');}
    }
    return $Block;
}
function company_management($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $uid = $user->uid;
    $Block['title'] = 'Management';
    if(!$write || $op == 'view' || $op == null){
        $content = "<ul class=''>";
        
        $sql = "select * from users_data where uid = $uid and data_type = 'management' order by data_attr";
        $result = db_query($sql);

        for($i = 1; $i <= 3; $i++)
        {
            $row = db_fetch_object($result);
            $position = '';
            $name = '';
            if($row !== false)
            {
                $name = ucwords(check_plain($row->data_value));
                $position = ucwords(check_plain($row->data_value2));
            }
            $content .= "<li>$name <span class='note'>($position)</span></li>";
        }
        $content .= "</ul>";
        if ($write) {
          $content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_company_management'>edit</a></div>";
        }
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        $position1 = sget($_POST,'position1');
        $name1 = sget($_POST,'name1');
        $recid1 = sget($_POST,'recid1');
        $position2 = sget($_POST,'position2');
        $name2 = sget($_POST,'name2');
        $recid2 = sget($_POST,'recid2');
        $position3 = sget($_POST,'position3');
        $name3 = sget($_POST,'name3');
        $recid3 = sget($_POST,'recid3');
        $flag = true;
        for($i = 1; $i <= 3; $i++)
        {
            $name = sget($_POST, "name" . $i);
            $position = sget($_POST, "position" . $i);
            $recid = sget($_POST, "recid" . $i);
            if($recid > 0)
            {
                $sql = "update users_data set data_attr = '%d', data_value = '%s', data_value2 = '%s' where uid = $uid and data_id =%d and data_type = 'management'";
                $sql_data = array($i, $name, $position, $recid);
            }else{
                $sql = "insert into users_data (uid, data_attr, data_value, data_value2,data_type) values('%d', '%d','%s','%s','management')";
                $sql_data = array($uid, $i, $name, $position);
            }
            if( ! db_query($sql, $sql_data))
            {
                $flag = false;
                break;
            }
        }
        if($flag) drupal_set_message("Company management information is updated.");
        else{drupal_set_message("Failed to update information. Please try again later", 'error');}
    }
    return $Block;
}
function company_market($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $uid = $user->uid;
    $Block['title'] = 'Existing Market';
    if(!$write || $op == 'view' || $op == null){
        $content = "<ul>";
        $sql = "select * from users_data where uid = $uid and data_type = 'marketinfo' and data_attr = 'market'";
        $result = db_query($sql);
        $counter = 1;
        while(($data = db_fetch_object($result)) !== false)
        {
            $content .= "<li>" . check_plain($data->data_value2) . "</li>";
            $counter++;
        }
        if($content == ''){$content = '<li>n/a</li>';}
        
        if ($write) {
          $content .= "</ul><div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_company_market_info'>edit</a></div>";
        }
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        
    }
    return $Block;
}
function company_market_info($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $uid = $user->uid;
    $Block['title'] = 'Market Information';
    $array = array("competitor","market","explansion");
    if(!$write || $op == 'view' || $op == null){
        $content = "";
        
        $sql = "select * from users_data where uid = $uid and data_type = 'marketinfo' order by data_id";
        $result = db_query($sql);
        
        $c = ''; $m = ''; $e = '';
        
        for($i = 1; $i <=3 ; $i++)
        { 
            if(($data = db_fetch_object($result)) !== false)
            {
                $c .= check_plain($data->data_value2) . "<br>"; 
            }
        }
        if($c == '') $c = 'n/a <br>';
        for($i = 1; $i <=3 ; $i++)
        { 
            if(($data = db_fetch_object($result)) !== false)
            {
                $m .= check_plain($data->data_value2) . "<br>"; 
            }
        }
        if($m == '') $m = 'n/a <br>';
        for($i = 1; $i <=3 ; $i++)
        { 
            if(($data = db_fetch_object($result)) !== false)
            {
                $e .= check_plain($data->data_value2) . "<br>"; 
            }
        }
        if($e == '') $e = 'n/a <br>';
        $content .= "<b>Top three competitors</b><br>" .
                    "$c" .
                    "<b>Top three markets you currently sell in</b><br>" .
                    "$m" .
                    "<b>Top three priority expansion markets</b><br>" .
                    "$e" ;
        
            
       
        
        if ($write) {
          $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_market_info'>edit</a></div>";
        }
        
        $Block['body'] = content_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        //test_array($_POST);return;
        
        foreach($array as $key)
        {
            for($i = 1; $i <= 3; $i++)
            {
                $input = sget($_POST, "${key}_" . $i);
                $recid = sget($_POST, "${key}_id_" . $i,'decimal');
                if($recid > 0){
                    $sql = "update users_data set data_value2 = '%s' where data_id = $recid and uid = $uid limit 1";
                    $sql_data = array($input);
                }else{
                    $sql = "insert into users_data (uid, data_type, data_attr, data_value2) values($uid, 'marketinfo', '$key', '%s')";
                    $sql_data = array($input);
                }
                if( ! db_query($sql, $sql_data))
                {
                    drupal_set_message("Failed to update market info. Please try again later",'error');
                    return ;
                }
            }
        }
        drupal_set_message("Company market information is updated.");
    }
    return $Block;
}
function company_milestone($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $uid = $user->uid;
    $Block['title'] = 'Recent Milestones';
    if(!$write || $op == 'view' || $op == null){
        $content = "<ul>";
        $sql = "select * from users_data where uid = $uid and data_type = 'milestone' order by data_id";
        $result =  db_query($sql);
        $counter = 1;
        while(($data = db_fetch_object($result)) !== false)
        {
            $milestone = check_plain($data->data_value2);
            $content .= "<li>" . $milestone . "</li>";
            $counter++;
        }
        if($content == '')$content = '<li>n/a</li>';
        if ($write) {
          $content .= "<ul><div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_company_milestones'>edit</a></div>";
        }
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        $flag = true;
        for($i = 1; $i <= 3; $i++)
        {
            $content = sget($_POST, 'content' . $i);
            $recid = (int)sget($_POST, 'recid' . $i);
            if($recid > 0)
            {
                $sql = "update users_data set data_attr = '$i', data_value2 = '%s' where uid = $uid and data_id = %d and data_type = 'milestone' limit 1";
                $sql_data = array($content, $recid);
            }else{
                $sql = "insert into users_data(data_attr, data_value2,uid,data_type) values($i,'%s', %d, 'milestone')";
                $sql_data = array($content, $uid);
            }
            if( ! db_query($sql, $sql_data))
            {
                $flag = false;
                break;
            }
        }
        if($flag) drupal_set_message("Company management information is updated.");
        else{drupal_set_message("Failed to update information. Please try again later", 'error');}
        
    }
    return $Block;
}



/* EOF */
