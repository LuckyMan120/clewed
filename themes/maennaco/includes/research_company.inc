<?php



function add_research_company($op = '')
{
    $Block['title'] =  'Add Company'; 
    if($op == 'view' || $op == '')  $Block['body']  =  "<div style='padding:5px;'><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
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
        
        $sql = "insert into research_company (company,email,address1, address2, city,state, zip, phone,fax, web, revenue, contact, sector, created,groupid) values
                                            ('$company', '$email', '$address1', '$address2', '$city', '$state', '$zip', '$phone','$fax', '$website', '$revenue', '$contact',
                                            '$sector', '$time',$managerid)";
        //echo $sql;
        if( db_query($sql))
        {
            $last_id = db_last_insert_id("research_company", 'company');
            drupal_set_message("A new company is added. Click <a href='/account?a=research&type=company_detail&recordid=${last_id}'>here</a> to edit company details");
        }else{
            drupal_set_message("Error: failed to add company",'error');
        }
        
    }
    return $Block;
}
function recently_added($op = null)
{
    $Block['title'] =  'Most recently registered'; 
    if($op == 'view' || $op == '') {
        $sql = "select * from research_company where created != '' order by created limit 4";
        $result = db_query($sql);
        $content = "<table cellspacing=0 cellpadding=0 border=0>";
        while(($row = db_fetch_array($result)) !== false)
        {
            $company = sget($row, 'company');
            $recordid = sget($row,'recordid');
            $created = sget($row, 'created');
            
            $company_link = "<a href='account?a=research&type=company_detail&recordid=$recordid'>" . htmlentities($company, ENT_QUOTES) . "</a>";
            $date = date("m/d/Y", $created);
            
            $content .= "<tr><td>$company_link</td><td>$date</td></tr>";
        }
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }
    return $Block;
}

function recently_updated($op = null)
{
    $Block['title'] =  'Recently Updated'; 
    if($op == 'view' || $op == '') {
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
            $content .= "\n<tr><td>" . date('m/d/Y', $access) . " - <a href='account?a=research&type=company_detail&recordid=$recordid'>" . $Obj->company . '</a></td></tr>';
        }
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }
    return $Block;
}

function priority_stats($op = null)
{
    $Block['title'] =  'Priority Stats'; 
    if($op == 'view' || $op == '') {
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
    }
    return $Block;
}

function research_manager($op = null)
{
    $Block['title'] =  'Top Relationship Manager'; 
    if($op == 'view' || $op == '') {
        

        $content = "<table cellspacing=0 cellpadding=0 border=0><tr><td>Manager</td><td>QL</td><td>Converted</td></tr>";
        
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
        //$Block['body']  =  "<div><a href='#' class='edit_wnd' rel='add_research_company'>Add Company</a></div>";
    }
    return $Block;
}
function key_statistics($op = null){
    $Block['title'] =  'Key Statistics'; 
    if($op == 'view' || $op == '') {
        

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
    $Block['title'] =  'Qualified leads by sector'; 
    if($op == 'view' || $op == '') {
        

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



/* EOF */