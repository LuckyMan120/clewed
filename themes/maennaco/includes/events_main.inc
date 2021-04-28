<?php
//   $maenna_page['content'] = array(''search_bar','acp_list','project_list');
//
$maenna_page['content'] = array('infoTabs','search_bar','event_list');

function search_bar($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
    {
        $date = 'Date';
        if(sget($_REQUEST, 'date'))$date = sget($_REQUEST, 'date');
        $industry = sget($_REQUEST, 'industry');
        $topic = sget($_REQUEST, 'topic');
        $option_topic = Options_infoTopic($topic);
        $option_industry =  Options_industry($industry);
        $option_month = Options_month($month);
        $year = date('Y');
        $content =<<< END
    <div>
        <form action='/account' method='post'>
        <table style='width:680px;' class='no-border'>
            <tr><td style='vertical-align:middle'><b>Search: </b></td>
                <td><select name=topic style=''><option value='' >Topic</option>
                    <option></option>$option_topic</select></td>
                <td><select name=industry style='width:250px;'><option value='' >Industry</option>
                    <option></option>$option_industry</select></td>
                <td><input type=text name=date value='$date' class='monthyearpicker' /></td>
                <td><input type=submit name=submit value='Filter' class=button /></td>
            </tr>
        </table>
            <input type='hidden' name='a' value='information' />
            <input type='hidden' name='tab' value='events' />
            <input type='hidden' name='year' value='$year' />
        </form>
    </div>
END;
        $content.= js_init("init_datepicker();");
     $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}


function event_list($op = null){
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();
    
    $SearchKeys = array('topic', 'industry', 'eventdate');
    $SQL_WHERE = $DBValues = array();
    $str_sql_where = '';
    foreach($SearchKeys as $key){
        if(sget($_REQUEST, $key))
        {
            $SQL_WHERE["$key"] = "$key = '%s'";
            $DBValues[] = sget($_REQUEST,$key);
        }
    }
    if(($eventdate = sget($_REQUEST, 'date')) && (strcasecmp($eventdate, 'date') != 0 )){
        $M = explode(" ", $eventdate);
        if(is_array($M) && (list($month, $year) = $M))
        {
           
            $firstdaytime = strtotime($eventdate);
            $lastdaytime = strtotime('+ 1 month', $firstdaytime);
            $SQL_WHERE[] = "( eventdate > $firstdaytime and eventdate < $lastdaytime)";
        }
    }
    if(count($SQL_WHERE) > 0){
        $str_sql_where = " and " . implode(' and ', $SQL_WHERE);
    }
    
    if($op == 'view' || $op == null)
    {
        $Table = array('title' => 'Group Events',
                       'class' => 'report',
                       'thead'=> array(),
                       'tbody'=> array());
        $Table['thead'] = array(
                            array('label'=> 'Event Description', 'style' => ''),
                            array('label'=> 'Date', 'style' => 'width:100px'),
                            array('label'=> 'Topic', 'style' => 'width:120px'),
                            array('label'=> 'Host', 'style' => 'width:120px'),
                            array('label'=> 'status', 'style'=>'width:100px'),
                            array('label'=> 'details', 'style'=>'width:100px'),
                            array('label'=> 'register', 'style'=>'width:70px')
                            );
        
        $sql =      "select maenna_events.*,
                        maenna_events_data.dataid,
                        maenna_events_data.data_value1 as applystatus,
                        maenna_events_data.data_value2 as roleid
                        from maenna_events left join maenna_events_data on
                            maenna_events.eventid = maenna_events_data.eventid and
                            maenna_events_data.data_type = 'member' and
                            maenna_events_data.data_attr = ".((int) $editorid)." 
                        where eventtype='group' $str_sql_where order by created desc";
    
        $result  = db_query($sql,$DBValues);
        $Table['tbody'] = processResult($result);
        $content .= render_table($Table);
        $content.= js_init("init_ajaxTrigger();");
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function processResult($result)
{
    $Tbody = array();
    while($Row = db_fetch_object($result))
    {
        $dataid = $Row->dataid;
        $eventid = $Row->eventid;
        $capacity = $Row->capacity;
        $topic = Options_infoTopic($Row->topic, 1);
        $register = '';
        if($Row->status == 'open'){
           if($Row->applystatus == 'cancel' || empty($Row->applystatus))
            {
                $register = array('label'=>'register',
                                            'url' => '#',
                                            'extra' => "table='maenna_events_data' eventid='$eventid' dataid='$dataid'",
                                            'class'=>'ajaxTrigger button');
            }else{
                $register = array('label'=>'cancal',
                                            'url' => '#',
                                            'extra' => "table='maenna_events_data' eventid='$eventid' dataid='$dataid'",
                                            'class'=>'ajaxTrigger button button_off');
            } 
        }
        
        
        $Tbody[] = array(
                         'event description' => htmlentities(excerpt($Row->description), ENT_QUOTES),
                         'date' => date('m/d/Y h:i', $Row->eventdate),
                         'topic' => $topic,
                         'host' => $Row->host,
                         'status' =>$Row->status,
                         'details' => "<a href='/account?a=information&tab=events&panel=event_detail&eventid=$eventid'>details</a>",
                         'register' => jsbutton_link(array('links'=> array($register )) ),
                         );
    }
    return $Tbody;
}


function event_detail($op = null){
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
    {
        $eventid = sget($_REQUEST, 'eventid');
        if(empty($eventid))$content = '';
        else{
            $sql = "select * from maenna_events where eventid = %d limit 1";
            $result = db_query($sql, array($eventid));
            $Row = db_fetch_array($result);
            if($Row)
            {
                extract($Row);
                $description = nl2br(htmlentities($description, ENT_QUOTES));
                $date = date('m/d/Y h:i', $eventdate);
                $topic = Options_infoTopic($topic, 1);
                $industry = Options_industry($industry, 1);
                
                $sql = "select * from maenna_events_data where data_type='member' and data_value1 = 'register' and eventid = %d";
                $result = db_query($sql, array($eventid));
                $Company = $Pro = array();
                while($Row = db_fetch_object($result))
                {
                    if($Row->data_value2 == '3'){
                        $Company[] = getProjectName($Row->data_attr);
                    }
                    else{
                        $Pro[] = getProId($Row->data_attr);
                    }
                }
                
                $company_users = implode(',', $Company);
                $pro_users = implode(',', $Pro);
                
                $content =<<< END
    <div>
        <div style='font:bold 12px arial'>Group Event Detail</div>
        <table class='report_detail'>
            <tr><td style='width:130px;'>Description</td>   <td>$description</td></tr>
            <tr><td>Event Host:</td><td>$host                  </td></tr>
            <tr><td>Date/Time</td><td>$date                      </td></tr>
            <tr><td>Topic</td><td>$topic                          </td></tr>
            <tr><td>Industry</td><td>$industry                       </td></tr>
            <tr><td>Status:</td><td>$status                        </td></tr>
            <tr><td>Capacity:</td><td>$capacity                      </td></tr>
            <tr><td>Paticipating Companies</td><td>     $company_users</td></tr>
            <tr><td>Paticipating Professionals:</td><td> $pro_users   </td></tr>
        </table>
    </div>
END;
            }
            
        }
        $content .= '<br>' . jsbutton_link(
                                        array('links'=> array
                                            (
                                                array('label'=>'back', 'url'=>'javascript:history.go(-1)'),
                                                //$Applycode    
                                            ),
                                            'position'=>'center'
                                        )
                                  );
        
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}
function project_list_panel($op = null){
    
}
/* EOF */