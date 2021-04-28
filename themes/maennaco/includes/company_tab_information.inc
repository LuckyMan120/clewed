<?php
defined("__ACCOUNT__") or die("no direct access allowed");
global $maenna_page;




function company_transcript($op = null)
{
    global $user;
    $uid = $user->uid;
    
    $Block['title'] = "Interveiws and advisory transcripts";
    if($op == 'view' || $op == null){
        $content = "TEXT";
        $Block['body'] = content_box($Block['title'], $content);
    }
    
    return $Block;
}
function company_article_researches($op = null)
{
    global $user;
    $uid = $user->uid;
    
    $Block['title'] = "Related articles & research";
    if($op == 'view' || $op == null){
        $content = "TEXT";
        $Block['body'] = content_box($Block['title'], $content);
    }
    
    return $Block;
}
function company_media($op = null)
{
    global $user;
    $uid = $user->uid;
    
    $Block['title'] = "Media";
    if($op == 'view' || $op == null){
        $content = "TEXT";
        $Block['body'] = content_box($Block['title'], $content);
    }
    
    return $Block;
}
function top_ranked_people($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $uid = $user->uid;
    
    $Block['title'] = 'Top Ranked';
    if($op == 'view' || $op == null){
        $content = "DSA";
        if ($write) {
          $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
        }
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
     return $Block;
}
function submit_information($op = null){
    global $user;
    $uid = $user->uid;
    
    $Block['title'] =  'Submit Information'; 
    if($op == 'view' || $op == '') $Block['body']  =   "<div class='pagelink'><h3>Submit Information</h3>" . l('Add information', 'account',array('query'=>'a=invite_people')) . "</div><br>";
    return $Block;
}
function industry_topics($op = null)
{
    $Block['title'] =  'Industry Topics'; 
    if($op == 'view' || $op == ''){
        $content = '<tr><td>link 1</td><td>Health Care</td></tr>'.
                    '<tr><td>link 2</td><td>Retail</td></tr>'.
                    '<tr><td>link 3</td><td>Technology</td></tr>';
        
        $html = <<< END
        <table class='two-column-a'>
            $content
        </table>
END;

        $Block['body']  =  sidebar_box($Block['title'], $html);
    }
    
    return $Block;
}
function management_topics($op = null)
{
    $Block['title'] =  'Management Topics'; 
    if($op == 'view' || $op == ''){
        $content = '<tr><td>link 1</td><td>Health Care</td></tr>'.
                    '<tr><td>link 2</td><td>Retail</td></tr>'.
                    '<tr><td>link 3</td><td>Technology</td></tr>';
        
        $html = <<< END
        <table class='two-column-a'>
            $content
        </table>
END;

        $Block['body']  =  sidebar_box($Block['title'], $html);
    }
    
    return $Block;
}
/* EOF */
