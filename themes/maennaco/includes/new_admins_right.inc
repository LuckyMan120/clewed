<?php

function add_admin_user($op = null)
{
    $Block['title'] =  'Add Admin User'; 
    if($op && $op != 'update') {
        
        $Block['title'] .= "<div class=editbtn>
                        <a href='/account?tab=admins&page=admin_detail&view=new'    class='tool' >Edit</a></div>";
    }elseif($op == 'update'){
        
        
        
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

/* EOF */