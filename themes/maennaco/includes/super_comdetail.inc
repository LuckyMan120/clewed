<?php
$maenna_page['content'] = array(   'maenna_company_name',
                                    'maenna_company_financials',
                                    'maenna_analysis',
                                    'advisory_advice',
                                    'relavent_info',
                                    'superadmin_comprogress'
                                );
$section_tab = sget($_REQUEST, 'tab');
if(empty($section_tab )) $section_tab = 'questionnaire';
switch($section_tab)
{
    case 'questionnaire':
        $maenna_page['content'] = array('questionnaire');
        break;
    case 'analysis':
        $maenna_page['content'] = array('maenna_analysis');
        break;
    case 'plan':
        $maenna_page['content'] = array('maenna_plan');
        break;
    case 'monitoring':
        include ("." . base_path() . path_to_theme() . "/includes/super_com_monitoring.inc");
        $maenna_page['content'] = array('mgmt_analysis', 'financial_data', 'council_advice');
        break;
    case 'minutes':
        $maenna_page['content'] = array("minutes");
        break;
    default:
        $maenna_page['content'] = array();
        break;
}
$redirect = rebuild_url(array('a','type','tab','companyid'));

mybreadcrumb(4, ucwords($section_tab), $redirect );


$panel = sget($_REQUEST, 'panel');
if(function_exists($panel) && $section_tab == 'monitoring')
{
    $redirect = rebuild_url(array('a','type','tab','companyid','panel'));
    $title = strtolower($panel('title'));
    mybreadcrumb(5, ucwords($title), $redirect );
}

function company_section_tabs($op = null)
{ 
    $tab = sget($_REQUEST, 'tab');
    if(empty($tab)) $tab = 'questionnaire';
    $companyid = sget($_REQUEST, 'companyid');
    $Tabs = array('questionnaire' => 'questionnaire',
                  'analysis'  => 'analysis',
                  'plan' => 'plan',
                  'monitoring' => 'monitoring',
                  'minutes'=> 'Minutes');
    $i = 0;
    foreach($Tabs as $key => $title){
        $active = ''; $class = '';
        if($key == $tab || ($tab == 'summary' && $key == 'analysis')) $active = ' active-trail active';
        if($i == 0)$class = 'first';
        elseif($i == (count($Tabs) - 1))$class = 'last';
        $content .= "\n<li class=' $class $active '><a href='/account?a=companies&type=company_detail&tab=$key&companyid=$companyid' class='$active'>".strtoupper($title)."</a></li>";
        $i++;
    }
    $content = <<< END
    <div class='account-section-tabs'>
        <ul>
        $content
        </ul>
    </div>
END;
    return $content;
}

function questionnaire($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'companyid');
    $Block['title'] = "Name";
    $content = '';
    
    if($op == null || $op == 'view'){
        $sql = "select * from maenna_questionair where status = 1 and target='company' and parentid is null order by recordid desc";
        $result = db_query($sql);
        $Row = db_fetch_object($result);
        if($Row)
        {
            $Block['title'] = htmlentities(ucwords($Row->content), ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $parentid = $Row->recordid;
        
            $sql = "select Q.*, A.answer from maenna_questionair as Q
                    left join maenna_qa as A on
                        Q.recordid = A.qid and
                        A.uid = %d
                    where Q.parentid = %d order by weight, recordid";
            $DBValues = array($companyid, $parentid);
            $result = db_query($sql, $DBValues);
        
            $page = sget($_REQUEST, 'page');
            if(empty($page)) $page = 1;
            $morePage = false;
            $pageCnt = 1;
            $curPage = $page;
            $preType = '';
            $counter = 1;
            while($Row = db_fetch_object($result)){
                $type = $Row->type;
        
                $questionid = $Row->recordid;
                if($type == 'pagedivider'){
                    if($preType != 'pagedivider')$pageCnt++;
                    
                }elseif($type == 'title'){
                    if($pageCnt == $page)
                    {
                        $subtitle = htmlentities($Row->content,ENT_QUOTES  | ENT_IGNORE, "UTF-8");
                        $content .= "\n<a name='t_$questionid'></a><div class='subtitle'>$subtitle</div>";
                    }
                }elseif($type == 'question')
                {
                    if($pageCnt == $page)
                    {
                        
                        $question = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                        if($Row->answer)
                        $answer = nl2br(htmlentities($Row->answer,ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                        else $answer = "N/A";
                        $content .= "\n<div class='entry'>";
                        $content .= "\n<div class='entry-title'>$counter. $question</div>";
                        $content .= "\n<div class='entry-content'><span class='blue'>Answer:</span> $answer</div>";
                        $content .= "\n</div>";
                        $counter++;
                    }
                }elseif($type == 'instruction'){
                    if($pageCnt == $page)
                    {
                        $instruction = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                        $content .= "\n<div style='margin-top:8px;font:italic 12px arial'>$instruction</div>";
                    }
                }elseif($type == 'fileuploader')
                {
                    if($pageCnt == $page)
                    {
                        $loaderTitle = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                        $filename = $Row->answer;
                        if(empty($filename)) $file = "N/A";
                        else {
                            $path =  "./" . file_directory_path() . "/" . $filename;
                            $file = "<a href='$path' target='_blank'>$filename</a>";
                        }
                        $content .= "\n<div class='entry'>";
                        $content .= "\n<div class='entry-title'>$counter. $loaderTitle</div>";
                        $content .= "\n<div class='entry-content'><span class='blue'>File:</span> $file</div>";
                        $content .= "\n</div>";
                        $counter++;
                    }
                }
                $preType = $type;
            }
            // Page navigation by divider
            $prePage = $nextPage = $submit = '';
            if($page > 1 && $pageCnt > 1){
                $pre= $page -1;
                $prePage = "<a href='/account?a=information&tab=questionnaire&panel=preview&id=$parentid&page=$pre&companyid=$companyid' class=button>previous page</a> &nbsp;&nbsp;&nbsp;";
            }
            if($pageCnt > 1 && $page < $pageCnt ){
                $next = $page + 1;
                $nextPage = "<a href='/account?a=information&tab=questionnaire&panel=preview&id=$parentid&page=$next&companyid=$companyid' class=button>next page</a>&nbsp;&nbsp;&nbsp;";
            }
            if($pageCnt == $page){
                $submit = "<input type=submit name=submit value='Submit' class=button />";
            }else{
                $submit = "<input type=submit name=submit value='save and continue' class=button />";
            }
            // Show view as userType for Pro
    
            
            $content = <<< END
        
            <form method='post' action='/account' enctype='multipart/form-data'>
                <div style='position:relative;width:100%;'>
                $content
                <input type=hidden name=a value=information />
                <input type=hidden name=tab value=questionnaire />
                <input type=hidden name=panel value='preview' />
                <input type=hidden name=id value='$parentid' />
                <input type='hidden' name='page' value='$next' />
                    <div style='margin-top:15px;'>$prePage $nextPage</div>
                </div>
            </form>
END;
        }
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function edit_company_info($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $time = time();
    
    $Block['title'] = "Edit Company Information";
    
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company where companyid = %d";
        $result = db_query($sql, array($companyid));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $company = $Row->company;
            $projName = $Row->projname;
            if(empty($projName))$projName = getProjectName($companyid);
            $address1 = $Row->address1;
            $address2 = $Row->address2;
            $city = $Row->city;
            $state = $Row->state;
            $option_state = Options_state($state);
            $zip = $Row->zip;
            $foundedYear = $Row->founded;
            $empnum = $Row->empnum;
            $description = $Row->description;
            $web = $Row->web;
            $phone = $Row->phone;
            $industry = $Row->sector;
            $option_industry = Options_industry($industry);
            
            $a = sget($_REQUEST, 'a');
            
            $content = <<< END
            <form method=post action='/account' enctype='multipart/form-data'>
                <table class='edit_table no-border'>
                    <tr><td style='width:140px;'>Company Name:</td><td><input type=text name=company value='$company' style='width:300px;' /></td></tr>
                    <tr><td>Project Name:</td><td><input type=text name=projname value='$projName' /></td></tr>
                    <tr><td>Industry:</td><td><select name=sector><option></option>$option_industry</select></td></tr>
                    <tr><td>Year Founded:</td><td><input type=text name='founded' value='$foundedYear' /></td></tr>
                    <tr><td>Number of Employees:</td><td><input type=text name=empnum value='$empnum' /></td></tr>
                    <tr><td>Introduction:</td><td><textarea name='description' style='width:400px;height:130px;'>$description</textarea></td></tr>
                    <tr><td colspan=2>&nbsp;</td></tr>
                    <tr><td>Address:</td><td><input type=text name=address1 value='$address1' /></td></tr>
                    <tr><td>Suite/Room #:</td><td><input type=text name=address2 value='$address2' /> </td></tr>
                    <tr><td>City:</td><td><input type=text name=city value='$city' /></td></tr>
                    <tr><td>State:</td><td><select name=state >$option_state</select></td></tr>
                    <tr><td>Zip Code:</td><td><input type=text name=zip value='$zip' style='width:60px' /></td></tr>
                    <tr><td colspan=2>&nbsp;</td></tr>
                    <tr><td>Phone:</td><td><input type=text name=phone value='$phone' /></td></tr>
                    <tr><td>Website:</td><td><input type=text name=web value='$web' /></td></tr>
                    <tr><td>Logo Image:</td><td><input type=file name=logo  /> <br /><span class='note'>(A small square picture, 120X120 pixels is preferred)</span></td></tr>
                    <tr><td colspan=2>&nbsp;</td></tr>
                    <tr><td></td><td><input type=submit name=submit value='Submit' class='button' /> <a href='javascript:location.href=document.referrer;' class=button>Cancel</a></td></tr>
                </table>
                <input type=hidden name=a value='$a' />
                <input type=hidden name=type value='company_detail' />
                <input type=hidden name=panel value='edit_company_info' />
                <input type=hidden name=companyid value='$companyid' />
                <input type=hidden name=update_section value='edit_company_info' />
            </form>
END;
        }
    }elseif($op == 'update')
    {
        $DBKeys = array(
                        'company',
                        'address1',
                        'address2',
                        'city',
                        'state',
                        'zip',
                        'phone',
                        'web',
                        'founded',
                        'empnum',
                        'description',
                        'sector',
                        'projname',
                        'logo'
                        );
        $DBValues = $SQL_STR = array();
        foreach($DBKeys as $key)
        {
            $DBValues["$key"] = sget($_POST, $key);
            $SQL_STR[] = "$key = '%s'";
        }
        
        if(isset($_FILES['logo']['name']) && ($_FILES['logo']['error'] == 0))
        {
            $filename = $_FILES['logo']['name'];
            $filename = $time . '_' . clearString($filename);
            $path = "./" . file_directory_path() . "/" . $filename;
            if(move_uploaded_file($_FILES['logo']['tmp_name'], $path))
            {
                $DBValues['logo'] = $filename;
            }else{
                drupal_set_message('Failed to upload logo', 'error');
                return;
            }
        }
        
        $DBValues[] = $companyid;
        
        $sql = "update maenna_company set ".implode(',', $SQL_STR)."where companyid = %d limit 1";
        if(db_query($sql, $DBValues)){
            drupal_set_message("Company information is updated");
        }else{
            drupal_set_message("Failed to update company information", 'error');
        }
        
        return '';
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}




function maenna_analysis($op=null){
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $time = time();
    
    $Block['title'] = "Analysis";
    $content = '';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    $data_type = 'analysis';
    $panel  = 'maenna_analysis_panel';
    if($op == 'view' || $op == null)
    {
        $limit = 5;
        $page = sget($_REQUEST,'page');
        if(empty($page)) $page = 1;
        $start = ($page - 1) * $limit;
        
        $sql = "select count(*) as total from maenna_company_data where data_type = '%s' and companyid = %d";
        $result = db_query($sql, array($data_type, $companyid));
        $Row = db_fetch_object($result);
        $total = $Row->total;
        
        $sql = "select * from maenna_company_data where data_type = '%s' and companyid = %d order by access desc";
        $result = db_query($sql, array($data_type, $companyid));
        while(($Row = db_fetch_object($result)) !== false)
        {
            $access = date('m/d/Y',$Row->access);
            $title = strtoupper($Row->data_value);
            if($title)
            $title = htmlentities($title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            else $title = "&nbsp;";
            $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
            $text = contentExcerpt($text, $more_link);
            $content .= "<div class=entry>";
            $content .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid' class='tool' >EDIT</a></div>";
            $content .= "<div class='entry-title'>$title</div><div class='entry-content'>" . $text ."</div>";
            //$content .= "<div class='posted_on'>Posted on $access</div>";
            $content .= "</div>";
        }
        $Pagenation = array('total' => $total,
            'limit' =>5,
            'baseurl' => $redirect,
            'num_of_links' => 8,
        );
        $content .= pagination($Pagenation);
    }
    
    $Block['title'] .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=add' class='tool' >ADD</a></div>";
  
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function maenna_analysis_panel($op=null){
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = 'Analysis';
    $content = '';
    $time = time();
    $panel = 'maenna_analysis_panel';
    $data_type = 'analysis';
    $tab = sget($_REQUEST, 'tab');
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    if($op == null || $op == 'view')
    {
        $view = sget($_REQUEST, 'view');
        if($view == 'detail')
        {
            $dataid = sget($_REQUEST, 'dataid');
            $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and dataid = %d";
            $result = db_query($sql, array($companyid, $data_type, $dataid));
            $Row = db_fetch_object($result);
            if($Row)
            {
                $title = htmlentities(strtoupper($Row->data_value),ENT_QUOTES | ENT_IGNORE, "UTF-8");
                $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                $dataid = $Row->dataid;
                
                $access = date('m/d/Y', $Row->access);
                $content .= "<div class=entry>";
                $content .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid' class='tool' >edit</a></div>";
                $content .= "<div class='entry-title'>$title</div><div class='entry-content'>" . _filter_autop(html_entity_decode($text)) ."</div></div>";
                $content .= "<div class='backbtn'><a href='$redirect' class=button>Back</a>";
            }
            $redirect = rebuild_url(array('a','type','tab', 'companyid'));
            $Block['title'] .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=add' class='tool' >Add</a></div>";
        }elseif($view == 'add' || $view == 'edit')
        {
            $text = '';$month=''; $dataid = sget($_REQUEST, 'dataid');
            if($dataid){
                $sql = "select * from maenna_company_data where dataid = %d and data_type = '%s' limit 1";
                $result = db_query($sql, array($dataid, $data_type)); $Row = db_fetch_object($result);
                if($Row) {
                    $title = $Row->data_value;
                    $text = $Row->data_value2;
                    $access = date('m/d/Y', $Row->access);
                }
            }
            $content .= <<< END
            <form method='post' action='/account' onsubmit='return check_input();'>
                <table class='edit_table no-border'>
                    <tr><td>Title:</td><td><input type=text name=title style='width:500px;' value='$title' /></td></tr>
                    <tr><td>Content:</td><td><textarea name='text' style='width:500px;height:240px;'>$text</textarea></td></tr>
                    <tr><td></td><td><input type=submit value='submit' class=button > <a href='$redirect' class='button'>Cancel</a> </td></tr>
                </table>
                <input type=hidden name=a  value=companies >
                <input type=hidden name=type  value=company_detail >
                <input type=hidden name=tab value=$tab />
                <input type=hidden name=update_section  value='${panel}' >
                <input type=hidden name=companyid  value='$companyid'>
                <input type=hidden name=dataid  value='$dataid'>
            </form>
END;
        }
    }elseif($op == 'update')
    {
        $dataid = sget($_REQUEST, 'dataid');
        $title = sget($_REQUEST, 'title');
        $text = sget($_REQUEST, 'text');
        $do = sget($_REQUEST, 'do');
       
        
        if(empty($do))
        {
            if($text)
            {
                if($dataid)
                {
                    $sql = "update maenna_company_data set data_value = '%s', data_value2 = '%s',access='%s', editorid=%d where dataid = %d and data_type = '%s' limit 1";
                    $DBValues = array($title, $text, $time, $editorid, $dataid, $data_type);
                }else{
                    $sql = "insert into maenna_company_data (companyid, access, data_type,data_value, data_value2, editorid) values(%d,'%s','%s','%s','%s',%d)";
                    $DBValues = array($companyid, $time, $data_type, $title, $text, $editorid);
                }
                if(db_query($sql, $DBValues)) drupal_set_message("the record is updated.");
                else drupal_set_message("failed to save changes.", 'error');
            }
        }elseif($do == 'delete')
        {
            if($dataid)
            {
                $sql = "delete from maenna_company_data where data_type = '%s' and dataid = %d limit 1";
                if(db_query($sql, array($data_t, $dataid))) drupal_set_message("the record is removed.");
                else drupal_set_message("failed to remove record.", 'error');
            }
        }
        
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}


function maenna_plan($op=null){
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $time = time();
    
    $Block['title'] = "Plan";
    $content = '';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    $data_type = 'plan';
    $panel  = 'maenna_plan_panel';
    if($op == 'view' || $op == null)
    {
        $limit = 5;
        $page = sget($_REQUEST,'page');
        if(empty($page)) $page = 1;
        $start = ($page - 1) * $limit;
        
        $sql = "select count(*) as total from maenna_company_data where data_type = '%s' and companyid = %d";
        $result = db_query($sql, array($data_type, $companyid));
        $Row = db_fetch_object($result);
        $total = $Row->total;
        
        $sql = "select * from maenna_company_data where data_type = '%s' and companyid = %d order by access desc";
        $result = db_query($sql, array($data_type, $companyid));
        while(($Row = db_fetch_object($result)) !== false)
        {
            $access = date('m/d/Y',$Row->access);
            $title = strtoupper($Row->data_value);
            if($title)
            $title = htmlentities($title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            else $title = "&nbsp;";
            $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
            $text = contentExcerpt($text, $more_link);
            $content .= "<div class=entry>";
            $content .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid' class='tool' >edit</a></div>";
            $content .= "<div class='entry-title'>$title</div><div class='entry-content'>" . $text ."</div></div><hr class='line' />";
        }
        $Pagenation = array('total' => $total,
            'limit' =>5,
            'baseurl' => $redirect,
            'num_of_links' => 8,
        );
        $content .= pagination($Pagenation);
    }
    
    $Block['title'] .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=add' class='tool' >Add</a></div>";
    /*$content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
    "<a href='/account?a=companies&type=company_detail&panel=maenna_analysis_panel&view=add&companyid=$companyid'>add</a>" . 
                    "&nbsp; <a href='/account?a=companies&type=company_detail&panel=maenna_analysis_panel&companyid=$companyid'>more</a></div>";
   
    */
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function maenna_plan_panel($op=null){
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = 'Plan';
    $content = '';
    $time = time();
    $panel = 'maenna_plan_panel';
    $data_type = 'plan';
    $tab = sget($_REQUEST, 'tab');
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    if($op == null || $op == 'view')
    {
        $view = sget($_REQUEST, 'view');
        if($view == 'detail')
        {
            $dataid = sget($_REQUEST, 'dataid');
            $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and dataid = %d";
            $result = db_query($sql, array($companyid, $data_type, $dataid));
            $Row = db_fetch_object($result);
            if($Row)
            {
                $title = htmlentities($Row->data_value,ENT_QUOTES | ENT_IGNORE, "UTF-8");
                $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                $dataid = $Row->dataid;
                
                $access = date('m/d/Y', $Row->access);
                $content .= "<div class=entry>";
                $content .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid' class='tool' >edit</a></div>";
                $content .= "<div class='entry-title'>$title</div><div class='entry-content'>" . $text ."</div></div>";
                $content .= "<div class='backbtn'><a href='$redirect' class=button>Back</a>";
            }
            $redirect = rebuild_url(array('a','type','tab', 'companyid'));
            $Block['title'] .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=add' class='tool' >Add</a></div>";
        }elseif($view == 'add' || $view == 'edit')
        {
            $text = '';$month=''; $dataid = sget($_REQUEST, 'dataid');
            if($dataid){
                $sql = "select * from maenna_company_data where dataid = %d and data_type = '%s' limit 1";
                $result = db_query($sql, array($dataid, $data_type)); $Row = db_fetch_object($result);
                if($Row) {
                    $title = $Row->data_value;
                    $text = $Row->data_value2;
                    $access = date('m/d/Y', $Row->access);
                }
            }
            $content .= <<< END
            <form method='post' action='/account' onsubmit='return check_input();'>
                <table class='edit_table no-border'>
                    <tr><td>Title:</td><td><input type=text name=title style='width:500px;' value='$title' /></td></tr>
                    <tr><td>Content:</td><td><textarea name='text' style='width:500px;height:240px;'>$text</textarea></td></tr>
                    <tr><td></td><td><input type=submit value='submit' class=button > <a href='$redirect' class='button'>Cancel</a> </td></tr>
                </table>
                <input type=hidden name=a  value=companies >
                <input type=hidden name=type  value=company_detail >
                <input type=hidden name=tab value=$tab />
                <input type=hidden name=update_section  value='${panel}' >
                <input type=hidden name=companyid  value='$companyid'>
                <input type=hidden name=dataid  value='$dataid'>
            </form>
END;
        }
    }elseif($op == 'update')
    {
        $dataid = sget($_REQUEST, 'dataid');
        $title = sget($_REQUEST, 'title');
        $text = sget($_REQUEST, 'text');
        $do = sget($_REQUEST, 'do');
       
        
        if(empty($do))
        {
            if($text)
            {
                if($dataid)
                {
                    $sql = "update maenna_company_data set data_value = '%s', data_value2 = '%s',access='%s', editorid=%d where dataid = %d and data_type = '%s' limit 1";
                    $DBValues = array($title, $text, $time, $editorid, $dataid, $data_type);
                }else{
                    $sql = "insert into maenna_company_data (companyid, access, data_type,data_value, data_value2, editorid) values(%d,'%s','%s','%s','%s',%d)";
                    $DBValues = array($companyid, $time, $data_type, $title, $text, $editorid);
                }
                if(db_query($sql, $DBValues)) drupal_set_message("the record is updated.");
                else drupal_set_message("failed to save changes.", 'error');
            }
        }elseif($do == 'delete')
        {
            if($dataid)
            {
                $sql = "delete from maenna_company_data where data_type = '%s' and dataid = %d limit 1";
                if(db_query($sql, array($data_t, $dataid))) drupal_set_message("the record is removed.");
                else drupal_set_message("failed to remove record.", 'error');
            }
        }
        
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
function superadmin_comprogress($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = 'Progress (super admin only) ';
    $content = '';
    $time = time();
    
    $content = '';
    
    if($op == null || $op == 'view')
    {
        if($companyid){
            $sql = "select * from maenna_company_data where data_type = 'progress' and companyid = %d order by access desc limit 3";
            $result = db_query($sql, array($companyid));
            while(($Row = db_fetch_object($result)) !== false)
            {
                $date = date('m/d/Y', $Row->access);
                $title = htmlentities($Row->data_value, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                $text = htmlentities(contentExcerpt($Row->data_value2), ENT_QUOTES | ENT_IGNORE, "UTF-8");
                $followup = $Row->data_attr;
                if($followup) $followup = "(follwup: " . date('m/d/Y', $followup) . ")";
                $access = date('m/d/Y', $Row->access);
                $content .= "\n<div class=row><b>$access - $title $followup</b><p>$text</p></div>";
            }
        }
    }
    $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
    "<a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&view=add&companyid=$companyid'>add</a>&nbsp;".
    "<a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&companyid=$companyid'>more</a></div>";
       
    
    $Block['body'] = content_box($Block['title'], $content);
    
    return $Block;
}

function superadmin_comprogress_panel($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = 'Progress (super admin only)';
    $content = '';
    $time = time();
    
    if($op == null || $op == 'view')
    {
        $view = sget($_REQUEST, 'view');
        if(empty($view))
        {
            $sql = "select * from maenna_company_data where companyid = %d and data_type = 'progress' order by access desc";
            $result = db_query($sql, array($companyid));
            while(($Row = db_fetch_object($result)) !== false)
            {
                $subject = htmlentities($Row->data_value,ENT_QUOTES | ENT_IGNORE, "UTF-8");
                $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                $dataid = $Row->dataid;
                $followup = $Row->data_attr;
                if($followup) $followup = "(follwup: " . date('m/d/Y', $followup) . ")";
                $access = date('m/d/Y', $Row->access);
            
                $content .= "\n<div style='position:relative;border-bottom:solid 1px #ebebeb;'>" .
                                "<div style='position:absolute;height:20px;width:50px;top:8px;right:10px'>".
                                "\n<a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&view=edit&dataid=$dataid&companyid=$companyid' class=edit-icon>edit</a>".
                                "\n<a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&do=delete&update_section=superadmin_comprogress_panel&dataid=$dataid&companyid=$companyid' class='delete-icon' onclick='return confirm(\"Continue to remove the record\")'>delete</a>".
                                "</div>" .
                                "\n<label><b>$access - $subject $followup </b></labe><p>$text</p></div>";
            }
        }elseif($view == 'add' || $view == 'edit')
        {
            $subject = $detail = $followup = '';
            $dataid = sget($_REQUEST, 'dataid');
            if($dataid){
                $sql = "select * from maenna_company_data where dataid = %d and data_type = 'progress' limit 1";
                $result = db_query($sql, array($dataid)); $Row = db_fetch_object($result);
                if($Row) {
                    $detail = $Row->data_value2;
                    $subject = $Row->data_value;
                    $followup = date('m/d/Y', $Row->access);
                }
            }
            $content .= <<< END
            <form method='post' action='/account' onsubmit='return check_input();'>
                <table class='edit_table'>
                    <tr><td>Subject</td><td><input type=text name=subject value='$subject' style='width:500px;'> </td></tr>
                    <tr><td>Detail</td><td><textarea name='detail' style='width:500px;height:240px;'>$detail</textarea></td></tr>
                    <tr><td>Followup</td><td><input type='text' name=followup value='$followup' class='datepicker' /></td></tr>
                    <tr><td></td><td><input type=submit value='submit' class=button ></td></tr>
                </table>
                <input type=hidden name=a  value=companies >
                <input type=hidden name=type  value=company_detail >
                <input type=hidden name=panel  value=superadmin_comprogress_panel >
                <input type=hidden name=update_section  value='superadmin_comprogress_panel' >
                <input type=hidden name=companyid  value='$companyid'>
                <input type=hidden name=dataid  value='$dataid'>
            </form>
            <script type='text/javascript'>
            $(document).ready(function(){
                init_datepicker();
            })
            </script>
END;
        }
    }elseif($op == 'update')
    {
        $dataid = sget($_REQUEST, 'dataid');
        $subject = sget($_REQUEST, 'subject');
        $detail = sget($_REQUEST, 'detail');
        $followup = sget($_REQUEST, 'followup');
        if($followup) $followup = strtotime($followup);
        $do = sget($_REQUEST, 'do');
        
        if(empty($do))
        {
            if($subject)
            {
                if($dataid)
                {
                    $sql = "update maenna_company_data set data_value = '%s',data_value2 = '%s',access='%s',data_attr='%s', editorid=%d where dataid = %d and data_type = 'progress' limit 1";
                    $DBValues = array($subject, $detail, $time, $followup, $editorid, $dataid);
                }else{
                    $sql = "insert into maenna_company_data (companyid, access, data_type,data_attr, data_value,data_value2, editorid) values(%d,'%s','%s','%s','%s','%s',%d)";
                    $DBValues = array($companyid, $time, 'progress',$followup,$subject,$detail, $editorid);
                }
                if(db_query($sql, $DBValues)) drupal_set_message("the record is updated.");
                else drupal_set_message("failed to save changes.", 'error');
            }
        }elseif($do == 'delete')
        {
            if($dataid)
            {
                $sql = "delete from maenna_company_data where data_type = 'progress' and dataid = %d limit 1";
                if(db_query($sql, array($dataid))) drupal_set_message("the record is removed.");
                else drupal_set_message("failed to remove record.", 'error');
            }
        }
        
    }
    $content .= "<div class='align-right edit-link'><a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&view=add&companyid=$companyid'>add</a></div>";
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function minutes($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $time = time();
    
    $Block['title'] = "Minutes";
    $content = '';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    $data_type = 'minutes';
    $panel  = 'multi';
    if($op == 'view' || $op == null)
    {
        $limit = 5;
        $page = sget($_REQUEST,'page');
        if(empty($page)) $page = 1;
        $start = ($page - 1) * $limit;
        
        $sql = "select count(*) as total from maenna_company_data where data_type = '%s' and companyid = %d";
        $result = db_query($sql, array($data_type, $companyid));
        $Row = db_fetch_object($result);
        $total = $Row->total;
        
        $sql = "select * from maenna_company_data where data_type = '%s' and companyid = %d order by access desc";
        $result = db_query($sql, array($data_type, $companyid));
        while(($Row = db_fetch_object($result)) !== false)
        {
            $access = date('m/d/Y',$Row->access);
            $title = htmlentities(strtoupper($Row->data_value), ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
            $text = contentExcerpt($text, $more_link);
            $content .= "<div class=entry>";
            $content .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid' class='tool' >edit</a></div>";
            $content .= "<div class='entry-title'>$title</div><div class='entry-content'>" . $text ."</div></div><hr class='line' />";
        }
        $Pagenation = array('total' => $total,
            'limit' =>5,
            'baseurl' => $redirect,
            'num_of_links' => 8,
        );
        $content .= pagination($Pagenation);
    }
    
    $Block['title'] .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=add&datatype=${data_type}' class='tool' >Add</a></div>";
    /*$content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
    "<a href='/account?a=companies&type=company_detail&panel=maenna_analysis_panel&view=add&companyid=$companyid'>add</a>" . 
                    "&nbsp; <a href='/account?a=companies&type=company_detail&panel=maenna_analysis_panel&companyid=$companyid'>more</a></div>";
   
    */
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

/*EOF */