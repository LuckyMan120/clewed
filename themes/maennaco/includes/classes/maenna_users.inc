<?php
class Mae_users
{
    public static function all_admins()
    {
        $Output = array();
        $sql = "select  users_extend.*,
                        users_roles.rid
                from    users_roles,
                        users_extend,
                        users
                where   users_roles.uid = users_extend.uid and
                        (users_roles.rid = 6 or users_roles.rid = 10 ) and
                        users.uid = users_roles.uid and
                        users.status = 1
                order by users_extend.first_name";
        $result = db_query($sql);
        while($Row = db_fetch_array($result))array_push($Output, $Row);
        return $Output;

    }

    public static function update_notifications($uid, $notifications, $industries, $categories, $discussion_rupdated, $discussion_fupdated, $professional_discussion, $company_update, $company_comment) {
        if(!empty($uid)) {
            $sql = "UPDATE users SET notifications = %d, notif_industries = '%s', notif_categories = '%s', discussion_rupdated = %d, discussion_fupdated = %d, professional_discussion = %d, company_update = %d, company_comment = %d  WHERE uid = %d";
            $values = array($notifications, implode('|', $industries), implode('|', $categories), $discussion_rupdated, $discussion_fupdated, $professional_discussion, $company_update, $company_comment, $uid);
            return db_query($sql, $values);
        }

        return false;
    }

    public static function update_account($uid, $user_type, $Param)
    {
        if(empty($uid) || empty($user_type) || (! is_array($Param))  || count($Param) == 0)
        {
            return false;
        }
        if(empty($Param['password']) && $Param['email'])
        {
            $sql = "update users set mail = '%s' where uid = %d limit 1";
            $DBValues = array($Param['email'], $uid);
            if(! db_query($sql, $DBValues)) return false;
        }elseif($Param['password'] && $Param['email']){
            $sql = "update users set mail = '%s', pass = '%s' where uid = %d limit 1";
            $DBValues = array($Param['email'], md5($Param['password']), $uid);
            if(! db_query($sql, $DBValues)) return false;
        }
        //echo $sql; test_array($DBValues);


        $sql = '';
        switch($user_type)
        {
            case "super": case "admin":
                $sql = "update users_extend set first_name = '%s', last_name = '%s', phone = '%s' where uid = %d limit 1 ";
                $DBValues = array($Param['first_name'], $Param['last_name'], $Param['phone'], $uid);
                break;
            case "company":
                $sql = "update maenna_company set contact = '%s',email = '%s', phone = '%s' where companyid = %d limit 1 ";
                $DBValues = array($Param['first_name'] . ' ' . $Param['last_name'], $Param['email'], $Param['phone'], $uid);
                break;
            case "people":
                $sql = "update maenna_people set real_first_name = '%s', real_last_name = '%s', email='%s', phone = '%s', cmp_member_email = '%s' where pid = %d limit 1 ";
                $DBValues = array($Param['first_name'], $Param['last_name'], $Param['email'], $Param['phone'], $Param['cmp_member_email'], $uid);
                break;
            default:
                return false;
                break;
        }
        //echo $sql;
        //test_array($DBValues);
        if($sql) return db_query($sql, $DBValues);
        else return false;

    }

    public static function cu_group()
    {
        return array('cu_regular' =>    'regular users',
                     'cu_premium' =>    'premium users',
                     'cu_level1' =>    'level 1',
                     'cu_level2' =>    'level 2',
                     'cu_level3' =>    'level 3',
                     'cu_level4' =>    'level 4',
                     'cu_level5' =>    'level 5',
                     'cu_level6' =>    'level 6',
                     'cu_level7' =>    'level 7',
                     'cu_level8' =>    'level 8',
                     'cu_level9' =>    'level 9',
                     'cu_level10' =>    'level 10',
                     'cu_visible' =>    'visible company users');
    }
    public static function pu_group($ptab)
    {
	if (empty($ptab) || $ptab == 'team') {
        return array('pu_analyst' =>    'analysts',
                     'pu_executive'=>   'executives',
		    'pu_other' =>      'other',
			'pu_investor' =>    'investors',
			'pu_partner' =>    'introductions',
                     'pu_visible' =>    'connectors',
		    'pu_client'=>   'colleagues'); }
	elseif ($ptab == 'coll') {
	    return array('pu1_collaborators'=>   'interested',
			'pu1_followers' =>    'followers');
	}
    }
    public static function au_group()
    {
        return array('au_assign' =>     'assigned admin');
    }
    public static function com_group()
    {
        return array(//'cu_visible' => 'visible company users',
                     'pu_analyst' => 'analysts',
                     'pu_executive'=> 'executives',
                     'pu_other' => 'other advisors',
                     //'pu_visible' => 'visible prof.'
                     );
    }
    public static function is_premium_acct($companyid)
    {
        $Output = array();
        $sql = "select membership, memberexpire from maenna_company where companyid = %d limit 1";
        $result = db_query($sql, $companyid);
        $Row = db_fetch_object($result);
        if($Row)
        {
            if($Row->membership == 'regular')
            {
                $Output = array('status' => false, 'detail' => 'REGULAR');
            }	elseif ($Row->membership == 'level1')
            {
                $Output = array('status' => false, 'detail' => 'LEVEL 1');
            }	elseif ($Row->membership == 'level2')
            {
                $Output = array('status' => false, 'detail' => 'LEVEL 2');
            }	elseif ($Row->membership == 'level3')
            {
                $Output = array('status' => false, 'detail' => 'LEVEL 3');
            }	elseif ($Row->membership == 'level4')
            {
                $Output = array('status' => false, 'detail' => 'LEVEL 4');
            }	elseif ($Row->membership == 'level5')
            {
                $Output = array('status' => false, 'detail' => 'LEVEL 5');
            }	elseif ($Row->membership == 'level6')
            {
                $Output = array('status' => false, 'detail' => 'LEVEL 6');
            }	elseif ($Row->membership == 'level7')
            {
                $Output = array('status' => false, 'detail' => 'LEVEL 7');
            }	elseif ($Row->membership == 'level8')
            {
                $Output = array('status' => false, 'detail' => 'LEVEL 8');
            }	elseif ($Row->membership == 'level9')
            {
                $Output = array('status' => false, 'detail' => 'LEVEL 9');
            }	elseif ($Row->membership == 'level10')
            {
                $Output = array('status' => false, 'detail' => 'LEVEL 10');
            }	else
            {
                $date = date("m/d/Y", $Row->memberexpire);
                $time_now = time();

                if($Row->memberexpire < $time_now)
                {
                    $Output = array('status' => false, 'detail' => "REGULAR (exipred on $date)");
                }else
                {
                    $Output = array('status' => true, 'detail' => "PREMIUM (expires on $date)");
                }
            }
        }
        return $Output;
    }

    public static function set_premium($companyid, $status, $dt) {
        if (empty($companyid)) return false;
        if (empty($dt)) {
            $sql = "UPDATE maenna_company SET membership = '%s' WHERE companyid = %d LIMIT 1";
            return db_query($sql, array($status, $companyid));
        } else {
            $sql = "UPDATE maenna_company SET membership = '%s',memberexpire='%s' WHERE companyid = %d LIMIT 1";
            return db_query($sql, array($status, $dt, $companyid));
        }
    }

    public static function get_name($uid)
    {
        $sql = "select * from users_roles where uid = %d";
        $result = db_query($sql, array($uid));
        $Row = db_fetch_object($result);
        if($Row)
        {

            if($Row->rid ==3 ) // company
            {
                return getProjectName($uid);
            }elseif($Row->rid == 6 || $Row->rid == 10) // admin or super admin
            {
                $sql = "select concat_ws(' ', first_name, last_name) as name from users_extend where uid = %d";
                $result = db_query($sql, $uid);
                $Row = db_fetch_object($result);

                if($Row ) return $Row->name;
            }elseif(in_array($Row->rid, array(4,5,7))) // investor
            {
                $sql = "select ws_concat(' ', firstname, lastname) as name from maenna_people where pid = %d";
                $result = db_query($sql, $uid);
                $Row = db_fetch_object($result);
                if($Row ) return $Row->name;
            }
        }
        return false;
    }
}



/* EOF */
