<?php
/*
pro_types
pro_memberstats
mlvl_stats
sector_stats
expertis_stats
*/

function pro_types($op = null)
{
    global $user;
    $editorid = $user->uid;

    $Block['title'] = 'Professional Type';
    $content = '';
    $time = time();
    
    $_ProType = _pro_roles();
    $type_keys = implode(',',array_keys($_ProType));
    if($op == 'view' || $op == null)
    {
        $sql = "select count(rid) as cnt, rid from users_roles, maenna_people where rid in ($type_keys) and uid = pid and (delete_marker is null or delete_marker != 1) group by rid";
        
        $result = db_query($sql);
        while(($Row = db_fetch_object($result)) !== false)
        {
            $cnt = $Row->cnt;
            $rid = $Row->rid;
            $type = $_ProType["$rid"];
            $content .= "<div class=row style='padding:2px 0;position:relative;'><div style='position:absolute;text-align:right;width:100%'>$cnt</div>$type</div>";
        }
//        $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
//                    "<a href='/account?a=professionals&panel=pro_types_panel'>more</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}

function pro_memberstats($op = null)
{
    global $user;
    $editorid = $user->uid;

    $Block['title'] = 'Member Statistics';
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
    {
        
        $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
                    "<a href='/account?a=professionals&panel=pro_memberstats_panel'>more</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}

function mlvl_stats($op = null)
{
    global $user;
    $editorid = $user->uid;

    $Block['title'] = 'Management Level';
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
    {
        
        $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
                    "<a href='/account?a=professionals&panel=mlvl_stats_panel'>more</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}

function sector_stats($op = null)
{
    global $user;
    $editorid = $user->uid;

    $Block['title'] = 'Sector Statistics';
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
    {
        
        $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
                    "<a href='/account?a=professionals&panel=sector_stats_panel'>more</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}

function expertis_stats($op = null)
{
    global $user;
    $editorid = $user->uid;

    $Block['title'] = 'Expertise Statistics';
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
    {
        
        $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
                    "<a href='/account?a=professionals&panel=expertis_stats_panel'>more</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}


/* EOF */