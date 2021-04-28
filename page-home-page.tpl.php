<?php
// $Id: page.tpl.php,v 1.18.2.1 2009/04/30 00:13:31 goba Exp $
global $user;
global $AccessObj;

?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">page

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>"
    lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">

<head>
    <?php print $head ?>
    <title>
        <?php print $head_title ?>
    </title>
    <?php print $styles ?>
    <?php print $scripts ?>
    <!--[if lt IE 7]>
    <?php print phptemplate_get_ie_styles(); ?>
    <![endif]-->

    <link type="text/css" rel="stylesheet" href="/css/homepage.css" />
    <link type="text/css" rel="stylesheet"
        href="/themes/maennaco/jui/css/redmond/homepagecss/jquery-ui-1.8.15.custom.css" />
    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-1.6.2.min.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-ui-1.8.15.custom.min.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js"></script>
    <script type="text/javascript" src="/js/homepage.js"></script>
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
    </script>

    <!-- Layout -->

    <div id="wrapper">
        <div id="container" class="clear-block" style="margin-top:20px !important;">

            <div id="header">
                <div style="position:relative;width:220px;float:left;">
                    <a href="/account"><img src="/<?php echo path_to_theme(); ?>/images/index_logo.png"
                            alt="website logo" border=0 width=220></a>

                </div>

                <div style="position:relative;width:220px;margin-right:50px;float:right;">
                    <?php if (!$user->uid) {
                    ?><a id="learn-more"
                        style="font-family: Helvetica, sans-serif; font-size:10px; font-weight:bold;cursor:pointer;">LEARN
                        MORE
                        > </a>
                    <?php
                } else {
                    ?><a id="my_account" href="/account"
                        style="font-family: Helvetica, sans-serif; font-size:10px; font-weight:bold;cursor:pointer;">MY
                        ACCOUNT
                        > </a>
                    <?php
                } ?>
                </div>

                <div id="user-status" style="float:left;">
                    <?php get_user_status_home($user); ?>
                </div>

                <?php if (!$is_front) get_topnavmenu(); ?>
            </div>
            <!-- /header -->

            <br style="clear:both;"><br>

            <div id="center" style='margin-top:55px;'>
                <div id="squeeze">
                    <div class="right-corner">
                        <div class="left-corner" style='padding:0;margin:0'>

                            <div class="clear-block">
                                <?php echo $content; ?>
                                <a href="#" id="home-register" class="home-register">[[[[REGISTER]]]]</a>

                                <div
                                    style="position:relative;margin-left:auto; margin-right:auto; width:263px;height:40px;">
                                    <div style="position:absolute;width:100px;top:0;left:-10px;margin:0;padding:0;">
                                        <a href="https://twitter.com/share" class="twitter-share-button">Tweet</a>
                                        <script type="text/javascript">!function (d, s, id) {
                                                var js, fjs = d.getElementsByTagName(s)[0];
                                                if (!d.getElementById(id)) {
                                                    js = d.createElement(s);
                                                    js.id = id;
                                                    js.src = "//platform.twitter.com/widgets.js";
                                                    fjs.parentNode.insertBefore(js, fjs);
                                                }
                                            }(document, "script", "twitter-wjs");</script>
                                    </div>
                                    <div style="position:absolute;width:100px;top:10px;left:90px;">
                                        <script src="//platform.linkedin.com/in.js" type="text/javascript"></script>
                                        <script type="IN/Share" data-url="www.clewed.com" data-counter="right"></script>
                                    </div>
                                    <div style="position:absolute;width:100px;top:10px;left:161px;">
                                        <iframe
                                            src="//www.facebook.com/plugins/like.php?href=http%3A%2F%2Fwww.clewed.com&amp;send=false&amp;layout=button_count&amp;width=450&amp;show_faces=false&amp;action=like&amp;colorscheme=light&amp;font&amp;height=21&amp;appId=267809316574109"
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
            <!-- /.left-corner, /.right-corner, /#squeeze, /#center -->

        </div>
        <!-- /container -->
        <div id="footer-new">
            <div id="footer-wrapper">
                <div id="footer-right-column">
                    <div id="rc-icon"><img src="themes/maennaco/images/research-icon.png"></div>
                    <br>
                    <span class="title">RESEARCH</span>

                    <div id="rc-content">We start with a long-term mindset and a rigorous analysis to generate a plan
                        that
                        enables clients to win markets.
                    </div>
                </div>
                <div class="headerdivider"></div>
                <div id="footer-right-column">
                    <div id="rc-icon"><img src="themes/maennaco/images/advice-icon.png"></div>
                    <br>
                    <span class="title">ADVICE</span>

                    <div id="rc-content"> A customized advisory team of leading executives and analysts infuses
                        strategic
                        insights and provide innovative problem-solving.
                    </div>
                </div>
                <div class="headerdivider"></div>
                <div id="footer-right-column">
                    <div id="rc-icon"><img src="themes/maennaco/images/monitor-icon.png"></div>
                    <br>
                    <span class="title">MONITOR</span>

                    <div id="rc-content"> We continuously measure risks and performance and iterate advice for new
                        learning
                        to
                        ensure we compound high growth.
                    </div>
                </div>
                <br style="clear:both;">

                <div style="margin-left:auto; margin-right:auto; margin-top:50px;width:420px; font-family:Helvetica;"><a
                        class="show_terms" type="terms" target="_blank"
                        style="cursor:pointer; font-family:Helvetica;">Terms
                        of
                        Use</a><a class="show_terms" type="privacy"
                        style=" cursor:pointer;margin-left:20px;font-family:Helvetica;">Privacy
                        Policy</a>
                    <span style="margin-left:20px;">&#169; 2020 clewed. All Rights Reserved.</span>
                </div>

            </div>
        </div>

        <div style="clear:both;text-align:left;display:none">
            <a href=""><img src="/<?php echo path_to_theme(); ?>/images/facebook-icons.jpg" style="display:inline"></a>
            <a href=""><img src="/<?php echo path_to_theme(); ?>/images/twitter-icons.jpg" style="display:inline"></a>
        </div>

    </div>
    <!-- /layout -->

    <?php print $closure ?>
    <div id='apply_account_type' class="hide">
        <div id="dialog" style="padding:0px 30px 0px 10px;height:20px;line-height:20px;">
            I represent a <a href="/company-apply-form">company</a> or a <a href="/people-apply-form">professional</a>.
        </div>
    </div>
    <script type="text/javascript">

        var _gaq = _gaq || [];
        _gaq.push(['_setAccount', 'UA-23393148-1']);
        _gaq.push(['_trackPageview']);

        (function () {
            var ga = document.createElement('script');
            ga.type = 'text/javascript';
            ga.async = true;
            ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
            var s = document.getElementsByTagName('script')[0];
            s.parentNode.insertBefore(ga, s);
        })();

    </script>
    <div id="learn-more-dialog" style="display:none;">
        <div class="dialog-left-column">
            <a id="wwb" class="dialog-left-column-title dialog-selected" style="cursor:pointer;">What We Believe</a><br>
            <a id="wwd" class="dialog-left-column-title" style="cursor:pointer;">What We Do</a><br>
            <a id="wsa" class="indented" style="cursor:pointer;">&#8226; What Sets Us Apart</a><br>
            <a id="companies" class="indented" style="cursor:pointer;">&#8226; Company Criteria</a><br><br>
            <a id="wwa" class="dialog-left-column-title" style="cursor:pointer;">Who We Are</a><br>
            <a id="advisors" class="indented" style="cursor:pointer;">&#8226; Advisor Criteria</a><br><br>
            <a id="investors" class="dialog-left-column-title" style="cursor:pointer;">Investors</a><br>
            <a id="partners" class="dialog-left-column-title" style="cursor:pointer;">Partners</a><br>
            <a id="contactus" class="dialog-left-column-title" style="cursor:pointer;">Contact Us</a><br>
        </div>
        <div class="verticaldivider"></div>
        <div class="dialog-right-column">
            <span class="dialog-right-column-title">What We Believe</span><br><br>
            We believe small to mid-size companies have the highest unrecognized potential and
            must connect more seamlessly with leading owner-minded professionals to realize their
            long-term potential. We created our platform to allow clients to compound maximum
            growth for as long as possible because doing so generates far greater wealth than any
            short-term fix.
        </div>
        <div id="register-dialog" style="display:none;">
            <div id="tabs">
                <ul>
                    <li id="reg-company"><a href="#tabs-1">COMPANY</a></li>
                    <li id="reg-company"><a id="reg-proffesional" href="#tabs-2">PROFESSIONAL</a></li>
                </ul>
                <input style="margin-left:30px; margin-top:7px;" type="checkbox" id="selectedType"><span
                    style="color:#4169AF; font-size:10px;">I have selected the correct form above</span>

                <div id="tabs-1">
                    <div id="reg-form" style="color:#4169AF;">
                        * <input class="required" type="text" id="company-firstname">
                        * <input class="required" type="text" id="company-lastname">
                        * <input class="required" type="text" id="company-name">
                        * <input class="required" type="text" id="company-email">
                        * <input class="required" type="password" id="company-password">
                        <span style="margin-left:10px;font-size:10px;"> 8 character minimum, at least one
                            number</span><br>

                        <select name="company-industry" id="company-industry">
                            <option value="industry">INDUSTRY</option>
                            <?php
                        $Sectors = _INDUSTRY();
                        foreach ($Sectors as $key => $value) {
                            echo "<optgroup label=\"$key\">";
                            foreach ($value as $key1 => $value1) echo "<option value=\"$key1\">$value1</option>";
                            echo "</optgroup>";
                        }
                        ?>
                        </select>
                        <input style="margin-left:20px; margin-top:7px;" type="checkbox" id="cmp-agree">
                        <label style="font-family:Helvetica; font-size:9px; color:#2E2F34;" for="cmp-agree">I agree with
                            Clewed's <a class="show_terms" type="terms" target="_blank" style="color:#4169AF;">Terms</a>
                            and <a target="_blank" class="show_terms" type="privacy" style="color:#4169AF;">Privacy
                                Policy</a></label>

                        <br>

                        <div style="float:left;" id='captcha_img' style='height:39px;width:110px;'><img height="39px"
                                width="110px" src='/captcha/captcha.php'>
                        </div>
                        <div></div>
                        <input class="required" style="float:left;width:125px;" type="text" id="company-captcha">
                    </div>
                    <br style="clear:both;"><a href="#" id="reload_captcha" style="font-size:11px;">Not readable? Change
                        text.</a>
                </div>
                <div id="tabs-2">

                    <div id="reg-form" style="color:#4169AF;">
                        <select name="pro-type" id="pro-type">
                            <option value="iam">I AM</option>
                            <?php
                        $ProType = _pro_types();
                        foreach ($ProType as $key => $value) {
                            echo "<option value=\"$key\">$value</option>";
                        }
                        ?>
                        </select>
                        * <input class="required" type="text" id="pro-firstname">
                        * <input class="required" type="text" id="pro-lastname">
                        * <input class="required" type="text" id="pro-email">
                        * <input class="required" type="password" id="pro-password">
                        <span style="margin-left:10px;font-size:10px;"> 8 character minimum, at least one
                            number</span><br>

                        <select name="pro-experties" id="pro-experties">
                            <option value="experties">EXPERTIES</option>
                            <?php
                        $Experties = _experties();
                        foreach ($Experties as $key => $value) {
                            echo "<optgroup label=\"$key\">";
                            foreach ($value as $key1 => $value1) echo "<option value=\"$key1\">$value1</option>";
                            echo "</optgroup>";
                        }
                        ?>
                        </select>
                        <input style="margin-left:20px; margin-top:7px;" type="checkbox" id="pro-agree">
                        <label style="font-family:Helvetica; font-size:9px; color:#2E2F34;" for="pro-agree">I agree
                            with
                            Clewed's
                            <a class="show_terms" type="terms" target="_blank" style="color:#4169AF;">Terms</a> and
                            <a class="show_terms" type="terms" target="_blank" style="color:#4169AF;">Privacy
                                Policy</a></label>
                        <br>

                        <div style="float:left;" id='captcha_img' style='height:39px;width:110px;'><img height="39px"
                                width="110px" src='/captcha/captcha.php'></div>
                        <div></div>
                        <input class="required" style="float:left;width:125px;" type="text" id="pro-captcha">
                    </div>
                    <br style="clear:both;"><a href="#" id="reload_captcha" style="font-size:11px;">Not readable? Change
                        text.</a>
                </div>
            </div>
        </div>
    </div>

    <div id="policy" style="font-family: Helvetica, arial; font-size:12px; display:none;"></div>

    </body>

</html>