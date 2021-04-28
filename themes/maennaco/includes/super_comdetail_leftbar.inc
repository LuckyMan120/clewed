<?php
$maenna_page['left'] = array('');

$section_tab = sget($_REQUEST, 'tab');

switch($section_tab)
{
    case 'questionnaire':
        $maenna_page['left'] = array('company_name','diligence');
        break;
    case 'analysis':
        $maenna_page['left'] = array('company_name','team_stats','sections');
        break;
    case 'plan':
        $maenna_page['left'] = array('company_name','team_stats','sections');
        break;
    case 'monitoring':
        $maenna_page['left'] = array('company_name','team_stats','who_we_are', 'what_we_do','key_markets', 'milestones' );
        break;
    case 'minutes':
        $maenna_page['left'] = array('company_name','team_stats','sections');
        break;
    default:
        $maenna_page['left'] = array('company_name','team_stats','sections');
        break;
}

function sections($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = "Name";
    $content = '';
    $tab = sget($_REQUEST, 'tab');
    $data_type = $tab;
    
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and deleted != 1 order by data_attr desc, dataid desc limit 10";
        $result = db_query($sql,array($companyid, $data_type));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $Block['title'] = "Sections"; // htmlentities(ucwords($Row->content), ENT_QUOTES);
            $parentid = $Row->recordid;
            $counter = 1;
            do
            {
                $dataid = $Row->dataid;
               
                $title =   "<a href='/account?a=companies&type=company_detail&companyid=$companyid&tab=${tab}&panel=multi&view=detail&dataid=${dataid}'>" . htmlentities(strtoupper($Row->data_value), ENT_QUOTES | ENT_IGNORE, "UTF-8") ."</a>";
                $content .= "\n<div class=row style='padding:7px 0'>$title</div>";
                $counter++;
            }while($Row = db_fetch_object($result));
        }
        /*$content .= "<div class=row style='border:none;height:14px;'>
                        <div class='editbtn'><a href='/account?a=questionnaire&parentid=$parentid' class='tool'>More</a>
                    </div></div>";
        */
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function company_name($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'companyid');
    $Block['title'] = "";
    $content = '';
    if($op == 'view' || $op == null){
        $a = sget($_REQUEST, 'a');
        if(empty($a))$a = 'questionnaire';
        $sql = "select * from maenna_company where companyid  = $companyid ";
        $result = db_query($sql);
        $Data = db_fetch_object($result);
        $projName = $Data->projname;
        if(empty($projName))$projName = getProjectName($companyid);
        $Block['title'] = "". htmlentities(strtoupper($projName), ENT_QUOTES | ENT_IGNORE, "UTF-8") ."";
        $Block['title'] .= "<div class=editbtn><a href='/account?a=companies&type=company_detail&companyid=$companyid&panel=edit_company_info' class='tool' >Edit</a></div>";
        
        
        $founded = 'n/a';$foundedid ="";
        $numOfEmp = 'n/a';$numOfEmpid = "";
        
        $numOfEmp = $Data->empnum;
        if($numOfEmp)$numOfEmp = number_format($numOfEmp);
        $founded = $Data->founded;
        $foundedid = $Data->data_id;
        
        $phone = ! empty($Data->phone) ? $Data->phone : 'n/a';
        $street1 =  ! empty($Data->address1) ? $Data->address1 : '';
        $street2 =  ! empty($Data->address2) ? $Data->address2 : '';
        $city =  ! empty($Data->city) ?  $Data->city : '';
        $state =  ! empty($Data->state) ?  $Data->state : '';
        $zip =  ! empty($Data->zip) ? $Data->zip : '';
        
        $address = '';
        if($street1) $address = "$street1 <br>";
        if($street2) $address .= "$street2 <br>";
        if($city)$address .= "$city <br>";
        if($state . $zip) $address.= "$state, $zip";
        
        $website =  ! empty($Data->web) ? $Data->web : 'n/a';
        if($website && strlen($website) > 14){
            $website = "<a href='$website' target='_blank'>".substr($website, 0, 14). "</a>";
        }
        
        $logo = $Date->logo;
        if(empty($logo))$logo = '132076369_factory.jpg';
        $src = "./" . file_directory_path() . "/" . $logo;
        
$content .= <<< END
    <table class='no-border'>
        <tr>
            <td style='width:130px;'>
                <div style='position:relative;height:117px;width:117px' class='blue-border'><img src='$src' style='width:117px;height:117px'</div>
            </td>
            <td style=''>
                <div class=row  style='font-family:Signika'>Founded: $founded</div>
                <div class=row  style='font-family:Signika'>Employees: $numOfEmp</div>
                
                <!--div class=row  style='font-family:Signika'>Website:       <span>$website</span></div-->
                <!--div class=row  style='font-family:Signika'>Phone:       <span>$phone</span></div>
                <div class=row>Address:       <div style=''>$address</div></div-->

            </td>
        </tr>
    </table>
    
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
        $zip = sget($_REQUEST,'zip');
        $time = time();
        if($numOfEmp) $numOfEmp = preg_replace("/[^0-9]/",'', $numOfEmp);
        if($phone) $phone = preg_replace("/[^0-9]/",'', $phone);
        
        $companyid = sget($_POST,'companyid');
        $sql = "update maenna_company set phone = '%s',
                                            web = '%s',
                                            address1='%s',
                                            address2 = '%s',
                                            city='%s',
                                            state='%s',
                                            zip='%s',
                                            empnum=%d,
                                            founded='%s',
                                            access='%s'
                                            where companyid = $companyid limit 1";
        if(db_query($sql, array($phone, $website, $street1, $street2, $city, $state,$zip, $numOfEmp, $founded, '$time')))
        {
            drupal_set_message("Company information is updated");
        }
        else{
            drupal_set_message("Failed to update company info",'error');
        }
        //$_SESSION['messages']['status'] = 'Cour company information is updated';
        
        //drupal_goto("account","a=edit_company_detail");
    }
    return $Block;
}

function diligence($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = "Name";
    $content = '';
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_questionair where parentid is null and target = 'company' and status = 1";
        $result = db_query($sql);
        $Row = db_fetch_object($result);
        if($Row)
        {
            $Block['title'] = "Sections"; // htmlentities(ucwords($Row->content), ENT_QUOTES);
            $parentid = $Row->recordid;
            
            $sql = "select * from maenna_questionair where parentid = %d and (type = 'title') order by weight, recordid";
            $result = db_query($sql, array($parentid));
            $counter = 1;
            while(($Row = db_fetch_object($result)) )
            {
                $questionid = $Row->recordid;
                $title =   "<a href='/account?a=companies&type=company_detail&companyid=$companyid#t_$questionid'>" . htmlentities(ucwords($Row->content), ENT_QUOTES | ENT_IGNORE, "UTF-8") ."</a>";
                $content .= "\n<div class=row style='padding:7px 0'>$title</div>";
                $counter++;
            }
        }
        /*$content .= "<div class=row style='border:none;height:14px;'>
                        <div class='editbtn'><a href='/account?a=questionnaire&parentid=$parentid' class='tool'>More</a>
                    </div></div>";
        */
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function what_we_do($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = "WHAT WE DO";
    $tab = sget($_REQUEST,'tab');
    $panel = "wwd_panel";
    $data_type = 'what_we_do';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    ///
    $content = '';
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type'";
        $result = db_query($sql,array($companyid));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = sideBarExcerpt($content, $redirect . "&panel=${panel}&view=detail");
            
        }
        $Block['title'] .= "<div class='editbtn'><a href='${redirect}&panel=${panel}&view=edit' class='tool'>Edit</a></div>";
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}
function wwd_panel($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $time = time();
    $Block['title'] = "What we do";
    $panel = "wwd_panel";
    $data_type = 'what_we_do';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    $dataid = '';
    $content = '';

    if($op == null || $op == 'view'){
        $view = sget($_REQUEST, 'view');
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type'";
        $result = db_query($sql, array($companyid));
        $Row = db_fetch_object($result);
        if($Row){
            $content = $Row->data_value2;
            $dataid = $Row->dataid;
        }
        if($view == 'edit'){
            $Block['title'] = "Edit - " . $Block['title'];
            $content = "<form method=post action='/account'>
                        <table class='no-border'>
                            <tr>
                                <td><textarea name='$data_type' style='width:98%;height:400px;'>$content</textarea></td> 
                            </tr>
                            <tr>
                                <td><input type=submit name=submit value=submit class=button /> <a href='$redirect' class=button>Cancel</a></td>
                            </tr>
                        </table>
                            <input type=hidden name=dataid value='$dataid' />
                            <input type=hidden name=update_section value='$panel' />
                            <input type=hidden name=view value='detail' />
                        ";
            $content .= hidden_post_values(array('a','type','companyid', 'tab','panel'));
            $content .= "</form>";
        }elseif($view == 'detail'){
            $content = nl2br(htmlentities($content, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $content = "<div class=entry>
                            <div class=entry-content>$content</div>
                        </div>
                        <div class=backbtn><a href='$redirect' class='button'>back</a></div>";
        }
    }elseif($op == 'update'){

        $data_value2 = sget($_REQUEST, $data_type);
        $companyid = sget($_REQUEST, 'companyid');
        $dataid = sget($_REQUEST, 'dataid');
    
        if(empty($companyid)) drupal_set_message("Invalid company id",'error');
        else{
            if($dataid){
                $sql = "update maenna_company_data set data_value2 = '%s', access='$time',editorid=$editorid where data_type = '$data_type' and dataid = %d limit 1";
                $DBValues = array($data_value2, $dataid);
            }else{
                $sql = "insert into maenna_company_data (companyid, access, data_type, data_value2,editorid ) values (%d, '%s','%s','%s',%d)";
                $DBValues = array($companyid, $time, $data_type, $data_value2, $editorid);
            }
            if(db_query($sql, $DBValues))drupal_set_message("Data record is updated");
            else drupal_set_message("Failed to update record", 'error');
        }
        return ;
    }
     $Block['body'] = content_box($Block['title'], $content);
    return $Block; 
}
///////////////////////////////////////////////////
function who_we_are($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = "WHO WE ARE";
    $tab = sget($_REQUEST,'tab');
    $panel = "wwa_panel";
    $data_type = 'who_we_are';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    ///
    $content = '';
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type'";
        $result = db_query($sql,array($companyid));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = sideBarExcerpt($content, $redirect . "&panel=${panel}&view=detail");
            
        }
        $Block['title'] .= "<div class='editbtn'><a href='${redirect}&panel=${panel}&view=edit' class='tool'>Edit</a></div>";
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}
function wwa_panel($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $time = time();
    $Block['title'] = "WHO WE ARE";
    $panel = "wwa_panel";
    $data_type = 'who_we_are';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    $dataid = '';
    $content = '';

    if($op == null || $op == 'view'){
        $view = sget($_REQUEST, 'view');
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type'";
        $result = db_query($sql, array($companyid));
        $Row = db_fetch_object($result);
        if($Row){
            $content = $Row->data_value2;
            $dataid = $Row->dataid;
        }
        if($view == 'edit'){
            $Block['title'] = "Edit - " . $Block['title'];
            $content = "<form method=post action='/account'>
                        <table class='no-border'>
                            <tr>
                                <td><textarea name='$data_type' style='width:98%;height:400px;'>$content</textarea></td> 
                            </tr>
                            <tr>
                                <td><input type=submit name=submit value=submit class=button /> <a href='$redirect' class=button>Cancel</a></td>
                            </tr>
                        </table>
                            <input type=hidden name=dataid value='$dataid' />
                            <input type=hidden name=update_section value='$panel' />
                            <input type=hidden name=view value='detail' />
                        ";
            $content .= hidden_post_values(array('a','type','companyid', 'tab','panel'));
            $content .= "</form>";
        }elseif($view == 'detail'){
            $content = nl2br(htmlentities($content, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $content = "<div class=entry>
                            <div class=entry-content>$content</div>
                        </div>
                        <div class=backbtn><a href='$redirect' class='button'>back</a></div>";
        }
    }elseif($op == 'update'){

        $data_value2 = sget($_REQUEST, $data_type);
        $companyid = sget($_REQUEST, 'companyid');
        $dataid = sget($_REQUEST, 'dataid');
    
        if(empty($companyid)) drupal_set_message("Invalid company id",'error');
        else{
            if($dataid){
                $sql = "update maenna_company_data set data_value2 = '%s', access='$time',editorid=$editorid where data_type = '$data_type' and dataid = %d limit 1";
                $DBValues = array($data_value2, $dataid);
            }else{
                $sql = "insert into maenna_company_data (companyid, access, data_type, data_value2,editorid ) values (%d, '%s','%s','%s',%d)";
                $DBValues = array($companyid, $time, $data_type, $data_value2, $editorid);
            }
            if(db_query($sql, $DBValues))drupal_set_message("Data record is updated");
            else drupal_set_message("Failed to update record", 'error');
        }
        return ;
    }
     $Block['body'] = content_box($Block['title'], $content);
    return $Block; 
}
//////////////////////////////////////////////////////
function key_markets($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = "OUR MARKETS";
    $tab = sget($_REQUEST,'tab');
    $panel = "key_markets_panel";
    $data_type = 'marketinfo';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    ///
    $content = '';
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type' order by (data_value3 * 1) desc";
        $result = db_query($sql,array($companyid));
        
        while($Row = db_fetch_object($result))
        {
            $market = $Row->data_value2;
            $share = $Row->data_value3;
            
            $content .= "<div class=row>$market <span class=right style='text-align:right;'>$share</span> </div>";
        }
        $Block['title'] .= "<div class='editbtn'><a href='${redirect}&panel=${panel}&view=edit' class='tool'>Edit</a></div>";
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}
function key_markets_panel($op = null){
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = "Edit - OUR MARKETS";
    $tab = sget($_REQUEST,'tab');
    $panel = "key_markets_panel";
    $data_type = 'marketinfo';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));

    $content = '';
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type' order by (data_value3 * 1) desc";
        $result = db_query($sql,array($companyid));
        $content = "<form method=post action='/account'>
                    <table class='edit_table no-border'><tr>
                                <td style='width:20px;'></td>
                                <td  style='width:200px;font-weight:bold'>OUR MARKETS</td>
                                <td style='font-weight:bold'>Share</td></tr>";
        $counter = 1;
        for($i = 1; $i <=5 ; $i++)
        {
            $Row = db_fetch_array($result);
            if(empty($Row)) $Row = array();
            $market = sget($Row, 'data_value2');
            $share = sget($Row, 'data_value3');
            $dataid = sget($Row, 'dataid');
            
            $content .= "\n<tr> <td >$counter.</td>
                                <td><input type=text name='market_$i' value='$market' style='width:180px;' /> </td>
                                <td><input type='text' name='share_$i' value='$share' style='width:80px;' />
                                    <input type=hidden name='dataid_$i' value='$dataid' /></td></tr>";
            $counter++;
        }
        $content .= "<tr><td></td><td colspan=2 style='padding-top:8px;'>
                        <input type=submit name=submit value='submit' class=button />
                        &nbsp;
                        <a href='$redirect' class=button>Cancel</a>
                    </td></tr></table>";
        $content .= hidden_post_values(array('a','type', 'tab', 'companyid', 'panel', 'view'));
        $content .= "<input type=hidden name='update_section' value='key_markets_panel' /></form>";
        
    }elseif($op == 'update')
    {
        for($i = 1; $i <= 5; $i++)
        {
            $market = sget($_REQUEST, "market_$i");
            $share = sget($_REQUEST, "share_$i");
            $dataid = sget($_REQUEST, "dataid_$i");
            if(empty($dataid)){
                 $sql = "insert into maenna_company_data (companyid, data_type, data_attr, data_value2,data_value3,editorid, access) values($companyid, 'marketinfo', '$i', '%s','%s',$editorid, $time)";
                  $sql_data = array($market, $share);
            }else{
                 $sql = "update maenna_company_data set data_value2 = '%s',data_value3 = '%s', editorid = $editorid, access='$time' where dataid = $dataid and companyid = $companyid limit 1";
                 $sql_data = array($market, $share);
            }
            if( ! db_query($sql, $sql_data))
            {
                drupal_set_message("Failed to update market info. Please try again later",'error');
                return ;
            }
        }
        drupal_set_message("Company market information is updated.");
        return;
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
//////////////////////////////////////////////////////////////////
function milestones($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = "OUR MILESTONES";
    $tab = sget($_REQUEST,'tab');
    $panel = "milestones_panel";
    $data_type = 'keymilestone';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    $content = '';
    
    if($op == 'view' || $op == null){
        $sql = "select * from maenna_company_data where companyid = %d and data_type = 'keymilestone' and deleted != 1 order by dataid desc limit 3";
        $result = db_query($sql, array($companyid));
        $counter = 0;
        while(($Row = db_fetch_object($result)) !== false)
        {
            $counter++;
            $title =    htmlentities($Row->data_attr,ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $text =     htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
            $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
            $text = sideBarExcerpt($text);
            $last = '';
            
            
            $content .= "<div class='entry'>
                            <div class=title>$title
                            
                            </div>
                        <div class=content>$text</div></div>";
                        ////    <div class=editbtn><a href='$edit_link' class=tool>Edit</a></div>
        }
        
        $url = $redirect . "panel=milestones_panel&view=add";
        $add_link = $redirect . "&panel=${panel}&view=add";
        $edit_link = $redirect . "&panel=${panel}&view=listview";
        $viewall_link = $redirect . "&panel=${panel}&view=listview";
        $Block['title'] .= "<div class=editbtn>
                                    <a href='$add_link' class=tool>Add</a>
                            </div>";
        $content .= "\n<div class='viewallbtn'><a href='$viewall_link' class=tool>More</a></div>";
    }
    
    
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function milestones_panel($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = "OUR MILESTONES";
    $tab = sget($_REQUEST,'tab');
    $panel = "milestones_panel";
    $data_type = 'keymilestone';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    $content = '';
    
    $add_link = $redirect . "&panel=${panel}&view=add";
    $Block['title'] .= "<div class=editbtn>
                                    <a href='$add_link' class=tool>Add</a>
                            </div>";
    
    if($op == null || $op == 'view')
    {
        $view = sget($_REQUEST, 'view');

        if($view == 'detail' || $view == 'edit' || $view == 'add')
        {
            
            $dataid = sget($_REQUEST, 'dataid');
            $Row = array();
            if($dataid)
            {
                $sql = "select * from maenna_company_data where dataid = %d limit 1";
                $result = db_query($sql, array($dataid));
                $Row = db_fetch_array($result);
            }elseif(empty($dataid) && ($view == 'detail')){
                $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' order by dataid desc limit 1";
                $result = db_query($sql, array($companyid, $data_type));
                $Row = db_fetch_array($result);
            }
            if($Row !== false)
            {
                if($view == 'detail'){
                    $dataid = sget($Row, 'dataid');
                    $year = strtoupper(sget($Row,'data_attr'));
                    $year = htmlentities($year, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $milestone = sget($Row, 'data_value2');                  
                    $milestone = nl2br(htmlentities($milestone, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                    $content = "<div class=entry>
                                    <div class=entry-title>$year</div>
                                    <div class=entry-content>$milestone</div>
                                </div>";
                    $rem_link = $redirect . "&panel=${panel}&view=listview&update_section=${panel}&do=remove&dataid=$dataid";
                    $content .= "<div class=backbtn>
                                    <div class='rembtn'><a href='$rem_link' class=button onclick='return confirm(\"Continue to remove record\")'>Delete</a></div>
                                    <a href='$redirect&panel=${panel}&view=listview' class=button>Back</a>
                                </div>";
                    $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
                    $Block['title'] .= "<div class=editbtn>
                                            <a href='$edit_link' class=tool>Edit</a></div>";
                }elseif($view == 'edit' || $view == 'add')
                {
                    $year = strtoupper(sget($Row,'data_attr'));
                    $dataid = sget($Row, 'dataid');
                    $milestone = sget($Row, 'data_value2');
                    if($view == 'edit'){
                        $title = "Edit Milestone";
                        $do = 'update';
                        $rem_link = $redirect . "&panel=${panel}&view=listview&update_section=${panel}&do=remove&dataid=$dataid";
                    }else{
                        $title = "Add Milestone";
                        $do = 'insert';
                        $rem_link = '';
                    }
                    $Block['title'] = $title;
                    $hv = hidden_post_values(array('a','type','companyid', 'tab','panel'));
                    $content .= <<< END
                    <form action='/account' method='post' onsubmit='return check_input();'>
                        <div class=entry>
                            <div class=entry-content>
                                <div>Year:<br /><input type=text name=year value='$year' class='require_string' /></div><br />
                                <div>Milestone:<br /><textarea name='milestone' style='width:99%;height:400px;' class='require_string' >$milestone</textarea>
                                <div class=backbtn>
                                    <div class='rembtn'><a href='$rem_link' class=button onclick='return confirm("Continue to remove record")'>Delete</a></div>
                                    <input type=submit name=submit value=submit class=button />
                                    &nbsp;&nbsp;
                                    <a href='$redirect&panel=${panel}&view=listview' class=button>Cancel</a>
                                </div>
                            </div>
                        </div>
                        $hv
                        <input type='hidden' name=dataid value='$dataid' />
                        <input type='hidden' name=view value='detail' />
                        <input type='hidden' name=do value='$do' />
                        <input type='hidden' name=update_section value='$panel' />
                    </form>
END;
                }
            }
       
            
        }
        elseif($view == 'listview')
        {
            $limit = 5;
            $page = sget($_REQUEST,'page');
            if(empty($page)) $page = 1;
            $start = ($page - 1) * $limit;
            
            $sql = "select count(*) as total from maenna_company_data where data_type = '$data_type' and companyid = %d and deleted != 1";
            $result = db_query($sql, array($companyid));
            $Row = db_fetch_object($result);
            $total = $Row->total;
            
            $sql = "select * from maenna_company_data where data_type = '$data_type' and companyid = %d and deleted != 1 order by dataid desc limit $start, $limit";
            $result = db_query($sql, array($companyid));
            while(($Row = db_fetch_object($result)) !== false)
            {
                $access = date('m/d/Y',$Row->access);
                $title = htmlentities($Row->data_attr, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                $dataid = $Row->dataid;
                $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
                $text = contentExcerpt($text, $more_link);
                $content .= "<div class=entry>";
                $content .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid' class='tool' >Edit</a></div>";
                $content .= "<div class='entry-title'>$title</div><div class='entry-content'>" . $text ."</div></div><hr class='line' />";
            }
            $Pagenation = array('total' => $total,
                'limit' =>5,
                'baseurl' => $redirect,
                'num_of_links' => 8,
            );
            $content .= pagination($Pagenation);
        }
    }elseif($op == 'update')
    {
        $do = sget($_REQUEST, 'do');
        $Correct = false;
        if($do == 'update' || $do == 'insert')
        {
            $dataid = sget($_REQUEST,'dataid');
            $year = sget($_REQUEST, 'year');
            $milestone = sget($_REQUEST, 'milestone');
                
            if(empty($year) || empty($milestone))
            {
                
            }
            elseif($dataid && ($do == 'update'))
            {
                $DBValues = array('access' => $time,
                                  'data_attr' => $year,
                                  'data_value2' => $milestone,
                                  'editorid' => $editorid);
                foreach($DBValues as $key => $val)
                {
                    $SQL_STR["$key"] = "$key = '%s'";
                }
                $SQL_STR["editorid"] = "editorid=%d";
                $sql = "update maenna_company_data set " . implode(',', $SQL_STR) . " where dataid = $dataid limit 1";
                if(db_query($sql, $DBValues))$Correct = true;
             
            }elseif($do == 'insert')
            {
                $DBKeys = array('companyid',
                                'access',
                                'data_type',
                                'data_attr',
                                'data_value2',
                                'editorid');
                $DBValues = array($companyid, $time, $data_type, $year, $milestone, $editorid);
                $SQL_STR = array("%d", "'%s'","'%s'","'%s'","'%s'","%d");
                $sql = "insert into maenna_company_data (".implode(',',$DBKeys).") values(".implode(',',$SQL_STR).")";
                if(db_query($sql, $DBValues)) $Correct = true;
            }
        }elseif($do == 'remove')
        {
            $dataid = sget($_REQUEST, 'dataid');
            if($dataid)
            {
                $sql = "update maenna_company_data set access = '%s', editorid = %d, deleted = 1 where dataid = %d limit 1";
                if(db_query($sql, array($time, $editorid, $dataid))) $Correct = true;
            }
        }
        if($Correct)drupal_set_message("Operation Successful");
        else{ drupal_set_message("Operation Failed", 'error'); }
        return ;
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function team_stats($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = '';
    $tab = sget($_REQUEST,'tab');
    $data_type = '';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    $content = '';
    
    if($op == null)
    {
        $content = <<< END
        <table class='team_stats' style='padding-bottom:0;margin-bottom:0;'>
            <tr class=link>
            <td class='active'><a href="#" ><span class='num'>20</span><span class=type>Advisors</span></a></td>
            <td ><a href="#" ><span class='num'>20</span><span class=type>Following</span></a></td>
            <td ><a href="#" ><span class='num'>20</span><span class=type>Followers</span></a></td>
            </tr>
            <tr>
                <td style='border:none;hieght:8px;line-height:8 px;' colspan=3>&nbsp;</td>
            </tr>
            <tr>
                <td colspan=3 style='border-color:#dce6f5;border-bottom:none'>
                    <div class="team_stat_list">
                        <div class="title" id="team_stat_title">Advisors</div>
                    </div>
                </td>
            </tr>
            <tr>
                <td colspan=3 style='border-top:none;'>
                    <div class="team_stat_list">
                        <div class="list" id="team_stat_list">
                            <div><a href=''>Professional A(analyst)</a></div>
                            <div><a href=''>Professional A(analyst)</a></div>
                            <div><a href=''>Professional A(analyst)</a></div>
                            <div><a href=''>Professional A(analyst)</a></div>
                        </div>
                    </div>
                </td>
            </tr>
        </table>
        
END;
    }
    $Block['body'] = sidebar_box('', $content);
    return $Block;
}
/*
function team_stats($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = '';
    $tab = sget($_REQUEST,'tab');
    $data_type = '';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    $content = '';
    
    if($op == null)
    {
        $content = <<< END
        <table class='team_stats'>
            <tr class=link>
            <td class='active'><a href="#" ><span class='num'>20</span><span class=type>Advisors</span></a></td>
            <td ><a href="#" ><span class='num'>20</span><span class=type>Following</span></a></td>
            <td ><a href="#" ><span class='num'>20</span><span class=type>Followers</span></a></td>
            </tr>
            <tr>
                <td style='border:none;hieght:8px;line-height:8 px;' colspan=3>&nbsp;</td>
            </tr>
            <tr>
                <td colspan=3 style='border-color:#658cc4;border-bottom:none'>
                    <div class="team_stat_list">
                        <div class="title" id="team_stat_title">Advisors</div>
                    </div>
                </td>
            </tr>
            <tr>
                <td colspan=3 style='border-top:none;'>
                    <div class="team_stat_list">
                        <div class="list" id="team_stat_list">
                            <div><a href=''>Professional A(analyst)</a></div>
                            <div><a href=''>Professional A(analyst)</a></div>
                            <div><a href=''>Professional A(analyst)</a></div>
                            <div><a href=''>Professional A(analyst)</a></div>
                        </div>
                    </div>
                </td>
            </tr>
        </table>
        
END;
    }
    $Block['body'] = sidebar_box('', $content);
    return $Block;
}*/
/* EOF */