<?php
error_reporting(0);
global $base_url;
include('dbcon.php');

$user_id = $_GET['user_id'];
$pagename = $_GET['pagename'];
if($pagename == 'insights') {
  $url = $base_url . "/account?tab=insights&sortdate=";
} else {
  $url = $base_url . "/account?tab=professionals&page=pro_detail&id=" . $user_id . "&section=pro_industry_view&type=discussion&sortdate=";
}



// Get Schedule dates from -  'maenna_professional' table

if($pagename == 'insights') 
{
	$sql = "SELECT DISTINCT id,datetime FROM maenna_professional WHERE approve_status = 1";
}
else
{
    $user_id = mysql_real_escape_string($user_id);
    $sql = "SELECT DISTINCT datetime FROM maenna_professional WHERE postedby = " . (int) $user_id . " AND approve_status = 1";
}
$result = mysql_query($sql);

$backup_array = array();
while($Row = mysql_fetch_array($result))
{

	$date = date('Y-m-d',$Row['datetime']);	
	$day = date('d',$Row['datetime']);	
	if( ! in_array($date, $backup_array))
	{
        $parts = explode('/', $_SERVER['HTTP_REFERER']);
        $ref = explode("?",end($parts));
        $ref = $ref[0];
        if ($ref == 'insights') $url = '/insights?sortdate='.$date; else $url = $url . $Row['datetime'];
		$array[] = array('id' => "",
						'title' => "",		  
						'start' => $date,
						'url' => $url,
						'day'=>$day);
	}
	$backup_array[] = $date;
}
echo json_encode($array);
?>
