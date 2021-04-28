<?php
global $user, $base_url;
define('__ACCOUNT__', 1);
require_once 'includes/new_functions.inc';

// $Id: page.tpl.php,v 1.18.2.1 2009/04/30 00:13:31 goba Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xmlns="http://www.w3.org/1999/html"
      xml:lang="<?php print $language->language ?>"
      lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
<head>

    <title><?php echo empty($_REQUEST['id']) ? $head_title : 'About Company'; ?></title>

    <?php print $styles ?>
    <?php print $scripts ?>
    <!--[if lt IE 7]>
    <?php print phptemplate_get_ie_styles(); ?>
    <![endif]-->
    <?php if ($user->uid > 1) : ?>
        <?php

        $redirect = $base_url . '/account';
        if(!empty($_REQUEST['id']))
            $redirect = $base_url . '/account?tab=companies&page=company_detail&id=' . ((int) $_REQUEST['id']);
        ?>
        <script type="text/javascript">
            window.location = "<?= $redirect;?>";
        </script>
    <?php endif; ?>

    <link rel="stylesheet" type="text/css" href="/css/companies.css"/>
    <link rel="stylesheet" type="text/css"
          href="/themes/maennaco/jui/css/redmond/homepagecss/jquery-ui-1.8.15.custom.css"/>
    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-1.6.2.min.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-ui-1.9.2.custom.min.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js"></script>
    <script type="text/javascript" src="/js/companies.js"></script>
</head>
<body<?php print phptemplate_body_class($left, $right); ?>>
<div id="fb-root"></div>
<script type="text/javascript">
    (function (d, s, id) {
        var js, fjs = d.getElementsByTagName(s)[0];
        if (d.getElementById(id)) return;
        js = d.createElement(s);
        js.id = id;
        js.src = "//connect.facebook.net/en_GB/all.js#xfbml=1&appId=267809316574109";
        fjs.parentNode.insertBefore(js, fjs);
    }(document, 'script', 'facebook-jssdk'));

    function show_hide(id_show, id_hide, bidblue, bidblack) {
        if (id_show == 'tabs-1') {
            window.utype = 'company';
        }
        else if (id_show == 'tabs-2') window.utype = 'professional';
        document.getElementById(id_show).style.display = 'block';
        document.getElementById(id_hide).style.display = 'none';
        document.getElementById(bidblue).style.backgroundColor = '#00a2bf';
        document.getElementById(bidblack).style.backgroundColor = '#D0D2D2';
        document.getElementById('form-submit1').style.display = 'block';
        document.getElementById('form-submit2').style.display = 'block';
    }
</script>

<!-- Layout -->

<div id="wrapper">
    <div id="container" class="clear-block custom-clear-block" style="margin-top:0px !important;">
        <?php require_once("header-regular.php");?>
        <!-- /header -->
        <br style="clear:both;">
        <?php if (isset($_REQUEST['id'])) {
            require './lib/clewed/company/template/about.php';
        } else {
            require 'companies/companies.php';
        } ?>
    </div>
    <?php if (!isset($_REQUEST['id']))
        require 'companies/companies-join.php';
    ?>
</div>

    <?php require_once("footer-regular.php");?>

<div style="clear:both;text-align:left;display:none">
    <a href=""><img src="/<?php echo path_to_theme(); ?>/images/facebook-icons.jpg" style="display:inline"></a>
    <a href=""><img src="/<?php echo path_to_theme(); ?>/images/twitter-icons.jpg" style="display:inline"></a>
</div>

<!-- /layout -->

<?php print $closure ?>
<div id='apply_account_type' class="hide">
    <div id="dialog" style="padding:0px 30px 0px 10px;height:20px;line-height:20px;">
        I represent a <a href="/company-apply-form">company</a> or a <a href="/people-apply-form">professional</a>.
    </div>
</div>

<script type="text/javascript">
    (function (i, s, o, g, r, a, m) {
        i['GoogleAnalyticsObject'] = r;
        i[r] = i[r] || function () {
            (i[r].q = i[r].q || []).push(arguments)
        }, i[r].l = 1 * new Date();
        a = s.createElement(o),
            m = s.getElementsByTagName(o)[0];
        a.async = 1;
        a.src = g;
        m.parentNode.insertBefore(a, m)
    })(window, document, 'script', '//www.google-analytics.com/analytics.js', 'ga');

    ga('create', 'UA-43665911-1', 'clewed.com');
    ga('send', 'pageview');

</script>


<?php
require 'dialogs/contrib.php';
require 'dialogs/comp.php';
require 'dialogs/login.php';
?>

<div id="policy" style="font-family: 'Lato Regular', sans-serif; font-size:12px; display:none;"></div>

</body>
</html>
