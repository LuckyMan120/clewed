<?php
defined("__ACCOUNT__") or die("no direct access allowed");
global $maenna_page;
$maenna_page = array();
$maenna_page['left'] = array();
$maenna_page['content'] = array('working_on',);
$maenna_page['right']= array('invite_companies', 'invite_people', 'user_management', 'my_statistics', 'my_referrals', 'my_research','my_schedule');
$maenna_page['Tabs'] = array(
                             'Companies' => '/account?a=ac',
                             'Professionals' => '/account?a=ap',
                             'Investors' => '/account?a=ai',
                             'Research' => '/account?a=research&type=company',);
$maenna_page['tab'] = "Companies";

function working_on($op = null)
{
    global $maenna_page;
    $Block['title'] = 'Working on';
    if($op == 'view'){
        $content = "DSA";
        $Block['body'] = content_box($Block['title'], $content);
    }else{
        $Block['body'] = "DdSADSA";
        $maenna_page['right'] = array("invite_companies");
    }
    
    return $Block;
}

function user_management($op = '')
{
    global $maenna_page;
    $Block['title'] = 'User Management'; 
    if($op == 'view' || $op == ''){
        $Block['body']  =   l('User Management', 'account',array('query'=>'a=user_management&o=list&t=c')) . "<br>";
    }
    elseif($op == 'list'){
        $maenna_page['right']= array('invite_companies', 'invite_people', 'top_ranked', 'contributions', );
        $Block['body'] = _get_user_table(array('caller_type'=>'admin'));
    }
    return $Block;
}

function top_ranked($op = '')
{
    global $maenna_page;
    $Block['title'] = 'Top Ranked';
    if($op == 'view' || $op == '')  $Block['body'] =  sidebar_box($Block['title'], "top ranked");
    
    return $Block;
}
function contributions($op = '')
{
    global $maenna_page;
    $Block['title'] = 'Contributions';
    if($op == 'view' || $op == '')  $Block['body'] =  sidebar_box($Block['title'], "contribution");
    
    return $Block;
}
/* EOF */