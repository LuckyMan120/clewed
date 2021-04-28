<?php
/*
function pro_list($op = null)
{
    $Block['title'] = 'Professionals';
    if($op == 'view' || $op == null){
        $content = '';
        
        $content = pro_search();
        
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function pro_search()
{
    $Search_parameter = array();
    $Selection = array();
    $Sectors = _sectors();
    $PRoles = _pro_roles();
    $States = get_us_states();
    $Search_parameter = array(
                                  
    );
    $Search_result  = get_search_result('p');
    $paginationBaseURL = '/account?a=professionals&';
    $block = render_people_search($Search_parameter, $Search_result, $paginationBaseURL);
    return $block;
}

*/


function employment_panel($op = null)
{
    $pid = sget($_REQUEST,'pid');
    $Block['title'] = "Employment";
    global $user;
    $editorid = $user->uid;
    $time = time();
    
    if($op == null || $op == 'view')
    {
        $content = "<div><a href='/account?a=ap&type=people_detail&pid=$pid'>back to detail</a></div><table>";
        if($pid > 0 )
        {
            $sql = "select * from maenna_people_data where pid = %d and data_type = 'employment' order by data_attr desc";
            $result = db_query($sql, array($pid));
            if(db_affected_rows($result) > 0)
            {
                while(($row = db_fetch_object($result)) !== false){
                    $data_attr = $row->data_attr;
                    $org = $row->data_value;
                    $position = $row->data_value2;
                    $start = $row->data_attr;
                    $end = $row->data_value3;
                    //list($start, $end ) = explode('|', $data_attr);
                    //if($start)$start = date('m/y', $start);
                    //if($end)$end = date('m/y', $end) ;//? date('m/y') : "Present";
                    $dataid = $row->dataid;
                    $content .= "\n
                    <tr>
                    <td>$start - $end </td></td>
                    <td>$position</td>
                    <td>$org</td>
                    <td><a href='#' class='edit_wnd' rel='edit_employment_panel' data='$dataid'>edit</a></td>
                    <td><a href='/account?a=ap&type=people_detail&pid=$pid&sub=work&dataid=${dataid}&do=delete&update_section=employment_panel' onclick='return confirm(\"Continue to remove this record?\")'>delete</a></td>
                    </tr>\n";
                }
            }
        }
        
        $content .= "</table>\n";
        //$content .= "<div style='text-align:right;margin-top:-10px;'><a href='/account?a=research&type=people_detail&pid=8&sub=work' style='font:12px arial'>more</a></div>";
        //$content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_work' data='$pid'>new entry</a></div>";
        $Block['body'] = content_box($Block['title'], $content);
        return $Block;
    }elseif($op = 'update')
    {
        //test_array($_REQUEST);exit;
        $do = sget($_REQUEST,'do');
        switch($do)
        {
            case 'update':
                    $start_month = sget($_REQUEST,'start_month');
                    $start_year = sget($_REQUEST,'start_year');
                    $end_month = sget($_REQUEST,'end_month');
                    $end_year = sget($_REQUEST,'end_year');
                    
                    $start = $start_month .'/'. $start_year;
                    $end = $end_month .'/' . $end_year;
                    $company = sget($_REQUEST,'company');
                    $position = sget($_REQUEST,'position');
                    $dataid = sget($_REQUEST, 'dataid');
                    if(empty($dataid)) {
                        drupal_set_message("Failed to update record." . db_error(),'error');
                        return;
                    }
                    $dataid = intval($dataid);
                    $sql = "update maenna_people_data set data_attr = '%s',data_value3 = '%s', data_value = '%s',data_value2 = '%s',access = '%s',editorid  = '%s' where dataid = %d limit 1";
                    $sql_data = array($start,$end, $company, $position, $time, $editorid, $dataid);
                    //test_array($sql_data);exit;
                    if(db_query($sql, $sql_data))
                        drupal_set_message("Reocrd information is updated.");
                    else
                        drupal_set_message("Failed to update record." . db_error(),'error');
                        
                    return;
                break;
            case 'delete':
                    $dataid = sget($_REQUEST, 'dataid');
                    if(empty($dataid)) {
                        drupal_set_message("Failed to update record." . db_error(),'error');
                        return;
                    }
                    $sql = "delete from maenna_people_data where dataid = %d limit 1";
                    $dataid = intval($dataid);
                    $sql_data = array($dataid);
                    if(db_query($sql, $sql_data))
                        drupal_set_message("The selected record is removed."  );
                    else
                        drupal_set_message("Failed to remove record." . db_error(),'error');
                        
                    return;
                break;
        }
        
    }
}

function education_panel($op = null)
{
    $pid = sget($_REQUEST,'pid');
    $Block['title'] = "Education Background";
    global $user;
    $editorid = $user->uid;
    $time = time();
    
    if($op == null || $op == 'view')
    {
        $content = "<div><a href='/account?a=ap&type=people_detail&pid=$pid'>back to detail</a></div><table>";
        if($pid > 0 )
        {
            $sql = "select * from maenna_people_data where pid = %d and data_type = 'education' order by data_attr desc";
            $result = db_query($sql, array($pid));
            if(db_affected_rows($result) > 0)
            {
                while(($row = db_fetch_object($result)) !== false){
                    $sql = "select * from maenna_people_data where pid = %d and data_type = 'education' order by (data_attr * 1) desc";
                    $result = db_query($sql, array($pid));
                    if(db_affected_rows($result) > 0)
                    {
                    
                        while(($row = db_fetch_object($result)) !== false){
                            $year = $row->data_attr;
                            //var_dump($row);exit;
                            //echo "<br>$data_attr";
                            $school = $row->data_value;
                            $study = $row->data_value2;
                            
                            $year = date("Y", $year);
                            $dataid = $row->dataid;
                            $content .= "\n
                            <tr><td >$study</td>
                            <td >$school ($year)</td>
                            <td><a href='#' class='edit_wnd' rel='edit_education_panel' data='$dataid'>edit</a></td>
                            <td><a href='/account?a=ap&type=people_detail&pid=$pid&sub=education&dataid=${dataid}&do=delete&update_section=education_panel' onclick='return confirm(\"Continue to remove this record?\")'>delete</a></td>
                            </tr>\n";
                        }
                    }
                }
            }
        }
        
        $content .= "</table>\n";
        //$content .= "<div style='text-align:right;margin-top:-10px;'><a href='/account?a=research&type=people_detail&pid=8&sub=work' style='font:12px arial'>more</a></div>";
        //$content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_research_people_work' data='$pid'>new entry</a></div>";
        $Block['body'] = content_box($Block['title'], $content);
        return $Block;
    }elseif($op = 'update')
    {
        //test_array($_REQUEST);exit;
        $do = sget($_REQUEST,'do');
        switch($do)
        {
            case 'update':
                
                    $year = sget($_REQUEST,'year');
                    //echo $year;
                    $year = strtotime('1/1/'.$year);
                    //echo " -  " . $year;
                    //echo " .. " .date("Y",$year);
                    $school = sget($_REQUEST,'school');
                    $study = sget($_REQUEST,'study');
                    $dataid = sget($_REQUEST, 'dataid');
                    if(empty($dataid)) {
                        drupal_set_message("Failed to update record." . db_error(),'error');
                        return;
                    }
                    $dataid = intval($dataid);
                    $sql = "update maenna_people_data set data_attr = '%s',data_value2 = '%s', data_value = '%s',access = '%s',editorid  = '%s' where dataid = %d limit 1";
                    $sql_data = array($year,$study, $school,  $time, $editorid,$dataid);
                    //test_array($sql_data);exit;
                    if(db_query($sql, $sql_data))
                        drupal_set_message("Reocrd information is updated.");
                    else
                        drupal_set_message("Failed to update record." . db_error(),'error');
                        
                    return;
                break;
            case 'delete':
                    $dataid = sget($_REQUEST, 'dataid');
                    if(empty($dataid)) {
                        drupal_set_message("Failed to update record." . db_error(),'error');
                        return;
                    }
                    $sql = "delete from maenna_people_data where dataid = %d limit 1";
                    $dataid = intval($dataid);
                    $sql_data = array($dataid);
                    if(db_query($sql, $sql_data))
                        drupal_set_message("The selected record is removed."  );
                    else
                        drupal_set_message("Failed to remove record." . db_error(),'error');
                        
                    return;
                break;
        }
        
    }
}
/* EOF */