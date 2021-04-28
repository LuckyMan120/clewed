<?php include('dbcon.php');
error_reporting(0);

if ($_REQUEST['type'] == 'batchFollow') {

    if (md5($_REQUEST['uid'] . "kyarata75") != $_REQUEST['m'])
        die('Authentication problem');

    if ($_REQUEST['stype'] == 'follow_cmp_exp') {

        mysql_query("
            INSERT INTO maenna_followers
            (cid,
            uid,
            automatic_follow)

            (SELECT " . $_REQUEST['uid'] . ",pid,1 from maenna_people mp LEFT JOIN users u
            ON mp.pid = u.uid
            WHERE public = 1 AND u.status = 1
            AND (experties = '" . $_REQUEST['exp'] . "' OR experties2 = '" . $_REQUEST['exp'] . "' OR experties3 = '" . $_REQUEST['exp'] . "'))
            ON DUPLICATE KEY UPDATE automatic_follow = automatic_follow

        ") or die("Something went wrong");

        die("Succesfully followed " . mysql_affected_rows() . " professionals.");

    }

    if ($_REQUEST['stype'] == 'follow_pro_exp') {

        mysql_query("
            INSERT INTO maenna_followers
            (cid,
            uid,
            automatic_follow)

            (SELECT pid," . $_REQUEST['uid'] . ",1 from maenna_people mp LEFT JOIN users u
            ON mp.pid = u.uid
            WHERE public = 1 AND u.status = 1 AND pid <> " . $_REQUEST['uid'] . "
            AND (experties = '" . $_REQUEST['exp'] . "' OR experties2 = '" . $_REQUEST['exp'] . "' OR experties3 = '" . $_REQUEST['exp'] . "'))
            ON DUPLICATE KEY UPDATE automatic_follow = automatic_follow

        ") or die("Something went wrong");

        die("Succesfully added " . mysql_affected_rows() . " professionals to Followers.");

    }

    if ($_REQUEST['stype'] == 'follow_cmp_ind') {

        mysql_query("
            INSERT INTO maenna_followers
            (cid,
            uid,
            automatic_follow)

            (SELECT companyid," . $_REQUEST['uid'] . ",1 from maenna_company mc LEFT JOIN users u
            ON mc.companyid = u.uid
            WHERE public = 1 AND u.status = 1
            AND sector =  '" . $_REQUEST['exp'] . "')
            ON DUPLICATE KEY UPDATE automatic_follow = automatic_follow

        ") or die("Something went wrong");

        die("Succesfully added " . mysql_affected_rows() . " companies to Followers.");

    }
}

if ($_REQUEST['type'] == 'setPublic') {

    if ($_REQUEST['target_publicity'] == 'public') $status = 1; else $status = 0;
    mysql_query("UPDATE maenna_people SET public = " . $status . " WHERE pid = '" . ((int) $_REQUEST['uid']) . "' ") or die(mysql_error());


} elseif ($_REQUEST['type'] == 'unfollow') {

    mysql_query("DELETE FROM maenna_followers WHERE uid = '" . ((int) $_REQUEST['pid']) . "' AND  cid = '" . ((int) $_REQUEST['companyId']) . "' ") or die(mysql_error());

    echo "Unfollowing succeeded";

} elseif ($_REQUEST['type'] == 'follow') {

    mysql_query("
            INSERT INTO maenna_followers (cid,uid)
            VALUES ('" . ((int) $_REQUEST['companyId']) . "','" . ((int) $_REQUEST['pid']) . "')
        ") or die(mysql_error());

    $followingId = (int) $_REQUEST['pid'];
    $followerId = (int) $_REQUEST['companyId'];

    if (empty($followerId) || empty($followingId))
        die('Invalid follower/following ids');

    if ($followerId === $followingId)
        die("You are now following this user.");

    $following = mysql_fetch_object(mysql_query("
            SELECT u.*, mp.*
            FROM users u
            INNER JOIN maenna_people mp ON mp.pid = u.uid
            WHERE u.uid = '{$followingId}'
        "));

    $follower = mysql_fetch_object(mysql_query("
            SELECT
                u.*,
                mp.*,
                mc.*,
                mc.companyid as isCompany,
                mp.pid as isProfessional,
                CASE mc.company IS NOT NULL WHEN TRUE THEN mc.firstname ELSE mp.firstname END as firstname,
                CASE mc.company IS NOT NULL WHEN TRUE THEN mc.lastname ELSE mp.lastname END as lastname
            FROM users u
            LEFT JOIN maenna_people mp ON mp.pid = u.uid
            LEFT JOIN maenna_company mc ON mc.companyid = u.uid
            WHERE u.uid = '{$followerId}'
        "));

    if (empty($follower) || empty($following))
        die('Invalid follower/following data');


    if ($follower->isCompany) {
        $followerName = ucwords(strtolower($follower->projname));
        if (empty($followerName))
            $followerName = 'Project ' . $followerId;
    }
    else {
        $followerName = trim(
            ucfirst($follower->firstname) . ' ' .
            (
                1 == $follower->username_type ?
                ucfirst($follower->lastname[0]) :
                ucfirst($follower->lastname)
            )
        );
        if (empty($followerName))
            $followerName = 'Professional ' . $followerId;
    }

    $followingName = $following->firstname;

    $to = $following->mail;
    if (empty($to))
        die("You are now following this user.");

    $subject = $followerName . ' is now following you on Clewed!';

    $headers[] = "From: clewed@clewed.com\r\n";
    $headers[] = "MIME-Version: 1.0\r\n";
    $headers[] = "Content-Type: text/html\r\n";

    $content = <<< END
        Hi $followingName,<br><br>
        $followerName is now following you to get notified about Insights you create.<br>
        Please login to your account at
        <a href="https://www.clewed.com/account?tab=professionals&page=pro_detail&id=$followingId&ref=follow">www.clewed.com</a>
        to read about $followerName.<br><br>
        The Clewed team.
END;

    mail($to, $subject, $content, implode('', $headers));

    echo "You are now following this user.";
}