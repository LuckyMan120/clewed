<?php

$company = new Clewed\Company\Model();
if (!isset($_REQUEST['company_id'])) {
    $_REQUEST['company_id'] = $_REQUEST['id'];
}
$companyId = $_REQUEST['company_id'];
$info = $company->getCompanyInfo();
$db = \Clewed\Db::get_instance();

$invetment_url = "account?tab=companies&page=company_detail&id=".$companyId."&section=company_name&panel=prof_explore_investment&mode=add";

$numLikes = (int) $db->get_column(
    'SELECT COUNT(*) FROM `like_company` WHERE `project_id` = :project_id',
    array(':project_id' => $companyId)
);

$collaboratorStatus = '';

if (!empty($user) && $user->uid != 0) {
    $userLikesTheCompany = (bool) $db->get_column(
        'SELECT COUNT(*) FROM `like_company` WHERE `project_id` = :project_id AND `user_id` = :user_id',
        array(':project_id' => $companyId, ':user_id' => $user->uid)
    );
    $collaboratorStatus = $db->get_column(
        'SELECT `status` FROM `maenna_connections` WHERE `assignee_uid` = :user_id AND `target_uid` = :project_id AND `conntype` = "collaborator" LIMIT 1',
        array(':user_id' => $user->uid, ':project_id' => $companyId)
    );
    $canCollaborate = $db->get_column(
        'SELECT COUNT(*)
         FROM `maenna_people_data`
         WHERE `pid` = :pid
         AND `data_type` = "addinfo"
         AND `data_value3` = "approved"
         AND `data_attr` IN ("experties", "industryview", "mgmtview")',
         array(':pid' => $user->uid)
    ) >= 3;
    switch ($collaboratorStatus) {
        case 'pending':
            $value = sprintf(
                '<span class="contribute"><a style="cursor:pointer;font-family:\'Lato Bold Italic\',sans-serif;font-size:14px;color:#00A2BF;" title="Disconnect" type="uncontribute" cid="%d" uid="%d" class="contribute">Pending</a></span>',
                $companyId,
                $user->uid
            );
            break;
        case 'active':
            $value = sprintf(
                '<span class="contribute"><a style="cursor:pointer;font-family:\'Lato Bold Italic\',sans-serif;font-size:14px;color:#00A2BF;" title="DISCONNECT" type="uncontribute" cid="%d" uid="%d" class="contribute">Connected</a></span>',
                $companyId,
                $user->uid
            );
            break;
        case 'deactivated':
            $value = '<span class="contribute"><a style="cursor:pointer;font-family:\'Lato Bold Italic\',sans-serif;font-size:14px;color:#00A2BF;" title="FULL" class="ablike contribute">Connected</a></span>';
            break;
        default:
            if ($user->uid != $companyId && $info['shareable']) {
                if (!$canCollaborate) {
                    $value = '<span class="contribute"><a class="ablike" type="no_coll" style="cursor: pointer;font-family:\'Lato Bold Italic\',sans-serif;font-size:14px;color:#00A2BF;">Request connection</a></span>';
                } else {
                    $value = sprintf(
                        '<span class="contribute"><a style="cursor:pointer;font-family:\'Lato Bold Italic\',sans-serif;font-size:14px;color:#00A2BF;" type="contribute" cid="%d" uid="%d" class="contribute">Request connection</a></span>',
                        $companyId,
                        $user->uid
                    );
                }
            }
    }
}



?>
    <style>
        #follow_dis a:hover {
            width: 150px !important;
        }
        
        div.row0 {
            position: relative;
            border-bottom: solid 1px #d0d2d3;
            padding-left: 20px;
            font-family: 'LatoRegular', sans-serif;
            font-style: italic;
            color: #ffffff;
            font-size: 14px;
            height: 16px;
            line-height: inherit;
        }
        
        div.abt {
            margin-top: 15px;
        }
        
        div.abt-content {
            width: 590px !important;
            height: 240px !important;
            background-color: #fff;
        }
        
        div.abtloop {
            padding: 0 0 0 11px;
        }
        
        div.gig-button-container {
            margin-left: 8px;
            margin-right: 12px;
        }
        
        span.gig-counter-text-top {
            font-size: 16px !important;
        }
        
        span.gig-counter-text {
            top: 0;
            color: #333;
            font: normal normal normal 11px/18px 'Helvetica Neue', Arial, sans-serif;
        }
        
        .ck-widget__type-around {
            display: none;
        }
        p {
            font-size: 16px;
            color:#929497 !important;
        }
        h2 {
            font-size: 24px;
            font-family: Lato Black;
            color: #929497;
        }
        h3 {
            font-size: 20px;
            font-family: Lato Light;
            color: #929497;
        }
        h4 {
            font-size: 12px;
            font-family: Lato Regular;
            color: #929497;
        }
        #mission_editor, #goal_editor{
            border: 1px solid #c4c4c4;
			border-top:none;
        }
        #mission_editor ol, #goal_editor ol {
            list-style: none;
            counter-reset: item;
            color: #686b83 !important;
        }
        #mission_editor ol li, #goal_editor ol li {
            counter-increment: item;
            margin-bottom: 5px;
            margin-left: 10px;
            color: #686b83 !important;
            font-size:  15px;
            font-family: Lato Light;
        }
        #mission_editor ol li:before, #goal_editor ol li:before {
            content: counter(item);
            color: #00A2BF;
            font-family: Lato Black;
            font-size: 15px;
            width: 1.2em;
            display: inline-block;
            font-weight: bold;
        }
        #mission_editor ul li, #goal_editor ul li {
            margin-left: 10px;
            padding: 0 0 .2em 0;
            background: none;
            list-style: none;
            font-size: 15px;
            font-family: Lato Light;
            color: #686b83 !important;
        }
        #mission_editor ul li:before, #goal_editor ul li:before {
              content: '•';
              color: #00A2BF;
              font-weight: bold;
              font-size: 15px;
              font-family: Lato Black;
        }
        .ck.ck-custom-heading, .ck.ck-heading_heading-h1, .ck.ck-heading_heading-h2, .ck.ck-heading_heading-h4, .ck.ck-heading_paragraph-p-1, .ck.ck-heading_paragraph-p-2, .about-company-h1, .about-company-h2, .about-company-h4, .about-company-p1, .about-company-p2 {
            color: #686b83 !important;
        }
        .ck.ck-heading_heading-h1, .about-company-h1 {
            font-family: Lato Black !important;
            font-size: 22pt !important;
            font-weight: normal !important;
            line-height: 35px;
        }
        .ck.ck-heading_heading-h2, .about-company-h2 {
            font-family: 'Lato Light';
            font-size: 22pt !important;
        	font-weight: normal !important;
        	line-height: 32px;
        }
        .ck.ck-heading_heading-h4, .about-company-h4 {
            color: #929497 !important;
            font-family: 'Lato Light';
            font-size: 12pt !important;
        }
        .ck.ck-heading_paragraph-p-1, .about-company-p1, #mission_editor p:not([class]), #goal_editor p:not([class]) {
            font-family: 'LatoRegular';
            font-size: 16px !important;
        	/* font-weight: bold !important; */
            line-height: 28px;
            color: #686b83 !important;
        }
        .ck.ck-heading_paragraph-p-2, .about-company-p2 {
            font-family: Lato Light !important;
            font-size: 15px !important;
            font-weight: normal !important;
		}
		.ck.ck-heading_paragraph-p-3, .about-company-p3 {
            font-family: 'LatoRegular'!important;
            font-size: 15pt !important;
            font-weight: normal !important;
        }
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
        .company-content-wrapper .about-company-p1, .company-content-wrapper p:not([class]) {
            font-family: 'LatoRegular';
            font-size: 16px !important;
        	/* font-weight: bold !important; */
            line-height: 28px;
            color: #686b83 !important;
        }
        /*br[data-cke-filler] {
            display: none;
            visibility: hidden;
        }*/

    </style>

    <div class="divider-top"></div>

    <div id="center">
        <div id="squeeze">
            <div class='account-content-box' style="margin-top: 0;">
                <table style="width: 936px;border-bottom: 1px solid #d0d2d3;" class='account-table company_detail'>
                    <tr>
                        <td class="left-td" style="padding-top: 7px; width: 170px; display: table-cell; background-color: #fafbfb;">
                            <div class='left-td-wrapper'>
                                <div class='sidebar_box '>
                                    <div class='wrapper'>
                                        <div class='box_title'>
                                            <span style="color:#284B54;font-family: 'Lato Bold', sans-serif;" class='proj_name'> <?= $info['projname'] ?></span>
                                        </div>
                                        <div class="box_content">
                                            <table class="no-border" style="margin-top: 0; width: 150px;">
                                                <tr>
                                                    <td>
                                                        <?php if (!empty($info['city'])) : ?>
                                                        <div class="row0">
                                                            <?= $info['city'] . ($info['state'] ? ', ' . $info['state'] : '' ) ?>
                                                        </div>
                                                        <?php endif ?>
                                                        <div class="row0">Founded:
                                                            <?= $info['founded'] ?>
                                                        </div>
                                                        <div class="row0">
                                                            <?= ucfirst($info['sector']) ?>
                                                                <?php if($info['revenue']):?> -
                                                                <?= $info['revenue'] ?>&thinsp;M
                                                                    <?php endif;?>
                                                        </div>
                                                    </td>
                                                </tr>
                                            </table>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </td>
                        <td class='act-content' style="text-align: left;width: 400px; padding:10px 0 0; border-left:10px solid #fff; border-right: 1px solid #d0d2d3;">
                            <div class=" " style="margin-top:-40px; ">
                                <div id="pro-com-bread-tab-spacer "></div>
                                <div class='account-section-tabs' style='width: 900px !important; position:absolute; margin:0 20px; height:29px; background-color: #fff;'>
                                    <ul style="margin:0; ">
                                        <li class='first active-trail active'>
                                            <a href='/account?tab=companies&page=company_detail&id=654&mtab=about'
                                            class='active-trail active'  style="background-color:#fafbfb;border-bottom:1px solid #fafbfb; ">
                                                about
                                            </a>
                                        </li>
                                    </ul>
                                </div>

                                <?php $path = ROOT . '/themes/maennaco/images/project/';?>
                                <div class="abt " style="margin:0 20px; padding-top:40px; ">
                                    <?php if (!empty($info['project']) && file_exists($path . $info['project'])):?>
                                    <div class="abt-content " style="height: 323px !important; ">
                                        <img src="/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/<?=$info['project']?>" width="100%" height="320px"/>
                                    </div>
                                    <?php elseif(!empty($user) && $user->uid == sget($_REQUEST, 'id')):?>
                                    <div class="abt-content" style="text-align:center;">
                                        <div class="abtadd">
                                            <span>
                                                                        Add a photo that<br/>
                                                                        tells your story.
                                                                    </span>
                                        </div>
                                    </div>
                                    <?php else: ?>
                                    <div class="abt-content" style="height: 442px !important;">
                                        <img src="/themes/maennaco/phpthumb/phpThumb.php?src=../images/cmp-avatar-service.png" width="100%" height="320px"/>
                                    </div>
                                    <?php endif;?>
                                    <div style="margin: 0 12px; text-align: center; width: 580px; color: #929497">
                                        <div class="" style="width: 190px; text-align: left; display: inline-block">
                                            <div
                                            class="status-label"
                                            style="font-family: Lato Bold Italic; font-size: 12px"
                                            >
                                            MAXIMUM
                                            <span
                                                class="light"
                                                style="font-family: Lato Light; font-size: 12px; color: #929497"
                                                >per investor</span
                                            >
                                            </div>
                                            <div
                                            class="status-label status-amount"
                                            style="font-family: Lato Bold Italic; font-size: 15px"
                                            >$
                                            <?=$info['max_per_investor']?>
                                            </div>
                                        </div>
                                        <div class="" style="width: 190px; display: inline-block">
                                            <div
                                            class="status-label"
                                            style="font-family: Lato Bold Italic; font-size: 12px"
                                            >
                                            PRICE/SHARE
                                            </div>
                                            <div
                                            class="status-label status-amount"
                                            style="font-family: Lato Bold Italic; font-size: 15px"
                                            >$
                                            <?=$info['estimated_share_price']?>
                                            </div>
                                        </div>
                                        <div class="" style="width: 190px; text-align: right; display: inline-block">
                                            <div
                                            class="status-label"
                                            style="font-family: Lato Bold Italic; font-size: 12px"
                                            >
                                            TERM
                                            </div>
                                            <div
                                            class="status-label status-amount"
                                            style="font-family: Lato Bold Italic; font-size: 15px"
                                            >
                                            <?=$info['term']?>
                                            </div>
                                        </div>
                                    </div>
                                    <div
                                        style="
                                            font-size: 22pt;
                                            font-family: Latoregular;
                                            margin-left: 12px;
                                            margin-top: 36px;
                                            margin-bottom: 8px;
                                            color: #929497;
                                            line-height: 35px;
                                            word-spacing: 2px;
                                        "
                                        >
                                    <?=$info['deal_summary_title']?>
                                    </div>
                                    <div
                                        style="
                                            text-align: justify;
                                            padding: 10px;
                                            border: 5px solid #fafbfb12;
                                            font-family: Lato Light, sans-serif;
                                            color: #929497;
                                            font-size: 16px;
                                            font-style: normal;
                                        "
                                        >
                                       <?=$info['deal_summary_statement']?>
                                    </div>
                                </div>
                            </div>
                        </td>
                        <td class="right-td">
                            <input type="hidden" id="user_id" value="892" />
                            <div class="sidebar_box company_analysis">
                                <div class="wrapper">
                                <div class="fieldset">
                                    <div class="amount raised-amount">$<?=$info['round_amount_raising']?></div>
                                    <div class="status_bars_wrapper">
                                    <div class="status-bar-wrapper">
                                        <div class="status-bar"></div>
                                        <div class="status-bar-progress" style="width: 0.3%"></div>
                                    </div>
                                    </div>
                                    <div class="status-label">Amount raising in this round</div>
                                </div>
                                <div class="fieldset">
                                    <div class="amount percentage-offered"><?=$info['amount_raising_percent']?>%</div>
                                    <div class="status-label">Offered in this round</div>
                                </div>
                                <div class="fieldset">
                                    <div class="amount days-to-go"><?=$close_date?></div>
                                    <div class="status-label" style="color: #929497">Close Date</div>
                                </div>
                                <div class="fieldset">
                                    <div class="amount percentage-offered"><?=$info['security_type']?></div>
                                    <div class="status-label">Security type</div>
                                </div>
                                <div class="fieldset">
                                    <div class="amount percentage-offered"><?=$info['interest_rate']?>%</div>
                                    <div class="status-label">Interest rate</div>
                                </div>
                                <div class="fieldset">
                                    <div class="status-label funding-purpose--value"><?=$info['funding_purpose']?></div>
                                    <div class="status-label funding-purpose--label">Funding Purpose</div>
                                </div>
                                <div class="fieldset">
                                    <div class="blue_button_box with-status-label">
                                    <a
                                        class="blue_button ablike"
                                        style="cursor: pointer; width: 100%; text-decoration: none;"
                                        cid="887"
                                        uid="892"
                                        flag="1"
                                        ><div
                                        class="main-text"
                                        style="width: 120%; margin-left: -20px; font-size: 16px"
                                        >
                                        EXPLORE INVESTMENT
                                        </div>
                                        <div class="button-status-label">$<?=$info['min_per_investor']?> Minimum</div></a
                                    >
                                    </div>
                                </div>
                                </div>
                            </div>
                        </td>
                    </tr>
                </table>
                <div style="clear:both"></div>
                <div class='abtloop'>
                    <div style="clear:both"></div>
                    <div class="company-content-wrapper">
                        <p style="font-size: 15px !important;">
                            <?= array_key_exists('mission', $info) ? $info['mission'] : '' ?>
                        </p>
                    </div>
                </div>
            </div>
            <!-- visitor-content-box -->

            <div style="clear:both"></div>
            <!-- squeeze -->
        </div>
        <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
        <script type="text/javascript">
            $(document).ready(function() {
                init_contribute();
            });
        </script>
        <div style="clear:both"></div>
        <?php if (empty($user) || $user->uid == 0) : ?>
        <?php
            include ROOT . '/themes/maennaco/dialogs/comp.php';
            //include ROOT . '/themes/maennaco/dialogs/register.php';
            ?>
            <script type="text/javascript">
                $('#comp-dialog').dialog({
                    modal: true,
                    autoOpen: false,
                    draggable: true,
                    resizable: false,
                    width: 350
                });
                $('#register-dialog').dialog({
                    modal: true,
                    autoOpen: false,
                    draggable: true,
                    resizable: false,
                    width: 350,
                    buttons: [{
                        text: 'Register',
                        click: function(e) {
                            var clicked = $(e.target);
                            var fValid = true;
                            var selTab = window.utype;
                            if (selTab == 'company') {
                                fValid = fValid && checkString($("#company-firstname"), 'First Name');
                                fValid = fValid && checkString($("#company-lastname"), 'Last Name');
                                fValid = fValid && checkString($("#company-name"), 'Company Name');
                                fValid = fValid && checkString($("#company-email"), 'Email');
                                fValid = fValid && checkString($("#company-password"), 'Password');
                                fValid = fValid && password_type($('#company-password'));
                                fValid = fValid && checkString($("#company-industry"), 'industry');
                                fValid = fValid && checkString($("#company-revenue"), 'revenue');
                                if (!fValid) {
                                    alert('Please fill in the required fields.');
                                }
                                if (fValid && !$('#cmp-agree').is(':checked')) {
                                    fValid = false;
                                    alert('You have to read and agree with terms and privacy policy!');
                                }
                                if (fValid) {
                                    clicked.parent().attr('disabled', true);
                                    $.post(
                                        'register_new_user.php?type=company', {
                                            firstname: $("#company-firstname").val(),
                                            lastname: $("#company-lastname").val(),
                                            name: $("#company-name").val(),
                                            email: $("#company-email").val(),
                                            password: $("#company-password").val(),
                                            industry: $("#company-industry").val(),
                                            revenue: $("#company-revenue").val()
                                        },
                                        function(response) {
                                            alert(response);
                                            if (response != 'Security code is not correct!') {
                                                location.reload();
                                            }
                                        });
                                }
                            } else {
                                fValid = fValid && checkString($("#pro-type"), 'iam');
                                fValid = fValid && checkString($("#pro-firstname"), 'First Name');
                                fValid = fValid && checkString($("#pro-lastname"), 'Last Name');
                                fValid = fValid && checkString($("#pro-email"), 'Email');
                                fValid = fValid && checkString($("#pro-password"), 'Password');
                                fValid = fValid && checkString($("#pro-experties"), 'experties');
                                if (!fValid) {
                                    alert('Please fill in the required fields.');
                                }
                                if (fValid && !$('#pro-agree').is(':checked')) {
                                    fValid = false;
                                    alert('You have to read and agree with terms and privacy policy!');
                                }
                                if (fValid) {
                                    clicked.parent().attr('disabled', true);
                                    $.post(
                                        'register_new_user.php?type=proffesional', {
                                            firstname: $("#pro-firstname").val(),
                                            lastname: $("#pro-lastname").val(),
                                            experties: $("#pro-experties").val(),
                                            email: $("#pro-email").val(),
                                            password: $("#pro-password").val(),
                                            pro_type: $("#pro-type").val()
                                        },
                                        function(response) {
                                            alert(response);
                                            if (response != 'Security code is not correct!') {
                                                location.reload();
                                            }
                                        }
                                    );
                                }
                            }
                        },
                        "class": 'form-submit1'
                    }, {
                        text: 'Cancel',
                        click: function() {
                            $(this).dialog('close');
                        },
                        "class": 'form-submit2'
                    }]
                });
                $('.ablike').bind('click', function() {
                    //$('#comp-dialog').dialog('open');
                    //$('.login-start').bind('click', function () {
                    $(".header-login").trigger("click");
                    //});
                });
            </script>
            <?php else : ?>
            <script type="text/javascript">
                function like_company(action, pid, uid) {
                    $.ajax({
                        type: 'get',
                        url: '/themes/maennaco/includes/like.php?a=' + action + '&p=' + pid + '&u=' + uid,
                        success: function(r) {
                            action = action === 'Like' ? 'Unlike' : 'Like';
                            $('#like_company_id').html('<a style="font-family: \'Lato Bold Italic\',sans-serif; font-size: 14px; color: #00A2BF;" onclick="like_company(\'' + action + '\', ' + pid + ',' + uid + ');">' + action + '</a>');
                            $('.counter').text(r);
                        }
                    });
                }
            </script>
            <?php endif; ?>
        </div>