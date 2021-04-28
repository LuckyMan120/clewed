<?php
$maenna_page['content'] = array('company_list');

function company_list($op = null)
{
    $Block['title'] = 'Companies';
    if($op == 'view' || $op == null){
        $content = '';
        
        //$content = company_search();
        $content = company_search_tab('super');
        $content .= company_search_result('super');

        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}


function company_search_tab($utype)
{
    global $user_type;
    $html = '';
    $STATES = get_us_states();
    $_Industry = _INDUSTRY();
    $_Revenue = RevenuesData();
    $_Earnings = EarningsData();

    
        ///////////////////
        $html .= "<form action='' method='get' id='search_form'>
        <input type=hidden name=a value=companies>
        <input type=hidden name=type value=company>
        <input type=hidden name=update_section value='' id='update_section'>
        <input type=hidden name=update_section_data value='' id='update_section_data' />";
        
        $html .= "<table cellspaing=0 cellpadding=0 class='search-tabs' style='width:770px;'>";
        
        $company_pam = 'Company';
        if(sget($_REQUEST,'company')) $company_pam = sget($_REQUEST,'company');
        $section = "<input type=text name=company value='$company_pam'>";
        $Letters =  "abcdeghijklmnopqrstuvw";
        $section .= "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        for($i = 0; $i < strlen($Letters); $i++)
        {
            $l = strtoupper($Letters[$i]);
            $section .= "<a href='#' class='letter_selector' data=$l>$l</a>&nbsp;";
        }
        $letter = sget($_REQUEST,'letter') ? sget($_REQUEST,'letter') : '';
        $section .= "<a href='#' class='letter_selector' data=x style='letter-spacing:0;'>XYZ</a>";
        $html .= "<tr><td colspan=3><input type=hidden name=letter value='' id='letter' value='$letter' />$section</td><td></td><td></td></tr>";
        //////////////////////////
        
        
        // industry
        $industry = sget($_REQUEST, 'industry');
        
        $option_industry = "<option value=''>Industry</option>" . OPTION_INDUSTRY($industry);
        $section1 = "<td width='33%'><select name=industry>$option_industry</industry></td>";
        
        // revenue low
        $section2 = "\n<td width='33%'><select name=revenue_low><option value=''>Revenue Low</option>";
        $revenue_min = sget($_REQUEST, 'revenue_low');
        $option_revenueLow = RevenuesOption($revenue_min);
        $section2 .= $option_revenueLow . "</select></td>";
        
        
        // earnings low
        $earnings_low = sget($_REQUEST, 'earnings_low');
        $option_earningslow = EarningsOption($earnings_low);
        $section3 = "<td><select name='earnings_low'><option value=''>Earnings Low</option>$option_earningslow</select></td>";
        
        //follow up
        $followup = (sget($_REQUEST,'followup_from'))? sget($_REQUEST,'followup_from') : 'FollowUp - From';
        $section_followup = "<td><input type=text name=followup_from value='$followup' class='datepicker'></td>";
        
        ////////////////
        // relationship manager
        if( ! isset($user_type ))
        {
            $Managers = array();
            $sql = "select users.mail, users.uid, concat_ws(',',users_extend.last_name, users_extend.first_name) as name from users, users_roles,users_extend where users.uid = users_extend.uid and users.uid = users_roles.uid and users_roles.rid = 6 order by users_extend.last_name ";
            $result = db_query($sql);
            while(($row = db_fetch_object($result ))!= false)
            {
                $uid = $row->uid;
                $name = $row->name;
                $Managers["$uid"] = $name;
            }
            $managerid = sget($_REQUEST,'managerid');
            $TD_relationshipmanager = "<td><select name=managerid><option  value=''>Relationship Manager</option><option></option>";
            foreach($Managers as $uid => $name)
            {
                $selected = '';
                if($managerid == $uid) $selected = "selected";
                $TD_relationshipmanager .= "<option $selected value='$uid'>$name</option>\n";
            }
            $TD_relationshipmanager .= "</select></td>";
        }else{
            $TD_relationshipmanager = "<td></td>";
        }



        $html .="<tr>$section1  $section2 $section3 $section_followup $TD_relationshipmanager</tr>";
        /////////////////////////
        $section1 = "\n<td><select name=state>";
        
        
        // State selection
        $STATES['']='State';
        $state = sget($_REQUEST, 'state');
        foreach($STATES as $key => $name) {
            $selected = '';
            if(($state == $key) && (! empty($state))) $selected = "selected";
            $section1 .= "\n<option value=$key $selected>$name</option>\n";
        }
        $section1 .= "</select></td>";
        
        
        //max revenue
        $revenue_high = sget($_REQUEST, 'revenue_high');
        $section2 = "<td><select name=revenue_high><option  value=''>Revenue High</option>";
        $option_revenueHigh = RevenuesOption($revenue_high);
        $section2 .= $option_revenueHigh . "</select></td>";
        
        // earnings high
        $earnings_high = sget($_REQUEST, 'earnings_high');
        $option_earningshigh = EarningsOption( $earnings_high);
        $section3 = "<td><select name='earnings_high'><option value=''>Earnings High</option>$option_earningshigh</select></td>";
        


        //follow up + submit button
        $submit_btn = "<td><input type=submit value=Filter class='button' ></td>";
        $followup_to = (sget($_REQUEST,'followup_to'))? sget($_REQUEST,'followup_to') : 'FollowUp - To';
        $section_followup_to = "<td><input type=text name=followup_to value='$followup_to' class='datepicker'> $submit_btn</td>";
        
        
         
        
        $html .="<tr>$section1  $section2 $section3 $section_followup_to</tr>";    
        $html .= "</table>";
      
        
        if(isset($_REQUEST['update_section_data']) && empty($user_type) ) $html .= batch_edit_panel();
        
       // $html .= "</td></tr></table>";
        
        ////////////////NEW TABLE//
    
        /// END OF NEW TABLE
        
        //if(isset($_REQUEST['update_section_data'])) $html .= result_group();
        $html .= "</form>";
    $html .= <<< END

<script type='text/javascript'>
$(document).ready(function(){
    init_search_selector();
    init_select_all();
    init_datepicker();
})
</script>
END;
    return $html;
}


function company_search_result($utype){
    
    $SearchKeys = array(
                        'company',
                        'industry',
                        'revenue_low',
                        'revenue_high',
                        'earnings_low',
                        'earnings_high',
                        'followup_from',
                        'followup_to',
                        'managerid',
                        'state',
                        'letter'
                        );
    // filter out empty values
    $SearchValues = array();
    foreach($SearchKeys as $key){
        if(sget($_REQUEST, $key))
        {
            $SearchValues["$key"] = sget($_REQUEST, $key);
        }
    }
    // filter out default values, and prepare for query
    $SQL_Where = array();
    foreach($SearchValues as $key=> $val)
    {
        switch($key)
        {
            case 'company':
                if(strcasecmp($val,'company') == 0){
                    $val = '';
                }else{
                    $val = "maenna_company.company like '%".($val) ."%'";    
                }
                break;
            case 'industry':
                $val = "maenna_company.industry = '$val'";
                break;
            case 'revenue_low':
                $val = "financial_tbl.revenue >= $val";
                break;
            case 'revenue_high':
                $val = "financial_tbl.revenue <= $val";
                break;
            case 'earnings_low':
                $val = "financial_tbl.earning >= $val";
                break;
            case 'earnings_high':
                $val = "financial_tbl.earning <= $val";
                break;
            case 'followup_from':
                if(strcasecmp($val, 'FollowUp - From') == 0)
                {
                    $val = '';
                }else{
                    $val = strtotime($val);
                    $val = "followup_tbl.followup >= '$val'";    
                }
                break;
            case 'followup_to':
                
                
                if(strcasecmp($val, 'FollowUp - to') == 0){
                    $val = '';
                }else{
                    
                    $val = strtotime($val);
                    $val = "followup_tbl.data_attr <= '$val'";  
                }
                break;
            case 'state':
                $val = "maenna_company.state = '$val'";
                break;
            case 'letter':
                $val = "maenna_company.company like '".($val) ."%'";
                break;
            case 'managerid':
                
                break;
        }
        if(! empty($val)) $SQL_Where["$key"] = $val;
    }
    $Join_array = array();
    $Join_array[] = " left join (select  companyid as financial_id,".
                                        "data_attr as financial_year,".
                                        "data_value as revenue, ".
                                        "data_value2 as earning, ".
                                        "data_value3 as equity ".
                                        "from maenna_company_data where dataid = (".
                                        "select dataid from maenna_company_data as temp where temp.companyid = maenna_company_data.companyid and temp.data_type = 'financial' and data_value > 0 order by temp.data_attr desc limit 1".
                                        ") ) as financial_tbl on financial_tbl.financial_id = maenna_company.companyid";
    $curTime = time();
    $Join_array[] = "left join ( select companyid as followupid, data_attr as followup from maenna_company_data where dataid = (select dataid from maenna_company_data as temp where (temp.data_type='followup' or temp.data_type='progress')  and temp.companyid = maenna_company_data.companyid order by dataid desc limit 1) ) as followup_tbl on followup_tbl.followupid = maenna_company.companyid ";

    $SQL_Where[] = 'users.uid = maenna_company.companyid';
    
    $sql_total = "select count(*) as total  from users, maenna_company " . implode(' ', $Join_array);
    $sql_total .= " where " .implode(' and ', $SQL_Where);
    
    $start = 0;
    $limit = 15;
    if(sget($_REQUEST, 'page'))$start = (sget($_REQUEST, 'page') - 1) * $limit; 
    
    $SQL = "select maenna_company.*,financial_tbl.*, followup_tbl.*,users.uid,users.created from  users, maenna_company " . implode(' ', $Join_array);
    $SQL .= " where " .implode(' and ', $SQL_Where);
    $SQL .= " order by users.created desc";
    $SQL .= " limit $start, $limit";
    
    //echo $SQL;
 
    $result_total = db_query($sql_total);
    $m = db_fetch_object($result_total);
    
    if($m->total <= 0)
    {
        return "no record found";
    }
    
    $result = db_query($SQL);
    
    $detail_array = array(
                    'companyid' =>  array('Project Name', '',70),
                    'sector'  =>    array('Industry', 'hidea',0),
                    'revenue' =>    array('Revenue','hidea',78),
                    'earning' =>    array('Earnings %','hidea',70),
                    'roe' =>        array('ROE','hidea',50),
                    'advisorsReq' => array('Advisor Requests','hidea',50),
                    'advisors' =>   array('Advisors','hidea',50),
                    'analyst' =>    array('Analyst','hidea',50),
                    'followers' =>  array('Followers', 'hidea',50),
                    'followup'   => array('Advisory Calendar','hidea',30),
                    'company' =>    array("Name",'hideb',0),
                    'state'   =>    array('State','hideb',50),
                    'membership' =>  array('Membership', 'hideb',70),
                    'contact' =>    array('Rel. Manager','hideb',),
                    //'position' =>   array('Position','hideb',50),
                    //'gender'  =>    array('M/F','hideb',50),
                    //'phone'   =>    array('Phone','hideb',60),
                    //'email'   =>    array('Email','hideb',0),
                    'created' =>    array("Registered",'hideb',70),
                    'spteam' => array('Sp. Proj. Team', 'hideb', 90),
                      );
    $html = '<div style="text-align:right">
                <a href="#" id="table_collapse" class=button>more info</a>
            </div>
            <table cellspacing=0 cellpadding=0 border=0 width=100% class="list">
                    <thead>
                        <th class="checkbox" align=left>
                            <input type=checkbox id="select_all" >
                        </th>';
    foreach($detail_array as $Row)
    {
        $name = $Row[0];
        $class = $Row[1];
        $width = '';
        if(! empty($Row[2])) $width = 'width=' . $Row[2];
        $html .= "\n<td class='$class' $width>$name</td>";
    }
    $html .= "<td style='width:30px;'></td></tr></thead><tbody>";
    $class='';
    $_SECTOR = _sectors();
    $_Industry = _INDUSTRY();
    if(! empty($result))
    {
        while(($row = db_fetch_array($result) ) !== false)
        {
        
            $recordid = sget($row, 'companyid');
            //test_array( $row); exit;
            $html .= "\n<tr><td><input type=checkbox name=records[] value='$recordid' class='records' /></td>";
            foreach($detail_array as $key=> $Row)
            {
                $value = sget($row, $key);
                $class = "class='".$Row[1] . "'";
                
                if($key == 'company'){
                    
                    $value =  htmlentities($value);

                }elseif($key == 'created')
                {
                    $value = date('m/d/Y', $value);
                }
                elseif($key == 'revenue') 
                {
                    if($value) $value = "$" . number_format($value);
                }elseif($key == 'earning'){
                    $revenue = sget($row,'revenue');
                    $revenue = filter_num($revenue);
                    $earning = filter_num($value);
                    if($earning && $revenue) $value = number_format(($earning / $revenue) * 100,2) . "%";
                    else $value = '';
                }elseif($key == 'data_attr')
                {
                    if($value) $value = date('m/d/Y', $value);
                }elseif($key == 'sector')
                {
                    
                    if($value) {
                        if(strlen($value)<=3)$value = $_SECTOR["$value"];
                        
                    }
                }elseif($key=='followup')
                {
                    if($value) $value = date('m/d/Y', $value);
                    else $value = '';
                    
                }elseif($key =='companyid'){
                    $value = getProjectName($value);
                    $value = "<a href='/account?a=companies&type=company_detail&companyid=$recordid'>".$value."</a>";
                }elseif($key == 'roe')
                {
                    $equity = filter_num(sget($row, 'equity'));
                    $earning = filter_num(sget($row,'earning'));
                    if($equity && $earning)$value = number_format(($earning / $equity) *100,2).'%';
                    else $value = '';
                }elseif(in_array($key, array('advisors','advisorsReq','analyst','followers')))
                {
                    if(empty($value)) $value = '-';
                }
                $html .= "\n<td $class>" . ($value) . "</td>";
            }
            $page = sget($_REQUEST,'page');
            $html .= "<td style='text-align:right;'><img src='/". path_to_theme() ."/images/edit-icon.png' width=14 style='vertical-align:middle;cursor:pointer' data='$recordid' class='company-edit-icon' page='$page'></td></tr>\n";
        }

        if( (sget($_REQUEST,'group_result') == 'batch assign') )
        {
            perform_grouping('company', $sql_where);
        }
    }
    $html .= "</tbody></table>";
    $Pagenation = array('total' => $m->total,
        'limit' =>$limit,
        'baseurl' => "/account?",
        'num_of_links' => 8,
    );
    $html.= '<table class="no-border"><tr border=0>

            <td><input type=button class=button value="delete" id="delete_btn" /></td>
            <td border=0>' .
            "<div style='width:100%;'>" .
            pagination($Pagenation) . "</div>" .
            '</td></tr></table>';
    return $html;
}


function batch_edit_panel($type = 'company')
{
   
    $sections = '';
    $Admins = admin_group();
    $admin_html = '';
    foreach($Admins as $a)
    {
        $id = $a[0]; $name = $a[1];
        $admin_html .= "\n". "<option value=$id>$name</option>";
    }
    
    $priority_html = '';
    $Prospects = array('high','medium','low','unlikely');
    foreach($Prospects as $level)
    {
        $priority_html .= "<option>$level</option>";
    }
    
    if($type == 'company')
    {
        $sections  .= "<tr><td >Name the results as a group <input type=text name=group_title value=''></td>";
        $sections  .= "<td >Assign the group to manager: <select name='group_manager_id'><option value=''>Manager</option>$admin_html</select></td>";
        //$sections  .= "<td >Set Prospect Level: <select name='batch_priority'><option></option>$priority_html</select></td>";
        $sections  .= "<td></td><td><input type='submit' name='group_result' value='batch assign' ></td></tr>";
    }elseif($type == 'people')
    {
        $sections  .= "<tr><td >Name the results as a group <input type=text name=group_title value=''></td>";
        $sections  .= "<td >Assign the group to manager: <select name='group_manager_id'><option value=''>Manager</option>$admin_html</select></td>";
        $sections  .= "<td >Set Prospect Level: <select name='batch_priority'><option></option>$priority_html</select></td>";
        $sections  .= "<td></td><td><input type='submit' name='group_result' value='batch assign' ></td></tr>";
    }
    
    $html  = "<div style='padding-left:30px;background:lightblue'><table cellpadding=8 cellspacing=6 class='no-border'>$sections</table></div>";
    return $html;
}
function perform_grouping($table, $sql_where)
{
    $return_html = '';

    if($table == 'company')
    {
        
        
        if(count($sql_where) <= 0)
        {
            drupal_set_message('Please check search terms and try again ','error');
            return;
        }
        $title = sget($_REQUEST,'group_title');
        $managerid = sget($_REQUEST,'group_manager_id');
        $manager_type = 'manager';
        $sql = "insert into maenna_company_manager (managerid, title, manager_type) values ('%d','%s','%s')";
        $sql_data = array($managerid, $title, $manager_type);
        if( ! db_query($sql, $sql_data))
        {
            drupal_set_message('Failed to add new group.','error');
            return ;
        }
        $last_id = db_last_insert_id("maenna_company_manager", 'groupid');
        if( empty($last_id ))
        {
            drupal_set_message('Failed to add new group. reference: last id','error');
            return ;
        }
        
        $batch_priority = sget($_REQUEST, 'batch_priority');
        //test_array($_REQUEST);
        if(empty($batch_priority) )  $sql = "update maenna_company set groupid = $last_id " . " where " . implode(" and ", $sql_where);
        else $sql = "update maenna_company set priority = '$batch_priority', groupid = $last_id " . " where " . implode(" and ", $sql_where);
        if(db_query($sql))
        {
            drupal_set_message("operation successful.");
        }else{
            drupal_set_message('Failed to add companies to the group','error');
        }

    }
    return $return_html;
}

function key_statistics($op = null){
    $Block['title'] =  'Key Statistics'; 
    if($op == 'view' || $op == '') {
        

        $content = "<table cellspacing=0 cellpadding=0 border=0>
                <tr><td>Registered online</td><td algin=right>1</td></tr> 
                <tr><td>Follow up</td><td algin=right>1</td></tr>";
        
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }
    return $Block;
}
function followup_stats($op = null)
{
    $Block['title'] =  'Follow-Up Stats'; 
    if($op == 'view' || $op == '') {
        

        $content = "<table cellspacing=0 cellpadding=0 border=0><tr><td>Month</td><td align=right># of Companies</td></tr>";
        
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }
    return $Block;
}

function sector_stats($op = null)
{
    $Block['title'] =  'Sector Stats'; 
    if($op == 'view' || $op == '') {
        

        $content = "<table cellspacing=0 cellpadding=0 border=0><tr><td>Sector</td><td align=right># of Companies</td></tr>";
        
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }
    return $Block;
}
function mark_for_deletion($op = null)
{
    global $user;
    $editorid = $user->uid;
    if($op == 'update')
    {
        $update_section_data = sget($_REQUEST, 'update_section_data');
        $update_section_data = preg_replace('/[^0-9,]/','', $update_section_data);
        if($update_section_data)
        {
            $sql = "update research_company set delete_marker = 1 where recordid in (0 $update_section_data)";
            if(db_query($sql)) drupal_set_message("The selected records are removed from search results");
            else{
                drupal_set_message("error: failed to remove records",'error');
            }
        }
    }
}

function company_search()
{
    $Search_parameter = array();
    $Selection = array();
    $Sectors = _sectors();
    $PRoles = _pro_roles();
    $States = get_us_states();
    $Search_parameter = array(
                                  
    );
    $Search_result  = get_search_result('company');
    $paginationBaseURL = '/account?a=companies&';
    $block = render_people_search($Search_parameter, $Search_result, $paginationBaseURL);
    return $block;
}


function company_priorities($op = '')
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    
    $Block['title'] =  'Priorities'; 
    if($op == 'view' || $op == '') {
        $content = '';
        $sql = "select * from users_data where uid = $companyid and data_type = 'priority' order by data_id";
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
            
            if($recid) $sql = "update users_data set data_value = '%s' where uid = $companyid and data_id = $recid limit 1";
            else $sql = "insert into users_data (uid, data_type, data_value) values($companyid, 'priority', '%s')";
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
function company_info($op = null)
{
    global $user;
    $editorid = $user->uid;
    $cid = sget($_REQUEST,'cid');
    
    
    $Data = _get_user_ext($cid);
    $Block['title'] = "<h2 style='color:#00a2bf;font-size:24px;'>" . ucwords($Data['company_name']) ."</h2>";
    if($op == 'view' || $op == null){
        $content = $Data['content'];
        $Block['body'] = content_box($Block['title'], $content);
    }
    
    return $Block;
}

function company_to_watch($op = null)
{
    $Block['title'] = 'Companies to watch';
    if($op == 'view' || $op == null){
        $content = "";
        
        $html = <<< END
            <table>
                <thead><tr><th style='width:45%'>Description</th>
                            <th>Sector</th>
                            <th>Revenue</th>
                            <th>Growth</th>
                            <th>Earnings</th>
                            <th>Growth</th>
                            <th>Detail</th>
                </tr></thead>
            </table>


END;
        
        $Block['body'] = content_box($Block['title'], $html);
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
function company_name($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    
    $sql = "select company_name,description from users_extend where uid = $companyid limit 1";
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
        $sql_data = array($desc, $companyid);
        if(db_query($sql, $sql_data)) drupal_set_message("Your company description is updated");
        else drupal_set_message("Failed to update information. Please try again later", 'error');
    }
    return $Block;
}

function company_competitors($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    
    $Block['title'] = 'Competitors';
    if($op == 'view' || $op == null){
        $content = "DSA";
        $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
     return $Block;
}
function comapny_expansion($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    
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
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    
    $Block['title'] = 'Tagged Information';
    if($op == 'view' || $op == null){
        $content = "links tagged from info tab";
        $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
     return $Block;
}

function company_leaders($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    
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
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    
    $Block['title'] = 'Relevant Information and Articles';
    if($op == 'view' || $op == null){
        
        
        
        $content = "DSA";
        $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
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
                $sql = "update users_data set data_value = '%s', data_value2 = '%s' where uid = $companyid and data_type = 'financial' and data_attr = '$theYear'";
                $sql_data = array($revenue, $earnings);
            }else{
                $sql = "insert into users_data (uid, data_type, data_attr, data_value, data_value2) values ($companyid, 'financial', '$theYear', '%s','%s')";
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
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    
    $Block['title'] = 'Company Financials';
    if($op == 'view' || $op == null){
        
        
        
        
        include_once '/var/www/vhosts/clewed.com/httpdocs/php-ofc-library/open_flash_chart_object.php';
        $content = "<div style='position:relative;z-index:2;' id='financial_chart'>";
        $content .= open_flash_chart_object( 500, 250, 'http://'. $_SERVER['SERVER_NAME'] .'/open_chart?companyid=' . $companyid , false );
        $content .= "</div>";
        $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
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
                $sql = "update users_data set data_value = '%s', data_value2 = '%s' where uid = $companyid and data_type = 'financial' and data_attr = '$theYear'";
                $sql_data = array($revenue, $earnings);
            }else{
                $sql = "insert into users_data (uid, data_type, data_attr, data_value, data_value2) values ($companyid, 'financial', '$theYear', '%s','%s')";
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
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    
    $Block['title'] = 'Logo';
    if($op == 'view' || $op == null){
        $sql = "select * from users_data where uid = $companyid and data_type = 'logo'";
        $result = db_query( $sql );
        if( db_affected_rows($result) > 0 )
        {
            $Data = db_fetch_object($result);
            $img = $Data->data_value;
            $src = "/" . file_directory_path() . "/" . $img;
            //echo $src;
            $content = "<div class='logo'><img src='$src' style='width:130px;'></div>";
            $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='update_company_logo'>edit</a></div>";
        }else{
            
            $content = "<div class='edit-link'><a href='#' class='edit_wnd' rel='company_logo'>upload a logo</a></div>";
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
                            $sql = "update users_data set data_value = '$filename' where data_id=$recid and uid=$companyid";
                            db_query($sql);
                        }else{
                            $sql = "insert into users_data (uid, data_type, data_value) values ($companyid, 'logo', '$filename')";
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
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    $Block['title'] = 'Company Info';
    if($op == 'view' || $op == null){
        $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_breif_info'>edit</a></div>";
        
        
        $sql = "select * from users_extend where uid = $companyid";
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
        
        $sql = "select * from users_data where uid = $companyid and data_type = 'numofemp'";
        $result = db_query($sql);
        $Data = db_fetch_object($result);
        if($Data){
            $numOfEmp = $Data->data_value;
            if($numOfEmp)$numOfEmp = number_format($numOfEmp);
            $numOfEmpid = $Data->data_id;
        }
        
        $sql = "select * from users_data where uid = $companyid and data_type = 'founded'";
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
        
        $sql = "update users_extend set phone = '%s', website = '%s', street1='%s', street2 = '%s', city='%s', state='%s' where uid = $companyid limit 1";
        db_query($sql, array($phone, $website, $street1, $street2, $city, $state));
        
        $fid = sget($_POST,'fid');
        $sql_data = array($founded, $companyid, 'founded');
        if($fid) $sql = "update users_data set data_value = '%s' where uid = %d and data_type = '%s' limit 1";
        else $sql = "insert into users_data (data_value, uid, data_type ) values('%s', %d,  '%s')";
        db_query($sql, $sql_data);
        
        $noeid = sget($_POST,'noeid');
        $sql_data = array($numOfEmp, $companyid, 'numofemp');
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
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    $Block['title'] = 'Management Discussion and Analysis';
    if($op == 'view' || $op == null){
        $content = "<ul class=''>";
        
        $sql = "select * from users_data where uid = $companyid and data_type = 'management' order by data_attr";
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
        $content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_company_management'>edit</a></div>";
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
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    $Block['title'] = 'Management';
    if($op == 'view' || $op == null){
        $content = "<ul class=''>";
        
        $sql = "select * from users_data where uid = $companyid and data_type = 'management' order by data_attr";
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
        $content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_company_management'>edit</a></div>";
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
                $sql = "update users_data set data_attr = '%d', data_value = '%s', data_value2 = '%s' where uid = $companyid and data_id =%d and data_type = 'management'";
                $sql_data = array($i, $name, $position, $recid);
            }else{
                $sql = "insert into users_data (uid, data_attr, data_value, data_value2,data_type) values('%d', '%d','%s','%s','management')";
                $sql_data = array($companyid, $i, $name, $position);
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
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    $Block['title'] = 'Existing Market';
    if($op == 'view' || $op == null){
        $content = "<ul>";
        $sql = "select * from users_data where uid = $companyid and data_type = 'marketinfo' and data_attr = 'market'";
        $result = db_query($sql);
        $counter = 1;
        while(($data = db_fetch_object($result)) !== false)
        {
            $content .= "<li>" . check_plain($data->data_value2) . "</li>";
            $counter++;
        }
        if($content == ''){$content = '<li>n/a</li>';}
        
        $content .= "</ul><div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_company_market_info'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        
    }
    return $Block;
}
function company_market_info($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    $Block['title'] = 'Market Information';
    $array = array("competitor","market","explansion");
    if($op == 'view' || $op == null){
        $content = "";
        
        $sql = "select * from users_data where uid = $companyid and data_type = 'marketinfo' order by data_id";
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
        
            
       
        
        $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_market_info'>edit</a></div>";
        
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
                    $sql = "update users_data set data_value2 = '%s' where data_id = $recid and uid = $companyid limit 1";
                    $sql_data = array($input);
                }else{
                    $sql = "insert into users_data (uid, data_type, data_attr, data_value2) values($companyid, 'marketinfo', '$key', '%s')";
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
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'cid');
    $Block['title'] = 'Recent Milestones';
    if($op == 'view' || $op == null){
        $content = "<ul>";
        $sql = "select * from users_data where uid = $companyid and data_type = 'milestone' order by data_id";
        $result =  db_query($sql);
        $counter = 1;
        while(($data = db_fetch_object($result)) !== false)
        {
            $milestone = check_plain($data->data_value2);
            $content .= "<li>" . $milestone . "</li>";
            $counter++;
        }
        if($content == '')$content = '<li>n/a</li>';
        $content .= "<ul><div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_company_milestones'>edit</a></div>";
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
                $sql = "update users_data set data_attr = '$i', data_value2 = '%s' where uid = $companyid and data_id = %d and data_type = 'milestone' limit 1";
                $sql_data = array($content, $recid);
            }else{
                $sql = "insert into users_data(data_attr, data_value2,uid,data_type) values($i,'%s', %d, 'milestone')";
                $sql_data = array($content, $companyid);
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