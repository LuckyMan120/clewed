<?php
// $maenna_page['content'] = array('info_list','related_list');

global $redirect;
$redirect = rebuild_url(array('tab', 'page'));


function pro_tabs()
{
	global $base_url;
	$page = sget($_REQUEST, 'page');
	$type = sget($_REQUEST, 'type');
	$id = sget($_REQUEST, 'id');
	
	
	if($page == 'myinsights' && $type == '')
		$active = ' first  active-trail active ';  
	else if($page == 'myinsights' && $type == 'registered')
		$active1 = ' first  active-trail active ';
	
	

	if($page == 'myinsights')
				{	
	$content = '<div class="account-section-tabs">
            <ul>

            </ul>
        </div>';
		
	}
		
		//$Block['body'] = content_box($Block['title'], $content);
   echo $content;
}

function myfollowing($op = null)
{
	global $user;
    global $AccessObj; 

	$user_type = $AccessObj->user_type;
	
    $editorid = $user->uid; // the currently logged in user, print_r this object to see all data available
	
    $companyid = sget($_REQUEST, 'id');

    $content = '';
    global $redirect;
    $data_type = 'discussions';
    $panel  = 'multi';
    $section=__FUNCTION__;

	$page = sget($_REQUEST, 'page');
	$type = sget($_REQUEST, 'type');
	$id = sget($_REQUEST, 'id');
	
	
	//if($page == 'myinsights' && $type == '')
		$title = 'FOLLOWING';  
	//else if($page == 'myinsights' && $type == 'registered')
		//$title = 'REGISTERED';

		$Block['title'] = $title;

		
		
    if (sget($_REQUEST, 'name')) {
	    $Block['title'] = sget($_REQUEST, 'name');
		$fname = sget($_REQUEST, 'name');
    }

	ob_start();
        $url = "http://" .$_SERVER['HTTP_HOST'];
	$editorname = $user->name;
	require_once("myfollowing.php");
	$content = ob_get_contents();
	ob_end_clean();
	
    //$content = sget($_REQUEST, 'file');

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}


        
?>
