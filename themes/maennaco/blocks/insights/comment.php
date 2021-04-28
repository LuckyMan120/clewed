<?php
/**
 * included in pro_comments_details.php
 */

/**
 * @param array $comment
 * @param int $currentUserId
 * @param bool $ifInsightAdmin
 */
function displayComment($comment, $currentUserId, $ifInsightAdmin) {

?>
<div class="aucomnts">
    <div class="aucpic">
        <img src="<?= $comment['author_avatar']; ?>" style="float:left; margin-top:13px; margin-right:5px; width:45px; height:45px;">&nbsp;
    </div>
    <div class="aucdisc">
        <h5 style="text-transform:uppercase;"><?= htmlspecialchars($comment['author_name']); ?></h5>

        <?php

        $comment['comment'] = preg_replace('#<\s*a[^>]+href\s*=\s*\"?\'?([^\s\'\">]+)\'?\"?[^>]*>[^<]*<\/a>#uims', '$1', $comment['comment']);
        $comment['comment'] = preg_replace('#<\s*img[^>]>#uims', '', $comment['comment']);
        $comment['comment'] = preg_replace('#<\s*iframe.*?\/iframe\s*>#uims', '', $comment['comment']);
        $comment['comment'] = preg_replace('#(https?\:\/\/[^\s]+\.[^\s\.<]+)#uims', '<a class="comment-inline" href="$1" target="_blank">$1</a>', $comment['comment']);

        ?>

        <div style="clear:both;"></div>
        <div style="padding: 0 !important;"
             id="com<?= $comment['id']; ?>"><?= $comment['comment']; ?></div>

        <div class='comment_anchor'>
            <div style='float:left;margin:0;padding:0;font-size:12px;'>
                <?= date("D, M j, Y g:i A T ", $comment['created_at_timestamp']); ?>
                &nbsp;|&nbsp;
            </div>
            <div style='float:left;padding:0;'>

                <?php if (!$comment['is_private']) { ?>
                    <a href="javascript:void(0);" id='likepostcomment<?= $comment['id']; ?>' style="cursor:pointer;"
                       onClick="like_post_comments('<?= ($comment['is_liked'] ? 'unlike' : 'like');?>',  '<?= $comment['id']; ?>', '<?= $currentUserId; ?>');">
                        <?= ($comment['is_liked'] ? 'Unlike' : 'Like');?>
                    </a>&nbsp;|
                <?php
                }
                if ($comment['author_id'] == $currentUserId || $ifInsightAdmin) {
                    ?>
                    &nbsp;<a style="cursor:pointer;" onclick="edit_comments(<?= $comment['id']; ?>,<?= $comment['post_id']; ?>);"
                                    id="edit_comment<?= $comment['id']; ?>"
                                    class="edit_comment">Edit</a>

                    &nbsp;|&nbsp;<a style="cursor:pointer;" href="javascript:void(0);"
                                    id="delete_comment<?= $comment['id']; ?>"
                                    class="delete_comment">Delete</a>
                <?php } ?><p></p>
            </div>
        </div>
    </div>
    <div style="clear:both"></div>
</div>

<?php } ?>