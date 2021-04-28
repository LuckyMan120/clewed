<?php

class Maenna_page {

    var $page_name, $AccessObj, $All_sections;
    var $Tabs, $Left, $Right, $Middle, $Middle_tabs;
    var $Com_sections;
    private $BeforeContent;

    function __construct($accessObj) {

        $this->Tabs = $this->Right = $this->Middle = $this->Middle_tabs = $this->All_sections = array();
        $this->AccessObj = $accessObj;
        $this->page_name = sget($_REQUEST, 'page');
        if (empty($this->page_name)) {
            $this->page_name = sget($_REQUEST, 'tab');
        }
        if (empty($this->page_name)) {
            $this->page_name = 'activities';
            if ($this->AccessObj->user_type == 'company') {
                $this->page_name = 'company_detail';
            }
            if ($this->AccessObj->user_type == 'people') {
                $this->page_name = 'pro_detail';
            }
        }
        $this->Tabs = Maenna_page::get_tabs($accessObj->user_type);
        $Page = $this->page_sections($accessObj->user_type, $this->page_name);
        $this->Left = $Page['Left'];
        $this->Right = $Page['Right'];
        $this->Middle = $Page['Middle'];
        $this->Content = $Page['Content'];
        $this->Middle_tabs = $Page['Middle_tabs'];
        $panel = sget($_REQUEST, 'panel');
        if ($panel) {
            $this->AccessObj->Objects = $Page['Middle'];
            $Page['Middle'] = array($panel);
            $this->Middle = $Page['Middle'];
        }
    }

    public static function get_tabs($user_type) {
        $Tabs = array();
        if ($user_type == 'super') {
            $Tabs = array(
                'activities'    => '',
                'insights'      => 'Insights',
                'companies'     => '',
                'professionals' => '',
                'admins'        => '',
                'investors'     => '',
                'research'      => '',
                'information'   => '',
                'settings'      => ''
            );
        } elseif ($user_type == 'company') {
            $Tabs = array(
                'company'       => 'my project',
                'services'      => 'Services',
                'insights'      => 'Insights',
                'selling'       => 'Selling',
                'professionals' => 'Follow Professionals',
                'settings'      => ''
            );
        } elseif ($user_type == 'people') {
            $Tabs = array(
                'profile'       => 'MY ACCOUNT',
                'customfilter'  => 'Browse Opportunities ',
                'investments'   => 'Portfolios',
                'services'      => 'Services',
                'insights'      => 'Insights',
                'insights&type=registered'     => 'My Orders',
                'selling'       => 'Selling',
                'questionnaire' => 'MY QUESTIONNAIRE',
                'linkbank'      => 'LINK  BANK',
                'settings'      => ''
            );
        }
        elseif ($user_type == 'admin') {
            $Tabs = array(
                'activities'    => '',
                'insights'      => 'Insights',
                'companies'     => '',
                'professionals' => '',
                'investors'     => '',
                'research'      => '',
                'information'   => '',
                'settings'      => ''
            );
        }
         elseif ($user_type == 'sales') {
            $Tabs = array(
            'companies'     => '',
            'professionals' => '',
            'settings'      => ''
            );
            }
        return $Tabs;
    }

    function page_sections($user_type, $page_name) {
        require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
        global $AccessObj;

        global $ifAdmin;
        $Page = array(
            'Left'   => array(),
            'Middle' => array(),
            'Right'  => array()
        );

        switch ($page_name) {
            case "companies":
                if (!(in_array($user_type, array('super', 'admin', 'people', 'sales')))) {
                    break;
                }
                if (in_array($user_type, array('super', 'admin', 'sales'))) $Page['Right'] = array('cmp_type', 'cmp_status');
                $Page['Middle'] = array('company_list');
                $this->AccessObj->Com_sections = $this->mae_com_sections();
                $this->AccessObj->_authz();
                break;
            case "company_detail":
            case "company":
                $mtab = sget($_REQUEST, 'mtab');
                $type = sget($_REQUEST, 'type');
                $this->AccessObj->Com_sections = $this->mae_com_sections();

                $this->AccessObj->_authz();
                //If company is public make about tab visible as well as company_name section,as it is not controlled by perm table
                if ($page_name == 'company_detail') {
                    $cid = sget($_REQUEST, 'id');
                    $userid = sget($_REQUEST,'userid');
                    $main_userid = $AccessObj->uid;
                    if($userid) $main_userid = $userid;

                    if (ifPublicCompany($cid) && !ifConnected($cid, $main_userid) && $AccessObj->user_type != 'super' && $cid != $main_userid) {
                        $public = 1;
                        $this->AccessObj->Com_sections['about']['access'] = 'read';
                        $this->AccessObj->Com_sections['about']['sections']['about_general']['access'] = 'read';
                        if ($this->AccessObj->Com_sections['common']['access'] == 'hide') {
                            $this->AccessObj->Com_sections['common']['access'] = 'read';
                        }
                        if ($this->AccessObj->Com_sections['common']['sections']['company_name']['access'] == 'hide') {
                            $this->AccessObj->Com_sections['common']['sections']['company_name']['access'] = 'read';
                        }
                    }
                }
                // after _authz, if mtab is undefined,  use the 1st visible tab name to initialize page sections
                if (empty($mtab)) {
                    foreach ($this->AccessObj->Com_sections as $tab_name => $Tab_data) {
                        if ($Tab_data['access'] != 'hide' && $tab_name != 'common') {
                            $mtab = $tab_name;
                            $_REQUEST['mtab'] = $tab_name;
                            break;
                        }
                    }
                }
                if ($type == 'details' && $mtab == 'summary') {
                    $Page['Middle'] = array('analysis_details');
                }
                if ($type == 'details' && $mtab == 'share_knowledge_details') {
                    $Page['Middle'] = array('share_knowledge_details');
                    array_unshift($Page['Left'],'connections','collaborators');
                }

                // fill page left, middle, right area with section name/functions related with current mtab value
                foreach ($this->AccessObj->Com_sections as $tab_name => $Tab_data) {
                    if ($tab_name != 'common' && $tab_name != 'goals') {
                        $Page['Middle_tabs'][] = $tab_name;
                        if (strcasecmp($mtab, $tab_name) == 0) {
                            foreach ($Tab_data['sections'] as $section_name => $Section_data) {
                                $position = ucwords($Section_data['position']);
                                $Page["$position"][] = $section_name;
                            }
                        }
                    }
                }

                $tabs = array_filter($Page['Middle_tabs'], function($item){
                    return $item != 'share_knowledge';
                });

                $Page['Middle_tabs'] = array_merge(
                    array_slice($tabs, 0, 2),
                    array('share_knowledge'),
                    array_slice($tabs, 2)
                );


                //check if professional user connected to this project returned obj
                $isMatchProfInProject = getActiveConnection($_REQUEST['id'],$AccessObj->uid);

                if ($mtab == 'about') {

//                  $Page['Left'] = array('');
//                  if ($AccessObj->uid == $_REQUEST['id']) {

                    if ($AccessObj->user_type != 'super' && $AccessObj->user_type != 'admin') {
                        array_push($Page['Right'], 'cmp_GettingStarted');
                        array_unshift($Page['Left'], 'connections', 'collaborators');
                    }

                    if ($AccessObj->user_type == 'people') {
//                        if ($isMatchProfInProject){
//                            array_unshift($Page['Right'], 'fundraising_section_cmp');
//                        }else{
                        array_unshift($Page['Right'], 'fundraising_section_pro');
//                        }
                        array_unshift($Page['Left'], 'connections', 'collaborators');
                    }

                    $company = getCompanyByID($_REQUEST['id']);

                    if ($AccessObj->uid == $_REQUEST['id'] || $AccessObj->user_type == 'super' ) {
                        if($company->started_fundraising && $company->started_fundraising == "1") {
                            array_unshift($Page['Left'], 'connections', 'collaborators');

                            if (sget($_REQUEST,'userid')){
                                array_unshift($Page['Right'], 'fundraising_section_pro');
                            }else{
                                array_unshift($Page['Right'], 'fundraising_section_cmp');
                            }

                        } else {

                            array_unshift($Page['Right'], 'fundraising_section_pro');
                            array_unshift($Page['Left'],'pro_project_box','connections');
                        }
                    }

                    //show three new sections when project is toggled fundraising
                    if ($AccessObj->user_type == 'super' || (!$company->started_fundraising || $company->started_fundraising == "1")){
                        array_push($Page['Right'], 'use_of_funds');
                        array_push($Page['Left'],'management_discussion','key_finance');
                    }

                }

                // add sections to left col. , not being controlled.
                if (!in_array($mtab, array('monitoring', "questionnaire"))) {
                    array_unshift($Page['Right'], 'sections');
                }
                if ($mtab == 'analysis' || $mtab == 'summary') {
                    array_push($Page['Right'], 'sortanalysis');
                }
                if($mtab == 'share_knowledge' || $mtab == 'share_knowledge_details') {
                    array_push($Page['Right'], 'share_knowledge_sort');
                }

                // add assign_admin to right col. on super's
                if ($mtab == 'about' && ($public == 1 || $this->AccessObj->uid == $cid)) {
                    // array_push($Page['Right'],'manage_connections','fundraising_administration','cmp_insights_stats', 'admin_section');
                    $Page['Middle'] = array('about_general', 'add_content');
                }
                if ($user_type == 'admin' && Connections::_relation() == 'au_assign') {
                    array_push($Page['Right'], 'admin_section_title','manage_connections','fundraising_administration','fundraising_administration' ,'admin_section');
                    // array_push($Page['Right'],'manage_connections','fundraising_administration','cmp_insights_stats', 'admin_section');
                    if ($mtab == 'about') {
                        $Page['Middle'] = array('about_general', 'add_content');
                    }
                }
                if ($user_type == 'super') {
                    array_push($Page['Right'], 'admin_section_title', 'admin_tools', 'com_questionnaire', 'assign_admin', 'manage_connections','fundraising_administration','cmp_insights_stats', 'com_premium');
                    if ($mtab == 'about') {
                        $content = sget($_REQUEST,'field');
                        $Page['Middle'] = array('about_general', 'add_content');
                    }
                }

                //If user is professional accessing company profile show profesional project box in left column
                if ($user_type == 'people' && $page_name == 'company_detail') {

                    $content = sget($_REQUEST,'field');
                    if($content) $Page['Middle'] = array('about_general_content', 'add_content');
                    if(!inProjectBox($AccessObj->uid, $_REQUEST['id'])) {
                        // $Page['Left'] = array('company_name');
                    }
                }

               // add company_name to left col, not being controlled
                array_unshift($Page['Left'], 'company_name');

                if (empty($Page['Middle'])) $Page['Middle'] = array('company_not_visible');

                $Page['Left'] = array_unique($Page['Left']);
//                dump($Page['Right']);
            break;
            case "professionals":
                if (!(in_array($user_type, array('super', 'admin', 'company', 'people', 'sales')))) {
                    break;
                }
                $Page['Middle'] = array('pro_list');
                $Page['Right'] = array('pro_filter');
                if (in_array($user_type, array('super', 'admin', 'sales'))) {
                    $Page['Right'] = array(
                        'maenna_recently_added_people',
                        'maenna_recently_updated_people',
                        'pro_types',
                        'pro_status',
                        'pro_memberstats',
                        'mlvl_stats',
                        'sector_stats',
                        'expertis_stats'
                    );
                }
                break;
            case 'pro_detail':
                global $redirect;
                $redirect = rebuild_url(array('tab', 'page'));
                $type = sget($_REQUEST, 'type');
                if ($type == 'discussion') {
                    $Page['Middle'] = array('pro_tabs', 'pro_discussion');
                    $Page['Right'] = array(
                        'pro_DiscussionCalendar'
                    );

                    $Page['Left'] = array('pro_name', 'pro_connections');
                }elseif ($type == 'my_projects'){

                    $Page['Middle'] = array('pro_tabs', 'pro_connections');
                    $Page['Right'] = array(
                        'pro_DiscussionCalendar'
                    );
                    $Page['Left'] = array('pro_name_no_data');

                } elseif ($type == 'details') {
                    $Page['Middle'] = array('pro_tabs', 'pro_details');
                    $Page['Right'] = array('discussion_join', 'pro_audio_preview', 'pro_participating', 'pro_share', 'pro_popularQuestions','pro_payment_stats');
                    $Page['Left'] = array('pro_name', 'pro_connections');
                } elseif ($type == 'fileview') {
                    $Page['Middle'] = array('pro_fileview');
                    $Page['Left'] = array('pro_name', 'pro_connections');
                } else {
                    $Page['Left'] = array('pro_name', 'pro_connections');
                    $Page['Middle'] = array(
                        'pro_tabs',
                        'pro_profile',
                        'pro_experties',
                        'pro_accomplishments',
                        'pro_personality',
                        'pro_industry_view',
                        'pro_mgmt_view'
                    );
                    if ($user_type == 'people') {
                        $id = sget($_REQUEST, 'id');
                        if ($id) {
                            if ($AccessObj != $id) {
                                $Page['Right'] = array('invest_activity', 'pro_gettingStarted', 'pro_selectWork', 'pro_eventCalendar', 'pro_service_payments_summary');
                            }
                        }
                    }
                    array_push($Page['Right'], 'pro_progress', 'invest_activity', 'pro_eventCalendar');
                }
                if ($user_type == 'company') {
                    $Page['Right'] = array();
                    if ($type == 'discussion') {
                        $Page['Right'] = array(
                            'pro_DiscussionCalendar'
                        );
                        $Page['Left'] = array('pro_name_no_data');
                    } elseif ($type == 'details') {
                        $Page['Right'] = array('discussion_join', 'pro_participating', 'pro_audio_preview', 'pro_share', 'pro_popularQuestions');
                    }
                }
                if ($user_type == 'admin' || $user_type == 'super') {
                    if ($_REQUEST['section'] == 'pro_industry_view' && ($_REQUEST['type'] == 'discussion' || $_REQUEST['type'] == 'details')) $get_st = ''; else $get_st = 'pro_gettingStarted';
                    array_push($Page['Right'], $get_st, 'pro_service_payments_summary', 'admin_section_title','pro_insights_stats', 'manage_connections', 'pro_vetting');
                }
                if ($user_type == 'super') {
                    array_push($Page['Right'], 'assign_admin');
                }
                //Prevent showing left column when Insights details is shown (Liliane pointed insights details to pro_detail page)
                if ($_REQUEST['section'] == 'pro_industry_view' && $_REQUEST['type'] == 'details' && (isset($_REQUEST['pro_id']) || isset($_REQUEST['files']))) {
                    $Page['Left'] = array('insight_sort_left_panel');
                }
                if ($_REQUEST['section'] == 'pro_industry_view' && $_REQUEST['type'] == 'discussion') {
                    $Page['Left'] = array('insight_sort_left_panel');
                }
                $ifAdminProfile = getUserTypeById($_REQUEST['id']);
                if (in_array($ifAdminProfile, array('admin', 'super_admin'))) $Page ['Right'] = array();
                break;
            case "insights":
                $type = sget($_REQUEST, 'type');
                $tab = sget($_REQUEST, 'tab');
                $page = sget($_REQUEST, 'page');

                if ($tab == 'information' && $page == 'insights') {

                    if ($AccessObj->user_type != 'super') {
                        return '';
                    }
                    $Page['Middle'] = array("infoTabs", 'info_list', 'related_list');
                    $Page["Right"] = array('upload_box', 'maenna_selection', 'recent_topics', 'admin_approve', 'info_tools');
                } else {
                    if ($type == 'following') {
                        $Page['Left'] = array('pro_DiscussionMyInsights');
                        $Page['Middle'] = array('pro_tabs', 'myfollowing');
                        $Page['Right'] = array('pro_DiscussionCalendar');
                    } else if ($type == 'registered') {
                        $Page['Left'] = array('pro_DiscussionMyInsights');
                        $Page['Middle'] = array('pro_tabs', 'myfollowing');
                        $Page['Right'] = array('pro_DiscussionCalendar');
                    } else if ($type == 'moderated') {
                        $Page['Left'] = array('pro_DiscussionMyInsights');
                        $Page['Middle'] = array('pro_tabs', 'myfollowing');
                        $Page['Right'] = array('pro_DiscussionCalendar');
                    } else {
                        $Page['Left'] = array('pro_DiscussionMyInsights');
                        $Page['Middle'] = array('pro_tabs', 'list_insights');
                        $Page['Right'] = array('pro_DiscussionCalendar');
                    }
                }
                break;
            case "admins":
                if (!(in_array($user_type, array('super')))) {
                    break;
                }
                $Page['Middle'] = array('admin_list');
                $Page['Right'] = array('add_admin_user');
                break;
            case 'admin_detail':
                if (!(in_array($user_type, array('super')))) {
                    break;
                }
                $Page['Middle'] = array('admin_user_info');
                $Page['Right'] = array();
                break;
            case 'settings':
                $Page['Middle'] = array('settings_instruction');
                $Page['Right'] = array('account_settings');
                if (in_array($user_type, array('super'))) {
                    array_unshift($Page['Right'], 'account_permissions');
                }
                if ($_REQUEST['section'] == 'account') $Page['Right'] = array();
                break;
            case 'questionnaire':
                $type = sget($_REQUEST, 'type');
                if (!in_array($AccessObj->user_type, array('company', 'people'))) {
                    return '';
                }
                $Page['Left'] = '';
                $Page['Middle'] = array('questionnaire');
                if ($type == 'accreditation') {
                    $Page['Middle'] = array('pro_accreditation');
                }
                break;
            case 'linkbank':
                $type = sget($_REQUEST, 'type');
                if (!in_array($AccessObj->user_type, array('company', 'people'))) {
                    return '';
                }
                $Page['Left'] = '';
                $Page['Middle'] = array('linkbank');
                break;
            case 'investments':
                $Page['Left'] = '';
                $Page['Middle'] = array('investments');
                break;
            case 'services':
                $Page['Left'] = array('');
                $Page['Middle'] = array('pro_tabs','services');
                $Page['Right'] = array('pro_DiscussionCalendar');
                break;
            case 'information':
                if ($AccessObj->user_type != 'super') {
                    return '';
                }
                $Page['Middle'] = array("infoTabs");
                $Page["Right"] = array('info_tools');
                break;
            case 'postings':
                if ($AccessObj->user_type != 'super') {
                    return '';
                }
                $Page['Middle'] = array('infoTabs', 'search_bar', 'posting_list');
                $Page["Right"] = array('info_tools', 'post_project', 'advisory_openings', 'project_openings');
                break;
            case 'qmgmt':
                if (!in_array($AccessObj->user_type, array('admin', 'super'))) {
                    return '';
                }
                $Page['Middle'] = array("infoTabs", 'qmgmt');
                $Page["Right"] = array('info_tools');
                if (sget($_REQUEST, 'panel') == 'answerdetail') {
                    array_unshift($Page['Right'], 'qmgmt_files');
                }
                break;
            case 'events':
                if ($AccessObj->user_type != 'super') {
                    return '';
                }
                $Page['Middle'] = array('infoTabs', 'search_bar', 'event_list');
                $Page["Right"] = array('info_tools', 'post_event_links');
                break;
            case 'research':
                if ($AccessObj->user_type != 'super' && $AccessObj->user_type != 'admin') {
                    return '';
                }
                $type = sget($_REQUEST, 'type');
                if (empty($type)) {
                    $type = 'company';
                }
                $Page['Middle'] = array("research_table");
                if ($type == 'company') {
                    $Page["Right"] = array(
                        'add_research_company',
                        'research_manager',
                        'key_statistics',
                        'recently_added',
                        'recently_updated',
                        'leads_by_sector',
                        'priority_stats',
                        'followup_stats',
                        'sector_stats'
                    );
                } else {
                    $Page['Right'] = array(
                        'add_research_people',
                        'recently_added_people',
                        'recently_updated_people',
                        'priority_stats_people',
                        'followup_stats_people'
                    );
                }
                break;
            case 'research_company_detail':
                $Page['Left'] = array(
                    'research_company_logo',
                    'research_company_brief_info',
                    'research_company_management',
                    'research_company_market_info',
                    'research_company_links'
                );
                $Page['Middle'] = array(
                    'research_company_name',
                    'research_company_financials',
                    'research_progress_status'
                );
                $Page['Right'] = array(
                    'research_company_followers',
                    'research_links_group',
                    'research_company_prospects',
                    'research_company_qualified_lead',
                    'research_followup_priority',
                    'research_companies_to_follow',
                    'research_company_articles'
                );
                break;
            case 'research_people_detail':
                $Page['Left'] = array(
                    'research_people_contact',
                    'research_people_work',
                    'research_people_education',
                    'research_people_links'
                );
                $Page['Middle'] = array(
                    'research_people_info',
                    'research_accomplishments',
                    'research_people_status'
                );
                $Page['Right'] = array(
                    'add_research_people',
                    'research_people_prospects',
                    'research_people_followup',
                    'research_people_articles'
                );
                break;
            case 'messages':
                $Page['Middle'] = array('messages_inbox');
                $Page['Right'] = array('messages_folders');
                break;
        }
        $Page['Right'] = array_unique($Page['Right']);
        return $Page;
    }

    function mae_com_sections() {
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
        while ($Row = db_fetch_array($result)) {
            $tab_id = sget($Row, 'tab_id');
            $tab_maenna_name = sget($Row, 'tab_maenna_name');
            $tab_title = sget($Row, 'tab_title');
            if ($prev != $tab_id) {
                $Sections["$tab_maenna_name"] = array(
                    'id'          => $tab_id,
                    'maenna_name' => $tab_maenna_name,
                    'title'       => $tab_title,
                    'sections'    => array()
                );
                $prev = $tab_id;
            }
            $section_name = sget($Row, 'maenna_name');
            $section_id = sget($Row, 'sectionid');
            $section_title = sget($Row, 'title');
            $section_panel = sget($Row, 'maenna_panel');
            $section_data_type = sget($Row, 'data_type');
            $position = sget($Row, 'position');
            $cu_edit = sget($Row, 'cu_edit');
            $Sections["$tab_maenna_name"]['sections']["$section_name"] = array(
                'id'        => $section_id,
                'title'     => $section_title,
                'panel'     => $section_panel,
                'data_type' => $section_data_type,
                'position'  => $position,
                'cu_edit'   => $cu_edit
            );
        }
        //Add commenting for analysis section
        $Sections["analysis"]['sections']['analysis_commenting'] = array(
            'id'        => 'analysis_comment',
            'title'     => 'Analysis commenting',
            'panel'     => '',
            'data_type' => '',
            'position'  => 'right',
            'cu_edit'   => ''
        );
        $Sections["share_knowledge"]['sections']['share_knowledge_commenting'] = array(
            'id'        => 'share_knowledge_comment',
            'title'     => 'Discuss project comments',
            'panel'     => '',
            'data_type' => '',
            'position'  => 'right',
            'cu_edit'   => ''
        );
        $Sections["share_knowledge"]['sections']['share_knowledge_add_advisor'] = array(
            'id'        => 'share_knowledge_add_advisor',
            'title'     => 'Add advisors',
            'panel'     => '',
            'data_type' => '',
            'position'  => 'right',
            'cu_edit'   => ''
        );
        $Sections["share_knowledge"]['sections']['share_knowledge_remove_interested'] = array(
            'id'        => 'share_knowledge_remove_interested',
            'title'     => 'Remove interested',
            'panel'     => '',
            'data_type' => '',
            'position'  => 'right',
            'cu_edit'   => ''
        );
        $_Tags = _TAGS();
        foreach ($_Tags as $key => $value) {
            $Sections["file"]['sections'][$key . "_1"] = array(
                'id'        => $key . "_1",
                'title'     => 'Own Files',
                'panel'     => '',
                'data_type' => '',
                'position'  => 'right',
                'cu_edit'   => ''
            );
            $Sections["file"]['sections'][$key . "_2"] = array(
                'id'        => $key . "_2",
                'title'     => 'Others Files',
                'panel'     => '',
                'data_type' => '',
                'position'  => 'right',
                'cu_edit'   => ''
            );
        }
        $Sections['about']['sections']["goals"] = array(
            'id'        => 1002,
            'title'     => Goals,
            'panel'     => '',
            'data_type' => '',
        );
        $Sections['common'] = array(
            'id'          => '',
            'maenna_name' => '',
            'title'       => 'Common Sections',
            'sections'    => array()
        );
        $sql = "select * from maenna_sections where type='common' and position = 'com_left'";
        $result = db_query($sql);
        while ($Row = db_fetch_array($result)) {
            $section_name = sget($Row, 'maenna_name');
            $section_id = sget($Row, 'sectionid');
            $section_title = sget($Row, 'title');
            $section_panel = sget($Row, 'maenna_panel');
            $Sections['common']['sections']["$section_name"] = array(
                'id'        => $section_id,
                'title'     => $section_title,
                'panel'     => $section_panel,
                'data_type' => $section_data_type,
            );
        }

        return $Sections;
    }

    function user_submit() {
        global $user;
        $section = sget($_REQUEST, 'section');
        $update_section = sget($_REQUEST, 'update_section');
        if ($update_section) {
            if ($this->AccessObj->_writable($section) && function_exists($update_section)) {
                $update_section('update');
            } else {
                user_error($this->AccessObj->user_type . " user {$user->uid} does not have write access to {$update_section}");
            }
        }
    }

    function render($location) {
        
        switch ($location) {
            case 'left':
                $Array = $this->Left;
                break;
            case 'middle':
                $Array = $this->Middle;
                // about_general
                // add_content
                break;
            case 'content':
                $Array = $this->Middle;
                break;
            case 'right':
                $Array = $this->Right;
                break;
            default:
                $Array = [];
                break;
        }

        if (count($Array) > 0) {
            if ($this->page_name == 'pro_detail') {
                foreach ($Array as $section) {
                    if (function_exists($section)) {
                        $mode = $this->AccessObj->pro_sections_access();
                        $Temp = $section($mode);
                        if ($Temp['body']) {
                            echo $Temp['body'];
                        }
                    }
                }
            } elseif ($this->page_name == 'company_detail') {
                foreach ($Array as $section) {

                    if (function_exists($section)) {
                        $mode = $this->AccessObj->section_mode($section);
                        if ($mode == 'hide') {
                            continue;
                        } elseif (empty($mode)) {
                            $mode = 'not being controlled';
                        }
                        $Temp = $section($mode);
                        if ($Temp['body']) {
                            if( ($this->AccessObj->user_type == 'super' && $section == 'admin_tools') || ($this->AccessObj->user_type == 'admin' && $section == 'manage_connections') ){
                                echo '<div id="administrative_dialog_modal" style="display:none;width:500px;">
                        		  <div style="padding:20px;">
                                    <style>
                                        /*h4 {font-size:130% !important;margin-top:10px;}
                                        a {color:#00a2bf !important;}*/
                                        .sidebar_box .wrapper {background:none;}
                                    </style>';
                            }
                            echo $Temp['body'];
                            if($section == 'com_premium'){
                                echo '</div></div>';
                            }
                        }
                    }
                }
            } else {
                foreach ($Array as $section) {
                    if (function_exists($section)) {
                        $Temp = $section('write');
                        if ($Temp['body']) {
                            echo $Temp['body'];
                        }
                    }
                }
            }
        }
    }

    function render_mtab() {
        global $AccessObj;
        $tab = sget($_REQUEST, 'mtab');
        if (empty($tab)) {
            foreach ($AccessObj->Com_sections as $tab_name => $Tab_data) {
                if ($Tab_data['access'] != 'hide') {
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

        $companyService = new \Clewed\Company\Service();
        $currentCompanyPermissions = $companyService->getPermissions($id);
        foreach ($mTabs as $key) {
            $access = $AccessObj->Com_sections["$key"]["access"];
            if ($access == 'hide')
                continue;

            if(empty($currentCompanyPermissions) || 'hide' == $currentCompanyPermissions[$key])
                continue;

            $active = '';
            $class = '';
            if ($key == $tab || ($tab == 'summary' && $key == 'analysis') || ($tab == 'share_knowledge_details' && $key == 'share_knowledge')) {
                $active = ' active-trail active';
            }
            if ($i == 0) {
                $class = 'first';
            } elseif ($i == (count($mTabs) - 1)) {
                $class = 'last';
            }

            if ($key == 'minutes') {
                $content .= "\n<li class=' $class $active '><a href='$redirect&mtab=$key' class='$active'>minutes</a></li>";
            } elseif ('share_knowledge' == $key) {
                $content .= "\n<li class=' $class $active '><a href='$redirect&mtab=$key' class='$active'>scope discussion</a></li>";
            } elseif ($key == 'questionnaire') {
                $content .= "\n<li class=' $class $active '><a href='$redirect&mtab=$key' class='$active'>due diligence</a></li>";
            } elseif ('advice' == $key) {
                $content .= "\n<li class=' $class $active '><a href='$redirect&mtab=$key' class='$active'>workspace</a></li>";
            } elseif ('monitoring' == $key) {
                $content .= "\n<li class=' $class $active '><a href='$redirect&mtab=$key' class='$active'>highlights</a></li>";
            }
                elseif ('file' == $key) {
                $content .= "\n<li class=' $class $active '><a href='$redirect&mtab=$key' class='$active'>data room</a></li>";
    }
            else {
                $content .= "\n<li class=' $class $active '><a href='$redirect&mtab=$key' class='$active'>$key</a></li>";
            }
            $i++;
        }
        $content = <<< END
        <div class='account-section-tabs' style='width: 980px !important; margin-left: 250px !important;'>
            <ul>
            $content
            </ul>
        </div>
END;
        return $content;
    }
    
    function account_tabs() {
        global $AccessObj;
        // Temporarily disable the tab professionals for the company user
        /*    if (isset($this->Tabs['company'])) {
              unset($this->Tabs['professionals']);
            }*/
        //Get right margin depending on the user type so tabs are aligned
        $utype = $AccessObj->user_type;
        if ($utype == 'people') {
            $rightm = 'style="right:54px !important;"';
        } elseif ($utype == 'company') $rightm = 'style="right:132px !important;"';
        $current_tab = sget($_REQUEST, 'tab');
        $current_page = $_REQUEST['project_type'];
        if (empty($current_tab)) {
            $current_tab = 'activities';
        }
        echo "<div class='account_tabs' $rightm>";
        echo "<ul class=\"menu\">";
        $class = '';
        if (($current_tab == 'companies' && isset($this->Tabs['company'])) ||
            ($current_tab == 'professionals' && isset($this->Tabs['profile'])) ||
            ($current_tab == 'settings') ||
            ($current_tab == 'questionnaire') ||
            ($current_tab == 'linkbank')
        ) {
            $class = 'active';
        }
        $profile = '';
        if (isset($this->Tabs['profile']) || isset($this->Tabs['company'])) {
            if (isset($this->Tabs['profile'])) {
                if ($this->Tabs['profile']) {
                    $title = strtolower($this->Tabs['profile']);
                } else {
                    $title = 'profile';
                }
                if ($current_tab == 'professionals' && $utype == 'people' && $_REQUEST['page'] == 'pro_detail' && $_REQUEST['section'] == 'pro_industry_view' && $_REQUEST['type'] == 'discussion' && $_REQUEST['openinsights'] == '1') $class = '';
                $profile .= "<li>
                    <a href='/account?tab=professionals&page=pro_detail&id=" . $this->AccessObj->uid . "' class='$class'>$title</a>";
                unset($this->Tabs['profile']);
            }
            if (isset($this->Tabs['company'])) {
                if ($this->Tabs['company']) {
                    $title = strtolower($this->Tabs['company']);
                } else {
                    $title = 'company';
                }
                $profile .= "<li><a href='/account?tab=companies&page=company_detail&id=" . $this->AccessObj->uid . "' class='$class'>$title</a>";
                unset($this->Tabs['company']);
            }
            $profile .= "<ul class=\"menu\">";
            if (isset($this->Tabs['settings'])) {
                if ($this->Tabs['settings']) {
                    $title = strtolower($this->Tabs['settings']);
                } else {
                    $title = 'settings';
                }
                $profile .= "<li><a href='/account?tab=settings&section=account&panel=account'>$title</a></li>";
                unset($this->Tabs['settings']);
            }
            if (isset($this->Tabs['questionnaire'])) {
                if ($this->Tabs['questionnaire']) {
                    $title = strtolower($this->Tabs['questionnaire']);
                } else {
                    $title = 'questionnaire';
                }
                $profile .= "<li><a href='/account?tab=questionnaire'>$title</a></li>";
                unset($this->Tabs['questionnaire']);
            }
            
            if (isset($this->Tabs['linkbank'])) {
                if ($this->Tabs['linkbank']) {
                    $title = strtolower($this->Tabs['linkbank']);
                } else {
                    $title = 'linkbank';
                }
                $profile .= "<li><a href='/account?tab=linkbank'>$title</a></li>";
                unset($this->Tabs['linkbank']);
            }
            $profile .= "<li class=\"last\"><a href=\"/logout\">Logout</a></li>";
            $profile .= "</ul>";
            $profile .= "</li>";
        } else {
            $profile = "<li class=\"last\"><a href=\"/logout\">Logout</a></li>";
        }
        foreach ($this->Tabs as $tab_key => $tab_title) {
            if ($tab_title) {
                $title = strtolower($tab_title);
            } else {
                $title = strtolower($tab_key);
            }
            $class = ''; $active_class = '';
            if ((strcasecmp($tab_key, $current_tab) == 0) ||
                ($current_tab == 'companies' && $tab_key == 'company') ||
                ($current_tab == 'professionals' && $tab_key == 'profile')
            ) {          
                $class = 'active';
                $active_class = '';
                if ($current_tab == 'companies' && $utype == 'people' && $_REQUEST['page'] == 'company_detail') $class = '';
                if ($current_tab == 'professionals' && $utype == 'company' && $_REQUEST['page'] == 'pro_detail') $class = ''; 
            }
            if ($current_page == 'fundraising') {
                $active_class = 'active';
                $class = '';
            }
            echo "\n";
            if ($tab_key == 'company') {
                echo "<li><a href='/account?tab=companies&page=company_detail&id=" . $this->AccessObj->uid . "' class='$class'>$title</a></li>";
            } elseif ($tab_key == 'profile') {
                echo "<li><a href='/account?tab=professionals&page=pro_detail&id=" . $this->AccessObj->uid . "' class='$class'>$title</a></li>";
            } elseif ($tab_key == 'customfilter' && $utype == 'people') {
                echo "<li><a href='/account?tab=companies&project_type=fundraising&industry=&revenue=&likes=&flag=1' class='$active_class'>$title</a></li>";
            } 
            elseif ($tab_key == 'companies' && $utype == 'people') {
                echo "<li><a href='/account?tab=companies&project_type=service&industry=&revenue=&likes=&others=' class='$class'><span data-tooltip=\"Services to connect with projects in your area of expertise. Connecting helps you gain more projects and attract buyers for your services.\">Services</span></a></li>";
            } 

            // elseif ($tab_key == 'myorders' && $utype == 'people') {
            //     echo "<li><a href='/account?tab=$tab_key' class='$class'>$title</a></li>";
            // } 
            elseif ($tab_key == 'investments' && $utype == 'people') {
                echo "<li><a href='/account?tab=$tab_key' class='$class'>$title</a></li>";
            } else {
                echo "<li><a href='/account?tab=$tab_key' class='$class'>$title</a></li>";
            }
        }
        echo $profile;
        echo "</ul>";
        echo "</div>";
    }

    function breadcrumb($level = null, $title = null, $link = null) {
        $array = array('tab', 'page', 'mtab', 'section');
        $Trail = array("<a href='/account'>Home</a>");
        if ($tab = sget($_REQUEST, 'tab')) {
            $link = rebuild_url(array('tab'));
            $title = ucwords($tab);
            $Trail[] = "<a href='$link'>$title</a>";
        }
        if ($page = sget($_REQUEST, 'page')) {
            $link = rebuild_url(array('tab', 'page', 'id'));
            $title = str_replace("_", ' ', $page);
            $title = ucwords($title);
            $Trail[] = "<a href='$link'>$title</a>";
        }
        if ($mtab = sget($_REQUEST, 'mtab')) {
            $link = rebuild_url(array('tab', 'page', 'mtab', 'id'));
            if ($mtab == 'minutes') {
                $mtab = 'advice';
            }
            $title = ucwords($mtab);
            $Trail[] = "<a href='$link'>$title</a>";
        }
        if ($section = sget($_REQUEST, 'section') && 0) {
            $link = rebuild_url(array('tab', 'page', 'mtab', 'section', 'id'));
            $title = ucwords($section);
            $Trail[] = "<a href='$link'>$title</a>";
        }
        echo "<div class='trail'>";
        echo implode("  &raquo; ", $Trail);
        echo "</div>";
    }

    function mae_com_sections_pro() {
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
        while ($Row = db_fetch_array($result)) {
            $tab_id = sget($Row, 'tab_id');
            $tab_maenna_name = sget($Row, 'tab_maenna_name');
            $tab_title = sget($Row, 'tab_title');
            if ($prev != $tab_id) {
                $Sections["$tab_maenna_name"] = array(
                    'id'          => $tab_id,
                    'maenna_name' => $tab_maenna_name,
                    'title'       => $tab_title,
                    'sections'    => array()
                );
                $prev = $tab_id;
            }
            $section_name = sget($Row, 'maenna_name');
            $section_id = sget($Row, 'sectionid');
            $section_title = sget($Row, 'title');
            $section_panel = sget($Row, 'maenna_panel');
            $section_data_type = sget($Row, 'data_type');
            $position = sget($Row, 'position');
            $cu_edit = sget($Row, 'cu_edit');
            $Sections["$tab_maenna_name"]['sections']["$section_name"] = array(
                'id'        => $section_id,
                'title'     => $section_title,
                'panel'     => $section_panel,
                'data_type' => $section_data_type,
                'position'  => $position,
                'cu_edit'   => $cu_edit
            );
        }
        $Sections['common'] = array(
            'id'          => '',
            'maenna_name' => '',
            'title'       => 'Common Sections',
            'sections'    => array()
        );
        $sql = "select * from maenna_sections where type='common' and position = 'com_left'";
        $result = db_query($sql);
        while ($Row = db_fetch_array($result)) {
            $section_name = sget($Row, 'maenna_name');
            $section_id = sget($Row, 'sectionid');
            $section_title = sget($Row, 'title');
            $section_panel = sget($Row, 'maenna_panel');
            $Sections['common']['sections']["$section_name"] = array(
                'id'        => $section_id,
                'title'     => $section_title,
                'panel'     => $section_panel,
                'data_type' => $section_data_type,
            );
        }
        $Sections['pro_info'] = array(
            'id'          => '',
            'maenna_name' => '',
            'title'       => 'Professional Info',
            'sections'    => array()
        );
        $Sections['pro_info']['sections']['edit_info'] = array(
            'id'        => '100',
            'title'     => 'Professional Info',
            'panel'     => 'pro_profile_panel',
            'data_type' => 'plan',
        );
        return $Sections;
    }

    public function render_before() {
        if (!empty($this->BeforeContent)) {
            return $this->BeforeContent;
        } else {
            return "";
        }
    }

    public function setBeforeContent($content) {
        $this->BeforeContent = $content;
        return $this;
    }
}
