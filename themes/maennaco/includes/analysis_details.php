<?php

error_reporting(0);
global $base_url;
global $user;
global $AccessObj;
$editorid = $user->uid;
$companyid = $_REQUEST['id'];
$time = time();
$data_type = 'analysis';
$id = $_REQUEST['id'];
$analysis_id = (int) $_REQUEST['analysis_id'];
$uname = $user->name;

$user_id = $user->uid;

$opCom = $AccessObj->Com_sections['analysis']['sections']['analysis_commenting']['access'];
//die("op=".$op."<br>opcom=".$opCom);


    include('dbcon.php');
// find out the domain:
$domain = $_SERVER['HTTP_HOST'];
// find out the path to the current file:
$path = $_SERVER['SCRIPT_NAME'];
$result = mysql_query ("select * from maenna_company_data where data_type = '".mysql_real_escape_string($data_type)."'  and deleted != 1 and dataid= '$analysis_id' order by dataid");

$likepost = mysql_num_rows($result);

$row = mysql_fetch_array($result);
date_default_timezone_set('EST');
?>
<script type="text/javascript" src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
<script type="text/javascript" src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery-ui-timepicker-addon.js"></script>
<script type="text/javascript" src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.autosuggest.js"></script>
<link href="<?php echo $base_url; ?>/themes/maennaco/jui/comments/css/screen.css?as" type="text/css" rel="stylesheet" />
<link href="<?php echo $base_url; ?>/themes/maennaco/jui/comments/css/autosuggest.css" type="text/css" rel="stylesheet" />
<link href="<?php echo $base_url; ?>/themes/maennaco/jui/comments/css/fileuploader.css" type="text/css" rel="stylesheet" />
<link href="<?php echo $base_url; ?>/themes/maennaco/jui/comments/css/SpryTabbedPanels.css" type="text/css" rel="stylesheet" />
<script src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.elastic.js" type="text/javascript" charset="utf-8"></script>
<script src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/fileuploader.js" type="text/javascript" charset="utf-8"></script>
<script src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.watermarkinput.js" type="text/javascript"></script>
<script src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/SpryTabbedPanels.js" type="text/javascript"></script>
<style type="text/css">
.comments_box {
display:block !important;
}
.text_button { background-color: transparent !important;border: medium none !important;color: #0fabc4 !important;cursor: pointer;
    font-family: 'LatoRegular' !important;font-size: 14px !important;font-style: normal !important; }
.conversations_forms textarea {
border: 1px solid #CCCCCC !important;
margin-top: 10px;
min-height: 26px;
padding: 6px;
width: 320px !important;
/*display: block!important;*/
}
.defaultSkin table.mceLayout {
border: 0;
border-left: 1px solid #CCC;
border-right: 1px solid #CCC;
}

</style>
<script type="text/javascript">

$('a[id^="post_com"]').live("click", function(event){

	event.preventDefault();
    var wrapper = $(this).parents('.cmtloop').eq(0);
    var comments = $('.askright', wrapper);
    
    post_id = $(this).attr('id').replace("post_com","");
//	dis_id = $('#dis_id').val();
	dis_id = $(this).attr('dissid');
	text = $(this).siblings('textarea[name="post_comment"]').val();
	editor = '<?=$user_id;?>';
    
	pro_profile = '<?=$_REQUEST['id'];?>';
    m = $(this).attr('m');
    sel_obj = $(this);
	if(text == '')
    {
        alert('Please type your comment.');
        return false;
    }
    sel_obj.attr("disabled",true);
    $.post("./themes/maennaco/includes/add_comment.php?type=analysis_comment", {post_id: post_id, text: text, editor: editor, m: m, pro_profile:pro_profile, dis_id:dis_id,
    }, function(response){
        if(response.status == 'success') {
          comments.append(response.display);
//          sel_obj.parents(":eq(2)").children(".askright:first").append(response.display);
//            sel_obj.parent().parent().parent().parent().children(".askright:first").children(":first").after(response.display);
            sel_obj.attr("disabled",false);
            //$("#form_id"+post_id).hide();
            sel_obj.siblings('textarea[name="post_comment"]').val('');
        }
        else (alert("Your request din`t go through. Please try again!"));

    },"json");
});
function showsubmit(id)
{
	$('#post_com'+id).show();
}

<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

$('.deletepost').live("click", function(e){

	if(confirm('Are you sure you want to delete this Post?')==false)
	return false;
	e.preventDefault();
	var temp    = $(this).attr('id').replace('deletepost','');
		$.ajax({ 
			type: 'get',
			url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
                'type=professional_post&' +
                'id='+temp+ "&" +
                "u=<?php echo $u; ?>&" +
                "m=<?php echo $m; ?>",
			data: '',
			beforeSend: function(){
			},
			success: function(){
				$('#dis_post' + temp).remove();
			}
		});
		return true;
});

$('.delete_comment').live("click", function(e){

	if(confirm('Are you sure you want to delete this Post?')==false)
	return false;
	e.preventDefault();
	var temp    = $(this).attr('id').replace('delete_comment',''),
        u = $(this).attr('u'),
        m = $(this).attr('m');
	$.ajax({ 
		type: 'get',
		url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
            'type=analysis_comments&' +
            'id='+temp + "&" +
            'u=' + u + "&" +
            'm=' + m,
		data: '',
		beforeSend: function(){
		},
		success: function(){
			$('#aucomnts' + temp).remove();
		}
	});
	return true;
});

$('.delete_post').live("click", function(e){

    if(confirm('Are you sure you want to delete this post?')==false)
        return false;
    e.preventDefault();
    var post_id = $(this).attr('id');
	<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>
    $.ajax({
        type: 'get',
        url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
            'type=analysis_posts&id=' + post_id + '&' +
            'u=<?php echo $u; ?>&' +
            'm=<?php echo $m; ?>',
        data: '',
        beforeSend: function(){
        },
        success: function(response){
            if (response != 'success') {alert('Action did not succeed.Please try again.');}
            else {
                to_remove = "dis_post"+post_id;
            $("#" + to_remove).remove();
            }
        }
    });
    return true;
});

<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

function like_posts(type,prof_id, post_id, userid)
{
	if(type == 'like')
		var status = 1;
	else
		var status = 0;
	
	$.ajax({ 

		type: 'get',
		url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
            'type=analysis_like_posts&' +
            'post_id='+post_id+'&' +
            'userid='+userid+'&' +
            'status='+status+'&' +
            'prof_id='+prof_id+ "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",
		data: '',
		beforeSend: function(){
		},
		success: function(){
			if(type == 'like')
			{
				
				$('#likepost1'+post_id).html('<a style="cursor:pointer;" onclick="like_posts(\'unlike\', '+prof_id+','+post_id+','+userid+');">Unlike</a>');
			}
			else
			{
				$('#likepost1'+post_id).html('<a style="cursor:pointer;" onclick="like_posts(\'like\', '+prof_id+','+post_id+','+userid+');">Like</a>');
			}
		}
	});
}

<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

function like_analysis(type,prof_id, userid)
{
	if(type == 'like')
		var status = 1;
	else
		var status = 0;
	$.ajax({ 
		type: 'get',
		url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
            'type=like_analysis&' +
            'userid='+userid+'&' +
            'status='+status+'&' +
            'prof_id='+prof_id+ "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",
		data: '',
		beforeSend: function(){
		},
		success: function(msg){
		//alert(msg);
			/*if(type == 'like')
			{
				location.reload();
			}
			else
			{
				location.reload();
				
				<a href="javascript:void(0);" style="cursor:pointer;" onClick="like_analysis('like', '<?=$row['dataid']?>','<?=$user->uid?>');">Like </a>
				
			}*/
			
			if(type == 'like')
					{
						$('#likepostnew').html("<a style='cursor:pointer;' onclick='like_analysis(\"unlike\", "+prof_id+","+userid+" );'>Unlike&nbsp;"+msg+"</a>");
					}
					else
					{
						$('#likepostnew').html("<a style='cursor:pointer;' onclick='like_analysis(\"like\", "+prof_id+","+userid+");'>Like&nbsp;"+msg+"</a>");
					}	
		}
	});
}
$("#question_post").live("click", function(event) {
		event.preventDefault();
		text = $("#dis_posts_question").val();
		if(text != '')
		{
			dissid = $(this).attr('dissid');
			editor = '<?=$user_id;?>';
			pro_profile = '<?=$_REQUEST['id'];?>';
			m = $(this).attr('m');
			tags = $("#tags_question :selected").val();
			sel_obj = $(this);
			sel_obj.attr("disabled",true);
		   $.post("./themes/maennaco/includes/add_comment.php?type=pro_analysis_comment", {dissid: dissid, text: text, editor: editor, m: m, tags: tags,pro_profile:pro_profile
		   }, function(response){
				  if(response.status == 'success') {
					  $('#question').hide();
					  $(".comts").after(response.display).show();
					  sel_obj.attr("disabled",false);
					  $("#dis_posts_question").val('');
					  $("#tags_question :selected").removeAttr("selected");
//            window.location.replace(window.location.toString());
				  }
			   else (alert("Your request din`t go through. Please try again!"));
	
		   },"json");
		  }
});

function formDisplay(id)
{
  if(!$('#form_id'+id).is('visible')){
    $('#form_id'+id).show();
  }
}
function DisplayCommentForm()
{
	$('#question').toggle();
}
function like_post_comments(type, comment_id, user_id, post_id)
{
	if(type == 'like')
		var status = 1;
	else
		var status = 0;

    <?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

	$.ajax({
				type: 'get',
				url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
                    'type=analysis_like_post_comments&' +
                    'comment_id='+comment_id+'&' +
                    'user_id='+user_id+'&' +
                    'status='+status+'&' +
                    'post_id='+post_id + "&" +
                    "u=<?php echo $u; ?>&" +
                    "m=<?php echo $m; ?>",
				data: '',
				beforeSend: function(){	},
				success: function(){
					if(type == 'like')
					{
						$('#likepostcomment'+comment_id).html("<a style='cursor:pointer;' onclick='like_post_comments(\"unlike\", "+comment_id+","+user_id+","+post_id+" );'>Unlike</a>");
					}
					else
					{
						$('#likepostcomment'+comment_id).html("<a style='cursor:pointer;' onclick='like_post_comments(\"like\", "+comment_id+","+user_id+","+post_id+");'>Like</a>");
					}					
				}
			});
}

		$('#shareButton').click(function(){

			var a = encodeURIComponent($("#watermark").val());
			var doc =  $("#documenturl").attr("title");
			var m =  $("#documenturl").attr("alt");
			var u =  $("#documenturl").attr("name");
                        var uid = '<?=$user->uid?>';
			
			if(a != "Discuss a topic or ask a question on this file ...")
			{
                        
                            var cid = '<?=$_REQUEST['id']?>';
                            var name =  '<?=$_REQUEST['name']?>';
                            var filename =  '<?=$_REQUEST['file']?>';
                            var invitees = $(".as-values").val();
            
                           $.post("/themes/maennaco/includes/posts.php?type=commInv", { cid: cid, name: name, filename: filename, invitees: invitees},
                                                   
                                                   function(response){          
             });

				$.post("/themes/maennaco/includes/posts.php?doc="+doc+"&u="+u+"&m="+m+"&value="+a+"&uid="+uid, {
	
				}, function(response){
					
					$('#posting').prepend($(response).show());
					$("#watermark").val("Discuss a topic or ask a question on this file ...");
					
					$('textarea').elastic();
					$(".commentMark").Watermark("Got advice / question on this topic?");
					$("#watermark").Watermark("watermark","#369");
					
					$(".commentMark").Watermark("watermark","#EEEEEE");

					$(".as-selection-item").each(function() {$(this).remove()});

					$(".as-values").val("");

				});
			}
		});
		

$('#question_post').live("click", function(e){

	var val = $('#dis_posts_question').val();
	if(val == '')
	{
		alert("Please enter comment.");
		return false;
	}
	else
	{
		return true;
	}
	e.preventDefault();
});

</script>
<style type="text/css">
div.content_box .box_title { margin-top:14px; }
/*.text_button { border: none !important; background-color: transparent!important; color:#8f9094!important; text-decoration: underline!important; /* if desired 
   color: #000000; cursor: pointer; font-family: 'LatoRegular'!important; font-size: 14px!important; font-weight: bold!important;
	font-style:normal !important; }*/

div.content_box .box_content{
	/*padding-bottom:15px;*/
	padding-top:2px;
	font-size:14px;
	text-align:left;
	font-family:'LatoRegular';
	float:left !important;
}
div.content_box{ padding-left:10px; position:relative; width:100%; padding-bottom:10px; float:left !important; }
</style>
<?php

	$usertype = $AccessObj->user_type;

	function nameToId($name) {

	$q = mysql_query("SELECT uid FROM users WHERE name = '".mysql_real_escape_string($name)."' LIMIT 1") or die(mysql_error());
	$r = mysql_fetch_array($q);
	return $r['uid'];

	}

	function getUserType($uid) {
        $uid = (int) $uid;
	//Update on 13-05-13
	/*$q = mysql_query("SELECT pid FROM maenna_people WHERE pid = '".$uid."' ");

	if (mysql_num_rows($q) > 0 ) return 'people';

	else {
		$q1 = mysql_query("SELECT companyid FROM maenna_company WHERE companyid = '".$uid."' ");
		if (mysql_num_rows($q1) > 0 ) return 'company';
		else return 'admin';
		}*/
		
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
?>
<div class="tabtags" style="padding: 0 0 0 10px; width:610px;">
 <div class="rt-title"><?php echo strtoupper($row['data_value']); ?></div>
  <?php 
	/*$sql_expertise = mysql_query ("SELECT * FROM  `maenna_people` WHERE `pid` = '" .$row['editorid'] ."'");
	$sql_exp_result = mysql_fetch_array($sql_expertise);
	if(mysql_num_rows($sql_exp_result) != 0) {
        if ($sql_exp_result['username_type'] == 1) $P_username = ucfirst($sql_exp_result['firstname']);
        else $P_username = ucfirst($sql_exp_result['firstname']) . ' ' . ucfirst($sql_exp_result['lastname']);
	}else
	{
		$sql_expertise = mysql_query ("SELECT * FROM  `users` WHERE `uid` = '" .$row['editorid'] ."'");
		$sql_exp_result = mysql_fetch_array($sql_expertise);
		$P_username = ucfirst($sql_exp_result['name']);
	}*/
    $P_username = getUserById($row['editorid']);
	$result3 = mysql_query ("SELECT * FROM  `like_analysis_posts` WHERE post_id = '" . $row['dataid'] ."' and user_id = '" . $user->uid ."'");
	$row3 = mysql_fetch_array($result3);
	$likepost1 = mysql_num_rows($result3);
	
	$show_like_unlike_sql = mysql_query ("SELECT * FROM  `like_analysis` WHERE prof_id = '" . $row['dataid'] ."' and user_id = '" . $user->uid ."'");
	
	$show_like_unlike_rows = mysql_fetch_array($show_like_unlike_sql);
	$show_like_unlike_count = mysql_num_rows($show_like_unlike_sql);
	
	$count_result3 = mysql_query ("SELECT * FROM  `like_analysis` WHERE prof_id = '" . $row['dataid'] ."'");
	$count_row3 = mysql_fetch_array($count_result3);
	$count_likepost1 = mysql_num_rows($count_result3);
	if($row['status'] == 0)
			{
				$savedby = 'Saved by';
			}
			else
			{
				$savedby = 'Published by';
			}
	
	?>
  <div style="margin-top: -7px; " class="rt-date"><?php echo $savedby; ?> <strong><?php echo $P_username; ?></strong> on <span class="rt-date" style="padding:0 !important;"><?=date("M j, Y ",$row['access']);?></span></div>
  <div class="analysis_details_post"><?php echo _filter_autop(utf8_decode($row['data_value2'])); //echo _filter_autop(utf8_decode($row['data_value2'])); ?></div>

<div  class='comment_anchor' style="width:610px;float:left;margin-top:-6px;padding: 0;">
    <?php if ($opCom == 'write' || ($opCom == 'edit' && $editorid == $row['editorid'] )) { ?> <a onclick='DisplayCommentForm();'>Comment</a>  &nbsp;| <?php }  if($show_like_unlike_count == 1) { ?>
    <span  id="likepostnew" style="float:none; padding:0px !important;" ><a href="javascript:void(0);" style="cursor:pointer;" onClick="like_analysis('unlike', '<?=$row['dataid']?>','<?=$user->uid?>');">Unlike </a><?php echo $count_likepost1; ?></span>
    <?php } else {?>
    <span id="likepostnew" style="float:none; padding:0px !important;" ><a href="javascript:void(0);" style="cursor:pointer;" onClick="like_analysis('like', '<?=$row['dataid']?>','<?=$user->uid?>');">Like </a><?php echo $count_likepost1; ?></span>
    <?php } ?>
   
    <?php $uType = getUserType($user->cid);
    if($op == 'write' || ($op == 'edit' && $editorid == $row['editorid'] )) { ?>
    |&nbsp;<a href="<?php echo $base_url; ?>/account?tab=companies&page=company_detail&mtab=analysis&id=<?php echo $companyid; ?>&section=maenna_analysis&panel=multi&view=edit&dataid=<?php echo $row['dataid']; ?>">Edit</a> |&nbsp;<a href="<?php echo $base_url; ?>/account?tab=companies&page=company_detail&mtab=analysis&id=<?php echo $companyid; ?>&section=maenna_analysis&panel=multi&view=edit&do=remove&dataid=<?php echo $row['dataid']; ?>">Delete</a>
    <?php } ?>
	<?php if($row['tags']) : ?><span style="float:right">Category: <?php echo $row['tags']; ?></span><?php endif; ?>
  </div>
  <br />
  <div class="comts" style="margin:0;">
    <div id="question" class="conversations_forms" style="display:none;">
      <form method="post" id="comments">
        <input type="hidden" name="prof_id" id="prof_id" value="<?php echo $row['dataid']; ?>"  />
        <input type="hidden" name="flag" id="flag" value="q"  />
        <div>
              <textarea style="display:inline; width:441px !important; margin-left: 14px; height: 25px;padding:0 5px;width: 98% !important; line-height: 25px;" name="dis_posts"  id="dis_posts_question" cols="82" rows="3" class="mceNoEditor" ></textarea>
              <a style="float:right;padding:0 !important; margin-left: 14px;" href="javascript:void(0);" type="submit" name="dis_post" id = "question_post" dissid="<?=$row['dataid'];?>" m="<?=md5($row['dataid']."kyarata75");?>">Submit</a>
        </div>
      </form>
    </div>
    <div style="clear:both"></div>
  </div>
  <?php

        if ($opCom != 'hide') {

		$result = mysql_query("SELECT *,UNIX_TIMESTAMP() - date_created AS TimeSpent FROM pro_analysis_posts where pro_id = ".$row['dataid']." order by pid desc");
		while($row1 = mysql_fetch_array($result)) {
		$crId = $row1['user'];
		$uid = $crId;
		$uType = getUserType($crId);
		$result3 = mysql_query ("SELECT * FROM  `like_analysis_posts` WHERE  prof_id = '".$row['dataid']."' and post_id = '".$row1['pid']."'");
		$row3 = mysql_fetch_array($result3);
		$likepost1 = mysql_num_rows($result3);
		if ($uType == 'people' || $uType == 'admin' || $uType == 'super_admin') {
				//Get user gender
				$q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = ".((int) $_REQUEST['id'])."");
				$gender_tmp = mysql_fetch_array($q1);
				$gender = $gender_tmp['gender'];
		if (file_exists('sites/default/images/profiles/50x50/'.$crId.'.jpg')) { $avatar = $base_url.'/sites/default/images/profiles/50x50/'.$crId.'.jpg';} 
			else 
			{
				if ($gender == 'm' || $gender == '') { $avatar =$base_url.'/themes/maennaco/images/prof-avatar-male.png';}
				else $avatar = $base_url.'/themes/maennaco/images/prof-avatar-female.png';
			}
		}
		else if ($uType == 'company')  {
		//Get cmp_role		
				$q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $crId") or die(mysql_error());
				$cmp_role_tmp = mysql_fetch_array($q1);
				$cmp_role = $cmp_role_tmp['company_type'];
				 //Check if user have a profile picture
				  if (file_exists($base_url.'/sites/default/images/company/50x50/'.$crId.'.jpg')) {$avatar = $base_url.'/sites/default/images/company/50x50/'.$crId.'.jpg';} 
				  else	
						if ($cmp_role == 'service') $avatar =$base_url.'/themes/maennaco/images/cmp-avatar-service.png';
						else $avatar =$base_url.'/themes/maennaco/images/cmp-avatar-product.png';
		
		}

	?>
  <div class="cmtloop" id="dis_post<?php echo $row1['pid'];?>">
    <div class="ask" style="margin-left:14px;">
      <div class="askpic" style="width: 35px;"> <?php echo "<img src='".$avatar."' style=\"float:left; margin-top:13px; margin-right:5px; width:35px; height:35px;\">&nbsp;"; ?> </div>
      <div class="asktitle"><?php echo $row1['f_name']; ?></div>
      <p style="padding: 0 0 8px 50px; ">
        <?=$row1['post']?>
      </p>
      <div class="askright" style="margin-top:-8px; width: 546px !important; float: left;">
        <div style="margin:0px 0px -12px 0px; padding:0px 0px 2px 0px;">
          <div style="height:30px; width: 535px;">
            <div class='comment_anchor' style="width:160px;float:left;margin-top:-5px;">
              <?php if($likepost1 == 1) { ?>
              <span style='margin:0px 0px 0px 0px;padding:0px 0px 0px 0px;' id="likepost1<?=$row1['pid']?>" ><a href="javascript:void(0);" style="cursor:pointer;" onClick="like_posts('unlike', '<?=$row['dataid']?>', '<?=$row1['pid']?>', '<?=$user->uid?>');">Unlike</a></span>
              <?php } else {?>
              <span style='margin:0px 0px 0px 0px;padding:0px 0px 0px 0px;' id="likepost1<?=$row1['pid']?>" ><a href="javascript:void(0);" style="cursor:pointer;" onClick="like_posts('like', '<?=$row['dataid']?>', '<?=$row1['pid']?>', '<?=$user->uid?>');">Like</a></span>
              <?php } ?>
              &nbsp; <?php if ($opCom == 'write' || ($opCom == 'edit' && $editorid == $row['editorid'] )) { ?> |&nbsp;<a onclick='formDisplay("<?=$row1['pid']?>");'>Comment</a> <?php } ?>
           <?php  if ($opCom == 'write' || ($opCom == 'edit' && $editorid == $row1['user'] )) { ?> &nbsp;|&nbsp;<a style="cursor:pointer;" href="javascript:void(0);" id="<?=$row1['pid']?>" class="delete_post">Delete</a>
                <?php } ?> </div>
          </div>
        </div>
        <?php
		$result2 = db_query("SELECT *, UNIX_TIMESTAMP() - datecreated AS TimeSpent FROM analysis_wall_post_comments where post_id=".$row1['pid']." order by cid asc");
		$comments_count = 0;
		while($row2 = db_fetch_array($result2)) { ?>
        <?php
        $comments_count++;
		$crId = nameToId($row2['username']);
		$uType = getUserType($crId);
		
		if ($uType == 'people' || $uType == 'admin') {
			//Get user gender
			$q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = ".((int) $_REQUEST['id'])."");
			$gender_tmp = mysql_fetch_array($q1);
			$gender = $gender_tmp['gender'];
	
		  if (file_exists('sites/default/images/profiles/50x50/'.$crId.'.jpg')) { $avatar = $base_url.'/sites/default/images/profiles/50x50/'.$crId.'.jpg';} 
		  else {
				if ($gender == 'm' || $gender == '') { $avatar =$base_url.'/themes/maennaco/images/prof-avatar-male.png';}
	
					else $avatar = $base_url.'/themes/maennaco/images/prof-avatar-female.png';
			}
		}
		else if ($uType == 'company')  {
			//Get cmp_role		
			$q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $crId") or die(mysql_error());
			$cmp_role_tmp = mysql_fetch_array($q1);
			$cmp_role = $cmp_role_tmp['company_type'];

			 //Check if user have a profile picture
			  if (file_exists($base_url.'/sites/default/images/company/50x50/'.$crId.'.jpg')) {$avatar = $base_url.'/sites/default/images/company/50x50/'.$crId.'.jpg';} 
			  else	
					if ($cmp_role == 'service') $avatar =$base_url.'/themes/maennaco/images/cmp-avatar-service.png';
					else $avatar =$base_url.'/themes/maennaco/images/cmp-avatar-product.png';
	
		} 
		$comment_result = mysql_query ("SELECT * FROM  `analysis_comments` WHERE  comment_id = '".$row2['cid']."' and post_id ='".$row1['pid']."'  and user_id = '".$user->uid."'");
		$likepost_comment = mysql_num_rows($comment_result);
	?>
        <div class="aucomnts" id="aucomnts<?php echo $row2['cid'];?>" style="float:left;">
          <div class="aucpic"> <?php echo "<img src=".$avatar." style=\"float:left; margin-top:13px; margin-right:5px; width:45px; height:45px;\">&nbsp;"; ?> </div>
          <div class="aucdisc" style="width: 91% !important;">
            <h5 style="float: none !important; margin-top: 0;color:#2db6de!important">
              <?php echo ($row2['username'] != '') ? $row2['username'] :  'User_' . $row2['user_id']; ?>
            </h5>
            <p style="padding: 0 !important; width: 95%;line-height: 14px;" id="com<?php echo $row2['cid']; ?>"><?php echo $row2['comment']; ?></p>
            <div class='comment_anchor' style="margin-top:-4px;">
              <div id='likepostcomment<?php echo $row2['cid'];?>' style='float:left; margin:0px;padding:0px 0px 0px 0px;'>
                <?php if($likepost_comment == 1) { ?>
                <a href="javascript:void(0);" style="cursor:pointer;" onClick="like_post_comments('like',  '<?=$row2['cid']?>', '<?=$user->uid?>', '<?=$row1['pid']?>');">Unlike</a>
                <?php } else {?>
                <a href="javascript:void(0);" style="cursor:pointer;" onClick="like_post_comments('like',  '<?=$row2['cid']?>', '<?=$user->uid?>', '<?=$row1['pid']?>');">Like</a>
                <?php } ?>
              </div>
                <?php if($opCom == 'write' || ($opCom == 'edit' && $editorid == $row2['user_id'] )){?>
                &nbsp;|&nbsp;<a style="cursor:pointer;"
                                href="javascript:void(0);"
                                id="delete_comment<?=$row2['cid']?>"
                                u="<?php echo $u = time();?>"
                                m="<?php echo md5('delete.php:' . $row2['cid'] . ':' . $u . ':kyarata75');?>"
                                class="delete_comment">Delete</a>
                <?php } ?>
              </div>
            </div>
          </div>
          <div style="clear:both"></div>
        
        <?php  } 
        
		if($comments_count == 0)
		{
			$comment_style = "style='float:left; display:none;margin:0px 10px 0px 65px; padding:21px 0px 0 0px;background:#f4f8fa!important'";
		}
		else
		{
			$comment_style = "style='float:left; margin:0px 10px 0px 65px; padding:21px 0px 0 0px;background:#f4f8fa!important'";
		}
		?>
</div>
            <?php if ($opCom == 'write' || ($opCom == 'edit' && $editorid == $row['editorid'] )) { ?>
        <div class="w" <?php echo $comment_style; ?> id="form_id<?php echo $row1['pid']; ?>">
          <form method="post" action="" id="comments" style="position: inherit;width:536px;">
            <textarea name="post_comment" class="comments_box mceNoEditor" id="post_comment<?php echo $row1['pid']; ?>" style="width:506px; margin: 5px 0px 0px 10px!important; height:25px;font-style:italic; font-family: 'Lato Light';color:#929497;" onFocus="showsubmit(<?php echo $row1['pid']; ?>);" ></textarea>
            <a class="analysis_subcomment" style="display:none;vertical-align: top;margin:8px;float:right" type="submit"  id="post_com<?php echo $row1['pid']; ?>" dissid="<?=$row1['pid']?>" m="<?=md5($row1['pid']."kyarata75");?>" >Submit</a>        
            <input type="submit" id="post_com<?php echo $row1['pid']; ?>" dissid="<?=$row1['pid']?>" m="<?=md5($row1['pid']."kyarata75");?>" value="Submit" class="text_button" style="display:none;vertical-align: top;"  />
          </form>
          <div style="clear:both"></div>
          
        </div> <?php } ?>
      <div style="clear:both"></div>
      </div>
    </div>
    <div id="paa" style="clear:both"></div>

  <?php } ?> 
<?php }  ?>
</div>