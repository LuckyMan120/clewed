<?php
//   $maenna_page['content'] = array('acp_list','project_list');
//        $maenna_page['right']= array('post_project', 'stats_byindustry','stats_bypro' );

$maenna_page['right']= array('info_tools', 'post_event_links');
global $redirect;
$redirect = rebuild_url(array('tab', 'page'));

function post_event_links($op = null){
    global $user, $redirect;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    
    $Block['title'] =  "<div style='padding:5px 0; '>
                            <a href='$redirect&panel=event_posting&eventtype=advisory'>Post Advisory Council Event</a></div>" .
                      "<div style='padding:5px 0; '>
                            <a href='$redirect&panel=event_posting&eventtype=group' >Post Group Event</a></div>" ;
    $content = '';
    $time = time();
      
    $Block['body'] = sidebar_box($Block['title'], $content);

    return $Block;
}

function event_posting($op = null)
{
    global $user, $redirect;
    $editorid = $user->uid;
    $time = time();
    
    if($op && $op != 'update')
    {
        $eventType = sget($_REQUEST, 'eventtype');
        if(empty($eventType)) return '';
        
        $title = '';
        if($eventType == 'group')$title = 'Group';
        else $title = "Adviosry Council";
        
        $option_industry = Options_industry();
        $option_hour = Options_hour();
        $option_minute = Options_minute();
        $option_topic = Options_infoTopic();
        $option_capacity = Options_capacity();
        
        $projectName = '';
        $tdMaennaId = '';
        if(sget($_REQUEST, 'companyid'))$projectName = getProjectName($companyid);
        if($eventType == 'advisory') $tdMaennaId = "<tr><td>Maenna ID</td><td><input type='text' name='maennaid' value='$projectName'  class='require_string' /></td></tr>";
        
        $content = <<< END
        <div style='font:italic bold 14px arial;'>Post $title Event</div>
        <form action='/account' method='post' onsubmit='return check_input();'>
        <table  class='edit_table' style='width:500px;'>
            $tdMaennaId
            <tr><td>Description</td><td><textarea name=description style='width:300px;height:80px;' class='require_string'></textarea></td></tr>
            <tr><td>Event Host:</td><td><input type=text name=host value='' class='require_string' /></td></tr>
            <tr><td>Date</td><td><input type=text name=date value='' class='datepicker require_string' style='width:80px;' />
                                    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                    Time:&nbsp;&nbsp;&nbsp;
                                            <select name='hour' class='require_string'><option value=''>Hour</option><option></option>$option_hour</select> :
                                            <select name='minute'  class='require_string'><option value=''>Minute</option><option></option>$option_minute</select></td></tr>
            <tr><td>Topic</td><td><select name='topic'  style='width:250px' class='require_string'><option></option>$option_topic</select></td></tr>
            <tr><td>Industry:</td><td><select name=industry style='width:250px'><option></option>$option_industry</select></td></tr>
            <tr><td>Capacity:</td><td><select name=capacity class='require_string'><option></option>$option_capacity</td></tr>
            <tr><td></td><td><input type=submit name=submit value='submit' class=button /></td></tr>
        </table>
            <input type=hidden name='tab' value='information' />
            <input type=hidden name='page' value='events' />
            <input type=hidden name='panel' value='event_posting' />
            <input type=hidden name='update_section' value='event_posting' />
            <input type=hidden name='eventtype' value='$eventType' />
            <input type=hidden name='status' value='open' />
            
        </form>
END;
        $content .= js_init("init_datepicker();");
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update'){
        $DBKeys = array('companyid', 'description', 'host','eventtype', 'topic', 'industry', 'status', 'capacity', 'created', 'postedby','eventdate');
        $DBValues = array();
        $SQL_STR = array();
        foreach($DBKeys as $key){
            $DBValues["$key"] = sget($_REQUEST, $key);
            $SQL_STR["$key"] = "'%s'";
        }
        if($DBValues["eventtype"] == 'advisory'){
            $DBValues["companyid"] = realid(sget($_REQUEST, 'maennaid'));
            if(empty($DBValues['companyid'])){
                drupal_set_message('Invalid Maenna Id', 'error');
                return;
            }
        }
        $DBValues['eventdate'] = sget($_REQUEST, 'date') . ' '. sget($_REQUEST, 'hour') . ':' . sget($_REQUEST, 'minute');
        $DBValues['eventdate'] = strtotime($DBValues['eventdate']);
        $DBValues["created"] = $time;
        $DBValues['postedby'] = $editorid;
        
        $sql = "insert into maenna_events (".implode(',', $DBKeys).") values (".implode(',', $SQL_STR). ")";
        if(db_query($sql, $DBValues)){
            drupal_set_message('New event posted');
        }else{
            drupal_set_message('Invalid Maenna ID', 'error');
        }
        return;
    }
    

    return $Block;
}
/* EOF */