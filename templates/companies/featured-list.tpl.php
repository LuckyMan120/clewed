<?php defined('ROOT') || die; ?>

<?php if (!empty($companies)): ?>
    <!--    <div class="featured-companies-grid">-->
    <div class="custom-cardholder-explore">
        <?php $counter = 0; ?>
        <?php foreach ($companies as $company): ?>
            <?php if ($counter && 0 == $counter % 4): ?>
                <div class="clear"></div>
            <?php endif; ?>
            <div class="custom-completed-card">
                <a class="custom-completed-href" href="/companies?id=<?php echo $company['id']; ?>">
                    <?php include "featured.tpl.php"; ?>
                </a>
            </div>
        <?php endforeach; ?>
        <div class="custom-button-by-show-more">
            <a href="/companies">
                <div class="show_more_cmp custom-show_more_cmp" rel="show" type="pastable">see more</div>
            </a>
        </div>
    </div>
    <div class="clear"></div>

    <!--    <div class="browse-companies-link"><a href="/companies">>> View more</a></div>-->

<?php endif; ?>