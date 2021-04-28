<?php

require_once __DIR__ . '/../../../lib/init.php';
use Clewed\Notifications\NotificationService;

    include 'dbcon.php';
    $db = \Clewed\Db::get_instance();
    error_reporting(0);
    if ($_REQUEST['type'] == 'clientRequest') {
        $q    = mysql_query("SELECT companyid FROM maenna_company WHERE projname = '" . mysql_real_escape_string($_POST['proj_name']) . "'");
        $row  = mysql_fetch_array($q);
        $time = time();
        if ($row['companyid']) {
            mysql_query(
                "INSERT INTO maenna_connections (conntype,target_uid,assignee_uid,access,status,editorid,edittime)
                        VALUES ('client'," . mysql_real_escape_string($row['companyid']) . "," . mysql_real_escape_string($_POST['uid']) . ",'" . $time . "','pending'," . mysql_real_escape_string($_POST['editor']) . ",'" . $time . "')"
            ) or die(mysql_error());
        } else die('fail');
        die('success');
    } elseif ($_REQUEST['type'] == 'setPublic') {
        if ($_REQUEST['target_publicity'] == 'public') $status = 1; else $status = 0;
        mysql_query("UPDATE maenna_company SET public = " . $status . " WHERE companyid = '" . mysql_real_escape_string($_REQUEST['cid']) . "' ") or die(mysql_error());

    } elseif ($_REQUEST['type'] == 'cnfColl') {

        function getDisplayName($id)
        {
            if (empty($id)) {
                return 'invalid id';
            }

            $sql     = "select users_roles.*, maenna_people.firstname from users_roles, maenna_people where users_roles.uid = '" . $id . "' and maenna_people.pid = '" . $id . "' limit 1";
            $result1 = mysql_query($sql);
            $Row     = mysql_fetch_assoc($result1);

            if (empty($Row)) {
                return "invalid user role setting - $id";
            }

            $rid       = $Row['rid'];
            $firstname = strtoupper($Row['firstname']);

            if (in_array($rid, array(6, 10))) {
                $output = "${firstname}MAEADM" . sprintf("%05s", $id + 100);
            } else {
                $output = "${firstname}MAE"; // . sprintf("%04s", $id +100);
            }

            $sql = mysql_query("SELECT name FROM role WHERE rid = $rid") or die(mysql_error());
            $Row = mysql_fetch_assoc($sql);
            $output .= ", " . strtoupper($Row['name']);

            return $output;
        }

        mysql_query("UPDATE maenna_connections SET status = '" . mysql_real_escape_string($_REQUEST['status']) . "' WHERE `status` = 'pending' AND assignee_uid = '" . mysql_real_escape_string($_REQUEST['pid']) . "' AND target_uid = '" . mysql_real_escape_string($_REQUEST['companyId']) . "' and conntype='collaborator' ") or die(mysql_error());

        $userId = (int) $_REQUEST['pid'];
        $targetId = (int) $_REQUEST['companyId'];
        $eventType = 'active' == $_REQUEST['status'] ? 'contribution_request_approved' : 'contribution_request_declined';

        $notificationService = new NotificationService();
        $notificationService->registerEvent($eventType, $targetId, $userId);

        $q1         = mysql_query("SELECT firstname, gender FROM maenna_people WHERE pid = $_REQUEST[pid]");
        $gender_tmp = mysql_fetch_array($q1);
        $comp       = mysql_query("SELECT first_name, mail FROM users LEFT JOIN users_extend USING(uid) WHERE uid = " . ((int)$_REQUEST['companyId']));
        $comp_data  = mysql_fetch_array($comp);
        $compname   = $comp_data['first_name'];
        $compmail   = $comp_data['mail'];
        $proname    = $gender_tmp['firstname'];
//        $content    = <<< END
//Hi $compname,
//
//$proname has joined your "contributors" team on Clewed to add analysis. Login at www.clewed.com to review.
//
//The clewed team!
//END;
//        $headers    = 'From: noreply@clewed.com' . "\r\n";
//
//        mail($compmail, 'New contributor on Clewed!', $content, $headers);

        //Get user gender
        $gender = $gender_tmp['gender'];

        //Check if user have a profile picture
        if (file_exists('sites/default/images/profiles/50x50/' . $value . '.jpg')) {
            $avatar = 'sites/default/images/profiles/50x50/' . $_REQUEST['pid'] . '.jpg';
        } else {
            if ($gender == 'm' || $gender == '') {
                $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
            } else {
                $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            }
        }

        $pro_maeid = getDisplayName($_REQUEST['pid']);

        $box_content = "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">";
        $box_content .= "<p style=\" font-size:11px; color:#666; font-family:Helvetica; text-transform:uppercase;margin-top:20px;\">$pro_maeid</p></div>";

        die($box_content);

    } elseif ($_REQUEST['type'] == 'cnfMgmnt') {
        function getDisplayName($id)
        {
            if (empty($id)) return 'invalid id';
            $sql     = "select users_roles.*, maenna_people.firstname from users_roles, maenna_people where users_roles.uid = '" . $id . "' and maenna_people.pid = '" . $id . "' limit 1";
            $result1 = mysql_query($sql);
            $Row     = mysql_fetch_assoc($result1);
            if (empty($Row)) return "invalid user role setting - $id";
            $rid       = $Row['rid'];
            $firstname = strtoupper($Row['firstname']);
            if (in_array($rid, array(6, 10))) {
                $output = "${firstname}MAEADM" . sprintf("%05s", $id + 100);
            } else {
                $output = "${firstname}MAE"; // . sprintf("%04s", $id +100);
            }
            $sql = mysql_query("SELECT name FROM role WHERE rid = $rid") or die(mysql_error());
            $Row = mysql_fetch_assoc($sql);
            $output .= ", " . strtoupper($Row['name']);
            return $output;
        }

        mysql_query("UPDATE maenna_connections SET status = '" . mysql_real_escape_string($_REQUEST['status']) . "' WHERE assignee_uid = '" . mysql_real_escape_string($_REQUEST[pid]) . "' AND target_uid = '" . mysql_real_escape_string($_REQUEST[companyId]) . "' and conntype='client' ") or die(mysql_error());
        //Get user gender
        $q1         = mysql_query("SELECT gender FROM maenna_people WHERE pid = " . mysql_real_escape_string($_REQUEST[pid]));
        $gender_tmp = mysql_fetch_array($q1);
        $gender     = $gender_tmp['gender'];
        //Check if user have a profile picture
        if (file_exists('sites/default/images/profiles/50x50/' . $value . '.jpg')) {
            $avatar = 'sites/default/images/profiles/50x50/' . $_REQUEST['pid'] . '.jpg';
        } else {
            if ($gender == 'm' || $gender == '') {
                $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
            } else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
        }
        $pro_maeid   = getDisplayName($_REQUEST['pid']);
        $box_content = "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:Helvetica; text-transform:uppercase;margin-top:20px;\">$pro_maeid</p></div>";
        die($box_content);
    } elseif ($_REQUEST['type'] == 'uncollaborate') {

        $success = $db->run("
            UPDATE maenna_connections
            SET status = 'deactivated'
            WHERE conntype='collaborator'
            AND assignee_uid = ?
            AND  target_uid = ?",
            array(
                (int) $_REQUEST['pid'],
                (int) $_REQUEST['companyId']
            ));

        echo $success ? "Disconnect succeeded" : 'Removal failed';

    } elseif ($_REQUEST['type'] == 'moveToAdvisors') {

        $success = $db->run("
            UPDATE maenna_connections
            SET conntype = 'advisor'
            WHERE conntype='collaborator'
            AND assignee_uid = ?
            AND  target_uid = ?",
            array(
                (int) $_REQUEST['pid'],
                (int) $_REQUEST['companyId']
            ));

        if($success) {
            $notificationService = new NotificationService();
            $notificationService->registerEvent(
                'company_advisor_added',
                (int) $_REQUEST['companyId'],
                0,
                array(
                    'advisorId' => (int) $_REQUEST['pid']
                )
            );
        }

        echo $success ? "success" : 'operation failed';

    } else {
        //#874 ticket, prevent duplicates on browse page (ON DUPLICATE KEY UPDATE not work here)
        $connection_params = array(":pid" => $_REQUEST['pid'], ":companyId" => $_REQUEST['companyId']);
        $some = $db->get_row("SELECT connid FROM `maenna_connections` WHERE `assignee_uid` = :pid AND `target_uid` = :companyId",$connection_params);
        if ($some){//just update connection status
        $query = "UPDATE `maenna_connections` SET `status` = 'pending',`edittime`=UNIX_TIMESTAMP(NOW()) WHERE `assignee_uid` = :pid AND `target_uid` = :companyId";
        $db->run($query, $connection_params);
        }
        else {//do old code
        $query = "INSERT INTO maenna_connections (`conntype`,`assignee_uid`,`target_uid`,`status`,`access`,`edittime`) VALUES
                  ('collaborator', :pid, :companyId, 'pending', UNIX_TIMESTAMP(NOW()), UNIX_TIMESTAMP(NOW()))
                  ON DUPLICATE KEY UPDATE `status` = 'pending',`edittime`=UNIX_TIMESTAMP(NOW()) ";
        $db->run($query, array(":pid" => $_REQUEST['pid'], ":companyId" => $_REQUEST['companyId']));
        }
		//send email to clewed and admin for that company
        $gender_tmp = $db->get_array("SELECT firstname, gender FROM maenna_people WHERE pid = :pid", array(":pid" => $_REQUEST[pid]));
        $admin_data = $db->get_array("select u.mail from maenna_connections mc 
									inner join users u on u.uid=mc.assignee_uid
									where mc.status='active' and mc.target_uid = :companyId and mc.conntype = 'admin' and u.status=1", array(":companyId" => $_REQUEST[companyId]));
        $compmail   = 'info@clewed.com';		
        $proname    = "Project " . sprintf("%03s", $_REQUEST[companyId] +100);
//        $content    = "
//		".$gender_tmp['firstname'].",
//
//		Professionals are requesting to share knowledge with $proname. Please review and approve.
//
//		The Clewed Team.";
//        $headers    = 'From: noreply@clewed.com' . "\r\n";
//		if(!empty($admin_data['mail']))
//		$headers	.= 'Cc: ' .$admin_data['mail']. "\r\n";
//
//        mail($compmail, 'New share knowledge request', $content, $headers);

        $notificationService = new NotificationService();
        $notificationService->registerEvent('contribution_request_sent', (int) $_REQUEST['companyId'], (int) $_REQUEST['pid']);

        echo 'Your request has been succesfully sent';
    }
