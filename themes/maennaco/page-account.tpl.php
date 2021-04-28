<?php
    header("Cache-Control: no-cache, no-store, must-revalidate"); // HTTP 1.1.
    header("Pragma: no-cache"); // HTTP 1.0.
    header("Expires: 0"); // Proxies.

    error_reporting(E_ALL);
    ini_set('display_errors', 1);

    global $base_url;
    if ($user == null) global $user;

    define("__ACCOUNT__", 1);
    
    $isAboutTab = isset($_GET['tab']) && $_GET['tab'] == 'companies' && isset($_GET['page']) && $_GET['page'] == 'company_detail' && ((isset($_GET['mtab']) && $_GET['mtab'] == 'about') || empty($_GET['mtab']));
    $isEditContentTab = (isset($_GET['tab']) && $_GET['tab'] == 'companies') && (isset($_GET['page']) && $_GET['page'] == 'company_detail') && (isset($_GET['mtab']) && $_GET['mtab'] == 'about') && (isset($_GET['view']) && $_GET['view'] == 'edit') && (isset($_GET['field']) && $_GET['field'] == 'content');


    function replaceEndSpaces($string){
        $streepedTags = strip_tags($string,'');

        $last_word_start = strrpos($streepedTags, ' ') + 1; // +1 so we don't include the space in our result
        $last_word = substr($streepedTags, $last_word_start);


        $start =strpos($string, $last_word) + strlen($last_word) +1;
        $end = strlen($string);
        $endOfString = substr($string,$start,$end);

        $prefix = substr($string,0,$start);
        $suffix = strip_tags($endOfString,'<p><span><b><i><div><strong><h1><h2><h3><h4><h5><h6><sup><sub><ul><li><ol><table><tr><th><td><tbody><thead><iframe><video><audio>');

        return $prefix . $suffix;
    }




?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>"
      lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
<head>
    <?php //print $head ?>
    <title><?php if ($user->roles[1] == 'anonymous user') echo "User Login"; else print $head_title ?></title>
    <?php print $styles ?>
    <?php print $scripts ?>
    <!--[if lt IE 7]>
    <?php print phptemplate_get_ie_styles(); ?>
    <![endif]-->
    <link href='https://fonts.googleapis.com/css?family=Signika' rel='stylesheet' type='text/css'>
    <link type="text/css" href="/themes/maennaco/jui/css/redmond/jquery-ui-1.8.15.custom.css" rel="stylesheet"/>
    <link href="/themes/maennaco/jui/comments/css/screen.css?as" type="text/css" rel="stylesheet"/>
    <link href="/themes/maennaco/jui/comments/css/autosuggest.css" type="text/css" rel="stylesheet"/>
    <link href="/themes/maennaco/jui/comments/css/fileuploader.css" type="text/css" rel="stylesheet"/>
    <link type="text/css" rel="stylesheet" href="/themes/maennaco/jquery.rating.css"/>
    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-1.6.2.min.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-ui-1.9.2.custom.min.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery.formatCurrency.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/js/maxlength.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.autosuggest.js"></script>
    <script src="/themes/maennaco/jui/comments/js/jquery.elastic.js" type="text/javascript" charset="utf-8"></script>
    <script src="/themes/maennaco/jui/comments/js/fileuploader.js" type="text/javascript" charset="utf-8"></script>
    <script src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js" type="text/javascript"></script>
    <script type="text/javascript" src="js/jquery.rating.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/js/main.js"></script>
    <meta charset="UTF-8">
    <script type="text/javascript">
        $(function () {
            var tab = "<?php echo $_REQUEST['tab']; ?>";
            var emailRegex = new RegExp(/^([\w\.\-]+)@([\w\-]+)((\.[\w\-]+)+)$/i);
            if (tab == 'companies' || tab == 'professionals' || tab == 'questionnaire' || tab == 'settings') {
                original_mail = $('#edit-email').val();
            }

            if($("#edit-pseudoname").length > 0) {
                $("#edit-pseudoname").focus(function(e){
                    var $input = $(this),
                        confirmed = $input.data('confirmed');

                    if(!confirmed && !confirm("Your username can only be your first name or unrecognizable pseudonym. You can not use your full name.")) {
                        e.preventDefault();
                        e.stopPropagation();
                        return $input.trigger('blur');
                    }

                    $input.data('confirmed', true);
                });
            }

            if($("#edit-email").length > 0 && '#invalid-email' == location.hash) {
                $('#edit-email').attr('style', 'background-color: rgb(255, 228, 228) !important');
            }

            if($("#edit-cmp-member-email").length > 0 && '#invalid-secondary-email' == location.hash) {
                $('#edit-cmp-member-email').attr('style', 'background-color: rgb(255, 228, 228) !important');
            }

            if($("#edit-email").length > 0 && $("#edit-cmp-member-email").length > 0 && '#invalid-emails' == location.hash) {
                $('#edit-email').attr('style', 'background-color: rgb(255, 228, 228) !important');
                $('#edit-cmp-member-email').attr('style', 'background-color: rgb(255, 228, 228) !important');
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

            <?php if(!$isAboutTab):?>
            var left_height = $('table.account-table').height();
            if(left_height<774) {
                // console.log('---------',left_height);
                $('td.left-td .left-td-wrapper').css('min-height', 774);
            }
            else{
                $('td.left-td .left-td-wrapper').css('min-height', left_height + 32);
                $('td.right-td').css('min-height', left_height - 10);
            }
            <?php endif;?>

            $('a[data-toggle="modal"]').click('click', function(){
            	$($(this).attr('href')).dialog({
                	width: '500px',
                    title: $(this).data('title'),
                    resizable: false,
                    open: function (event, ui) {
                        $(this).scrollTop(0);
                    }
                });
                return false;
            })
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
                    "u=<?php echo $u; ?>&" +
                    "m=<?php echo $m; ?>",
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

        function follow_discussion(type, prof_id, user_id, clickObj) {
            if (type == 'follow') {
                var status = 1;
            } else {
                var status = 0;
            }
            $.ajax({
                type: 'get',
                url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
                    'type=followdis&' +
                    'prof_id=' + prof_id + '&' +
                    'user_id=' + user_id + '&' +
                    'status=' + status + '&' +
                    'u=<?php echo $u;?>&' +
                    'm=<?php echo $m;?>',
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

        .tabtags .ask {
            float: none !important;
        }

        .tabtags .ask p {
            margin: 10px 0 !important;
        }
        .tabtags .cmtloop {
            margin: 0 0 10px 0 !important;
        }
        .tabtags .ask {
            margin: 0 0 10px 0 !important;
        }
        .tabtags .ask .aucomnts {
            margin-left: 65px !important;
        }
        .tabtags .askright .w {
            margin-left: 65px !important;
        }
        #question {
            padding-top: 20px;
        }

        .page-account-wrapper {
            height: 100%;
        }

        .page-account-container {
            box-sizing: border-box;
            min-height: 100%;
            padding-bottom: 80px;
        }

        <?php if($isAboutTab):?>
        .left-td-wrapper, .sidebar_box.company_analysis {
            background-color:#fafbfb;
        }

        .abtbtn, .abt .like-bar {
            background:#fff;
        	border: 10px solid #fff;
        }
        <?php endif;?>

    </style>
    <style type="text/css">
    /* Editor & content styles */
    /*, .company-content-wrapper p, #mission_editor, #mission_editor p, #mission_editor ul li*/
    .company-content-wrapper {
    	color: #929497 !important;
        font-family: 'LatoRegular';
        line-height: 24px;
        font-size: 16px !important;
    }
    .company-content-wrapper ul, .company-content-wrapper ol, #mission_editor ul, #mission_editor ol, #goal_editor ul, #goal_editor ol {
	   display: table;
    }
    .company-content-wrapper ul > li, .company-content-wrapper ol > li, #mission_editor ul > li, #mission_editor ol > li, #goal_editor ul > li, #goal_editor ol > li {
	   display: table-row;
    }
    .company-content-wrapper ul > li::before, .company-content-wrapper ol > li::before, #mission_editor ul > li::before, #mission_editor ol > li::before, #goal_editor ul > li::before, #goal_editor ol > li::before {
        display: table-cell !important;
    	width: 1.2em !important;
    }
    .company-content-wrapper ul li, .company-content-wrapper ol li {
	   font-family: 'LatoRegular' !important;
       line-height: 28px;
       font-size: 20px !important;
    }
    /*.company-content-wrapper h2, #mission_editor h2, #goal_editor h2 {
    	color: #929497 !important;
        font-family: 'Lato Black';
        font-size: 24px !important;
    	font-weight: normal !important;
    	line-height: 32px;
    }
    .company-content-wrapper h3, #mission_editor h3, #goal_editor h3 {
    	color: #929497 !important;
        font-family: 'Lato Light';
        font-size: 20px !important;
    	font-weight: bold !important;
    	line-height: 28px;
    }
    .company-content-wrapper h4, #mission_editor h4, #goal_editor h4 {
    	color: #929497 !important;
        font-family: 'LatoRegular';
        font-size: 14px !important;
    }*/
        .ck.ck-custom-heading, .ck.ck-heading_heading-h1, .ck.ck-heading_heading-h2, .ck.ck-heading_heading-h4, .ck.ck-heading_paragraph-p-1, .ck.ck-heading_paragraph-p-2, .about-company-h1, .about-company-h2, .about-company-h4, .about-company-p1, .about-company-p2 {
            color: #686b83 !important;
        }
        .company-content-wrapper .about-company-h1 {
            font-family: Lato Black !important;
            font-size: 22pt !important;
            font-weight: normal !important;
            line-height: 35px;
        }
        .company-content-wrapper .about-company-h2 {
            font-family: 'Lato Light';
            font-size: 22pt !important;
        	font-weight: normal !important;
        	line-height: 32px;
        }
        .company-content-wrapper .about-company-h4 {
            color: #929497 !important;
            font-family: 'Lato Light';
            font-size: 12pt !important;
        }
        .company-content-wrapper .about-company-p1, .company-content-wrapper p:not([class]) {
            font-family: 'LatoRegular';
            font-size: 16px !important;
        	/* font-weight: bold !important; */
            line-height: 28px;
            color: #686b83 !important;
        }
        .company-content-wrapper .about-company-p2 {
            font-family: 'Lato Light' !important;
            font-size: 15px !important;
            font-weight: normal !important;
        }
        .company-content-wrapper  .about-company-p3 {
            font-family: 'LatoRegular'!important;
            font-size: 15pt !important;
            font-weight: normal !important;
        }
        <?php if(!isset($_GET['view']) || $_GET['view'] != 'edit'):?>
        .ck-widget__type-around{
           display:none;
        }
        <?php endif;?>
        figure.media.ck-widget{
            margin: 0 auto;
            width:590px;
        }
        figure.media.ck-widget iframe{
            margin-left:-20px;
        }
    </style>
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
<a name="top"></a>

<div id="wrapper" class="page-account-wrapper">
    <div id="container" class="clear-block page-account-container"
         style="width:<?php if (user_is_logged_in()) echo "1200px"; else echo "940px"; ?>">
        <div id="header">
            <div style="width:280px;padding-left:30px; padding-top: 5px; margin-top:12px; ">
                <a href="<?php echo (!user_is_logged_in() ? '/' : '/account')?>"><img src="<?php echo $base_url; ?>/themes/maennaco/images/index_logo.png"
                                        alt="website logo" border=0 height="30px"></a>
            </div>
            <div id="user-status">
            </div>
            <div class="header-notifications-container">
                <?php if(user_is_logged_in()):?>
                    <?php include 'blocks/profile/header_notifications.php';?>
                <?php endif;?>
            </div>
        </div>


        <?php
        if (!user_is_logged_in())
        {
            ?>
            <?php if ($messages): ?>
            <div id="messages">
                <div class="section clearfix">
                    <?php print $messages; ?>
                </div>
            </div>
        <?php endif; ?>

            <?php
            echo "<div style='padding:30px;'>";
            $n = node_load(54);
            $form = drupal_get_form('maenna_login_form');
            echo theme_status_messages();
            echo $form;
            echo "</div>";
        } else {
        //This block redirects user depending on user type
        if (!isset($_REQUEST['tab'])) {
            if (end($user->roles) == 'Company') {
                    die("<META http-equiv=\"refresh\" content=\"0;URL=/account?tab=companies&page=company_detail&id=" . $user->uid . "\">");
            } else {
                if (end($user->roles) == 'Analyst' ||
                    end($user->roles) == 'Partner' ||
                    end($user->roles) == 'Client Management' ||
                    end($user->roles) == 'Company Member' ||
                    end($user->roles) == 'Operator' ||
                    end($user->roles) == 'Executive' ||
                    end($user->roles) == 'Other Expert' ||
                    end($user->roles) == 'Investor' ||
                    end($user->roles) == 'Author' ||
                    end($user->roles) == 'Consultant') {
                    //if ($user->first_time == '1') {
                        die ("<META http-equiv=\"refresh\" content=\"0;URL=/account?tab=professionals&page=pro_detail&id=" . $user->uid . "\">");
                    //} else {
                    //    die ("<META http-equiv=\"refresh\" content=\"0;URL=/account?tab=insights\">");
                    //}
                }
            }
        }

        require_once("includes/new_functions.inc");

        require_once("includes/maenna_access.inc");
        require_once("includes/maenna_page.inc");


        global $AccessObj, $Maenna_page;

        $AccessObj = new Maenna_access();
        //Printing page identifier under menu line
        echo pageTopIdentifier();

        $Maenna_page = new Maenna_page($AccessObj);

        //Defining global variable for admin to check if admin is assigned to current user

        global $ifAdmin;

        if ($AccessObj->user_type == 'admin') {
            $ifAdmin = in_array($_REQUEST['id'], $AccessObj->assigned_users);
        } elseif ($AccessObj->user_type == 'super') {
            $ifAdmin = true;
        } else $ifAdmin = false;
        $page = "new_" . $Maenna_page->page_name;

        $Pages[] = "${page}_left.inc";
        $Pages[] = "${page}.inc";
        $Pages[] = "${page}_right.inc";

        foreach ($Pages as $p) {
            $path = "includes/" . $p . "";
            include($path);
        }

        $Maenna_page->account_tabs(); ?>
        <div class='divider-top'></div>

        <?php if ($AccessObj->user_type == 'people' && $Maenna_page->page_name == 'companies') : ?>
            <div id="discovertxt">Discover companies to collaborate with</div>
        <?php endif; ?>
        <div id="center">
            <div id="squeeze">
                <?php
                if (isset($node)) {
                    if (in_array($node->nid, array(15, 16, 17))) {
                        get_people_menu(1);
                    } elseif (in_array($node->nid, array(8, 13))) get_people_menu(2);
                }
                ?>
                <div class='account-content-box' style="margin-top:0;">
                    <?php
                    print $Maenna_page->render_before();
                    if (!($AccessObj->user_type == 'people' && $Maenna_page->page_name == 'companies')) :

                        $Maenna_page->user_submit();
                        echo "<div id=\"pro-com-bread-tab-spacer\"></div>";
                        if (count($Maenna_page->Middle_tabs) > 0) {
                            echo $Maenna_page->render_mtab(); /////
                        }
                    endif;
                    ?>
                        <?php if (!($AccessObj->user_type == 'people' && $Maenna_page->page_name == 'companies')) : ?>
                            <table cellpadding=0 cellspacing=0 border=0
                                   class='account-table <?= $Maenna_page->page_name ?> <?= $_GET['section'] ?>'
                                 >
                                <tr>
                                    <?php ob_start(); ?>
                                    <?php if (count($Maenna_page->Left) > 0) { ?>
                                        <td class='left-td'>
                                            <div class='left-td-wrapper'>
                                                <?php $Maenna_page->render('left'); ?>
                                            </div>
                                        </td>
                                    <?php } ?>
                                    <td class='act-content'>
                                        <?php if ($messages): ?>
                                            <div id="messages">
                                                <div class="section clearfix">
                                                    <?php print $messages; ?>
                                                </div>
                                            </div>
                                        <?php endif; ?>
                                        <?php if($isEditContentTab):?>
                                            <?php
                                                $Maenna_page->render('content');
                                            ?>
                                        <?php else:?>
                                            <?php
                                                echo theme_status_messages();
                                                $Maenna_page->render('middle');
                                            ?>
                                        <?php endif; ?>
                                    </td>
                                    <td class='right-td'>
                                        <?php $Maenna_page->render('right'); ?>
                                    </td>
                                    <?php ob_end_flush(); ?>
                                </tr>






                                <!--start => highlights-->
                                <?php
                                if ($AccessObj) {

                                    $mode = $AccessObj->section_mode('highlights');
                                    $m_tab = sget($_REQUEST, 'mtab');
                                    $_section = sget($_REQUEST, 'section');
                                    $_panel = sget($_REQUEST, 'panel');
                                    $_view = sget($_REQUEST, 'view');
                                    if ($m_tab == 'about' && !$_section && !$_panel && !$_view) {

                                        $highlightsNav = null;
                                        if (function_exists('highlights')) {
                                            $highlightsNav = highlights($mode);
                                        }
                                        if ($highlightsNav && $highlightsNav['count'] > 0) {
                                            ?>
                                            <tr>
                                                <td class='left-td '>
                                                    <div class='left-td-wrapper highlights-left'>
                                                        <?= $highlightsNav['body']; ?>
                                                    </div>
                                                </td>
                                                <td class='act-content'>
                                                    <div class="highlights_content">
                                                        <?php
                                                        if (function_exists('highlights_content')) {
                                                            echo highlights_content($mode)['body'];
                                                        }
                                                        ?>
                                                    </div>
                                                </td>
                                                <td class='right-td'>

                                                </td>

                                            </tr>


                                            <script>
                                                function changeTab(evt, tabId) {
                                                    var i, tabcontent, tablinks;
                                                    tabcontent = document.getElementsByClassName("tabcontent");
                                                    for (i = 0; i < tabcontent.length; i++) {
                                                        tabcontent[i].style.display = "none";
                                                    }
                                                    tablinks = document.getElementsByClassName("tablinks");
                                                    for (i = 0; i < tablinks.length; i++) {
                                                        tablinks[i].className = tablinks[i].className.replace(" active", "");
                                                    }
                                                    tabId.style.display = "block";
                                                    evt.currentTarget.className += " active";
                                                }
                                            </script>
                                            <?php
                                        }
                                    }
                                } ?>
                                <!--end => highlights-->

                            </table>
                        <?php else: $Maenna_page->render('middle'); ?>
                        <?php endif; ?>
                    <?php
                    }
                    ?>
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
    <!-- container -->
</div>
<!-- <div id="footer" style="padding-top:20px;clear:both;" class="page-account-footer"> -->

<div id="footer" style="clear:both;" class="page-account-footer">
    <div class="divider">
        <?php require_once("footer-maennaco.php"); ?>
    </div>
    <!-- footer -->

    <div id="policy" style="font-family: Helvetica, arial; font-size:12px; display:none;"></div>
    <div id="learn-more-dialog" style="display:none;">
        <?php require_once './themes/maennaco/learn-more-dialog.php' ?>
    </div>

</div>

<script>
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
        $(".show_terms").click(function () {
            $.post("/themes/maennaco/includes/homepage_posts.php?type=policy&req_type=" + $(this).attr('type'), {}, function (response) {
                $("#policy").html(response);
            });
            $("#policy").dialog("open");
            return false;
        });
    });
    $(document).tooltip({
        items: "a[data-tooltip], span[data-tooltip], div[data-tooltip],li[data-tooltip],td[data-tooltip],input[data-tooltip],textarea[data-tooltip]",
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
    $(".pro-card-tool").tooltip({
        items: "[pro-card-tooltip]",
        tooltipClass: "card_tip",
        show: null, // show immediately
        hide: {effect: ""}, //fadeOut
        close: function (event, ui) {
            ui.tooltip.hover(
                function () {
                    $(this).stop(true).fadeTo(400, 1);
                },
                function () {
                    $(this).fadeOut("400", function () {
                        $(this).remove();
                    })
                }
            );
        },
        content: function () {
            var element = $(this);
            return $("div[uid='" + $(this).attr("pro-card-tooltip") + "']").html();
        }
    });
    
    function showExpertInfo(expertId) {
    var uid = "<?= $user->uid;?>";
    $.post("/themes/maennaco/includes/pro_posts.php?type=profileInfo&display=true&pro_id=" + expertId + "&uid=" + uid, function (response) {

        $("#pro_popup").dialog({
            autoOpen: true,
            width: 650,
            title: "Profile",
            resizable: false,
            draggable: false,
            height: 400,
            closeText: "hide",
            buttons: {},
            closeOnEscape: true,
            modal: true
        }).html(response);

    }, "html");
}
    
</script>
<div id="pro_popup" class="hidden"></div>

</body>
</html>
