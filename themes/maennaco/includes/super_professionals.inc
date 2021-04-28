<?php
function pro_list($op = null)
{
    $Block['title'] = 'Professionals';
    if($op == 'view' || $op == null){
        $content = '';
        
        $content = pro_search();
        
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function pro_search()
{
    $Search_parameter = array();
    $Selection = array();
    $Sectors = _sectors();
    $PRoles = _pro_roles();
    $States = get_us_states();
    $Search_parameter = array(
                                  
    );
    $Search_result  = get_search_result('p');
    $paginationBaseURL = '/account?a=professionals&';
    $block = render_people_search($Search_parameter, $Search_result, $paginationBaseURL);
    return $block;
}


/* EOF */