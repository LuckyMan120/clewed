<style>
    .Categories_2.Categories_2_hover li {

        background: none;
        padding-left:20px; !important;
        padding-top:4px !important;
        padding-bottom:4px !important;
        /*border-bottom: solid 1px #d0d2d3;*/

    }
    .Categories_2.Categories_2_hover li a {
        font-size: 14px;
        font-family: 'LatoRegular';
        font-style: italic;
        color:#91939e;
    }

    .Categories_2.Categories_2_hover li a:hover {
        color:#00aad6!important;
    }

    div.Categories_2_hover.Categories_2 li:hover {
        background: none!important;
        padding-left: 20px !important;
    }

    .Categories_2.Categories_2_hover li a.active {
        color:#00aad6;
    }
    div.adtitle span {
        font-family: 'Lato Bold Italic';
        color: #284B54;
        font-size: 14px !important;
    }
    div.adtitle
    {
        /*border-bottom: solid 1px #d0d2d3;*/
    }
</style>

<div class="Categories_2 Categories_2_hover" style="margin-top: 47px; margin-left:16px;margin-right:16px; border:none">
    <?php
    global $AccessObj;
    if ($AccessObj->user_type == 'people') {
        $profile = '<a style="color:#284B54 !important;" href="account?tab=professionals&page=pro_detail&id=' . $AccessObj->uid . '">' . $AccessObj->firstname .'</a>
        <br><a class="edit_account" href="account?tab=professionals&page=pro_detail&id=' . $AccessObj->uid . '">My Account / Projects</a>';
    $profile = '<div class="adtitle">
        <span>'.$profile.'</span></div><br>';
    }
    elseif ($AccessObj->user_type == 'company') {
        $profile = '<a style="color:#284B54 !important;" href="account?tab=companies&page=company_detail&id=' . $AccessObj->uid . '">' . ucfirst($AccessObj->firstname) .'</a>
        <br><a class="edit_account" href="account?tab=companies&page=company_detail&id=' . $AccessObj->uid . '&mtab=about">My Project Pages</a>';
        $profile = '<div class="adtitle">
        <span>'.$profile.'</span></div><br>';
    }
    else $profile = '';
    ?>
    <div class="adtitle">
        <span><?=$profile;?></span>
    </div>
    <div class="adtitle">
        <span>Browse services</span>
    </div>
    <br>
    <ul>
        <li>
            <a <?=($_GET['type'] == '' && $_GET['ftype'] == '' && $_GET['ftype'] == '' && $_GET['tab'] == 'insights')? 'class="active"':''?> href="<?php echo $base_url; ?>/account?tab=insights"

                style="cursor:pointer;">Insights &amp; services</a>
        </li>
    <?php
        global $AccessObj;
        if ($AccessObj->user_type != 'company') {
    ?>

        <li>
            <a href="<?php echo $base_url; ?>/account?tab=insights&type=moderated"
               class="<?= ($_GET['type'] == 'moderated') ? "active" : "" ?>"
               style="cursor:pointer;">Guest expert</a>
        </li>
    <?php } ?>

        <li>
            <a href="<?php echo $base_url; ?>/account?tab=insights&type=following" class="<?= ($_GET['type'] == 'following') ? "active" : "" ?>"
                style="cursor:pointer;">Following</a>
        </li>

    <?php

      if (isset($_REQUEST['id']) && $AccessObj->uid != $_REQUEST['id']) {
          $pro_name = strtolower(getProId($_REQUEST['id']));
          $tmp = explode(" ",$pro_name);
          $url = "account?tab=professionals&page=pro_detail&id=".$_REQUEST['id']."&section=pro_industry_view&type=discussion";
      ?>
          <div style="margin-top:32px;" class="adtitle">
          </div>
          <li>
              <a <?=($_GET['pro_id'] == '') ? 'class="active"' : ''?> href="<?=$url?>"
                    style="cursor:pointer;"><?=ucfirst($tmp[0]);?>`s Insights & Services</a>
          </li>
<?php } ?>
      </ul>

      <?php if ($AccessObj->user_type == 'people') { ?>
          <div style="margin-top:32px;" class="adtitle">
              <span>My Insights & Services</span>
          </div>
          <br>
          <ul>
              <?php
              $isOwn = ($_REQUEST['id'] == $AccessObj->uid);
              $isPrivate = isset($_REQUEST['private']) && $_REQUEST['private'];
              $isPublic = isset($_REQUEST['public']) && $_REQUEST['public'];
              $isActive = isset($_REQUEST['active']) && $_REQUEST['active'];
              $url = "account?tab=professionals&page=pro_detail&id=".$AccessObj->uid."&section=pro_industry_view&type=discussion";
              ?>
              <li>
                  <a <?=($isOwn && $isActive) ? 'class="active"' : ''; ?> href="<?=($url . '&active=1');?>" style="cursor:pointer;">Active Insights & Services</a>
              </li>
              <li>
                  <a <?=($isOwn && $isPublic && !$isActive) ? 'class="active"' : ''; ?> href="<?=($url . '&public=1');?>" style="cursor:pointer;">Insights created</a>
              </li>
              <li>
                  <a <?=($isOwn && $isPrivate && !$isActive) ? 'class="active"' : ''; ?>  href="<?=($url . '&private=1');?>" style="cursor:pointer;">Services created</a>
              </li>
              <li>
                  <a <?=($isOwn && !$isPrivate && !$isPublic && !$isActive) ? 'class="active"' : ''; ?>  href="<?=$url;?>" style="cursor:pointer;">Insights & Services created</a>
              </li>
          </ul>
      <?php } ?>

</div>