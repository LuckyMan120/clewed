<?php
define("__ACCOUNT__", 1);
global $user;
require_once 'new_functions.inc';


if(!isset($_GET['action']) || $_GET['action'] == 'inbox') $folder = 'inbox';
elseif($_GET['action'] == 'outbox') $folder = 'outbox';
read_folder($user->uid, $folder);

function read_folder($uid, $folder = 'inbox'){
    $page = isset($_POST['page']) ? $_POST['page'] : 1;
    $rp = isset($_POST['rp']) ? $_POST['rp'] : 10;
    //$sortname = isset($_POST['sortname']) ? $_POST['sortname'] : 'id';
    $sortorder = isset($_POST['sortorder']) ? $_POST['sortorder'] : 'desc';
    //$query = isset($_POST['query']) ? $_POST['query'] : false;
    //$qtype = isset($_POST['qtype']) ? $_POST['qtype'] : false;
    $start = (($page-1) * $rp);
    $limit = "LIMIT $start, $rp";

    if($folder == 'inbox'){
        $toOrFrom = '`to`';
        $toOrFromOpposite = '`from`';
    } else {
        $toOrFrom = '`from`';
        $toOrFromOpposite = '`to`';
    }


    $sql = "update maenna_message_settings s, maenna_message_text t where s.id=t.id set s.deleted = 1 where t.date<date_add(now(), interval -90 day)";
    mysql_query($sql, $conn);



    $sql = "SELECT usr.uid,x.id,
                   CASE
		   WHEN (usrex.firstname IS NULL AND usrex.lastname IS NULL AND c.company IS NOT NULL) THEN c.company
		   WHEN (usrex.firstname IS NULL AND usrex.lastname IS NULL) THEN usr.name
                      ELSE CONCAT_WS(' ', usrex.firstname, usrex.lastname)
                   END
                      `name`,
                   mtx.subject,
		           mtx.`date` as `date`,
		           mtx.isread,
		           mtx.body
              FROM (SELECT id,
                           " . $toOrFromOpposite . ",
                           message_text_id
                      FROM maenna_message_settings
                     WHERE (deleted=0 or (deleted=1 and `from`=".$uid.")) AND " . $toOrFrom . " = '" . $uid. "'
                    ORDER BY id " . mysql_real_escape_string($sortorder, $conn) . "
                     LIMIT " . $start.", " . $rp . ") x
                   LEFT JOIN maenna_message_text mtx
                      ON mtx.id = x.message_text_id
                   LEFT JOIN maenna_people usrex
                      ON usrex.pid = x." . $toOrFromOpposite . "
                   LEFT JOIN users usr
                      ON usr.uid = x." . $toOrFromOpposite . "
		   LEFT JOIN maenna_company c
		   	ON c.companyid = usr.uid
            ORDER BY mtx.date DESC" . mysql_real_escape_string($sortorder, $conn);

    //die($sql);

    $rows = array();
    $result = mysql_query($sql);
    while ($row = mysql_fetch_array($result)) {
        //$row['name'] = getUserName($row['uid'], $conn);
        $rows[] = $row;
    }
   // test_array($rows);

    $sql = "SELECT count(1) FROM maenna_message_settings WHERE " . $toOrFrom . " = '" . mysql_real_escape_string($uid) . "'";
    $result = mysql_query($sql, $conn);
    $total = mysql_fetch_array($result);
    $total = $total[0];

    $html = "<table class='inboxoutbox'>";
    foreach($rows AS $row){

        $date = showTime($row['date']);
        if (empty($row['isread'])) {$unread = '';}
        else {$unread = 'background-color: #d4dff2;';}
        $html .= "<tr style='".$unread." height:70px;cursor:pointer;' messId = ".utf8_encode($row['id']).">
                     <td style='vertical-align:middle; width:50px;'><img width='50px' height = '50px' src='".getAvatarUrl($row['uid'])."'></td>
                    <td style='vertical-align:middle; width:160px;text-align:center;'><strong>".utf8_encode(strtoupper(getUserById($row['uid'])))."</strong></td>
                    <td style='vertical-align:middle;'>".substr(utf8_encode(ucwords($row['body'])),0,50)."</td>
                    <td  style='width:50px;vertical-align:middle;'>".utf8_encode($date)."</td>

    </tr>";
    }

    $html .= '</table>';


    /* header("Content-type: text/xml");
     $xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
     $xml .= "<rows>";
     $xml .= "<page>$page</page>";
     $xml .= "<total>$total</total>";
     foreach($rows AS $row){
         $date = strtotime($row['date']);
         if ($date < time()-3600*24*06) $row['date'] = date("m/d/Y h:i a", $date);
         else $row['date'] = date("D, h:i a", $date);
         if (empty($row['isread'])) {$bolds="<b>"; $bolde="</b>";}
         else {$bolds=""; $bolde="";}
         $xml .= "<row id='".$row['id']."'>";
         $xml .= "<cell><![CDATA[".$bolds.utf8_encode(strtoupper($row['name'])).$bolde."]]></cell>";
         $xml .= "<cell><![CDATA[".$bolds.utf8_encode(ucwords($row['subject'])).$bolde."]]></cell>";
         $xml .= "<cell><![CDATA[".$bolds.utf8_encode($row['date']).$bolde."]]></cell>";
         $xml .= "<cell><![CDATA[".utf8_encode($row['id'])."]]></cell>";
         $xml .= "</row>";
     }
     $xml .= "</rows>";
     echo $xml;
     exit();*/
   echo $html;
}
