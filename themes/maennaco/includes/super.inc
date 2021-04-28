<?php
defined("__ACCOUNT__") or die("no direct access allowed");
include ("." . base_path() . path_to_theme() . "/includes/admin.inc");
global $maenna_page;
$maenna_page = array();
$maenna_page['left'] = array();
$maenna_page['content'] = array('working_on',);
$maenna_page['right']= array('invite_companies', 'invite_people', 'user_management', 'my_statistics', 'my_referrals', 'my_research','my_schedule');
$maenna_page['Tabs'] = array('Messages' => '/account?a=messages',
                             'Admins' => '/account?a=admins',
                             'Companies' => '/account?a=companies',
                             'Professionals' => '/account?a=professionals',
                             'Investors' => '/account?a=investors',
                             'Research' => '/account?a=research&type=company',
                             'Information' => '/account?a=information',
                             );

$a = sget($_REQUEST, 'a');
if(isset($a) && $a == 'information') 
{
    include ("." . base_path() . path_to_theme() . "/includes/information_rightbar.inc");
    mybreadcrumb(2, "Information", "/account?a=information");
    $maenna_page['tab']= 'Information';
    $maenna_page['left'] = array();

    $pageTab = sget($_REQUEST, 'tab');
    if(empty($pageTab))$pageTab = 'insights';
    $title = ucwords($pageTab);

    $target = sget($_REQUEST, 'target');
    if(empty($target))$target = 'company';
    $panel = sget($_REQUEST, 'panel');
    mybreadcrumb(3, $title, "/account?a=information&tab=$pageTab&panel=$panel&target=$target");
    
    include ("." . base_path() . path_to_theme() . "/includes/${pageTab}_main.inc");
    include ("." . base_path() . path_to_theme() . "/includes/${pageTab}_rightbar.inc");
    $panel = sget($_REQUEST, 'panel');
    
    if($panel && function_exists($panel)) $maenna_page['content'] = array('infoTabs', $panel);
    
}elseif(isset($a) && $a == 'companies') //super company
{
    $type = sget($_REQUEST,'type');
    $v = sget($_REQUEST,'v');
    
    $maenna_page['tab']= 'companies';
    mybreadcrumb(2, "Companies", "/account?a=companies");
    if(empty($type))
    {
        include ("." . base_path() . path_to_theme() . "/includes/super_companies.inc");
        include ("." . base_path() . path_to_theme() . "/includes/superadmin_comsum_sidebar.inc");
        $maenna_page['left'] = array();
        
    }
    elseif($type == 'company_detail')
    {
        $companyid = sget($_REQUEST, 'companyid');
        if(empty($companyid)) return "Invalid company id";
        mybreadcrumb(3, "Detail", "account?a=companies&type=company_detail&companyid=$companyid");
        $maenna_page['section_tab'] ='company_section_tabs';
        
        include ("." . base_path() . path_to_theme() . "/includes/shared_functions.inc");
        include ("." . base_path() . path_to_theme() . "/includes/super_comdetail_leftbar.inc");
        include ("." . base_path() . path_to_theme() . "/includes/super_comdetail_rightbar.inc");
        include ("." . base_path() . path_to_theme() . "/includes/super_comdetail.inc");
    }elseif($type == 'diligence'){
        $companyid = sget($_REQUEST, 'companyid');
        
        mybreadcrumb(3, "Diligence", "account?a=companies&type=diligence&companyid=$companyid");
        include ("." . base_path() . path_to_theme() . "/includes/superadmin_comdiligence.inc");
        $panel = sget($_REQUEST, 'panel');
        if($panel && function_exists($panel)) $maenna_page['content'] = array($panel);
    }
    $panel = sget($_REQUEST, 'panel');
    if($panel && function_exists($panel)) $maenna_page['content'] = array($panel);
}
elseif(isset($a) && $a == 'professionals') // super pro
{
    include ("." . base_path() . path_to_theme() . "/includes/super_pro.inc");
    include ("." . base_path() . path_to_theme() . "/includes/super_proSumSidebar.inc");
    include ("." . base_path() . path_to_theme() . "/includes/shared_functions_people.inc");
    mybreadcrumb(2, "Professionals", "/account?a=professionals");
    $maenna_page['left'] = array();
    $maenna_page['content'] = array('pro_list');
    $maenna_page['right']= array('maenna_recently_added_people',
                                 'maenna_recently_updated_people',
                                 'pro_types',
                                 'pro_memberstats',
                                 'mlvl_stats',
                                 'sector_stats',
                                 'expertis_stats');
    $maenna_page['tab']= 'professionals';
    
    $type = sget($_REQUEST,'type');
    if($type == "people_detail")
    {
        $pid = sget($_REQUEST,'pid');
        if(! empty($pid))
        {
            include ("." . base_path() . path_to_theme() . "/includes/super_proDetail.inc");
            include ("." . base_path() . path_to_theme() . "/includes/super_proDetail_rightbar.inc");
            include ("." . base_path() . path_to_theme() . "/includes/super_proDetail_leftbar.inc");
            //$maenna_page['tab']= 'Research';
            $pid = sget($_REQUEST,'pid');
            mybreadcrumb(3, "Detail", "account?a=professionals&type=people_detail&pid=$pid");
            
            $panel = sget($_REQUEST, 'panel');
            if($panel && function_exists($panel)) $maenna_page['content'] = array($panel);
            
        }
    }
    
}

function company_super_section()
{
    
}

/* EOF */