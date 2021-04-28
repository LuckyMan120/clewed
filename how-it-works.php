<?php
require_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
$return = menu_execute_active_handler();
if (is_int($return)) {
    switch ($return) {
        case MENU_NOT_FOUND:
            drupal_not_found();
            break;
        case MENU_ACCESS_DENIED:
            drupal_access_denied();
            break;
        case MENU_SITE_OFFLINE:
            drupal_site_offline();
            break;
    }
}
include_once 'sites/all/modules/maenna_configuration/maenna_configuration.module';

?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en"
      lang="en" dir="ltr">
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
        <title>How It Works | Clewed</title>
        <link type="text/css" rel="stylesheet" media="all" href="/modules/cck/theme/content-module.css?v=1.0"/>
        <link type="text/css" rel="stylesheet" media="all" href="/modules/node/node.css?v=1.0"/>
        <link type="text/css" rel="stylesheet" media="all" href="/modules/system/defaults.css?v=1.0"/>
        <link type="text/css" rel="stylesheet" media="all" href="/modules/system/system.css?v=1.0"/>
        <link type="text/css" rel="stylesheet" media="all" href="/modules/system/system-menus.css?v=1.0"/>
        <link type="text/css" rel="stylesheet" media="all" href="/modules/user/user.css?v=1.0"/>
        <link type="text/css" rel="stylesheet" media="all" href="/sites/all/modules/ckeditor/ckeditor.css?v=1.0"/>
        <link type="text/css" rel="stylesheet" media="all" href="/modules/cck/modules/fieldgroup/fieldgroup.css?v=1.0"/>
        <link type="text/css" rel="stylesheet" media="all" href="/sites/all/modules/views/css/views.css?v=1.0"/>
        <link type="text/css" rel="stylesheet" media="all" href="/themes/maennaco/style.css?v=1.0"/>
        <link type="text/css" rel="stylesheet" media="all" href="/themes/maennaco/maennaco.css?v=1.0"/>
        <link type="text/css" rel="stylesheet" media="print" href="/themes/maennaco/print.css?v=1.0"/>
        <script type="text/javascript" src="/misc/jquery.js?v=1.0"></script>
        <script type="text/javascript" src="/misc/drupal.js?v=1.0"></script>
        <script type="text/javascript" src="/sites/all/libraries/tinymce/jscripts/tiny_mce/tiny_mce.js?v=1.0"></script>
        <script type="text/javascript" src="/themes/maennaco/maennaco.js?v=1.0"></script>
        <!--[if lt IE 7]>
        <link type="text/css" rel="stylesheet" media="all" href="/themes/maennaco/fix-ie.css?v=1.0"/><![endif]-->
        <link type="text/css" rel="stylesheet" href="/css/how-it-works.css?v=1.0"/>
        <link type="text/css" rel="stylesheet" href="/themes/maennaco/jui/css/redmond/homepagecss/jquery-ui-1.8.15.custom.css?v=1.0"/>
        <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-1.6.2.min.js?v=1.0"></script>
        <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-ui-1.8.15.custom.min.js?v=1.0"></script>
        <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js?v=1.0"></script>
        <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js?v=1.0"></script>
        <script type="text/javascript" src="/js/how-it-works.js?v=1.0"></script>
        <script type="text/javascript" src="/js/register-dialog.js?v=1.0"></script>
        <?php include_once 'templates/header.php' ?>
    </head>
    <body>
        <div class="hiw-line" id="hiw-top-line"></div>
        <div class="hiw-line" id="hiw-bottom-line"></div>
        <div id="wrapper">
            <div id="container" class="clear-block">
                <div id="header">
                    <div class="logo">
                        <a href="/">
                            <img src="/themes/maennaco/images/index_logo_1.png" alt="website logo"/>
                        </a>
                    </div>
                    <?php $currentPage = $_SERVER["REQUEST_URI"]; ?>

                    <div class="homemenu">
                        <div class="item first <?php if($currentPage=="/about/the-company") echo "selected"; ?>"><a href="/about/the-company">Learn <br/>More</a></div>
                        <div style="border-left:none;" class="item last <?php if($currentPage=="/professionals") echo "selected"; ?>"><a href="/professionals">Join as <br/>professional</a></div>
                        <div class="item last <?php if($currentPage=="/insights") echo "selected"; ?>"><a href="/insights">Insights &<br>Services123</a></div>
                        <div style="border-left:none;" class="item last <?php if($currentPage=="/companies") echo "selected"; ?>"><a href="/companies">Discover<br/>Opportunities </a></div>
                    </div>
                    <div class="loginStuff">
                        <?php if ($user->uid) { ?>
                            <a id="my_account" href="/account"
                               style="font-family:'Lato Regular', sans-serif; font-size:10px; font-weight:bold;cursor:pointer; margin-left:20px;">
                                MY ACCOUNT >
                            </a>
                            <a href="/logout">
                                <input type="submit"
                                       style="display:inline-block; margin:0 0 0 20px; width:97px;font-size:15px;font-family: 'Lato Black', sans-serif;height:32px;"
                                       name="op" id="edit-submit" value="LOG OUT" class="form-submit">
                            </a>
                        <?php } ?>
                    </div>
                    <div id="user-status" class="loginStuff">
                        <div style="display:none">
                            <?php get_user_status_home($user); ?>
                        </div>
                        <a style="cursor: pointer;text-decoration: none;" class='header-login'>LOG IN</a>&nbsp;&nbsp;
                        <a style="cursor: pointer;text-decoration: none;" class='header-register'>REGISTER</a>
                        <br style="clear:both;">
                        <a href="<?php echo $base_url;?>/user/password">Forgot password?</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                        <a href='/about/contact-us'><span style="color: #898B8E;font-family: 'Lato Italic'; font-size: 13px; font-weight: normal; padding: 5px 3px 0 0;">In beta,</span> feedback?</a>&nbsp;&nbsp;
                    </div>
                </div>
                <!-- /header -->
                <?php

                if ($_REQUEST['pro']) $subtitle = "Old business models are outdated to meet today's demands. The Clewed platform enables professionals to connect with companies and opportunities and thrive more easily.";

                else $subtitle = "Old business models are being disrupted. The Clewed platform enables you to plug into the expertise, relationships and capital you need to compete today.";

                if ($_REQUEST['pro']) $hiw = "How it works for professionals";

                else $hiw = "How it works for companies";

                ?>
                <div style="clear:both;"></div>
                <div id="center">
                    <div id="squeeze">
<!--                        <div class="right-corner">-->
<!--                            <div class="left-corner" style='padding:0;margin:0'>-->
<!--                                <div class="clear-block">-->
                                    <div id="hiw-banner">
                                        <h1>Why Clewed?</h1>
                                        <div id="hiw-banner-text">
                                            We believe companies that can grow at a high rate over a long period of time provide the best investment opportunity available – as they allow the power of compound interest to work for them. So, we’ve built a system to help such companies to continually transform, find partners and grow – while enabling us to invest and grow with their business for as long as possible.
                                            <br/>
                                            <div style="margin-top:27px;">"Our favorite holding period is forever."
                                                    <span style="font-size:12px;">Warren Buffett.</span>
                                            </div>
                                        </div>
                                    </div>
                                    <h1 id="hiw"><?php echo $hiw?></h1>
                                    <!--?
                                    /*<div id="hiw_tabs">
                                        <a href="/how-it-works.php" class="hiw_tab <?if(!$_REQUEST['pro']):?>active<?endif?>"><span></span>FOR COMPANIES</a>
                                        <a href="/how-it-works.php?pro=1" class="hiw_tab <?if($_REQUEST['pro']):?>active<?endif?>"><span></span>FOR PROFESSIONALS</a>
                                    </div>*/
                                    ?-->
<!--                                </div>-->
                                <?php require 'templates/how-it-works/professional.php' ?>
                                <?php require 'templates/how-it-works/company.php' ?>
                                <div style="margin-left: 350px;">
                                    <a href="#" id="home-register-new1" class="home-register">GET STARTED<br><span>&raquo; Register for FREE</span></a>
                                </div>
<!--                            </div>-->
<!--                        </div>-->
                    </div>
                </div>
            </div>
            <!-- /container -->
            <?php require_once("themes/maennaco/footer-regular.php"); ?>
        </div>
        <div id="policy" style="font-family: 'Lato Regular', sans-serif; font-size:12px; display:none;"></div>

        <div id="learn-more-dialog" style="display:none;">
            <?php require_once('themes/maennaco/dialogs/learn-more-nav.php') ?>
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
    </body>
</html>
