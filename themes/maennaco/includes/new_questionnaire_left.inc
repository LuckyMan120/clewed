<?php

/*
function company_name($op = null)
{
    global $user, $AccessObj;
    $editorid = $user->uid;
    $companyid = $AccessObj->uid;
    $Block['title'] = "";
    $content = '';
    $redirect = rebuild_url(array('tab','page','mtab','id'));
    $section = __FUNCTION__;
    if($AccessObj->user_type != 'company') return '';
    $panel = 'edit_company_info';
    if( ($op != 'update') && $op){
        
        $sql = "select * from maenna_company where companyid  = $companyid ";
        $result = db_query($sql);
        $Data = db_fetch_object($result);
        $projName = $Data->projname;
        if(empty($projName))$projName = getProjectName($companyid);
        $_title = "". htmlentities(ucwords($projName), ENT_QUOTES | ENT_IGNORE, "UTF-8") ."";
        
        
        
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
        
        $edit = '';
        $follow_link = $request_link = '';
        if($AccessObj->user_type == 'people')
        {
            if(in_array($AccessObj->_relation, array('pu_analyst','pu_expert', 'pu_visible', 'pu_other')))
            {
                if(! Connections::is_following($companyid, $AccessObj->uid))
                {
                    $follow_link = "<div style='padding:0;margin:0;' class='inline-btn'><a href='$redirect&update_section=follow'>FOLLOW</a></div>";
                }else{
                    $follow_link = "FOLLOWING";
                }
                
            }
            if(in_array($AccessObj->_relation, array('pu_visible')))
            {
                if( Connections::allow_to_request($companyid, $AccessObj->uid))
                {
                    $request_link = "<div style='padding:0;margin:0;' class='inline-btn'><a href='$redirect&update_section=request_join_an'>REQUEST TO JOIN ADVISORY NETWORK</a></div>";
                }else{
                    $request_link = "<div>REQUESTED</div>";
                }
            }
        }
        
        
$content .= <<< END
    <div style='margin-top:-3px;padding-bottom:3px;'>
        <table cellpadding=0 cellspacing=0 class='no-border' style='padding:0;margin:0;'>
            <tr>
                <td colspan=2>
                    <div style='width:100%;height:80px;background:#f8f8f8 url(/themes/maennaco/images/pro_bg1.png) no-repeat;font:bold 16px Signika; color:white;text-align:center;line-height:80px;overflow:hidden'>
                        $_title
                    </div>
                </td>
            </tr>
            <tr>
                <td style='padding:5px 0; font: 12px Signika'>Founded: $founded</td>
                <td style='padding:5px 0; font: 12px Signika'>Employees: $numOfEmp</td>
            </tr>
        </table>
        
    </div>
    <!--table class='no-border'>
        <tr>
            <td style='width:130px;'>
                <div style='position:relative;height:117px;width:117px' class='blue-border'><img src='$src' style='width:117px;height:117px'</div>
            </td>
            <td style=''>
                <div class=row  style='font-family:Signika'>Founded: $founded</div>
                <div class=row  style='font-family:Signika'>Employees: $numOfEmp</div>
                
                <div class=row  style='font-family:Signika'>Website:       <span>$website</span></div>
                <div class=row  style='font-family:Signika'>Phone:       <span>$phone</span></div>
                <div class=row>Address:       <div style=''>$address</div></div>

            </td>
        </tr>
    </table-->
    $follow_link
    $request_link
END;
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function pro_name($op =  null)
{
    global $user,$AccessObj;
    $id = $AccessObj->uid;
    $Block['title'] = '';
    $content = '';
    if($AccessObj->user_type != 'people')return '';
    if($op )
    {
        $maenna_id = getProId($id);
    $content = <<< END
     <div style='margin-top:-3px;padding-bottom:3px;'>
        <table cellpadding=0 cellspacing=0 class='no-border' style='padding:0;margin:0;'>
            <tr>
                <td colspan=2>
                    <div style='width:100%;height:80px;background:#f8f8f8 url(/themes/maennaco/images/pro_bg1.png) no-repeat;font:bold 16px Signika; color:white;text-align:center;line-height:80px;overflow:hidden'>
                        $maenna_id
                    </div>
                </td>
            </tr>
        </table>
        
    </div>
END;
    }else return '';
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
    
}
 */

 
function company_name($op = null)
{
    global $user, $AccessObj;
    if($AccessObj->user_type != 'company') return '';
    $editorid = $user->uid;
    $companyid = $AccessObj->uid;
    $Block['title'] = "";
    $content = '';
    $redirect = rebuild_url(array('tab','page','id'));
    $section = __FUNCTION__;
    $panel = 'edit_company_info';
    if( ($op != 'update') && $op){
        
        $sql = "select * from maenna_company where companyid  = $companyid ";
        $result = db_query($sql);
        $Data = db_fetch_object($result);
        $projName = $Data->projname;
        if(empty($projName))$projName = getProjectName($companyid);
        $_title = "". htmlentities(ucwords($projName), ENT_QUOTES | ENT_IGNORE, "UTF-8") ."";
        
        
        
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
        if(empty($logo))
        {
            $src = "/themes/maennaco/images/com_bg.png";//$logo = 'com_bg.png';//'132076369_factory.jpg';
        }
        else $src = "./" . file_directory_path() . "/" . $logo;
        
        
        $edit = '';
        $follow_link = $request_link = '';
        if($AccessObj->user_type == 'people')
        {
            if(in_array($AccessObj->_relation, array('pu_analyst','pu_expert', 'pu_visible', 'pu_other')))
            {
                if(! Connections::is_following($companyid, $AccessObj->uid))
                {
                    $follow_link = "<div style='padding:0;margin:0;' class='inline-btn'><a href='$redirect&update_section=follow'>FOLLOW</a></div>";
                }else{
                    $follow_link = "FOLLOWING";
                }
                
            }
            if(in_array($AccessObj->_relation, array('pu_visible')))
            {
                if( Connections::allow_to_request($companyid, $AccessObj->uid))
                {
                    //$request_link = "<div style='padding:0;margin:0;' class='inline-btn'><a href='$redirect&update_section=request_join_an'>Request to Join Advisory Network</a></div>";
                }else{
                    $request_link = "<div>Requested</div>";
                }
            }
        }
         $Block['title'] = "$_title";
        if($op == 'write' )
        {
            $Block['title'] .= "&nbsp;<div class=editbtn>
                        <a href='/account?tab=companies&page=company_detail&id=$companyid&section=company_name&panel=$panel' class='tool' >Edit</a></div>";
        }
       
$content .= <<< END
    <!--div style='margin-top:-3px;padding-bottom:3px;'>
        <table cellpadding=0 cellspacing=0 class='no-border' style='padding:0;margin:0;'>
            <tr>
                <td colspan=2>
                    <div style='width:100%;height:80px;background:#f8f8f8 url(/themes/maennaco/images/pro_bg1.png) no-repeat;font:bold 16px Signika; color:white;text-align:center;line-height:80px;overflow:hidden'>
                        $_title
                    </div>
                </td>
            </tr>
            <tr>
                <td style='padding:5px 0; font: 12px Signika'>Founded: $founded</td>
                <td style='padding:5px 0; font: 12px Signika'>Employees: $numOfEmp</td>
            </tr>
        </table>
        
    </div-->
    <table class='no-border' style='margin-top:0;'>
        <tr>
            <td style='width:130px;'>
                <div style='position:relative;height:117px;width:117px' class='blue-border'><img src='$src' style='width:117px;height:117px'</div>
            </td>
            <td style=''>
                <div class=row >Founded: $founded</div>
                <div class=row >Employees: $numOfEmp</div>
                
                <!--div class=row >Website:       <span>$website</span></div>
                <div class=row >Phone:       <span>$phone</span></div>
                <div class=row>Address:       <div style=''>$address</div></div-->

            </td>
        </tr>
    </table>
    $follow_link
    $request_link
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
        
        $companyid = sget($_POST,'id');
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

function pro_name($op =  null)
{
    global $user,$AccessObj;
    if($AccessObj->user_type != 'people') return '';
    $id =$AccessObj->uid;
    $Block['title'] = '';
    $content = '';
    $cur_section = $cur_panel = '';
    $cur_section = sget($_REQUEST, 'section');
    $cur_panel = sget($_REQUEST, 'panel');
    $redirect = rebuild_url(array('tab'));
       
        $maenna_id = getProId($id);
        $Block['title'] = "$maenna_id";
      $Block['title'] .= "&nbsp;<div class=editbtn><a href='$redirect&panel=pro_profile_panel&view=edit' class=tool>Edit</a></div>";
        $user_type = ucwords(userType($id));
        $sql = "select yearwork,experties,protype,proj_name,gender,pic from maenna_people where pid = %d";
        $result = db_query($sql, array($id));
        $Row = db_fetch_object($result);
        if($Row && $Row->yearwork && strlen($Row->yearwork) == 4)
        {
            $yrs = date("Y") - $Row->yearwork . " years";
        }else{
            $yrs = '';
        }
        $expertise = '';
        if($Row && $Row->experties)
        {
            $temp = Options_experties($Row->experties, 1);
            if(empty($temp)) $temp = $Row->experties;
            $expertise = htmlentities(ucwords($temp), ENT_QUOTES);
        }

        $src = getProAvatar($Row, '117', '117');
		
       if ($Row->protype == 'client') { $prof_type = $Row->proj_name." Management"; }
	   else { $prof_type = $Row->protype; }
    
	$content = <<< END
    <table class='no-border' style='margin-top:0;'>
        <tr>
            <td style='width:130px;'>
                <div style='position:relative;height:117px;width:117px;overflow:hidden' class='blue-border'><img src=".$src" width='117px' height='117px'></div>
            </td>
            <td style=''>
                <div class=row >$yrs</div>
                <div class=row style='text-align:left'>$expertise</div>
                <div class=row style='text-align:left;text-transform:capitalize;'>$prof_type</div>
            </td>
        </tr>
    </table>
END;
    
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
    
}


function questionnaire_list($op = null){
    global $AccessObj;
    $editorid = $AccessObj->uid;
    $roleid = $AccessObj->role_id;
    $time = time();
    
    $Block['title'] = 'SECTIONS';
    $content = '';
    
    if($AccessObj->user_type == 'company')
    {
        $target = 'company';
    }
    else {
        $target = 'pro';
    }
    
    if($op)
    {
    
        $sql = "select  *
                from    maenna_questionair
                where   status = 1 and
                        target='%s' and
                        parentid is null
                order by recordid desc";
        $result = db_query($sql,array($target));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $parentid = $Row->recordid;
           
            if($AccessObj->user_type == 'people')
            {
                $sql =  "select     recordid,
                                    content
                        from        maenna_questionair 
                        where       parentid = %d and
                                    (usertype = %d or usertype = 0 ) and
                                    type = 'title'
                        order by weight, recordid";
                $DBValues =  array( $parentid, $AccessObj->role_id);
            }else
            {
                $sql = "select  recordid,
                                content
                        from    maenna_questionair
                        where   parentid = %d and
                                type = 'title' 
                        order by weight, recordid";
                $DBValues = array($parentid);
            }
            $result = db_query($sql, $DBValues);
            $has_section = false;
            while($Row = db_fetch_object($result))
            {
                $has_section = true;
                $title = htmlentities($Row->content, ENT_QUOTES);
                $questionid = $Row->recordid;
                $content .= "\n<div class=row><a href='#t_$questionid'>$title</a></div>";
            }
            
        }
        
    }
    if( ! $has_section) return '';
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}




/* EOF */