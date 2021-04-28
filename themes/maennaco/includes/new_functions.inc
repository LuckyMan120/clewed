<?php
session_start();
defined("__ACCOUNT__") or die("no direct access allowed");

function getProfessionalType(){
    $pageType = '' ;
    if ($_REQUEST['q']){
        if ($_REQUEST['q'] === 'buy-services'){
            $pageType =  'service';
        }elseif ($_REQUEST['q'] == 'join-insights'){
            $pageType =  'insight';
        }
    }
    return $pageType;
}


function ifUserRatedForService($type,$editor_uid,$target_uid,$service_id) {
    $db = \Clewed\Db::get_instance();
    if ($type == 'service') $column_name = "service_id";
    else if ($type == 'insight') $column_name = "insight_id";

    $sql = "SELECT * FROM user_rating WHERE editor_uid = :editor_uid AND target_uid = :target_uid AND ".$column_name." = :service_id";

    $result = $db->get_array($sql,array('editor_uid' => $editor_uid,'target_uid' => $target_uid,'service_id' => $service_id));

    if (count($result) > 0) return true;
    else return false;

}
function getClientCompany($uid) {
    $sql = "SELECT (IF (mc.projname <> '',mc.projname,CONCAT('PROJECT ',mc.companyid+100))) AS display_name,mcc.target_uid FROM maenna_connections mcc JOIN maenna_company mc ON mcc.target_uid = mc.companyid WHERE assignee_uid = %d AND mcc.status = 'active' AND mcc.conntype = 'client' ORDER BY mcc.connid DESC LIMIT 1";
    $result = db_query($sql, $uid);
    $Row = db_fetch_object($result);
    return "<a class='tooltip_link' href='account?tab=companies&page=company_detail&id=" . $Row->target_uid . "' target='_blank'>" . $Row->display_name . "</a>";
}

function canRateUser($target_uid, $editor_uid) {
//Check if user is admin ot super admin
    global $ifAdmin;
    if ($target_uid == $editor_uid) return false;
    if ($ifAdmin) return true;
    //Check if user is attending target`s insight
    $sql = "SELECT * FROM maenna_professional_payments mpp
            LEFT JOIN maenna_professional mp ON mpp.pro_id = mp.id
            WHERE (mp.postedby = %d OR (SELECT COUNT(*)
                             FROM maenna_discussion_moderator
                             WHERE maenna_discussion_moderator.discussion_id = mp.id AND
                                   maenna_discussion_moderator.user_id = %d) > 0  AND mpp.status = 1 AND mpp.user_id = %d";
    $result = db_query($sql, array($target_uid,$target_uid, $editor_uid));
    if (mysql_num_rows($result) > 0) return true;
    //Check if user is attending target`s insight
    if (inProjectBox($target_uid, $editor_uid)) return true;
    return false;
}

function getUserRatings($uid) {
    $user_weight = $admin_weight = 0;
    $sql = "SELECT
          sum(CASE WHEN admin <> 1 THEN 1 ELSE 0 END) AS non_admin,
          sum(CASE WHEN admin = 1 THEN 1 ELSE 0 END) AS admin_cnt
                FROM user_rating ur
                WHERE target_uid = %d";
    $result = db_query($sql, $uid);
    $Data = db_fetch_object($result);
    if ($Data->non_admin < 25) {
        $user_weight = round($Data->non_admin * 0.02, 2);
        $admin_weight = round(1.00 - $user_weight, 2);
    } else {
        $user_weight = 0.50;
        $admin_weight = 0.50;
    }
    $total_weight = round($Data->non_admin * $user_weight + $Data->admin_cnt * $admin_weight, 2);
//die("u=".$user_weight."a=".$admin_weight."t=".$total_weight);
    $sql = "SELECT
          count(*) AS total,
          sum(CASE WHEN admin <> 1 THEN rate_overall * %f ELSE 0 END) AS non_admin_rates,
          sum(CASE WHEN admin = 1 THEN rate_overall * %f ELSE 0 END) AS admin_rates
                FROM user_rating ur
                WHERE target_uid = %d";
    $result = db_query($sql, array($user_weight, $admin_weight, $uid));
    $Row = db_fetch_object($result);
    $wrate = round(($Row->non_admin_rates + $Row->admin_rates) / $total_weight, 2);
    return array('total' => $Row->total, 'rate' => $wrate);
}

function getInsightTopic($id) {
    $sql = "SELECT title FROM maenna_professional WHERE id = %d";
    $result = db_query($sql, array($id));
    $ins = db_fetch_object($result);
    return ucfirst($ins->title);
}

function duplicate_br($str) {
    return preg_replace("/<br\W*?\/>/", "<br><br>", $str);
}

function ifAssignedAdmin($uid, $admin_id) {
    $sql = "SELECT * FROM maenna_connections WHERE target_uid = %d AND assignee_uid = %d AND conntype = '%s' AND status = '%s'";
    $result = db_query($sql, array($uid, $admin_id, 'admin', 'active'));
    if (mysql_num_rows($result) > 0) return true; else return false;
}

function proToProRelation($pid1, $pid2) {
    $return = array();
    $sql = "SELECT target_uid, count(target_uid) AS cnt FROM (SELECT assignee_uid,target_uid FROM maenna_connections WHERE conntype IN ('visible','advisor','collaborator','client') AND status = 'active' AND (assignee_uid = %d OR assignee_uid = %d) GROUP BY assignee_uid,target_uid) AS tmp GROUP BY target_uid HAVING cnt > 1";
    $result = db_query($sql, $pid1, $pid2);
    while ($Row = db_fetch_object($result)) $return[] = $Row->target_uid;
    $aql = "SELECT cid,count(cid) AS cnt FROM maenna_followers WHERE uid = %d OR uid = %d GROUP BY cid HAVING cnt > 1";
    $result = db_query($sql, array($pid1, $pid2));
    $result = db_query($sql, $pid1, $pid2);
    while ($Row = db_fetch_object($result)) $return[] = $Row->target_uid;
    return array_unique($return);
}

//Checks if $compannyid is in preject box of $pid
//@return mixed, false if not,else returns connection type (array)
function inProjectBox($pid, $companyid) {
    $return = array();
    $sql = "SELECT conntype FROM maenna_connections WHERE target_uid = %d AND assignee_uid = %d AND status = '%s'
            AND conntype IN ('advisor','collaborator','visible','client','partner') LIMIT 1";
    $result = db_query($sql, array($companyid, $pid, 'active'));
    if (mysql_num_rows($result) > 0) {
        while ($Row = mysql_fetch_object($result))
            $return[] = $Row->conntype;
    }
    $sql = "SELECT id FROM maenna_followers WHERE uid = %d AND cid = %d";
    $result = db_query($sql, array($pid, $companyid));
    if (mysql_num_rows($result) > 0) $return[] = 'followers';
    if (count($return) == 0) {
        return false;
    } else return $return;
}

function getUserRealName($uid) {
    $usr_type = getUserTypeById($uid);
    if ($usr_type == 'company') {
        $sql = "SELECT contact FROM maenna_company WHERE companyid = %d";
        $result = db_query($sql, array($uid));
        $contact = db_fetch_object($result);
        $tmp = explode(" ", $contact->contact);
        return array("firstname" => $tmp[0], "lastname" => $tmp[1]);
    } elseif ($usr_type == 'people') {
        $sql = "SELECT firstname,lastname FROM maenna_people WHERE pid = %d";
        $result = db_query($sql, array($uid));
        $contact = db_fetch_array($result);
        return $contact;
    } else {
        return array('Clewed');
    }
}

function proHasInsights($pid) {
    $sql = "SELECT id FROM maenna_professional WHERE postedby = %d AND approve_status = 1";
    $result = db_query($sql, array($pid));
    if (mysql_num_rows($result) == 0) {
        return false;
    } else return true;
}

function pageTopIdentifier() {
    global $AccessObj;
    $page = $_REQUEST['page'];
    $tab = $_REQUEST['tab'];
    $id = $_REQUEST['id'];
    $ref = $_REQUEST['ref'];
    $ref2 = $_REQUEST['ref2'];
    if (in_array(getUserTypeById($id), array('admin', 'super_admin'))) return '';
    $content = '<div class="custom_breadcrumb">';

    if ($AccessObj->user_type == 'people' &&  ($page == 'company_detail' || $page == 'pro_detail')) {

        $projname = ucwords(strtolower((getUserById($id))));
        if ($projname == '') $projname = "Project " . (string) ((int) $id + 100);
        $conntype = inProjectBox($AccessObj->uid, $id);

        if ($conntype) {
            switch ($conntype[0]) {
                case 'advisor':
                    $connstring = 'Advisors';
                    break;
                case 'visible':
                    $connstring = 'Connectors';
                    break;
                case 'collaborator':
                    $connstring = 'Interested';
                    break;
                case 'client':
                    $connstring = 'My Projects';
                    break;
                case 'followers':
                    $connstring = 'Following';
                    break;
                case 'partner':
                    $connstring = 'Introductions';
                    break;
            }
            $projectsLink = '<a href="/account?tab=professionals&page=pro_detail&id='.$AccessObj->uid.'&section=pro_industry_view&type=my_projects">Projects</a>';
            $content .= $projectsLink.' >' . $connstring . ' >';
            $content .= $projname;
        }else{
            $pro_name = getProId($_REQUEST['id']);
            $projectsLink = '<a href="/account?tab=professionals&page=pro_detail&id='.$AccessObj->uid.'&section=pro_industry_view&type=my_projects">Projects</a>';
            $content .= $projectsLink.' >' . ucwords(strtolower($pro_name)) ;
        }
    }
    elseif ($AccessObj->user_type == 'company' && $page == 'pro_detail') {
        $pro_name = getProId($id);
        if ($_REQUEST['section'] == 'pro_industry_view' && ($_REQUEST['type'] == 'discussion' || ($_REQUEST['type'] == 'details' && $_REQUEST['pro_id']))) {
            $tmp = explode(" ", $pro_name);
            $content .= ucwords(strtolower($tmp[0])) . "'s Insights & Services";
        } else {
            $content .= ucwords(strtolower($pro_name));
        }
    }
    elseif ($AccessObj->user_type == 'people' && $page == 'pro_detail' && $AccessObj->uid != $_REQUEST['id']) {
        $pro_name = getProId($_REQUEST['id']);
        if ($_REQUEST['section'] == 'pro_industry_view' && $_REQUEST['type'] == 'discussion') {
            $content .= ucwords(strtolower($pro_name)) . "'s Insights & Services";
        } else {
            $tmp = proToProRelation($id, $AccessObj->uid);
            if (count($tmp) > 0) {
                foreach ($tmp as $key => $value) $tmp[$key] = ucwords(strtolower(getUserbyId($value)));
                $content .= "Project team of " . implode(', ', $tmp);
            } else $content .= ucwords(strtolower($pro_name));
        }
    }
    elseif ($id == $AccessObj->uid && $AccessObj->user_type == 'people' && $page = 'pro_detail' && $_REQUEST['type'] == 'fileview') {
        $pro_name = getProId($_REQUEST['id']);
        $content .= ucwords(strtolower($pro_name)) . ", Readings / Ideas";
    }
    $content .= '</div>';
    return $content;
}

function ifDiscussionModerator($discId, $uid) {
    $sql = "SELECT * FROM maenna_discussion_moderator WHERE discussion_id = %d AND user_id = %d";
    $result = db_query($sql, array($discId, $uid));
    if (mysql_num_rows($result) > 0) return true; else return false;
}

function getProCard($pid) {
    $db = \Clewed\Db::get_instance();
    $sql = '
    SELECT
      users.created AS register,
      users.uid,
      maenna_people.*,
      users_roles.rid,
      mpd.expertise
    FROM
      users,
      maenna_people
    LEFT JOIN (
      SELECT
        pid,
        data_value2 AS expertise
      FROM
        maenna_people_data mp1d
      WHERE
        data_type = "addinfo"
      AND
        data_attr = "experties"
      AND
        dataid = (
          SELECT
            dataid
          FROM
            maenna_people_data
          WHERE
            data_type = "addinfo"
          AND
            data_attr = "experties"
          AND
            pid = mp1d.pid
          ORDER BY
            dataid DESC
          LIMIT 1
        )
      ) mpd
    ON
      maenna_people.pid = mpd.pid,
      users_roles
    WHERE
      users.uid = maenna_people.pid
    AND
      users.uid = users_roles.uid
    AND
      maenna_people.pid = :pid';
    $pro = $db->get_row($sql, array(':pid' => $pid));
    $usrType = strtoupper(userType($pid));
    if ($usrType == 'COMPANY MEMBER') {
        $tmp = getClientCompany($pid);
        if ($tmp != '') $usrType = 'MEMBER, ' . $tmp;
    }
    $avatar = getAvatarUrl($pid, "150");
    $pro['experties'] = preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $pro['experties']);
    $pro['experties2'] = preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $pro['experties2']);
    $pro['experties3'] = preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $pro['experties3']);
    $sql1 = '
    SELECT
        `data_value` AS undergraduate,
        `data_value3` AS graduate
    FROM
        `maenna_people_data`
    WHERE
        `data_type` = "education"
    AND
        `pid` = :pid
    ORDER BY
        `dataid` DESC
    LIMIT 1';
    $pro = array_merge(
        $pro,
        $db->get_row(
            $sql1,
            array(
                ':pid' => $pid
            )
        )
    );
    if (strlen($pro['expertise']) >= 110) {
        $pro['expertise'] = substr(strip_tags($pro['expertise']), 0, 110) . '&hellip;';
    } else {
        $pro['expertise'] = strip_tags($pro['expertise']);
    }
    if (strlen($pro['profile']) >= 139) {
        $pro['profile'] = substr(strip_tags($pro['profile']), 0, 139) . '&hellip;';
    }
    $pro['profile'] = strip_tags($pro['profile']);
    $content = '
        <div class="pro_card" style="margin:0; width: auto; height:auto;margin-top:0 !important;">
            <img width="50" height="50" src ="' . $avatar . '">
            <div style="float:left;width:340px;">
                <div class="name_title">' . strtoupper($pro['firstname']) . ', ' . $usrType . '</div>
                <br style="clear:both;">
                <div class="pro_summary">' . $pro['expertise'] . '</div>
            </div>
            <br style="clear:both">
            <hr style="width:97%; background-color:#D0D2D3; margin-left:5px;margin-top:11px;">
            <div class="pro_exp">' . ($pro['experties'] != '' ? $pro['experties'] : '') . ($pro['experties2'] != '' ? ', ' . $pro['experties2'] : '') . ($pro['experties3'] != '' ? ', ' . $pro['experties3'] : '') . '</div>
            <div class="pro_exp">' . $pro['graduate'] . ($pro['undergraduate'] != '' ? ';' . $pro['undergraduate'] : '') . '</div>
            <hr style="width:97%; background-color:#D0D2D3; margin-left:5px;margin-top:11px;margin-bottom:11px;">
            <div class="pro_summary" style="margin-left:20px;">' . $pro['profile'] . '</div>
        </div>';
    return $content;
}

function getCompanyCard($cid) {
    $sql = '
    SELECT
      CAST(financial_tbl.revenue AS UNSIGNED) AS revenue,
      maenna_company.*,
      maenna_about.mission,
      maenna_about.project
    FROM
      users,
      maenna_company
    LEFT JOIN (
      SELECT
        companyid AS financial_id,
        data_value AS revenue
      FROM
        maenna_company_data
      WHERE
        dataid = (
          SELECT
            dataid
          FROM
            maenna_company_data AS temp
          WHERE
            temp.companyid = maenna_company_data.companyid
          AND
            temp.data_type = "financial"
          AND
            temp.data_value > 0
          ORDER BY
            data_attr DESC
          LIMIT 1
        )
      ) AS financial_tbl
    ON
      financial_tbl.financial_id = maenna_company.companyid
    LEFT JOIN
      maenna_about
    ON
      maenna_company.companyid = maenna_about.project_id
    WHERE
      users.uid = maenna_company.companyid
    AND
      maenna_company.companyid = %d';
    $result = db_query($sql, array($cid));
    $row = mysql_fetch_array($result);
    $recordid = sget($row, 'companyid');
    $sql = "SELECT data_value FROM maenna_company_data WHERE data_type = 'financial' AND companyid = " . $recordid . " AND data_attr = YEAR(NOW())";
    $rev = mysql_query($sql);
    $rev = mysql_fetch_array($rev);
    if (!empty($row['project']) && file_exists('themes/maennaco/images/project/' . $row['project'])) {
        $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/' . urlencode($row['project']) . '&zc=1&w=191&h=143';
    } else {
        $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/big-' . str_replace(' /themes/maennaco/images/', '', getAvatarUrl($row['companyid'])) . '&zc=1&w=191&h=143';
    }
    $content = '<div class="card' . (!($i % 4) ? ' last' : '') . '">
    <div class="projectName">' . getProjectName($row['companyid']) . '</div>
    <div class="avatar"><img src="' . $avatar . '" /></div>';
    $sectorRev = array();
    if (!empty($row['sector'])) {
        array_push($sectorRev, preg_replace('/(?<=\\w)(?=[A-Z])/', ' $1', $row['sector']));
        $sectorStr = implode(' - ', $sectorRev);
        if (strlen($sectorStr) > 13) {
            $sectorStr = substr($sectorStr, 0, 10) . '...';
        }
    }
    if (!empty($rev['data_value'])) {
        $revenue = "Rev < $" . (int) ($rev['data_value'] / 1000000) . " M";
    } else {
        $revenue = '';
    }
    $content .= '<div class="sector-revenue"><span class="sector">' . $sectorStr . '</span><span class="revenue">' . $revenue . '</span></div>';
    $cityState = array();
    if (!empty($row['city'])) array_push($cityState, ucwords($row['city']));
    if (!empty($row['state'])) array_push($cityState, strtoupper($row['state']));
    $content .= '<div class="city-state">' . implode(', ', $cityState) . '</div>';
    $content .= '<div class="mission" style="padding:8px 0px 0px;">' . substr(strip_tags($row['mission']), 0, 75) . (!empty($row['mission']) && strlen(strip_tags($row['mission'])) > 75 ? '...' : '') . '</div>';
    $content .= '</div></div>';
    return $content;
}

function makePseudoName() {
    $max = 1;
    $names = array();
    $sql = "SELECT pseudo_name FROM maenna_company mc LEFT JOIN users u ON mc.companyid = u.uid WHERE u.status = 1 AND pseudo_name IS NOT NULL
AND TRIM(pseudo_name) <> ''";
    $result = db_query($sql);
    while ($Row = db_fetch_object($result)) {
        $names[] = $Row->pseudo - name;
        if (strlen($Row->pseudo_name) > $max) $max = strlen($Row->pseudo_name);
    }
    if ($max == 1 && count($names) < 26) {
        $characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
        $randomString = $characters[rand(0, strlen($characters) - 1)];
        while (in_array($randomString, $names)) {
            $randomString = $characters[rand(0, strlen($characters) - 1)];
        }
        return $randomString;
    } elseif ($max == 2 && count($names) < 1296) {
        $characters = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ';
        $randomString = '';
        for ($i = 0; $i < 2; $i++) {
            $randomString .= $characters[rand(0, strlen($characters) - 1)];
        }
        while (in_array($randomString, $names)) {
            $randomString = '';
            for ($i = 0; $i < 2; $i++) {
                $randomString .= $characters[rand(0, strlen($characters) - 1)];
            }
        }
        return $randomString;
    } else {
        $rnd = rand(3, 5);
        $characters = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ';
        $randomString = '';
        for ($i = 0; $i < $rnd; $i++) {
            $randomString .= $characters[rand(0, strlen($characters) - 1)];
        }
        while (in_array($randomString, $names)) {
            $randomString = '';
            for ($i = 0; $i < $rnd; $i++) {
                $randomString .= $characters[rand(0, strlen($characters) - 1)];
            }
        }
        return $randomString;
    }
}

//CP DEV
function getRoleId($uid) {
    $sql = "  SELECT rid FROM users_roles WHERE uid = %d ";
    $result = db_query($sql, array($uid));
    $Row = db_fetch_object($result);
    return $Row->rid;
}

function getUserTypeById($uid) {
    $query = "SELECT rid FROM users_roles WHERE uid = %d ";
    $result = db_query($query, array($uid));
    $Row = db_fetch_object($result);
    $rid = $Row->rid;
    if (in_array($rid, array(4, 5, 7, 8, 11, 12, 13, 14))) {
        return 'people';
    } elseif ($rid == 6 || $rid == 15) return 'admin';
    elseif ($rid == 10) return 'super_admin';
    elseif ($rid == 3) return 'company';
    return "invalid uid";
}

function getProExpertise($uid) {
    $sql = "SELECT experties FROM maenna_people WHERE pid = %d";
    $result = db_query($sql, array($uid));
    $Row = db_fetch_object($result);
    //die(test_array($Row));
    return preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $Row->experties);
}

function ImageToBlackAndWhite($im) {
    for ($x = imagesx($im); $x--;) {
        for ($y = imagesy($im); $y--;) {
            $rgb = imagecolorat($im, $x, $y);
            $r = ($rgb >> 16) & 0xFF;
            $g = ($rgb >> 8) & 0xFF;
            $b = $rgb & 0xFF;
            $gray = ($r + $g + $b) / 3;
            if ($gray < 0xFF) {
                imagesetpixel($im, $x, $y, 0xFFFFFF);
            } else {
                imagesetpixel($im, $x, $y, 0x000000);
            }
        }
    }
    imagefilter($im, IMG_FILTER_NEGATE);
}

function getAvatarUrl($uid, $size = null) {
    if ($size == null) $size = "50x50"; else $size = "150x150";
    $usrType = getUserTypeById($uid);
    if ($usrType == 'people' || $usrType == 'admin' || $usrType == 'super_admin') {
        //Get user gender
        $query = "SELECT gender FROM maenna_people WHERE pid = %d";
        $result = db_query($query, array($uid));
        $gender_tmp = db_fetch_object($result);
        $gender = $gender_tmp->gender;
        //Check if user have a profile picture
        if ($usrType == 'admin' || $usrType == 'super_admin') {return '/themes/maennaco/images/discussion_logo.png';}
        if (file_exists('sites/default/images/profiles/' . $size . '/' . $uid . '.jpg')) {
            $avatar = 'sites/default/images/profiles/' . $size . '/' . $uid . '.jpg';
        } else {
            if ($gender == 'm' || $gender == '') {
                $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
            } else {
                $avatar = ' /themes/maennaco/images/prof-avatar-female.png';
            }
        }
    } elseif ($usrType = 'company') {
        //Get cmp_role
        $query = "SELECT company_type FROM maenna_company WHERE companyid = %d";
        $result = db_query($query, array($uid));
        $cmp_role_tmp = db_fetch_object($result);
        $cmp_role = $cmp_role_tmp->company_type;
        //Check if user have a profile picture
        if (file_exists('sites/default/images/company/' . $size . '/' . $uid . '.jpg')) {
            $avatar = 'sites/default/images/company/' . $size . '/' . $uid . '.jpg';
        } else if ($cmp_role == 'service') {
            $avatar = ' /themes/maennaco/images/cmp-avatar-service.png';
        } else $avatar = ' /themes/maennaco/images/cmp-avatar-product.png';
    }
    return $avatar;
}

function isApproved($uid) {
    $query = "SELECT approved FROM maenna_people WHERE pid = %d ";
    $result = db_query($query, array($uid));
    $Row = db_fetch_object($result);
    return $Row->approved;
}

function hasApprovedProfile($uid) {
    $sql = "SELECT data_value3  FROM maenna_people_data WHERE pid = %d AND data_type = 'addinfo' AND data_value3 = 'approved' AND data_attr IN ('experties', 'industryview', 'mgmtview')";
    $result = db_query($sql, array($uid));
    $approved = mysql_num_rows($result);
    return ($approved >= 3);
}

function getMatchedProfessionals($companyid) {
    if ($companyid) {
        $Conns = array();
        $q = mysql_query(
            "SELECT * FROM maenna_connections mc
                                                            WHERE conntype = 'match'
                                                            AND status = 'active'
                                                            AND target_uid = '" . $companyid . "'
                                                            AND  NOT EXISTS (SELECT * FROM maenna_connections mc1 WHERE mc1.status = 'active'
                                                                                                              AND mc1.target_uid = '" . $companyid . "'
                                                                                                              AND mc1.assignee_uid = mc.assignee_uid
                                                                                                              AND mc1.conntype <> 'match'
                                                                                                              AND (mc1.conntype = 'advisor' OR mc1.conntype = 'visible' OR mc1.conntype='client' OR mc1.conntype = 'propose' OR mc1.conntype = 'collaborator'))"
        );
        while ($row = mysql_fetch_object($q))
            $Conns[] = $row;
    }
    return $Conns;
}

function getAdminCompanies($admin_id) {
    if ($admin_id) {
        $cmps = array();
        $q = mysql_query(
            "SELECT target_uid,projname FROM maenna_connections conn LEFT JOIN maenna_company ON target_uid = companyid
                                              LEFT JOIN users u ON companyid = u.uid
                                                            WHERE conntype = 'admin'
                                                            AND conn.status = 'active'
                                                            AND conn.role = '3'
                                                            AND u.status = 1  
                                                            AND assignee_uid = '" . $admin_id . "'"
        );
        while ($row = mysql_fetch_array($q)) {
            if ($row['projname'] == '') {
                $projname = 'Project ' . (string) ($row['target_uid'] + 100);
            } else $projname = $row['projname'];
            $cmps[$row['target_uid']] = $projname;
        }
    }
    return $cmps;
}

function showTime($timestamp) {
    if (date('Y-m-d') == date('Y-m-d', $timestamp)) {
        return date("G:i", $timestamp);
    } elseif (date('Y') == date('Y', $timestamp)) {
        return date("M j", $timestamp);
    } elseif (date('Y') != date('Y', $timestamp)) {
        return date("M j, Y", $timestamp);
    }
}

function getAdminName($admin_id) {
    $sql = "SELECT CONCAT(first_name,' ',last_name) AS name FROM users_extend WHERE uid = %d";
    if ($result = db_query($sql, array($admin_id))) {
        $Row = db_fetch_object($result);
    }
    return ucwords($Row->name);
}

function getUserById($id) {
    if (empty($id)) return 'invalid id';
    $sql = mysql_query("SELECT rid FROM users_roles WHERE uid = '" . $id . "' LIMIT 1 ");
    $ridn = mysql_fetch_array($sql);
    if ($ridn['rid'] == '3') {
        $sql = "SELECT users_roles.*, maenna_company.projname FROM users_roles, maenna_company WHERE users_roles.uid = '" . $id . "' AND maenna_company.companyid = '" . $id . "' LIMIT 1";
    } else {
        $sql = "SELECT users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) AS firstname FROM users_roles, maenna_people WHERE users_roles.uid = '" . $id . "' AND maenna_people.pid = '" . $id . "' LIMIT 1";
    }
    $result = mysql_query($sql);
    $Row = mysql_fetch_assoc($result);
    $rid = $ridn['rid'];
    $firstname = ucwords(strtolower($Row['firstname']));
    if ($rid == 6) {
        $output = 'Admin';
    } elseif ($rid == 10) $output = 'Clewed';
    elseif ($rid == "3") {
        if ($Row['projname'] != '') {
            $output = strtoupper($Row['projname']);
        } else $output = 'Project ' . (string) ($id + 100);
    } else {
        $output = $firstname;
    }
    return $output;
}

//CP DEV
function getAccreditationVerData($pid) {
    $sql = "SELECT * FROM maenna_people_data WHERE pid = %d AND data_type = '%s' AND data_attr = '%s'";
    $query = db_query($sql, array($pid, 'verify', 'accreditation'));
    $cnt = mysql_num_rows($query);
    if ($cnt == 0) {
        return false;
    } else return db_fetch_array($query);
}

function get_pro_accreditation_status($pid = null) {
    if ($pid == null) $pid = $_REQUEST['id'];
    $pro_sql = "SELECT * FROM maenna_people_data WHERE data_type = '%s' AND  pid = %d";
    $pro_result = db_query($pro_sql, array('accreditation', $pid));
    $pro_req = mysql_num_rows($pro_result);
    if ($pro_req != 0) return 100; else return 0;
}

function getAccreditationData($pid) {
    $tmp = array();
    $q = mysql_query("SELECT * FROM maenna_people_data WHERE data_type = 'accreditation' AND pid = " . $pid);
    while ($row = mysql_fetch_array($q)) {
        $tmp[$row['data_attr']] = 'checked';
    }
    return $tmp;
}

function requestsNum($cid) {
    $q = mysql_query(
        "SELECT DISTINCT assignee_uid
    FROM maenna_connections
    WHERE target_uid = '" . $cid . "' AND (conntype = 'client' OR conntype='collaborator') AND status = 'pending'"
    );
    return mysql_num_rows($q);
}

function collaboratorReqNum($cid) {
    $q = mysql_query(
        "SELECT DISTINCT assignee_uid
FROM maenna_connections
WHERE target_uid = " . $cid . "
AND STATUS =  'pending' AND conntype='collaborator'"
    );
    return mysql_num_rows($q);
}

function collaboratorStatus($uid, $cid) {
    $q = mysql_query("SELECT status FROM maenna_connections WHERE assignee_uid = '" . $uid . "' AND target_uid = '" . $cid . "' AND conntype = 'collaborator' ORDER BY connid DESC LIMIT 1") or die(mysql_error());
    if ($q) {
        $row = mysql_fetch_array($q);
        return $row['status'];
    }
}

function ifPublicCompany($cid) {
    $q = mysql_query("SELECT * FROM maenna_company WHERE companyid = '" . $cid . "'");
    if ($q) {
        $r = mysql_fetch_array($q);
        return $r['public'];
    }
}

function followerStatus($uid, $cid) {
    $q = mysql_query("SELECT * FROM maenna_followers WHERE uid = '" . $uid . "' AND cid = '" . $cid . "'") or die(mysql_error());
    $num = mysql_num_rows($q);
    return $num;
}

function getFileCategoryName($type) {
    $tags = _TAGS();
    $tmp = $tags[$type];
    if ($tmp == '') {
        return 'Other';
    } else return $tmp;
}

function getConnectedForDiscussions($pid) {
    $sql = "SELECT target_uid FROM maenna_connections WHERE assignee_uid = %d AND status = 'active'
    AND (conntype = 'advisor' OR conntype = 'collaborator' OR conntype = 'client')";
    $q = db_query($sql, array($pid));
    $tmp = array();
    while ($row = db_fetch_array($q)) $tmp[] = $row['target_uid'];
    return $tmp;
}

function ifConnected($cid, $pid) {
    $cid = (int) mysql_real_escape_string($cid);
    $pid = (int) mysql_real_escape_string($pid);
    $q = mysql_query(
        "SELECT * FROM maenna_connections WHERE target_uid = '" . $cid . "' AND assignee_uid = '" . $pid . "' AND status = 'active'
    AND (conntype = 'visible' OR conntype = 'advisor' OR conntype = 'client' OR conntype = 'collaborator' OR conntype = 'follow')

    "
    );
    $cnt = mysql_num_rows($q);
    if ($cnt > 0) {
        return true;
    } else return false;
}

function getCollaborators($cid) {
    $q = mysql_query("SELECT * FROM maenna_connections WHERE target_uid = '" . $cid . "' AND status <> 'deactivated' AND conntype='collaborator'");
    $tmp1 = array();
    $tmp2 = array();
    $colls = array();
    if ($q) {
        while ($r = mysql_fetch_array($q)) if ($r['status'] != 'deactivated') $colls[$r['status']][] = $r['assignee_uid'];
        return $colls;
    }
}

function getFollowing($cid) {
    $q = mysql_query("SELECT * FROM maenna_followers WHERE cid = '" . $cid . "'");
    $colls = array();
    if ($q) {
        while ($r = mysql_fetch_array($q)) $colls[] = $r['uid'];
        return $colls;
    }
}

function getFollowers($uid) {
    $q = mysql_query("SELECT * FROM maenna_followers WHERE uid = '" . $uid . "'");
    $colls = array();
    if ($q) {
        while ($r = mysql_fetch_array($q)) $colls[] = $r['cid'];
        return $colls;
    }
}

function getRelated($id) {
    $relatedPros = array();
    $sql = "SELECT * FROM maenna_connections WHERE (target_uid = '%s' OR assignee_uid = '%s') AND conntype='related' AND status = 'active'";
    $result = db_query($sql, $id, $id);
    while ($Row = db_fetch_object($result)) {
        if ($Row->target_uid == $id) {
            $relatedPros[] = $Row->assignee_uid;
        } else $relatedPros[] = $Row->target_uid;
    }
    return $relatedPros;
}

function getCollaborationCompanies($uid) {
    $q = mysql_query("SELECT * FROM maenna_connections WHERE assignee_uid = '" . $uid . "' AND status <> 'deactivated' AND conntype='collaborator'");
    $tmp1 = array();
    $tmp2 = array();
    $colls = array();
    if ($q) {
        while ($r = mysql_fetch_array($q)) if ($r['status'] != 'deactivated') $colls[$r['status']][] = $r['target_uid'];
        return $colls;
    }
}

function getAdvisorsNum($uid) {
    $sql = "SELECT *
		FROM  `maenna_connections`
		WHERE conntype =  'advisor'
		AND assignee_uid = '" . $uid . "'
		AND STATUS =  'active'";
    $q = mysql_query($sql);
    $num = mysql_num_rows($q);
    return $num;
}

function getDiscussionsNum($uid) {
    $sql = "SELECT *
		FROM  `maenna_company_events_inv`
		WHERE uid = '" . $uid . "'
		AND STATUS =  'confirmed'";
    $q = mysql_query($sql);
    $num = mysql_num_rows($q);
    return $num;
}

function getCollaborationNum($uid) {
    $sql = "SELECT *
		FROM  `maenna_connections`
		WHERE assignee_uid = '" . $uid . "'
		AND STATUS =  'active' AND conntype='collaborator'";
    $q = mysql_query($sql);
    $num = mysql_num_rows($q);
    return $num;
}

function getFollowersNum($uid) {
    $sql = "SELECT *
		FROM  `maenna_followers`
		WHERE uid = '" . $uid . "'";
    $q = mysql_query($sql);
    $num = mysql_num_rows($q);
    return $num;
}

function getUserName($uid, $conn) {
    $sql = "SELECT users_roles.uid + 100 AS uid,
                   users_roles.rid,
                       CASE
                      WHEN users_roles.rid IN (6, 10)
                      THEN
		      CONCAT('ADMIN_',LEFT(users_extend.first_name,1),LEFT(users_extend.last_name,1))
                          WHEN  users_roles.rid NOT IN (6, 10, 3)
                          THEN
                             WHEN  users_roles.rid NOT IN (6, 10, 3)
                          THEN
                             IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname, maenna_people.lastname))
			 WHEN (users_roles.rid = 3)
			 THEN maenna_company.projname
			 END AS `name`
			  FROM    users
			  LEFT JOIN
			  maenna_people
			  ON (users.uid = maenna_people.pid)
			  LEFT JOIN
			  maenna_company
			  ON (maenna_company.companyid = users.uid)
			  LEFT JOIN
			  users_extend
			  ON (users.uid = users_extend.uid)
                       LEFT JOIN
                          users_roles
                       ON (users_roles.uid = users.uid)
		       WHERE users.uid=" . mysql_real_escape_string($uid, $conn);
    $result = mysql_query($sql, $conn);
    $row = mysql_fetch_array($result);
    $firstname = strtoupper($row['name']);
    if (in_array($row['rid'], array(6, 10))) {
        $uid = sprintf("%04s", $row['uid']);
    } else {
        $uid = sprintf("%04s", $row['uid']);
    }
    if ($row['rid'] == 3) {
        $output = $firstname;
    } else $output = $firstname . $uid;
    if ($uid == 160) $output = "Maennaco Admin";
    return ($output);
}

function createRandomPassword() {
    $chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    $nums = "0123456789";
    srand((double) microtime() * 1000000);
    $i = 0;
    $pass = '';
    while ($i <= 7) {
        $num = rand() % 52;
        $tmp = substr($chars, $num, 1);
        $pass = $pass . $tmp;
        $i++;
    }
    while ($i <= 10) {
        $num = rand() % 10;
        $tmp = substr($nums, $num, 1);
        $pass = $pass . $tmp;
        $i++;
    }
    return $pass;
}

function sidebar_box($title, $content, $admin_class = '') {
    $html = "<div class='sidebar_box $admin_class'><div class='wrapper'>";
    if ($title) $html .= "<div class='box_title'>" . ($title) . "</div>";
    $html .= "<div class='box_content'>$content</div></div></div>";
    return $html;
}

function content_box($title, $content, $attr = '') {
    $html = "<div class='content_box $attr'>";

    $title_height = '';
    $close_link = '';
    if (is_string($attr)) {
        if ($attr == 'blue_bar') {
            $title_height = 'style="height:47px;padding-top:15px;font-size:18px;"';
        }
    } else if (is_array($attr)) {
        if (isset($attr['close_link'])) {
            $close_link = '<a href="'.$attr['close_link'].'" class="close-link">X</a>';
        }
    }

    if ($title) $html .= "<div class='box_title shaded_title'" . $title_height . " >" . '<span>'.$title.'</span>'.$close_link."</div>";
    $html .= "<div class='box_content main_content'>$content</div></div>";
    return $html;
}

function clearString($str) {
    return preg_replace("/[^a-z0-9\.]/i", '_', $str);
}

function getCompanyByID($id){
    $sql = "select * from maenna_company where companyid = $id";
    $result = db_query($sql);
    $Data = db_fetch_object($result);

    return $Data;
}

function getActiveConnection($c_id,$uid){
    $sql = "select * from maenna_connections where target_uid = $c_id and assignee_uid = $uid and status = 'active' and conntype = 'match'";
    $result = db_query($sql);
    $Data = db_fetch_object($result);

    return $Data;
}

function getProjectName($id, $isadmin = '') {
    if (empty($id)) return '';
    $sql = "select * from maenna_company where companyid = $id";
    $result = db_query($sql);
    $Data = db_fetch_object($result);
    $projName = $Data->projname;
    if (empty($projName)) {
        $projName = "Project " . sprintf("%03s", $id + 100);
    }
    return $projName;
}

function realid($name) {
    $output = '';
    if ($name) {
        $output = preg_replace("/^[a-z]*0*/i", '', $name);
        $output -= 100;
    }
    return $output;
}

function getProId($id) {
    if (empty($id)) return 'invalid id';
    $sql = mysql_query("SELECT rid FROM users_roles WHERE uid = '" . $id . "' LIMIT 1 ");
    $ridn = mysql_fetch_array($sql);
    if ($ridn['rid'] == '3') {
        $sql = "SELECT users_roles.*, maenna_company.projname FROM users_roles, maenna_company WHERE users_roles.uid = '" . $id . "' AND maenna_company.companyid = '" . $id . "' LIMIT 1";
    } else {
        $sql = "SELECT users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) AS firstname FROM users_roles, maenna_people WHERE users_roles.uid = '" . $id . "' AND maenna_people.pid = '" . $id . "' LIMIT 1";
    }
    $result = mysql_query($sql);
    $Row = mysql_fetch_assoc($result);
    $rid = $ridn['rid'];
    $firstname = ucwords(strtolower($Row['firstname']));
    if ($rid == 6) {
        $output = 'Admin';
    } elseif ($rid == 10) $output = 'Clewed';
    elseif ($rid == "3") {
        if ($Row['projname'] != '') {
            $output = strtoupper($Row['projname']);
        } else $output = 'Project ' . (string) ($id + 100);
    } else {
        $output = $firstname;
    }
    return $output;
}

function filter_num($value) {
    if ($value) {
        return preg_replace("/[^0-9\.\-]/", '', $value);
    } else return 0;
}

function sideBarExcerpt($text, $more_link = '') {
    $output = '';
    if ($text) {
        if (strlen($text) > 200) {
            $output = substr($text, 0, 200) . ' ...';
            if ($more_link) {
                $output .= "<div class=btnwrapper><div class='morebtn'><a href='$more_link' class=tool>More</a></div></div>";
            }
        } else $output = $text;
    }
    return $output;
}

function sideBarMonitoringExcerpt($text, $more_link = '') {
    $output = '';
    if ($text) {
        if (strlen($text) > 200) {
            $output = substr($text, 0, 200) . ' ...';
        } else $output = $text . ' ...';
        if ($more_link) {
            $output .= "<a style=\"font-style:italic;\" href='$more_link' class=tool>More</a>";
//                $output .= "<div class=btnwrapper><div class='morebtn'><a href='$more_link' class=tool>More</a></div></div>";
        }
    }
    return $output;
}

function excerpt($text, $len = 120) {
    if ($text) {
        if (strlen($text) > $len) {
            return substr($text, 0, $len) . ' ...';
        } else return $text;
    } else {
        return '';
    }
}

function contentExcerpt($text, $more_link = '', $limit = 0) {
    $length = 1000;
    if ($limit) $length = $limit;
    $output = '';
    if ($text) {
        if (strlen($text) > $length) {
            $output = substr($text, 0, $length) . ' ...';
            if ($more_link) {
                $output .= "<div class=btnwrapper><div class='morebtn'><a href='$more_link' class=tool>More</a></div></div>";
            }
        } else $output = $text;
    }
    return $output;
}

function contentExcerpt2($text, $more_link = '', $limit = 0) {
    $length = 400;
    if ($limit) $length = $limit;
    $output = '';
    if ($text) {
        if (strlen($text) > $length) {
            $output = "<p class='limited'>" . str_replace("</p>", "", str_replace("<p>", "", substr($text, 0, $length))) . '...';
            if ($more_link) {
                $output .= "<a href='$more_link'  class='tool show_more_pro'><i>More</i></a></p> ";
            }
        } else $output = $text;
    }
    return "<div class=\"excerpt2\">" . $output . "</div><div class='long' style=\"display:none;\">" . $text . "</div>";
}

function Options_state($state = '', $mode = '') {
    $_State = get_us_states();
    return option_code($_State, $state, $mode);
}

function Options_admin_types_($admin_type = '', $mode = '') {
    $_admin_types = _admin_types();
    return option_code($_admin_types, $admin_type, $mode);
}

function Options_degree($degree = '', $mode = '') {
    $D = _Degrees();
    $option_degree = '';
    foreach ($D as $key => $Array) {
        if (empty($key)) continue;
        if (!is_array($Array)) {
            $option_degree .= "\n<option value='$key'>$Array</option>";
        } else {
            $option_degree .= "<optgroup label='$key'>";
            foreach ($Array as $subKey => $title) {
                $selected = "";
                if ((strcasecmp($subKey, $degree) == 0) && $degree) {
                    $selected = 'selected';
                    if ($mode == 1) return $title;
                }
                $option_degree .= "\n<option value='$subKey' $selected>$title</option>";
            }
            $option_degree .= "\n</optgroup>";
        }
    }
    if ($mode == 1) {
        return '';
    } else return $option_degree;
}

function Options_mlvl($val = '', $mode = '') {
    $_MLVL = _managementLevel();
    return option_code($_MLVL, $val, $mode);
}

function Options_rights($Rights, $selected, $tag_name) {
    if (count($Rights) == 2) {
        $return = '<select name="' . $tag_name . '">';
        if (empty($selected)) $selected = 'hide';
        foreach ($Rights as $key => $title) {
            $sel = '';
            if ($key == $selected || ($key == 'read' && $selected == 'write')) $sel = "selected";
            $return .= "<option value='$key' $sel>$title</option>";
        }
        $return .= "</select>";
        return $return;
    } else {
        $return = '<select name="' . $tag_name . '">';
        if (empty($selected)) $selected = 'hide';
        foreach ($Rights as $key => $title) {
            $sel = '';
            if ($key == $selected) $sel = "selected";
            $return .= "<option value='$key' $sel>$title</option>";
        }
        $return .= "</select>";
        return $return;
    }
}

function Options_infoTopic($topic = '', $mode = '') {
    $Topics = _Topics();
    return option_code($Topics, $topic, $mode);
}

function Options_infoType($infoType = '', $mode = '') {
    $InfoTypes = _InfoTypes();
    return option_code($InfoTypes, $infoType, $mode);
}

function Options_industry($industry = '', $mode = '') {
    $Industry = _INDUSTRY(); //_experties
    $option_industry = '';
    if ((!empty($mode)) && empty($industry)) return '';
    foreach ($Industry as $key => $Array) {
        if (empty($key)) continue;
        if (!is_array($Array)) {
            $option_industry .= "\n<option value='$key'>$Array</option>";
        } else {
            $option_industry .= "<optgroup label='$key'>";
            foreach ($Array as $subKey => $title) {
                $selected = "";
                if (($subKey == $industry) && $industry) {
                    $selected = 'selected';
                    if ($mode == 1) return $title;
                }
                $option_industry .= "\n<option value='$subKey' $selected>$title</option>";
            }
            $option_industry .= "\n</optgroup>";
        }
    }
    if ($mode) return '';
    return $option_industry;
}

function Options_experties($experties = '', $mode = '') {
    $Experties = _experties();
    $option_experties = '';
    foreach ($Experties as $key => $Array) {
        if (empty($key)) continue;
        if (!is_array($Array)) {
            if ((strcasecmp($key, $experties) == 0) && $experties) {
                $selected = 'selected';
                if ($mode == 1) return $Array;
            }
            else $selected = '';
            $option_experties .= "\n<option value='$key' $selected>$Array</option>";
        } else {
            $option_experties .= "<optgroup label='$key'>";
            foreach ($Array as $subKey => $title) {
                $selected = "";
                if ((strcasecmp($subKey, $experties) == 0) && $experties) {
                    $selected = 'selected';
                    if ($mode == 1) return $title;
                }
                $option_experties .= "\n<option value='$subKey' $selected>$title</option>";
            }
            $option_experties .= "\n</optgroup>";
        }
    }
    if ($mode == 1) {
        return '';
    } else return $option_experties;
}

function Options_proType($proType = '', $mode = '') {
    $ProType = _pro_roles();
    return option_code($ProType, $proType, $mode);
}

function Options_cmpType($proType = '', $mode = '') {
    $cmpType = _cmp_types();
    return option_code($cmpType, $proType, $mode);
}

function Options_rating($proType = '', $mode = '') {
    $ProType = array('1' => 1, '2' => 2, '3' => 3, '4' => 4, '5' => 5);
    return option_code($ProType, $proType, $mode);
}

function Options_status($proType = '', $mode = '') {
    $ProType = array('pending' => 'pending', 'approved' => 'approved', 'remove' => 'remove');
    return option_code($ProType, $proType, $mode);
}

function Options_public($proType = '', $mode = '') {
    $PublicType = array('0' => 'Private', '1' => 'Public');
    return option_code($PublicType, $proType, $mode);
}

function Options_selection($proType = '', $mode = '') {
    $ProType = array('0' => '', '1' => 'select');
    return option_code($ProType, $proType, $mode);
}

function Options_month($month = '', $mode = '') {
    $Month = _Month();
    return option_code($Month, $month, $mode);
}

function Options_hour($hour = '', $mode = '') {
    $Hours = array();
    for ($i = 6; $i <= 22; $i++) $Hours["$i"] = sprintf("%02s", $i);
    return option_code($Hours, $hour, $mode);
}

function Options_capacity($capacity = '', $mode = '') {
    $Capacity = array();
    for ($i = 1; $i <= 100; $i++) $Capacity["$i"] = $i;
    return option_code($Capacity, $capacity, $mode);
}

function Options_weight($_weight = '', $mode = '') {
    $Weight = array();
    for ($i = 1; $i <= 150; $i++) {
        $Weight["$i"] = $i;
    }
    return option_code($Weight, $_weight, $mode);
}

function Options_minute($minute = '', $mode = '') {
    $Minute = array('0' => '00', '15' => 15, '30' => 30, '45' => 45);
    return option_code($Minute, $minute, $mode);
}

function option_code($Array, $val, $mode) {
    $option = '';
    if ($mode && empty($val)) return '';
    foreach ($Array as $key => $title) {
        $selected = "";
        if (("$key" == "$val") && $val) {
            $selected = 'selected=true';
            if ($mode) return $title;
        }
        $option .= "\n<option value='$key' $selected>$title</option>";
    }
    if ($mode) return '';
    return $option;
}

function render_table($Table) {
    if (empty($Table) || (!is_array($Table))) return "invalid table array";
    $class = sget($Table, 'class');
    $style = sget($Table, 'style');
    $html = "<table class='$class' $style>";
    $numOfCol = $Table['numOfCol'];
    if (isset($Table['title']) && (!empty($Table['title']))) {
        $html = "<div style='font:italic bold 14px arial'>" . $Table['title'] . "</div>" . $html;
    }
    if (isset($Table['thead']) && (!empty($Table['thead']))) {
        $html .= "\n<thead><tr>";
        foreach ($Table['thead'] as $Header) {
            $style = (sget($Header, 'style'));
            $label = ucwords(sget($Header, 'label'));
            $html .= "\n<td style='$style'>$label</td>";
        }
        $html .= "\n</tr></thead>";
    }
    $html .= "\n<tbody>";
    foreach ($Table['tbody'] as $Row) {
        $html .= "\n<tr>";
        foreach ($Table['thead'] as $H) {
            $text = sget($Row, strtolower($H['label']));
            $html .= "\n<td>$text</td>";
        }
        $html .= "\n</tr>";
    }
    $html .= "</tbody></table>";
    return $html;
}

function button_link($Links, $align = '') {
    if ($align != '') {
        $align = "text-align:$align";
    }
    $html = "<div style='margin-top:4px; width:100%;$align'>";
    $counter = 1;
    foreach ($Links as $link) {
        $label = sget($link, 'label');
        $target = sget($link, 'url');
        $html .= "<a href='$target' class=button>$label</a>";
        if ($counter != count($Links)) $html .= "&nbsp;&nbsp;";
        $counter++;
    }
    $html .= "</div>";
    return $html;
}

function jsbutton_link($Array) {
    $position = sget($Array, 'position');
    $Links = $Array['links'];
    if ($position) {
        $position = "text-align:$position";
    }
    $output = "<div style='width:100%;$position'>";
    $counter = 1;
    if (is_array($Links)) {
        foreach ($Links as $L) {
            $extra = sget($L, 'extra');
            $class = sget($L, 'class');
            $label = sget($L, 'label');
            $url = sget($L, 'url');
            $output .= "<a href='$url' class='button $class' $extra>$label</a>";
            if ($counter != count($Links)) $output .= "&nbsp;&nbsp;";
            $counter++;
        }
    }
    $output .= "</div>";
    return $output;
}

function text_link($Links, $align = '') {
    if ($align != '') {
        $align = "text-align:$align";
    }
    $html = "<div style='margin-top:4px; width:100%;$align'>";
    $counter = 1;
    foreach ($Links as $link) {
        $label = sget($link, 'label');
        $target = sget($link, 'url');
        $html .= "<a href='$target'>$label</a>";
        if ($counter != count($Links)) $html .= "&nbsp;&nbsp;&nbsp;";
        $counter++;
    }
    $html .= "</div>";
    return $html;
}

function js_init($code) {
    if (empty($code)) return '';
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

function rebuild_url($Keys = '', $Array = null) {
    if (empty($Keys)) return '';
    if (empty($Array)) $Array = $_REQUEST;
    $Components = array();
    foreach ($Keys as $key) {
        if (sget($Array, $key)) {
            $Components[] = "$key=" . sget($Array, $key);
        }
    }
    return "/account?" . implode("&", $Components);
}

function hidden_post_values($Keys = '') {
    $output = '';
    if (empty($Keys)) return '';
    foreach ($Keys as $key) {
        $val = sget($_REQUEST, $key);
        $output .= "\n<input type=hidden name='$key' value='$val' />";
    }
    return $output;
}

function backup_copy($text, $operation = '') {
    global $user;
    $uid = $user->uid;
    $sql = "INSERT INTO maenna_user_post(uid, serialized_data, operation) VALUES(%d, '%s', '%s')";
    db_query($sql, array($uid, $text, $operation));
}

function upload_file($FILE) {
    if ($FILE['error'] == 0) {
        $filename = time() . '_' . $FILE['name'];
        $path = "./" . file_directory_path() . "/" . $filename;
        if (move_uploaded_file($FILE['tmp_name'], $path)) {
            return array(
                'name' => $filename,
                'type' => $FILE['type']
            );
        }
    }
    return false;
}

function get_search_result($user_type) {
    $sql = '';
    //
    $limit = 20;
    $page = sget($_REQUEST, 'page') ? (int) (sget($_REQUEST, 'page')) : 1;
    $start = ($page - 1) * $limit;
    $SQL_where_filter = "";
    $SQL_data = array();
    if ($user_type == 'p') {
        //$Positions = get_p_positions();
        //$Education = get_p_education();
        $rid = array(4, 5, 7);
        $type = sget($_REQUEST, 'user_type');
        if ($type > 0) $rid = array($type);
        if (sget($_REQUEST, 'sector')) {
            $SQL_where_filter .= " and ta.sector = '%d'";
            $SQL_data[] = sget($_REQUEST, 'sectors');
        }
        $SQL_begin = " SELECT * FROM users_extend AS ta, users_roles AS tb  ";
        $SQL_where .= "where ta.uid = tb.uid and tb.rid in (" . implode(',', $rid) . ") ";
        $SQL_limit = " limit $start, $limit";
        $SQL_begin_total = " SELECT count(*) AS total FROM users_extend AS ta, users_roles AS tb  ";
    } elseif ($user_type == 'admin') {
        $rid = array(6);
        $SQL_begin = " SELECT *, tc.mail FROM users_extend AS ta, users_roles AS tb, users AS tc ";
        $SQL_where .= "where tc.status != 0 and ta.uid = tc.uid and ta.uid = tb.uid and tb.rid in (" . implode(',', $rid) . ") ";
        $SQL_limit = " limit $start, $limit";
        $SQL_begin_total = " SELECT count(*) AS total FROM users_extend AS ta, users_roles AS tb, users AS tc   ";
    } elseif ($user_type == 'company') {
        $rid = array(3);
        $SQL_begin = " SELECT *, tc.mail FROM users_extend AS ta, users_roles AS tb, users AS tc ";
        $SQL_where .= "where ta.uid = tc.uid and ta.uid = tb.uid and tb.rid in (" . implode(',', $rid) . ") ";
        $SQL_limit = " limit $start, $limit";
        $SQL_begin_total = " SELECT count(*) AS total FROM users_extend AS ta, users_roles AS tb, users AS tc   ";
    } elseif ($user_type == 'investor') {
        $rid = array(8);
        $SQL_begin = " SELECT *, tc.mail FROM users_extend AS ta, users_roles AS tb, users AS tc ";
        $SQL_where .= "where ta.uid = tc.uid and ta.uid = tb.uid and tb.rid in (" . implode(',', $rid) . ") ";
        $SQL_limit = " limit $start, $limit";
        $SQL_begin_total = " SELECT count(*) AS total FROM users_extend AS ta, users_roles AS tb, users AS tc   ";
    }
    $Result = db_query($SQL_begin_total . $SQL_where . $SQL_where_filter, $SQL_data);
    $row = db_fetch_object($Result);
    $total = $row->total;
    $SQL = $SQL_begin . $SQL_where . $SQL_where_filter . $SQL_limit;
    $Data = db_query($SQL, $SQL_data);
    $Result = array();
    while (($row = db_fetch_object($Data)) !== false) {
        $Result[] = $row;
    }
    return array('total' => $total, 'limit' => $limit, 'result' => $Result, 'userType' => $user_type);
}

function render_people_search($Search_parameter, $Result, $paginationBaseURL) {
    $block = render_searchbar($Search_parameter);
    $block .= render_result_page($Search_parameter, $Result, $paginationBaseURL);
    //echo $block;exit;
    return $block;
}

function render_result_page($Search_parameter, $Search_result, $paginationBaseURL) {
    $Pagenation = array(
        'total'        => $Search_result['total'],
        'limit'        => $Search_result['limit'],
        'baseurl'      => $paginationBaseURL,
        'num_of_links' => 8,
    );
    $Result = $Search_result['result'];
    //test_array($Result);
    //var_dump( $Search_result['userType']);
    $block = "";
    if ($Search_result['userType'] == 'p') {
        foreach ($Result as $row) {
            $mcid = '';
            $uid = $row->uid;
            $mcid = sprintf("%06d", $uid + 1100);
            $block .=
                "<div style='position:relative;padding:8px 10px 15px 10px;border:solid 1px #f2f2f2;margin-top:-1px;'>
            <div style='font-size:18px;color:#6792d0;line-height:26px;height:26px;padding-bottom:8px;'>MC " . $mcid . "</div>
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
    } elseif ($Search_result['userType'] == 'admin') {
        $block .= "
                    <form action='/account' method='post'>
                    <input type=hidden name=tab value=admins />
                    <input type=hidden name=update_section value=remove_admins />
                    <table cellspacing=0 cellpadding=0 border=0 width='100%' class='left-align'>" .
            "<tr><td></td><td>Name</td><td>Email</td><td></td><td>Detail</td></tr>";
        foreach ($Result as $row) {
            $id = $row->uid;
            $block .= "<tr><td style='width:30px;'><input type=checkbox name='uids[]' value='" . $row->uid . "'></td>
                                <td style='width:140px;'>" . ucwords($row->first_name . " " . $row->last_name) . "</td>
                                <td>" . $row->mail . "</td><td></td>
                                <td style='width:60px;'><a href='account?tab=admins&page=admin_detail&id=$id' >view</a></td></tr>";
        }
        $block .= "</table>";
        $block .= "<div style='margin-top:15px;'><input type='submit' name=submit value='delete' class='button' />";
        $block .= "</div></form>";
    } elseif ($Search_result['userType'] == 'company') {
        $block .= "<table class='data-table'>" .
            "<thead><tr><th><input type=checkbox class='select_all_cb'></th><th>Name</th><th>Email</th><th></th><th>Detail</th></tr></thead><tbody>";
        foreach ($Result as $row) {
            $block .= "<tr><th><input type=checkbox name='uids[]' value='" . $row->uid . "'></th><th>" . strtoupper($row->company_name) . "</th>
                        <th>" . $row->mail . "</th><th></th><th><a href='/account?a=companies&v=company_mgmt&cid=" . $row->uid . "'>view</a></th></tr>";
        }
        $block .= "</tbody></table>";
    } elseif ($Search_result['userType'] == 'investor') {
        $block .= "<table class='data-table'>" .
            "<thead><tr><th><input type=checkbox class='select_all_cb'></th><th>Name</th><th>Email</th><th></th><th>Detail</th></tr></thead><tbody>";
        foreach ($Result as $row) {
            $block .= "<tr><th><input type=checkbox name='uids[]' value='" . $row->uid . "'></th><th>" . $row->first_name . " " . $row->last_name . "</th>
                        <th>" . $row->mail . "</th><th></th><th><a href=''>view</a></th></tr>";
        }
        $block .= "</tbody></table>";
    }
    $block .= "<div style='width:100%;'>" . pagination($Pagenation) . "</div>";
    return $block;
}

function render_searchbar($SP) {
    //var_dump($SP);
    if ((!is_array($SP)) || count($SP) == 0) return "";
    $block = "<div style='clear:both'></div>";
    $block .= "<form action='' method='get'><div class='filter_bar'><table border=0 cellspacing=0 cellpadding=0 width='100%'><tr>";
    $block .= "<td style='border-top:none;padding-bottom:5px'>";
    foreach ($SP as $key => $value) {
        $selection = sget($_REQUEST, $key);
        $block .= "<div style='float:left;padding-right:4px;'><label class='italic e-text grey'>" . ucwords($key) . ":</label><br> ";
        $counter = 1;
        if (is_array($value)) {
            $block .= "\n<select class='search_select' name='$key'>";
            foreach ($value as $k => $v) {
                $v = trim($v);
                $selected = ($k == $selection) ? ' selected ' : '';
                if ($counter == 1 && (!empty($v))) {
                    $block .= "<option $selected> </option>";
                }
                $counter = 2;
                $block .= "<option value='$k' $selected>" . $v . "</option>";
            }
            $block .= '</select></div>';
        }
    }
    $block .= "<div style='float:left;padding-right:20px;'><br><input type=image value=filter src='/themes/maennaco/images/filter.png' style='display:block'></div>";
    $block .= "</td>";
    $block .= "</tr></table></div>";
    $block .= "<input type=hidden name=a value='cpro'>";
    $block .= "<input type=hidden name=o value='view'>";
    $block .= "</form>";
    // **************************
    return $block;
}

function MergeArrays($Arr1, $Arr2) // overwrites
{
    foreach ($Arr2 as $key => $Value) {
        if (array_key_exists($key, $Arr1) && is_array($Value)) {
            $Arr1[$key] = MergeArrays($Arr1[$key], $Arr2[$key]);
        } else {
            $Arr1[$key] = $Value;
        }
    }
    return $Arr1;
}

function assign_admin($op = null) {
    global $AccessObj;
    if ($AccessObj->user_type != 'super') return '';
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
    require_once("." . base_path() . path_to_theme() . "/includes/classes/maenna_users.inc");
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Assign admin";
    $section = __FUNCTION__;
    $content = '';
    $redirect = rebuild_url(array('tab', 'page', 'mtab', 'id'));
    $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
    $Block['title'] .= "<div class=editbtn><a href='$redirect&section=${section}&view=edit' class=tool>Edit</a></div>";
    $Com_conns = Connections::Com_conns($companyid);
    $Assigned = $Com_conns['Admin'];
    if ($op && $op != 'update') {
        $view = sget($_REQUEST, 'view');
        if (empty($view) && $section == sget($_REQUEST, 'section')) {
            if (count($Assigned) == 0) {
                $content = 'no admin user found';
            } else {
                foreach ($Assigned as $Admin) {
                    $admin_id = $Admin->assignee_uid;
                    $name = ucwords(Mae_users::get_name($admin_id));
                    $content .= "\n<div class=row ><a href=''>$name</a></div>";
                }
            }
        } elseif ($view == 'edit' && $section == sget($_REQUEST, 'section')) {
            $All_admins = Mae_users::all_admins();
            //test_array($All_admins);
            if (count($All_admins) > 0) {
                $content .= "<form method=post action='/account'>";
                foreach ($All_admins as $Mae_admin) {
                    $admin_uid = sget($Mae_admin, 'uid');
                    $admin_name = ucwords(sget($Mae_admin, 'first_name')) . ' ' . ucwords(sget($Mae_admin, 'last_name'));
                    $str_checkbox = "";
                    foreach ($Assigned as $Admin) {
                        $AA_uid = $Admin->assignee_uid;
                        if ($AA_uid == $admin_uid) {
                            $str_checkbox = "checked='checked'";
                        }
                    }
                    $content .= "<div class=row>
                        <input type=radio name='admins[]' value='$admin_uid' $str_checkbox /> &nbsp;&nbsp;
                        $admin_name

                    </div>";
                }
                $HV = hidden_post_values(array('tab', 'page', 'id', 'section', 'mtab'));
                $content .= <<< END
                    $HV
                    <input type=hidden name='update_section' value='assign_admin' />
                    <div><input type=submit name=submit value='submit' class='button' />&nbsp;&nbsp;
                            <a href='$redirect' class=button>cancel</a>
                    </div>
                    </form>
END;
            }
        }
    } elseif ($op == 'update') {
        $Posted_admins = $_POST['admins'];
        if (count($Posted_admins) == 1) {
            $assignee_uid = $Posted_admins[0];
            if (Connections::assign_admin($companyid, $assignee_uid)) {
                drupal_set_message("Setting updated");
            } else {
                drupal_set_message("Failed to set admin", 'error');
            }
        }
        return '';
    }
    $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
    return $Block;
}

function info_tools($op = null) {
    global $user, $AccessObj;
    $editorid = $user->uid;
    if ($AccessObj->user_type != 'super') return '';
    $id = sget($_REQUEST, 'id');
    $page = sget($_REQUEST, 'page');
    $target = sget($_REQUEST, 'target');
    $eventtype = sget($_REQUEST, 'eventtype');
    $postingtype = sget($_REQUEST, 'postingtype');
    $selectedStyle = 'style="color:#00a2bf;"';
    $Block['title'] = '';
    $content = '';
    $time = time();
    $redirect = rebuild_url(array('tab'));
    $content = "
        <div style='padding-left:15px;'>
            <div class=row><a href='$redirect&page=insights&panel=upload_box_panel'>ADD INFORMATION</a></div>
            <div class=row><a href='$redirect&page=postings&panel=posting_panel&postingtype=council' " . (($page == 'postings' && $postingtype == 'council') ? $selectedStyle : '') . ">POST ADVISORY COUNCIL OPENNING</a></div>
            <div class=row><a href='$redirect&page=postings&panel=posting_panel&postingtype=project' " . (($page == 'postings' && $postingtype == 'project') ? $selectedStyle : '') . ">POST A SPECIAL PROJECT</a></div>
            <div class=row><a href='$redirect&page=events&panel=event_posting&eventtype=advisory' " . (($page == 'events' && $eventtype == 'advisory') ? $selectedStyle : '') . ">POST ADVISORY COUNCIL EVENT</a></div>
            <div class=row><a href='$redirect&page==events&panel=event_posting&eventtype=group'>POST GROUP EVENT</a></div>
            <div class=row><a href=''>ASK A QUESTION</a></div>
            <div>&nbsp;</div>
            <div class=row><a href='$redirect&page=qmgmt&panel=qmgmt&target=company' " . (($page == 'qmgmt' && $target == 'company') ? $selectedStyle : '') . ">COMPANY QUESTIONNAIRE</a></div>
            <div class=row><a href='$redirect&page=qmgmt&panel=qmgmt&target=pro' " . (($page == 'qmgmt' && $target == 'pro') ? $selectedStyle : '') . ">PROFESSIONAL QUESTIONNAIRE</a></div>
            <div>&nbsp;</div>
            <div class=row><a href=''>REFER A COMPANY</a></div>
            <div class=row><a href=''>REFER A PROFESSIONAL</a></div>
        </div>
    ";
    $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
    return $Block;
}

function infoTabs($Tabs, $tab = '') {
    $Block['title'] = 'Information';
    $content = '';
    $page = sget($_REQUEST, 'page');
    $Tabs = array('insights' => '', 'postings' => '', 'events' => '', 'qmgmt' => 'questionnaire');
    $cur_pos = 0;
    foreach ($Tabs as $key => $title) {
        if (empty($title)) $title = $key;
        $active = '';
        $class = '';
        if ($key == $page) $active = ' active-trail active';
        if ($cur_pos == 0) {
            $class = 'first';
        } elseif ($cur_pos == (count($Tabs) - 1)) $class = 'last';
        $content .= "\n<li class=' $class $active '>
            <a href='/account?tab=information&page=$key' class='$active'>" . strtoupper($title) . "</a></li>";
        $cur_pos++;
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

function admin_section_title($op = null) {
    $html = "<div class='sidebar_box path_to_launch' style='margin-top:30px;'><div class='wrapper'>";
    $html .= '<div class="block_title">
                <a data-tooltip="Click to open" id="administrative" href="#administrative_dialog_modal" data-toggle="modal" data-title="Administrative" style="display:block;color:#929497;width:260px;margin-left:-10px;">Administrative <span class="collapse-icon" style="float: right">+</span>
                </a>
            </div>';
    $html .= "<div class='box_content'>&nbsp;</div></div></div>";
    $html .= "<script>
                $(document).ready(function(){
                    $('a').click(function(e) {
                        // e.preventDefault();
                        var link = this.getAttribute('href');
                        if(link != null){
                            // console.log(link);
                            // $(document.body).load(link);
                            // window.location.replace(link);
                            // window.history.replaceState('','',link);
                        }
                      });
                });
            </script>";
    $Block['body'] =  $html;
    
    // $Block['body'] = sidebar_box('Administrative', '&nbsp;', 'css_admin_box_title');
    return $Block;
}

function replace_email($subject) {
    $pattern = "/(?:[a-z0-9!#$%&'*+=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+=?^_`{|}~-]+)*|\"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*\")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\])/";
    return preg_replace($pattern, '<i>e-mail obscured</i>', $subject);
}

/**
 * Get image path for pro avatar for requested size
 * $proRecord record of the user (mysql row object)
 * $width
 * $height
 */
function getProAvatar($proRecord, $width, $height) {
    if (is_array($proRecord)) {
        $proRecord = json_decode(json_encode($proRecord));
    }
    if (!empty($proRecord->pic)) {
        $widthHeight = ($width == '150' ? '150' : ($width == '117' ? '117' : ($width == '50' ? '50' : 'original')));
        $widthHeight .= ($height == '150' ? 'x150' : ($height == '117' ? 'x117' : ($width == '50' ? 'x50' : '')));
        return '/sites/default/images/profiles/' . $widthHeight . '/' . $proRecord->pic;
    } else {
        $src = '/themes/maennaco/images/';
        if ($proRecord->gender == 'f') {
            if ($proRecord->protype == 'analyst') {
                return $src . 'Prof_Analyst_F.jpg';
            } elseif ($proRecord->protype == 'executive') {
                return $src . 'Prof_Executive_F.jpg';
            } else {
                return $src . 'Prof_Other_F.jpg';
            }
        } else {
            if ($proRecord->protype == 'analyst') {
                return $src . 'Prof_Analyst_M.jpg';
            } elseif ($proRecord->protype == 'executive') {
                return $src . 'Prof_Executive_M.jpg';
            } else {
                return $src . 'Prof_Other_M.jpg';
            }
        }
    }
}

function imagetograyscale($im) {
    if (imageistruecolor($im)) {
        imagetruecolortopalette($im, false, 256);
    }
    for ($c = 0; $c < imagecolorstotal($im); $c++) {
        $col = imagecolorsforindex($im, $c);
        $gray = round(0.299 * $col['red'] + 0.587 * $col['green'] + 0.114 * $col['blue']);
        imagecolorset($im, $c, $gray, $gray, $gray);
    }
}

function deleteUserImages($type, $imageName) {
    foreach (array('original', '150x150', '117x117', '50x50') as $size) {
        unlink('sites/default/images/' . $type . '/' . $size . '/' . $imageName);
    }
    return true;
}

function resize_and_upload_image($image, $id, $type, $element_id) {
    $allowedExts = array("jpg", "jpeg", "gif", "png");
    $extension = end(explode(".", $image["name"]["$element_id"]));
    if (in_array($image['type'][$element_id], array('image/gif', 'image/jpeg', 'image/pjpeg', 'image/png')) && in_array($extension, $allowedExts)) {
        if ($image["error"]['logo'] > 0) {
            drupal_set_message('Image upload error.', 'error');
        } else {
            switch (strtolower($image['type']["$element_id"])) {
                case 'image/jpeg':
                    $img = imagecreatefromjpeg($image['tmp_name']["$element_id"]);
                    break;
                case 'image/png':
                    $img = imagecreatefrompng($image['tmp_name']["$element_id"]);
                    break;
                case 'image/gif':
                    $img = imagecreatefromgif($image['tmp_name']["$element_id"]);
                    break;
                default:
                    exit('Unsupported type: ' . $image['type']["$element_id"]);
            }
            //First save the original picture
            if (!file_exists('sites/default/images/' . $type . '/original/')) {
                mkdir('sites/default/images/' . $type . '/original/', 0777, true);
            }
            move_uploaded_file($image['tmp_name']["$element_id"], 'sites/default/images/' . $type . '/original/' . $id . "." . $extension) or die("problem" . 'sites/default/images/' . $type . '/original/' . $id . "." . $extension);
            //Resize to 150x150 px
            $new150 = imagecreatetruecolor(150, 150);
            $oldHeight = imagesy($img);
            $oldWidth = imagesx($img);
            if ($oldHeight < $oldWidth) {
                $newImageHeight = 150;
                $newImageWidth = ceil((150 * $oldWidth) / $oldHeight);
                imagecopyresampled($new150, $img, -ceil(($newImageWidth - 150) / 2), 0, 0, 0, $newImageWidth, $newImageHeight, $oldWidth, $oldHeight) or die("problem0");
            } else {
                $newImageHeight = ceil((150 * $oldHeight) / $oldWidth);
                $newImageWidth = 150;
                imagecopyresampled($new150, $img, 0, -ceil(($newImageHeight - 150) / 2), 0, 0, $newImageWidth, $newImageHeight, $oldWidth, $oldHeight) or die("problem1");
            }
            //imagetograyscale($new150,383);
            if (!file_exists('sites/default/images/' . $type . '/150x150/')) {
                mkdir('sites/default/images/' . $type . '/150x150/', 0777, true);
            }
            imagejpeg($new150, 'sites/default/images/' . $type . '/150x150/' . $id . ".jpg") or die("problem22" . 'sites/default/images/' . $type . '/150x150/' . $id . ".jpg");
            //Resize to 117x117 px
            $new117 = imagecreatetruecolor(117, 117);
            $oldHeight = imagesy($img);
            $oldWidth = imagesx($img);
            if ($oldHeight < $oldWidth) {
                $newImageHeight = 117;
                $newImageWidth = ceil((117 * $oldWidth) / $oldHeight);
                imagecopyresampled($new117, $img, -ceil(($newImageWidth - 117) / 2), 0, 0, 0, $newImageWidth, $newImageHeight, $oldWidth, $oldHeight) or die("problem0");
            } else {
                $newImageHeight = ceil((117 * $oldHeight) / $oldWidth);
                $newImageWidth = 117;
                imagecopyresampled($new117, $img, 0, -ceil(($newImageHeight - 117) / 2), 0, 0, $newImageWidth, $newImageHeight, $oldWidth, $oldHeight) or die("problem1");
            }
            //imagetograyscale($new117,383);
            if (!file_exists('sites/default/images/' . $type . '/117x117/')) {
                mkdir('sites/default/images/' . $type . '/117x117/', 0777, true);
            }
            imagejpeg($new117, 'sites/default/images/' . $type . '/117x117/' . $id . ".jpg") or die("problem22" . 'sites/default/images/' . $type . '/117x117/' . $id . ".jpg");
//Resize to 50x50
            $new50 = imagecreatetruecolor(50, 50);
            if ($oldHeight < $oldWidth) {
                $newImageHeight = 50;
                $newImageWidth = ceil((50 * $oldWidth) / $oldHeight);
                imagecopyresampled($new50, $img, -ceil(($newImageWidth - 50) / 2), 0, 0, 0, $newImageWidth, $newImageHeight, $oldWidth, $oldHeight) or die("problem2");
            } else {
                $newImageHeight = ceil((50 * $oldHeight) / $oldWidth);
                $newImageWidth = 50;
                imagecopyresampled($new50, $img, 0, -ceil(($newImageHeight - 50) / 2), 0, 0, $newImageWidth, $newImageHeight, $oldWidth, $oldHeight) or die("problem3");
            }
            //imagetograyscale($new50,383);
            if (!file_exists('sites/default/images/' . $type . '/50x50/')) {
                mkdir('sites/default/images/' . $type . '/50x50/', 0777, true);
            }
            imagejpeg($new50, 'sites/default/images/' . $type . '/50x50/' . $id . ".jpg") or die("problem4");
        }
        return true;
    } else {
        drupal_set_message('File type not allowed.Only jpeg,png or gif allowed.', 'error');
        return false;
    }
}

function resize_and_upload_image1($image, $id) {
    $allowedExts = array("jpg", "jpeg", "gif", "png");
    $extension = end(explode(".", $image["name"][$element_id]));
    if ((($image["type"] == "image/gif")
            || ($image["type"] == "image/jpeg")
            || ($image["type"] == "image/pjpeg"))
        && in_array($extension, $allowedExts)
    ) {
        if ($image["error"] > 0) {
            drupal_set_message('Image upload error.', 'error');
        } else {
            switch (strtolower($image['type'])) {
                case 'image/jpeg':
                    $img = imagecreatefromjpeg($image['tmp_name']);
                    break;
                case 'image/png':
                    $img = imagecreatefrompng($image['tmp_name']);
                    break;
                case 'image/gif':
                    $img = imagecreatefromgif($image['tmp_name']);
                    break;
                default:
                    exit('Unsupported type: ' . $image['type']);
            }
//First save the original picture
            move_uploaded_file($image['tmp_name'], 'sites/default/images/profiles/original/' . $id . "." . $extension) or die("problem");
//Resize to 117x117 px
            $new117 = imagecreatetruecolor(117, 117);
            $oldHeight = imagesy($img);
            $oldWidth = imagesx($img);
            if ($oldHeight < $oldWidth) {
                $newImageHeight = 117;
                $newImageWidth = ceil((117 * $oldWidth) / $oldHeight);
                imagecopyresampled($new117, $img, -ceil(($newImageWidth - 117) / 2), 0, 0, 0, $newImageWidth, $newImageHeight, $oldWidth, $oldHeight) or die("problem");
            } else {
                $newImageHeight = ceil((117 * $oldHeight) / $oldWidth);
                $newImageWidth = 117;
                imagecopyresampled($new117, $img, 0, -ceil(($newImageHeight - 117) / 2), 0, 0, $newImageWidth, $newImageHeight, $oldWidth, $oldHeight) or die("problem");
            }
            imagetograyscale($new117, 383);
            imagejpeg($new117, 'sites/default/images/profiles/117x117/' . $id . ".jpg") or die("problem");
//Resize to 50x50
            $new50 = imagecreatetruecolor(50, 50);
            if ($oldHeight < $oldWidth) {
                $newImageHeight = 50;
                $newImageWidth = ceil((50 * $oldWidth) / $oldHeight);
                imagecopyresampled($new50, $img, -ceil(($newImageWidth - 50) / 2), 0, 0, 0, $newImageWidth, $newImageHeight, $oldWidth, $oldHeight) or die("problem");
            } else {
                $newImageHeight = ceil((50 * $oldHeight) / $oldWidth);
                $newImageWidth = 50;
                imagecopyresampled($new50, $img, 0, -ceil(($newImageHeight - 50) / 2), 0, 0, $newImageWidth, $newImageHeight, $oldWidth, $oldHeight) or die("problem");
            }
            imagetograyscale($new50, 383);
            imagejpeg($new50, 'sites/default/images/profiles/50x50/' . $id . ".jpg") or die("problem");
        }
    } else {
        drupal_set_message('File type not allowed.Only jpeg,png or gif allowed.', 'error');
    }
}

function ifFundraisingCheckListComplete($checkboxes) {
    if (empty($checkboxes)) {
        return false;
    }

    $count = count($checkboxes);
    $checked = 0;
    foreach($checkboxes as $key => $checkbox){
        if($checkbox) $checked++;
        elseif ($key != 'fundraising_declined') return 0;
    }
    //Additional check
    if (($checked == $count - 1) && !$checkboxes['fundraising_declined']) return 1;
    else return 0;
}

function checkCompaniesReadyForFundraising($companyIds) {

    $ifFundraisingListComplete = array();
        foreach ($companyIds as $companyId) {
            if ($companyId) {
                $checkboxes = getCompanyCheckboxes($companyId)['fundraising'];
                $ifFundraisingListComplete[$companyId] = ifFundraisingCheckListComplete($checkboxes);
            }
        }
    return $ifFundraisingListComplete;
}


/**
 * @param array $selectedOptions
 * @param $output
 * @return string
 */
function fundraising_additional_info(array $selectedOptions)
{
    $output = '';
    if (!empty($selectedOptions[0]['max_per_investor'])
        || !empty($selectedOptions[0]['estimated_share_price'])
        || !empty($selectedOptions[0]['term'])) {
        // Additional Information
        $sectionTitle = 'Additional Information';
        $output .= '<div class="fieldset">';
        $output .= '<div class="section-title">' . $sectionTitle . '</div>';
        $output .= '</div>';

        // MAXIMUM per investor
        if (!empty($selectedOptions[0]['max_per_investor'])) {
            $str_maximum = $selectedOptions[0]['max_per_investor'] ? '$' . number_format($selectedOptions[0]['max_per_investor'], 0, '.', ',') : 'N/A';

            $output .= '<div class="fieldset">';
            $output .= '<div class="status-label">MAXIMUM <span class="light">per investor</span></div>';
            $output .= '<div class="status-label status-amount">' . $str_maximum . '</div>';
            $output .= '</div>';
        }

        // PRICE/SHARE
        if (!empty($selectedOptions[0]['estimated_share_price'])) {
            $str_price_per_share = $selectedOptions[0]['estimated_share_price'] ? '$' . number_format($selectedOptions[0]['estimated_share_price'], 2, '.', ',') : 'N/A';

            $output .= '<div class="fieldset">';
            $output .= '<div class="status-label">PRICE/SHARE</div>';
            $output .= '<div class="status-label status-amount">' . $str_price_per_share . '</div>';
            $output .= '</div>';
        }

        // TERM
        if (!empty($selectedOptions[0]['term'])) {
            $str_price_per_share = $selectedOptions[0]['term'] ? strtoupper($selectedOptions[0]['term']) : 'N/A';

            $output .= '<div class="fieldset">';
            $output .= '<div class="status-label">TERM</div>';
            $output .= '<div class="status-label status-amount">' . $str_price_per_share . '</div>';
            $output .= '</div>';
        }
    }

    $output .= '</div>';
    $output .= '</div>';
    return $output;
}

/**
 * creating audio item
 * @param $fileName
 * @param null $op
 * @return string
 */
function discussion_audio_item($fileName,$op = null, $data = null){

    $audioPreviewPath = '';
    $content = '';
    $fileLink = 'discussion/audio-previews/' . $fileName;
    if(is_readable(ROOT . 'sites/default/files/events_tmp/' . $fileLink)){
        $db = \Clewed\Db::get_instance();
        $hash = sha1('hash' . time() . $fileLink);
        $db->run('DELETE FROM `audio_files` WHERE DATE_ADD(`created`, INTERVAL 1 DAY) < NOW()');
        $db->run('
                        INSERT INTO `audio_files` (`hash`, `file`)
                        VALUES (:hash, :file)',
            array(
                ':hash' => $hash,
                ':file' => $fileLink
            )
        );

        $audioPreviewPath = '/' . $hash . '.mp3';
    }

    if($audioPreviewPath){
        $content .= '<div class="audio-preview">
                         <audio preload="auto" controls="controls" src="'.$audioPreviewPath.'">Sorry, your browser is too old and is not supported anymore. Please update it and try again.
                         </audio>';
             if(!empty($audioPreviewPath) && $data['access']){
                 $content.= '
                         <div data-m="'.md5($data['id'] . ':_token').'"
                              data-id="'. $data['id'] .'"
                              data-name="'. $fileName .'"
                              data-company-id="'. $data['company_id'] .'"
                              class="audio-preview-remove">
                                Remove
                         </div>';
             }
        $content .= '</div>' ;
    }
    return $content;
}



/* EOF */
