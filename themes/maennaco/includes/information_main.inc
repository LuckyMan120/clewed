<?php
// $maenna_page['content'] = array('info_list','related_list');


function infoTabs($Tabs , $tab = ''){
    $Block['title']  = 'Information';
    $content = '';
    $pageTab = sget($_REQUEST, 'tab');
    
    $Tabs = array('insights', 'postings', 'events','questionnaire');
    for($i = 0; $i < count($Tabs); $i++){
        $active = ''; $class = '';
        if($Tabs[$i] == $pageTab)$active = ' active-trail active';
        if($i == 0)$class = 'first';
        elseif($i == (count($Tabs) - 1))$class = 'last';
        $title = $Tabs[$i];
       /// var_dump(($pageTab == 'questionnaire'));
        if($Tabs[$i] == 'questionnaire') 
            $content .= "\n<li class=' $class $active '><a href='/account?a=information&tab=".$Tabs[$i]."&panel=questionnaire&target=company' class='$active'>".strtoupper($title)."</a></li>";
        else
            $content .= "\n<li class=' $class $active '><a href='/account?a=information&tab=".$Tabs[$i]."' class='$active'>".strtoupper($title)."</a></li>";
    }
    $content = <<< END
    <div  class='about-menu-div' style='margin-top:15px;'>
        <ul class="top-nav-menu-item">
            $content
        </ul>
    </div>
END;
    $Block['body'] = content_box($Block['title'], $content);
return $Block;
}


        
?>
