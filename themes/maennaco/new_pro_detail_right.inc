<?php

function pro_rating($op = null){
    require_once("includes/classes/connections.inc");
    global $user;
    global $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $Block['title'] = 'CLEWED RATING';
    $redirect = rebuild_url(array('tab','page','id'));
    $panel = 'pro_rating_panel';
    
    if($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid)))
    {
        $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>EDIT</a></div>";
    }
    $content = '';
    $time = time();
    if(empty($op )) return '';
    if($op == 'title')
    {
        return $Block['title'];
    }elseif($op)
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
        $content .= "<div class=editbtn style='top:4px;text-align:left;'><a href='$redirect&panel=${panel}' class=tool>VIEW</a></div></div></div>";
    
        $Block['body'] = sidebar_box($Block['title'], $content,'css_admin_box');
    }
    return $Block;
}



function pro_rating_panel($op = null){
    require_once("includes/classes/connections.inc");
    global $user;
    global $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $dataid = sget($_REQUEST, 'dataid');
    
    $Block['title'] = 'CLEWED RATING';
    $redirect = rebuild_url(array('tab','page','id'));
    $panel = 'pro_rating_panel';
    if($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid)))
    {
        $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>EDIT</a></div>";
    }
    $content = '';
    $time = time();
    if(empty($op)) return '';
    if($op != 'update'){
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
        if($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid)))
        {
            // continue
        }else{
            drupal_set_message("Unauthorized operation", 'error');
            return '';
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

function ProRatingTable($Data, $mode,$dataid = '')
{
    $_RatingAnchors = _proRatingAnchors();
    $strOutput =  '';
    $redirect = rebuild_url(array('tab','page','id'));
    $pid = sget($_REQUEST, 'id');
    $strOutput = "<form action='/account' method='post'><table style='width:100%' border=0><tr>
                            <td width=370>Rating Anchors</td>
                            <td width=80 style='text-align:center;font-weight:bold;'>1</td>
                            <td width=80 style='text-align:center;font-weight:bold;'>2</td>
                            <td width=80 style='text-align:center;font-weight:bold;'>3</td>
                            <td width=80 style='text-align:center;font-weight:bold;'>4</td>
                            <td width=80 style='text-align:center;font-weight:bold;'>5</td>
                            </tr>";
    /*
      <td width=100>Strong Evidence Candidate Lacks Competency</td>
                            <td width=100>Some Evidence Candidate Lacks Competency</td>
                            <td width=100></td>
                            <td width=100>Some Evidence Candidate Has Competency</td>
                            <td width=100>Strong Evidence Candidate Has Competency</td>
                            </tr>";
     
    */
    $odd = '#f3f3f3';
    if($mode == 'view'){
        foreach($_RatingAnchors as $key => $Items){
            $strOutput .= "\n<tr><td colspan=6><b>$key<b></td></tr>";
            foreach($Items as $shortKey => $title){
                $strOutput .= "\n<tr><td style='text-align:left;'>$title</td>";
                $value = sget($Data, $shortKey);
                for($i = 1; $i <=5; $i++){
                    if($i == $value)$strOutput .= "<td style='background:lightblue;text-align:center;'>$value</td>";
                    else{$strOutput .= "<td style='text-align:center;'>-</td>";}
                }
                $strOutput .= "</tr>";
            }
            $strOutput .= "\n<tr><td colspan=6 class=no-border>&nbsp;</td></tr>";
        }
        $strOutput .= "\n<tr><td colspan=6 style='border:none'>&nbsp;</td></tr>";
        $strOutput .= "\n<tr><td colspan=6 style='border:none'><a href='$redirect' class=button>back</a></td></tr>";
       
    }elseif($mode == 'edit')
    {
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
                    
                    if($i == $value)$strOutput .= "<td style='text-align:center;$background'><input type=radio name='$shortKey' value='$i' checked /></td>";
                    else{$strOutput .= "<td  style='text-align:center;$background'><input type=radio name='$shortKey' value='$i' /></td>";}
                }
                $strOutput .= "</tr>";
            }
            $strOutput .= "\n<tr><td colspan=6>&nbsp;</td></tr>";
        }
        $strOutput .= "<tr><td colspan=6 style='text-align:right;'><br><input type='submit'  name=submit value='submit' class=button /> <a href='$redirect' class=button>cancel</a>" ;
        
        $strOutput .= "<input type=hidden name=tab value='professionals' />
                        <input type=hidden name=page value='pro_detail' />
                        <input type=hidden name=panel value='pro_rating_panel' />
                        <input type=hidden name=update_section value='pro_rating_panel' />
                        <input type=hidden name=dataid value='$dataid' />
                        <input type=hidden name=section value='pro_rating' />
                        <input type=hidden name=id value='$pid' />";
        
        $strOutput .= "</td></tr>";
    }
    $strOutput .= "</table></form>";
    $strOutput .= <<< END
    
    <ul>
        <li>1 = Strong Evidence Candidate Lacks Competency</li>
        <li>5 = Strong Evidence Candidate Has Competency</li>
    </ul>
                            
END;
    return $strOutput;
}
///////////////////////////////////////

function progress_status($op = null)
{
    require_once("includes/classes/connections.inc");
    global $user, $AccessObj;
    
    if($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid)))
    {
        // continue
    }else{
        return '';
    }
    
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $Block['title'] = 'PROGRESS STATUS';
    $redirect = rebuild_url(array('tab','page','id'));
    $panel = 'pro_progress_panel';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>ADD</a></div>";
    $content = '';
    $time = time();
    
    if($op && $op != 'update')
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
        $content .= "<div class='viewallbtn' style='height:10px;'><a href='$viewall_link' class=tool>MORE</a></div><br style='clear:both' />";
    
        $Block['body'] = sidebar_box($Block['title'], $content,'css_admin_box');
    }
    return $Block;
}

function pro_progress_panel($op = null)
{
    require_once("includes/classes/connections.inc");
    global $user, $AccessObj;
    
    if($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid)))
    {
        // continue
    }else return '';

    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $dataid = sget($_REQUEST, 'dataid');
    $Block['title'] = 'PROGRESS STSTUS';
    $redirect = rebuild_url(array('tab','page','id'));
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
    if($op && $op != 'update'){
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
                $delete = "<a href='$redirect&panel=pro_progress_panel&update_section=pro_progress_panel&view=view&do=delete&section=pro_progress&dataid=$dataid&id=$pid' class='delete-icon'  onclick='return confirm(\"Continue to remove the record\")'>>delete</a>";
                
                $edit = "<a href='$redirect&panel=pro_progress_panel&view=edit&dataid=$dataid&id=$pid'  class=>edit</a>";
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
            <br />
            <a href='$redirect' class=button>back</a>
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
            $HV = hidden_post_values(array('tab','page','id'));
            $content .= <<< END
            <form action='/account' method=post>
    <table class='edit_table'>
        <tr><td>Date:</td><td><input type=text name=date value='$date' class='datepicker' /></td></tr>
        <tr><td>Contact Name:</td><td><input type=text name=contact value="$contact" /></td></tr>
        <tr><td>Message:</td><td><textarea name=text style='width:300px;height:80px;'>$text</textarea></td></tr>
        <tr><td>Followup:</td><td><input type=text name=followup value='$followup' class='datepicker' /></td></tr>
        <tr><td></td><td><input type=submit name=submit value=submit class=button /> <a href='$redirect' class=button>cancel</a></td></tr>
    </table>
        $HV 
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

function pro_eventCalendar($op = null){
    function ifAdmin ($uname) {
	 
	 $result = mysql_query("SELECT *  FROM users u WHERE u.name = '".$uname."' AND EXISTS (SELECT * FROM users_roles WHERE uid = u.uid AND rid IN (SELECT rid FROM role WHERE name = 'Super admin' OR name = 'Maennaco admin'))");
	 
	 if (mysql_num_rows($result) > 0 ) {return true;} else return false;
	}

    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $Block['title'] = 'DISCUSSIONS';
    $redirect = rebuild_url(array('tab','page','id'));
    $panel = 'pro_hourStats_panel';
    //if($op == 'write')$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>EDIT</a></div>";
    $content = '';
    
    if(empty($op)) return '';
    
    
    $content .= "<div class='entry' style='border:none'>";

   if (ifAdmin($user->name) || $editorid == $pid ) {

    
    $eventRes = mysql_query("SELECT * FROM maenna_company_events_inv inv JOIN maenna_company_events ev ON ev.eventid = inv.eventid WHERE uid = '".$pid."' AND inv.status = 'confirmed' ORDER BY datetime DESC");
    
    $cnt = 1;
    while ($events = mysql_fetch_array($eventRes)) {
        if ($cnt == 6) break;
        $content .= '<div class="column row" style="width:100%;">'.date("m/d/Y",$events['datetime'])." - <a target = \"_blank\" href=\"http://www.clewed.com/account?tab=companies&page=company_detail&id=".$events['companyid']."&mtab=advice\" >".strtoupper($events['title'])."</a></div><br style='clear:both' />";
        $cnt++;
        
    }
    if ($events = mysql_fetch_array($eventRes) || $cnt==6)
    $content .= "<a id = \"moreEvents\" style = \"cursor:pointer;\"><br>show more events</a>
    <script>
    $(document).ready(function(){
    
    $(\"#moreEvents\").click(function() {
    
                        var a = $(this);
    
    			$.post(\"/themes/maennaco/includes/posts.php?type=moreEventsCalendar&u=".$editorid."\", {
			
				}, function(response){
					
                                    a.parent().append(response);
                                    a.remove();
                                    
			
				});
    
    });
    
    });
    </script>
    
    ";
   } else { $content .= "You`re not allowed to see this user calendar!";}
    $content .= "</div>";

    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function pro_hourStats($op = null){
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $Block['title'] = 'STATISTICS';
    $redirect = rebuild_url(array('tab','page','id'));
    $panel = 'pro_hourStats_panel';
    //if($op == 'write')$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>EDIT</a></div>";
    $content = '';
    
    if(empty($op)) return '';
    
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
        $title = strtoupper($title);
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
    $pid = sget($_REQUEST, 'id');
    $Block['title'] = 'READINGS / IDEAS';
    
    $redirect = rebuild_url(array('tab','page','id'));
    $content = '';
    if(empty($op)) return '';
    $data_type = 'proselectwork';
    $panel = 'file_and_links';
    if($op == 'write' || ($user->uid == $pid))
    {
        $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>ADD</a></div>";
    }
    $content = '';
    if($op )
    {
        $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' order by dataid desc";
        $result = db_query($sql, array($pid, $data_type));
        
        while($Row = db_fetch_object($result))
        {
            $title = strtoupper($Row->data_value);
            $title = htmlentities($title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $temp = $Row->data_value2;
            if($Row->data_attr == 'file')
            {
                $temp =  "./" . file_directory_path() . "/" . $temp;
            }
            $content .= "<div class='row singlerow'><a href='$tem' target='_blank'>$title</a></div>";
        }
    }
    
    
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}


function file_and_links($op = null)
{
    require_once("includes/classes/connections.inc");
    global $user, $AccessObj;
    
    

    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    
    if($AccessObj->user_type == 'super' ||
        ($AccessObj->user_type == 'admin' && Connections::admin_of($pid)) ||
        $AccessObj->uid == $pid)
    {
        // continue
    }else return '';
    
    $redirect = rebuild_url(array('tab','page','id'));
    $HV = hidden_post_values(array('tab','page','id'));
    $content = '';
    $data_type = 'proselectwork';
    
    $content = '';
    $Block['title'] = 'READINGS1 / IDEAS';
    //$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>ADD</a></div>";
    if(empty($op )) return '';
    if($op && $op != 'update')
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
                    <form method=post action='/account' enctype='multipart/form-data'>
                        <div style='margin-top:15px;'>
                            <label>TITLE:</label><br /><input type=text name=title value='' style='width:300px;' />
                        </div>
                        <div style='margin-top:10px;'>Please upload a file OR enter a web link:<br>
                        <div style='margin-top:8px;'>File: <br ><input type=file name=file value='' /> </div>
                        <div style='margin-top:8px;'>Web Link: <br ><input type=text name=url value='' style='width:300px;'  /></div>
                        <br />
                        <input type='submit' name=submit value=submit class=button /> <a href='$redirect' class=button>cancel</a>
                        $HV
                        <input type='hidden' name='update_section' value='file_and_links' />
                        <input type='hidden' name='do' value='new' />
                    </form>
END;
                $content .= js_init("init_openbox();");
            }
        }
        
    }elseif($op == 'update')
    {
       
        $do = sget($_REQUEST, 'do');
        if( $do = 'new' )
        {
            $title = sget($_REQUEST, 'title');
            $url = sget($_REQUEST, 'url');
            if($title && $url)
            {
                $data_attr = 'url';
                $data_value = $title;
                $data_value2 = $url;
            }elseif($title && isset($_FILES['file']) && ($_FILES['file']['error'] == 0))
            {
                $file = $_FILES['file']['name'];
                $filename = $time . '_' . clearString($file);
                $path =  "./" . file_directory_path() . "/" . $filename;
                if(move_uploaded_file($_FILES['file']['tmp_name'], $path))
                {
                    $data_attr = 'file';
                    $data_value = $title;
                    $data_value2 = $filename;
                }else
                {
                    drupal_set_message("File uploading failed", 'error');
                }
            }else
            {
                drupal_set_message("Please complete the form and try again", 'error');
                return;
            }
            $sql = "insert into maenna_people_data (pid, access, data_type, data_attr, data_value, data_value2) values (%d, '%s', '%s', '%s', '%s','%s')";
            $DBValues = array($pid, $time,$data_type, $data_attr, $data_value, $data_value2 );
            if(db_query($sql, $DBValues))
            {
                drupal_set_message("Record Added");
            }else{
                drupal_set_message("Failed to add new record", 'error');
            }
        }elseif($do = 'del')
        {
            $dataid = sget($_REQUEST, 'dataid');
            if($dataid)
            {
                $sql = "update maenna_people_data set deleted = 1 where dataid = %d limit 1";
                if(db_query($sql, array($dataid)))
                {
                    drupal_set_message("Record removed");
                }else{
                    drupal_set_message("Operation Failed", 'error');
                }
            }else{
                drupal_set_message("Invalid dataid", 'error');
            }
        }
        return;
    }
    
    
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function pro_bio($op = null)
{
    require_once("includes/classes/connections.inc");
    global $user, $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "BIO";
    $redirect = rebuild_url(array('tab','page','id'));
    
    global $user, $AccessObj;
    
    if($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid)))
    {
        //continue;
    }else{
        return '';
    }
    if(empty( $op )) return '';
    $redirect = rebuild_url(array('tab','page','id'));
    $content = '';
    $panel = 'bio_panel';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=detail' class=tool>VIEW</a></div>";
    
    $Block['body'] = sidebar_box($Block['title'], $content,'css_admin_box');
    return $Block;
}

function bio_panel($op = null)
{
    require_once("includes/classes/connections.inc");
    global $user, $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "BIO";
    $redirect = rebuild_url(array('tab','page','id'));
    if($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid)))
    {
        //continue;
    }else{
        return '';
    }
    
    $redirect = rebuild_url(array('tab','page','id'));
    $content = '';
    $panel = 'bio_panel';
    $HV = hidden_post_values(array('tab','page', 'id'));
    
    if($op && $op != 'update' )
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
            $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>EDIT</a></div>";
            $content = nl2br(htmlentities($text, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $content .= "<br /><br /><a href='$redirect' class='button'>back</a>";
        }elseif($view == 'edit')
        {
            $Block['title'] = "EDIT - " . $Block['title'];
            $content = "<form action='/account' method=post>
                    <textarea name='text' style='width:99%;height:400px;'>$text</textarea>
                    <br>
                    <input type=submit name=submit value=submit /> <a href='$redirect' class='button'>back</a>
                    $HV
                    <input type=hidden name=update_section value='$panel' />
                    <input type=hidden name=panel value='$panel' />
                    <input type=hidden name=section value='pro_bio' />
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

function manage_connections($op = null)
{
    global $AccessObj;
    $id = sget($_REQUEST, 'id');
    if(empty($id)) return '';
    if($AccessObj->user_type != 'super' && (! Connections::admin_of($id))) return '';

    $section = __FUNCTION__;
    $editorid = $AccessObj->uid;

    $time = time();
    $redirect = rebuild_url(array('tab','page','id','mtab'));
    $HV =  hidden_post_values(array('tab','page','mtab','id'));
    
    $Block['title'] = "MANAGE CONNECTIONS";
    ///$Block['title'] .= "<div class=editbtn><a href='$redirect&section=${section}&panel=mcp' class=tool>EDIT</a></div>";
    
    $content = "<div class=row><a href='$redirect&section=${section}&panel=mcp&type=com'>COMPANY CONNECTIONS</a></div>
                <div class=row><a href='$redirect&section=${section}&panel=mcp&type=pro'>PROFESSIONAL CONNECTIONS</a></div>
                ";
    
    
    $Block['body'] = sidebar_box($Block['title'], $content,'css_admin_box');
    return $Block;
}

function mcp($op = null)
{
    require_once("includes/classes/maenna_users.inc");
    require_once("includes/classes/connections.inc");
    global $AccessObj;
    $id = sget($_REQUEST, 'id');
    if(empty($id)) return '';
    if($AccessObj->user_type != 'super' && (! Connections::admin_of($id))) return '';
    
    $type = sget($_REQUEST, 'type');
    $section = __FUNCTION__;
    $redirect = rebuild_url(array('tab','page','id','mtab'));
    $HV =  hidden_post_values(array('tab','page','mtab','id'));
    $Block['title'] = "MANAGE CONNECTIONS";
    
    if($op && $op != 'update')
    {
        if($type == 'com') $Block['title'] .= " - Company Connections";
        else $Block['title'] .= " - Professional Connections";
        
        $Admin_conns    =   Connections::admin_connections($AccessObj->uid);
        $User_conns     =   Connections::Pro_conns($id);
        //test_array($User_conns);
        $TR = '';

        if($type == 'pro')
        {
            
            $Options = array('none' => '----',
                             'related' => 'Visible');
            foreach($Admin_conns['Pro'] as $Conn)
            {
                $pro_id = $Conn->target_uid;
                if($pro_id == $id)continue;
                $maennaid = getProId($pro_id);
                $conn_id = '';
                $relation = 'none';
                foreach($User_conns['Related'] as $User_conn)
                {
                    if($User_conn->assignee_uid == $pro_id)
                    {
                        $conn_id = $User_conn->connid;
                        $relation = 'related';
                        break;
                    }
                }
                $select = "<select name='REL[$pro_id][conntype]'>" .option_code($Options, $relation). " </select>";
                $TR .= "\n<tr><td>$maennaid <input type='hidden' name='REL[$pro_id][connid]' value='$conn_id' /></td><td>$select</td></tr>";
            }
        }elseif($type == 'com')
        {
            $Options1 = array('none' => '----',
                             'visible' => 'Visible',
                             'advisor' => 'Advisor',
                             'propose' => 'Propose as advisor');
            $Options2 = array('none' => '----',
                             'visible' => 'Visible',
                             'advisor' => 'Advisor',
                             );
            
            foreach($Admin_conns['Com'] as $Admin_conn)
            {
                $com_id = $Admin_conn->target_uid; // 
                $maenna_id = getProjectName($com_id);
                $conn_id = '';
                $relation = 'none';
                $Options = $Options1;
                foreach($User_conns as $key => $User_conn)
                {
                    $found = false;
                    if(in_array($key, array('Advisor','Propose','Visible')))
                    {
                        foreach($User_conn as $_Conn)
                        {
                            
                            if($_Conn->target_uid == $com_id)
                            {
                                
                                $conn_id = $_Conn->connid;
                                $relation = $_Conn->conntype;
                                echo "$com_id, $id, $relation<br>";
                                if(strcasecmp($relation, 'advisor') == 0)
                                {
                                    $Options = $Options2;
                                }
                                $found = true;
                                break;
                                
                            }
                        }
                    }
                    if($found) break;
                }
                $select = "<select name='REL[$com_id][conntype]'>" .option_code($Options, $relation). " </select>";
                $TR .= "\n<tr><td>$maenna_id <input type='hidden' name='REL[$com_id][connid]' value='$conn_id' /></td><td>$select</td></tr>";
            }
        }
        $content = <<< END
        <form method=post action='/account'>
            <table cellspacing=0 cellpadding=0 class='table-list'>
                <tr style='background:#f2f2f2;'><td>Maenna ID</td><td>Connection Type</td></tr>
                $TR
            </table>
            <input type=submit name=submit value=submit class=button /> <a href='$redirect' class=button>cancel</a>
            $HV
            <input type=hidden name=type value='$type' />
            <input type=hidden name=section value='manage_connections' />
            <input type=hidden name=panel value='$section' />
            <input type=hidden name=update_section value='$section' />
        </form>
END;
        
    }elseif($op == 'update')
    {
        $REL = $_POST['REL'];
        //test_array($REL);
        if($type == 'pro')
        {
            foreach($REL as $assignee_uid => $Rel)
            {
                $connid = sget($Rel, 'connid');
                $conntype = sget($Rel, 'conntype');
                if( ! Connections::set_pro_connection($id , $assignee_uid, $connid, $conntype)) // current pro id as target_uid
                {
                    //test_array($Rel);
                    //test_array(array($target_uid, $id, $connid, $conntype));
                    drupal_set_message("Failed to set connections", 'error');
                    return;
                }
            }
            drupal_set_message("Operation Successful");
            return;
        }elseif($type == 'com')
        {
            foreach($REL as $com_id => $Rel)
            {
                $connid = sget($Rel, 'connid');
                $conntype = sget($Rel, 'conntype');
                if($conntype == 'advisor' || $conntype == 'propose')
                {
                    if( ! Connections::set_pro_connection( $id,$com_id, $connid, $conntype))
                    {

                        drupal_set_message("Failed to set $conntype connections", 'error');
                        return;
                    }
                }else{
                    if( ! Connections::set_pro_connection(  $id,$com_id , $connid, $conntype))
                    {
                        drupal_set_message("Failed to set $conntype connections", 'error');
                        return;
                    }
                }
                
                
            }
            drupal_set_message("Operation Successful");
            return;
        }
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function pro_questionnaire($op = null)
{
    require_once("includes/classes/connections.inc");
    global $user, $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "PRO QUESTIONNAIRE";

    
    global $user, $AccessObj;
    
    if($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid)))
    {
        //continue;
    }else{
        return '';
    }
    if(empty( $op )) return '';

    $content = '';
    $sql = "select recordid from maenna_questionair where target='pro' and status = 1 and type = 'version'";
    $result = db_query($sql);
    $Row = db_fetch_object($result);
    if($Row)
    {
        $parentid = $Row->recordid;
    }else return '';
    
    $Block['title'] .= "<div class=editbtn><a href='/account?tab=information&page=qmgmt&panel=answerdetail&target=pro&id=$parentid&uid=$pid' class=tool>VIEW</a></div>";
    
    $Block['body'] = sidebar_box($Block['title'], $content,'css_admin_box');
    return $Block;
}

/* EOF */