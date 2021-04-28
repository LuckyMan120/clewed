<?php defined('ROOT') || die; ?>

<?php if (!empty($insights)): ?>
    <div class="featured-insights-grid">
        <?php $counter = 0;?>
        <?php foreach ($insights as $insight): ?>
            <?php if($counter && 0 == $counter % 4):?>
                <div class="clear"></div>
            <?php endif;?>
            <a href="/insights?id=<?php echo $insight['id'];?>"><?php include "featured.tpl.php"; ?></a>
        <?php endforeach; ?>
    </div>
    <div class="clear"></div>
    <div class="browse-insights-and-service-link"><a href="/insights">>> Browse Insights and Services</a></div>
<?php endif; ?>