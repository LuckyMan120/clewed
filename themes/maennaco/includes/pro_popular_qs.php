<?php
error_reporting(0);
global $base_url;
include('dbcon.php');
global $user;
$user_id = $user->uid;

$pro_id = sget($_REQUEST, 'pro_id');

$pro_sql = "SELECT PWP.pid,(SELECT COUNT(*) FROM like_discussion_posts WHERE post_id = PWP.pid) as post_count, PWP.post
            FROM pro_wall_posts AS PWP
            WHERE PWP.flag = 'q' AND PWP.pro_id = %d
            ORDER BY post_count DESC
            LIMIT 5";
$pro_result = db_query($pro_sql, array($pro_id));
$count = mysql_num_rows($pro_result);
//$pro_row = db_fetch_array($pro_result);
//echo "<pre>"; print_r($pro_row); echo "</pre>";exit;
?>
<div class="month_2" style="border-bottom:none;margin-top:10px !important;">
<?php
if($count > 0)
{
?>
<ul>
<?php
	while($pro_row = db_fetch_array($pro_result))
	{ 
		$post = $pro_row['post'];
		$length = strlen($pro_row['post']);
		if($length > 30)
		{
			$post = substr($pro_row['post'], 0, 30) . '...';
		}
?>
	<li style="cursor:pointer;" class="pop_qs_" rel="<?=$pro_row['pid'];?>">
		<?php echo ucfirst($post);?>
	</li>
<?php } ?>
</ul>
<?php } 
else {
	/*echo "Please come back later.";*/
}
?>
</div>