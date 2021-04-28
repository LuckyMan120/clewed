<?php require_once __DIR__ . '/../blocks/insights/insights.php';?>
<div id="posting" align="left" style="float:left;margin-top:36px;margin-left:152px;width:600px;">
    <?php
        $offerType = getProfessionalType();
        $cnt = renderInsightsPreview($user, $_REQUEST['sort'], $_REQUEST['sortmonth'], $_REQUEST['sortdate'],0,-1 , $offerType );
    ?>
</div>

<div id="cmp-filter-cont" style="margin-top:46px;margin-left:27px;">
    <?php if ($cnt > 0) { ?>
        <span class="cmp-filter" style="margin-left:0;">Filter by</span>

<!--    <span class="filter-parent" rel="offer-type-filter">Offer type</span>-->
<!--        <div style="height:1px;clear:both;"></div>-->
<!--    <div id="offer-type-filter" style="display:none;">-->
<!--       <span class="filter-entry" filter="offer-type" rel="insight">Insight</span>-->
<!--            <span class="filter-entry" filter="offer-type" rel="service">Service</span>-->
<!--       </div>-->

        <input type="hidden" value="<?=$offerType?>" id="offerType">
        <span class="filter-parent" rel="industry-filter">Categories</span>
        <div style="height:1px;clear:both;"></div>
        <div id="industry-filter" style="display:none;">
            <?php
            $sql = 'SELECT `tags` FROM `maenna_professional` WHERE `approve_status` = 1 GROUP BY `tags`';
            $result = db_query($sql);
            while ($row = mysql_fetch_array($result)) {
                if ($row['tags'] != '') {
                    ?><span class="filter-entry" filter="category"
                            rel="<?= $row['tags'] ?>"><?= $row['tags'] ?></span><?php
                }
            }
            ?>
        </div>
    <?php } ?>
</div>
