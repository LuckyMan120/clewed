<?php
function pro_name($op =  null)
{
    global $user,$AccessObj;
    $id = sget($_REQUEST, 'id');
    $Block['title'] = '';
    $content = '';
    $cur_section = $cur_panel = '';
    $cur_section = sget($_REQUEST, 'section');
    $cur_panel = sget($_REQUEST, 'panel');
    $cur_type = sget($_REQUEST,'type');
    if(($cur_section && (! in_array($cur_section,array('assign_admin', 'connections'))))  || $cur_panel )
    {
        if ($cur_section == 'pro_industry_view' && $cur_type == '') return '';
        $id = sget($_REQUEST, 'id');
        $maenna_id = getProId($id);
        $Block['title'] = "$maenna_id";


        $user_type = strtoupper(userType($id));
        $sql = "select * from maenna_people where pid = %d";
        $result = db_query($sql, array($id));
        $Row = db_fetch_object($result);
//        if($Row && $Row->yearwork && strlen($Row->yearwork) == 4)
//        {
//            $yrs = date("Y") - $Row->yearwork . " Years";
//        }else{
//            $yrs = '';
//        }

        if($Row){
          //$protype = strtolower($user_type); //Damjan changed this so usertype is calculated based on users_roles table $Row->protype;
          //$protypes = _pro_types();
         // $protype = $protypes[$protype];

            $protype = $user_type;

            if($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($id)))

                if (get_pro_accreditation_status() != 0) $acc= ' Accredited (self reported) ';

            $accData = getAccreditationVerData($id);
            if ($accData)  $acc = ' Accredited (verified)';//$protype .= "<br>Verified on ".date('m/d/Y',$accData['data_value']);
            $protype = $protype.$acc;
        } else {
          $protype = "";
        }

        $expertise = '';
        if($Row && $Row->experties)
        {
            $temp = Options_experties($Row->experties, 1);
            if(empty($temp)) $temp = $Row->experties;
            $expertise = htmlentities(($temp), ENT_QUOTES);
        }


        $src = getProAvatar($Row, '150', '150');


    $content = <<< END
    <!--
    <table class='no-border' style='margin-top:0;'>
        <tr>
            <td style='width:130px;'>
                <div style='position:relative;height:117px;width:117px;overflow:hidden' class='blue-border'><img src=".$src" width='117px' height='117px'></div>
            </td>
            <td style=''>
                <div class=row >$yrs</div>
                <div class=row style='text-align:left;text-transform:capitalize;'>$expertise</div>
            </td>
        </tr>
    </table>
    -->
	
	    <table class='no-border' style='margin-top:0;'>
        <tr>
            <td align="center" style='width:130px;'>
                <div class='blue-border'><img src='$src' </div>
            </td>
            <td style=''>
                

            </td>
        </tr>
		<tr><td>&nbsp;</td></tr>
		<tr>
			<td>
			<div class=row0 >$protype</div>
                <div class=row0 style="border-bottom:none;text-transform:capitalize;">$expertise</div>
			</td>
		</tr>
    </table><br/>
	
	
	
END;
    }else return '';
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;

}
function pro_relatedPro($op = null)
{
    global $user, $AccessObj;
    $editorid = $user->uid;
    $id = sget($_REQUEST, 'id');
    $Block['title'] = 'Related Professionals';
    $content = '';
    if(empty($op) && $id != $AccessObj->uid) return '';
    $sql = "select * from maenna_connections where (target_uid = '%s' or assignee_uid = '%s') and conntype='related' and status = 'active'";
    $result = db_query($sql, $id,$id);
    while($Row = db_fetch_object($result))
    {
        if($Row->target_uid == $id)$pro_id = $Row->assignee_uid;
        else $pro_id = $Row->target_uid;
        $maenna_id = getProId($pro_id);
        $roleType = strtoupper(userType($pro_id));
        $content .= "<div class='row singlerow'><a href='/account?tab=professionals&page=pro_detail&id=$pro_id&closebtn=1'>$maenna_id, $roleType</a></div>";
    }

    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;

}

//////  connection widget for company and pro
function pro_connections($op = null)
{
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
    global $AccessObj;
    global $ifAdmin;
    global $user;

    $editorid = $user->uid;
    $time = time();
    //if ($AccessObj->user_type == 'people') $id = $AccessObj->uid; else
    $id = sget($_REQUEST, 'id');
    $Block['title'] = '';
    $data_type = '';
    $redirect = rebuild_url(array('tab','page','id'));
    $content = '';

    $Conns = Connections::Pro_conns($id);

    //hide a pro's teambox
    //test_array($AccessObj);
    //test_array($Conns);
    if($AccessObj->user_type == 'company')
        return '';
    elseif($AccessObj->user_type == 'people')
    {
        if($AccessObj->uid != $id) return '';
    }
    elseif($AccessObj->user_type == 'admin')
    {
        $flag = false;
        foreach($Conns['Admin'] as $_Admin)
        {
            if($_Admin->assignee_uid == $AccessObj->uid){
                $flag = true;
                break;
            }
        }
        if(! $flag ) return '';
    }

    if($op  && ($op != 'update'))
    {


       	$coll_companies = getCollaborationCompanies($id);
        $foll_companies = getFollowers($id);
        $relPros = getRelated($id);
        //test_array($relPros);
        $cnt_box_0 = count($Conns['Partner']);
        $cnt_box_1 = count($Conns['Advisor']);
        $cnt_box_2 = count($Conns['Visible']);
	$cnt_box_3 = count($coll_companies['active']);
        $cnt_box_4 = count($foll_companies);
        $cnt_box_5 = count($relPros);
        $cnt_box_6 = count($Conns['Client']);
        $cnt_box_7 = count($Conns['Following']);
        $cnt_box_8 = count($Conns['Invitations']);
		$cnt_box_9 = count($Conns['Investor']);

        $active_box_0 = $active_box_1 = $active_box_2 = $active_box_3 = $active_box_4 = $active_box_5 = $active_box_6 = $active_box_7 = '';
        $ctype = sget($_REQUEST, 'ctype');

        $ptype = sget($_REQUEST, 'ptype');
        $LIST = '';
        $box_content = '';
		if($ctype != '') {

			$box_content = '';
			$box_title = '';
			$style = 'style=display:block;';
			$style2 = 'style=display:none;';
		}
		elseif($ptype != '')
		{

			$box2_content = '';
			$box2_title = '';
			$style2 = 'style=display:block;';
			$style = 'style=display:none;';
		}
		else
		{
			$box_content = '';
			$box_title = '';
			$box2_content = '';
			$box2_title = '';
			$style = 'style=display:none;';
			$style2 = 'style=display:none;';
		}

		if($ctype != 'projects')
		{
			$active_box_1 = '';
		}
		else
			$active_box_1 = 'activebox';

		if($ptype != 'followers')
			$active_box_4 = '';
		else
			$active_box_4 = 'activebox';

        if(empty($ctype) || $ctype == 'projects')
        {


            $box_title = "Connections";
            foreach($Conns['Advisor'] as $Proj)
            {
		  $target_uid = $Proj->target_uid;
		//Get cmp_role
		$q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $target_uid") or die(mysql_error());
		$cmp_role_tmp = mysql_fetch_array($q1);
		$cmp_role = $cmp_role_tmp['company_type'];
		 //Check if user have a profile picture
		  if (file_exists('sites/default/images/company/50x50/'.$target_uid.'.jpg')) {$avatar = 'sites/default/images/company/50x50/'.$target_uid.'.jpg';}
		  else
				if ($cmp_role == 'service') $avatar =' /themes/maennaco/images/cmp-avatar-service.png';
				else $avatar =' /themes/maennaco/images/cmp-avatar-product.png';

 ;
                $maenna_id = getProjectName($target_uid);
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><a style=\"margin - top:20px;\" href='/account?tab=companies&page=company_detail&id=$target_uid&closebtn=1'><img src=".$avatar." style=\"float:left; margin-right:5px; width:50px; height:50px;\">$maenna_id</a></div>";


            }
        }elseif($ctype == 'connected')
        {
            $active_box_2 = 'activebox';
            $box_title = "Connected";
            foreach($Conns['Visible'] as $Proj)
            {
                $target_uid = $Proj->target_uid;

		//Get cmp_role
		$q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $target_uid") or die(mysql_error());
		$cmp_role_tmp = mysql_fetch_array($q1);
		$cmp_role = $cmp_role_tmp['company_type'];
		 //Check if user have a profile picture
		  if (file_exists('sites/default/images/company/50x50/'.$target_uid.'.jpg')) {$avatar = 'sites/default/images/company/50x50/'.$target_uid.'.jpg';}
		  else
				if ($cmp_role == 'service') $avatar =' /themes/maennaco/images/cmp-avatar-service.png';
				else $avatar =' /themes/maennaco/images/cmp-avatar-product.png';

 ;

                $maenna_id = getProjectName($target_uid);
                $box_content .= "\n<div  style=\"height:55px;\" class='row singlerow'><a style=\"margin - top:20px;\" href='/account?tab=companies&page=company_detail&id=$target_uid&closebtn=1'><img src=".$avatar." style=\"float:left; margin-right:5px; width:50px; height:50px;\">$maenna_id</a></div>";
            }
        }elseif($ctype == 'management') {
          $active_box_6 = 'activebox';
          $box_title = "Management";
          foreach($Conns['Client'] as $Proj)
            {
                $target_uid = $Proj->target_uid;

		//Get cmp_role
		$q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $target_uid") or die(mysql_error());
		$cmp_role_tmp = mysql_fetch_array($q1);
		$cmp_role = $cmp_role_tmp['company_type'];
		 //Check if user have a profile picture
		  if (file_exists('sites/default/images/company/50x50/'.$target_uid.'.jpg')) {$avatar = 'sites/default/images/company/50x50/'.$target_uid.'.jpg';}
		  else
				if ($cmp_role == 'service') $avatar =' /themes/maennaco/images/cmp-avatar-service.png';
				else $avatar =' /themes/maennaco/images/cmp-avatar-product.png';

 ;

                $maenna_id = getProjectName($target_uid);
                $box_content .= "\n<div  style=\"height:55px;\" class='row singlerow'><a style=\"margin - top:20px;\" href='/account?tab=companies&page=company_detail&id=$target_uid&closebtn=1'><img src=".$avatar." style=\"float:left; margin-right:5px; width:50px; height:50px;\">$maenna_id</a></div>";
            }

          }else{

            $active_box_3 = 'activebox';
            $box_title = "Collaborating";
	        foreach ($coll_companies['active'] as $value )
            {
			  $target_uid = $value;
		//Get cmp_role
		$q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $target_uid") or die(mysql_error());
		$cmp_role_tmp = mysql_fetch_array($q1);
		$cmp_role = $cmp_role_tmp['company_type'];
		 //Check if user have a profile picture
		  if (file_exists('sites/default/images/company/50x50/'.$target_uid.'.jpg')) {$avatar = 'sites/default/images/company/50x50/'.$target_uid.'.jpg';}
		  else
				if ($cmp_role == 'service') $avatar =' /themes/maennaco/images/cmp-avatar-service.png';
				else $avatar =' /themes/maennaco/images/cmp-avatar-product.png';

 ;
                $maenna_id = getProjectName($target_uid);
                $box_content .= "<div style=\"height:55px;\" class='row singlerow'>
                                    <a style=\"margin - top:20px;\" href='/account?tab=companies&page=company_detail&id=$target_uid&closebtn=1'>
                                    <img src=".$avatar." style=\"float:left; margin-right:5px; width:50px; height:50px;\">$maenna_id</a></div>";
            }



        }

        if (empty($ptype) || $ptype == 'followers') {


            $box2_title = "Followers";
	        foreach ($foll_companies as $value )
	            {
			  $target_uid = $value;
			//Get cmp_role
			$q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $target_uid") or die(mysql_error());
			$cmp_role_tmp = mysql_fetch_array($q1);
			$cmp_role = $cmp_role_tmp['company_type'];
			 //Check if user have a profile picture
			  if (file_exists('sites/default/images/company/50x50/'.$target_uid.'.jpg')) {$avatar = 'sites/default/images/company/50x50/'.$target_uid.'.jpg';}
			  else
					if ($cmp_role == 'service') $avatar =' /themes/maennaco/images/cmp-avatar-service.png';
					else $avatar =' /themes/maennaco/images/cmp-avatar-product.png';

	 ;
	                $maenna_id = getProjectName($target_uid);
	                $box2_content .= "<div style=\"height:55px;\" class='row singlerow'>
<a style=\"margin - top:20px;\" href='/account?tab=companies&page=company_detail&id=$target_uid&closebtn=1'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
$maenna_id</a></div>";
	            }
        }

        else {

            $active_box_5 = 'activebox';
            $box2_title = "Related";

            foreach ($relPros as $pro_uid )
            {
                        //Get user gender
			$q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = $pro_uid");
			$gender_tmp = mysql_fetch_array($q1);
			$gender = $gender_tmp['gender'];
			//Check if user have a profile picture
			if (file_exists('sites/default/images/profiles/50x50/' . $pro_uid . '.jpg')) { $avatar = 'sites/default/images/profiles/50x50/' . $pro_uid . '.jpg';
			} else {
				if ($gender == 'm' || $gender == '') { $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
				} else
					$avatar = '/themes/maennaco/images/prof-avatar-female.png';
			}
			$pro_maeid = getProId($pro_uid);
			$rid = userRoleId($pro_uid);
			$pro_type = Options_proType($rid, 1);
            $pro_exp = getProExpertise($pro_uid);
			if ($AccessObj -> Com_sections[$_REQUEST['mtab']]['sections']['followers']['access'] == 'read')
				$box2_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:Helvetica; text-transform:uppercase;margin-top:20px;\">$pro_maeid, $pro_exp</p></div>";

else
				$box2_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1'> $pro_maeid, $pro_exp</a></div>";

            }

        }


        // {
            // $active_box_3 = 'active';
            // $box_title = "SELECTIONS";
            // foreach($Conns['Follow'] as $Proj)
            // {
                // $target_uid = $Proj->target_uid;
//
		// //Get cmp_role
		// $q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $target_uid") or die(mysql_error());
		// $cmp_role_tmp = mysql_fetch_array($q1);
		// $cmp_role = $cmp_role_tmp['company_type'];
		 // //Check if user have a profile picture
		  // if (file_exists('sites/default/images/company/50x50/'.$target_uid.'.jpg')) {$avatar = 'sites/default/images/company/50x50/'.$target_uid.'.jpg';}
		  // else
				// if ($cmp_role == 'service') $avatar =' /themes/maennaco/images/cmp-avatar-service.png';
				// else $avatar =' /themes/maennaco/images/cmp-avatar-product.png';
//
 // ;
//
                // $maenna_id = getProjectName($target_uid);
                // $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=".$avatar." style=\"float:left; margin-right:5px; width:50px; height:50px;\"><a style=\"margin-top:20px;\" href='/account?tab=companies&page=company_detail&id=$target_uid&closebtn=1' >$maenna_id</a></div>";
            // }
        // }
        $management_div = "";

        $msql = "select * from maenna_people where pid = %d";
        $mresult = db_query($msql, $_REQUEST['id']);
        $mRow = db_fetch_object($mresult);

        if($mRow && $mRow->protype){
          $protype = $mRow->protype;
        }

        // if the type = my_projects
        $type = sget($_REQUEST, 'type');
        $middleStyles = [] ;
        $myProjectsStyle = '';
        if ($type == 'my_projects'){
            $middleStyles['none'] = 'display:none;';
            $myProjectsStyle = <<< END
                <style>
                    .adcoments  a{
                        font-size: 15.5pt!important;
                        font-family: LatoRegular;
                    }
                    #advisor .adcoments span {
                         font-size: 15.5pt!important;
                         font-family: LatoRegular;
                    }
                    div.team_stat_list {
                    width: 590px;
                }
                </style>
END;
        }



//        if($protype == 'client' && ($AccessObj->uid == $_REQUEST['id'] || ($AccessObj->user_type == 'super' || $ifAdmin))) {
          $management_div = <<< END
              <div class="adcoments"><a id="selManagement" class='$active_box_6' style="width:120px;cursor:pointer" href="$redirect&section=connections&ctype=management">Own projects <span>$cnt_box_6</span></a> </div>
END;
//        } //clew-401
        $ref = $_REQUEST['ref'];
        $content = <<< END
        <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
        <script type="text/javascript">

        	$(document).ready(function(){

        	var ref = '$ref';

        	if (ref == 'follow') {

        	var thisObj = $("#selFollowers");

        if(thisObj.hasClass('team_active')) {
            thisObj.removeClass('team_active');
            $('.team1').hide();
            return true;
        }

        type = 'Followers';

        $.post("/themes/maennaco/includes/fetch.php?type=get"+type, {proId: $id},

            function(response){

                console.log(thisObj.parent());
                thisObj.addClass('team_active');

                $("#team_stat_title").html(type);
                $("#team_stat_list").html($(response).show());

                $('.team1').insertAfter(thisObj.parent());
                $('.team1').show();

            });}

                $('#get-help-btn').click(function(e) {
                    e.preventDefault();
                  $("#get-help-dialog").dialog({
                    modal: true,
                    autoOpen: true,
                    resizable: false,
                    title: 'Your connections',
                    width: 560,
                  });
                });



    $("a[id^='sel']").livequery("click", function(event){

        event.preventDefault();
        var thisObj = $(this);
        
        if(thisObj.hasClass('team_active')) {
            thisObj.removeClass('team_active');
            $('.team1').hide();
            return true;
        }

        type = $(this).attr('id').substr(3);
        console.log(type)
        $.post("/themes/maennaco/includes/fetch.php?type=get"+type, {proId: $id},

            function(response){

                console.log(thisObj.parent());

                $('a[id^="sel"]').removeClass('team_active');
                thisObj.addClass('team_active');
                
                $("#team_stat_title").html(type);
                $("#team_stat_list").html($(response).show());

                $('.team1').insertAfter(thisObj.parent());
                $('.team1').show();

            });

    });
    });
    </script>
		$myProjectsStyle
		<div id="advisor" >
            <br/>
		$management_div
		<div class="adcoments"><a id="selInvestors" class='$active_box_9' style="width:120px;cursor:pointer" href="$redirect&section=connections&ctype=investors">Investor <span>$cnt_box_9</span></a> </div>
		<div class="adcoments"><a id="selPartners" class='$active_box_0' style="width:120px;cursor:pointer" href="$redirect&section=connections&ctype=partners">Introductions <span>$cnt_box_0</span></a> </div>
		<div class="adcoments"><a id="selProjects" class='$active_box_1' style="width:120px;cursor:pointer" href="$redirect&section=connections&ctype=projects">Advising <span>$cnt_box_1</span></a> </div>
		<div class="adcoments"><a id="selCollaborating" class='$active_box_3' style="width:120px;cursor:pointer" href="$redirect&section=connections&ctype=collaborating">Interested <span>$cnt_box_3</span></a> </div>
                <div class="adcoments"><a id="selConnected" class='$active_box_2' style="width:120px;cursor:pointer" href="$redirect&section=connections&ctype=connected" > Connector <span>$cnt_box_2</span></a> </div>
		<div class="adcoments"><a id="selFollowers" style="width:120px;cursor:pointer" class='$active_box_4' href="$redirect&section=connections&ptype=followers">Followers <span>$cnt_box_4</span></a> </div>
		<div class="adcoments"><a id="selProFollowing" style="width:120px;cursor:pointer" class='$active_box_7' href="$redirect&section=connections&ptype=pro_following">Following <span>$cnt_box_7</span></a> </div>
		<div class="adcoments"><a id="selInvited" style="width:120px;cursor:pointer" class='$active_box_8' href="$redirect&section=connections&ptype=pro_invitation">Invitations <span>$cnt_box_8</span></a> </div>
		
	</div><br/>
		
		
	<div id="get-help-dialog" style="display:none">
	  <div style="width:500px;padding:20px;">
	  	  <p>Clewed connects professionals to projects and opportunities through the Connections section. This section gives you access to information based on your connection level as follows:</p>

	  <table style="font-family: LatoRegular !important;">
	    <thead style="text-decoration: underline; font-weight: bold;">
	        <tr>
	            <td width="150">Connection</td><td>Available Information</td>
	        </tr>
	    </thead>
	    <tbody>
        <tr>
            <td>My Company</td><td>Access all tabs for projects and opportunities you bring.</td>
        </tr>
        <tr>
            <td>Introductions</td><td>About, Due Diligence, Vetting, Analysis, Data Room, Services, Monitoring tabs on an as needed basis</td>
        </tr>
        <tr>
            <td>Advising</td><td>About, Vetting, Services for basic clients; Analysis and Monitoring tabs as applicable</td>
        </tr>
        <tr>
            <td>Interested</td><td>About, Vetting</td>
        </tr>
        <tr>
            <td>Connector</td><td>About, Vetting, Analysis, Data Room as applicable</td>
        </tr>
        <tr>
            <td>Followers</td><td>About, Profiles</td>
        </tr>
                <tr>
            <td>Following</td><td>About, Profiles</td>
        </tr>
	    </tbody>
	  </table>

	  <p>Note: All company connections are initiated by Clewed based on client needs except “Interested”. You can proactively build relationships with companies by requesting to join their Interested team.</p>
	  
	  </div>
    </div>
		
        <table class='team_stats' style='padding-bottom:0;margin-bottom:0;'>

<!--            <tr class="team1" $style>
                <td colspan=3 style='border-color:#dce6f5;border-bottom:none'>
                    <div class="team_stat_list" style="border-bottom:solid 2px #DCE6F5;">
                        <div class="title" id="team_stat_title">
                            $box_title
                            
                        </div>
                        
                    </div>
                </td>
            </tr>-->
            <tr class="team1" $style>
                <td colspan=3 style='border-top:none;'>
                    <div class="team_stat_list">
                        <div class="list" id="team_stat_list">
                            $box_content
                        </div>
                    </div>
                </td>
            </tr>
            <tr>
                <td style='border:none;hieght:8px;line-height:8 px;' colspan=3>&nbsp;</td>
            </tr>
        </table>
            
            <table class='team_stats' style='padding-bottom:0;margin-bottom:0;' $style2>
            <tr  $style2>
                <td colspan=3 style='border-color:#dce6f5;border-bottom:none'>
                    <div class="team_stat_list" style="border-bottom:solid 2px #DCE6F5;">
                        <div class="title" id="team_stat_title">
                            $box2_title
                            
                        </div>
                        
                    </div>
                </td>
            </tr>
            <tr $style2>
                <td colspan=3 style='border-top:none;'>
                    <div class="team_stat_list">
                        <div class="list" id="team_stat_list">
                            $box2_content
                        </div>
                    </div>
                </td>
            </tr>
        </table>
        
END;
    }
    $Block['body'] = sidebar_box('', $content);
    return $Block;
}

function insight_sort_left_panel() {

    require_once __DIR__ . '/pro_myinsights_block.php';
}

/* EOF */