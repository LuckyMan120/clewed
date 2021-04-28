
<style type="text/css">
    div.content_box .box_title {
        margin-top: 14px;
    }

    .text_button {
        border: none !important;
        background-color: transparent !important;
        color: #0fabc4 !important;
        cursor: pointer;
        font-family: 'LatoRegular', sans-serif !important;
        font-size: 14px !important;
        font-style: normal !important;
    }

    div.content_box .box_content {
        /*padding-bottom:15px;*/
        padding-top: 2px;
        font-size: 14px;
        text-align: left;
        font-family: 'LatoRegular', sans-serif;
        float: left !important;
    }

    div.content_box {
        padding-left: 10px;
        position: relative;
        width: 100%;
        padding-bottom: 10px;
    }

    .ui-tooltip {
        color: white !important;
    }

    div.content_box {
        z-index: 10 !important;
    }

    div#docprev {
        text-align: left !important;
    }

    #cardholder .card {
        margin-bottom: 0 !important;
    }

    div.gig-button-container {
        margin-left: 8px;
        margin-right: 12px;
    }

    span.gig-counter-text-top {
        font-size: 16px !important;
    }

    span.gig-counter-text {
        top: 0;
        color: #333;
        font: normal normal normal 11px/18px 'Helvetica Neue', Arial, sans-serif;
    }

    a.pro-participations {
        float:right;
        border-left: solid 1px #006274;
        height: 15px;
        padding-left: 3px;
        margin-top: 15px;
        margin-right:25px;
        padding-bottom: 4px;
        color: #76787F;
        font-size: 12px;
        font-family: 'LatoRegular';
        text-transform: capitalize;
        font-weight: normal;
        cursor: pointer;
    }

    a.pro-participations:hover {
        color: #313136;
    }

</style>
<?php
global $user, $baseUrl;
require_once __DIR__ . '/../../../lib/init.php';
use Clewed\Insights\InsightHelper;
use Clewed\Insights\InsightRepository;
use Clewed\Insights\InsightService;
use Clewed\User\Service as UserService;

function nameToId($name){
        $q = mysql_query("SELECT uid FROM users WHERE name = '" . mysql_real_escape_string($name) . "' LIMIT 1") or die(mysql_error());
        $r = mysql_fetch_array($q);
        return $r['uid'];
}

function getUserType($uid)
{
        $q = mysql_query("SELECT pid FROM maenna_people WHERE pid = '" . (int)mysql_real_escape_string($uid) . "' ");
        if (mysql_num_rows($q) > 0) {
            return 'people';
        } else {
            $q1 = mysql_query("SELECT companyid FROM maenna_company WHERE companyid = '" . (int)mysql_real_escape_string($uid) . "' ");
            if (mysql_num_rows($q1) > 0) {
                return 'company';
            } else return 'admin';
        }
}

    $user_id = $user->uid;
    $pro_id = (int)sget($_REQUEST, 'id');
    if (!InsightHelper::seenInsight($pro_id)) {
        $iService = new InsightService;
        $iService->addView($pro_id);
    }
    $result1 = mysql_query(
        "SELECT * FROM `maenna_professional` WHERE  id = '"
        . (int)mysql_real_escape_string($_REQUEST['id'])
        . "'"
    ) or die(mysql_error());
    $row1 = mysql_fetch_array($result1);

    $insightRepository = new InsightRepository();
    $insight = $insightRepository->findById($pro_id);

    $result3 = mysql_query(
        "SELECT * FROM `like_discussion_professional` WHERE  prof_id = '"
        . (int)mysql_real_escape_string($pro_id)
        . "' and user_id = '"
        . (int)mysql_real_escape_string($id)
        . "'"
    );
    $likepost = mysql_num_rows($result3);
    $row3 = mysql_fetch_array($result3);
    $sql_expertise = mysql_query("SELECT * FROM `maenna_people` WHERE `pid` = '" . (int)$insight->postedby . "'");
    $sql_exp_result = mysql_fetch_array($sql_expertise);

    $utype = getUserTypeById($insight->postedby);
    if ($utype == 'admin') {
        $P_username = 'Admin';
    } elseif ($utype == 'super_admin') $P_username = 'Super admin';
    else if ($sql_exp_result['username_type'] == 1) {
        $P_username = ucfirst($sql_exp_result['firstname']);
    } else $P_username = ucfirst($sql_exp_result['firstname']) . ' ' . ucfirst($sql_exp_result['lastname']);
    $disc_title = $insight->title;
    $disc_why = $insight->whyattend;
    if (strlen($disc_title) <= 64) $top = 'margin-top:13px;'; else $top = 'margin-top:5px;;';
    if (strtoupper($disc_title) == $disc_title) $disc_title = strtolower($disc_title);
    if (!$insight->isApproved() || $insight->isPrivateInsight()) {
    echo "<span style='margin-top:20px;'><br>This Insights is not scheduled yet.</span>";
} else {
?>
<div id='docprev'
     style="padding-right:30px;float:left;width:610px;border-right:1px solid #D0D2D3">
    <div class="dischead"
         style="padding: 5px 0 5px 7px; height:52px;width:610px;background-color:#94c9da;color:#fff; line-height:21px;">
        <div class="dtitle" style="<?= $top; ?>color:#fff;"><?= $disc_title ?></div>
    </div>
    <div>
        <div class="join">
            <div class="jleft">
                <br>Host:
                <a href="#" id="pro_id<?= $insight->postedby; ?>" class="profile_details detail"
                   style="font-size: 14px;font-style:normal; font-weight: bold;"
                   name="<?= $P_username; ?>">
                    <?php echo $P_username; ?>,
                    <?php echo ucfirst(preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $sql_exp_result['experties'])); ?>
                </a>
                <?php
                    $res = db_query("SELECT mdm.*,IF (mp.username_type = 1,mp.firstname,CONCAT(mp.firstname,' ', mp.lastname)) as firstname,mp.protype as protype,mp.experties FROM maenna_discussion_moderator mdm join maenna_people mp ON mdm.user_id = mp.pid WHERE discussion_id = %d", $insight->id . " LIMIT 1");
                    while ($moderator = db_fetch_array($res)) {
                        if ($moderator['status'] == 'active') {
                            $status = '<span class="inv_status">Guest expert: </span>';
                            echo '<a style="font-size: 14px;font-style:normal; font-weight: bold;margin-left:20px;" href="#" id="pro_id' . $moderator['user_id'] . '" ref="pro_id"class="profile_details detail">' . $status . ucfirst($moderator['firstname']) . ", " . ucfirst($moderator['experties']) . "</a> ";
                        }
                    }
                ?>
            </div>
            <span style="float:right; margin-top: 22px;">Category: <?= $insight->tags; ?></span>
        </div>

        <div class="jrght">
            <br/><br/><br/>

            <div id="TabbedPanels1" class="TabbedPanels">

                <ul class="TabbedPanelsTabGroup">
                    <li class="TabbedPanelsTab TabbedPanelsTabSelected" id="tab1" tabindex="0">About</li>
                    <li class="TabbedPanelsTab" id="tab2" tabindex="0"
                        data-tooltip="This section requires joining the discussion. Login or sign up to access.">
                        Conversations
                    </li>
                    <?php if (isset($_REQUEST['file'])) :
                        list($time_stamp, $file_name) = explode('_', $_REQUEST['file']);
                        list($name, $ext) = explode('.', $file_name);
                        ?>
                    <?php endif; ?>
                </ul>

                <div class="TabbedPanelsContentGroup">

                    <!--- start about tab content -->

                    <div class="TabbedPanelsContent">
                        <div style="float: left;padding: 15px;width:569px;">

                            <strong
                                style="font-style:normal; font-weight:bold;text-align:left !important; color:#686b70;text-transform: uppercase;">
                                Target audience
                            </strong>

                            <p style="padding-bottom: 20px"><?= nl2br(htmlspecialchars($insight->description)); ?></p>

                            <strong style="font-style:normal; font-weight:bold; color:#686b70;text-transform: uppercase;">
                                <?= 0 == $insight->type ? 'Insight ':'Service ' ?> description
                            </strong>
                            <p style="padding-bottom: 20px"><?= nl2br(htmlspecialchars($insight->whyattend)); ?></p>

                            <?php
                            if ($insight->buyer_requirement){
                            ?>
                                <strong style="font-style:normal; font-weight:bold; color:#686b70;text-transform: uppercase;">
                                    Buyer Requirement
                                </strong>
                                <p style="padding-bottom: 20px"><?= nl2br(htmlspecialchars($insight->buyer_requirement)); ?></p>
                            <?php } ?>

                            <?php
                            if ($insight->datetime > time()) {
                                ?>
                                <strong style="font-style:normal; font-weight:bold; color:#686b70;text-transform: uppercase;">LIVE DISCUSSION DATE</strong>
                                <p style="padding-bottom: 20px"><?= date("l, M j, Y g:i A T ", $insight->datetime); ?></p>

                                <strong style="font-style:normal; font-weight:bold; color:#686b70;text-transform: uppercase;">Where</strong>
                                <p style="padding-bottom: 20px">Private</p>
                            <?php
                            }
                            ?>
                        </div>

                        <div class="materials"
                             data-tooltip="This section requires joining the discussion. Login or sign up to access.">
                            <strong
                                style="font-style:normal; font-weight:bold; color:#686b70;text-transform: uppercase; margin-left:10px;">Files</strong>
                            <br/>
                            <ul>
                                <?php
                                    $sql_images = mysql_query("SELECT * FROM wall_documents WHERE ref_id = '" . $pro_id . "'");
                                    while ($images = mysql_fetch_assoc($sql_images)) {
                                        if ($images['document_name'] != '') {
                                            list($time_stamp, $file_name) = explode('_', $images['document_name']);
                                            list($name) = explode('.', $file_name);
                                            $ext = end(explode('.', $images['document_name']));
                                            $id        = $_REQUEST['id'];
                                            $pro_id    = $_REQUEST['pro_id'];
                                            $file_name = $images['document_name'];
                                            if('mp3' !== $ext) {
                                                echo '<li><a style="color:#8d8f9a;" onclick="cmpClick()">' . $name . '</a></li>';
                                            }
                                            else {
                                                echo '
                                                <li onmousedown="cmpClick();">
                                                    <div>
                                                    <audio preload="auto" controls="controls">Sorry, your browser is too old and is not supported anymore. Please update it and try again.</audio>
<!--                                                    <script type="text/javascript" src="/js/swfobject.js"></script>
                                                        <script type="text/javascript">
                                                            swfobject.registerObject("player","9.0.0");
                                                            var flashvars = {
                                                                height: "20",
                                                                width: "200",
                                                                backcolor: "0x999999",
                                                                frontcolor: "0xFFFFFF",
                                                                overstretch: "none",
                                                                usefullscreen: "false",
                                                                enablejs: "true",
                                                                javascriptid: "player"
                                                            };
                                                            var params = {
                                                                wmode: "transparent"
                                                            };
                                                            var attributes = { };
                                                            swfobject.embedSWF("/mediaplayer.swf","audio_player' . md5($images['document_name']) . '","200","20","9.0.0","",flashvars,params,attributes);
                                                        </script>
                                                        <div id="audio_player' . md5($images['document_name']) . '"></div> -->
                                                    </div>
                                                </li>';
                                            }
                                        }
                                    }
                                ?>
                            </ul>
                            <?php
                                //Getting images and links of event.
                                $sql_links = mysql_query("SELECT * FROM maenna_professional_links WHERE professional_id = '" . (int)mysql_real_escape_string($_REQUEST['id']) . "'");
                                while ($links = mysql_fetch_assoc($sql_links)) {
                                    printf('<ul><li style="color:#8d8f9a;">%s</li></ul>', $links['name']);
                                }
                            ?>
                        </div>
                    </div>
                    <!--- end about tab content -->
                    <div></div>
                </div>
            </div>
        </div>
    </div>
</div>
<div style="float:left;display:inline-block;width:290px;" class="right_side_inights">
<div style='height:135px;' class='join_section'>
    <?php
        //Join part
        $pro_id = (int)sget($_REQUEST, 'id');
        global $user, $AccessObj;
        $usertype = $AccessObj->user_type;
        $resPayment = mysql_query("SELECT * FROM `maenna_professional_payments` WHERE pro_id = '" . (int)mysql_real_escape_string($pro_id) . "' and user_id = '" . (int)$user_id . "'");
        $sql_check_spots = mysql_query("SELECT * FROM `maenna_professional` WHERE id = '" . (int)mysql_real_escape_string($pro_id) . "'");
        $sql_check_spots_count = mysql_fetch_array($sql_check_spots);
        $checkUserCount = mysql_num_rows($resPayment);
        $checkUser = mysql_fetch_array($resPayment);
        $result1 = mysql_query("SELECT * FROM `maenna_professional` WHERE id = '" . (int)mysql_real_escape_string($_REQUEST['id']) . "' ORDER BY id DESC LIMIT 1 ");
        $sql_likes = mysql_query("SELECT * FROM `like_discussion_professional` WHERE `user_id` = '" . $user_id . "' AND `prof_id` = '" . (int)mysql_real_escape_string($pro_id) . "' ORDER BY id DESC");
        $sql_likes_result = mysql_num_rows($sql_likes);
        $row1 = mysql_fetch_array($result1);
        $result4 = mysql_query(
            "SELECT * FROM `maennaco_discussion_follow` WHERE  prof_id = '"
            . (int)mysql_real_escape_string($pro_id)
            . "' and user_id = '"
            . (int)mysql_real_escape_string($user_id)
            . "'"
        );
        $followdisrow = mysql_num_rows($result4);
        $followdis = mysql_fetch_array($result4);
        if ((int)$row1['cost'] > 0) {
            $costClear = "$" . number_format((int)str_replace(',', '', $row1['cost']));
        } else {
            $costClear = 'Free';
        }
        $cost = $row1['type'] == 0 ? 'Join ' . $costClear : ($row1['cost'] > 0 ? 'Buy ' . $costClear : 'Join Free');
        $type = "follow";
    ?>
    <p style="cursor:pointer;color:#00A3BF;font-weight: bold;text-align: center;" onclick="cmpClick();">
        Like <?php echo $likes_count ?>
    </p>
        <span id="follow_dis">
            <a style="cursor:pointer;float:left;margin-top:10px; width: 257px;text-align: center; padding:0px 7px 0px 7px;line-height:12px; "
               onclick="cmpClick()" title="Follow" class="tool follow">
                <strong>Follow</strong>
            </a>
        </span><?php
        $uname = "";

    $personsToShow = 60;

    $pro_id = (int) sget($_REQUEST, 'id');

    $attendees = array();
    $insightService = new InsightService();
    $userService = new UserService();
    $attendeeIds = $insightService->getAttendeeIds(array($pro_id));
    if(!empty($attendeeIds[$pro_id]))
        $attendees = $userService->get(array_values($attendeeIds[$pro_id]));

    $count = count($attendees);
    if ((int) $sql_check_spots_count['capacity'] > (int) $count) {
            ?><p style="text-align: center;">
            <input type="button" onclick="cmpClick()" class="join offer-join-btn" value="<?php echo $cost ?>">
            </p><?php
        } else {
            ?><p style="text-align: center;"><input type="button" class="join" value="Sold out"></p><?php
        }
        if ($sql_likes_result != '0') {
            $likes_count = $sql_likes_result;
        } else {
            $likes_count = '0';
        }
        $spots = $sql_check_spots_count['spots'];
        if ($checkUserCount <= 0) {
            echo $follow;
        }
    ?>
</div>

    <div class="insight-audio-preview">

        <?php $fileName = 'insights/audio-previews/' . $insight->id . '.mp3';

        if(is_readable(ROOT . 'sites/default/files/events_tmp/' . $fileName)):
            $db = \Clewed\Db::get_instance();
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
        endif; ?>

        <?php if($audioPreviewPath):?>

            <div class="insight-audio-preview-caption">Preview</div>

        <?php $playerNo = md5($audioPreviewPath . time());?>
          <audio preload="auto" controls="controls" src="<?=$audioPreviewPath;?>">Sorry, your browser is too old and is not supported anymore. Please update it and try again.</audio>
<!--            <script type="text/javascript" src="/js/swfobject.js"></script>
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


<?php if ($count >= 5 || in_array($AccessObj->user_type, array('super', 'admin'))) :?>

<div style="margin-left:10px;margin-top:20px;margin-bottom:14px;max-width: 279px;">
    <span style="font-size: 14px;color: #284b5a;font-family: Lato Bold Italic,serif;">Joined (<?php echo $count;?>)</span>
    <div class="month_2" style="margin-top:20px;border-bottom:none;">

    <?php $counter = 0;?>
    <?php foreach($attendees as $attendee):?>

        <?php $avatar = $baseUrl . '/themes/maennaco/phpthumb/phpThumb.php?src=' . $attendee['avatar'] . '&zc=1&w=40&h=41'; ?>

        <li style="list-style:none;background:none;margin:0 10px 10px 0;padding:0;width:35px;height:35px;float:left;">
            <a class="card-tool" data-id="<?= $attendee['uid'];?>" onclick="cmpClick();" title="<?= htmlspecialchars($attendee['full_name'], ENT_QUOTES, 'utf-8');?>">
                <img src="<?php echo $avatar; ?>" style="max-width:50px;">
            </a>
        </li>

        <?php if (++$counter >= $personsToShow) break; ?>

    <?php endforeach; ?>

    <?php if($count > $personsToShow):?>
        <div><a href="#" class="tool pro-participations" onclick="cmpClick();return false;">More</a></div>
    <?php endif;?>

    <div style="clear:both;"></div>
    </div>
</div>

<?php endif; ?>

    <?php

    function escapeJavaScriptText($string) {
        return str_replace("\n", '\n', str_replace('"', '\"', addcslashes(str_replace("\r", '', (string) $string), "\0..\37'\\")));
    }

    if (strlen($disc_why) >= 150)
        $disc_why = substr($disc_why, 0, 150) . "...";

    $url = $_SERVER['HTTP_HOST'] . "/insights-and-services?id=" . $pro_id;
    $proto = $_SERVER['HTTPS'] ? 'https' : 'http';

    ?>

    <div style="margin-left: 10px; margin-top: 30px;">
        <span style="font-size: 14px; color: #284b5a; font-family: 'Lato Bold Italic';">Share</span>

    </div>
    <script type="text/javascript" src="//s7.addthis.com/js/300/addthis_widget.js#pubid=ra-599d25270410f116"></script>
    <script type="text/javascript">
        var addthis_share = {

            url_transforms : {
                shorten: {
                    twitter: 'bitly'
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
    <div class="addthis_inline_share_toolbox" data-description="<?=$disc_why?>" data-media="https://www.clewed.com/share-logo.jpg" data-url="<?=$proto . '://' . $url;?>" data-title="<?=$disc_title?>"></div>
    <!--    <div data-title="Clewed Insights" data-url="<?/*=$proto . '://' . $url;*/?>" data-message="Proba poruke" data-username="clewed" data-title="" class="sharethis-inline-share-buttons"></div>-->

    <!--<a class="twitter-share-button"
       href="https://twitter.com/intent/tweet?text=Hello%20world">
        Tweet</a>
    <script src="//platform.linkedin.com/in.js" type="text/javascript"> lang: en_US</script>
    <script type="IN/Share" data-url="http://www.clewed.com" data-counter="right"></script>
    <div class="fb-share-button" data-href="http://www.clewed.com" data-layout="button_count" data-size="small" data-mobile-iframe="true"><a class="fb-xfbml-parse-ignore" target="_blank" href="https://www.facebook.com/sharer/sharer.php?u=http%3A%2F%2Fwww.clewed.com%2F&amp;src=sdkpreparse">Share</a></div>
    <div style="clear:both;"></div>-->
    <div style="margin-left:10px;margin-top:14px;">
        <span style="font-size: 14px;color: #284b5a; font-family: 'Lato Bold Italic';">Popular questions</span>
        <?php
            $user_id = $user->uid;
            $pro_id = sget($_REQUEST, 'id');
            $pro_sql = "SELECT PWP.pid,(SELECT COUNT(*) FROM like_discussion_posts WHERE post_id = PWP.pid) as post_count, PWP.post
            FROM pro_wall_posts AS PWP
            WHERE PWP.flag = 'q' AND PWP.pro_id = %d
            ORDER BY post_count DESC
            LIMIT 5";
            $pro_result = db_query($pro_sql, array($pro_id));
            $count = mysql_num_rows($pro_result);
        ?>
        <div>
            <?php if ($count > 0) { ?>
                <ul>
                    <?php while ($pro_row = db_fetch_array($pro_result)) {
                        $post   = $pro_row['post'];
                        $length = strlen($pro_row['post']);
                        if ($length > 30) {
                            $post = substr($pro_row['post'], 0, 30) . '...';
                        }
                        ?>
                        <li style="padding: 0;">
                            <?php echo ucfirst($post); ?>
                        </li>
                    <?php } ?>
                </ul>
            <?php } ?>
        </div>
    </div>
    <?php } //End of Popular questions part// ?>
    <div class="how-insight-work">
       <?php if(0 != $insight->type):?>
            <div class="how-services-work-wrapper">
                <?php include __DIR__ . '/../../../templates/insights/how-services-work.tpl.php';?>
            </div>
       <?php else:?>
            <div class="how-insights-work-wrapper">
                <?php include __DIR__ . '/../../../templates/insights/how-insights-work.tpl.php';?>
            </div>
       <?php endif;?>
    </div>
</div>
<div style="clear:both;"></div>
