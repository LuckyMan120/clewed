<?php
$maenna_page['content'] = array('pro_profile',
                                'prp_experties',
                                'pro_accomplishments',
                                'pro_personality',
                                'pro_industry_view',
                                'pro_mgmt_view');

                                            
            
function pro_profile($op = null)
{
    $pid = sget($_REQUEST, 'pid');
    if(empty($pid)) return 'Invalid id';
    
    $sql = "select * from maenna_people where pid = %d limit 1";
    $result = db_query($sql, array($pid));
    $Maenna_people = db_fetch_array($result);
    $_Industry = _experties();
    $_MLVL = _managementLevel();
    $_plResponsibility = _plResponsibility();
    $_Degrees = _Degrees();
    if(empty($Maenna_people)) return '';
    
    $Block['title'] = '';//'Professional Profile';
    
    $Info = array();
    $maenna_id =  getProId(sget($Maenna_people, 'pid')) ;
    $account_type = userType($pid);
    $Info[] = $account_type;

    $yearwork = sget($Maenna_people, 'yearwork');
    if($yearwork)
    {
        $yearwork = date("Y") - $yearwork;
        if($yearwork == 1)$yearwork .= " Year";
        elseif($yearwork > 1) $yearwork .= " Years";
        $Info[] = $yearwork;
    }
    $bio = nl2br(htmlentities(sget($Maenna_people, 'brief_intro'),ENT_QUOTES |  ENT_IGNORE, "UTF-8"));
    
    
    
    $sql  = "select * from maenna_people_data where data_type = '%s' and pid = %d order by access desc limit 1";
    $result = db_query($sql, array('employment', $pid));
    $Companies = $Industries = $Positions = $PLs = array();
    while(($Row = db_fetch_object($result))){
        if($Row->data_value2){
            foreach($_Industry as $key => $array){
                foreach($array as $index=> $val){
                    if(strcasecmp($index, $Row->data_value2) == 0){
                        $Industries[] = $val;
                        $Info[] = htmlentities($val,ENT_QUOTES |  ENT_IGNORE, "UTF-8");
                        break;
                    }
                }
            }
        }
        if( $Row->data_value)
        {
            $pos = $_MLVL[$Row->data_value3];
            if($pos)$pos .= ", ";
            $pos_com = ucwords($pos . $Row->data_value);
            $Info[] = htmlentities($pos_com,ENT_QUOTES |  ENT_IGNORE, "UTF-8");
        }
        
        
        if($Row->data_value4){
            $PLs[] = $_plResponsibility[$Row->data_value4];
        }
    }
   // if(count($Companies ) > 1) $Info[] = implode(",", $Companies);
   
    
    
    $result = db_query($sql, array('education', $pid));
    $Degree = $School = array();
    while(($Row = db_fetch_object($result))){
        $school = $Row->data_value;
        $degree = $Row->data_value2;
        $deg_sch = ucwords($degree . ', '. $school);
        $Info[] = htmlentities($deg_sch,ENT_QUOTES |  ENT_IGNORE, "UTF-8");
       
    }
   
    $content .= "<div class='pro_box'>
        <div class='maenna_id'>$maenna_id</div>
        <div class='rows'>";
    foreach($Info as $info)
    {
        $content .= "<div title='$info'>$info</div>";
    }
    $content .= "</div></div>";
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function tplSummaryBox($Array = null)
{
    if(empty($Array)) return '';
    $title = sget($Array, 'title');
    $panel = sget($Array, 'panel');
    $box = sget($Array, 'style');
    $pid = sget($Array, 'pid');
    $text = sget($Array, 'text');
    if(empty($text)) $text = '-';
    $textExcerpt = contentExcerpt($text);
    $more = '';
    if($text != $textExcerpt)
    $more = "<div style='text-align:right'><a href='/account?a=professionals&type=people_detail&pid=$pid&panel=pro_addInfo_panel&key=$panel'>More</a></div>";
    return "<br><div class=row $box><h4 style='font-weight:bold;'>$title</h4><p>$textExcerpt</p> $more</div>";
}


function prp_experties($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $time = time();
    $Block['title'] = "Domain Expertise";
    
    $data_type = 'addinfo';
    $data_attr = 'experties';
    $redirect = rebuild_url(array('a','type','pid'));
    $content = '';
    $panel = 'single';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit&dtype=${data_type}&dattr=${data_attr}' class=tool>EDIT</a></div>";
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' and data_attr = '%s'";
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = contentExcerpt($content, $redirect . "&panel=${panel}&view=detail&dtype=${data_type}&dattr=${data_attr}");
        }
    }else{
        
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function pro_accomplishments($op = null)
{
        global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $time = time();
    $Block['title'] = "KEY ACCOMPLISHMENTS";
    
    $data_type = 'addinfo';
    $data_attr = 'accomplishments';
    $redirect = rebuild_url(array('a','type','pid'));
    $content = '';
    $panel = 'single';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit&dtype=${data_type}&dattr=${data_attr}' class=tool>EDIT</a></div>";
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' and data_attr = '%s'";
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = contentExcerpt($content, $redirect . "&panel=${panel}&view=detail&dtype=${data_type}&dattr=${data_attr}");
        }
    }else{
        
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function pro_personality($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $time = time();
    $Block['title'] = "Personality";
    
    $data_type = 'addinfo';
    $data_attr = 'personality';
    $redirect = rebuild_url(array('a','type','pid'));
    $content = '';
    $panel = 'single';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit&dtype=${data_type}&dattr=${data_attr}' class=tool>EDIT</a></div>";
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' and data_attr = '%s'";
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = contentExcerpt($content, $redirect . "&panel=${panel}&view=detail&dtype=${data_type}&dattr=${data_attr}");
        }
    }else{
        
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function pro_industry_view($op = null)
{
        global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $time = time();
    $Block['title'] = "Industry View";
    
    $data_type = 'addinfo';
    $data_attr = 'industryview';
    $redirect = rebuild_url(array('a','type','pid'));
    $content = '';
    $panel = 'single';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit&dtype=${data_type}&dattr=${data_attr}' class=tool>EDIT</a></div>";
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' and data_attr = '%s'";
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = contentExcerpt($content, $redirect . "&panel=${panel}&view=detail&dtype=${data_type}&dattr=${data_attr}");
        }
    }else{
        
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function pro_mgmt_view($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $time = time();
    $Block['title'] = "Management View";
    
    $data_type = 'addinfo';
    $data_attr = 'mgmtview';
    $redirect = rebuild_url(array('a','type','pid'));
    $content = '';
    $panel = 'single';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit&dtype=${data_type}&dattr=${data_attr}' class=tool>EDIT</a></div>";
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' and data_attr = '%s'";
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = contentExcerpt($content, $redirect . "&panel=${panel}&view=detail&dtype=${data_type}&dattr=${data_attr}");
        }
    }else{
        
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}


function single($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $time = time();
    
    $panel = "single";
    $data_type = 'addinfo';
    $data_attr = sget($_REQUEST, 'dattr');
    $redirect = rebuild_url(array('a','type', 'pid'));
    $dataid = '';
    $content = '';
    
    switch($data_attr)
    {
        case "experties":
            $title = "Domain Expertise";
            break;
        case "accomplishments":
            $title = "Key Accomplishments";
            break;
        case "personality":
            $title = "Personality";
            break;
        case "industryview":
            $title = "Industry View";
            break;
        case "mgmtview":
            $title = "Management View";
            break;
    }
    $Block['title'] = $title;
    
    if($op == null || $op == 'view'){
        $text = '';
        $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' and data_attr = '%s'";
       
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        $Row = db_fetch_object($result);
      
        if($Row){
            $text = $Row->data_value2;
            $dataid = $Row->dataid;
        }
        $view = sget($_REQUEST, 'view');
        if($view == 'edit'){
            $Block['title'] = "Edit - " . $Block['title'];
            $content = "<form method=post action='/account'>
                        <table class='no-border'>
                            <tr>
                                <td><textarea name='content' style='width:98%;height:400px;'>$text</textarea></td> 
                            </tr>
                            <tr>
                                <td><input type=submit name=submit value=submit class=button /> <a href='$redirect' class=button>Cancel</a></td>
                            </tr>
                        </table>
                            <input type=hidden name=dataid value='$dataid' />
                            <input type=hidden name=update_section value='$panel' />
                            <input type=hidden name=view value='detail' />
                            <input type=hidden name=dattr value='$data_attr' />
                        ";
            $content .= hidden_post_values(array('a','type','panel','pid'));
            $content .= "</form>";
        }elseif($view == 'detail'){
            $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit&dtype=${data_type}&dattr=${data_attr}' class=tool>EDIT</a></div>";
            $content = nl2br(htmlentities($text, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $content = "<div class=entry>
                            <div class=entry-content>$content</div>
                        </div>
                        <div class=backbtn><a href='$redirect' class='button'>back</a></div>";
        }
    }elseif($op == 'update'){

        $data_value2 = sget($_REQUEST, 'content');
        $pro_id = sget($_REQUEST, 'pid');
        $dataid = sget($_REQUEST, 'dataid');
    
        if(empty($pro_id)) drupal_set_message("Invalid company id",'error');
        else{
            if($dataid){
                $sql = "update maenna_people_data set data_value2 = '%s', access='$time',editorid=$editorid where data_attr = '%s' and dataid = %d limit 1";
                $DBValues = array($data_value2, $data_attr, $dataid);
            }else{
                $sql = "insert into maenna_people_data (pid, access, data_type, data_attr, data_value2,editorid ) values (%d, '%s','%s','%s','%s',%d)";
                $DBValues = array($pid, $time, $data_type,$data_attr, $data_value2, $editorid);
            }
            if(db_query($sql, $DBValues))drupal_set_message("Data record is updated");
            else drupal_set_message("Failed to update record", 'error');
        }
        return ;
    }
     $Block['body'] = content_box($Block['title'], $content);
    return $Block; 
}


/* EOF */