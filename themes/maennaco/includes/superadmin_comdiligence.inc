<?php

$maenna_page['content']= array('diligence_setup');
$maenna_page['right']= array('diligence_files'); 
function diligence_setup($op = null){
    $companyid = sget($_REQUEST,'companyid');
    global $user;
    $editorid = $user->uid;
    
    $editorrid = userRoleId($editorid);

    $Block['title'] = 'Diligence Question';
    $content = '';
    if($op == null || $op == 'view')
    {
        if($editorrid == 10){
            $Block['title'] .= " (superadmin)";
            
        }
        
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

/* EOF */