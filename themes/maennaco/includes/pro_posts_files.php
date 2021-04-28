<?php
include('dbcon.php');
include '../../../includes/bootstrap.inc';
include '../../../includes/file.inc';
session_start();
error_reporting (E_ALL ^ E_NOTICE);
if(isset($_POST))
{
    if($_SESSION['files'] != '')
    {
        unset($_SESSION['files']);
    }
    if($_SESSION['links'] != '')
    {
        unset($_SESSION['links']);
    }
    if($_SESSION['names'] != '')
    {
        unset($_SESSION['names']);
    }

    $_SESSION['files'] = $_POST['files'];
    $_SESSION['links'] = $_POST['links'];
    $_SESSION['names'] = $_POST['names'];
}

function nameToId($name) {

    $q = mysql_query("SELECT uid FROM users WHERE name = '".mysql_real_escape_string($name)."' LIMIT 1") or die(mysql_error());
    $r = mysql_fetch_array($q);
    return $r['uid'];

}

function getUserType($uid) {

    $q = mysql_query("SELECT pid FROM maenna_people WHERE pid = '".(int) mysql_real_escape_string($uid)."' ");

    if (mysql_num_rows($q) > 0 ) return 'people';

    else {
        $q1 = mysql_query("SELECT companyid FROM maenna_company WHERE companyid = '".(int) mysql_real_escape_string($uid)."' ");
        if (mysql_num_rows($q1) > 0 ) return 'company';
        else return 'admin';
    }
}

function ifAdmin ($uname) {

    $result = mysql_query("SELECT *  FROM users u WHERE u.name = '".mysql_real_escape_string($uname)."' AND EXISTS (SELECT * FROM users_roles WHERE uid = u.uid AND rid IN (SELECT rid FROM role WHERE name = 'Super admin' OR name = 'Maennaco admin'))");

    if (mysql_num_rows($result) > 0 ) {return true;} else return false;
}

function checkValues($value)
{
    $value = trim($value);

    if (get_magic_quotes_gpc()) {
        $value = stripslashes($value);
    }

    $value = strtr($value,array_flip(get_html_translation_table(HTML_ENTITIES)));

    $value = strip_tags($value);
    $value = mysql_real_escape_string($value);
    $value = htmlspecialchars ($value);
    return $value;

}


function clickable_link($text = '')
{
    $text = preg_replace('#(script|about|applet|activex|chrome):#is', "\\1:", $text);
    $ret = ' ' . $text;
    $ret = preg_replace("#(^|[\n ])([\w]+?://[\w\#$%&~/.\-;:=,?@\[\]+]*)#is", "\\1<a href=\"\\2\" target=\"_blank\">\\2</a>", $ret);

    $ret = preg_replace("#(^|[\n ])((www|ftp)\.[\w\#$%&~/.\-;:=,?@\[\]+]*)#is", "\\1<a href=\"http://\\2\" target=\"_blank\">\\2</a>", $ret);
    $ret = preg_replace("#(^|[\n ])([a-z0-9&\-_.]+?)@([\w\-]+\.([\w\-\.]+\.)*[\w]+)#i", "\\1<a href=\"mailto:\\2@\\3\">\\2@\\3</a>", $ret);
    $ret = substr($ret, 1);
    return $ret;
}
function ago($time)
{
    $periods = array("second", "minute", "hour", "day", "week", "month", "year", "decade");
    $lengths = array("60","60","24","7","4.35","12","10");

    $now = time();

    $difference     = $now - $time;
    $tense         = "ago";

    for($j = 0; $difference >= $lengths[$j] && $j < count($lengths)-1; $j++) {
        $difference /= $lengths[$j];
    }

    $difference = round($difference);

    if($difference != 1) {
        $periods[$j].= "s";
    }

    return "$difference $periods[$j] ago ";
}



$next_records = 10;
$show_more_button = 0;

$res = mysql_query("SELECT d_id FROM wall_documents where document_name = '".  mysql_real_escape_string(basename($url)) ."'");
$red = mysql_fetch_array($res);
$d_id =		$red['d_id'];

if ($_REQUEST['type'] == 'moreEventsCalendar') {

    $eventRes = mysql_query("SELECT * FROM maenna_company_events_inv inv JOIN maenna_company_events ev ON ev.eventid = inv.eventid WHERE uid = '".(int) mysql_real_escape_string($_REQUEST['u'])."' AND inv.status = 'confirmed' ORDER BY datetime DESC LIMIT 5,120000000000");


    while ($events = mysql_fetch_array($eventRes)) {

        $content .= date("m/d/Y",$events['datetime'])." - <a target = \"_blank\" href=\"http://maennaco.cp-dev.com/account?tab=companies&page=company_detail&id=".$events['companyid']."&mtab=advice\" >".$events['title']."</a><br>";

    }
    die($content);
}

if ($_REQUEST['type']=='removeFile') {

    mysql_query("UPDATE maenna_company_data SET deleted = 1 WHERE dataid = '".$_REQUEST['fileid']."'");
    die();

}

if ($_REQUEST['type'] == 'editInsight') {

    if ($_FILES['file']['tmp_name']) {
        if ($_POST['file_name']) {
            $file_name = $_FILES['file']['name'];
            $file_ext = substr($file_name, strripos($file_name, '.'));
            $new_file_name = time() . '_' . $_POST['file_name'] . $file_ext;
            $path = './' . file_directory_path() . '/' . $new_file_name;
            move_uploaded_file($_FILES['service_file']['tmp_name'], $path);
            $db = Clewed\Db::get_instance();
            $db->run("INSERT INTO wall_documents (ref_id,document_name) VALUES ({$_POST['eventid']}, '{$new_file_name}')");
            if ($_POST['user_id']) {
                $document_id = $db->lastInsertId();
                $db->run("INSERT INTO wall_posts (f_name, post, f_image, date_created, user, document_id)
                    VALUES (:user_fname, '', '', :date_created, :user_name, :d_id)",
                    array(
                        ':user_fname' => $_POST['user_fname'],
                        ':date_created' => strtotime(date('Y-m-d H:i:s')),
                        ':user_name' => $_POST['username'],
                        ':d_id' => $document_id
                    )
                );
            }
        }
    }

    $posts = explode("&",$_POST['str']);

    $files = $links = '';

    foreach ($posts as $value) {

        $var = explode("=",$value);
        $var[1] = str_replace("+"," ",$var[1]);
        if ($var[0] == 'name' && $var[1] != '') $links.= "(".$_POST['eventid'].",'".$var[1]."'";
        elseif ($var[0] == 'link' && $var[1] != '') $links.= ",'".$var[1]."'),";
        elseif ($var[0] == 'fileupl' && $var[1] != '') {
            $files.= "(".$_POST['eventid'].",'".$var[1]."'),";
        }




    }
    if ($links != '') mysql_query("INSERT INTO maenna_professional_links (professional_id,name,links) VALUES ".rtrim($links,',')) or die("There was a problem.Please try again.");

    if ($files != '') {
        $db = Clewed\Db::get_instance();
        $db->run("INSERT INTO wall_documents (ref_id,document_name) VALUES " . rtrim($files,','));
        if ($_POST['user_id']) {
            $document_id = $db->lastInsertId();
            $db->run("INSERT INTO wall_posts (f_name, post, f_image, date_created, user, document_id) 
            VALUES (:user_fname, '', '', :date_created, :user_name, :d_id)", array(
                ':user_fname' => $_POST['user_fname'],
                ':date_created' => strtotime(date('Y-m-d H:i:s')),
                ':user_name' => $_POST['username'],
                ':d_id' => $document_id
            ));
        }
    }

    die('Your changes were saved successfully');
}
if ($_REQUEST['type']=='commInv') {



    $invitees = explode(",",$_REQUEST['invitees']);


    $cmpname = mysql_query("SELECT projname FROM maenna_company WHERE companyid = '".(int) mysql_real_escape_string($_REQUEST['cid'])."'");
    $cmpname = mysql_fetch_array($cmpname);

    foreach ($invitees as $value) {

        $sqlmail = mysql_query("SELECT mail FROM users WHERE uid = '".$value."'");
        $invmail = mysql_fetch_array($sqlmail);

        $to      =  $invmail['mail'];
        $subject = 'Give advice on MAENNA for '.ucwords($cmpname['projname']);
        $message.=  ucwords($cmpname['projname']).' requests your advice / comments on '.$_REQUEST['name'];
        $message.= '<br><br>';
        $message.= 'Follow this link to see and comment on this file. <br>';
        $message.= 'http://maennaco.com/account?tab=companies&page=company_detail&id='.$_REQUEST['cid'].'&mtab=file&file='.urlencode($_REQUEST['filename']).'&name='.urlencode($_REQUEST['name']).' <br>';
        $headers = "From:maenna@maennaco.com \r\n";
        $headers .= "Content-type: text/html; charset=iso-8859-1 \r\n";
        $headers .= "Bcc: Maenna@maennaco.com \n";


        mail($to, $subject, $message, $headers) or die("Message couldn`t be send. Please try again!");

        $to = '';
        $message = '';
        $headers = '';

    }

    die('Your invitations were sent succesfully!');

}

if ($_REQUEST['type']=='attachFile') {

    //if ($_REQUEST['display'] == 'true') {

    $sql = mysql_query ("SELECT * FROM maenna_company_events WHERE eventid = '".(int) mysql_real_escape_string($_REQUEST['eventid'])."'");

    $sql = mysql_fetch_array($sql);

    $sql1 = mysql_query ("SELECT * FROM maenna_company_events_inv WHERE eventid = '".(int) mysql_real_escape_string($_REQUEST['eventid'])."'");

    $sql2 = mysql_query("SELECT * FROM maenna_company_data WHERE data_type = 'events' AND data_value6 = '".(int) mysql_real_escape_string($_REQUEST['eventid'])."'");

    $sqlfile = mysql_query("SELECT * FROM wall_documents WHERE  ref_id = '".(int) mysql_real_escape_string($_REQUEST['professional_id'])."'");
    //echo "$sqlfile";
    // die;



    ?>
<style>
    textarea{
        border: 2px solid #DCE6F5;
        margin-top: 10px;
        min-height: 26px;
        padding: 6px;
        width: 285px !important;
    }
    .qq-upload-button {
        color:#00A3BF;
        margin-top:9px !important;
        font-size:13px;
        width:91px;
        height:21px;
        background: #e3eef2;
        font-family:LatoRegular;
        text-transform: lowercase;
        margin-left:27px !important;}

    .qq-upload-button:first-letter {text-transform: uppercase;}

    .qq-upload-success
    {
        padding:0 0 0.2em 1.5em !important;
    }
    .input {
        border: 2px solid #DCE6F5;
        margin-top: 10px;
        min-height: 26px;
        padding: 6px;
        width: 250px !important;
    }
</style>
<script language="javascript">

    fields = 0;

    function addInput() {

        fields = 2;
        if (fields != 10) {

            $('<table><tr><td>Name: <td><td><input class="input" id="name'+fields+'" name="name" style="margin-left:10px;border:1px solid #D6D6D8";></td></tr><tr><td>Link: <td><td><input class="input" id="link'+fields+'" name="link" style="margin-left:10px;border:1px solid #D6D6D8";></td></tr></table>').appendTo('#text');
            /*document.getElementById('text').innerHTML += '<textarea class="input" id="eventType" name="eventType[]" style="height:30px; font:15px/160% Verdana, sans-serif" cols="30"></textarea><br />';*/

            fields += 1;

        } else {

            document.getElementById('text').innerHTML += "<br />Only 10 upload fields allowed.";

            document.form.add.disabled=true;

        }

    }

</script>

</head>

<body>

	 <div style="clear:both;"></div>

     <?php if ($_REQUEST['reason'] == 'edit') $act = '/themes/maennaco/insludes/pro_posts_files.php?type=editInsight'; else $act = '';?>
<form action ="<?=$act;?>" enctype="multipart/form-data" method="post" name="editEvent" id="editEventForm">

    <div id='evFileEdit'  style='margin-top:-22px;text-align:left;'>
        <span style="margin-top:7px;float:left;">File:</span>
        <input style="display:none !important;background:none;float:left;padding:3px;margin-left:25px !important;width:258px !important;margin-left: 10px;" type="text" name="fileTitleEdit" id="fileTitleEdit" />
        <?php
        if ($_REQUEST['reason'] == 'edit') $upl = 'file-uploader2'; else $upl = 'file-uploader1';
        ?>
        <div id="<?=$upl;?>">
            <noscript>
                <p>Please enable JavaScript to use file uploader.</p>
                <!-- or put a simple form for upload here -->
            </noscript>
        </div>
    </div>

    <div id="text" style="float:left;">
        <table>
            <tr>
                <td>Name:</td>
                <td><input class="input" id="name" name="name" placeholder="Add link name here" style="margin-left:10px;border:1px solid #D6D6D8" ></td>
            </tr>
            <tr>
                <td>Link:</td>
                <td><input class="input" placeholder="Add url of link here" style="margin-left:10px;padding:6px !important;" id="link" name="link"></td>
            </tr>
        </table>
    </div>
    <!--<div id="event">
				<textarea class="input" id="eventType" name="eventType" style="height:30px; width:285px !important; font:15px/160% Verdana, sans-serif" cols="30"><?=$sql['title']?></textarea><br>
				</div>
			<div id="text">
			</div>-->
    <div style="clear:both;"></div>
			<span style="float:left;margin-top:-19px;margin-left:54px;margin-bottom:10px;">
			<a style="font-family:LatoRegular;color:#00A3BF;"href="javascript:void(0);" onClick="addInput()">Add more links</a>
			</span>
    <!--<input type='text' id="eventDate" name="eventDate" class='datepicker' style="15px/160% Verdana, sans-serif;" value="<?=date("Y-m-d g:i",$sql['datetime']-60*60)?>" /><br>
			
			<textarea class="input" id="eventLoc" name="eventLoc" style="height:30px; font:15px/160% Verdana, sans-serif" cols="60"><?=$sql['location']?></textarea><br>
			
			<textarea class="input" id="eventDesc" name="eventDesc" style="font:15px/160% Verdana, sans-serif" cols="60"><?=$nl = preg_replace('#<br\s*/?>#i', "\n", $sql['description']);?></textarea><br>-->
    <div style="clear:both"></div>
    <!--<a style="pointer:cursor; float:left; padding:0;" class='hidebox button' boxid=evFile>CLOSE</a>-->
    <!-- <div style="clear:both"></div>
     -->

    <!-- <input type="checkbox" id="chkNot" value="chkNot"> Send notification about changes to attendees</br>-->
</form>
     <style>

         .ui-widget-header{color:#929497 !important;}

     </style>

    <?php



    die ();
} else {


    mysql_query ("UPDATE maenna_company_events SET title = '".mysql_real_escape_string($_REQUEST['title'])."',description = '".mysql_real_escape_string($_REQUEST['agenda'])."',location = '".mysql_real_escape_string($_REQUEST['loc'])."',datetime = '".(strtotime($_REQUEST['datetime'])+60*60)."' WHERE eventid = '".(int) mysql_real_escape_string($_REQUEST['eventid'])."'") or die(mysql_error());

    $eventId = mysql_insert_id();
    $invitees = explode(",",$_REQUEST['invitees']);

    $umail = mysql_query("SELECT mail FROM users WHERE uid = '".(int) mysql_real_escape_string($_REQUEST['uid'])."'");
    $umail = mysql_fetch_array($umail);

    foreach ($_REQUEST['files'] as $value) {

        $fileName = $value['timestamp']."_".$value['path'];

        if ($value['title'] == '') $value['title'] = substr($value['path'], 0, strrpos($value['path'], '.'));

        rename ('../../../sites/default/files/events_tmp/'.$fileName , '../../../sites/default/files/'.$fileName) or die('problem');

        $sql = "insert into maenna_company_data (companyid, access, data_type,data_value, data_value2,data_value6, editorid )
						values('".$_REQUEST['cid']."','".time()."','events','".$value['title']."','".$fileName."','".$_REQUEST['eventid']."',".$_REQUEST['uid'].")";
        mysql_query($sql) or die(mysql_error());

    }

    $cmpname = mysql_query("SELECT projname FROM maenna_company WHERE companyid = '".(int) mysql_real_escape_string($_REQUEST['cid'])."'");
    $cmpname = mysql_fetch_array($cmpname);

    if ($_REQUEST['notif'] == 'true') {

        $q = mysql_query("SELECT mail FROM users u  JOIN maenna_company_events_inv mce ON u.uid = mce.uid WHERE eventid = '".(int)mysql_real_escape_string($_REQUEST['eventid'])."' AND mce.status = 'confirmed' ");
        while ($inv = mysql_fetch_array($q)) {

            $to	     = $inv['mail'];
            $subject = ucwords($cmpname['projname'])." Discussion on ".ucfirst(date("M j ",strtotime($_REQUEST['datetime'])));
            $message.= ucwords($cmpname['projname']).' Discussion on '.ucfirst(date("M j ",strtotime($_REQUEST['datetime']))).'  is updated for additional files, topic or participants. Please go to http://www.maennaco.com/account?tab=companies&page=company_detail&id='.$_REQUEST['cid'].'&mtab=advice  to review <br>';
            $headers = "From:".$umail['mail']."\r\n";
            $headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";


            mail($to, $subject, $message, $headers);



        }
    }

    foreach ($invitees as $value) {

        //Checking if user is already invited so we don`t have duplicates

        $testSql = mysql_query("SELECT COUNT(*) cnt FROM maenna_company_events_inv WHERE eventid = '".(int) mysql_real_escape_string($_REQUEST['eventid'])."' AND uid = '".$value."'") or die(mysql_error());
        $invCount = mysql_fetch_array($testSql);

        if ($invCount['cnt'] == 0) {

            mysql_query ("INSERT INTO maenna_company_events_inv (eventid,uid,status) VALUES ('".$_REQUEST['eventid']."',".$value.",'sent')");
            $sqlmail = mysql_query("SELECT mail FROM users WHERE uid = '".$value."'");
            $invmail = mysql_fetch_array($sqlmail);

            $to      =$invmail['mail'];
            $subject = ucwords($cmpname['projname'])." Discussion on ".ucfirst(date("M j ",strtotime($_REQUEST['datetime'])));
            $message.= 'MAENNA has invited you to a discussion for '.ucwords($cmpname['projname']);
            $message.= '<br><br>';
            $message.= 'Topic: '.strtoupper($_REQUEST['title']).'<br><br>';
            $message.= date("l, M j, Y g:i A T ",strtotime($_REQUEST['datetime'])).'<br><br>';
            $message.= $_REQUEST['loc'].'<br><br>';
            $message.= 'Agenda: <br>'.nl2br($_REQUEST['agenda']).'<br><br>';
            $message.= 'Follow this link to confirm your attendance. <br>';
            $message.= 'http://www.maennaco.com/account?tab=companies&page=company_detail&id='.$_REQUEST['cid'].'&mtab=advice <br>';
            $headers = "From:".$umail['mail']."\r\n";
            $headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";


            mail($to, $subject, $message, $headers);

            $message = '';
            $headers = '';

        }

    }

    //}

}

function replace_email ($subject) {

    $pattern="/(?:[a-z0-9!#$%&'*+=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+=?^_`{|}~-]+)*|\"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*\")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\])/";

    return preg_replace($pattern,'<i>e-mail obscured</i>',$subject);
}
function getProId($id)
{
    if(empty($id)) return 'invalid id';
    $sql = "select users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname from users_roles, maenna_people where users_roles.uid = '".(int) mysql_real_escape_string($id)."' and maenna_people.pid = '".(int) mysql_real_escape_string($id)."' limit 1";
    $result1 = mysql_query($sql);
    $Row = mysql_fetch_assoc($result1);
    if(empty($Row)) return "invalid user role setting - $id";
    $rid = $Row['rid'];
    $firstname = strtoupper($Row['firstname']);
    if(in_array($rid, array(6, 10))){
        $output = "${firstname}MAEADM" . sprintf("%05s", $id +100);
    }else{
        $output = "${firstname}MAE";// . sprintf("%04s", $id +100);
    }

    return $output;
}

if ($_REQUEST['type']=='addEvent') {




    $val = explode(" ",$_REQUEST['date']);
    $date = explode("-",$val[0]);
    $time = explode(":",$val[1]);
    $time[0] = 0;
    date_default_timezone_set('EST');


    //echo date("M j Y \a\t g:i A T",strtotime($_REQUEST['date']));
    //die();
    mysql_query("INSERT INTO maenna_professional (username,title,description,location,datetime,whyattend,cost,capacity,tags,created,postedby) VALUES ('".$_REQUEST['u']."','".$_REQUEST['title']."','".nl2br($_REQUEST['desc'])."','".$_REQUEST['loc']."','".strtotime($_REQUEST['date'])."','".$_REQUEST['whyattend']."','".$_REQUEST['cost']."','".$_REQUEST['capacity']."','".$_REQUEST['tags']."','".time()."','".$_REQUEST['uid']."')");

    $eventId = mysql_insert_id();


    $umail = mysql_query("SELECT mail FROM users WHERE uid = '".(int) mysql_real_escape_string($_REQUEST['uid'])."'");
    $umail = mysql_fetch_array($umail);




    ?>

<div class="event" id="event<?=$eventId;?>">

    <div id="clear" style="clear:both"></div>
    <div style="float:left;" class="calendar">
        <span class="day"><?=date("d",strtotime($_REQUEST['date']));?></span>
        <span class="month"><?=strtoupper(date("M",strtotime($_REQUEST['date'])));?></span>

    </div><!-- calendar -->

    <div class="event-info">
        <div style="margin-left:10px; float:left; width: 420px;">
            <span class="eventTitle" style="float:left; cursor:pointer; font-size:15px;"><strong><a style="cursor:pointer;" onClick="showprodetails('<?php echo $eventId; ?>');"><?=replace_email(strtoupper($_REQUEST['title']));?> (<?=$_REQUEST['tags']?>)</a></strong></span><div id="clear" style="clear:both"></div>
            <span class="prodatetime"><?=date("l, M j, Y g:i A T ",strtotime($_REQUEST['date']));?></span>
            <div id="clear" style="clear:both"></div>
            <span style="float:left; font-size: 12px;"><strong><?=$_REQUEST['u']?>,</strong> <?=$_REQUEST['tags']?></span>
            <div id="clear" style="clear:both"></div>
            <span style="float:left;"><?=$_REQUEST['desc'];?></span><br>
            <span style="float:left; display:none;"><?=nl2br($_REQUEST['desc'])?></span>
        </div>

				    <span class="attFiles" style="float:left;font-size:10px;">
				    
			   
			   </span>
    </div>
</div>

    <?php
    if($row1['postedby'] == $user->uid || $AccessObj->Com_sections['advice']['sections']['maenna_events']['access'] == 'write'){?>
    <a style="float:right;margin-left:10px;margin-right:8px;" href="#" id="remove_id<?php  echo $eventId?>" alt="<?php  echo md5($eventId.$user->name."kyarata75") ?>" name="<?=$user->name;?>" delType = 'event' class="tool evdelete"> REMOVE</a>

    <a style="float:right; margin-left:10px;" href="#" id="edit_id<?php  echo $row1['id']?>" alt="<?php  echo md5($row1['id'].$user->name."kyarata75") ?>" name="<?=$user->name;?>" delType = 'event' class="tool evedit"> EDIT</a>
        <?php } ?>


<div id="clear" style="clear:both"></div> <br />

<br />
</div>

    <?php


}

if ($_REQUEST['type'] == 'confirmAtt') {

    if ($_REQUEST['status'] == 'confirmed') {

        $dbl = mysql_query("SELECT *
				    FROM maenna_company_events ev1
				    WHERE eventid ='".(int) mysql_real_escape_string($_REQUEST['eventid'])."'
				    AND EXISTS (
				    
				    SELECT * 
				    FROM maenna_company_events ev2
				    JOIN maenna_company_events_inv inv1 ON ev2.eventid = inv1.eventid
				    WHERE inv1.uid ='".(int) mysql_real_escape_string($_REQUEST['uid'])."'
				    AND inv1.status =  'confirmed'
				    AND DATE( FROM_UNIXTIME( ev1.datetime ) ) = DATE( FROM_UNIXTIME( ev2.datetime ) )
                                    AND ABS(time_to_sec(timediff(FROM_UNIXTIME(ev1.datetime),FROM_UNIXTIME(ev2.datetime))) / 3600) < 2
				    )");

        if (mysql_num_rows($dbl) > 0) die("overlap");

    }



    mysql_query("UPDATE maenna_company_events_inv SET status = '".mysql_real_escape_string($_REQUEST['status'])."' WHERE eventid = '".(int) mysql_real_escape_string($_REQUEST['eventid'])."' AND uid = '".(int) mysql_real_escape_string($_REQUEST['uid'])."' ");



    if ($_REQUEST['status'] == 'confirmed') {

        $sql = mysql_query ("SELECT * FROM maenna_company_events mce JOIN users u ON mce.companyid = u.uid WHERE mce.eventid = '".(int) mysql_real_escape_string($_REQUEST['eventid'])."'");
        $sql1 = mysql_query ("SELECT mail FROM users WHERE uid = '".(int) mysql_real_escape_string($_REQUEST['uid'])."'");
        $sql2 = mysql_query ("SELECT * FROM maenna_company_events_inv mcei JOIN users u ON mcei.uid = u.uid WHERE eventid = '".(int) mysql_real_escape_string($_REQUEST['eventid'])."'");

        $sqlmail = mysql_query("SELECT mail FROM users WHERE uid = '".(int) mysql_real_escape_string($_REQUEST['cid'])."'");
        $cmpmail = mysql_fetch_array($sqlmail);


        $event = mysql_fetch_array($sql);
        $umail = mysql_fetch_array($sql1);


        $to      = $cmpmail['mail'];
        $subject = '[Attendance confirmed] '.strtoupper($event['title']).' !!!';
        $message.= 'User '.$umail['mail'].' has confirmed acceptance!!!';
        $message.= '<br><br>';
        $message.= 'Event: '.strtoupper($event['title']).'<br><br>';
        $message.= date("l, M j, Y g:i A T ",$event['datetime']).'<br><br>';
        $message.= $event['location'].'<br><br>';
        $message.= 'Agenda: <br>'.$event['description'].'<br><br>';
        $message.= 'Attendees/Invitees <br>';

        while ($allinv = mysql_fetch_array($sql2)) {

            $message.= $allinv['mail'].'&nbsp;&nbsp; - &nbsp;&nbsp;'.$allinv[3].'<br>';

        }
        $headers = "From:".$umail['mail']."\r\n";
        $headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";


        mail($to, $subject, $message, $headers);

    }

    switch ($_REQUEST['status']) {

        case "confirmed" : die("YOUR STATUS: <a class = \"rsvp\" name=\"confirmatt\" eventid = \"".$_REQUEST['eventid']."\" style=\"color:#6792D0 !important; cursor:pointer;\" >CONFIRMED</a>");break;
        case "maybe" : die("YOUR STATUS: <a class = \"rsvp \" name=\"confirmatt\" eventid = \"".$_REQUEST['eventid']."\" style=\color:#6792D0 !important; cursor:pointer; \" >TENTATIVE</a>");break;
        case "declined" : die("YOUR STATUS: <a class = \"rsvp \" name=\"confirmatt\" eventid = \"".$_REQUEST['eventid']."\" style=\"color:#6792D0 !important; cursor:pointer;\" >DECLINED</a>");break;

    }

}

if ($_REQUEST['type']=='teamStats') {

    function getProId($id)
    {
        if(empty($id)) return 'invalid id';
        $sql = "select users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname from users_roles, maenna_people where users_roles.uid = '".(int) mysql_real_escape_string($id)."' and maenna_people.pid = '".(int) mysql_real_escape_string($id)."' limit 1";
        $result = mysql_query($sql);
        $Row = mysql_fetch_assoc($result);
        if(empty($Row)) return "invalid user role setting - $id";
        $rid = $Row['rid'];
        $firstname = strtoupper($Row['firstname']);
        if(in_array($rid, array(6, 10))){
            $output = "${firstname}MAEADM" . sprintf("%05s", $id +100);
        }else{
            $output = "${firstname}MAE";// . sprintf("%04s", $id +100);
        }

        return $output;
    }
    function userRoleId($uid = null){
        if(empty($uid)) return '';
        $sql = "select rid from users_roles where uid = '".(int) mysql_real_escape_string($uid)."' ";
        $result = mysql_query($sql);
        while($Row = mysql_fetch_array($result))
        {
            return $Row['rid'];
        }
        return '';
    }
    function getRole($id) {
        $result = mysql_query("SELECT r.name as name FROM users_roles ur JOIN role r on ur.rid = r.rid WHERE ur.uid = '".(int) mysql_real_escape_string($id)."'");
        while($Row = mysql_fetch_array($result))
        {
            return $Row['name'];
        }
        return '';
    }
    function Com_conns($id)
    {
        $Conns = array('Admin' => array(),
            'Follower' => array(),
            'Follow' => array(),
            'Advisor' => array(),
            'Watchlist' => array(),
            'Inwatchlist' => array(),
            'Visible' => array(), //
            'Propose' => array());
        if(empty($id)) return $Conns;
        $sql = "select  *
                from    maenna_connections
                where   status = 'active' and
                        ((assignee_uid = '".$id."') or (target_uid = '".$id."'))
                order by conntype
                ";
        $result = mysql_query($sql);
        while($Row = mysql_fetch_assoc($result))
        {

            $conntype = ucwords($Row['conntype']);
            if($Row['conntype'] == 'follow')
            {
                if($Row['target_uid'] == $id) $Conns['Follower'][] = $Row;
                else $Conns['Follow'][] = $Row;
            }elseif($Row['conntype'] == 'watchlist')
            {
                if($Row['target_uid'] == $id) $Conns['Watchlist'][] = $Row; // target com is the one being watched.
                else $Conns['Inwatchlist'] = $Row;
            }elseif($Row['conntype'] == 'visible')
            {
                if($Row['target_uid'] == $id) $Conns['Visible'][] = $Row; // target com is the one being watched.
            }
            else
            {
                $Conns["$conntype"][] = $Row;
            }
        }
        return $Conns;
    }


    $Conns = Com_conns($_REQUEST['companyId']);


    $box1_cnt = count($Conns['Advisor']);
    $box2_cnt = count($Conns['Visible']);
    $box3_cnt = count($Conns['Follow']);


    $advisor_active = $following_active = $conn_active = '';
    $ctype = $_REQUEST['ctype'];
    $LIST = '';
    $box_content = '';
    if(empty($ctype) || $ctype == 'advisor')
    {
        $advisor_active = 'active';
        $box_title = "ADVISORS";
        foreach($Conns['Advisor'] as $Pro)
        {
            $pro_uid = $Pro['assignee_uid'];
            //Get user gender
            $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
            $gender_tmp = mysql_fetch_array($q1);
            $gender = $gender_tmp['gender'];
            //Check if user have a profile picture
            if (file_exists('sites/default/images/profiles/50x50/'.$pro_uid.'.jpg')) { $avatar = 'sites/default/images/profiles/50x50/'.$pro_uid.'.jpg';}
            else {
                if ($gender == 'm' || $gender == '') { $avatar =' /themes/maennaco/images/prof-avatar-male.png';}

                else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            }
            $pro_maeid = getProId($pro_uid);
            $rid = userRoleId($pro_uid);
            $pro_type = getRole($pro_uid);

            if ($_REQUEST['perm'] == 'read')

                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=".$avatar." style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:'Lato Regular'; text-transform:uppercase;margin-top:20px;\">$pro_maeid, $pro_type</p></div>";

            else    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=".$avatar." style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1' target='_blank'> $pro_maeid, $pro_type</a></div>";

        }
    }elseif($ctype == 'following')
    {

        $following_active = 'active';
        $box_title = "MANAGEMENT";
        foreach($Conns['Client'] as $Pro)
        {
            $pro_uid = $Pro['assignee_uid'];
            echo $pro_uid;
            //Get user gender
            $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
            $gender_tmp = mysql_fetch_array($q1);
            $gender = $gender_tmp['gender'];
            //Check if user have a profile picture
            if (file_exists('sites/default/images/profiles/50x50/'.$pro_uid.'.jpg')) { $avatar = 'sites/default/images/profiles/50x50/'.$pro_uid.'.jpg';}
            else {
                if ($gender == 'm' || $gender == '') { $avatar =' /themes/maennaco/images/prof-avatar-male.png';}

                else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            }
            $pro_maeid = getProId($pro_uid);
            $rid = userRoleId($pro_uid);
            $pro_type = getRole($pro_uid);

            if ($_REQUEST['perm'] == 'read')

                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=".$avatar." style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:'Lato Regular'; text-transform:uppercase;margin-top:20px;\">$pro_maeid, $pro_type</p></div>";

            else    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=".$avatar." style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1' target='_blank'> $pro_maeid, $pro_type</a></div>";
            $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=".$avatar." style=\"float:left; margin-right:5px; width:50px; height:50px;\"><a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1' target='_blank'>$pro_maeid, $pro_type</a></div>";
        }        }elseif($ctype == 'connections')
    {
        $conn_active = 'active';
        $box_title = "CONNECTED";
        foreach($Conns['Visible'] as $Pro)
        {
            $pro_uid = $Pro['assignee_uid'];
            echo $pro_uid;
            //Get user gender
            $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
            $gender_tmp = mysql_fetch_array($q1);
            $gender = $gender_tmp['gender'];
            //Check if user have a profile picture
            if (file_exists('sites/default/images/profiles/50x50/'.$pro_uid.'.jpg')) { $avatar = 'sites/default/images/profiles/50x50/'.$pro_uid.'.jpg';}
            else {
                if ($gender == 'm' || $gender == '') { $avatar =' /themes/maennaco/images/prof-avatar-male.png';}

                else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            }
            $pro_maeid = getProId($pro_uid);
            $rid = userRoleId($pro_uid);
            $pro_type = getRole($pro_uid);

            if ($_REQUEST['perm'] == 'read')

                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=".$avatar." style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:'Lato Regular'; text-transform:uppercase;margin-top:20px;\">$pro_maeid, $pro_type</p></div>";

            else    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=".$avatar." style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1' target='_blank'> $pro_maeid, $pro_type</a></div>";
        }
    }

    if ($box_content == '') $box_content='No users';
    die($box_content);

}

if(checkValues($_REQUEST['value']))
{

    function getProId($id)
    {
        if(empty($id)) return 'invalid id';
        $sql = mysql_query("SELECT rid FROM users_roles WHERE uid = '".(int) mysql_real_escape_string($id)."' LIMIT 1 ");
        $ridn = mysql_fetch_array($sql);
        if ($ridn['rid'] == '3') {

            $sql = "select users_roles.*, maenna_company.projname from users_roles, maenna_company where users_roles.uid = '".(int) mysql_real_escape_string($id)."' and maenna_company.companyid = '".$id."' limit 1";


        }
        else {

            $sql = "select users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname from users_roles, maenna_people where users_roles.uid = '".(int) mysql_real_escape_string($id)."' and maenna_people.pid = '".(int) mysql_real_escape_string($id)."' limit 1";

        }
        $result = mysql_query($sql);
        $Row = mysql_fetch_assoc($result);
        $rid = $ridn['rid'];
        $firstname = strtoupper($Row['firstname']);
        if(in_array($rid, array(6, 10))){
            $output = "MAENNA";
        } elseif ($rid == "3") {
            $output = strtoupper($Row['projname']);

        }
        else
        {
            $output = "${firstname}MAE";// . sprintf("%04s", $id +100);
        }
        return $output;
    }



    function replace_email ($subject) {

        $pattern="/(?:[a-z0-9!#$%&'*+=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+=?^_`{|}~-]+)*|\"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*\")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\])/";

        return preg_replace($pattern,'<i>e-mail obscured</i>',$subject);
    }

    $res = mysql_query("SELECT d_id FROM wall_documents where document_name = '".  mysql_real_escape_string($_REQUEST['doc']) ."'");
    $red = mysql_fetch_array($res);
    $d_id =		$red['d_id'];
    $editorname = getProId($_REQUEST['uid']);


    if ($_REQUEST['type'] == 'eventcom') $d_id = $_REQUEST['eventid'];

    if (md5($_REQUEST['u']."kyarata75") === $_REQUEST['m'])
    {
        mysql_query("INSERT INTO wall_posts (post,f_name,user,date_created,document_id) VALUES('".checkValues($_REQUEST['value'])."','".$editorname."','".$_REQUEST['u']."','".strtotime(date("Y-m-d H:i:s"))."','".$d_id."')");
    }


    $result = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS TimeSpent FROM wall_posts WHERE document_id = '" . $d_id ."' order by p_id desc limit 1");

}
elseif($_REQUEST['show_more_post']) // more posting paging
{
    $next_records = $_REQUEST['show_more_post'] + 10;

    $result = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS TimeSpent FROM wall_posts WHERE document_id = " . $d_id ." order by p_id desc limit ".$_REQUEST['show_more_post'].", 10");

    $check_res = mysql_query("SELECT * FROM wall_posts WHERE document_id = " . $d_id ." order by p_id desc limit ".(int) mysql_real_escape_string($next_records).", 10");

    $show_more_button = 0; // button in the end

    $check_result = mysql_num_rows(@$check_res);
    if($check_result > 0)
    {
        $show_more_button = 1;
    }
}
else
{


    $show_more_button = 1;
    $result = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS TimeSpent FROM wall_posts WHERE document_id = " . $d_id ." order by p_id desc limit 0,10");

}

if ($result) {
    while ($row = mysql_fetch_array($result))
    {
        $comments = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS CommentTimeSpent FROM wall_posts_comments where post_id = ".$row['p_id']." order by c_id asc");		?>
    <div class="friends_area" id="record-<?php  echo $row['p_id']?>" >
        <?php

        $crId = nameToId($row['user']);
        $uType = getUserType($crId);

        if ($uType == 'people' || $uType == 'admin') {

            //Get user gender
            $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
            $gender_tmp = mysql_fetch_array($q1);
            $gender = $gender_tmp['gender'];

            if (file_exists('sites/default/images/profiles/50x50/'.$crId.'.jpg')) { $avatar = 'sites/default/images/profiles/50x50/'.$crId.'.jpg';}
            else {
                if ($gender == 'm' || $gender == '') { $avatar =' /themes/maennaco/images/prof-avatar-male.png';}

                else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            }
        }
        else if ($uType == 'company')  {

//Get cmp_role		
            $q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $crId") or die(mysql_error());
            $cmp_role_tmp = mysql_fetch_array($q1);
            $cmp_role = $cmp_role_tmp['company_type'];
            //Check if user have a profile picture
            if (file_exists('sites/default/images/company/50x50/'.$crId.'.jpg')) {$avatar = 'sites/default/images/company/50x50/'.$crId.'.jpg';}
            else
                if ($cmp_role == 'service') $avatar =' /themes/maennaco/images/cmp-avatar-service.png';
                else $avatar =' /themes/maennaco/images/cmp-avatar-product.png';

        }

        echo "<img src=".$avatar." style=\"float:left; margin-top:13px; margin-right:5px; width:50px; height:50px;\">";

        ?>
        <label style="float:left;margin-top:9px;width:90%;" class="name" >
			   <span style="color:#4169AF;">
					<b><?php if (ifAdmin($row['user']))
                    {echo "MAENNA";}
                    else echo $row['f_name'];?>
                    </b>
					<span style='color:#666;font-style:italic;'>&nbsp;shares a new idea:</span>
			   </span>
            <em>&nbsp;<?php  echo nl2br(replace_email($row['post']));?></em>
            <br clear="all" />

            <span><?php echo ago($row['date_created']); ?></span>
            <!--a href="#" class="tool show_reply_box" rowid="<?php echo $row['p_id'];?>" > Reply</a-->

            <?php
//echo getProId($_REQUEST['id']);
            if($row['user'] == $_REQUEST['u'] || ifAdmin($user->name) || $row['user'] == $user->name)
            {
                ?>
                <a href="#" id="remove_id<?php  echo $row['p_id']?>" style="float:none;" alt="<?php  echo md5($row['p_id'].$row['user']."kyarata75") ?>" name="<?=$row['user'];?>" class="delete tool"> Remove</a>
                <?php 	} ?>


            <!-- <a href="javascript: void(0)" id="post_id<?php  echo $row['p_id']?>" class="showCommentBox">Comments</a>
-->
        </label>
        <br clear="all" />
        <div id="CommentPosted<?php  echo $row['p_id']?>">
            <?php
            $comment_num_row = mysql_num_rows(@$comments);
            if($comment_num_row > 0)
            {
                while ($rows = mysql_fetch_array($comments))
                {
                    $days2 = floor($rows['date_created'] / (60 * 60 * 24));
                    $remainder = $rows['date_created'] % (60 * 60 * 24);
                    $hours = floor($remainder / (60 * 60));
                    $remainder = $remainder % (60 * 60);
                    $minutes = floor($remainder / 60);
                    $seconds = $remainder % 60;
                    ?>
                    <div class="commentPanel" id="comment-<?php  echo $rows['c_id'];?>"
                         align="left" style='padding-top:6px;border-bottom:dotted 1px #CCC;'>
                        <?php
                        $crId = nameToId($rows['user']);
                        $uType = getUserType($crId);

                        if ($uType == 'people' || $uType == 'admin') {

                            //Get user gender
                            $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
                            $gender_tmp = mysql_fetch_array($q1);
                            $gender = $gender_tmp['gender'];

                            if (file_exists('sites/default/images/profiles/50x50/'.$crId.'.jpg')) { $avatar = 'sites/default/images/profiles/50x50/'.$crId.'.jpg';}
                            else {
                                if ($gender == 'm' || $gender == '') { $avatar =' /themes/maennaco/images/prof-avatar-male.png';}

                                else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                            }
                        }
                        else if ($uType == 'company')  {

//Get cmp_role		
                            $q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $crId") or die(mysql_error());
                            $cmp_role_tmp = mysql_fetch_array($q1);
                            $cmp_role = $cmp_role_tmp['company_type'];
                            //Check if user have a profile picture
                            if (file_exists('sites/default/images/company/50x50/'.$crId.'.jpg')) {$avatar = 'sites/default/images/company/50x50/'.$crId.'.jpg';}
                            else
                                if ($cmp_role == 'service') $avatar =' /themes/maennaco/images/cmp-avatar-service.png';
                                else $avatar =' /themes/maennaco/images/cmp-avatar-product.png';

                        }

                        echo "<img src=".$avatar." style=\"float:left; margin-top:5px;margin-right:5px; width:35px; height:35px;\">";

                        ?>

                        <label style="width:85%;"  class="postedComments" >
							  <span style="color:#4169AF;"><b>
                        <?php 	if (ifAdmin($rows['user']))
                    {
                        echo "MAENNA";
                    }
                    else
                    {
                        echo $rows['f_name'];
                    }

                        echo "</b><span style='color:#666;font-style:italic;font-size:10px;font-style:italic'>&nbsp;continues discussion:</span>&nbsp;";
                        echo "<span style='color:#666;'>".nl2br(replace_email($rows['comments']))."</span>";
                        ?>

                        </span>

                        <br>
                        <?php  ?>


                        <span style="display:inline-block;width:90px;margin-left:0px; color:#666666; font-size:11px">
						<?php
                            /*
                            if($days2 > 0)
                            echo date('F d Y H:i:s', $rows['date_created']);
                            elseif($days2 == 0 && $hours == 0 && $minutes == 0)
                            echo "few seconds ago";
                            elseif($days2 == 0 && $hours == 0)
                            echo $minutes.' minutes ago';
                            else
                echo "few seconds ago";
                */
                            echo ago($rows['date_created']);
                            ?>
						</span>
                        <?php

                        if($rows['user'] == $_REQUEST['u'] || ifAdmin($user->name) || $rows['user'] == $user->name){?>
                            &nbsp;&nbsp;<a href="#" id="CID-<?php  echo $rows['c_id'];?>" alt="<?php  echo md5($rows['c_id'].$rows['user']."kyarata75") ?>" name="<?=$rows['user'];?>" class="c_delete tool">Delete</a>
                            </label><?php
                        }?>
                    </div>
                    <?php
                }?>
                <?php
            }?>
        </div>
        <div class="commentBox" align="right"
             id="commentBox-<?php  echo $row['p_id'];?>"
            <?php //echo (($comment_num_row) ? '' :'style=""')?>
             name="<?php echo $editorname; ?>"
             alt="<?php  echo md5($editorname."kyarata75") ?>" >
            <label id="record-<?php  echo $row['p_id'];?>" style="padding-top:0;">
                <textarea class="commentMark"
                          rowid="<?php echo $row['p_id'];?>"
                          id="commentMark-<?php  echo $row['p_id'];?>"
                          name="commentMark" cols="120" style="margin-top:0;"></textarea>
            </label>
            <br clear="all" />

            <a class="tool clseCommentBox comentboxBtns-<?php echo $row['p_id'];?>" style="display: none">CANCEL</a>&nbsp;&nbsp;&nbsp;
            <a id="SubmitComment" class="tool comment  comentboxBtns-<?php echo $row['p_id'];?>" style="display: none" comBox =  >SUBMIT</a>&nbsp;&nbsp;

        </div>
    </div>
        <?php
    }
}
if($show_more_button == 1){?>

<!--<div id="bottomMoreButton">
	<a id="more_<?php // echo @$next_records?>" class="more_records" href="javascript: void(0)">Older Posts</a>
	</div>-->
    <?php  }  ?>
<script type="text/javascript">
    $(document).ready(function(){
        /*$('.commentMark').each(function(){


             $(this).bind('click', function(){
                  var commentBoxId = $(this).attr('rowid');
                  $('.comentboxBtns-'+ commentBoxId).each(function(){
                       $(this).show();
                  })
             })
        });
        $('.clseCommentBox').each(function(){
             $(this).bind('click',function(evt){
                  evt.preventDefault();
                  $(this).hide();
                  $(this).next().hide();
             });
        })*/

    })
</script>
	