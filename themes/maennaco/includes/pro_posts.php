<?php
global $base_url;
require __DIR__ . '/../../../lib/init.php';
require __DIR__ . '/safe_functions.inc';
require __DIR__ . '/dbcon.php';

use Clewed\Insights\InsightModel;
use Clewed\Insights\InsightRepository;


error_reporting (E_ALL ^ E_NOTICE);
session_start();
date_default_timezone_set('EST');

$insightDiscountModel = new Clewed\Insights\DiscountModel();

function ifFollowing($pid, $companyid) {
    $result = mysql_query("select * from maenna_followers where uid =  " . (int) $pid . " and cid =  " . (int) $companyid);
    if (mysql_num_rows($result) > 0) return true; else return false;
}

function getFollowers($uid) {

    $q = mysql_query("SELECT * FROM maenna_followers WHERE uid = '".$uid."'");
    $colls = array();
    if ($q){
        while ($r = mysql_fetch_array($q)) $colls[] =$r['cid'];
    }
    return $colls;
}

function proHasInsights($pid) {
    $sql    = "select id from maenna_professional WHERE postedby = " . (int) $pid . " and approve_status = 1";
    $result = mysql_query($sql);
    if (mysql_num_rows($result) == 0) {
        return false;
    } else return true;
}

function getUserRatings ($uid) {

    $user_weight = $admin_weight = 0;

    $sql = "select
          sum(case when admin <> 1 then 1 else 0 end) as non_admin,
          sum(case when admin = 1 then 1 else 0 end) as admin_cnt
                from user_rating ur
                where target_uid = ". (int) $uid;
    $result = mysql_query($sql);
    $Data = mysql_fetch_object($result);

    if ($Data->non_admin < 25) {
        $user_weight = round($Data->non_admin * 0.02,2);
        $admin_weight = round(1.00 - $user_weight,2);
    }
    else {
        $user_weight = 0.50;
        $admin_weight = 0.50;
    }

    $total_weight = round($Data->non_admin*$user_weight + $Data->admin_cnt*$admin_weight,2);
//die("u=".$user_weight."a=".$admin_weight."t=".$total_weight);
    $sql = "select
          count(*) as total,
          sum(case when admin <> 1 then rate_overall *".$user_weight." else 0 end) as non_admin_rates,
          sum(case when admin = 1 then rate_overall *".$admin_weight." else 0 end) as admin_rates
                from user_rating ur
                where target_uid = ".(int) $uid;
    $result = mysql_query($sql);
    $Row = mysql_fetch_object($result);
    $wrate = $total_weight ? round(($Row->non_admin_rates + $Row->admin_rates)/$total_weight,2) : 0;
    return array('total'=>$Row->total, 'rate' => $wrate);
}

if ($_REQUEST['type'] == 'unmatch_pro') {
    $conn_id = $_REQUEST['cid'];
    $hash = $_REQUEST['m'];

    if ($hash != md5($conn_id."kyarata75")) die("false");

    $db = \Clewed\Db::get_instance();

   if ($db->run("delete from maenna_connections where connid = ?",array($conn_id))) die("true");
   else die("false");

}


$next_records = 10;
$show_more_button = 0;

$res = mysql_query("SELECT d_id FROM wall_documents where document_name = '" . basename($url) . "'");
$red = mysql_fetch_array($res);
$d_id = $red['d_id'];

if ($_REQUEST['type'] == 'moreEventsCalendar') {
    $eventRes = mysql_query("SELECT * FROM maenna_company_events_inv inv JOIN maenna_company_events ev ON ev.eventid = inv.eventid WHERE uid = '" . $_REQUEST['u'] . "' AND inv.status = 'confirmed' ORDER BY datetime DESC LIMIT 5,120000000000");
    while ($events = mysql_fetch_array($eventRes)) {
        $content .= date("m/d/Y", $events['datetime']) . " - <a target = \"_blank\" href=\"http://maennaco.cp-dev.com/account?tab=companies&page=company_detail&id=" . $events['companyid'] . "&mtab=advice\" >" . $events['title'] . "</a><br>";
    }
    die($content);
}

if ($_REQUEST['type'] == 'removeFile') {
    mysql_query("UPDATE maenna_company_data SET deleted = 1 WHERE dataid = '" . $_REQUEST['fileid'] . "'");
    die();
}

if ($_REQUEST['type'] == 'commInv') {
    $invitees = explode(",", $_REQUEST['invitees']);
    $cmpname  = mysql_query("SELECT projname FROM maenna_company WHERE companyid = '" . $_REQUEST['cid'] . "'");
    $cmpname  = mysql_fetch_array($cmpname);
    foreach ($invitees as $value) {
        $sqlmail = mysql_query("SELECT mail FROM users WHERE uid = '" . $value . "'");
        $invmail = mysql_fetch_array($sqlmail);
        $to      = $invmail['mail'];
        $subject = 'Give advice on MAENNA for ' . ucwords($cmpname['projname']);
        $message .= ucwords($cmpname['projname']) . ' requests your advice / comments on ' . $_REQUEST['name'];
        $message .= '<br><br>';
        $message .= 'Follow this link to see and comment on this file. <br>';
        $message .= 'http://clewed.com/account?tab=companies&page=company_detail&id=' . $_REQUEST['cid'] . '&mtab=file&file=' . urlencode($_REQUEST['filename']) . '&name=' . urlencode($_REQUEST['name']) . ' <br>';
        $headers = "From:clewed@clewed.com \r\n";
        $headers .= "Content-type: text/html; charset=iso-8859-1 \r\n";
        $headers .= "Bcc: Maenna@maennaco.com \n";
        mail($to, $subject, $message, $headers) or die("Message couldn`t be send. Please try again!");
        $to      = '';
        $message = '';
        $headers = '';
    }
    die('Your invitations were sent succesfully!');
}

if ($_REQUEST['type'] == 'profileInfo') {
    $base_url    = $_REQUEST['base_url'];
    $sql         = mysql_query("SELECT * FROM maenna_professional WHERE id = '" . $_REQUEST['eventid'] . "'");
    $sql         = mysql_fetch_array($sql);
    $like_result = mysql_query("SELECT *  FROM  `maenna_followers` WHERE  uid = '" . $sql['postedby'] . "' and cid = '" . $_REQUEST['uid'] . "'");
    $likeabout   = mysql_num_rows($like_result);
    if ($_REQUEST['eventid']) {
        $user_id = $sql['postedby'];
    } elseif ($_REQUEST['pro_id']) {
        $user_id = $_REQUEST['pro_id'];
    }
    $utype       = getUserType($_REQUEST['uid']);
    $user_sql    = mysql_query("SELECT * FROM maenna_people WHERE pid = '" . $user_id . "'");
    $user_data   = mysql_fetch_array($user_sql);
    $people_pid  = $user_data['pid'];
    $people_sql  = mysql_query("select * from maenna_people_data where pid = '$people_pid' and data_type = 'addinfo' and data_attr = 'experties'");
    $people_data = mysql_fetch_array($people_sql);
    if ($user_data['username_type'] == 1) {
        $username = ucfirst($user_data['firstname']);
    } else {
        $username = ucfirst($user_data['firstname']) . ' ' . ucfirst($user_data['lastname']);
    }
    $avatar = getAvatarUrl($user_id, "150");
    ?>
    <div id="get_reviews_dialog" style="display:none"></div>

    <script type="text/javascript">
        $(document).ready(function(){

            $(".rateit").rateit();
            init_rate();
            $("#get_reviews_dialog").dialog({
                modal: true,
                autoOpen: false,
                width:700,
                height:500,
                resizable: true,
                title: "User Ratings",
                buttons: {
                    Close: function() {
                        $(this).dialog("close");
                    }

                }
            });

        });

        <?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

        function follow(thisObj,type, id, uid) {

            thisObj = $(thisObj);
            var status = thisObj.attr('type') == 'follow' ? 1 : 0;

            $.ajax({
                type: 'get',
                url: './themes/maennaco/includes/delete.php?' +
                    'type=follow&' +
                    'id=' + id + '&' +
                    'user_id=' + uid + '&' +
                    'status=' + status + "&" +
                    "u=<?php echo $u; ?>&" +
                    "m=<?php echo $m; ?>",
                success: function () {
                    if (thisObj.attr('type') == 'follow') {
                        thisObj.attr("type", 'unfollow');
                        thisObj.html('Unfollow');
                    }
                    else {
                        thisObj.attr("type", 'follow');
                        thisObj.html('Follow');
                    }
                }
            });
        }

        function showMoreInfo() {
            $('#info').hide();
            $('#show_more').show();

        }
    </script>
    <table width="100%">
        <tr>
            <?php
            $p_id = $user_id;
            $sql_profdetails = mysql_query("SELECT * FROM  `maenna_people_data` WHERE `pid` = '" . $p_id . "' AND `data_attr` = 'experties'");
            $pdetails_res = mysql_fetch_array($sql_profdetails);
            $sql_inddetail = mysql_query("SELECT * FROM  `maenna_people_data` WHERE `pid` = '" . $p_id . "' AND `data_attr` = 'industryview'");
            $pind_res = mysql_fetch_array($sql_inddetail);
            $sql_managdetail = mysql_query("SELECT * FROM  `maenna_people_data` WHERE `pid` = '" . $p_id . "' AND `data_attr` = 'mgmtview'");
            $pmanage_res = mysql_fetch_array($sql_managdetail);

            $sql_edu = mysql_query("SELECT data_value,data_value3 FROM  `maenna_people_data` WHERE `pid` = " . $p_id . " and data_type = 'education' order by dataid desc limit 1");
            $edu_res = mysql_fetch_array($sql_edu);

            $graduate = $edu_res['data_value3'];
            $undergraduate = $edu_res['data_value'];

            $ratings = getUserRatings($user_id);

            $followers = getFollowers($user_id);

            //Determine whether user can rate pro

            if ($utype == 'admin') $readonly = 'data-rateit-readonly="false"';

            //if (!canRateUser($user_id,$_REQUEST['uid']))  $readonly = 'data-rateit-readonly="true"';



            if ($user_data['protype'] == 'other') {
                $user_data['protype'] = 'Other Expert';
            } else {
                if ($user_data['protype'] == 'executive'){
                    $user_data['protype'] = 'Operator';
                }
            }

            if ($undergraduate != '') $school = $undergraduate . "<br>";
            if ($graduate != '') $school .= $graduate;
            if ($ratings['total'] == 0) $reviews ="<span style='margin-left:10px;'>No reviews</span>";
            else $reviews = "<span
                style='margin-left:10px;cursor:pointer;'
                data-tooltip='".(((int)$ratings['total']) >= 5 ? $ratings['total'].' total reviews':' Read reviews')."'
                data-uid='".$user_id."'
                class='get_reviews'>".(((int)$ratings['total']) >= 5 ? '('.$ratings['total'].')':'')." Read reviews</span>";
            ?>
            <td width="45%" valign="top">
                <img src="<?php echo $avatar; ?>" width="90" height="90"><br>
                <div data-tooltip="<?=number_format((float)$ratings['rate'], 2, '.', '');?> star rating"
                     data-rateit-readonly="true"
                     data-rateit-starwidth="12"
                     data-rateit-starheight="12"
                     data-rateit-step="1"
                     data-rateit-value="<?=$ratings['rate'];?>"
                     class="rateit"></div> <?=$reviews;?>
                <div style="font-family:Lato Italic; font-size:14px;" class="poptitle">
                    <?= $username ?>, <?= ucwords($user_data['protype']) ?>
                </div>
                <div style="font-family:'Lato Italic'; font-size:14px;">
                        <?= preg_replace('/(?<!\ )[A-Z]/', ' $0', $user_data['experties']) ?></div>
                <div style="font-family:'Lato Italic'; font-size:14px;">
                        <?= $school; ?></div>
                <br/>
                <?php
                if ($_REQUEST['iid'] != '') {
                    ?>
                    <a style="margin-top:0;margin-left:0;color: #00a2bf !important;font-family: 'Lato Bold Italic';font-size: 14px;float: left;cursor: pointer;"
                       class="invite" itopic="<?= $_REQUEST['itopic'] ?>" uid="<?= $p_id; ?>"
                       pname="<?= ucfirst($username) ?>" type="<?= $_REQUEST['edit'] ?>"
                       m="<?= md5($_REQUEST['iid'] . "kyarata75"); ?>" iid="<?= $_REQUEST['iid'] ?>"
                       pid="<?= $_REQUEST['pid'] ?>">Invite</a><br>
                <?php
                } else {
                        if (in_array($utype,array('super','admin'))) {

                            $callback = 'onclick="alert(\'Administrator cannot follow professionals!\');"';

                        }
                        else $callback = 'type="'.(in_array($_REQUEST['uid'],$followers) ? "unfollow":"follow").'"';
                        if (in_array($_REQUEST['uid'],$followers)) $ftype = 'unfollow'; else $ftype = 'follow';
                        ?>
                        <a style="color: #00a2bf !important;font-family: 'Lato Bold Italic';font-size: 14px;float: left;cursor: pointer;" data-tooltip="Be the first to know <?=$username?> 's next Insight"
                           <?= $callback; ?> cid="<?= $_REQUEST['uid']; ?>"
                           uid="<?= $p_id; ?>"><?= ucfirst($ftype); ?></a>
                    <?php
                    if(count($followers) >= 10) {
                    ?>
                        <div style="color: #00a2bf !important;font-family: 'Lato Bold Italic';font-size: 14px;margin-top:1px;margin-left:3px;float:left;" class="foll_cnt"><?=count($followers);?></div>
                    <?php } ?>
                    <div style="clear:both"></div>
                    <?php

                }
                if (proHasInsights($p_id)) {
                    ?><a
                    href="account?tab=professionals&page=pro_detail&id=<?= $p_id; ?>&section=pro_industry_view&type=discussion"
                    class="hover"
                    style="color: #00a2bf !important;font-family: 'Lato Bold Italic';font-size: 14px;float: left;cursor: pointer;">
                        Insights</a> <?php }
                if (in_array($utype,array('super','admin'))) {
                    ?><div style="clear:both"></div>
                    <a
                            href="account?tab=professionals&page=pro_detail&id=<?= $p_id; ?>"
                            class="hover"
                            style="color: #00a2bf !important;font-family: 'Lato Bold Italic';font-size: 14px;float: left;cursor: pointer;">
                        Profile</a> <?php
                }

                        ?>
            <td>
                <?php
                if (!empty($user_data['profile'])) {
                    echo "<b><div style='margin-bottom:10px;'>Summary</b></div>";
                    echo '<p id="info" style="line-height:20px;margin-top:0 !IMPORTANT;margin-right:10px;">';
                    echo $user_data['profile'];
                }
                if (!empty($pdetails_res['data_value2'])) {
                    echo "<b><div style='margin-bottom:10px;'>Why you should listen to $username </div> </b>";
                    echo '<p id="info" style="line-height:20px;margin-top:0 !IMPORTANT;margin-right:10px;">';
                    $pdetails_res['data_value2'] = strip_tags($pdetails_res['data_value2'], "<strong><em>");
                    $pdetails_res['data_value2'] = html_entity_decode($pdetails_res['data_value2'], ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    echo $pdetails_res['data_value2'];
                    echo "</p>";
                }
                if (!empty($pind_res['data_value2'])) {
                    echo "<b><div style='margin-bottom:10px;'>Industry view </div></b>";
                    echo '<p id="info" style="line-height:20px;margin-top:0 !IMPORTANT;margin-right:10px;">';
                    $pind_res['data_value2'] = strip_tags($pind_res['data_value2'], "<strong><em>");
                    $pind_res['data_value2'] = html_entity_decode($pind_res['data_value2'], ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    echo $pind_res['data_value2'];
                    echo "</p>";
                }
                if (!empty($pmanage_res['data_value2'])) {
                    echo "<b><div style='margin-bottom:10px;'>Management view </div></b>";
                    echo '<p id="info" style="line-height:20px;margin-top:0 !IMPORTANT;margin-right:10px;">';
                    $pmanage_res['data_value2'] = strip_tags($pmanage_res['data_value2'], "<strong><em>");
                    $pmanage_res['data_value2'] = html_entity_decode($pmanage_res['data_value2'], ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    echo $pmanage_res['data_value2'];
                    echo "</p>";
                }
                ?>
                </p>
            </td>
        </tr>
    </table>

    <?php

    die();
}

if ($_REQUEST['type'] == 'eventEdit') {

    require_once ROOT . 'sites/all/modules/maenna_configuration/maenna_configuration.module';

    $repository = new InsightRepository();
    $insightModel = new InsightModel();

    $insight = $repository->findById($_REQUEST['eventid']);
    if (!$insight) {
        exit('Insight was not found');
    }
    // get milestones of service
    $companyService = new Clewed\Company\Service();
    $milestones = $companyService->getServiceMilestones($_REQUEST['eventid']);
    if (!$milestones) {
        $milestones = array();
    }

    if ($_REQUEST['display'] == 'true') {

        $sql1 = mysql_query("SELECT * FROM maenna_company_events_inv WHERE eventid = '" . $_REQUEST['eventid'] . "'");
        $sql2 = mysql_query("SELECT * FROM maenna_company_data WHERE data_type = 'events' AND data_value6 = '" . $_REQUEST['eventid'] . "'");
        ?>
        <style type="text/css">
            .qq-upload-success {
                padding: 0 0 0.2em 1.5em !important;
            }

            .qq-upload-button {
                position: relative;
                overflow: hidden;
                direction: ltr;
                background: #e3eef2;
                width: 97px;
                margin: 3px;
                height: 21px;
                text-align: center;
                margin: 0 !important;
                text-transform: lowercase;
            }

            .qq-upload-button:first-letter { text-transform: uppercase; }

            #editEventForm select {
                background: none repeat scroll 0 0 #FFFFFF !important;
                border: 1px solid #cccccc;
                clear: both;
                color: #8f9095;
                font-style: italic;
                font-weight: bold;
                display: table-cell;
                float: left;
                font-family: 'Lato Regular',sans-serif;
                margin: 10px 0 15px 3px;
                min-height: 26px;
                padding: 5px 10px 5px 0px;
                vertical-align: middle;
                width: 290px !important;
            }

            .milestone-data {
                margin-top: 5px;
            }

        </style>
        <?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>
        <script type="text/javascript">
            function deleteFiles(event_id, image_id) {
                if (confirm("Are you sure want to delete the file.") == true) {
                    $.ajax({
                        type: "POST",
                        url: "./themes/maennaco/includes/delete.php?type=professional_images",
                        data: "event_id=" + event_id + "&image_id=" + image_id + '&u=<?php echo $u?>&m=<?php echo $m;?>',
                        success: function (msg) {
                            if (msg == 1) {
                                $("ul li#image" + image_id).remove();
                            }
                        }
                    });
                }
            }

            function deleteLinks(event_id, link_id) {
                if (confirm("Are you sure want to delete the file.") == true) {
                    var event_id = event_id;
                    var link_id = link_id;
                    $.ajax({
                        type: "POST",
                        url: "./themes/maennaco/includes/delete.php?type=professional_links",
                        data: "event_id=" + event_id + "&link_id=" + link_id + '&u=<?php echo $u?>&m=<?php echo $m;?>',
                        success: function (msg) {
                            if (msg == 1) {
                                $("ul li#link" + link_id).remove();
                            }
                        }
                    });
                }
            }

            function addInput() {
                fields = 2;
                if (fields != 10) {
                    $('<table><tr><td>Name: <td><td><input placeholder="Add link name" class="input" id="name' + fields + '" name="name" style="height:32px !important;padding:0px 3px 0px 6px !important;border:solid 1px #D6D6D8;width:390px !important;font:15px/160% Verdana, sans-serif;"></td></tr><tr><td>Link: <td><td><input placeholder="Add url of link" class="input" id="link' + fields + '" name="link" style="height:32px !important;padding:0px 3px 0px 6px !important;border:solid 1px #D6D6D8;width:390px !important;font:15px/160% Verdana, sans-serif;"></td></tr></table>').appendTo('#text');
                    fields += 1;
                } else {
                    document.getElementById('text').innerHTML += "<br />Only 10 upload fields allowed.";
                    document.form.add.disabled = true;
                }
            }

            $(function () {
                $('#eventCost').blur(function () {
                    $(this).formatCurrency({roundToDecimalPlace: -2, eventOnDecimalsEntered: true});
                }).bind('decimalsEntered', function (e, cents) {
                    var errorMsg = 'Please do not enter any cents (.' + cents + ')';
                    alert(errorMsg);
                });
            });

            //Limit the num of characters in description filed
            function limitText(limitField, limitCount, limitNum) {
                //Show limit
                $('#limit').show();
                if (limitField.value.length > limitNum) {
                    limitField.value = limitField.value.substring(0, limitNum);
                } else {
                    limitCount.value = limitNum - limitField.value.length;
                }
            }
            function isNumberKey(evt) {
                var charCode = (evt.which) ? evt.which : event.keyCode;
                return !(charCode != 46 && charCode > 31 && (charCode < 48 || charCode > 57));
            }

            $('#addMilestone').click(function () {
                var durations = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 23, 25, 27, 30];
                var singleMilestoneInputs = '<div class="milestone-data"><select style="margin: 0; padding: 6px; width: 150px !important; height: 25px !important; box-sizing: content-box; font-size: 14px !important;">' +
                    '<option value="" selected="selected">Milestone Deliverable</option>';
                for (var i in durations) {
                    singleMilestoneInputs += '<option value="' + durations[i] + '">In ' + durations[i] + ' days</option>';
                }
                singleMilestoneInputs += '</select>' +
                    '<input type="text" placeholder="Deliverable description" style="margin: 0 0 0 5px; width: 271px !important; height: 37px !important;"></div>';
                $('#milestones').append(singleMilestoneInputs);
            });
        </script>
        <div style="clear:both;"></div>

        <form action="" method="post" name="editEvent" id="editEventForm">
            <div><strong><?= $insight->isGroupInsight() ? 'Insight topic:' : 'I will help you with:'; ?></strong></div>
            <textarea class="input" id="eventType" maxlength="120" placeholder="Insight topic (max 120 characters)"
                      data-maxsize="120" name="eventType"
                      style="height:30px;width:440px !important; font:15px/160% Verdana, sans-serif"
                      cols="20"><?= htmlspecialchars($insight->title); ?></textarea>

            <div style="display: flex;padding-bottom: 10px;">
                <!-- Location section -->
                <div style="width: 230px;">
                    <div><strong>Clewed call in:</strong></div>
                    <textarea class="input" id="eventLoc" name="eventLoc" placeholder="WHERE?"
                              style="margin: 5px 0 0 0;height: 30px;width: 216px !important;font:15px/160% Verdana, sans-serif;"
                              cols="60"><?= htmlspecialchars($insight->location); ?></textarea>
                </div>

                <div style="width: 200px; margin-left: 10px;">
                    <div><strong><?= $insight->isGroupInsight() ? 'When?' : 'Duration'; ?></strong></div>
                    <input type='<?= $insight->isGroupInsight() ? 'text' : 'hidden' ?>' id="date" readonly="readonly" name="date"
                           style="width: 200px !important;height: 42px !important;margin-top: 5px;font: 15px/160% Verdana, sans-serif;"
                           class='datepicker ins_picker' creator="<?= $insight->postedby; ?>" placeholder="WHEN?"
                           value="<?= date("m-d-Y H:i", $insight->datetime) ?>"/>
                    <?php $service_duration = array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 23, 25, 27, 30); ?>
                    <select class="discuss" id="eventEditServiceDuration" name="eventEditServiceDuration" style="margin: 5px 0 0 0;padding: 6px;height: 29px;box-sizing: content-box;width: 200px !important;font-size: 14px !important;<?= $insight->isGroupInsight() ? 'display:none;' : 'display:inline-block;'; ?>">
                        <option value="">Choose Duration</option>
                        <?php foreach ($service_duration as $duration) : ?>
                            <option value="<?= $duration; ?>" <?= $insight->duration == $duration ? 'selected="selected"' : ''; ?>>In <?= $duration; ?> days</option>
                        <?php endforeach; ?>
                    </select>
                </div>
            </div>

            <!-- Target audience -->
            <div>
                <div <?= $insight->isGroupInsight() ? '' : 'style="display:none;"'; ?>><strong>Target audience</strong></div>
                <textarea class="input" id="eventDesc" placeholder="Agenda" name="eventDesc" cols="60"
                          style="font:15px/160% Verdana, sans-serif;width:440px !important;height:113px;<?= $insight->isGroupInsight() ? '' : 'display:none;'; ?>"><?= htmlspecialchars($insight->description); ?></textarea>
            </div>

            <!-- Why attend -->
            <div>
                <div><strong><?= $insight->isGroupInsight() ? 'Insight description:' : 'What will buyers get from this service:'; ?></strong></div>
                <textarea class="input" id="whyattend" placeholder="<?= $insight->isGroupInsight() ? 'Insight description:' : 'Service description:'; ?>" name="whyattend" cols="60"
                          style="font:15px/160% Verdana, sans-serif;width:440px !important;height:150px;"><?= htmlspecialchars($insight->whyattend); ?></textarea>
            </div>

            <?php if (!$insight->isGroupInsight()) : ?>
            <!-- Milestones -->
            <div id="milestones">
                <div style="display: inline-block;"><strong>Milestones</strong></div>
                <div id="addMilestone" style="width: 105px;color: #00A3BF;float: right; display: inline-block;cursor: pointer;">Add Milestone</div>
                <?php foreach ($milestones as $milestone) : ?>
                <div id="ms_id<?= $milestone['id'] ?>" class="milestone-data">
                    <select style="margin: 0; padding: 6px; width: 150px !important; height: 25px !important; box-sizing: content-box;font-size: 15px !important;font-style: normal;font-family: 'Lato Light';">
                        <option value="">Milestone deliverable</option>
                        <?php foreach ($service_duration as $duration) : ?>
                        <option value="<?= $duration; ?>" <?= $duration == $milestone['duration'] ? 'selected="selected"' : ''; ?>>In <?= $duration; ?> days</option>
                        <?php endforeach; ?>
                    </select>
                    <input type="text" value="<?= $milestone['description']; ?>" placeholder="Deliverable description" style="margin: 0 0 0 5px; width: 271px !important; height: 37px !important;font-size: 15px !important;font-style: normal;font-family: 'Lato Light';">
                </div>
                <?php endforeach; ?>
            </div>
            <?php endif; ?>

            <?php if (!$insight->isGroupInsight()) : ?>
            <div style="display: inline-block;">
                <div><strong>Buyer requirement:</strong></div>
                <textarea class="input" id="buyer_requirement" placeholder="Describe buyer requirement" name="buyer_requirement" cols="60"
                          style="font:15px/160% Verdana, sans-serif;width:450px !important;height:140px;"><?= htmlspecialchars($insight->buyer_requirement); ?></textarea>
            </div>
            <?php endif; ?>
                <div><strong>Cost:</strong></div>
            <input type='text' class="input" id="eventCost" name="eventCost" placeholder="COST"
                   style="height:30px; width:440px !important; font:15px/160% Verdana, sans-serif; margin-top:5px;margin-bottom:20px;"
                   value="<?= $insight->cost ?>"/>

            <?php
            $discount = $insightDiscountModel->getInsightDiscount($_REQUEST['eventid']);
            if ($discount instanceof \Clewed\Insights\Discount) :
                ?>
            <div style="display: flex;">
                <div>
                    <div><strong>Discount:</strong></div>
                    <label><input type="checkbox" name="use_discount" value="1" checked/>Enabled</label>
                    <br/>
                    <label><input type="checkbox" name="approve_discount" value="1"<?= $discount->approved ? ' checked' : '' ?>/>Approved</label>
                </div>
                <div style="margin-left: 10px;">
                    <div><strong>Rate:</strong></div>
                    <label><input type="text" name="rate" value="<?= $discount->rate ?>" style="width: 65px !important;margin: 5px 2px 0 0;"/>%</label>
                </div>
                <div style="margin-left: 10px;">
                    <div><strong>Code:</strong></div>
                    <label><input type="text" name="code" value="<?= $discount->code ?>" readonly style="margin-top: 5px;"/></label>
                </div>
            </div>
            <?php else : ?>
                <div><strong>Discount not enabled</strong></div><br/>
            <?php endif; ?>

            <div style="display: flex;margin-top: 20px;">
                <div>
                    <div><strong>Capacity:</strong></div>
                    <input type='text' class="input" id="eventCapacity" onkeypress="return isNumberKey(event)"
                           name="eventCapacity" placeholder="Capacity"
                           style="margin: 5px 0 0 0;width: 150px !important; font:15px/160% Verdana, sans-serif;"
                           value="<?= $insight->capacity; ?>"/>
                </div>
                <div style="margin-left: 10px;">
                    <div><strong>Categories:</strong></div>
                    <select class="discuss" id="eventTags" style="width:135px !important;height: 35px;margin: 5px 0 0 0;"
                            name="eventTags" placeholder="TAGS">
                        <?= OPTION_TAGS(_categories(), htmlspecialchars($insight->tags)); ?>
                    </select>
                </div>
                <div style="margin-left: 10px;">
                    <div><strong>Industry:</strong></div>
                    <select class="insight-editor-industry" id="eventIndustry" style="width:135px !important;height: 35px;margin: 5px 0 0 0;"
                            name="eventIndustry" placeholder="INDUSTRY">
                        <?php $options = _INDUSTRY();?>
                        <?= OPTION_TAGS(array_combine(array_keys($options),array_keys($options)), htmlspecialchars($insight->industry)); ?>
                    </select>
                </div>
            </div>

            <div style="clear:both"></div>

            <div class="insight-cover-image-uploader-wrapper">
                <div class="insight-cover-image-uploader-caption"><strong>Cover image:</strong></div>
                <div class="insight-cover-image-uploader">
                    <input type="button" value="Upload" />
                    <noscript>
                        <p>Please enable JavaScript to use file uploader.</p>
                    </noscript>
                    <script type="text/javascript">
                        (function() {
                            var $uploader = $('.insight-cover-image-uploader');
                            if($uploader.length > 0) {
                                new qq.FileUploader({
                                    params: {
                                        pro_id: "<?php echo $insight->id;?>",
                                        itype: 'insight-cover-image'
                                    },
                                    multiple: false,
                                    element: $uploader[0],
                                    action: '/themes/maennaco/includes/file_upload.php',
                                    allowedExtensions: ["jpg"],
                                    onComplete: function(id, fileName, responseJSON){
                                        if(responseJSON.success) {
                                            var $c = $uploader.parent().find('.insight-cover-image'),
                                                $parent = $c.parent(),
                                                $img = $c.find('img'),
                                                ratio = 191/135,
                                                width = 600;

                                            if($img.length > 0)
                                                $img.remove();

                                            $c.append('<img />');
                                            $c.find('img')
                                                .attr('src', responseJSON.url + '?' + (new Date().toTimeString()))
                                                .css('width', '100%')
                                                .css('height', '100%')
                                                .Jcrop({
                                                    aspectRatio: ratio,
                                                    resizable: true,
                                                    minSize: [135, 135 * ratio],
                                                    maxSize: [600, 600],
                                                    setSelect: [0, 0, 135, 135 * ratio],
                                                    onChange: function(c) {
                                                        $c.find('.insight-cover-image-crop-btn')
                                                            .data('x', c.x)
                                                            .data('y', c.y)
                                                            .data('w', c.w)
                                                            .data('h', c.h)
                                                    }
                                                });

                                            $c.append('<a class="insight-cover-image-crop-btn">crop</a>');
                                            $c.find('.insight-cover-image-crop-btn').click(function(){
                                                var $this = $(this),
                                                    x = $this.data('x'),
                                                    y = $this.data('y'),
                                                    w = $this.data('w'),
                                                    h = $this.data('h'),
                                                    progress = $this.data('progress');

                                                if(w == 0 || h == 0)
                                                    return alert('Please, select the image area to crop');

                                                if(progress)
                                                    return false;

                                                $(this).text('cropping...');
                                                $(this).data('progress', 1);

                                                $.ajax({
                                                    url: '/themes/maennaco/includes/cropper.php',
                                                    type: 'post',
                                                    data: {
                                                        x: x,
                                                        y: y,
                                                        w: w,
                                                        h: h,
                                                        relativeWidth: width,
                                                        type: 'insight-cover-image',
                                                        insight: '<?php echo $insight->id;?>',
                                                        time: <?php echo $time = time();?>,
                                                        m: '<?php echo md5('cropper.php:' . $time . ':' . $insight->id . ':kyarata75')?>'
                                                    },
                                                    dataType: 'json',
                                                    success: function(r) {
                                                        if(r.success) {
                                                            $c.dialog("close");
                                                            $parent.append(
                                                                '<div class="insight-cover-image">' +
                                                                '   <img src="' + responseJSON.url + '?' + (new Date().toTimeString()) + '"/>' +
                                                                '</div>');
                                                        }
                                                    }
                                                });
                                            });

                                            $c.dialog({
                                                width: width,
                                                title: "Please, select the image area to crop",
                                                modal: true,
                                                autoOpen: true,
                                                closeOnEscape: false,
                                                close: function() {
                                                    $(this).dialog("close");
                                                }
                                            });

                                            $c.parent().find('.ui-dialog-titlebar-close').hide();

                                            if($uploader.parent().find('.qq-upload-success').length > 1)
                                                $uploader.parent().find('.qq-upload-success').first().remove();
                                        }

                                    },
                                    onSubmit: function(id, fileName){},
                                    fileTemplate: '<li>' +
                                        '<span class="qq-upload-file"></span>' +
                                        '<span class="qq-upload-spinner"></span>' +
                                        '<span class="qq-upload-size"></span>' + '<a class="qq-upload-remove"></span>'+
                                        '&nbsp;<a class="qq-upload-cancel" href="#"></a>' +
                                        '<span class="qq-upload-failed-text">Failed</span>' +
                                    '</li>'
                                });
                            }
                        }());
                    </script>
                </div>

                <?php $path = null;
                    if(is_readable(ROOT . 'sites/default/images/insights/original/' . $insight->id . '.jpg'))
                        $path = 'sites/default/images/insights/original/' . $insight->id . '.jpg';?>

                <div class="insight-cover-image <?php if(!$path) echo 'hidden';?>">
                    <?php if($path):?>
                        <img src="<?php echo $path . '?' . time();?>" />
                    <?php endif;?>
                </div>
            </div>
            <table>
                <tr>
                    <td style="width: 44px"><strong>Files:</strong></td>
                    <td>
                        <div id='evFileEdit' style='text-align:left;'>
                            <div id="file-uploader1" data-pro-id="<?php echo $insight->id;?>">
                                <noscript>
                                    <p>Please enable JavaScript to use file uploader.</p>
                                    <!-- or put a simple form for upload here -->
                                </noscript>

                            </div>
                        </div>
                    </td>
                </tr>
            </table>
            <ul>
                <?php
                //Getting images and links of event.
                $sql_images = mysql_query("SELECT * FROM wall_documents WHERE ref_id = '" . $_REQUEST['eventid'] . "'");
                while ($images = mysql_fetch_assoc($sql_images)) {
                    if ($images['document_name'] != '') {
                        list($time_stamp, $file_name) = explode('_', $images['document_name'], 2);
                        echo '<li id="image' . $images['d_id'] . '">'
                            . '<a target="_new" href="sites/default/files/events_tmp/' . $images['document_name'] . '">' . $file_name . '</a>&nbsp;&nbsp;<a onClick="return deleteFiles(\'' . $_REQUEST['eventid'] . '\', \'' . $images['d_id'] . '\')" href="javascript:void(0);">Remove</a></li>';
                    }
                }
                ?>
            </ul>

            <div class="insight-audio-preview-uploader-wrapper">
                <div class="insight-audio-preview-uploader-caption"><strong>Audio preview:</strong></div>
                <div class="insight-audio-preview-uploader">
                    <input type="button" value="Upload" />
                    <noscript>
                        <p>Please enable JavaScript to use file uploader.</p>
                    </noscript>
                    <script type="text/javascript">
                        (function() {
                            var $uploader = $('.insight-audio-preview-uploader');
                            if($uploader.length > 0) {
                                new qq.FileUploader({
                                    params: {
                                        pro_id: "<?php echo $insight->id;?>",
                                        itype: 'insight-audio-preview'
                                    },
                                    multiple: false,
                                    element: $uploader[0],
                                    action: '/themes/maennaco/includes/file_upload.php',
                                    allowedExtensions: ["mp3"],
                                    onComplete: function(id, fileName, responseJSON){
                                        if(responseJSON.success) {
                                            $uploader.parent().find('.insight-audio-preview').addClass('hidden');
                                            if($uploader.parent().find('.qq-upload-success').length > 1)
                                                $uploader.parent().find('.qq-upload-success').first().remove();
                                        }

                                    },
                                    onSubmit: function(id, fileName){},
                                    fileTemplate: '<li>' +
                                    '<span class="qq-upload-file"></span>' +
                                    '<span class="qq-upload-spinner"></span>' +
                                    '<span class="qq-upload-size"></span>' + '<a class="qq-upload-remove"></span>'+
                                    '&nbsp;<a class="qq-upload-cancel" href="#"></a>' +
                                    '<span class="qq-upload-failed-text">Failed</span>' +
                                    '</li>'
                                });
                            }
                        }());
                    </script>
                </div>

                <?php $fileName = 'insights/audio-previews/' . $insight->id . '.mp3';

                if(is_readable(ROOT . 'sites/default/files/events_tmp/' . $fileName)):
                    $db = \Clewed\Db::get_instance();
                    $hash = sha1('hash' . time() . $fileName);
                    $db->run('
                        INSERT INTO `audio_files` (`hash`, `file`)
                        VALUES (:hash, :file)',
                        array(
                            ':hash' => $hash,
                            ':file' => $fileName
                        )
                    );
                    $audioPreviewPath = '/' . $hash . '.mp3';
                endif; ?>

                <div class="insight-audio-preview <?php if(!$audioPreviewPath) echo 'hidden';?>">
                    <?php if($audioPreviewPath):?>
                        <?php $playerNo = md5($audioPreviewPath . time());?>
                        <audio preload="auto" controls="controls" src="<?=$audioPreviewPath;?>">Sorry, your browser is too old and is not supported anymore. Please update it and try again.</audio>
<!--                        <script type="text/javascript" src="/js/swfobject.js"></script>
                        <script type="text/javascript">
                            swfobject.registerObject("player","9.0.0");
                            var flashvars = {
                                height: "20",
                                width: "200",
                                file: "<?php echo $audioPreviewPath;?>",
                                backcolor: "0x999999",
                                frontcolor: "0xFFFFFF",
                                overstretch: "none",
                                usefullscreen: "false",
                                enablejs: "true",
                                javascriptid: "player"
                            };
                            var params = { };
                            var attributes = { };
                            swfobject.embedSWF("/mediaplayer.swf","audio_player<?= $playerNo ?>","200","20","9.0.0","",flashvars,params,attributes);
                        </script>
                        <div id="audio_player<?= $playerNo ?>"></div>-->

                    <?php endif;?>
                </div>
                <?php if(!empty($audioPreviewPath)):?>
                    <div data-m="<?php echo md5($insight->id . ':kyarata75')?>"
                         data-id="<?php echo $insight->id;?>"
                         class="insight-audio-preview-remove"><a>Remove</a></div>

                    <script type="text/javascript">
                        $(document).ready(function(){
                            $('.insight-audio-preview-remove a').click(function() {
                                var $el = $(this).parent(),
                                    id = $el.data('id'),
                                    m = $el.data('m');

                                if(confirm('Are you sure you want to delete the audio preview file?'))
                                    $.ajax({
                                        type: 'get',
                                        url: './themes/maennaco/includes/delete.php?type=insight-audio-preview&id=' + id + '&m=' + m,
                                        success: function (msg) {
                                            if('error'!==msg) {
                                                $el.parent().find('.insight-audio-preview').addClass('hidden');
                                                $el.parent().addClass('hidden');
                                            }
                                        }
                                    });
                            });
                        });
                    </script>

                <?php endif;?>
            </div>

            <strong>Links:</strong>
            <?php
            //Getting images and links of event.
            $sql_links = mysql_query("SELECT * FROM  maenna_professional_links WHERE professional_id = '" . $_REQUEST['eventid'] . "'");
            while ($links = mysql_fetch_assoc($sql_links)) {
                echo "<ul>";
                if ($links['links'] != '') {
                    echo '<li id="link' . $links['id'] . '">name :' . $links['name'] . '<br>Link :' . $links['links'] . '&nbsp;&nbsp;<br><a onClick="return deleteLinks(\'' . $_REQUEST['eventid'] . '\', \'' . $links['id'] . '\')" href="javascript:void(0);">Remove</a></li>';
                }
                echo "</ul>";
            }
            ?><br/>
            <input type="hidden" name="fileTitleEdit" id="fileTitleEdit" placeholder="File title"
                   style="height:30px; width:285px !important; font:15px/160% Verdana, sans-serif"/>

            <div id="text">
                <table>
                    <tr>
                        <td>Name:</td>
                        <td><input class="input" id="name" placeholder="Add link name" name="name"
                                   style="height:32px !important;padding:0 3px 0 6px !important;border:solid 1px #D6D6D8;width:390px !important;font:15px/160% Verdana, sans-serif">
                        </td>
                    </tr>
                    <tr>
                        <td>Link:</td>
                        <td><input class="input" id="link" placeholder="Add url of link" name="link"
                                   style="height:32px !important;;padding:0 3px 0 6px !important;border:solid 1px #D6D6D8;width:390px !important;font:15px/160% Verdana, sans-serif">
                        </td>
                    </tr>
                </table>
            </div>
            <span style="float:left;margin-top:-19px;margin-left:54px;margin-bottom:10px;">
                <a style="font-family:LatoRegular;color:#00A3BF;" href="javascript:void(0);" onClick="addInput()">
                    Add more links
                </a>
            </span>
        </form>
        <?php
        die;
    } else {

        $ok = 'true';

        $insight->populateFromRequest($_REQUEST);
        $insight->spots = (int)$_REQUEST['capacity'];

        $insightId = (int) $_REQUEST['eventid'];
        $discountModel = new \Clewed\Insights\DiscountModel();
        $discount = $discountModel->getInsightDiscount($insightId);
        if ($discount) {
            $discount->approved = (bool) $_REQUEST['approve_discount'];
            $discount->rate = (int) $_REQUEST['discount_rate'];
            $discountModel->save($discount);
        }

        $insightModel->save($insight);

        if (array_key_exists('milestones', $_REQUEST)) {
            // update milestones
            $companyService = new Clewed\Company\Service();
            $companyService->updateServiceMilestones($insightId, $_REQUEST['milestones']);
        }

        //Inserting Files
        if (!empty($_POST['files'])) {
            foreach ($_POST['files'] as $key => $value) {
                mysql_query("INSERT INTO wall_documents (ref_id,document_name) VALUES ('" . $_REQUEST['eventid'] . "', '$value[timestamp]_$value[path]')") or $ok = 'false';
            }
        }
        //Inserting Links
        if (!empty($_POST['links']) OR !empty($_POST['names'])) {
            $i = 0;
            foreach ($_POST['links'] as $key => $value) {
                if ($value != '') {
                    mysql_query("INSERT INTO maenna_professional_links (professional_id,links, name) VALUES ('" . $_REQUEST['eventid'] . "', '" . $value . "', '" . $_POST['names'][$key] . "')") or $ok = 'false';
                }
                $i++;
            }
        }
        $umail = mysql_query("SELECT mail FROM users WHERE uid = '" . $_REQUEST['uid'] . "'");
        $umail = mysql_fetch_array($umail);
        if ($_REQUEST['files']) {
            foreach ($_REQUEST['files'] as $value) {
                $fileName = $value['timestamp'] . "_" . $value['path'];
                if ($value['title'] == '') $value['title'] = substr($value['path'], 0, strrpos($value['path'], '.'));
                $sql = "insert into maenna_company_data (companyid, access, data_type,data_value, data_value2,data_value6, editorid )
                        values('" . $_REQUEST['cid'] . "','" . time() . "','events','" . $value['title'] . "','" . $fileName . "','" . $_REQUEST['eventid'] . "'," . $_REQUEST['uid'] . ")" or $ok = 'false';
                mysql_query($sql);
            }
        }
        $cmpname = mysql_query("SELECT projname FROM maenna_company WHERE companyid = '" . $_REQUEST['cid'] . "'");
        $cmpname = mysql_fetch_array($cmpname);
        if ($_REQUEST['notif'] == 'true') {
            $q = mysql_query("SELECT mail FROM users u  JOIN maenna_company_events_inv mce ON u.uid = mce.uid WHERE eventid = '" . $_REQUEST['eventid'] . "' AND mce.status = 'confirmed' ");
            while ($inv = mysql_fetch_array($q)) {
                $to = $inv['mail'];
                $subject = ucwords($cmpname['projname']) . " Discussion on " . ucfirst(date("M j ", strtotime($_REQUEST['datetime'])));
                $message .= ucwords($cmpname['projname']) . ' Discussion on ' . ucfirst(date("M j ", strtotime($_REQUEST['datetime']))) . '  is updated for additional files, topic or participants. Please go to http://www.maennaco.com/account?tab=companies&page=company_detail&id=' . $_REQUEST['cid'] . '&mtab=advice  to review <br>';
                $headers = "From:" . $umail['mail'] . "\r\n";
                $headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";
                mail($to, $subject, $message, $headers);
            }
        }
        die($ok);
    }
}


    function getProId($id) {
    if (empty($id)) return 'invalid id';
    $sql     = "select users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname from users_roles, maenna_people where users_roles.uid = '" . $id . "' and maenna_people.pid = '" . $id . "' limit 1";
    $result1 = mysql_query($sql);
    $Row     = mysql_fetch_assoc($result1);
    if (empty($Row)) return "invalid user role setting - $id";
    $rid       = $Row['rid'];
    $firstname = ucwords($Row['firstname']);
    if (in_array($rid, array(6, 10))) {
        $output = "${firstname}MAEADM" . sprintf("%05s", $id + 100);
    } else {
        $output = "${firstname}MAE"; // . sprintf("%04s", $id +100);
    }

    return $output;
}

if ($_REQUEST['type'] == 'addEvent') {

    $uid = (int)$_REQUEST['uid'];

    // set default value of capacity
    if (!(int)$_REQUEST['capacity']) {
        $_REQUEST['capacity'] = 100;
    }

    $insight = new \Clewed\Insights\InsightEntity();
    $insight->populateFromRequest($_REQUEST);
    $insight->type           = $_REQUEST['insightType'];
    $insight->username       = $_REQUEST['u'];
    $insight->created        = time();
    $insight->postedby       = $uid;
    $insight->spots          = (int)$_REQUEST['capacity'];
    $insight->approve_status = 0;
    $insight->views          = 0;

    $insightModel = new InsightModel();
    $insightModel->save($insight);
    $eventId = $insight->id;
    // save milestones of service
    if ($_REQUEST['insightType'] == '1' && array_key_exists('milestones', $_REQUEST)) {
        $companyService = new Clewed\Company\Service();
        $companyService->createServiceMilestones($eventId, $_REQUEST['milestones']);
    }

    if ($_REQUEST['use_discount']) {
        $insightDiscountModel->save($insightDiscountModel->create($eventId, $_REQUEST['rate'], $_REQUEST['code']));
    }
    $dbConn = \Clewed\Db::get_instance();
    $dbConn->run(
        'INSERT INTO `notifications` (`action`, `state`, `author`, `item`, `created`)
            VALUES ("offer_created", 1, :author, :event , NOW())',
        array(
            ':author' => $uid,
            ':event'  => $eventId,
        )
    );

    //Inserting Files
    if (!empty($_SESSION['files'])) {
        foreach ($_POST['files'] as $key => $value) {
            mysql_query("INSERT INTO wall_documents (ref_id,document_name) VALUES ('" . $eventId . "', '$value[timestamp]_$value[path]')");
        }
    }
    //Inserting Links
    if (!empty($_SESSION['links']) OR !empty($_SESSION['names'])) {
        $i = 0;
        foreach ($_SESSION['links'] as $key => $value) {
            if ($value != '') {
                mysql_query("INSERT INTO maenna_professional_links (professional_id,links, name) VALUES ('" . $eventId . "', '" . $value . "', '" . $_SESSION['names'][$key] . "')");
            }
            $i++;
        }
    }

    unset($_SESSION['files']);
    unset($_SESSION['links']);
    unset($_SESSION['names']);

    $umail = mysql_query("SELECT mail FROM users WHERE uid = '" . $uid . "'");
    $umail = mysql_fetch_array($umail);

    $userData = mysql_query("SELECT * FROM maenna_people WHERE pid = " . $uid);
    $userData = mysql_fetch_array($userData);

    if ($userData['username_type'] == 1) {
        $proid = ucfirst($userData['firstname']);
    } else $proid = ucfirst($userData['firstname']) . ' ' . ucfirst($userData['lastname']);

    $date        = $_REQUEST['date'];
    $bcc         = '';
    $amail = mysql_query("SELECT mail FROM maenna_connections LEFT JOIN users ON users.uid = maenna_connections.assignee_uid WHERE conntype = 'admin' AND target_uid = " . ((int) $_REQUEST['uid']) . " AND maenna_connections.status = 'active'");
    $amail = mysql_fetch_array($amail);
    if ($amail) $bcc = $amail['mail'];

    $content = <<< END
A new insight was created by $proid. Details:
Title: $insight->title
Location: $insight->location
Date: $date
Description: $insight->description
END;

    $headers = 'From: noreply@clewed.com' . "\r\n" .
        "Bcc: $bcc" . "\r\n";

    mail('insights@clewed.com', 'New insight scheduled', $content, $headers);

    echo 'OK';
}

if ($_REQUEST['type'] == 'confirmAtt') {

    if ($_REQUEST['status'] == 'confirmed') {

        $dbl = mysql_query("SELECT *
				    FROM maenna_company_events ev1
				    WHERE eventid ='" . $_REQUEST['eventid'] . "'
				    AND EXISTS (

				    SELECT *
				    FROM maenna_company_events ev2
				    JOIN maenna_company_events_inv inv1 ON ev2.eventid = inv1.eventid
				    WHERE inv1.uid ='" . $_REQUEST['uid'] . "'
				    AND inv1.status =  'confirmed'
				    AND DATE( FROM_UNIXTIME( ev1.datetime ) ) = DATE( FROM_UNIXTIME( ev2.datetime ) )
                                    AND ABS(time_to_sec(timediff(FROM_UNIXTIME(ev1.datetime),FROM_UNIXTIME(ev2.datetime))) / 3600) < 2
				    )");

        if (mysql_num_rows($dbl) > 0) die("overlap");
    }

    mysql_query("UPDATE maenna_company_events_inv SET status = '" . $_REQUEST['status'] . "' WHERE eventid = '" . $_REQUEST['eventid'] . "' AND uid = '" . $_REQUEST['uid'] . "' ");

    if ($_REQUEST['status'] == 'confirmed') {

        $sql  = mysql_query("SELECT * FROM maenna_company_events mce JOIN users u ON mce.companyid = u.uid WHERE mce.eventid = '" . $_REQUEST['eventid'] . "'");
        $sql1 = mysql_query("SELECT mail FROM users WHERE uid = '" . $_REQUEST['uid'] . "'");
        $sql2 = mysql_query("SELECT * FROM maenna_company_events_inv mcei JOIN users u ON mcei.uid = u.uid WHERE eventid = '" . $_REQUEST['eventid'] . "'");

        $sqlmail = mysql_query("SELECT mail FROM users WHERE uid = '" . $_REQUEST['cid'] . "'");
        $cmpmail = mysql_fetch_array($sqlmail);

        $event = mysql_fetch_array($sql);
        $umail = mysql_fetch_array($sql1);

        $to      = $cmpmail['mail'];
        $subject = '[Attendance confirmed] ' . ucwords($event['title']) . ' !!!';
        $message .= 'User ' . $umail['mail'] . ' has confirmed acceptance!!!';
        $message .= '<br><br>';
        $message .= 'Event: ' . ucwords($event['title']) . '<br><br>';
        $message .= date("l, M j, Y g:i A T ", $event['datetime']) . '<br><br>';
        $message .= $event['location'] . '<br><br>';
        $message .= 'Agenda: <br>' . $event['description'] . '<br><br>';
        $message .= 'Attendees/Invitees <br>';

        while ($allinv = mysql_fetch_array($sql2)) {

            $message .= $allinv['mail'] . '&nbsp;&nbsp; - &nbsp;&nbsp;' . $allinv[3] . '<br>';
        }
        $headers = "From:" . $umail['mail'] . "\r\n";
        $headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";

        mail($to, $subject, $message, $headers);
    }

    switch ($_REQUEST['status']) {

        case "confirmed" :
            die("YOUR STATUS: <a class = \"rsvp\" name=\"confirmatt\" eventid = \"" . $_REQUEST['eventid'] . "\" style=\"color:#6792D0 !important; cursor:pointer;\" >CONFIRMED</a>");
            break;
        case "maybe" :
            die("YOUR STATUS: <a class = \"rsvp \" name=\"confirmatt\" eventid = \"" . $_REQUEST['eventid'] . "\" style=\color:#6792D0 !important; cursor:pointer; \" >TENTATIVE</a>");
            break;
        case "declined" :
            die("YOUR STATUS: <a class = \"rsvp \" name=\"confirmatt\" eventid = \"" . $_REQUEST['eventid'] . "\" style=\"color:#6792D0 !important; cursor:pointer;\" >DECLINED</a>");
            break;
    }
}

if ($_REQUEST['type'] == 'teamStats') {

    function getProId($id) {
        if (empty($id)) return 'invalid id';
        $sql    = "select users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname from users_roles, maenna_people where users_roles.uid = '" . $id . "' and maenna_people.pid = '" . $id . "' limit 1";
        $result = mysql_query($sql);
        $Row    = mysql_fetch_assoc($result);
        if (empty($Row)) return "invalid user role setting - $id";
        $rid       = $Row['rid'];
        $firstname = ucwords($Row['firstname']);
        if (in_array($rid, array(6, 10))) {
            $output = "${firstname}MAEADM" . sprintf("%05s", $id + 100);
        } else {
            $output = "${firstname}MAE";
        }

        return $output;
    }

    function userRoleId($uid = null) {
        if (empty($uid)) return '';
        $sql    = "select rid from users_roles where uid = '" . $uid . "' ";
        $result = mysql_query($sql);
        while ($Row = mysql_fetch_array($result)) {
            return $Row['rid'];
        }
        return '';
    }

    function getRole($id) {
        $result = mysql_query("SELECT r.name as name FROM users_roles ur JOIN role r on ur.rid = r.rid WHERE ur.uid = '" . $id . "'");
        while ($Row = mysql_fetch_array($result)) {
            return $Row['name'];
        }
        return '';
    }

    function Com_conns($id) {
        $Conns = array(
            'Admin'       => array(),
            'Follower'    => array(),
            'Follow'      => array(),
            'Advisor'     => array(),
            'Watchlist'   => array(),
            'Inwatchlist' => array(),
            'Visible'     => array(), //
            'Propose'     => array()
        );
        if (empty($id)) return $Conns;
        $sql    = "select  *
                from    maenna_connections
                where   status = 'active' and
                        ((assignee_uid = '" . $id . "') or (target_uid = '" . $id . "'))
                order by conntype
                ";
        $result = mysql_query($sql);
        while ($Row = mysql_fetch_assoc($result)) {

            $conntype = ucwords($Row['conntype']);
            if ($Row['conntype'] == 'follow') {
                if ($Row['target_uid'] == $id) {
                    $Conns['Follower'][] = $Row;
                } else $Conns['Follow'][] = $Row;
            } elseif ($Row['conntype'] == 'watchlist') {
                if ($Row['target_uid'] == $id) {
                    $Conns['Watchlist'][] = $Row;
                } // target com is the one being watched.
                else $Conns['Inwatchlist'] = $Row;
            } elseif ($Row['conntype'] == 'visible') {
                if ($Row['target_uid'] == $id) $Conns['Visible'][] = $Row; // target com is the one being watched.
            } else {
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
    $ctype          = $_REQUEST['ctype'];
    $LIST           = '';
    $box_content    = '';
    if (empty($ctype) || $ctype == 'advisor') {
        $advisor_active = 'active';
        $box_title      = "ADVISORS";
        foreach ($Conns['Advisor'] as $Pro) {
            $pro_uid = $Pro['assignee_uid'];
            //Get user gender
            $q1         = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
            $gender_tmp = mysql_fetch_array($q1);
            $gender     = $gender_tmp['gender'];
            //Check if user have a profile picture
            if (file_exists('sites/default/images/profiles/50x50/' . $pro_uid . '.jpg')) {
                $avatar = 'sites/default/images/profiles/50x50/' . $pro_uid . '.jpg';
            } else {
                if ($gender == 'm' || $gender == '') {
                    $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                } else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            }
            $pro_maeid = getProId($pro_uid);
            $rid       = userRoleId($pro_uid);
            $pro_type  = getRole($pro_uid);

            if ($_REQUEST['perm'] == 'read') {
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:'Lato Regular'; text-transform:uppercase;margin-top:20px;\">$pro_maeid, $pro_type</p></div>";
            } else {
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1' target='_blank'> $pro_maeid, $pro_type</a></div>";
            }
        }
    } elseif ($ctype == 'following') {

        $following_active = 'active';
        $box_title        = "MANAGEMENT";
        foreach ($Conns['Client'] as $Pro) {
            $pro_uid = $Pro['assignee_uid'];
            echo $pro_uid;
            //Get user gender
            $q1         = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
            $gender_tmp = mysql_fetch_array($q1);
            $gender     = $gender_tmp['gender'];
            //Check if user have a profile picture
            if (file_exists('sites/default/images/profiles/50x50/' . $pro_uid . '.jpg')) {
                $avatar = 'sites/default/images/profiles/50x50/' . $pro_uid . '.jpg';
            } else {
                if ($gender == 'm' || $gender == '') {
                    $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                } else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            }
            $pro_maeid = getProId($pro_uid);
            $rid       = userRoleId($pro_uid);
            $pro_type  = getRole($pro_uid);

            if ($_REQUEST['perm'] == 'read') {
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:'Lato Regular'; text-transform:uppercase;margin-top:20px;\">$pro_maeid, $pro_type</p></div>";
            } else {
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1' target='_blank'> $pro_maeid, $pro_type</a></div>";
            }
            $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\"><a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1' target='_blank'>$pro_maeid, $pro_type</a></div>";
        }
    } elseif ($ctype == 'connections') {
        $conn_active = 'active';
        $box_title   = "CONNECTED";
        foreach ($Conns['Visible'] as $Pro) {
            $pro_uid = $Pro['assignee_uid'];
            echo $pro_uid;
            //Get user gender
            $q1         = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
            $gender_tmp = mysql_fetch_array($q1);
            $gender     = $gender_tmp['gender'];
            //Check if user have a profile picture
            if (file_exists('sites/default/images/profiles/50x50/' . $pro_uid . '.jpg')) {
                $avatar = 'sites/default/images/profiles/50x50/' . $pro_uid . '.jpg';
            } else {
                if ($gender == 'm' || $gender == '') {
                    $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                } else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            }
            $pro_maeid = getProId($pro_uid);
            $rid       = userRoleId($pro_uid);
            $pro_type  = getRole($pro_uid);

            if ($_REQUEST['perm'] == 'read') {
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:'Lato Regular'; text-transform:uppercase;margin-top:20px;\">$pro_maeid, $pro_type</p></div>";
            } else {
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1' target='_blank'> $pro_maeid, $pro_type</a></div>";
            }
        }
    }

    if ($box_content == '') $box_content = 'No users';
    die($box_content);
}

if (checkValues($_REQUEST['value'])) {

    function getProId($id) {
        if (empty($id)) return 'invalid id';
        $sql  = mysql_query("SELECT rid FROM users_roles WHERE uid = '" . $id . "' LIMIT 1 ");
        $ridn = mysql_fetch_array($sql);
        if ($ridn['rid'] == '3') {

            $sql = "select users_roles.*, maenna_company.projname from users_roles, maenna_company where users_roles.uid = '" . $id . "' and maenna_company.companyid = '" . $id . "' limit 1";
        } else {

            $sql = "select users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname from users_roles, maenna_people where users_roles.uid = '" . $id . "' and maenna_people.pid = '" . $id . "' limit 1";
        }
        $result    = mysql_query($sql);
        $Row       = mysql_fetch_assoc($result);
        $rid       = $ridn['rid'];
        $firstname = ucwords($Row['firstname']);
        if (in_array($rid, array(6, 10))) {
            $output = "MAENNA";
        } elseif ($rid == "3") {
            $output = ucwords($Row['projname']);
        } else {
            $output = "${firstname}MAE"; // . sprintf("%04s", $id +100);
        }
        return $output;
    }

    $res        = mysql_query("SELECT d_id FROM wall_documents where document_name = '" . $_REQUEST['doc'] . "'");
    $red        = mysql_fetch_array($res);
    $d_id       = $red['d_id'];
    $editorname = getProId($_REQUEST['uid']);

    if ($_REQUEST['type'] == 'eventcom') $d_id = $_REQUEST['eventid'];

    if (md5($_REQUEST['u'] . "kyarata75") === $_REQUEST['m']) {
        mysql_query("INSERT INTO wall_posts (post,f_name,user,date_created,document_id) VALUES('" . checkValues($_REQUEST['value']) . "','" . $editorname . "','" . $_REQUEST['u'] . "','" . strtotime(date("Y-m-d H:i:s")) . "','" . $d_id . "')");
    }

    $result = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS TimeSpent FROM wall_posts WHERE document_id = '" . $d_id . "' order by p_id desc limit 1");
} elseif ($_REQUEST['show_more_post']) // more posting paging
{
    $next_records = $_REQUEST['show_more_post'] + 10;

    $result = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS TimeSpent FROM wall_posts WHERE document_id = " . $d_id . " order by p_id desc limit " . $_REQUEST['show_more_post'] . ", 10");

    $check_res = mysql_query("SELECT * FROM wall_posts WHERE document_id = " . $d_id . " order by p_id desc limit " . $next_records . ", 10");

    $show_more_button = 0; // button in the end

    $check_result = mysql_num_rows(@$check_res);
    if ($check_result > 0) {
        $show_more_button = 1;
    }
} else {

    $show_more_button = 1;
    $result           = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS TimeSpent FROM wall_posts WHERE document_id = " . $d_id . " order by p_id desc limit 0,10");
}

if ($result) {
    while ($row = mysql_fetch_array($result)) {
        $comments = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS CommentTimeSpent FROM wall_posts_comments where post_id = " . $row['p_id'] . " order by c_id asc");        ?>
        <div class="friends_area" id="record-<?php echo $row['p_id'] ?>">
            <?php

            $crId = nameToId($row['user']);
            $avatar = getAvatarUrl($crId);

            echo "<img src=" . $avatar . " style=\"float:left; margin-top:13px; margin-right:5px; width:50px; height:50px;\">";

            ?>
            <label style="float:left;margin-top:9px;width:90%;" class="name">
			   <span style="color:#4169AF;">
					<b><?php if (ifAdmin($row['user'])) {
                            echo "MAENNA";
                        } else echo $row['f_name'];?>
                    </b>
					<span style='color:#666;font-style:italic;'>&nbsp;shares a new idea:</span>
			   </span>
                <em>&nbsp;<?php echo nl2br(replace_email($row['post'])); ?></em>
                <br clear="all"/>

                <span><?php echo ago($row['date_created']); ?></span>

                <?php

                if ($row['user'] == $_REQUEST['u'] || ifAdmin($user->name) || $row['user'] == $user->name) {
                    ?>
                    <a href="#" id="remove_id<?php echo $row['p_id'] ?>" style="float:none;"
                       alt="<?php echo md5($row['p_id'] . $row['user'] . "kyarata75") ?>" name="<?= $row['user']; ?>"
                       class="delete tool"> Remove</a>
                <?php } ?>

            </label>
            <br clear="all"/>

            <div id="CommentPosted<?php echo $row['p_id'] ?>">
                <?php
                $comment_num_row = mysql_num_rows(@$comments);
                if ($comment_num_row > 0) {
                    while ($rows = mysql_fetch_array($comments)) {
                        $days2     = floor($rows['date_created'] / (60 * 60 * 24));
                        $remainder = $rows['date_created'] % (60 * 60 * 24);
                        $hours     = floor($remainder / (60 * 60));
                        $remainder = $remainder % (60 * 60);
                        $minutes   = floor($remainder / 60);
                        $seconds   = $remainder % 60;
                        ?>
                        <div class="commentPanel" id="comment-<?php echo $rows['c_id']; ?>"
                             align="left" style='padding-top:6px;border-bottom:dotted 1px #CCC;'>
                            <?php
                            $crId = nameToId($rows['user']);
                            $uType = getUserType($crId);

                            if ($uType == 'people' || $uType == 'admin') {

                                //Get user gender
                                $q1         = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
                                $gender_tmp = mysql_fetch_array($q1);
                                $gender     = $gender_tmp['gender'];

                                if (file_exists('sites/default/images/profiles/50x50/' . $crId . '.jpg')) {
                                    $avatar = 'sites/default/images/profiles/50x50/' . $crId . '.jpg';
                                } else {
                                    if ($gender == 'm' || $gender == '') {
                                        $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                                    } else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                                }
                            } else if ($uType == 'company') {

//Get cmp_role
                                $q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $crId") or die(mysql_error());
                                $cmp_role_tmp = mysql_fetch_array($q1);
                                $cmp_role     = $cmp_role_tmp['company_type'];
                                //Check if user have a profile picture
                                if (file_exists('sites/default/images/company/50x50/' . $crId . '.jpg')) {
                                    $avatar = 'sites/default/images/company/50x50/' . $crId . '.jpg';
                                } else if ($cmp_role == 'service') {
                                    $avatar = ' /themes/maennaco/images/cmp-avatar-service.png';
                                } else $avatar = ' /themes/maennaco/images/cmp-avatar-product.png';
                            }

                            echo "<img src=" . $avatar . " style=\"float:left; margin-top:5px;margin-right:5px; width:35px; height:35px;\">";

                            ?>

                            <label style="width:85%;" class="postedComments">
							  <span style="color: #4169AF;"><b>
                                      <?php    if (ifAdmin($rows['user'])) {
                                          echo "MAENNA";
                                      } else {
                                          echo $rows['f_name'];
                                      }

                                      echo "</b><span style='color:#666;font-style:italic;font-size:10px;'>&nbsp;continues discussion:</span>&nbsp;";
                                      echo "<span style='color:#666;'>" . nl2br(replace_email($rows['comments'])) . "</span>";
                                      ?>

                                </span>

                                <br>
                                <?php ?>


                                <span
                                    style="display:inline-block;width:90px;margin-left:0px; color:#666666; font-size:11px">
						<?= ago($rows['date_created']) ?>
						</span>
                                <?php
                                if ($rows['user'] == $_REQUEST['u'] || ifAdmin($user->name) || $rows['user'] == $user->name) {
                                ?>
                                &nbsp;&nbsp;<a href="#" id="CID-<?php echo $rows['c_id']; ?>"
                                               alt="<?php echo md5($rows['c_id'] . $rows['user'] . "kyarata75") ?>"
                                               name="<?= $rows['user']; ?>" class="c_delete tool">Delete</a>
                            </label><?php
                        } ?>
                        </div>
                    <?php
                    } ?>
                <?php
                } ?>
            </div>
            <div class="commentBox" align="right"
                 id="commentBox-<?php echo $row['p_id']; ?>"
                 name="<?php echo $editorname; ?>"
                 alt="<?php echo md5($editorname . "kyarata75") ?>">
                <label id="record-<?php echo $row['p_id']; ?>" style="padding-top:0;">
                    <textarea class="commentMark"
                              rowid="<?php echo $row['p_id']; ?>"
                              id="commentMark-<?php echo $row['p_id']; ?>"
                              name="commentMark" cols="120" style="margin-top:0;"></textarea>
                </label>
                <br clear="all"/>

                <a class="tool clseCommentBox comentboxBtns-<?php echo $row['p_id']; ?>"
                   style="display: none">Cancel</a>&nbsp;&nbsp;&nbsp;
                <a id="SubmitComment" class="tool comment  comentboxBtns-<?php echo $row['p_id']; ?>"
                   style="display: none" comBox=>Submit</a>&nbsp;&nbsp;
            </div>
        </div>
    <?php
    }
}
