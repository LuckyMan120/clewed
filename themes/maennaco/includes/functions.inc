<?php
defined("__ACCOUNT__") or die("no direct access allowed");

function company_tabs($tab)
{
    $Tabs = array( "Activity" =>'/account', "People" => '/account?a=people', "Information" => '/account?a=information');
    echo "<div class='company_tabs'>";
    foreach($Tabs as $title => $link)
    {
        $class='';
        if( $tab == $title)
        {
             $class = 'active';
        }
        
        echo "<a href='$link' class='$class'>$title</a> ";
    }
    echo "</div><br style='clear:both'>";
}
function account_tabs($TABS)
{
    $current_tab = sget($_REQUEST, 'tab');//$maenna_page['tab'];
    if(empty($current_tab)) $current_tab = 'activities';
    echo "<div class='account_tabs'>";
    foreach($TABS as $tab_name)
    {

        $title = strtoupper($tab_name);
        $class='';
        if( strcasecmp($tab_name, $current_tab) == 0)
        {
             $class = 'active';
        }
        echo "<a href='/account?tab=$tab_name' class='$class'>$title</a> ";
    }
    echo "</div>";

}
function mybreadcrumb($level = null, $title = null, $link = null)
{
    if( ! isset($_SESSION['trail']))
    {
        $_SESSION['trail'] = array();
    }
    //display
    if($level === null ){
        
        echo "<div class='trail'>";
        foreach($_SESSION['trail'] as $k => $d)
        {
            if($k > 1 ) echo " &raquo; ";
            $title = $d[0];
            $link = $d[1];
            $class='';
            if(count($_SESSION['trail']) == $k )$class ="class='active'";
            echo "<a href='$link' $class>$title</a> ";
        }
        echo "</div>";
    }else{ // set trail
        $_SESSION['trail'][$level] = array($title,$link);
        foreach($_SESSION['trail'] as $i => $d)
        {
            if($i > $level){
                $_SESSION['trail'][$i] = NULL;
                unset($_SESSION['trail'][$i]);
            }
        }
    }
}

function theme_page($f=null)
{
    global $maenna_page;
    global $user;
    global $user_type;

    if((isset($f) && $f == 'user_management'))
    {
        mybreadcrumb(3, "User Account Management", "/account?a=user_management&o=list&t=c");
        $maenna_page['content'] = array('user_management');
    }elseif((isset($f) && $f == 'company_detail')) // company detail
    {
        include ("." . base_path() . path_to_theme() . "/includes/shared_functions.inc");
        mybreadcrumb(3, "User Account Management", "/account?a=user_management&o=list&t=c");
        mybreadcrumb(4, "Company Detail", "");
        $maenna_page['left'] = array('view_company_logo', 'view_company_brief_info', 'view_company_management', 'view_company_market', 'view_company_milestone');
        $maenna_page['content'] = array('view_company_name', 'view_company_financial', 'view_company_market_info','view_company_analyst');
        $maenna_page['right']= array('view_company_follower', 'view_related_companies', 'view_company_priorities', 'view_company_resources', 'view_company_events', 'view_company_statistics');
    }elseif(isset($f) && $f == 'cpro') // company people
    {
        include ("." . base_path() . path_to_theme() . "/includes/company_tab_people.inc");
        mybreadcrumb(2, "People", "/account?a=cpro");
        $maenna_page['left'] = array();
        $maenna_page['content'] = array('search_people');
        $maenna_page['right']= array('invite_people','top_ranked_people', 'contributions');
        $maenna_page['tab']= 'people';
    }elseif(isset($f) && $f == 'cinfo') // company info
    {
        include ("." . base_path() . path_to_theme() . "/includes/company_tab_information.inc");
        mybreadcrumb(2, "Information", "/account?a=information");
        $maenna_page['left'] = array();
        $maenna_page['content'] = array('company_transcript', 'company_article_researches', 'company_media');
        $maenna_page['right']= array('submit_information', 'industry_topics', 'management_topics');
        $maenna_page['tab']= 'information';
    }elseif(isset($f) && $f == 'admins') // super admin
    {
        include ("." . base_path() . path_to_theme() . "/includes/super_admins.inc");
        mybreadcrumb(2, "Admins", "/account?a=admins");
        $maenna_page['left'] = array();
        $maenna_page['content'] = array('admin_list');
        $maenna_page['right']= array('add_admin_user');
        $maenna_page['tab']= 'admins';
        
        $type = sget($_REQUEST,'type');
        $id = sget($_REQUEST,'id');
        if(($type == 'detail') && $id)
        {
            include ("." . base_path() . path_to_theme() . "/includes/admin_shared_functions.inc");
            mybreadcrumb(3, "Detail", "/account?a=admins&type=detail&id=$id" );
            $maenna_page['content'] = array('admin_user_info');
            $maenna_page['right']= array('admin_update_info');
        }
        
    }elseif(isset($f) && $f == 'investors') //super investors
    {
        include ("." . base_path() . path_to_theme() . "/includes/super_investors.inc");
        mybreadcrumb(2, "Investors", "/account?a=investors");
        $maenna_page['left'] = array();
        $maenna_page['content'] = array('investor_list');
        $maenna_page['right']= array();
        $maenna_page['tab']= 'investors';
    }elseif(isset($f) && $f == 'ac') //admin company
    {
        include ("." . base_path() . path_to_theme() . "/includes/admin_companies.inc");
        mybreadcrumb(2, "Companies", "/account?a=ac");
        $maenna_page['left'] = array();
        $maenna_page['content'] = array('company_list');
        $maenna_page['right']= array( 'key_statistics',  'recently_added', 'recently_updated', 'priority_stats', 'followup_stats','sector_stats');

        $v = sget($_REQUEST,'v');
        $type = sget($_REQUEST,'type');
        if($type == 'company_detail')
        {
            $companyid = sget($_REQUEST, 'companyid');
            include ("." . base_path() . path_to_theme() . "/includes/shared_functions.inc");
            $maenna_page['left'] = array('maenna_company_logo','maenna_company_brief_info','maenna_company_management','maenna_company_market_info','maenna_company_links');
            $maenna_page['content'] = array('maenna_company_name','maenna_company_financials','maenna_progress_status');
            $maenna_page['right']= array('maenna_company_followers','maenna_links_group',  'maenna_followup_priority','maenna_companies_to_follow','maenna_company_articles');
            $maenna_page['tab']= 'Research';
            mybreadcrumb(3, "Company", "account?a=ac&type=company");
            mybreadcrumb(4, "Detail", "account?a=ac&type=company_detail&companyid=$companyid");
        }
        /*
        include ("." . base_path() . path_to_theme() . "/includes/admin_companies.inc");
        mybreadcrumb(2, "Companies", "/account?a=ac");
        $maenna_page['left'] = array('');
        $maenna_page['content'] = array('');
        $maenna_page['right']= array('');
        $maenna_page['tab']= 'Companies';
        */
    }elseif(isset($f) && $f == 'ap') // admin pro
    {
        include ("." . base_path() . path_to_theme() . "/includes/admin_professionals.inc");
        include ("." . base_path() . path_to_theme() . "/includes/professionals.inc");
        include ("." . base_path() . path_to_theme() . "/includes/shared_functions_people.inc");
        mybreadcrumb(2, "Professionals", "/account?a=ap");

        $maenna_page['left'] = array();
        $maenna_page['content'] = array('pro_list');
        $maenna_page['right']= array('maenna_recently_added_people', 'maenna_recently_updated_people', 'maenna_priority_stats_people', 'maenna_followup_stats_people');
        $maenna_page['tab']= 'professionals';
        
        $type = sget($_REQUEST,'type');
        if($type == "people_detail")
        {
            $pid = sget($_REQUEST,'pid');
            if(! empty($pid))
            {
                $maenna_page['left'] = array('maenna_people_contact','maenna_people_work','maenna_people_education','maenna_people_links');
                $maenna_page['content'] = array('maenna_people_info','maenna_accomplishments', 'maenna_people_status');
                $maenna_page['right']= array( 'maenna_people_followup','maenna_people_articles');
                //$maenna_page['tab']= 'Research';
                $pid = sget($_REQUEST,'pid');
                mybreadcrumb(3, "Detail", "account?a=ap&type=people_detail&pid=$pid");
                
                $sub = sget($_REQUEST,'sub');
                if($sub == 'work')
                {
                    mybreadcrumb(4, "Employment", "account?a=ap&type=people_detail&pid=$pid&sub=work");
                    $maenna_page['content'] = array("employment_panel");
                }elseif($sub == 'education')
                {
                    mybreadcrumb(4, "Education", "account?a=ap&type=people_detail&pid=$pid&sub=education");
                    $maenna_page['content'] = array("education_panel");
                }
            }
        }
    }elseif(isset($f) && $f == 'ai') // admin information
    {
        include ("." . base_path() . path_to_theme() . "/includes/admin_investors.inc");
        mybreadcrumb(2, "Investors", "/account?a=ai");
        $maenna_page['left'] = array('');
        $maenna_page['content'] = array('');
        $maenna_page['right']= array('');
        $maenna_page['tab']= 'Investors';
    }elseif(isset($f) && ($f == 'research' || $f == 'ar'))
    {
        include ("." . base_path() . path_to_theme() . "/includes/super_research.inc");
        mybreadcrumb(2, "Research", "/account?a=research&type=company");
        $maenna_page['left'] = array();
        $maenna_page['content'] = array('research_table');
        $maenna_page['right']= array( 'add_research_company','research_manager', 'key_statistics',  'recently_added', 'recently_updated', 'leads_by_sector','priority_stats', 'followup_stats','sector_stats');
        
        if(array_key_exists(6, $user->roles))
        {
            $user_type = 'admin';
            //include ("." . base_path() . path_to_theme() . "/includes/admin_research.inc");
            mybreadcrumb(2, "Research", "/account?a=ar&type=company");
            $maenna_page['right']= array('add_research_company', 'invite_collaborator',  'recently_added', 'recently_updated', 'priority_stats', 'followup_stats','sector_stats');
            $maenna_page['tab']= 'Investors';
        }
        $maenna_page['tab']= 'Research';
        $type = sget($_REQUEST,'type');
        if( $type == 'company')
        {
            mybreadcrumb(3, "Company", "account?a=research&type=company");
            include ("." . base_path() . path_to_theme() . "/includes/research_company.inc");
        }elseif($type == 'people') {
            mybreadcrumb(3, "People", "account?a=research&type=people");
            include ("." . base_path() . path_to_theme() . "/includes/research_people.inc");
            $maenna_page['right']= array('add_research_people', 'recently_added_people', 'recently_updated_people', 'priority_stats_people', 'followup_stats_people');
        }elseif($type == 'company_detail')
        {
            include ("." . base_path() . path_to_theme() . "/includes/research_company_detail.inc");
            $maenna_page['left'] = array('research_company_logo','research_company_brief_info','research_company_management','research_company_market_info','research_company_links');
            $maenna_page['content'] = array('research_company_name','research_company_financials','research_progress_status');
            $maenna_page['right']= array('research_company_followers','research_links_group', 'research_company_prospects', 'research_company_qualified_lead','research_followup_priority','research_companies_to_follow','research_company_articles');
            $maenna_page['tab']= 'Research';
            mybreadcrumb(3, "Company", "account?a=research&type=company");
            $recordid = sget($_REQUEST,'recordid');
            mybreadcrumb(4, "Detail", "account?a=research&type=company_detail&recordid=$recordid");
        }elseif($type == 'people_detail')
        {
            include ("." . base_path() . path_to_theme() . "/includes/research_people_detail.inc");
            $maenna_page['left'] = array('research_people_contact','research_people_work','research_people_education','research_people_links');
            $maenna_page['content'] = array('research_people_info','research_accomplishments', 'research_people_status');
            $maenna_page['right']= array( 'add_research_people', 'research_people_prospects', 'research_people_followup','research_people_articles');
            $maenna_page['tab']= 'Research';
            mybreadcrumb(3, "People", "account?a=research&type=people");
            $pid = sget($_REQUEST,'pid');
            mybreadcrumb(4, "Detail", "account?a=research&type=people_detail&pid=$pid");
            
            $sub = sget($_REQUEST,'sub');
            if($sub == 'work')
            {
                mybreadcrumb(5, "Employment", "account?a=research&type=people_detail&pid=$pid&sub=work");
                $maenna_page['content'] = array("employment_panel");
            }elseif($sub == 'education')
            {
                mybreadcrumb(5, "Education", "account?a=research&type=people_detail&pid=$pid&sub=education");
                $maenna_page['content'] = array("education_panel");
            }
        }
    }elseif(isset($f) && $f == 'messages'){
        mybreadcrumb(2, "Messages", "/account?a=messages");
        $maenna_page['tab']= 'Messages';
        
    }
    
}


function infoTabs($Tabs , $tab = ''){
    $Block['title']  = 'Information';
    $content = '';
    $pageTab = sget($_REQUEST, 'tab');
    
    $Tabs = array('insights', 'postings', 'events','questionnaire');
    for($i = 0; $i < count($Tabs); $i++){
        $active = ''; $class = '';
        if($Tabs[$i] == $pageTab)$active = ' active-trail active';
        if($i == 0)$class = 'first';
        elseif($i == (count($Tabs) - 1))$class = 'last';
        $title = $Tabs[$i];
       /// var_dump(($pageTab == 'questionnaire'));
        if($Tabs[$i] == 'questionnaire') 
            $content .= "\n<li class=' $class $active '><a href='/account?a=information&tab=".$Tabs[$i]."&panel=questionnaire&target=company' class='$active'>".strtoupper($title)."</a></li>";
        else
            $content .= "\n<li class=' $class $active '><a href='/account?a=information&tab=".$Tabs[$i]."' class='$active'>".strtoupper($title)."</a></li>";
    }
    $content = <<< END
    <div  class='about-menu-div' style='margin-top:15px;'>
        <ul class="top-nav-menu-item">
            $content
        </ul>
    </div>
END;
    $Block['body'] = content_box($Block['title'], $content);
return $Block;
}


function function_allowed($function_name)
{
    global $maenna_page;

    foreach($maenna_page as $section)
    {
        if(in_array($function_name, $section)) return true;
    }
    return false;
}
function request_a_professional($op = '')
{
    $Block['title'] =  'Request a professional'; 
    if($op == 'view' || $op == '') $Block['body']  =  "<div class='pagelink'>" . l('Request a professional', 'account',array('query'=>'a=people')) . "</div>";
    
    return $Block;
}
function invite_companies($op = '')
{
    $Block['title'] =  'Invite Companies'; 
    if($op == 'view' || $op == '')  $Block['body']  =  "<div class='pagelink'>" . l('Invite companies', 'account',array('query'=>'a=invite_companies')) . "</div>";
    return $Block;
}
function invite_people($op = '')
{
    $Block['title'] =  'Invite People'; 
    if($op == 'view' || $op == '') $Block['body']  =   "<div class='pagelink'>" . l('Invite People', 'account',array('query'=>'a=invite_people')) . "</div><br>";
    return $Block;
}


function my_referrals($op = '')
{
    $Block['title'] = "My Referrals";
    if($op == 'view' || $op == '')  $Block['body'] =  sidebar_box($Block['title'], "DSADSA");
    
    return $Block;
}
function my_research($op = '')
{
    $Block['title'] = "My Research";
    if($op == 'view' || $op == '')  $Block['body'] =  sidebar_box($Block['title'], "DSADSA");
    
    return $Block;
}

function my_schedule($op = '')
{
    $Block['title'] = "My Schedule";
    if($op == 'view' || $op == '')  $Block['body'] =  sidebar_box($Block['title'], "DSADSA");
    
    return $Block;
}


function sidebar_box($title, $content)
{
    
    $html = "<div class='sidebar_box'><div class='wrapper'>";
    if($title) $html.= "<div class='box_title'>$title</div>";
    $html.= "<div class='box_content'>$content</div></div></div>";
    return $html;
}

function content_box($title, $content, $attr = ''){
    $html = "<div class='content_box $attr'>";
    if($title) $html.= "<div class='box_title'>$title</div>";
    $html.= "<div class='box_content'>$content</div></div>";
    return $html;
}

function sortFinancials($mysqlResource)
{
    $Output_array = array();
    while(($Row = db_fetch_array($mysqlResource)))
    {
        $type = sget($Row, 'data_type');
        $year = sget($Row, 'data_attr');
        $value = sget($Row, 'data_value');
        if(  ! isset($Output_array["$year"]))
        {
            $Output_array["$year"] = array('revenue' => '',
                                                                             'earning' => '',
                                                                             'year' => '');
            $Output_array["$year"]["year"] = $year;
        }
        $Output_array["$year"]["$type"] = $value;
    }
    ksort($Output_array);
    return $Output_array;
}

function _get_user_table($Array)
{
    $block = '';
    $Data = array();
    $caller_type = $Array['caller_type'];
    if($caller_type == 'admin'){
        $c = (sget($_REQUEST,'t') == 'c') ? "active" :'';
        $p = (sget($_REQUEST,'t') == 'p') ? "active" :'';
        $i = (sget($_REQUEST,'t') == 'i') ? "active" :'';
        $block .= "<div class='button-links'>" .  l('Companies', 'account',array('query'=>'a=user_management&o=list&t=c', 'attributes'=>array('class'=>$c) )) .
                             l('Professionals', 'account',array('query'=>'a=user_management&o=list&t=p', 'attributes'=>array('class'=>$p) )) .
                              l('Investors', 'account',array('query'=>'a=user_management&o=list&t=i', 'attributes'=>array('class'=>$i) )) . "</div>";
        
    }elseif($caller_type == 'professional')
    {
        //$block .= "<div class='button-links'>" .  l('Companies', 'account',array('query'=>'a=user_management&o=list&t=c', )) . "</div>";
    }elseif($caller_type == 'company')
    {
        //$block .= "<div class='button-links'>" .  l('Professionals', 'account',array('query'=>'a=user_management&o=list&t=p',  )) . "</div>";
    }elseif($caller_type == 'investor')
    {
        //$block .= "<div class='button-links'>" . l('Investors', 'account',array('query'=>'a=user_management&o=list&t=i',  )) . "</div>";
    }
    // **************************
    
    $block .= "<div style='clear:both'></div>";
    $search_account_type = sget($_REQUEST,'t');
    $block .= "<form action='' method='get'><div class='filter_bar'><table border=0 cellspacing=0 cellpadding=0 width='100%'><tr><!--td style='vertical-align:bottom;width:100px;'> <label>BROWSE BY:</label></td-->";
    $sectors = _sectors();
    $states = get_us_states();
    $education = _education_degree();
    $revenue = _revenue();
    if($search_account_type == 'p'){
        $interests = _pro_interested();
        $p_roles = _pro_roles(); 
        $search_parameter = array(
                                    'user_type' => $p_roles,
                                    'sectors' => $sectors,
                                    'interested_in'=> $interests,
                                    'location' => $states,
                                    'education' => $education);
    }
    elseif( $search_account_type == 'c' ){
        $interests = _company_interest();
        $search_parameter = array('sectors' => $sectors,
                                  'interested_in'=> $interests,
                                  'revenue' => $revenue,
                                  'location' => $states,);
    }
    elseif( $search_account_type == 'i' ){
        $interests = _pro_interested();
        $search_parameter = array('investor_type' => $sectors,
                                  'interested_in'=> $interests,
                                  'location' => $states,);
    }
    $block .= "\n<td>";
    foreach($search_parameter as $key => $value)
    {
        $selection = sget($_REQUEST, $key);
        
        $block .=     "<div style='float:left;padding-right:4px;'><label class='italic e-text grey'>" . ucwords($key) . ":</label><br> ";
        $counter = 1;
        if(is_array($value))
        {
            $block .= "<select class='search_select' name='$key'>";
            
            foreach($value as $k => $v)
            {
                $v = trim($v);
                $selected = ( $k == $selection) ? ' selected ' : '';
                
                if($counter == 1 && ( ! empty($v)))
                {
                    $block .= "<option $selected> </option>";
                    
                }
                $counter = 2;
                $block .= "<option value='$k' $selected>" .$v . "</option>";
            }
            $block .= '</select></div>';
        }
        
    }
    $block .= "<div style='float:left;padding-right:20px;'><br><input type=image value=filter src='/themes/maennaco/images/filter.png' style='display:block'></div>";
    $block .= "</td>";
    $block .= "</tr></table></div>";
    $block .= "<input type=hidden name=t value='".sget($_REQUEST, 't') . "'>";
    $block .= "<input type=hidden name=a value='user_management'>";
    $block .= "<input type=hidden name=o value='list'>";
    $block .="</form>";
    // **************************
    $block .= get_users_table($search_account_type);
    
    
    
    
    return $block;
}

function get_users_table($user_type = '')
{
    if($user_type == '') return "";
    $limit = 8;
    $page = sget($_REQUEST,'page') ? (int)(sget($_REQUEST, 'page')) : 1;
    $start = ($page - 1) * $limit;
    //test_array($_REQUEST);
    // role id: company:3, investor: 8, admin:6 , analyst+executives+talents: 4,5,7
    // company table: project name, description, sector, revenue, earnings, Detail, # of analysts, rank, follow.-
    $SQL_where_filter = "";
    $SQL_data = array();
    if($user_type == 'c') {
        $rid = array(3);
        if(sget($_REQUEST, 'sectors')){
            $SQL_where_filter .= " and ta.sector = '%d'" ;
            $SQL_data[] = sget($_REQUEST, 'sectors');
        }
        if(sget($_REQUEST, 'interested_in')){
            $SQL_where_filter .= " and ta.interest like '%%%s%%'" ;
            $SQL_data[] = sget($_REQUEST, 'interested_in');
        }
        /* if(sget($_REQUEST, 'revenue')){
            $SQL_where_filter .= " and ta.revenue = '%d'" ;
            $SQL_data[] = sget($_REQUEST, 'revenue');
        }*/
        if(sget($_REQUEST, 'location')){
            $SQL_where_filter .= " and ta.state like '%%%s%%'" ;
            $SQL_data[] = sget($_REQUEST, 'location');
        }
    }
    elseif($user_type == 'i') {
        $rid = array(8);
        if(sget($_REQUEST, 'investor_type')){
            $SQL_where_filter .= " and ta.investor_type  = %d" ;
            $SQL_data[] = sget($_REQUEST, 'investor_type');
        }
        if(sget($_REQUEST, 'interested_in')){
            $SQL_where_filter .= " and ta.interest like '%%%s%%'" ;
            $SQL_data[] = sget($_REQUEST, 'interested_in');
        }
        if(sget($_REQUEST, 'location')){
            $SQL_where_filter .= " and ta.state = '%s'" ;
            $SQL_data[] = sget($_REQUEST, 'location');
        }
    }
    elseif($user_type == 'p'){
        $rid = array(4,5,7);
        $type = sget($_REQUEST,'user_type');
        if($type > 0) $rid = array($type);
        if(sget($_REQUEST, 'sectors')){
            $SQL_where_filter .= " and ta.sector = '%d'" ;
            $SQL_data[] = sget($_REQUEST, 'sectors');
        }
        if(sget($_REQUEST, 'interested_in')){
            $SQL_where_filter .= " and ta.interest like '%%%s%%'" ;
            $SQL_data[] = sget($_REQUEST, 'interested_in');
        }
        if(sget($_REQUEST, 'location')){
            $SQL_where_filter .= " and ta.state = '%s'" ;
            $SQL_data[] = sget($_REQUEST, 'location');
        }
        if(sget($_REQUEST, 'education')){
            $SQL_where_filter .= " and ta.education = '%d'" ;
            $SQL_data[] = sget($_REQUEST, 'education');
        }
    }else{
        return "User type is not defined.";
    }
    $SQL_begin = " select * from users_extend as ta, users_roles as tb  ";
    $SQL_where .= "where ta.uid = tb.uid and tb.rid in (". implode(',', $rid ) . ") ";
    $SQL_limit = " limit $start, $limit";
    
   
    
    $SQL_begin_total = " select count(*) as total from users_extend as ta, users_roles as tb  ";
    $Result = db_query($SQL_begin_total . $SQL_where . $SQL_where_filter , $SQL_data);
    $row = db_fetch_object($Result);
    $total =  $row->total;
    
    $SQL = $SQL_begin . $SQL_where . $SQL_where_filter .$SQL_limit;
    $Result = db_query($SQL, $SQL_data );
    
    $Sectors = _sectors();
    $Revenue = _revenue();

    $table = '';
    
    $Pagenation = array('total' => $total,
                        'limit' => $limit,
                        'baseurl' => '/account?',
                        'num_of_links' => 8,
                        );
    
    
    if($user_type == 'c') {
        $table .= '<table class="table-list"><thead>  <th width="15%">Project Name</th>
                                    <th>Description</th>
                                    <th>Sector</th>
                                    <th>Revenue</th>
                                    <th>Earnings<br>Growth</th>
                                    <th>Company<br> Detail</th>
                                    <th>Analysts</th>
                                    <th>Rank Company</th>
                                    <th>Follow</th></thead><tbody>';
        $count = 1;
        while($row = db_fetch_object($Result))
        {
            
            $uid = $row->uid;
            
            if($count % 2) $table .= "\n<tr class='odd'>";
            else $table .= "\n<tr class='even'>";
    
            $table .=  "<td>".ucwords($row->company_name) ."</td>
                        <td style='width:25%;'><div style='width:96%;height:40px;overflow:hidden'>".$row->description."</div></td>
                        <td>".$Sectors["{$row->sector}"]."</td>
                        <td>".$Revenue["{$row->revenue}"]."</td>
                        <td></td>
                        <td>".l('view', 'account',array('query'=>'a=company_detail&id=' . $uid) ) ."</td>
                        <td></td>
                        <td></td>
                        <td></td></tr>";
                        
            
            $table .= '</tr>';
            $count++;
        }
        $table .= '</tbody></table>';
    }elseif($user_type == 'i') {
        $table .= '<table class="table-list"><thead>  <th width="15%">Company Name</th>
                                    <th>Description</th>
                                    <th>Sector</th>
                                    <th>Revenue</th>
                                    <th>Earnings<br>Growth</th>
                                    <th>Company<br> Detail</th>
                                    <th>Analysts</th>
                                    <th>Rank Company</th>
                                    <th>Follow</th></thead><tbody>';
        $count = 1;
        while($row = db_fetch_object($Result))
        {
            
            $uid = $row->uid;
            
            if($count % 2) $table .= "\n<tr class='odd'>";
            else $table .= "\n<tr class='even'>";
    
            $table .=  "<td >".ucwords($row->company_name) ."</td>
                        <td style='width:25%;'><div style='width:96%;height:40px;overflow:hidden'>".$row->description."</div></td>
                        <td>".$Sectors["{$row->sector}"]."</td>
                        <td>".$Revenue["{$row->revenue}"]."</td>
                        <td></td>
                        <td>".l('view', 'account',array('query'=>'a=company_detail&id=' . $uid) ) ."</td>
                        <td></td>
                        <td></td>
                        <td></td></tr>";
                        
            
            $table .= '</tr>';
            $count++;
        }
        $table .= '</tbody></table>';
    }elseif($user_type == 'p'){
        $table .= '<table class="table-list"><thead>  <th width="15%">Name</th>
                                    <th>Sector</th>
                                    <th>Experience</th>
                                    <th>Positions</th>
                                    <th>Companies</th>
                                    <th width=25%>Bio</th>
                                    <th>Max hrs/wk</th>
                                    <th>Request</th>
                                    ';
                                  
        $count = 1;
        while($row = db_fetch_object($Result))
        {
            
            $uid = $row->uid;
            
            if($count % 2) $table .= "\n<tr class='odd'>";
            else $table .= "\n<tr class='even'>";
    
            $table .=  "<td><a href=''>".ucwords($row->first_name) . ", " . ucwords($row->last_name) ."</a></td>
                        <td style='width:25%;'><div style='width:96%;height:40px;overflow:hidden'>". $Sectors["{$row->sector}"]. "</div></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td><div style='width:96%;height:40px;overflow:hidden'>" . $row->description . "</div></td>
                        <td></td>
                        <td></td></tr>";
                        
            
            $table .= '</tr>';
            $count++;
        }
        $table .= '</tbody></table>';
    }else{
        
    }
    $table .= "<div style='width:100%;'>" . pagination($Pagenation) . "</div>";
    return $table;
    
}

function _get_user_ext($uid)
{
    if($uid <= 0) return false;
    
    $theYear = date("Y");
    $revenue = 0;
    $revenue_g = 0;
    $revenue_year = '';
    $earnings = 0;
    $earnings_g = 0;
    $earnings_year = '';
    $sql = "select * from users_data where uid = %d and data_type = 'financial' order by data_attr desc";
    $result = db_query($sql, array($uid));
    while(($data = db_fetch_object($result))  !== false)
    {
        $revenue = ($data->data_value)? (int) $data->data_value : 0;
        $earnings = ($data->data_value2)? (int) $data->data_value2 : 0;
        if($theYear != $data->data_attr)
        {
            $revenue_year = "(".$data->data_attr.")";
        }
       
        if($revenue && $earnings)
        {
            while(($data = db_fetch_object($result)) !== false)
            {
                $revenue_prior = ($data->data_value) ? (int)$data->data_value : 0;
                $earnings_prior = ($data->data_value2) ? (int)$data->data_value2 : 0;
                if($revenue_prior && $earnings_prior)
                {
                    $revenue_g = (int)((($revenue - $revenue_prior) / $revenue_prior )  * 100);
                    $earnings_g = (int)((($earnings - $earnings_prior) / $earnings_prior ) * 100);
                    
                    break;
                }
            }
            $revenue = number_format($revenue);
            $earnings = number_format($earnings);
            break;
        }
    }

    
    
        $sql = "select * from users_extend where uid = %d";
        $result = db_query($sql, array($uid));
        $row = db_fetch_object($result);
       $Data = array();
 
        
        $Data['company_name'] = $row->company_name;
        $Data['content'] = <<< END
        <table>
            <thead>
                <tr>
                    <th>Description</th>
                    <th>Revenues</th>
                    <th>Growth</th>
                    <th>Earnings</th>
                    <th>Growth</th>
                    <th>ROE</th>
                    <th>Detail</th>
                </tr>
            </thead>
            <tbody>
                <tr>
                    <td>{$row->description}</td>
                    <td>$revenue $revenue_year</td>
                    <td>{$revenue_g}%</td>
                    <td>$earnings  $revenue_year</td>
                    <td>{$earnings_g}%</td>
                    <td></td>
                    <td><a href='/account?a=cactivities&p=edit_company_detail'> &gt; </a></td>
                </tr>
            </tbody>
        </table>
END;
        return $Data;
   
}
function company_detail($op = '')
{
    
    $Block['title'] = "Company Detail";
    if($op == 'view' || $op == ''){
        
    }
    
    return $Block;
}
function get_search_result($user_type)
{
    $sql = '';
    
    
    //
    $limit = 20;
    $page = sget($_REQUEST,'page') ? (int)(sget($_REQUEST, 'page')) : 1;
    $start = ($page - 1) * $limit;
    $SQL_where_filter = "";
    $SQL_data = array();
    
    if($user_type == 'p')
    {
        
        //$Positions = get_p_positions();
        //$Education = get_p_education();
        
        $rid = array(4,5,7);
        $type = sget($_REQUEST,'user_type');
        if($type > 0) $rid = array($type);
        if(sget($_REQUEST, 'sector')){
            $SQL_where_filter .= " and ta.sector = '%d'" ;
            $SQL_data[] = sget($_REQUEST, 'sectors');
        }
        $SQL_begin = " select * from users_extend as ta, users_roles as tb  ";
        $SQL_where .= "where ta.uid = tb.uid and tb.rid in (". implode(',', $rid ) . ") ";
        $SQL_limit = " limit $start, $limit";
        $SQL_begin_total = " select count(*) as total from users_extend as ta, users_roles as tb  ";
    }elseif($user_type == 'admin')
    {
        $rid = array(6);
        $SQL_begin = " select *, tc.mail from users_extend as ta, users_roles as tb, users as tc ";
        $SQL_where .= "where tc.status != 0 and ta.uid = tc.uid and ta.uid = tb.uid and tb.rid in (". implode(',', $rid ) . ") ";
        $SQL_limit = " limit $start, $limit";
        $SQL_begin_total = " select count(*) as total from users_extend as ta, users_roles as tb, users as tc   ";
    }elseif($user_type == 'company')
    {
        $rid = array(3);
        $SQL_begin = " select *, tc.mail from users_extend as ta, users_roles as tb, users as tc ";
        $SQL_where .= "where ta.uid = tc.uid and ta.uid = tb.uid and tb.rid in (". implode(',', $rid ) . ") ";
        $SQL_limit = " limit $start, $limit";
        $SQL_begin_total = " select count(*) as total from users_extend as ta, users_roles as tb, users as tc   ";
    }elseif($user_type == 'investor')
    {
        $rid = array(8);
        $SQL_begin = " select *, tc.mail from users_extend as ta, users_roles as tb, users as tc ";
        $SQL_where .= "where ta.uid = tc.uid and ta.uid = tb.uid and tb.rid in (". implode(',', $rid ) . ") ";
        $SQL_limit = " limit $start, $limit";
        $SQL_begin_total = " select count(*) as total from users_extend as ta, users_roles as tb, users as tc   ";
    }
    
    $Result = db_query($SQL_begin_total . $SQL_where . $SQL_where_filter , $SQL_data);
    $row = db_fetch_object($Result);
    $total =  $row->total;
    
    $SQL = $SQL_begin . $SQL_where . $SQL_where_filter .$SQL_limit;
    $Data = db_query($SQL, $SQL_data );

    $Result = array();
    while(($row = db_fetch_object($Data)) !== false)
    {
        $Result[] = $row;
    }

    return array('total'=>$total, 'limit'=>$limit, 'result'=>$Result,'userType'=>$user_type);
}
function render_people_search($Search_parameter, $Result, $paginationBaseURL)
{

    $block = render_searchbar($Search_parameter);
    $block .= render_result_page($Search_parameter, $Result, $paginationBaseURL );
    //echo $block;exit;
    return $block;
}
function clearString($str)
{
    return preg_replace("/[^a-z0-9\.]/i", '_', $str);
}
function render_result_page($Search_parameter, $Search_result, $paginationBaseURL)
{
    $Pagenation = array('total' => $Search_result['total'],
        'limit' => $Search_result['limit'],
        'baseurl' => $paginationBaseURL,
        'num_of_links' => 8,
    );
    $Result = $Search_result['result'];
    //test_array($Result);
    //var_dump( $Search_result['userType']);
    $block = "";
    
        if($Search_result['userType'] == 'p')
        {
            foreach($Result as $row)
            {
    
                $mcid = '';
                $uid = $row->uid;
                $mcid = sprintf("%06d", $uid+1100);
                $block .= 
        "<div style='position:relative;padding:8px 10px 15px 10px;border:solid 1px #f2f2f2;margin-top:-1px;'>
            <div style='font-size:18px;color:#6792d0;line-height:26px;height:26px;padding-bottom:8px;'>MC ".$mcid."</div>
            <div style='position:relative;border:#f2f2f2;background:#dce6f5;width:250px;height:60px;float:left;padding:7px 3px'>
                <div style='position:relative'>Experience: <div style='position:absolute;top:0;left:70px;overflow:hidden'>- 20 years</div></div>
                <div style='position:relative'>Positions: <div style='position:absolute;top:0;left:70px;overflow:hidden'>- 20 years</div></div>
                <div style='position:relative'>Companies: <div style='position:absolute;top:0;left:70px;overflow:hidden'>- 20 years</div></div>
                
            </div>
            <div style='position:relative;margin-left:20px;width:250px;height:80px;float:left;padding:7px 3px''>
            BIO
            </div>
            <div style='position:relative;width:50px;height:60px;float:left'>
            4
            </div>
            <div style='position:relative;width:50px;height:60px;float:left'>
            request
            </div>
            <br class='clearfix'>
        </div>\n";
    
                
            }
        }elseif($Search_result['userType'] == 'admin')
        {
            $block .= "
                    <form action='/account' method='post'>
                    <input type=hidden name=a value=admins />
                    <input type=hidden name=update_section value=remove_admins />
                    <table class='data-table'>" .
                    "<thead><tr><th></th><th>Name</th><th>Email</th><th></th><th>Detail</th></tr></thead><tbody>";
            foreach($Result as $row)
            {
                $id= $row->uid;
                $block .= "<tr><th style='width:20px;'><input type=checkbox name='uids[]' value='".$row->uid."'></th>
                                <th style='width:140px;'>". ucwords($row->first_name . " " . $row->last_name) ."</th>
                                <th>" . $row->mail .  "</th><th></th>
                                <th style='width:60px;'><a href='account?a=admins&type=detail&id=$id' >view</a></th></tr>";
            }
            $block .= "</tbody></table>";
            $block .= "<div style='margin-top:15px;'><input type='submit' name=submit value='delete' class='button' />";
            
            $block .= "</div></form>";
        }elseif($Search_result['userType'] == 'company')
        {
            $block .= "<table class='data-table'>" .
                    "<thead><tr><th><input type=checkbox class='select_all_cb'></th><th>Name</th><th>Email</th><th></th><th>Detail</th></tr></thead><tbody>";
            foreach($Result as $row)
            {
                $block .= "<tr><th><input type=checkbox name='uids[]' value='".$row->uid."'></th><th>". strtoupper($row->company_name) ."</th>
                        <th>" . $row->mail .  "</th><th></th><th><a href='/account?a=companies&v=company_mgmt&cid=".$row->uid."'>view</a></th></tr>";
            }
            $block .= "</tbody></table>";
        }elseif($Search_result['userType'] == 'investor')
        {
            $block .= "<table class='data-table'>" .
                    "<thead><tr><th><input type=checkbox class='select_all_cb'></th><th>Name</th><th>Email</th><th></th><th>Detail</th></tr></thead><tbody>";
            foreach($Result as $row)
            {
                $block .= "<tr><th><input type=checkbox name='uids[]' value='".$row->uid."'></th><th>". $row->first_name . " " . $row->last_name ."</th>
                        <th>" . $row->mail .  "</th><th></th><th><a href=''>view</a></th></tr>";
            }
            $block .= "</tbody></table>";
        }
        
    

    
    $block .= "<div style='width:100%;'>" . pagination($Pagenation) . "</div>";
    return $block;
}
function render_searchbar($SP)
{
    //var_dump($SP);
    if((! is_array($SP))  || count($SP) == 0 ) return "";
    $block = "<div style='clear:both'></div>";
    $block .= "<form action='' method='get'><div class='filter_bar'><table border=0 cellspacing=0 cellpadding=0 width='100%'><tr>";
    $block .= "<td style='border-top:none;padding-bottom:5px'>";
    foreach($SP as $key => $value)
    {
        $selection = sget($_REQUEST, $key);
        
        $block .=     "<div style='float:left;padding-right:4px;'><label class='italic e-text grey'>" . ucwords($key) . ":</label><br> ";
        $counter = 1;
        if(is_array($value))
        {
            $block .= "\n<select class='search_select' name='$key'>";
            
            foreach($value as $k => $v)
            {
                $v = trim($v);
                $selected = ( $k == $selection) ? ' selected ' : '';
                
                if($counter == 1 && ( ! empty($v)))
                {
                    $block .= "<option $selected> </option>";
                    
                }
                $counter = 2;
                $block .= "<option value='$k' $selected>" .$v . "</option>";
            }
            $block .= '</select></div>';
        }
        
    }
    $block .= "<div style='float:left;padding-right:20px;'><br><input type=image value=filter src='/themes/maennaco/images/filter.png' style='display:block'></div>";
    $block .= "</td>";
    $block .= "</tr></table></div>";
    $block .= "<input type=hidden name=a value='cpro'>";
    $block .= "<input type=hidden name=o value='view'>";
    $block .="</form>";
    // **************************
    
    return $block;
}
function getProjectName($id, $isadmin = '')
{
    if(empty($id)) return '';
    $sql = "select * from maenna_company where companyid = %d";
    $result = db_query($sql, array($id));
    $Data = db_fetch_object($result);
    $projName = $Data->projname;
    if(empty($projName)){
        $projName = "Project " . sprintf("%03s", $id +100);
    }
    return $projName;
}
function realid($name){
    $output = '';
    if($name){
        $output = preg_replace("/^[a-z]*0*/i",'', $name);
        $output -= 100;
    }
    return $output;
}
function getProId($id)
{
    if(empty($id)) return 'invalid id';
    $sql = "select users_roles.*, maenna_people.firstname from users_roles, maenna_people where users_roles.uid = %d and maenna_people.pid = %d limit 1";
    $result = db_query($sql, array($id, $id));
    $Row = db_fetch_object($result);
    if(empty($Row)) return "invalid user role setting - $id";
    $rid = $Row->rid;
    $firstname = strtoupper($Row->firstname);
    if(in_array($rid, array(6, 10))){
        $output = "${firstname}MAEADM" . sprintf("%05s", $id +100);
    }else{
        if($user_type == 'admin' || $user_type == 'super')
		{
			$output = "${firstname}MAE" . sprintf("%04s", $id +100);
		}
		else
		{	
			$output = "${firstname} " . sprintf("%04s", $id +100);
		}
    }
    
    return $output;
}
function filter_num($value){
    if($value) return preg_replace("/[^0-9\.\-]/",'', $value);
    else return 0;
}
function sideBarExcerpt($text,$more_link = '')
{
    $output = '';
    if($text){
        if(strlen($text) > 200)
        {
            $output = substr($text, 0, 200) . ' ...';
            if($more_link){
                $output .= "<div class=btnwrapper><div class='morebtn'><a href='$more_link' class=tool>More</a></div></div>";
            }
        }
        else $output = $text;
    }
    return $output;
}
function excerpt($text, $len = 120){
    if($text){
        if(strlen($text) > $len) return substr($text, 0, $len) . ' ...';
        else return $text;
    }else{
        return '';
    }
}
function contentExcerpt($text,$more_link = '',$limit = 0)
{
    $length = 1000;
    if($limit) $length = $limit;
    $output = '';
    if($text){
        if(strlen($text) > $length)
        {
            $output = substr($text, 0, $length) . ' ...';
            if($more_link){
                $output .= "<div class=btnwrapper><div class='morebtn'><a href='$more_link' class=tool>More</a></div></div>";
            }
        }
        else $output = $text;
    }
    return $output;
}
function Options_state($state = '', $mode = '')
{
    $_State = get_us_states();
    return option_code($_State, $state, $mode);
}

function Options_infoTopic($topic = '', $mode = ''){
    $Topics = _Topics();
    return option_code($Topics, $topic, $mode);
}
function Options_infoType($infoType = '', $mode = ''){
    $InfoTypes = _InfoTypes();
    return option_code($InfoTypes, $infoType, $mode);
}

function Options_industry($industry = '', $mode = ''){
    $Industry = _INDUSTRY(); //_experties
    $option_industry = '';
    if((! empty($mode)) && empty($industry) ) return '';
    foreach($Industry as $key=>$Array){
            if(empty($key)) continue;
            if(! is_array($Array)){
                $option_industry .= "\n<option value='$key'>$Array</option>";
            }else{
                $option_industry .= "<optgroup label='$key'>";
                foreach($Array as $subKey => $title){
                    $selected = "";
                    if(($subKey == $industry) && $industry){
                        $selected = 'selected';if($mode == 1)return $title;
                    }
                    $option_industry .= "\n<option value='$subKey' $selected>$title</option>";
                }
                $option_industry .= "\n</optgroup>";
            }
        }
    return $option_industry;
}
function Options_experties($experties = '', $mode = ''){
    $Experties = _experties();
    $option_experties = '';
    foreach($Experties as $key=>$Array){
            if(empty($key)) continue;
            if(! is_array($Array)){
                $option_experties .= "\n<option value='$key'>$Array</option>";
            }else{
                $option_experties .= "<optgroup label='$key'>";
                foreach($Array as $subKey => $title){
                    $selected = "";
                    if((strcasecmp($subKey,$experties)==0) && $experties){
                        $selected = 'selected'; if($mode == 1)return $title;
                    }
                    $option_experties .= "\n<option value='$subKey' $selected>$title</option>";
                }
                $option_experties .= "\n</optgroup>";
            }
    }
    if($mode == 1) return '';
    else return $option_experties;
}
function Options_proType($proType = '', $mode = ''){
    $ProType = _pro_roles();
    
    return option_code($ProType, $proType, $mode);
}
function Options_rating($proType = '', $mode = ''){
    $ProType = array('1'=>1, '2'=>2,'3'=>3,'4'=>4,'5'=> 5);
    return option_code($ProType, $proType, $mode);
}
function Options_status($proType = '', $mode = ''){
    $ProType = array('pending' => 'pending', 'approved'=>'approved', 'remove'=>'remove');
    return option_code($ProType, $proType, $mode);
}
function Options_selection($proType = '', $mode = ''){
    $ProType = array('0' => '', '1'=>'select');
    return option_code($ProType, $proType, $mode);
}
function Options_month($month='', $mode='')
{
    $Month = _Month();
    return option_code($Month, $month, $mode);
}
function Options_hour($hour='', $mode = ''){
    $Hours = array();
    for($i = 6; $i <= 22; $i++ ) $Hours["$i"] = sprintf("%02s", $i);
    return option_code($Hours, $hour, $mode);
}
function Options_capacity($capacity='', $mode = ''){
    $Capacity = array();
    for($i = 1; $i <= 100; $i++ ) $Capacity["$i"] = $i;
    return option_code($Capacity, $capacity, $mode);
}
function Options_weight($_weight='', $mode = ''){
    $Weight = array();
    for($i = 1; $i <= 150; $i++){
        $Weight["$i"] = $i;
    }
    return option_code($Weight, $_weight, $mode);
}
function Options_minute($minute='', $mode = ''){
    $Minute = array('0' => '00', '15'=>15,'30'=>30, '45'=>45);
    
    return option_code($Minute, $minute, $mode);
}
function option_code($Array, $val, $mode){
    $option = '';
    if($mode && empty($val)) return '';
    foreach($Array as $key => $title){
            $selected = "";
            if(($key == $val) && $val)
            {
                $selected = 'selected';
                if($mode == 1)return $title;
            }
           $option .= "\n<option value='$key' $selected>$title</option>";
    }
    return $option;
}

function render_table($Table){
    if(empty($Table) || (! is_array($Table))) return "invalid table array";
    $class = sget($Table,'class');
    $style = sget($Table, 'style');
    $html = "<table class='$class' $style>";
    
    $numOfCol = $Table['numOfCol'];
    if(isset($Table['title']) && (! empty($Table['title']))){
        $html = "<div style='font:italic bold 14px arial'>".$Table['title']."</div>" . $html;
    }
    if(isset($Table['thead']) && (! empty($Table['thead']))){
        $html .= "\n<thead><tr>";
        foreach($Table['thead'] as $Header){
            $style = (sget($Header,'style'));
            $label = ucwords(sget($Header, 'label'));
            $html .= "\n<td style='$style'>$label</td>";
        }
        $html .= "\n</tr></thead>";
    }
    $html .= "\n<tbody>";
    foreach($Table['tbody'] as $Row){
        $html .= "\n<tr>";
        
        foreach($Table['thead'] as $H){
            
            $text = sget($Row, strtolower($H['label']));
            $html .= "\n<td>$text</td>";
        }
        $html .= "\n</tr>";
    }
    $html .= "</tbody></table>";
    return $html;
}
function button_link($Links, $align = ''){
    if($align != ''){
        $align = "text-align:$align";
    }
    $html = "<div style='margin-top:4px; width:100%;$align'>";
    $counter = 1;
    foreach($Links as $link){
        $label = sget($link, 'label');
        $target = sget($link, 'url');
        $html .= "<a href='$target' class=button>$label</a>";
        if($counter != count($Links)) $html .= "&nbsp;&nbsp;";
        $counter++;
    }
    $html .= "</div>";
    return $html;
}
function jsbutton_link($Array){
    $position = sget($Array, 'position');
    $Links = $Array['links'];
    if($position){
        $position = "text-align:$position";
    }
    $output = "<div style='width:100%;$position'>";
    $counter = 1;
    if(is_array($Links)){
        foreach($Links as $L){
            $extra = sget($L, 'extra');
            $class = sget($L, 'class');
            $label = sget($L, 'label');
            $url = sget($L, 'url');
            $output .= "<a href='$url' class='button $class' $extra>$label</a>";
            if($counter != count($Links)) $output .= "&nbsp;&nbsp;";
            $counter++;
        }
    }
    $output .="</div>";
    return $output;
}
function text_link($Links, $align = ''){
    if($align != ''){
        $align = "text-align:$align";
    }
    $html = "<div style='margin-top:4px; width:100%;$align'>";
    $counter = 1;
    foreach($Links as $link){
        $label = sget($link, 'label');
        $target = sget($link, 'url');
        $html .= "<a href='$target'>$label</a>";
        if($counter != count($Links)) $html .= "&nbsp;&nbsp;&nbsp;";
        $counter++;
    }
    $html .= "</div>";
    return $html;
}
function js_init($code){
    if(empty($code)) return '';
    $html = <<< END
    \n
    <script type='text/javascript'>
    $(document).ready(function(){
        $code
    });
    </script>
END;
    return $html;
}
function userRoleId($uid = null){
    if(empty($uid)) return '';
    $sql = 'select rid from users_roles where uid = %d';
    $result = db_query($sql, array($uid));
    while($Row = db_fetch_object($result))
    {
        return $Row->rid;
    }
    return '';
}
function userType($uid = null)
{
    if(empty($uid)) return '';
    $sql = "select role.name from role, users_roles where role.rid = users_roles.rid and users_roles.uid = %d";
    $result = db_query($sql, $uid);
    $Row = db_fetch_object($result);
    if($Row)
    {
        return $Row->name;
    }
    return '';
}
function rebuild_url($Keys = '')
{
    if(empty($Keys) ) return '';
    $Components = array();
    foreach($Keys as $key){
        $Components[] = "$key=" . sget($_REQUEST, $key);
    }
    return "/account?" . implode("&", $Components);
}
function hidden_post_values($Keys = '')
{
    $output = '';
    if(empty($Keys)) return '';
    foreach($Keys as $key){
        $val = sget($_REQUEST, $key);
        $output .= "\n<input type=hidden name='$key' value='$val' />";
    }
    return $output;
}
function backup_copy($text, $operation = '')
{
    global $user;
    $uid = $user->uid;
    $sql = "insert into maenna_user_post(uid, serialized_data, operation) values(%d, '%s', '%s')";
    db_query($sql, array($uid, $text, $operation));
}
function upload_file($FILE)
{
    if($FILE['error'] == 0)
    {
        $filename = time() .'_' . $FILE['name'];
        $path = "./" . file_directory_path() . "/" . $filename;
        if(move_uploaded_file($FILE['tmp_name'], $path))
        {
            return array('name' => $filename,
                         'type' => $FILE['type']) ;
        }
    }
    return false;
}

/* EOF */
