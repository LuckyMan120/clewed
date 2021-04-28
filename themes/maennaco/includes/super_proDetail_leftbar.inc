<?php
$maenna_page['left'] = array(
                              'pro_relatedPro');

function pro_relatedPro($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = 'Related Professionals';
    $content = '';
    
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
    
}

/* EOF */