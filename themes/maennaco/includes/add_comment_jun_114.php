<?php

	include('dbcon.php');
	error_reporting (E_ALL ^ E_NOTICE);
date_default_timezone_set('EST');
global $base_url;

if ($_REQUEST['type'] == 'pro_dis_comment' || $_REQUEST['type'] == 'pro_post_comment' || $_REQUEST['type'] == 'pro_file_comment' ) {

    function getUserTypeById($uid) {

        $query = "SELECT rid FROM users_roles WHERE uid = '".$uid."' ";

        $result = mysql_query($query);

        $Row = mysql_fetch_object($result);

        $rid =  $Row->rid;

        if (in_array($rid,array(4,5,7,8,11,12))) return 'people';

        elseif ($rid == 6) return 'admin';

        elseif ($rid == 10) return 'super_admin';

        elseif ($rid == 3) return 'company';

        return "invalid uid";

    }

    function getAvatarUrl($uid) {

        $usrType = getUserTypeById($uid);

        if ($usrType == 'people')     {

            //Get user gender
            $query = "SELECT gender FROM maenna_people WHERE pid = '".$uid."'";
            $result = mysql_query($query);
            $gender_tmp = mysql_fetch_object($result);
            $gender = $gender_tmp->gender;
			if ($usrType == 'admin' || $usrType == 'super_admin') {return '/themes/maennaco/images/discussion_logo.png';}
            //Check if user have a profile picture
            if (file_exists('../../../sites/default/images/profiles/50x50/' . $uid . '.jpg')) { $avatar = '/sites/default/images/profiles/50x50/' . $uid . '.jpg';
            } else {
                if ($gender == 'm' || $gender == '') { $avatar = ' ../images/prof-avatar-male.png';
                } else
                    $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            }
        }

        elseif ($usrType == 'company') {

            //Get cmp_role
            $query = "SELECT company_type FROM maenna_company WHERE companyid = '".$uid."'";
            $result = mysql_query($query);
            $cmp_role_tmp = mysql_fetch_object($result);
            $cmp_role = $cmp_role_tmp->company_type;
            //Check if user have a profile picture
            if (file_exists('../../../sites/default/images/company/50x50/'.$uid.'.jpg')) {$avatar = '/sites/default/images/company/50x50/'.$uid.'.jpg';}
            else
                if ($cmp_role == 'service') $avatar =$base_url.'themes/maennaco/images/cmp-avatar-service.png';
                else $avatar =' /themes/maennaco/images/cmp-avatar-product.png';

        }

        else $avatar = '/themes/maennaco/images/prof-avatar-female.png';


        return $avatar;

    }

    function getUserName($uid) {

        $sql = "SELECT users_roles.uid + 100 AS uid,
                   users_roles.rid,
                       CASE
                      WHEN users_roles.rid IN (6, 10)
                      THEN
		      CONCAT('ADMIN_',LEFT(users_extend.first_name,1),LEFT(users_extend.last_name,1))
                          WHEN  users_roles.rid NOT IN (6, 10, 3)
                          THEN
                             CONCAT(UPPER(maenna_people.firstname), 'MAE')
			 WHEN (users_roles.rid = 3)
			 THEN maenna_company.projname
			 END as `name`
			  FROM    users
			  LEFT JOIN
			  maenna_people
			  ON (users.uid = maenna_people.pid)
			  LEFT JOIN
			  maenna_company
			  ON (maenna_company.companyid = users.uid)
			  LEFT JOIN
			  users_extend
			  ON (users.uid = users_extend.uid)
                       LEFT JOIN
                          users_roles
                       ON (users_roles.uid = users.uid)
		       WHERE users.uid=".mysql_real_escape_string($uid);

        $result = mysql_query($sql);
        $row = mysql_fetch_array($result);
        $firstname = strtoupper($row['name']);
        if(in_array($row['rid'], array(6, 10))){
            $uid = sprintf("%04s", $row['uid']);
        } else {
            $uid = sprintf("%04s", $row['uid']);
        }
        if ($row['rid']==3) $output = $firstname;
        else $output = $firstname . $uid;
        if ($uid==160) $output = "clewed";


        return ($output);

    }

}

    if ($_REQUEST['type'] == 'pro_file_comment') {

        if ($_REQUEST['m'] == md5($_REQUEST['uid'].$_REQUEST['pid']."kyarata75")) {

            $editor = $_REQUEST['uid'];
            $editorname = getUserName($editor);
            mysql_query("INSERT INTO pro_wall_posts_comments (post_id,comment,username,user_id,datecreated) VALUES('".$_REQUEST['pid']."','".$_REQUEST['value']."','".$editorname."','".$editor."','".time()."')")  or die(mysql_error());

            $result = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - datecreated AS CommentTimeSpent FROM pro_wall_posts_comments order by cid desc limit 1")  or die(mysql_error());

            while ($rows = mysql_fetch_array($result))
            {
                $days2 = floor($rows['CommentTimeSpent'] / (60 * 60 * 24));
                $remainder = $rows['CommentTimeSpent'] % (60 * 60 * 24);
                $hours = floor($remainder / (60 * 60));
                $remainder = $remainder % (60 * 60);
                $minutes = floor($remainder / 60);
                $seconds = $remainder % 60;	?>
            <div class="commentPanel" id="comment-<?php  echo $rows['cid'];?>" align="left">

                <?php
                $crId = $editor;

                $avatar = getAvatarUrl($crId);
                echo "<img src=".$avatar." style=\"float:left;margin-top:5px; margin-right:5px; width:35px; height:35px;\">";

                ?>
                <label style="width:85%;" class="postedComments">
                    <span ><b><?php echo $rows['f_name']; echo "</b><span style='color:#666;'>&nbsp;continues discussion:</span>";?></span>
                    <?php  echo nl2br($rows['comment']);?>

                    <br><span style="display:inline-block;width:90px; color:#666666; font-size:11px">
			<?php
                    if($days2 > 0)
                        echo date('F d Y H:i:s', $rows['date_created']);
                    elseif($days2 == 0 && $hours == 0 && $minutes == 0)
                        echo "few seconds ago";
                    elseif($days2 == 0 && $hours == 0)
                        echo $minutes.' minutes ago';
                    else
                        echo "few seconds ago";
                    ?>
			</span>

                        &nbsp;&nbsp;<a href="#" id="cid-<?php  echo $rows['cid'];?>" cid="<?=$rows['cid'];?>" m="<?php  echo md5($rows['cid']."kyarata75") ?>" class="c_delete tool">Delete</a>

            </div></label>
            <?php
            }

        }

        else die('Authentication problem');


    }

    if ($_REQUEST['type'] == 'pro_dis_comment') {

    if ($_REQUEST['m'] == md5($_REQUEST['dissid']."kyarata75")) {

        $uid = $_REQUEST['editor'];
        $dissid = $_REQUEST['dissid'];
        $uname = getUserName($uid);


        mysql_query("INSERT INTO pro_wall_posts (pro_id,post,f_name,user,tags,flag,date_created) VALUES('".$dissid."','".checkValues(urldecode($_REQUEST['text']))."','".$uname."','".$uid."','". $_REQUEST['tags']."','q','".time()."')");

        $post_id = mysql_insert_id();

        $avatar = getAvatarUrl($uid);

       $html = '<div class="ask">
        <div class="askpic">
            <img src="'.$avatar.'" style="float:left; margin-top:13px; margin-right:5px; width:50px; height:50px;">&nbsp;
        </div>
            <div class="asktitle">'.$uname.'&nbsp;<strong>asks a question:</strong></div>

 					<p style="margin:5px 0px 0px 0px;">'.urldecode($_REQUEST['text']).' </p>
					<div class="askright" style="width:525px !important; float: left; margin-left: 65px;">
						<div style="margin:0px 0px 5px 0px; padding:0px 0px 2px 0px;">
                            <div style="height:30px; width: 535px;">
                                <div class="comment_anchor" style="width:130px;float:left;margin-top:5px;">
                                        <span style="margin:0px 0px 0px 0px;padding:0px 0px 0px 0px;" id="likepost1'.$dissid.'" ><a href="javascript:void(0);" style="cursor:pointer;" onClick="like_posts(\'like\', \''.$uid.'\', \''.$post_id.'\', \''.$_REQUEST['pro_profile'].'\');">Like</a></span>
                                    &nbsp;|&nbsp;<a onclick="formDisplay(\''.$post_id.'\');">Comment</a>
                                </div>
                                <div style="float:right;margin:7px 0px 0px 0px;color:#76787f">Topic: '.$_REQUEST['tags'].'</div>
                            </div>

                        </div>';
        $html .= '<div class="w" style="display:none;margin:21px 0px 0px 0px; padding:0px 0px 0px 20px; background:#f4f8fa!important;" id="form_id'.$post_id.'">
								<form method="post" action="/account?tab=professionals&page=pro_detail&id='.$_REQUEST['pro_profile'].'&section=pro_industry_view&type=details&pro_id='.$dissid.'; ?>" id="comments">
                                    <input type="hidden" name="post_id" id="post_id" value="'.$post_id.'"  />
                                    <input type="hidden" name="dis_id" id="dis_id" value="'.$dissid.'"  />

                                    <textarea name="post_comment" id="post_comment'.$post_id.' class=" input watermark" style="width:87%;  margin: 5px 0px 0px -9px!important; height:25px; " onFocus="showsubmit(\''.$post_id.'\');"></textarea>

                                    <input type="submit" id="post_com'.$post_id.'" m = "'.md5($post_id."kyarata75").'" value="Submit" class="tool" style="display:none;vertical-align: top;margin-top: 0px;margin-left:74%;"  />
                                </form>
							</div>';

        $return = array("status" => "success", "display" => $html);
        die(json_encode($return));
    }
    else (die("Authentication problem"));


}

elseif ($_REQUEST['type'] == 'pro_post_comment') {

    if ($_REQUEST['m'] == md5($_REQUEST['post_id']."kyarata75")) {

        $uid = $_REQUEST['editor'];
        $uname = getUserName($uid);

        $now = time();


        mysql_query("INSERT INTO pro_wall_posts_comments (post_id,user_id,comment,username,datecreated) VALUES('".$_REQUEST['post_id']."','".$uid."','".checkValues($_REQUEST['text'])."','".$uname."','".$now."')");

        $comm_id = mysql_insert_id();

        $avatar = getAvatarUrl($uid);

        $html = '<div class="aucomnts">
						<div class="aucpic">
						<img src="'.$avatar.'" style="float:left; margin-top:13px; margin-right:5px; width:45px; height:45px;">&nbsp;
						</div>
						<div class="aucdisc">
                            <h5>'.$uname.'</h5>

                            <p id="com'.$comm_id.'">'.$_REQUEST['text'].'</p>
                            <div class="comment_anchor">
                                <div style="float:left;margin:0px;padding:0px;font-size:12px;">
                                    '.date("l, M j, Y g:i A T ",$now).'
                                    &nbsp;|&nbsp;
                                </div>
                                <div id="likepostcomment'.$comm_id.'" style="float:left;padding:0px 0px 0px 0px;">

                                        <a href="javascript:void(0);" style="cursor:pointer;" onClick="like_post_comments(\'like\',  \''.$comm_id.'\', \''.$uid.'\');">Like</a>
                                </div>
                                <div>
                                        &nbsp;|&nbsp;<a style="cursor:pointer;" href="javascript:void(0);" id="delete_comment'.$comm_id.'" class="delete_comment">Delete</a>
                                       </p></div></div>
                        </div><div style="clear:both"></div>
	</div>';

        $return = array("status" => "success", "display" => $html);
        die(json_encode($return));
    }
    else (die("Authentication problem"));

}


	function nameToId($name) {
	//
	$q = mysql_query("SELECT uid FROM users WHERE name = '".$name."' LIMIT 1") or die(mysql_error());
	$r = mysql_fetch_array($q);
	return $r['uid'];

	}

	function getUserType($uid) {

	$q = mysql_query("SELECT pid FROM maenna_people WHERE pid = '".$uid."' ");

	if (mysql_num_rows($q) > 0 ) return 'people';

	else {
		$q1 = mysql_query("SELECT companyid FROM maenna_company WHERE companyid = '".$uid."' ");
		if (mysql_num_rows($q1) > 0 ) return 'company';
		else return 'admin';
		}
}

	function checkValues($value)
	{
		 // Use this function on all those values where you want to check for both sql injection and cross site scripting
		 //Trim the value
		 $value = trim($value);
		 
		// Stripslashes
		if (get_magic_quotes_gpc()) {
			$value = stripslashes($value);
		}
		
		 // Convert all &lt;, &gt; etc. to normal html and then strip these
		 $value = strtr($value,array_flip(get_html_translation_table(HTML_ENTITIES)));
		
		 // Strip HTML Tags
		 $value = strip_tags($value);
		
		// Quote the value
		$value = mysql_real_escape_string($value);
		$value = htmlspecialchars ($value);
		return $value;
		
	}	
	
	if($_REQUEST['comment_text'] && $_REQUEST['post_id'])
	{
		function getProId($id)
		  {
		      if(empty($id)) return 'invalid id';
		      $sql = mysql_query("SELECT rid FROM users_roles WHERE uid = '".$id."' LIMIT 1 ");
		      $ridn = mysql_fetch_array($sql);
		      if ($ridn['rid'] == '3') {

				$sql = "select users_roles.*, maenna_company.projname from users_roles, maenna_company where users_roles.uid = '".$id."' and maenna_company.companyid = '".$id."' limit 1";


}
		else {

		      $sql = "select users_roles.*, maenna_people.firstname from users_roles, maenna_people where users_roles.uid = '".$id."' and maenna_people.pid = '".$id."' limit 1";
		      
}
		      $result = mysql_query($sql);
		      $Row = mysql_fetch_assoc($result);
		      $rid = $ridn['rid'];
		      $firstname = strtoupper($Row['firstname']);
		      if(in_array($rid, array(6, 10))){
			  $output = "clewed";
		      }elseif ($rid == "3") {
					$output = strtoupper($Row['projname']);

						}
			else
			{
			  $output = "${firstname}MAE" . sprintf("%04s", $id +100);
		      }
		      return $output;
		  }



	    $editorname = getProId($_REQUEST['uid']);

		if (md5($_REQUEST['u']."kyarata75") === $_REQUEST['m'])
		{
		mysql_query("INSERT INTO wall_posts_comments (post_id,comments,f_name,user,date_created) VALUES('".$_REQUEST['post_id']."','".$_REQUEST['comment_text']."','".$editorname."','".$_REQUEST['u']."','".strtotime(date("Y-m-d H:i:s"))."')")  or die(mysql_error());
		}
		$result = mysql_query("SELECT *,
		UNIX_TIMESTAMP() - date_created AS CommentTimeSpent FROM wall_posts_comments order by c_id desc limit 1")  or die(mysql_error());
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

	function ifAdmin ($uname) {
	 
	 $result = mysql_query("SELECT *  FROM users u WHERE u.name = '".$uname."' AND EXISTS (SELECT * FROM users_roles WHERE uid = u.uid AND rid IN (SELECT rid FROM role WHERE name = 'Super admin' OR name = 'Maennaco admin'))");
	 
	 if (mysql_num_rows($result) > 0 ) {return true;} else return false;
	}

	while ($rows = mysql_fetch_array($result))
	{
		$days2 = floor($rows['CommentTimeSpent'] / (60 * 60 * 24));
		$remainder = $rows['CommentTimeSpent'] % (60 * 60 * 24);
		$hours = floor($remainder / (60 * 60));
		$remainder = $remainder % (60 * 60);
		$minutes = floor($remainder / 60);
		$seconds = $remainder % 60;	?>
		<div class="commentPanel" id="comment-<?php  echo $rows['c_id'];?>" align="left">

<?php
$crId = nameToId($rows['user']);
$uType = getUserType($crId);

if ($uType == 'people' || $uType == 'admin') {

		//Get user gender
		$q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
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
				if ($cmp_role == 'service') $avatar =$base_url.'/themes/maennaco/images/cmp-avatar-service.png';
				else $avatar =' /themes/maennaco/images/cmp-avatar-product.png';

}
                        
echo "<img src=".$avatar." style=\"float:left;margin-top:5px; margin-right:5px; width:35px; height:35px;\">";

?>
			<label style="width:85%;" class="postedComments">
				<span ><b><?php if (ifAdmin($rows['user'])) {echo "clewed";} else  echo $rows['f_name']; echo "</b><span style='color:#666;'>&nbsp;continues discussion:</span>";?></span>
				<?php  echo nl2br($rows['comments']);?>
			
			<br><span style="display:inline-block;width:90px; color:#666666; font-size:11px">
			<?php
						if($days2 > 0)
						echo date('F d Y H:i:s', $rows['date_created']);
						elseif($days2 == 0 && $hours == 0 && $minutes == 0)
						echo "few seconds ago";		
						elseif($days2 == 0 && $hours == 0)
						echo $minutes.' minutes ago';
						else
			echo "few seconds ago";	
?>
			</span>
			
			<?php
			
			if($rows['user'] == $_REQUEST['u']){?>
			&nbsp;&nbsp;<a href="#" id="CID-<?php  echo $rows['c_id'];?>" alt="<?php  echo md5($rows['c_id'].$rows['user']."kyarata75") ?>" name="<?=$rows['user'];?>" class="c_delete tool">Delete</a>
			<?php
			}?>
		</div></label>
	<?php
	}?>	

		
		
		
		