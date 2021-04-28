<?php defined('ROOT') || die; ?>
<?php if ($_helper->e($insight['type']) == 'Service') { ?>

    <?php if($insight['author']['reviews-count'] > 10):?>
        <div class="custom-completed-project-name text-d-n rateit"
             data-tooltip="<?php echo $insight['author']['rating'];?> star rating"
             data-rateit-readonly="true"
             data-rateit-starwidth="12"
             data-rateit-starheight="12"
             data-rateit-value="<?php echo $insight['author']['rating'];?>"
             data-rateit-step="1">
            <div class="count-like">(<?php echo $insight['author']['reviews-count'];?>)</div>
        </div>
    <?php else:?>
        <div class="custom-completed-project-name text-d-n"></div>
    <?php endif;?>

    <div class="custom-completed-avatar">
        <?php if(is_readable(ROOT . 'sites/default/images/insights/original/' . $insight['id'] . '.jpg')):?>
            <img src="<?php echo 'sites/default/images/insights/original/' . $insight['id'] . '.jpg';?>" width="100%">
        <?php else:?>
            <img src="/themes/maennaco/custom-images/default_avatar.png" width="100%">
        <?php endif;?>
        <div class="custom-avatar-layer">
            <div class="avatar-layer-content">
                <div class="avatar-border">
                    <?php if(is_readable(ROOT . 'sites/default/images/profiles/50x50/' . $insight['author']['id'] . '.jpg')):?>
                        <img src="/sites/default/images/profiles/50x50/<?php echo $insight['author']['id'];?>.jpg" alt="avatar">
                    <?php else: ?>
                        <img src="/themes/maennaco/custom-images/profile.png" alt="avatar">
                    <?php endif;?>
                </div>
                <?php echo $_helper->e($insight['author']['first-name']) . '\'s ' . $_helper->e($insight['type']);?>
            </div>
        </div>
    </div>

    <div class="custom-sector-revenue">
        <?php echo $_helper->e(substr($insight['title'], 0, 27)). (strlen(strip_tags($insight['title']) )> 27 ? '&hellip;' : '');?>
    </div>

    <div class="custom-mission">
        <?php echo $_helper->e($insight['author']['full-name']);
        if(!empty($insight['author']['tag'])):
            echo ', ' . $_helper->e($insight['author']['tag']) . ', ' . $insight['author']['work-years'];?> Yrs
        <?php endif;?>
        <?php echo $_helper->buildExpertiseString($insight);?>
        <?php echo $_helper->e($insight['author']['education']);?>
    </div>

    <div class="custom-sector-revenue-bottom-pricing">
        <span class="custom-sector"><?php echo $_helper->e($insight['category']) ?></span>
        <span class="custom-revenue"><?php echo $_helper->buildViewsString($insight);?> view</span>
    </div>
<?php } ?>


