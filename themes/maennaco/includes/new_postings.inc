<?php
// 'council_list', 'project_list'
$maenna_page['content'] = array('infoTabs', 'search_bar','posting_list');

global $redirect;
$redirect = rebuild_url(array('tab', 'page'));

function search_bar($op = null)
{
    global $user, $redirect;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();
    
    if($op && $op != 'update')
    {
        $proType = sget($_REQUEST, 'protype');
        $industry = sget($_REQUEST, 'industry');
        $expertise = sget($_REQUEST, 'expertise');
        $option_proType =   Options_proType($proType);
        $option_industry =  Options_industry($industry);
        $option_expertise = Options_experties($expertise);
    
        $postingType = sget($_REQUEST, 'postingtype');
    
        $content =<<< END
    <div>
        <form action='/account' method='post'>
        <table style='width:750px;' class='no-border'>
            <tr><td style='vertical-align:middle'><b>Search: </b></td>
                <td><select name=protype style='width:150px;'><option value=''>Professional Type</option>
                    <option></option>$option_proType</optgroup></select></td>
                <td><select name=expertise  style='width:200px;'><option value=''>Expertise</option>
                    <option></option>$option_expertise</select></td>
                <td><select name=industry style='width:250px;'><option value='' >Industry</option>
                    <option></option>$option_industry</select></td>
                <td><input type=submit name=submit value='Filter' class=button /></td>
            </tr>
        </table>
            <input type='hidden' name='tab' value='information' />
            <input type='hidden' name='page' value='postings' />
            <input type='hidden' name='panel' value='posting_list' />
            <input type='hidden' name='postingtype' value='$postingType' />
        </form>
    </div>
END;
     $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}

function posting_list($op = null){
    global $user, $redirect;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();
    
    if($op && $op != 'update')
    {
        // use panel to determin panel type
        $postingType = sget($_REQUEST, 'postingtype');
        
        
        $SearchKeys = array('protype', 'expertise', 'industry','postingtype');
        $SQL_WHERE = array();
        $ORDER_BY = array();
        $DBValues = array();
        // panel default view
        foreach($SearchKeys as $key)
        {
            if(sget($_REQUEST,$key))
            {
                $SQL_WHERE["$key"] = "$key = '%s'";
                $DBValues["$key"] = sget($_REQUEST, $key);
            }
        }
        $limit = 5;
        
        $Table = array('title' => 'Advisory Council Openings',
                       'class' => 'report',
                       'thead'=> array(),
                       'tbody'=> array());
        $Table['thead'] = array(
                            array('label'=> 'project name', 'style' => 'width:80px;'),
                            array('label'=> 'professional type', 'style' => 'width:100px'),
                            array('label'=> 'expertise', 'style' => 'width:120px'),
                            array('label'=> 'deadline', 'style' => 'width:120px'),
                           /* array('label'=> 'industry', 'style' => 'width:100px'),
                            
                            array('label'=> 'Est. Hr', 'style' => 'width:40px'),
                            array('label'=> 'message', 'style' => ''),*/
                            array('label'=> 'details', 'style' => 'width:40px;'),
                            array('label'=> 'apply', 'style'=>'width:50px')
                            );
        
            
        if(empty($postingType))
        {
            if(! isset($SQL_WHERE['postingtype']))$SQL_WHERE['postingtype'] = "postingtype = '%s'";
            
            $DBValues['postingtype'] = 'council';
            $sql = "select maenna_postings.*, maenna_postings_data.data_value1 as applystatus from maenna_postings left join
                            maenna_postings_data on ( maenna_postings.postingid = maenna_postings_data.postingid and
                                                        maenna_postings_data.data_type = 'member')
                            where ". implode(' and ',$SQL_WHERE) . " order by created desc limit $limit";
            $result = db_query($sql,$DBValues);
            $Table['tbody'] = formTbody($result);
            $content .= render_table($Table);
            $content .= text_link(array(array('label'=>'more &raquo;', 'url'=>"/account?a=information&tab=postings&postingtype=council")),'right');
            
            $DBValues['postingtype'] = 'project';
            $Table['title'] = 'Special Projects Openings';
            $sql = "select maenna_postings.*, maenna_postings_data.data_value1 as applystatus from maenna_postings left join
                            maenna_postings_data on ( maenna_postings.postingid = maenna_postings_data.postingid and
                                                        maenna_postings_data.data_type = 'member')
                            where ". implode(' and ',$SQL_WHERE) . " order by created desc limit $limit";
            $result = db_query($sql,$DBValues);
            $Table['tbody'] = formTbody($result);
            $content .= "<br>". render_table($Table);
            $content .= text_link(array(array('label'=>'more &raquo;', 'url'=>"/account?a=information&tab=postings&postingtype=project")),'right');
        }elseif( in_array($postingType, array('council','project')))
        {
            $page = 1;
            $limit = 15;
            if(sget($_REQUEST, 'page')){
                $start = ($page - 1) * $limit;
            }
            if($postingType == 'council'){
                $Table['title'] = 'Advisory Council Openings';
            }else{
                $Table['title'] = 'Special Projects Openings';
            }
            
            $sql = "select maenna_postings.*, maenna_postings_data.data_value1 as applystatus from maenna_postings left join
                            maenna_postings_data on ( maenna_postings.postingid = maenna_postings_data.postingid and
                                                        maenna_postings_data.data_type = 'member')
                            where ". implode(' and ',$SQL_WHERE) . " order by created desc limit $limit";
            
            $result = db_query($sql, $DBValues);
            $Table['tbody'] = formTbody($result);
            $content .= "<br>". render_table($Table);
        }
        $content .= js_init("init_ajaxTrigger();");
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;

}

function formTbody($result){
            $Tbody = array();
            while($Row = db_fetch_object($result)){
                $postingid = $Row->postingid;
                $applystatus = $Row->applystatus;
                if($applystatus == 'apply'){
                    $Applycode = array('label'=>'cancel',
                                        'url' => '#',
                                        'extra' => "table='maenna_postings_data' postingid='$postingid' dataid='$dataid'",
                                        'class'=>'ajaxTrigger button_off');
                }elseif($applystatus == 'cancel'){
                    $Applycode = array('label'=>'apply',
                                        'url' => '#',
                                        'extra' => "table='maenna_postings_data' postingid='$postingid' dataid='$dataid'",
                                        'class'=>'ajaxTrigger');
                }else{
                    $Applycode = array('');
                }
                $Tbody[] = array(
                                          'project name' => getProjectName($Row->companyid),
                                          'professional type' => Options_proType($Row->protype, 1),
                                          'expertise' => Options_experties($Row->expertise, 1),
                                          'industry' => Options_industry($Row->industry, 1),
                                          'Est. Hr' => '',
                                      /*'start/end date' => date('m/d/Y', $Row->startdate) . ' - ' .  date('m/d/Y', $Row->enddate),*/
                                          'deadline' => date('m/d/Y', $Row->enddate),
                                          'message' => htmlentities(excerpt($Row->message, 85), ENT_QUOTES),
                                          'details' => "<a href='$redirect&panel=posting_detail_panel&postingid=$postingid'>details</a>",
                                          'apply'=> jsbutton_link(
                                                                array('links'=> array
                                                                    (
                                                                        $Applycode
                                                                    )
                                                                )
                                                          )
                                          );
            }
    return $Tbody;
}



function posting_detail_panel($op = null)
{
    global $user, $redirect;
    $editorid = $user->uid;
    $Block['title'] = '';
    $content = '';
    $time = time();
    
    $postingid = sget($_REQUEST, 'postingid');
    
    if($op && $op != 'update')
    {
        $sql = "select maenna_postings.*,
                    maenna_postings_data.dataid,
                    maenna_postings_data.data_attr as applyuid,
                    maenna_postings_data.data_value1 as applystatus
                from maenna_postings left join
                    maenna_postings_data on
                maenna_postings.postingid = maenna_postings_data.postingid and
                maenna_postings_data.data_type = 'member' 
                where
                    maenna_postings.postingid = %d";
        $result = db_query($sql, array($postingid));
        $counter = 1;
        $Pending = $Approved = $Rejected = array();
        if($Row =  db_fetch_array($result))
        {

            extract($Row);
            if($counter == 1)
            {
                $maennaid = getProjectName($companyid);
                $message = nl2br($message);
                $proType = Options_proType($protype, 1);
                $expertise = Options_experties($expertise, 1);
                $industry = Options_industry($industry, 1);
                $startdate = date('m/d/Y', $startdate);
                $enddate = date('m/d/Y', $enddate);
                $dataid = sget($Row, 'dataid');
                if($applystatus == 'apply'){
                    $Applycode = array('label'=>'cancel',
                                        'url' => '#',
                                        'extra' => "table='maenna_postings_data' postingid='$postingid' dataid='$dataid'",
                                        'class'=>'ajaxTrigger button_off');
                }elseif($applystatus == 'cancel'){
                    $Applycode = array('label'=>'apply',
                                        'url' => '#',
                                        'extra' => "table='maenna_postings_data' postingid='$postingid' dataid='$dataid'",
                                        'class'=>'ajaxTrigger');
                }else{
                    $Applycode = array('');
                }
            }
            $counter = 2;
            if($applystatus == 'apply'){
                $Pending[] = $applyuid;
            }elseif($applystatus == 'approved'){
                $Approved[] = $applyuid;
            }elseif($applystatus == 'rejected'){
                $Rejected[] = $applyuid;
            }
        
        }
        
        
        
            $html_pending = implode(',', $Pending);
            $html_approved = implode(',', $Approved);
            $html_rejected = implode(',', $Rejected);
            $content = <<< END
        <table class='report_detail'>
            <tr><td style='width:130px;'>Project Name</td><td>$maennaid</td></tr>
            <tr><td>Message:</td><td>$message</td></tr>
            <tr><td>Professional Type</td><td>$proType</td></tr>
            <tr><td>Expertise</td><td>$expertise</td></tr>
            <tr><td>Industry</td><td>$industry</td></tr>
            <tr><td>Start Date</td><td>$startdate</td></tr>
            <tr><td>Est. Hrs</td><td>$esthr</td></tr>
            <tr><td>End Date</td><td>$enddate</td></tr>
            <tr><td>Approved Users</td><td>$html_approved</td></tr>
            <tr><td>Pending Users</td><td>$html_pending</td></tr>
            <tr><td>Rejected Users</td><td>$html_rejected</td></tr>
        </table>
END;


            $content .= jsbutton_link(
                                        array('links'=> array
                                            (
                                                array('label'=>'back', 'url'=>'javascript:history.go(-1)'),
                                                //$Applycode    
                                            ),
                                            'position'=>'center'
                                        )
                                  );
        $content .= js_init("init_ajaxTrigger();");
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}


        
?>
