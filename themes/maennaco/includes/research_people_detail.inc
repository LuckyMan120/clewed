<?php

function research_people_dummy($op = null)
{
    global $user;
    $editorid = $user->uid;
    $recordid = sget($_REQUEST,'pid');
    $time = time();
   
    $Block['title'] = '';
    if($op == 'view' || $op == null){
        $content = "<div style='font-size:12px;'>$company_desc</div>";
        
        $content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_company_name' data='$recordid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        if(db_query($sql, $sql_data))
            drupal_set_message("Company information is updated.");
        else
            drupal_set_message("Failed to update company information.",'error');
    }
    return $Block;
}


function research_people_contact($op = null)
{
    $pid = sget($_REQUEST,'pid','int');
    $time = time();
    global $user;
    $editorid = $user->uid;
    
    $Block['title'] = 'Contact Info';
    
    if($op == 'view' || $op == null){
        if($pid > 0 )
        {
            $sql = "select * from research_people where pid = $pid limit 1";
            $result = db_query($sql);
            $content = "<div >";
            if(($row = db_fetch_object($result)) !== false)
            {
                $firstname = $row->firstname;
                $lastname = $row->lastname;
                $email = $row->email;
                $phone = $row->phone;
                $fax = $row->fax;
                $city = $row->city;
                $state = $row->state;
                $sector = $row->sector;
                $experience = $row->experience;
                $intro = $row->brief_intro;
                
                $address = '';
                $content.= "$firstname, $lastname<br>";
                $content .= "$email" . "<br>";
                if($phone )$content .= $phone . "<br>";
                if($fax ) $content .= "$fax(fax)". "<br>";
                if($city )  $content .= "$city, $state" . "<br>"; 
            }
            
            $content.="</div>";
            
        }
        
        $content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_contact' data='$pid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        $sql_data = array();
        $sql_query = array();
        $POST = array('firstname' => 'firstname',
                          'lastname' => 'lastname',
                          'email' => 'email',
                          'phone' => 'phone',
                      /*    'fax' => 'fax',
                          'address1' => 'street1',
                          'address2' => 'street2',
                          'city' => 'city',
                          'state' => 'state',
                          'zip' => 'zip',
                          'sector' => 'sector',
                          'experience' => 'yearofexp',
                          'brief_intro' => 'intro',
                          'position' => 'position',*/
                          );
       
        
        $status_query = array(        'access'   => "access = '%s'",
                                 'editorid' => "editorid = '%d'"
                                 );
        $status_data = array($time, $editorid);
        
        foreach($POST as $table_key =>  $post_index)
        {
            $sql_query["$table_key"] = "$table_key = '%s'";
            $sql_data["$table_key"] = sget($_REQUEST,$post_index);
        }
        
        $sql_query = array_merge($sql_query, $status_query);
        $sql_data = array_merge($sql_data, $status_data);
        
        $sql = "update research_people set " . implode(",", $sql_query) . " where pid = $pid limit 1";
        //echo $sql;
        if($pid > 0 && db_query($sql, $sql_data))
            drupal_set_message("Reocrd information is updated.");
        else
            drupal_set_message("Failed to update record." . db_error(),'error');
            
        return;
    }
    return $Block;
}
function research_people_work($op = null)
{
    $pid = sget($_REQUEST,'pid','int');
    $time = time();
    global $user;
    $editorid = $user->uid;
    
    $Block['title'] = 'Employment';
    if($op == 'view' || $op == null){
        $content = "<table>";
        if($pid > 0 )
        {
            $sql = "select * from research_people_data where pid = $pid and data_type = 'employment' order by data_attr desc";
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
                   
                    $content .= "\n<tr  style='border-bottom:none'><td colspan=2  style='border-bottom:none'>
                        $start - $end </td></tr>
                    <tr><td align=left style='border-top:none'>$position</td><td align=right style='border-top:none'>$org</td></tr>\n";
                }
            }
        }
        $content .= "</table>\n";
        $content .= "<div style='text-align:right;margin-top:-10px;'><a href='/account?a=research&type=people_detail&pid=$pid&sub=work' style='font:12px arial'>more</a></div>";
        $content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_work' data='$pid'>new entry</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        //test_array($_POST);

        $start_month = sget($_REQUEST,'start_month');
        $start_year = sget($_REQUEST,'start_year');
        $end_month = sget($_REQUEST,'end_month');
        $end_year = sget($_REQUEST,'end_year');
        
        $start = $start_month .'/'. $start_year;
        $end = $end_month .'/' . $end_year;
        $company = sget($_REQUEST,'company');
        $position = sget($_REQUEST,'position');
        
        $sql = "insert into research_people_data (pid, data_type, data_attr,data_value3, data_value,data_value2,access,editorid) values('%d','%s','%s','%s','%s','%s','%s','%d')";
        $sql_data = array($pid, 'employment', $start,$end, $company, $position, $time, $editorid);
        //test_array($sql_data);exit;
        if($pid > 0 && db_query($sql, $sql_data))
            drupal_set_message("Reocrd information is updated.");
        else
            drupal_set_message("Failed to update record." . db_error(),'error');
            
        return;
    }
    return $Block;
}

function research_people_education($op = null)
{
    $pid = sget($_REQUEST,'pid','int');
    $time = time();
    global $user;
    $editorid = $user->uid;
    
    $Block['title'] = 'Education';
    if($op == 'view' || $op == null){
        $content = "<table>";
        if($pid > 0 )
        {
            $sql = "select * from research_people_data where pid = $pid and data_type = 'education' order by (data_attr * 1) desc";
            $result = db_query($sql);
            if(db_affected_rows($result) > 0)
            {
                while(($row = db_fetch_object($result)) !== false){
                    $year = $row->data_attr;
                    //echo "<br>$data_attr";
                    $school = $row->data_value;
                    $study = $row->data_value2;
                    $year = date("Y", $year);
                   
                    $content .= "<!--tr  style='border-bottom:none'><td colspan=2  style='border-bottom:none'>$start - $end </td></tr-->\n
                    <tr><td align=left style='border-top:none'>$study</td><td align=right style='border-top:none'>$school ($year)</td></tr>\n";
                }
            }
        }
        $content .= "</table>\n";
        $content .= "<div style='text-align:right;margin-top:-10px;'><a href='/account?a=research&type=people_detail&pid=$pid&sub=education' style='font:12px arial'>more</a></div>";
        $content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_education' data='$pid'>new entry</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        
        $year = sget($_REQUEST,'end');
        $school = sget($_REQUEST,'school');
        $study = sget($_REQUEST,'study');
        
        $year = "1/1/$year";
        $year = strtotime($year);
        
        $sql = "insert into research_people_data (pid, data_type, data_attr,data_value,data_value2,access,editorid) values('%d','%s','%s','%s','%s','%s','%d')";
        $sql_data = array($pid, 'education', $year, $school, $study, $time, $editorid);
        if($pid > 0 && db_query($sql, $sql_data))
            drupal_set_message("Reocrd information is updated.");
        else
            drupal_set_message("Failed to update record." . db_error(),'error');
            
        return;
    }
    return $Block;
}

function research_people_links1($op = null)
{
    $pid = sget($_REQUEST,'pid','int');
    $time = time();
    global $user;
    $editorid = $user->uid;
    
    $Block['title'] = 'Links';
    if($op == 'view' || $op == null){
        $content = "<div>";
        if($pid > 0 )
        {
            $sql = "select * from research_people_data where data_type = 'links' and pid = $pid order by data_value asc";
            $result = db_query($sql);
            
            if(db_affected_rows($result )>0)
            {
                while(($row = db_fetch_object($result)) !== false)
                {
                    $title = $row->data_value;
                    $link = $row->data_value2;
                    $content .= "<a href='$link' target='_blank'>" . htmlentities($title) . "</a><br>\n";
                }
            }
        }
        $content .= "</div>";
        $content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_links' data='$pid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
       $title = sget($_REQUEST,'title');
       $url = sget($_REQUEST,'url');
       $sql = "insert into research_people_data (pid, data_type, data_value,data_value2,access, editorid) values('%d','%s','%s','%s','%s','%d')";
       $sql_data = array($pid, 'links', $title, $url, $time, $editorid);
       if($pid > 0 && db_query($sql, $sql_data))
            drupal_set_message("Reocrd information is updated.");
        else
            drupal_set_message("Failed to update record." . db_error(),'error');
            
        return;
    }
    return $Block;
}

function research_people_links($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST,'pid','int');
    $time = time();
    $correct = true;
    
    $Block['title'] = 'Links';
    if($op == 'view' || $op == null){
        $content = "<ul>";
        $sql = "select * from research_people_data where data_type = 'links' and pid = $pid order by data_value asc";
       
        $result = db_query($sql);
        $counter = 1;
        while(($data = db_fetch_object($result)) !== false)
        {
            $link = $data->data_value2;
            if(! $link ) continue;
            $content .= "<li><a href='$link' target='_blank'>" . check_plain($data->data_value) . "</a></li>";
            $counter++;
        }
        if($content == ''){$content = '<li>n/a</li>';}
        
        $content .= "</ul><div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_links' data='$pid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        for($i = 1; $i <= 5; $i++)
        {
            $title = sget($_POST, "name_$i");
            $url = sget($_POST, "link_$i");
            $dataid = sget($_POST,"dataid_$i");
            if($dataid)
            {
                $sql = "update research_people_data set data_value = '$title', data_value2 = '$url', access='$time', editorid='$editorid' where dataid = $dataid limit 1 ";
                $sql_data = array();
            }else{
                $sql = "insert into research_people_data (pid, access, data_type, data_value, data_value2, editorid) values('%d','%s','%s','%s','%s','%d')";
                $sql_data = array($pid, $time, 'links', $title, $url, $editorid);
            }
            if( ! db_query($sql, $sql_data)){
                drupal_set_message("Error: failed to update links.",'error');
                $correct = false;
                break;
            }
        }
        if($correct) drupal_set_message("Information is updated.");
    }
    return $Block;
}

function add_research_people($op = null)
{
    $time = time();
    global $user;
    $editorid = $user->uid;
    
    $Block['title'] =  'Add People'; 
    if($op == 'view' || $op == '') {
    
        $Block['body']  =  "<div style='padding:5px 0 0 5px;'><a href='#' class='edit_wnd' rel='edit_add_research_people'>Add People</a></div>";
    }elseif($op == 'update')
    {
        //test_array($_POST);exit;
        $database = array('firstname' => 'firstname',
                          'lastname' => 'lastname',
                          'email' => 'email',
                          'phone' => 'phone',
                          'fax' => 'fax',
                          'address1' => 'street1',
                          'address2' => 'street2',
                          'city' => 'city',
                          'state' => 'state',
                          'zip' => 'zip',
                          'industry' => 'industry',
                          'experience' => 'experience',
                          'brief_intro' => 'intro',
                          'position' => 'position',
                          );
       
        
        $database_status = array('created' => $time,
                                 'access'   => $time,
                                 'editorid' => $editorid,
                                 );
        foreach($database as $index => $key )
        {
            $database["$index"] = sget($_REQUEST,$key);
        }
        $database_value = array_merge($database, $database_status);
        $database_index = array_keys($database_value);
        
        $sql = "insert into research_people (" . implode(",", $database_index) . ") values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%d','%s')";
        if(db_query($sql, $database_value))
        {
            $last_id = db_last_insert_id('research_people', 'pid');
            drupal_set_message("A new record is added. Click <a href='$last_id'>here</a> to add details.");
        }else{
            drupal_set_message("Error: failed to add new record",'error');
        }
        
        
        return;
    }
    return $Block;
}
function research_people_info($op = null)
{
    $pid = sget($_REQUEST,'pid','int');
    $time = time();
    global $user;
    $editorid = $user->uid;

    
    
    
    
    if($op == 'view' || $op == null){

        $sql = "select * from research_people where pid = $pid limit 1";
        $result = db_query($sql);
        $content = "<div >";
        if(($row = db_fetch_object($result)) !== false)
        {
            $firstname = $row->firstname;
            $lastname = $row->lastname;
            $industry = $row->industry;
            $experience = $row->experience;
            if($experience) $experience = date("Y") - $experience * 1;
            $intro = htmlentities($row->brief_intro,ENT_QUOTES);
            
            if($industry ) $content.= "Industry - $industry ($experience years)";
            if($row->brief_intro ) $content .= "<div lines=3 trueH=0 sH=0 class='morediv'>" . nl2br($intro) ."</div>";
             if(strlen($intro) > 300)
            {
                $content .= "<div style='text-align:right;padding-right:5px;'><a href='#' class='morediv_link' closed=1>More >></a></div>";
            }
        }
        
        $content.="</div>";
        $Block['title'] = ucwords($firstname .', ' . $lastname);
        $content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_info' data='$pid'>edit</a></div>";
        $Block['body'] = content_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        $sql_data = array();
        $sql_query = array();
       // test_array($_POST);
        $POST = array(/*'firstname' => 'firstname',
                          'lastname' => 'lastname',
                          'email' => 'email',
                          'phone' => 'phone',
                          'fax' => 'fax',
                          'address1' => 'street1',
                          'address2' => 'street2',
                          'city' => 'city',
                          'state' => 'state',
                          'zip' => 'zip',*/
                          'industry' => 'industry',
                          'experience' => 'yearofexp',
                          'brief_intro' => 'intro',
                          'position' => 'position',
                          );
       
        
        $status_query = array(        'access'   => "access = '%s'",
                                 'editorid' => "editorid = '%d'"
                                 );
        $status_data = array($time, $editorid);
        
        foreach($POST as $table_key =>  $post_index)
        {
            $sql_query["$table_key"] = "$table_key = '%s'";
            $sql_data["$table_key"] = sget($_REQUEST,$post_index);
        }
        
        $sql_query = array_merge($sql_query, $status_query);
        $sql_data = array_merge($sql_data, $status_data);
        //test_array($_POST);
        $sql = "update research_people set " . implode(",", $sql_query) . " where pid = $pid limit 1";
        //test_array($sql_data);
        //echo $sql;exit;
        if($pid > 0 && db_query($sql, $sql_data))
            drupal_set_message("Reocrd information is updated.");
        else
            drupal_set_message("Failed to update record." . db_error(),'error');
            
        return;
    }
    return $Block;
}

function research_people_status($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST,'pid');
    $time = time();

    $Block['title'] = 'Progress Status';
    if($op == 'view' || $op == null){
        $content = "<div><table cellspacing=0 cellpadding=0><tr><td width=70>Date</td><td width=100>Contact Name</td><td>Message</td><td width=100 align=right>Follow-Up Date</td></tr>";
        $sql = "select * from research_people_data  where pid = $pid and data_type = 'progress' order by dataid desc ";
        $result = db_query($sql);
        $counter = 1;
        if(db_affected_rows($result )> 0 )
        {
            while(($row = db_fetch_object($result)) !== false)
            {
                $date = $row->data_value3;
                $followup = $row->data_attr;
                if($row->data_attr) $followup = date("m/d/Y", $row->data_attr);
                if($row->data_value3)$date = date("m/d/Y", $row->data_value3);
               
                $contact = $row->data_value;
                $message = htmlentities( $row->data_value2,ENT_QUOTES);
                
                $content .= "<tr><td>$date</td><td>$contact</td><td>$message</td><td align=right>$followup</td></tr>\n";
            }
        }
        
        $content .= "</table></div>";
        
        $content .= "</ul><div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_progress' data='$pid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
  
        $date = sget($_REQUEST,'date');
        $contact = sget($_REQUEST,'contact');
        $message = sget($_REQUEST, 'message');
        $followup = sget($_REQUEST,'followup');
        if($followup)$followup = strtotime($followup);
        if($date)$date = strtotime($date);
        
        $sql = "insert into research_people_data (pid, editorid, access, data_type, data_attr, data_value, data_value2,data_value3) values('%d','%d','%s','%s','%s','%s','%s','%s')";
        $sql_data = array($pid, $editorid, $time, 'progress',  $followup, $contact, $message,$date);
        if(db_query($sql, $sql_data) === false)
        {
                //echo $sql,
                //test_array($sql_data);
                //echo mysql_error();
                drupal_set_message("Faild to update progress status. Please try again later",'error');
                
        }else
        drupal_set_message("Progress status is updated");
    }
    return $Block;
}
function research_accomplishments($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST,'pid');
    $time = time();

    $Block['title'] = 'Accomplishments';
    if($op == 'view' || $op == null){
        $content = "<div><table cellspacing=0 cellpadding=0>";
        $sql = "select * from research_people_data  where pid = $pid and data_type = 'accomplish' order by dataid desc ";
        $result = db_query($sql);
        $counter = 1;
        if(db_affected_rows($result )> 0 )
        {
            while(($row = db_fetch_object($result)) !== false)
            {
                $value = check_plain($row->data_value);
                
                $content .= "<tr><td>$value</td></tr>\n";
            }
        }
        
        $content .= "</table></div>";
        
        $content .= "</ul><div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_accomplish' data='$pid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        $pid = (int)sget($_REQUEST,'pid');
        
        $access = time();
        for($i = 1; $i <= 3; $i++){
            $accomplish = sget($_REQUEST, 'accomplish_' . $i);
            if($accomplish && $pid)
            {
                $dataid = sget($_REQUEST, 'dataid_' . $i);
                if($dataid){
                    $sql = "update research_people_data set data_value = '%s', access='%s',editorid = '%s' where dataid = %d limit 1";
                    $sql_data = array($accomplish,$access, $editorid, $dataid);
                }else{
                    $sql = "insert into research_people_data (pid, access, data_type, data_value) values (%d, '%s', '%s', '%s')";
                    $sql_data = array($pid, $access, 'accomplish', $accomplish);
                }
                if(db_query($sql, $sql_data) === false)
                {
                    drupal_set_message("Faild to update database. Please try again later",'error');
                }else{
                    drupal_set_message("Accomplishment info updated");
                }
            }
        }
    }
    return $Block;
}

function research_people_prospects($op = null)
{

    $pid = sget($_REQUEST,'pid');
    $Block['title'] = "Prospects";
    global $user;
    $editorid = $user->uid;
    $time = time();
    if($op == null || $op == 'view')
    {
        $Prospects = array('high', 'medium', 'low', 'unlikely');
        $prospect = 'not set';
        $sql = "select priority from research_people where pid = $pid limit 1";
        $result = db_query($sql);
        if(($row = db_fetch_object($result)) !== false)
        {
            
            if($row->priority) $prospect = $row->priority;
        }

        $content = "\n<div style='width:100%;height:30px;line-height:30px;text-align:center;font:12px arial;'>$prospect</div>";
        $content .= "\n<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_prospects' data='$pid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        $prospect = sget($_REQUEST,'prospect');
       
        
            $sql = "update research_people set priority = '$prospect', access='$time',editorid = $editorid where pid = $pid limit 1";
            

        
        if(db_query($sql))
        {
            drupal_set_message("Prospect setting updated");
        }else{
            drupal_set_message("Error: failed to update prospect setting",'error');
        }
        return;
    }
    return $Block;
    
}

function research_people_followup($op = null)
{
    $pid = sget($_REQUEST,'pid');
    $Block['title'] = "Follow-Up Priorities";
    global $user;
    $editorid = $user->uid;
    $time = time();
    
    if($op == null || $op == 'view')
    {
        
        $sql = "select * from research_people_data where data_type = 'followup' and pid = $pid order by data_attr asc limit 6";
        $result = db_query($sql);
        $content = "\n<table><tr><td></td><td></td></tr>";
        if(db_affected_rows($result) > 0)
        {
            while(($row = db_fetch_object($result)) !== false)
            {
                
                $date = date('m/d/Y', $row->data_attr);
                $note = htmlentities($row->data_value);
                $content .= "<tr><td colspan=2>$date<br>$note    </td></tr>\n";
            }
        }
        

        $content .= "</table>";
        $content .= "\n<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_followup' data='$pid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        

        $note = sget($_REQUEST,'note');
        $date = sget($_REQUEST,'date');
        $date_stamp = strtotime($date);
        
            $sql = "insert into research_people_data(pid, data_type, data_attr, data_value, access, editorid) values('%d','%s','%s','%s','%s','%d') ";
            $sql_data = array($pid, 'followup', $date_stamp, $note, $time, $editorid);
        
        if(db_query($sql, $sql_data))
        {
            drupal_set_message("Prospect setting updated");
        }else{
            drupal_set_message("Error: failed to update prospect setting",'error');
        }
        return;
    }
    return $Block;
}

function  research_people_articles($op = null)
{
    $pid = sget($_REQUEST,'pid');
    $Block['title'] = "Articles/Publications";
    global $user;
    $editorid = $user->uid;
    $time = time();
    
    if($op == null || $op == 'view')
    {
        
        $sql = "select * from research_people_data where data_type = 'article' and pid = $pid order by data_value asc limit 6";
        $result = db_query($sql);
        $content = "\n<table><tr><td></td><td></td></tr>";
        if(db_affected_rows($result) > 0)
        {
            while(($row = db_fetch_object($result)) !== false)
            {
                
                $data_attr = $row->data_attr;
                if($data_attr == 'webpage')
                $content .= "<tr><td><a href='".$row->data_value2."' target='_blank'>" . htmlentities($row->data_value) ."</a></td></tr>\n";
                else{
                    $src = "/" . file_directory_path() . "/" . $row->data_value;
                    $content .= "<tr><td><a href='$src' target='_blank'>" . htmlentities($row->data_value) ."</a></td></tr>\n";
                }
            }
        }
        

        $content .= "</table>";
        $content .= "\n<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_article' data='$pid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        $title = sget($_REQUEST,'title');
        $url = sget($_REQUEST,'url');
        $article_type = '';
        $data_type = 'article';
        
        if( (! empty($title ))  && (! empty($url)))
        {
            $data_attr = "webpage";
            $sql = "insert into research_people_data (pid, data_type, data_attr, data_value, data_value2, access, editorid)
                                                        values('%d','%s','%s','%s','%s','%s','%d')";
            $sql_array = array($pid, $data_type, $data_attr, $title, $url, $time, $editorid);
        }
        if(isset($_FILES['file']) && $_FILES['error'] == 0)
        {
                $path = "./" . file_directory_path() . "/";
               
                $filename = "";
                if($ext = get_filetype($_FILES["file"]["name"]))
                {
                    $filename = ranStr() . "." . $ext;
                    if(move_uploaded_file($_FILES["file"]["tmp_name"], $path . $filename))
                    {
                        
                        $sql = "insert into research_people_data (pid, data_type, data_value, access, editorid) values ($pid, 'article', '$filename', '$time', $editorid)";
                        $$sql_array = array();
                    }else{
                        drupal_set_message("Failed to upload new file",'error');
                    }
                }else{
                    drupal_set_message("The file type is not allowed",'error');
                }
        }
        
        
        if(db_query($sql,$sql_array))
        {
            drupal_set_message("A new article record is added");
        }else{
            drupal_set_message("Error: failed to update article record",'error');
        }
        return;
    }
    return $Block;
    
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
        $content = "<div><a href='/account?a=research&type=people_detail&pid=$pid'>back to detail</a></div><table>";
        if($pid > 0 )
        {
            $sql = "select * from research_people_data where pid = $pid and data_type = 'employment' order by data_attr desc";
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
                    <td><a href='/account?a=research&type=people_detail&pid=$pid&sub=work&dataid=${dataid}&do=delete&update_section=employment_panel' onclick='return confirm(\"Continue to remove this record?\")'>delete</a></td>
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
                    $sql = "update research_people_data set data_attr = '%s',data_value3 = '%s', data_value = '%s',data_value2 = '%s',access = '%s',editorid  = '%s' where dataid = %d limit 1";
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
                    $sql = "delete from research_people_data where dataid = %d limit 1";
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
        $content = "<div><a href='/account?a=research&type=people_detail&pid=$pid'>back to detail</a></div><table>";
        if($pid > 0 )
        {
            $sql = "select * from research_people_data where pid = $pid and data_type = 'education' order by data_attr desc";
            $result = db_query($sql);
            if(db_affected_rows($result) > 0)
            {
                while(($row = db_fetch_object($result)) !== false){
                    $sql = "select * from research_people_data where pid = $pid and data_type = 'education' order by (data_attr * 1) desc";
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
                            <td><a href='/account?a=research&type=people_detail&pid=$pid&sub=education&dataid=${dataid}&do=delete&update_section=education_panel' onclick='return confirm(\"Continue to remove this record?\")'>delete</a></td>
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
                    $sql = "update research_people_data set data_attr = '%s',data_value2 = '%s', data_value = '%s',access = '%s',editorid  = '%s' where dataid = %d limit 1";
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
                    $sql = "delete from research_people_data where dataid = %d limit 1";
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