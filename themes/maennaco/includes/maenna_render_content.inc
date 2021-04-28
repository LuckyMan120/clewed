<?php
defined("__ACCOUNT__") or die("no direct access allowed");

?>


<?php
    if(isset($maenna_page['section_tab']))
    {
        $section_tab = $maenna_page['section_tab'];
        if(function_exists($section_tab)){
            echo $section_tab();
        }else{
            echo "Missing function: " .$section_tab;
        }
        echo "</div>";
    }
?>
<table cellpadding=0 cellspacing=0 border=0 class='account-table'>
    <tr>
        <?php if(count($maenna_page['left']) > 0) maenna_render_left($maenna_page['left']); ?>
        
        <?php
            maenna_render_content($maenna_page['content']);
        ?>
        
        <?php if(count($maenna_page['right']) > 0) maenna_render_right($maenna_page['right']); ?>
    </tr>
</table>

<?php


function maenna_render_left($Array)
{
    $content = '';
    $Temp = '';
    echo "<td class='left-td'>";
    foreach($Array as $function_name)
    {
       if(function_exists($function_name))
        {
            $Temp = $function_name();
            $content .= $Temp['body'];
        }
    }
    echo $content;
    echo "</td><td style='width:30px;border:none'></td>";
}

function maenna_render_content($Array = null)
{   
    echo "<td class='act-content'>";
    echo theme_status_messages();
    if (isset($_SESSION['messages']) && count($_SESSION['messages']) > 0 && 0 ) {
                echo '<div class="messages"><ul>';
                foreach($_SESSION['messages'] as $type=>$messages) {
                    echo "<li>". $messages."</li>";
                   
                }
                echo '</ul></div>';
        unset($_SESSION["messages"]);
    }
    
    $op = '';
    $maenna_content = array();
    $maenna_content['title'] = '';
    $maenna_content['body'] = '';
    $Temp = '';
    if(isset($Array) && is_array($Array))
    {
        $maenna_content['title']  = 'My Activity';
        foreach($Array as $k => $function_name)
        {
            if(function_exists($function_name) && function_allowed($function_name) )
            {
                if($k == 0)
                {
                    $op = (isset($_REQUEST['o']) ) ? check_plain($_REQUEST['o']) : "view";
                    $maenna_content = $function_name($op);
                }else
                {
                    $Temp = $function_name('view');
                    $maenna_content['body'] .= $Temp['body'];
                }
            }else{
                drupal_set_message("Page not found.",'warning');
            }
        }
    }
 // echo "<h3>" . $maenna_content['title']  . "</h3><br>";
    echo theme_status_messages();
    echo  isset($maenna_content['body']) ? $maenna_content['body'] : '';
    echo "</td>";
}


function maenna_render_right($Array = null)
{
    
    $content = '';
    $Temp = '';
    echo "<td style='width:30px;border:none;'></td><td class='right-td'>";
    foreach($Array as $function_name)
    {
        if(function_exists($function_name))
        {
            
            $Temp = $function_name();
            $content .= $Temp['body'];
        }
    }
    echo $content;
    echo "</td>";
}


/* EOF */


