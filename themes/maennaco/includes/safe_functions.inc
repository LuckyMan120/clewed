<?php
/**
 * Created by PhpStorm.
 * User: Vladimir Sergeev v.s.sergeev@gmail.com
 * Date: 25.07.15
 * Time: 19:28
 */

/**
 * Please use these functions and add others instead of duplicating the code
 * They are safe to include and don't require drupal libs
 */


if (!function_exists('getUserTypeById')) {
    function getUserTypeById($uid) {
        $q = mysql_query("SELECT rid FROM users_roles WHERE uid = '" . $uid . "'");
        $Row = mysql_fetch_object($q);
        $rid = $Row->rid;
        if (in_array($rid, array(4, 5, 7, 8, 11, 12, 13, 14))) {
            return 'people';
        } elseif ($rid == 6 || $rid == 15) return 'admin';
        elseif ($rid == 10) return 'super_admin';
        elseif ($rid == 3) return 'company';
        return "invalid uid";
    }
}

if (!function_exists('getUserType')) {
    function getUserType($uid) {
        $q = mysql_query("SELECT pid FROM maenna_people WHERE pid = '" . $uid . "' ");
        if (mysql_num_rows($q) > 0) {
            return 'people';
        } else {
            $q1 = mysql_query("SELECT companyid FROM maenna_company WHERE companyid = '" . $uid . "' ");
            if (mysql_num_rows($q1) > 0) {
                return 'company';
            } else return 'admin';
        }
    }
}
if (!function_exists('getUserById')) {
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
}

if (!function_exists('ifAdmin')) {
    //this function works for given uid or uname both
    function ifAdmin($user) {
        $result = mysql_query("SELECT * FROM users_roles WHERE uid = '" . (int)$user . "' AND rid IN (SELECT rid FROM role WHERE name = 'Super admin' OR name = 'Maennaco admin')");
        if (mysql_num_rows($result) > 0) {
            return true;
        }

        $result = mysql_query("SELECT *  FROM users u WHERE u.name = '" . mysql_real_escape_string($user) . "' AND EXISTS (SELECT * FROM users_roles WHERE uid = u.uid AND rid IN (SELECT rid FROM role WHERE name = 'Super admin' OR name = 'Maennaco admin'))");
        return mysql_num_rows($result) > 0;
    }
}

if (!function_exists('ifDiscussionModerator')) {
    function ifDiscussionModerator($discId, $uid) {
        $sql = "SELECT * FROM maenna_discussion_moderator WHERE discussion_id = '" . $discId . "' AND user_id = '" . $uid . "'";
        $result = mysql_query($sql);
        return mysql_num_rows($result) > 0;
    }
}

if (!function_exists('ifInsightOwner')) {
    function ifInsightOwner($discussionId, $uid)
    {
        $sql = "SELECT * FROM `maenna_professional` WHERE id = '" . $discussionId . "' AND postedby = '" . $uid . "'";
        $result = mysql_query($sql);
        return mysql_num_rows($result) > 0;
    }
}

if (!function_exists('getAvatarUrl')) {
    function getAvatarUrl($uid, $size = null) {
        if ($size == null) $size = "50x50"; else $size = "150x150";
        $usrType = getUserTypeById($uid);
        if ($usrType == 'people' || $usrType == 'admin' || $usrType == 'super_admin') {
            //Get user gender
            $query      = "SELECT gender FROM maenna_people WHERE pid = " . (int) $uid;
            $result     = mysql_query($query);
            $gender_tmp = mysql_fetch_object($result);
            $gender     = $gender_tmp->gender;
            if ($usrType == 'admin' || $usrType == 'super_admin') {return '/themes/maennaco/images/discussion_logo.png';}
            //Check if user have a profile picture
            if (file_exists(__DIR__ . '/../../../sites/default/images/profiles/' . $size . '/' . $uid . '.jpg')) {
                $avatar = '/sites/default/images/profiles/' . $size . '/' . $uid . '.jpg';
            } else {
                if ($gender == 'm' || $gender == '') {
                    $avatar = '/themes/maennaco/images/prof-avatar-male.png';
                } else {
                    $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                }
            }
        } elseif ($usrType = 'company') {
            //Get cmp_role
            $query        = "SELECT company_type FROM maenna_company WHERE companyid = " . (int) $uid;
            $result       = mysql_query($query);
            $cmp_role_tmp = mysql_fetch_object($result);
            $cmp_role     = $cmp_role_tmp->company_type;
            //Check if user have a profile picture
            if (file_exists(__DIR__ . '/../../../sites/default/images/company/' . $size . '/' . $uid . '.jpg')) {
                $avatar = '/sites/default/images/company/' . $size . '/' . $uid . '.jpg';
            } else if ($cmp_role == 'service') {
                $avatar = '/themes/maennaco/images/cmp-avatar-service.png';
            } else {
                $avatar = '/themes/maennaco/images/cmp-avatar-product.png';
            }
        } else {
            $avatar = '/themes/maennaco/images/prof-avatar-male.png';
        }
        return $avatar;
    }
}

if (!function_exists('checkValues')) {
    function checkValues($value) {
        $value = trim($value);
        if (get_magic_quotes_gpc()) {
            $value = stripslashes($value);
        }
        $value = strtr($value, array_flip(get_html_translation_table(HTML_ENTITIES)));
        $value = strip_tags($value);
        $value = mysql_real_escape_string($value);
        $value = htmlspecialchars($value);
        return $value;
    }
}

if (!function_exists('clickable_link')) {
    function clickable_link($text = '') {
        $text = preg_replace('#(script|about|applet|activex|chrome):#is', "\\1:", $text);
        $ret  = ' ' . $text;
        $ret  = preg_replace("#(^|[\n ])([\w]+?://[\w\#$%&~/.\-;:=,?@\[\]+]*)#is", "\\1<a href=\"\\2\" target=\"_blank\">\\2</a>", $ret);
        $ret  = preg_replace("#(^|[\n ])((www|ftp)\.[\w\#$%&~/.\-;:=,?@\[\]+]*)#is", "\\1<a href=\"http://\\2\" target=\"_blank\">\\2</a>", $ret);
        $ret  = preg_replace("#(^|[\n ])([a-z0-9&\-_.]+?)@([\w\-]+\.([\w\-\.]+\.)*[\w]+)#i", "\\1<a href=\"mailto:\\2@\\3\">\\2@\\3</a>", $ret);
        $ret  = substr($ret, 1);
        return $ret;
    }
}

if (!function_exists('ago')) {
    function ago($time) {
        $periods    = array("second", "minute", "hour", "day", "week", "month", "year", "decade");
        $lengths    = array("60", "60", "24", "7", "4.35", "12", "10");
        $now        = time();
        $difference = $now - $time;
        for ($j = 0; $difference >= $lengths[$j] && $j < count($lengths) - 1; $j++) {
            $difference /= $lengths[$j];
        }
        $difference = round($difference);
        if ($difference != 1) {
            $periods[$j] .= "s";
        }
        return "$difference $periods[$j] ago ";
    }
}

if (!function_exists('replace_email')) {
    function replace_email ($subject) {
        $pattern="/(?:[a-z0-9!#$%&'*+=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+=?^_`{|}~-]+)*|\"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*\")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\])/";
        return preg_replace($pattern,'<i>e-mail obscured</i>',$subject);
    }
}
if (!function_exists('nameToId')) {
    function nameToId($name) {
        $q = mysql_query("SELECT uid FROM users WHERE name = '" . $name . "' LIMIT 1") or die(mysql_error());
        $r = mysql_fetch_array($q);
        return $r['uid'];
    }
}

if (!function_exists('canRateUser')) {
    function canRateUser($target_uid, $editor_uid)
    {

        if ($target_uid == $editor_uid) return false;

        //Check if user is attending target`s insight
        $sql = "select * from maenna_professional_payments mpp
            left join maenna_professional mp on mpp.pro_id = mp.id
            where mp.postedby = " . (int)$target_uid . " and mpp.status = 1 and mpp.user_id = " . (int)$editor_uid;
        $result = mysql_query($sql);

        if (mysql_num_rows($result) > 0) return true;

        if (inProjectBox($target_uid, $editor_uid)) return true;

        return false;
    }
}

if (!function_exists('inProjectBox')) {
    function inProjectBox($pid, $companyid)
    {

        $return = array();

        $sql = "SELECT conntype FROM maenna_connections WHERE target_uid = " . (int)$companyid . " and assignee_uid = " . (int)$pid . " and status = 'active'
            and conntype in ('advisor','collaborator','visible','client')";

        $result = mysql_query($sql);

        if (mysql_num_rows($result) > 0) {

            while ($Row = mysql_fetch_object($result))

                $return[] = $Row->conntype;
        }

        $sql = "select id from maenna_followers where uid = " . (int)$pid . " and cid = " . (int)$companyid;
        $result = mysql_query($sql);

        if (mysql_num_rows($result) > 0) $return[] = 'followers';

        if (count($return) == 0) return false;
        else return $return;
    }
}