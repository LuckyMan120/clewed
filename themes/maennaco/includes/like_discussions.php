<style type="text/css">
    #likepost a:hover {
        text-decoration: underline;
        color: #00A3BF !important;
        font-weight: bold !important;
        font-family: 'LatoRegular' !important;
    }
</style>
<?php
error_reporting(0);
include 'dbcon.php';
global $user;
$pro_id = sget($_REQUEST, 'pro_id');
$likesCount = (int) $db->get_column(
    'SELECT COUNT(*) FROM `like_discussion_professional` WHERE `prof_id` = :pid',
    array(':pid' => $pro_id)
);
$userLikes = (bool) $db->get_column(
    'SELECT COUNT(*) FROM `like_discussion_professional` WHERE prof_id = :pid AND user_id = :uid',
    array(':pid' => $pro_id, ':uid' => $user->uid)
);
$action = $userLikes ? 'Unlike' : 'Like';
?>
<p style="text-align: center;" id="likepost">
    <a style="cursor:pointer; color:#00A3BF;font-weight: bold;"
       onclick="like_discussion('<?= $action ?>', <?= $pro_id ?>, <?= $user->uid ?>);"><?= $action ?></a>
    <?= $likesCount ?>
</p>
<script type="text/javascript">
    $(document).ready(function () {
    $("#follow_dis a[atype='unfollow'],#follow_dis a[atype='follow']").click(function () {

        type = $(this).attr('atype');
        prof_id = $(this).data('ins');
        user_id = $(this).data('uid');
        sel_obj = $(this);
        if (type == 'follow') {
            var status = 1;
        } else {
            var status = 0;
        }

        <?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

        $.ajax({
            type: 'get',
            url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
                'type=followdis&' +
                'prof_id=' + prof_id + "&" +
                "user_id=" + user_id + "&" +
                "status=" + status + "&" +
                "u=<?php echo $u;?>&" +
                "m=<?php echo $m;?>",
            success: function (msg) {
                if (type == 'follow') {

                    sel_obj.html('Following');
                    sel_obj.attr('atype','unfollow');
                    sel_obj.css('margin-left','3px');
                    sel_obj.parent().find('.foll_cnt').html(parseInt(sel_obj.parent().find('.foll_cnt').html())+1);

                } else {
                    sel_obj.html('Follow');
                    sel_obj.attr('atype','follow');
                    sel_obj.css('margin-left','12px');
                    sel_obj.parent().find('.foll_cnt').html(parseInt(sel_obj.parent().find('.foll_cnt').html())-1);
                }
            }
        });

    });
    });
    function like_discussion(action, pid, uid) {
        $.ajax({
            type: 'get',
            url: '/themes/maennaco/includes/like.php?a=' + action + '&p=' + pid + '&u=' + uid,
            success: function (r) {
                action = action === 'Like' ? 'Unlike' : 'Like';
                $('#likepost').html('<a style="cursor:pointer;color:#00A3BF;font-weight:bold;" onclick="like_discussion(\'' + action + '\', ' + pid + ',' + uid + ');">' + action + '</a>&nbsp;' + r);
            }
        });
    }
</script>
