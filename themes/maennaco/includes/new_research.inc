<?php

function research_table($op = null)
{
    $Block['title'] = ("Research");
    
    if($op){
        $content = get_research_tabs();
        $type = sget($_REQUEST,'type');
        if(empty($type)) $type = 'company';
        if($type == 'company')
        {
            $content .= get_research_company();
        }elseif($type == 'people'){
            $content .= get_research_people();
        }elseif($type == 'company_detail')
        {
            $content .= get_company_detail();
        }
        
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}


function get_research_tabs()
{
    global $AccessObj;
    $html = '';
    $type = sget($_REQUEST,'type');
    if(empty($type)) $type = 'company';
    
    $array = array("COMPANY", "PEOPLE");
    $html .= '<div class="searchtab" style="margin-top:-22px;">';
    foreach($array as $_type)
    {
        $type_lc = strtolower($_type);
        $class='';
        if(strcasecmp($type, $_type) == 0){
            $class = 'active';
        }
        $html .= "&nbsp;&nbsp;&nbsp;&nbsp;<a href='/account?tab=research&type=$type_lc' class='$class'>$_type</a>";
    }
    $html .="</div>";
    $Prospects = array('high','medium','low','unlikely');
    if($type == 'company')
    {
        $Rrevenue_sizes = array(
                                "5mil" => "5 million",
                                "10mil" => "10 million",
                                "25mil" => "25 million",
                                "50mil" => "50 million",
                                "100mil" => "100 million",
                                "250mil" => "250 million",
                                );
        
        
        ///////////////////
       
        
        $company_pam = 'Company';
        if(sget($_REQUEST,'company')) $company_pam = sget($_REQUEST,'company');
        $td_company_name = "<input type=text name=company value='$company_pam'>";
        $Letters =  "abcdeghijklmnopqrstuvw";
        $td_company_name .= "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        for($i = 0; $i < strlen($Letters); $i++)
        {
            $l = strtoupper($Letters[$i]);
            $td_company_name .= "<a href='#' class='letter_selector' data=$l>$l</a>&nbsp;";
        }
        $letter = sget($_REQUEST,'letter') ? sget($_REQUEST,'letter') : '';
        $td_company_name .= "<a href='#' class='letter_selector' data=x style='letter-spacing:0;'>XYZ</a>
                        <input type=hidden name=letter value='' id='letter' value='$letter' />";

        
        $sic = (sget($_REQUEST,'sic')) ? sget($_REQUEST,'sic') : 'SIC';
        $td_sic = "<input type=text name=sic value='$sic' />";
        
        $td_rev_low = "\n<select name=revenue_min><option value=''>Revenue Low</option>";
        $revenue_min = sget($_REQUEST, 'revenue_min');
        foreach($Rrevenue_sizes as $key => $name)
        {
            $selected = '';
            if($revenue_min == $key) $selected = "selected";
            $td_rev_low .= "<option value='$key' $selected>$name</option>\n";
        }
        $td_rev_low .= "</select>";
        
        
        $pri_pam = '';
        if(sget($_REQUEST,'priority')) $pri_pam = sget($_REQUEST,'priority');
        $td_priority = "\n<select name='priority'><option value=''>Prospect</option>";
        foreach($Prospects as $p)
        {
            $selected = '';
            if($p == sget($_REQUEST,'priority'))$selected = 'selected';
            $td_priority .= "<option $selected>$p</option>";
        }
        $td_priority .= "</select>";
        
        
        $followup = (sget($_REQUEST,'followup_from'))? sget($_REQUEST,'followup_from') : 'FollowUp - From';
        $td_followup_from = "<input type=text name=followup_from value='$followup' class='datepicker'>";
        
        ////////////////
       
        
        
     
        $td_state = "\n<select name=state><option value=''>State<option>";
       
        $state = sget($_REQUEST, 'state');
        $td_state .= Options_state($state);
        $td_state .= "</select>";
        
        $revenue_max = sget($_REQUEST, 'revenue_max');
        $td_rev_high = "<select name=revenue_max><option  value=''>Revenue High</option>";
        foreach($Rrevenue_sizes as $key => $name)
        {
            $selected = '';
            if($revenue_max == $key) $selected = "selected";
            $td_rev_high .= "<option value='$key' $selected>$name</option>\n";
        }
        $td_rev_high .= "</select>";
        
        $td_managers = '';
        
        if( $AccessObj->user_type == 'super')
        {
            $Managers = Mae_users::all_admins();
            array_unshift($Managers, array('uid' => 'none', 'first_name' => '*not assigned', 'last_name' => ''));
            $options_manager = '';
            $posted_managerid = sget($_REQUEST, 'managerid');
            foreach($Managers as $Admin)
            {
                $selected = '';
                $admin_id = sget($Admin,'uid');
                $name = ucwords(sget($Admin,'first_name')) . " " . ucwords(sget($Admin,'last_name'));
                if(($admin_id == $posted_managerid) && $posted_managerid) $selected = "selected";
                $options_manager .= "<option $selected value='$admin_id'>$name</option>\n";
            }
            $td_managers .= "<select name=managerid>
                                <option  value=''>Relationship Manager</option>
                                <option></option>$options_manager</select>";
        }
        
        $submit_btn = "<input type=submit value=Filter class='button' >";
        
        $followup_to = (sget($_REQUEST,'followup_to'))? sget($_REQUEST,'followup_to') : 'FollowUp - To';
        $td_followup_to = "<input type=text name=followup_to value='$followup_to' class='datepicker'>";
        
         
        $HV = hidden_post_values(array('tab'));
        $html .= <<< END
        <form action='/account' method='post' id='search_form'>
            $HV
            <input type=hidden name=type value=company>
            <input type=hidden name=update_section value='' id='update_section'>
            <input type=hidden name=update_section_data value='' id='update_section_data' />
            <input type=hidden name='_page' id='_page' value='$_page' />
                
            <table cellspaing=0 cellpadding=0 class='search-tabs' style='width:770px;'>
                <tr>
                    <td colspan=5>$td_company_name</td>
                </tr>
                <tr>
                    <td>$td_sic</td>
                    <td>$td_rev_low</td>
                    <td>$td_followup_from</td>
                    <td>$td_priority</td>
                    <td></td>
                </tr>
                <tr>
                    <td>$td_state</td>
                    <td>$td_rev_high</td>
                    <td>$td_followup_to</td>
                    <td>$td_managers</td>
                    <td>$submit_btn</td>
                </tr>
            
            </table>
        </form>
END;
        
      
        
        if(isset($_REQUEST['update_section_data']) && $AccessObj->user_type == 'super' )
        {
           // $html .= batch_edit_panel();
        }
       
        // $html .= "</td></tr></table>";
        ////////////////NEW TABLE//
        /// END OF NEW TABLE
        //if(isset($_REQUEST['update_section_data'])) $html .= result_group();
        
        /////////////////////////////////
    }elseif(sget($_REQUEST,'type') == 'people')
    {
        //test_array($_REQUEST);
       
        
        $name = 'Name';
        if(sget($_REQUEST,'name'))
        {
            $name = sget($_REQUEST,'name');
        }
        $email = (sget($_REQUEST,'email')) ? sget($_REQUEST,'email') : 'Email';
        $experience_low = (sget($_REQUEST,'experience_low')) ? sget($_REQUEST,'experience_low') : 'Years of Experience - Low';
        $experience_high = (sget($_REQUEST,'experience_high')) ? sget($_REQUEST,'experience_high') : 'Years of Experience - High';
        $followup_from = (sget($_REQUEST,'followup_from')) ? sget($_REQUEST,'followup_from') : 'Follow Up - from';
        $followup_to = (sget($_REQUEST,'followup_to')) ? sget($_REQUEST,'followup_to') : 'Follow Up - to';
        
        $td_pro_name = "<input type=text name=name value='$name'>";
        $Letters =  "abcdeghijklmnopqrstuvw";
        $td_pro_name .= "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        for($i = 0; $i < strlen($Letters); $i++)
        {
            $l = strtoupper($Letters[$i]);
            $td_pro_name .= "<a href='#' class='letter_selector' data=$l>$l</a>&nbsp;";
        }
        $letter = sget($_REQUEST,'letter') ? sget($_REQUEST,'letter') : '';
        $td_pro_name .= "<a href='#' class='letter_selector' data=x style='letter-spacing:0;'>XYZ</a>";
        $td_pro_name .= "<input type=hidden name=letter value='' id='letter' value='$letter' />";
        //////////////////////////
        
        $industry = (sget($_REQUEST,'industry')) ? sget($_REQUEST,'industry') : 'Industry';
        $_Industry = _INDUSTRY();
        $options_industry = '';
        foreach($_Industry as $key => $array){
            $options_industry .= "\n<optgroup label='$key'>";
            foreach($array as $val){
                $selected = '';
                if($industry == $val)$selected = 'selected';
                $options_industry .= "\n<option $selected>$val</option>";
            }
            $options_industry .= "\n</optgroup>";
        }
        $td_industry = "<select name=industry><option value=''>Industry</option>$options_industry</select>";
        
        $_Roles = _ROLES();
        $role = (sget($_REQUEST,'role')) ? sget($_REQUEST,'role') : 'Role';
        $option_role = '';
        foreach($_Roles as $r)
        {
            $selected = '';
            if($r == $role) $selected = 'selected';
            $option_role .= "\n<option $selected>$r</option>";
        }
        $td_role = "\n<select name=role><option value=''>Role</option>$option_role</select>";
        
        
        $Experience = array("1" => '1 - 5 year', '2' => '5-10 years', '3' => '10 to 15 years' , '4' => 'over 15 years');
        $option_experience = '';
        $e =  sget($_REQUEST,'experience');
        foreach($Experience as $key => $value)
        {
            $selected = '';
            if($e == $key )$selected = 'selected';
            $option_experience .= "<option value=$key $selected>$value</option>";
        }
        $td_experience = "\n<select name=experience><option value=''>Years of Experience</option>$option_experience</select>";
        
        $td_followup_from = "\n<input type=text name=followup_from value='$followup_from' class='datepicker'>";
        
        $Leads = array('high' => 'high',
                       'medium' => 'medium',
                       'low' => 'low',
                       'unlikely' => 'unlikely');
        $ql = sget($_REQUEST,'ql');
        $options_lead = '';
        foreach($Leads as $key => $val)
        {
            $selected = '';
            if($ql == $key)$selected = 'selected';
            $options_lead .= "<option value='$key' $selected>$val</option>";
        }
        $td_lead = "\n<select name=ql><option value=''>Qualified Lead</option>$options_lead</select>";
        
        $td_state = "\n<select name=state><option value=''>State</option>";
        
        $STATES['0']='State';
        $state = sget($_REQUEST, 'state');
        
        foreach($STATES as $key => $name) {
            $selected = '';
            if($state == $key) {
                $selected = "selected";
            }
           
            $td_state .= "<option value=$key $selected>$name</option>\n";
        }
        $td_state .= "</select></td>";
        
        $td_managers = '';
        if( $AccessObj->user_type == 'super')
        {
            $Managers = Mae_users::all_admins();
            array_unshift($Managers, array('uid' => 'none', 'first_name' => '*not assigned', 'last_name' => ''));
            $options_manager = '';
            $posted_managerid = sget($_REQUEST, 'managerid');
            foreach($Managers as $Admin)
            {
                $selected = '';
                $admin_id = sget($Admin,'uid');
                $name = ucwords(sget($Admin,'first_name')) . " " . ucwords(sget($Admin,'last_name'));
                if(($admin_id == $posted_managerid) && $posted_managerid) $selected = "selected";
                $options_manager .= "<option $selected value='$admin_id'>$name</option>\n";
            }
            $td_managers .= "<select name=managerid>
                                <option  value=''>Relationship Manager</option>
                                <option></option>$options_manager</select>";
        }
        
        
        //$section3 = "\n<td><!--input type=text name=experience_high value='$experience_high' --></td>";
        
        $td_followup_to = "\n<input type=text name=followup_to value='$followup_to' class='datepicker' />";

        
        ////////////////
        $submit_btn = "<input type=submit value=Filter class='button' >";
    
    //    if(isset($_REQUEST['update_section_data']) && empty($user_type) ) $html .= batch_edit_panel();
    //    $html .= "</form>";
        
        
        $html .= <<< END
        <form action='/account' method='post' id='search_form'>
            <input type=hidden name=tab value=research>
            <input type=hidden name=type value=people>
            <input type=hidden name=update_section value='' id='update_section'>
            <input type=hidden name=update_section_data value='' id='update_section_data' />
            <input type=hidden name='_page' id='_page' value='$_page' />
        
            <table cellspaing=0 cellpadding=0 class='search-tabs' style='width:770px;'>
                <tr>
                    <td colspan=5>$td_pro_name</td>
                </tr>
                <tr>
                    <td>$td_industry</td>
                    <td>$td_role</td>
                    <td>$td_followup_from</td>
                    <td>$td_experience</td>
                    <td></td>
                </tr>
                <tr>
                    <td>$td_lead</td>
                    <td>$td_state</td>
                    <td>$td_followup_to</td>
                    <td>$td_managers</td>
                    <td>$submit_btn</td>
                </tr>
            
            </table>
        </form>
            
END;
    }
$html .= <<< END
<script type='text/javascript'>
$(document).ready(function(){
    init_search_selector();
    init_select_all();
    init_datepicker();
   init_search_pagination();
})
</script>
END;
    return $html;
}
function get_research_company()
{
    
    $letter = sget($_REQUEST,'letter');
    
    $company = sget($_REQUEST,'company');
    if(strcasecmp($company, 'company') == 0 )$company = '';
    
    $sic = sget($_REQUEST,'sic');
    if($sic) $sic = preg_replace('/[^0-9]/','',$sic);
    
    $revenue_min_pam = sget($_REQUEST, 'revenue_min');
    $revenue_max_pam = sget($_REQUEST, 'revenue_max');
    
    if($revenue_min_pam ) $revenue_min = convert_mil($revenue_min_pam);
    if($revenue_max_pam ) $revenue_max = convert_mil($revenue_max_pam);
    
    
    $state = sget($_REQUEST, 'state');
    
    $priority = sget($_REQUEST,'priority');
    $managerid = sget($_REQUEST,'managerid');
    
    $followup_from = sget($_REQUEST,'followup_from');
    $followup_to = sget($_REQUEST,'followup_to');
    if($followup_from == 'FollowUp - From')$followup_from = '';
    if($followup_to == 'FollowUp - To') $followup_to = '';
    
    $_page = sget($_REQUEST, '_page');
    if(empty($_page)) $_page = 1;
    $limit = 20;
    
    $sql_where = array();
    if($letter  )
    {
        if(strcasecmp('x', $letter) != 0) $sql_where[] = "company like '$letter%%'";
        else{
            $sql_where[] = "company like 'y%%' OR company like 'x%%' OR company like 'z%%' ";
        }
    }
    //test_array($_REQUEST);
    if($sic ) $sql_where[] = "sic like '$sic%%'";
    if($revenue_min )   $sql_where[] = "revenue >= $revenue_min";
    if($revenue_max )   $sql_where[] = "revenue <= $revenue_max";
    if($state )         $sql_where[] = "state = '$state'";
    if($priority ) $sql_where[] = "priority = '$priority'";
   
    
    $sql_where[] = " (delete_marker != 1 OR delete_marker is NULL)";
    global $user_type;
    global $user;
    if( (isset($user_type) && $user_type == 'admin') )
    {

        $uid = $user->uid;
        $sql =  "select * from research_company_manager where managerid = $uid";
        $result = db_query($sql);
        $Groupid = array(-1);
        while(($row = db_fetch_object($result)) !== false)
        {
            $Groupid[] = $row->groupid; 
        }
        $sql_where[] = "groupid in(".implode(',', $Groupid) .")";
        
    }
    if($company ){
        $C = explode(" ", $company);
        $D = array();
        foreach($C as $key => $value)
        {
            $value = trim($value);
            if($value) $D[] = "company like '%%" . $value . "%%'";
        }
        $sql_where[] = "(" . implode(" or " , $D) . ")";
    }
    if($managerid ){
        $ManagerId = array(-1);
        $sql_group = "select * from research_company_manager where managerid = $managerid";
        $result_group = db_query($sql_group);
        if(db_affected_rows($result_group) > 0)
        {
            while(($row = db_fetch_object($result_group)))
            {
                $ManagerId[] = $row->groupid;
            }
        }
        if(count($ManagerId) > 0) $sql_where[] = "groupid is not null and groupid in (". implode(',', $ManagerId) .")";
    }
    $SQL_JOIN = '' ;

    $fA = array();
    if($followup_from){
        $followup_from =  strtotime($followup_from);
        $fA[] = "research_company_data.data_attr >= $followup_from";
    }
    if($followup_to){
        $followup_to = strtotime($followup_to);
        $fA[] = "research_company_data.data_attr <= $followup_to";
    }
    $fA_where = '';
    if(count($fA) > 0)
    {
        $sql_where = array_merge($sql_where, $fA);
        $sql_where[] = "research_company.recordid = research_company_data.recordid and (research_company_data.data_type = 'followup' OR  research_company_data.data_type = 'progress' and data_attr is not null)";
        $SQL_JOIN = ', research_company_data ';
    }else{
        $SQL_JOIN = " left join research_company_data on research_company_data.dataid = ";
        $SQL_JOIN .= "(select dataid from research_company_data where research_company_data.recordid = research_company.recordid and (research_company_data.data_type = 'followup' OR  research_company_data.data_type = 'progress' and data_attr is not NULL) order by data_attr limit 1) ";
    }

    if(count($fA) <= 0)$total_sql = "select count(*) as total from research_company  "; //$SQL_JOIN";
    else $total_sql = "select count(*) as total from research_company $SQL_JOIN";
    
    if(count($sql_where) > 0) $total_sql .= ' where ' . implode(" and ", $sql_where);
    //test_array($_REQUEST);
    //echo $total_sql;
    $result = db_query($total_sql);
    $row = db_fetch_object($result);

    //test_array($sql_where);
    $total = $row->total;
    //echo $total_sql;
    if($total == 0) return "Found 0 Result";
    //echo $total;
    $detail_array = array(
                      'company' => "Company",
                      'sic_desc'    => 'Description',
                      'sic'  => 'Sector',
                      'state'   => 'State',
                      'revenue' => 'Revenue',
                      'contact' => 'Manager',
                      'position' => 'Position',
                      'gender'  => 'M/F',
                      'phone'   =>'Phone',
                      'email'   => 'Email',
                      'data_attr'   => 'Follow Up',
                      );
    $html = '<div style="text-align:right"><input type=button id="table_collapse" value="show contact info"></div>' .
                '<table cellspacing=0 cellpadding=0 border=0 width=100% class="list"><thead><th class="checkbox" align=left><input type=checkbox id="select_all" ></th>';
    foreach($detail_array as $name)
    {
        $class = '';
        if($name == 'Sector' || $name == 'Description') $class = "class='hidea'";
        elseif( in_array($name, array('Manager','Position','M/F','Phone','Email' )))$class = "class='hideb'";
        elseif($name == 'State')$class ="align=center class='hidea'";
        elseif($name == 'Revenue' || $name == 'followup_date')$class ="align=center";

        $html .= "<td $class>$name</td>";
    }
    $html .= "<td></td></tr></thead><tbody>
                <tr>
                    <td width=20></td>
                    <td width=250></td>
                    <td  class='hidea'></td>
                    <td width=50 class='hidea'></td>
                    <td width=40 class='hidea'></td>
                    <td width=70></td>
                    <td width=120 class='hideb'></td>
                    <td width=80 class='hideb'></td>
                    <td width=50 class='hideb'></td>
                    <td width=90    class='hideb'></td>
                    <td width=150 class='hideb'></td>
                    <td width=80></td>
                    <td widt=26></td>
                </tr>";
    
    $start = ($_page - 1) * $limit;
    $sql = "select *, research_company.recordid as companyid from research_company $SQL_JOIN";
    if(count($sql_where) >0 )  $sql .= " where " . implode(" and ", $sql_where);
    if(count($fA) == 0) $sql .= " order by company limit $start, $limit ";
    else{  $sql .= " order by research_company_data.data_attr limit $start, $limit "; }
    //test_array($_REQUEST);
    //echo $sql;
    $result = db_query($sql);
    //var_dump($result);
    $class='';
    if(! empty($result))
    {
        while(($row = db_fetch_array($result) ) !== false)
        {
           
            
            $recordid = sget($row, 'companyid');
            //test_array( $row); exit;
            $html .= "\n<tr><td><input type=checkbox name=records[] value='$recordid' class='records' /></td>";
            foreach($detail_array as $key=> $name)
            {
                
                
                $class = '';
                if($name == 'Sector' || $name == 'Description') $class = "class='hidea'";
                elseif( in_array($name, array('Manager','Position','M/F','Phone','Email' )))$class = "class='hideb'";
                
                if($key == 'company'){
                    $value = sget($row, $key);
                    $html .= "<td><a href='/account?tab=research&page=research_company_detail&recordid=$recordid'>" . htmlentities($value) ."</a></td>";
                    continue;
                }elseif($key == 'revenue')
                {
                    $value = sget($row, 'revenue');
                    if($value) $value = number_format($value);
                    $html .= "<td class='textright' style='padding-right:10px;'>$value</td>";
                    continue;
                }elseif($key == 'state')
                {
                    $class="align=center class='hidea'";
                }elseif($key == 'data_attr')
                {
                    $value = sget($row, $key);
                    if($value) $value = date('m/d/Y', $value);
                    $html .= "<td>$value</td>";
                    continue;
                }
                $value = sget($row, $key);
                $html .= "<td $class>" . htmlentities($value) . "</td>";
            }
            $_page = sget($_REQUEST,'_page');
            $html .= "<td><img src='/". path_to_theme() ."/images/edit-icon.png' width=14 style='vertical-align:middle;cursor:pointer' data='$recordid' class='research-edit-icon' page='$_page'></td></tr>\n";
        }

        if( (sget($_REQUEST,'group_result') == 'batch assign') )
        {
            perform_grouping('company', $sql_where);
        }
    }
    $html .= "</tbody></table>";
    $Pagenation = array('total' => $total,
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

function update_research_edit_company($op = null)
{
    if($op == 'update')
    {
        $dataid = sget($_REQUEST,'dataid');
        if(empty($dataid)) {
            echo "Invalid data Id";
            return;
        }
        $company = sget($_REQUEST, 'edit_company');
        $desc = sget($_REQUEST, 'edit_desc');
        $state = sget($_REQUEST, 'edit_state');
        $owner = sget($_REQUEST, 'edit_owner');
        $position = sget($_REQUEST, 'edit_position');
        $gender = sget($_REQUEST, 'edit_gender');
        $phone = sget($_REQUEST, 'edit_phone');
        $email = sget($_REQUEST, 'edit_email');
        $revenue = sget($_REQUEST, 'edit_revenue');
        $sql = "update research_company set company = '%s', sic_desc = '%s', state='%s', contact = '%s', position = '%s', gender = '%s', phone = '%s', email='%s', revenue='%s' where recordid = %d limit 1";
        
        if(db_query($sql , array($company, $desc, $state, $owner, $position, $gender, $phone, $email, $revenue, $dataid)))
        {
            drupal_set_message("operation successful.");
        }else{
            drupal_set_message('Failed to update record','error');
        }
    }
}


////////////////////////////// SEARCH PEOPLE



function get_research_people($op = null)
{
    $letter = sget($_REQUEST,'letter');
    $name = sget($_REQUEST, 'name');
    $industry = sget($_REQUEST,'industry');
    $role = sget($_REQUEST,'role');
    $ql = sget($_REQUEST,'ql');
    $experience_low = sget($_REQUEST,'experience_low');
    $experience_high = sget($_REQUEST,'experience_high');
    $state = sget($_REQUEST,'state');
    $followup_from = sget($_REQUEST,'followup_from');
    $followup_to = sget($_REQUEST,'followup_to');
    $experience = sget($_REQUEST, 'experience');
    $managerid = sget($_REQUEST,'managerid');
    
    if(strcasecmp($state, 'state') == 0) $state = '';
    if(strcasecmp($name, 'name') == 0) $name = '';
    if(strcasecmp($industry,'industry') == 0 )$industry = '';
    if(strcasecmp($role, 'role') == 0 )$role = '';
    if(strcasecmp($experience_low, 'Years of Experience - Low') == 0 )$experience_low = '';
    if(strcasecmp($experience_high, 'Years of Experience - High') == 0 )$experience_high = '';
    if(strcasecmp($followup_from, 'Follow Up - from') == 0 )$followup_from = '';
    if(strcasecmp($followup_to, 'Follow Up - to') == 0 )$followup_to = '';
    if(strcasecmp($experience, 'years of experience') == 0) $experience = '';
    if(strcasecmp($managerid, 'relationship manager') == 0) $managerid = '';
   
    $_page = sget($_REQUEST, '_page');
    if(empty($_page)) $_page = 1;
    $limit = 20;
    $thisYear = intval(date("Y"));
    
    $sql_where = array();
    global $user_type;
    global $user;
    if( (isset($user_type) && $user_type == 'admin') )
    {

        $uid = $user->uid;
        $sql =  "select * from research_people_manager where managerid = $uid";
        $result = db_query($sql);
        $Groupid = array(-1);
        while(($row = db_fetch_object($result)) !== false)
        {
            $Groupid[] = $row->groupid; 
        }
        $sql_where[] = "groupid in(".implode(',', $Groupid) .")";
        
    }
    if($letter  )
    {
        if(strcasecmp('x', $letter) != 0) $sql_where[] = "lastname like '$letter%%'";
        else{
            $sql_where[] = "lastname like 'y%%' OR lastname like 'x%%' OR lastname like 'z%%' ";
        }
    }
    
    if($name ){
        $Name = explode(" ", $name);
        $R = array();
        foreach($Name as $n)
        {
            $n = trim($n);
            if( ! empty($n)) $R[] = "(lastname like '%%$n%%' or firstname like '%%$n%%' ) ";
        }
        $sql_where[] = "(" . implode(" and ", $R) .")";
    }
    if($industry) $sql_where[] = "industry = '$industry'";
    if($role)$sql_where[] = "position like '%%$role%%'";
    if($ql)$sql_where[] = "prospect like '%%$ql%%'";
    if($experience){
        
        switch($experience){
            case '1':
                $low = $thisYear - 0;
                $high = $thisYear - 5;
                $sql_where[] = "((experience * 1) >= $high)";
                break;
            case '2':
                $low = $thisYear - 6;
                $high = $thisYear - 10;
                $sql_where[] = "((experience * 1) >= $high and (experience * 1) <= $low)";
                break;
            case '3':
                $low = $thisYear - 11;
                $high = $thisYear - 15;
                $sql_where[] = "((experience * 1) >= $high and (experience * 1) <= $low)";
                break;
            case '4':
                $high = $thisYear - 16;
                $sql_where[] = "((experience * 1) <= $high and (experience * 1 >= 1000) )";
                break;
            default:
                break;
            
        }
    }
    if($state)$sql_where[] = "state = '$state'";
    //if($followup_low)$sql_where[] = "experience >= " . intval($experience_low);
    
    if($managerid ){
        $ManagerId = array(-1);
        $sql_group = "select * from research_people_manager where managerid = $managerid";
        $result_group = db_query($sql_group);
        if(db_affected_rows($result_group) > 0)
        {
            while(($row = db_fetch_object($result_group)) !== false)
            {
                $ManagerId[] = $row->groupid;
            }
        }
        if(count($ManagerId) > 0) $sql_where[] = "groupid is not null and groupid in (". implode(',', $ManagerId) .")";
    }

    $SQL_JOIN = '' ;
    $fA = array();
    if($followup_from){
        $followup_from =  strtotime($followup_from);
        $fA[] = "research_people_data.data_attr >= $followup_from";
    }
    if($followup_to){
        $followup_to = strtotime($followup_to);
        $fA[] = "research_people_data.data_attr <= $followup_to";
    }
    $fA_where = '';
    if(count($fA) > 0)
    {
        $sql_where = array_merge($sql_where, $fA);
        $sql_where[] = "research_people.pid = research_people_data.pid and (research_people_data.data_type = 'followup' OR  research_people_data.data_type = 'progress' and data_attr is not null)";
        $SQL_JOIN = ', research_people_data ';
    }else{
        $SQL_JOIN = " left join research_people_data on research_people_data.dataid = ";
        $SQL_JOIN .= "(select dataid from research_people_data where research_people_data.pid = research_people.pid and (research_people_data.data_type = 'followup' OR  research_people_data.data_type = 'progress' and data_attr is not NULL) order by data_attr limit 1) ";
    }
    
    if(count($fA) <= 0)$total_sql = "select count(*) as total from research_people  "; //$SQL_JOIN";
    else $total_sql = "select count(*) as total from research_people $SQL_JOIN";
    //test_array($fA);
    $sql_where[] = " (delete_marker != 1 OR delete_marker is NULL)";
    $str_sql_where = implode(' and ', $sql_where);

    //////////////
    //$total_sql = "select count(*) as total from research_people ";
    if(count($sql_where) > 0 ) $total_sql .= " where " . $str_sql_where;
    //echo $total_sql . "<hr>";
    $result = db_query($total_sql);
    $row = db_fetch_object($result);


    $total = $row->total;
    //echo $total_sql;
    if($total == 0) return "Found 0 Result";
    //echo $total;
    $detail_array = array(
                      'firstname' => "First Name",
                      'lastname' => "Last Name",
                      'industry'    => 'Industry',
                      'experience'  => 'Yrs of Exp.',
                      'position'   => 'Role',
                      'state' => 'State',
                      'gender' => 'M/F',
                      'phone' => 'Phone',
                      'email'   => 'Email',
                      'data_attr'   => 'Follow Up',
                      'detail' => 'Detail',
                      );
    $html =  '<div style="text-align:right"><input type=button id="table_collapse" value="show contact info"></div>' .
    '<table cellspacing=0 cellpadding=0 border=0><thead><th><input type=checkbox id="select_all" ></th>';
    foreach($detail_array as $key => $name)
    {
        
        $class = '';
        if(in_array($key, array('gender','email','phone'))){
            $class = 'hideb';
        }elseif(in_array($key, array('state', 'experience', 'position'))){
            $class = 'hidea';
        }
        $html .= "<td class='$class'>$name</td>";
    }
    $html .= "</tr></thead><tbody>";
    
    $start = ($_page - 1) * $limit;
    $sql = "select *,research_people.pid as peopleid from research_people $SQL_JOIN";
    if(count($sql_where) >0 )  $sql .= " where " . implode(" and ", $sql_where);
    if(count($fA) == 0) $sql .= " order by lastname limit $start, $limit ";
    else{  $sql .= " order by research_people_data.data_attr limit $start, $limit "; }
    
    //echo $sql;
    
    
    $result = db_query($sql);
    //var_dump($result);
    if(! empty($result))
    {
        
        while(($row = db_fetch_array($result) ) !== false)
        {
            //test_array($row);exit;
            $pid = sget($row, 'peopleid');
            $html .= "\n<tr><td width=20><input type=checkbox name=records[] value='$pid' class='records' /></td>";
            foreach($detail_array as $key=> $name)
            {
                $class = '';
                $value = sget($row, $key);
                if($key == 'detail'){
                    $html .= "<td><a href='/account?tab=research&page=research_people_detail&pid=$pid'>detail</a></td>";
                    continue;
                }elseif(in_array($key, array('gender','email','phone'))){
                    $class = 'hideb';
                }elseif(in_array($key, array('state',  'position'))){
                    $class = 'hidea';
                }elseif($key == 'experience'){
                    $class = 'hidea';
                    if($value) $value = $thisYear - intval($value);
                }elseif($key == 'data_attr'){
                    if($value) $value = date("m/d/Y",$value);
                }
               
                $html .= "<td class='$class'>" . htmlentities($value) . "</td>";
            }
            $html .= "</tr>";
        }
        if( (sget($_REQUEST,'group_result') == 'batch assign') )
        {
            perform_grouping('people', $sql_where);
        }
    }
    $html .= "</tbody></table>";
    $Pagenation = array('total' => $total,
        'limit' =>$limit,
        'baseurl' => "/account?",
        'num_of_links' => 8,
    );
    $html.= '<table class="no-border"><tr border=0><td>
            <input type=button class=button value="delete" id="delete_btn" />
            </td><td border=0>' .
            "<div style='width:100%;'>" .
            pagination($Pagenation) . "</div>" .
            '</td></tr></table>';
        
    return $html;
}




function batch_edit_panel()
{
    $type = sget($_REQUEST,'type');
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
        $sections  .= "<td >Set Prospect Level: <select name='batch_priority'><option></option>$priority_html</select></td>";
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
        $sql = "insert into research_company_manager (managerid, title, manager_type) values ('%d','%s','%s')";
        $sql_data = array($managerid, $title, $manager_type);
        if( ! db_query($sql, $sql_data))
        {
            drupal_set_message('Failed to add new group.','error');
            return ;
        }
        $last_id = db_last_insert_id("research_company_manager", 'groupid');
        if( empty($last_id ))
        {
            drupal_set_message('Failed to add new group. reference: last id','error');
            return ;
        }
        
        $batch_priority = sget($_REQUEST, 'batch_priority');
        //test_array($_REQUEST);
        if(empty($batch_priority) )  $sql = "update research_company set groupid = $last_id " . " where " . implode(" and ", $sql_where);
        else $sql = "update research_company set priority = '$batch_priority', groupid = $last_id " . " where " . implode(" and ", $sql_where);
        echo $sql;
        if(db_query($sql))
        {
            drupal_set_message("operation successful.");
        }else{
            drupal_set_message('Failed to add companies to the group','error');
        }

    }elseif($table == 'people')
    {
        if(count($sql_where) <= 0)
        {
            drupal_set_message('Please check search terms and try again ','error');
            return;
        }
        $title = sget($_REQUEST,'group_title');
        $managerid = sget($_REQUEST,'group_manager_id');
        $manager_type = 'manager';
        $sql = "insert into research_people_manager (managerid, title, manager_type) values ('%d','%s','%s')";
        $sql_data = array($managerid, $title, $manager_type);
        
        if( ! db_query($sql, $sql_data))
        {
            drupal_set_message('Failed to add new group.','error');
            return ;
        }
        $last_id = db_last_insert_id("research_people_manager", 'groupid');
        if( empty($last_id ))
        {
            drupal_set_message('Failed to add new group. reference: last id','error');
            return ;
        }
        
        $batch_priority = sget($_REQUEST, 'batch_priority');
        //test_array($_REQUEST);
        if(empty($batch_priority) )  $sql = "update research_people set groupid = $last_id " . " where " . implode(" and ", $sql_where);
        else $sql = "update research_people set priority = '$batch_priority', groupid = $last_id " . " where " . implode(" and ", $sql_where);
     
        if(db_query($sql))
        {
            drupal_set_message("operation successful.");
        }else{
            drupal_set_message('Failed to add companies to the group','error');
        }
    }
    return $return_html;
}

/* EOF */