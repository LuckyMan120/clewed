<?php defined('ROOT') || die; ?>

<?php if (!empty($insights)): ?>
    <div class="custom-cardholder-expert">
        <?php $counter = 0; ?>

        <?php foreach ($insights as $insight): ?>
        <?php if( $_helper->e($insight['type'])== 'Insight') {?>
            <?php if ($counter && 0 == $counter % 4): ?>
                <div class="clear"></div>
            <?php endif; ?>
            <div class="custom-completed-card">
                <a class="custom-completed-href" href="/join-insights?id=<?php echo $insight['id']; ?>">
                    <?php include "featured-insight.tpl.php"; ?>
                </a>
            </div>
            <?php }?>
        <?php endforeach; ?>
        <div class="custom-button-by-show-more">
            <a href="/join-insights">
                <div class="custom-show_more_cmp" rel="show" type="pastable">SEE MORE INSIGHT</div>
            </a>
        </div>
    </div>
<?php endif; ?>

