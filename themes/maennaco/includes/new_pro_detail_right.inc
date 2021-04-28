<?php
date_default_timezone_set('EST');
function pro_gettingStarted($op = null) {
    /**
     * Created by JetBrains PhpStorm.
     * User: acme
     * Date: 3/17/13
     * Time: 2:10 PM
     * To change this template use File | Settings | File Templates.
     */
    if (file_exists('themes/maennaco/includes/get_started.inc')) {
        include_once('themes/maennaco/includes/get_started.inc');
        $output = get_started();
        echo($output);
        global $user;
        if ($user->uid == $_REQUEST['id'] && get_questionnaire_status() < 100 && !isset($_SESSION['q-popup']) && $user->first_time == '1') {
            $_SESSION['q-popup'] = 1;
            user_save($user, array('first_time' => '0'));
            $js_popup = <<< END
          <div id="q-popup">Please complete your profile to get started and access more tools.</div>
          <script type="text/javascript">
            $('#q-popup').dialog({
              autoOpen: 1,
              modal: 1,
              resizable: false
            });
          </script>
END;
            print $js_popup;
        }
    } else {
        print('cant find include file');
    }
}

function pro_rating($op = null) {
    require_once("classes/connections.inc");
    global $user;
    global $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $Block['title'] = 'Clewed Rating';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $panel = 'pro_rating_panel';
    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid))) {
        $Block['title'] .= "<div class=editbtn ><a href='$redirect&panel=${panel}' class=tool>View</a></div>";
    }
    $content = '';
    $time = time();
    if (empty($op)) return '';
    if ($op == 'title') {
        return $Block['title'];
    } elseif ($op) {
        $sql = "select * from maenna_people where pid = %d limit 1";
        $result = db_query($sql, array($pid));
        $Row = db_fetch_object($result);
        $rating = 'Not Rated';
        if ($Row) {
            if (!empty($Row->rating)) $rating = $Row->rating;
        }
        $content .= "<div class='row'>$rating";
        $content .= "</div></div>";
        $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
    }
    return $Block;
}

function pro_DiscussionCalendar($op = null) {
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $pro_id = sget($_REQUEST, 'pro_id');
    //$Block['title'] = 'Calendar';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    //if($op == 'write')$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>Edit</a></div>";
    $content = '';
    if (empty($op)) return '';
    ob_start();
    $url = "http://" . $_SERVER['HTTP_HOST'];
    $editorname = $user->name;
    require_once("pro_calendar.php");
    $content = ob_get_contents();
    ob_end_clean();
    //$content = sget($_REQUEST, 'file');
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function pro_rating_panel($op = null) {
    require_once("classes/connections.inc");
    global $user;
    global $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $dataid = sget($_REQUEST, 'dataid');
    $Block['title'] = 'Clewed Rating';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $panel = 'pro_rating_panel';
    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid))) {
        $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>Edit</a></div>";
    }
    $content = '';
    $time = time();
    if (empty($op)) return '';
    if ($op != 'update') {
        $view = sget($_REQUEST, 'view');
        if ($view == '' || $view == 'view') {
            $sql = "select * from maenna_people_data where pid = %d and data_type = 'rating' limit 1";
            $result = db_query($sql, array($pid));
            $Row = db_fetch_object($result);
            if (empty($Row) || (!is_object($Row))) {
                $Data = array();
            } else {
                if ($Row->data_value2) $Data = unserialize($Row->data_value2);
            }
            $content .= ProRatingTable($Data, 'view');
        } elseif ($view == 'edit') {
            $sql = "select * from maenna_people_data where pid = %d and data_type = 'rating' limit 1";
            $result = db_query($sql, array($pid));
            $Row = db_fetch_object($result);
            if (empty($Row) || (!is_object($Row))) {
                $Data = array();
            } else {
                if ($Row->data_value2) $Data = unserialize($Row->data_value2);
                $dataid = $Row->dataid;
            }
            $content .= ProRatingTable($Data, 'edit', $dataid);
        }
    } elseif ($op == 'update') {
        if (empty($pid)) {
            drupal_set_message('invalid pid value', 'error');
            return;
        }
        if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid))) {
            // continue
        } else {
            drupal_set_message("Unauthorized operation", 'error');
            return '';
        }
        $_RatingAnchors = _proRatingAnchors();
        $Data = array();
        $sum = 0;
        $numOfKeys = 0;
        foreach ($_RatingAnchors as $Items) {
            foreach ($Items as $shortKey => $title) {
                $Data["$shortKey"] = sget($_REQUEST, $shortKey);
                if ($Data["$shortKey"]) {
                    $sum += intval($Data["$shortKey"]);
                    $numOfKeys++;
                }
            }
        }
        if (count($Data) < 1) {
            drupal_set_message('Please choose ratings and try again', 'error');
        }
        $data_value2 = serialize($Data);
        if ($numOfKeys) $averageRating = number_format(($sum / $numOfKeys), 2);
        if (empty($dataid)) {
            $sql = "insert into maenna_people_data (pid,
                                                    access,
                                                    data_type,
                                                    data_attr,
                                                    data_value2,
                                                    editorid) values (
                                                    %d,
                                                    '%s',
                                                    '%s',
                                                    '%s',
                                                    '%s',
                                                    %d)";
            $DBValues = array($pid, $time, 'rating', $averageRating, $data_value2, $editorid);
            if (db_query($sql, $DBValues) === false) {
                drupal_set_message('failed to save rating record', 'error');
                return;
            }
        } else {
            $sql = "update maenna_people_data set data_attr = '%s',data_value2 = '%s', access = '%s', editorid = %d where pid = %d and data_type = 'rating' and dataid = %d ";
            $DBValues = array($averageRating, $data_value2, $time, $editorid, $pid, $dataid);
            if (db_query($sql, $DBValues) === false) {
                drupal_set_message('failed to save rating record', 'error');
                return;
            }
        }
        $sql = "update maenna_people set rating = $averageRating where pid = %d limit 1";
        if (db_query($sql, array($pid)) === false) {
            drupal_set_message('failed to update main rating record', 'error');
            return;
        }
        drupal_set_message('rating record is updated');
        return;
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function ProRatingTable($Data, $mode, $dataid = '') {
    $_RatingAnchors = _proRatingAnchors();
    $strOutput = '';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $pid = sget($_REQUEST, 'id');
    $strOutput = "<form action='/account' method='post'><table style='width:100%' border=0><tr>
                            <td width=370>Rating Anchors</td>
                            <td width=80 style='text-align:center;font-weight:bold;'>1</td>
                            <td width=80 style='text-align:center;font-weight:bold;'>2</td>
                            <td width=80 style='text-align:center;font-weight:bold;'>3</td>
                            <td width=80 style='text-align:center;font-weight:bold;'>4</td>
                            <td width=80 style='text-align:center;font-weight:bold;'>5</td>
                            </tr>";
    /*
      <td width=100>Strong Evidence Candidate Lacks Competency</td>
                            <td width=100>Some Evidence Candidate Lacks Competency</td>
                            <td width=100></td>
                            <td width=100>Some Evidence Candidate Has Competency</td>
                            <td width=100>Strong Evidence Candidate Has Competency</td>
                            </tr>";

    */
    $odd = '#f3f3f3';
    if ($mode == 'view') {
        foreach ($_RatingAnchors as $key => $Items) {
            $strOutput .= "\n<tr><td colspan=6><b>$key<b></td></tr>";
            foreach ($Items as $shortKey => $title) {
                $strOutput .= "\n<tr><td style='text-align:left;'>$title</td>";
                $value = sget($Data, $shortKey);
                for ($i = 1; $i <= 5; $i++) {
                    if ($i == $value) {
                        $strOutput .= "<td style='background:lightblue;text-align:center;'>$value</td>";
                    } else {
                        $strOutput .= "<td style='text-align:center;'>-</td>";
                    }
                }
                $strOutput .= "</tr>";
            }
            $strOutput .= "\n<tr><td colspan=6 class=no-border>&nbsp;</td></tr>";
        }
        $strOutput .= "\n<tr><td colspan=6 style='border:none'>&nbsp;</td></tr>";
        $strOutput .= "\n<tr><td colspan=6 style='border:none'><a href='$redirect' class=button>back</a></td></tr>";
    } elseif ($mode == 'edit') {
        foreach ($_RatingAnchors as $key => $Items) {
            $strOutput .= "\n<tr><td colspan=6><b>$key<b></td></tr>";
            $counter = 0;
            foreach ($Items as $shortKey => $title) {
                $counter++;
                $background = '';
                if ($counter % 2) $background = 'background:lightblue;';
                $strOutput .= "\n<tr><td style='font:12px arial;vertical-align:middle;padding-left:5px;$background'>$title</td>";
                $value = sget($Data, $shortKey);
                for ($i = 1; $i <= 5; $i++) {
                    if ($i == $value) {
                        $strOutput .= "<td style='text-align:center;$background'><input type=radio name='$shortKey' value='$i' checked /></td>";
                    } else {
                        $strOutput .= "<td  style='text-align:center;$background'><input type=radio name='$shortKey' value='$i' /></td>";
                    }
                }
                $strOutput .= "</tr>";
            }
            $strOutput .= "\n<tr><td colspan=6>&nbsp;</td></tr>";
        }
        $strOutput .= "<tr><td colspan=6 style='text-align:right;'><br><input type='submit'  name=submit value='submit' class=button /> <a href='$redirect' class=button>cancel</a>";
        $strOutput .= "<input type=hidden name=tab value='professionals' />
                        <input type=hidden name=page value='pro_detail' />
                        <input type=hidden name=panel value='pro_rating_panel' />
                        <input type=hidden name=update_section value='pro_rating_panel' />
                        <input type=hidden name=dataid value='$dataid' />
                        <input type=hidden name=section value='pro_rating' />
                        <input type=hidden name=id value='$pid' />";
        $strOutput .= "</td></tr>";
    }
    $strOutput .= "</table></form>";
    $strOutput .= <<< END

    <ul>
        <li>1 = Strong Evidence Candidate Lacks Competency</li>
        <li>5 = Strong Evidence Candidate Has Competency</li>
    </ul>

END;
    return $strOutput;
}

///////////////////////////////////////
function progress_status($op = null) {
    require_once("classes/connections.inc");
    global $user, $AccessObj;
    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid))) {
        // continue
    } else {
        return '';
    }
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $Block['title'] = 'Progress Status';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $panel = 'pro_progress_panel';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>Add</a></div>";
    $content = '';
    $time = time();
    if ($op && $op != 'update') {
        $sql = "select * from maenna_people_data where pid = %d and data_type = 'progress' order by access desc limit 3";
        $result = db_query($sql, array($pid));
        while (($Row = db_fetch_object($result)) !== false) {
            $date = date('m/d/Y', $Row->data_value3);
            $text = htmlentities(sideBarExcerpt($Row->data_value2), ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content .= "\n<div class=row style='padding:5px 0'><b>$date</b>&nbsp;&nbsp; $text</div>";
        }
        $content .= "<div style='font:bold 16px arial;text-align:center;'>$rating</div>";
        $viewall_link = "$redirect&panel=${panel}";
        $content .= "<div class='viewallbtn' style='height:10px;'><a href='$viewall_link' class=tool>More</a></div><br style='clear:both' />";
        $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
    }
    return $Block;
}

function pro_progress_panel($op = null) {
    require_once("classes/connections.inc");
    global $user, $AccessObj;
    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid))) {
        // continue
    } else return '';
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $dataid = sget($_REQUEST, 'dataid');
    $Block['title'] = 'PROGRESS STATUS';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $panel = 'pro_progress_panel';
    $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>Add</a></div>";
    $content = '';
    $time = time();
    $view = sget($_REQUEST, 'view');
    $do = sget($_REQUEST, 'do');
    if (empty($pid)) {
        drupal_set_message("Invalid pid", 'error');
        return;
    }
    if ($op && $op != 'update') {
        if ($view == '' || $view == 'view') {
            $sql = "select * from maenna_people_data where pid = %d and data_type = 'progress' order by access ";
            $result = db_query($sql, array($pid));
            $str = '';
            while (($Row = db_fetch_object($result)) !== false) {
                $dataid = $Row->dataid;
                $date = date('m/d/Y', $Row->data_value3);
                $text = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                $contact = htmlentities($Row->data_value, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                if ($Row->data_attr) {
                    $followup = date('m/d/Y', $Row->data_attr);
                } else $followup = '';
                $delete = "<a href='$redirect&panel=pro_progress_panel&update_section=pro_progress_panel&view=view&do=delete&section=pro_progress&dataid=$dataid&id=$pid' class='delete-icon'  onclick='return confirm(\"Continue to remove the record\")'>>delete</a>";
                $edit = "<a href='$redirect&panel=pro_progress_panel&view=edit&dataid=$dataid&id=$pid'  class=>edit</a>";
                $str .= "\n<tr><td>$date</td><td>$contact</td><td>$text</td><td>$followup</td><td>$edit</td><td>$delete</td></tr>";
            }
            $content .= <<< END
            <table class='report'>
                <tr>
                    <td width=100>Date</td>
                    <td width=100>Contact Name</td>
                    <td>Message</td>
                    <td width=100>Followup Date</td>
                    <td width=30></td>
                    <td width=30></td>
                </tr>
                $str
            </table>
            <br />
            <a href='$redirect' class=button>back</a>
END;
        } elseif ($view == 'edit' || $view == 'add') {
            $contact = $followup = $text = '';
            $date = date('m/d/Y');
            if ($dataid) {
                $sql = "select * from maenna_people_data where dataid = %d limit 1";
                $result = db_query($sql, array($dataid));
                $Row = db_fetch_object($result);
                if ($Row) {
                    $date = date('m/d/Y', $Row->data_value3);
                    if ($Row->data_attr) $followup = date('m/d/Y', $Row->data_attr);
                    if ($Row->data_value) $contact = $Row->data_value;
                    $text = ($Row->data_value2);
                }
            }
            $HV = hidden_post_values(array('tab', 'page', 'id'));
            $content .= <<< END
            <form action='/account' method=post>
    <table class='edit_table'>
        <tr><td>Date:</td><td><input type=text name=date value='$date' class='datepicker' /></td></tr>
        <tr><td>Contact Name:</td><td><input type=text name=contact value="$contact" /></td></tr>
        <tr><td>Message:</td><td><textarea name=text style='width:300px;height:80px;'>$text</textarea></td></tr>
        <tr><td>Followup:</td><td><input type=text name=followup value='$followup' class='datepicker' /></td></tr>
        <tr><td></td><td><input type=submit name=submit value=submit class=button /> <a href='$redirect' class=button>cancel</a></td></tr>
    </table>
        $HV
        <input type=hidden name=dataid value=$dataid />
        <input type=hidden name=view value=view />
        <input type=hidden name=panel value=pro_progress_panel />
        <input type=hidden name=update_section value=pro_progress_panel />
        <input type=hidden name=do value=$view />
        </form>
END;
            $content .= '<script type="text/javascript">$(document).ready(function(){init_datepicker();})</script>';
        }
    } elseif ($op == 'update') {
        if ($do == 'add' || $do == 'edit') {
            $date = sget($_REQUEST, 'date');
            $followup = sget($_REQUEST, 'followup');
            if ($date) $date = strtotime($date);
            if ($followup) $followup = strtotime($followup);
            $contact = sget($_REQUEST, 'contact');
            $text = sget($_REQUEST, 'text');
            if ($do == 'edit') {
                if ($dataid) {
                    $sql = "update maenna_people_data set data_value2 = '%s', data_value='%s', access='%s', data_attr = '%s', data_value3 = '%s',editorid=%d where data_type = 'progress' and dataid = %d limit 1";
                    $DBValue = array($text, $contact, $time, $followup, $date, $editorid, $dataid);
                    if (!db_query($sql, $DBValue)) {
                        drupal_set_message("failed to update record", 'error');
                        return;
                    }
                }
            } elseif ($do == 'add') {
                $sql = "insert into maenna_people_data (pid, access, data_type, data_attr, data_value, data_value2, data_value3,editorid) values (%d, '%s','%s','%s','%s','%s','%s',%d)";
                $DBValue = array($pid, $time, 'progress', $followup, $contact, $text, $date, $editorid);
                if (!db_query($sql, $DBValue)) {
                    drupal_set_message("Failed to add new record", 'error');
                    return;
                }
            }
        } elseif ($do == 'delete') {
            if (empty($dataid)) {
                drupal_set_message('invalud record id', 'error');
                return;
            }
            $sql = "delete from maenna_people_data where data_type = 'progress' and dataid = %d";
            if (db_query($sql, array($dataid))) {
                drupal_set_message("the record is removed");
            } else {
                drupal_set_message("failed to remove the record", 'error');
            }
            return;
        }
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function ifAdmin($uname) {
    $result = mysql_query("SELECT *  FROM users u WHERE u.name = '" . $uname . "' AND EXISTS (SELECT * FROM users_roles WHERE uid = u.uid AND rid IN (SELECT rid FROM role WHERE name = 'Super admin' OR name = 'Maennaco admin'))");
    if (mysql_num_rows($result) > 0) {
        return true;
    } else return false;
}

function invest_activity($op = null) {
    global $user, $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $Block['title'] = "<a href='/account?tab=investments&id=$pid' style='font-size:14px;text-transform:capitalize !important;'>Investment Activity</a>";
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $panel = 'pro_hourStats_panel';
    //if($op == 'write')$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>Edit</a></div>";

    if('people' == $AccessObj->user_type && $_REQUEST['id'] != $user->uid)
        return '';

    $content = '';

    $Block['body'] = sidebar_box($Block['title'], $content, 'ev_calendar');

    return $Block;
}

function pro_eventCalendar($op = null) {
    global $user, $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $Block['title'] = "<span style='font-size:14px;text-transform:capitalize !important;'>Service participation</span>";
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $panel = 'pro_hourStats_panel';
    //if($op == 'write')$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>Edit</a></div>";

    if('people' == $AccessObj->user_type && $_REQUEST['id'] != $user->uid)
        return '';

    $content = '';
    if (empty($op)) return '';
    $content .= "<div class='entry' style='border:none;padding-bottom: 20px;'>";
    if (true) {

        $db = \Clewed\Db::get_instance();
        $expertInvitations = $db->get_array("
            SELECT *
            FROM maenna_company_events
            WHERE executor_id = ?
            AND TIMESTAMPDIFF(MONTH, DATE(FROM_UNIXTIME(datetime)), DATE(NOW())) <= 3
            AND approved = 1
            AND (
                executor_status <> 'declined'
                OR executor_status IS NULL
            )",
            array($editorid)
        );

        $rows = array();

        $userService = new \Clewed\User\Service();
        foreach($expertInvitations ?: array() as $i => $event) {
            $company = $userService->get(array($event['companyid']));
            $company = $company[$event['companyid']];
            if('confirmed' == $event['executor_status'])
                $event['executor_status'] = 'yes';
            if('declined' == $event['executor_status'])
                $event['executor_status'] = 'no';
            $href = "/account?tab=companies&page=company_detail&id={$event['companyid']}&mtab=advice&view_id={$event['eventid']}";
            if (empty($event['executor_status'])) {
                $rsvp = '<span class="status"><a target="_blank" href="' . $href . '&rsvp=1"><b style="color: #00A2BF;">Confirm</b></a></span>';
                $rsvpParam = '&rsvp=1';
            }
            else {
                $rsvp = '<span class="status">' . strtoupper($event['executor_status']) .'</span>';
                $rsvpParam = '';
            }

            $href .= $rsvpParam;
            $rows[] = array(
                'date' => $event['datetime'],
                'content' => '
                    <div class="column advisory-sessions-block-new row" style="width:100%;">
                        <a class="project-name" target="_blank" href="' . $href . '" >' . ucwords($company['full_name']) . '</a>
                        <span class="service-deadline">' . (empty($event['datetime']) || date('Y', $event['datetime']) <= '1970' ? '' : date("m/d/Y", $event['datetime'])) . '</span>
                        ' . $rsvp. '
                    </div>
                    <br style="clear:both" />'
            );
        }

        $eventRes = mysql_query(
            "SELECT *, inv.status conf 
             FROM maenna_company_events_inv inv 
             JOIN maenna_company_events ev ON ev.eventid = inv.eventid
             WHERE TIMESTAMPDIFF(MONTH, DATE(FROM_UNIXTIME(datetime)), DATE(NOW())) <= 3
             AND ev.approved = 1 
             AND inv.status <> 'declined' 
             AND uid = '" . $pid . "' 
             ORDER BY datetime DESC"
        ) or die(mysql_error());
        $conn_arr = getConnectedForDiscussions($pid);

        $cnt = 1;
        while ($event = mysql_fetch_array($eventRes)) {
            if (!in_array($event['companyid'], $conn_arr)) continue;
            $company = $userService->get(array($event['companyid']));
            $company = $company[$event['companyid']];
            if('confirmed' == $event['conf'])
                $event['conf'] = 'yes';
            if('declined' == $event['conf'])
                $event['conf'] = 'no';
            $href = "/account?tab=companies&page=company_detail&id={$event['companyid']}&mtab=advice&view_id={$event['eventid']}";
            if ($event['conf'] == 'sent') {
                $rsvp = '<span class="status"><a target="_blank" href="' . $href . '&rsvp=1"><b style="color: #00A2BF;">Confirm</b></a></span>';
                $rsvpParam = '&rsvp=1';
            }
            else {
                $rsvp = '<span class="status">' . strtoupper($event['conf']) .'</span>';
                $rsvpParam = '';
            }

            $href .= $rsvpParam;
            $rows[] = array(
                'date' => $event['datetime'],
                'content' => '
                    <div class="column advisory-sessions-block-new row" style="width:100%;">
                        <a class="project-name" target="_blank" href="' . $href . '" >' . ucwords($company['full_name']) . '</a>
                        <span class="service-deadline">' . (empty($event['datetime']) || date('Y', $event['datetime']) <= '1970' ? '' : date("m/d/Y", $event['datetime'])) . '</span>
                        ' . $rsvp. '
                    </div>
                    <br style="clear:both" />'
            );

            $cnt++;
        }

        usort($rows, function($a, $b) {
            return $a['date'] == $b['date'] ? 0 : ($a['date'] > $b['date'] ? -1 : 1);
        });

        foreach($rows as $row)
            $content .= $row['content'];

//        if ($events = mysql_fetch_array($eventRes) || $cnt == 3) {
//            $content .= "<a id = \"moreEvents\" class=\"tool\" style = \"cursor:pointer;display: block;text-align: right;font-size: 14px;margin-right: -11px;\">| More</a>
//    <script>
//    $(document).ready(function(){
//
//    $(\"#moreEvents\").click(function() {
//
//                        var a = $(this);
//
//    			$.post(\"/themes/maennaco/includes/posts.php?type=moreEventsCalendar&u=" . $editorid . "\", {
//
//				}, function(response){
//
//                                    a.parent().append(response);
//                                    a.remove();
//
//
//				});
//
//    });
//
//    });
//    </script>
//
//    ";
//        }
    } else {
        $content .= "";
    }
    $content .= "</div>";
    $content .= '<div class="separator"></div>';
    $Block['body'] = sidebar_box($Block['title'], $content, 'ev_calendar');

    return $Block;
}

function pro_hourStats($op = null) {
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $pro_id = sget($_REQUEST, 'pro_id');
    $Block['title'] = 'STATISTICS';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $panel = 'pro_hourStats_panel';
    //if($op == 'write')$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>Edit</a></div>";
    $content = '';
    if (empty($op)) return '';
    $data_type = "activitystats";
    $DATA_ATTR = array(
        'advisory' => "Advisory",
        'sproj'    => "Special Projects",
        'analysis' => "Analysis",
        'proref'   => "Pro. Referrals",
        'coref'    => "Co. Referrals"
    );
    $Keys = array_keys($DATA_ATTR);
    $Data = array();
    $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' and data_attr = '%s' ";
    for ($i = 0; $i < 3; $i++) {
        $total_hr = 0;
        $num_of_company = 0;
        $data_attr = $Keys[$i];
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        while ($Row = db_fetch_array($result)) {
            $total_hr += sget0($Row, 'data_value2');
            $num_of_company += 1;
        }
        $Data["$data_attr"] = array($total_hr . " Hrs", $num_of_company . " Co.");
    }
    $content .= "<div class='entry' style='border:none'>";
    foreach ($DATA_ATTR as $key => $title) {
        $val1 = $Data["$key"][0];
        $val2 = $Data["$key"][1];
        $title = ucwords($title);
        $content .= "\n     <div class='column row' style='width:60%;'>$title</div>
                            <div class='row column' style='width:25%;'>$val1 &nbsp;</div>
                            <div class='row column' style='width:15%;'>$val2 &nbsp;</div><br style='clear:both' />";
    }
    if ($pro_id != '') {
        $sql_like = db_query("select * from like_discussion_professional where prof_id = " . $pro_id . "");
        $Rowlike = mysql_num_rows($sql_like);
        $content .= "\n     <div class='column row' style='width:60%;'>DISCUSSION LIKES</div>
                            <div class='row column' style='width:25%;'>$Rowlike &nbsp;</div>";
    }
    $content .= "</div>";
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function pro_hourStats_panel($op = null) {
}

function pro_collaboration($op = null) {
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $Block['title'] = 'STATISTICS';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $panel = 'pro_hourStats_panel';
    //if($op == 'write')$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>Edit</a></div>";
    $content = '';
    if (empty($op)) return '';
    $data_type = "activitystats";
    $DATA_ATTR = array(
        'advisory' => "Advisory",
        'sproj'    => "Special Projects",
        'analysis' => "Analysis",
        'proref'   => "Pro. Referrals",
        'coref'    => "Co. Referrals"
    );
    $Keys = array_keys($DATA_ATTR);
    $Data = array();
    $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' and data_attr = '%s' ";
    for ($i = 0; $i < 3; $i++) {
        $total_hr = 0;
        $num_of_company = 0;
        $data_attr = $Keys[$i];
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        while ($Row = db_fetch_array($result)) {
            $total_hr += sget0($Row, 'data_value2');
            $num_of_company += 1;
        }
        $Data["$data_attr"] = array($total_hr . " Hrs", $num_of_company . " Co.");
    }
    $content .= "<div class='entry' style='border:none'>";
    foreach ($DATA_ATTR as $key => $title) {
        $val1 = $Data["$key"][0];
        $val2 = $Data["$key"][1];
        $title = ucwords($title);
        $content .= "\n     <div class='column row' style='width:60%;'>$title</div>
                            <div class='row column' style='width:25%;'>$val1 &nbsp;</div>
                            <div class='row column' style='width:15%;'>$val2 &nbsp;</div><br style='clear:both' />";
    }
    $content .= "</div>";
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function pro_selectWork($op = null) {
    return '';
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $Block['title'] = 'Readings / Ideas';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $content = '';
    if (empty($op)) return '';
    $data_type = 'proselectwork';
    $panel = 'file_and_links';
    if ($op == 'write' || ($user->uid == $pid)) {
        $Block['title'] .= "<div  class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>Add</a></div>";
    }
    $content = '';
    if ($op) {
        $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' order by dataid desc";
        $result = db_query($sql, array($pid, $data_type));
        while ($Row = db_fetch_object($result)) {
            $title = ucwords($Row->data_value);
            $title = htmlentities($title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $temp = $Row->data_value2;
            if ($Row->data_attr == 'file') {
                $temp = '/account?tab=professionals&page=pro_detail&id=' . $Row->pid . '&type=fileview&file=' . $temp . '&fid=' . $Row->dataid . '&root=1';
            }
            if ($temp) {
                $content .= "<div style='width:236px; padding-bottom: 7px;' class='row singlerow'><a href='$temp' target='_blank'>" . ucwords(strtolower($title)) . "</a><div style='top:5px !important;margin-top:-3px;' class='editbtn'><a class='tool' href='account?tab=professionals&page=pro_detail&id=" . $_REQUEST['id'] . "&update_section=file_and_links&do=del&dataid=" . $Row->dataid . "'>Del</a></div></div>";
            }
        }
    }
    $Block['body'] = sidebar_box($Block['title'], $content, 'readings-ideas');
    return $Block;
}

function file_and_links($op = null) {
    require_once("classes/connections.inc");
    global $user, $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    if ($AccessObj->user_type == 'super' ||
        ($AccessObj->user_type == 'admin' && Connections::admin_of($pid)) ||
        $AccessObj->uid == $pid
    ) {
        // continue
    } else return '';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    //$HV = hidden_post_values(array('tab','page','id'));
    $content = '';
    $data_type = 'proselectwork';
    $content = '';
    $Block['title'] = 'READINGS / IDEAS';
    //$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>Add</a></div>";
    if (empty($op)) return '';
    if ($op && $op != 'update') {
        $view = sget($_REQUEST, 'view');
        if ($view == 'listview' || empty($view)) {
            $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' order by dataid desc";
            $result = db_query($sql, array($pid, $data_type));
            $content .= "<table class='report-table'>";
            while ($Row = db_fetch_object($result)) {
                $title = $Row->data_value;
                $type = $Row->data_value3;
                if ($type == 'link') {
                    $content .= "<tr><td></td><td></td></tr>";
                }
            }
            $content .= "</table>";
        } elseif ($view == 'add' || $view == 'edit') {
            $dataid = sget($_REQUEST, 'dataid');
            $Row = array();
            if ($dataid) {
                $sql = "select * from maenna_people_data where dataid = %d";
                $result = db_query($sql, $dataid);
                $Row = db_fetch_array($result);
            }
            if (count($Row) == 0) {
                $content = <<< END
                    <form method=post action='/account?tab=professionals&page=pro_detail&id=$pid' enctype='multipart/form-data' id="readingideas-form">
                        <div style='margin-top:15px;'>
                            <label>TITLE:</label><br /><input type=text id="title" name=title value='' style='width:300px;' />
                        </div>
                        <div style='margin-top:10px;'>Please upload a file OR enter a web link:<br>
                        <div style='margin-top:8px;'>File: <br ><input type=file id="file" name=file value='' /> </div>
                        <div style='margin-top:8px;'>Web Link: <br ><input type=text id="url" name=url value='' style='width:300px;'  /></div>
                        <br />
                        <input type='submit' name=submit value=submit class=button /> <a href='$redirect' class=button>cancel</a>
                        <input type='hidden' name='update_section' value='file_and_links' />
                        <input type='hidden' name='do' value='new' />
                    </form>
                    <script type="text/javascript">
                        $('#readingideas-form').submit(function(e){
                            if($('#title').val() == "") {
                                e.preventDefault();
                                $('#title').css('border', '1px solid red');
                            } else if($('#file').val() == "" && $('#url').val() == "") {
                                e.preventDefault();
                                $('#file').css('border', '1px solid red');
                            }
                        });
                    </script>
END;
                $content .= js_init("init_openbox();");
            }
        }
    } elseif ($op == 'update') {
        $do = sget($_REQUEST, 'do');
        if ($do == 'new') {
            $title = sget($_REQUEST, 'title');
            $url = sget($_REQUEST, 'url');
            if ($title && $url) {
                $data_attr = 'url';
                $data_value = $title;
                $data_value2 = $url;
            } elseif ($title && isset($_FILES['file']) && ($_FILES['file']['error'] == 0)) {
                $file = $_FILES['file']['name'];
                $filename = $time . '_' . clearString($file);
                $path = "./" . file_directory_path() . "/" . $filename;
                if (move_uploaded_file($_FILES['file']['tmp_name'], $path)) {
                    $data_attr = 'file';
                    $data_value = $title;
                    $data_value2 = $filename;
                } else {
                    drupal_set_message("File uploading failed", 'error');
                }
            } else {
                drupal_set_message("Please complete the form and try again", 'error');
                return;
            }
            $sql = "insert into maenna_people_data (pid, access, data_type, data_attr, data_value, data_value2) values (%d, '%s', '%s', '%s', '%s','%s')";
            $DBValues = array($pid, $time, $data_type, $data_attr, $data_value, $data_value2);
            if (!db_query($sql, $DBValues)) {
                drupal_set_message("Failed to add new record", 'error');
            } else {
                drupal_set_message("New record added successfully");
            }
        } elseif ($do == 'del') {
            $dataid = sget($_REQUEST, 'dataid');
            if ($dataid) {
                $sql = "delete from maenna_people_data where dataid = %d limit 1";
                if (!db_query($sql, array($dataid))) {
                    drupal_set_message("Operation Failed", 'error');
                } else {
                    drupal_set_message("Record deleted successfully");
                }
            } else {
                drupal_set_message("Invalid dataid", 'error');
            }
        }
        return;
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function pro_bio($op = null) {
    require_once("classes/connections.inc");
    global $user, $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Bio";
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    global $user, $AccessObj;
    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid))) {
        //continue;
    } else {
        return '';
    }
    if (empty($op)) return '';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $content = '';
    $panel = 'bio_panel';
    $Block['title'] .= "<div style='top:243px !important;' class=editbtn><a href='$redirect&panel=${panel}&view=detail' class=tool>View</a></div>";
    $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
    return $Block;
}

function bio_panel($op = null) {
    require_once("classes/connections.inc");
    global $user, $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Bio";
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid))) {
        //continue;
    } else {
        return '';
    }
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $content = '';
    $panel = 'bio_panel';
    $HV = hidden_post_values(array('tab', 'page', 'id'));
    if ($op && $op != 'update') {
        $view = sget($_REQUEST, 'view');
        $sql = "select * from maenna_people where pid = %d";
        $result = db_query($sql, array($pid));
        $Row = db_fetch_object($result);
        $text = '';
        if ($Row) {
            $text = $Row->brief_intro;
        }
        if ($view == 'detail') {
            $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>Edit</a></div>";
            $content = nl2br(htmlentities($text, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $content = _filter_autop(html_entity_decode($content));
            $content .= "<br /><br /><a href='$redirect' class='button'>back</a>";
        } elseif ($view == 'edit') {
            $Block['title'] = "Edit - " . $Block['title'];
            $content = "<form action='/account' method=post>
                    <textarea name='text' style='width:99%;height:400px;'>$text</textarea>
                    <br>
                    <input type=submit name=submit value=submit /> <a href='$redirect' class='button'>back</a>
                    $HV
                    <input type=hidden name=update_section value='$panel' />
                    <input type=hidden name=panel value='$panel' />
                    <input type=hidden name=section value='pro_bio' />
                    <input type=hidden name=view value='detail' />
                </form>
            ";
        }
    } elseif ($op == 'update') {
        $text = sget($_REQUEST, 'text');
        $sql = "update maenna_people set access = '%s', editorid = %d, brief_intro = '%s' where pid = %d limit 1";
        if (db_query($sql, array($time, $editorid, $text, $pid))) {
            drupal_set_message("Recrod updated");
        } else {
            drupal_set_message("Operation failed", 'error');
        }
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function manage_connections($op = null) {
    global $AccessObj;
    $id = sget($_REQUEST, 'id');
    if (empty($id)) return '';
    if ($AccessObj->user_type != 'super' && (!Connections::admin_of($id))) return '';
    $section = __FUNCTION__;
    $editorid = $AccessObj->uid;
    $time = time();
    $redirect = rebuild_url(array('tab', 'page', 'id', 'mtab'));
    $HV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
    $Block['title'] = "<span style='font-size:14px;color: #284B54;font-family: Lato Bold Italic;text-transform:capitalize !important;'>Manage Connections</span>";
    ///$Block['title'] .= "<div class=editbtn><a href='$redirect&section=${section}&panel=mcp' class=tool>Edit</a></div>";
    $content = "<div class=row><a href='$redirect&section=${section}&panel=mcp&type=com'>Company Connections</a></div>
                <div class=row><a href='$redirect&section=${section}&panel=mcp&type=pro'>Professional Connections</a></div>
                <div class='separator'></div>
                ";
    $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
    return $Block;
}

function mcp($op = null) {
    require_once("classes/maenna_users.inc");
    require_once("classes/connections.inc");
    global $AccessObj;
    $id = sget($_REQUEST, 'id');
    if (empty($id)) return '';
    if ($AccessObj->user_type != 'super' && (!Connections::admin_of($id))) return '';
    $type = sget($_REQUEST, 'type');
    $section = __FUNCTION__;
    $redirect = rebuild_url(array('tab', 'page', 'id', 'mtab'));
    $HV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
    $Block['title'] = "Manage Connections";
    if ($op && $op != 'update') {
        if ($type == 'com') {
            $Block['title'] .= " - Company Connections";
        } else $Block['title'] .= " - Professional Connections";
        $Admin_conns = Connections::admin_connections($AccessObj->uid);
        $User_conns = Connections::Pro_conns($id);
        //test_array($User_conns);
        $TR = '';
        if ($type == 'pro') {
            $Options = array(
                'none'    => '----',
                'related' => 'Visible'
            );
            foreach ($Admin_conns['Pro'] as $Conn) {
                $pro_id = $Conn->target_uid;
                if ($pro_id == $id) continue;
                $maennaid = getProId($pro_id);
                $conn_id = '';
                $relation = 'none';
                foreach ($User_conns['Related'] as $User_conn) {
                    if ($User_conn->assignee_uid == $pro_id) {
                        $conn_id = $User_conn->connid;
                        $relation = 'related';
                        break;
                    }
                }
                $select = "<select name='REL[$pro_id][conntype]'>" . option_code($Options, $relation) . " </select>";
                $TR .= "\n<tr><td>$maennaid <input type='hidden' name='REL[$pro_id][connid]' value='$conn_id' /></td><td>$select</td></tr>";
            }
        } elseif ($type == 'com') {
            $Options1 = array(
                'none'    => '----',
                'visible' => 'Visible',
                'advisor' => 'Advisor',
                'propose' => 'Propose as advisor'
            );
            $Options2 = array(
                'none'    => '----',
                'visible' => 'Visible',
                'advisor' => 'Advisor',
            );
            foreach ($Admin_conns['Com'] as $Admin_conn) {
                $com_id = $Admin_conn->target_uid; //
                $maenna_id = getProjectName($com_id);
                $conn_id = '';
                $relation = 'none';
                $Options = $Options1;
                foreach ($User_conns as $key => $User_conn) {
                    $found = false;
                    if (in_array($key, array('Advisor', 'Propose', 'Visible'))) {
                        foreach ($User_conn as $_Conn) {
                            if ($_Conn->target_uid == $com_id) {
                                $conn_id = $_Conn->connid;
                                $relation = $_Conn->conntype;
                                echo "$com_id, $id, $relation<br>";
                                if (strcasecmp($relation, 'advisor') == 0) {
                                    $Options = $Options2;
                                }
                                $found = true;
                                break;
                            }
                        }
                    }
                    if ($found) break;
                }
                $select = "<select name='REL[$com_id][conntype]'>" . option_code($Options, $relation) . " </select>";
                $TR .= "\n<tr><td>$maenna_id <input type='hidden' name='REL[$com_id][connid]' value='$conn_id' /></td><td>$select</td></tr>";
            }
        }
        $content = <<< END
        <form method=post action='/account'>
            <table cellspacing=0 cellpadding=0 class='table-list'>
                <tr style='background:#f2f2f2;'><td>Maenna ID</td><td>Connection Type</td></tr>
                $TR
            </table>
            <input type=submit name=submit value=submit class=button /> <a href='$redirect' class=button>cancel</a>
            $HV
            <input type=hidden name=type value='$type' />
            <input type=hidden name=section value='manage_connections' />
            <input type=hidden name=panel value='$section' />
            <input type=hidden name=update_section value='$section' />
        </form>
END;
    } elseif ($op == 'update') {
        $REL = $_POST['REL'];
        //test_array($REL);
        if ($type == 'pro') {
            foreach ($REL as $assignee_uid => $Rel) {
                $connid = sget($Rel, 'connid');
                $conntype = sget($Rel, 'conntype');
                if (!Connections::set_pro_connection($id, $assignee_uid, $connid, $conntype)) // current pro id as target_uid
                {
                    //test_array($Rel);
                    //test_array(array($target_uid, $id, $connid, $conntype));
                    drupal_set_message("Failed to set connections", 'error');
                    return;
                }
            }
            drupal_set_message("Operation Successful");
            return;
        } elseif ($type == 'com') {
            foreach ($REL as $com_id => $Rel) {
                $connid = sget($Rel, 'connid');
                $conntype = sget($Rel, 'conntype');
                if ($conntype == 'advisor' || $conntype == 'propose') {
                    if (!Connections::set_pro_connection($id, $com_id, $connid, $conntype)) {
                        drupal_set_message("Failed to set $conntype connections", 'error');
                        return;
                    }
                } else {
                    if (!Connections::set_pro_connection($id, $com_id, $connid, $conntype)) {
                        drupal_set_message("Failed to set $conntype connections", 'error');
                        return;
                    }
                }
            }
            drupal_set_message("Operation Successful");
            return;
        }
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function pro_questionnaire($op = null) {
    require_once("classes/connections.inc");
    global $user, $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "<span style='font-size:14px;color: #284B54;font-family: Lato Bold Italic;text-transform:capitalize !important;'>Pro Questionnaire</span>";
    global $user, $AccessObj;
    $roleId = userRoleId($pid);
    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid))) {
        //continue;
    } else {
        return '';
    }
    if (empty($op)) return '';
    $content = '';
    $sql = "select recordid from maenna_questionair where target='pro' and usertype = %d and status = 1 and type = 'version' limit 1";
    $result = db_query($sql, array($roleId));
    $Row = db_fetch_object($result);
    if ($Row) {
        $parentid = $Row->recordid;
    } else return '';
    $Block['title'] .= "<div class=editbtn><a href='/account?tab=information&page=qmgmt&panel=answerdetail&target=pro&id=$parentid&uid=$pid' class=tool>View</a></div>";
    $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
    return $Block;
}

function pro_createprofile($op = null) {
    require_once("classes/connections.inc");
    global $user, $AccessObj;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $Block['title'] = "<span style='font-size:14px;color: #284B54;font-family: Lato Bold Italic;text-transform:capitalize !important;'>Create Profile</span>";
    global $user, $AccessObj;
    $roleId = userRoleId($pid);
    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid))) {
        //continue;
    } else {
        return '';
    }
    //if(empty( $op )) return '';
    $content = '';
    $Block['title'] .= "<div class=editbtn><a href='/account?tab=professionals&page=pro_detail&id=$pid&section=pro_industry_view' class=tool>View</a></div>";
    $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
    return $Block;
}

function pro_participating($op = null) {
    global $user, $AccessObj;
    if (empty($op)) {
        return '';
    }
    $db = \Clewed\Db::get_instance();
    //$editorid = $user->uid;
    //$pid = (int) sget($_REQUEST, 'id');
    $pro_id = (int) sget($_REQUEST, 'pro_id');

    $attendees = array();
    $insightService = new Clewed\Insights\InsightService();
    $userService = new Clewed\User\Service();
    $attendeeIds = $insightService->getAttendeeIds(array($pro_id));
    if(!empty($attendeeIds[$pro_id]))
        $attendees = $userService->get(array_values($attendeeIds[$pro_id]));

    $count = count($attendees);
    if ($count < 5 && !in_array($AccessObj->user_type, array('super', 'admin')) && $AccessObj->uid != $_REQUEST['id']) {
        return '';
    }
    $Block['title'] = '<span class="rghttd_title">Joined (' . $count . ')</span>';
    //$redirect = rebuild_url(array('tab', 'page', 'id'));
    //$content = '';
    //if (empty($op)) return '';
    ob_start();
    //$url = "http://" . $_SERVER['HTTP_HOST'];
    //$editorname = $user->name;
    require 'pro_participation.php';
    $content = ob_get_clean();
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function discussion_join() {
    global $user, $AccessObj;
    $db = \Clewed\Db::get_instance();
    $proId = (int) sget($_REQUEST, 'pro_id');
    $insight = $db->get_row(
        'SELECT * FROM `maenna_professional` WHERE `id` = :id LIMIT 1',
        array(':id' => $proId)
    );
    if (empty($insight)) {
        return;
    }
    $userType = $AccessObj->user_type;
    $userId = $user->uid;
    $paymentsCount = $db->get_column(
        'SELECT COUNT(*) FROM `maenna_professional_payments` WHERE `pro_id` = :id AND `user_id` = :uid',
        array(':id' => $proId, ':uid' => $userId)
    );
    $totalPaymentsCount = $db->get_column(
        'SELECT COUNT(*) FROM `maenna_professional_payments` WHERE `pro_id` = :id',
        array(':id' => $proId)
    );
    $spotsCount = $db->get_column(
        'SELECT `spots` FROM `maenna_professional` WHERE `id` = :id LIMIT 1',
        array(':id' => $proId)
    );
    $capacityCount = $db->get_column(
        'SELECT `capacity` FROM `maenna_professional` WHERE `id` = :id LIMIT 1',
        array(':id' => $proId)
    );    $spotsCount = $db->get_column(
        'SELECT `spots` FROM `maenna_professional` WHERE `id` = :id LIMIT 1',
        array(':id' => $proId)
    );
    if ((int) $insight['cost'] > 0) {
        $costClear = '$&thinsp;' . number_format((int) str_replace(',', '', $insight['cost']));
    } else {
        $costClear = 'Free';
    }
    $cost = $insight['type'] == 0 ? 'Join ' . $costClear : ($insight['cost'] > 0 ? 'Buy ' . $costClear : 'Join Free');
    $isFollower = (bool) $db->get_column(
        'SELECT COUNT(*) FROM `maennaco_discussion_follow` WHERE `prof_id` = :id AND `user_id` = :user_id LIMIT 1',
        array(':id' => $proId, ':user_id' => $userId)
    );

    $noFollowing = (int) $db->get_column(
        'SELECT COUNT(*) FROM `maennaco_discussion_follow` WHERE `prof_id` = :id',
        array(':id' => $proId)
    );
    echo '<div class="join_section">';
    if ($isFollower)
        $atype = 'unfollow';
    else $atype = 'follow';

    $follow = '<div style="margin-left:auto;margin-right:auto;width:90px;margin-top:5px;" id="follow_dis"><a style="font-weight:bold;cursor:pointer;float:left;width:auto;'.($isFollower ? 'margin-left:3px;':'margin-left:12px;').'" data-ins="'.$insight['id'].'" data-uid="'.$userId . '" title="'.ucfirst($atype).'" atype = "'.$atype.'" class="tool follow">'.($isFollower ? 'Following':'Follow').'</a>';
    $follow.=
      $noFollowing >= 10?
        '<div style="font-weight:bold;padding-top: 1px;margin-left:5px;float:left;" class="foll_cnt">'.$noFollowing.'</div>'
        : '';
    $follow.= '</div>';


    $bOwnInsight = $user->uid == $insight['postedby'] ? 'true' : 'false';
//    $bOldInsight = $insight['datetime'] < time() ? 'true' : 'false';
    $bOldInsight = 'false';

    if ($bOwnInsight == 'true') {
        $costAsString = (int)$insight['cost'] > 0 ? ('$' . number_format((int) str_replace(',', '', $insight['cost']))) : 'Free';
        $approveStatusAsString = $insight['approve_status'] ? 'Listed: ' : 'Pending approval: ';
        echo '<p style="text-align: center;"><input type="button" class="join" style="border: none;' . ($insight['approve_status'] ? '' : 'width: 180px;') . '"  value="' . $approveStatusAsString . $costAsString . '"></p>';
    } elseif ((int) $totalPaymentsCount >= (int) $capacityCount && (int)$paymentsCount <= 0 ) {
        echo '<p style="text-align: center;"><input type="button" id="joinnow" class="join" value="Sold&nbsp;out"></p>';
    } elseif ($paymentsCount <= 0 && $spotsCount > 0) {
        if ($insight['cost'] == 0) {
            $onclick = "joinnow2($proId,$userId, $bOwnInsight, $bOldInsight);";
            echo "<p style='text-align: center;'><input type='button' onclick='".$onclick."' class='join' value='" . $cost . "'></p>";
        } else {
            $onclick = "to_join($bOwnInsight,$bOldInsight);";
            echo '<p style="text-align: center;"><input type="button" id="joinnow" onclick="'.$onclick.'" class="join offer-join-btn" value="' . $cost . '"><br/>';
            echo '<a href="#" id="enter-discount-code">Enter discount code</a>';

            require ROOT . '/themes/maennaco/dialogs/discount-code.php';
            echo '</p>';
        }
    }
    echo
        '<script type="text/javascript">
            $(function(){
                if("#join" == location.hash) {
                    location.hash = "";
                    $(".offer-join-btn").trigger("click");
                }
            });
        </script>';
    if ($paymentsCount <= 0) {
        require 'like_discussions.php';
        echo $follow;
    } else {
        if (!in_array($userType, array('super', 'admin'))) {
            echo '<p style="text-align: center;"><input type="button" class="join" value="You\'ve joined"></p>';
        }
        require 'like_discussions.php';
        echo $follow . '<br/>';
    }
    echo '</div>';
}

function pro_audio_preview() {

    global $user;
    global $AccessObj;
    $user_id = $user->uid;
    $ifAdmin = ifAdmin($user_id);

    $ifAttendee = ($checkUser['status'] == 1);
    $ifInsightOwner = ($row1['postedby'] == $user_id);
    $ifInsightAdmin = ($AccessObj->user_type == 'super' || $ifAdmin || ifDiscussionModerator($pro_id, $user->uid));
    $accessAllowed = $ifAttendee || $ifInsightOwner || $ifInsightAdmin;

    $body = '';

    $db = \Clewed\Db::get_instance();
    $proId = (int) sget($_REQUEST, 'pro_id');

    $insight = $db->get_row("
        SELECT * FROM
        maenna_professional
        WHERE id = :id
        LIMIT 1",
        array(':id' => $proId)
    );

    if (!empty($insight)) {

        $fileName = 'insights/audio-previews/' . $insight['id'] . '.mp3';

        if(is_readable(ROOT . 'sites/default/files/events_tmp/' . $fileName)) {

            $hash = sha1('hash' . time() . $fileName);
            $db->run('DELETE FROM `audio_files` WHERE DATE_ADD(`created`, INTERVAL 1 DAY) < NOW()');
            $db->run('
                INSERT INTO `audio_files` (`hash`, `file`)
                VALUES (:hash, :file)',
                array(
                    ':hash' => $hash,
                    ':file' => $fileName
                )
            );
            $audioPreviewPath = '/' . $hash . '.mp3';
            $playerNo = md5($audioPreviewPath . time());
            $full_media = '';

                //Getting images and links of event.
                $sql_images = mysql_query("SELECT * FROM wall_documents WHERE ref_id = '" . mysql_real_escape_string($_REQUEST['pro_id']) . "'");
                $allowedExtensions = array("txt", "xml", "doc", "docx", "xls", "xlsx", "rtf", "ppt", "pdf");
                $playerNo = 1;
                while ($images = mysql_fetch_assoc($sql_images)) {
                    if ($images['document_name'] != '') {
                        list($time_stamp, $file_name) = explode('_', $images['document_name'], 2);
                        list($name, $ext) = explode('.', $file_name);
                        $id        = $_REQUEST['id'];
                        $pro_id    = $_REQUEST['pro_id'];
                        $file_name = $images['document_name'];
                        if ($accessAllowed) {
                            $extension = end(explode('.', $file_name));
                            if (in_array($extension, $allowedExtensions)) {

                            $full_media .= "<a href=\"/account?tab=professionals&page=pro_detail&id=" .$_REQUEST['id'] ."&section=pro_industry_view&type=details&mtab=advice&view_id=" .$_REQUEST['view_id'] ."&file=" .$file_name ."&fid=" .$images['d_id'] ."&pro_id=" .$_REQUEST['pro_id'] ."\">" .$name ."</a>";
                             } elseif ($extension === 'mp3') {
                                $db = \Clewed\Db::get_instance();
                                // TODO remove after deploy
                                $db->run(
                                    'CREATE TABLE IF NOT EXISTS `audio_files` (`id` INT(11) NOT NULL AUTO_INCREMENT, `hash` CHAR(40) NOT NULL, `file` VARCHAR(300) NOT NULL, `created` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, PRIMARY KEY (`id`), UNIQUE KEY `hash` (`hash`), KEY `created` (`created`)) ENGINE = InnoDB DEFAULT CHARSET = utf8;'
                                );
                                $hash = sha1('hash' . time() . $file_name);
                                // Delete old hashes
                                $db->run('DELETE FROM `audio_files` WHERE DATE_ADD(`created`, INTERVAL 1 DAY) < NOW()');
                                $db->run('INSERT INTO `audio_files` (`hash`, `file`) VALUES (:hash, :file)', array(':hash' => $hash, ':file' => $file_name));
                                $audioFile = '/' . $hash . '.mp3';
                                $full_media .= "<audio controls preload type=\"audio/mpeg\" style=\"width: 100%\" src=\"" .$audioFile ."\">
                                    <script type=\"text/javascript\" src=\"/js/swfobject.js\"></script>
                                    <script type=\"text/javascript\">
                                        swfobject.registerObject('player','9.0.0');
                                        var flashvars = {
                                            height: '20',
                                            width: '200',
                                            file: '" .$audioFile ."'',
                                            backcolor: \"0x999999\",
                                            frontcolor: \"0xFFFFFF\",
                                            overstretch: \"none\",
                                            usefullscreen: \"false\",
                                            enablejs: \"true\",
                                            javascriptid: \"player\"
                                        };
                                        var params = { };
                                        var attributes = { };
                                        swfobject.embedSWF(\"/mediaplayer.swf\",\"audio_player" .$playerNo ."\",\"200\",\"20\",\"9.0.0\",\"\",flashvars,params,attributes);
                                    </script>
                                    <div id=\"audio_player" .$playerNo++ ."\"></div>
                                </audio>";
                            } else {
                                $full_media .= "<a href=\"/download.php?file_name=" .$file_name ."\">" .$name ."</a>";

                            }
                        } else {

                            $extension = end(explode('.', $file_name));
                            if('mp3' == $extension) {
                                $full_media .= "<div onmousedown = \"alert('This section is for people attending this insight. Please join to access information.')\" >
                                    <audio controls preload > Sorry, your browser is too old and is not supported anymore . Please update it and try again .</audio >
                                </div >";
                            } else {
                                $full_media .= "<a style=\"cursor:pointer;\"
                               onclick=\"alert('This section is for people attending this insight. Please join to access information.')\">" . $name . "</a>";

                            }
                        }
                    }
                }

            $body = <<<EOF
<div class="insight-audio-preview">
    <div class="insight-audio-preview-caption">Preview</div>
    <audio preload="auto" controls="controls" style="width: 100%" src="{$audioPreviewPath}">Sorry, your browser is too old and is not supported anymore. Please update it and try again.</audio>
<!--    <script type="text/javascript" src="/js/swfobject.js"></script>
    <script type="text/javascript">
        swfobject.registerObject("player","9.0.0");
        var flashvars = {
            height: "20",
            width: "200",
            file: "{$audioPreviewPath}",
            backcolor: "0x999999",
            frontcolor: "0xFFFFFF",
            overstretch: "none",
            usefullscreen: "false",
            enablejs: "true",
            javascriptid: "player"
        };
        var params = { };
        var attributes = { };
        swfobject.embedSWF("/mediaplayer.swf","audio_player{$playerNo}","200","20","9.0.0","",flashvars,params,attributes);
    </script>
    <div id="audio_player{$playerNo}"></div>-->
<div class="insight-audio-preview-caption">Full Audio</div>    
{$full_media}
</div>
EOF;
        }
    }

    return array(
        'body' => $body
    );
}

function pro_share($op = null) {
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $pro_id = sget($_REQUEST, 'pro_id');
    $sql = db_query("SELECT title,whyattend,approve_status FROM maenna_professional where id = %d", array($pro_id));
    $title = db_fetch_object($sql);
    if (mysql_num_rows($sql) == 0 || $title->approve_status != 1) return '';
    $Block['title'] = '<span class="rghttd_title">Share</span>';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $content = '';
    if (empty($op))
        return '';
    ob_start();
    if (strlen($title->whyattend) >= 150) {
        $disc_why = substr($title->whyattend, 0, 150) . "...";
    }
    else
        $disc_why = $title->whyattend;

    function escapeJavaScriptText($string) {
        return str_replace("\n", '\n', str_replace('"', '\"', addcslashes(str_replace("\r", '', (string) $string), "\0..\37'\\")));
    }

    $url = $_SERVER['HTTP_HOST'] . "/insights-and-services?id=" . $pro_id;
    $proto = $_SERVER['HTTPS'] ? 'https' : 'http';
    $content = '
    <script type="text/javascript" src="//s7.addthis.com/js/300/addthis_widget.js#pubid=ra-599d25270410f116"></script>
    <script type="text/javascript">
        var addthis_share = {

            url_transforms : {
                shorten: {
                    twitter: "bitly"
                }
            },
            shorteners : {
                bitly : {}
            },
            passthrough : {
                twitter: {
                    via: "clewed"
                }
            }
        }
    </script>
    <div class="addthis_inline_share_toolbox" data-description="'.$disc_why.'" data-media="https://www.clewed.com/share-logo.jpg" data-url="'.$proto . "://" . $url.'" data-title="'.$title->title.'"></div>
    <style>
    .at-share-btn-elements {
        margin-left:10px !important;
        margin-bottom:15px;
    }
</style>
    
    ';
    /*	$content = '
    <div>
    <a href="https://twitter.com/share" class="twitter-share-button" data-text="'.$title->title.'" data-url="'.$url.'" data-count="vertical">Tweet</a>
    <script>!function(d,s,id){var js,fjs=d.getElementsByTagName(s)[0];if(!d.getElementById(id)){js=d.createElement(s);js.id=id;js.src="//platform.twitter.com/widgets.js";fjs.parentNode.insertBefore(js,fjs);}}(document,"script","twitter-wjs");</script>

    <script src="//platform.linkedin.com/in.js" type="text/javascript">
      lang: en_US
    </script>
    <script type="IN/Share" data-url="'.$url.'" data-counter="top"></script>

    <div class="fb-share-button" data-href="'.$url.'" data-width="57px" data-type="box_count"></div></div></div><div style="height:16px;clear:both;">';*/
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function pro_popularQuestions($op = null) {
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $pro_id = sget($_REQUEST, 'pro_id');
    $result1 = mysql_query("SELECT * FROM  `maenna_professional` WHERE  id = '" . $pro_id . "' ORDER BY id DESC ");
    if (mysql_num_rows($result1) == 0) return '';
    $Block['title'] = '<span class="rghttd_title">Popular Questions</span>';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $content = '';
    //if(empty($op)) return '';
    ob_start();
    $url = "http://" . $_SERVER['HTTP_HOST'];
    $editorname = $user->name;
    require_once("pro_popular_qs.php");
    $content = ob_get_contents();
    ob_end_clean();
    //$content = sget($_REQUEST, 'file');
    /*    $Block['body'] = '<div class="box_title shaded_title"><span style="float:left;" class="rghttd_title">Popular Questions</span></div>
    <div class="box_content main_content">' . $content . '</div>';*/
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function pro_insights_stats ($op = null) {
    global $user, $AccessObj;
    $pid = sget($_REQUEST, 'id');
    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid)) || $AccessObj->uid == $_REQUEST['id']) {
        $Block['title'] = 'Insights Attended';
        $editorid = $user->uid;
        $db = \Clewed\Db::get_instance();

        $payments = $db->get_array("SELECT (SELECT SUM(amount) FROM maenna_professional_payments WHERE user_id = :user_id and status = 1) as total, mpp.*,
IF (mp.username_type = 1,mp.firstname,CONCAT(mp.firstname,' ', mp.lastname)) as display_name,
mp.gender,mps.postedby
FROM maenna_professional_payments mpp
  LEFT JOIN maenna_professional mps ON mpp.pro_id = mps.id
  LEFT JOIN maenna_people mp ON mps.postedby = mp.pid
WHERE user_id = :user_id and status = 1 LIMIT 5",
            array(':user_id' => $pid));
        if (count($payments) == 0) $content = 'No Insights attended yet.<div class="separator"></div>';
        else {
            $content = "<table class='payment_stats' style='margin:0 !important;'>";
            foreach ($payments as $payment) {
                $timestamp = date('n/j/y', $payment['date_created']);

                if (file_exists('sites/default/images/profiles/150x150/' . $payment['postedby'] . '.jpg')) { $avatar = 'sites/default/images/profiles/150x150/' . $payment['postedby'] . '.jpg';
                } else {
                    if ($payment['gender'] == 'm' || $payment['gender'] == '') { $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                    } else
                        $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                }

                /*if ($payment['user_type'] == 'people') $display_name = "<a href='#' id='pro_id" . $payment['user_id'] . "' ref='pro_id' style='color:#00a2bf;' class='profile_details'>" . ucfirst($payment['display_name']) . "</a>";
 else $display_name = ucfirst($payment['display_name']);*/
                /*$display_name = "<span style='color:#00a2bf;'>".ucfirst($payment['display_name'])."</span>";*/
                $content .= "<tr style='height:70px;border-bottom:solid 1px #d0d2d3;'><td data-tooltip='".$payment['display_name']."' style='vertical-align:middle;'><img src='".$avatar."' width='40px' height='40px'></td><td style='vertical-align:middle;padding-left:10px;'>" . $timestamp . "</td><td style='vertical-align:middle;text-align:right;padding-right:20px;'>$" . number_format($payment['amount'],1) . "</td></tr>";

            }
            $content .= "<tr style='height:70px;border-bottom:solid 1px #d0d2d3;'><td style='vertical-align:middle;'>Total:</td><td></td><td style='vertical-align:middle;text-align:right;padding-right:20px;'>$" . number_format((double)$payment['total'],1) . "</td></tr></table><div style='background-color:#FAFBFB'><a href='#' style='float:right;border-left: solid 1px #006274;height: 15px;padding-left: 3px;margin-top: 10px;margin-right:9px;' data-ins_id = '".$payment['user_id']."' data-hash='".md5('kyarata75:'. $user->uid . ':' . $AccessObj->user_type.':'.$payment['user_id'])."' data-uid='" . $user->uid . "' data-user-type='" . $AccessObj->user_type . "' data-utype='user'  class='tool payment_details'>More</a></div>";
            $content .= js_init('init_payment_details();init_payment_details_dialog();');
            $content .= "<div id='payment_details_dialog' style='display:none;'></div>
            ";
        }
        $Block['body'] = sidebar_box($Block['title'], $content,'css_admin_box');
        return $Block;
    } else {
        return '';
    }
}

function pro_payment_stats ($op = null) {
    global $user, $AccessObj;
    $pid = sget($_REQUEST, 'id');
    $pro_id = sget($_REQUEST,'pro_id');
    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid)) || $AccessObj->uid == $_REQUEST['id']) {
        $Block['title'] = 'Payment Summary';
        $editorid = $user->uid;
        $db = \Clewed\Db::get_instance();

        $payments = $db->get_array("SELECT (SELECT SUM(amount) FROM maenna_professional_payments WHERE pro_id = :pro_id and status = 1) as total, mpp.*,IF(mc.companyid is null, (IF (mp.username_type = 1,mp.firstname,CONCAT(mp.firstname,' ', mp.lastname))),(IF (mc.projname <> '',mc.projname,CONCAT('PROJECT ',mc.companyid+100)))) as display_name,IF(mc.companyid is null, 'people','company') as user_type,
  IF(mc.companyid is null, '',ma.project) as project,
  IF(mc.companyid is null, mp.gender,'') as gender
FROM maenna_professional_payments mpp
  LEFT JOIN maenna_people mp ON mpp.user_id = mp.pid
  LEFT JOIN maenna_company mc ON mpp.user_id = mc.companyid
  LEFT JOIN maenna_about ma ON mpp.user_id = ma.project_id
WHERE pro_id = :pro_id and status = 1 LIMIT 5",
            array(':pro_id' => $pro_id));
        if (count($payments) == 0) $content = 'No payments yet.';
        else {
            $content = "<table class='payment_stats' style='margin:0 !important;'>";
            foreach ($payments as $payment) {
                $timestamp = date('n/j/y', $payment['date_created']);

                if ($payment['user_type'] == 'company') {
                    if (!empty($payment['project']) && file_exists('./themes/maennaco/images/project/'. $payment['project'])) {
                        $avatarFileName = "project/" . $payment['project'];
                        $avatar = "/themes/maennaco/phpthumb/phpThumb.php?src=../images/" . $avatarFileName . "&zc=1&w=40&h=40";
                    }
                    else {
                        $avatar =' /themes/maennaco/images/cmp-avatar-product.png';
                    }
                }
                else {
                    if (file_exists('sites/default/images/profiles/150x150/' . $payment['user_id'] . '.jpg')) { $avatar = 'sites/default/images/profiles/150x150/' . $payment['user_id'] . '.jpg';
                    } else {
                        if ($payment['gender'] == 'm' || $payment['gender'] == '') { $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                        } else
                            $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                    }
                }
                /*if ($payment['user_type'] == 'people') $display_name = "<a href='#' id='pro_id" . $payment['user_id'] . "' ref='pro_id' style='color:#00a2bf;' class='profile_details'>" . ucfirst($payment['display_name']) . "</a>";
 else $display_name = ucfirst($payment['display_name']);*/
                /*$display_name = "<span style='color:#00a2bf;'>".ucfirst($payment['display_name'])."</span>";*/
                $content .= "<tr style='height:70px;border-bottom:solid 1px #d0d2d3;'><td data-tooltip='".$payment['display_name']."' style='vertical-align:middle;'><img src='".$avatar."' width='40px' height='40px'></td><td style='vertical-align:middle;padding-left:10px;'>" . $timestamp . "</td><td style='vertical-align:middle;text-align:right;padding-right:20px;'>$" . number_format($payment['amount'],1) . "</td></tr>";

            }
            $content .= "<tr style='height:70px;border-bottom:solid 1px #d0d2d3;'><td style='vertical-align:middle;'>Total:</td><td></td><td style='vertical-align:middle;text-align:right;padding-right:20px;'>$" . number_format((double)$payment['total'],1) . "</td></tr></table><div style='background-color:#FAFBFB'><a href='#' style='float:right;border-left: solid 1px #006274;height: 15px;padding-left: 3px;margin-top: 10px;margin-right:9px;' data-uid='" . $user->uid . "' data-user-type='" . $AccessObj->user_type . "' data-ins_id = '".$payment['pro_id']."' data-hash='".md5('kyarata75:' . $user->uid . ':' . $AccessObj->user_type . ':' . $payment['pro_id'])."' class='tool payment_details'>More</a></div>";
            $content .= js_init('init_payment_details();init_payment_details_dialog();');
            $content .= "<div id='payment_details_dialog' style='display:none;'></div>";
        }
        $Block['body'] = sidebar_box($Block['title'], $content, 'shade_background');
        return $Block;
    } else {
        return '';
    }
}

function pro_service_payments_summary ($op = null) {
    global $user, $AccessObj, $base_url;

    $expertId = sget($_REQUEST, 'id');

    $isSuper = $AccessObj->user_type == 'super';
    $isAdmin = $AccessObj->user_type == 'admin' && Connections::admin_of($expertId);
    $isExpert = $user->uid == $_REQUEST['id'];

    if(!$isSuper && !$isAdmin && !$isExpert)
        return '';

    $title = 'Services led';
    $body = sidebar_box($title, 'No order activity<div class="separator"></div>', 'project-service-payments-summary');

    $companyService = new \Clewed\Company\Service();
    $services = $companyService->getServicesByConfirmedLeadId($expertId);
    if (!empty($services)) {

        $funded = 0;
        $total = 0;
        $fundedServices = array();
        foreach($services as $serviceId => $service) {
            $budget = ceil($service['budget']);
            $services[$serviceId]['budget'] = number_format($budget);
            $total += $budget;
            if($service['isFunded']) {
                $funded += $budget;
                $fundedServices[] = $services[$serviceId];
            }
        }

        $templatesEngine = new Twig_Environment(new Twig_Loader_Filesystem(ROOT . '/templates'));
        $content = $templatesEngine->render('/professionals/services/summary/block.twig', array(
            'services' => $services,
            'fundedServices' => $fundedServices,
            'total' => number_format($total),
            'funded' => number_format($funded),
            'baseUrl' => $base_url
        ));

        $content .= '<div class="separator"></div>';

        $body = sidebar_box($title, $content, 'project-service-payments-summary');
    }

    return array(
        'title' => $title,
        'body' => $body
    );
}

function verify_accreditation($op = null) {
    global $user, $AccessObj;
    $pid = sget($_REQUEST, 'id');
    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid))) {
        // continue
    } else {
        return '';
    }
    $editorid = $user->uid;
    $Block['title'] = 'Accreditation Status';
    $accData = getAccreditationVerData($pid);
    if ($accData) $verdate = date('m/d/Y', $accData['data_value']);
    if ($op && $op != 'update') {
        $HV = hidden_post_values(array('tab', 'page', 'id'));
        $content = "<div class=row><a class='openbox' boxid='ver_acc' style=\"cursor:pointer;\" >Verify Investor Accreditation</a></div>
                ";
        $content .= "<div id='ver_acc' style='display:none;'>
                       <form method=post action='/account'>
                       <label for='ver_date'><strong>Verified On</strong></label>
                       <input type='text' class='datepicker' id='ver_date' name='ver_date' value = '" . $verdate . "'><br>
                       <span style='font-size:12px;'>*Leaving blank field unverifies accreditation</span><br>
            <input type=submit name=submit value=submit class=button /> <a class='openclose' boxid='ver_acc' href='$redirect' class=button>cancel</a>
            $HV
            <input type='hidden' name='section' value='verify_accreditation'>
            <input type='hidden' name='update_section' value='verify_accreditation' />
        </form></div>";
        $content .= js_init("init_openbox();init_datepicker();");
        $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
    } elseif ($op == 'update') {
        global $user;
        $pid = $_REQUEST['id'];
        $date = strtotime($_REQUEST['ver_date']);
        if ($date == '') {
            $sql = "DELETE FROM maenna_people_data WHERE pid = %d and data_type = '%s' and data_attr = '%s'";
            $success = db_query($sql, array($pid, 'verify', 'accreditation'));
            if ($success) {
                drupal_set_message('Operation Successful');
            } else drupal_set_message('Operation was not successful', 'error');
            return;
        }
        if ($accData) {
            $sql = "UPDATE maenna_people_data SET data_value = '%s',access = '%s' WHERE pid = %d and data_type = '%s' and data_attr = '%s'";
            $success = db_query($sql, array(strval($date), strval(time()), $pid, 'verify', 'accreditation'));
            if ($success) {
                drupal_set_message('Operation Successful');
            } else drupal_set_message('Operation was not successful', 'error');
            return;
        }
        $sql = "INSERT INTO maenna_people_data (pid,access,data_type,data_attr,data_value,editorid) VALUES (%d,'%s','%s','%s','%s',%d)";
        $values = array($_REQUEST['id'], strval(time()), 'verify', 'accreditation', strval($date), $user->uid);
        $success = db_query($sql, $values);
        if ($success) {
            drupal_set_message('Operation Successful');
        } else drupal_set_message('Operation was not successful', 'error');
    }
    return $Block;
}

function pro_vetting ($op = null) {
    global $user, $AccessObj;
    $pid = sget($_REQUEST, 'id');
    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid))) {
        // continue
    } else {
        return '';
    }
    $editorid = $user->uid;
    $Block['title'] = 'Vetting';

    //progress_status part

    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $panel = 'pro_progress_panel';
    $viewall_link = "$redirect&panel=${panel}";
    $content = "<p><a style='float:left;' href=\"$viewall_link\">Progress status</a>";
    $content .= "<div style='float:right;border-left: solid 1px #006274;padding-left:4px;'><a style='font-size:14px !important;' href='$redirect&panel=${panel}&view=add' class=tool>Add</a></div></p><br style='line-height:1.5em !important;clear:both;'>";
    $time = time();
    if ($op && $op != 'update') {
        $sql = "select * from maenna_people_data where pid = %d and data_type = 'progress' order by access desc limit 3";
        $result = db_query($sql, array($pid));
        while (($Row = db_fetch_object($result)) !== false) {
            $date = date('m/d/Y', $Row->data_value3);
            $text = htmlentities(sideBarExcerpt(strip_tags($Row->data_value2)), ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content .= "\n<div class=row style='padding:5px 0'><b>$date</b>&nbsp;&nbsp; $text</div>";
        }
        if ($rating) $content .= "<div style='font:bold 16px arial;text-align:center;'>$rating</div>";
    }

    //End progress_status part

    //Clewed rating part
    $panel = 'pro_rating_panel';
    $content .= "<a href='$redirect&panel=${panel}'>Clewed rating</a>";
    if (empty($op)) return '';
    if ($op == 'title') {
        return $Block['title'];
    } elseif ($op) {
        $sql = "select * from maenna_people where pid = %d limit 1";
        $result = db_query($sql, array($pid));
        $Row = db_fetch_object($result);
        $rating = 'Not Rated';
        if ($Row) {
            if (!empty($Row->rating)) $rating = $Row->rating;
        }
        $content .= "<div style='margin-top:0px !important;font-style:italic;' class='row'>$rating";
        $content .= "</div></div>";
    }
    //End clewed rating part
    //Pro questionnaire part
    $roleId = userRoleId($pid);
    if (empty($op)) return '';
    $sql = "select recordid from maenna_questionair where target='pro' and usertype = %d and status = 1 and type = 'version' limit 1";
    $result = db_query($sql, array($roleId));
    $Row = db_fetch_object($result);
    if ($Row) {
        $parentid = $Row->recordid;
    } else return '';
    $content .= "<a href='/account?tab=information&page=qmgmt&panel=answerdetail&target=pro&id=$parentid&uid=$pid'>Pro Questionnaire</a><br>";

    //End pro questionnaire part
    //Acreditation status
    $accData = getAccreditationVerData($pid);
    if ($accData) $verdate = date('m/d/Y', $accData['data_value']);
    if ($op && $op != 'update') {
        $HV = hidden_post_values(array('tab', 'page', 'id'));
        $content .= "<a class='openbox' boxid='ver_acc' style=\"cursor:pointer;\" >Verify Investor Accreditation</a>
                ";
        $content .= "<div id='ver_acc' style='display:none;'>
                       <form method=post action='/account'>
                       <label for='ver_date'><strong>Verified On</strong></label>
                       <input type='text' class='datepicker' id='ver_date' name='ver_date' value = '" . $verdate . "'><br>
                       <span style='font-size:12px;'>*Leaving blank field unverifies accreditation</span><br>
            <input type=submit name=submit value=submit class=button /> <a class='openclose' boxid='ver_acc' href='$redirect' class=button>cancel</a>
            $HV
            <input type='hidden' name='section' value='verify_accreditation'>
            <input type='hidden' name='update_section' value='verify_accreditation' />
        </form></div>";
        $content .= js_init("init_openbox();init_datepicker();");
    }  elseif ($op == 'update') {
        global $user;
        $pid = $_REQUEST['id'];
        $date = strtotime($_REQUEST['ver_date']);
        if ($date == '') {
            $sql = "DELETE FROM maenna_people_data WHERE pid = %d and data_type = '%s' and data_attr = '%s'";
            $success = db_query($sql, array($pid, 'verify', 'accreditation'));
            if ($success) {
                drupal_set_message('Operation Successful');
            } else drupal_set_message('Operation was not successful', 'error');
            return;
        }
        if ($accData) {
            $sql = "UPDATE maenna_people_data SET data_value = '%s',access = '%s' WHERE pid = %d and data_type = '%s' and data_attr = '%s'";
            $success = db_query($sql, array(strval($date), strval(time()), $pid, 'verify', 'accreditation'));
            if ($success) {
                drupal_set_message('Operation Successful');
            } else drupal_set_message('Operation was not successful', 'error');
            return;
        }
        $sql = "INSERT INTO maenna_people_data (pid,access,data_type,data_attr,data_value,editorid) VALUES (%d,'%s','%s','%s','%s',%d)";
        $values = array($_REQUEST['id'], strval(time()), 'verify', 'accreditation', strval($date), $user->uid);
        $success = db_query($sql, $values);
        if ($success) {
            drupal_set_message('Operation Successful');
        } else drupal_set_message('Operation was not successful', 'error');
    }
    //End acreditation status

    $content .= "<div class=\"separator\"></div>";

    $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');

    return $Block;


}

/* EOF */
