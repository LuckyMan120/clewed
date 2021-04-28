<?php

//$maenna_page['right'] = array('info_tools', 'upload_box', 'maenna_selection', 'recent_topics', 'admin_approve');
global $redirect;
$redirect = rebuild_url(array('tab', 'page'));

function pro_DiscussionCalendar($op = null) {
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $pro_id = sget($_REQUEST, 'pro_id');
    //$Block['title'] = 'Calendar';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    //if($op == 'write')$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>EDIT</a></div>";
    $content = '';

    if (empty($op))
        return '';

    ob_start();
    $url = "http://" . $_SERVER['HTTP_HOST'];
    $editorname = $user->name;
    require_once("pro_calendar_new.php");
    $content = ob_get_contents();
    ob_end_clean();

    //$content = sget($_REQUEST, 'file');

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

?>