<?php
global $base_url;
// $Id: page.tpl.php,v 1.18.2.1 2009/04/30 00:13:31 goba Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>"
      lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
<head>
    <?php //print $head ?>
    <title><?php print $head_title ?></title>
    <?php print $styles ?>
    <?php print $scripts ?>
    <!--[if lt IE 7]>
    <?php print phptemplate_get_ie_styles(); ?>
    <![endif]-->

    <style>
        body {
            margin: 0px;
            padding: 0px;
            background: #fcf7f7 !important;
        }

        html, body, #container {
            height: 100%;
            background: none !important;
        }

        .ui-dialog .ui-dialog-titlebar {
            padding: .4em 1em;
            position: relative;
            background: none;
            border-top: none;
            border-left: none;
            border-bottom: none;
            border-right: none;
            color: #231F20;
        }

        .ui-dialog .ui-dialog-title {
            float: left;
            margin: .1em 16px .1em 0;
        }

        .ui-dialog .ui-dialog-titlebar-close {
            display: block !important;
            background-image: url('<?php echo $base_url; ?>/themes/maennaco/images/close-icon-off.png') !important;
            width: 33px;
            height: 32px;
        }

        .ui-dialog .ui-dialog-titlebar-close span {
            display: block;
            margin: 1px;
        }

        .ui-dialog .ui-dialog-titlebar-close:hover {
            background-image: url('<?php echo $base_url; ?>/themes/maennaco/images/close-icon-on.png') !important;
        }

        .ui-dialog .ui-dialog-titlebar-close:focus {
            padding: 0;
        }

        .ui-dialog .ui-dialog-titlebar-close {
            position: absolute;
            top: 23% !important;
            right: -0.2em;
            margin: -10px 0 0 0;
            padding: 1px;
        }

        #wrapper {
            /*background: url("
        <?php echo $base_url; ?> themes/maennaco/images/homepage-bg.jpg") repeat-y;*/
            margin-top: -25px;
            background-color: #ffffff;
        }

        #container {
            margin-top: 15px !important;
        }

        p {
            font-family: 'LatoRegular';
            font-size: 12px;
        }

        li {
            font-family: 'LatoRegular';
            font-size: 14px;
        }

        ul li, ul.menu li, .item-list ul li, li.leaf {
            height: 30px;
            background: #E8E8E9 !important;
        }

        ul li, ul.menu li, .item-list ul li, li.list-a {
            background: none !important;
            height: auto;
        }

        .ui-dialog .ui-dialog-content {
            padding: 0 !important;
        }

        .ui-dialog {
            padding: 0 !important;
        }

        .ui-widget-overlay {
            opacity: 0.8 !important;
        }

    </style>
    <?php if ($user->uid > 1): ?>
      <script type="text/javascript">
        window.location = "<?=$base_url?>/account";
      </script>
    <?php endif; ?>
</head>
<body<?php print phptemplate_body_class($left, $right); ?>>
<link type="text/css"
      href="<?php echo $base_url; ?>/themes/maennaco/jui/css/redmond/homepagecss/jquery-ui-1.8.15.custom.css"
      rel="stylesheet"/>
<script type="text/javascript" src="<?php echo $base_url; ?>/themes/maennaco/jui/js/jquery-1.6.2.min.js"></script>
<script type="text/javascript"
        src="<?php echo $base_url; ?>/themes/maennaco/jui/js/jquery-ui-1.8.15.custom.min.js"></script>
<script type="text/javascript"
        src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
<script src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.watermarkinput.js"
        type="text/javascript"></script>

<div id="fb-root"></div>
<script>(function (d, s, id) {
        var js, fjs = d.getElementsByTagName(s)[0];
        if (d.getElementById(id)) return;
        js = d.createElement(s);
        js.id = id;
        js.src = "//connect.facebook.net/en_GB/all.js#xfbml=1&appId=267809316574109";
        fjs.parentNode.insertBefore(js, fjs);
    }(document, 'script', 'facebook-jssdk'));


    function show_hide(id_show, id_hide, bidblue, bidblack) {

        if (id_show == 'tabs-1') window.utype = 'company';
        else if (id_show == 'tabs-2') window.utype = 'professional';
        document.getElementById(id_show).style.display = 'block';
        document.getElementById(id_hide).style.display = 'none';
        document.getElementById(bidblue).style.backgroundColor = '#00a2bf';
        document.getElementById(bidblack).style.backgroundColor = '#D0D2D2';
        document.getElementById('form-submit1').style.display = 'block';
        document.getElementById('form-submit2').style.display = 'block';
    }
//    $('document').ready(function(){
//      $('#watch-video-link').click();
//    });
</script>

<!-- Layout -->
<!--div id="header-region" class="clear-block"><?php print $header; ?></div-->

<div id="wrapper">
<div id="container" class="clear-block" style="margin-top:0px !important;">

    <?php require_once("header-regular.php");?>
    <!-- /header -->

    <div style="clear:both;"></div>

    <div id="center" style='margin-top:400px;'>
        <div id="squeeze" class="professionals-page">
            <div class="right-corner">
                <div class="left-corner" style='padding:0;margin:0'>
                    <div class="clear-block">

                        <div id="node-75" class="node">

                            <div class="content clear-block">
                                <div class="index-text-a"><span class="index-text-a-bold">clewed.</span> It's the best way to engage with other business people.</div>
                                <div class="index-text-a-blue">The first platform for industry analysts, business operators and subject-matter experts to share knowledge and resources and work together on opportunities.<br>
                                    <div style="margin-top:12px">
                                        <!--<a href="/insights" style="cursor:pointer;">» Browse Insights</a>--> <a style=" :hover {text-decoration:none;}" data-type="professional" class="request_demo" rel="request">» Request Demo</a>
                                    </div>
                                </div>
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

<div id="slidebg" class="professional">
    <div class="slwrap">
        <div class="slquote">
            Share knowledge<br /> and opportunities<br />in your business field.<br />
            <a style="margin-top:15px;" href="/how-it-works.php?pro=1">&raquo; How it works</a>
        </div>
        <a style="margin-top: 20px;cursor: pointer;text-decoration: none;" id="home-register-new" class="home-register" data-type="professional">GET STARTED<br><span>» Register for FREE</span></a>
    </div>
    <div id="home-video" style="display:none;">

    </div>
</div>
<div id="footer-new">
    <div id="footer-wrapper">

        <div id="footer-right-column">
            <div id="rc-icon"><img src="/themes/maennaco/images/hiw/p3.png"></div>
            <br>
            <span class="title research">SHARE EXPERTISE</span>

            <div id="rc-content">
                Package, price and market your insight broadly. Engage with
                your audience, add value and earn new revenue.

            </div>
        </div>
        <div class="headerdivider"></div>
        <div id="footer-right-column">
            <div id="rc-icon"><img src="/themes/maennaco/images/hiw/p2.png"></div>
            <br>
            <span class="title">START EASY </span>

            <div id="rc-content">
                Join free & start immediately. No commitment. No hidden fees. Keep
                80% to 90% of the profits from audience you bring.

            </div>

        </div>
        <div class="headerdivider"></div>
        <div id="footer-right-column">
            <div id="rc-icon"><img src="/themes/maennaco/images/hiw/p4.png"></div>
            <br>
            <span class="title">GROW FASTER</span>

            <div id="rc-content">
                Leverage the power of technology to unleash new opportunities and transform the way you work.
            </div>
        </div>
        <br style="clear:both;">

<!--        --><?php //require_once("footer-regular.php");?>
    </div>
</div>
            <?php require_once("footer-regular.php");?>


<script type="text/javascript">
$(document).ready(function () {

//    window.utype = 'company';
    window.utype = 'professional';
    var emailRegex = new RegExp(/^([\w\.\-]+)@([\w\-]+)((\.[\w\-]+)+)$/i);

    function password_type(value){

        if (value.val().length < 8)
        {
            alert('Your password needs to contain 8 characters minimum!');
            value.focus();
            return false;
        }

        re = /^[A-Za-z]+$/;
        if(re.test(value.val()))
        {
            alert('Your password needs to contain at least one number!');
            value.focus();
            return false;
        }

        return true;

    }

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

    $('#company-firstname').Watermark('First Name');
    $('#company-lastname').Watermark('Last Name');
    $('#company-name').Watermark('Company Name');
    $('#company-email').Watermark('Email');
    $('#company-password').Watermark('Password');

    $('#pro-firstname').Watermark('First Name');
    $('#pro-lastname').Watermark('Last Name');
    $('#pro-email').Watermark('Email');
    $('#pro-password').Watermark('Password');

    $('#edit-email').Watermark('Email');
    //$('#edit-password').Watermark('Password');

    $("#feedback").click(function (event) {

        event.preventDefault();
        $.post("/themes/maennaco/includes/homepage_posts.php?type=contactus", {

        }, function (response) {

            $(".dialog-left-column a").removeClass('dialog-selected');
            $('#contactus').addClass('dialog-selected');
            $(".dialog-right-column").html(response).fadeIn("slow");

            $('#contact-name').Watermark('Name');
            $('#contact-email').Watermark('Email');
            $('#contact-subject').Watermark('Subject');
            $('#contact-message').Watermark('Message');
            $("#learn-more-dialog").dialog("open");


        });
    });


    $("body").delegate("button.contactus", "click", function () {

        fValid = true;

        fValid = fValid && checkString($("#contact-name"), 'Name');
        fValid = fValid && checkString($("#contact-email"), 'Email');
        fValid = fValid && checkString($("#contact-subject"), 'Subject');
        fValid = fValid && checkString($("#contact-message"), 'Message');

        if (fValid) {

            $.post("/themes/maennaco/includes/homepage_posts.php?type=feedback",
                {name: $("#contact-name").val(), email: $("#contact-email").val(), subject: $("#contact-subject").val(), message: $("#contact-message").val() },
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

    $("body").delegate("a.pagree", "click", function () {

        $.post("/themes/maennaco/includes/homepage_posts.php?type=terms", {

        }, function (response) {

            $("#right-column-left").html(response).fadeIn("slow");

        });


    });


    $(".show_terms").click(function () {

        $.post("/themes/maennaco/includes/homepage_posts.php?type=policy&req_type=" + $(this).attr('type'), {

        }, function (response) {

            $("#policy").html(response);


        });
        $("#policy").dialog("open");
        return false;
    });


    $("#tabs").tabs();

    $("#company-email").bind('focusout', function () {

        var com = $("#company-email").val();
        if (com != '') {
            $.post("/check_email_exists.php",
                {email: $("#company-email").val()},
                function (response) {

                    if (response == 'false') {

                        alert("User with this email already exists. Please use another one.");
                        $("#company-email").val('').focus();


                    }


                });

            var valid = emailRegex.test($("#company-email").val());
            if (!valid) {
                alert("Format of the e-mail address is not correct");
                $("#company-email").val('').focus();
            }
        }
        else
            return false;

    });

    $("#pro-email").bind('focusout', function () {

        var pro = $("#pro-email").val();
        if (pro != '') {
            $.post("/check_email_exists.php",
                {email: $("#pro-email").val()},
                function (response) {

                    if (response == 'false') {

                        alert("User with this email already exists. Please use another one.");
                        $("#pro-email").val('').focus();


                    }


                });
            var valid = emailRegex.test($("#pro-email").val());
            if (!valid) {
                alert("Format of the e-mail address is not correct");
                $("#pro-email").val('').focus();
            }

        }
        else
            return false;

    });


    $("#learn-more-dialog").dialog({
        modal: true,
        autoOpen: false,
        width: "920",
        height: "620",
        draggable: false
    });

    $("#policy").dialog({
        modal: true,
        autoOpen: false,
        width: "920",
        height: "620",
        draggable: false
    });

    $("#register-dialog").dialog({
        modal: true,
        autoOpen: false,
        draggable: true,
        resizable: false,
        width: "350",
        buttons: [
            {
                text: "Register",
                click: function (e) {
                    var clicked = $(".form-submit1");//$(e.target);

                    fValid = true;

                    selTab = window.utype;



                    if (selTab == 'company') {


                        fValid = checkString($("#company-firstname"), 'First Name') && fValid;
                        fValid = checkString($("#company-lastname"), 'Last Name') && fValid;
                        fValid = checkString($("#company-name"), 'Company Name') && fValid;
                        fValid = checkString($("#company-email"), 'Email') && fValid;
                        fValid = checkString($("#company-password"), 'Password') && fValid;
                        fValid = checkString($("#company-industry"), 'industry') && fValid;
                        fValid = checkString($("#company-revenue"), 'revenue') && fValid;
                        if (!fValid) {
                            alert('Please fill in the required fields.');
                        }
                        if (fValid && !$('#cmp-agree').is(':checked')) {
                            fValid = false;
                            alert('You have to read and agree with terms and privacy policy!');
                        }




                        if (fValid) {

                            clicked.attr('disabled', true);


                            $.post("register_new_user.php?type=company",
                                {firstname: $("#company-firstname").val(), lastname: $("#company-lastname").val(), name: $("#company-name").val(), email: $("#company-email").val(), password: $("#company-password").val(), industry: $("#company-industry").val(), revenue: $("#company-revenue").val() },
                                function (response) {
                                    alert(response);
                                    if (response != 'Security code is not correct!')
                                        location.reload();

                                });
                        }

                    }
                    else {
                        fValid = checkString($("#pro-type"), 'iam') && fValid;
                        fValid = checkString($("#pro-firstname"), 'First Name') && fValid;
                        fValid = checkString($("#pro-lastname"), 'Last Name') && fValid;
                        fValid = checkString($("#pro-email"), 'Email') && fValid;
                        fValid = checkString($("#pro-password"), 'Password') && fValid;
                        fValid = password_type($('#pro-password')) && fValid;
                        fValid = checkString($("#pro-experties"), 'experties') && fValid;
                        if (!fValid) {
                            alert('Please fill in the required fields.');
                        }
                        if (fValid && !$('#pro-agree').is(':checked')) {
                            fValid = false;
                            alert('You have to read and agree with terms and privacy policy!');
                        }


                        if (fValid) {

                            clicked.attr('disabled', true);

                            if (clicked.attr('rel') == 'request') {

                                ajax_path = "register_new_user.php?type=proffesional&request=true";

                            }

                            else ajax_path = "register_new_user.php?type=proffesional";
                            $.post(ajax_path,
                                {firstname: $("#pro-firstname").val(), lastname: $("#pro-lastname").val(), experties: $("#pro-experties").val(), email: $("#pro-email").val(), password: $("#pro-password").val(), pro_type: $("#pro-type").val() },
                                function (response) {
                                    alert(response);
                                    if (response != 'Security code is not correct!')
                                        location.reload();

                                });
                        }


                    }


                },
                "class": "form-submit1"
            },
            {
                text: "Cancel",
                click: function () {
                    $(this).dialog("close");
                },
                "class": "form-submit2"
            }
        ]        });

    $("#learn-more").click(function () {
        $("#learn-more-dialog").dialog("open");
        return false;
    });

    $(".request_demo").click(function () {
            $("#register-dialog #tabs-1").hide();
            $("#register-dialog").parent().children(".ui-dialog-titlebar").remove();
            $(".reg-choose").hide();
            $("#register-dialog").children(".reg-top").html("Schedule a Demo");
            $(".form-submit1").children(".ui-button-text").html("Request");
            $(".form-submit1").attr('rel','request');
            $(".agree_terms").hide();
            $("#register-dialog").dialog("open");
        return false;
    });

    $("#home-register").click(function () {

        $("#register-dialog").children(".reg-top").html("JOIN CLEWED, IT'S FREE TO START");
        $(".form-submit1").children(".ui-button-text").html("Register");
        $(".form-submit1").attr('rel','');
      if($(this).data('type') == 'professional') {
        $("#register-dialog #tabs-1").hide();
        $("#register-dialog").parent().children(".ui-dialog-titlebar").remove();
        $(".reg-choose").hide();
        $("#register-dialog").dialog("open");
      } else {
        $("#register-dialog").dialog("open");
        $("#register-dialog").parent().children(".ui-dialog-titlebar").remove();
        $("#register-dialog #tabs-1").hide();
        $("#register-dialog #tabs-2").hide();
        $(".ui-dialog-buttonset").hide();
        $('.reg-but1').css('background-color','#D0D2D2');
        $('.reg-but2').css('background-color','#D0D2D2');
      }
      return false;
    });

    $('.reg-but1').click(function(){
      $(".ui-dialog-buttonset").show();
    });

    $('.reg-but2').click(function(){
      $(".ui-dialog-buttonset").show();
    });

/*
    $("input:text").focus(function () {
        $(this).val('');
    });
*/

    var page_height = $(window).height();

    var container_height = $('#container').css('height');

    var diff = parseInt(page_height) - 480;
    diff = parseInt(diff / 2);
    if (diff) $('#container').css('margin-top', diff);
    var a = 0;
    if (diff) $('#container').css('margin-top', diff);
    $('#show_overlay').click(function () {
        $('#olay').fadeIn("fast");
    });
    $('#olay').bind("mouseenter",function () {
        a = 1;
    }).bind("mouseleave", function () {
            a = 0;
            $(this).fadeOut();
        });
    $('#closeolay').click(function () {
        $('#olay').fadeOut();
    })
})
</script>

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
        <?php
        /*
            $array = array(
                array('src' => 'companies.png', 'href' =>'/company-apply-form', 'alt'=>'companies'),
                array('src' => 'professionals.png', 'href' =>'/people-apply-form', 'alt'=>'professionals'),
            );
            button_links($array);
        */
        ?>
    </div>
</div>
<!--<script type="text/javascript">

//    var _gaq = _gaq || [];
//    _gaq.push(['_setAccount', 'UA-23393148-1']);
//    _gaq.push(['_trackPageview']);
//
//    (function () {
//        var ga = document.createElement('script');
//        ga.type = 'text/javascript';
//        ga.async = true;
//        ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
//        var s = document.getElementsByTagName('script')[0];
//        s.parentNode.insertBefore(ga, s);
//    })();

</script>-->

<script>
  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
  })(window,document,'script','//www.google-analytics.com/analytics.js','ga');

  ga('create', 'UA-43665911-1', 'clewed.com');
  ga('send', 'pageview');

</script>

<div id="policy" style="font-family: 'Lato Regular'; font-size:12px; display:none;"></div>


</body>
</html>