<?php
$maenna_page['right'] = array('pro_rating',
                              'pro_bio',
                              'pro_progress',
                              'pro_hourStats',
                              'pro_selectWork',
                              );

function pro_rating($op = null){
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = 'Maenna Rating';
    $redirect = rebuild_url(array('a','type','pid'));
    $panel = 'pro_rating_panel';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>Edit</a></div>";
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
    {
        
        $sql = "select * from maenna_people where pid = %d limit 1";
        $result = db_query($sql, array($pid));
        $Row = db_fetch_object($result);
        $rating = 'n/a';
        if($Row)
        {
            if( ! empty($Row->rating)) $rating = $Row->rating;
        }
        $content .= "<div style='position:relative;font:bold 16px arial;text-align:center;'>$rating";
        $content .= "<div class=editbtn style='top:4px;text-align:left;'><a href='$redirect&panel=${panel}' class=tool>View</a></div></div></div>";
    
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}



function pro_rating_panel($op = null){
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $dataid = sget($_REQUEST, 'dataid');
    $Block['title'] = 'Maenna Rating';
    $redirect = rebuild_url(array('a','type','pid'));
    $panel = 'pro_rating_panel';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>Edit</a></div>";
    $content = '';
    $time = time();
    
    if($op == null || $op == 'view'){
        $view = sget($_REQUEST, 'view');
        if($view == '' || $view == 'view'){
            $sql = "select * from maenna_people_data where pid = %d and data_type = 'rating' limit 1";
            $result = db_query($sql, array($pid));
            $Row = db_fetch_object($result);
            if(empty($Row) || (! is_object($Row))){
                $Data = array();
            }else{
                if($Row->data_value2) $Data = unserialize($Row->data_value2);
            }
            
            $content .= ProRatingTable($Data, 'view');
           
        }elseif($view == 'edit'){
            $sql = "select * from maenna_people_data where pid = %d and data_type = 'rating' limit 1";
            $result = db_query($sql, array($pid));
            $Row = db_fetch_object($result);
            if(empty($Row) || (! is_object($Row))){
                $Data = array();

            }else{
                if($Row->data_value2)$Data = unserialize($Row->data_value2);
                $dataid = $Row->dataid;
            }
    
            $content .= ProRatingTable($Data, 'edit', $dataid);
        }
    }elseif($op == 'update'){
        if(empty($pid)){
            drupal_set_message('invalid pid value', 'error');return;
        }
        $_RatingAnchors = _proRatingAnchors();
        $Data = array();
        $sum = 0;
        $numOfKeys = 0;
        foreach($_RatingAnchors as $Items){
            foreach($Items as $shortKey => $title){
                $Data["$shortKey"] = sget($_REQUEST, $shortKey);
                if($Data["$shortKey"]) {
                    $sum += intval($Data["$shortKey"]);
                    $numOfKeys++;
                }
            }
        }
        if(count($Data) < 1) {
            drupal_set_message('Please choose ratings and try again', 'error');
        }
        $data_value2 = serialize($Data);
        if($numOfKeys) $averageRating = number_format(($sum / $numOfKeys),2);
        if(empty($dataid)){
            $sql = "insert into maenna_people_data (pid,
                                                    access,
                                                    data_type,
                                                    data_attr,
                                                    data_value2,
                                                    editorid) values (
                                                    %d,
                                                    '%s',
                                                    '%s',
                                                    '%s',
                                                    '%s',
                                                    %d)";
            $DBValues = array($pid, $time, 'rating', $averageRating, $data_value2, $editorid );
            if(db_query($sql, $DBValues) === false) {
                drupal_set_message('failed to save rating record', 'error');return;
            }
        }else{
            $sql = "update maenna_people_data set data_attr = '%s',data_value2 = '%s', access = '%s', editorid = %d where pid = %d and data_type = 'rating' and dataid = %d ";
            $DBValues = array($averageRating, $data_value2, $time, $editorid, $pid, $dataid);
            if(db_query($sql, $DBValues) === false) {
                drupal_set_message('failed to save rating record', 'error');return;
            }
            
           
        }
        $sql = "update maenna_people set rating = $averageRating where pid = %d limit 1";
        if(db_query($sql,array($pid)) === false){
                drupal_set_message('failed to update main rating record', 'error');return;
        }
        drupal_set_message('rating record is updated');return;
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function ProRatingTable($Data, $mode,$dataid = ''){
    $_RatingAnchors = _proRatingAnchors();
    $strOutput =  '';
    $pid = sget($_REQUEST, 'pid');
    $strOutput = "<form action='/account' method='post'><table style='width:100%' border=0><tr>
                            <td width=200>Rating Anchors</td>
                            <td width=100>Strong Evidence Candidate Lacks Competency</td>
                            <td width=100>Some Evidence Candidate Lacks Competency</td>
                            <td width=100></td>
                            <td width=100>Some Evidence Candidate Has Competency</td>
                            <td width=100>Strong Evidence Candidate Has Competency</td>
                            </tr>";
    $odd = '#f3f3f3';
    if($mode == 'view'){
        foreach($_RatingAnchors as $key => $Items){
            $strOutput .= "\n<tr><td colspan=6><b>$key<b></td></tr>";
            foreach($Items as $shortKey => $title){
                $strOutput .= "\n<tr><td>$title</td>";
                $value = sget($Data, $shortKey);
                for($i = 1; $i <=5; $i++){
                    if($i == $value)$strOutput .= "<td style='background:lightblue'>$value</td>";
                    else{$strOutput .= "<td>-</td>";}
                }
                $strOutput .= "</tr>";
            }
            $strOutput .= "\n<tr><td colspan=6>&nbsp;</td></tr>";
        }
        $strOutput .= "<tr><td colspan=6 style='text-align:right;'><a  href='/account?a=professionals&type=people_detail&view=edit&panel=pro_rating_panel&pid=$pid'>edit</a></tr>" ;
        
    }elseif($mode == 'edit'){
        foreach($_RatingAnchors as $key => $Items){
            $strOutput .= "\n<tr><td colspan=6><b>$key<b></td></tr>";
            $counter = 0;
            foreach($Items as $shortKey => $title){
                $counter++;
                $background = '';
                if($counter % 2)$background = 'background:lightblue;';
                $strOutput .= "\n<tr><td style='font:12px arial;vertical-align:middle;padding-left:5px;$background'>$title</td>";
                $value = sget($Data, $shortKey);
                for($i = 1; $i <= 5; $i++){
                    
                    if($i == $value)$strOutput .= "<td style='padding-left:10px;$background'><input type=radio name='$shortKey' value='$i' checked /></td>";
                    else{$strOutput .= "<td  style='padding-left:10px;$background'><input type=radio name='$shortKey' value='$i' /></td>";}
                }
                $strOutput .= "</tr>";
            }
            $strOutput .= "\n<tr><td colspan=6>&nbsp;</td></tr>";
        }
        $strOutput .= "<tr><td colspan=6 style='text-align:right;'><br><input type='submit'  name=submit value='submit' class=button />" ;
        $pid = sget($_REQUEST, 'pid');
        $strOutput .= "<input type=hidden name=a value='professionals' />
                        <input type=hidden name=type value='people_detail' />
                        <input type=hidden name=panel value='pro_rating_panel' />
                        <input type=hidden name=update_section value='pro_rating_panel' />
                        <input type=hidden name=dataid value='$dataid' />
                        <input type=hidden name=pid value='$pid' />";
        
        $strOutput .= "</td></tr>";
    }
    $strOutput .= "</table></form>";
    return $strOutput;
}
///////////////////////////////////////

function pro_progress($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = 'Progress Status';
    $redirect = rebuild_url(array('a','type','pid'));
    $panel = 'pro_progress_panel';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>ADD</a></div>";
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
    {
        
        $sql = "select * from maenna_people_data where pid = %d and data_type = 'progress' order by access desc limit 3";
        $result = db_query($sql, array($pid));
        while(($Row = db_fetch_object($result)) !== false){
            $date = date('m/d/Y', $Row->data_value3);
            $text = htmlentities(sideBarExcerpt($Row->data_value2),ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content .= "\n<div class=row style='padding:5px 0'><b>$date</b>&nbsp;&nbsp; $text</div>";
        }
        $content .= "<div style='font:bold 16px arial;text-align:center;'>$rating</div>";
        $viewall_link = "$redirect&panel=${panel}";
        $content .= "<div class='viewallbtn'><a href='$viewall_link' class=tool>View all</a></div>";
    
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}

function pro_progress_panel($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $dataid = sget($_REQUEST, 'dataid');
    $Block['title'] = 'PROGRESS STSTUS';
    $redirect = rebuild_url(array('a','type','pid'));
    $panel = 'pro_progress_panel';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>ADD</a></div>";
    $content = '';
    $time = time();
    
    $view = sget($_REQUEST, 'view');
    $do = sget($_REQUEST, 'do');
    
    if(empty($pid)) {
        drupal_set_message("Invalid pid",'error');
        return;
    }
    if($op == '' || $op == 'view'){
        if($view == '' || $view == 'view'){
            $sql = "select * from maenna_people_data where pid = %d and data_type = 'progress' order by access ";
            $result = db_query($sql, array($pid));
            $str = '';
            while(($Row = db_fetch_object($result))!== false){
                $dataid = $Row->dataid;
                $date = date('m/d/Y', $Row->data_value3);
                $text = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                $contact = htmlentities($Row->data_value,ENT_QUOTES | ENT_IGNORE, "UTF-8");
                if($Row->data_attr)$followup = date('m/d/Y', $Row->data_attr);
                else $followup = '';
                $delete = "<a href='/account?a=professionals&type=people_detail&panel=pro_progress_panel&update_section=pro_progress_panel&view=view&do=delete&dataid=$dataid&pid=$pid' class='delete-icon'  onclick='return confirm(\"Continue to remove the record\")'>>delete</a>";
                
                $edit = "<a href='/account?a=professionals&type=people_detail&panel=pro_progress_panel&view=edit&dataid=$dataid&pid=$pid'  class=>edit</a>";
                $str .= "\n<tr><td>$date</td><td>$contact</td><td>$text</td><td>$followup</td><td>$edit</td><td>$delete</td></tr>";
            }
            $content .= <<< END
            <table class='report'>
                <tr>
                    <td width=100>Date</td>
                    <td width=100>Contact Name</td>
                    <td>Message</td>
                    <td width=100>Followup Date</td>
                    <td width=30></td>
                    <td width=30></td>
                </tr>
                $str
            </table>
END;
        }elseif($view == 'edit' || $view == 'add'){
            $contact = $followup = $text = '';
            $date = date('m/d/Y');
            if($dataid){
                $sql = "select * from maenna_people_data where dataid = %d limit 1";
                $result = db_query($sql,array($dataid));
                $Row = db_fetch_object($result);
                if($Row){
                    $date = date('m/d/Y', $Row->data_value3);
                    if($Row->data_attr)$followup = date('m/d/Y', $Row->data_attr);
                    if($Row->data_value) $contact = $Row->data_value;
                    $text = ($Row->data_value2);
                }
            }
           
            $content .= <<< END
            <form action='/account' method=post>
    <table class='edit_table'>
        <tr><td>Date:</td><td><input type=text name=date value='$date' class='datepicker' /></td></tr>
        <tr><td>Contact Name:</td><td><input type=text name=contact value="$contact" /></td></tr>
        <tr><td>Message:</td><td><textarea name=text style='width:300px;height:80px;'>$text</textarea></td></tr>
        <tr><td>Followup:</td><td><input type=text name=followup value='$followup' class='datepicker' /></td></tr>
        <tr><td></td><td><input type=submit name=submit value=submit class=button /></td></tr>
    </table>
        <input type=hidden name=a value=professionals />
        <input type=hidden name=type value=people_detail />
        <input type=hidden name=pid value=$pid />
        <input type=hidden name=dataid value=$dataid />
        <input type=hidden name=view value=view />
        <input type=hidden name=panel value=pro_progress_panel />
        <input type=hidden name=update_section value=pro_progress_panel />
        <input type=hidden name=do value=$view />
        </form>
END;
            $content .= '<script type="text/javascript">$(document).ready(function(){init_datepicker();})</script>';
            
        }
    }elseif($op == 'update'){
        
        if($do == 'add' || $do == 'edit'){
            $date = sget($_REQUEST, 'date');
            $followup = sget($_REQUEST, 'followup');
            if($date) $date = strtotime($date);
            if($followup)$followup = strtotime($followup);
            $contact = sget($_REQUEST, 'contact');
            $text = sget($_REQUEST, 'text');
            if($do == 'edit' ){
                if($dataid){
                    $sql = "update maenna_people_data set data_value2 = '%s', data_value='%s', access='%s', data_attr = '%s', data_value3 = '%s',editorid=%d where data_type = 'progress' and dataid = %d limit 1";
                    $DBValue = array($text, $contact, $time, $followup, $date, $editorid, $dataid);
                    if(! db_query($sql, $DBValue)){
                        drupal_set_message("failed to update record", 'error');
                        return;
                    }
                }
            }elseif($do == 'add'){
                $sql = "insert into maenna_people_data (pid, access, data_type, data_attr, data_value, data_value2, data_value3,editorid) values (%d, '%s','%s','%s','%s','%s','%s',%d)";
                $DBValue = array($pid, $time, 'progress',$followup, $contact, $text, $date,$editorid);
                if(! db_query($sql, $DBValue)){
                    drupal_set_message("Failed to add new record", 'error');
                    return;
                }
            }
        }elseif($do == 'delete'){
            
            if(empty($dataid)){
                drupal_set_message('invalud record id', 'error');
                return;
            }
            $sql = "delete from maenna_people_data where data_type = 'progress' and dataid = %d";
            if(db_query($sql, array($dataid))){
                drupal_set_message("the record is removed");
            }else{
                drupal_set_message("failed to remove the record", 'error');
            }
            return ;
        }
    }
    
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function pro_addInfo($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = 'ADD INFORMATION';
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
    {
        $content .= "<div style='font:bold 16px arial;text-align:center;'>$rating</div>";
        $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
                    "<a href='/account?a=professionals&type=people_detail&view=edit&panel=pro_addInfo_panel&pid=$pid'>edit</a>&nbsp;" .
                    "</div>";
    
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}
function pro_addInfo_panel($op = null){
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = 'Professional Profile';
    
    $content = "<h3>" . getProId(sget($Maenna_people, 'pid')) . "</h3>";
   
    $time = time();
    
    $view = sget($_REQUEST, 'view');
    $InfoKeys = array('domain' => 'Domain Expertise',
                      'keyaccmp' => 'Key Accomplishments',
                      'personality' => 'Personality',
                      'industryview' => 'Industry View',
                      'managementview' => 'Management View');
    $InfoValues = array();
    if($op == null || $op == 'view')
    {        
        foreach($InfoKeys as $key => $val){
            $InfoValues["$key"] = '';
        }
        if($view == 'view' || $view == 'edit' || $view == ''){
            $dataid = '';
            $sql = "select * from maenna_people_data where pid = %d and data_type = 'addInfo' limit 1";
            $result = db_query($sql, array($pid));
            $Row = db_fetch_object($result);
            if($Row){
                $dataid = $Row->dataid;
                $Data = unserialize($Row->data_value2);
                foreach($InfoKeys as $key => $val){
                    $InfoValues["$key"] = sget($Data, $key);
                }
            }
            
            if($view == 'view' || $view == ''){
                $key = sget($_REQUEST, 'key');
                $title = sget($InfoKeys, $key);
               
                $text = nl2br(htmlentities(sget($InfoValues, $key), ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                $content .= "<div style='margin-top:20px;'><div style='font:bold 14px arial'>$title</div><p>$text</p></div>";
            }elseif($view == 'edit'){
                $content = "<form action='/account' method=post>";
                foreach($InfoKeys as $key => $title){
                    $text = sget($InfoValues, $key);
                    $content .= "\n<div class='row' style='padding:10px 0'><b>$title</b><br><textarea name=$key style='width:550px;height:100px;'>$text</textarea></div>";
                }
                $content .= <<< END
                    <input type=submit name=submit value=submit class=button />
                    <input type=hidden name=a value=professionals />
                    <input type=hidden name=type value=people_detail />
                    <input type=hidden name=panel value=pro_addinfo_panel />
                    <input type=hidden name=update_section value=pro_addinfo_panel />
                    <input type=hidden name=pid value='$pid' />
                    <input type=hidden name=dataid value='$dataid' />
                    <input type=hidden name=view value=edit />
END;
            }
        
            
        }
    }elseif($op == 'update'){
        foreach($InfoKeys as $key => $title){
            $InfoValues["$key"] = sget($_REQUEST, $key);
        }
        $dataid = sget($_REQUEST, 'dataid');
        $data_value2 = serialize($InfoValues);
        
        if( empty($dataid)){
            $sql = "insert into maenna_people_data (pid, access, data_type, data_value2, editorid) values(%d,'%s','%s','%s',%d)";
            $DBValues = array($pid , $time, 'addinfo', $data_value2, $editorid);
        }else{
            $sql = "update maenna_people_data set data_value2 = '%s', access='%s', editorid = %d where dataid = %d and pid = %d and data_type = 'addinfo' limit 1";
            $DBValues = array($data_value2, $time, $editorid, $dataid, $pid);
        }
        if(db_query($sql, $DBValues))drupal_set_message("Information is updated");
        else drupal_set_message("Failed to update information", 'error');
        return;
        
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function pro_hourStats($op = null){
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = 'STATISTICS';
    $redirect = rebuild_url(array('a','type','pid'));
    $panel = 'pro_hourStats_panel';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>Edit</a></div>";
    $content = '';
    $data_type = "activitystats";
    $DATA_ATTR = array('advisory' => "Advisory",
                       'sproj' => "Special Projects",
                       'analysis'=> "Analysis",
                       'proref' => "Pro. Referrals",
                       'coref' => "Co. Referrals");
    $Keys = array_keys($DATA_ATTR);
    $Data = array();
    $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' and data_attr = '%s' ";
    for($i = 0; $i <3; $i++)
    {
        $total_hr = 0;
        $num_of_company = 0;
        $data_attr = $Keys[$i];
        
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        while($Row = db_fetch_array($result))
        {
            $total_hr += sget0($Row, 'data_value2');
            $num_of_company += 1;
        }
        $Data["$data_attr"] = array($total_hr . " Hrs" , $num_of_company . " Co.");
    }
    
    
    $content .= "<div class='entry' style='border:none'>";
    foreach($DATA_ATTR as $key => $title)
    {
        $val1 = $Data["$key"][0];
        $val2 = $Data["$key"][1];
        
        $content .= "\n     <div class='column row' style='width:60%;'>$title</div>
                            <div class='row column' style='width:25%;'>$val1 &nbsp;</div>
                            <div class='row column' style='width:15%;'>$val2 &nbsp;</div><br style='clear:both' />";
        
    }
    $content .= "</div>";

    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function pro_hourStats_panel($op = null)
{
    
}

function pro_selectWork($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = 'Select Work / Admirable';
    
    $redirect = rebuild_url(array('a','type','pid'));
    $content = '';
    $data_type = 'proselectwork';
    $panel = 'file_and_links';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>ADD</a></div>";
    $content = '';
    
    
    
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}


function file_and_links($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    
    $redirect = rebuild_url(array('a','type','pid'));
    $content = '';
    $data_type = sget($_REQUEST,'dtype');
    $panel = 'proselectwork';
    $content = '';
    $Block['title'] = 'Select Work / Admirable';
    //$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>ADD</a></div>";
    
    if($op == null || $op == 'view')
    {
        $view = sget($_REQUEST, 'view');
        if($view == 'listview' || empty($view))
        {
            $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' order by dataid desc";
            $result = db_query($sql, array($pid, $data_type));
            $content .= "<table class='report-table'>";
            while($Row = db_fetch_object($result))
            {
                $title = $Row->data_value;
                $type = $Row->data_value3;
                
                if($type == 'link')
                {
                    $content .= "<tr><td></td><td></td></tr>";
                }
            }
            $content .= "</table>";
        }elseif($view == 'add' || $view == 'edit')
        {
            $dataid = sget($_REQUEST, 'dataid');
            $Row = array();
            if($dataid)
            {
                $sql = "select * from maenna_people_data where dataid = %d";
                $result = db_query($sql, $dataid);
                $Row = db_fetch_array($result);
            }
            if(count($Row) == 0)
            {
                $content = <<< END
                    <form method=post action='/account'>
                        <div style='margin-top:15px;'>
                            <label>TITLE:</label><br /><input type=text name=title value='' style='width:300px;' />
                        </div>
                        <div style='margin-top:10px;'>Please upload a file or post a web link:<br>
                        <div style='margin-top:8px;'>File: <br ><input type=file name=file value='' /> </div>
                        <div style='margin-top:8px;'>Web Link: <br ><input type=text name=url value='' style='width:300px;'  /></div>
                        
                        
                    </form>
END;
                $content .= js_init("init_openbox();");
            }
        }
        
    }
    
    
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function pro_bio($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $time = time();
    $Block['title'] = "Bio";

    
    $redirect = rebuild_url(array('a','type','pid'));
    $content = '';
    $panel = 'bio_panel';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=detail' class=tool>View</a></div>";
    
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}
function bio_panel($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $time = time();
    $Block['title'] = "Bio";

    
    $redirect = rebuild_url(array('a','type','pid'));
    $content = '';
    $panel = 'bio_panel';
    $HV = hidden_post_values(array('a','type', 'pid'));
    
    if($op == null || $op == 'view')
    {
        $view = sget($_REQUEST, 'view');
        $sql = "select * from maenna_people where pid = %d";
        $result = db_query($sql, array($pid));
        $Row = db_fetch_object($result);
        $text = '';
        if($Row)
        {
            $text = $Row->brief_intro;
           
        }
        if($view == 'detail')
        {
            $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>Edit</a></div>";
            $content = nl2br(htmlentities($text, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
        }elseif($view == 'edit')
        {
            $Block['title'] = "Edit - " . $Block['title'];
            $content = "<form action='/account' method=post>
                    <textarea name='text' style='width:99%;height:400px;'>$text</textarea>
                    <br>
                    <input type=submit name=submit value=submit />
                    $HV
                    <input type=hidden name=update_section value='$panel' />
                    <input type=hidden name=panel value='$panel' />
                    <input type=hidden name=view value='detail' />
                </form>
            ";
        }
    }elseif($op == 'update')
    {
        $text = sget($_REQUEST, 'text');
        $sql = "update maenna_people set access = '%s', editorid = %d, brief_intro = '%s' where pid = %d limit 1";
        if(db_query($sql, array($time, $editorid, $text, $pid)))
        {
            drupal_set_message("Recrod updated");
        }else{
            drupal_set_message("Operation failed", 'error');
        }
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}


/* EOF */