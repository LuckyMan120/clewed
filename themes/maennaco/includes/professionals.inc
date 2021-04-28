<?php
function pro_list($op = null)
{
    $Block['title'] = 'Professionals';

    if($op == 'view' || $op == null){
        $content = get_maenna_tabs();
        $content .= get_maenna_people('people');
        
        
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}

function get_maenna_tabs($type)
{
    //test_array($_REQUEST);
    global $user_type;
    $html = '';
    
    
    $Prospects = array('high','medium','low','unlikely');
    
        //test_array($_REQUEST);
       $html .= "<form action='' method='get' id='search_form'>
       <input type=hidden name=a value=professionals>
       <input type=hidden name=type value=people>
       <input type=hidden name=update_section value='' id='update_section'>
       <input type=hidden name=update_section_data value='' id='update_section_data' />";
        
        $html .= "<table cellspaing=0 cellpadding=0 class='search-tabs' style='border-bottom:solid 2px #cccccc;width:770px;'>";
        
        $name = 'Name';
        if(sget($_REQUEST,'name')) $name = sget($_REQUEST,'name');
        
        // Search Tab - first row
        $section = "<input type=text name=name value='$name'>";
        $Letters =  "abcdeghijklmnopqrstuvw";
        $section .= "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        for($i = 0; $i < strlen($Letters); $i++)
        {
            $l = strtoupper($Letters[$i]);
            $section .= "<a href='#' class='letter_selector' data=$l>$l</a>&nbsp;";
        }
        $letter = sget($_REQUEST,'letter') ? sget($_REQUEST,'letter') : '';
        $section .= "<a href='#' class='letter_selector' data=x style='letter-spacing:0;'>XYZ</a>";
        $html .= "<tr><td colspan=4><input type=hidden name=letter value='' id='letter' value='$letter' />$section<td></tr>";
        //// Second Row //////////////////////
        
        $proid = sget($_REQUEST, 'proid');
        if(empty($proid)) $proid = 'Professional ID';
        $sel_proid = "<input type=text name=proid value='$proid' />";
        $section1 = "\n<td>$sel_proid</td>";
        
        $industry = (sget($_REQUEST,'industry')) ? sget($_REQUEST,'industry') : 'Industry';
        $_Industry = _INDUSTRY();
        $options_industry = '';
        foreach($_Industry as $key => $array){
            $options_industry .= "\n<optgroup label='$key'>";
            foreach($array as $kk=> $val){
                $selected = '';
                if($industry == $kk)$selected = 'selected';
                $options_industry .= "\n<option value='$kk' $selected>$val</option>";
            }
            $options_industry .= "\n</optgroup>";
        }
        $section2 = "<td ><select name=industry><option value=''>Industry</option>$options_industry</select></td>";
        
        // Experties
        /*
        $_Roles = _ROLES();
        $role = (sget($_REQUEST,'role')) ? sget($_REQUEST,'role') : 'Role';
        $option_role = '';
        foreach($_Roles as $r)
        {
            $selected = '';
            if($r == $role) $selected = 'selected';
            $option_role .= "\n<option $selected>$r</option>";
        }
        */
        $experties = sget($_REQUEST, 'experties');
        $_Experties = _experties();
        $option_experties = '';
        foreach($_Experties as $key=>$array)
        {
            if(empty($key)) continue;
            $option_experties .= "\n<optgroup label='$key'>";
            foreach($array as $k => $val)
            {
                $selected = '';
                if($experties == $k)$selected = 'selected';
                $option_experties .= "\n<option value='$k' $selected>$val</option>";
            }
            $option_experties .= "\n</optgroup>";
        
        }
        $section3 = "\n<td><select name=experties><option value=''>Expertise</option>$option_experties</td>";
        
        // Row 2 - Experience
        /*
        $Experience = array("1" => '1 - 5 year', '2' => '5-10 years', '3' => '10 to 15 years' , '4' => 'over 15 years');
        $experience_html = '';
        $e =  sget($_REQUEST,'experience');
        foreach($Experience as $key => $value)
        {
            $selected = '';
            if($e == $key )$selected = 'selected';
            $experience_html .= "<option value=$key $selected>$value</option>";
        }
        $section3 = "\n<td><select name=experience><option value=''>Years of Experience</option>$experience_html</select></td>";
        */
        
        // Row 2 - State
        $_STATES = get_us_states();
        $state = sget($_REQUEST, 'state');
        $option_states = '';
        foreach($_STATES as $key=>$val)
        {
            if(empty($key)) continue;
            $selected = '';
            if($state == $key) $selected = 'selected';
            $option_states .= "\n<option value='$key' $selected>$val</option>";
        }
        
        $section4 = "\n<td><select name=state><option value=''>State</option>$option_states</select></td>";
        
        $eventFrom = sget($_REQUEST, 'eventFrom');
        if(empty($eventFrom))$eventFrom = 'Advisory Event From';
        $section5 = "<td><input type=text name=eventFrom value='$eventFrom' class='datepicker' /></td>";
        $html .="<tr>$section1  $section2 $section3 $section4 $section5</tr>";
        
        
        
        /// Row 3 - 1 professional type///////////
        $proType = sget($_REQUEST, 'proType');
        $_ProType = _pro_roles();
        $option_proType = '';
        foreach($_ProType as $key=>$val)
        {
            $selected = '';
            if($key == $proType )$selected = 'selected';
            $option_proType .= "\n<option value='$key' $selected>$val</option>";
        }
        $section1 = "\n<td><select name=proType><option value=''>Professional Type</option>$option_proType</select></td>";
        
        // Row 3 - management level
        $_ManagementLevel = _managementLevel();
        $mLvl = sget($_REQUEST, 'managementLevel');
        $option_mLvl = '';
        foreach($_ManagementLevel as $key => $val)
        {
            $selected = '';
            if($key == '') continue;
            if($mLvl === $key)$selected = 'selected';
            $option_mLvl .= "\n<option value='$key' $selected>$val</option>";
        }
        $section2 = "\n<td><select name=managementLevel><option value=''>Management Level</option>$option_mLvl</select></td>";
        
        // Row3  - PL Responsibility
        $_PLRes =_plResponsibility();
        $plres = sget($_REQUEST, 'plres');
        
        $option_plres = '';
        foreach($_PLRes as $key => $val)
        {
            $selected = '';
            if($key =='') continue;
            if($key == $plres)$selected = 'selected';
            $option_plres .= "\n<option value='$key' $selected>$val</option>";
        }
        
        $section3 = "\n<td><select name='plres'><option value=''>PL Responsibility</option>$option_plres</select></td>";
        
        
        //$section3 = "\n<td><!--input type=text name=experience_high value='$experience_high' --></td>";
        $rating = sget($_REQUEST, 'rating');
        $sel_rating = "<select name=rating><option value=''>Rating</option>";
        $array = array('1' => '0 - 1',
                       '2' => '1 - 2',
                       '3' => '2 - 3',
                       '4' => '3 - 4',
                       '5' => '4 - 5');
        
        foreach($array as $key => $val)
        {
            $selected = '';
            if($rating == $key) $selected = 'selected';
            $sel_rating .= "\n<option value=$key $selected>$val</option>";
        }
        $sel_rating .= "</select>";
        $section4 = "\n<td>$sel_rating</td>";
        
        $eventTo = sget($_REQUEST, 'eventTo');
        if(empty($eventTo))$eventTo = 'Advisory Event To';
        $section5 = "<td><input type=text name=eventTo value='$eventTo' class='datepicker' /></td>";
        $html .="<tr>$section1  $section2 $section3 $section4 $section5</tr>";        
        ////////////////
        $html .= "<tr><td></td><td></td><td></td><td></td><td><input type=submit value=Filter class='button' ></td></tr>";
        $html .= "</table>";
       // if(isset($_REQUEST['update_section_data']) && empty($user_type) ) $html .= batch_edit_panel();
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

function get_maenna_people($op = null)
{

    
    
    $SearchKeys = array(
                  'letter',
                  'name',
                  'proid',
                  'industry',
                  'experties',
                  'state',
                  'proType',
                  'managementLevel',
                  'plres',
                  'rating',
                  'eventFrom',
                  'eventTo'
                  );
    $SearchValues = array();
    foreach($SearchKeys as $key)
    {
        if(sget($_REQUEST, $key) === '')continue;
        $SearchValues["$key"] = sget($_REQUEST, $key);
    }
    $SQL_Where = array();
    $thisYear = date("Y");

    foreach($SearchValues as $key=> $val)
    {
        switch($key)
        {
            case 'name':
                if(strcasecmp($val,'name') == 0){
                    $val = '';
                }else{
                    $val = "((maenna_people.firstname like '%".($val) ."%') or (maenna_people.lastname like '%${val}%'))";    
                }
                break;
            case 'industry':
                $val = "maenna_people.industry = '$val'";
                break;
            case 'experties':
                $val = "maenna_people.experties = '$val'";
                break;
            case 'proType':
                $val = "(users_roles.rid = '$val')";
                break;
            case 'managementLevel':
                $val = "maenna_people.mlvl = '$val'";
                break;
            case 'plres':
                $val = "(maenna_people.plres = '$val' and maenna_people.plres is not null)";
                break;
            case 'experience':
                $year5 = $thisYear - 5;
                $year10 = $thisYear - 10;
                $year15 = $thisYear - 15;
                if($val == 1) $val = "maenna_people.yearwork >= $year5";
                elseif($val == 2) $val = "(maenna_people.yearwork >= $year10 and maenna_people.yearwork <= $year5)";
                elseif($val == 3) $val = "(maenna_people.yearwork >= $year15 and maenna_people.yearwork <= $year10)";
                elseif($val == 4) $val = "maenna_people.yearwork <= $year15";
                else break;
                break;
            case 'state':
                $val = "maenna_people.state = '$val'";
                break;
            case 'letter':
                $val = "maenna_people.lastname like '".filter_text($val) ."%'";
                break;
            case 'rating':
                $val = "(maenna_people.rating is not null and maenna_people.rating >= " .($val - 1) . " and maenna_people.rating < " .$val. ")";
                break;
            case 'eventFrom':
                if(strcasecmp('advisory event from', $val) == 0) $val = '';
                else{
                    $val = '';
                }
                break;
            case 'eventTo':
                if(strcasecmp('advisory event to', $val) == 0) $val = '';
                else{
                    $val = '';
                }
                break;
            case 'proid':
                if(strcasecmp('Professional ID', $val) == 0) $val = '';
                else{
                    
                    $proid = intval(filter_num($val)) - 100;
                    $val = "maenna_people.pid = $proid";
                }
                break;
            default:
                continue;
                break;
        }
        if(! empty($val)) $SQL_Where["$key"] = $val;
    }
    $SQL_Where[] = " (delete_marker != 1 OR delete_marker is NULL)";
    $SQL_Where[] = " users.uid = maenna_people.pid";
    $SQL_Where[] = " users.uid = users_roles.uid";

    
    global $user_type;
    global $user;
    
    $str_sqlWhere = ' where ' . implode(" and ", $SQL_Where);
    
    $sql_total = "select count(*) as total from maenna_people, users, users_roles " . $str_sqlWhere;
    //echo $sql_total;
    $result = db_query($sql_total);
    $row = db_fetch_object($result);
    $total = $row->total;
    //echo $total_sql;
    
    // finialize SQL before sql_total returns
    $page = sget($_REQUEST, 'page');
    if(empty($page)) $page = 1;
    $limit = 20;
    $start = ($page -1 ) * $limit;
    $thisYear = intval(date("Y"));
    
    $sql = "select users.created as register, users.uid, maenna_people.*, users_roles.rid from users,maenna_people,users_roles " . $str_sqlWhere . " order by created desc limit $start, $limit";
    echo $sql;
    // total_sql  return if 0
    
    if($total == 0) return "Found 0 Result";
    
    
    
    $detail_array = array(
                    'checkbox' => '',
                    'proid' => 'Pro ID',
                    'rid' => 'Type',
                    'industry'    => 'Industry',
                    'experties'  => 'Experties',
                    'mlvl' => 'Level',
                    'yearwork' => 'Yrs',
                    'rating' => 'Rating',
                    'state' => 'State',
                    'followercnt' => 'Followers',
                    'advisoryhrs' => 'Advisory Hrs',
                    'referralcnt' => 'Referrals',
                    'name' => 'Name', 
                    'email'   => 'Email',
                    'relmanager' => 'Relationship Manager',
                    'register' => "Registered",
                    'advisorydate' => 'Advisory Date',
                    'detail' => 'Detail'
                      );
    $Hidea = array('industry',
                   'experties',
                   'mlvl',
                   'yearwork',
                   'rating',
                   'state',
                   'followercnt',
                   'advisoryhrs');
    $Hideb = array('referralcnt',
                   'name',
                   'email',
                   'relmanager',
                   'register');
    $html =  '<div style="text-align:right"><input type=button id="table_collapse" value="show contact info"></div>' .
    '<table cellspacing=0 cellpadding=0 border=0><thead><th><input type=checkbox id="select_all" ></th>';
    foreach($detail_array as $key => $name)
    {
        if($key=='checkbox')continue;
        $class = '';
        if(in_array($key, $Hideb)){
            $class = 'hideb';
        }elseif(in_array($key, $Hidea)){
            $class = 'hidea';
        }
        $html .= "<td class='$class'>$name</td>";
    }
    $html .= "</tr></thead><tbody>";
    
    
    
    
    $result = db_query($sql);
    //var_dump($result);
    $_ProType = _pro_roles(); // numeric index array
    $_Industry = _INDUSTRY();
    $_Experties = _experties();
    $_mLvl = _managementLevel();

    if(! empty($result))
    {
        
        while(($row = db_fetch_array($result) ) !== false)
        {
            $pid = sget($row, 'pid');
            
            foreach($detail_array as $key=> $name)
            {
                $class = '';
                if(in_array($key, $Hideb)){
                    $class = 'hideb';
                }elseif(in_array($key, $Hidea)){
                    $class = 'hidea';
                }
                $created = sget($row, 'register'); 
                
                $value = sget($row, $key);
                
                
                if($key == 'detail'){
                    $html .= "<td><a href='/account?a=professionals&type=people_detail&pid=$pid'>detail</a></td>";
                    continue;
                }elseif($key=='checkbox'){
                    $html .= "\n<tr><td width=20><input type=checkbox name=records[] value='$pid' class='records' /></td>";
                    continue;
                }elseif($key == 'register')
                {
                    $value = date('m/d/Y', $value);
                }elseif($key == 'proid'){
                    $value = getProId($pid);
                }elseif($key == 'rid')
                {
                    $value = $_ProType["$value"];
                }elseif($key == 'industry')
                {
                    if($value && ($created > '1322005040')){
                        foreach($_Industry as $SubArray){
                            foreach($SubArray as $index => $val){
                                if(strcasecmp($index, $value) == 0)
                                {
                                    $value = $val ;
                                    break;
                                }
                            }
                        }
                    }
                    $html .= "<td class='$class'>" . ($value) . "</td>";
                    continue;
                }elseif($key == 'experties')
                {
                    if($value && ($created > '1322005040')){
                        foreach($_Experties as $SubArray){
                            foreach($SubArray as $index => $val){
                                if(strcasecmp($index, $value) == 0)
                                {
                                    $value = $val ;
                                    break;
                                }
                            }
                        }
                    }
                    $html .= "<td class='$class'>" . ($value) . "</td>";
                    continue;
                }elseif($key == 'mlvl'){
                    if($value && ($created > '1322005040')){
                        foreach($_mLvl as $index => $val){
                            if(strcasecmp($index, $value) == 0)
                            {
                                $value = $val ;
                                break;
                            }
                        }
                    }
                }elseif($key == 'yearwork'){
                    if(! empty($value)) $value = $thisYear - $value;
                }elseif($key == 'name'){
                    $value = sget($row, 'lastname') . ', ' . sget($row, 'firstname');
                }elseif($key == 'advisoryhrs'){
                    if(! empty($value)){
                        $hour = floor($value / 60);
                        $minute = $value % 60;
                        $values = "$hour Hrs, $minute Ms";
                    }
                }
                if(empty($value))$value = '-';
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
    
    
        $sections  .= "<tr><td >Name the results as a group <input type=text name=group_title value=''></td>";
        $sections  .= "<td >Assign the group to manager: <select name='group_manager_id'><option value=''>Manager</option>$admin_html</select></td>";
        $sections  .= "<td >Set Prospect Level: <select name='batch_priority'><option></option>$priority_html</select></td>";
        $sections  .= "<td></td><td><input type='submit' name='group_result' value='batch assign' ></td></tr>";
    
    
    $html  = "<div style='padding-left:30px;background:lightblue'><table cellpadding=8 cellspacing=6 class='no-border'>$sections</table></div>";
    return $html;
}
function perform_grouping($table, $sql_where)
{
    $return_html = '';

    
        if(count($sql_where) <= 0)
        {
            drupal_set_message('Please check search terms and try again ','error');
            return;
        }
        $title = sget($_REQUEST,'group_title');
        $managerid = sget($_REQUEST,'group_manager_id');
        $manager_type = 'manager';
        $sql = "insert into maenna_people_manager (managerid, title, manager_type) values ('%d','%s','%s')";
        $sql_data = array($managerid, $title, $manager_type);
        
        if( ! db_query($sql, $sql_data))
        {
            drupal_set_message('Failed to add new group.','error');
            return ;
        }
        $last_id = db_last_insert_id("maenna_people_manager", 'groupid');
        if( empty($last_id ))
        {
            drupal_set_message('Failed to add new group. reference: last id','error');
            return ;
        }
        
        $batch_priority = sget($_REQUEST, 'batch_priority');
        //test_array($_REQUEST);
        if(empty($batch_priority) )  $sql = "update maenna_people set groupid = $last_id " . " where " . implode(" and ", $sql_where);
        else $sql = "update maenna_people set priority = '$batch_priority', groupid = $last_id " . " where " . implode(" and ", $sql_where);
     
        if(db_query($sql))
        {
            drupal_set_message("operation successful.");
        }else{
            drupal_set_message('Failed to add companies to the group','error');
        }
    
    return $return_html;
}



function employment_panel($op = null)
{
    $pid = sget($_REQUEST,'pid');
    $Block['title'] = "Employment";
    global $user;
    $editorid = $user->uid;
    $time = time();
    
    if($op == null || $op == 'view')
    {
        $content = "<div><a href='/account?a=professionals&type=people_detail&pid=$pid'>back to detail</a></div><table>";
        if($pid > 0 )
        {
            $sql = "select * from maenna_people_data where pid = $pid and data_type = 'employment' order by data_attr desc";
            $result = db_query($sql);
            if(db_affected_rows($result) > 0)
            {
                while(($row = db_fetch_object($result)) !== false){
                    $data_attr = $row->data_attr;
                    $org = $row->data_value;
                    $position = $row->data_value2;
                    $start = $row->data_attr;
                    $end = $row->data_value3;
                    //list($start, $end ) = explode('|', $data_attr);
                    //if($start)$start = date('m/y', $start);
                    //if($end)$end = date('m/y', $end) ;//? date('m/y') : "Present";
                    $dataid = $row->dataid;
                    $content .= "\n
                    <tr>
                    <td>$start - $end </td></td>
                    <td>$position</td>
                    <td>$org</td>
                    <td><a href='#' class='edit_wnd' rel='edit_employment_panel' data='$dataid'>edit</a></td>
                    <td><a href='/account?a=professionals&type=people_detail&pid=$pid&sub=work&dataid=${dataid}&do=delete&update_section=employment_panel' onclick='return confirm(\"Continue to remove this record?\")'>delete</a></td>
                    </tr>\n";
                }
            }
        }
        
        $content .= "</table>\n";
        //$content .= "<div style='text-align:right;margin-top:-10px;'><a href='/account?a=research&type=people_detail&pid=8&sub=work' style='font:12px arial'>more</a></div>";
        //$content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_work' data='$pid'>new entry</a></div>";
        $Block['body'] = content_box($Block['title'], $content);
        return $Block;
    }elseif($op = 'update')
    {
        //test_array($_REQUEST);exit;
        $do = sget($_REQUEST,'do');
        switch($do)
        {
            case 'update':
                    $start_month = sget($_REQUEST,'start_month');
                    $start_year = sget($_REQUEST,'start_year');
                    $end_month = sget($_REQUEST,'end_month');
                    $end_year = sget($_REQUEST,'end_year');
                    
                    $start = $start_month .'/'. $start_year;
                    $end = $end_month .'/' . $end_year;
                    $company = sget($_REQUEST,'company');
                    $position = sget($_REQUEST,'position');
                    $dataid = sget($_REQUEST, 'dataid');
                    if(empty($dataid)) {
                        drupal_set_message("Failed to update record." . db_error(),'error');
                        return;
                    }
                    $dataid = intval($dataid);
                    $sql = "update maenna_people_data set data_attr = '%s',data_value3 = '%s', data_value = '%s',data_value2 = '%s',access = '%s',editorid  = '%s' where dataid = %d limit 1";
                    $sql_data = array($start,$end, $company, $position, $time, $editorid, $dataid);
                    //test_array($sql_data);exit;
                    if(db_query($sql, $sql_data))
                        drupal_set_message("Reocrd information is updated.");
                    else
                        drupal_set_message("Failed to update record." . db_error(),'error');
                        
                    return;
                break;
            case 'delete':
                    $dataid = sget($_REQUEST, 'dataid');
                    if(empty($dataid)) {
                        drupal_set_message("Failed to update record." . db_error(),'error');
                        return;
                    }
                    $sql = "delete from maenna_people_data where dataid = %d limit 1";
                    $dataid = intval($dataid);
                    $sql_data = array($dataid);
                    if(db_query($sql, $sql_data))
                        drupal_set_message("The selected record is removed."  );
                    else
                        drupal_set_message("Failed to remove record." . db_error(),'error');
                        
                    return;
                break;
        }
        
    }
}

function education_panel($op = null)
{
    $pid = sget($_REQUEST,'pid');
    $Block['title'] = "Education Background";
    global $user;
    $editorid = $user->uid;
    $time = time();
    
    if($op == null || $op == 'view')
    {
        $content = "<div><a href='/account?a=professionals&type=people_detail&pid=$pid'>back to detail</a></div><table>";
        if($pid > 0 )
        {
            $sql = "select * from maenna_people_data where pid = $pid and data_type = 'education' order by data_attr desc";
            $result = db_query($sql);
            if(db_affected_rows($result) > 0)
            {
                while(($row = db_fetch_object($result)) !== false){
                    $sql = "select * from maenna_people_data where pid = $pid and data_type = 'education' order by (data_attr * 1) desc";
                    $result = db_query($sql);
                    if(db_affected_rows($result) > 0)
                    {
                    
                        while(($row = db_fetch_object($result)) !== false){
                            $year = $row->data_attr;
                            //var_dump($row);exit;
                            //echo "<br>$data_attr";
                            $school = $row->data_value;
                            $study = $row->data_value2;
                            
                            $year = date("Y", $year);
                            $dataid = $row->dataid;
                            $content .= "\n
                            <tr><td >$study</td>
                            <td >$school ($year)</td>
                            <td><a href='#' class='edit_wnd' rel='edit_education_panel' data='$dataid'>edit</a></td>
                            <td><a href='/account?a=professionals&type=people_detail&pid=$pid&sub=education&dataid=${dataid}&do=delete&update_section=education_panel' onclick='return confirm(\"Continue to remove this record?\")'>delete</a></td>
                            </tr>\n";
                        }
                    }
                }
            }
        }
        
        $content .= "</table>\n";
        //$content .= "<div style='text-align:right;margin-top:-10px;'><a href='/account?a=research&type=people_detail&pid=8&sub=work' style='font:12px arial'>more</a></div>";
        //$content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_work' data='$pid'>new entry</a></div>";
        $Block['body'] = content_box($Block['title'], $content);
        return $Block;
    }elseif($op = 'update')
    {
        //test_array($_REQUEST);exit;
        $do = sget($_REQUEST,'do');
        switch($do)
        {
            case 'update':
                
                    $year = sget($_REQUEST,'year');
                    //echo $year;
                    $year = strtotime('1/1/'.$year);
                    //echo " -  " . $year;
                    //echo " .. " .date("Y",$year);
                    $school = sget($_REQUEST,'school');
                    $study = sget($_REQUEST,'study');
                    $dataid = sget($_REQUEST, 'dataid');
                    if(empty($dataid)) {
                        drupal_set_message("Failed to update record." . db_error(),'error');
                        return;
                    }
                    $dataid = intval($dataid);
                    $sql = "update maenna_people_data set data_attr = '%s',data_value2 = '%s', data_value = '%s',access = '%s',editorid  = '%s' where dataid = %d limit 1";
                    $sql_data = array($year,$study, $school,  $time, $editorid,$dataid);
                    //test_array($sql_data);exit;
                    if(db_query($sql, $sql_data))
                        drupal_set_message("Reocrd information is updated.");
                    else
                        drupal_set_message("Failed to update record." . db_error(),'error');
                        
                    return;
                break;
            case 'delete':
                    $dataid = sget($_REQUEST, 'dataid');
                    if(empty($dataid)) {
                        drupal_set_message("Failed to update record." . db_error(),'error');
                        return;
                    }
                    $sql = "delete from maenna_people_data where dataid = %d limit 1";
                    $dataid = intval($dataid);
                    $sql_data = array($dataid);
                    if(db_query($sql, $sql_data))
                        drupal_set_message("The selected record is removed."  );
                    else
                        drupal_set_message("Failed to remove record." . db_error(),'error');
                        
                    return;
                break;
        }
        
    }
}


/* EOF */