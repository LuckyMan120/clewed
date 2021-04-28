<?php defined('ROOT') || die; ?>

<div class="featured-insights-grid-item">

    <?php if($insight['author']['reviews-count'] > 10):?>
        <div class="featured-insight-rating rateit"
             data-tooltip="<?php echo $insight['author']['rating'];?> star rating"
             data-rateit-readonly="true"
             data-rateit-starwidth="12"
             data-rateit-starheight="12"
             data-rateit-value="<?php echo $insight['author']['rating'];?>"
             data-rateit-step="1"></div>
        <div class="featured-insight-reviews-count">(<?php echo $insight['author']['reviews-count'];?>)</div>
    <?php else:?>
        <div class="featured-insight-rating"></div>
    <?php endif;?>

    <div class="featured-insight-image">
        <?php if(is_readable(ROOT . 'sites/default/images/insights/original/' . $insight['id'] . '.jpg')):?>
            <img class="cover" src="<?php echo 'sites/default/images/insights/original/' . $insight['id'] . '.jpg';?>">
        <?php else:?>
            <img class="cover" src="/themes/maennaco/images/cmp-avatar-product.png">
        <?php endif;?>
    </div>

    <?php if(is_readable(ROOT . 'sites/default/images/profiles/50x50/' . $insight['author']['id'] . '.jpg')):?>
        <div class="featured-insight-expert">
            <img src="/sites/default/images/profiles/50x50/<?php echo $insight['author']['id'];?>.jpg" />
            <div class="caption"><?php echo $_helper->e($insight['author']['first-name']) . '\'s ' . $_helper->e($insight['type']);?></div>
        </div>
    <?php endif;?>

    <div class="featured-insight-title"><?php echo $_helper->e(substr($insight['title'], 0, 100));?></div>
    <div class="featured-insight-author">
        <div class="featured-insight-author-description">
            <?php echo $_helper->e($insight['author']['full-name']);
                if(!empty($insight['author']['tag'])):
                    echo ', ' . $_helper->e($insight['author']['tag']) . ', ' . $insight['author']['work-years'];?> Yrs
            <?php endif;?>
        </div>
        <div class="featured-insight-author-expertise">
            <?php echo $_helper->buildExpertiseString($insight);?>
        </div>
        <div class="featured-insight-author-education"><?php echo $_helper->e($insight['author']['education']);?></div>
    </div>
    <div class="featured-insight-footer">
        <div class="featured-insight-footer-left">
            <span class="featured-insight-category"><?php echo $_helper->e($insight['category']) ?></span>
            <?php if(!empty($insight['industry'])):?>
                <span class="featured-insight-industry"><?php echo $_helper->e($insight['industry']); ?></span>
            <?php endif;?>
        </div>
        <div class="featured-insight-views">
            <img src="/themes/maennaco/images/eye.png" /><?php echo $_helper->buildViewsString($insight);?>
        </div>
    </div>
</div>
