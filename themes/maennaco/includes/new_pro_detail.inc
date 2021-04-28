<?php
function pro_tabs() {
    global $base_url;
    $page = sget($_REQUEST, 'page');
    $type = sget($_REQUEST, 'type');
    $id = sget($_REQUEST, 'id');
    $active = ($page == 'pro_detail' && $type == '') ? ' first  active-trail active ' : '';
    $activeInsights = ($page == 'pro_detail' && ($type == 'discussion' || $type == 'details' )) ? ' first  active-trail active ' : '';
    $activeMyProjects = ($page == 'pro_detail' && ($type == 'my_projects' || $type == 'details')) ? ' first  active-trail active ' : '';

    if ($_REQUEST['notif'] == '1') {
        echo '<div id="messages" class="messages status" style="margin: -2.0em 0 3.75em 0 !important;text-align:center;">You are in Your profile page</div>';
    }
    $pro_name = strtolower(getProId($id));
    $tmp = explode(" ", $pro_name);



    $content = '<div class="account-section-tabs" style="margin:-22px 0px 0px 0px;width:250px;">
            <ul>

<li class="' . $activeMyProjects . '"><a class="" href="' . $base_url . '/account?tab=professionals&page=pro_detail&id=' . $id . '&section=pro_industry_view&type=my_projects">My Projects</a></li>
<li class="' . $active . '"><a class="" href="' . $base_url . '/account?tab=professionals&page=pro_detail&id=' . $id . '&section=pro_industry_view">Profile</a></li>
<li class="' . $activeInsights . '"><a style="text-transform:none;" class="" href="' . $base_url . '/account?tab=professionals&page=pro_detail&id=' . $id . '&section=pro_industry_view&type=discussion">' . ucfirst($tmp[0]) . '`s Insights & Services</a></li>';

    if ($type == 'fileview' && isset($_REQUEST['file'])) {
        $filename = substr($_REQUEST['file'], strpos($_REQUEST['file'], '_') + 1);
        $content .= '<li class="first  active-trail active"><a class="" href="#">' . $filename . '</a></li>';
    }
    $content .= '</ul>
        </div>';

    echo $content;
}

function pro_details($op = null) {
    global $user;
    global $AccessObj;
    $editorid = $user->uid; // the currently logged in user, print_r this object to see all data available
    $companyid = sget($_REQUEST, 'id');
    $content = '';
    global $redirect;
    $data_type = 'discussions';
    $panel = 'multi';
    $section = __FUNCTION__;
    $proId = (int) $_REQUEST['pro_id'];
    $dis_query = mysql_query(
        "SELECT * FROM  `maenna_professional` WHERE `id` = '" . $proId . "' ORDER BY id DESC"
    );
    if (mysql_num_rows($dis_query) == 0) {
        echo "This Insight has not been scheduled yet.";
    } else {
        $dis_details = mysql_fetch_array($dis_query);
        if (!empty($dis_details)) {
            if (strlen($dis_details['title']) <= 64) $top = 'top:55%;'; else $top = 'top:30%;';
            if (strtoupper($dis_details['title']) == $dis_details['title']) $dis_details['title'] = strtolower($dis_details['title']);
            $Block['title'] = '<div class="dischead" style="height:52px;padding: 5px 7px 5px 7px;line-height:21px;">
        	<div class="dtitle" style="position:absolute;' . $top . ' text-transform: none !important;">' . $dis_details["title"] . '</div>';
        }
        if (sget($_REQUEST, 'name')) {
            $Block['title'] = sget($_REQUEST, 'name');
            $fname = sget($_REQUEST, 'name');
        }
        ob_start();
        $url = "http://" . $_SERVER['HTTP_HOST'];
        $editorname = $user->name;
        require_once 'pro_comments_details.php';
        $content = ob_get_contents();
        ob_end_clean();
        //$content = sget($_REQUEST, 'file');
        $Block['body'] = content_box($Block['title'], $content);
        return $Block;
    }
}

function pro_discussion($op = null) {
    global $user;
    global $AccessObj;
    global $ifAdmin;
    $user_type = $AccessObj->user_type;
    $editorid = $user->uid; // the currently logged in user, print_r this object to see all data available
    $companyid = sget($_REQUEST, 'id');
    $content = '';
    global $redirect;
    $data_type = 'discussions';
    $panel = 'multi';
    $section = __FUNCTION__;
    if ($user_type == 'people' || $ifAdmin) {
        $open_insights = (!empty($_POST['openinsights']) || !empty($_GET['openinsights'])) ? true : false;
        $open_services = (!empty($_POST['openservices']) || !empty($_GET['openservices'])) ? true : false;
        $Block['title'] = 'INSIGHTS AND SERVICES
        <div class="schedule">
            <div class="sched">
                <span id="questmark" class="insights" style="float: right;">
                    <a href="#" id="get-help-btn2">
                        <img src="/themes/maennaco/images/questionmark_white.png" style="padding-top:5px; margin-left: 6px;">
                    </a>
                </span>
            </div>
        </div>
        <div class="schedule" id="serviceTab" ' . ($open_services ? 'style="display: none;"' : '') . '>
            <div class="sched">
                <div class="discmnts"></div>
                <a id="showEventFormService">
                    <span style="text-transform:none !important;" data-tooltip="Share your expertise by creating your own Services. Click the ? on the right for guidelines.">
                        Create Services
                    </span>
                </a>
            </div>
        </div>
        <div class="schedule" id="insightTab" ' . ($open_insights ? 'style="display: none;"' : '') . '>
            <div class="sched">
                <div class="discmnts"></div>
                <a id="showEventFormInsight">
                    <span style="text-transform:none !important;" data-tooltip="Share your expertise by creating your own Insights. Click the ? on the right for guidelines.">
                        Create Insight
                    </span>
                </a>
            </div>
        </div>';
    } else {
        $Block['title'] = 'INSIGHTS AND SERVICES';
    }
    if (sget($_REQUEST, 'name')) {
        $Block['title'] = sget($_REQUEST, 'name');
        $fname = sget($_REQUEST, 'name');
    }
    ob_start();
    $url = "http://" . $_SERVER['HTTP_HOST'];
    $editorname = $user->name;
    require_once("pro_comments.php");
    $content = ob_get_contents();
    ob_end_clean();
    //$content = sget($_REQUEST, 'file');
    $Block['body'] = content_box($Block['title'], $content, "blue_bar");
    return $Block;
}

function pro_fileview($op) {
    if (empty($op)) {
        return '';
    } elseif ($op != 'update') {
        $pid = sget($_REQUEST, 'id');
        if (empty($pid)) return 'Invalid id';
        $file_path = sget($_REQUEST, 'file');
        $fid = $_REQUEST['fid'];
        $sql_files = mysql_query("SELECT * FROM wall_documents WHERE id = '" . $fid . "' LIMIT 1");
        $file_n = db_fetch_array($sql_files);
        if ($file_n['document_name'] != '') {
            list($time_stamp, $file_name) = explode('_', $file_n['document_name']);
            list($name, $ext) = explode('.', $file_name);
            $Block['title'] = $name;
        }
        global $redirect;
        global $AccessObj;
        $host = $_SERVER['HTTP_HOST'];
        $editor = $AccessObj->uid;
        if (empty($file_path)) {
            $content = 'No file selected!You have to select file';
        } else {
            ob_start();
            require_once("pro_fileview.php");
            $content = ob_get_contents();
            ob_end_clean();
        }
        $Block['body'] = content_box($Block['title'], $content);
        return $Block;
    }
}

function pro_profile($op = null) {

    require_once __DIR__ . '/../blocks/profile/expert_review_form.php';

    if (empty($op)) {
        return '';
    } elseif ($op != 'update') {
        $pid = sget($_REQUEST, 'id');
        if (empty($pid)) return 'Invalid id';
        global $AccessObj;
        global $redirect;
        $redirect = rebuild_url(array('tab', 'page', 'id'));
        $sql = "SELECT * FROM maenna_people WHERE pid = %d LIMIT 1";
        $result = db_query($sql, array($pid));
        $Maenna_people = db_fetch_array($result);
        if (empty($Maenna_people)) return '';
        $Block['title'] = '';//'Professional Profile';
        if ($op == 'write' || $pid == $AccessObj->uid) {
            $Block['title'] .= "&nbsp;<div class='editbtn spaced'><a href='$redirect&panel=pro_profile_panel&view=edit' class=tool>Edit</a></div>" . '<span id="questmark" class="pro_profile" style="float: right"><a href="#" id="get-help-btn-pro"><img src="/themes/maennaco/images/questionmark.png" style="padding-top:5px;"></a></span>';
        }
        $Info = array(
            'acc_type'      => '',
            'industries'    => '',
            'management'    => '',
            'expertise'     => '',
            'degree_school' => '',
        );
        $maenna_id = getProId(sget($Maenna_people, 'pid'));
        $account_type = userType($pid);
        $yearwork = sget($Maenna_people, 'yearwork');
        if ($yearwork && $yearwork != '') ;
        {
            $yearwork = date("Y") - $yearwork;
            if ($yearwork == date("Y") || $yearwork == '') $yearwork = '0 Years';
            if ($yearwork == 1) {
                $yearwork .= " Year";
            } elseif ($yearwork > 1) $yearwork .= " Years";
        }
//        $Info[0] = "$account_type, $yearwork";
        $Info['acc_type'] = $account_type;
        $Info['years'] = $yearwork;
        $Industries = array();
        if ($val = sget($Maenna_people, 'industry')) {
            $val = Options_industry($val, 1);
            if ($val) $Industries[] = ucwords($val);
        }
        if ($val = sget($Maenna_people, 'industry2')) {
            $val = Options_industry($val, 1);
            if ($val) $Industries[] = ucwords($val);
        }
        $Info['industries'] = implode("<br>", $Industries);
        $Info['hourly_rate'] = sget($Maenna_people, 'hourly_rate');
        //employment
        $sql = "SELECT * FROM maenna_people_data WHERE data_type = '%s' AND pid = %d ORDER BY access DESC LIMIT 1";
        $result = db_query($sql, array('employment', $pid));
        $Companies = $Positions = $PLs = $Experties = $Mlvl = array();
        $_MLVL = _managementLevel();
        if ($Row = db_fetch_object($result)) {
            if ($Row->data_value3) {
                //management level
                $val = option_code($_MLVL, $Row->data_value3, 1);
                if ($val) {
                    $Info['management'] = ucwords($val);
                }
            }
            if ($Row->data_value2) {
                //experties
                $val = Options_experties($Row->data_value2, 1);
                if ($val) $Info['expertise'] = ucwords($val);
            }
        }
        if ($Maenna_people && $Maenna_people['experties']) {
            $temp = Options_experties($Maenna_people['experties'], 1);
            if (empty($temp)) $temp = $Maenna_people['experties'];
            $Info['expertise'] = htmlentities(($temp), ENT_QUOTES);
        }
        //education
        $result = db_query($sql, array('education', $pid));
        $Degree = $School = array();
        $graduate = $undergraduate = '';
        while (($Row = db_fetch_object($result))) {
            $school = $Row->data_value;
            $degree = $Row->data_value2;
            $graduate = $Row->data_value;
            $undergraduate = $Row->data_value3;
            if ($degree) $degree = Options_degree($degree, 1);
            $deg_sch = ucwords($degree . ', ' . $school);
            $Info['degree_school'] = htmlentities($deg_sch, ENT_QUOTES | ENT_IGNORE, "UTF-8");
        }

        $profile = sget($Maenna_people, 'profile');
        $profile = substr($profile, 0, 250);
        if ($profile) $profile = htmlentities($profile, ENT_QUOTES | ENT_IGNORE, "UTF-8");
        $gender = sget($Maenna_people, 'gender');
        $protype = sget($Maenna_people, 'protype');
        $src = getProAvatar($Maenna_people, '150', '150');
        $ratings = getUserRatings($_REQUEST['id']);
        $followers = getFollowers($_REQUEST['id']);
        $content = "<div class='pro_box' style='min-height: 220px;'>
                        <div class='maenna_id'>
                          <img src=" . $src . " width='150px' height='151px'>
						</div>
                        <div class='profile'>" . (empty($profile) && $AccessObj->uid == $pid ? '<div class="adminarea_notice">You can use pseudonym and unidentifable photo for your profile. Your profile may be edited and optimized by clewed for consistency.</div>' : "<div style='font-style:italic;font-size:16px;color:#91939e;height:110px'>" . $profile . "</div>");
        if (in_array($AccessObj->user_type, array('super', 'admin'))) {
            $callback = 'onclick="alert(\'Administrator cannot follow professionals!\');"';
        } else $callback = 'type="' . (in_array($AccessObj->uid, $followers) ? "unfollow" : "follow") . '"';
        $content .= "<div style='margin-top:10px;color:#00a2bf !important;font-family: \"Lato Bold Italic\";font-size: 14px;'>";
        if (in_array($AccessObj->uid, $followers)) {
            $content .= "<a " . $callback . " class='follow'  cid='" . $AccessObj->uid . "' uid='" . $_REQUEST['id'] . "'>Following</a>";
        } else {
            $content .= "<a " . $callback . " data-tooltip='Be the first to know " . ucwords(strtolower($maenna_id)) . "`s next Insight' class='follow'  cid='" . $AccessObj->uid . "' uid='" . $_REQUEST['id'] . "'>Follow</a>";
        }
        if(count($followers) >= 10)
            $content .= " <span class='foll_cnt'>" . count($followers) . "</span>";
        $content .= "</div>
                    </div>";
        $content .= "<div class=\"pro_box_info\">";
        $content .= "<table  style=\"margin: 0 auto;\"><tr>";
        if ($Info['acc_type'] === 'Operating Executive') $Info['acc_type'] = 'Operator';
        if ($Info['acc_type'] === 'Company Member') $Info['acc_type'] = 'Member';
        $firstLine = array();
        if (!empty($maenna_id)) $firstLine[] = trim(ucwords(strtolower($maenna_id)));
        if (!empty($Info['expertise'])) $firstLine[] = $Info['expertise'];
        if (!empty($Info['acc_type'])) $firstLine[] = $Info['acc_type'];
        if (!empty($Info['years'])) $firstLine[] = $Info['years'];
        $content .= '<td><i>' . implode(', ', $firstLine) . '</i></td>';
        $content .= '<td align="right"><i>' . (empty($graduate) ? $undergraduate : $graduate) . '</i></td>';
        $content .= "</tr>";
        $content .= '<tr><td></table><div style="margin-top:3px; lo">';
        $content .= '<div class="rateit"
            data-tooltip="' . number_format((float) $ratings['rate'], 2, '.', '') . ' star rating"
            data-rateit-readonly="true"
            data-rateit-starwidth="12"
            data-rateit-starheight="12"
            data-rateit-value="' . number_format((float) $ratings['rate'], 2, '.', '') . '"
            style="margin-top:3px;float:left;"
            data-rateit-step="1">
        </div>
        <span
            id="questmark"
            class="pro_profile"
            style="float: left;margin-top:1px;margin-left:10px;">
                <a href="#" id="get-help-btn-rate">
                    <img
                        src="/themes/maennaco/images/questionmark.png"
                        style="padding-top:5px;">
                </a>
        </span>';
        /*            $content .= '<img class="rate_user" style="cursor:pointer;float:left;" data-rate="1" width="20" src="/themes/maennaco/images/th_up.png"><img class="rate_user" data-rate="0"  style="cursor:pointer;margin-left:20px;float:left;margin-top:5px;" width="20" src="/themes/maennaco/images/th_down.png">';*/
        if ($ratings['total'] == 0) {
            $content .= "<div style='float:left;font-size:12px;vertical-align: top;margin-top:2px;margin-left:20px;'>No reviews</div>";
        } else $content .= "
        <div
            data-tooltip='" . (((int) $ratings['total']) >= 5 ? $ratings['total'] . ' total reviews' : ' Read reviews') . "'
            data-user='".encrypt_string($AccessObj->uid)."'
            data-uid='" . $_REQUEST['id'] . "'
            style='float:left; " . ($_REQUEST['id'] != $AccessObj->uid ? "margin-left:20px;font-size:12px;" : '') . "'
            class='get_reviews'>".(((int) $ratings['total']) >= 5 ? '('.$ratings['total'].')'  : '')." Read reviews</div>";

        if (canRateUser($_REQUEST['id'], $AccessObj->uid)) $content .= "<div class='add_review'>Add review</div>";

        $userService = new Clewed\User\Service();
        if(!empty($Info['hourly_rate']) && ($AccessObj->uid == $pid || $userService->isSuperAdmin($AccessObj->uid))) {
            $hourlyRate = number_format((float) $Info['hourly_rate'], 0);
            $content .= "<div
                data-tooltip=\"Your hourly rate is private only visible to Clewed. It is not applicable for projects which are priced as products but will help us estimate cost.\"
                style='float:right;font-style:italic;font-size:14px;'>Hourly Rate: $" . round($hourlyRate * 1.25) . "</div>";
        }
        $content .= "</div>";

        ob_start();
        echo displayExpertReviewForm($_REQUEST['id']);

        $content .= ob_get_clean();

        $content .= "</div>";
        $content .= "</div>";
        $content .= '
            <script type="text/javascript">

            	$(document).ready(function(){

				   $("#get-help-btn-rate").click(function(e) {
                        e.preventDefault();
                      $("#get-help-dialog-rate").dialog({
                        modal: true,
                        autoOpen: true,
                        resizable: false,
                        width: 400,
                        title: "Professional rating system",
                        buttons: {
                          Close: function() {
                            $(this).dialog("close");
                          }
                        }
                      });
                    });
					});
			</script>
			<div id="get-help-dialog-rate" style="display:none">
        	  <p>Users can only rate experts for services received. Clewed\'s rating weighs 50% initially until the experts has over 10 user feedback.</p>
            </div>
			';
    }
    $Block['body'] = content_box($Block['title'], $content);
    if ($op == 'write' || $pid == $AccessObj->uid) {
        $Block['body'] .= <<< END
            <script type="text/javascript">

            	$(document).ready(function(){

                    $('#get-help-btn-pro').click(function(e) {
                        e.preventDefault();
                      $("#get-help-dialog-pro").dialog({
                        modal: true,
                        autoOpen: true,
                        resizable: false,
                        title: 'Your Profile',
                        buttons: {
                          Close: function() {
                            $(this).dialog("close");
                          }
                        }
                      });
                    });

                    $(".show_more_pro").click(function (ev){

                    ev.preventDefault();

                    box = $(this).parent().parent();
                    box.find("p.limited").remove();
                    box.parent().find("div.long").show();

                    });

                });
            </script>

           	<div id="get-help-dialog-pro" style="display:none">
        	  <p>We’re looking for people with the hands-on experience, analytical rigor and education to give them greater perspective on time-tested business ideas and frameworks. It is important that professionals thoroughly understand the economic models and history of the industries in which they seek to advise, and that they can offer fresh solutions to emerging problems in those industries. As a Clewed professional, you will be expected to recommend direct actions that companies can take to grow earnings and adapt to changes that might affect their underlying economics.</p>

        	  <p>Please answer the questions below in 200 words or less. Industry and management view sections are optional. Click “Edit” to see the questions. All your information must be professionally presented in clear, correct English. You may not include your brand name or advertise any outside business. Clewed may edit your profile information as needed for consistency and relevance.</p>



            </div>


END;
    }
    return $Block;
}

function pro_profile_panel($op = null) {
    global $AccessObj;
    $editorid = $AccessObj->uid;
    $id = sget($_REQUEST, 'id');
    if (empty($id)) $id = $editorid;
    $time = time();
    $Block['title'] = "USER PROFILE";
    $section = __FUNCTION__;
    $panel = __FUNCTION__;
    $content = '';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    if (empty($op)) {
        return '';
    }
	elseif ($op && $op != 'update')
	{
        $mgmt_level = $experties = $industry = $industry2 = $school = $degree = '';
        $sql = "SELECT * FROM maenna_people WHERE pid = %d LIMIT 1";
        $result = db_query($sql, array($id));
        $Maenna_people = db_fetch_array($result);
        $yearwork = sget($Maenna_people, 'yearwork');
        $industry = sget($Maenna_people, 'industry');
        $industry2 = sget($Maenna_people, 'industry2');
        $profile = sget($Maenna_people, 'profile');
        $sql = "SELECT * FROM maenna_people_data
                WHERE data_type = '%s' AND pid = %d ORDER BY access DESC LIMIT 1";
        $result = db_query($sql, array('employment', $id));
        $Row = db_fetch_object($result);
        if ($Row) {
            if ($Row->data_value3) {
                //management level
                $mgmt_level = $Row->data_value3;
            }
            if ($Row->data_value2) {
                //experties
                $experties = $Row->data_value2;
            }
        }
        $sql = "SELECT * FROM maenna_people_data
                WHERE data_type = '%s' AND pid = %d ORDER BY access DESC LIMIT 1";
        $result = db_query($sql, array('education', $id));
        $Row = db_fetch_object($result);
        if ($Row) {
            $school = $Row->data_value;
            $degree = $Row->data_value2;
        }
        ////
        $content .= "<form method=post action='/account'>";
        $content .= "<table class='table-list'>";
        // role - ROW 1
        /*
          $rid = userRoleId($id);
         $_Roles = _pro_roles();
         $options = option_code($_Roles, $rid);
         $content .= "<tr>
                         <td style='width:200px;'>PROFESSIONAL TYPE:</td>
                         <td><select name='rid'>$options</select></td>
                     </tr>";
        */
        // year started work -ROW2
        $_YEARS = _Year(50);
        $options = option_code($_YEARS, $yearwork);
        $content .= "<tr>
                        <td>YEAR STARTED WORKING:</td>
                        <td><select name='year'>$options</select></td>
                    </tr>";
        // industry
        $options = Options_industry($industry);
        $options_2 = Options_industry($industry2);
        $content .= "<tr>
                        <td>INDUSTRY 1:</td>
                        <td><select name='industry'><option></option>$options</select><br /></td>
                    </tr>";
        $content .= "<tr>
                        <td>INDUSTRY 2:</td>
                        <td><select name='industry2'><option></option>$options_2</select></td>
                    </tr>";
        /*$content .= "<tr>
                        <td>LATEST EMPLOYMENT(company):</td>
                        <td><input type=text name=company value='' /></td>
                    </tr>";
        */
        $options = Options_experties($experties);
        $content .= "<tr>
                        <td>EXPERTISE:</td>
                        <td><select name='experties'><option></option>$options</select></td>
                    </tr>";
        $options = Options_mlvl($mgmt_level);
        $content .= "<tr>
                        <td>MANAGEMENT LEVEL:</td>
                        <td><select name='mlevl'>$options</select></td>
                    </tr>";
        $content .= "<tr>
                        <td>SCHOOL:</td>
                        <td><input type=text name=school value='$school' /></td>
                    </tr>";
        $options = Options_degree($degree);
        $content .= "<tr>
                        <td>DEGREE(highest):</td>
                        <td><select name='degree'>$options</select></td>
                    </tr>";
        /*-------*/
        $content .= "<tr>
                        <td>PROFILE:</td>
                        <td><textarea name='profile' style='width:90%;height:120px;' maxlength=254>$profile</textarea></td>
                    </tr>";
        $content .= "</table>";
        $content .= hidden_post_values(array('tab', 'page', 'id'));
        $content .= "<input type=hidden name=section value='$panel' />
                        <input type=hidden name=update_section value='$panel' />
                        <input type=submit name=submit value='submit' class=button />
                        <a href='$redirect' class=button>cancel</a>
        </form>";
        $sql_schedule = "SELECT pid, discussion_status FROM maenna_people WHERE pid=" . $id;
        $dis_query = mysql_query($sql_schedule);
        $result_dis = mysql_fetch_object($dis_query);
        $content = "<style>

			#edit-nex, #edit-publish {float:left;margin-right:300px;}
			//.statuson {float: right; margin-right: 314px !imporatant; }
			.ptit {font-family: 'Lato Bold'; font-size:14px;}
		</style>";
        $style = '<div style="height: 32px; /*width: 602px; */ background-color:#91CAD9;margin-bottom:23px;"><span style="margin-left:16px; color:#ffffff;line-height: 32px;">
                EDIT PROFESSIONAL INFORMATION</span></div>
                <style>
                #edit-profile{
                    /*margin-bottom:27px !important;
                }*/
                #edit-graduate {
                    margin-bottom: -15px !important;
                }
                #edit-description{
                   height: 84px !important;
                }
                select,
                 label {
                    display: table-cell;
                    margin: 3px;
                }
                </style>';
        echo $style;
        /*		if($result_dis->discussion_status == 1) {
                        echo $style;
                echo "<div style='margin-bottom:20px;margin-top:23px;'><p class='ptit'>Professional Discussion Tool</p> Turn professional Discussions tool <span class='statuson2'><a href='javascript:void(0);' style='cursor:pointer;' onclick='discussion($result_dis->pid, 1);'> OFF</a></span>
                <p>Current status: <span class='statuson1'>ON</span></p></div>";
                }else{
                        echo $style;
                            echo "<div style='margin-bottom:20px;margin-top:23px;><p class='ptit'>Professional Discussion Tool</p> Turn professional Discussions tool <span class='statuson2'><a href='javascript:void(0);' style='cursor:pointer;' onclick='discussion($result_dis->pid, 0);'>ON</a></span>
                    <p>Current status: <span class='statuson1'>OFF</span></p></div>";
                }*/
        $content .= "
        <script type='text/javascript'>

        $(document).ready(function() {
            $('#follow_pro_exp').click(function() {
               clickObj = $(this);
               clickObj.prop('disabled', true);
               if ($('#edit-expertise-match').val() === '') {

               alert ('Please select one Expertise from the list.');
               clickObj.prop('disabled', false);
               return false;
               }

               else  {
               if (confirm('Continue to make professionals with the selected Expertise followers?')) {


                var uid = " . $_REQUEST['id'] . ";
                var m ='" . md5($_REQUEST['id'] . "kyarata75") . "';
                var exp = $('#edit-expertise-match').val();

                $.post('themes/maennaco/includes/follower.php?type=batchFollow&stype=follow_pro_exp', {uid: uid,m:m,exp:exp}, function(response){
                    alert(response);
                    clickObj.prop('disabled', false);
                });


               }

               else { clickObj.prop('disabled', false); return false; }
               }

               });

            $('#follow_cmp_ind').click(function() {
               clickObj = $(this);
               clickObj.prop('disabled', true);
               if ($('#edit-industry-match').val() === '') {

               alert ('Please select one of Industries from the list.');
               clickObj.prop('disabled', false);
               return false;
               }

               else  {
               if (confirm('Continue to make companies with the selected Industry followers?')) {


                var uid = " . $_REQUEST['id'] . ";
                var m ='" . md5($_REQUEST['id'] . "kyarata75") . "';
                var exp = $('#edit-industry-match').val();

                $.post('themes/maennaco/includes/follower.php?type=batchFollow&stype=follow_cmp_ind', {uid: uid,m:m,exp:exp}, function(response){
                    alert(response);
                    clickObj.prop('disabled', false);
                });


               }

               else { clickObj.prop('disabled', false); return false; }
               }

               });

            $( '#examples-summary').dialog({
                          autoOpen: true,
                          width: 800,
                          title: 'Profile Summary example',
                          height:250,
                          buttons: { 'Ok': function() { $(this).dialog('close'); }},
                          closeOnEscape: true,
                          autoOpen: false,
                          modal:true,
                          resizable: false
                      });


            $('#get-summary-example').click(function(){

                $('#examples-summary').dialog('open');
        });

            $('#maenna-forms-pro-form-page2').submit(function(e) {
         var self = this;
         e.preventDefault();
            if(validate()){
         self.submit();
     }
     else (alert('You cannot leave empty fields. Please sumbit empty fields.'));
   });

	function validate() {

		var status = true;

	   $('#maenna-forms-pro-form-page2 :input').each(function() {

		$(this).attr('style','background-color:none;');



	   if ($(this).attr('id') == 'edit-pro-type' || $(this).attr('id') == 'edit-first-name' || $(this).attr('id') == 'edit-last-name' ) {

			if ($(this).val() == '') {

				 $(this).attr('style','background-color:rgb(255, 228, 228) !important');
				 //alert($(this).attr('id'));
				 status = false;

			}

	   }



	   });
	   return status;

	}
        });

        </script>
        ";
        $content.= '<style>
		.questmark-investor-type {
			display: inline-block;
			padding: 18px 5px 0px;
		}
		</style>';
		include __DIR__ . '/html_blocks/investor_type_text.php';
        $content .= '<div id="examples-summary" style="font-family:\'LatoRegular\';display:none;">
            <span style="display:block;margin-bottom:16px;color: #00a2bf;font-family: \'Lato Black\';">Example</span>
<p style="line-height:20px;">30+ years in Finance / CFO related roles in high tech manufacturing companies internationally. CFO of public and venture-backed companies. Prescient Applied Intelligence, GE, Ford, Infotron Systems.
</p></div>
';
        $content .= drupal_get_form('maenna_forms_pro_form_page2');
    }
	elseif ($op == 'update') {
        //$rid = sget($_REQUEST, 'rid');
        $yearwork = sget($_REQUEST, 'year');
        $industry = sget($_REQUEST, 'industry');
        $industry2 = sget($_REQUEST, 'industry2');
        $experties = sget($_REQUEST, 'experties');
        $mlvl = sget($_REQUEST, 'mlevl');
        $school = sget($_REQUEST, 'school');
        $degree = sget($_REQUEST, 'degree');
        $id = sget($_REQUEST, 'id');
        $profile = sget($_REQUEST, 'profile');
        if (empty($id)) return '';
        $sql = "UPDATE maenna_people SET    yearwork    = '%s',
                                            industry    = '%s',
                                            industry2   = '%s',
                                            profile     = '%s'
                WHERE pid = %d LIMIT 1";
        if (!db_query($sql, array($yearwork, $industry, $industry2, $profile, $id))) {
            drupal_set_message('Failed to update record - 001', 'error');
            return;
        }
        // experties = data_value2, mlevel = data_value3
        $sql = "UPDATE maenna_people_data SET   data_value2 = '%s',
                                                data_value3 = '%s'
                WHERE data_type = 'employment' AND pid = %d LIMIT 1";
        if (!db_query($sql, array($experties, $mlvl, $id))) {
            drupal_set_message('Failed to update record - 002', 'error');
            return;
        }
        $sql = "UPDATE maenna_people_data SET   data_value = '%s',
                                                data_value2 = '%s'
                WHERE data_type = 'education' AND pid = %d LIMIT 1";
        if (!db_query($sql, array($school, $degree, $id))) {
            drupal_set_message('Failed to update record - 003', 'error');
            return;
        }
        drupal_set_message('Record updated successfully');
        return '';
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function tplSummaryBox($Array = null) {
    global $redirect;
    if (empty($Array)) return '';
    $title = sget($Array, 'title');
    $panel = sget($Array, 'panel');
    $box = sget($Array, 'style');
    $pid = sget($Array, 'pid');
    $text = sget($Array, 'text');
    if (empty($text)) $text = '-';
    $textExcerpt = contentExcerpt($text);
    $more = '';
    if ($text != $textExcerpt) {
        $more = "<div style='text-align:right'><a href='$redirect&id=$pid&panel=pro_addInfo_panel&key=$panel'>More</a></div>";
    }
    return "<br><div class=row $box><h4 style='font-weight:bold;'>$title</h4><p>$textExcerpt</p> $more</div>";
}

function pro_experties($op = null) {
    global $user;
    global $AccessObj;
    global $ifAdmin;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $user_type = getUserTypeById($pid);
    if (in_array($user_type, array('admin', 'super_admin'))) {
        $Block['title'] = '<span style="margin-left:7px;text-transform:none;">Admin user has no profile</span>';
        $Block['body'] = content_box($Block['title'], '');
        return $Block;
    }
    $time = time();
    $Block['title'] = "<span style='font: 14px LatoRegular;color: #686B70;font-weight: bold;'>WHY YOU SHOULD LISTEN TO " . strtoupper(getProId($pid)) . "</span>";
    $data_type = 'addinfo';
    $data_attr = 'experties';
    global $redirect;
    $section = __FUNCTION__;
    $content = '';
    $message = '';
    $panel = 'profilecontent';
    if (empty($op)) {
        return '';
    } elseif ($op == 'title') {
        return $Block['title'];
    } elseif ($op) {
        $sql = "SELECT * FROM maenna_people_data WHERE pid = %d AND data_type = '%s' AND data_attr = '%s' AND data_value3 = 'approved' ORDER BY access DESC LIMIT 1";
        if ($AccessObj->user_type == 'super' || $ifAdmin || $user->uid == $pid) {
            $sql = "SELECT * FROM maenna_people_data WHERE pid = %d AND data_type = '%s' AND data_attr = '%s' ORDER BY access DESC LIMIT 1";
        }
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        $Row = db_fetch_object($result);
        if ($Row) {
            $content = strip_tags($Row->data_value2, "<strong><em><br>");
            $content = htmlentities($content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = "<div class='exp_text'><p>" . $content;
            $content = _filter_autop(html_entity_decode($content)) . "</p></div>";
            $content = duplicate_br($content);
            if ($Row->data_value3 == 'rejected') {
                if ($user->uid == $pid && time() > ($Row->access + (90 * 24 * 60 * 60))) {
                    $Block['title'] = '<span style="text-transform:none;">Why you should listen to ' . ucfirst(strtolower(getProId($pid))) . '</span>';
                }
                $message = "";
            } elseif ($Row->data_value3 == 'approved' && $user->uid == $pid && time() > ($Row->access + (90 * 24 * 60 * 60))) {
                $Block['title'] = '<span style="text-transform:none;">Why you should listen to ' . ucfirst(strtolower(getProId($pid))) . '</span>';
            } elseif (!empty($Row->data_value4)) {
                $message = "<div class=\"messages warning\">" . $Row->data_value4 . "</div>";
            }
        }
        //echo $content;
        //$content = contentExcerpt2($content, $redirect . "&id=$pid&panel=${panel}&view=detail&section=$section");
        /* This javascript function needs to be called only once for the profile page
         * that's why it is not in the other functions (industry, management view)
         */
        //$content .= "<script>$('document').ready(function(){pro_profile_more_link();})</script>";
        if ($AccessObj->user_type == 'super' || $ifAdmin || $user->uid == $pid) {
            if ($Row->access) {
                $statusTooltip = "On " . date("F d Y @ h:i:s A", $Row->access);
                $Block['title'] .= "<span data-tooltip='$statusTooltip' style='cursor:pointer; color: #888888;font-size: 11px;font-style: italic;font-weight: normal;float:right;padding-right:50px;padding-top:2px;'>" . ucfirst($Row->data_value3) . "</span>";
            }
            $Block['title'] .= "<div class='editbtn spaced'><a href='$redirect&id=$pid&panel=${panel}&view=edit&section=$section&box=experties' class=tool>Edit</a></div>";
        }
    }
    $Block['body'] = content_box($Block['title'], $message . $content);
    return $Block;
}

/* function pro_accomplishments($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "KEY ACCOMPLISHMENTS";

    $data_type = 'addinfo';
    $data_attr = 'accomplishments';
    global $redirect;
    $section = __FUNCTION__;
    $content = '';
    $panel = 'single';
    if($op == 'write'){
        $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit&section=$section&id=$pid' class=tool>Edit</a></div>";
    }
    if(empty($op) ) return '';
    elseif($op == 'title')
    {
        return $Block['title'];
    }elseif($op)
    {
        $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' and data_attr = '%s'";
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = contentExcerpt($content, $redirect . "&id=$pid&panel=${panel}&view=detail&section=$section");
        }
    }else{

    }

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function pro_personality($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "GUIDING PRINCIPLES";

    $data_type = 'addinfo';
    $data_attr = 'personality';
    global $redirect;
    $section = __FUNCTION__;
    $content = '';
    $panel = 'single';
    if($op == 'write') $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit&section=$section&id=$pid' class=tool>Edit</a></div>";
    if(empty($op)) return '';
    elseif($op == 'title')
    {
        return $Block['title'];
    }elseif($op)
    {
        $sql = "select * from maenna_people_data where pid = %d and data_type = '%s' and data_attr = '%s'";
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = contentExcerpt($content, $redirect . "&id=$pid&panel=${panel}&view=detail&section=$section");
        }
    }else{

    }

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
} */
function pro_industry_view($op = null) {
    global $user;
    global $AccessObj;
    global $ifAdmin;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "<span style='font: 14px LatoRegular;color: #686B70;font-weight: bold;'>INDUSTRY VIEW</span>";
    $data_type = 'addinfo';
    $data_attr = 'industryview';
    global $redirect;
    $section = __FUNCTION__;
    $content = '';
    $message = '';
    $panel = 'profilecontent';
    if (empty($op)) return '';
    if ($op == 'title') {
        return $Block['title'];
    } elseif ($op) {
        $sql = "SELECT * FROM maenna_people_data WHERE pid = %d AND data_type = '%s' AND data_attr = '%s'  AND data_value3 = 'approved' ORDER BY access DESC LIMIT 1";
        if ($AccessObj->user_type == 'super' || $ifAdmin || $user->uid == $pid) {
            $sql = "SELECT * FROM maenna_people_data WHERE pid = %d AND data_type = '%s' AND data_attr = '%s' ORDER BY access DESC LIMIT 1";
        }
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        $Row = db_fetch_object($result);
        if ($Row) {
            if ($Row->data_value2 == '') return '';
            $content = strip_tags($Row->data_value2, "<strong><em>");
            $content = htmlentities($content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = _filter_autop(html_entity_decode($content));
            $content = duplicate_br($content);
            if ($Row->data_value3 == 'rejected') {
                if ($user->uid == $pid && time() > ($Row->access + (90 * 24 * 60 * 60))) {
                }
                $message = "";
            } elseif ($Row->data_value3 == 'approved' && $user->uid == $pid && time() > ($Row->access + (90 * 24 * 60 * 60))) {
            } elseif (!empty($Row->data_value4)) {
                $message = "<div class=\"messages warning\">" . $Row->data_value4 . "</div>";
            }
        } else {
            return '';
        }
        $content = contentExcerpt2($content, $redirect . "&panel=${panel}&view=detail&section=$section&id=$pid", 800);
        if ($AccessObj->user_type == 'super' || $ifAdmin || $user->uid == $pid) {
            $statusTooltip = "On " . date("F d Y @ h:i:s A", $Row->access);
            $Block['title'] .= "<span data-tooltip='" . $statusTooltip . "' style='cursor:pointer; color: #888888;font-size: 11px;font-style: italic;font-weight: normal;float:right;padding-right:50px;padding-top:2px;'>" . ucfirst($Row->data_value3) . "</span>";
            $Block['title'] .= "<div class='editbtn spaced'><a href='$redirect&panel=${panel}&view=edit&section=$section&id=$pid&box=industryview' class=tool>Edit</a></div>";
        }
    } else {
    }
    $Block['body'] = content_box($Block['title'], $message . $content);
    return $Block;
}

function pro_mgmt_view($op = null) {
    global $user;
    global $AccessObj;
    global $ifAdmin;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "<span style='font: 14px LatoRegular;color: #686B70;font-weight: bold;'>MANAGEMENT VIEW</span>";
    $data_type = 'addinfo';
    $data_attr = 'mgmtview';
    global $redirect;
    $section = __FUNCTION__;
    $content = '';
    $message = '';
    $panel = 'profilecontent';
    if (empty($op)) return '';
    if ($op == 'title') {
        return $Block['title'];
    } elseif ($op) {
        $sql = "SELECT * FROM maenna_people_data WHERE pid = %d AND data_type = '%s' AND data_attr = '%s' AND data_value3 = 'approved' ORDER BY access DESC LIMIT 1";
        if ($AccessObj->user_type == 'super' || $ifAdmin || $user->uid == $pid) {
            $sql = "SELECT * FROM maenna_people_data WHERE pid = %d AND data_type = '%s' AND data_attr = '%s' ORDER BY access DESC LIMIT 1";
        }
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        $Row = db_fetch_object($result);
        if ($Row) {
            if ($Row->data_value2 == '') return '';
            $content = strip_tags($Row->data_value2, "<strong><em>");
            $content = htmlentities($content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = _filter_autop(html_entity_decode($content));
            $content = duplicate_br($content);
            if ($Row->data_value3 == 'rejected') {
                if ($user->uid == $pid && time() > ($Row->access + (90 * 24 * 60 * 60))) {
                }
                $message = "";
            } elseif ($Row->data_value3 == 'approved' && $user->uid == $pid && time() > ($Row->access + (90 * 24 * 60 * 60))) {
            } elseif (!empty($Row->data_value4)) {
                $message = "<div class=\"messages warning\">" . $Row->data_value4 . "</div>";
            }
        } else {
            return '';
        }
        $content = contentExcerpt2($content, $redirect . "&panel=${panel}&view=detail&section=$section&id=$pid", 800);
        if ($AccessObj->user_type == 'super' || $ifAdmin || $user->uid == $pid) {
            $statusTooltip = "On " . date("F d Y @ h:i:s A", $Row->access);
            $Block['title'] .= "<span data-tooltip='" . $statusTooltip . "' style='cursor:pointer;color: #888888;font-size: 11px;font-style: italic;font-weight: normal;float:right;padding-right:50px;padding-top:2px;'>" . ucfirst($Row->data_value3) . "</span>";
            $Block['title'] .= "<div class='editbtn spaced'><a href='$redirect&panel=${panel}&view=edit&section=$section&id=$pid&box=mgmtview' class=tool>Edit</a></div>";
        }
    } else {
    }
    $Block['body'] = content_box($Block['title'], $message . $content);
    return $Block;
}

function single($op = null) {
    global $user;
    global $AccessObj;
    global $redirect;
    global $ifAdmin;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    $panel = "single";
    $data_type = 'addinfo';
    $section = sget($_REQUEST, 'section');
    //$data_attr = sget($_REQUEST, 'dattr');
    $dataid = '';
    $content = '';
    switch ($section) {
        case "pro_experties":
            $title = "DOMAIN EXPERTISE";
            $data_attr = 'experties';
            break;
        case "pro_accomplishments":
            $title = "KEY ACCOMPLISHMENTS";
            $data_attr = 'accomplishments';
            break;
        case "pro_personality":
            $title = "PERSONALITY";
            $data_attr = 'personality';
            break;
        case "pro_industry_view":
            $title = "INDUSTRY VIEW";
            $data_attr = 'industryview';
            break;
        case "pro_mgmt_view":
            $title = "MANAGEMENT VIEW";
            $data_attr = 'mgmtview';
            break;
    }
    $Block['title'] = $title;
    if (empty($op)) return '';
    if ($op && $op != 'update') {
        $text = '';
        $sql = "SELECT * FROM maenna_people_data WHERE pid = %d AND data_type = '%s' AND data_attr = '%s'";
        $result = db_query($sql, array($pid, $data_type, $data_attr));
        $Row = db_fetch_object($result);
        if ($Row) {
            $text = $Row->data_value2;
            $admin_comment = $Row->data_value4;
            $dataid = $Row->dataid;
            $current_status = ((empty($Row->data_value3)) ? 'pending' : $Row->data_value3);
        }
        $view = sget($_REQUEST, 'view');
//        if($view == 'edit' && $op == 'write'){
        if ($view == 'edit') {
            $user_type = $AccessObj->user_type;
            $Block['title'] = "Edit - " . $Block['title'] . ' <i>(' . $current_status . ')</i>';
            $content = "<form method=post action='/account'>";
            $content .= "<div style=\"line-height:14px;font-size:14px;color:gray;margin-top:10px;\">"
                . pro_section_instructions($section)
                . "</div>";
            $content .= "<table class='no-border'>";
            $content .= "    <tr>";
            $content .= "        <td><textarea name='content' style='width:98%;height:400px;'>$text</textarea></td> ";
            $content .= "    </tr>";
            if ($AccessObj->user_type == 'super' || $ifAdmin) {
                $content .= "    <tr>";
                $content .= "        <td>
                <div>Short comment</div>
                <textarea maxlength=\"255\" class='mceNoEditor' name='admin_comment' style='width:98%;height:70px;'>$admin_comment</textarea>
                                  </td> ";
                $content .= "    </tr>";
            }
            $content .= "    <tr>";
            $content .= "        <td>";
            if ($user_type == 'people') {
                $content .= '<input type="submit" name="save" value="Save" class="button" /> ';
                $content .= '<input type="submit" name="submit" value="Submit" class="button" /> ';
            } else if ($AccessObj->user_type == 'super' || $ifAdmin) {
                $content .= '<input type="submit" name="save" value="Save" class="button" /> ';
                $content .= '<input type="submit" name="publish" value="Publish" class="button" /> ';
                $content .= '<input type="submit" name="reject" value="Reject" class="button" /> ';
            }
            $content .= "          <a href='$redirect&id=$pid' class=button>Cancel</a>";
            $content .= "        </td>";
            $content .= "    </tr>";
            $content .= "</table>";
            $content .= "    <input type=hidden name=dataid value='$dataid' />";
            $content .= "    <input type=hidden name=update_section value='$panel' />";
            $content .= "    <input type=hidden name=view value='detail' />";
            $content .= hidden_post_values(array('tab', 'page', 'panel', 'id', 'section'));
            $content .= "</form>";
        } elseif ($view == 'detail') {
            if ($op == 'write') $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit&section=$section&id=$pid' class=tool>Edit</a></div>";
            $content = nl2br(htmlentities($text, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $content = "<div class=entry>
                            <div class=entry-content>" . _filter_autop(html_entity_decode($content)) . "</div>
                        </div>
                        <div class=backbtn><a href='$redirect&id=$pid&section=$section' class='button'>back</a></div>";
        }
    } elseif ($op == 'update') {
        $data_value2 = sget($_REQUEST, 'content');
        $pro_id = sget($_REQUEST, 'id');
        $dataid = sget($_REQUEST, 'dataid');
        $submit = sget($_REQUEST, 'submit');
        $publish = sget($_REQUEST, 'publish');
        $reject = sget($_REQUEST, 'reject');
        $adm_cmt = sget($_REQUEST, 'admin_comment');
        $status = $Row->data_value3;
        if (!empty($submit)) {
            $status = 'submitted';
        } elseif (!empty($publish) && ($AccessObj->user_type == 'super' || $ifAdmin)) {
            $status = 'approved';
        } elseif (!empty($reject) && ($AccessObj->user_type == 'super' || $ifAdmin)) {
            $status = 'rejected';
        }
        if (empty($pro_id)) {
            drupal_set_message("Invalid pid", 'error');
        } else {
            if ($dataid) {
                $sql = "update maenna_people_data set data_value2 = '%s', access='$time',editorid=$editorid,data_value3='%s',data_value4='%s' where data_attr = '%s' and dataid = %d limit 1";
                $DBValues = array($data_value2, $status, $adm_cmt, $data_attr, $dataid);
            } else {
                $sql = "INSERT INTO maenna_people_data (pid, access, data_type, data_attr, data_value2, data_value3, data_value4, editorid ) VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', %d)";
                $DBValues = array($pid, $time, $data_type, $data_attr, $data_value2, $status, $adm_cmt, $editorid);
            }
            if (db_query($sql, $DBValues)) {
                drupal_set_message("Data record is updated");
            } else drupal_set_message("Failed to update record", 'error');
        }
        return '';
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function pro_section_instructions($section) {
    switch ($section) {
        case "experties":
            return "Write a brief summary describing your core skills in managing a business or a process. Describe what
                  industries this skill may be most relevant to and include an example as appropriate.";
            break;
        case "industryview":
            return "Discuss industry/industries you know well from a fundamental, long-term perspective.";
            break;
        case "mgmtview":
            return "What types of managers and operators do you admire the most? If possible, provide examples of companies you have observed to have such managers who contribute to their success.";
            break;
    }
    return "";
}

function profilecontent($op = null) {
    global $user;
    global $AccessObj;
    global $redirect;
    global $ifAdmin;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'id');
    $time = time();
    $panel = "profilecontent";
    $data_type = 'addinfo';
    $section = sget($_REQUEST, 'section');
    //$data_attr = sget($_REQUEST, 'dattr');
    $dataid = '';
    $content = '';
    $attrs = array(
        'experties'    => 'DOMAIN EXPERTISE',
        'industryview' => 'INDUSTRY VIEW',
        'mgmtview'     => 'MANAGEMENT VIEW'
    );
    if (empty($op)) return '';
    if ($op == 'update') {
        $attr_records = array();
        $sql = "SELECT dataid,data_attr,data_value2,data_value3 FROM maenna_people_data WHERE pid = %d AND data_type = '%s' AND data_attr IN ('" . implode("','", array_keys($attrs)) . "') GROUP BY dataid,data_attr,data_value3 ORDER BY access DESC";
        $result = db_query($sql, array($pid, $data_type));
        while (($Row = db_fetch_object($result)) !== false) {
            if (!isset($attr_records[$Row->data_value3][$Row->data_attr])) $attr_records[$Row->data_value3][$Row->data_attr] = array("id" => $Row->dataid, "content" => $Row->data_value2);
        }
        $ok = true;
        foreach ($attrs as $attr => $attr_name) {
            $data_value2 = sget($_REQUEST, 'content_' . $attr);
            $pro_id = sget($_REQUEST, 'id');
            $dataid = sget($_REQUEST, 'dataid_' . $attr);
            $submit = sget($_REQUEST, 'submit');
            $publish = sget($_REQUEST, 'publish');
            $reject = sget($_REQUEST, 'reject');
            $adm_cmt = sget($_REQUEST, 'admin_comment_' . $attr);
            if (!empty($submit)) {
                $status = 'submitted';
            } elseif (!empty($publish) && ($AccessObj->user_type == 'super' || $ifAdmin)) {
                $status = 'approved';
            } elseif (!empty($reject) && ($AccessObj->user_type == 'super' || $ifAdmin)) {
                $status = 'rejected';
            }
            if (isset($_POST['notify'])) {
                $sql = "SELECT firstname,mail FROM users u JOIN maenna_people mp ON u.uid = mp.pid WHERE uid = %d ";
                $result = db_query($sql, array($pro_id));
                $Row = db_fetch_object($result);
                $firstname = ucfirst($Row->firstname);
//A.R.
                $content = <<< END
Hi $firstname,<br><br>

There is an information update in relation to your profile on Clewed.<b>
Please login to your account at <a href="https://www.clewed.com/">www.clewed.com</a> to review and proceed.<br><br>

The Clewed team.
END;
                $headers = "From: noreply@clewed.com\r\n";
                $headers .= "MIME-Version: 1.0\r\n";
                $headers .= "Content-Type: text/html\r\n";
                mail($Row->mail, 'Your Clewed account is updated', $content, $headers) or die('damjan');
            }
            if (empty($pro_id)) {
                drupal_set_message("Invalid pid", 'error');
            } else {
                if (!empty($publish)) {
                    if (count($attr_records['approved'][$attr]) > 0) {
                        $sql = "UPDATE maenna_people_data SET data_value2 = '%s', access ='%s',editorid = %d, data_value4 = '%s'
                            WHERE  dataid = %d";
                        $DBValues = array($data_value2, time(), $editorid, $adm_cmt, $attr_records['approved'][$attr]['id']);
                    } else {
                        $sql = "INSERT INTO maenna_people_data (pid, access, data_type, data_attr, data_value2, data_value3, data_value4, editorid ) VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', %d)";
                        $DBValues = array($pid, $time, $data_type, $attr, $data_value2, 'approved', $adm_cmt, $editorid);
                    }
                } else {
                    $sql_hlp = "SELECT dataid FROM maenna_people_data
                            WHERE pid = %d AND data_type = '%s' AND data_attr = '%s' AND data_value3 <> '%s'
                            ORDER BY access DESC LIMIT 1";
                    $result_hlp = db_query($sql_hlp, array($pid, 'addinfo', $attr, 'approved'));
                    $Row_hlp = db_fetch_object($result_hlp);
                    if ($Row_hlp->dataid) {
                        $sql = "UPDATE maenna_people_data SET data_value2 = '%s', access ='%s',editorid = %d, data_value3 = '%s', data_value4 = '%s'
                            WHERE  dataid = %d";
                        $DBValues = array($data_value2, time(), $editorid, $status, $adm_cmt, $Row_hlp->dataid);
                    } else {
                        $sql = "INSERT INTO maenna_people_data (pid, access, data_type, data_attr, data_value2, data_value3, data_value4, editorid ) VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', %d)";
                        $DBValues = array($pid, $time, $data_type, $attr, $data_value2, $status, $adm_cmt, $editorid);
                    }
                }
                /*            if($dataid) {
                                $sql = "update maenna_people_data set data_value2 = '%s', access='$time',editorid=$editorid,data_value3='%s',data_value4='%s', access='$time' where data_attr = '%s' and dataid = %d";
                                $DBValues = array($data_value2, $status, $adm_cmt, $attr, $dataid);
                            } else {
                                $sql = "insert into maenna_people_data (pid, access, data_type, data_attr, data_value2, data_value3, data_value4, editorid ) values (%d, '%s', '%s', '%s', '%s', '%s', '%s', %d)";
                                $DBValues = array($pid, $time, $data_type, $attr, $data_value2, $status, $adm_cmt, $editorid);
                            }
                            */
                if (!db_query($sql, $DBValues)) {
                    drupal_set_message("Failed to update profile information.", 'error');
                    $ok = false;
                }
            }
        }
        if ($status == 'approved') {
            if (!db_query("UPDATE maenna_people SET discussion_status = 1 WHERE pid = %d", array($pro_id))) {
                drupal_set_message("Failed to approve profile information.", 'error');
            }
            $ok = false;
        }
        if ($ok) drupal_set_message($status == 'approved' ? 'Profile is now visible and discussion tool is turned on for this professional.' : "Profile information updated successfuly.");
        /*      if($AccessObj->user_type == 'super' || $ifAdmin) {
                $q1 = mysql_query("SELECT IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname, email FROM maenna_people WHERE pid = " . ((int) $pro_id));
                $pro_data = mysql_fetch_array($q1);
                $proname = ucfirst($pro_data['firstname']);
                $promail = $pro_data['email'];

                $content = <<< END
        Hi $proname,<br><br>
        Your Clewed profile is updated by Admin.<br>
        Please login to your account at <a href="https://www.clewed.com/">www.clewed.com</a> to review.<br><br>

        Thank you.<br>
        The Clewed team.
        END;
                  $headers = "From: noreply@clewed.com\r\n";
                  $headers .= "MIME-Version: 1.0\r\n";
                  $headers .= "Content-Type: text/html\r\n";
                mail($promail, 'Clewed profile updated', $content, $headers);
              }*/
        header('Location: account?tab=professionals&page=pro_detail&id=' . $_REQUEST['id']);
        return '';
    } else {
        $attr_records = array();
        $sql = "SELECT * FROM maenna_people_data WHERE pid = %d AND data_type = '%s' AND data_attr IN ('" . implode("','", array_keys($attrs)) . "') ORDER BY access DESC";
        $result = db_query($sql, array($pid, $data_type));
        while (($Row = db_fetch_object($result)) !== false) {
            if (!isset($attr_records[$Row->data_attr])) $attr_records[$Row->data_attr] = $Row;
        }
        $view = sget($_REQUEST, 'view');
        if ($view == 'edit') {
            $Block['title'] = 'Edit profile';
            $content = '<div class="edit-tools"><a rel="experties" class="tool edit-btn">|Edit expertise</a> <a rel="industryview" class="tool edit-btn">|Edit Industry view (Optional)</a> <a rel = "mgmtview" class="tool edit-btn">|Edit Management view (Optional)</a></div>';
            $content .= "<form method=post action='/account?tab=professionals&page=pro_detail&id=" . $pid . "'>";
        }
        $i = 0;
        foreach ($attrs as $section => $name) {
            $Row = $attr_records[$section];
            $i++;
            $text = $admin_comment = $dataid = $current_status = '';
            if ($Row) {
                $text = $Row->data_value2;
                $admin_comment = $Row->data_value4;
                $dataid = $Row->dataid;
                $current_status = ((empty($Row->data_value3)) ? 'pending' : $Row->data_value3);
            }
            if ($view == 'edit') {
                if ($section != $_REQUEST['box']) $box_style = "style='display:none';"; else $box_style = '';
                if ($section == 'mgmtview') $rel = 'examples-management'; else if ($section == 'industryview') $rel = 'examples-industry'; else $rel = 'examples-expertise';
                $user_type = $AccessObj->user_type;
                $activeUid = $_REQUEST['id'];
                // $Block['title'] = "Edit - " . $Block['title'].' <i>('.$current_status.')</i>';
                $content .= "<div class='$section' " . $box_style . ">";
                $content .= "<div class='box_title shaded_title'><a name='$section'></a>" . $attrs[$section] . ($current_status ? "($current_status)" : '') . "</div><div style=\"line-height:14px;font-size:14px;color:gray;margin-top:10px;\">" . pro_section_instructions($section) . "</div>";
                $content .= "<table class='no-border'>";
                $content .= "    <tr>";
                $content .= "        <td><textarea max_words='500' name='content_$section' style='width:98%;height:400px;'>$text</textarea><input type=hidden name='dataid_$section' value='$dataid' />


              <div>
                              <span id='questmark' class='pro_profile' style='float:left;'><a style='cursor:pointer;color:#00a2bf !important;' class='get-examples-btn' rel='$rel'>See examples</a></span><span style='font-size:13px;margin-left:5px;'>Maximum 500 words</span>
                              <span style='float:right;padding-right:13px;' class='note'>";
                if ($Row->access) {
                    $content .= "Saved on " . date("F d Y @ h:i:s A", $Row->access);
                } else $content .= "Saved on 00:00:00";
                $content .= "</span>
                               <a href='#' style='float:right;padding-right:13px;' class='tool save-btn' data-holder='content_$section' data-id='$dataid' data-section='$section' data-uid='$activeUid'>Save</a>
                              <a href='#' style='float:right;padding-right:13px;' class='tool review-btn' data-holder='content_$section' data-title='" . $attrs[$section] . "'>Review</a> &nbsp;&nbsp;&nbsp;

            </div>

          </td> ";
                $content .= "    </tr>";
                if ($AccessObj->user_type == 'super' || $ifAdmin) {
                    $content .= "    <tr>";
                    $content .= "        <td>
              <div>Short comment</div>
              <textarea maxlength=\"255\" class='mceNoEditor' name='admin_comment_$section' style='width:98%;height:70px;'>$admin_comment</textarea>
                                </td> ";
                    $content .= "    </tr>";
                }
                $content .= "</table></div>";
            }
        }
        $content .= '            <div id="get-help-pro-examples" style="display:none;">

            <div id="examples-expertise" style="font-family:\'LatoRegular\';display:none;">
            <span style="display:block;margin-bottom:16px;color: #00a2bf;font-family: \'Lato Black\';">Example 1</span>
<p style="line-height:20px;">My expertise is fundamental analysis. I employ macro level analysis to identify industries that are set to enjoy robust times or are about to enter a rough patch. I use bottoms up analysis to analyze the individual merits of a company or to identify those that they may be set to disappoint. It is hard to ignore the macro view because no matter the company, its fortunes in part will be determined by the supply/demand outlook and the competitive environment of the industry.
<p style="line-height:20px;margin-top:16px;">My analytical approach always starts with and focuses on understanding the key revenue and cost drivers of a company. This entails determining what product/service make up the largest part of the revenue/profit mix, understanding historical financials for insight into which aspects (price or volume) is the key variable, learning how much pricing power there is based on the uniqueness of the product.  Then, figuring out the competitive forces in the industry, including the company\'s ability to increase market for its product, and the source of supply (raw materials and inputs) and demand drive the next steps in the research process.</p>
                <p style=line-height:20px;margin-top:16px;">Once the revenue picture is understood then the key cost drivers must be analyzed. Sometimes the product costs are the biggest component while other times employee or marketing costs might be the key cost to analyze. Again the research and data gathering will revolve around identifying the drivers first. After projecting financial results, there is always a process of going back and reviewing estimates made line by line to identify the sources of significant deviation. When there is significant deviation, research must be done to identify the cause as this might give rise to insights that prove useful in recalibrating expected results going forward.
</p>
<span style="display:block;margin-bottom:16px;color: #00a2bf;font-family:\'Lato Black\';">Example 2</span>
<p style="line-height:20px;">While I feel that I am very skilled across nearly all functions, perhaps my greatest strengths lay in strategy development and deployment acquired from more than a decade of experience in various executive roles including CEO and CFO for global multi billion dollar publicly traded companies as well as small privately owned businesses. I initially built these skills early in my career as an operational finance manager. These initial assignments provided fantastic exposure to all areas of a company, allowing me to understand each one not just as a standalone function but how each interacts with the other parts of the company. This is critical with regards to strategy, especially when it comes to deployment. I also conducted a significant amount of competitive and economic research, which are cornerstones for proper strategic planning.</p>
<p style="line-height:20px;margin-top:16px;">
                Later in my career I led Strategy and Process Improvement for the international Flooring division for a $3 billion global leader in the design and manufacture of floors, ceilings and cabinets. I also served as the CEO of the Cabinets division with about $230 million in revenues. Here I was formally responsible for developing the division\'s strategic plan and the tools and processes necessary to ensure world class execution. With regards to the latter, I introduced and taught the organization process improvement skills modeled on the GE work-out program as well as various Lean and Six Sigma tools.</p>
            </div>

             <div id="examples-industry" style="display:none;">

            <span style="display:block;margin-bottom:16px;color: #00a2bf;font-family: \'Lato Black\';">Example 1</span>
<p style="line-height:20px;">Have experience with various industries. Follow companies across a variety of sectors including cyclical and commodity influenced businesses. I have studied transportation, media, airlines, technology, business services as an example.</p><p style="line-height:20px;margin-top:16px;">
The airline industry has always been an interesting study. The industry is caught in the cross-hairs of fierce competition and significant capital requirement. Historically airlines have competed in a market share game that has served to destroy the pricing model causing airlines to consistently operate at a loss. Only during times were fuel prices were low did airlines flourish. Since oil prices climbed to $140/bbl back in 2008, airline management teams have become more disciplined, actually cutting back capacity since then. This combined with the government stepping out of the way of mergers has reduced supply of seats easing some of the pressure on pricing. Evidence of the benefit of the reduced capacity can be seen in the fact that airlines have been reporting profits recently even with oil prices around $100/bbl. This would not have been possible the middle of last decade as the industry struggled with overcapacity. The industry will continue to be competitive going forward since the barriers to entry are low. Each time an airline goes away, a new competitor seems to emerge each with a new business model (JetBlue, Virgin, Spirit, etc). So while the industry is healthier than it has been in a long time, it will remain competitive and constantly be very vulnerable to price competition. If a merger with American into another existing airline is allowed to happen that could further benefit the industry by once again reducing competition. Likely though existing or new competitors would fill some of the routes that would be abandoned due to the merger. I expect the industry to continue to be very cost conscious, seeking ways to continue to generate ancillary revenue via fees and sales of extra products (WiFi for example) or services (access to lounges for example). This cost consciousness combined with reduced capacity should allow airlines to remain profitable assuming the price of oil does not climb much beyond current levels.</p>

<span style="display:block;margin-bottom:16px;color: #00a2bf;font-family: \'Lato Black\';">Example 2</span>
<p style="line-height:20px;">The Building Material industry is just beginning to show signs of improvement after an extended (4+ years) trough in the cycle. I project the industry to largely bounce along the bottom with a slight upward bias in 2012. New construction rates have dipped well below the 1.2 million household unit growth that historically sustained the sector so this area will likely show some growth over the next 5 years despite pending defaults and \'hidden\' housing inventory. 10-15 years, technology will play a greater role.</p><p style="line-height:20px;margin-top:16px;">
There is potential for dramatic change on both the supply side and on the distribution side. Foreign sourcing will continue to play a large role but I expect sourcing to rotate through various nations and China faces increased competition due to wage, currency, and inflationary pressures in their homeland. On the distribution side, there will be further consolidation as 2-step distribution gets replaced by service oriented 1-step distributors. 2-steppers will remain a large factor, but they will carry a narrower product line in the future. Both will also face increased competition from manufacturers selling direct.</p><p style="line-height:20px;margin-top:16px;">
Margins, which have been compressed during the trough in the cycle, will not return to pre-crash levels. Cost control via process improvement and dynamic sourcing models will be imperative to both survive and thrive. Maintaining a low cost position will also be increasingly harder to defend, as the gap has narrowed between the best and the next in line. Differentiating via product offering and service will be increasingly important.</p>

            </div>

             <div id="examples-management" style="display:none;">

                        <span style="display:block;margin-bottom:16px;color: #00a2bf;font-family: \'Lato Black\';">Example 1</span>
            <p style="line-height:20px;">I admire companies that have done something different in sleepy industries where profits and good margins have sometimes been elusive. Amazon and Spirit Airlines are good examples.</p><p style="line-height:20px;margin-top:16px;">
Amazon has done a great job of consistently evolving its business model while remaining true to its core business. The company now has a very defensible position through its brand name and distribution reach. Amazon has successfully expanded its product offering taking advantage of struggling retailers dealing with the high cost of maintaining a physical retail foothold.
Spirit Airlines has brought a new business model to the airline industry. In an industry where low cost airlines were prevalent, Spirit brought a new way to price airline seats and a cost structure that is the envy of the industry. They have turned flying into a complete ala carte model by pricing the seat and making everything else an add-on.
            </p>
            <span style="display:block;margin-bottom:16px;color: #00a2bf;font-family: \'Lato Black\';">Example 2</span>
           <p style="line-height:20px;">I learned that every situation faced is unique. And so, the need for situational management skills in modern business is key. A good manager is one who listens to customers and employees in equal accord, in order to tailor a custom strategy for each situation and goal, with the hopes of building a "well-oiled machine" that operates efficiently on the strengths of all of its parts.</p>

            </div>
            </div>';
        if ($view == 'edit') {
            $content .= "<table class='no-border'><tr>";
            $content .= "        <td>";
            if ($user_type == 'people') {
                $content .= '<input type="submit" log="true" log-action="1" log-type="pro_profile_change" name="submit" value="Submit" class="button" /> ';
            } else if ($AccessObj->user_type == 'super' || $ifAdmin) {
                $content .= '<input type="submit" log="true" log-action="2" log-type="pro_profile_change" name="publish" value="Approve" onClick="return confirm(\'Please confirm you have completed the profile summary in the form section.\');" class="button" /> ';
                $content .= '<input type="submit" log="true" log-action="4" log-type="pro_profile_change" name="reject" value="Reject" class="button" /> ';
                $content .= '<input type="submit" log="true" log-action="5" log-type="pro_profile_change" name="submit" value="Request Resubmission" class="button" style="width: 200px;" /> ';
            }
            $content .= "          <a href='$redirect&id=$pid' class=button>Cancel</a>";
            $content .= "        </td>";
            $content .= "    </tr></table>";
            if ($AccessObj->user_type == 'super' || $ifAdmin) {
                $content .= '<div class="chkbox">
                <input type="checkbox" value="1" id="notify" name="notify" />
                <label for="notify"></label>
            </div>
             <p style="font-family: Lato Italic; font-size:14pt;">Send notification to professional.</p>
             <style>.chkbox {
            width: 25px;
            position: relative;
            float:left;
            margin-left:5px;
        }
        .chkbox label {
            cursor: pointer;
            position: absolute;
            width: 15px;
            height: 15px;
            top: 0;
            left: 0;
            background: white;
            border:1px solid gray;
            border-radius:2px;
        }
        .chkbox label:after {
            opacity: 0;
            content: "";
            position: absolute;
            width: 9px;
            height: 5px;
            background: transparent;
            top: 2px;
            left: 2px;
            border: 3px solid #333;
            border-top: none;
            border-right: none;
            -webkit-transform: rotate(-45deg);
            -moz-transform: rotate(-45deg);
            -o-transform: rotate(-45deg);
            -ms-transform: rotate(-45deg);
            transform: rotate(-45deg);
        }
        .chkbox label:hover::after {
            opacity: 0.5;
        }
        .chkbox input[type=checkbox]:checked + label:after {
            opacity: 1;
        }</style>';
            }
            $content .= "    <input type=hidden name=update_section value='$panel' />";
            //$content .= "    <input type=hidden name=view value='detail' />";
            //$content .= hidden_post_values(array('tab','page','id'));
            $content .= "</form>";
        } elseif ($view == 'detail') {
            $content .= "<div class=backbtn><a href='$redirect&id=$pid&section=$section' class='button'>back1</a></div><br>";
        }
        $content .= '
      <script type="text/javascript">
      var ajaxobj = {};
      var save_btn_obj = {};
      function hide_ajax_obj(res)
      {
          $(".load-gif").remove();
          if(res.status == "ok"){
              ajaxobj.html(res.content);
          } else if(res.status == "new") {
              ajaxobj.html(res.content);
              if(res.dataid == "")
              {
                  ajaxobj.html("Invalid operation. failed to save content");
              } else {
                  save_btn_obj.data("id", res.dataid);
                  $("#dataid_" + res.section).val(res.dataid);
              }
          } else {
              ajaxobj.append("Please try again.");
          }
      }
      function openDialog(boxtitle, boxcontent){
                  $( "#dialog" ).dialog({
                          autoOpen: true,
                          width: 800,
                          title: boxtitle,
                          height:500,
                          buttons: { "Ok": function() { $(this).dialog("close"); }},
                          closeOnEscape: true,
                          modal:true,
                          resizable: false
                      }).html(boxcontent);

      }

      $(document).ready(function(){

            $("a[rel=\'' . $_REQUEST['box'] . '\']").addClass("active");

            $(".edit-btn").click(function(evt){

            if ($(this).hasClass("active")) return;

            act_box = $(this).attr("rel");

            $( "div.edit-tools" ).find( "a" ).removeClass("active");

            $(this).addClass("active");

            $(".experties").hide();
            $(".industryview").hide();
            $(".mgmtview").hide();

            $("."+act_box).show();


            });
          $(".review-btn").click(function(evt){
              evt.preventDefault();
              boxcontent = tinyMCE.get($(this).data("holder")).getContent();
              boxtitle = $(this).data("title");
              openDialog(boxtitle, boxcontent);
          });

          $(".save-btn").click(function(evt){
              evt.preventDefault();
              save_btn_obj = $(this);
              var content = tinyMCE.get($(this).data("holder")).getContent();
              ajaxobj = $(".note");
              ajaxobj.prepend("<img class=\"load-gif\" src=\"/ajax-loader.gif\" />");
              dataid = $(this).data("id");
              uid = $(this).data("uid");
              section = $(this).data("section");

              POSTDATA = "a=maennaco_ajax&action=ajaxTrigger&table=update_profile&dataid="+ dataid + "&uid=" + uid + "&section=" + section + "&text=" + encodeURIComponent(content);
              ajax_update(POSTDATA, hide_ajax_obj);
          });

        $(\'.get-examples-btn\').click(function() {

                            rel = $(this).attr(\'rel\');
                            $("#"+rel).show();
                            $("#get-help-pro-examples").dialog({
                              modal: true,
                              height: "450",
                              autoOpen: true,
                              title: \'Examples\',
                              resizable: false,
                              buttons: {
                                Close: function() {
                                  $("[id^=\'examples-\']").hide();
                                  $(this).dialog("close");
                                }
                              },
                             open: function (event, ui) {
                             if (rel == "examples-expertise") dlg_title = "Expertise"; else if(rel == "examples-industry") dlg_title = "Industry view"; else dlg_title = "Management view"
                              $(this).dialog("option", "title", dlg_title);
                              $(this).scrollTop(0);
                             }

                            });
                          });


      });
      </script>';
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function add_question($op = null) {
    global $user;
    $question = $_REQUEST['questionText'];
    $service_id = (int)$_REQUEST['pro_id'];
    $query = "INSERT INTO `maenna_questionair` (`parentid`, `target`, `type`, `content`, `created`, `editorid`) 
        VALUES (%d, 'service', 'question', '%s', '%s', %d);";
    $values = array($service_id, $question, time(), (int)$user->uid);
    if (db_query($query, $values)) {
        drupal_set_message("Question added successfully.");
        $return = array("status" => true);
    } else {
        drupal_set_message("Failed to add question.", 'error');
        $return = array("status" => false, "error" => array('message' => 'Failed to add question.'));
    }
    return json_encode($return);
}

function save_answers($op = null) {
    global $user;
    if (!$_REQUEST['answers']) {
        drupal_set_message("There are no answers to save.");
        return false;
    }
    $answers = $_REQUEST['answers'];
    foreach ($answers as $answer) {
        if ($answer['answer_id']) {
            $query = "UPDATE `maenna_qa` SET `answer`='%s' WHERE `qaid`=%d;";
            $values = array($answer['answer_text'], $answer['answer_id']);
        } else if ($answer['question_id']) {
            $query = "INSERT INTO `maenna_qa` (`qid`, `qtype`, `uid`, `answer`, `created`) VALUES (%d, 'question', %d, '%s', '%s');";
            $values = array($answer['question_id'], (int)$user->uid, $answer['answer_text'], time());
        } else {
            continue;
        }
        db_query($query, $values);
    }
    drupal_set_message("Answers saved successfully.");
    return true;
}

function save_milestone($op = null) {
    global $user;
    if (!$_REQUEST['milestones']) {
        return;
    }
    $milestones = $_REQUEST['milestones'];
    $companyService = new Clewed\Company\Service();
    $companyService->updateServiceMilestones($_REQUEST['pro_id'], $milestones);
    drupal_set_message("Milestones saved successfully.");
}

function approve_milestones($op = null) {
    $query = "UPDATE `project_service_milestones` SET `approve_status` = 1, `due_date` = NOW() + INTERVAL `duration` DAY + INTERVAL 5 HOUR WHERE `service_id` = " . $_REQUEST['pro_id'] . ";";
    db_query($query);
    drupal_set_message("Milestones approved successfully.");
}

/* EOF */
