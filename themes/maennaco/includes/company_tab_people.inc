<?php
defined("__ACCOUNT__") or die("no direct access allowed");
global $maenna_page;




function search_people($op = null)
{
    global $user;
    $uid = $user->uid;
    
    $Block['title'] = "";
    if($op == 'view' || $op == null){
        $content = people_search();

        $Block['body'] = content_box($Block['title'], $content);
    }
    
    return $Block;
}
function top_ranked_people($op = null)
{
    global $user;
    $uid = $user->uid;
    
    $Block['title'] = 'Top Ranked';
    if($op == 'view' || $op == null){
        $content = "DSA";
        
        $html = <<< END
        <table>
            <tr><td>name</td><td>points</td><td>cos</td></tr>
        </table>
END;
        $Block['body'] = sidebar_box($Block['title'], $html);
    }
     return $Block;
}
function contributions($op = null){
    global $user;
    $uid = $user->uid;
    
    $Block['title'] = 'Contributions';
    if($op == 'view' || $op == null){
        $content = "DSA";
        
         $html = <<< END
        <table class='two-column-a'>
            <tr><td>id</td><td>link</td></tr>
            <tr><td>id</td><td>link</td></tr>
            <tr><td>id</td><td>link</td></tr>
        </table>
END;
        $Block['body'] = sidebar_box($Block['title'], $html);
    }
     return $Block;
}
function people_search()
{    
    $Search_parameter = array();
    $Selection = array();
    $Sectors = _sectors();
    $PRoles = _pro_roles();
    $States = get_us_states();
    $Search_parameter = array(
                                    'user_type' => $PRoles,
                                    'sectors' => $Sectors,
    );
    $Search_result  = get_search_result('p');
    $paginationBaseURL = '/account?a=cpro&';
    $block = render_people_search($Search_parameter, $Search_result, $paginationBaseURL);
    return $block;
}


function get_p_positions()
{
    $sql = "select distinct ";
}
/* EOF */