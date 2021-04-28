	<?php
	error_reporting (E_ALL ^ E_NOTICE);
	date_default_timezone_set('EST');

	function nameToId($name) {

	$q = mysql_query("SELECT uid FROM users WHERE name = '".mysql_real_escape_string($name)."' LIMIT 1") or die(mysql_error());
	$r = mysql_fetch_array($q);
	return $r['uid'];

	}

	function getUserType($uid) {

	$q = mysql_query("SELECT pid FROM maenna_people WHERE pid = '".((int) $uid)."' ");

	if (mysql_num_rows($q) > 0 ) return 'people';

	else {
		$q1 = mysql_query("SELECT companyid FROM maenna_company WHERE companyid = '".((int) $uid)."' ");
		if (mysql_num_rows($q1) > 0 ) return 'company';
		else return 'admin';
		}
}
	
	function ifAdmin ($uname) {
	 
	 $result = mysql_query("SELECT *  FROM users u WHERE u.name = '".mysql_real_escape_string($uname)."' AND EXISTS (SELECT * FROM users_roles WHERE uid = u.uid AND rid IN (SELECT rid FROM role WHERE name = 'Super admin' OR name = 'Maennaco admin'))");
	 
	 if (mysql_num_rows($result) > 0 ) {return true;} else return false;
	}
	
	function printComments($resultSet) {
	 	while ($row = mysql_fetch_array($resultSet))
	{
	 global $user;
		$comments = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS CommentTimeSpent FROM wall_posts_comments where post_id = ".$row['p_id']." order by c_id asc");		?>
	   <div class="friends_area" name="<?=$row['user'];?>" alt="<?php  echo md5($row['user']."kyarata75") ?>" id="record-<?php  echo $row['p_id']?>">
<?php

$crId = nameToId($row['user']);
$uType = getUserType($crId);

if ($uType == 'people' || $uType == 'admin') {

		//Get user gender
		$q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = ".((int) $pro_uid));
		$gender_tmp = mysql_fetch_array($q1);
		$gender = $gender_tmp['gender'];

if (file_exists('sites/default/images/profiles/50x50/'.$crId.'.jpg')) { $avatar = 'sites/default/images/profiles/50x50/'.$crId.'.jpg';} 
		  else {
				if ($gender == 'm' || $gender == '') { $avatar =' /themes/maennaco/images/prof-avatar-male.png';}

					else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
			}
}
else if ($uType == 'company')  {

//Get cmp_role		
		$q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $crId") or die(mysql_error());
		$cmp_role_tmp = mysql_fetch_array($q1);
		$cmp_role = $cmp_role_tmp['company_type'];
		 //Check if user have a profile picture
		  if (file_exists('sites/default/images/company/50x50/'.$crId.'.jpg')) {$avatar = 'sites/default/images/company/50x50/'.$crId.'.jpg';} 
		  else	
				if ($cmp_role == 'service') $avatar =' /themes/maennaco/images/cmp-avatar-service.png';
				else $avatar =' /themes/maennaco/images/cmp-avatar-product.png';

}
                        
echo "<img src=".$avatar." style=\"float:left; margin-top:5px; margin-right:5px; width:40px; height:40px;\">";

?>

		   <label style="width:90%; float:left" class="name">

		   <span style="color:#006274;"><b><?php if (ifAdmin($row['f_name'])) {echo "Clewed";} else echo $row['f_name']; echo "</b>
		   <span style='color:#666;font-style:italic'>&nbsp;shares a new idea:</span>";?></span><?php

			if($row['user'] == $user->name || ifAdmin($user->name))
 { ?>
		  	<a href="#" style="float:none;" id="remove_id<?php  echo $row['p_id']?>" alt="<?php  echo md5($row['p_id'].$row['user']."kyarata75") ?>" name="<?=$row['user'];?>" class="delete tool"> Remove</a>

 <? } ?>

			<br>
		   <em><?php  echo nl2br(replace_email($row['post']));?></em>
		   <br clear="all" />
		
		   <span>
		   <?php  
		   
		    /* echo strtotime($row['date_created'],"Y-m-d H:i:s");
   		    
		    $days = floor($row['date_created'] / (60 * 60 * 24));
			$remainder = $row['date_created'] % (60 * 60 * 24);
			$hours = floor($remainder / (60 * 60));
			$remainder = $remainder % (60 * 60);
			$minutes = floor($remainder / 60);
			$seconds = $remainder % 60;
			
			if($days > 0)
			echo date('F d Y H:i:s', $row['date_created']);
			elseif($days == 0 && $hours == 0 && $minutes == 0)
			echo "few seconds ago";		
			elseif($days == 0 && $hours == 0)
			echo $minutes.' minutes ago';
			else
			echo "few seconds ago";	
			*/
			echo ago($row['date_created']);
			
		   ?>
		   </span></label>
		  <!-- <a href="javascript: void(0)" id="post_id<?php  echo $row['p_id']?>" class="showCommentBox">Comments</a>
-->
		   
		   
			
		    <br clear="all" />
			<div id="CommentPosted<?php  echo $row['p_id']?>">
				<?php
				$comment_num_row = mysql_num_rows(@$comments);
				if($comment_num_row > 0)
				{
					while ($rows = mysql_fetch_array($comments))
					{
						$days2 = floor($rows['date_created'] / (60 * 60 * 24));
						$remainder = $rows['date_created'] % (60 * 60 * 24);
						$hours = floor($remainder / (60 * 60));
						$remainder = $remainder % (60 * 60);
						$minutes = floor($remainder / 60);
						$seconds = $remainder % 60;						
						?>
					<div class="commentPanel" id="comment-<?php  echo $rows['c_id'];?>" align="left">
<?php
$crId = nameToId($rows['user']);
$uType = getUserType($crId);

if ($uType == 'people' || $uType == 'admin') {

		//Get user gender
		$q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = ".((int) $pro_uid));
		$gender_tmp = mysql_fetch_array($q1);
		$gender = $gender_tmp['gender'];

if (file_exists('sites/default/images/profiles/50x50/'.$crId.'.jpg')) { $avatar = 'sites/default/images/profiles/50x50/'.$crId.'.jpg';} 
		  else {
				if ($gender == 'm' || $gender == '') { $avatar =' /themes/maennaco/images/prof-avatar-male.png';}

					else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
			}
}
else if ($uType == 'company')  {

//Get cmp_role		
		$q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $crId") or die(mysql_error());
		$cmp_role_tmp = mysql_fetch_array($q1);
		$cmp_role = $cmp_role_tmp['company_type'];
		 //Check if user have a profile picture
		  if (file_exists('sites/default/images/company/50x50/'.$crId.'.jpg')) {$avatar = 'sites/default/images/company/50x50/'.$crId.'.jpg';} 
		  else	
				if ($cmp_role == 'service') $avatar =' /themes/maennaco/images/cmp-avatar-service.png';
				else $avatar =' /themes/maennaco/images/cmp-avatar-product.png';

}
                        
echo "<img src=".$avatar." style=\"float:left;continues discussion margin-top:5px;margin-right:5px; width:35px; height:35px;\">";

?>
						<label style="width:85%;" class="postedComments">
							<span><b><?php if (ifAdmin($rows['user'])) {echo "clewed";} else  echo $rows['f_name'];
							echo "</b><span style='color:#666;font-style:italic;font-size:11px;'>&nbsp;continues discussion:</span>";?></span>
						
							<?php   echo nl2br(replace_email($rows['comments']));?>
						
						<br>
						<span style="display:inline-block;width:90px;margin-left:0px; color:#666666; font-size:11px">
						<?php
						/*
						if($days2 > 0)
						echo date('F d Y H:i:s', $rows['date_created']);
						elseif($days2 == 0 && $hours == 0 && $minutes == 0)
						echo "few seconds ago";		
						elseif($days2 == 0 && $hours == 0)
						echo $minutes.' minutes ago';
						else
			echo "few seconds ago";	
			*/
						echo ago($rows['date_created']);
						?>
						</span>
						<?php
						
						if($rows['user'] == $user->name ||  ifAdmin($user->name)){?>
						&nbsp;&nbsp;<a href="#" id="CID-<?php  echo $rows['c_id'];?>" alt="<?php  echo md5($rows['c_id'].$rows['user']."kyarata75") ?>" name="<?=$rows['user'];?>" class="c_delete tool">Delete</a>
						</label><?php
						}?>
					</div>
					<?php
					}?>				
					<?php
				}?>
			</div>
			<div class="commentBox" align="right"
			     id="commentBox-<?php  echo $row['p_id'];?>" <?php echo (($comment_num_row) ? '' :'style=""')?> name="<?php echo $user->name; ?>" alt="<?php  echo md5($user->name."kyarata75") ?>" >
				<label id="record-<?php  echo $row['p_id'];?>">
					<textarea style="width:540px;" class="commentMark" id="commentMark-<?php  echo $row['p_id'];?>" name="commentMark" cols="120"></textarea>
				</label>
				<br clear="all" />
				<a id="SubmitComment" style="cursor:pointer;" class="tool comment" comBox="commentBox-<?=$row['p_id'];?>"> REPLY</a>
			</div>
	<?php
        }
	}
	 
	

	function checkValues($value)
	{
		 $value = trim($value);
		 
		if (get_magic_quotes_gpc()) {
			$value = stripslashes($value);
		}
		
		 $value = strtr($value,array_flip(get_html_translation_table(HTML_ENTITIES)));
		
		 $value = strip_tags($value);
		$value = mysql_real_escape_string($value);
		$value = htmlspecialchars ($value);
		return $value;
		
	}	
	
	function clickable_link($text = '')
	{
		$text = preg_replace('#(script|about|applet|activex|chrome):#is', "\\1:", $text);
		$ret = ' ' . $text;
		$ret = preg_replace("#(^|[\n ])([\w]+?://[\w\#$%&~/.\-;:=,?@\[\]+]*)#is", "\\1<a href=\"\\2\" target=\"_blank\">\\2</a>", $ret);
		
		$ret = preg_replace("#(^|[\n ])((www|ftp)\.[\w\#$%&~/.\-;:=,?@\[\]+]*)#is", "\\1<a href=\"http://\\2\" target=\"_blank\">\\2</a>", $ret);
		$ret = preg_replace("#(^|[\n ])([a-z0-9&\-_.]+?)@([\w\-]+\.([\w\-\.]+\.)*[\w]+)#i", "\\1<a href=\"mailto:\\2@\\3\">\\2@\\3</a>", $ret);
		$ret = substr($ret, 1);
		return $ret;
	}
		function ago($time)
		{
		   $periods = array("second", "minute", "hour", "day", "week", "month", "year", "decade");
		   $lengths = array("60","60","24","7","4.35","12","10");

		   $now = time();

			   $difference     = $now - $time;
			   $tense         = "ago";

		   for($j = 0; $difference >= $lengths[$j] && $j < count($lengths)-1; $j++) {
			   $difference /= $lengths[$j];
		   }

		   $difference = round($difference);

		   if($difference != 1) {
			   $periods[$j].= "s";
		   }

		   return "$difference $periods[$j] ago ";
		}
		
	$next_records = 10;
	$show_more_button = 0;
	
	if(checkValues($_REQUEST['value']))
	{
	 
		$editorname = $_REQUEST['u'];
		$d_id = (int) $_REQUEST['eventId'];
		
		if (md5($_REQUEST['u']."kyarata75") === $_REQUEST['m'])
		{
			mysql_query("INSERT INTO wall_posts (post,f_name,user,date_created,document_id) VALUES('".checkValues($_REQUEST['value'])."','".$editorname."','".$editorname."','".strtotime(date("Y-m-d H:i:s"))."','".$d_id."')");
		}
		
		$result = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS TimeSpent FROM wall_posts WHERE document_id = " . $d_id ." order by p_id desc limit 1");
		
		die(printComments($result));
	 
	}
	elseif($_REQUEST['show_more_post']) // more posting paging
	{
		$next_records = $_REQUEST['show_more_post'] + 10;
		
		$result = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS TimeSpent FROM wall_posts WHERE document_id = " . $d_id ." order by p_id desc limit ".((int) $_REQUEST['show_more_post']).", 10");
		
		$check_res = mysql_query("SELECT * FROM wall_posts WHERE document_id = " . $d_id ." order by p_id desc limit ".$next_records.", 10");
		
		$show_more_button = 0; // button in the end
		
		$check_result = mysql_num_rows(@$check_res);
		if($check_result > 0)
		{
			$show_more_button = 1;
		}
	}
	else
	{
	 
		  if (ifAdmin($user->name)) $result1 = mysql_query("SELECT * FROM maenna_company_events mce WHERE  companyid = '".((int) $_REQUEST['id'])."' ORDER BY eventid DESC"); else
		  $result1 = mysql_query ("SELECT * 
					     FROM  `maenna_company_events` mce
					     WHERE (".$user->uid." 
					     IN (
					     
					     SELECT uid
					     FROM maenna_company_events_inv
					     WHERE eventid = mce.eventid
					     ) AND companyid = '".$_REQUEST['id']."' ) OR (postedby = '".$user->uid."' AND companyid = '".((int) $_REQUEST['id'])."') ORDER BY eventid DESC ");
		  while ($row1 = mysql_fetch_array($result1)) {
		  
		  $resuid = mysql_query("SELECT name FROM users WHERE uid = '".$row1['postedby']."'");
		  $postedUname = mysql_fetch_array($resuid);
		  
		  $invres = mysql_query("SELECT status FROM maenna_company_events_inv WHERE uid = '".$user->uid."' AND eventid = '".$row1['eventid']."'");
		  $invstatus = mysql_fetch_array($invres);
		  
		  $attres = mysql_query ("SELECT uid FROM maenna_company_events_inv WHERE eventid = '".$row1['eventid']."' AND status = 'confirmed' ");
		  $invAll = mysql_query ("SELECT uid FROM maenna_company_events_inv WHERE eventid = '".$row1['eventid']."' AND status <> 'confirmed'");
		  
			   
		  $files = mysql_query ("SELECT * FROM maenna_company_data WHERE data_value6 = '".$row1['eventid']."' AND data_type='events' AND deleted = 0 ORDER BY dataid DESC");
		  
?>
	 <div class="event" id="event<?=$row1['eventid'];?>">

	 <div id="clear" style="clear:both"></div>
		  <div style="float:left;" class="calendar">
			   <span class="day"><?=date("d",$row1['datetime']);?></span>
			   <span class="month"><?=strtoupper(date("M",$row1['datetime']));?></span>
			   
		  </div><!-- calendar -->
		    
		  <div class="event-info">
                       <div style="margin-left:10px; float:left;">
			   <span class="eventTitle" style="float:left; cursor:pointer; font-size:15px;"><strong><?=replace_email(strtoupper($row1['title']));?></strong></span><div id="clear" style="clear:both"></div>
			   <span style="float:left;"><?=date("l, M j, Y g:i A T ",$row1['datetime']);?></span>
			   <div id="clear" style="clear:both"></div>
			   <span style="float:left;"><?=$row1['location'];?></span><div id="clear" style="clear:both"></div>
			     <span style="float:left; display:none"><?=$row1['description'];?></span>
			     <div id="clear" style="clear:both"></div>
			   </div>
			   <?php
			   if (mysql_num_rows($attres) > 0 || mysql_num_rows($invres) > 0 || mysql_num_rows($invAll) > 0 ) {
			   
			   
			   echo '<div style="clear:both;"></div><div style="float:left;"><span class="invatt" style="float:left;font-size:10px;">';
			   
			   if (ifAdmin($user->name)) {
			   if (mysql_num_rows($invAll) > 0) {
				    
				    echo "INVITED:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
				    while ($inv = mysql_fetch_array($invAll)) {
				    $proId = getProId($inv['uid']);
				    if (substr($proId,0,7) == 'invalid') {echo "CLEWED ADMIN &nbsp;&nbsp;";} else echo "<a class=\"tool\" target=\"_blank\" href=\"/account?tab=professionals&page=pro_detail&id=".$inv['uid']."&closebtn=1\">".$proId."</a> &nbsp;&nbsp;";
			   
			   }
			   echo "<br>";
				    
			   }
			   }
			   if (mysql_num_rows($attres) > 0) {
			   echo "CONFIRMED:&nbsp;&nbsp;";
			   while ($att = mysql_fetch_array($attres)) {
				    $proId = getProId($att['uid']);
				    if (substr($proId,0,7) == 'invalid') {echo "CLEWED ADMIN &nbsp;&nbsp;";} else echo "<a class=\"tool\" target=\"_blank\" href=\"/account?tab=professionals&page=pro_detail&id=".$att['uid']."&closebtn=1\">".$proId."</a> &nbsp;&nbsp;";
			   }
			   echo "<br>";
			   }
			   
			   if (mysql_num_rows($invres) > 0) {

			   if ($invstatus['status'] == 'sent') echo "<span id=\"att".$row1['eventid']."\"  class='attendance'>CONFIRM YOUR ATTENDANCE! <a class = \"rsvp\" name=\"confirmatt\" eventid = \"".$row1['eventid']."\" style=\"color:#00a2bf !important; cursor:pointer;\" >RSVP</a></span>";
			   
			   elseif ($invstatus['status'] == 'confirmed') echo "<span id=\"att".$row1['eventid']."\"  class='attendance'>YOUR STATUS: <a class = \"rsvp\" name=\"confirmatt\" eventid = \"".$row1['eventid']."\" style=\"color:#00a2bf; cursor:pointer;\" >CONFIRMED</a></span>";
			   
			   elseif ($invstatus['status'] == 'maybe') echo "<span id=\"att".$row1['eventid']."\" class='attendance'>YOUR STATUS: <a class = \"rsvp \" name=\"confirmatt\" eventid = \"".$row1['eventid']."\" style=\"color:#00a2bf; cursor:pointer;\" >TENTATIVE</a></span>";
			   
			   elseif ($invstatus['status'] == 'declined') echo "<span id=\"att".$row1['eventid']."\"  class='attendance'>YOUR STATUS: <a class = \"rsvp \" name=\"confirmatt\" eventid = \"".$row1['eventid']."\" style=\"color:##00a2bf; cursor:pointer;\" >DECLINED</a></span>";
			   }
			   echo '</span><div id="clear" style="clear:both"></div>';
			   }
			   
			   
				    ?>
				    <span class="attFiles" style="float:left;font-size:10px;">
				    <?php
				    if (mysql_num_rows($files) > 0) {
				    echo "FILES:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
				    while ($filesres = mysql_fetch_array($files)) {
				    echo "<a target='_blank' class= \"tool\" style=\"color:#00a2bf;\" href='/account?tab=companies&page=company_detail&id=".$filesres['companyid']."&mtab=file&file=".urlencode('/sites/default/files/'.$filesres[data_value2])."&name=".urlencode($filesres[data_value])."'>".strtoupper($filesres[data_value])."</a> &nbsp;&nbsp; ";
				    }
			   }
			   
			   ?>
			   
			   </span>
			   </div>
	 <div id="clear" style="clear:both"></div>
	 	 <?php
	 if($row1['postedby'] == $user->uid || $AccessObj->Com_sections['advice']['sections']['maenna_events']['access'] == 'write'){?>
		  	<a style="float:right;margin-left:10px;margin-right:8px;" href="#" id="remove_id<?php  echo $row1['eventid']?>" alt="<?php  echo md5($row1['eventid'].$user->name."kyarata75") ?>" name="<?=$user->name;?>" delType = 'event' class="tool evdelete"> REMOVE</a>
			
			<a style="float:right; margin-left:10px;" href="#" id="edit_id<?php  echo $row1['eventid']?>" alt="<?php  echo md5($row1['eventid'].$user->name."kyarata75") ?>" name="<?=$user->name;?>" delType = 'event' class="tool evedit"> EDIT</a>
<?php } ?>
		  <span style="float:right;color:#231F20; margin-left:1px;" class = "tool">
	<?php
		  if (ifAdmin($postedUname['name'])) {echo "clewed";} else echo strtoupper($postedUname['name']);?><?="&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;".strtoupper(date("M j ",$row1['created']));?></span>

		  
	 <br>
	<div style="position: relative;margin-bottom:10px; ">
	 	 <form action="" method="post" name="postsForm">
	
		<div class="UIComposer_Box" style="position: relative;height: 80px;">
			
		<span class="w" >
			<textarea eventid = "event<?=$row1['eventid'];?>" name="<?=$user->name;?>" alt="<?=md5($user->name."kyarata75");?>"
			 class=" input watermark" style="height:20px;" cols="60"></textarea>
		</span>
	
			
			
		<div align="right" style="height: 16px;line-height: 16px;background: white;margin-top:-4px;;">
			<a id="shareButton" textref =  "event<?=$row1['eventid'];?>" class="tool" style="display:none;"> SUBMIT </a>
		</div>
			
		</div>
	
		</form>
		 <div style="clear: both;"></div>
		<!--<div style="position: relative;border-bottom:dotted 1px #ccc;bottom: 4px;z-index:-1">
			<div class="discussions">
				Discussions
			</div>
		 </div>-->
		<div style="clear: both;"></div>
	</div>
<?php

			   $show_more_button = 1;
			   $d_id = 'event'.$row1['eventid'];
			   $result = mysql_query("SELECT *,
			   UNIX_TIMESTAMP() - date_created AS TimeSpent FROM wall_posts WHERE document_id = '" . $d_id ."' order by p_id desc limit 0,10");
			   printComments($result);
			   //echo "<hr style=\"color: #E7E6E7;background-color: #E7E6E7;height: 5px; width:100%;\">";
			   echo "</div></div></div>";
			   
		  }
		
	}
	
	if($show_more_button == 1){?>
	 
	<!--<div id="bottomMoreButton">
	<a id="more_<?php // echo @$next_records?>" class="more_records" href="javascript: void(0)">Older Posts</a>
	</div>-->
	<?php
	}?>
	