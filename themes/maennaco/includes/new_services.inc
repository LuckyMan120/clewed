<?php
global $redirect;
$redirect = rebuild_url(array('tab', 'page'));

function pro_tabs()
{
    global $base_url;
    $page = sget($_REQUEST, 'page');
    $type = sget($_REQUEST, 'type');
    $id = sget($_REQUEST, 'id');

    $content = '<div class="account-section-tabs">
            <ul>
         		 

            </ul>
        </div>';

    //$Block['body'] = content_box($Block['title'], $content);
    echo $content;
}

function services($op = null)
{
    global $user;
    global $AccessObj;

    $user_type = $AccessObj->user_type;

    $editorid = $user->uid; // the currently logged in user, print_r this object to see all data available

    $companyid = sget($_REQUEST, 'id');

    $content = '';
    global $redirect;
    $data_type = 'discussions';
    $panel  = 'multi';
    $section=__FUNCTION__;

    $previleges = array('super', 'admin');
    if(in_array($user_type, $previleges)) {
        $Block['title'] = 'SERVICES';
    } elseif($user_type == 'people') {
        $open_insights = (!empty($_POST['openinsights']) || !empty($_GET['openinsights'])) ? true : false;
        $open_services = (!empty($_POST['openservices']) || !empty($_GET['openservices'])) ? true : false;
        $Block['title'] = 'SERVICES 
        <div class="schedule">
            <div class="sched">
                <span id="questmark" class="insights" style="float: right;">
                    <a href="#" id="get-help-btn2">
                        <img src="/themes/maennaco/images/questionmark_white.png" style="padding-top:5px; margin-left: 6px;">
                    </a>
                </span>
            </div>
        </div>
        <div class="schedule" id="serviceTab" ' . ($open_services ? 'style="display: none;"' : '') . '>
            <div class="sched">
                <div class="discmnts"></div>
                <a href="/account?tab=professionals&page=pro_detail&id='.$AccessObj->uid.'&section=pro_industry_view&type=discussion&openservices=1">
                    <span style="text-transform:none !important;" data-tooltip="Share your expertise by creating your own Services. Click the ? on the right for guidelines.">
                        Create Services
                    </span>
                </a>
            </div>
        </div>
        ';
    } else {
        $Block['title'] = 'SERVICES';
    }


    if (sget($_REQUEST, 'name')) {
        $Block['title'] = sget($_REQUEST, 'name');
        $fname = sget($_REQUEST, 'name');
    }

    ob_start();
    $url = "http://" .$_SERVER['HTTP_HOST'];
    $editorname = $user->name;
    require_once("pro_comments.php");
    $content = ob_get_contents();
    ob_end_clean();

    //$content = sget($_REQUEST, 'file');

    $Block['body'] = content_box($Block['title'], $content, 'services-middle blue_bar');
    return $Block;
}

/* EOF */
