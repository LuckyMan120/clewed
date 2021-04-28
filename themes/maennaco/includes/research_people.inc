<?php

//array('add_research_people', 'recently_added_people', 'recently_updated_people', 'priority_stats_people', 'followup_stats_people');

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

function recently_added_people($op = null)
{
    $Block['title'] =  'Recently Added'; 
    if($op == 'view' || $op == '') {
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
                $link = "<a href='/account?a=research&type=people_detail&pid=$pid'>$name</a>";
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
    $Block['title'] =  'Recently Updated'; 
    if($op == 'view' || $op == '') {
    
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
                $link = "<a href='/account?a=research&type=people_detail&pid=$pid'>$name</a>";
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
    $Block['title'] =  'Priority Stats'; 
    if($op == 'view' || $op == '') {
    
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
    $Block['title'] =  'Follow Up Stats'; 
    if($op == 'view' || $op == '') {
    
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
            $sql = "update research_people set delete_marker = 1 where pid in (0 $update_section_data)";
            if(db_query($sql)) drupal_set_message("The selected records are removed from search results");
            else{
                drupal_set_message("error: failed to remove records",'error');
            }
        }
    }
}


/* EOF */