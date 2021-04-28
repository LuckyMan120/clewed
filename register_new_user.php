<?php
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
function makeCodeToRefer() {
    $characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $randomString = $characters[rand(0, strlen($characters) - 1)];
    $rand = '';
    $validChars = array('0', '1', '2', '3', '4', '5', '6', '7', '8', '9');
    $validCharsCount = count($validChars);
    $str = '';
    for ($i = 0; $i < 6; $i++) {
        $rand .= $validChars[rand(0, $validCharsCount - 1)];
    }
    $code = $randomString . $rand;
    return $code;
}

function makeInitialProjectName() {
    $sql = "SELECT projname FROM maenna_company mc LEFT JOIN users u on mc.companyid = u.uid where u.status = 1 and projname IS NOT NULL AND TRIM(projname) <> '' AND projname LIKE 'Project %'";
    $result = db_query($sql);
    while ($Row = db_fetch_object($result)) {
        $names[] = substr($Row->projname, 8);
    }
    $characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $randomString = $characters[rand(0, strlen($characters) - 1)];
    $rand = substr(uniqid('', true), -3);
    $name = $randomString . $rand;
    if (!in_array($name, $names)) {
        return "Project " . $name;
    } else return makeInitialProjectName();
}

function makePseudoName() {
    $max = 1;
    $names = array();
    $sql = "SELECT pseudo_name FROM maenna_company mc LEFT JOIN users u on mc.companyid = u.uid where u.status = 1 and pseudo_name IS NOT NULL
AND TRIM(pseudo_name) <> ''";
    $result = db_query($sql);
    while ($Row = db_fetch_object($result)) {
        $names[] = $Row->pseudo_name;
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

//die(print_r($_SESSION));
//die($_REQUEST['captcha']." = ".$_SESSION['my_captcha']);
//if ($_REQUEST['captcha'] != $_SESSION['my_captcha']) die('Security code is not correct!');
if (!user_load(array('mail' => $_REQUEST['email']))) {
    if ($_REQUEST['type'] == 'company') {

        $companyName = $_REQUEST['name'];
        $sql = "SELECT companyid AS id FROM maenna_company WHERE SOUNDEX(LOWER(company)) = SOUNDEX('%s')";
        $result = mysql_fetch_object(db_query($sql, strtolower($companyName)));
        if($result->id)
            die('It looks like your company name or email already exists. Please register as a professional and select to be connected to your company\'s team.');

        $Page_one = $_REQUEST;
        $last_name = $Page_one['lastname'];
        if (empty($last_name)) {
            echo 'Please fill in the last name';
            exit;
        }
        $New_user = array(
            'name'   => "Temp" . date('s'),
            'mail'   => $Page_one['email'],
            'init'   => $Page_one['email'],
            'roles'  => array(3 => 'Company'),
            'pass'   => $Page_one['password'],
            'status' => 1,
            'discussion_rupdated' => 1
        );
        $userObj = user_save(null, $New_user);
        $uid = $userObj->uid;
        if(empty($uid))
            die('There was an error saving your user profile. Please try again!');

        $SQL = 'update {users} set name = "%s" where uid = %d';
        db_query($SQL, "SID_" . $userObj->uid, $userObj->uid);
        $m = db_affected_rows();
        if ($m != 1) alert_admin("new company db query error");
        $first_name = $Page_one['firstname'];
        $company = $Page_one['name'];
        $this_year = date("Y");
        $last_year = (int) $this_year - 1;
        $industry = $Page_one['industry'];
        $revenue = $Page_one['revenue'];
        $projname = makeInitialProjectName();
        $pseudo_name = makePseudoName();
        $referral_code = $Page_one['cmp_referral'];
        $sql = "insert into maenna_company (companyid, company, email, sector, contact, membership,projname,pseudo_name, referral_code) values " .
            "(%d, '%s','%s','%s','%s','%s','%s','%s','%s')";
        $contact = $first_name . ' ' . $last_name;
        $Values = array($uid, $company, $Page_one['email'], $industry, $contact, 'level1', $projname, $pseudo_name, $referral_code);
        if (db_query($sql, $Values)) {
            //Insert default project logo based on configuration array
            /*$avatar = _IndustryDefaultLogo($industry);
            if ($avatar != '') {
                $sql = "INSERT INTO maenna_about (project_id, project) VALUES " .
                    "(%d, '%s')";
                $Values = array($uid, $avatar . ".jpg");
                db_query($sql, $Values);
            }*/
            $Param = array('to' => $Page_one['email'], 'from' => 'Clewed <clewed@clewed.com>', 'firstname' => $first_name, 'lastname' => $last_name);
            if ($_REQUEST['request'] == 'true') {
                notify_user('new_user_request', $Param);
                notify_user('new_company_account', $Param);
                echo("You have submitted successfully. Login and we'll schedule a brief demonstration shortly.");
            } else {
                notify_user('new_company_account', $Param);
                echo('You have registered successfully. Login to get started.');
            }
        } else {
            echo "There was an error. Please try again!";
        }
        $sql1 = "insert into maenna_company_data (companyid, access, data_type, data_attr, data_value) values (%d, '%s', '%s', '%s', '%s' )";
        $Values1 = array($uid, strval(time()), 'financial', strval(date("Y")), $revenue);
        if (db_query($sql1, $Values1)) {
        } else {
            echo "There was an error saving your revenue range. Please try again!";
        }
    } else {
        $Page_one = $_REQUEST;
        $last_name = $Page_one['lastname'];
        if (empty($last_name)) {
            echo 'Please fill in the last name';
            exit;
        }
        $referral_code = $Page_one['pro_referral'];
        $pro_cmp_email = '';
        $code_to_refer = makeCodeToRefer();
        $pro_type = $Page_one['pro_type'];
        if ($pro_type == "analyst") {
            $Roles = array(4 => "Analyst");
        } elseif ($pro_type == "executive") $Roles = array(5 => "Operating Executive");
        elseif ($pro_type == "other") $Roles = array(7 => "Talented Pro");
        elseif ($pro_type == "investor") $Roles = array(11 => "Investor");
        elseif ($pro_type == "client") $Roles = array(12 => "Company Member");
        elseif ($pro_type == "author") $Roles = array(13 => "Author");
        elseif ($pro_type == "consultant") $Roles = array(14 => "Consultant");
        if (isset($Page_one['pro_member']) && $Page_one['pro_member'] == 'true') {
//            $Roles = array(12 => 'Company Member');
            $pro_cmp_email = $Page_one['pro_cmp_email'];
            $pro_type = 'client';
        }
        $New_user = array(
            'name'   => "Temp" . date('s'),
            'mail'   => $Page_one['email'],
            'init'   => $Page_one['email'],
            'roles'  => $Roles,
            'pass'   => $Page_one['password'],
            'status' => 1,
            'discussion_rupdated' => 1,
            'company_update' => 1
        );
        $userObj = user_save(null, $New_user);
        $uid = $userObj->uid;
        if(empty($uid))
            die('There was an error saving your user profile. Please try again!');

        $SQL = 'update {users} set name = "%s" where uid = %d';
        db_query($SQL, "MU_" . $userObj->uid, $userObj->uid);
        $m = db_affected_rows();
        if ($m != 1) alert_admin("new user type $pro_type db query error");
        $first_name = $Page_one['firstname'];
        $password = $Page_one['password'];
        $email = sget($Page_one, 'email');
        $experties = sget($Page_one, 'experties');
        $DBValues = array(
            'pid'              => $uid,
            'firstname'        => $first_name,
            'lastname'         => "",
            'real_first_name'  => $first_name,
            'real_last_name'   => $last_name,
            'username_type'    => '1',
            'email'            => $email,
            'created'          => time(),
            'experties'        => $experties,
            'protype'          => $pro_type,
            'referral_code'    => $referral_code,
            'cmp_member_email' => $pro_cmp_email,
            'code_to_refer'    => $code_to_refer
        );
        $DBKeys = array_keys($DBValues);
        $SQLSTR = array();
        foreach ($DBValues as $key => $val) {
            $SQLSTR["$key"] = "'%s'";
        }
        $SQLSTR['pid'] = "%d";
        $SQL = "insert into maenna_people (" . implode(',', $DBKeys) . ") values (" . implode(',', $SQLSTR) . ")";
        if (!db_query($SQL, $DBValues)) {
            die('There was an error submiting your data. Please try again!');
        } else {
            $Param = array('to' => $email, 'from' => 'Clewed <clewed@clewed.com>', 'firstname' => $first_name, 'lastname' => $last_name, 'pass' => $password);
            if ($_REQUEST['request'] == 'true') {
                notify_user('new_user_request', $Param);
                notify_user('new_user_account', $Param);
                die("You have submitted successfully. Login and we'll schedule a brief demonstration shortly.");
            } else {
                notify_user('new_user_account', $Param);
                die('You have registered successfully. Login to get started.');
            }
        }
    }
} else {
    echo "The email already exists on our database. Please try logging in instead.";
}
