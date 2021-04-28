<?php
include('dbcon.php');
// error_reporting (E_ALL ^ E_NOTICE);

error_reporting(0);

if ($_REQUEST['type'] == 'add_invitation_code') {

    if ($_REQUEST['utype'] == 'companies') {
        $table = 'maenna_company';
        $id = 'companyid';
    } elseif ($_REQUEST['utype'] == 'professionals') {
        $table = 'maenna_people';
        $id = 'pid';
    }

    if (mysql_query("UPDATE $table SET referral_code = '" . mysql_real_escape_string($_REQUEST['ref_code']) . "' WHERE $id = " . (int) $_REQUEST['uid'] . " LIMIT 1")) die('success');
    else die("failure");
}

function nameToId($name)
{
    $q = mysql_query("SELECT uid FROM users WHERE name = '" . $name . "' LIMIT 1") or die(mysql_error());
    $r = mysql_fetch_array($q);
    return $r['uid'];
}

function getUserType($uid)
{
    $q = mysql_query("SELECT pid FROM maenna_people WHERE pid = '" . $uid . "' ");

    if (mysql_num_rows($q) > 0) {
        return 'people';
    } else {
        $q1 = mysql_query("SELECT companyid FROM maenna_company WHERE companyid = '" . $uid . "' ");
        if (mysql_num_rows($q1) > 0) return 'company';
        else return 'admin';
    }
}

function ifAdmin($uname)
{
    $result = mysql_query("SELECT *  FROM users u WHERE u.name = '" . $uname . "' AND EXISTS (SELECT * FROM users_roles WHERE uid = u.uid AND rid IN (SELECT rid FROM role WHERE name = 'Super admin' OR name = 'Maennaco admin'))");
    return mysql_num_rows($result) > 0 ? true : false;
}

function checkValues($value)
{
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
    $lengths = array("60", "60", "24", "7", "4.35", "12", "10");

    $now = time();

    $difference = $now - $time;
    $tense = "ago";

    for ($j = 0; $difference >= $lengths[$j] && $j < count($lengths) - 1; $j++) {
        $difference /= $lengths[$j];
    }

    $difference = round($difference);

    if ($difference != 1) {
        $periods[$j] .= "s";
    }

    return "$difference $periods[$j] ago ";
}


$next_records = 10;
$show_more_button = 0;

if (isset($url) && $url) {
    $res = mysql_query("SELECT d_id FROM wall_documents where document_name = '" . basename($url) . "'");
    $red = mysql_fetch_array($res);
    $d_id = $red['d_id'];
}

if ($_REQUEST['type'] == 'moreEventsCalendar') {

    $eventRes = mysql_query("SELECT * FROM maenna_company_events_inv inv JOIN maenna_company_events ev ON ev.eventid = inv.eventid WHERE uid = '" . $_REQUEST['u'] . "' AND inv.status = 'confirmed' ORDER BY datetime DESC LIMIT 5,120000000000");


    while ($events = mysql_fetch_array($eventRes)) {

        $content .= date("m/d/Y", $events['datetime']) . " - <a target = \"_blank\" href=\"http://maennaco.cp-dev.com/account?tab=companies&page=company_detail&id=" . $events['companyid'] . "&mtab=advice\" >" . $events['title'] . "</a><br>";

    }
    die($content);
}

if ($_REQUEST['type'] == 'removeFile') {

    $fileId = (int) $_REQUEST['fileid'];

    $db = \Clewed\Db::get_instance();
    $file = $db->get_row("SELECT * FROM maenna_company_data WHERE dataid = ?", array($fileId));
    if(empty($file))
        die;

    $posts = $db->get_array("SELECT * FROM pro_wall_posts WHERE pro_id = ?", array($fileId));
    if(!empty($posts))
        die;

    $db->run("UPDATE maenna_company_data SET deleted = 1 WHERE dataid = ?", array($fileId));
    die();
}
if ($_REQUEST['type'] == 'unDeleteFile') {

    $fileId = (int) $_REQUEST['fileid'];

    $db = \Clewed\Db::get_instance();
    $file = $db->get_row("SELECT * FROM maenna_company_data WHERE dataid = ?", array($fileId));
    if(empty($file))
        die;

/*    $posts = $db->get_array("SELECT * FROM pro_wall_posts WHERE pro_id = ?", array($fileId));
    if(!empty($posts))
        die;*/

    $db->run("UPDATE maenna_company_data SET deleted = 0 WHERE dataid = ?", array($fileId));
    die();
}
if ($_REQUEST['type'] == 'commInv') {


    $invitees = explode(",", $_REQUEST['invitees']);


    $cmpname = mysql_query("SELECT projname FROM maenna_company WHERE companyid = '" . $_REQUEST['cid'] . "'");
    $cmpname = mysql_fetch_array($cmpname);

    foreach ($invitees as $value) {

        $sqlmail = mysql_query("SELECT mail FROM users WHERE uid = '" . $value . "'");
        $invmail = mysql_fetch_array($sqlmail);

        $to = $invmail['mail'];
        $subject = 'Give advice on clewed for ' . ucwords($cmpname['projname']);
        $message .= ucwords($cmpname['projname']) . ' requests your advice / comments on ' . $_REQUEST['name'];
        $message .= '<br><br>';
        $message .= 'Follow this link to see and comment on this file. <br>';
        $message .= 'http://clewed.com/account?tab=companies&page=company_detail&id=' . $_REQUEST['cid'] . '&mtab=file&file=' . urlencode($_REQUEST['filename']) . '&name=' . urlencode($_REQUEST['name']) . ' <br>';
        $headers = "From:clewed@clewed.com \r\n";
        $headers .= "Content-type: text/html; charset=iso-8859-1 \r\n";
        $headers .= "Bcc: clewed@clewed.com \n";


        mail($to, $subject, $message, $headers) or die("Message couldn`t be send. Please try again!");

        $to = '';
        $message = '';
        $headers = '';

    }

    die('Your invitations were sent succesfully!');

}

if ($_REQUEST['type'] == 'eventEdit') {

    if ($_REQUEST['display'] == 'true') {

        $uid = (int) $_POST['uid'];
        $time = (int) $_POST['time'];
        $hash = $_POST['hash'];
        $computedHash = md5($uid . ':' . $time . ':kyarata75');

        if ($hash !== $computedHash)
            die();

        $serviceId = (int) $_REQUEST['eventid'];
        $companyService = new Clewed\Company\Service();
        $services = $companyService->getServices(array($serviceId));
        $sql = $service = $services[$serviceId];
        $companyId = (int) $service['companyid'];

        $userService = new Clewed\User\Service();
        $isSuperAdmin = $userService->isSuperAdmin($uid);
        $isAdmin = $userService->isAdmin($uid, $companyId);
        $isColleague = $userService->isColleague($uid, $companyId);
        $isProjectOwner = $uid == $companyId;

        $expertIds = $companyService->getExpertIds($serviceId);

        $blockedTooltip = "";
        $blockControls = !$isAdmin && !$isSuperAdmin && !empty($service['start_date']);
        if($blockControls)
            $blockedTooltip = " data-tooltip=\"This service has started. You can only edit the subsequent adjustment section, add new milestones and upload files after service has kicked off.\" ";

        $sql2 = mysql_query("
              SELECT 
                  mcd.*,
                  CASE mp.firstname IS NULL WHEN TRUE THEN mc.projname ELSE mp.firstname END as uploaded_by,
                  CASE mp.pid IS NULL AND mc.companyid IS NULL WHEN TRUE THEN 1 ELSE 0 END as uploaded_by_clewed
              FROM maenna_company_data mcd
              LEFT JOIN maenna_people mp ON mp.pid = mcd.editorid 
              LEFT JOIN maenna_company mc ON mc.companyid = mcd.editorid 
              WHERE mcd.deleted <> 1 
              AND mcd.data_type = 'service-file' 
              AND mcd.data_value6 = '" . $_REQUEST['eventid'] . "'
              ORDER BY mcd.dataid DESC");

        ?>
        <div style="clear:both;"></div>
        <style>
            #editEventForm div strong {
                font-family: 'LatoRegular';
            }

        </style>
        <form action="" method="post" name="editEvent" id="editEventForm" class="edit-service-form">

            <div align="left"><strong>Title:</strong></div>
            <input class="input"
                   id="eventType"
                   name="eventType"
                   <?= $blockedTooltip;?>
                   <?= $blockControls ? 'readonly="readonly"' : ''?>
                   value="<?= $sql['title'] ?>"/>

            <div align="left" style="width:48%;float:left;"><strong>Deadline:</strong></div>
            <div align="left" style="width:48%;float:right;"><strong>How is service delivered?</strong></div>

            <input type='text'
                   id="eventDateEdit"
                   name="eventDate"
                   class='edit-event-datepicker'
                    <?= $blockedTooltip;?>
                    <?= $blockControls ? 'readonly="readonly"' : ''?>
                   value="<?= (!empty($sql['datetime']) && '1970' < date('Y', $sql['datetime']) ? date("Y-m-d g:i", $sql['datetime'] - 60 * 60) : '') ?>"/>

            <input class="input"
                   id="eventLoc"
                   name="eventLoc"
                    <?= $blockedTooltip;?>
                    <?= $blockControls ? 'readonly="readonly"' : ''?>
                   value="<?= $sql['location'] ?>" />

            <?php if(!$blockControls):?>
                <script type="text/javascript">
                    $(function () {
                        $('.edit-event-datepicker').datetimepicker({
                            dateFormat: 'yy-mm-dd',
                            timeFormat: 'hh:mm',
                            minDate: 0
                        });

                    });
                </script>
            <?php endif;?>

            <div align="left" style="margin-top: 20px;"><strong>Describe service and deliverables:</strong></div>
            <textarea class="input"
                      id="eventDesc"
                      name="eventDesc"
                      <?= $blockedTooltip;?>
                      <?= $blockControls ? 'readonly="readonly"' : ''?>
                      cols="60"><?= $nl = preg_replace('#<br\s*/?>#i', "\n", $sql['description']); ?></textarea>

            <div class="milestones-section" style="margin-bottom:35px;">
                <div align="left"><strong>Milestones:</strong></div>
                <div class="milestones-container">
                </div>
                <div style="clear:both;"></div>
                <div class="milestones-controls">
                    <div class="add-milestone-btn" style="float:right;color: #00a2bf;line-height: 35px;cursor: pointer;">Add milestones</div>
                </div>
                <script type="text/javascript">

                    function renderMilestone(id, date, description, readonly) {
                        return $(
                            "<div data-id=\"" + (id ? id : "") + "\" class=\"milestone\" style=\"margin-top: 15px;\">" +
                            "   <input " +
                            "       class=\"due-date\"" +
                            "       type=\"text\"" +
                            "       placeholder=\"Due date\"" +
                            "       value=\"" + (date ? date : "") + "\"" +
                            (true === readonly ? ' readonly=\"readonly\" ' : '') +
                            (true === readonly ? ' <?= $blockedTooltip;?> ' : '') +
                            "       style=\"width: 33%!important;height: 46px!important;font-size:14px;\"/>" +
                            "   <textarea " +
                            "        class=\"description\"" +
                            "        type=\"text\"" +
                            "        placeholder=\"Description\"" +
                            "        title=\"" + (description ? description : "") + "\"" +
                            (true === readonly ? ' readonly=\"readonly\" ' : '') +
                            "        style=\"padding-top:6px;width: 65%!important;height: 46px!important;font-size:14px;float:right\">"+ (description ? description : "") + "</textarea>" +
                            "</div>"
                        );
                    }

                    function addMilestone(id, date, description, readonly) {
                        var $milestone = renderMilestone(id, date, description, readonly);
                        $('.edit-service-form .milestones-container').append($milestone);
                        if(!readonly)
                            $milestone.find('.due-date').datepicker({
                                dateFormat: 'mm/dd/y',
                                minDate: 0
                            });
                    }

                    $(function(){
                        $('.edit-service-form .add-milestone-btn').attr('onclick', 'addMilestone();return false;');

                        <?php foreach ($companyService->getServiceMilestones($serviceId) as $milestone):?>
                            <?php $dueDate = date('m/d/y', strtotime($milestone['due_date']));?>
                            <?php $description = str_replace("/n", '//n', htmlspecialchars($milestone['description'], ENT_QUOTES));?>
                            addMilestone('<?=$milestone['id']?>', '<?=$dueDate ?>', '<?= $description ?>', <?= $blockControls ? 'true' : 'false' ?>);
                        <?php endforeach;?>
                    });

                </script>
            </div>

            <?php if(!empty($service['start_date'])):?>
                <div align="left" style="margin-top: -20px;"><strong>Subsequent adjustments:</strong></div>
                <textarea class="input"
                          id="eventDesc"
                          name="subsequent_adjustments"
                          style="height: 100px;margin-bottom: 35px;"
                          cols="60"><?= $nl = preg_replace('#<br\s*/?>#i', "\n", $sql['subsequent_adjustments']); ?></textarea>
            <?php endif;?>

            <?php if ('admin' == getUserType($uid)): ?>

                <script type="text/javascript">
                    $(function(){
                        var $form = $('#editEventForm'),
                            recalculateTotal = function(){
                                var budget = $form.find('input[name=budget]').val(),
                                    clewedFee = $form.find('input[name=clewed_fee]').val(),
                                    $total = $('.edit-service-total-fee');

                                if(!clewedFee.trim().length)
                                    clewedFee = 0;

                                budget = budget.replace('Funded', '');
                                if(!budget.trim().length)
                                    budget = 0;

                                if(0 == budget && 0 == clewedFee)
                                    return $total.val('0.00');

                                if(Number(budget) == budget && Number(clewedFee) == clewedFee)
                                    $total.val((budget * (100 + (clewedFee - 0)) / 100).toFixed(2));
                            };

                        $form.find('input[name=budget]').keyup(recalculateTotal);
                        $form.find('input[name=clewed_fee]').keyup(recalculateTotal);
                        $form.find('input[name=budget]').trigger('keyup');
                    });

                </script>

                <div style="float:left;width: 32%;" align="left"><strong>Expert Fee $:</strong></div>
                <div style="float:left;width: 32%;margin-left: 15px;" align="left"><strong>Clewed Fee %:</strong></div>
                <div style="float:right;width: 32%;" align="left"><strong>Total fee $:</strong></div>

                    <input type="text"
                           name="budget"
                           <?php //echo !empty($sql['payment_id']) ? "disabled=\"disabled\"" : ''; ?>
                           value="<?php echo $sql['budget']//echo $sql['budget'] . (!empty($sql['payment_id']) ? ' Funded' : ''); ?>"
                           style="width: 32%!important;height: 46px!important;font-size:14px;margin-right: 10px;"/>

                    <input type="text"
                           name="clewed_fee"
                           <?php echo !empty($sql['payment_id']) ? "disabled=\"disabled\"" : ''; ?>
                           value="<?php echo $sql['clewed_fee']; ?>"
                           style="width: 32%!important;height: 46px!important;font-size:14px;"/>

                    <input type="text"
                           class="edit-service-total-fee"
                           readonly="readonly"
                           style="width: 32%!important;height: 46px!important;font-size:14px;float:right;"/>

            <?php elseif($uid == $sql['companyid'] || $isColleague): ?>
                <div align="left"><strong>Total fee $:</strong></div>
                <div>
                    <input type="text"
                           name="total"
                           <?php echo $blockControls ? 'readonly="readonly"' : ''?>
                           <?php echo (!empty($sql['payment_id']) || !empty($sql['start_date'])) ? "disabled=\"disabled\"" : ''; ?>
                           value="<?php echo number_format($sql['budget'] * (100 + $sql['clewed_fee']) / 100, 2, '.', '')
                                . (!empty($sql['payment_id']) ? ' Funded' : ''); ?>"
                           style="width:100%!important;height: 46px!important;margin-top: 9px;margin-right: 10px;font-size:14px;"/>
                </div>
            <?php elseif($uid == $sql['executor_id']): ?>
                <div align="left"><strong>Expert fee $:</strong></div>
                <div>
                    <input type="text"
                           name="budget"
                           <?= $blockControls ? 'readonly="readonly"' : ''?>
                           <?= $blockedTooltip ?>
                           <?php echo !empty($sql['payment_id']) || !empty($sql['start_date']) ? "disabled=\"disabled\"" : ''; ?>
                           value="<?php echo $sql['budget'] . (!empty($sql['payment_id']) ? ' Funded' : ''); ?>"
                           style="width:100%!important;height: 46px!important;margin-top: 9px;margin-right: 10px;font-size:14px;"/>
                </div>
            <?php endif; ?>
            <div class="main-expert-container" style="margin-top: 13px;">
                <input type='text'
                       name="executor_id"
                        <?= $blockControls ? 'readonly="readonly"' : ''?>
                       placeholder="Lead expert"
                       value="<?php echo $sql['executor_id'] ?>"
                       style="color:#8f9095 !important;font-family: 'Lato Italic', serif!important;font-size: 14px!important;"/>
            </div>

            <?php $blockInvite = $blockControls;
            if($blockControls && ($isProjectOwner || $isColleague))
                $blockInvite = false;?>

            <div class="invited-experts-container <?php echo $blockInvite ? 'readonly' : ''?>" style="margin-top: 13px;">
                <input type='text'
                       id="eventInv"
                       name="eventInv"
                       <?php echo $blockInvite ? 'readonly="readonly"' : ''?>
                       placeholder="Invite expert collaborators or client colleagues"
                       title="You are able to invite experts in the Active section of you project team or client colleagues before deliverables are set. After, you can only invite colleages to protect confidentiality."
                       style="width:94%!important;"
                       value="<?php echo implode(',', $expertIds); ?>"/><br>
            </div>

            <?php if($isAdmin || $isSuperAdmin):?>
                <script>
                    $(function(){
                        $('.invited-experts-container input').focus(function(e){
                            var $input = $(this),
                                confirmed = $input.data('confirmed');

                            if(!confirmed && !confirm("Service may include confidential materials that can not be openly shared")) {
                                e.preventDefault();
                                e.stopPropagation();
                                return $input.trigger('blur');
                            }

                            $input.data('confirmed', true);
                        });
                    })
                </script>
            <?php endif;?>

            <?php if($blockControls):?>
                <div class="click-blocker"
                    <?= $blockedTooltip ?>
                     style="z-index:1000;width: 95%;position: absolute;height: <?= $blockInvite ? '150':'60'?>px;margin-top: -180px;"></div>
                <script>
                    $(function(){
                        $('.click-blocker').click(function(e){
                            e.stopPropagation();
                            e.preventDefault();
                            return false;
                        });
                    });
                </script>
            <?php endif;?>

<!--            <input type="checkbox" id="chkNot" value="chkNot"> Send notification about changes to attendees</br>-->
        </form>

        <?php die();
    } else {

        require_once __DIR__ . '/../../../lib/init.php';

        $uid = (int) $_POST['uid'];
        $time = (int) $_POST['time'];
        $hash = $_POST['hash'];
        $computedHash = md5($uid . ':' . $time . ':kyarata75');

        $companyService = new Clewed\Company\Service();



        if ($hash !== $computedHash)
            die();

        $db = \Clewed\Db::get_instance();

        $service = $db->get_row("
            SELECT *
            FROM maenna_company_events
            WHERE eventid = ?",
            array((int) $_REQUEST['eventid'])
        );

        $mainExpert = array_filter(explode(',', $_REQUEST['executor_id']), function ($item) {
            return preg_replace('#[^0-9]#', '', $item) !== '';
        });

        $isColleague = in_array($uid,$companyService->getColleagueIds($service['companyid']));
        $isProjectOwner = $uid == $service['postedby'];

        $mainExpertId = !empty($mainExpert) && 1 == count($mainExpert) ? reset($mainExpert) : null;
        if (!isset($_REQUEST['fee']))
            $_REQUEST['fee'] = $service['clewed_fee'];

        if(!isset($_REQUEST['budget']) && !isset($_REQUEST['total']))
            $_REQUEST['budget'] = $service['budget'];

        if(!isset($_REQUEST['budget']) && isset($_REQUEST['total']) && $_REQUEST['total'] != 0) {
            if ($isColleague || $isProjectOwner) {
                $oldTotal = $service['budget'] * (100 + $service['clewed_fee']) / 100;


                if ($service['budget'] == 0) {
                    $_REQUEST['budget'] = $_REQUEST['total'] * 100 / (100 + $_REQUEST['fee']);
                }
                else {
                    $_REQUEST['budget'] = $service['budget'];
                    $_REQUEST['fee'] = ($service['clewed_fee'] * ($_REQUEST['total'] - $service['budget'])) / ($oldTotal - $service['budget']);
                }
            }
            else
            $_REQUEST['budget'] = $_REQUEST['total'] * 100 / (100 + $_REQUEST['fee']);
        }
        $mainExpertConfirmation = $service['executor_status'];
        if (!empty($mainExpertConfirmation) && $mainExpertId != $service['executor_id'])
            $mainExpertConfirmation = null;

        if (empty($mainExpertConfirmation) && $_REQUEST['uid'] == $mainExpertId && $service['executor_id'] != $mainExpertId)
            $mainExpertConfirmation = 'confirmed';

        $userService = new Clewed\User\Service();
        if (!empty($service['payment_id'])) {
            if ($mainExpertId != $service['executor_id'] && 'confirmed' == $service['executor_status']) {
                $mainExpertId = $service['executor_id'];
                $mainExpertConfirmation = $service['executor_status'];
            }

            if(!$userService->isSuperAdmin($_REQUEST['uid'])) {
                $_REQUEST['budget'] = $service['budget'];
                $_REQUEST['fee'] = $service['clewed_fee'];
            }
        }

        $db->run("
            UPDATE maenna_company_events
            SET
                title = ?,
                description = ?,
                subsequent_adjustments = ?,
                location = ?,
                datetime = ?,
                budget = ?,
                clewed_fee = ?,
                executor_id = ?,
                executor_status = ?
            WHERE eventid = ?",
            array(
                $_REQUEST['title'],
                str_replace("\n", '<br />', $_REQUEST['agenda']),
                str_replace("\n", '<br />', $_REQUEST['subsequent_adjustments']),
                $_REQUEST['loc'],
                strtotime($_REQUEST['datetime']) + 60 * 60,
                abs((float) str_replace(',', '.', $_REQUEST['budget'])),
                abs((float) str_replace(',', '.', $_REQUEST['fee'])),
                $mainExpertId,
                $mainExpertConfirmation,
                (int) $_REQUEST['eventid']
            )
        );

        $companyService = new Clewed\Company\Service();
        $companyService->replaceServiceMilestones($service['eventid'], $_REQUEST['milestones']);

        $eventId = (int) $_REQUEST['eventid'];

        $invitees = array_filter(explode(",", $_REQUEST['invitees'] ?: array()), function ($item) {
            return preg_replace('#[^0-9]#', '', $item) !== '';
        });

        $umail = mysql_query("SELECT mail FROM users WHERE uid = '" . $_REQUEST['uid'] . "'");
        $umail = mysql_fetch_array($umail);

        $db = \Clewed\Db::get_instance();
        $isApproved = $db->get_row("
            SELECT approved
            FROM maenna_company_events
            WHERE eventid = ?",
            array((int) $_REQUEST['eventid'])
        );

        $isApproved = $isApproved['approved'];

        $notificationService = new Clewed\Notifications\NotificationService();

        foreach ($_REQUEST['files'] as $value) {

            $fileName = $value['path'];

            if ($value['title'] == '') $value['title'] = substr($value['path'], 0, strrpos($value['path'], '.'));

            if(is_readable(ROOT . 'sites/default/files/events_tmp/' . $fileName))
                rename(ROOT . 'sites/default/files/events_tmp/' . $fileName, ROOT . 'sites/default/files/' . $fileName) or die('problem');

            $sql = "insert into maenna_company_data (companyid, access, data_type,data_value, data_value2,data_value6, editorid )
						values('" . $_REQUEST['cid'] . "','" . time() . "','service-file','" . $value['title'] . "','" . $fileName . "','" . $_REQUEST['eventid'] . "'," . $_REQUEST['uid'] . ")";
            mysql_query($sql) or die(mysql_error());

            if($isApproved)
                $notificationService->registerEvent(
                    'company_service_file_added',
                    (int) $_REQUEST['eventid'],
                    (int) $_REQUEST['uid'],
                    array(
                        'id' => mysql_insert_id(),
                        'fileName' => $fileName,
                        'authorId' => (int) $_REQUEST['uid']
                    )
                );
        }

        $cmpname = mysql_query("SELECT projname FROM maenna_company WHERE companyid = '" . $_REQUEST['cid'] . "'");
        $cmpname = mysql_fetch_array($cmpname);


        if ($isApproved && !empty($mainExpertId) && $mainExpertId != $service['executor_id'] && empty($mainExpertConfirmation))
            $notificationService->registerEvent(
                'company_service_invite_sent',
                (int) $_REQUEST['eventid'],
                (int) $_REQUEST['uid'],
                array(
                    'invitedUserId' => (int) $mainExpertId,
                    'lead' => true
                )
            );

        if (!empty($invitees)) {
            $inviteePlaceholder = rtrim(str_repeat('?, ', count($invitees)), ', ');
            $params = $invitees;
            array_unshift($params, $eventId);
            $db->run("
                DELETE FROM maenna_company_events_inv
                WHERE eventid = ?                  
                AND uid NOT IN({$inviteePlaceholder})",
                $params
            );
        } else {
            $db->run("
                DELETE FROM maenna_company_events_inv
                WHERE eventid = ?",
                array($eventId)
            );
        }

        $companyService = new Clewed\Company\Service();
        $colleagueIds = $companyService->getColleagueIds($service['companyid']);

        foreach ($invitees as $value) {

            //Checking if user is already invited so we don`t have duplicates

            $testSql = mysql_query("SELECT COUNT(*) cnt FROM maenna_company_events_inv WHERE status = 'sent' AND eventid = '" . $_REQUEST['eventid'] . "' AND uid = '" . $value . "'") or die(mysql_error());
            $invCount = mysql_fetch_array($testSql);

            if ($invCount['cnt'] == 0) {

                mysql_query("DELETE FROM maenna_company_events_inv WHERE eventid='{$eventId}' AND uid = " . (int) $value);
                mysql_query("INSERT INTO maenna_company_events_inv (eventid,uid,status) VALUES ('" . $_REQUEST['eventid'] . "'," . $value . ",'sent')");

                if ($isApproved)
                    $notificationService->registerEvent(
                        'company_service_invite_sent',
                        (int) $_REQUEST['eventid'],
                        (int) $_REQUEST['uid'],
                        array(
                            'invitedUserId' => (int) $value,
                            'lead' => false,
                            'colleague' => in_array($value, $colleagueIds)
                        )
                    );
            }
        }
    }
}

if ($_REQUEST['type'] == 'eventMilestonesEdit') {
    if ($_REQUEST['display'] == 'true') {
        $service_id = (int)$_REQUEST['eventid'];
        $companyService = new Clewed\Company\Service();
        $milestones = $companyService->getServiceMilestones($service_id);
        $durations = array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 23, 25, 27, 30);
        ?>
        <form action="" method="post" name="editMilestonesEvent" id="editMilestonesEventForm" class="edit-service-form">
            <div class="milestones-section" style="margin-bottom:35px;">
                <div align="left"><strong>Milestones:</strong></div>
                <div class="milestones-container">
                    <?php foreach ($milestones as $milestone) : ?>
                        <div id="ms_id<?= $milestone['id'] ?>" class="milestone-data" style="margin-top: 5px;display: inline-block;">
                            <select style="margin: -5px 0 0 0;padding: 6px;width: 150px !important;height: 25px !important;box-sizing: content-box;font-size: 14px !important;">
                                <option value="">Milestone deliverable</option>
                                <?php foreach ($durations as $duration) : ?>
                                    <option value="<?= $duration; ?>" <?= $duration == $milestone['duration'] ? 'selected="selected"' : ''; ?>>In <?= $duration; ?> days</option>
                                <?php endforeach; ?>
                            </select>
                            <input type="text" value="<?= $milestone['description']; ?>" placeholder="Deliverable description" style="margin: 0 0 0 5px; width: 419px !important; height: 37px !important;">
                        </div>
                    <?php endforeach; ?>
                </div>
                <div style="clear:both;"></div>
                <div class="milestones-controls">
                    <div class="add-milestone-btn" style="float:right;color: #00a2bf;line-height: 35px;cursor: pointer;">Add milestones</div>
                </div>
                <script type="text/javascript">

                    function addMilestone() {
                        var durations = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 23, 25, 27, 30];
                        var singleMilestoneInputs = '<div class="milestone-data" style="margin-top: 5px;display: inline-block;"><div>' +
                            '<select style="margin: -5px 0 0 0; padding: 6px; width: 150px !important; height: 25px !important; box-sizing: content-box; font-size: 14px !important;">' +
                            '<option value="" selected="selected">Milestone Deliverable</option>';
                        for (var i in durations) {
                            singleMilestoneInputs += '<option value="' + durations[i] + '">In ' + durations[i] + ' days</option>';
                        }
                        singleMilestoneInputs += '</select>' +
                            '<input type="text" placeholder="Deliverable description" style="margin: 0 0 0 7px; width: 419px !important; height: 37px !important;"></div></div>';
                        $('.edit-service-form .milestones-container').append(singleMilestoneInputs);
                    }

                    $(function(){
                        $('.edit-service-form .add-milestone-btn').attr('onclick', 'addMilestone();return false;');
                    });

                </script>
            </div>
        </form>
        <?php
        die();
    }
}

if ($_REQUEST['type'] == 'addEvent') {

    if (md5($_REQUEST['uid'] . "kyarata75") === $_REQUEST['m']) {


        require_once __DIR__ . '/../../../lib/init.php';

        function replace_email($subject)
        {

            $pattern = "/(?:[a-z0-9!#$%&'*+=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+=?^_`{|}~-]+)*|\"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*\")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\])/";

            return preg_replace($pattern, '<i>e-mail obscured</i>', $subject);
        }

        function getProId($id)
        {
            if (empty($id)) return 'invalid id';
            $sql = "select users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname from users_roles, maenna_people where users_roles.uid = '" . $id . "' and maenna_people.pid = '" . $id . "' limit 1";
            $result1 = mysql_query($sql);
            $Row = mysql_fetch_assoc($result1);
            if (empty($Row)) return "invalid user role setting - $id";
            $rid = $Row['rid'];
            $firstname = strtoupper($Row['firstname']);
            if (in_array($rid, array(6, 10))) {
                $output = "clewed";
            } else {
                $output = "${firstname}";// . sprintf("%04s", $id +100);
            }

            return $output;
        }

        $val = explode(" ", $_REQUEST['date']);
        $date = explode("-", $val[0]);
        $time = explode(":", $val[1]);
        $time[0] = 0;
        date_default_timezone_set('EST');
        //echo date("M j Y \a\t g:i A T",strtotime($_REQUEST['date']));
        //die();

        $budget = abs((float) str_replace(',', '.', $_REQUEST['budget']));
        $fee = abs((float) str_replace(',', '.', $_REQUEST['fee']));
        $total = abs((float) str_replace(',', '.', $_REQUEST['total']));

        if(!isset($_REQUEST['fee']))
            $_REQUEST['fee'] = $fee = 100;

        if (isset($_REQUEST['budget']) && trim($_REQUEST['budget']) == '')
            {
                $budget = 250;
                $total = $budget + ($fee/100)*$budget;
            }

        if (isset($_REQUEST['total']) && trim($_REQUEST['total']) == '') {
            $budget = 250;
            $total = $budget + ($fee/100)*$budget;
        }

        if(isset($_REQUEST['total']) && trim($_REQUEST['total']) != '' && !isset($_REQUEST['budget']))
            $budget = $total * 100 / (100 + $fee);

        $mainExpert = array_filter(explode(',', $_REQUEST['executor_id']), function ($item) {
            return preg_replace('#[^0-9]#', '', $item) !== '';
        });

        $executorId = !empty($mainExpert) && 1 == count($mainExpert) ? reset($mainExpert) : 'null';
        $executorStatus = null;
        if ($_REQUEST['uid'] == $executorId)
            $executorStatus = 'confirmed';

        if (!isset($_REQUEST['serviceType']) || $_REQUEST['serviceType'] == '') {$insertType = 'service';}
        else $insertType = $_REQUEST['serviceType'];

        $sql = "
            INSERT INTO maenna_company_events (
                companyid,
                title,
                description,
                location,
                datetime,
                created,
                postedby,
                tags,
                budget,
                " . (isset($_REQUEST['fee']) ? 'clewed_fee,' : '') . "
                " . (!empty($executorStatus) ? 'executor_status,' : '') . "
                executor_id,
                type
            )
            VALUES (
                ?,
                ?,
                ?,
                ?,
                ?,
                ?,
                ?,
                ?,
                ?,
                " . (isset($_REQUEST['fee']) ? "?," : '') . "
                " . (!empty($executorStatus) ? "?," : '') . "
                ?,
                ?
            )
        ";

        $params = array(
            (int) $_REQUEST['cid'],
            $_REQUEST['event'],
            str_replace("\n", '<br />', $_REQUEST['desc']),
            $_REQUEST['loc'],
            strtotime($_REQUEST['date']),
            time(),
            (int) $_REQUEST['uid'],
            $_REQUEST['tags'],
            $budget
        );

        if(isset($_REQUEST['fee']))
            $params[] = $fee;

        if(!empty($executorStatus))
            $params[] = $executorStatus;

        $params[] = $executorId;
        $params[] = $insertType;

        \Clewed\Db::get_instance()->run($sql, $params);

        $eventId = \Clewed\Db::get_instance()->lastInsertId();
        $invitees = array_filter(explode(",", $_REQUEST['invitees'] ?: array()), function ($item) {
            return preg_replace('#[^0-9]#', '', $item) !== '';
        });

        $companyService = new Clewed\Company\Service();
        $companyService->replaceServiceMilestones($eventId, $_REQUEST['milestones']);

        $umail = mysql_query("SELECT mail FROM users WHERE uid = '" . $_REQUEST['uid'] . "'");
        $umail = mysql_fetch_array($umail);

        $cmpname = mysql_query("SELECT projname FROM maenna_company WHERE companyid = '" . $_REQUEST['cid'] . "'");
        $cmpname = mysql_fetch_array($cmpname);

        $db = \Clewed\Db::get_instance();
        $isApproved = $db->get_row("
            SELECT approved
            FROM maenna_company_events
            WHERE eventid = ?",
            array((int) $eventId)
        );

        $isApproved = $isApproved['approved'];

        $companyService = new Clewed\Company\Service();
        $colleagueIds = $companyService->getColleagueIds($_REQUEST['cid']);

        $notificationService = new Clewed\Notifications\NotificationService();
        foreach ($invitees as $value) {

            $invInfo = mysql_query("SELECT mail FROM users WHERE uid = '" . $value . "' LIMIT 1");
            $invInfo = mysql_fetch_array($invInfo);
            if ($value != '') {

                //Checking if user is already invited so we don`t have duplicates

                $testSql = mysql_query("SELECT COUNT(*) cnt FROM maenna_company_events_inv WHERE status='sent' AND eventid = '" . $eventId . "' AND uid = '" . $value . "'") or die(mysql_error());
                $invCount = mysql_fetch_array($testSql);

                if ($invCount['cnt'] == 0) {
                    mysql_query("DELETE FROM maenna_company_events_inv WHERE eventid = '{$eventId}' AND uid = " . (int) $value);
                    mysql_query("INSERT INTO maenna_company_events_inv (eventid,uid,status) VALUES ('" . $eventId . "'," . $value . ",'sent')") or die(mysql_error());

                    if ($isApproved)
                        $notificationService->registerEvent(
                            'company_service_invite_sent',
                            (int) $eventId,
                            (int) $_REQUEST['uid'],
                            array(
                                'invitedUserId' => (int) $value,
                                'lead' => false,
                                'colleague' => in_array($value, $colleagueIds)
                            )
                        );
                }
            }
        }

        $posterType = getUserType($_REQUEST['uid']);
        if(!empty($eventId) && 'admin' != $posterType)
            $notificationService->registerEvent(
                'company_service_created',
                (int) $eventId,
                (int) $_REQUEST['uid']
            );

        foreach ($_REQUEST['files'] as $value) {

            $fileName = $value['timestamp'] . "_" . $value['path'];

            if ($value['title'] == '') $value['title'] = substr($value['path'], 0, strrpos($value['path'], '.'));

            rename('../../../sites/default/files/events_tmp/' . $fileName, '../../../sites/default/files/' . $fileName) or die('problem');

            $db->run("
                INSERT INTO maenna_company_data (companyid, access, data_type, data_value, data_value2, data_value6, editorid )
                VALUES( ?, ?, ?, ?, ?, ?, ?)",
                array(
                    (int) $_REQUEST['cid'],
                    time(),
                    'service-file',
                    $value['title'],
                    $fileName,
                    $eventId,
                    (int) $_REQUEST['uid']
                )
            );

        }

        unlink('../../../sites/default/files/events_tmp/');
        $attres = mysql_query("SELECT uid FROM maenna_company_events_inv WHERE eventid = '" . $eventId . "' AND status = 'confirmed'");

        $files = mysql_query("SELECT * FROM maenna_company_data WHERE data_value6 = '" . $eventId . "' AND deleted = 0");

        $invAll = mysql_query("SELECT uid FROM maenna_company_events_inv WHERE eventid = '" . $eventId . "' AND status <> 'confirmed'");

        $q1 = mysql_query("SELECT IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname, gender FROM maenna_people WHERE pid = " . ((int) $_REQUEST['uid']));
        $pro_data = mysql_fetch_array($q1);
        $comp = mysql_query("SELECT first_name, mail FROM users LEFT JOIN users_extend USING(uid) WHERE uid = " . ((int) $_REQUEST['cid']));
        $comp_data = mysql_fetch_array($comp);

        $compname = $comp_data['first_name'];
        $compmail = $comp_data['mail'];
        $proname = $pro_data['firstname'];

//        $content = <<< END
//Hi $compname,
//
//$proname updated your advice page on clewed. Login at www.clewed.com to review.
//
//The clewed team!
//END;
//        $headers = 'From: noreply@clewed.com' . "\r\n";
//        mail($compmail, 'Advice page updated on Clewed!', $content, $headers);


        ?>

        <div class="event" id="event<?= $eventId; ?>">

            <div id="clear" style="clear:both"></div>
            <div style="float:left;" class="calendar">
                <span class="day"><?= date("d", strtotime($_REQUEST['date'])); ?></span>
                <span class="month"><?= strtoupper(date("M", strtotime($_REQUEST['date']))); ?></span>

            </div><!-- calendar -->

            <div class="event-info">
                <div style="margin-left:10px; float:left;">
                    <span class="eventTitle"
                          style="float:left; cursor:pointer; font-size:15px;"><strong><?= replace_email(strtoupper($_REQUEST['event'])); ?></strong></span><br>
                    <span style="float:left;"><?= date("l, M j, Y g:i A T ", strtotime($_REQUEST['date'])); ?></span>

                    <div id="clear" style="clear:both"></div>
                    <span style="float:left;"><?= $_REQUEST['loc']; ?></span><br>
                    <span style="float:left; display:none;"><?= nl2br($_REQUEST['desc']) ?></span>
                </div>
                <?php
                if (mysql_num_rows($attres) > 0 || mysql_num_rows($invAll) > 0) {


                    echo '<div style="clear:both;"></div><div style="float:left;"><span class="invatt" style="float:left; font-size:10px;">';
                    if (mysql_num_rows($invAll) > 0) {

                        echo "INVITED:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                        while ($inv = mysql_fetch_array($invAll)) {
                            $proId = getProId($inv['uid']);
                            if (substr($proId, 0, 7) == 'invalid') {
                                echo "clewed ADMIN &nbsp;&nbsp;";
                            } else echo "<a  href=\"/account?tab=professionals&page=pro_detail&id=" . $inv['uid'] . "&closebtn=1\">" . $proId . "</a> &nbsp;&nbsp;";

                        }
                        echo "<br>";

                    }
                    if (mysql_num_rows($attres) > 0) {
                        echo "ATTENDEES:&nbsp;&nbsp;";
                        while ($att = mysql_fetch_array($attres)) {
                            $proId = getProId($att['uid']);
                            if (substr($proId, 0, 7) == 'invalid') {
                                echo "clewed ADMIN &nbsp;&nbsp;";
                            } else echo "<a target= href=\"/account?tab=professionals&page=pro_detail&id=" . $att['uid'] . "&closebtn=1\">" . $proId . "</a> &nbsp;&nbsp;";
                        }
                        echo "<br>";
                    }

                    echo '</span><div id="clear" style="clear:both"></div>';
                }


                ?>
                <span class="attFiles" style="float:left;font-size:10px;">
				    <?php
                    if (mysql_num_rows($files) > 0) {
                        echo "FILES:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<br>";
                        while ($filesres = mysql_fetch_array($files)) {
                            echo "<a target='_blank' class= \"actionword\" style=\"color:#6792D0;\" href='/account?tab=companies&page=company_detail&id=" . $filesres['companyid'] . "&mtab=file&file=" . urlencode('/sites/default/files/' . $filesres[data_value2]) . "&name=" . urlencode($filesres[data_value]) . "'>" . strtoupper($filesres[data_value]) . "</a><br>";
                        }
                    }

                    ?>

			   </span>
            </div>
            <div id="clear" style="clear:both"></div>

            <a style="float:right;margin-right:8px;margin-left:10px;" href="#" id="remove_id<?php echo $eventId ?>"
               alt="<?php echo md5($eventId . $_REQUEST['u'] . "kyarata75") ?>" name="<?= $_REQUEST['u']; ?>"
               delType='event' class="tool evdelete"> REMOVE</a>

            <a style="float:right;margin-left:10px;" href="#" id="edit_id<?php echo $eventId ?>"
               alt="<?php echo md5($eventId . $_REQUEST['u'] . "kyarata75") ?>" name="<?= $_REQUEST['u']; ?>"
               delType='event' class="tool evedit"> EDIT</a>

		  <span class="tool" style="float:right;font-size:10px;margin-left:1px;">
		  <?php
          if ($_REQUEST['urole'] == 'Super admin' || $_REQUEST['urole'] == 'Maennaco admin') {
              echo "Clewed";
          } else echo strtoupper($_REQUEST['u']);
          ?>

          <?= "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" . strtoupper(date("M j", time())); ?></span>


            <form action="" method="post" name="postsForm">

                <div class="UIComposer_Box" style="position: relative;height: 80px;">

		<span class="w">
			<textarea eventid="event<?= $eventId ?>" name="<?= $user->name; ?>"
                      alt="<?= md5($user->name . "kyarata75"); ?>"
                      class=" input watermark" style="height:20px;" cols="60"></textarea>
		</span>


                    <div align="right" style="height: 16px;line-height: 16px;background: white;margin-top:-4px;;">
                        <a id="shareButton" textref="event<?= $eventId ?>" class="tool" style="display:none;">
                            SUBMIT </a>
                    </div>

                </div>

            </form>
            <br>
        </div>
        <hr style="color: #E7E6E7;background-color: #E7E6E7;height: 5px;width:100%;">

        <?php
    }

}

if ($_REQUEST['type'] == 'confirmAtt') {

//    if ($_REQUEST['status'] == 'confirmed') {
//
//        $dbl = mysql_query("SELECT *
//				    FROM maenna_company_events ev1
//				    WHERE eventid ='".$_REQUEST['eventid']."'
//				    AND EXISTS (
//
//				    SELECT *
//				    FROM maenna_company_events ev2
//				    JOIN maenna_company_events_inv inv1 ON ev2.eventid = inv1.eventid
//				    WHERE inv1.uid ='".$_REQUEST['uid']."'
//				    AND inv1.status =  'confirmed'
//				    AND DATE( FROM_UNIXTIME( ev1.datetime ) ) = DATE( FROM_UNIXTIME( ev2.datetime ) )
//                                    AND ABS(time_to_sec(timediff(FROM_UNIXTIME(ev1.datetime),FROM_UNIXTIME(ev2.datetime))) / 3600) < 2
//				    )");
//
//        if (mysql_num_rows($dbl) > 0) die("overlap");
//
//    }

    $db = \Clewed\Db::get_instance();

    $event = $db->get_row("
        SELECT *
        FROM maenna_company_events
        WHERE eventid = ?",
        array(
            (int) $_REQUEST['eventid']
        )
    );

    $db->run("
        UPDATE maenna_company_events_inv
        SET status = ?
        WHERE eventid = ?
        AND uid = ? ",
        array(
            preg_replace('#[^a-zA-Z0-9]#', '', $_REQUEST['status']),
            (int) $_REQUEST['eventid'],
            (int) $_REQUEST['uid']
        )
    );

    if (!empty($event['payment_id']) && $_REQUEST['uid'] == $event['executor_id'] && !empty($event['executor_status']))
        die('Lead expert\'s status can\'t be changed after a service is charged');

    $db->run("
        UPDATE maenna_company_events
        SET executor_status = ?
        WHERE eventid = ?
        AND executor_id = ? ",
        array(
            preg_replace('#[^a-zA-Z0-9]#', '', $_REQUEST['status']),
            (int) $_REQUEST['eventid'],
            (int) $_REQUEST['uid']
        )
    );

    $expertId = (int) $_REQUEST['uid'];
    if($event['approved'] && $expertId == $event['executor_id'] && 'confirmed' == $_REQUEST['status']) {
        $notificationService = new \Clewed\Notifications\NotificationService();
        $notificationService->registerEvent(
            'company_service_approved',
            (int) $event['eventid'],
            $expertId,
            array(
                'authorId' => $event['postedby'],
                'leadId' => $expertId
            )
        );
    }

//    if ($_REQUEST['status'] == 'confirmed') {
//
//        $sql = mysql_query ("SELECT * FROM maenna_company_events mce JOIN users u ON mce.companyid = u.uid WHERE mce.eventid = '".$_REQUEST['eventid']."'");
//        $sql1 = mysql_query ("SELECT mail FROM users WHERE uid = '".$_REQUEST['uid']."'");
//        $sql2 = mysql_query ("SELECT * FROM maenna_company_events_inv mcei JOIN users u ON mcei.uid = u.uid WHERE eventid = '".$_REQUEST['eventid']."'");
//
//        $sqlmail = mysql_query("SELECT mail FROM users WHERE uid = '".$_REQUEST['cid']."'");
//        $cmpmail = mysql_fetch_array($sqlmail);
//
//
//        $event = mysql_fetch_array($sql);
//        $umail = mysql_fetch_array($sql1);


//        $to      = $cmpmail['mail'];
//        $subject = '[Attendance confirmed] '.strtoupper($event['title']).' !!!';
//        $message.= 'User '.$umail['mail'].' has confirmed acceptance!!!';
//        $message.= '<br><br>';
//        $message.= 'Event: '.strtoupper($event['title']).'<br><br>';
//        $message.= date("l, M j, Y g:i A T ",$event['datetime']).'<br><br>';
//        $message.= $event['location'].'<br><br>';
//        $message.= 'Agenda: <br>'.$event['description'].'<br><br>';
//        $message.= 'Attendees/Invitees <br>';
//
//        while ($allinv = mysql_fetch_array($sql2)) {
//
//            $message.= $allinv['mail'].'&nbsp;&nbsp; - &nbsp;&nbsp;'.$allinv[3].'<br>';
//
//        }
//        $headers = "From:".$umail['mail']."\r\n";
//        $headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";
//
//
//        mail($to, $subject, $message, $headers);

//    }

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
        default:
            die('Please, select an option to save!');
    }

}

if ($_REQUEST['type'] == 'teamStats') {

    function getProId($id)
    {
        if (empty($id)) return 'invalid id';
        $sql = "select users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname from users_roles, maenna_people where users_roles.uid = '" . $id . "' and maenna_people.pid = '" . $id . "' limit 1";
        $result = mysql_query($sql);
        $Row = mysql_fetch_assoc($result);
        if (empty($Row)) return "invalid user role setting - $id";
        $rid = $Row['rid'];
        $firstname = ucfirst($Row['firstname']);
        if (in_array($rid, array(6, 10))) {
            $output = "clewed";
        } else {
            $output = "${firstname}";// . sprintf("%04s", $id +100);
        }

        return $output;
    }

    function userRoleId($uid = null)
    {
        if (empty($uid)) return '';
        $sql = "select rid from users_roles where uid = '" . $uid . "' ";
        $result = mysql_query($sql);
        while ($Row = mysql_fetch_array($result)) {
            return $Row['rid'];
        }
        return '';
    }

    function getRole($id)
    {
        $result = mysql_query("SELECT r.name as name FROM users_roles ur JOIN role r on ur.rid = r.rid WHERE ur.uid = '" . $id . "'");
        while ($Row = mysql_fetch_array($result)) {
            return $Row['name'];
        }
        return '';
    }

    function getProExpertise($uid)
    {

        $sql = "select experties from maenna_people where pid = $uid";

        $result = mysql_query($sql);

        $Row = mysql_fetch_object($result);
        //die(test_array($Row));

        return preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $Row->experties);

    }

    function Com_conns($id)
    {
        $Conns = array(
            'Partner' => array(),
            'Admin' => array(),
            'Follower' => array(),
            'Follow' => array(),
            'Advisor' => array(),
            'Watchlist' => array(),
            'Inwatchlist' => array(),
            'Visible' => array(), //
            'Propose' => array()
        );

        if (empty($id)) return $Conns;

        $sql = "select  *
                from    maenna_connections
                where   status = 'active' and
                        ((assignee_uid = '" . $id . "') or (target_uid = '" . $id . "'))
                order by conntype
                ";

        $result = mysql_query($sql);

        while ($Row = mysql_fetch_assoc($result)) {

            $conntype = ucwords($Row['conntype']);

            if ($Row['conntype'] == 'follow') {
                if ($Row['target_uid'] == $id) $Conns['Follower'][] = $Row;
                else $Conns['Follow'][] = $Row;

            } elseif ($Row['conntype'] == 'watchlist') {
                if ($Row['target_uid'] == $id) $Conns['Watchlist'][] = $Row; // target com is the one being watched.
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

    $partnersCount = array_key_exists('Partner', $Conns) ? count($Conns['Partner']) : 0;

    $advisor_active = $following_active = $conn_active = '';
    $ctype = $_REQUEST['ctype'];
    $LIST = '';
    $box_content = '';
    if (empty($ctype) || $ctype == 'advisor') {
        if (isset($_REQUEST['ref']) && $_REQUEST['ref'] != '') $ref2 = "&ref=" . $_REQUEST['ref'] . "&ref2=Advisors";
        $advisor_active = 'active';
        $box_title = "ADVISORS";
        foreach ($Conns['Advisor'] as $Pro) {
            $pro_uid = $Pro['assignee_uid'];
            //Get user gender
            $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
            $gender_tmp = mysql_fetch_array($q1);
            $gender = $gender_tmp['gender'];
            //Check if user have a profile picture
            //die('sites/default/images/profiles/50x50/'.$pro_uid.'.jpg');
            if (file_exists('../../../sites/default/images/profiles/50x50/' . $pro_uid . '.jpg')) $avatar = 'sites/default/images/profiles/150x150/' . $pro_uid . '.jpg';
            elseif ($gender == 'm' || $gender == '') $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
            else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            $pro_maeid = getProId($pro_uid);
            $rid = userRoleId($pro_uid);
            $pro_type = getRole($pro_uid);
            $pro_exp = getProExpertise($pro_uid);

            if ($_REQUEST['perm'] == 'read')

                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:12px !important; color:#91939e; font-family:'LatoRegular' !important;margin-top:12px;\">$pro_maeid<br> $pro_exp</p></div>";

            else    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"cursor: pointer; margin-top:9px !important;\" onclick='showExpertInfo($pro_uid);' > $pro_maeid<br> $pro_exp</a></div>";

        }
    } elseif ($ctype == 'partner') {
        if (isset($_REQUEST['ref']) && $_REQUEST['ref'] != '') $ref2 = "&ref=" . $_REQUEST['ref'] . "&ref2=Partners";
        $partner_active = 'active';
        $box_title = "PARTNERS";
        foreach ($Conns['Partner'] as $Pro) {
            $pro_uid = $Pro['assignee_uid'];
            //Get user gender
            $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
            $gender_tmp = mysql_fetch_array($q1);
            $gender = $gender_tmp['gender'];
            //Check if user have a profile picture
            //die('sites/default/images/profiles/50x50/'.$pro_uid.'.jpg');
            if (file_exists('../../../sites/default/images/profiles/50x50/' . $pro_uid . '.jpg')) $avatar = 'sites/default/images/profiles/150x150/' . $pro_uid . '.jpg';
            elseif ($gender == 'm' || $gender == '') $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
            else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            $pro_maeid = getProId($pro_uid);
            $rid = userRoleId($pro_uid);
            $pro_type = getRole($pro_uid);
            $pro_exp = getProExpertise($pro_uid);

            if ($_REQUEST['perm'] == 'read')

                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:12px !important; color:#91939e; font-family:'LatoRegular' !important;margin-top:12px;\">$pro_maeid<br> $pro_exp</p></div>";

            else    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"cursor: pointer; margin-top:9px !important;\" onclick='showExpertInfo($pro_uid);' > $pro_maeid<br> $pro_exp</a></div>";

        }
    } elseif ($ctype == 'following') {

        if (isset($_REQUEST['ref']) && $_REQUEST['ref'] != '') $ref2 = "&ref=" . $_REQUEST['ref'] . "&ref2=Colleagues";

        $following_active = 'active';
        $box_title = "MANAGEMENT";
        if (isset($Conns['Client']))
            foreach ($Conns['Client'] as $Pro) {
                $pro_uid = $Pro['assignee_uid'];
                echo $pro_uid;
                //Get user gender
                $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
                $gender_tmp = mysql_fetch_array($q1);
                $gender = $gender_tmp['gender'];
                //Check if user have a profile picture
                if (file_exists('../../../sites/default/images/profiles/50x50/'.$pro_uid.'.jpg')) { $avatar = 'sites/default/images/profiles/50x50/'.$pro_uid.'.jpg';}
                else {
                    if ($gender == 'm' || $gender == '') { $avatar =' /themes/maennaco/images/prof-avatar-male.png';}

                    else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                }
                $pro_maeid = getProId($pro_uid);
                $rid = userRoleId($pro_uid);
                $pro_type = getRole($pro_uid);
                $pro_exp = getProExpertise($pro_uid);

                if ($_REQUEST['perm'] == 'read')

                    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:Helvetica; text-transform:uppercase;margin-top:9px !important;\">$pro_maeid<br> $pro_exp</p></div>";

                else
                    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"cursor: pointer; margin-top:9px !important;\" onclick='showExpertInfo($pro_uid);' > $pro_maeid<br> $pro_exp</a></div>";
                //$box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=".$avatar." style=\"float:left; margin-right:5px; width:50px; height:50px;\"><a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1' target='_blank'>$pro_maeid, $pro_exp</a></div>";
            }
    } elseif ($ctype == 'connections') {

        if (isset($_REQUEST['ref']) && $_REQUEST['ref'] != '') {
            $ref2 = "&ref=" . $_REQUEST['ref'] . "&ref2=Connections";
        }

        $conn_active = 'active';
        $box_title = "CONNECTED";
        foreach ($Conns['Visible'] as $Pro) {
            $pro_uid = $Pro['assignee_uid'];
            echo $pro_uid;
            //Get user gender
            $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
            $gender_tmp = mysql_fetch_array($q1);
            $gender = $gender_tmp['gender'];
            //Check if user have a profile picture
            if (file_exists('../../../sites/default/images/profiles/50x50/'.$pro_uid.'.jpg')) { $avatar = 'sites/default/images/profiles/50x50/'.$pro_uid.'.jpg';}
            else {
                if ($gender == 'm' || $gender == '') { $avatar =' /themes/maennaco/images/prof-avatar-male.png';}

                else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            }
            $pro_maeid = getProId($pro_uid);
            $rid = userRoleId($pro_uid);
            $pro_type = getRole($pro_uid);
            $pro_exp = getProExpertise($pro_uid);

            if ($_REQUEST['perm'] == 'read')

                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:Helvetica; text-transform:uppercase;margin-top:20px;\">$pro_maeid, $pro_exp</p></div>";

            else    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"cursor: pointer; margin-top:9px !important;\" onclick='showExpertInfo($pro_uid);' > $pro_maeid<br> $pro_exp</a></div>";
        }
    } elseif ($ctype == 'partner') {

        if (isset($_REQUEST['ref']) && $_REQUEST['ref'] != '') {
            $ref2 = "&ref=" . $_REQUEST['ref'] . "&ref2=Partners";
        }

        $conn_active = 'active';
        $box_title = "CONNECTED";

        foreach ($Conns['Partner'] as $Pro) {
            $pro_uid = $Pro['assignee_uid'];
            echo $pro_uid;
            //Get user gender
            $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
            $gender_tmp = mysql_fetch_array($q1);
            $gender = $gender_tmp['gender'];
            //Check if user have a profile picture
            if (file_exists('../../../sites/default/images/profiles/50x50/'.$pro_uid.'.jpg')) { $avatar = 'sites/default/images/profiles/50x50/'.$pro_uid.'.jpg';}
            else {
                if ($gender == 'm' || $gender == '') { $avatar =' /themes/maennaco/images/prof-avatar-male.png';}

                else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            }
            $pro_maeid = getProId($pro_uid);
            $rid = userRoleId($pro_uid);
            $pro_type = getRole($pro_uid);
            $pro_exp = getProExpertise($pro_uid);

            if ($_REQUEST['perm'] == 'read')

                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:Helvetica; text-transform:uppercase;margin-top:20px;\">$pro_maeid, $pro_exp</p></div>";

            else    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"cursor: pointer; margin-top:9px !important;\" onclick='showExpertInfo($pro_uid);' > $pro_maeid<br> $pro_exp</a></div>";
        }
    }

    if ($box_content == '') $box_content = 'No users';
    die($box_content);

}

if (checkValues($_REQUEST['value'])) {

    function getProId($id)
    {
        if (empty($id)) return 'invalid id';
        $sql = mysql_query("SELECT rid FROM users_roles WHERE uid = '" . $id . "' LIMIT 1 ");
        $ridn = mysql_fetch_array($sql);
        if ($ridn['rid'] == '3') {

            $sql = "select users_roles.*, maenna_company.projname from users_roles, maenna_company where users_roles.uid = '" . $id . "' and maenna_company.companyid = '" . $id . "' limit 1";


        } else {

            $sql = "select users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname from users_roles, maenna_people where users_roles.uid = '" . $id . "' and maenna_people.pid = '" . $id . "' limit 1";

        }
        $result = mysql_query($sql);
        $Row = mysql_fetch_assoc($result);
        $rid = $ridn['rid'];

        if (in_array($rid, array(6, 10))) {
            $output = "clewed";
        } elseif ($rid == "3") {
            $output = strtoupper($Row['projname']);

        } else {
            $firstname = strtoupper($Row['firstname']);
            $output = "${firstname}";// . sprintf("%04s", $id +100);
        }
        return $output;
    }


    function replace_email($subject)
    {

        $pattern = "/(?:[a-z0-9!#$%&'*+=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+=?^_`{|}~-]+)*|\"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*\")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\])/";

        return preg_replace($pattern, '<i>e-mail obscured</i>', $subject);
    }

    $res = mysql_query("SELECT d_id FROM wall_documents where document_name = '" . $_REQUEST['doc'] . "'");
    $red = mysql_fetch_array($res);
    $d_id = $red['d_id'];
    $editorname = getProId($_REQUEST['uid']);


    if ($_REQUEST['type'] == 'eventcom') $d_id = $_REQUEST['eventid'];

    if (md5($_REQUEST['u'] . "kyarata75") === $_REQUEST['m']) {
        mysql_query("INSERT INTO wall_posts (post,f_name,user,date_created,document_id) VALUES('" . checkValues($_REQUEST['value']) . "','" . $editorname . "','" . $_REQUEST['uid'] . "','" . strtotime(date("Y-m-d H:i:s")) . "','" . $d_id . "')");
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
    $result = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS TimeSpent FROM wall_posts WHERE document_id = " . $d_id . " order by p_id desc limit 0,10");

}

if ($result) {
    while ($row = mysql_fetch_array($result)) {
        $comments = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS CommentTimeSpent FROM wall_posts_comments where post_id = " . $row['p_id'] . " order by c_id asc"); ?>
        <div class="friends_area" id="record-<?php echo $row['p_id'] ?>">
            <?php

            $crId = $_REQUEST['uid'];
            $uType = getUserType($crId);

            if ($uType == 'people' || $uType == 'admin') {

                //Get user gender
                $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $crId");
                $gender_tmp = mysql_fetch_array($q1);
                $gender = $gender_tmp['gender'];

                if (file_exists('sites/default/images/profiles/50x50/'.$crId.'.jpg')) { $avatar = 'sites/default/images/profiles/50x50/'.$crId.'.jpg';}
                else {
                    if ($gender == 'm' || $gender == '') { $avatar =' /themes/maennaco/images/prof-avatar-male.png';}

                    else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                }
            } else if ($uType == 'company') {

//Get cmp_role
                $q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $crId") or die(mysql_error());
                $cmp_role_tmp = mysql_fetch_array($q1);
                $cmp_role = $cmp_role_tmp['company_type'];
                //Check if user have a profile picture
                if (file_exists('sites/default/images/company/50x50/'.$crId.'.jpg')) {$avatar = 'sites/default/images/company/50x50/'.$crId.'.jpg';}
                else
                    if ($cmp_role == 'service') $avatar = ' /themes/maennaco/images/cmp-avatar-service.png';
                    else $avatar = ' /themes/maennaco/images/cmp-avatar-product.png';

            }

            echo "<img src=" . $avatar . " style=\"float:left; margin-top:13px; margin-right:5px; width:35px; height:35px;\">";

            ?>
            <label style="float:left;margin-top:9px;width:90%;" class="name">
			   <span style="color:#00a2bf;">
					<b><?php if (ifAdmin($row['user']))
                        {echo "clewed";}
                        else echo $row['f_name'];?>
                    </b>
					<span style='color:#666;font-style:italic;'>&nbsp;shares a new idea:</span><br/>
			   </span>
                <em style="font-family: 'LatoRegular'; font-size: 13px!important;color: #8f9094!important;">
                    &nbsp;<?php echo nl2br(replace_email($row['post'])); ?></em>
                <br clear="all"/>

                <span><?php echo ago($row['date_created']); ?></span>
                <!--a href="#" class="tool show_reply_box" rowid="<?php echo $row['p_id']; ?>" > Reply</a-->

                <a href="#" id="remove_id<?php echo $row['p_id'] ?>" style="float:none;"
                   alt="<?php echo md5($row['p_id'] . $row['user'] . "kyarata75") ?>" name="<?= $row['user']; ?>"
                   class="delete tool">Delete</a>
                <?php ?>


                <!-- <a href="javascript: void(0)" id="post_id<?php echo $row['p_id'] ?>" class="showCommentBox">Comments</a>
-->
            </label>
            <br clear="all"/>

            <div id="CommentPosted<?php echo $row['p_id'] ?>">
                <?php
                $comment_num_row = mysql_num_rows(@$comments);
                if ($comment_num_row > 0) {
                    while ($rows = mysql_fetch_array($comments)) {
                        $days2 = floor($rows['date_created'] / (60 * 60 * 24));
                        $remainder = $rows['date_created'] % (60 * 60 * 24);
                        $hours = floor($remainder / (60 * 60));
                        $remainder = $remainder % (60 * 60);
                        $minutes = floor($remainder / 60);
                        $seconds = $remainder % 60;
                        ?>
                        <div class="commentPanel"
                             style='padding-top:6px;border-bottom:solid 2px #fff;width:540px;background-color: #f4f8fa!important'
                             id="comment-<?php echo $rows['c_id']; ?>"
                             align="left">
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
                            } else if ($uType == 'company') {

//Get cmp_role
                                $q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $crId") or die(mysql_error());
                                $cmp_role_tmp = mysql_fetch_array($q1);
                                $cmp_role = $cmp_role_tmp['company_type'];
                                //Check if user have a profile picture
                                if (file_exists('sites/default/images/company/50x50/'.$crId.'.jpg')) {$avatar = 'sites/default/images/company/50x50/'.$crId.'.jpg';}
                                else
                                    if ($cmp_role == 'service') $avatar = ' /themes/maennaco/images/cmp-avatar-service.png';
                                    else $avatar = ' /themes/maennaco/images/cmp-avatar-product.png';

                            }

                            echo "<img src=" . $avatar . " style=\"float:left; margin-top:5px;margin-right:5px; width:35px; height:35px;\">";

                            ?>

                            <label style="width:85%;" class="postedComments">
							  <span style="color:#00a2bf;">
                        <?php if (ifAdmin($rows['user'])) {
                            echo "clewed";
                        } else {
                            echo $rows['f_name'];
                        }

                        echo "<span style='color:#8f9094!important;font-style:italic;font-size:12px;font-style:italic'>&nbsp;continues discussion:</span><br />&nbsp;";
                        echo "<span style='color:#8f9094!important;'>" . nl2br(replace_email($rows['comments'])) . "</span>";
                        ?>

                        </span>

                                <br>
                                <?php ?>


                                <span
                                    style="display:inline-block;width:90px;margin-left:0px; color:#8f9094!important; font-size:13px">
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

                                if ($rows['user'] == $_REQUEST['u'] || ifAdmin($user->name) || $rows['user'] == $user->name){
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
            <div style='background-color:#f4f8fa!important; width: 540px;' class="commentBox" align="right"
                 id="commentBox-<?php echo $row['p_id']; ?>"
                <?php //echo (($comment_num_row) ? '' :'style=""')
                ?>
                 name="<?php echo $editorname; ?>"
                 alt="<?php echo md5($editorname . "kyarata75") ?>">
                <label id="record-<?php echo $row['p_id']; ?>" style="padding-top:0;">
                    <textarea class="commentMark"
                              rowid="<?php echo $row['p_id']; ?>"
                              id="commentMark-<?php echo $row['p_id']; ?>"
                              name="commentMark" cols="110"
                              style="margin-left: 0px;width: 528px;border:solid 1px #d0d2d3;margin-top:0;font-family: 'Lato Light';font-size:12px;font-style:italic;height:25px;line-height: 25px"></textarea>
                </label>
                <br clear="all"/>

                <!--<a class="tool clseCommentBox comentboxBtns-<?php // echo $row['p_id'];
                ?>" style="">CANCEL</a>&nbsp;&nbsp;&nbsp;-->
                <a id="SubmitComment" class="tool comment  comentboxBtns-<?php echo $row['p_id']; ?>"
                   style="margin-right:-24px; color:#0fabc4!important;font-size: 13px;">Submit</a>&nbsp;&nbsp;

            </div>
        </div>
        <div style="clear:both;"></div>
        <?php
    }
}

?>
