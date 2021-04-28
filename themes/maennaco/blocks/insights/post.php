<?php
/**
 * included in pro_comments_details.php
 */

/**
 * @param array $post
 * @param array $comments
 * @param int $currentUserId
 * @param bool $ifPrivate
 * @param bool $ifInsightAdmin
 */
function displayPost($post, $comments, $currentUserId, $ifPrivate, $ifInsightAdmin) {

?>
<div class="cmtloop" id="dis_post<?= $post['id']; ?>">

    <div class="ask">
        <?php if ($post['flag'] != 'm' || true) { ?>
            <div class="askpic">
                <img src="<?= $post['author_avatar']; ?>" style="float:left; margin-top:13px; margin-right:5px; width:45px; height:45px;">&nbsp;
            </div>
            <div class="asktitle"><?= $post['author_name']; ?>&nbsp;
                <?php if ($post['flag'] == 'q') {
                    echo '<strong>asks a question:</strong>';
                } ?>
            </div>
        <?php } ?>

        <?php

        $post['post'] = preg_replace('#<\s*a[^>]+href\s*=\s*\"?\'?([^\s\'\">]+)\'?\"?[^>]*>[^<]*<\/a>#uims', '$1', $post['post']);
        $post['post'] = preg_replace('#<\s*img[^>]>#uims', '', $post['post']);
        $post['post'] = preg_replace('#<\s*iframe.*?\/iframe\s*>#uims', '', $post['post']);
        $post['post'] = preg_replace('#(https?\:\/\/[^\s]+\.[^\s\.<]+)#uims', '<a class="comment-inline" href="$1" target="_blank">$1</a>', $post['post']);

        ?>

        <div style="margin:5px 0 0 65px;" class="post_content">
            <?= urldecode($post['post']) ?>
        </div>

        <div style="clear:both"></div>
        <div style='float:left;margin-left:65px;line-height:1.1em;padding:0;font-size:12px;'>
            <?= date("D, M j, Y g:i A T ", $post['created_at_timestamp']); ?>
            &nbsp;|&nbsp;
        </div>
        <div class='comment_anchor' style="float:left;margin-top:-4px;">
            <?php if (!$ifPrivate) { ?>
                        <span style='margin: 0; padding: 0;' id="likepost1<?= $post['id']; ?>">
                            <a href="javascript:void(0);" style="cursor:pointer;"
                               onClick="like_posts('<?= ($post['is_liked'] ? 'unlike' : 'like');?>', '<?= $post['discussion_id']; ?>', '<?= $post['id']; ?>', '<?= $currentUserId; ?>');">
                                <?= ($post['is_liked'] ? 'Unlike' : 'Like');?>
                            </a>
                                (<span class="like_cnt"><?= $post['likes_cnt']; ?></span>)
                        </span>&nbsp;|
            <?php } ?>
            &nbsp;<a onclick='commentFormDisplay("<?= $post['id']; ?>");'><?=($ifPrivate ? 'Reply' : 'Comment');?></a>

            <?php  if ($post['author_id'] == $currentUserId || $ifInsightAdmin) { ?>
                &nbsp;|&nbsp;
                <a onclick="edit_minutes('<?= $post['id']; ?>')">Edit</a>
                <div style="display: none;" id="edit_minutes_<?= $post['id']; ?>">
                    <?= $post['post']; ?>
                </div>
                &nbsp;|&nbsp;<a style="cursor:pointer;" href="javascript:void(0);" id="deletepost<?= $post['id']; ?>" class="deletepost">Delete</a>
            <?php } ?>

        </div>
        <div class="askright" id="comments_<?= $post['id']; ?>" style="width:592px !important; margin-top: 10px;">
<?php

            foreach ($comments as $comment) {
                displayComment($comment, $currentUserId, $ifInsightAdmin);
            }

?>
            <div class="w" style="display:none;margin:1px 0; padding:0 0 0 20px; background:#f4f8fa!important;width:516px;" id='form_id<?= $post['id']; ?>'>
                <form style="margin: 0 -26px;" method="post"
                      action="/account?tab=professionals&page=pro_detail&id=<?= $post['insight_id']; ?>&section=pro_industry_view&type=details&pro_id=<?= $post['discussion_id']; ?>"
                      id="comments">
                    <input type="hidden" name="post_id" id="post_id" value="<?= $post['id']; ?>"/>
                    <input type="hidden" name="comment_id" id="comment_id" value=""/>
                    <input type="hidden" name="comment_type" value="<?= $ifPrivate ? 'private' : '';?>"/>
                    <input type="hidden" name="dis_id" id="dis_id" value="<?= $post['discussion_id']; ?>"/>

                    <textarea name="post_comment" id="post_comment<?= $post['id']; ?>"
                              class=" input watermark mceNoEditor"
                              style="width:87%;  margin: 5px 0 0 -9px!important; height:25px; "
                              onFocus="showsubmit('<?= $post['id']; ?>');"></textarea>
                    <input type="submit" id="post_com<?= $post['id']; ?>"
                           m= <?= md5($post['id'] . "kyarata75"); ?>  value="Submit" class="text_button"
                           style="display:none;vertical-align: top;margin-top: 0;margin-left:78%;"/>
                </form>
            </div>
        </div>
        <?php if (!$ifPrivate) { ?>
            <div style="margin:0; padding:0; float:left;">
                <div style="height:30px; width: 535px;">
                    <div style='text-align:right; margin:7px 0 0 0;color:#76787f'>
                        Topic:&nbsp;<span style="float:right;" id="topic_<?= $post['id']; ?>"><?= $post['tags']; ?></span></div>
                </div>
            </div>
        <?php } ?>
    </div>
    <div style="clear:both"></div>
</div>

<?php } ?>