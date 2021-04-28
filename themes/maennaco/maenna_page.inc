<?php

class Maenna_page
{
    var $page_name, $AccessObj, $All_sections;
    var $Tabs, $Left, $Right, $Middle, $Middle_tabs;
    var $Com_sections;
    function __construct($accessObj)
    {
        die;
        $this->Tabs = $this->Right = $this->Middle = $this->Middle_tabs = $this->All_sections = array();
        $this->AccessObj = $accessObj;
        $this->page_name = sget($_REQUEST, 'page');
        if(empty($this->page_name)) $this->page_name = sget($_REQUEST, 'tab');
        if(empty($this->page_name))
        {
            $this->page_name = 'activities';
            //echo "damjan".$this->AccessObj->user_type;
            if($this->AccessObj->user_type == 'company') $this->page_name = 'company_detail';
            
            if ($this->AccessObj->user_type == 'people') $this->page_name = 'pro_detail';
        }
        $this->Tabs = Maenna_page::get_tabs($accessObj->user_type);
        
        $Page = $this->page_sections($accessObj->user_type, $this->page_name);
        $this->Left = $Page['Left'];
        $this->Right = $Page['Right'];
        $this->Middle = $Page['Middle'];
        $this->Middle_tabs = $Page['Middle_tabs'];
        
        $panel = sget($_REQUEST, 'panel');
        if($panel){
            $this->AccessObj->Objects = $Page['Middle'];
            $Page['Middle'] = array($panel);
            $this->Middle = $Page['Middle'];
        }
    }
    public static function get_tabs($user_type)
    {
        //echo $role_id;
        $Tabs = array();
        if($user_type == 'super')
        {
            $Tabs = array('activities' => '',
                          'messages' => '',
                          'companies' => '',
                          'professionals' => '',
                          'admins' => '',
                          'investors' => '',
                          'research' => '',
                          'information' => '',
                          'settings' => '');
        }
        elseif($user_type == 'company')
        {
            $Tabs = array(  
                            //'activities' => '',
                            
                            'company' => 'my company',
                            'messages' => '',
                            'questionnaire'=>'QUESTIONNAIRE',
                            //'information' => '',
                            'settings' => '');
        }
        elseif($user_type == 'people')
        {
            $Tabs = array(
                          
                            'profile' => 'MY ACCOUNT',
                            //'activities' => '',
                            'messages' => '',
                            'questionnaire'=>'QUESTIONNAIRE',
                            //'information' => '',
                            'settings' => '');
        }
        elseif($user_type == 'admin')
        {
            $Tabs = array('activities' => '',
                          'messages' => '',
                          'companies' => '',
                          'professionals' => '',
                          'investors' => '',
                          #'research' => '',
                          #'information' => '',
                          'settings' => '');
        }
        return $Tabs;
    }
    
    
    function page_sections($user_type, $page_name)
    {
        require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
        global $AccessObj; 
        $Page = array('Left'=> array(),
                      'Middle'=> array(),
                      'Right'=> array());
        switch($page_name)
        {
            case "companies":
                if( !(in_array($user_type, array('super', 'admin'))) ) break; 
                $Page['Middle'] = array('company_list');
                $Page['Right'] = array();
                break;
            case "company_detail": case "company": 

                    $mtab = sget($_REQUEST, 'mtab');
                    
                    // load page sections
                    $this->AccessObj->Com_sections = $this->mae_com_sections();
                    //test_array($this->AccessObj->Com_sections);
                    $this->AccessObj->_authz();
                    //test_array($this->AccessObj->Com_sections);
                    // after _authz, if mtab is undefined,  use the 1st visible tab name to initialize page sections
                    if(empty($mtab))
                    {
                        foreach($this->AccessObj->Com_sections as $tab_name => $Tab_data)
                        {
                            if($Tab_data['access'] != 'hide' && $tab_name != 'common')
                            {
                                $mtab = $tab_name;
                                break;
                            }
                        }
                    }
                    
                    // fill page left, middle, right area with section name/functions related with current mtab value
                    foreach($this->AccessObj->Com_sections as $tab_name => $Tab_data)
                    {
                        if($tab_name != 'common')
                        {
                            $Page['Middle_tabs'][] = $tab_name;
                            if(strcasecmp($mtab, $tab_name) == 0)
                            {
                                foreach($Tab_data['sections'] as $section_name => $Section_data)
                                {
                                    $position = ucwords($Section_data['position']);
                                    $Page["$position"][] = $section_name;
                                }
                            }
                        }
                    }
                    // add company_name to left col, not being controlled
                    array_unshift($Page['Left'], 'company_name');
                    
                    // add sections to left col. , not being controlled.
                    if( ! in_array($mtab,array('monitoring',"questionnaire") )) array_push($Page['Left'], 'sections');
                    
                    // add assign_admin to right col. on super's
    
                    if($user_type == 'admin' && Connections::_relation() == 'au_assign')
                    {
                        array_push($Page['Right'],'admin_section_title', 'manage_connections','foundrising_administration','admin_section');
                    }
                    
                    if($user_type == 'super')
                    {
                        array_push($Page['Right'],'admin_section_title','admin_tools', 'com_questionnaire', 'assign_admin', 'manage_connections','foundrising_administration','com_premium');
                    }
                    
                break;
            case "professionals":
                    if( !(in_array($user_type, array('super', 'admin'))) ) break; 
                    $Page['Middle'] = array('pro_list');
                    $Page['Right']  = array('maenna_recently_added_people',
                                                 'maenna_recently_updated_people',
                                                 'pro_types',
                                                 'pro_eventCalendar',
                                                 'pro_memberstats',
                                                 'mlvl_stats',
                                                 'sector_stats',
                                                 'expertis_stats');
                break;
            case "pro_detail":
                    global $redirect;
                    $redirect = rebuild_url(array('tab', 'page'));
                    $Page['Middle'] = array('pro_profile',
                                            'pro_experties',
                                            'pro_accomplishments',
                                            'pro_personality',
                                            'pro_industry_view',
                                            'pro_mgmt_view');
                    $Page['Left'] = array('pro_name','pro_connections','pro_relatedPro');
                    $Page['Right'] = array( 'pro_progress',
                                            'pro_eventCalendar',
                                            'pro_hourStats',
                                            'pro_selectWork',
                                            );
                    if($user_type == 'company')
                    {
                        $Page['Right'] = array('pro_selectWork');
                    }
                    if($user_type == 'people')
                    {
                        $id = sget($_REQUEST, 'id');
                        if($id){
                            if($AccessObj != $id)
                            {
                                $Page['Right'] = array('pro_eventCalendar','pro_selectWork');
                            }
                        }
                    }
                    if(in_array($user_type, array('admin','super')))
                    {
                        array_push($Page['Right'], 'admin_section_title','manage_connections','pro_rating','pro_questionnaire','progress_status','pro_vetting');
                    }
                    if($user_type == 'super')array_push($Page['Right'], 'assign_admin');

                break;
            case "admins":
                if( !(in_array($user_type, array('super'))) ) break;
                    $Page['Middle'] = array('admin_list');
                    $Page['Right'] = array('add_admin_user');
                break;
            case 'admin_detail':
                if( !(in_array($user_type, array('super'))) ) break;
                    $Page['Middle'] = array('admin_user_info');
                    $Page['Right'] = array();
                break;
            case 'settings':
                $Page['Middle'] = array('settings_instruction');
                $Page['Right'] = array('account_settings');
                if(in_array($user_type, array('super') ))array_unshift($Page['Right'], 'account_permissions');
                break;
            case 'questionnaire':
                if(! in_array($AccessObj->user_type, array('company', 'people') )) return '';
                $Page['Left'] = array('company_name', 'pro_name','questionnaire_list');
                $Page['Middle'] = array('questionnaire');
                $Page['Right'] = array('qfiles');
                break;
            case 'information':
                if($AccessObj->user_type != 'super') return '';
                $Page['Middle'] = array("infoTabs");
                $Page["Right"] = array('info_tools');
                break;
            case 'insights':
                if($AccessObj->user_type != 'super') return '';
                $Page['Middle'] = array("infoTabs",'info_list','related_list');
                $Page["Right"] = array('upload_box','maenna_selection', 'recent_topics', 'admin_approve','info_tools');

                break;
            case 'postings':
                if($AccessObj->user_type != 'super') return '';
                $Page['Middle'] = array('infoTabs', 'search_bar','posting_list');
                $Page["Right"] = array('info_tools','post_project','advisory_openings', 'project_openings');
                break;
            case 'qmgmt':
                if(! in_array($AccessObj->user_type, array('admin', 'super')))return '';
                $Page['Middle'] = array("infoTabs",'qmgmt');
                $Page["Right"] = array('info_tools');
                if(sget($_REQUEST, 'panel') == 'answerdetail') array_unshift($Page['Right'], 'qmgmt_files');
                break;
            case 'events':
                if($AccessObj->user_type != 'super') return '';
                $Page['Middle'] =  array('infoTabs','search_bar','event_list');
                $Page["Right"] = array('info_tools', 'post_event_links');
                break;
            case 'research':
                if($AccessObj->user_type != 'super' && $AccessObj->user_type != 'admin') return '';
                $type = sget($_REQUEST, 'type');
                if(empty($type))$type = 'company';
                
                $Page['Middle'] = array("research_table");
                if($type == 'company')
                {
                     $Page["Right"] = array('add_research_company',
                                            'research_manager',
                                            'key_statistics',
                                            'recently_added',
                                            'recently_updated',
                                            'leads_by_sector',
                                            'priority_stats',
                                            'followup_stats',
                                            'sector_stats');
                }else
                {
                    $Page['Right'] = array( 'add_research_people',
                                            'recently_added_people',
                                            'recently_updated_people',
                                            'priority_stats_people',
                                            'followup_stats_people');
                }
               
                break;
            case 'research_company_detail':
                $Page['Left'] = array(  'research_company_logo',
                                        'research_company_brief_info',
                                        'research_company_management',
                                        'research_company_market_info',
                                        'research_company_links');
                $Page['Middle'] = array('research_company_name',
                                        'research_company_financials',
                                        'research_progress_status');
                $Page['Right']= array('research_company_followers',
                                      'research_links_group',
                                      'research_company_prospects',
                                      'research_company_qualified_lead',
                                      'research_followup_priority',
                                      'research_companies_to_follow',
                                      'research_company_articles');

                break;
            case 'research_people_detail':
                $Page['Left'] = array(  'research_people_contact',
                                        'research_people_work',
                                        'research_people_education',
                                        'research_people_links');
                $Page['Middle'] = array('research_people_info',
                                        'research_accomplishments',
                                        'research_people_status');
                $Page['Right']= array(  'add_research_people',
                                        'research_people_prospects',
                                        'research_people_followup',
                                        'research_people_articles');
                break;
            case 'messages':
                $Page['Middle'] = array('messages_inbox');
                $Page['Right']= array( 'messages_folders');
                break;
        }
        // eo super
        /*
       
        */

        return $Page;
    }
    function user_submit()
    {
        global $user;
        //test_array($_REQUEST);
        $section = sget($_REQUEST, 'section');
        $update_section = sget($_REQUEST, 'update_section');
        
        if($update_section)
        {
            if($this->AccessObj->_writable($section) && function_exists($update_section))
            {
                //echo "UPDATE SECTION = $section";
                $update_section('update');
            }else{
                user_error($this->AccessObj->user_type . " user {$user->uid} does not have write access to {$update_section}");
            }
        }
        
    }
    
    function render($location)
    {
        //test_array($this->AccessObj->Com_sections);
        
        switch ($location) {
            case 'left':
                $Array = $this->Left;
                break;
            case 'middle':
                $Array = $this->Middle;
                break;
            case 'right':
                $Array = $this->Right;
                break;
            default:
                $Array = [];
                break;
        }
        
        if(count($Array) > 0)
        {
            if($this->page_name == 'pro_detail')
            {
                foreach($Array as $section)
                {
                    if(function_exists($section))
                    {
                        $mode = $this->AccessObj->pro_sections_access();
                        $Temp = $section($mode);
                        if( $Temp['body']) echo $Temp['body'];
                    }
                }
            }elseif($this->page_name == 'company_detail')
            {
                
                foreach($Array as $section)
                {
                    if(function_exists($section))
                    {
                        $mode = $this->AccessObj->section_mode($section);
                        if($mode == 'hide') continue;
                        elseif(empty($mode)) $mode = 'not being controlled';
                        //echo "<br> $section = $mode";
                        $Temp = $section($mode);
                        if( $Temp['body']) echo $Temp['body'];
                    }
                }
            }else
            {
                foreach($Array as $section)
                {
                    if(function_exists($section))
                    {
                        $Temp = $section('write');
                        if( $Temp['body']) echo $Temp['body'];
                    }
                }
            }
            
        }
    }
    
    function render_mtab()
    {
        global $AccessObj;
        $tab = sget($_REQUEST, 'mtab');
        
	//$AccessObj->Com_sections['file']['sections']['maenna_file']['access']='write';
	//$AccessObj->Com_sections['file']['sections']['connections']['access']='write';
	//$AccessObj->Com_sections['file']['access']='write';
        if(empty($tab)){
            foreach($AccessObj->Com_sections as $tab_name => $Tab_data)
            {
                if($Tab_data['access'] != 'hide')
                {
                    $this->AccessObj->mtab = $tab_name;
                    $tab = $tab_name;
                    break;
                }
            }
        }
        $id = sget($_REQUEST, 'id');
        $mTabs = $this->Middle_tabs;
        $i = 0;
        $redirect = rebuild_url(array('tab', 'page', 'id'));
        foreach($mTabs as $key){
            $access = $AccessObj->Com_sections["$key"]["access"];
            if($access == 'hide') continue;
            $active = ''; $class = '';
            if($key == $tab)$active = ' active-trail active';
            if($i == 0)$class = 'first';
            elseif($i == (count($mTabs) - 1))$class = 'last';
            if($key == 'minutes')
            {
                $content .= "\n<li class=' $class $active '><a href='$redirect&mtab=$key' class='$active'>".strtoupper('minutes')."</a></li>";
            }elseif($key == 'questionnaire')
            {
                $content .= "\n<li class=' $class $active '><a href='$redirect&mtab=$key' class='$active'>".strtoupper('due diligence')."</a></li>";
            }else
            {
                $content .= "\n<li class=' $class $active '><a href='$redirect&mtab=$key' class='$active'>".strtoupper($key)."</a></li>";
            }
            
            $i++;
        }
        $content = <<< END
        <div class='account-section-tabs'>
            <ul>
            $content
            </ul>
        </div>
END;
        return $content;
    }
    function account_tabs()
    {
        
        $current_tab = sget($_REQUEST, 'tab');//$maenna_page['tab'];

        global $AccessObj;
        
        if(empty($current_tab)) $current_tab = 'activities';
        echo "<div class='account_tabs1'>";
        foreach($this->Tabs as $tab_key => $tab_title)
        {
            if($tab_title)  $title = strtoupper($tab_title);
            else $title = strtoupper($tab_key);
            $class='';
            if( (strcasecmp($tab_key, $current_tab) == 0) ||
                ($current_tab == 'companies' && $tab_key == 'company') ||
                ($current_tab == 'professionals' && $tab_key == 'profile' )
              )
            {
                 $class = 'active';
            }
            echo "\n";
            if($tab_key == 'company') echo  "<a href='/account?tab=companies&page=company_detail&id=".$this->AccessObj->uid."' class='$class'>$title</a> ";
            elseif($tab_key == 'profile') echo "<a href='/account?tab=professionals&page=pro_detail&id=".$this->AccessObj->uid."' class='$class'>$title</a> ";
            else echo "<a href='/account?tab=$tab_key' class='$class'>$title</a> ";
        }
        echo "</div>";
    
    }
    
    function breadcrumb($level = null, $title = null, $link = null)
    {
        $array = array('tab','page', 'mtab','section');
        
        $Trail = array("<a href='/account'>Home</a>");
        
        if($tab = sget($_REQUEST, 'tab')){
            $link = rebuild_url(array('tab'));
            $title = ucwords($tab);
            $Trail[] = "<a href='$link'>$title</a>";
        }
        if($page = sget($_REQUEST, 'page')){
            $link = rebuild_url(array('tab','page', 'id'));
            $title = str_replace("_", ' ', $page);
            $title = ucwords($title);
            $Trail[] = "<a href='$link'>$title</a>";
        }
        if($mtab = sget($_REQUEST, 'mtab')){
            $link = rebuild_url(array('tab','page', 'mtab',  'id'));
            if($mtab == 'minutes')$mtab = 'advice';
            $title = ucwords($mtab);
            $Trail[] = "<a href='$link'>$title</a>";
        }
        if($section = sget($_REQUEST, 'section') && 0){
            $link = rebuild_url(array('tab','page', 'mtab', 'section', 'id')); $title = ucwords($section);
            $Trail[] = "<a href='$link'>$title</a>";
        }
        
        echo "<div class='trail'>";
        echo implode("  &raquo; ", $Trail);
        echo "</div>";
    }
    function mae_com_sections()
    {
        $Sections = array();
        $sql = "SELECT  maenna_sections.sectionid AS tab_id,
                        maenna_sections.maenna_name AS tab_maenna_name,
                        maenna_sections.title AS tab_title,
                        sections . *
                FROM    maenna_sections
                LEFT JOIN maenna_sections AS sections ON maenna_sections.sectionid = sections.parentid
                WHERE   maenna_sections.type = 'mtab'
                ORDER BY tab_id, sections.position,  sections.weight";
        $result = db_query($sql);
        $prev = '';
        while($Row = db_fetch_array($result))
        {
            $tab_id = sget($Row, 'tab_id');
            
            $tab_maenna_name = sget($Row,'tab_maenna_name');
            
            $tab_title = sget($Row, 'tab_title');
            
            if($prev != $tab_id)
            {
                
                $Sections["$tab_maenna_name"] = array('id' => $tab_id,
                                                      'maenna_name' => $tab_maenna_name,
                                                      'title'=> $tab_title,
                                                      'sections' => array());
                $prev = $tab_id;
            }
            
            $section_name = sget($Row, 'maenna_name');
            $section_id = sget($Row,'sectionid');
            $section_title = sget($Row, 'title');
            $section_panel = sget($Row, 'maenna_panel');
            $section_data_type = sget($Row, 'data_type');
            $position = sget($Row, 'position');
            $cu_edit = sget($Row, 'cu_edit');
            $Sections["$tab_maenna_name"]['sections']["$section_name"]  =array('id' => $section_id,
                                                                               'title' => $section_title,
                                                                               'panel' => $section_panel,
                                                                               'data_type' => $section_data_type,
                                                                               'position' => $position,
                                                                               'cu_edit' => $cu_edit
                                                                               );
            
           
        }
        
        $Sections['common'] = array('id' => '',
                                    'maenna_name' => '',
                                    'title'=> 'Common Sections',
                                    'sections' => array());
        $sql = "select * from maenna_sections where type='common' and position = 'com_left'";
        $result = db_query($sql);
        while($Row = db_fetch_array($result))
        {
            $section_name = sget($Row, 'maenna_name');
            $section_id = sget($Row,'sectionid');
            $section_title = sget($Row, 'title');
            $section_panel = sget($Row, 'maenna_panel');
            
            $Sections['common']['sections']["$section_name"] = array('id' => $section_id,
                                                            'title' => $section_title,
                                                            'panel' => $section_panel,
                                                            'data_type' => $section_data_type,
                                                            
                                                        );
        }
        return $Sections;
    }
    
}


/* EOF */
