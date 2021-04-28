<?php
$maenna_page['content'] = array('company_list');
function company_list($op = null)
{
    //$time_start = microtime(true);
    global $AccessObj;
    global $user;
    $Block['title'] = 'Companies';
    $SearchKeys = array(
        'company',
        'letter',
        'industry',
        'revenue_low',
        'revenue_high',
        'earnings_low',
        'earnings_high',
        'followup_from',
        'followup_to',
        'managerid',
        'state',
        'cmp_type',
        'ref_code'
    );
    $filter_cnt = 0;
    foreach ($SearchKeys as $key) {
        if (sget($_REQUEST, $key) === '')
            continue;
        $filter_cnt++;
    }
    //Edit company overlay handling
    if ($op && $op == 'write' && isset($_REQUEST['update_section']) && $_REQUEST['update_section'] == 'update_maenna_edit_company') {

        $dataid = sget($_REQUEST, 'dataid');
        if (empty($dataid)) {
            echo "Invalid data Id";
            return;
        }
        $company = sget($_REQUEST, 'edit_company');
        $sector = sget($_REQUEST, 'edit_industry');
        $projname = sget($_REQUEST, 'edit_project');
        $revenue = sget($_REQUEST, 'edit_revenue');
        $name = sget($_REQUEST, 'edit_name');
        $position = sget($_REQUEST, 'edit_position');
        $heard = sget($_REQUEST, 'edit_heard');
        $email = sget($_REQUEST, 'edit_email');
        $website = sget($_REQUEST, 'edit_website');
        $ref = sget($_REQUEST, 'edit_ref');
        $purpose = sget($_REQUEST, 'edit_purpose');

        $sql = "update maenna_company mc INNER JOIN maenna_company_data mcd on mc.companyid = mcd.companyid
                                         INNER JOIN users u ON mc.companyid = u.uid
        set mc.company = '%s', mc.sector = '%s', mc.contact = '%s', mc.position = '%s', mc.hear_about = '%s', mc.web = '%s', mc.referral_code = '%s', mc.invite_purpose = '%s', mc.projname = '%s', mcd.data_value = '%s', u.mail = '%s'
        where mc.companyid = %d
              and data_type = 'financial'
              and data_attr = (SELECT MAX(data_attr) from (select * from maenna_company_data where data_type = 'financial' and companyid = %d) as t limit 1)
              and u.uid =mc.companyid";

        if (db_query($sql, array($company, $sector, $name, $position, $heard, $website, $ref, $purpose, $projname, $revenue, $email, $dataid, $dataid))) {
            drupal_set_message("operation successful.");
        } else {
            drupal_set_message('Failed to update record', 'error');
        }

    }
    if ($op) {
        $content = ' ';
        if (in_array($AccessObj->user_type, array('admin'))) {
            $content .= type_company_tabs($AccessObj->user_type);
            $content .= company_search_tab($AccessObj->user_type);
            $content .= search_company($AccessObj->user_type);
        } elseif (in_array($AccessObj->user_type, array('super', 'sales'))) {
            $content .= company_search_tab($AccessObj->user_type);
            $content .= search_company($AccessObj->user_type);
        } elseif ($AccessObj->user_type == 'people') {
            $_SECTOR = _sectors();
            $_Industry = _INDUSTRY();
            $Project_Type = CompanyTypeData();
            $Block['title'] = '';
            $detail_array = array(
                'companyid' => array(strtoupper('Project Name'), 'tab_a', 360),
                'sector' => array(strtoupper('Industry'), 'tab_a', 200),
                'revenue' => array(strtoupper('Revenue'), 'tab_a', 100),
                'state' => array(strtoupper('State'), 'tab_a', 50),
                'collaborate' => array(strtoupper('Status'), 'tab_a', 120)
            );
            $sql_total = "select count(*) as total  from  users left join maenna_company
                            on users.uid = maenna_company.companyid
                            where users.uid = maenna_company.companyid and users.status = 1 and public = 1";
            if (!empty($_REQUEST['flag'])){
                $flag = $_REQUEST['flag'];
                $sql_total .= " AND maenna_company.stateable = $flag";
            }
            if (!empty($_REQUEST['project_type'])) {
                $project_type = $_REQUEST['project_type'] == 'service' ? 0 : 1;
                $sql_total .= " AND fundraising=" . $project_type;
            }
            $result_total = db_query($sql_total);
            $m = db_fetch_object($result_total);
            if ($m->total <= 0) {
                $Block['body'] = content_box($Block['title'], "No visible companies");
                // return $Block;
            }
            $canCollaborate = hasApprovedProfile($user->uid);
            $start = 0;
            $limit = 15;
            if (sget($_REQUEST, '_page'))
                if ($filter_cnt > sget($_REQUEST, 'filter_cnt')) $start = 0;
                else $start = (sget($_REQUEST, '_page') - 1) * $limit;

            $revenue_field = "(SELECT data_value FROM maenna_company_data mcd
			WHERE mcd.data_type = 'financial' AND mcd.data_value > 0 and mcd.companyid = maenna_company.companyid ORDER BY data_attr DESC LIMIT 1 ) as revenue";
            $earning_field = "(SELECT data_value2 FROM maenna_company_data mcd
			WHERE mcd.data_type = 'financial' AND mcd.data_value > 0 and mcd.companyid = maenna_company.companyid ORDER BY data_attr DESC LIMIT 1 ) as earning";
            $coll_tbl = "left join (select target_uid,status as collaboration
                                                    from maenna_connections WHERE assignee_uid = " . (int)$user->uid . " and status <> 'deactivated'" .
                ") as coll_tbl on coll_tbl.target_uid = maenna_company.companyid";
            $abt_tbl = "LEFT JOIN maenna_about ON maenna_company.companyid = maenna_about.project_id";
            $SQL = "select coll_tbl.collaboration as collaboration,$revenue_field,$earning_field,maenna_company.*, maenna_about.mission, maenna_about.project, IFNULL(likes.amount, 0) as likes, likes.users as liking_users from users, maenna_company $coll_tbl $abt_tbl LEFT JOIN (SELECT project_id, COUNT(la_id) as amount, GROUP_CONCAT(user_id) AS users FROM like_company GROUP BY project_id) as likes ON likes.project_id = maenna_company.companyid";
            $SQL .= " where users.uid = maenna_company.companyid and users.status = 1 and public = 1";
            if (!empty($_REQUEST['flag'])){
                $flag = $_REQUEST['flag'];
                $SQL .= " AND maenna_company.stateable = $flag";
            }
            // if (!empty($_REQUEST['project_type'])) {
            //     $project_type = $_REQUEST['project_type'] == 'service' ? 0 : 1;
            //     $SQL .= " AND maenna_company.fundraising=" . $project_type;
            // }
            if (!empty($_REQUEST['industry'])) {
                $industries = "'" . implode("', '", array_keys($_Industry[$_REQUEST['industry']])) . "'";
                $SQL .= " AND sector IN($industries)";
            }
            $SQL .= ' group by maenna_company.companyid';
            if (!empty($_REQUEST['revenue'])) {
                if (($_REQUEST['revenue'] % 2) == 1) {
                    $SQL .= " HAVING (revenue > " . ((int)$_REQUEST['revenue']) . ")";
                } else {
                    $SQL .= " HAVING (revenue IS NULL OR revenue < " . ((int)$_REQUEST['revenue']) . ")";
                }
            }
            if (empty($_GET['likes'])) {
                $SQL .= ' ORDER BY companyid DESC';
            } else {
                $SQL .= ' ORDER BY likes ' . (($_GET['likes'] == 'ASC') ? 'ASC' : 'DESC');
            }
            if ($_REQUEST['industry'] == '' && $_REQUEST['revenue'] == '' && $_GET['likes'] == '' && $m->total > 12) {
                $SQL .= ' LIMIT 12';
                $project_type = $_REQUEST['project_type'] == 'service' ? 0 : 1;
                $flag = $_REQUEST['flag'];
                $view_more = "<span total='" . $m->total . "' page='2' rel='show' can='" . $canCollaborate . "' project_type='" .$project_type."' flag='".$flag."' class='show_more_cmp'>view more</span>";
            }
            $result = db_query($SQL);
            $content .= '<div id="cardholder" style="width:999px!important;border-right: 1px solid;border-color: #D0D2D3;">';
            if($flag == '1') $content_text = "Open opportunities";
            else if($flag == '2') $content_text = "Coming Soon";
            else if($flag == '3') $content_text = "Past opportunities";
            else $content_text = "We are working to connect you with opportunities";
            $content .= '<div id="cardstitle">'.$content_text.'</div>';
            $i = 0;
            $projectIds = array();
            while (($row = db_fetch_array($result)) !== false)
                $projectIds[$projectId] = $projectId = (int)$row['companyid'];

            $companyService = new \Clewed\Company\Service();
            $teamSizes = $companyService->getTeamSize(array_keys($projectIds));
            mysql_data_seek($result, 0);
            if (!empty($result)) {
                if($flag == null || $flag == 3){
                    while (($row = db_fetch_array($result)) !== false) {
                        $i++;
                        $recordid = sget($row, 'companyid');
                        $link = rebuild_url(array('tab'));
                        if (!empty($row['project']) && file_exists('themes/maennaco/images/project/' . $row['project'])) {
                            $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/' . urlencode($row['project']) . '&zc=1&w=270&h=194'; // 191 143
                        } else {
                            $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/big-' . str_replace(' /themes/maennaco/images/', '', getAvatarUrl($row['companyid'])) . '&zc=1&w=270&h=194';
                        }
                        if(($row['stateable']) == 1) $satae_str = "Open";
                        if(($row['stateable']) == 2) $satae_str = "Coming Soon";
                        if(($row['stateable']) == 3) $satae_str = "Past";
                        $sectorRev = array();
                        if (!empty($row['sector'])) {
                            array_push($sectorRev, preg_replace('/(?<=\\w)(?=[A-Z])/', ' $1', $row['sector']));
                            $sectorStr = implode(' - ', $sectorRev);
                            if (strlen($sectorStr) > 13) {
                                $sectorStr = substr($sectorStr, 0, 10) . '...';
                            }
                        }
                        if (!empty($row['revenue'])) {
                            if ($row['revenue'] >= 1000000000)
                                $revenue = "Rev < $" . (int)($row['revenue'] / 1000000000) . " B";
                            else
                                $revenue = "Rev < $" . (int)($row['revenue'] / 1000000) . " M";
                        } else $revenue = '';
                        $cityState = array();
                        if (!empty($row['city']))
                            array_push($cityState, ucwords($row['city']));
                        if (!empty($row['state']))
                            array_push($cityState, strtoupper($row['state']));

                        $content .= '
                            <div class="card' . (!($i % 3) ? ' last' : '') . '">
                                <a href="' . "$link&page=company_detail&id=$recordid" . '">
                                <div class="projectName">' . getProjectName($row['companyid']) . '</div>
                                <div class="avatar"><img src="' . $avatar . '" width="100%"/></div>
                            ';
                        $content .= '<div class="sector-revenue"><span class="sector">' . $sectorStr . '</span><span class="revenue">' . $revenue . '</span></div>';
                        
                        $content .= '<div class="city-state">' . implode(', ', $cityState) . '</div>';
                        $content .= '<div class="mission" style="color:#686b83">' . substr(strip_tags($row['mission']), 0, 75) . (!empty($row['mission']) && strlen(strip_tags($row['mission'])) > 75 ? '...' : '') . '</div>';
                        $content .= '</a>';
                        $content .= '<div class="like-contrib" style="padding: 0 10px;">';
                        if($row['shareable']){
                            //ticket #874: detect connection status and set type of collaboration (collaborate||uncollaborate)
                            $sql_status='SELECT `status` FROM `maenna_connections` WHERE `assignee_uid` = '.$user->uid.' AND `target_uid` = '.$row['companyid'].' AND `conntype` = "collaborator" LIMIT 1';
                            $collaboratorStatus = db_query($sql_status);
                            $collaboratorStatus = db_fetch_array($collaboratorStatus);
                            $collaboratorStatus = $collaboratorStatus['status'];
//                            $content .= $collaboratorStatus;
                            if($collaboratorStatus==='deactivated'||$collaboratorStatus=='') {
                            $content .= '<span class="project-team-size"
                                            style="float: right; padding-left: 3px; color: #898b8e; cursor: pointer;"
                                            title= "Projects team size">'.(int) $teamSizes[$recordid].'</span>
                                        <span class="contrib">
                                            <a data-tooltip="
                                                    This tool allows you to connect to learn more and explore fit/qualifications privately. You must have related industry and/or operating expertise for services."
                                                type="collaborate"  cid="'.$row['companyid'].'" uid="'.$user->uid.'"
                                                style="cursor:pointer; color: #00A2BF; font-family:\'Lato Bold Italic\'; font-size: 14px;">Connect</a>
                                        </span>';
                            };
                            // if ($collaboratorStatus==='pending') {
                            // $content .= '<span class="project-team-size"
                            //                 style="float: right; padding-left: 3px; color: #898b8e; cursor: pointer;"
                            //                 title= "Projects team size">'.(int) $teamSizes[$recordid].'</span>
                            //             <span class="contrib">
                            //                 <a data-tooltip="
                            //                         Cancel your connection request."
                            //                     type="uncollaborate"  cid="'.$row['companyid'].'" uid="'.$user->uid.'"
                            //                     style="cursor:pointer; color: #00A2BF; font-family:\'Lato Bold Italic\'; font-size: 14px;">Pending</a>
                            //             </span>';
                            // };
                            if ($collaboratorStatus==='active') {
                            $content .= '<span class="project-team-size"
                                            style="float: right; padding-left: 3px; color: #898b8e; cursor: pointer;"
                                            title= "Projects team size">'.(int) $teamSizes[$recordid].'</span>
                                        <span class="contrib">
                                            <a data-tooltip="
                                                    Disconnect."
                                                type="uncollaborate"  cid="'.$row['companyid'].'" uid="'.$user->uid.'"
                                                style="cursor:pointer; color: #00A2BF; font-family:\'Lato Bold Italic\'; font-size: 14px;">Connected</a>
                                        </span>';
                            };
                        }
                        $content .= '</div>';

                        if($row['fundraising'] == 1){
                            $sql_total = "SELECT COUNT(*) AS counts,(CASE WHEN SUM(amount) > 0  THEN SUM(amount) WHEN SUM(amount) IS NULL THEN 0 END) AS SUM FROM maenna_professional_investment WHERE company_id = '".$recordid."' AND STATUS = 3";
                            $result_total = mysql_query($sql_total);
                            $row_amount = mysql_fetch_object($result_total);
                            $total_amount = $row_amount->SUM;
                            $committed_count = $row_amount->counts;
        
                            $sql_round_amount = "select round_amount_raising as rmount,close_date from maenna_company_data where companyid=%d and data_type='financial' and data_attr = ".date('Y')."";
                            $result_amount = db_query($sql_round_amount, array($recordid));
                            $row_round = db_fetch_object($result_amount);
                            $goal_amount = is_null($row_round->rmount) ? 0 : $row_round->rmount;
                            $close_date = is_null($row_round->close_date) ? '00/00/00' : $row_round->close_date;
                            if($total_amount>0 && $goal_amount>0){
                                $amount_raising_percent = $total_amount / $goal_amount * 100;
                                if($amount_raising_percent>20) $possible_percent = $amount_raising_percent;
                            }
                            $content .= '<div class="status_bars_wrapper" style="display:grid;padding:0px;margin-top:30px;font-size:12px">
                                <div class="status-bar-wrapper" style="width:290px;">
                                    <div class="status-bar"></div>
                                    <div class="status-bar-progress" style="width: '.number_format($amount_raising_percent, 0, '.', ',').'%"></div>
                                </div><div style="display: flex;margin-top: 5px;text-align: center;">';
                            if($goal_amount != null)
                            $content .='<div style="margin-left:0px">
                                        <div style="font-size: 12px;">$'.number_format($goal_amount, 0, '.', ',').'</div>
                                        <div style="text-align:left;width:108%;">Amount Rasing</div>
                                    </div><div style="height: 30px;background: #929497;width: 2px;margin-top: 6px;margin-left: 17px;"></div>';
                            if($amount_raising_percent != null)
                            $content .='<div style="margin-left:15px">
                                        <div style="font-size: 12px;">'.number_format($amount_raising_percent, 0, '.', ',').'%</div>
                                        <div>Funded</div>
                                    </div>
                                    <div style="height: 30px;background: #929497;width: 2px;margin-top: 6px;margin-left: 17px;"></div>';                                                   
                            if($close_date != null)
                            $content .='<div style="margin-left:15px">
                                        <div style="font-size: 12px;">'.$close_date.'</div>
                                        <div>Close Date</div>
                                    </div>';
                            $content .='</div></div>';
                        };
                        $content .= '</div>';
                        if (sget($_REQUEST, '_page'))
                            if ($filter_cnt > sget($_REQUEST, 'filter_cnt')) $page = 1;
                            else $page = sget($_REQUEST, '_page');
                        else $page = 1;
                    }
                } else {
                    while (($row = db_fetch_array($result)) !== false) {
                        $i++;
                        $recordid = sget($row, 'companyid');
                        $link = rebuild_url(array('tab'));
                        if (!empty($row['project']) && file_exists('themes/maennaco/images/project/' . $row['project'])) {
                            $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/' . urlencode($row['project']) . '&zc=1&w=270&h=194'; // 191 143
                        } else {
                            $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/big-' . str_replace(' /themes/maennaco/images/', '', getAvatarUrl($row['companyid'])) . '&zc=1&w=270&h=194';
                        }
                        $cityState = array();
                        if (!empty($row['city'])) array_push($cityState, ucwords($row['city']));
                        if (!empty($row['state'])) array_push($cityState, strtoupper($row['state']));
                        $sectorRev = array();
                        if (!empty($row['sector'])) {
                            array_push($sectorRev, preg_replace('/(?<=\\w)(?=[A-Z])/', ' $1', $row['sector']));
                            $sectorStr = implode(' - ', $sectorRev);
                            if (strlen($sectorStr) > 13) {
                                $sectorStr = substr($sectorStr, 0, 10) . '...';
                            }
                        }
                        if (!empty($rev['data_value'])) {
                            if($rev['data_value'] >= 1000000000 )
                                $revenue = "Rev : $" . (int) ($rev['data_value'] / 1000000000) . " B";
                            else
                                $revenue = "Rev : $" . (int) ($rev['data_value'] / 1000000) . " M";
                        } else $revenue = "Rev : $".(int)$rev['data_value']."";
                
                        $sql_round_amount = "select round_amount_raising as rmount,close_date,security_type from maenna_company_data where companyid=%d and data_type='financial' and data_attr = ".date('Y')."";
                        $result_amount = db_query($sql_round_amount, array($recordid));
                        $row_round = db_fetch_array($result_amount);
                        $goal_amount = is_null($row_round['rmount']) ? 0 : $row_round['rmount'];
                        $close_date = $row_round['close_date'];
                        $data_style = date("d/m/y", time($close_date));
                        $amount_raising_percent = $total_amount / $goal_amount * 100;
                        if($amount_raising_percent>20) $possible_percent = $amount_raising_percent;
                        if($goal_amount>0){
                            $gola_amount_style = '$' . number_format($goal_amount, 0, '.', ',');
                        }
                        $seq_type = $row_round['security_type'];
                        if($flag == '1') $state_str = "Open";
                        else if($flag == '2') $state_str = "Coming Soon";
                        else if($flag == '3') $state_str = "Past";
                        $content .= '
                            <div class="card-wrap">
                                <div class="card-inner" style="width: 250px;">
                                    <div class="projectName" style="text-align: left;display: block;">'. $row['projname'] .'</div>
                                    <a style="cursor:pointer;" href="/companies?id='.$row['companyid'].'">
                                        <img src='.$avatar.'" alt="'. $row['projname'] .'" width="100%" height="80%"/>
                                    </a>
                                    <div class="city-state" style="color:#898B8E;">'.implode(', ', $cityState).'</div>
                                </div>
                                <div class="card-inner" style="width: 750px;">
                                    <div style="display:flex;margin-left:25px;">                                
                                        <div class="sector-revenue" style="color:#898B8E; width:30%; margin-right:15%;">
                                            <span class="sector">Industry:'.$sectorStr.'</span>
                                        </div>
                                        <div style="max-width:30%;margin-right: 30%;color:#898B8E;"> 
                                            <span class="revenue">'.$revenue.'</span>
                                        </div>
                                        <div style="max-width: 30%;color:#898B8E;">';
                        if($row['shareable']){
                            //ticket #874: detect connection status and set type of collaboration (collaborate||uncollaborate)
                            $sql_status='SELECT `status` FROM `maenna_connections` WHERE `assignee_uid` = '.$user->uid.' AND `target_uid` = '.$row['companyid'].' AND `conntype` = "collaborator" LIMIT 1';
                            $collaboratorStatus = db_query($sql_status);
                            $collaboratorStatus = db_fetch_array($collaboratorStatus);
                            $collaboratorStatus = $collaboratorStatus['status'];
                            if($collaboratorStatus==='deactivated'||$collaboratorStatus=='') {
                            $content .= '<span>
                                            <a data-tooltip="This tool allows you to connect to learn more and explore fit/qualifications privately. You must have related industry and/or operating expertise for services."
                                                type="collaborate" cid="'.$row['companyid'].'" uid="'.$user->uid.'"
                                                style="cursor:pointer; color: #00A2BF; font-family:\'Lato Bold Italic\'; font-size: 14px;">
                                                Connect
                                            </a>
                                            </span>
                                            <span>'.(int) $teamSizes[$recordid].'</span>';
                            };
                            // if ($collaboratorStatus==='pending') {
                            // $content .= '<span>
                            //                 <a data-tooltip="Cancel your connection request"
                            //                     type="uncollaborate" cid="'.$row['companyid'].'" uid="'.$user->uid.'"
                            //                     style="cursor:pointer; color: #00A2BF; font-family:\'Lato Bold Italic\'; font-size: 14px;">
                            //                     Pending
                            //                 </a>
                            //                 </span>
                            //                 <span>'.(int) $teamSizes[$recordid].'</span>';
                            // }
                            if ($collaboratorStatus==='active') {
                            $content .= '<span>
                                            <a data-tooltip="Disconnect"
                                                type="uncollaborate" cid="'.$row['companyid'].'" uid="'.$user->uid.'"
                                                style="cursor:pointer; color: #00A2BF; font-family:\'Lato Bold Italic\'; font-size: 14px;">
                                                Connected
                                            </a>
                                            </span>
                                            <span>'.(int) $teamSizes[$recordid].'</span>';
                            }
                        }
                        $content .= '</div>
                                    </div>
                                    <a style="cursor:pointer;" href="/companies?id='.$row['companyid'].'">
                                        <div style="height:170px;width:620px;margin-left:25px;">
                                            <div class="subtitile" style="color:#686b83">'.$row['deal_summary_title'].'</div>                                    
                                            <div
                                                class="mission" style="font-size: 15px;font-family: Lato Light; color:#686b83;">'.substr(strip_tags($row["deal_summary_statement"]), 0, 150) . (!empty($row["deal_summary_statement"]) && strlen(strip_tags($row["deal_summary_statement"])) > 150 ? "&hellip;" : "").'
                                            </div>
                                        </div>
                                    </a>
                                    <div style="width:93%;margin-left:25px;">';
                        if($row['fundraising'] == 1){
                            $content .= '
                            <div class="status_bars_wrapper" style="margin-top:20px; width:70%;display:grid;padding:0px;">
                                <div class="status-bar-wrapper">
                                    <div class="status-bar"></div>
                                    <div class="status-bar-progress" style="width: '.number_format($amount_raising_percent, 0, '.', ',').'%"></div>
                                </div><div style="display:flex;margin-top: 5px;text-align: center;font-size: 12pt;font-family:Lato Italic; color: #284B5A;">';
                            if ($goal_amount>0)
                            $content .= '<div>
                                        <div style="display:flex;">'.$gola_amount_style.'</div>
                                        <div style="display:flex;">Amount raising</div>
                                    </div><div style="height: 35px;background: #929497;width: 2px;margin-top: 6px;margin-left: 40px;"></div>';                                            
                            if($seq_type != null) 
                            $content .='<div style="margin-left:45px">
                                        <div style="display:flex;">'.$seq_type.'</div>
                                        <div style="display:flex">Security Type</div>
                                    </div>
                                    <div style="height: 35px;background: #929497;width: 2px;margin-top: 6px;margin-left: 35px;"></div>';
                            if($data_style != null) 
                            $content .='
                                    <div style="margin-left:60px">
                                    <div style="display:flex;">'.$data_style.'</div>
                                    <div style="display:flex;">Launch Date</div>
                                </div>';
                            $content .=' </div></div>';
                        }
                        if($row['fundraising'] == 1) $top="null;";
                        else $top = "5px;";
                        $content .= '<div class="amount-button" style="margin-top:'.$top.';margin-left:74%;">
                        <a style="cursor:pointer;" href="/companies?id='.$row['companyid'].'">
                        <button class="oppor-btn">View Opportunity</button></a>
                                </div></div></div></div>';
                    }
                }
            }
            $content .= '<div style="clear:both;"></div>' . $view_more . '</div><!-- card holder-->';
            $revenues = array(
                '< $10 Million' => '10000000',
                '< $25 Million' => '25000000',
                '< $50 Million' => '50000000',
                '< $100 Million' => '100000000',
                '< $200 Million' => '200000000',
                '< $500 Million' => '500000000',
                '< $1 Billion' => '1000000000',
                '< $10 Billion' => '10000000000',
                '> $10 Billion' => '10000000001'
            );
            $revenueFilter = '<ul id="revenueFilter" class="filterOptions" style="' . (empty($_GET['revenue']) ? 'display: none;' : '') . '">';
            $revenueFilter .= "<li class='" . ($_GET['revenue'] == '' ? 'selected' : '') . "'><a class='filterOption' href='/account?tab=companies&project_type=" . $_GET['project_type'] . "&industry=" . $_GET['industry'] . "&likes=" . $_GET['likes'] . "'>All</a></li>";
            foreach ($revenues as $label => $value) {
                $revenueFilter .= "<li class='" . ($_GET['revenue'] == $value ? 'selected' : '') . "'><a class='filterOption' href='/account?tab=companies&project_type=" . $_GET['project_type'] . "&industry=" . $_GET['industry'] . "&revenue=$value&likes=" . $_GET['likes'] . "'>$label</a></li>";
            }
            $revenueFilter .= "</ul>";
            $industryFilter = '<ul id="industryFilter" class="filterOptions" style="' . (empty($_GET['industry']) ? 'display: none;' : '') . '">';
            $industryFilter .= "<li class='" . ($_GET['industry'] == '' ? 'selected' : '') . "'><a class='filterOption' href='/account?tab=companies&project_type=" . $_GET['project_type'] . "&revenue=" . $_GET['revenue'] . "&likes=" . $_GET['likes'] . "'>All</a></li>";
            foreach ($_Industry as $cluster => $industries) {
                if (empty($cluster)) continue;
                $industryFilter .= "<li class='" . ($_GET['industry'] == $cluster ? 'selected' : '') . "'><a class='filterOption' href='/account?tab=companies&project_type=" . $_GET['project_type'] . "&industry=$cluster&revenue=" . $_GET['revenue'] . "&likes=" . $_GET['likes'] . "'>$cluster</a></li>";
            }
            $industryFilter .= "</ul>";
            $likesFilter = '<ul id="likesFilter" class="filterOptions" style="' . (empty($_GET['likes']) ? 'display: none;' : '') . '">' .
                "<li class='" . ($_GET['likes'] == 'DESC' ? 'selected' : '') . "'><a class='filterOption' href='/account?tab=companies&project=" . $_GET['project_type'] . "&industry=" . $_GET['industry'] . "&revenue=" . $_GET['revenue'] . "&likes=DESC'>Most likes</a></li>" .
                "<li class='" . ($_GET['likes'] == 'ASC' ? 'selected' : '') . "'><a class='filterOption' href='/account?tab=companies&project=" . $_GET['project_type'] . "&industry=" . $_GET['industry'] . "&revenue=" . $_GET['revenue'] . "&likes=ASC'>Least likes</a></li>" .
                '</ul>';
            $othersFilter = '<ul id="othersFilter" class="filterOptions" style="' . (empty($_GET['others']) ? 'display: block;' : '') . '">' .
                "<li class='" . ($_GET['flag'] == '1' ? 'selected' : '') . "'><a class='filterOption' href='/account?tab=companies&project_type=" . $_GET['project_type'] . "&industry=" . $_GET['industry'] . "&revenue=" . $_GET['revenue'] . "&flag=1'>Open opportunities</a></li>" .
                "<li class='" . ($_GET['flag'] == '2' ? 'selected' : '') . "'><a class='filterOption' href='/account?tab=companies&project_type=" . $_GET['project_type'] . "&industry=" . $_GET['industry'] . "&revenue=" . $_GET['revenue'] . "&flag=2'>Coming Soon</a></li>" .
                "<li class='" . ($_GET['flag'] == '3' ? 'selected' : '') . "'><a class='filterOption' href='/account?tab=companies&project_type=" . $_GET['project_type'] . "&industry=" . $_GET['industry'] . "&revenue=" . $_GET['revenue'] . "&flag=3'>Past opportunities</a></li>" .
                '</ul>';
            // FILTER
            if($_GET['project_type'] == 'fundraising'){
                $content .= '
                <div id="cardfilter" style="border-left: 1px solid #D0D2D3; margin-left: -1px;">
                    <div class="title">Filter';
                        // foreach ($Project_Type as $type => $value) {
                        //     if (empty($type) || $type == 'fundraising') continue;
                        //     $content .= "<a class='filterParentNoChild " . ($_GET['project_type'] == $type ? 'selected' : '') . "' href='/account?tab=companies&project_type=" . $type . "&industry=" . $_GET['industry'] . "&revenue=" . $_GET['revenue'] . "&likes=" . $_GET['likes'] . "'>$value</a></li>";
                        // }
                    $content .= $othersFilter;
                    $content .= '</div><!-- eof cardfilter -->';
            }
            if($_GET['project_type'] != 'fundraising'){
                $content .= '
                <div id="cardfilter" style="border-left: 1px solid #D0D2D3; margin-left: -1px;">
                    <div class="title">Filter</div>';
                        // foreach ($Project_Type as $type => $value) {
                        //     if (empty($type) || $type == 'fundraising') continue;
                        //     $content .= "<a class='filterParentNoChild " . ($_GET['project_type'] == $type ? 'selected' : '') . "' href='/account?tab=companies&project_type=" . $type . "&industry=" . $_GET['industry'] . "&revenue=" . $_GET['revenue'] . "&likes=" . $_GET['likes'] . "'>$value</a></li>";
                        // }
                    $content .= '<a href="#" class="filterParent" data-for="#industryFilter">Industry</a>
                ' . $industryFilter . '
                    <a href="#" class="filterParent" data-for="#revenueFilter">Revenue</a>
                ' . $revenueFilter . '
                    <a href="#" class="filterParent" data-for="#likesFilter">Likes</a>
                ' . $likesFilter.'
                    <a href="#" class="filterParent" data-for="#othersFilter">Others</a>
                ' . $othersFilter;
                $content .= '</div><!-- eof cardfilter -->';
            }              
            $content .= '</tbody></table>';

            $deletePhpU = time();
            $deletePhpM = md5('delete.php:' . $deletePhpU . ':kyarata75');

            $content .= '
<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
<style>
.box_title {
    background:#ffffff !important;
    height:30px;
    line-height:30px;
    padding-left:15px;
}
.main_content {
    padding-top:0 !important;
}
.no-padd {
padding-right:5px !important;
}
</style>
<script type="text/javascript">
$(document).ready(function(){
    init_collaborate();
    init_sortPanel();

   $(".show_more_cmp").livequery("click",function(e){
    e.preventDefault();
    page = $(this).attr("page");
    total = $(this).attr("total");
    can = $(this).attr("can");
    project_type = $(this).attr("project_type");
    flag = $(this).attr("flag");
    console.log(flag);
    $.post("/themes/maennaco/includes/fetch_cmp.php?type=cmp_find&page=" + page + "&total="+total+"&can="+can+"&project_type="+project_type+"&flag="+flag, {
        u: "' . ($u = $user->uid) . '",
        n: "' . ($n = uniqid()) . '",
        t: "' . ($t = time()) . '",
        m: "' . md5("fetch_cmp.php:{$u}:{$n}:{$t}:kyarata75") . '"
    }, function(response) {
    $(".show_more_cmp").remove();
    $("#cardholder").append(response);
    });


    });
$(".filterParent").click(function(e){
    e.preventDefault();
    $($(this).data("for")).toggle();
});


$("#indSelect").change(function() {
    window.location.replace("/account?tab=companies&industry=" + $(this).val() + "&revenue=" + $("#revSelect").val());
});

$("#revSelect").change(function() {
    window.location.replace("/account?tab=companies&industry=" + $("#indSelect").val() + "&revenue=" + $(this).val());
});

    $(".like_unlike").livequery("click",function(e){
        e.preventDefault();
        var status = $(this).html() == "Like" ? 1 : 0;
        project_id = $(this).data("projectid");
        user_id = $(this).data("userid");

        self = this;
        $.ajax({
            type: "get",
            url: "/themes/maennaco/includes/delete.php?" +
                "type=like_company&" +
                 "project_id="+project_id+"&" +
                  "user_id="+user_id+"&" +
                  "status="+status+ "&" +
                  "u=' . $deletePhpU . '&" +
                  "m=' . $deletePhpM . '",
            success: function(resp) {
                $(self).html(status == 1 ? "Unlike" : "Like");
                counter = $(self).next();
                counter.html( status == 1 ? parseInt(counter.html(), 10) + 1 : parseInt(counter.html(), 10) - 1);
            }
        })
    });

    function like_company(type, project_id, user_id)
    {
    	if(type == "like")
    		var status = 1;
    	else
    		var status = 0;

    	$.ajax({
    			type: "get",
    			url: "/themes/maennaco/includes/delete.php?" +
    			    "type=like_company&"+
    			    "project_id="+project_id+"&"+
    			    "user_id="+user_id+"&"+
    			    "status="+status+"&"+
    			    "u=' . $deletePhpU . '&"+
                    "m=' . $deletePhpM . '" ,
    			data: "",
    			beforeSend: function(){	},
    			success: function(){
    				        counter = $("counter_" + project_id);
    						if(type == "like")
    						{
    							$("#like_company_" + project_id).html("Unlike");
    							$("#like_company_" + project_id).attr("onclick", "like_company(\"unlike\", "+project_id+","+user_id+");");
    							counter.html( parseInt(counter.html(), 10) + 1 );
    						}
    						else
    						{
    						    $("#like_company_" + project_id).html("Like");
    							$("#like_company_" + project_id).attr("onclick", "like_company(\"like\", "+project_id+","+user_id+");");
    							counter.html( parseInt(counter.html(), 10) - 1 );
    						}
    					}
    		});
    }

});
</script>';

        }
        //$time_end = microtime(true);
        //$content.= $time_end - $time_start;
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}

function type_company_tabs($utype)
{

    $ctab = sget($_REQUEST, 'ctab');

    if ($ctab == '' || $ctab == '2') {
        $active1 = 'first active-trail active';
        $ctab = 2;
    } else {
        $active = 'first active-trail active';
    }

    $content = '<div class="account-section-tabs" style="margin:20px 0px 0px 0px;width:250px;">
            <ul>
<li class="' . $active . '"><a class="" href="/account?tab=companies&ctab=1">Registered companies</a></li>
<li class="' . $active1 . '"><a style="text-transform:none;" class="" href="/account?tab=companies&ctab=2">Assigned companies</a></li>';
    $content .= '</ul></div><br><br>';

    return $content;

}

function company_search_tab($utype)
{
    //company search box
    if (sget($_REQUEST, 'company')) $company_pam = sget($_REQUEST, 'company');
    $td_company = "<input type=text name='company' placeholder='Company' value='$company_pam'>";
    //letter selection
    $Letters = "abcdefghijklmnopqrstuvw";
    $section = "";
    $SearchKeys = array(
        'company',
        'letter',
        'industry',
        'revenue_low',
        'revenue_high',
        'earnings_low',
        'earnings_high',
        'followup_from',
        'followup_to',
        'managerid',
        'state',
        'cmp_type',
        'ref_code'
    );
    $filter_cnt = 0;
    foreach ($SearchKeys as $key) {
        if (sget($_REQUEST, $key) === '')
            continue;
        $filter_cnt++;
    }
    for ($i = 0; $i < strlen($Letters); $i++) {
        $l = strtoupper($Letters[$i]);
        $section .= "<a href='#' class='letter_selector' data='$l'>$l</a>&nbsp;";
    }
    $letter = sget($_REQUEST, 'letter') ? sget($_REQUEST, 'letter') : '';
    $section .= "<a href='#' class='letter_selector' data=x style='letter-spacing:0;'>XYZ</a>";
    $td_letters = "<input type=hidden name=letter id='letter' value='$letter' />$section";
    //////////////////////////
    // industry
    $industry = sget($_REQUEST, 'industry');
    $option_industry = "<option value=''>Industry</option>" . OPTION_INDUSTRY($industry);
    $td_industry = "<select name=industry>$option_industry</industry>";
    // revenue low
    $revenue_min = sget($_REQUEST, 'revenue_low');
    $option_revenueLow = RevenuesOption($revenue_min, true);
    $td_rev_low = "<select name=revenue_low><option value=''>Revenue Low</option>" . $option_revenueLow . "</select>";
    // earnings low
    $earnings_low = sget($_REQUEST, 'earnings_low');
    $option_earningslow = EarningsOption($earnings_low);
    $td_earnings_low = "<select name='earnings_low'><option value=''>Earnings Low</option>$option_earningslow</select>";
    //follow up
    $followup = (sget($_REQUEST, 'followup_from')) ? sget($_REQUEST, 'followup_from') : '';
    $td_followup_from = "<input type=text name=followup_from value='$followup' placeholder = 'FollowUp - From' class='datepicker' />";
    ////////////////
    // relationship manager
    if ($utype == 'super') {
        $Managers = Mae_users::all_admins();
        array_unshift($Managers, array('uid' => 'none', 'first_name' => '*not assigned', 'last_name' => ''));
        $options_manager = '';
        $posted_managerid = sget($_REQUEST, 'managerid');
        foreach ($Managers as $Admin) {
            $selected = '';
            $admin_id = sget($Admin, 'uid');
            $name = ucwords(sget($Admin, 'first_name')) . " " . ucwords(sget($Admin, 'last_name'));
            if (($admin_id == $posted_managerid) && $posted_managerid) $selected = "selected";
            $options_manager .= "<option $selected value='$admin_id'>$name</option>\n";
        }
        $td_rel_manager = "<select name=managerid><option  value=''>Relationship Manager</option><option></option>$options_manager</select>";
    } else {
        $td_rel_manager = '';
    }
    // State selection
    $state = sget($_REQUEST, 'state');
    $option_industry = Options_state($state);
    $td_states = "<select name=state><option value=''>State</option>$option_industry</select></td>";
    //max revenue
    $revenue_high = sget($_REQUEST, 'revenue_high');
    $option_revenueHigh = RevenuesOption($revenue_high, true);
    $td_rev_high = "<select name=revenue_high><option  value=''>Revenue High</option>" . $option_revenueHigh . "</select>";
    // earnings high
    $earnings_high = sget($_REQUEST, 'earnings_high');
    $option_earningshigh = EarningsOption($earnings_high);
    $td_earnings_high = "<select name='earnings_high'><option value=''>Earnings High</option>$option_earningshigh</select>";
    //Services selection
    $service_sel = sget($_REQUEST, 'service_sel');
    $option_service_sel = ServicesOption($service_sel);
    $td_service_sel = "<select name='service_sel'><option value=''>Services</option>$option_service_sel</select>";
    //follow up + cmp_type + submit button
    $followup_to = (sget($_REQUEST, 'followup_to')) ? sget($_REQUEST, 'followup_to') : '';
    $td_followup_to = "<input type=text name=followup_to value='$followup_to' placeholder = 'FollowUp - To' class='datepicker' />";
    $cmp_type = sget($_REQUEST, 'cmp_type');
    $option_cmp_type = CompanyTypeOption($cmp_type);
    $td_cmp_type = "<select name='cmp_type'><option value=''>Company Type</option>$option_cmp_type</select>";
    if (sget($_REQUEST, 'ref_code')) $ref_code = sget($_REQUEST, 'ref_code');
    $td_ref_code = "<input type=text name=ref_code placeholder='Referral Code' value='$ref_code' />";
    $submit_btn = "<input type=submit value=Filter class='button' id='filter_btn' />";
    $info_tab = sget($_REQUEST, 'info_tab');
    if (empty($info_tab)) $info_tab = 'tab_a';
    if (sget($_REQUEST, '_page'))
        if ($filter_cnt > sget($_REQUEST, 'filter_cnt')) $_page = 1;
        else $_page = sget($_REQUEST, '_page');
    else $_page = 1;
    $cmpType = sget($_REQUEST, 'cmpType');
    $cmpStatus = sget($_REQUEST, 'cmpStatus');
    $html = <<< END
        <div class="company-fixed-filters"><form method=get action='/account' id='search_form'>
            <table cellspacing=0 cellpadding=0 class='search-tabs' style='width:80%'>
                <tr>
                    <td>$td_company</td>
                    <td colspan="4" style="padding-top:8px">$td_letters</td>
                </tr>
                <tr>
                    <td style='width:20%'>$td_industry</td>
                    <td style='width:20%'>$td_rev_low</td>
                    <td style='width:20%'>$td_earnings_low</td>
                    <td style='width:20%'>$td_followup_from</td>
                    <td style='width:20%'>$td_cmp_type</td>
                    <td style='text-align:right;padding-right:4px;'>$td_ref_code</td>
                </tr>
                <tr>
                    <td>$td_states</td>
                    <td>$td_rev_high</td>
                    <td>$td_earnings_high</td>
                    <td>$td_followup_to</td>
                    <td style='width:20%;padding-right:0'>$td_rel_manager</td>
                    <td style='padding-right:4px;'>$td_service_sel</td>

                </tr>
                <tr>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td style='width:20%;padding-right:0'></td>
                    <td>$submit_btn</td>
                </tr>
            </table>
            <input type=hidden name=tab value='companies' />
            <input type=hidden name='admin_action' id='admin_action' value='' />
            <input type=hidden name='selected_records' id='selected_records' value='' />
            <input type=hidden name='admin_id' id='admin_id' value='' />
            <input type=hidden name='info_tab' id='info_tab' value='$info_tab' />
            <input type=hidden name='cmpType' id='cmpType' value='$cmpType' />
            <input type=hidden name='cmpStatus' id='cmpStatus' value='$cmpStatus' />
            <input type=hidden name='_page' id='_page' value='$_page' />
            <input type=hidden name='filter_cnt' id='filter_cnt' value='$filter_cnt' />
            <input type=hidden name='update_section' value='com_update_section' />
            <input type=hidden name='reject_company' id='reject_company' value='' />
        </form>
<script type='text/javascript'>
$(document).ready(function(){
    init_search_selector();
    init_select_all();
    init_datepicker();
    init_search_pagination();
    init_assign_admin();
    init_set_public();
})
</script>
END;
    return $html;
}

function search_company($user_type)
{
    global $AccessObj;
    $ctab = sget($_REQUEST, 'ctab');

    if ($ctab == '' || $ctab == '2') {
        $active1 = 'first active-trail active';
        $ctab = 2;
    } else {
        $active = 'first active-trail active';
    }

    $SearchKeys = array(
        'company',
        'letter',
        'industry',
        'revenue_low',
        'revenue_high',
        'earnings_low',
        'earnings_high',
        'followup_from',
        'followup_to',
        'managerid',
        'state',
        'cmp_type',
        'ref_code'
    );
    $SearchValues = array();
    foreach ($SearchKeys as $key) {
        if (sget($_REQUEST, $key)) {
            $SearchValues["$key"] = sget($_REQUEST, $key);
        }
    }
    $filter_cnt = count($SearchValues);
    $SQL_Where = array();
    $followup_from = '';
    $followup_to = '';
    foreach ($SearchValues as $key => $val) {
        switch ($key) {
            case 'company':
                if (strcasecmp($val, 'company') == 0) {
                    $val = '';
                } else {
                    $val = "(
                        maenna_company.companyid = '" . ($val) . "' OR
                        maenna_company.projname like '%%" . ($val) . "%%' OR
                        maenna_company.pseudo_name like '%%" . ($val) . "%%' OR
                        maenna_company.company like '%%" . ($val) . "%%' OR
                        maenna_company.contact like '%%" . ($val) . "%%' OR
                        maenna_company.email like '%%" . ($val) . "%%'
                    ) ";
                }
                break;
            case 'industry':
                $val = "maenna_company.sector = '$val'";
                break;
            case 'revenue_low':
                $val = "financial_tbl.revenue >= $val";
                break;
            case 'revenue_high':
                $val = "financial_tbl.revenue <= $val";
                break;
            case 'earnings_low':
                $val = "financial_tbl.earning >= $val";
                break;
            case 'earnings_high':
                $val = "financial_tbl.earning <= $val";
                break;
            case 'followup_from':
                if (strcasecmp($val, '') == 0) {
                    $val = '';
                } else {
                    $val = strtotime($val);
                    $followup_from = $val;
                    $val = "followup_tbl.followup >= '$val'";
                }
                break;
            case 'followup_to':
                if (strcasecmp($val, '') == 0) {
                    $val = '';
                } else {
                    $val = strtotime($val);
                    $followup_to = $val;
                    $val = "followup_tbl.data_attr <= '$val'";
                }
                break;
            case 'state':
                if (strcasecmp($val, 'state') != 0) {
                    $val = "maenna_company.state = '$val'";
                } else {
                    $val = '';
                }
                break;
            case 'letter':
                $val = "maenna_company.projname like '" . ($val) . "%'";
                break;
            case 'managerid':
                if ($val) {
                    if ($val == 'none') {
                        $val = "(managerid = '' or managerid is null)";
                    } else $val = "managerid = '$val'";
                }
                break;
            case 'cmp_type':
//                if ($val) {
//                    if ($val == 'none') {
//                        $val = "(membership = '' or membership is null)";
//                    } else $val = "membership = '$val'";
//                }
                if ($val) {
                    if ($val == 'fundraising') $val = "fundraising = 1";
                    else $val = "fundraising = 0";
                }
                break;
            case 'ref_code':
                if (strcasecmp($val, 'referral code') == 0) {
                    $val = '';
                } else {
                    $val = "maenna_company.referral_code like '%%" . ($val) . "%%' ";
                }
        }
        if (!empty($val)) $SQL_Where["$key"] = $val;
    }
    $SQL_Where[] = 'users.uid = maenna_company.companyid and users.status = 1';
    // $SQL_Where[] = 'users.status = 1 and maenna_company.`projname` != ""';
    // to limit admin's view to assigned company only
    if ($user_type == 'admin' && $ctab == '2') $SQL_Where[] = "managerid = '" . $AccessObj->uid . "'";
    if (($user_type == 'admin' && $ctab == '1') || $user_type == 'sales') $SQL_Where[] = "maenna_company.active = 1";
    if ($_REQUEST['cmpType'] != '') $SQL_Where[] = "membership = '" . $_REQUEST['cmpType'] . "'";
    if ($_REQUEST['cmpStatus'] == 'visible') $SQL_Where[] = "public  = 1";
    if ($_REQUEST['cmpStatus'] == 'deactivated') $SQL_Where[] = "maenna_company.active = 0";
    if ($_REQUEST['cmpStatus'] == 'featured') $SQL_Where[] = "maenna_company.featured = 1";
    if ($_REQUEST['cmpStatus'] == 'insight') $SQL_Where[] = "exists (select user_id from maenna_professional_payments mpp where user_id = maenna_company.companyid)";
    if ($_REQUEST['cmpStatus'] == 'fundraising') $SQL_Where[] = "maenna_company.fundraising = 1";
    if ($_REQUEST['cmpStatus'] == 'started_fundraising') $SQL_Where[] = "maenna_company.started_fundraising = 1";
    if ($_REQUEST['cmpStatus'] == 'opening') $SQL_Where[] = "maenna_company.stateable = 1";
    if ($_REQUEST['cmpStatus'] == 'coming') $SQL_Where[] = "maenna_company.stateable = 2";
    if ($_REQUEST['cmpStatus'] == 'pasting') $SQL_Where[] = "maenna_company.stateable = 3";
    // most recent year financial data in database
    $revenue_field = "(SELECT data_value FROM maenna_company_data mcd
			WHERE mcd.data_type = 'financial' AND mcd.data_value > 0 and mcd.companyid = maenna_company.companyid ORDER BY data_attr DESC LIMIT 1 ) as revenue";
    $earning_field = "( SELECT data_value2 FROM maenna_company_data mcd
			WHERE mcd.data_type = 'financial' AND mcd.data_value > 0 and mcd.companyid = maenna_company.companyid ORDER BY data_attr DESC LIMIT 1 ) as earning";
    $search_insert = '';
    if ($followup_from) $search_insert .= " and temp.data_attr >= '$followup_from' ";
    if ($followup_to) $search_insert .= "  and temp.data_attr <=  '$followup_to' ";
    $followup_table = "left join (
        SELECT companyid AS followupid, data_attr AS followup FROM maenna_company_data WHERE (data_type='followup' OR data_type='progress') AND data_attr != '' $search_insert GROUP BY followupid ORDER BY dataid DESC
    ) as followup_tbl on followup_tbl.followupid = maenna_company.companyid";
    $manager_table = " left join( select target_uid,
                                        assignee_uid as managerid
                                    from maenna_connections
                                    where
                                        maenna_connections.conntype = 'admin' and
                                        maenna_connections.status = 'active'
                                ) as manager_table on manager_table.target_uid = maenna_company.companyid";
    $abt_tbl = " LEFT JOIN maenna_about ON maenna_company.companyid = maenna_about.project_id";
    $financial_table = "left join (SELECT t1.companyid as fid,t1.data_value as revenue,t1.data_value2 as earning
                                     FROM maenna_company_data AS t1
                                       LEFT OUTER JOIN maenna_company_data AS t2
                                         ON t1.companyid = t2.companyid
                                            AND (t1.data_attr < t2.data_attr
                                                 OR (t1.data_attr= t2.data_attr AND t1.dataid < t2.dataid))
                                     WHERE t1.data_type = 'financial' and t2.companyid IS NULL
                                     GROUP BY fid,revenue,earning
                                    ) as financial_tbl on financial_tbl.fid = maenna_company.companyid";
    $collaborator_tbl = "LEFT JOIN (SELECT
                                       mcd.target_uid as colid,
                                       COUNT(*) as collaboratorscnt
                                     FROM maenna_connections mcd INNER JOIN users u ON mcd.assignee_uid = u.uid
                                     WHERE u.status = 1 and conntype = 'collaborator' AND mcd.status = 'active'
                                     GROUP BY colid ) as collaborator_tbl ON collaborator_tbl.colid = maenna_company.companyid";
    $connection_tbl = "LEFT JOIN (SELECT
               target_uid,
               SUM(IF(conntype = 'visible', 1, 0)) AS visiblecnt,
               SUM(IF(conntype = 'advisor', 1, 0)) AS advisorscnt,
               SUM(IF(conntype = 'client', 1, 0))  AS clientscnt
             FROM maenna_connections mc
               INNER JOIN users u ON u.uid = mc.assignee_uid
             WHERE mc.status = 'active' and u.status = 1 and conntype IN ('visible', 'client', 'advisor')
             GROUP BY target_uid) as connection_tbl ON connection_tbl.target_uid = maenna_company.companyid";
    $services_tbl = "LEFT JOIN (SELECT companyid,MAX(created) as srv_created FROM maenna_company_events GROUP BY companyid) as service_tbl ON service_tbl.companyid = maenna_company.companyid";
    
    $sql_total = "select count(*) as total  from users, maenna_company $manager_table $financial_table
                    where " . implode(' and ', $SQL_Where);

                    $result_total = db_query($sql_total);
    $m = db_fetch_object($result_total);
    if ($m->total <= 0) {
        return "no record found";
    }

    $start = 0;
    $limit = 25;
    if (sget($_REQUEST, '_page')) {
        if ($filter_cnt > sget($_REQUEST, 'filter_cnt')) {
            $start = 0;
        } else {
            $start = (sget($_REQUEST, '_page') - 1) * $limit;
        }
    }

    $SQL = "SELECT maenna_company.*, followup_tbl.*, manager_table.*,financial_tbl.*, collaborator_tbl.collaboratorscnt, connection_tbl.*, service_tbl.srv_created, users.uid, users.created, maenna_about.project, maenna_about.mission,
        (SELECT COUNT(*) FROM maenna_followers WHERE cid = maenna_company.companyid) as followercnt,
        (SELECT COUNT(*) FROM maenna_company_events mce WHERE companyid = maenna_company.companyid) as servicecnt
        FROM
            users,
            maenna_company
            ${followup_table}
            ${manager_table}
            ${financial_table}
            ${abt_tbl}
            ${collaborator_tbl}
            ${connection_tbl}
            ${services_tbl} 
        WHERE " . implode(' AND ', $SQL_Where);
    // $SQL .= " GROUP BY maenna_company.companyid ";
    if (isset($_REQUEST['service_sel']) && $_REQUEST['service_sel'] != '') {

        if ($_REQUEST['service_sel'] == 'top') {
            $SQL .= " ORDER BY servicecnt desc";
        } else {
            $SQL .= " ORDER BY service_tbl.srv_created desc";
        }
    } else {
        $SQL .= " ORDER BY active desc,users.created desc";
    }

    $SQL .= " LIMIT %d, %d";

    $result = db_query($SQL, array($start, $limit));
    $info_tab = sget($_REQUEST, 'info_tab');
    if (empty($info_tab)) $info_tab = 'tab_a';
    $class_tab_a = $class_tab_b = $class_tab_c = $class_tab_d = '';
    if ($info_tab === 'tab_a') {
        $class_tab_a = 'active';
    } elseif ($info_tab === 'tab_b' && $user_type === 'super') {
        $class_tab_b = 'active';
    } elseif ($info_tab === 'tab_c') {
        $class_tab_c = 'active';
    }


    if ($user_type == 'sales' || ($user_type == 'admin' && $ctab == '1')) {

        $detail_array = array(
            'companyid' => array('Project Name', '', 147),
            'sector' => array('Industry', 'tab_a', 147),
            'revenue' => array('Revenue', 'tab_a', 86),
            'state' => array('State', 'tab_a', 50),
            'creatednew' => array('Registered', 'tab_a', 90),
        );

    } else {
        $detail_array = array(
            'companyid' => array('Project Name', '', 210),
            'make_public' => array('<span class="color-swatch" data-tooltip="Visibility" style="background-color:#00a3bf; border:1px solid #00a3bf;"></span>', '', 20),
            'started_fundraising' => array('<span class="color-swatch" data-tooltip="Fundraising" style="background-color:#ffdb00; border:1px solid #ffdb00;"></span>', '', 20),
            'fundraising' => array('<span class="color-swatch" data-tooltip="Investing" style="background-color:#216dff; border:1px solid #216dff;"></span>', '', 20),
            'insight' => array('<span class="color-swatch" data-tooltip="Insight" style="background-color:#013241; border:1px solid #013241;"></span>', '', 20),
            'active' => array('<span class="color-swatch" data-tooltip="Deactivated" style="background-color:#939597; border:1px solid #939597;"></span>', '', 20),
            'featured' => array('<span class="color-swatch" data-tooltip="Featured" style="background-color:#970090; border:1px solid #970090;"></span>', '', 20),
            'shareable' => array('<span class="color-swatch" data-tooltip="Shareable" style="background-color:#c8e1ea ; border:1px solid #c8e1ea;"></span>', '', 20),
            'stateable' => array('', '', 20),
            // 'stateable' => array('<span class="color-swatch" data-tooltip="Open" style="background-color:#1eb508 ; border:1px solid #1eb508; width:9px; heigth:9px;"></span>', '', 20),
            // 'comingable' => array('<span class="color-swatch" data-tooltip="Coming Soon" style="background-color:#dc502b ; border:1px solid #dc502b; width:9px; heigth:9px;"></span>', '', 20),
            // 'pastable' => array('<span class="color-swatch" data-tooltip="Past" style="background-color:red ; border:1px solid red; width:9px; heigth:9px;"></span>', '', 20),
            'sector' => array('Industry', 'tab_a', 147),
            'revenue' => array('Revenue', 'tab_a', 86),
            'earning' => array('E%', 'tab_a', 50),
            'roe' => array('ROE', 'tab_a', 90),
            'membership' => array('Level', 'tab_a', 40),
            'mngmnt' => array('<span class="color-swatch" data-tooltip="Colleagues" style="background-color:#00234D; border:1px solid #00234D;"></span>', 'tab_a', 20),
            'advisors' => array('<span class="color-swatch" data-tooltip="Advisors" style="background-color:#366092; border:1px solid #366092;"></span>', 'tab_a', 20),
            'connected' => array('<span class="color-swatch" data-tooltip="Tagged" style="background-color:#95b3d7; border:1px solid #95b3d7;"></span>', 'tab_a', 20),
            'servicecnt' => array('<span class="color-swatch" data-tooltip="Services" style="background-color:#9995d7; border:1px solid #9995d7;"></span>', 'tab_a', 20),
            'collaborators' => array('<span class="color-swatch" data-tooltip="Interested" style="background-color:#b8cce4; border:1px solid #b8cce4;"></span>', 'tab_a', 20),
            'followercnt' => array('<span class="color-swatch" data-tooltip="Following" style="background-color:#DDEDFF; border:1px solid #DDEDFF;"></span>', 'tab_c', 20),
            'coll_reqs' => array('Requests', 'tab_a', 50),
            'admin_assign' => array('Assign Admin', 'tab_a', 0),
            'sector2' => array('Industry', 'tab_c', 150),
            'created' => array("Registered", 'tab_c', 90),
            'referal' => array("Referal Code", 'tab_c', 110),
            'followup' => array('Calendar', 'tab_c', 120),
            'spteam' => array('Sp. Proj. Team', 'tab_c', 0),
            'creatednew' => array('Registered', 'tab_b', 90),
            'company' => array("Name", 'tab_b', 200),
            'state' => array('State', 'tab_b', 50),
            'firstname' => array('First Name', 'tab_b', 100),
            'lastname' => array('Last Name', 'tab_b', 100),
            'email' => array('Email', 'tab_b',),
        );
        $html .= '<div class="searchtab"><a href="#" class="info_tab_btn ' . $class_tab_a . '" tab="tab_a">PROJECTS</a>&nbsp;&nbsp;&nbsp;';
        if ($user_type == 'super') {
            $html .= '<a href="#" class="info_tab_btn ' . $class_tab_b . '" tab="tab_b">CONTACTS</a>&nbsp;&nbsp;&nbsp;';
        }
        $html .= '<a href="#" class="info_tab_btn ' . $class_tab_c . '" tab="tab_c">ACTIVITIES</a>&nbsp;&nbsp;&nbsp;</div>';
    }
    if ($user_type == 'super') {
        $del_btn = '<a type=button class=tool id="delete_btn">Delete</a>';
        $vis_btn = '<a data-tooltip="This tool will make company visible to professionals." type=button class=tool id="public_btn" user="company">Make Visible</a>';
        $unq_btn = '<a data-tooltip="This tool will delete the company user and keep his email account to ensure same email is not registered again." type=button class=tool id="reject_btn" user="company">Disqualify</a>';
        $start_fundraising_btn = '<a data-tooltip="This tool will allow company to start fundraising process." type=button class=tool id="start_fundraising_btn" user="company">Toggle Fundraising</a>';
        $fundraising_btn = '<a data-tooltip="This tool will mark the company to show fundraising statistics." type=button class=tool id="fundraising_btn" user="company">Toggle Investing</a>';
        $ins_btn = '<a data-tooltip="This tool will mark the user as a company who registered to attend Insights." type=button class=tool id="insight_btn" user="company">Toggle Insight</a>';
        $act_btn = '<a data-tooltip="This tool will list the user at the bottom of the company list due to lack of activity."  type=button class=tool id="active_btn" user="company">Toggle Deacitve</a>';
        $feature_btn = '<a data-tooltip="This tool will mark the company as featured"  type=button class=tool id="companies_feature_btn" user="company">Toggle Feature</a>';
        $shareable_btn = '<a data-tooltip="This tool will mark the company as shareable"  type=button class=tool id="companies_shareable_btn" user="company">Toggle Connect</a>';
        $openpro_btn = '<a data-tooltip="This tool will mark the company as open"  type=button class=tool id="companies_openpro_btn" user="company">Toggle OpenProject</a>';
        $comingpro_btn = '<a data-tooltip="This tool will mark the company as coming soon"  type=button class=tool id="companies_comingpro_btn" user="company">Toggle ComingSoon</a>';
        $pastpro_btn = '<a data-tooltip="This tool will mark the company as past"  type=button class=tool id="companies_pastpro_btn" user="company">Toggle PastProject</a>';
        $adm_btn = '<a type=button class=tool id="assign_admin_btn">Submit</a>';
        $option_admins = option_code($AccessObj->All_admins);
        $value_adm = "\n<select id='select_adminid' style='width:333px;margin-left:10px;'>
          <option value='none'>Choose admin to assign project</option>
                $option_admins
                  </select>";
        $html .= '<div class="actions">'
                . '<div class="left-area">' . $del_btn . $vis_btn . $unq_btn .$start_fundraising_btn. $fundraising_btn . $ins_btn . $act_btn . $feature_btn . $shareable_btn . $openpro_btn.$comingpro_btn.$pastpro_btn.'</div>'
                . '<div class="right-area">' . $value_adm . $adm_btn . '</div>'
                . '</div>';
    } else
        $html .= '<table style="height:0px;" class="no-border actions"></table>';
    $html .= '</div>';
    $html .= "<div id=\"filter-table-spacer\"></div>";
    $html .= <<<DNE
            <script type="text/javascript">
            $('document').ready(function(){
             $("table.account-table table.left-align thead th").css('padding-right',0);
             $("table.account-table table.left-align thead th.no-padd").css('padding-right',5);
              $("body").append('<div id="sticky-head"><table class="no-border actions">'+$(".actions").html()+'</table><table id="sticky-head1"><thead>'+$("#proba-result thead").html()+'</thead></table></div>');
              var offset  = $("table.account-table table.left-align thead").offset();
              $('#sticky-head').css({
                position:'fixed',
                width: $("#proba-result thead").width(),
                top: 0, left: offset.left,
                display: 'none',
                margin: 0
              });
              $("div#sticky-head table.actions").css({
              'font-size': '10px',
              'font-weight': 'bold',
              'color':'#91939e',
              'text-transform': 'uppercase',
              'text-decoration': 'none',
              'font-family': 'LatoRegular',
              'background-color':'#fff',
              'margin':'0',
              'cursor':'pointer'
              });

              $("div#sticky-head table.actions a").click(function(){
                id = $(this).attr('id');
                $("#"+id).click();
              });

              $("div#sticky-head table.actions select").css({
              'height':'34px',
              'text-transform':'none',
              'font-style':'italic',
              'font-size':'14px'
              });

              $('#sticky-head1').css({
                background: '#94c9da',
                margin:0
              });
              $("#sticky-head1 thead th").css('padding-right',0);
              $("#sticky-head1 thead th.no-padd").css('padding-right',5);

              $(window).resize(function(){
                  offset = $("table.account-table table.left-align thead").offset();
                  $('#sticky-head').css({
                    left: offset.left
                  });
              });

              $(window).scroll(function(){
                if($(window).scrollTop() > (offset.top*1)){
                  $('#sticky-head').show();
                }else{
                  $('#sticky-head').hide();
                }
              });
            });
            </script>
DNE;
    $html .= '<table cellspacing="0" cellpadding="0" border="0" width="100%" id="proba-result" class="result-table left-align" style="width:1069px;">
                    <thead style="background:#94c9da;">
                        <th class="checkbox" style="width: 25px; text-align: left;" >
                            <input type=checkbox id="select_all" >
                        </th>
                        <th class="rownum" style="width: 25px; text-align: left;" >
                            #
                        </th>
                        ';
    foreach ($detail_array as $key => $Row) {
        if ($key == 'assign_admin' && $user_type != 'super') continue;
        $name = $Row[0];
        $class = $Row[1];
        if (!empty($class)) {
            if ($class != $info_tab) continue;
        }
        $width = '';
        if (!empty($Row[2])) {
            $width = "width:" . $Row[2] . "px;";
        }
        /*if ($key == 'make_public') $padd_class = 'no-padd'; else */
        $padd_class = '';
        $html .= "\n<th class='$padd_class' style='$width'>$name</th>";
    }
    $html .= "<th></th></tr></thead><tbody style='font-family:Lato Light'>";
    $class = '';
    $_SECTOR = _sectors();
    $_Industry = _INDUSTRY();

    require_once 'classes/connections.inc';

    if (!empty($result)) {
        $rownum = $start + 1;

        while (($row = db_fetch_array($result)) !== false) {
            $recordid = sget($row, 'companyid');
            $txt_align = '';
            $html .= "\n<tr><td><input type=checkbox value='$recordid' class='cb_record records'/'></td>";
            $html .= "<td>" . $rownum . "</td>";
            $rownum++;
            foreach ($detail_array as $key => $Row) {
                $value = sget($row, $key);
                //$Conns = Connections::Com_conns($recordid);
                if ($key != 'email') $value = ucwords($value);
                $tab = $Row[1];
                if (!empty($tab)) {
                    if ($tab != $info_tab) continue;
                }
                if ($key == 'assign_admin' && $user_type != 'super') continue;
                if ($key == 'company') {
                    $value = htmlentities($value);
                    $value = "<div style='width:200px;overflow:hidden;text-align:left;height:20px;' title='$value'>" . $value . "</div>";
                } elseif ($key == 'firstname') {
                    $value = ucfirst(array_shift(explode(' ', $row['contact'])));
                } elseif ($key == 'lastname') {
                    $value = ucfirst(array_pop(explode(' ', $row['contact'])));
                } elseif ($key == 'referal') {
                    $value = sget($row, 'referral_code');
                    $style = "width:30px;";
                } elseif ($key == 'created') {
                    $value = sget($row, 'created');
                    $value = date('m/d/Y', $value);
                    $style = "width:30px;";
                } elseif ($key == 'creatednew') {
                    $value = sget($row, 'created');
                    $value = date('m/d/Y', $value);
                    $style = "width:30px;";

                } elseif ($key == 'revenue') {
                    $txt_align = 'style="text-align:center;"';
                    if ($value) {
                        if ($value / 1000000000 >= 1)
                            $value = "$" . number_format($value / 1000000000) . 'B';
                        elseif ($value / 1000000 >= 1)
                            $value = "$" . number_format($value / 1000000) . 'M';
                        elseif ($value / 1000 >= 1)
                            $value = "$" . number_format($value / 1000) . 'K';
                        elseif (is_int($value))
                            $value = "$" . number_format($value);
                        else
                            $value = "-";
                    }

                } elseif ($key == 'earning') {
                    $txt_align = 'style="text-align:right;"';
                    $revenue = sget($row, 'revenue');
                    $revenue = filter_num($revenue);
                    $earning = filter_num($value);
                    if ($earning && $revenue) {
                        $value = number_format(($earning / $revenue) * 100, 1) . "%";
                    } else $value = '';
                    $style = "width:30px;";

                } elseif ($key == 'data_attr') {
                    if ($value) {
                        $value = date('m/d/Y', $value);
                    }

                } elseif ($key == 'sector' || $key == 'sector2' || $key == 'sector3') {
                    $value = sget($row, 'sector');
                    if ($value) {
                        if (strlen($value) <= 3) $value = $_SECTOR["$value"];
                        $value = preg_replace('/\B([A-Z])/', ' $1', $value);
                        $value = "<div style='width:120px;overflow:hidden;text-align:left;height:20px;' title='$value'>$value</div>";
                    }

                } elseif ($key == 'coll_reqs') {
                    $value = requestsNum($recordid);
                    $txt_align = 'style="text-align:center;"';
                    if ($value == 0) {
                        $value = '-';
                    } else {
                        $link = rebuild_url(array('tab'));
                        $value = "<a class='tool' style='font-size:12px;' href='$link&page=company_detail&id=$recordid#coll' >" . $value . "</a>";
                    }

                } elseif ($key == 'admin_assign') {
                    if ($user_type != 'super') continue;
                    $admin_id = sget($row, 'managerid');
                    $option_admins = option_code($AccessObj->All_admins, $admin_id);
                    $value = "\n<select class='select_assign_admin' name='admin_$recordid' style='width:100px;' target_uid='$recordid'><option value='none'></option>$option_admins</select>";

                } elseif ($key == 'followup') {
                    if ($value) $value = date('m/d/Y', $value);
                    else $value = '';
                    $style = "width:40px;";

                } elseif ($key == 'companyid2' || $key == 'companyid' || $key == 'companyid3') {
                    $value = sget($row, 'companyid');
                    $value = $title = getProjectName($value);
                    $value = ucwords(strtolower($value));
                    global $base_url;
                    $link = rebuild_url(array('tab'));
                    $color = "";
                    if (!empty($row['project']) && !empty($row['mission'])) {
                        $color = "color:black;";
                        $value = "<a style=\"$color\" href='" . $base_url . "$link&page=company_detail&id=$recordid&show_type=1' >" . $value . "</a>";
                    } else {
                        $value = "<a style=\"$color\" href='" . $base_url . "$link&page=company_detail&id=$recordid&mtab=about&view=edit&field=content' >" . $value . "</a>";
                    }
                    
                    $style = "width:170px;";
                    $value = "<div class='company-title' style='overflow:hidden;text-align:left;height:20px;' title='$title'>$value</div>";

                } elseif ($key == 'roe') {
                    $txt_align = 'style="text-align:right;"';
                    $equity = filter_num(sget($row, 'equity'));
                    $earning = filter_num(sget($row, 'earning'));
                    if ($equity && $earning) $value = number_format(($earning / $equity) * 100, 1) . '%';
                    else $value = '';
                    $style = "width:30px;";

                } elseif ($key == 'advisors') {
                    $txt_align = 'style="text-align:center;background-color:#F9FAFC;border-right:dotted 1px #DCE6F5;"';
                    //$value = count($Conns['Advisor']);
                    $value = sget($row, 'advisorscnt');
                    if (empty($value) || $value == 0) {
                        $value = '-';
                    }

                } elseif ($key == 'connected') {
                    $txt_align = 'style="text-align:center;background-color:#F9FAFC;border-right:dotted 1px #DCE6F5;"';
                    $value = sget($row, 'visiblecnt');
                    if (empty($value) || $value == 0) {
                        $value = '-';
                    }

                } elseif ($key == 'servicecnt') {
                    $txt_align = 'style="text-align:center;background-color:#F9FAFC;border-right:dotted 1px #DCE6F5;"';
                    $value = sget($row, 'servicecnt');
                    if (empty($value) || $value == 0) {
                        $value = '-';
                    }

                } elseif ($key == 'mngmnt') {
                    $txt_align = 'style="text-align:center;background-color:#F9FAFC;border-right:dotted 1px #DCE6F5;"';
                    //$value = count($Conns['Client']);
                    $value = sget($row, 'clientscnt');
                    if (empty($value) || $value == 0) {
                        $value = '-';
                    }

                } elseif ($key == 'collaborators') {
                    $value = sget($row, 'collaboratorscnt');
                    $txt_align = 'style="text-align:center;background-color:#F9FAFC;border-right:dotted 1px #DCE6F5;"';
                    /*$colls = getCollaborators($recordid);
                    $value = count($colls['active']);*/
                    if (empty($value) || $value == 0) {
                        $value = '-';
                    }

                } elseif ($key == 'followercnt') {
                    if ($user_type != 'super' && $user_type != 'admin')
                        continue;

                    $follCnt = $value;
                    if ($follCnt == 0)
                        $follCnt = '-';
                    $html .= '<td style="text-align:center;">' . $follCnt . '</td>';

                    continue;

                } elseif ($key == 'active') {
                    if (!in_array($user_type, array('super', 'admin'))) continue;
                    $txt_align = 'style="background-color:#F9FAFC;border-right:dotted 1px #DCE6F5;"';
                    if (sget($row, 'active') == 0) {
                        $value = '<span class="color-swatch" style="background-color:#939597; border:1px solid #939597;"></span>';
                    } else {
                        $value = '';
                    }
                } elseif ($key == 'featured') {
                    if (!in_array($user_type, array('super', 'admin'))) continue;
                    $txt_align = 'style="background-color:#F9FAFC;border-right:dotted 1px #DCE6F5;"';
                    if (sget($row, 'featured') == 1) {
                        $value = '<span class="color-swatch" style="background-color:#970090; border:1px solid #970090;"></span>';
                    } else {
                        $value = '';
                    }
                } elseif ($key == 'shareable') {
                    if (!in_array($user_type, array('super', 'admin'))) continue;
                    $txt_align = 'style="background-color:#F9FAFC;border-right:dotted 1px #DCE6F5;"';
                    if (sget($row, 'shareable') == 1) {
                        $value = '<span class="color-swatch" style="background-color:#c8e1ea; border:1px solid #c8e1ea;"></span>';
                    } else {
                        $value = '';
                    }
                } elseif ($key == 'stateable') {
                    if (!in_array($user_type, array('super', 'admin'))) continue;
                    if (sget($row, 'stateable') == 1) {
                        $value = '<span class="color-swatch" style="background-color:#1eb508; border:1px solid #1eb508; width:12px; heigth:12px;"></span>';
                    } 
                    else if (sget($row, 'stateable') == 2) {
                        $value = '<span class="color-swatch" style="background-color:orange; border:1px solid orange; width:12px; heigth:12px;"></span>';
                    }
                    else if (sget($row, 'stateable') == 3) {
                        $value = '<span class="color-swatch" style="background-color:red; border:1px solid red; width:12px; heigth:12px;"></span>';
                    } else {
                        $value = '';
                    }
                } elseif ($key == 'make_public') {
                    if (!in_array($user_type, array('super', 'admin'))) continue;
                    $txt_align = 'class="no-padd" style="background-color:#F9FAFC;border-right:dotted 1px #DCE6F5;"';
                    if (sget($row, 'public') == 1) {
                        $value = '<span class="color-swatch" style="background-color:#00a3bf; border:1px solid #00a3bf;"></span>';
                    } else {
                        $value = '';
                    }

                } elseif ($key == 'started_fundraising') {
                    if (!in_array($user_type, array('super', 'admin'))) {
                        continue;
                    }
                    $txt_align = 'style="background-color:#F9FAFC;border-right:dotted 1px #DCE6F5;"';
                    if (sget($row, 'started_fundraising') == 1) {
                        $value = '<span class="color-swatch" style="background-color:#FFDB00; border:1px solid #FFDB00;"></span>';
                    } else {
                        $value = '';
                    }

                } elseif ($key == 'fundraising') {
                    if (!in_array($user_type, array('super', 'admin'))) {
                        continue;
                    }
                    $txt_align = 'style="background-color:#F9FAFC;border-right:dotted 1px #DCE6F5;"';
                    if (sget($row, 'fundraising') == 1) {
                        $value = '<span class="color-swatch" style="background-color:#216dff; border:1px solid #216dff;"></span>';
                    } else {
                        $value = '';
                    }

                } elseif ($key == 'insight') {
                    if (!in_array($user_type, array('super', 'admin'))) continue;
                    $txt_align = 'class="no-padd" style="background-color:#F9FAFC;border-right:dotted 1px #DCE6F5;"';
                    if (sget($row, 'insight_came_flag') == 1) {
                        $value = '<span class="color-swatch" style="background-color:#013241; border:1px solid #013241;"></span>';
                    } else {
                        $value = '';
                    }

                } elseif ($key == 'membership') {
                    $txt_align = 'style="text-align:center;background-color:#F9FAFC;border-right:dotted 1px #DCE6F5;"';
                    switch ($value) {
                        case 'Level1':
                            $value = 'L1';
                            break;
                        case 'Level2':
                            $value = 'L2';
                            break;
                        case 'Level3':
                            $value = 'L3';
                            break;
                        case 'Level4':
                            $value = 'L4';
                            break;
                        case 'Level5':
                            $value = 'L5';
                            break;
                        case 'Level6':
                            $value = 'L6';
                            break;
                        case 'Level7':
                            $value = 'L7';
                            break;
                        case 'Level8':
                            $value = 'L8';
                            break;
                        case 'Level9':
                            $value = 'L9';
                            break;
                        case 'Level10':
                            $value = 'L10';
                            break;
                        case 'Regular':
                            $value = 'Rg';
                            break;
                        case 'Premium':
                            $value = 'Pr';
                            break;
                    }
                }
                $html .= "\n<td " . $txt_align . ">" . ($value) . "</td>";
            }
            if (sget($_REQUEST, '_page'))
                if ($filter_cnt > sget($_REQUEST, 'filter_cnt')) $_page = 1;
                else $_page = sget($_REQUEST, '_page');
            else $_page = 1;

            if ($user_type == 'super' || ($user_type == 'admin' && $ctab == '2'))
                $html .= "<td style='text-align:right;'><img src='/themes/maennaco/images/edit-icon.png' width=14 style='vertical-align:middle;cursor:pointer' data='$recordid' class='company-edit-icon' page='$_page'></td></tr>\n";
        }
    }
    $html .= '</tbody></table>';
    $Pagenation = array(
        'total' => $m->total,
        'limit' => $limit,
        'baseurl' => "/account?",
        'num_of_links' => 8,
        '_page' => $_page
    );
    $html .= '<table class="no-border"><tr border="0"><td>&nbsp;<td><td border="0"><div style="width:100%;">' . pagination($Pagenation) . '</div></td></tr></table>';
    return $html;
}

function com_update_section($op = null)
{
    global $AccessObj;
    if ($AccessObj->user_type !== 'super') return '';
    if ($op === 'update') {
        $admin_actions = $_REQUEST['admin_action'];
        $selected_uid = sget($_REQUEST, 'selected_records');
        if ($selected_uid === ',') $selected_uid = '';
        $UID = array();
        if ($selected_uid) {
            $A = explode(',', $selected_uid);
            foreach ($A as $id) {
                $UID[] = trim($id);
            }
        }
        if ($admin_actions === 'action_delete') {
            if (count($UID) > 0) {
                foreach ($UID as $uid) {
                    if ($uid == '') continue;
                    //Get email before delete to put it in rejected list if needed.
                    $sql = "SELECT `mail` FROM `users` WHERE `uid` = %d";
                    $result = db_query($sql, $uid);
                    $mail = db_fetch_object($result);
                    $sql0 = "DELETE FROM `maenna_followers` WHERE `cid` = '%d';";
                    $sql1 = "DELETE FROM `maenna_connections` WHERE `target_uid` = '%d' OR `assignee_uid` = '%d';";
                    $sql2 = "DELETE FROM `users` WHERE `uid` = '%d'";
                    $sql3 = "DELETE FROM `maenna_company` WHERE `companyid` = '%d'";
                    $sql4 = "DELETE FROM `maenna_company_data` WHERE `companyid` = '%d'";
                    if ($_REQUEST['reject_company'] === 'reject') {
                        $now = time();
                        $sql5 = "INSERT INTO rejected_users (user_email,rejected,rejected_by,user_type) VALUES ('%s','%s',%d,'%s')";
                        if (!db_query($sql5, array($mail->mail, $now, $AccessObj->uid, 'company'))) {
                            drupal_set_message("Error");
                            return;
                        }
                    }
                    if (!db_query($sql0, array($uid)) || !db_query($sql1, array($uid, $uid)) || !db_query($sql2, array($uid)) || !db_query($sql3, array($uid)) || !db_query($sql4, array($uid))) {
                        drupal_set_message("Error");
                        return;
                    }
                    drupal_set_message("Operation Successful");
                }
            }
        } elseif ($admin_actions === 'action_activate') {
            if (count($UID) > 0) {
                foreach ($UID as $uid) {
                    $sql = "UPDATE `maenna_company` SET `featured` = CASE active WHEN 1 THEN 0 ELSE featured END, `active` = NOT(`active`) WHERE `companyid` = %d";
                    if (!db_query($sql, array($uid))) {
                        drupal_set_message("Error");
                        return;
                    }
                }
                drupal_set_message("Operation Successful");
            }
        } elseif ($admin_actions === 'action_start_fundraising') {
            if (count($UID) > 0) {
                foreach ($UID as $uid) {
                    if ($uid) {
                        $sql = "UPDATE `maenna_company` SET `started_fundraising` = 1 - `started_fundraising` WHERE `companyid` = %d";
                    if (!db_query($sql, array($uid))) {
                        drupal_set_message("Error");
                        return;
                    }
                }
                }
                drupal_set_message("Operation Successful");
            }
        } elseif ($admin_actions === 'action_fundraising') {

            if (count($UID) > 0) {
                $failedCompanies = array();
                $successfullCompanies = 0;
                $ifCompaniesReadyForFundraising = checkCompaniesReadyForFundraising($UID);
                foreach ($UID as $uid) {
                    if ($uid) {
                        if ($ifCompaniesReadyForFundraising[$uid]) {
                            $sql = "UPDATE `maenna_company` SET `fundraising` = 1 - `fundraising` WHERE `companyid` = %d";
                            if (!db_query($sql, array($uid))) {
                                drupal_set_message("Error");
                                return;
                            }
                            $successfullCompanies++;
                        } else $failedCompanies[] = $uid;
                    }
                }
                if (sizeof($failedCompanies) > 0) {
                    if ($successfullCompanies == 0) {
                    drupal_set_message("Action denied. This project is not yet approved to fundraise. Please go to Administrative > Company
                        Information to review and approve all required information.","error");
                    return;
                    } else {
                        $failedCompanyNames = array();
                        $sql = "select projname FROM `maenna_company` WHERE `companyid` in (%s)";
                        if ($result = db_query($sql, array(join(",", $failedCompanies)))) {
                            while ($Row = db_fetch_object($result))
                                $failedCompanyNames[] = $Row->projname;
                        } else {
                            drupal_set_message("Error in getting company names", "error");
                            return;
                        }
                        drupal_set_message("For following companies action denied. These projects are not yet approved to fundraise. <br>
                                                Please go to Administrative > Company Information to review and approve all required information.<br>
                                                List of failed companies: [" . join(",", $failedCompanyNames) . "]","warning");
                    }
                } else
                    drupal_set_message("Operation Successful");
            }
        } elseif ($admin_actions === 'action_insights') {
            if (count($UID) > 0) {
                foreach ($UID as $uid) {
                    $sql = "UPDATE `maenna_company` SET `insight_came_flag` = NOT(`insight_came_flag`) WHERE `companyid` = %d";
                    if (!db_query($sql, array($uid))) {
                        drupal_set_message("Error");
                        return;
                    }
                }
                drupal_set_message("Operation Successful");
            }
        } elseif ($admin_actions === 'action_public') {
            $UIDS = array_filter($UID);
            if (count($UIDS) > 0) {
                foreach ($UIDS as $uid) {
                    $sql = "SELECT `public`, `public_date` FROM `maenna_company` WHERE `companyid` = %d";
                    $result = db_query($sql, array($uid));
                    if ($row = db_fetch_object($result)) {
                        //First Time Make it Visible and Update DateTime
                        $sql2 = "SELECT `mission` FROM `maenna_about` WHERE `project_id` = %d";
                        $result2 = db_query($sql2, array($uid));
                        $row2 = db_fetch_object($result2);
                        if (!$row2 || !strlen(strip_tags($row2->mission))) {
                                drupal_set_message("This company is not ready to be visible. Please complete the story in the About tab first.", 'error', false);
                                return;
                        }
                        if ($row->public == 0 and (empty($row->public_date) or ($row->public_date) == '0000-00-00 00:00:00')) {
                            $sql = "update maenna_company set public = NOT(public), public_date = '" . date('Y-m-d H:i:s') . "' WHERE companyid = %d";
                            $sqln = "INSERT INTO notifications (action, state, author, item, created) VALUES ('company_approved', 1, 1, '" . $uid . "','" . date('Y-m-d H:i:s') . "')";
                            if (!db_query($sqln, array($uid))) {
                                drupal_set_message("Error");
                            }
                        } else {
                            //Make it Visible second time.
                            $sql = 'UPDATE `maenna_company` SET `public` = NOT(`public`) WHERE `companyid` = %d';
                        }
                        if (!db_query($sql, array($uid))) {
                            drupal_set_message("Error");
                            return;
                        }
                    } else {
                        drupal_set_message("This company is not ready to be visible. Please complete the story in the About tab first.", 'error');
                        return;
                    }
                }
                drupal_set_message("Operation Successful");
            }
        } elseif ($admin_actions === 'action_assign_admin') {
            require_once 'classes/connections.inc';
            $admin = sget($_REQUEST, 'admin_id');
            if (count($UID) > 0) {
                foreach ($UID as $key => $uid) {
                    if ($key == 0) continue;
                    if ($admin) {
                        if ($admin === 'none') {
                            $return = Connections::remove_assigned_admin($uid);
                        } else {
                            $return = Connections::assign_admin($uid, $admin);
                        }
                    }
                }
                if ($return) {
                    drupal_set_message("Operation Successful");
                } else {
                    drupal_set_message("Error Occurred");
                }
            }
        }
        if ($_REQUEST['admin_action'] != ' ' && $_REQUEST['admin_action'] != '') header("Location: /account?tab=companies");
    }
} ?>

<script type="text/javascript">
    $(document).ready(function () {
        $('#companies_feature_btn').livequery('click', function () {
            var $this = $(this),
                checked = [],
                checkedNames = [];

            $('.cb_record').each(function (i, el) {
                if ('checked' == $(el).attr('checked') || true == $(el).attr('checked')) {
                    checked.push($(el).val());
                    checkedNames.push($(el).parent().parent().find('.company-title').attr('title'));
                }
            });

            if (0 == checked.length)
                return alert("Please select records to toggle featured state");

            if (!confirm('Are you sure you want to toggle featured state for the following companies: ' + checkedNames.join(', ') + '?'))
                return false;

            $.ajax({
                type: 'POST',
                url: '/themes/maennaco/includes/delete.php',
                data: {
                    type: 'feature-company',
                    'ids[]': checked,
                    u: '<?php echo $time = time();?>',
                    m: '<?php echo md5('delete.php:' . $time . ':kyarata75');?>'
                },
                success: function (msg) {
                    alert(msg);
                    $("#search_form").submit();
                }
            });
        });

        $('#companies_shareable_btn').livequery('click', function () {
            var checked = [],
                checkedNames = [];

            $('.cb_record').each(function (i, el) {
                if ('checked' == $(el).attr('checked') || true == $(el).attr('checked')) {
                    checked.push($(el).val());
                    checkedNames.push($(el).parent().parent().find('.company-title').attr('title'));
                }
            });

            if (0 == checked.length)
                return alert("Please select records to toggle connect");

            if (!confirm('Are you sure you want to toggle connect for the following companies: ' + checkedNames.join(', ') + '?'))
                return false;

            $.ajax({
                type: 'POST',
                url: '/themes/maennaco/includes/delete.php',
                data: {
                    type: 'shareable-company',
                    'ids[]': checked,
                    u: '<?php echo $time = time();?>',
                    m: '<?php echo md5('delete.php:' . $time . ':kyarata75');?>'
                },
                success: function (msg) {
                    alert(msg);
                    $("#search_form").submit();
                }
            });
        });
        $('#companies_openpro_btn').livequery('click', function () {
            var checked = [],
                checkedNames = [];

            $('.cb_record').each(function (i, el) {
                if ('checked' == $(el).attr('checked') || true == $(el).attr('checked')) {
                    checked.push($(el).val());
                    checkedNames.push($(el).parent().parent().find('.company-title').attr('title'));
                }
            });

            if (0 == checked.length)
                return alert("Please select records to toggle open");

            if (!confirm('Would you like to change the listing status?'))
                return false;
            $.ajax({
                type: 'POST',
                url: '/themes/maennaco/includes/delete.php',
                data: {
                    type: 'shareable-open',
                    'ids[]': checked,
                    u: '<?php echo $time = time();?>',
                    m: '<?php echo md5('delete.php:' . $time . ':kyarata75');?>'
                },
                success: function (msg) {
                    alert(msg);
                    $("#search_form").submit();
                }
            });
        });
        $('#companies_comingpro_btn').livequery('click', function () {
            var checked = [],
                checkedNames = [];

            $('.cb_record').each(function (i, el) {
                if ('checked' == $(el).attr('checked') || true == $(el).attr('checked')) {
                    checked.push($(el).val());
                    checkedNames.push($(el).parent().parent().find('.company-title').attr('title'));
                }
            });

            if (0 == checked.length)
                return alert("Please select records to toggle coming");

            if (!confirm('Would you like to change the listing status?'))
                return false;
            $.ajax({
                type: 'POST',
                url: '/themes/maennaco/includes/delete.php',
                data: {
                    type: 'shareable-coming',
                    'ids[]': checked,
                    u: '<?php echo $time = time();?>',
                    m: '<?php echo md5('delete.php:' . $time . ':kyarata75');?>'
                },
                success: function (msg) {
                    alert(msg);
                    $("#search_form").submit();
                }
            });
        });

        $('#companies_pastpro_btn').livequery('click', function () {
            var checked = [],
                checkedNames = [];

            $('.cb_record').each(function (i, el) {
                if ('checked' == $(el).attr('checked') || true == $(el).attr('checked')) {
                    checked.push($(el).val());
                    checkedNames.push($(el).parent().parent().find('.company-title').attr('title'));
                }
            });

            if (0 == checked.length)
                return alert("Please select records to toggle past");

            if (!confirm('Would you like to change the listing status?'))
                return false;
            $.ajax({
                type: 'POST',
                url: '/themes/maennaco/includes/delete.php',
                data: {
                    type: 'shareable-past',
                    'ids[]': checked,
                    u: '<?php echo $time = time();?>',
                    m: '<?php echo md5('delete.php:' . $time . ':kyarata75');?>'
                },
                success: function (msg) {
                    alert(msg);
                    $("#search_form").submit();
                }
            });
        });
    });
</script>
