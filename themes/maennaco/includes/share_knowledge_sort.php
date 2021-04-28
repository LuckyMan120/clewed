<?php
error_reporting(0);
global $base_url;
include('dbcon.php');
global $user;
//$user_id = $user->uid;

$user_id = $_REQUEST['data_id'];
$pagename = '';
$uid = $_REQUEST['data_id'];

?>
<link rel='stylesheet' type='text/css' href='<?php echo $base_url; ?>/themes/maennaco/fullcalendar.css' />
<style type='text/css'>	
	#loading {
		position: absolute;
		top: 5px;
		right: 5px;
		}

#calendar { width: 265px; margin: 0px auto 20px -10px; }
.Categories{ color:#686b72; }	
.Categories a{ color:#686b72; }
.Categories a strong{ color:#686b72;  }

.Categories a:hover{ color:#686b72; text-decoration:none!important; }			
		
.Categories_2{ margin:0px; padding:0px 0px 20px 0px; border-bottom: 1px #ccc solid; }
.Categories_2 ul{ margin:0px; padding:0px}		
.Categories_2 ul li{ margin:0px 0px 0px 0px; padding:5px 6px; list-style:none; background:none;}
.Categories_2 ul li a{ margin:0px; padding:0px; list-style:none; color:#444;}	
.Categories_2 ul li a:hover{ text-decoration:none; color:#444!important;}		
.Categories_2 ul li:hover{ margin:0px; padding:5px 6px; list-style:none; background:#e3eef2!important;color:#444; text-decoration:none!important;}

.month_2{ margin:0px; padding:0px 0px 20px 0px; border-bottom: 1px #ccc solid;}
.month_2 ul{ margin:0px; padding:0px}		
.month_2 ul li{ margin:0px 0px 0px 0px; padding:5px 6px; list-style:none; background:none;}
.month_2 ul li a{ margin:0px; padding:0px; list-style:none; color:#444;}	
.month_2 ul li a:hover{ text-decoration:none; color:#444!important}		
.month_2 ul li:hover{ margin:0px; padding:5px 6px; list-style:none; background:#e3eef2!important;color:#444; text-decoration:none!important; }


.categories .box_title {
    border-bottom: 2px solid #E8E8E8;
    color: #284B54;
    font-family: 'Lato Bold';
    font-size: 14px;
    font-weight: bold;
    height: 30px;
    text-transform: uppercase;
}
</style>


<div id='loading' style='display:none'></div>
<div id='calendar'></div>

<!--  Code for Twitter, Facebook,  LinkedIn >
<div style="position:relative;margin-left:auto; margin-right:auto; width:263px;height:40px;">
<div style="position:absolute;width:100px;top:0;left:0px;margin:0;padding:0;">
<a href="https://twitter.com/share" class="twitter-share-button" data-count="none">Tweet</a>
<script>!function(d,s,id){var js,fjs=d.getElementsByTagName(s)[0];if(!d.getElementById(id)){js=d.createElement(s);js.id=id;js.src="//platform.twitter.com/widgets.js";fjs.parentNode.insertBefore(js,fjs);}}(document,"script","twitter-wjs");</script></div>
<div style="position:absolute;width:100px;top:10px;left:80px;"><script src="//platform.linkedin.com/in.js" type="text/javascript"></script>
<script type="IN/Share" data-url="www.maennaco.com" data-counter="right"></script></div>
<div style="position:absolute;width:100px;top:10px;left:150px;"><iframe src="//www.facebook.com/plugins/like.php?href=http%3A%2F%2Fwww.maennaco.com&amp;send=false&amp;layout=button_count&amp;width=450&amp;show_faces=false&amp;action=like&amp;colorscheme=light&amp;font&amp;height=21&amp;appId=267809316574109" scrolling="no" frameborder="0" style=" margin-left:30px;border:none; overflow:hidden; width:80px; height:21px;" allowTransparency="true"></iframe></div> 
</div>
<!--  End of Twitter, Facebook,  LinkedIn -->
	  
<script type="text/javascript" src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
<script type="text/javascript">
$(".category").livequery("click",function (e) {
			e.preventDefault();
		$('#categories').toggle();					
		});
$(".month").livequery("click",function (e) {
			e.preventDefault();
		$('#months').toggle();					
		});
$(".like").livequery("click",function (e) {
			e.preventDefault();
		$('#likes').toggle();					
		});
$("#myinsights").livequery("click",function (e) {
		$('#showinsights').show();					
		});
</script>
<?php /*?><?php
if($_REQUEST['analysis_id'] != '')
{
$show_like_unlike_sql = mysql_query ("SELECT * FROM  `like_analysis` WHERE prof_id = '" . $user_id ."' and user_id = '" . $user->uid ."'");
$show_like_unlike_rows = mysql_fetch_array($show_like_unlike_sql);
$show_like_unlike_count = mysql_num_rows($show_like_unlike_sql);

$count_result3 = mysql_query ("SELECT * FROM  `like_analysis` WHERE prof_id = '" . $user_id ."'");
$count_row3 = mysql_fetch_array($count_result3);
$count_likepost1 = mysql_num_rows($count_result3);
?>
    <?php if($show_like_unlike_count >= 1) { ?>
    <div style=' text-align: center;font-weight: bold;padding:0px 0px 0px 0px;' id="likepost1<?=$user_id?>" ><a href="javascript:void(0);" style="cursor:pointer;" onClick="like_analysis('unlike', '<?=$user_id?>','<?=$user->uid?>');">Unlike </a><?php echo $count_likepost1; ?></div>
    <?php } else {?>
    <div style=' text-align: center;font-weight: bold;padding:0px 0px 0px 0px;' id="likepost1<?=$row['dataid']?>" ><a href="javascript:void(0);" style="cursor:pointer;" onClick="like_analysis('like', '<?=$user_id?>','<?=$user->uid?>');">Like </a><?php echo $count_likepost1; ?></div>
    <?php } ?>
<?php } ?><?php */?>
<div class="categories">
<div class="box_title shaded_title" style="text-transform:capitalize; font-family: Lato Bold Italic !important;font-weight:normal;"><?php echo strtolower('Filter'); ?></div>
<div class="Categories"><div class="left_border" style="background:#ebecec;margin-bottom:2px;"><a class="category" style="cursor:pointer;"><strong style="font-style:italic;">Categories</strong></a></span></div>
<div id="categories" style="display:none;">
<div class="Categories_2">
<ul>
<?php
$tab = sget($_REQUEST, 'tab');	
$categories = mysql_query("SELECT tags FROM maenna_company_data where deleted != 1 group by tags") or die(mysql_error());
while($resCategories = mysql_fetch_array($categories))
{ ?><li>
	<a href="<?=$base_url?>/account?tab=companies&page=company_detail&id=<?=$_REQUEST['id']?>&mtab=share_knowledge&sort=<?=$resCategories['tags']?>"><?=$resCategories['tags']?></a></li>
<?php }
?></ul></div>

</div>
<div class="left_border" style="background:#ebecec;margin-bottom:2px;"><span><a class="month" style="cursor:pointer;"><strong style="font-style:italic;">Month</strong></a></span></div>
<div id="months" style="display:none;">

<div class="month_2">
<ul>
<?php 
$months = array('01' => 'January', '02' => 'February', '03' => 'March', '04' => 'April', '05' => 'May', '06' => 'June', '07' => 'July', '08' => 'August', '09' => 'September', '10' => 'October', '11' => 'November', '12' => 'December');
foreach($months as $key=> $value) 
{
?><li>
<a href="<?=$base_url?>/account?tab=companies&page=company_detail&id=<?=$_REQUEST['id']?>&mtab=share_knowledge&sortmonth=<?=$key?>"><?=$value?></a></li>
<?php } ?></ul></div>
</div>

<div class="left_border" style="background:#ebecec;margin-bottom:2px;"><span><a class="likess" href="<?=$base_url?>/account?tab=companies&page=company_detail&id=<?=$_REQUEST['id']?>&mtab=share_knowledge&sortby=likes" style="cursor:pointer;"><strong style="font-style:italic;">Likes</strong></a></span></div>
<?php /*?><div id="likes" style="display:none;">
<div class="month_2">
<ul>
<?php

$sql_likes = mysql_query("select count(*) as count_likes,prof_id, com.data_value from like_analysis, maenna_company_data com WHERE com.dataid = like_analysis.prof_id group by prof_id order by count(*) desc limit 5");

while($likes = mysql_fetch_array($sql_likes))
{ ?><li>
	<a href="<?=$base_url?>/account?tab=companies&page=company_detail&id=<?=$_REQUEST['id']?>&mtab=summary&type=details&analysis_id=<?=$likes['prof_id']?>"><?=$likes['data_value']?></a></li>
<?php } ?></ul></div>
</div><?php */?>
</div>

