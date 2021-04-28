<?php



if ($_REQUEST['type'] == 'pro_file_post') {
    include "dbcon.php";

    	if (md5($_REQUEST['pid']."kyarata75") === $_REQUEST['m']) {

            mysql_query("delete from pro_wall_posts where pid ='".((int) $_REQUEST['pid'])."'") or die(mysql_error());
            die(json_encode(array("status"=>"success","display"=> "Post successfully deleted")));

        }

    else die(json_encode(array("status"=>"error","display"=> "Authentication problem")));
}

if ($_REQUEST['type'] == 'pro_file_comment') {
    include "dbcon.php";

    if (md5($_REQUEST['cid']."kyarata75") === $_REQUEST['m']) {

        mysql_query("delete from pro_wall_posts_comments where cid ='".((int) $_REQUEST['cid'])."'") or die(mysql_error());
        die(json_encode(array("status"=>"success","display"=> "Post successfully deleted")));

    }

    else die(json_encode(array("status"=>"error","display"=> "Authentication problem")));
}

chdir('../../../');
require_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

$return = menu_execute_active_handler();

if (is_int($return)) {
    switch ($return) {
        case MENU_NOT_FOUND:
            drupal_not_found();
            break;
        case MENU_ACCESS_DENIED:
            drupal_access_denied();
            break;
        case MENU_SITE_OFFLINE:
            drupal_site_offline();
            break;
    }
}
	
	if (md5($_REQUEST['c_id'].$_REQUEST['u']."kyarata75") === $_REQUEST['m'])
	{
	
	mysql_query("delete from wall_posts_comments where c_id ='".((int) $_REQUEST['c_id'])."' AND user ='".mysql_real_escape_string($_REQUEST['u'])."'");
	}
?>