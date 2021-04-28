<?php
/**
 * Created by PhpStorm.
 * User: Ihor Borysyuk
 * Date: 7/25/15
 * Time: 10:56 PM
 */

function renderInsightPreview($row1, $user)
{

    $id = $row1['postedby'];
    $utype = getUserTypeById($id);
    if ($utype == 'admin') {
        $P_username = 'Admin';
    } elseif ($utype == 'super_admin') $P_username = 'Clewed';
    else $P_username = ucfirst($row1['firstname']);
    $userId = $row1['uid'];
    $avatar = getAvatarUrl($userId, "150");
    $sql = "select user_id from maenna_discussion_moderator where discussion_id = '" . $row1['id'] . "' limit 1";
    $moderatorResource = mysql_query(
        $sql
    );
    $moderatorId = 0;
    $response = mysql_fetch_assoc($moderatorResource);
    $moderatorAvatar = false;
    if ($response) {
        $moderatorId = $response['user_id'];
        $moderatorAvatar = getAvatarUrl($moderatorId, "150");
    }
    ?>
    <div class="event" id="event<?= $row1['id']; ?>">
        <div class="clear"></div>
        <div
            style="float:left;<?php if ($moderatorAvatar): ?>width: 0;<?php endif; ?>margin: 0; padding: 3px 0 0 0; height: 40px;">
            <img src="<?= $avatar; ?>" alt="" width="35px" class="grayscale"/>
        </div>
        <?php if ($moderatorAvatar): ?>
            <div style="float: left; height: 40px; margin-top: 40px;">
                <img src="<?= $moderatorAvatar; ?>" alt="" width="35px" class="grayscale"/>
            </div>
        <?php endif; ?>
        <!-- calendar -->
        <div class="event-info">
            <div style="margin-left: 10px; float: left; width: 525px; margin-top: 5px;">
                <a onclick="discussion('<?= $id ?>', '<?= $row1['id'] ?>');"
                   href="/<?=$_REQUEST['q']?>?id=<?= $row1['id'] ?>">
                        <span class="eventTitle"
                              style="width: 465px; float: left; font-size: 18px; font-weight: bold; cursor: pointer; text-decoration: none; white-space: nowrap; overflow: hidden !important; text-overflow: ellipsis;">
                            <?= replace_email(substr($row1['title'], 0, 80)); ?>
                        </span>
                </a>

                <div class="clear"></div>
                <div><span style="color:#91939E !important">By:</span>
                    <a href="#" id="pro_id<?php echo $row1['postedby']; ?>" style="color:#00a2bf;"
                       class="profile_details" name="<?= $user->name; ?>">
                        <?php echo $P_username; ?>,
                        <?= ucfirst($row1['pexperties']); ?></a>
                    <?php
                    $res = db_query(
                        'SELECT mdm.*, IF (mp.username_type = 1, mp.firstname, CONCAT(mp.firstname, " ", mp.lastname)) AS firstname,
                            mp.protype AS protype,
                            mp.experties as experties
                            FROM maenna_discussion_moderator mdm
                            JOIN maenna_people mp ON mdm.user_id = mp.pid
                            WHERE discussion_id = %d
                            ORDER BY id
                            LIMIT 1',
                        $row1['id']
                    );
                    if ($res) {
                        $moderator = db_fetch_array($res);
                    }
                    if ($moderator['status'] == 'invited') {
                        $status = '<span  style="color:#91939E !important" class="inv_status">Invited: </span>';
                    } elseif ($moderator['status'] == 'active') {
                        $status = '<span  style="color:#91939E !important" class="inv_status">Collaborator: </span>';
                    } else {
                        $status = '';
                    }
                    if ($moderator) {
                        ?>
                        <a href="#"
                           id="pro_id<?php echo $moderator['user_id'] ?>"
                           ref="pro_id"
                           style="margin-left: 20px; color: #00a2bf;"
                           class="profile_details">
                            <?php echo $status . ucfirst($moderator['firstname']) . ', ' . ucfirst($moderator['experties']) ?>
                        </a>
                    <?php
                    }
                    if ($moderator['status'] == 'invited' && $moderator['user_id'] == $user->uid) {
                        ?>
                        <span class='rsvp_inv'>
                                <a style='margin-left:10px;cursor:pointer;' iid='<?php echo $moderator['id'] ?>'
                                   rel='accept' class='invitation'>
                                    Accept
                                </a>
                                /
                                <a iid='<?php echo $moderator['id'] ?>' style='cursor: pointer;' rel='reject'
                                   class='invitation'>
                                    Reject
                                </a>
                            </span>
                    <?php
                    }
                    ?>
                </div>
                <div class="clear"></div>
                <?php
                if (strlen($row1['whyattend']) > 170) {
                    $more_link = '&hellip;';
                } else {
                    $more_link = '';
                }
                ?>
                <span style="float: left; margin: 0; text-align: left;">
                        <?= substr($row1['whyattend'], 0, 170) . $more_link; ?>
                    </span>

                <div class="clear"></div>


                <div style="color:#76787F; font-size:12px;float:left;margin-top:4px;">
                    <?php
                    $pro_sql = '
                        SELECT COUNT(DISTINCT(maenna_professional_payments.user_id)) as count
                        FROM maenna_professional_payments
                        LEFT JOIN maenna_people ON maenna_professional_payments.user_id = maenna_people.pid
                        LEFT JOIN maenna_company ON maenna_professional_payments.user_id = maenna_company.companyid
                        WHERE maenna_professional_payments.pro_id = %d';
                    $pro_result = db_query($pro_sql, array((int)$row1['id']));
                    $count = mysql_fetch_assoc($pro_result);
                    $count = (int) $count['count'];
                    ?>
                    <?php
                    if ($row1['type'] == 0) {
//                    if ($row1['type'] == \Clewed\Insights\InsightEntity::TYPE_GROUP_INSIGHT) {
                        if ($row1['capacity'] > $count) {
                            $spotsLeft = (int)($row1['capacity'] - $count);
                            if ($count >= 10) {
                                if ($spotsLeft <= 5) {
                                    echo  $spotsLeft . (($spotsLeft > 1) ? ' Spots Left' : ' Spot Left');
                                } else {
                                    echo (int)($count) . ' Joined';
                                }
                            } else {
                                echo  'Insight';
                            }
                        } else {
                            echo '<span class="sold-out">Sold out</span>';
                        }
                    } else {
                        echo 'Service';
                    }
                    ?>

                </div>

                <div style="color:#76787F; font-size:12px;float:right;margin-top:4px;">
                    <?= $row1['views'] ?: 'no' ?> view<?php echo $row1['views'] == 1 ? '' : 's' ?>
                        |
                    <?= $row1['tags'] ?>
                </div>
                <div class="clear"></div>
            </div>
        </div>
    </div>
    <div class="clear"></div>
    <hr style="margin-top:23px;background:#d0d2d3;">
<?php
}
