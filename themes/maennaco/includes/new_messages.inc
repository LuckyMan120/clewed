<?php

function messages_inbox($op = null)
{
//
    global $user;
    if(!isset($_GET['action']) || $_GET['action'] == 'inbox' || $_GET['action'] == 'delete'){
        $fromOrTo = 'From';
        return inbox_outbox($fromOrTo);
    } elseif($_GET['action'] == 'outbox'){
        $fromOrTo = 'To';
        return inbox_outbox($fromOrTo);
    } elseif($_GET['action'] == 'newmessage'){
        return show_new_message_page();
    }/* elseif($_GET['action'] == 'readinbox'){
        read_folder('inbox');
    } elseif($_GET['action'] == 'readoutbox'){
        read_folder('outbox');
   }*/elseif($_GET['action'] == 'newmessagepost'){
        return insert_new_message();
    } elseif($_GET['action'] == 'readmessage'){
        return read_message($_GET['msgid']);
    }
}

function messages_folders($op = null)
{
    $Block['title'] = ("Folders");
    // just inbox here for now
    $content = "
		<div class='row' style='padding:7px 0'>
		<a style='cursor:pointer;' id = 'compose' >New Message</a><br />
		</div>
		<div class='row' style='padding:7px 0'>
		<a href='/account?tab=messages&action=inbox'>Inbox</a><br />
		</div>
		<div class='row' style='padding:7px 0'>
                <a href='/account?tab=messages&action=outbox'>Sent</a>
		</div>";
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
/*
function read_folder($folder = 'inbox'){

    $page = isset($_POST['page']) ? $_POST['page'] : 1;
    $rp = isset($_POST['rp']) ? $_POST['rp'] : 10;
    $sortname = isset($_POST['sortname']) ? $_POST['sortname'] : 'id';
    $sortorder = isset($_POST['sortorder']) ? $_POST['sortorder'] : 'desc';
    $query = isset($_POST['query']) ? $_POST['query'] : false;
    $qtype = isset($_POST['qtype']) ? $_POST['qtype'] : false;

    $sort = "ORDER BY $sortname $sortorder";

    $start = (($page-1) * $rp);

    $limit = "LIMIT $start, $rp";

    if($folder == 'inbox'){
        $toOrFrom = '`to`';
        $toOrFromOpposite = '`from`';
    } else {
        $toOrFrom = '`from`';
        $toOrFromOpposite = '`to`';
    }

    global $user;

    $sql = "SELECT x.id,
                   usr.name,
                   mtx.subject,
                   mtx.`date`,
                   x.unread
              FROM (SELECT id,
                           `from`,
                           unread,
                           message_text_id
                      FROM maenna_message_settings
                     WHERE " . $toOrFrom . " = '" . mysql_real_escape_string($user->name) . "'
                    ORDER BY id DESC
                     LIMIT " . mysql_real_escape_string($start) . ", " . mysql_real_escape_string($rp) . ") x
                   LEFT JOIN maenna_message_text mtx
                      ON mtx.id = x.message_text_id
                   LEFT JOIN users usr
                      ON usr.uid = x." . $toOrFromOpposite . "
            ORDER BY x.id " . mysql_real_escape_string($sortorder);

    $rows = array();
    global $active_db;
    $result = mysql_query($sql, $active_db);
    while ($row = mysql_fetch_array($result)) {
        $rows[] = $row;
    }

    $sql = "SELECT count(1) FROM maenna_message_settings WHERE " . $toOrFrom . " = '" . mysql_real_escape_string($user->name) . "'";
    $result = mysql_query($sql, $active_db);
    $total = mysql_fetch_array($result);

    header("Content-type: text/xml");
    $xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    $xml .= "<rows>";
    $xml .= "<page>$page</page>";
    $xml .= "<total>$total</total>";
    foreach($rows AS $row){
        $xml .= "<row id='".$row['id']."'>";
        $xml .= "<cell><![CDATA[".utf8_encode($row['name'])."]]></cell>";
        $xml .= "<cell><![CDATA[".utf8_encode($row['subject'])."]]></cell>";
        $xml .= "<cell><![CDATA[".utf8_encode($row['date'])."]]></cell>";
        $xml .= "</row>";
    }

    $xml .= "</rows>";
    echo $xml;
    exit();
}
*/

function show_new_message_page($to = null, $toUid = null, $subject = null, $body = null, $errorMessage = null){
    global $user;
    global $active_db;

    if (!empty($_GET['msgid'])) {

        $sql = "SELECT * FROM maenna_message_settings s
		                                 LEFT JOIN maenna_message_text mtx
						                               ON mtx.id = s.id

		      WHERE s.id = " . mysql_real_escape_string($_GET['msgid'], $active_db);

        $result = mysql_query($sql, $active_db);
        $row = mysql_fetch_array($result);

        $subject = $row['subject'];
        $to = getUserName($row['from'], $active_db);
        $toUid = $row['from']+100;
        if (!preg_match('/^RE:/i', $subject)) $subject = "RE: ".$subject;
    } //else $subject ='';

    $to_html = '';


    foreach($user->roles  as $rid => $role ){
        if($role == 'authenticated user'){
            continue;
        }

        if($role == 'Company'){//- message between companies and their admin
            $sql = "SELECT assignee_uid
                      FROM maenna_connections 
                     WHERE target_uid='" . mysql_real_escape_string($user->uid, $active_db) . "'
                       AND conntype = 'admin'";
            $result = mysql_query($sql, $active_db);
            $row = mysql_fetch_array($result);
            $to_html = '<input name="toUid" type="hidden" value="' . $row['assignee_uid'] . '" id="searchFieldUid"/>';
            $to_html .= '<textarea class="mesinp" disabled="disabled" >To: '.getUserName($row['assignee_uid'],$active_db)." (company admin)</textarea>";

        }


        else if($role == 'Analyst' //message between professionals connected to a company (listed in team box) and that company's admin
            || $role == 'Investor'
            || $role == 'Other Expert'
            || $role == 'Executive'
        ){
            $sql = "SELECT mc2.assignee_uid
                      FROM maenna_connections mc
                           INNER JOIN users_roles ur
                              ON (    mc.assignee_uid = ur.uid
                                  AND ur.rid = (SELECT role.rid
                                                  FROM role
                                                 WHERE role.name = 'Company'))
                           INNER JOIN maenna_connections mc2
                              ON (    mc.assignee_uid = mc2.target_uid
                                  AND mc2.conntype = 'admin'
                                  )
                     WHERE mc.target_uid = '" . mysql_real_escape_string($user->uid, $active_db) . "'";
            $result = mysql_query($sql, $active_db);
            $to_html = "<select name='toUid'>";
            $to_html .= "<option value=''>Select Destination</option>";
            while ($row = mysql_fetch_array($result)) {
                $to_html .= "<option value='{$row['assignee_uid']}'>".getUserName($row['assignee_uid'],$active_db)."</option>";
            }

            //message between related professionals
            $sql = "SELECT mc2.assignee_uid, mc2.conntype
                      FROM maenna_connections mc1, maenna_connections mc2
		      WHERE mc1.target_uid=mc2.target_uid 
		      AND mc2.conntype!='watchlist'
		      AND mc1.assignee_uid = '" . mysql_real_escape_string($user->uid, $active_db) . "'
		      GROUP BY mc2.assignee_uid
		      ";
            $result = mysql_query($sql, $active_db);
            while ($row = mysql_fetch_array($result)) {
                if ($row['conntype']=="watchlist") continue;
                $to_html .= "<option value='{$row['assignee_uid']}'>".getUserName($row['assignee_uid'],$active_db)."</option>";
            }
            $to_html .= "</select>";
        }
    }



    $content = <<<EOD
<div id="newemailform">
<form action="/account?tab=messages&action=newmessagepost" method="post" enctype="application/x-www-form-urlencoded" target="_self" id="new_message_form">
EOD;
    if(!empty($errorMessage)){
        //
        //          drupal_get_messages('error');
        //   form_set_error('email', t($errorMessage));
        //drupal_set_message($errorMessage);
        $content .= '<div  ><strong>' . $errorMessage . '</strong></div>';//errorMessageMaenna
    }
    if (empty($to_html)) {
        $content .= '<textarea class="mesinp" name="to" autocomplete="off" id="searchField" name="q" >' . $to . '</textarea>';
        $content .= '<input name="toUid" type="hidden" value="' . $toUid . '" id="searchFieldUid"/>';
    } else {
        $content .= $to_html;
    }
    $content .= '<br />                <textarea id="subjectField" class="mesinp" name="subject">' . $subject . '</textarea>
                <br /><br />
                <textarea name="body" class="entry-textarea" >' . $body . '</textarea>
                <br />
		<script src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js" type="text/javascript"></script>
                <script type="text/javascript">';

    if (empty($to_html)) {
        $roles = implode(",", $user->roles);
        $md5=md5($roles.$user->uid."bljoKica75");

        $content .= "jQuery('#searchField').liveSearch({url: '/themes/maennaco/includes/livesearch.php?md5={$md5}&roles={$roles}&uid={$user->uid}&q='});";
        $content .= "jQuery('#searchField').Watermark('To');";
    }
    $content .= "jQuery('#subjectField').Watermark('Subject');";
    $content .= '</script><input name="uid" type="hidden" value="' . $user->uid . '" />';
    $content .= <<<EOD5
<div class="reply_delete_buttons">
            <a class="tool" href="#" onClick="$('#new_message_form').submit();">Send</a>
</div>
</form>
</div>
EOD5;
    /*
    <script type="text/javascript">
    jQuery('#newemailform input[name="to"]').liveSearch({url: '/account?tab=messages&action=livesearch&q='});
    </script>
    */
    $Block['title'] = ("New Message");
    $Block['body'] = content_box($Block['title'], $content);
    //
    return $Block;
}

function insert_new_message(){
    // CHECK PERMISSIONS FIRST

    /*
    1) Sto se tice admina kompanije,nisam ni ja siguran,ali mislim da si u pravu nema sta drugo da bude...
    2),3) Ovo su ti oni boxovi sa leve strane ako si video advisor,connected i selection. To ti je zapravo to. Cini mi se da se kod za generisanje toga nalazi u new_company_detail_right.inc in function connection(). Ja sam to neshto sredjivao,videcesh,ako nisi u nekoj drugoj skripti lako ces pokupiti te podatke.
    4) Super admin su ti useri koji imaju tu ulogu.
    */
    $body = $_POST['body'];
    $subject = $_POST['subject'];
    $recipientName = $_POST['to'];
    $recipientUid = $_POST['toUid'];

    global $active_db;
    global $user;
    $user_can_send_message = false;
    $errMsg = "Access denied. You can only send messages to users connected to you. Thank you.";
    foreach($user->roles  as $rid => $role ){
        if($role == 'authenticated user'){
            continue;
        }

        if($role == 'Company'){//- message between companies and their admin
            /*
            [14:21:57] Nick Cosic: select assignee_uid from maenna_connections where target_uid='{$company_id}'
            [14:21:57] Nick Cosic: related - to su svi assignee_uid koji su povezani na isti target_uid;
            [14:22:45] Nick Cosic: a da bi naso admina u ono gore moras da dodas and conntype='admin';
            [14:22:55] Nick Cosic: inace ti samo daje sve vrste veza
            [14:46:12] Nick Cosic: ukljuci samo u related i taj target_uid
            [14:46:44] Nick Cosic: znaci sve target_uidove koji su vezani za nekog usera i onda sve assignee_uid koji su vezani za te target_idove
            */
            $sql = "SELECT assignee_uid
                      FROM maenna_connections 
                     WHERE target_uid='" . mysql_real_escape_string($user->uid, $active_db) . "'
                       AND conntype = 'admin'";
            $result = mysql_query($sql, $active_db);
            $row = mysql_fetch_array($result);
            if(!empty($row)){
                $user_can_send_message = true;
                $errMsg = false;
                break;
            }
        }

        if($role == 'Super admin'){ //- message between superadmin and admins
            /*$sql = "SELECT 'yes'
                      FROM users_roles
                     WHERE     rid IN (SELECT rid
                                         FROM role
                                        WHERE name = 'Maennaco admin')
                           AND uid = '" . mysql_real_escape_string($recipientUid, $active_db) . "'";
            $result = mysql_query($sql, $active_db);
            $row = mysql_fetch_array($result);
	    if(!empty($row)){*/
            $user_can_send_message = true;
            $errMsg = false;
            /*  break;
      }*/
        }

        if($role == 'Analyst' //message between professionals connected to a company (listed in team box) and that company's admin
            || $role == 'Investor'
            || $role == 'Other Expert'
            || $role == 'Executive'
        ){
            $sql = "SELECT mc.assignee_uid
                      FROM maenna_connections mc
                           INNER JOIN users_roles ur
                              ON (    mc.assignee_uid = ur.uid
                                  AND ur.rid = (SELECT role.rid
                                                  FROM role
                                                 WHERE role.name = 'Company'))
                           INNER JOIN maenna_connections mc2
                              ON (    mc.assignee_uid = mc2.target_uid
                                  AND mc2.conntype = 'admin'
                                  AND mc2.assignee_uid = '" . mysql_real_escape_string($recipientUid, $active_db) . "')
                     WHERE mc.target_uid = '" . mysql_real_escape_string($user->uid, $active_db) . "'";
            $result = mysql_query($sql, $active_db);
            $row = mysql_fetch_array($result);
            if(!empty($row)){
                $user_can_send_message = true;
                $errMsg = false;
                break;
            }

            //message between related professionals
            $sql = "SELECT mc1.assignee_uid
                      FROM maenna_connections mc1, maenna_connections mc2
		      WHERE mc1.target_uid=mc2.target_uid 
		       AND mc1.assignee_uid = '" . mysql_real_escape_string($user->uid, $active_db) . "'
                       AND mc2.assignee_uid = '" . mysql_real_escape_string($recipientUid, $active_db) . "'";
            $result = mysql_query($sql, $active_db);
            $row = mysql_fetch_array($result);
            if(!empty($row)){
                $user_can_send_message = true;
                $errMsg = false;
                break;
            }
        }

        /*
        * Now I have to do checks in the opposite direction, for example if super admins are allowed to send
        * messages to admins, then I should allow admins to reply and send messages to super admins too
        */

        if($role == 'Maennaco admin') {
            // message between company's admin and professionals connected to that company
            $sql = "SELECT mc.target_uid
                      FROM maenna_connections mc
                           INNER JOIN users_roles ur
                              ON (    mc.target_uid = ur.uid
                                  AND ur.rid = (SELECT role.rid
                                                  FROM role
                                                 WHERE role.name = 'Company'))
                           INNER JOIN maenna_connections mc2
                              ON (    mc.target_uid = mc2.target_uid
                                  AND mc2.assignee_uid = '" . mysql_real_escape_string($recipientUid, $active_db) . "')
                     WHERE mc.assignee_uid = '" . mysql_real_escape_string($user->uid, $active_db) . "' AND mc.conntype = 'admin'";
            $result = mysql_query($sql, $active_db);
            $row = mysql_fetch_array($result);
            if(!empty($row)){
                $user_can_send_message = true;
                $errMsg = false;
                break;
            }

            // message between admins and superadmin
            $recipientIsSuperAdmin = "SELECT 'Yes'
                                        FROM users_roles
                                       WHERE     rid = (SELECT role.rid
                                                          FROM role
                                                         WHERE role.name = 'Super admin')
                                             AND uid = '" . mysql_real_escape_string($recipientUid, $active_db) . "'";
            $result = mysql_query($recipientIsSuperAdmin, $active_db);
            $row = mysql_fetch_array($result);
            if(!empty($row)){
                $user_can_send_message = true;
                $errMsg = false;
                break;
            }

            // company admin and his company
            $matchCompany ="SELECT 'Yes'
                              FROM maenna_connections
                             WHERE target_uid = '" . mysql_real_escape_string($recipientUid, $active_db) . "'
                               AND assignee_uid ='" . mysql_real_escape_string($user->uid, $active_db) . "'";
            $result = mysql_query($matchCompany, $active_db);
            $row = mysql_fetch_array($result);
            if(!empty($row)){
                $user_can_send_message = true;
                $errMsg = false;
                break;
            }
        }




    }// end foreach

    if($user_can_send_message){
        $result = mysql_query(
            "INSERT INTO maenna_message_text (subject, body)
             VALUES ('" . mysql_real_escape_string($_POST['subject']) . "', '" . mysql_real_escape_string($_POST['body']) . "')",
            $active_db
        );
        $maenna_message_text_id = mysql_insert_id();
        $sql = "INSERT INTO maenna_message_settings (`to`, `from`, unread, message_text_id)
                VALUES ('" . mysql_real_escape_string($recipientUid) . "',
                        '" . mysql_real_escape_string($user->uid) . "',
                        'u',
                        " . $maenna_message_text_id . ")";
        $result = mysql_query($sql, $active_db);

        //$maenna_message_id = mysql_insert_id();

        // get recipient's email
        $sql = "SELECT mail
                  FROM users
                 WHERE uid = '" . mysql_real_escape_string($recipientUid, $active_db) . "'";
        $result = mysql_query($sql, $active_db);
        $row = mysql_fetch_array($result);
        $recipientEmail = $row['mail'];
        $email_text = <<<endofemail
Please login into your account on Maennaco to see your new message.

Yours,
Maennaco
endofemail;

        $headers = "From: noreply@clewed.com";
        $headers .= "\r\nReply-To: noreply@clewed.com";
        $headers .= "\r\nX-Mailer: PHP/".phpversion();

        # testing:
        #print "$recipientEmail";
        #$recipientEmail="nick.cosic@cp-dev.com";

        mail($recipientEmail, "You've got new message on Maennaco", $email_text, $headers);

        $fromOrTo = 'From';
        $Block = inbox_outbox($fromOrTo);
        return $Block;
    }

    $Block = show_new_message_page($recipientName, $recipientUid, $_POST['subject'], $_POST['body'], $errMsg);
    return $Block;
}

function inbox_outbox($fromOrTo, $errMsg = null){
    global $user;
    global $AccessObj;
    global $active_db;

    if (!empty($_GET['action']) && $_GET['action']=='delete' && !empty($_GET['msgid'])) {
        // this will only delete messages in inbox, not sent items
        $msgid = $_GET['msgid'];
        $sql = "update maenna_message_settings set deleted=1 where id= ".mysql_real_escape_string($msgid, $active_db).
            " AND `to` = " . mysql_real_escape_string($user->uid, $active_db) ;
        mysql_query($sql);
        print "Message deleted!";
    }

    if($fromOrTo == 'To'){
        $Block['title'] = "Sent";
        $url = "url : '/themes/maennaco/includes/inbox_outbox.php?uid={$user->uid}&folder=outbox',";
        $title = "title : 'Outbox',";
    } else {
        $Block['title'] = "Inbox";
        $title = "title : 'Inbox',";
        $url = "url : '/themes/maennaco/includes/inbox_outbox.php?uid={$user->uid}&folder=inbox',";
    }

    $Block['title'] .= "<span style='margin-top:5px;height:20px;line-height:22px;border-left:thin solid #666;padding-left:40px; float:right;display:inline-block;font-weight:bold; font-family:Helvetica; font-size:13px;'>Sort By <img class='openSortPanel' panelid='messages_sort' style='width:20px; margin-left:20px;' src='themes/maennaco/images/arrow_down.png'></span>";

    if ($_GET['sort'] == '' || $_GET['sort'] == 'date') $curr_sort = 'date';

    else $curr_sort = 'name';

    $sortLinks = '<a href="account?tab=messages&sort=name" style="';
    if ($curr_sort == 'name')
        $sortLinks .= 'font-weight:bold;';
    else
        $sortLinks .= 'font-style:italic;';
    $sortLinks .= 'cursor:pointer; margin-right:15px;">Name</a> |<a href="account?tab=messages&sort=date" style="';
    if ($curr_sort == 'date')
        $sortLinks .= 'font-weight:bold;';
    else
        $sortLinks .= 'font-style:italic;';
    $sortLinks .= 'cursor:pointer; margin-right:15px;margin-left:15px;">Date</a></div>';

    $html1 = '

            <style>

            .ui-dialog .ui-dialog-titlebar { padding: .4em 1em; position: relative; background-color:#d4e0f1;border-top:none;border-left:none;border-bottom:none; border-right:none;color:#5d6168; }
            .ui-dialog .ui-dialog-title { margin-left:135px !important; }
            .ui-dialog .ui-dialog-buttonpane .ui-dialog-buttonset {margin-left: 100px !important;}
            .ui-dialog .ui-dialog-buttonpane .ui-dialog-buttonset {float: none !important; }
            .ui-state-default, .ui-widget-content .ui-state-default, .ui-widget-header .ui-state-default {
            border: 1px solid #c5dbec;
            background: none;
            background-color: #33363e;
            font-weight: bold;
            color: #dadbd5;
            }

            .ui-corner-all, .ui-corner-bottom, .ui-corner-right, .ui-corner-br {
                -moz-border-radius-bottomright:0px !important;
                -webkit-border-bottom-right-radius:0px !important;
                -khtml-border-bottom-right-radius:0px !important;
                border-bottom-right-radius:0px !important;
                }
                .ui-corner-all, .ui-corner-bottom, .ui-corner-left, .ui-corner-bl {
                -moz-border-radius-bottomleft:0px !important;
                -webkit-border-bottom-left-radius:0px !important;
                -khtml-border-bottom-left-radius:0px !important;
                border-bottom-left-radius:0px !important;
                }
                .ui-corner-all, .ui-corner-top, .ui-corner-right, .ui-corner-tr {
                -moz-border-radius-topright:0px !important;
                -webkit-border-top-right-radius:0px !important;
                -khtml-border-top-right-radius:0px !important;
                border-top-right-radius:0px !important;
                }
                .ui-corner-all, .ui-corner-top, .ui-corner-left, .ui-corner-tl {
                -moz-border-radius-topleft:0px !important;
                -webkit-border-top-left-radius:0px !important;
                -khtml-border-top-left-radius:0px !important;
                border-top-left-radius:0px !important;
                }

            .ui-state-hover {background-color: #33539e !important;}

            .ui-corner-all, .ui-corner-bottom, .ui-corner-left, .ui-corner-bl {
            -moz-border-radius-bottomleft: 0px !important;
            -webkit-border-bottom-left-radius: 0px !important;
            -khtml-border-bottom-left-radius:  0px !important;
            border-bottom-left-radius: 0px !important;
            }

            .form-submit {
                background-color: #33363e;
                background:none;
                font-weight: bold;
                color: #2e6e9e;

            }

                        .shaded_title {

                            background:#dde6f5;
                            height:30px;
                            line-height:30px;
                            padding-left:15px;

                        }

                        .main_content {

                            padding-top:0 !important;

                        }
                        ul.as-selections {
                            background-color: #EDEDED !important;
                            border: none !important;
                            width:450px;
                            }

                        .newMessageInput {

                        background-color: #ededed;
                        background-image: none !important;
                        /*border: solid 1px #ededed !important;*/
                        width: 335px;
                        height: 20px;
                        padding-left:10px !important;
                        }

                        .newMessageTxt {

                        background-color: #ededed;
                        background-image: none !important;
                        border: solid 1px #ededed !important;
                        width: 450px;
                        height: 60px;
                        padding-left:10px !important;



                        }



                    </style>
    <div id="messages_sort" style="display:none;line-height:30px;height:30px;width:100%; background-color:#f2f2f2"><span style="margin-top:0; display:inline-block;vertical-align:middle;margin-left:15px;">
    '.$sortLinks.'</span></div>';
    ob_start();
    include '/themes/maennaco/includes/inbox_outbox.php';
    $html1 .= ob_get_clean();
    $html1 .= "<div id='newMessageDlg' style='display:none;' title = 'NEW MESSAGE'>

    <input type='text' class='newMessageInput' id='messTo'><br>
    <textarea id='messTxt' class='mceNoEditor newMessageTxt'></textarea>

    </div>";
    $html1 .= "
    <link type='text/css' href='/themes/maennaco/jui/css/redmond/homepagecss/jquery-ui-1.8.15.custom.css' rel='stylesheet' />
    <script type='text/javascript' src='/themes/maennaco/jui/comments/js/jquery.livequery.js'></script>
    <script type='text/javascript' src='/themes/maennaco/jui/comments/js/jquery.autosuggest.js'></script>
    <script src='/themes/maennaco/jui/comments/js/jquery.watermarkinput.js' type='text/javascript'></script>
    <script type='text/javascript' src='/themes/maennaco/jui/comments/js/jquery.autosuggest.js'></script>
    <link href='themes/maennaco/jui/comments/css/autosuggest.css' type='text/css' rel='stylesheet' />
    <script type='text/javascript'>

        $(document).ready(function(){
         init_sortPanel();

        var availableTags = {items: [
		//Get the advisors and connected users for the autocomplete feature; $companyid was gotten in the earlier phase in new_company_detail_left.php
";
    if ($AccessObj->user_type == 'company') {
            $companyid = $user->uid;
			$Conns = Connections::Com_conns($companyid);
				foreach($Conns['Advisor'] as $Pro)
			    {
				$pro_uid = $Pro->assignee_uid;
				$pro_maeid = getProId($pro_uid);
				$html1 .=  '{value: "'.$pro_uid.'", name: "'.$pro_maeid.'"},';
			    }

				foreach($Conns['Client'] as $Pro)
			    {
				$pro_uid = $Pro->assignee_uid;
				$pro_maeid = getProId($pro_uid);
				$html1 .= '{value: "'.$pro_uid.'", name: "'.$pro_maeid.'"},';
			    }
			    $q = mysql_query("SELECT projname FROM maenna_company WHERE companyid = '".$_REQUEST['id']."'");
			    $q = mysql_fetch_array($q);
                            $html1 .= '{value: "'.$_REQUEST['id'].'", name: "'.$q['projname'].' "}';
    }

		$html1 .= "]};

        $('#messTo').autoSuggest(availableTags.items, {startText: 'TO', selectedItemProp: 'name', searchObjProps: 'name'});

         $('#compose').click(function () {

         $('#newMessageDlg').dialog('open');

         });

         $('#newMessageDlg').dialog({
			modal: true,
			autoOpen: false,
			width: '400',
			height: '260',
						buttons:[
            {
                text:'SEND',
                click:function () {

                var from = '".$user->uid."';
                var toArray = $('.as-values').val();
                var body = $('#messTxt').val();

                $.post('/themes/maennaco/includes/messaging_fetch.php?type=new', {sender: from,toArray: toArray,body: body},

                function(response){

                    alert(response);


                });


                },
                'class':'form-submit'
            },
            {
                text:'CANCEL',
                click:function () {
                $(this).dialog('close');
            },
                'class':'form-submit'
            }
        ]
		});

         $('#messTo').Watermark('TO');
         $('#messTxt').Watermark('MESSAGE');
         });
    </script>";

    /*<table class="inboxoutbox" style="display: none"></table>

    <script type="text/javascript">
var goingtomes=false;


$(document).ready(function(){
 init_sortPanel();
 });

        $(".inboxoutbox").flexigrid({
EOD;

    if(isset($_POST['toUid'])){
        $html1 = '<div class="messages" >Message sent!</div>' . $html1;
    }
    $html2 = <<<EOD2
            dataType : 'xml',
            colModel : [ {
EOD2;
    $html2 .= "  display : '" . $fromOrTo . "',
                 name : '" . strtolower($fromOrTo) . "',";
    $html2 .= <<<EOD55
                width : 159,
                sortable : false,
                align : 'left',
		process: procMe
            }, {
                display : 'Subject',
                name : 'subject',
                width : 540,
                sortable : false,
                align : 'left',
		process: procMe
            }, {
                display : 'Date',
                name : 'date',
                width : 152,
                sortable : true,
                align : 'left',
		process: procMe
            }, {
                display : 'Id',
                name : 'id',
                width : 20,
                sortable : false,
                align : 'left',
		process: procMe,
                hide : true
            } ],
            buttons : [ {
                name : 'Open',
                bclass : 'open',
                onpress : test
	    }, {
                name : 'New',
                bclass : 'new256',
                onpress : test
            }, {
                name : 'Delete',
                bclass : 'delete',
                onpress : test
            }, {
                separator : true
            } ],
            sortname : "id",
            sortorder : "desc",
            usepager : true,
EOD55;
    $html3 = <<<EOD3
            useRp : true,
            rp : 15,
            showTableToggleBtn : false,
            width : 900,
            height : 300
        });
/*$(document).ready(function(){
	$('.flexigrid .bDiv .inboxoutbox td div').css('margin:0px;');
	$('.flexigrid .bDiv .inboxoutbox td').css('width:1px;');
});
function procMe(celDiv,id){
    $(celDiv).dblclick(
      function(){
setTimeout(gotomes, 500);
}
    )
    $(celDiv).click(
      function(){
	gotomes();
}
    )
  //process rowData here
}
function gotomes() {

	if ($('.trSelected :nth-child(4) div').html()==null) {
		setTimeout(gotomes, 500);
		return;
	} else {
	}
	window.location.href = "/account?tab=messages&action=readmessage&msgid=" + $('.trSelected :nth-child(4) div').html();
}

        function test(com, grid) {
        //debugger;
            if (com == 'Open' && $('.trSelected', grid).length > 0) {
                window.location.href = "/account?tab=messages&action=readmessage&msgid=" + $('.trSelected :nth-child(4) div', grid).html();
            } else if (com == 'New') {
                window.location.href = "/account?tab=messages&action=newmessage";
	    } else if (com == 'Delete' && $('.trSelected', grid).length > 0) {
                window.location.href = "/account?tab=messages&action=delete&msgid=" + $('.trSelected :nth-child(4) div', grid).html();
            }
        }
    </script>
EOD3;*/

    $content = $html1;//$url . $html2 /*$title . */. $html3;

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function read_message($msgid){

    global $active_db;
    global $user;
    $sql = "SELECT x.id,
                   CASE
                      WHEN (usrExFrom.firstname IS NULL AND usrExFrom.lastname IS NULL) THEN usrFrom.name
                      ELSE CONCAT_WS(' ', usrExFrom.firstname, usrExFrom.lastname)
                   END
                      `from`,
                   CASE
                      WHEN (usrExTo.firstname IS NULL AND usrExTo.lastname IS NULL) THEN usrTo.name
                      ELSE CONCAT_WS(' ', usrExTo.firstname, usrExTo.lastname)
                   END
                      `to`,
                   mtx.subject,
                   mtx.body,
		   usrFrom.uid as fromuid,
		   usrTo.uid as touid,
                   DATE_FORMAT(mtx.`date`, '%W %D %b %Y') as `date`
              FROM (SELECT id,
                           `to`,
                           `from`,
                           unread,
                           message_text_id
                      FROM maenna_message_settings
                     WHERE id = " . mysql_real_escape_string($msgid, $active_db) . "
                           AND (`to` = " . mysql_real_escape_string($user->uid, $active_db) . "
                                OR `from` = " . mysql_real_escape_string($user->uid, $active_db) . ")
                   ) x
                   LEFT JOIN maenna_message_text mtx
                      ON mtx.id = x.message_text_id
                   LEFT JOIN maenna_people usrExFrom
                      ON usrExFrom.pid = x.`from`
                   LEFT JOIN maenna_people usrExTo
                      ON usrExTo.pid = x.`to`
                   LEFT JOIN users usrFrom
                      ON usrFrom.uid = x.`from`
                   LEFT JOIN users usrTo
                      ON usrTo.uid = x.`to`;";
    $result = mysql_query($sql, $active_db);
    $row = mysql_fetch_array($result);
    $Block['title'] = $row['subject'];
    mysql_query("update maenna_message_text mtx, maenna_message_settings mms set mtx.isread=1 where mms.`to` = " . mysql_real_escape_string($user->uid, $active_db) . " AND mms.message_text_id = mtx.id and mms.id= ".mysql_real_escape_string($msgid, $active_db));
    $content = '<table width="100%" border="0" cellspacing="0" cellpadding="3">
                  <tr>
                    <td width="100">From:</td>
                    <td>' . htmlentities(getUserName($row['fromuid'],$active_db)) . '</td>
                  </tr>
                  <tr>
                    <td width="100">To:</td>
                    <td>' . htmlentities(getUserName($row['touid'],$active_db)) . '</td>
                  </tr>
                  <tr>
                    <td >Date:</td>
                    <td>' . htmlentities($row['date']) . '</td>
                  </tr>
                  <tr>
                    <td >Subject:</td>
                    <td>' . htmlentities($row['subject']) . '</td>
                  </tr>
                  <tr>
                    <td colspan="2">' . htmlentities($row['body']) . '</td>
                  </tr>
		  </table>
          <div class="reply_delete_buttons">
            <a class="tool" href="?tab=messages&action=newmessage&msgid='.$msgid.'">Reply</a>
          </div>
          <div class="reply_delete_buttons">
            <a class="tool" href="?tab=messages&action=delete&msgid='.$msgid.'">Delete</a>
          </div>';
    //  <a class="button" href="?tab=messages&action=newmessage&msgid='.$msgid.'">Reply</a>
    // <a class="button" href="?tab=messages&action=delete&msgid='.$msgid.'">Delete</a>';
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
