<?php
 $maenna_page['right']= array('comsum_super_shortcuts');

function comsum_super_shortcuts($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $Block['title'] = "";
    $content = "
        <a href=''>Rank by ROE</a><br />
        <a href=''>Rank by Revenue Growth</a><br />
        <a href=''>Rank Earning Growth</a><br />
        <a href=''>Industry Statistics</a><br />
        <a href=''>Geography Statistics</a><br />
    ";
    $Block['body'] = sidebar_box($Block['title'], $content);
    
    return $Block;
}

/*EOF */