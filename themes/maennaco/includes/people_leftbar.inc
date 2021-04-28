<?php


function people_info($op = null)
{
    
    global $user;
    $editorid = $user->uid;
    $roleid = userRoleId($editorid);
    $roletype = strtoupper(Options_proType($roleid, 1));
    $roletype = ucwords($roletype);
    $Block['title'] = "";
    $content = '';
    if($op == 'view' || $op == null){
        $a = sget($_REQUEST, 'a');
        if(empty($a))$a = 'questionnaire';
        $sql = "select * from maenna_people where pid  = $editorid ";
        $result = db_query($sql);
        $Data = db_fetch_object($result);
        $projName = getProId($editorid);
        $Block['title'] = "". htmlentities(strtoupper($projName), ENT_QUOTES) ."";
        $a = sget($_REQUEST, 'a');
        if(empty($a))$a = 'questionnaire';
        $Block['title'] .= "<div class=editbtn><a href='/account?a=$a&panel=edit_people_info' class='tool' >Edit</a></div>";
        
        
        $yearwork = $Data->yearwork;
        if(strlen($yearwork)) $yearwork = date("Y") - $yearwork;
        else{$yearwork = '0';}
        
        $pic = $Date->pic;
        if(empty($pic)){
            //$pic = '132076369_factory.jpg';
            $src = "./" . file_directory_path() . "/maenna_pro.png";
            $img = "<img src='$src' style='width:122px;height:112px' />";
        }else{
            $src = "./" . file_directory_path() . "/" . $pic;
            $img = "<img src='$src' style='width:122px;height:112px' />";
        }
        
        $expertise = $Data->experties;
        if($expertise) $expertise = Options_experties($expertise, 1);
        $industry = $Data->industry;
        if($industry) $industry = Options_industry($industry, 1);
        
        $companies = '';
        $sql = "select * from maenna_people_data where pid = %d and data_type = 'employment'";
        $result = db_query($sql, array($editorid));
        $Temp = array();
        while($Row = db_fetch_object($result))
        {
            if($Row->data_value)
            $Temp[] = htmlentities(strtoupper($Row->data_value),ENT_QUOTES);
        }
        if(count($Temp) > 0)$companies = implode(', ', $Temp);
        
        $degree = '';
        $school = '';
        $Temp = array();
        $Temp1 = array();
        $sql = "select * from maenna_people where pid = %d and data_type = 'education'";
        $result = db_query($sql, array($editorid));
        while($Row = db_fetch_object($result))
        {
            $Temp[] = htmlentities(strtoupper($Row->data_value2),ENT_QUOTES) ;
            $Temp1[] = htmlentities(ucwords($Row->data_value),ENT_QUOTES) ;
        }
        if(count($Temp) > 0) $degree = implode(', ', $Temp);
        if(count($Temp1) > 0) $school = implode(', ', $Temp1);
    
$content .= <<< END
    <table class='no-border'>
        <tr>
            <td style='width:130px;'>
                <div style='position:relative;height:112px;width:122px'>$img</div>
            </td>
            <td style=''>
                <div class=row  style='font-family:Signika'>$roletype</div>
                <div class=row  style='font-family:Signika'>Years: $yearwork</div>
                <div class=row  style='font-family:Signika;overflow:hidden' title='$expertise'>$expertise</div>
                <div class=row  style='font-family:Signika;overflow:hidden' title='$industry'>$industry</div>
            </td>
        </tr>
        <!--tr>
            <td colspan=2 style='padding-top:7px;'>
                <div class=row  style='font-family:Signika'>$expertise</div>
                <div class=row  style='font-family:Signika'>$industry</div>
                <div class=row  style='font-family:Signika'>$companies</div>
                <div class=row  style='font-family:Signika'>$degree</div>
                <div class=row  style='font-family:Signika'>$school</div>
            </td>
        </tr-->
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
    $uid = $user->uid;
    $rid = userRoleId($uid);
    $Block['title'] = "Name";
    $content = '';
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_questionair where parentid is null and target = 'pro' and status = 1";
        $result = db_query($sql);
        $Row = db_fetch_object($result);
        if($Row)
        {
            $Block['title'] = 'SECTIONS'; //htmlentities(ucwords($Row->content), ENT_QUOTES);
            $parentid = $Row->recordid;
            
            $sql = "select * from maenna_questionair where parentid = %d and (type = 'title') and ((usertype = 0) or (usertype = $rid)) order by weight, recordid";
            $result = db_query($sql, array($parentid));
            $counter = 1;
            
            while(($Row = db_fetch_object($result)) )
            {
                $questionid = $Row->recordid;
                $title = $counter . ". <a href='#q_$questionid'>" . htmlentities(ucwords($Row->content), ENT_QUOTES) ."</a>";
                $content .= "\n<div class=row>$title</div>";
                $counter++;
            }
            
        }
       /* $content .= "<div class=row style='border:none;'>&nbsp;
                        <span><a href='/account?a=questionnaire&parentid=$parentid'>More</a>
                    </span></div>";
       */
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

/* EOF */