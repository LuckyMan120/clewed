<?php
global $base_url;
global $user;
define('__ACCOUNT__', 1);
require_once __DIR__ . '/includes/new_functions.inc';
require_once __DIR__ . '/../../lib/init.php';
date_default_timezone_set('EST');
// $Id: page.tpl.php,v 1.18.2.1 2009/04/30 00:13:31 goba Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>"
      lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
<head>
        <?php
    if (empty($_REQUEST['id'])) {
        echo '<title>Browse Insights | Clewed</title>';
    } else {
        $insightRepository = new \Clewed\Insights\InsightRepository();
        $insight = $insightRepository->findById($_REQUEST['id']);
        if (!$insight->isApproved() || $insight->isPrivateInsight()) {
            echo '<title>Insight not scheduled yet</title>';
        } else {
            echo '<title>' . htmlspecialchars($insight->title) . '</title>';
        }

    }
    echo $styles;
    echo $scripts;
    ?>
    <!--[if lt IE 7]>
    <?php print phptemplate_get_ie_styles(); ?>
    <![endif]-->

    <link type="text/css"  rel="stylesheet" href="/themes/maennaco/insights.css"/>

    <?php if ( $user->uid > 1 ) : ?>
        <?php

        $redirect = $base_url . '/account';
        if(!empty($_REQUEST['id']))
             $redirect = $base_url . '/account?tab=professionals&page=pro_detail&id=' . $insight->postedby . '&section=pro_industry_view&type=details&pro_id=' . $insight->id;
        ?>
        <script type="text/javascript">
            window.location = "<?= $redirect;?>";
        </script>
    <?php endif; ?>
</head>
<body<?php echo phptemplate_body_class($left, $right); ?>>
<link type="text/css"
      href="/themes/maennaco/jui/css/redmond/homepagecss/jquery-ui-1.8.15.custom.css"
      rel="stylesheet"/>
<script type="text/javascript" src="/themes/maennaco/jui/js/jquery-1.6.2.min.js"></script>
<script type="text/javascript" src="/themes/maennaco/jui/js/jquery-ui-1.9.2.custom.min.js"></script>
<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js"></script>
<script type="text/javascript" src="/themes/maennaco/jui/comments/js/SpryTabbedPanels.js"></script>
<script type='text/javascript' src='/themes/maennaco/fullcalendar.js'></script>
<script type="text/javascript" src="/js/register-dialog.js"></script>
<script type="text/javascript" src="/js/insights.js"></script>
<link type="text/css" rel="stylesheet" media="screen" href="/themes/maennaco/jui/comments/css/screen.css?as"/>
<link type="text/css" rel="stylesheet" href="/themes/maennaco/jui/comments/css/SpryTabbedPanels.css"/>
<link type="text/css" rel="stylesheet" href="/themes/maennaco/fullcalendar.css"/>
<link type="text/css" rel="stylesheet" href="/css/insights.css"/>

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
</script>

<!-- Layout -->

<div id="wrapper">
    <div id="container" class="clear-block page-insights" style="margin-top: 0 !important;">

        <?php require_once("header-regular.php");?>
        <!-- /header -->

        <br style="clear:both;">

        <div id="center">
            <div id="squeeze">
                <div class="right-corner">
                    <div class="left-corner" style='padding:0;margin:0'>
                        <div style="display: block;">
                            <?php if (!isset($_REQUEST['id'])):?>
                                <div class="insights-list-caption content clear-block">
                                    <div class="index-text-a">Operate with industry intelligence and experience.</div>
                                    <div class="index-text-a-blue">
                                        Access proprietary research and management services – hassle free.
                                    </div>
                                </div>
                            <?php endif;?>
                            <div
                                style="display:none;position:relative;margin-left:auto; margin-right:auto; width:263px;height:40px;">
                                <div style="position:absolute;width:100px;top:0;left:0px;margin:0;padding:0;">
                                    <a href="https://twitter.com/share" class="twitter-share-button"
                                       data-count="none">Tweet</a>
                                    <script type="text/javascript">
                                        !function (d, s, id) {
                                            var js, fjs = d.getElementsByTagName(s)[0];
                                            if (!d.getElementById(id)) {
                                                js = d.createElement(s);
                                                js.id = id;
                                                js.src = "//platform.twitter.com/widgets.js";
                                                fjs.parentNode.insertBefore(js, fjs);
                                            }
                                        }(document, "script", "twitter-wjs");</script>
                                </div>
                                <div style="position:absolute;width:100px;top:10px;left:80px;">
                                    <script src="//platform.linkedin.com/in.js" type="text/javascript"></script>
                                    <script type="IN/Share" data-url="www.maennaco.com" data-counter="right"></script>
                                </div>
                                <div style="position:absolute;width:100px;top:10px;left:150px;">
                                    <iframe
                                        src="//www.facebook.com/plugins/like.php?href=http%3A%2F%2Fwww.maennaco.com&amp;send=false&amp;layout=button_count&amp;width=450&amp;show_faces=false&amp;action=like&amp;colorscheme=light&amp;font&amp;height=21&amp;appId=267809316574109"
                                        scrolling="no" frameborder="0"
                                        style=" margin-left:30px;border:none; overflow:hidden; width:80px; height:21px;"
                                        allowTransparency="true"></iframe>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        <div id="eveditdlg"></div>
        <?php if (isset($_REQUEST['id'])) {
            require 'insight/insight.php';
        } else {
            require 'insight/insights.php';
        } ?>
<!--        --><?php //require_once("footer-regular.php");?>
    </div>

    <div style="clear:both;text-align:left;display:none">
        <a href=""><img src="/<?php echo path_to_theme(); ?>/images/facebook-icons.jpg" style="display:inline"></a>
        <a href=""><img src="/<?php echo path_to_theme(); ?>/images/twitter-icons.jpg" style="display:inline"></a>
    </div>

</div>
<?php require_once("footer-regular.php");?>

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
require 'dialogs/learn-more.php';
//require 'dialogs/register.php';
?>
<div id="policy" style="font-family: 'Lato Regular', sans-serif; font-size: 12px; display: none;"></div>

</body>
</html>
