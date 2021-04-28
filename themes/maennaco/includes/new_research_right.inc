<?php
global $redirect;
$redirect = rebuild_url(array('tab'));
function add_research_company($op = '')
{
    global $redirect;
    $Block['title'] =  'Add Company';
    $content = '';
    if($op && $op != 'update')
    {
        $Block['title']  .=  "<div class='editbtn'><a href='#' class='edit_wnd tool' rel='add_research_company'>Add</a></div>";
    }
    elseif($op == 'update')
    {
        $company = sget($_REQUEST,'company');
        $sector = sget($_REQUEST,'sector');
        $revenue = sget($_REQUEST,'revenue');
        if($revenue) $revenue = @preg_replace("/[^0-9]/",'',$revenue);
        $address1 = sget($_REQUEST,'address1');
        $address2 = sget($_REQUEST,'address2');
        $city = sget($_REQUEST, 'city');
        $state = sget($_REQUEST,'state');
        $zip = sget($_REQUEST,'zip');
        $contact = sget($_REQUEST,'contact');
        $email = sget($_REQUEST,'email');
        $phone = sget($_REQUEST,'phone');
        $fax = sget($_REQUEST,'fax');
        $website = sget($_REQUEST,'website');
        $managerid = sget($_REQUEST,'managerid');
        $time = time();
        
        $sql = "insert into research_company
                (company,email,address1, address2, city,state, zip, phone,fax, web, revenue, contact, sector, created,groupid) values
                ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s','%s', '%s', '%s', '%s',
                                            '%s', '%s',%d)";
        //echo $sql
        $DBValues = array($company, $email, $address1, $address2, $city, $state, $zip, $phone, $fax, $website, $revenue, $contact, $sector, $time, $managerid);
        if( db_query($sql,$DBValues))
        {
            $last_id = db_last_insert_id("research_company", 'company');
            drupal_set_message("A new company is added. Click <a href='$redirect&page=research_company_detail&recordid=${last_id}'>here</a> to edit company details");
        }else{
            drupal_set_message("Error: failed to add company",'error');
        }
        
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}
function recently_added($op = null)
{
    global $redirect;
    $Block['title'] =  'RECENTLY REGISTERED';
    $content = '';
    if($op && $op != 'update') {
        $sql = "select * from research_company where created != '' order by created limit 4";
        $result = db_query($sql);
        $content = "<table cellspacing=0 cellpadding=0 border=0>";
        while(($row = db_fetch_array($result)) !== false)
        {
            $company = sget($row, 'company');
            $recordid = sget($row,'recordid');
            $created = sget($row, 'created');
            
            $company_link = "<a href='$redirect&page=research_company_detail&recordid=$recordid'>" . htmlentities($company, ENT_QUOTES) . "</a>";
            $date = date("m/d/Y", $created);
            
            $content .= "<tr><td>$company_link</td><td>$date</td></tr>";
        }
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function recently_updated($op = null)
{
     global $redirect;
    $Block['title'] =  'RECENTLY UPDATED';
    $content = '';
    if($op && $op != 'update') {
        $Array = array();
        $condition = time() - (30 * 24 * 60 * 60);
        $sql = "select company, recordid, access from research_company where access is not null and access > $condition order by access desc limit 6";
        
        $result = db_query($sql);
        if(db_affected_rows($result )> 0 )
        {
            while(($row = db_fetch_object($result)) !== false)
            {
                $access = $row->access;
                $Array["$access"] = $row;
            }
        }
        
        $sql = "select ta.company,ta.recordid,tb.access from research_company as ta, research_company_data as tb where tb.access is not null and tb.access > $condition and ta.recordid = tb.recordid order by tb.access desc limit 6";
        $result = db_query($sql);
  
        if(db_affected_rows($result )> 0 )
        {
            while(($row = db_fetch_object($result)) !== false)
            {
                $access = $row->access;
                $Array["$access"] = $row;
            }
        }
        krsort($Array);

        $content = "<table cellspacing=0 cellpadding=0 border=0 style='margin-top:0'>";
        $counter=1;
        foreach($Array as $access => $Obj)
        {
            if($counter > 5) break;
            $counter++;
            $recordid = $Obj->recordid;
            $content .= "\n<tr><td>" . date('m/d/Y', $access) . " - <a href='$redirect&page=research_company_detail&recordid=$recordid'>" . $Obj->company . '</a></td></tr>';
        }
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function priority_stats($op = null)
{
    global $redirect;
    $Block['title'] =  'PRIORITY STATS';
    $content = '';
    if($op && $op != 'update') {
        $Prospects = array('high', 'medium', 'low', 'unlikely');
        $str = "'" . implode("','",$Prospects) ."'";
        $sql = "select count(*) as total, priority from research_company where priority is not NULL and priority in ($str) group by priority";
    
        $result = db_query($sql);
        $Data = array();
        if(db_affected_rows($result ) > 0 )
        {
            while(($row = db_fetch_object($result)) !== false)
            {
                $total = $row->total;
                $priority = $row->priority;
                $Data["$priority"] = $total;
            }
        }

        $content = "<table cellspacing=0 cellpadding=0 border=0><tr><td></td><td align=right># of Companies</td></tr>";
        foreach($Prospects as $p)
        {
            $total = sget($Data,$p) ? sget($Data, $p) : '0';
            $content .= "\n<tr><td>".strtoupper($p)."</td><td align=right>$total</td></tr>";
        }
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }$Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function research_manager($op = null)
{
     global $redirect;
    $Block['title'] =  'TOP RELATIONSHIP MANAGER'; 
    if($op && $op != 'update') {
        

        $content = "<table cellspacing=0 cellpadding=0 border=0><tr><td>Manager</td><td>QL</td><td>Converted</td></tr>";
        
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }
    return $Block;
}
function key_statistics($op = null)
{
     global $redirect;
    $Block['title'] =  'KEY STATS'; 
    if($op && $op != 'update') {
        

        $content = "<table cellspacing=0 cellpadding=0 border=0>
                <tr><td>Registered online</td><td algin=right>1</td></tr> 
                <tr><td>Qaulified leads</td><td algin=right>1</td></tr>
                <tr><td>Follow up</td><td algin=right>1</td></tr>";
        
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }
    return $Block;
}
function leads_by_sector($op = null)
{
     global $redirect;
    $Block['title'] =  'QUALIFIED LEADS BY SECTOR'; 
    if($op && $op != 'update') {
        

        $content = "<table cellspacing=0 cellpadding=0 border=0>
                <tr><td>Sector</td><td algin=right># of leads</td></tr> 
                <tr><td>Qaulified leads</td><td algin=right>1</td></tr>
                <tr><td>Follow up</td><td algin=right>1</td></tr>";
        
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }
    return $Block;
}
function followup_stats($op = null)
{
     global $redirect;
    $Block['title'] =  'FOLLOWUP STATS'; 
    if($op && $op != 'update') {
        

        $content = "<table cellspacing=0 cellpadding=0 border=0><tr><td>Month</td><td align=right># of Companies</td></tr>";
        
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }
    return $Block;
}

function sector_stats($op = null)
{
     global $redirect;
    $Block['title'] =  'SECTOR STATS'; 
    if($op && $op != 'update') {
        

        $content = "<table cellspacing=0 cellpadding=0 border=0><tr><td>Sector</td><td align=right># of Companies</td></tr>";
        
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }
    return $Block;
}
function mark_for_deletion($op = null)
{
     global $redirect;
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

function add_research_people($op = null)
{
     global $redirect;
    $time = time();
    global $user;
    $editorid = $user->uid;
    $content = '';
    
    $Block['title'] =  'Add PEOPLE'; 
    if($op && $op != 'update') {
    
        $Block['title']  .=  "<div class=editbtn><a href='#' class='edit_wnd tool' rel='edit_add_research_people'>Add</a></div>";
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
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function recently_added_people($op = null)
{
     global $redirect;
    $Block['title'] =  'RECENTLY AddED'; 
    if($op && $op != 'update') {
        $sql = "select * from research_people order by pid desc limit 6";
        $result = db_query($sql);
        
        $content = "<table cellspacing=0 cellpadding=0 border=0><tr><td></td><td align=right></td></tr>";
        if(db_affected_rows($result) > 0)
        {
            while(($row = db_fetch_object($result)) != false)
            {
                $pid = $row->pid;
                $date = date('m/d/Y', $row->created);
                $name = $row->lastname . ', ' . $row->firstname;
                $link = "<a href='$redirect&page=research_people_detail&pid=$pid'>$name</a>";
                $content .= "\n<tr><td>$date</td><td align=right>$link</td></tr>";
            }
        }
        
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }elseif($op == 'update')
    {
        return;
    }
    return $Block;
}

function recently_updated_people($op = null)
{
     global $redirect;
    $Block['title'] =  'RECENTLY UPDATED'; 
    if($op && $op != 'update') {
    
        $content = "<table cellspacing=0 cellpadding=0 border=0><tr><td></td><td align=right># of Companies</td></tr>";
        $time_past_month = time() - (30 * 24 * 60 * 60);
        
        $sql = "select * from research_people  where access is not null and access > '$time_past_month' order by access desc limit 6";
        $result = db_query($sql);
        if(db_affected_rows($result) > 0)
        {
            while(($row = db_fetch_object($result)) !== false)
            {
                $pid = $row->pid;
                $date = date('m/d/Y', $row->access);
                $name = $row->lastname . ', ' . $row->firstname;
                $link = "<a href='$redirect&page=research_people_detail&pid=$pid'>$name</a>";
                $content .= "\n<tr><td>$date</td><td align=right>$link</td></tr>";
            }
        }
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }elseif($op == 'update')
    {
        return;
    }
    return $Block;
}

function priority_stats_people($op = null)
{
     global $redirect;
    $Block['title'] =  'PRIORITY STATS'; 
    if($op && $op != 'update') {
    
        $content = "<table cellspacing=0 cellpadding=0 border=0><tr><td></td><td align=right></td></tr>";
       
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }elseif($op == 'update')
    {
        return;
    }
    return $Block;
}

function followup_stats_people($op = null)
{
     global $redirect;
    $Block['title'] =  'FOLLOWUP STATS'; 
    if($op && $op != 'update') {
    
        $content = "<table cellspacing=0 cellpadding=0 border=0><tr><td></td><td align=right></td></tr>";
       
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }elseif($op == 'update')
    {
        return;
    }
    return $Block;
}



/* EOF */