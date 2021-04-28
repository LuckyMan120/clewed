<?php defined('ROOT') || die; ?>

<div class="custom-completed-project-name text-d-n"><?php echo $_helper->e($company['projname']);?></div>

<div class="custom-completed-avatar">
    <img src="<?php echo $_helper->buildImageUrlString($company);?>" alt="<?php echo $_helper->e($company['projname']);?>" width="100%;" style="object-fit: cover">
</div>
<div class="custom-sector-revenue">
    <span class="custom-sector"><?php echo strip_tags(substr($_helper->buildSectorString($company), 0, 13)). (strlen(strip_tags($_helper->buildSectorString($company)) )> 13 ? '&hellip;' : '');?></span>
    <span class="custom-revenue"><?php echo $_helper->buildRevenueString($company); ?></span>
</div>
<!--<div class="featured-company-city">--><?php //echo $_helper->buildCityStateString($company);?><!--</div>-->
<div class="custom-mission">
    <?php echo strip_tags(substr($company['mission'], 0, 240));?>
</div>
