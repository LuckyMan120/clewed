<?php defined('ROOT') || die; ?>

<?php if (!empty($insights)): ?>
    <div class="custom-cardholder-research">
        <?php $counter = 0; ?>

        <?php foreach ($insights as $insight): ?>
            <?php if( $_helper->e($insight['type']) == 'Service') {?>
                <?php if ($counter && 0 == $counter % 4): ?>
                    <div class="clear"></div>
                <?php endif; ?>
                <div class="custom-completed-card">
                    <a class="custom-completed-href" href="/buy-services?id=<?php echo $insight['id']; ?>">
                        <?php include "featured-service.tpl.php"; ?>
                    </a>
                </div>
            <?php }?>
        <?php endforeach; ?>
        <div class="custom-button-by-show-more">
            <a href="/buy-services">
                <div class="custom-show_more_cmp" rel="show" type="pastable">SEE MORE SERVICES</div>
            </a>
        </div>
    </div>
<?php endif; ?>

