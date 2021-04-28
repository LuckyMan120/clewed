<?php
chdir('../../../');
require_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
$return = menu_execute_active_handler();
if (is_int($return)) {
    switch ($return) {
        case MENU_NOT_FOUND:
            drupal_not_found();
            break;
        case MENU_ACCESS_DENIED:
            drupal_access_denied();
            break;
        case MENU_SITE_OFFLINE:
            drupal_site_offline();
            break;
    }
}

if ($_REQUEST['type'] == 'cmp_find') {
    global $user;
    define("__ACCOUNT__", 1);
    require_once("themes/maennaco/includes/new_functions.inc");

    $u = (int) $_REQUEST['u'];
    $n = $_REQUEST['n'];
    $t = (int) $_REQUEST['t'];
    $m = $_REQUEST['m'];

    $calculatedM = md5("fetch_cmp.php:{$u}:{$n}:{$t}:kyarata75");
    if($m !== $calculatedM)
        die();

    require_once 'lib/init.php';
    $userService = new \Clewed\User\Service();
    $users = $userService->get(array($u));
    $user = $users[$u];
    if(empty($user))
        die();

    $user = json_decode(json_encode($user));

    $page = $_REQUEST['page'];
    $total = $_REQUEST['total'];
    $canCollaborate = $_REQUEST['can'];
    $project_type = $_REQUEST['project_type'];
    $flag = $_REQUEST['flag'];
    $limit = " limit " . (($page - 1) * 12) . ", 12";
    $financial_table = "  LEFT JOIN  (SELECT mcd.companyid,mcd.data_value as revenue
              FROM maenna_company_data mcd
              WHERE data_type = 'financial'
              AND data_attr = YEAR(NOW())) as financial_tbl
    ON financial_tbl.companyid = maenna_company.companyid";
    $coll_tbl = "left join (select target_uid,status as collaboration
                                                    from maenna_connections WHERE assignee_uid = " . (int) $user->uid . " and status <> 'deactivated' and conntype='collaborator'
                                            ) as coll_tbl on coll_tbl.target_uid = maenna_company.companyid";
    $abt_tbl = "LEFT JOIN maenna_about ON maenna_company.companyid = maenna_about.project_id";
    $SQL = "select CAST(financial_tbl.revenue AS UNSIGNED) as revenue,coll_tbl.collaboration as collaboration,maenna_company.*, maenna_about.mission, maenna_about.project, IFNULL(likes.amount, 0) as likes, likes.users as liking_users from users, maenna_company $financial_table $coll_tbl $abt_tbl LEFT JOIN (SELECT project_id, COUNT(la_id) as amount, GROUP_CONCAT(user_id) AS users FROM like_company GROUP BY project_id) as likes ON likes.project_id = maenna_company.companyid";
    $SQL .= " where users.uid = maenna_company.companyid and users.status = 1 and public = 1";
    $SQL .= " and fundraising = ".$project_type."";
    if($flag) $SQL .= 'and  maenna_company.stateable = '.$flag.'';
    $SQL .= ' group by maenna_company.companyid';
    $SQL .= ' ORDER BY companyid DESC';
    $SQL .= $limit;
    $result = db_query($SQL);

    $projectIds = array();
    while (($row = db_fetch_array($result)) !== false)
        $projectIds[$projectId] = $projectId = (int) $row['companyid'];

    $companyService = new \Clewed\Company\Service();
    $teamSizes = $companyService->getTeamSize(array_keys($projectIds));

    mysql_data_seek($result, 0);

    if (!empty($result)) {
        $i = 0;
        while (($row = db_fetch_array($result)) !== false) {
            $i++;
            $recordid = sget($row, 'companyid');
            $link = "/account?tab=companies";
            if (!empty($row['project']) && file_exists('themes/maennaco/images/project/' . $row['project'])) {
                $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/' . urlencode($row['project']) . '&zc=1&w=270&h=194'; // 191 143
            } else {
                $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/big-' . str_replace(' /themes/maennaco/images/', '', getAvatarUrl($row['companyid'])) . '&zc=1&w=270&h=194';    // 191 143
            }
            $content .= '
            <div class="card' . (!($i % 3) ? ' last' : '') . '">
                <a href="' . "$link&page=company_detail&id=$recordid" . '">
                <div class="projectName">' . getProjectName($row['companyid']) . '</div>
                <div class="avatar"><img src="' . $avatar . '" width="100%"/></div>
            ';
            $sectorRev = array();
            if (!empty($row['sector'])) {
                array_push($sectorRev, preg_replace('/(?<=\\w)(?=[A-Z])/', ' $1', $row['sector']));
                $sectorStr = implode(' - ', $sectorRev);
                if (strlen($sectorStr) > 13) {
                    $sectorStr = substr($sectorStr, 0, 25) . '...';
                }
            }

            if (!empty($rev['revenue'])) {
                if($rev['revenue'] >= 1000000000 )
                    $revenue = "Rev < $" . (int) ($rev['revenue'] / 1000000000) . " B";
                else
                    $revenue = "Rev < $" . (int) ($rev['revenue'] / 1000000) . " M";
            } else $revenue = '';


            $content .= '<div class="sector-revenue"><span class="sector">' . $sectorStr . '</span><span class="revenue">' . $revenue . '</span></div>';
            $cityState = array();
            if (!empty($row['city'])) array_push($cityState, ucwords($row['city']));
            if (!empty($row['state'])) array_push($cityState, strtoupper($row['state']));
            $content .= '<div class="city-state">' . implode(', ', $cityState) . '</div>';
            $liking_users = explode(',', $row['liking_users']);
            $content .= '<div class="mission" style="color:#686b83;">' . substr(strip_tags($row['mission']), 0, 224) . (!empty($row['mission']) && strlen(strip_tags($row['mission'])) > 224 ? '...' : '') . '</div>';
            $content .= '</a>';
            $content .= '<div class="like-contrib" style="padding: 0 10px;">';
            if($row['shareable']){
                $content .= '<span class="project-team-size"
                                style="float: right; padding-left: 3px; color: #898b8e; cursor: pointer;"
                                title= "Projects team size">'.(int) $teamSizes[$recordid].'</span>
                            <span class="contrib" onclick="showContMessage()">
                                <a data-tooltip="
                                        This tool allows you to connect to learn more and explore fit/qualifications privately. You must have related industry and/or operating expertise for services."
                                    type="collaborate" style="cursor:pointer;">Connect</a>
                            </span>';
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
                // $content .= '<div class="status_bars_wrapper" style="display:grid;padding:0px;margin-top:30px;">
                //     <div class="status-bar-wrapper" style="width:290px;">
                //         <div class="status-bar"></div>
                //         <div class="status-bar-progress" style="width: '.number_format($amount_raising_percent, 0, '.', ',').'%"></div>
                //     </div><div style="display: flex;margin-top: 5px;text-align: center;">';
                if($goal_amount != null)
                $content .='<div style="margin-left:0px">
                            <div style="font-size: 16px;">$'.number_format($goal_amount, 0, '.', ',').'</div>
                            <div style="text-align:left;width:108%;">Amount Rasing</div>
                        </div><div style="height: 30px;background: #929497;width: 2px;margin-top: 6px;margin-left: 17px;"></div>';
                if($amount_raising_percent != null)
                $content .='<div style="margin-left:15px">
                            <div style="font-size: 16px;">'.number_format($amount_raising_percent, 0, '.', ',').'%</div>
                            <div>Funded</div>
                        </div>
                        <div style="height: 30px;background: #929497;width: 2px;margin-top: 6px;margin-left: 17px;"></div>';                                                   
                if($close_date != null)
                $content .='<div style="margin-left:15px">
                            <div style="font-size: 16px;">'.$close_date.'</div>
                            <div>Close Date</div>
                        </div>';
                $content .='</div></div>';
            };
            $content .= '</div>';
        }
    }
    if (($page) * 12 < $total) {
        // $content = "<span total='" .$total . "' page='" . ++$page . "' rel='show' can='" . $canCollaborate . "' project_type='" .$project_type."' class='show_more_cmp'>view more</span>";
        $content .= "<span total='" . $total . "' page='" . ++$page . "' rel='show' class='show_more_cmp' can='" . $canCollaborate . "' project_type='".$project_type."'>view more</span>";
    }
    die($content);
}
else {
    function getAvatarUrl($uid) {
        //Get cmp_role
        $query = "SELECT company_type FROM maenna_company WHERE companyid = %d";
        $result = db_query($query, array($uid));
        $cmp_role_tmp = db_fetch_object($result);
        $cmp_role = $cmp_role_tmp->company_type;
        //Check if user have a profile picture
        if (file_exists('sites/default/images/company/50x50/' . $uid . '.jpg')) {
            $avatar = 'sites/default/images/company/50x50/' . $uid . '.jpg';
        } else if ($cmp_role == 'service') {
            $avatar = ' /themes/maennaco/images/cmp-avatar-service.png';
        } else $avatar = ' /themes/maennaco/images/cmp-avatar-product.png';
        return $avatar;
    }

    $start = 0;
    $limit = 3;
    $_Industry = _INDUSTRY();
    if (sget($_REQUEST, '_page')) $start = (sget($_REQUEST, '_page') - 1) * $limit;
    if (sget($_REQUEST, 'rel') == 'show') {
        $start = $limit;
        $limit = 3;
        $flag = sget($_REQUEST, 'type');
        if($flag == "openable") $flag = 1;
        if($flag == "comingable") $flag = 2;
        if($flag == "pastable") $flag = 3;
    }
    //Calculate no of all companies with current filter
    $sql_all = 'SELECT COUNT(*) as cnt FROM `maenna_company` mc LEFT JOIN `users` u ON `mc`.`companyid` = `u`.`uid`
                WHERE `u`.`status` = 1 AND `mc`.`public` = 1 AND `mc`.`statetable` = '.$flag.'';
    if (!empty($_REQUEST['industry'])) {
        $industries = "'" . implode("', '", array_keys($_Industry[$_REQUEST['industry']])) . "'";
        $sql_all .= " AND sector IN($industries)";
    }
    if (!empty($_REQUEST['revenue'])) {
        $sql_all .= " AND (revenue IS NULL OR revenue <= " . ((int) $_REQUEST['revenue']) . ")";
    }
    $result_all = db_query($sql_all);
    $cnt_all = db_fetch_object($result_all);
    $cnt_all = $cnt_all->cnt;
    //Get companies for current page (6 per page)
    $financial_table = "left join (select companyid as financial_id,
                                                    data_value as revenue
                                                    from maenna_company_data where dataid = (
                                                        select dataid from maenna_company_data as temp
                                                            where temp.companyid = maenna_company_data.companyid and
                                                                temp.data_type = 'financial' and
                                                                temp.data_value > 0 and
                                                                temp.data_value is not NULL
                                                            order by data_attr desc limit 1
                                                    )
                                            ) as financial_tbl on financial_tbl.financial_id = maenna_company.companyid";
    $abt_tbl = "LEFT JOIN maenna_about ON maenna_company.companyid = maenna_about.project_id";
    $SQL = "select CAST(financial_tbl.revenue AS UNSIGNED) as revenue,maenna_company.*, maenna_about.mission, maenna_about.project, IFNULL(likes.amount, 0) as likes, likes.users as liking_users from users, maenna_company $financial_table $abt_tbl LEFT JOIN (SELECT project_id, COUNT(la_id) as amount, CONCAT(user_id) AS users FROM like_company GROUP BY project_id) as likes ON likes.project_id = maenna_company.companyid";
    $SQL .= " where users.uid = maenna_company.companyid and users.status = 1 and public = 1 and maenna_company.stateable = ".$flag."";
    if (!empty($_REQUEST['industry'])) {
        $industries = "'" . implode("', '", array_keys($_Industry[$_REQUEST['industry']])) . "'";
        $SQL .= " AND sector IN($industries)";
    }
    if (!empty($_REQUEST['revenue'])) {
        $SQL .= " AND (revenue IS NULL OR revenue <= " . ((int) $_REQUEST['revenue']) . ")";
    }
    $SQL .= ' group by maenna_company.companyid';
    if (empty($_REQUEST['likes'])) {
        $SQL .= ' ORDER BY companyid DESC';
    } else {
        $SQL .= ' ORDER BY likes ' . (($_GET['likes'] == 'ASC') ? 'ASC' : 'DESC');
    }
    $SQL .= ' LIMIT ' . $start . ", " . $limit;
    $result = db_query($SQL);
    $content = '';
    $i = 0;
    $projectIds = array();
    while (($row = db_fetch_array($result)) !== false)
        $projectIds[$projectId] = $projectId = (int) $row['companyid'];

    $companyService = new \Clewed\Company\Service();
    $teamSizes = $companyService->getTeamSize(array_keys($projectIds));

    mysql_data_seek($result, 0);
    $cmp_cnt = mysql_num_rows($result);
    if ($cmp_cnt > 0) {
        if($flag == 3){
            while (($row = db_fetch_array($result)) !== false) {
                $i++;
                $recordid = sget($row, 'companyid');
                $sql = "select data_value from maenna_company_data where data_type = 'financial' and companyid = " . $recordid . " and data_attr = YEAR(NOW())";
                $rev = mysql_query($sql);
                $rev = mysql_fetch_array($rev);
    
                if (!empty($row['project']) && file_exists('themes/maennaco/images/project/' . $row['project'])) {
                    $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/' . urlencode($row['project']) . '&zc=1&w=191&h=143';
                } else {
                    $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/big-' . str_replace(' /themes/maennaco/images/', '', getAvatarUrl($row['companyid'])) . '&zc=1&w=191&h=143';
                }
                $cityState = array();
                if (!empty($row['city'])) array_push($cityState, ucwords($row['city']));
                if (!empty($row['state'])) array_push($cityState, strtoupper($row['state']));
                $sectorRev = array();
                if (!empty($row['sector'])) {
                    array_push($sectorRev, preg_replace('/(?<=\\w)(?=[A-Z])/', ' $1', $row['sector']));
                    $sectorStr = implode(' - ', $sectorRev);
                    if (strlen($sectorStr) > 13) {
                        $sectorStr = substr($sectorStr, 0, 25) . '...';
                    }
                }
                if (!empty($rev['data_value'])) {
                    if($rev['data_value'] >= 1000000000 )
                        $revenue = "Rev < $" . (int) ($rev['data_value'] / 1000000000) . " B";
                    else
                        $revenue = "Rev < $" . (int) ($rev['data_value'] / 1000000) . " M";
                } else $revenue = "Rev : $".(int)$rev['data_value']."";

                $content .= '<div class="card' . (!($i % 3) ? ' last' : '') . '">';
                $content .= ' <a style="cursor:pointer;" class="register" href="/companies?id=' . $row['companyid'] . '">';
                $content .= ' <div class="projectName">' . $row['projname'] . '</div>';
                $content .= ' <div class="avatar"><img src="' . $avatar . '" width="100%"/></div>';
                $content .= ' <div class="city-state">' . implode(', ', $cityState) . '</div>';
                $content .= ' <div class="sector-revenue"><span class="sector">' . $sectorStr . '</span><span class="revenue">' . $revenue . '</span></div>';
                $content .= ' <div class="mission" style="color:#686b83;">' . substr(strip_tags($row['mission']), 0, 224) . (!empty($row['mission']) && strlen(strip_tags($row['mission'])) > 224 ? '...' : '') . '</div>';
                $content .= '<div class="like-contrib" style="display:flex; justify-content:flex-end; height:15px;">';
                        if($row['shareable']){
                            $content .= '<span class="contrib" onclick="showContMessage()">
                                            <a data-tooltip="
                                                    This tool allows you to connect to learn more and explore fit/qualifications privately. You must have related industry and/or operating expertise for services."
                                                type="collaborate" style="cursor:pointer;">Connect</a>
                                        </span>
                                        <span class="project-team-size"
                                            style="float: right; padding-left: 3px; color: #898b8e; cursor: pointer;"
                                            title= "Projects team size">'.(int) $teamSizes[$recordid].'</span>';
                                        
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
                    $content .= '<div class="status_bars_wrapper"  style="display:grid;padding:0px;margin-top:5px; font-size:12px">
                        <div class="status-bar-wrapper">
                            <div class="status-bar"></div>
                            <div class="status-bar-progress" style="width: '.number_format($amount_raising_percent, 0, '.', ',').'%"></div>
                        </div><div style="display: flex;margin-top: 5px;text-align: center;">';
                    if($goal_amount != null)
                    $content .='<div style="margin-left:0px">
                                <div style="font-size: 12px;">$'.number_format($goal_amount, 0, '.', ',').'</div>
                                <div>Rasing</div>
                            </div><div style="height: 30px;background: #929497;width: 2px;margin-top: 6px;margin-left: 17px;"></div>';
                    if($amount_raising_percent != null)
                    $content .='<div style="margin-left:15px">
                                <div style="font-size: 12px;">'.number_format($amount_raising_percent, 0, '.', ',').'%</div>
                                <div>Funded</div>
                            </div>
                            <div style="height: 30px;background: #929497;width: 2px;margin-top: 6px;margin-left: 17px;"></div>';                                                   
                    if($amount_raising_percent != null)
                    $content .='<div style="margin-left:15px">
                                <div style="font-size: 12px;">'.$close_date.'</div>
                                <div>Close Date</div>
                            </div>';
                    $content .='</div></div>';
                };
                $content .= '</div>';
                if ($i % 3 == 0) $content .= "<br style='clear:both''>";
                $page = (int) sget($_REQUEST, '_page') + 1;
            }
        } 
        else {
            while (($row = db_fetch_array($result)) !== false) {
                $i++;
                $recordid = sget($row, 'companyid');
                $sql = "select data_value from maenna_company_data where data_type = 'financial' and companyid = " . $recordid . " and data_attr = YEAR(NOW())";
                $rev = mysql_query($sql);
                $rev = mysql_fetch_array($rev);
    
                if (!empty($row['project']) && file_exists('themes/maennaco/images/project/' . $row['project'])) {
                    $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/' . urlencode($row['project']) . '&zc=1&w=191&h=143';
                } else {
                    $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/big-' . str_replace(' /themes/maennaco/images/', '', getAvatarUrl($row['companyid'])) . '&zc=1&w=191&h=143';
                }
                $cityState = array();
                if (!empty($row['city'])) array_push($cityState, ucwords($row['city']));
                if (!empty($row['state'])) array_push($cityState, strtoupper($row['state']));
                $sectorRev = array();
                if (!empty($row['sector'])) {
                    array_push($sectorRev, preg_replace('/(?<=\\w)(?=[A-Z])/', ' $1', $row['sector']));
                    $sectorStr = implode(' - ', $sectorRev);
                    if (strlen($sectorStr) > 13) {
                        $sectorStr = substr($sectorStr, 0, 25) . '...';
                    }
                }
                if (!empty($rev['data_value'])) {
                    if($rev['data_value'] >= 1000000000 )
                        $revenue = "Rev : $" . (int) ($rev['data_value'] / 1000000000) . " B";
                    else
                        $revenue = "Rev : $" . (int) ($rev['data_value'] / 1000000) . " M";
                } else $revenue = "Rev : $".(int)$rev['data_value']."";
                if($flag == 1) $stateStr = "Open";
                if($flag == 2) $stateStr = "Coming Soon";
                if($row['fundraising'] == 1) $show_num = 224;
                else $show_num = 150;

                $content .= '
                    <div class="card-wrap">
                        <div class="card-inner" style="width: 250px;">
                            <div class="projectName" style="text-align: left;display: block;">'. $row['projname'] .'</div>
                            <a style="cursor:pointer;" href="/companies?id='.$row['companyid'].'">
                                <img src='.$avatar.'" alt="'. $row['projname'] .'" width="100%" height="80%"/>
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
                                <div style="max-width: 30%;color:#898B8E;"> 
                                    <span class="revenue">'.$stateStr.'</span>
                                </div>
                            </div>
                            <div style="height:170px;width:620px;margin-left:25px;">
                                <div class="subtitile" style="color:#686b83">'.$row['deal_summary_title'].'</div>                                    
                                <div
                                    class="mission" style="font-size: 15px;font-family: Lato Light; color:#686b83;">'.substr(strip_tags($row["deal_summary_statement"]), 0, $show_num) . (!empty($row["deal_summary_statement"]) && strlen(strip_tags($row["deal_summary_statement"])) > $show_num ? "&hellip;" : "").'
                                </div> </a>
                                <div class="like-contrib" style="padding: 0 10px;">';
                if($row['shareable']){
                    $content .= '<span class="project-team-size"
                                    style="float: right; padding-left: 3px; color: #898b8e; cursor: pointer;"
                                    title= "Projects team size">'.(int) $teamSizes[$recordid].'</span>
                                <span class="contrib" onclick="showContMessage()">
                                    <a data-tooltip="
                                            This tool allows you to connect to learn more and explore fit/qualifications privately. You must have related industry and/or operating expertise for services."
                                        type="collaborate" style="cursor:pointer;">Request connection</a>
                                </span>';
                            }
                $content .=  '</div></div><div style="width:93%;margin-left:25px;">';
                if($row['fundraising'] == 1){
                    $sql_total = "SELECT COUNT(*) AS counts,(CASE WHEN SUM(amount) > 0  THEN SUM(amount) WHEN SUM(amount) IS NULL THEN 0 END) AS SUM FROM maenna_professional_investment WHERE company_id = '".$recordid."' AND STATUS = 3";
                    $result_total = mysql_query($sql_total);
                    $row_amount = mysql_fetch_object($result_total);
                    $total_amount = $row_amount->SUM;
                    $committed_count = $row_amount->counts;
                    
                    $sql_round_amount = "select round_amount_raising as rmount,close_date,security_type from maenna_company_data where companyid=%d and data_type='financial' and data_attr = ".date('Y')."";
                    $result_amount = db_query($sql_round_amount, array($recordid));
                    $row_round = db_fetch_object($result_amount);
                    $goal_amount = is_null($row_round->rmount) ? 0 : $row_round->rmount;
                    $close_date = $row_round->close_date;
                    $data_style = date("d/m/y", time($close_date));
                    $amount_raising_percent = $total_amount / $goal_amount * 100;
                    if($amount_raising_percent>20) $possible_percent = $amount_raising_percent;
                    if($goal_amount>0){
                        $gola_amount_style = '$' . number_format($goal_amount, 0, '.', ',');
                    }
                    $seq_type = $row_round->security_type;

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
                                    <div style="text-align: left;width: 101%;">Amount Rasing</div>
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
                }
                if($row['fundraising'] == 1) $top="null;";
                else $top = "5px;";
                $content .= '<div class="amount-button" style="margin-top:'.$top.';margin-left:74%;">
                <a style="cursor:pointer;" href="/companies?id='.$row['companyid'].'">
                <button class="oppor-btn">View Opportunity</button></a>
                        </div></div></div></div>';
                if ($i % 3 == 0) $content .= "<br style='clear:both''>";
                $page = (int) sget($_REQUEST, '_page') + 1;
            }
        }
    } else
    {
        $die_mess = sget($_REQUEST, '_page') == 1 ? 'No visible companies':'No more companies to show';
        die("<span style='color: #00A2BF;font: 14px Lato Bold;margin-top: 20px;margin-left: 20px;display: block;'>".$die_mess."</span>");
    }
    if (!empty($_REQUEST['industry'])) $ind = 'industry="' . $_REQUEST['industry'] . '"';
    if (!empty($_REQUEST['revenue'])) $rev = 'revenue="' . $_REQUEST['revenue'] . '"';
    if (sget($_REQUEST, 'rel') != 'show' && $cnt_all > 12) {
        $content .= '<br style="clear:both;"><a ' . $rev . ' ' . $ind . ' class="show_more_cmp" onclick="loadMoreCmp(this)">see more</a>';
    } else {
        if($flag == 3)
            $content .= '<br style="clear:both;"><a  class="show_more_cmp" style="width:250px !important;" onclick="showLoginDlg();">Login or register to see more</a>';
    }
    echo $content;
}
