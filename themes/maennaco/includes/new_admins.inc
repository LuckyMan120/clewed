<?php

// center 
function admin_list($op = null)
{
    $Block['title'] = 'Admin Users';
    if($op){
        $content = '';
        $content .= admin_search();
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function admin_search()
{
    $Search_parameter = array();
    $Selection = array();
    $Sectors = _sectors();
    $PRoles = _pro_roles();
    $States = get_us_states();
    $Search_parameter = array(
                                  
    );
    $Search_result  = get_search_result('admin');
    $paginationBaseURL = '/account?tab=admins&';
    $block = render_people_search($Search_parameter, $Search_result, $paginationBaseURL);
    return $block;
}
function remove_admins($op = null)
{
    if(isset($_POST['uids']) && (count($_POST['uids'] > 0)))
    {
        $sql_where = implode(',', $_POST['uids']);
        $SQL = "update users set status = 0 where uid in ($sql_where)";
        if(db_query($SQL) !== false)
        {
          drupal_set_message( t("Admin user records updated."));
          
        }
        else{
           drupal_set_message( t("Failed to update user record."),'error');
        }
    }
}
// right side bar



/* EOF */