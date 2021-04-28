<?php

function account_permissions($op = null)
{
    global $AccessObj;
    $Block['title'] = "Permission Settings";
   
    $content = '';
    $section = __FUNCTION__;
    
    if($AccessObj->user_type == 'super')
        $Block['title'] .= "<div class=editbtn><a href='/account?tab=settings&section=com_permissions&panel=com_permissions' class=tool>Edit</a></div>";
    elseif($AccessObj->user_type == 'company')
    {
        $id = $AccessObj->uid;
        $Block['title'] .= "<div class=editbtn><a href='/account?tab=settings&section=ccom_permissions&panel=ccom_permissions&id=$id' class=tool>Edit</a></div>";
    }elseif($AccessObj->user_type == 'admin')
    {
        $id = sget($_REQUEST, 'id');
        $Block['title'] .= "<div class=editbtn><a href='/account?tab=settings&section=ccom_permissions&panel=ccom_permissions&id=$id' class=tool>Edit</a></div>";
    }elseif($AccessObj->user_type == 'people')
    {
        return '';
    }

    if($op)
    {
        $redirect = rebuild_url(array('tab','page','section','panel'));
        $section = sget($_REQUEST, 'section');
        if($section == 'com_permissions' && $AccessObj->user_type == 'super')
        {
            $content .= "<ul>
                <li><a " . ($_REQUEST['type']=='cu'?"style='color:#00a2bf;'":'') . " href='$redirect&type=cu'>For Company Users</a></li>
                <li><a " . ($_REQUEST['type']=='pu'?"style='color:#00a2bf;'":'') . " href='$redirect&type=pu'>For Professional Users</a></li>
                <li><a " . ($_REQUEST['type']=='au'?"style='color:#00a2bf;'":'') . " href='$redirect&type=au'>For Admin Users</a></li>
            </ul>";
        }
    }
    
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function account_settings($op = null)
{
    $Block['title'] = "Account Settings";
   
    $content = '';
    $section = __FUNCTION__;
    $Block['title'] .= "<div class=editbtn><a href='/account?tab=settings&section=account&panel=account' class=tool>Edit</a></div>";
    
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

/* EOF */