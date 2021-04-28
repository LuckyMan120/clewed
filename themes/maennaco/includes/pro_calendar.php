<?php
error_reporting(0);
global $base_url;
include('dbcon.php');
global $user;
global $AccessObj;
$usertype = $AccessObj->user_type;
$tab = sget($_REQUEST, 'tab');
//$user_id = $user->uid;
if($_REQUEST['id'] == '')
{
	$user_id = $user->uid;
	$pagename = 'insights';
	$uid = '';
}
else
{
	$user_id = $_REQUEST['id'];
	$pagename = '';
	$uid = $_REQUEST['id'];
}
if ($tab != 'insights') {
?>
<link rel='stylesheet' type='text/css' href='<?php echo $base_url; ?>/themes/maennaco/fullcalendar.css' />
<script type='text/javascript' src='<?php echo $base_url; ?>/themes/maennaco/fullcalendar.js'></script>
<script type='text/javascript'>

	$(document).ready(function() {
	
		$('#calendar').fullCalendar({
		header: {
		   left: 'prev ',
		   center: 'title',
		   right: 'next'
	   },
			editable: false,
			
			events: "<?php echo $base_url; ?>/themes/maennaco/includes/json-events.php?user_id=<?php echo $user_id;?>&pagename=<?php echo $pagename; ?>",
            eventRender: function (event, element, view) {
                if (event.start.getMonth() != view.start.getMonth())
                    return false;
            },
			
			eventDrop: function(event, delta) {
				alert(event.title + ' was moved ' + delta + ' days\n' +
					'(should probably update your database)');
			},
			
			loading: function(bool) {
				if (bool) $('#loading').show();
				else $('#loading').hide();
				
				$('.fc-event-hori').css({height:'35px',top:'-=24'});
			}
			
		});
		
	});

</script>
<style type='text/css'>	
	#loading {
		position: absolute;
		top: 5px;
		right: 5px;
		}

#calendar { background-color: #fafbfb; width: 265px; margin: 0px auto 20px -10px; }


</style>


<div id='loading' style='display:none'></div>
<!-- <div id='calendar'></div> -->
    <?php } ?>

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
/*    else {
        rel_str = 'sort';
        var regex = new RegExp("&?" + rel_str + "=([^&]$|[^&]*)","i");

        location.href = location.href.replace(regex, "");
    }*/

		});
$(".month").livequery("click",function (e) {
			e.preventDefault();
    $('#months').toggle();
/*    else {
        rel_str = 'sortmonth';
        var regex = new RegExp("&?" + rel_str + "=([^&]$|[^&]*)","i");

        location.href = location.href.replace(regex, "");
    }*/

});
$(".prof").livequery("click",function (e) {
			e.preventDefault();
		$('#professional').toggle();					
		});
$("#myinsights").livequery("click",function (e) {
e.preventDefault();
		$('#showinsights').toggle();					
		});
</script>
<div class="categories">
<!-- <div class="box_title shaded_title"><span id="rghttd_title">Filter</span></div>
    <?php if ($usertype == 'super' || $usertype == 'admin') {

        echo '<a href="/account?tab=insights&ftype=live"><strong style="color:';
        if ($_REQUEST['ftype'] == 'live') echo '#00aad6;'; else echo '#686b72;';
        echo 'font-style:italic;">Live</strong></a><br>
    <a href="/account?tab=insights&ftype=progressing"><strong style="color:';
        if ($_REQUEST['ftype'] == 'progressing') echo '#00aad6;'; else echo '#686b72;';
        echo 'font-style:italic;">Progressing</strong></a>';
    } ?>
    <div class="Categories"><div class="left_border"><a class="category" style="cursor:pointer;"><strong style="font-style:italic;">Categories</strong></a></span></div> -->
<!-- <div id="categories" style="<?php /*if (!isset($_GET['sort']) || $_GET['sort'] == "") {echo "display:none;";} */?>">
<div class="Categories_2">
<ul>
    <style>
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
    </style>
<?php


if($tab == 'insights')

{
	$categories = mysql_query("SELECT * FROM maenna_professional group by tags") or die(mysql_error());
}
else
{
    $cat_sql = "SELECT * FROM maenna_professional where postedby = %d group by tags";
	$categories = db_query($cat_sql,array($user_id));

}
while($resCategories = mysql_fetch_array($categories))
{
    if($resCategories['tags'] == 'Choose a Category' || $resCategories['tags'] == '') continue; ?><li<?php if ($_GET['sort'] == $resCategories['tags']) { echo ' style="background-color:#e3eef2;"'; } ?>>
	<?php if($tab =='insights'):?>
  <a href="<?=$base_url?>/account?tab=insights&sort=<?=$resCategories['tags']?><?=empty($_REQUEST['sortmonth'])? '' : '&sortmonth='.$_REQUEST['sortmonth'] ?>"><?=$resCategories['tags']?></a>
  <?php else: ?>
  <a href="<?=$base_url?>/account?tab=professionals&page=pro_detail&id=<?=$resCategories['postedby']?>&section=pro_industry_view&type=discussion&sort=<?=$resCategories['tags']?><?=empty($_REQUEST['sortmonth'])? '' : '&sortmonth='.$_REQUEST['sortmonth'] ?>"><?=$resCategories['tags']?></a>
  <?php endif; ?>
    </li>
<?php }
?></ul></div> -->

</div>
<!-- <div class="left_border"><span><a class="month" style="cursor:pointer;"><strong style="font-style:italic;">Month</strong></a></span></div>
<div id="months" style="<?php /*if (!isset($_GET['sortmonth']) || $_GET['sortmonth'] == "") {echo "display:none;";} */?>"> -->
<!-- <div class="month_2">
<ul>
<?php 
$months = array('01' => 'January', '02' => 'February', '03' => 'March', '04' => 'April', '05' => 'May', '06' => 'June', '07' => 'July', '08' => 'August', '09' => 'September', '10' => 'October', '11' => 'November', '12' => 'December');
foreach($months as $key=> $value) 
{
?><li<?php if ($_GET['sortmonth'] == $key) { echo ' style="background-color:#e3eef2;"'; } ?>>

  	<?php if($tab =='insights'):?>
  	<a href="<?=$base_url?>/account?tab=insights&sortmonth=<?=$key?><?=empty($_REQUEST['sort'])? '' : '&sort='.$_REQUEST['sort'] ?>"><?=$value?></a></li>
    <?php else: ?>
    <a href="<?=$base_url?>/account?tab=professionals&page=pro_detail&id=<?=$uid?>&section=pro_industry_view&type=discussion&sortmonth=<?=$key?><?=empty($_REQUEST['sort'])? '' : '&sort='.$_REQUEST['sort'] ?>"><?=$value?></a></li>
    <?php endif; ?>

<?php } ?></ul></div> -->
</div>
<?php /*?><?php
if($pagename == 'insights')
{
?>
<div class="left_border"><span><a class="prof" style="cursor:pointer;"><strong style="font-style:italic;">Professional</strong></a></span></div>
<div id="professional" style="display:none;">
<div class="Categories_2">
<ul>
<?php

$prof_user = mysql_query("SELECT postedby FROM maenna_professional group by postedby") or die(mysql_error());

while($resprof = mysql_fetch_array($prof_user))
{

$sql_expertise = mysql_query ("SELECT * FROM  `maenna_people` WHERE `pid` = '" .$resprof['postedby'] ."'");
$sql_exp_result = mysql_fetch_array($sql_expertise);
$P_username = ucfirst($sql_exp_result['firstname']) . ' ' . ucfirst($sql_exp_result['lastname']);

if($sql_exp_result['firstname'] == '')
	$P_username = $resprof['username'];
else
	$P_username = $P_username;

?><li>
	<a href="<?=$base_url?>/account?tab=professionals&page=pro_detail&id=<?=$resprof['postedby']?>&section=pro_industry_view&type=discussion"><?=$P_username?></a></li>
<?php }
?></ul></div>

</div>
<?php } ?><?php * /?>

</div>

*/ ?>