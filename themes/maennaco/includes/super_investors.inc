<?php
function investor_list($op = null)
{
    $Block['title'] = 'Investors';
    if($op == 'view' || $op == null){
        $content = '';
        
        $content = investor_search();
        
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function investor_search()
{
    $Search_parameter = array();
    $Selection = array();
    $Sectors = _sectors();
    $PRoles = _pro_roles();
    $States = get_us_states();
    $Search_parameter = array(
                                  
    );
    $Search_result  = get_search_result('investor');
    $paginationBaseURL = '/account?a=investors&';
    $block = render_people_search($Search_parameter, $Search_result, $paginationBaseURL);
    return $block;
}


/* EOF */