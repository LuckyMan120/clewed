<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);
global $base_url;
if ($user == null) global $user;
define("__ACCOUNT__", 1);
require_once("includes/new_functions.inc");
require_once("includes/maenna_access.inc");
require_once("includes/maenna_page.inc");
global $AccessObj, $Maenna_page;

$AccessObj = new Maenna_access();
$Maenna_page = new Maenna_page($AccessObj);
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>"
      lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
<head>
    <title><?php print $head_title ?></title>
    <?php print $styles ?>
    <?php //print $scripts ?>
    <!--[if lt IE 7]>
    <?php print phptemplate_get_ie_styles(); ?>
    <![endif]-->
    <link href='https://fonts.googleapis.com/css?family=Signika' rel='stylesheet' type='text/css'>
    <link type="text/css" href="/themes/maennaco/jui/css/redmond/jquery-ui-1.8.15.custom.css" rel="stylesheet"/>
    <link href="/themes/maennaco/jui/comments/css/screen.css?as" type="text/css" rel="stylesheet"/>
    <link href="/themes/maennaco/jui/comments/css/autosuggest.css" type="text/css" rel="stylesheet"/>
    <link href="/themes/maennaco/jui/comments/css/fileuploader.css" type="text/css" rel="stylesheet"/>
    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-1.6.2.min.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-ui-1.9.2.custom.min.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery.formatCurrency.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/js/maxlength.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.autosuggest.js"></script>
    <script src="/themes/maennaco/jui/comments/js/jquery.elastic.js" type="text/javascript" charset="utf-8"></script>
    <script src="/themes/maennaco/jui/comments/js/fileuploader.js" type="text/javascript" charset="utf-8"></script>
    <script src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js" type="text/javascript"></script>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0"/>

    <script type="text/javascript">
        $(function () {
            var tab = "<?php echo $_REQUEST['tab']; ?>";
            var emailRegex = new RegExp(/^([\w\.\-]+)@([\w\-]+)((\.[\w\-]+)+)$/i);
            if (tab == 'companies' || tab == 'professionals' || tab == 'questionnaire' || tab == 'settings') {
                original_mail = $('#edit-email').val();
            }
            $("#edit-email").bind('focusout', function () {
                thisObj = $(this);
                if (thisObj.parent().parent().parent().attr('id') == 'maenna-login-form') {
                    return;
                }
                if ($("#edit-email").val() == '') {
                    alert('Please update your email');
                    $('#edit-email').attr('style', 'background-color: rgb(255, 228, 228) !important');
                    $('#edit-next').attr('disabled', true);
                    return;
                } else if (!emailRegex.test($("#edit-email").val())) {
                    alert('Please insert a valid email address');
                    $('#edit-email').attr('style', 'background-color: rgb(255, 228, 228) !important');
                    $('#edit-next').attr('disabled', true);
                    return;
                }
                if ((tab == 'companies' || tab == 'professionals' || tab == 'questionnaire' || tab == 'settings') && $("#edit-email").val() != original_mail) {
                    $.post("check_email_exists.php",
                        {email: $("#edit-email").val()},
                        function (response) {
                            if (response == 'false') {
                                alert("User with this email already exists.Please use another one.");
                                $('#edit-next').attr('disabled', true);
                                $('#edit-email').attr('style', 'background-color: rgb(255, 228, 228) !important');
                                thisObj.focus();
                            } else {
                                $('#edit-next').removeAttr('disabled');
                                $('#edit-email').attr('style', 'background-color:none;');
                            }
                        });
                } else if ((tab == 'companies' || tab == 'professional' || tab == 'questionnaire') && $("#edit-email").val() == original_mail) {
                    $('#edit-next').removeAttr('disabled');
                    $('#edit-email').attr('style', 'background-color:none;');
                } else {
                    return;
                }
            });
            if (tab == 'settings') {
                function checkSettings() {
                    if ($('form').attr('disabled')) {
                        return false;
                    }
                    return false;
                }
            }
            $("#popup_layer").dialog({
                modal: true,
                autoOpen: false,
                width: "920",
                height: "620"
            });
            var left_height = $('table.account-table').height();
            $('td.left-td .left-td-wrapper').css('min-height', left_height + 32);
            $('td.right-td').css('min-height', left_height - 10);
        });
        function showmoretopics() {
            $('#showmore').show();
            $('#showmoretopics').hide();
        }
        function showlesstopics() {
            $('#showmore').hide();
            $('#showmoretopics').show();
            $('#showlesstopics').hide();
        }
        function checktags() {
            if (document.getElementById('tags').value == '') {
                alert("Please select Tags");
            } else {
                return true;
            }
        }
    </script>
    <?php
    if ($_GET['tab'] == 'messages') {
        if ($_GET['action'] == 'newmessage' || $_GET['action'] == 'newmessagepost') {
            print '<link type="text/css" href="' . $base_url . '/themes/maennaco/jquery.liveSearch.css" rel="stylesheet" />'
                . '<script type="text/javascript" src="' . $base_url . '/themes/maennaco/jquery.liveSearch.js"></script>'
                . '<link type="text/css" href="' . $base_url . '/themes/maennaco/inbox-outbox.css" rel="stylesheet" />'
                . '<link type="text/css" href="' . $base_url . '/themes/maennaco/newmessage.css" rel="stylesheet" />'
                . '<script type="text/javascript" src="' . $base_url . '/themes/maennaco/newmessage.js"></script>'
                . '<script type="text/javascript" src="' . $base_url . '/themes/maennaco/jquery.cookie.js"></script>'
                . '<link type="text/css" href="' . $base_url . '/themes/maennaco/flexigrid.pack.css" rel="stylesheet" />'
                . '<script type="text/javascript" src="' . $base_url . '/themes/maennaco/flexigrid.pack.js"></script>';
        } else {
            print '<link type="text/css" href="' . $base_url . '/themes/maennaco/inbox-outbox.css" rel="stylesheet" />'
                . '<script type="text/javascript" src="' . $base_url . '/themes/maennaco/jquery.cookie.js"></script>'
                . '<link type="text/css" href="' . $base_url . '/themes/maennaco/flexigrid.pack.css" rel="stylesheet" />'
                . '<link type="text/css" href="' . $base_url . '/themes/maennaco/newmessage.css" rel="stylesheet" />'
                . '<script type="text/javascript" src="' . $base_url . '/themes/maennaco/flexigrid.pack.js"></script>';
        }
    }
    ?>
    <script type="text/javascript">
        function positionFooter() {
            var mFoo = $("#footer");
            if ((($(document.body).height() + mFoo.height()) < $(window).height() && mFoo.css("position") == "fixed") || ($(document.body).height() < $(window).height() && mFoo.css("position") != "fixed")) {
                mFoo.css({position: "fixed", bottom: "0px"});
            } else {
                mFoo.css({position: "static"});
            }
        }
        $(document).ready(function () {
            positionFooter();
            $(window).scroll(positionFooter);
            $(window).resize(positionFooter);
            $(window).load(positionFooter);
        });
    </script>
    <script type="text/javascript">

        <?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

        function discussion(id, status) {
            $.ajax({
                type: 'get',
                url: '/themes/maennaco/includes/delete.php?' +
                    'type=discussion_status&' +
                    'id=' + id + '&' +
                    'status=' + status + "&" +
                    "u=<?php echo $u;?>&" +
                    "m=<?php echo $m;?>",
                data: '',
                beforeSend: function () {
                },
                success: function (msg) {
                    if (status == 1) {
                        $('.statuson2').html("<a href='javascript:void(0);' onclick='discussion(" + id + ",0);'>ON</a>&nbsp;");
                        $('.statuson1').html("OFF");
                    }
                    else {
                        $('.statuson2').html("<a href='javascript:void(0);' onclick='discussion(" + id + ",1);'>OFF</a>&nbsp;");
                        $('.statuson1').html("ON");
                    }
                }
            });
        }

        function follow_discussion(type, prof_id, user_id) {
            if (type == 'follow') {
                var status = 1;
            } else {
                var status = 0;
            }
            $.ajax({
                type: 'get',
                url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
                    'type=followdis&' +
                    'prof_id=' + prof_id + "&" +
                    "user_id=" + user_id + "&" +
                    "status=" + status + "&" +
                    "u=<?php echo $u;?>&" +
                    "m=<?php echo $m;?>",
                success: function (msg) {
                    if (type == 'follow') {
                        $('#follow_dis').html("<a style='cursor:pointer;float:left;margin-top:10px; width: 245px;text-align: center; padding:0px 7px 0px 7px;line-height:12px;' title='Unfollow' onclick='follow_discussion(\"unfollow\", " + prof_id + "," + user_id + ");' delType = 'event' class='tool follow'><strong>Following</strong></a>");
                    } else {
                        $('#follow_dis').html("<a style='cursor:pointer;float:left;margin-top:10px; width: 245px;text-align: center; padding:0px 7px 0px 7px;line-height:12px;' title='Follow' onclick='follow_discussion(\"follow\", " + prof_id + "," + user_id + ");' delType = 'event' class='tool follow'><strong>Follow</strong></a>");
                    }
                }
            });
        }
    </script>
    <style type="text/css">
        .statuson {
            float: right;
        }

        .privacyDialog {
            width: 920px !important;
        }

        .privacyDialog ul li {
            padding: 0 0 .2em 1.5em;
            list-style: none none;
            font-family: LatoRegular;
            font-size: 14px !important;
            background: none;
        }

        .privacyDialog .ui-dialog-titlebar {
            padding: .4em 1em;
            position: relative;
            background: none;
            border-style: none;
            color: #231F20;
        }

        .privacyDialog .ui-dialog-title {
            float: left;
            margin: .1em 16px .1em 0;
        }

        .privacyDialog .ui-dialog-titlebar-close {
            display: block !important;
            background-image: url('/themes/maennaco/images/close-icon-off.png') !important;
            width: 33px;
            height: 32px;
        }

        .privacyDialog .ui-dialog-titlebar-close span {
            display: block;
            margin: 1px;
        }

        .privacyDialog .ui-dialog-titlebar-close:hover {
            background-image: url('/themes/maennaco/images/close-icon-on.png') !important;
        }

        .privacyDialog .ui-dialog-titlebar-close:focus {
            padding: 0;
        }

        .privacyDialog .ui-dialog-titlebar-close {
            position: absolute;
            top: 23% !important;
            right: -0.2em;
            margin: -10px 0 0 0;
            padding: 1px;
        }

        .privacyDialog .ui-dialog-titlebar-close span {
            background-image: none;
        }

        a.indented {
            color: #222222;
        }

        a.dialog-left-column-title {
            color: #222222;
        }

        #center a:visited {
            color: #222222;
        }

        .ui-dialog {
            padding: 0px;
        }

        .ui-dialog .ui-dialog-content {
            padding: 0px;
        }

        .ui-widget-content {
            border: none;
        }

        .ui-dialog .ui-dialog-buttonpane {
            text-align: center;
        }

        .ui-dialog .ui-dialog-buttonpane .ui-dialog-buttonset {
            float: none;
        }

        .ui-button-text-only .ui-button-text {
            padding: 2px 1em;
        }

        .ui-button-text {
            width: 95px;
        }

        .form-submit2 .ui-button-text {
            height: 30px;
            background-color: #D0D2D2 !important;
            border-radius: 4px;
            -webkit-border-radius: 4px;
            -moz-border-radius: 4px;
            -khtml-border-radius: 4px;
            border: none;
            padding-right: 12px;
            padding-left: 12px;
        }

        .form-submit1 span.ui-button-text {
            padding-right: 12px;
            padding-left: 12px !important;
        }
    </style>
</head>
<body<?php print phptemplate_body_class($left, $right); ?>>
<div id="fb-root"></div>
<script type="text/javascript">
    (function (d, s, id) {
        var js, fjs = d.getElementsByTagName(s)[0];
        if (d.getElementById(id)) {
            return;
        }
        js = d.createElement(s);
        js.id = id;
        js.src = "//connect.facebook.net/en_GB/all.js#xfbml=1&appId=267809316574109";
        fjs.parentNode.insertBefore(js, fjs);
    }(document, 'script', 'facebook-jssdk'));
</script>
<a name="top"></a>


<?php
if ($user->uid) { ?>
<div id="wrapper">
    <div id="container" class="clear-block"
         style="width:1024px">
        <div id="header">
            <div style="width:280px;padding-left:30px; padding-top: 5px; margin-top:12px; ">
                <a href="/"><img src="<?php echo $base_url; ?>/themes/maennaco/images/index_logo.png"
                                 alt="website logo" border=0 height="30px"></a>
            </div>
            <div id="user-status">
            </div>
        </div>
        <?php
        $Maenna_page->account_tabs();
        }
        else {
            echo '<div style="margin-top:-25px;background-color:#ffffff !important;" id="wrapper">
    <div id="container" class="clear-block">';
            require_once('header-regular.php');
        } ?>

        <div class='divider-top'></div>

        <div id="center">
            <div id="squeeze">
                <?php
                $currentPage = $_SERVER["REQUEST_URI"];
                if (substr($currentPage, 1, 5) === 'about') { ?>
                    <div>
                        <div class="dialog-left-column">
                            <a id="the_company"
                               class="dialog-left-column-title <?php if ($currentPage == "/about/the-company") echo 'dialog-selected'; ?>"
                               style="cursor:pointer;"
                               href="/about/the-company">
                                The Company
                            </a>
                            <br>
                            <br>
                            <!--<a id="wsa" class="indented <?php /*if($currentPage == "/about/the-company/why-use-clewed") echo 'dialog-selected';*/ ?>" style="cursor:pointer;" href="/about/the-company/why-use-clewed">&#8226;&nbsp;&nbsp;&nbsp; Why Use Clewed?</a>-->
                            <?php /*<a id="wwb" class="dialog-left-column-title <?php if($currentPage == "/about/what-we-belive") echo 'dialog-selected';?>" style="cursor:pointer;" href="/about/what-we-belive">What We Believe</a><br><br>
							<a id="ost" class="dialog-left-column-title <?php if($currentPage == "/about/our-story") echo 'dialog-selected';?>" style="cursor:pointer;" href="/about/our-story">Our Story</a><br>
							<a id="wsa" class="indented <?php if($currentPage == "/about/our-story/what-sets-us-apart") echo 'dialog-selected';?>" style="cursor:pointer;" href="/about/our-story/what-sets-us-apart">&#8226;&nbsp;&nbsp;&nbsp; What Sets Us Apart</a><br><br>*/ ?>
                            <!--							<a id="companies" class="dialog-left-column-title <?php /*if($currentPage == "/about/our-clients") echo 'dialog-selected';*/ ?>" style="cursor:pointer;" href="/about/our-clients">Our Clients</a><br><br>-->
<!--                            <a id="wwd"-->
<!--                               class="dialog-left-column-title --><?php //if ($currentPage == "/about/our-services") echo 'dialog-selected'; ?><!--"-->
<!--                               style="cursor:pointer;"-->
<!--                               href="/about/our-services">-->
<!--                                Our Products-->
<!--                            </a>-->
<!--                            <br>-->
<!--                            <br>-->
                            <!--<a id="cser" class="indented <?php /*if($currentPage == "/about/our-services/advisory-services") echo 'dialog-selected';*/ ?>" style="cursor:pointer;" href="/about/our-services/advisory-services">&#8226;&nbsp;&nbsp;&nbsp; Advisory Services</a><br>
							<a id="inv" class="indented <?php /*if($currentPage == "/about/our-services/clewed-insights") echo 'dialog-selected';*/ ?>" style="cursor:pointer; " href="/about/our-services/clewed-insights">&#8226;&nbsp;&nbsp;&nbsp; Clewed Insights</a><br><br>-->
							<a id="ost"
							   class="dialog-left-column-title <?php if ($currentPage == "/about/team/our-story") echo 'dialog-selected'; ?>"
							   style="cursor:pointer;"
							   href="/about/team/our-story">
								Our Story
							</a>
							<br>
							<br>
							<a id="pron"
                               class="dialog-left-column-title <?php if ($currentPage == "/about/professional-network") echo 'dialog-selected'; ?>"
                               style="cursor:pointer;"
                               href="/about/professional-network">
                                Our People
                            </a>
                            <br>
                            <br>
                            <a id="wwa"
                               class="dialog-left-column-title <?php if ($currentPage == "/about/team") echo 'dialog-selected'; ?>"
                               style="cursor:pointer;" href="/about/team">
								Our Founders
							</a>
							<br>
							<br>
							<a id=""
							   class="dialog-left-column-title <?php if ($currentPage == "/about/invite-a-friend") echo 'dialog-selected'; ?>"
							   style="cursor:pointer;"
							   href="/about/invite-a-friend">
								Invite a Friend
							</a>
							<br>
							<br>
							<!-- <a id="investors" class="dialog-left-column-title <?php /*if($currentPage == "/about/investors") echo 'dialog-selected';*/ ?>" style="cursor:pointer;" href="/about/investors">Investors</a><br><br>-->
							<a id="contactus"
							   class="dialog-left-column-title <?php if ($currentPage == "/about/contact-us") echo 'dialog-selected'; ?>"
							   style="cursor:pointer;" href="/about/contact-us">
								Contact Us
							</a>
							<br>
						</div>
						<div
								class="verticaldivider" <?php if ($currentPage == "/about/professional-network" || $currentPage == "/about/the-company/why-use-clewed") {
							echo 'style="height:650px;"';
						} elseif ($currentPage == "/about/team") echo 'style="height:1050px;"';
						elseif ($currentPage == "/about/the-company") echo 'style="height:900px;"'; ?>></div>
						<div class="dialog-right-column" style="padding-top: 20px;font-family:'LatoRegular' !important;font-size:16px !important;">
							<?php
							if ($currentPage == "/about/team") { ?>
								<span class="dialog-right-column-title">Our Founders</span><br><br><br>
								We're a passionate team with decades of business experience dedicated to help promising companies achieve results they won't achieve on their own. We work hard to ensure we build the best platform that connects companies with expertise traditionally hard to access.
							<br><br>
								<!--                                 <span class="dialog-right-column-subtitle">Team</span><br><br> -->
								<div class="dialog-box">
									<div class="dialog-box-img-left">
										<img
												src="<?php echo $base_url; ?>/themes/maennaco/images/project/Hiwot_Nega1.jpg"
												alt="HIWOT NEGA - Founder & CEO" width="150px" height="150px"><br>
										<span class="dialog-right-column-subtitle">HIWOT NEGA</span><br>
										Founder & CEO
									</div>
									<div class="dialog-box-txt-right">
										Prior to founding Clewed, Hiwot was a Principal at Solera Capital since 2004,
										where she sourced, evaluated, executed and monitored investments across
										industries for a $250 million middle market growth equity fund. Previously,
										Hiwot spent 8 years in financial management roles in media, telecom and finance
										industries. Most recently, she was with Pearson Inc., USA where she managed
										financial reporting and analysis for Pearson's US entities and RSL
										communications Inc. Hiwot began her career as analyst with AMI Capital, now part
										of Wells Fargo. Hiwot holds an MBA from Columbia Business School and a B.S. from
										SUNY New Paltz, in New York. She holds a CPA.
									</div>
								</div>
							<br><br><!--<br><span class="dialog-right-column-subtitle">Advisors</span><br><br> -->
								<div class="dialog-box">
									<div class="dialog-box-img-left">
										<img
												src="<?php echo $base_url; ?>/themes/maennaco/images/project/Ramon_De_Oliveira.png"
												alt="RAMON DE OLIVEIRA - Clewed Advisor" width="150px" height="150px"><br>
										<span class="dialog-right-column-subtitle">RAMON DE OLIVEIRA</span><br>
										Advisor
									</div>
									<div class="dialog-box-txt-right">
										Ramon de Oliveira is the Managing Partner of the consulting firm ROC Partners,
										based in New York city. Between 2002 and 2006, Mr. de Oliveira was an adjunct
										professor of Finance at Columbia University. Starting in 1977, Mr. de Oliveira
										spent 24 years at JP Morgan & Co. From 1996 to 2001, Mr. de Oliveira was
										Chairman and CEO of JP Morgan Investment Management. Mr. de Oliveira was also a
										member of the firm's Management Committee since its inception in 1995. Upon the
										merger with Chase Manhattan Bank in 2001, Mr. de Oliveira was the only executive
										from JP Morgan & Co. asked to join the Executive Committee of the new firm with
										operating responsibilities. He is a graduate of the University of Paris and of
										the Institut d'Etudes Politiques (Paris).
										<br><br>Directorships currently held: ROC Partners, Managing Director. AXA -
										Director and Chairman of the Finance Committee. Fonds de Dotation du Louvre-
										Chairman of the Investment Committee. Taittinger-Kobrand USA (United States).
										The Red Cross, Member of the Investment Committee.
										<br><br>Previous directorships: Friends of Education (a New York-based
										not-for-profit organization), Chairman of the Board of Directors; JP Morgan
										Suisse (Switzerland); American Century Company, Inc (United States), Sungard
										Data Systems (SDS) (United States). The Hartford Insurance Company (end of
										mandate April 29, 2009)
									</div>
								</div>
							<?php
							} elseif ($currentPage == "/about/contact-us") { ?>
								<span class="dialog-right-column-title">Contact Us</span><br><br>
								<div style="font-size:16px !important;"><!--Clewed.<br>
                                                                        42 West 24th Street<br>
                                                                        New York, NY, 10010<br><br>
                                                                        <!--T: 212-731-9563<br><br>-->
                                                                        If you have any question or feedback, please
                                                                        fill the form below or just send an email to
                                    <a href="mailto:info@clewed.com">info@clewed.com</a></div> <br><br>
                                <div class="contact-wrapper">
                                    <div id="contact-us">
                                        <div style="width:287px;margin-left:17px;margin-bottom: 4px;display:block;"
                                             class="partner-header">Contact Us
                                        </div>
                                        <div id="contact-form" style="margin-left:10px; color:#00a1be;">
                                            <input type="text" id="contact-name">
                                            <input type="text" id="contact-email" class="required">
                                            <input type="text" id="contact-subject">
                                            <textarea style="border:none !important;width:278px !important;"
                                                      id="contact-message" rows="5"></textarea>
                                        </div>
                                    </div>
                                    <div style="padding-left:10px;margin-top:4px;">
                                        <div
                                            style="float:left;font-family: 'Lato Light';font-size:12px;margin-left:10px;">
                                            *All fields required
                                        </div>
                                        <button
                                            style="float:left;font-family:'Lato Bold'!important; margin-left:100px;font-size: 17px;padding:2px 1em"
                                            name="op" id="contact-submit" class="form-submit contactus ">Submit
                                        </button>
                                        <div style='clear:both;'></div>
                                    </div>
                                </div>
                                <script type="text/javascript">
                                    $(document).ready(function () {
                                        window.utype = 'company';
                                        var body = $('body');
                                        var emailRegex = new RegExp(/^([\w\.\-]+)@([\w\-]+)((\.[\w\-]+)+)$/i);

                                        function checkString(obj, watermark) {
                                            if (obj.val() == '' || obj.val() == watermark) {
                                                obj.css('border', '1px solid #C52020');
                                                obj.focus();
                                                return false;
                                            } else {
                                                obj.css('border', 'none');
                                                return true;
                                            }
                                        }

                                        body.delegate("button.contactus", "click", function () {
                                            var fValid = true;
                                            fValid = fValid && checkString($("#contact-name"), 'Name');
                                            fValid = fValid && checkString($("#contact-email"), 'Email');
                                            fValid = fValid && checkString($("#contact-subject"), 'Subject');
                                            fValid = fValid && checkString($("#contact-message"), 'Message');
                                            if (fValid) {
                                                $.post("/themes/maennaco/includes/homepage_posts.php?type=feedback",
                                                    {
                                                        name: $("#contact-name").val(),
                                                        email: $("#contact-email").val(),
                                                        subject: $("#contact-subject").val(),
                                                        message: $("#contact-message").val()
                                                    },
                                                    function (response) {
                                                        if (response == 0) {
                                                            $("#contact-name").val('');
                                                            $("#contact-email").val('');
                                                            $("#contact-subject").val('');
                                                            $("#contact-message").val('');
                                                            alert("Your information was successfully sent");
                                                        } else {
                                                            $("#contact-name").val('');
                                                            $("#contact-email").val('');
                                                            $("#contact-subject").val('');
                                                            $("#contact-message").val('');
                                                            alert("Message couldn't be send. Please try again!");
                                                        }
                                                        $("#learn-more-dialog").dialog("open");
                                                    });
                                            }
                                        });
                                    });
                                    $('#contact-name').Watermark('Name');
                                    $('#contact-email').Watermark('Email');
                                    $('#contact-subject').Watermark('Subject');
                                    $('#contact-message').Watermark('Message');
                                </script>
                            <?php } elseif ($currentPage == "/about/invite-a-friend") { ?>
                                <style type="text/css">
                                    #invite-a-friend ol li {
                                        list-style: decimal;
                                        padding: 0;
                                        margin-left: 300px;
                                    }
                                    #invite-a-friend, #invite-a-friend p {
                                        font-size: 16px !important;
                                    }
                                    #invite-a-friend .dialog-right-column-subtitle {
                                        font-size: 16px;
                                    }
                                </style>
                                <div id="invite-a-friend">
                                    <span class="dialog-right-column-title">Invite a Friend</span><br><br>
                                    <p>
                                        Invite your friends and business network to try Clewed. When they sign up using your unique invitation code, they can join their first Insight free and get 10% off on their first service. After, their first Insight, you will receive 10% on any service purchases they make with Clewed in their first six month.
                                    </p>
                                    <br>
                                    <span class="dialog-right-column-subtitle">How it Works</span>
                                    <ol>
                                        <li>Login to your account and navigate to My Account > Edit your profile summary to find your unique Invitation Code.
                                        </li>
                                        <li>Tell your business network to use this code as their “Invitation” code when they register on Clewed.
                                        </li>
                                        <li>View the “Invitations” section of your profile page to see those who have registered using your Invitation code in the last 12 months.
                                        </li>
                                        <li>4.	Each time an “Invitation” makes a purchase in their first six month of Clewed, you will earn 10% of their payment.
                                        </li>
                                        <li>Twice a month, Clewed will pay you the amount you earned and received by Clewed from your Invitation, to the extent your credit exceeds $25.
                                        </li>
                                    </ol>
                                    <p>
                                        Give your network a free Insight and discount on services! It’s a great gift they’d like and a great way for you to unlock perks and engage with your network.
                                    </p>
                                </div>
                            <?php } elseif ($currentPage == "/about/the-company") { ?>
                                <script type="text/javascript" src="/js/how-it-works.js"></script>
                                <style type="text/css">
                                    div.imageBlock h2 {
                                        padding: 35px 0px;
                                    }

                                    div.imageBlock p {
                                        font-size: 16px !important;
                                        font-family: 'LatoRegular' !important;
                                        line-height: 20px;
                                        color: #898B8E;
                                    }

                                    h1 {
                                        padding: 22px 0px;
                                        font-family: "LatoRegular", sans-serif;
                                        font-size: 32px;
                                        color: #547381;
                                        display: block;
                                        text-align: center;
                                    }

                                    .clear-block {
                                        min-height: 60px;
                                    }

                                    div.links {
                                        width: 100%;
                                        text-align: center;
                                    }

                                    div.links a {
                                        display: inline-block;
                                        width: 222px;
                                        height: 50px;
                                        background-color: #D0D2D2;
                                        color: #1f3947;
                                        font-size: 21px;
                                        text-transform: uppercase;
                                        border: 2px solid #1f3a48;
                                        font-family: "Lato Black", sans-serif;
                                        position: relative;
                                        line-height: 54px;
                                        text-decoration: none;
                                        cursor: pointer;
                                    }

                                    div.links a div.line {
                                        display: none;
                                        position: absolute;
                                        top: 8px;
                                        left: 0;
                                    }

                                    div.links a.active, div.links a:hover {
                                        color: #fbfbfb;
                                        background: #1f3a48 url(/themes/maennaco/images/topline.png) no-repeat center 4px;
                                    }

                                    #companiesblock .column {
                                        height: 365px;
                                    }

                                    #problock .column {
                                        height: 450px;
                                    }

                                </style>
                                <span class="dialog-right-column-title">The Company</span><br><br>
                                <div class="imageBlock" style="margin-bottom:50px;">
                                        <p>Clewed is the smartest way for companies to get more done. By seamlessly enabling skilled professionals across fields to work in their area of expertise on opportunities we manage through our platform, we remove the friction for companies to connect with the right resources and partners – opening up more possibilities for businesses to accelerate performance and for professionals to monetize their expertise.</p>
                                </div>
                                <div class="clear-block1" style="margin-bottom:50px;">
                                    <div class="links">
                                        <a id="company"<?php if (empty($_REQUEST['pro'])) echo ' class="active"' ?>>
                                            For Companies</a>
                                        <a id="pro"<?php if (!empty($_REQUEST['pro'])) echo ' class="active"' ?>>
                                            For Professionals</a>
                                    </div>
                                </div>
                                <div id="companiesblock"
                                     class="imageBlock jsblock"<?php if (!empty($_REQUEST['pro'])) echo ' style="display: none;"' ?>>
                                    <p>
                                        In today’s fast-paced digital world, the most important resource is no longer what you own, but what you can access. Our platform makes it easy for you to complete more project and seize more opportunities faster and easier by sharing the skills and network of the world’s smart people.
                                    </p>
                                    <!--?
                                    /*<div class="column">
                                        <span style="margin-bottom:30px;"
                                              class="column_title">Post <br> Projects</span><br><br>
                                                <span class="column_content">
                                                    Post any size project and get help from one or more experts quickly. We help you clarify deliverables, create a project team and control quality while you track progress and provide feedback.
                                                </span>
                                    </div>

                                    <div class="column">
                                        <span style="margin-bottom:30px;"
                                              class="column_title">Access Opportunities</span><br><br>
                                                <span class="column_content">
                                                    Our technology simplifies the diligence of your business while helping us to organize your data and analysis in one place. We leverage our technology, data and network to present your company to the right people so you attract owner-friendly strategic or M&A partners.
                                                </span>
                                    </div>

                                    <div style="margin-bottom:35px;margin-right:0px !important;" class="column">
                                        <span style="margin-bottom:30px;"
                                              class="column_title">High <br>Quality</span><br><br>
                                        <span class="column_content">
                                            Our software and approach is designed to enable you to attract more smart people and partners to continuously help you innovate, problem-solve and find great opportunities – so you build a great business and scale beyond old limits.
                                        </span>
                                    </div>*/
                                    ?-->

                                    <div class="index-text-a-blue" style="margin-top:22px">
                                        <a style=""
                                           data-type="company"
                                           class="request_demo"
                                           rel="request">» Request Demo</a>&nbsp;
                                    </div>
                                </div>
                                <div id="problock"
                                     class="imageBlock jsblock"<?php if (empty($_REQUEST['pro'])) echo ' style="display: none;"' ?>>
                                    <p>
                                        No matter what company you work for, your knowledge and network can help many more companies and make a much bigger impact on our world. Clewed helps talented researchers, analysts, operators and subject-matter experts across fields connect with opportunities in their field for a simple and clear transaction from anywhere, anytime.
                                    </p>
                                    <!--?
                                    /*<div class="column">
                                        <span style="margin-bottom:30px;"
                                              class="column_title">Easy to <br> Start</span><br><br>
                                        <span class="column_content">
                                            It’s never been easier to promote and sell your services online! Create your profile, get approved and start immediately! You can simply package, price and list high value insights and services as a product and share your unique link with your target audience and engage with companies.
                                        </span>
                                    </div>

                                    <div class="column">
                                        <span style="margin-bottom:30px;"
                                              class="column_title">Access<br> Opportunities</span><br><br>
                                        <span class="column_content">
                                            You will receive invitations to participate in available projects and opportunities in your area of expertise or can bring opportunities that fit our criteria to present to our network. You connect to help each other, collaborate with like-minded people and earn extra income sharing your skills or bringing resources to bear for our or your clients.
                                        </span>
                                    </div>

                                    <div style="margin-bottom:35px;margin-right:0px !important;" class="column">
                                        <span style="margin-bottom:30px;"
                                              class="column_title">Grow<br> Faster</span><br><br>
                                        <span class="column_content">
                                           Our platform breaks the limitation for you to sell unlimited services and build relationships with potential clients. You can bring companies to co-sell bigger services, complement your skills or help clients find partners. You can refer clients for services outside your expertise and earn income while benefiting from the network effects of a growing community.
                                        </span>
                                    </div>*/
                                    ?-->
                                    <div class="index-text-a-blue" style="margin-top:22px">
                                        <a data-type="professional" class="request_demo" rel="request">» Request Demo</a>&nbsp;
                                    </div>
                                </div>
                            <?php } else {
                                $title_color = '';
                                $tab_box = false;
                                if (isset($node)) {
                                    if (in_array($node->nid, array(44, 45))) {
                                        get_people_menu(1);
                                        $title_color = '';
                                        $tab_box = 1;
                                    } elseif (in_array($node->nid, array(41, 42, 43, 49, 64))) {
                                        get_people_menu(2);
                                        $title_color = '';
                                        $tab_box = 1;
                                    } elseif (in_array($node->nid, array(37, 38, 39, 40, 50))) {
                                        get_people_menu(3);
                                        $title_color = '';
                                        $tab_box = 1;
                                    } elseif (in_array($node->nid, array(46, 47))) {
                                        get_people_menu(4);
                                        $title_color = '';
                                        $tab_box = 1;
                                    } elseif ($node->nid == 65) {
                                        $n = node_load(54);
                                        echo $n->body . "<br><br>";
                                    }
                                }
                                if ($tab_box) echo "<div class='tab-box'>";
                                if ($title && !in_array($node->nid, array(41, 45, 47))) {
                                    print '<h2>' . $title . '</h2>';
                                }
                                if ($tab_box) echo "</div>";
                                print $content;
                            } ?>
                        </div>
                    </div>
                <?php } else { ?>
                    <?php
                    echo "<div class='page-content' >";
                    //$title_color = ' nav-title';
                    $title_color = '';
                    $tab_box = false;
                    if (isset($node)) {
                        if (in_array($node->nid, array(44, 45))) {
                            get_people_menu(1);
                            $title_color = '';
                            $tab_box = 1;
                        } elseif (in_array($node->nid, array(41, 42, 43, 49, 64))) {
                            get_people_menu(2);
                            $title_color = '';
                            $tab_box = 1;
                        } elseif (in_array($node->nid, array(37, 38, 39, 40, 50))) {
                            get_people_menu(3);
                            $title_color = '';
                            $tab_box = 1;
                        } elseif (in_array($node->nid, array(46, 47))) {
                            get_people_menu(4);
                            $title_color = '';
                            $tab_box = 1;
                        } elseif ($node->nid == 65) {
                            $n = node_load(54);
                            echo $n->body . "<br><br>";
                        }
                    }
                    if ($tab_box) echo "<div class='tab-box'>";
                    if ($title && !in_array($node->nid, array(41, 45, 47))) {
                        print '<h2' . /*($tabs ? ' class="with-tabs ' . $title_color. '" ' : '') . */
                            '  >' . $title . '</h2>';
                    }
                    print $content;
                    if ($tab_box) echo "</div>";
                    echo "</div>";
                } ?>

            </div>
            <!-- visitor-content-box -->

            <div style="clear:both"></div>
            <!-- squeeze -->
        </div>
        <!-- center -->

    </div>
    <!-- container -->

</div>
<!-- /header -->
<div id="footer-new" style="background:none !important;height: 40px; text-align: center;">

</div>

<?php require_once("dialogs/footer-login.php");?>

<div id="footer" style="padding-top:20px;clear:both;">
    <div class="divider">
        <?php require_once("footer-maennaco.php"); ?>
    </div>
    <!-- footer -->
    <div id="policy" style="font-family: Helvetica, arial; font-size:12px; display:none;"></div>
</div>
<!-- wrapper -->
<script type="application/javascript">
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
if (isset($AccessObj->loadGoogleChart)) {
    $str_chart_data = $AccessObj->str_chart_data;
    $h = <<< END
            <script type="text/javascript" src="https://www.google.com/jsapi"></script>
<script type="text/javascript">
              // Load the Visualization API and the piechart package.
              google.load('visualization', '1.0', {'packages':['corechart']});
              // Set a callback to run when the Google Visualization API is loaded.
              google.setOnLoadCallback(drawChart);
              function drawChart() {
                var data = new google.visualization.DataTable();
                data.addColumn('string', 'Year');
                data.addColumn('number', 'Rev');
                data.addColumn('number', 'Earnings');

                data.addRows([
                  $str_chart_data
                ]);
                var formatter = new google.visualization.NumberFormat({fractionDigits: 0});
                formatter.format(data, 1); // Apply formatter to second column
                formatter.format(data, 2); // Apply formatter to third column
                var options = {
                  width: 570, height: 240,
                  colors:['#527382','#00A2BF'],
                  title: '',
                };
                var chart = new google.visualization.ColumnChart(document.getElementById('chart'));
                chart.draw(data, options);
                }
</script>
END;
    echo $h;
}

?>

<div id='popup_layer' class='hide'>
    <div id="dialog" style="padding:20px;">
    </div>
</div>

<script type="text/javascript">

    $(document).ready(function () {
        setTimeout(function () {
            $("#messages").fadeOut("slow");
        }, 10000);
        $("#policy").dialog({
            modal: true,
            autoOpen: false,
            width: "920",
            height: "620",
            dialogClass: "privacyDialog",
            draggable: false
        });
        $("#learn-more-dialog").dialog({
            modal: true,
            autoOpen: false,
            width: "920",
            height: "620",
            dialogClass: "privacyDialog"
        });
        $("#learn-more").click(function () {
            $("#learn-more-dialog").dialog("open");
            return false;
        });
    });
    $(document).tooltip({
        items: "a[data-tooltip], span[data-tooltip], div[data-tooltip],li[data-tooltip],td[data-tooltip]",
        content: function () {
            return $(this).attr('data-tooltip')
        }
    });
    $(".card-tool").tooltip({
        items: "[card-tooltip]", tooltipClass: "card_tip", content: function () {
            var element = $(this);
            return $("div[uid='" + $(this).attr("card-tooltip") + "']").html();
        }
    });
</script>
</body>
</html>
