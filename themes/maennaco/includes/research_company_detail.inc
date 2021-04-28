<?php

function research_company_logo($op = null)
{
    global $user;
    $recordid = sget($_REQUEST,'recordid');
    $Block['title'] = 'Logo';
    if($op == 'view' || $op == null){
        $sql = "select * from research_company_data where recordid = $recordid and data_type = 'logo'";
        $result = db_query( $sql );
        if( db_affected_rows($result) > 0 )
        {
            $Data = db_fetch_object($result);
            $img = $Data->data_value;
            $src = "/" . file_directory_path() . "/" . $img;
            //echo $src;
            $content = "<div class='logo'><img src='$src' style='width:130px;'></div>";
            $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='research_company_logo' data='$recordid'>edit</a></div>";
        }else{
            
            $content = "<div class='edit-link'><a href='#' class='edit_wnd' rel='research_company_logo' data='$recordid'>upload a logo</a></div>";
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
                        $recordid = sget($_REQUEST,'recordid');
                        $sql = "select * from research_company_data where recordid = $recordid and data_type = 'logo'";
                        $result = db_query($sql);
                        $count = db_affected_rows($result);
                        $editorid = $user->uid;
                        $time = time();
                        if($count)
                        {
                            $row = db_fetch_array($result);
                            $dataid = sget($row, 'dataid');
                            $sql = "update research_company_data set data_value = '$filename', editorid = $editorid, access = '$time' where dataid = $dataid";
                            db_query($sql);
                        }else{
                            $sql = "insert into research_company_data (recordid, data_type, data_value, access, editorid) values ($recordid, 'logo', '$filename', '$time', $editorid)";
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


function research_company_brief_info($op = null)
{
    global $user;
    $editorid = $user->uid;
    $recordid = sget($_REQUEST,'recordid');
    $Block['title'] = 'Company Info';
    if($op == 'view' || $op == null){
        $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_research_company_breif_info' data='$recordid'>edit</a></div>";
        
        
        $sql = "select * from research_company where recordid = $recordid";
        $result = db_query($sql);
        $Data = db_fetch_object($result);
   
        $phone = ! empty($Data->phone) ? $Data->phone : 'n/a';
        $street1 =  ! empty($Data->address1) ? $Data->address1 : '';
        $street2 =  ! empty($Data->address2) ? ", " . $Data->address2 : '';
        $city =  ! empty($Data->city) ? "<br>" . $Data->city : 'n/a';
        $state =  ! empty($Data->state) ? ", " . $Data->state : 'n/a';
        $website =  ! empty($Data->web) ? $Data->web : 'n/a';
        
        
        $founded = 'n/a';$foundedid ="";
        $numOfEmp = 'n/a';$numOfEmpid = "";
        
       
            $numOfEmp = $Data->empnum;
            if($numOfEmp)$numOfEmp = number_format($numOfEmp);
  
        
        
        
            $founded = $Data->founded;
            $foundedid = $Data->data_id;
        
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
        $numOfEmp = sget($_POST,'empnum');
        $street1 = sget($_POST,'street1');
        $street2 = sget($_POST,'street2');
        $city = sget($_POST,'city');
        $state = sget($_POST,'state');
        $website = sget($_POST,'website');
        $phone = sget($_POST,'phone');
        
        $time = time();
        if($numOfEmp) $numOfEmp = preg_replace("/[^0-9]/",'', $numOfEmp);
        if($phone) $phone = preg_replace("/[^0-9]/",'', $phone);
        
        $recordid = sget($_POST,'recordid');
        $sql = "update research_company set phone = '%s', web = '%s', address1='%s', address2 = '%s', city='%s', state='%s',empnum=%d, founded='%s',access='%s' where recordid = $recordid limit 1";
        db_query($sql, array($phone, $website, $street1, $street2, $city, $state, $numOfEmp, $founded, '$time'));
        
        
        drupal_set_message("Company information is updated");
        //$_SESSION['messages']['status'] = 'Cour company information is updated';
        
        //drupal_goto("account","a=edit_company_detail");
    }
    return $Block;
}
function research_company_management($op = null)
{
    global $user;
    $editorid = $user->uid;
    $recordid = sget($_REQUEST,'recordid');
    $Block['title'] = 'Management';
    if($op == 'view' || $op == null){
        $content = "<ul class=''>";
        
        $sql = "select * from research_company_data where recordid = $recordid and data_type = 'management' order by data_attr";
        $result = db_query($sql);

        for($i = 1; $i <= 3; $i++)
        {
            $row = db_fetch_object($result);
            $position = '';
            $name = '';
            if($row !== false)
            {
                $name = ucwords(check_plain($row->data_value));
                $position = strtoupper(check_plain($row->data_value2));
                $phone = $row->data_value3;
                $email = $row->data_value4;
            }
            if(empty($name) && empty($position) && empty($phone) && empty($email)) continue;
            $content .= "<li style='list-style:none;margin-left:-10px;'>
                <table width='100%'><tr><td align=left style='border-bottom:none;padding-bottom:0;'>$name</td><td align=right style='border-bottom:none;padding-bottom:0;'>$phone</td></tr>
            <tr><td align=left style='border-top:none;'>$position</td><td align=right style='border-top:none;'>$email</td></tr></table></li>";
        }
        $content .= "</ul>";
        $content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_company_management' data='$recordid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        //test_array($_POST);exit;
        $position1 = sget($_POST,'position1');
        $name1 = sget($_POST,'name1');
        $phone1 = sget($_POST, 'phone1');
        $email1 = sget($_POST,'email1');
        $recid1 = sget($_POST,'recid1');
        $position2 = sget($_POST,'position2');
        $name2 = sget($_POST,'name2');
        $phone2 = sget($_POST, 'phone2');
        $email2 = sget($_POST,'email2');
        $recid2 = sget($_POST,'recid2');
        $position3 = sget($_POST,'position3');
        $name3 = sget($_POST,'name3');
        $phone3 = sget($_POST, 'phone3');
        $email3 = sget($_POST,'email3');
        $recid3 = sget($_POST,'recid3');
        $flag = true;
        $time = time();
        for($i = 1; $i <= 3; $i++)
        {
            $name = sget($_POST, "name" . $i);
            $position = sget($_POST, "position" . $i);
            $phone = sget($_POST, "phone" . $i);
            $email = sget($_POST, "email" . $i);
            $dataid = sget($_POST, "recid" . $i);
            
            if($dataid > 0)
            {
                $sql = "update research_company_data set data_value = '%s', data_value2 = '%s',data_value3 = '%s',data_value4 = '%s',access='%s',editorid='%s' where recordid = $recordid and dataid =$dataid and data_type = 'management'";
                $sql_data = array($name, $position,$phone,$email, $time,$editorid);
            }else{
                $sql = "insert into research_company_data (recordid, data_attr, data_value, data_value2,data_value3,data_value4,data_type,access,editorid) values('%d', '%d','%s','%s','%s','%s','%d')";
                $sql_data = array($recordid, $i, $name, $position,$phone, $email,'management', $time,$editorid);
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
    $uid = $user->uid;
    $Block['title'] = 'Existing Market';
    if($op == 'view' || $op == null){
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
        
        $content .= "</ul><div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_company_market_info'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        
    }
    return $Block;
}
function research_company_market_info($op = null)
{
    global $user;
    $editorid = $user->uid;
    $recordid = sget($_REQUEST,'recordid');
    $Block['title'] = 'Market Information';
    $array = array("competitor","market","expansion");
    $time = time();
    if($op == 'view' || $op == null){
        $content = "";
        
        $sql = "select * from research_company_data where recordid = $recordid and data_type = 'marketinfo' order by (data_value3 * 1) desc";
        $result = db_query($sql);
        
        $html_market = '<table border=0>'; 
        
        
        for($i = 1; $i <=5 ; $i++)
        { 
            if(($data = db_fetch_object($result)) !== false)
            {

                $c .= "\n<tr><td align=left>&nbsp;&nbsp;".check_plain($data->data_value2) . "</td><td align=right>".$data->data_value3."% &nbsp;&nbsp;</td></tr>"; 
            }
        }
        if($c == '') $c = '<tr><td>n/a </td></tr>';
        $html_market .= $c . "</table>";
            
        
            
        $content .= $html_market;
        
        $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='research_company_market_info' data='$recordid'>edit</a></div>";
        
        $Block['body'] = content_box($Block['title'], $content);
    }elseif($op == 'update')
    {
            for($i = 1; $i <= 5; $i++)
            {
                $market = sget($_POST, "market_" . $i);
                $share = sget($_POST, "share_" . $i);
                $dataid = sget($_POST, "market_id_" . $i,'decimal');
          
                if($dataid > 0){
                    $sql = "update research_company_data set data_value2 = '%s',data_value3 = '%s', editorid = $editorid, access='$time' where dataid = $dataid and recordid = $recordid limit 1";
                    
                    $sql_data = array($market, $share);
                }else{
                    $sql = "insert into research_company_data (recordid, data_type, data_attr, data_value2,data_value3,editorid, access) values($recordid, 'marketinfo', '$key', '%s','%s',$editorid, $time)";
                     $sql_data = array($market, $share);
                }
                
                if( ! db_query($sql, $sql_data))
                {
                    drupal_set_message("Failed to update market info. Please try again later",'error');
                    return ;
                }
            }
        
        drupal_set_message("Company market information is updated.");
    }
    return $Block;
}
function research_company_links($op = null)
{
    global $user;
    $editorid = $user->uid;
    $recordid  = sget($_REQUEST,'recordid');
    $time = time();
    $correct = true;
    $Block['title'] = 'Links';
    if($op == 'view' || $op == null){
        $content = "<ul>";
        $sql = "select * from research_company_data where recordid = $recordid and data_type = 'links' order by dataid";
       
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
        
        $content .= "</ul><div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_company_links' data='$recordid'>edit</a></div>";
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
                $sql = "update research_company_data set data_value = '$title', data_value2 = '$url', access='$time', editorid='$editorid' where dataid = $dataid limit 1 ";
                $sql_data = array();
            }else{
                $sql = "insert into research_company_data (recordid, access, data_type, data_value, data_value2, editorid) values('%d','%s','%s','%s','%s','%d')";
                $sql_data = array($recordid, $time, 'links', $title, $url, $editorid);
            }
            if( ! db_query($sql, $sql_data)){
                drupal_set_message("Error: failed to update company links.",'error');
                $correct = false;
                break;
            }
        }
        if($correct) drupal_set_message("Company market information is updated.");
    }
    return $Block;
}
function research_company_name($op = null)
{
    global $user;
    $editorid = $user->uid;
    $recordid = sget($_REQUEST,'recordid');
    $time = time();
   
    $sql = "select company, description from research_company where recordid =$recordid";
    $result = db_query($sql);
    $row = db_fetch_object($result);
    $company_name = strtoupper($row->company);
    $company_desc = htmlentities($row->description, ENT_QUOTES);

    $Block['title'] = $company_name;
    if($op == 'view' || $op == null){
        
        $content = "<div style='font-size:12px;' lines=3 trueH=0 sH=0 class='morediv'>$company_desc</div>";
        if(strlen($company_desc) > 300)
        {
            $content .= "<div style='text-align:right;padding-right:5px;'><a href='#' class='morediv_link' closed=1>More >></a></div>";
        }
        $content .= "</ul><div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_company_name' data='$recordid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
       
        $company_name = sget($_POST,'company');
        $company_desc = sget($_POST,'desc');
        $sql = "update research_company set company = '%s', description = '%s',access='%s' where recordid = $recordid limit 1";
        $sql_data = array($company_name, $company_desc,$time);
        if(db_query($sql, $sql_data)) drupal_set_message("Company information is updated.");
        else drupal_set_message("Failed to update company information.",'error');
    }
    return $Block;
}

function research_company_financials($op = null)
{
    global $user;
    $editorid = $user->uid;
    $recordid = sget($_REQUEST,'recordid');
    $time = time();
    $Block['title'] = 'Company Financials';
    if($op == 'view' || $op == null){
        
        /*include_once '/var/www/vhosts/clewed.com/httpdocs/php-ofc-library/open_flash_chart_object.php';
        $content = "<div style='position:relative;z-index:2;' id='financial_chart'>";
        $content .= open_flash_chart_object( 500, 250, 'http://'. $_SERVER['SERVER_NAME'] .'/open_chart?companyid=' . $uid , false );
        $content .= "</div>";
        */
        $sql = "select * from research_company_data where recordid = $recordid and data_type = 'financial' order by data_attr desc";
        $result = db_query($sql);
        $Data = array();
        $content = "<div><table cellspacing=0 cellpadding=0><tr><td></td><td></td><td></td><td></td><td></td><td></td></tr>";
        if(db_affected_rows($result) > 0)
        {
            while(($row = db_fetch_object($result) ) !== false)
            {
                $year = $row->data_attr;
                $Data["$year"] = $row;
            }
            $row1 = "<tr><td width=100></td>";
            $row2 = "<tr><td>Revenues</td>";
            $row3 = "<tr><td>After-Tax Earnings</td>";
            $thisYear = date("Y");
            $Years = array();
            for($i = $thisYear; $i >= $thisYear - 4; $i--)
            {
                $Years[] = $i;
            }
            if(count($Data) > 0)
            {
                foreach($Years as $year)
                {
                    $row1 .= "<td align=right>$year</td>";
                    $revenue = '';
                    $earnings = '';
                    if(isset($Data["$year"])){
                        $row = $Data["$year"];
                        if($row->data_value)$revenue = "\$" .number_format($row->data_value);
                        if($row->data_value2)$earnings ="\$" . number_format($row->data_value2);
                    }
                    $row2 .= "<td align=right>$revenue</td>";
                    $row3 .= "<td align=right>$earnings</td>";
                }
            }
            $row1 .= "</tr>";
            $row2 .= "</tr>";
            $row3 .= "</tr>";
        }
        $content .= "\n $row1 \n $row2 \n $row3 \n</table></div>";
        
        $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_research_company_financial' data='$recordid'>edit</a></div>";
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
            $dataid = (int)sget($_POST,'dataid_'.$theYear);
            if($dataid > 0){
                $sql = "update research_company_data set data_value = '%s', data_value2 = '%s',access='%s',editorid='%d' where dataid = $dataid and data_type = 'financial' and data_attr = '$theYear'";
                $sql_data = array($revenue, $earnings,$time, $editorid);
            }else{
                $sql = "insert into research_company_data (recordid, data_type, data_attr, data_value, data_value2,access,editorid) values ($recordid, 'financial', '$theYear', '%s','%s','$time','$editorid')";
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
function research_progress_status($op = null)
{
    global $user;
    $editorid = $user->uid;
    $recordid = sget($_REQUEST,'recordid');
    $time = time();

    $Block['title'] = 'Progress Status';
    if($op == 'view' || $op == null){
        $content = "<div><table cellspacing=0 cellpadding=0><tr><td width=70>Date</td><td width=100>Contact Name</td><td>Message</td><td width=100 align=right>Follow-Up Date</td></tr>";
        $sql = "select * from research_company_data  where recordid = $recordid and data_type = 'progress' order by dataid desc ";
        $result = db_query($sql);
        $counter = 1;
        
        while(($row = db_fetch_object($result)) !== false)
        {
            $date = '';
            $followup = '';
            if($row->data_attr) $followup = date('m/d/Y', $row->data_attr);
            if($row->data_value3) $date = date('m/d/Y', $row->data_value3);
           
            $contact = $row->data_value;
            $message = htmlentities( $row->data_value2,ENT_QUOTES);
            
            $content .= "<tr><td>$date</td><td>$contact</td><td>$message</td><td align=right>$followup</td></tr>\n";
        }
        $content .= "</table></div>";
        
        $content .= "</ul><div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_company_progress' data='$recordid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        $date = sget($_REQUEST,'date');
        if($date ) $date = strtotime($date);
        $contact = sget($_REQUEST,'contact');
        $message = sget($_REQUEST, 'message');
        $followup = sget($_REQUEST,'followup');
        if($followup)$followup = strtotime($followup);
        $sql = "insert into research_company_data (recordid, editorid, access, data_type, data_attr, data_value, data_value2, data_value3) values('%d','%d','%s','%s','%s','%s','%s','%s')";
        $sql_data = array($recordid, $editorid, $time, 'progress', $followup, $contact, $message,$date);
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
function research_company_followers($op = null){
    $recordid = sget($_REQUEST,'recordid');
    $Block['title'] = 'Followers';
    if($op == null || $op == 'view')
    {
        $sql = "select * from research_company_data where recordid = $recordid and data_type = 'follower' order by dataid desc limit 6";
        $result = db_query($sql);
        $content = '<table cellspacing=0 cellpadding=0 border=1 style="margin-top:0">';
        for($m = 1; $m <=2; $m++)
        {
            $content .= "\n<tr>";
            for($i = 1; $i <= 3 ; $i++)
            {
                $followerid = '';
                $followerName = '';
                $td = '&nbsp;';
                
                if(($row = @db_fetch_object($result)) != false)
                {
                    $followerid = $row->data_attr;
                    $followerName = $row->data_value;
                    $td = "<a href=''>$followerName</a>";
                }
                
                
                $content .= "<td>$td</td>";
            }
            $content .= "</tr>";
        }
        
        $content .= "</table>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}

function research_links_group($op = null)
{
    $recordid = sget($_REQUEST, 'recordid');

    $Block['title'] =  'Request a professional'; 
    if($op == 'view' || $op == '')
    {
        $Block['body']  =  "<div class='pagelink'><a href='#' class='edit_wnd' rel='edit_research_company_financial' data='$recordid'>Ask a question</a></div>" .
                           "<div class='pagelink'><a href='#' class='edit_wnd' rel='edit_research_company_financial' data='$recordid'>Invite Collaborator</a></div>" .
                           "<div class='pagelink'><a href='#' class='' rel='edit_research_company_financial' data='$recordid'>Follow this company</a></div>" .
                           "<div class='pagelink'><a href='#' class='edit_wnd' rel='edit_research_company_financial' data='$recordid'>Join Relationship Team</a></div>" ;
    }
    return $Block;
}

function research_company_prospects($op = null)
{
    $recordid = sget($_REQUEST,'recordid');
    $Block['title'] = "Screening Prospect";
    global $user;
    $editorid = $user->uid;
    $time = time();
    if($op == null || $op == 'view')
    {
        $Prospects = array('high', 'medium', 'low', 'unlikely');
        $prospect = 'not set';
        $sql = "select company,recordid,priority from research_company where recordid = $recordid limit 1";
        $result = db_query($sql);
        if(($row = db_fetch_object($result)) !== false)
        {
            
            if($row->priority) $prospect = $row->priority;
        }

        $content = "\n<div style='width:100%;height:30px;line-height:30px;text-align:center;font:12px arial;'>$prospect</div>";
        $content .= "\n<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_company_prospects' data='$recordid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
       
        $prospect = sget($_REQUEST,'prospect');
        $dataid = sget($_REQUEST,'dataid');
        $recordid = sget($_REQUEST,'recordid');
        
            $sql = "update research_company set priority = '$prospect', access='$time' where recordid = $recordid limit 1";
            
            $sql_data = array();
        
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

function research_company_qualified_lead($op = null)
{
    $recordid = sget($_REQUEST,'recordid');
    $Block['title'] = "Qualified Sales Lead";
    global $user;
    $editorid = $user->uid;
    $time = time();
    if($op == null || $op == 'view')
    {
        $Prospects = array('high', 'medium', 'low', 'unlikely');
        $prospect = 'not set';
        $sql = "select company,recordid,qualified from research_company where recordid = $recordid limit 1";
        $result = db_query($sql);
        if(($row = db_fetch_object($result)) !== false)
        {
            
            if($row->qualified) $prospect = $row->qualified;
        }

        $content = "\n<div style='width:100%;height:30px;line-height:30px;text-align:center;font:12px arial;'>$prospect</div>";
        $content .= "\n<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_company_qualified_lead' data='$recordid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
       
        $prospect = sget($_REQUEST,'prospect');
        $dataid = sget($_REQUEST,'dataid');
        $recordid = sget($_REQUEST,'recordid');
        
            $sql = "update research_company set qualified = '$prospect', access='$time' where recordid = $recordid limit 1";
            
            $sql_data = array();
        
        if(db_query($sql, $sql_data))
        {
            drupal_set_message("Qaulified Lead setting updated");
        }else{
            drupal_set_message("Error: failed to update Qaulified Lead setting",'error');
        }
        return;
    }
    return $Block;
    
}

function research_followup_priority($op = null)
{
    $recordid = sget($_REQUEST,'recordid');
    $Block['title'] = "Follow-Up Priorities";
    global $user;
    $editorid = $user->uid;
    $time = time();
    
    if($op == null || $op == 'view')
    {
        
        $sql = "select * from research_company_data where data_type = 'followup' and recordid = $recordid order by data_attr asc limit 6";
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
        $content .= "\n<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_followup_priority' data='$recordid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        
        $recordid = sget($_REQUEST,'recordid');
        $note = sget($_REQUEST,'note');
        $date = sget($_REQUEST,'date');
        $date_stamp = strtotime($date);
        
            $sql = "insert into research_company_data(recordid, data_type, data_attr, data_value, access, editorid) values('%d','%s','%s','%s','%s','%d') ";
            $sql_data = array($recordid, 'followup', $date_stamp, $note, $time, $editorid);
        
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

function research_companies_to_follow($op = null)
{
    $recordid = sget($_REQUEST,'recordid');
    $Block['title'] = "Companies To Follow";
    global $user;
    $editorid = $user->uid;
    $time = time();
    $content = 'n/a';
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}


function research_company_articles($op = null)
{
    $recordid = sget($_REQUEST,'recordid');
    $Block['title'] = "Articles";
    global $user;
    $editorid = $user->uid;
    $time = time();
    
    if($op == null || $op == 'view')
    {
        
        $sql = "select * from research_company_data where data_type = 'article' and recordid = $recordid order by dataid asc limit 6";
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
        $content .= "\n<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_company_article' data='$recordid'>edit</a></div>";
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
            $sql = "insert into research_company_data (recordid, data_type, data_attr, data_value, data_value2, access, editorid)
                                                        values('%d','%s','%s','%s','%s','%s','%d')";
            $sql_array = array($recordid, $data_type, $data_attr, $title, $url, $time, $editorid);
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
                        drupal_set_message("A new file has been uploaded.");
                        $recordid = sget($_REQUEST,'recordid');
                        $sql = "select * from research_company_data where recordid = $recordid and data_type = 'logo'";
                        $result = db_query($sql);
                        $count = db_affected_rows($result);
                        $editorid = $user->uid;
                        $time = time();
                        
                            $sql = "insert into research_company_data (recordid, data_type, data_value, access, editorid) values ($recordid, 'article', '$filename', '$time', $editorid)";
                            $sql_array = array();
                            
                        
                    }else{
                        drupal_set_message("Failed to upload new file",'error');
                    }
                }else{
                    drupal_set_message("The file type is not allowed",'error');
                }
        }
        
        
        if(db_query($sql, $sql_array))
        {
            drupal_set_message("A new article record is added");
        }else{
            drupal_set_message("Error: failed to update article record",'error');
        }
        return;
    }
    return $Block;
    
}


/* EOF */