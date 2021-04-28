<style type="text/css">
	.abtloop p {
		font-size: 15.5px !important;
	}
	.investor-type-select{
		width:207px;
		height:28px;
		margin-top:10px;
		float: left;
		display: block;
	}
    .questmark-investor-type{
        display: inline-block;
        padding: 15px 5px;
    }
</style>
<?php

error_reporting(1);

function about_general_content (){
	global $base_url;
	global $redirect;
	global $user;
	global $AccessObj;
	include 'dbcon.php';
	$user_id = $user->uid;
	$redirect_path = $base_url . $redirect;
	$project_id = (int) sget($_REQUEST, 'id');
	$companyService = new Clewed\Company\Service();
	// Get Like details of the project with login user
	$like_result = db_query(
		"SELECT * FROM `like_company` WHERE project_id = %d AND user_id = %d", array($project_id, $user_id)
	);
	$likeabout = mysql_num_rows($like_result);
	$like_count = db_query("SELECT count(la_id) AS likes FROM like_company WHERE project_id = %d", array($project_id));
	$likesobj = db_fetch_object($like_count);
	$numlikes = (int) $likesobj->likes;
	// Get Project details from -  'maenna_company' table
	$pro_sql = "SELECT * FROM maenna_company WHERE companyid = %d";
	$pro_result = db_query($pro_sql, array($project_id));
	$pro_Row = db_fetch_object($pro_result);
	// Get Project details from -  'maenna_about' table
	$sql = "SELECT * FROM maenna_about WHERE project_id = %d";
	$result = db_query($sql, array($project_id));
	$Row = db_fetch_object($result);
	$path = './themes/maennaco/images/project/';
	$option = sget($_REQUEST, 'view');
	$field = sget($_REQUEST, 'field');
    $check_db = db_query("SHOW COLUMNS FROM `maenna_about` LIKE 'mission_temp'");
    $temp_row = db_fetch_object($check_db);
    if (empty($temp_row)) {
        db_query("ALTER TABLE `maenna_about` ADD COLUMN  `mission_temp` text COLLATE 'latin1_swedish_ci' NULL AFTER `mission`;");
    }
    $check_db = db_query("SHOW COLUMNS FROM `maenna_about` LIKE 'goal_temp'");
    $temp_row = db_fetch_object($check_db);
    if (empty($temp_row)) {
        db_query("ALTER TABLE `maenna_about` ADD COLUMN  `goal_temp` text COLLATE 'latin1_swedish_ci' NULL AFTER `goal`;");
    }
	$param_val = sget($_REQUEST, 'param');
	$mission = sget($_REQUEST, 'mission');
	$goal = sget($_REQUEST, 'goal');
	if(!empty($mission) && !empty($goal) && empty($field)){
        $field = 'content';
    }
    $temp_sufix = (isset($_POST['temp_save']) && $_POST['temp_save'])?'_temp':'';
	//Get colleagues id array
	$colleagueArray = $companyService->getColleagueIds($project_id);
	if ($option == 'delete') {
		$sql = "UPDATE maenna_about SET $param_val = '' where project_id = %d";
		$result = db_query($sql, array($project_id));
		header('Location: ' . $redirect_path);
	}
	company_about_get_top_block($op, $AccessObj, $redirect_path, $Row, $user, $likeabout, $project_id, $user_id, $numlikes, $pro_Row, $colleagueArray, $content);
}

function about_general($op = null, $content = false) {
	global $base_url;
	global $redirect;
	global $user;
	global $AccessObj;
	include 'dbcon.php';
	$user_id = $user->uid;
	$redirect_path = $base_url . $redirect;
	$project_id = (int) sget($_REQUEST, 'id');
	$companyService = new Clewed\Company\Service();
	// Get Like details of the project with login user
	$like_result = db_query(
		"SELECT * FROM `like_company` WHERE project_id = %d AND user_id = %d", array($project_id, $user_id)
	);
	$likeabout = mysql_num_rows($like_result);
	$like_count = db_query("SELECT count(la_id) AS likes FROM like_company WHERE project_id = %d", array($project_id));
	$likesobj = db_fetch_object($like_count);
	$numlikes = (int) $likesobj->likes;
	// Get Project details from -  'maenna_company' table
	$pro_sql = "SELECT * FROM maenna_company WHERE companyid = %d";
	$pro_result = db_query($pro_sql, array($project_id));
	$pro_Row = db_fetch_object($pro_result);
	// Get Project details from -  'maenna_about' table
	$sql = "SELECT * FROM maenna_about WHERE project_id = %d";
	$result = db_query($sql, array($project_id));
	$Row = db_fetch_object($result);
	$path = './themes/maennaco/images/project/';
	$option = sget($_REQUEST, 'view');
	$field = sget($_REQUEST, 'field');
    $check_db = db_query("SHOW COLUMNS FROM `maenna_about` LIKE 'mission_temp'");
    $temp_row = db_fetch_object($check_db);
    if (empty($temp_row)) {
        db_query("ALTER TABLE `maenna_about` ADD COLUMN  `mission_temp` text COLLATE 'latin1_swedish_ci' NULL AFTER `mission`;");
    }
    $check_db = db_query("SHOW COLUMNS FROM `maenna_about` LIKE 'goal_temp'");
    $temp_row = db_fetch_object($check_db);
    if (empty($temp_row)) {
        db_query("ALTER TABLE `maenna_about` ADD COLUMN  `goal_temp` text COLLATE 'latin1_swedish_ci' NULL AFTER `goal`;");
    }
	$param_val = sget($_REQUEST, 'param');
	$mission = sget($_REQUEST, 'mission');
	$goal = sget($_REQUEST, 'goal');
	if(!empty($mission) && !empty($goal) && empty($field)){
        $field = 'content';
    }
    $temp_sufix = (isset($_POST['temp_save']) && $_POST['temp_save'])?'_temp':'';
	//Get colleagues id array
	$colleagueArray = $companyService->getColleagueIds($project_id);
	if ($option == 'delete') {
		$sql = "UPDATE maenna_about SET $param_val = '' where project_id = %d";
		$result = db_query($sql, array($project_id));
		header('Location: ' . $redirect_path);
	}
	if ($option == 'edit' || (!empty($mission) && !empty($goal))) {

		switch ($field) {
			case 'project' :
				if(empty($Row)) {
					$Row = new stdClass();
					$Row->project_id = $project_id;
				}

				$file_name = preg_replace('|[^0-9A-Za-z\-\.]|', '', $_REQUEST['project']);
				//                $new_file_name = $_POST['project_img'];
				//                if (!empty($new_file_name)) {
				if (!empty($file_name)) {
					$path = dirname(dirname(__FILE__)) . '/images/project/';
					if(is_readable($path . $file_name)) {

						$info = pathinfo($path . $file_name);
						$ext = strtolower($info['extension']);

						$project_id = (int) $project_id;
						$new_file_name = $project_id . '.' . $ext;
						foreach (glob($path . $project_id . ".*") as $f)
							unlink($f);

						if(copy($path . $file_name, $path . $new_file_name )) {
							if (empty($Row->about_id)) {
								$sql = "INSERT INTO maenna_about(project_id, project) VALUES(%d, '" . arrangeValues($new_file_name) . "') ";
								$result = db_query($sql, array($project_id));
							} else {
								$sql = "UPDATE maenna_about SET project = '" . arrangeValues($new_file_name) . "' WHERE project_id = %d";
								$result = db_query($sql, array($project_id));
							}
							header('Location: ' . $redirect_path);
						}
						else {
							maenna_project($Row, $path);
						}
					}
					else {
						maenna_project($Row, $path);
					}
				} else {
					maenna_project($Row, $path);
				}
				//                else {
				//                    maenna_project($Row, $path);
				//                }

				break;
			case 'content' :
				if (!empty($_POST)) {
					if (empty($Row)) {
						$sql = "INSERT INTO maenna_about(project_id, mission".$temp_sufix.", goal".$temp_sufix.") VALUES(%d, '" . arrangeValues($_POST['mission']) . "', '" . arrangeValues($_POST['goal']) . "') ";
						$result = db_query($sql, array($project_id));
					} else {
						if($temp_sufix && (empty($_POST['mission']) || $_POST['mission'] == '<p><br data-cke-filler="true"></p>') && (empty($_POST['goal']) || $_POST['goal'] == '<p><br data-cke-filler="true"></p>')){
                            $sql = "UPDATE maenna_about SET mission_temp = null, goal_temp = null WHERE project_id = %d";
                            $result = db_query($sql, array($project_id));
                        }else{
                            $sql = "UPDATE maenna_about SET mission".$temp_sufix." = '" . arrangeValues($_POST['mission']) . "', goal".$temp_sufix." = '" . arrangeValues($_POST['goal']) . "' WHERE project_id = %d";
                            $result = db_query($sql, array($project_id));
                            if(!$temp_sufix){
                                $sql = "UPDATE maenna_about SET mission_temp = null, goal_temp = null WHERE project_id = %d";
                                $result = db_query($sql, array($project_id));
                            }
                        }
                    }
					header('Location: ' . $redirect_path);
				} else {

					maenna_aboutcontent($Row);
				}
				break;
			case 'mission' :
				if (!empty($_POST)) {
					if (empty($Row)) {
						$sql = "INSERT INTO maenna_about(project_id, mission) VALUES(%d, '" . arrangeValues($_POST['mission']) . "') ";
						$result = db_query($sql, array($project_id));
					} else {
						$sql = "UPDATE maenna_about SET mission = '" . arrangeValues($_POST['mission']) . "' WHERE project_id = %d";
						$result = db_query($sql, array($project_id));

						// if(! $result){
						//    file_put_contents(__DIR__ . '/.log.txt', '---------------------------------------' . PHP_EOL . $sql . PHP_EOL, FILE_APPEND);
						// }
					}
					header('Location: ' . $redirect_path);
				} else {
					maenna_mission($Row);
				}
				break;
			case 'goal' :
				if (!empty($_POST)) {
					if (empty($Row)) {
						$sql = "INSERT INTO maenna_about(project_id, goal) VALUES(%d, '" . arrangeValues($_POST['goal']) . "') ";
						$result = db_query($sql, array($project_id));
					} else {
						$sql = "UPDATE maenna_about SET goal = '" . arrangeValues($_POST['goal']) . "' WHERE project_id = %d";
						$result = db_query($sql, array($project_id));
					}
					header('Location: ' . $redirect_path);
				} else {
					maenna_goal($Row);
				}
				break;
			case 'founded' :
				if (!empty($_POST)) {
					if (empty($Row)) {
						$sql = "INSERT INTO maenna_about(project_id, founded) VALUES(%d, '" . arrangeValues($_POST['founded']) . "') ";
						$result = db_query($sql, array($project_id));
					} else {
						$sql = "UPDATE maenna_about SET founded = '" . arrangeValues($_POST['founded']) . "' WHERE project_id = %d";
						$result = db_query($sql, array($project_id));
					}
					header('Location: ' . $redirect_path);
				} else {
					maenna_founded($Row);
				}
				break;
			case 'industry' :
				if (!empty($_POST)) {
					if (empty($Row)) {
						$sql = "INSERT INTO maenna_about(project_id, industry) VALUES(%d, '" . arrangeValues($_POST['industry']) . "') ";
						$result = db_query($sql, array($project_id));
					} else {
						$sql = "UPDATE maenna_about SET industry = '" . arrangeValues($_POST['industry']) . "' WHERE project_id = %d";
						$result = db_query($sql, array($project_id));
					}
					header('Location: ' . $redirect_path);
				} else {
					maenna_industry($Row);
				}
				break;
			case 'links' :
				break;
		}
	}
	if ($option != 'edit'){
        company_about_get_top_block($op, $AccessObj, $redirect_path, $Row, $user, $likeabout, $project_id, $user_id, $numlikes, $pro_Row, $colleagueArray, $content);
        company_about_get_content($op, $AccessObj, $redirect_path, $Row, $user, $likeabout, $project_id, $user_id, $numlikes, $pro_Row, $colleagueArray, $content);
	}
}

function company_about_get_top_block($op, $AccessObj, $redirect_path, $Row, $user, $likeabout, $project_id, $user_id, $numlikes, $pro_Row, $colleagueArray, $content){
		?>
	<div class="abt" > <!--style="width:646px;"-->
		<?php if (!empty($_REQUEST['notif'])) : ?>
			<div id="messages" class="messages status" style="text-align:center;">You are in About Page</div>
		<?php endif; ?>



        <div class="about-top-section">

            <?php
                if ($AccessObj->user_type == 'super' || $AccessObj->uid == sget($_REQUEST, 'id') || $op == 'write' || in_array($AccessObj->uid,$colleagueArray)) : ?>
                <div class="action-wrapper">
                        <a href="<?php echo $redirect_path; ?>&view=edit&field=project" class="action-link">| Add</a>
                </div>
            <?php endif; ?>

            <div class="source-area">
              <?php if ($Row->video_url) { ?>
                  <iframe id="pitch_video" width="590" height="320" src="#" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
                  <script>
                            $(document).ready(function () {
                                let url = prepareEmbededUrl('<?=$Row->video_url;?>');
                                if (url)
                                    $("#pitch_video").attr("src", url);
                                else $("#pitch_video").remove();

                            });
                        </script>
                <?php }
                else {
                    if (!empty($Row->project) && $Row->project != '<div></div>'/* && file_exists($path . $Row->project)*/) {
                        ?>
                            <?php
                            if (strlen($Row->project) >= 50) {
                                echo $Row->project
                                ?>
                            <?php } else {
                                ?>
                                <img src="/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/<?= urlencode($Row->project) ?>&zc=1&w=590&h=415"
                                     width="100%" height="320px"/>
                            <?php } ?>
                        <?php
                    } elseif ($AccessObj->uid == sget($_REQUEST, 'id') || in_array($AccessObj->user_type, array('super', 'admin')) || in_array($AccessObj->uid, $colleagueArray)) {

                        ?>
                                <span class="hint-text">
                                    Add a photo & video that<br/>
                                    tells your story.
                                </span>
                        <?php
                    }
                    else { ?>
                            <img src="/themes/maennaco/phpthumb/phpThumb.php?src=../images/cmp-avatar-service.png&zc=1&w=590&h=415"/>
                    <?php }
                }
                ?>
            </div>


            <?php
            if ($user->uid != $_REQUEST['id']) {?>

            <?php
            if ($AccessObj->user_type == 'company' || $AccessObj->user_type == 'people' || $AccessObj->user_type == 'super' || $AccessObj->user_type == 'admin' || $AccessObj->user_type == 'company'){

                if ($pro_Row->deal_summary_title){
                    echo '<div style="font-size: 22pt;font-family: Latoregular;margin-left:12px;margin-top:36px; margin-bottom:8px; color:#929497;line-height: 35px;
				word-spacing: 2px;color: #686b83 !important;">';
                    echo($pro_Row->deal_summary_title);
                    echo '</div>';
                }
                if ($pro_Row->deal_summary_statement){
                    echo "<div style='text-align: justify;padding: 10px;border: 5px solid #fafbfb12;font-family: Lato Light !important;color:#929497; font-size: 15px;font-style: normal;color: #686b83 !important;'>";
                    echo ($pro_Row->deal_summary_statement);
                    echo '</div>';
                }
            }

            ?>
            <div class="like-bar">

                <?php if ($likeabout == 1) : ?>
                    <span class="like" id="like_company_id">
                    <a class="ablike" onclick="like_company('unlike', '<?= $project_id ?>','<?= $user_id ?>');">Unlike</a>
                </span>
                <?php else : ?>
                    <span class="like" id="like_company_id">
                    <a class="ablike" onclick="like_company('like', '<?= $project_id ?>','<?= $user_id ?>');">Like</a>
                </span>
                <?php endif; ?>
                <span style="margin-right:-2px;" class="counter"><?= $numlikes ?></span>
                <?php
                }
                $value = '';
                $coll_status = collaboratorStatus($user_id, $project_id);
                if ($coll_status == 'pending') {
                    $value = "<span class='contribute'><a style='cursor:pointer;' title='Disconnect' type='uncontribute' cid='" . $project_id . "' uid='" . $user_id . "' class='contribute'>Pending</a></span>";
                }
                elseif ($coll_status == 'active') {
                    $value = "<span class='contribute'><a style='cursor:pointer;' title='Disconnect' type='uncontribute' cid='" . $project_id . "' uid='" . $user_id . "' class='contribute'>Connected</a></span>";
                }
                elseif ($coll_status == 'deactivated') {
                    $value = "<span class='contribute'><a style='cursor:pointer;' type='contribute' cid = '" . $project_id . "' uid='" . $user_id . "' class='contribute'>Request connection</a></span>";
                }
                elseif ($user_id != $project_id) {
                    $canCollaborate = hasApprovedProfile($user->uid);

                    if($pro_Row->shareable) {
                        if (!$canCollaborate) {
                            $value = '<span class="contribute"><a class="ablike" type="no_coll" style="cursor:pointer;">Request connection</a></span>';
                        } else {
                            $value = "<span class='contribute'><a style='cursor:pointer;' type='contribute' cid = '" . $project_id . "' uid='" . $user_id . "' class='contribute'>Request connection</a></span>";
                        }
                    }

                }
                if ($value) {
                    echo '&nbsp;|&nbsp;' . $value;
                }
                ?>

            </div>

        </div>



	</div>

	<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
	<script type="text/javascript">
		$(document).ready(function () {
			init_contribute();
		});
	</script>
	<?php
}

function company_about_get_content($op, $AccessObj, $redirect_path, $Row, $user, $likeabout, $project_id, $user_id, $numlikes, $pro_Row, $colleagueArray, $content){
	if(!empty($_GET['panel']) || $_GET['panel'] == 'edit_company_info') return;


	if($content):?> <hr style="margin-top: -9px; margin-bottom: 36px; background:#d1d2d4;">
		<style>
			p {
				font-size: 16px;
				color: #929497 !important;
			}
			h2 {
				font-size: 24px;
				font-family: "Lato Black";
				color: #929497;
			}
			h3 {
				font-size: 20px;
				font-family: Lato Light;
				color: #929497;
			}
			h4 {
				font-size: 12px;
				font-family: Lato Regular;
				color: #929497;
			}
			div ol {
				list-style: none;
				counter-reset: item;
			}
			div ol li {
				list-style-type: none;
				counter-increment: item;
				margin-bottom: 5px;
				margin-left: 10px;
				color: #929497;
				font-size: 16px;
			}
			div ol li:before {
				content: counter(item);
				color: #00A2BF;
				font-family: Lato Light;
				font-size: 14pt;
				font-weight: bold;
				width: 1.2em;
				display: inline-block;
			}
			div.abtloop div ul li {
				list-style: none;
				margin-left: 10px;
				padding: 0 0 .2em 0;
				background: none;
				color: #929497;
				font-size: 16px;
			}
			div.abtloop div ul li:before {
				content: "•";
				margin-right: 5px;
				color: #00A2BF;
				font-weight: bold;
				font-size: 16pt;
				font-family: Lato Light;
			}
			.ck-widget >img {
				display: block;
				margin: 0 auto;
				max-width: 100%;
				min-width: 50px;
			}
			.stuck {
				position: fixed;
				margin-top: -200px !important;
				transition: margin-top .25s ease-in-out;
				z-index: -1;
			}
			.stuck1 {
				position: fixed;
				margin-top: -120px !important;
				max-height: 200px;
				transition: margin-top .25s ease-in-out;
				z-index: -1;
			}
		</style>
	<?php endif;
	if ($AccessObj->user_type == 'super' || $AccessObj->uid == sget($_REQUEST, 'id') || $op == 'write' || in_array($AccessObj->uid,$colleagueArray)) : ?>
		<div class="abtbtn abtbtn-full" style="margin-bottom:8px; <?php if($content):?> width: auto; <?php endif;?> <?php if($AccessObj->user_type != 'people'):?><?php endif;?>" >
			<a href="<?php echo $redirect_path; ?>&view=edit&field=content">| Edit</a>
			<a class="company-about-edit-tooltip" style="cursor:pointer;float: right;margin-left: 5px;margin-top: 5px; margin-right: -16px">
				<img src="/themes/maennaco/images/questionmark.png">
			</a>
			<div class="company-about-edit-tooltip-dialog hidden">
				In the About section, please describe your business, its mission, industry and size.
				Use the project goal section to describe the project you need help with, your estimated budget,
				start and finish timelines, and estimated team size for this project.
				<b>Do not include company name, logo or similar information that reveal your company's identity.</b>
			</div>

			<script>
				/*                    window.onscroll = function(ev) {
										if (window.scrollY > 900) {
											$('.right-td').addClass('stuck');
										}
										else {
											$('.right-td').removeClass('stuck');
										}
										if (window.scrollY > 900) {
											$('.left-td').addClass('stuck1');
										}
										else {
											$('.left-td').removeClass('stuck1');
										}
									};*/
				$(".company-about-edit-tooltip").click(function () {
					$(".company-about-edit-tooltip-dialog").dialog({
						modal: true,
						height: "270",
						autoOpen: true,
						title: "About help",
						resizable: false,
						open: function (event, ui) {
							$(this).scrollTop(0);
						}
					});
				});
			</script>
		</div>
	<?php endif; ?>

	<?php if ($Row->mission == null || $Row->mission == '<p><br data-cke-filler="true"></p>'){
        // maenna_aboutcontent($Row,true);
    } else { ?>
        <div class='abtloop' style="padding: 0" <?php if(!$content):?><?php endif;?>>
            <!--<div class='abt-title'>ABOUT</div>-->
            <div style="clear:both"></div>
            <div class="company-content-wrapper" <?php if($AccessObj->user_type == 'super'):?>style="margin-top:-55px;"<?php endif;?>>
                <p>
                    <?php
                    if (!empty($Row->mission)) {
                        echo $Row->mission;
                    } elseif($AccessObj->uid == sget($_REQUEST, 'id') || in_array($AccessObj->user_type, array('super', 'admin')) || in_array($AccessObj->uid,$colleagueArray)) {
                        echo '<span style="font-style:italic;color: #999999;">Please describe your business or project. Include project size (employees, revenue or deliverables as applicable), sector, key products or services, competition and similar information that will help us target the right resources more quickly. Do not include company name, logo or similar information for privacy.</span>';
                    }
                    ?></p>
            </div>
        </div>

        <div class='abtloop' <?php if(!$content):?> style='width: 610px;'<?php endif;?>>
            <?php
            if ($AccessObj->Com_sections['about']['sections']['goals']['access'] != 'hide') {
                ?>
                <!--<div class='abt-title'><?php /*echo "PROJECT GOAL"; */?></div>-->
                <div style="clear:both"></div>
                <div class="company-content-wrapper">
                    <p>
                        <?php
                        if (!empty($Row->goal)) {
                            echo replaceEndSpaces($Row->goal);
                        } elseif($AccessObj->uid == sget($_REQUEST, 'id') || in_array($AccessObj->user_type, array('super', 'admin')) || in_array($AccessObj->uid,$colleagueArray)) {
                            echo "<span style=\"font-style:italic;color: #999999;\">Describe your key goals and the resources and expertise you believe you need to achieve them. Provide your estimated budget and timelines. You will have the opportunity to clarify details once we start the process.</span>";
                        }
                        ?>
                    </p>
                </div>
            <?php } ?>
        </div>
    <?php } ?>

	</div>
	<?php
}

/**
 * @param $output
 * @return string
 */
function fundraising_additional_info_new()
{
	$selectedOptions = getSelectedCompanyPage2($_REQUEST['id']);

	$output = '<div style="margin: 0 12px;text-align:center; width: 580px; color:#929497;">';
	if (!empty($selectedOptions[0]['max_per_investor'])
		|| !empty($selectedOptions[0]['estimated_share_price'])
		|| !empty($selectedOptions[0]['term'])) {

		// MAXIMUM per investor
		if (!empty($selectedOptions[0]['max_per_investor'])) {
			$str_maximum = $selectedOptions[0]['max_per_investor'] ? '$' . number_format($selectedOptions[0]['max_per_investor'], 0, '.', ',') : 'N/A';

			$output .= '<div class="" style="width: 190px; text-align: left; display:inline-block;">';
			$output .= '<div class="status-label" style="font-family: Lato Bold Italic; font-size:12px;">MAXIMUM <span class="light" style="font-family: Lato Light; font-size:12px; color:#929497;">per investor</span></div>';
			$output .= '<div class="status-label status-amount" style="font-family: Lato Bold Italic; font-size:15px;">' . $str_maximum . '</div>';
			$output .= '</div>';
		}

		// PRICE/SHARE
		if (!empty($selectedOptions[0]['estimated_share_price'])) {
			$str_price_per_share = $selectedOptions[0]['estimated_share_price'] ? '$' . number_format($selectedOptions[0]['estimated_share_price'], 2, '.', ',') : 'N/A';

			$output .= '<div class="" style="width: 190px; display:inline-block;">';
			$output .= '<div class="status-label" style="font-family: Lato Bold Italic; font-size:12px;">PRICE/SHARE</div>';
			$output .= '<div class="status-label status-amount" style="font-family: Lato Bold Italic; font-size:15px;">' . $str_price_per_share . '</div>';
			$output .= '</div>';
		}

		// TERM
		if (!empty($selectedOptions[0]['term'])) {
			$str_price_per_share = $selectedOptions[0]['term'] ? strtoupper($selectedOptions[0]['term']) : 'N/A';

			$output .= '<div class="" style="width: 190px; text-align: right; display:inline-block;">';
			$output .= '<div class="status-label" style="font-family: Lato Bold Italic; font-size:12px;">TERM</div>';
			$output .= '<div class="status-label status-amount" style="font-family: Lato Bold Italic; font-size:15px;">' . $str_price_per_share . '</div>';
			$output .= '</div>';
		}
	}
	$output .= '</div>';
	return $output;
}

function add_content($op = null) {
	return '';
	$type = $_REQUEST['type'];
	$ctype = $_REQUEST['ctype'];
	$companyid = $_REQUEST['id'];
	$data_type = 'about_content';
	$panel = 'add_content';
	global $user, $redirect;
	if (!$type) {
		$Block['body'] = '<div class="abtbtn spacer" style="color:#929497">Add Content &nbsp; | &nbsp; Add Images</div>';
	} elseif ($type == 'add_content') {
		if ($op != 'update') {
			$Block['title'] = 'CONTENT';
			$view = sget($_REQUEST, 'view');
			if ($view == '') $view = 'add';
			if ($view == 'detail' || $view == 'edit' || $view == 'add') {
				$dataid = sget($_REQUEST, 'dataid');
				$Row = array();
				if ($dataid) {
					$sql = "SELECT * FROM maenna_company_data WHERE dataid = %d LIMIT 1";
					$result = db_query($sql, array($dataid));
					$Row = db_fetch_array($result);
				} elseif (empty($dataid) && ($view == 'detail')) {
					$sql = "SELECT * FROM maenna_company_data WHERE companyid = %d AND data_type = '%s' ORDER BY dataid DESC LIMIT 1";
					$result = db_query($sql, array($companyid, $data_type));
					$Row = db_fetch_array($result);
				}
				//print_r($Row);
				//mysql_query("update maenna_company_data set status=1");
				if ($Row !== false) {
					if ($view == 'detail') {
						$dataid = sget($Row, 'dataid');
						$sub_title = strtoupper(sget($Row, 'data_value'));
						if ($sub_title) {
							$sub_title = htmlentities($sub_title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
						} else {
							$sub_title = "&nbsp;";
						}
						$text = sget($Row, 'data_value2');
						$text = nl2br(htmlentities($text, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
						$rem_link = '';
						if ($op == 'write' || ($op == 'edit' && $editorid == $companyid)) {
							$rem_link = $redirect . "&update_section=${panel}&do=remove&dataid=$dataid";
							$rem_link = "<a href='$rem_link' class=tool onclick='return confirm(\"Continue to remove record\")'>Delete</a>";
							$edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
							$edit_link = "<div class=editbtn><a href='$edit_link' class=tool>EDIT</a></div>";
						} else {
							$rem_link = '';
							$edit_link = '';
						}
						$content = "<div class=entry>
                                    <div class=entry-title>$sub_title &nbsp;$edit_link</div>
                                    <div class=entry-content>" . _filter_autop(html_entity_decode($text)) . "</div>
                                </div>";
						$content .= "<div class=backbtn>

                                    <a href='$back' class=button>back</a>&nbsp;&nbsp;
                                    $rem_link
                                </div>";
					} elseif (($view == 'edit' || $view == 'add') && $op == 'write' || ($op == 'edit' && $editorid == $companyid)) {
						$dataid = sget($Row, 'dataid');
						$status = sget($Row, 'status');
						if ($status == 0) {
							$sub_title = strtoupper(sget($Row, 'data_value_save'));
							$text = sget($Row, 'saved_content');
							$short_desc = sget($Row, 'short_description2');
							$tags = sget($Row, 'tags2');
						} else {
							$sub_title = strtoupper(sget($Row, 'data_value'));
							$text = sget($Row, 'data_value2');
							$short_desc = sget($Row, 'short_description');
							$tags = sget($Row, 'tags');
						}
						if ($view == 'edit') {
							$title = "Edit " . $Block['title'];
							$do = 'update';
							$rem_link = $redirect . "&panel=${panel}&view=listview&update_section=${panel}&do=remove&dataid=$dataid";
						} else {
							$title = "Add " . $Block['title'];
							$do = 'insert';
							$rem_link = '';
						}
						$Block['title'] = $title;
						$hv = hidden_post_values(array('tab', 'page', 'id', 'mtab', 'type'));
						$rr = '<div>Tags:<br /><select class="discuss" id="tags" name="tags"><option value="">Choose a Category</option>';
						$rr .= OPTION_TAGS(_categories(), $tags);
						$rr .= '</select></div><br />';
						$content .= <<< END
                    <form action='$base_url/account' method='post' name='addanalysis' onsubmit='return check_input();'>
                        <div class=rt-block1>
                            <div class=rt-title>
                                <div>Title:<br /><input type=text name=title value='$sub_title'/></div>
                                <div>Content:<br /><textarea name='content' style='width:99%;height:500px;' class='require_string' $length_limit>$text</textarea></div><br>


								$rr

                                <div class=backbtn>
                                <input type=submit name=submit value=Publish class=button_add onclick='return checktags();'  />
								<input type=submit name=submit value=Save class=button_add onclick='return checktags();' />
                                <!--<a href='$rem_link' class=tool onclick='return confirm("Continue to remove record")'>Delete</a>-->
									<a href='$redirect' class=tool><input type=button name=submit value=Cancel class=button_add /></a>
                                </div>
                            </div>
                        </div>
                        $hv
                        <input type='hidden' name=dataid value='$dataid' />
                        <input type='hidden' name=view value='detail' />
                        <input type='hidden' name=do value='$do' />
                        <input type='hidden' name=update_section value='$panel' />
                    </form>
END;
					}
				}
			}
			//$Block['title'] = 'ADD CONTENT';
			$Block['body'] = content_box($Block['title'], $content);
		} elseif ($op == 'update') {
			$editorid = $user->uid;
			date_default_timezone_set('EST');
			$do = sget($_REQUEST, 'do');
			$Correct = false;
			if ($do == 'update' || $do == 'insert') {
				$dataid = sget($_REQUEST, 'dataid');
				$sub_title = sget($_REQUEST, 'title');
				$text = sget($_REQUEST, 'content');
				$short_description = sget($_REQUEST, 'short_desc');
				$tags = sget($_REQUEST, 'tags');
				$time = time();
				$data_type = 'about_content';
				$editorid = $user->uid;
				if (empty($text)) {
				} elseif ($dataid && ($do == 'update')) {
					if (sget($_REQUEST, 'submit') == 'Save') {
						$status = 0;
						$DBValues = array(
							'access'             => $time,
							'short_description2' => $short_description,
							'tags2'              => $tags,
							'data_value_save'    => $sub_title,
							'saved_content'      => $text,
							'status'             => $status,
							'editorid'           => $editorid
						);
					} else {
						$status = 1;
						$DBValues = array(
							'access'            => $time,
							'short_description' => $short_description,
							'tags'              => $tags,
							'data_value'        => $sub_title,
							'data_value2'       => $text,
							'status'            => $status,
							'editorid'          => $editorid
						);
					}
					foreach ($DBValues as $key => $val) {
						$SQL_STR["$key"] = "$key = '%s'";
					}
					$SQL_STR["editorid"] = "editorid=%d";
					$sql = "UPDATE maenna_company_data SET " . implode(',', $SQL_STR) . " WHERE dataid = " . ((int) $dataid) . " LIMIT 1";
					if (db_query($sql, $DBValues)) $Correct = true;
					header('location:' . $base_url . '/account?tab=companies&page=company_detail&id=' . $companyid . '&mtab=' . $mtab);
				} elseif ($do == 'insert') {
					if (sget($_REQUEST, 'submit') == 'Save') {
						$status = 0;
					} else {
						$status = 1;
					}
					$DBKeys = array(
						'companyid',
						'access',
						'data_type',
						'short_description2',
						'tags2',
						'data_value_save',
						'saved_content',
						'short_description',
						'tags',
						'data_value',
						'data_value2',
						'editorid',
						'status'
					);
					$DBValues = array($companyid, $time, $data_type, $short_description, $tags, $sub_title, $text, $short_description, $tags, $sub_title, $text, $editorid, $status);
					$SQL_STR = array("%d", "'%s'", "'%s'", "'%s'", "'%s'", "'%s'", "'%s'", "'%s'", "'%s'", "'%s'", "'%s'", "%d", "'%s'");
					$sql = "INSERT INTO maenna_company_data (" . implode(',', $DBKeys) . ") VALUES(" . implode(',', $SQL_STR) . ")";
					/*echo($sql);
					test_array($DBValues);
					die();*/
					if (db_query($sql, $DBValues)) $Correct = true;
					header('location:' . $base_url . '/account?tab=companies&page=company_detail&id=' . $companyid . '&mtab=' . $mtab);
				}
			} elseif ($do == 'remove') {
				$dataid = sget($_REQUEST, 'dataid');
				if ($dataid) {
					$sql = "UPDATE maenna_company_data SET access = '%s', editorid = %d, deleted = 1 WHERE dataid = %d LIMIT 1";
					if (db_query($sql, array($time, $editorid, $dataid))) $Correct = true;
				}
			}
			if ($Correct) {
				drupal_set_message("Operation Successful");
			} else {
				drupal_set_message("Operation Failed", 'error');
			}
			return;
		}
	}
	return $Block;
}

function investment_content($op = null) {
	return '';
	$dataid = sget($_REQUEST, 'dataid');
	$companyid = sget($_REQUEST, 'id');
	$data_type = 'about_content';
	global $user, $redirect;
	$Row = array();
	if ($dataid) {
		$sql = "SELECT * FROM maenna_company_data WHERE companyid = %d AND data_type = '%s' ORDER BY dataid DESC LIMIT 1";
		$result = db_query($sql, array($companyid, $data_type));
		$Row = db_fetch_array($result);
	}
	if ($Row !== false) {
		$dataid = sget($Row, 'dataid');
		$sub_title = strtoupper(sget($Row, 'data_value'));
		if ($sub_title) {
			$sub_title = htmlentities($sub_title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
		} else {
			$sub_title = "&nbsp;";
		}
		$text = sget($Row, 'data_value2');
		$text = nl2br(htmlentities($text, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
		$rem_link = '';
		if ($op == 'write' || ($op == 'edit' && $editorid == $companyid)) {
			$rem_link = $redirect . "&update_section=${panel}&do=remove&dataid=$dataid";
			$rem_link = "<a href='$rem_link' class=tool onclick='return confirm(\"Continue to remove record\")'>Delete</a>";
			$edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
			$edit_link = "<div class=editbtn><a href='$edit_link' class=tool>EDIT</a></div>";
		} else {
			$rem_link = '';
			$edit_link = '';
		}
		$content = "<div class=entry>
                                    <div class=entry-title>$sub_title &nbsp;$edit_link</div>
                                    <div class=entry-content>" . _filter_autop(html_entity_decode($text)) . "</div>
                                </div>";
		$content .= "<div class=backbtn>

                                    <a href='$back' class=button>back</a>&nbsp;&nbsp;
                                    $rem_link
                                </div>";
		$Block['title'] = 'CONTENT';
	}
	$Block = content_box($Block['title'], $content);
	return $Block;
}

function maenna_project($Row = '', $path = '') {
	if ($_POST) {
		if (db_query("UPDATE maenna_about SET video_url = '%s' WHERE project_id = '%s'", array($_POST['pitch_video_url'], $Row->project_id))) {
			$Row->video_url = $_POST['pitch_video_url'];
		}
		header("Location: /account?tab=companies&page=company_detail&id=". $Row->project_id. "&mtab=about");

	}
	$param = '"project"';
	if (!empty($Row->project) && file_exists($path . $Row->project)) {
		$image = "<img src='" . $path . $Row->project . '?' . time() . "'/>";
		$delete = "<input type='button' value='Delete' onclick='delete_record($param);' class='button'>";
	} else {
		$image = "";
	}
	$content = "<div class='act-content'>";
	$content .= "<form method='post' action='' enctype='multipart/form-data' onsubmit='return validate_video_url();' >
					<div class='content_box'><div class='box_title shaded_title'>ADD COVER IMAGE AND PITCH VIDEO</div></div>
					<div class='entry'>
          <div style=\"line-height:16px;\">Pick a unique photo that represents your company without revealing the
            identity of your business. <a href=\"#\" onmouseover=\"$('#more-help-files').show();\" onmouseout=\"$('#more-help-files').hide();\" >More</a>
          </div>
          <div id=\"more-help-files\" style=\"display:none; color: #91939E; margin: 16px 0 16px 0; line-height:16px;border: none !important\">
            Please don't use a logo or content that could easily be linked to your business or is commercial, promotional,
            copyright-infringing or already in use on other people's covers to protect the confidentiality of all your
            information on our platform.
          </div>

						<div class='company-cover-image-uploader-wrapper'>
                            <input type='hidden' name='project'/>
                            <div class='company-cover-image-uploader'
                                data-id='" . $Row->project_id . "'
                                data-u='" . ($time = time()) . "'
                                data-m='" . md5('cropper.php:' . $time . ':' . $Row->project_id . ':kyarata75' ) . "'></div>
                            <div class='entry-content'>
                                Please upload jpg/png images of size not less than 590 x 440
                            </div>
                            <div class='company-cover-image'>$image</div>
                        </div>

					</div><br />
				<div style='margin-top:60px;line-height:16px;' class='pitch_video_form'>
				     <div style='margin-bottom:30px;'>Add a pitch video summarizing why investors should invest in your company</div>
				    <input style='width:100% !important;' type='text' value='$Row->video_url' id='pitch_video_url' name = 'pitch_video_url' placeholder='Cut and paste an url for youtube or vimeo video'>
                    <p style='font-style: italic'> * Deleting the link will remove teaser video</p>
                </div>
                <div style='margin-top:30px;'>
						<input type='submit' value='Submit' class='button' style='margin-left:0'>
						$delete
						<input type='button' value='Cancel' onclick='redirect();' class='button' style='background: #284B5A!important;'>
					</div>
				</form>
				<script>
				    $(document).ready(function () {
                        $('#pitch_video_url').blur(function () {
                            var url = $(this).val();
                            if (url != '') {
                            var embededUrl = prepareEmbededUrl($(this).val());
                            if (embededUrl == null) {
                                $(this).addClass('error_select');
                            }
                            else {
                                 $(this).removeClass('error_select');
                                 $(this).insertAfter('Damjan');
                            }
                            }
                            else $(this).removeClass('error_select');
                        })
});
                </script>

				";

	$content .= "</div>";
	echo $content;

	//    $param = '"project"';
	//    if (!empty($Row->project) && file_exists($path . $Row->project)) {
	//        $image = "<img src='" . $path . $Row->project . '?' . time() . "'/>";
	//        $delete = "<input type='button' value='Delete' onclick='delete_record($param);' class='button'>";
	//    } else {
	//        $image = "";
	//    }
	//    $content = "<div class='act-content'>
	//        <form method='post' action='' enctype='multipart/form-data' onsubmit='onSubmit();' >
	//					<div class='content_box'><div class='box_title shaded_title'>CHOOSE YOUR COVER PHOTO</div></div>
	//					<div class='entry'>
	//          <div style=\"line-height:16px;\">Pick a unique photo to tell your company’s solution without revealing the
	//            identity of your business. <a href=\"#\" onmouseover=\"$('#more-help-files').show();\" onmouseout=\"$('#more-help-files').hide();\" >More</a>
	//          </div>
	//          <div id=\"more-help-files\" style=\"display:none; color: #91939E; margin: 16px 0 16px 0; line-height:16px;border: none !important\">
	//            Please don't use a logo or content that could easily be linked to your business or is commercial, promotional,
	//            copyright-infringing or already in use on other people's covers to protect the confidentiality of all your
	//            information on our platform.
	//          </div>
	//
	//                <div class=\"document-editor\" style='width: 610px;'>
	//                    <div class=\"toolbar-container3\"></div>
	//                    <div class=\"content-container\">
	//                        <input type='hidden' name='project_img' id='project_img'>
	//                        <div id=\"project_img_editor\" style='height: 350px; overflow-y: auto;'>" . $Row->project . "</div>
	//                    </div>
	//                </div>
	//					</div><br />
	//					<div>
	//						<input type='submit' value='Submit' class='button' style='margin-left:0'>
	//						<input type='button' value='Cancel' onclick='redirect();' class='button' style='background: #284B5A!important;'>
	//					</div>
	//				</form>";
	//    $content .= "</div>";
	//    $content .= "<script>
	//            DecoupledEditor
	//                .create( document.querySelector( '#project_img_editor' ), {
	//                    toolbar: [ 'imageupload', 'mediaembed' ],
	//                    ckfinder: {
	//                        uploadUrl: '/themes/maennaco/includes/cropper.php/?command=uploadImage',
	//
	//                        options: {
	//                            resourceType: 'Images'
	//                        }
	//                    }
	//                } )
	//                .then( editor => {
	////                    window.editor = editor;
	//
	//                    const toolbarContainer = document.querySelector( '.toolbar-container3' );
	//
	//                    toolbarContainer.prepend( editor.ui.view.toolbar.element );
	//
	//                    window.editor = editor;
	//                } )
	//                .catch( err => {
	//                    console.error( err.stack );
	//                } );
	//
	//
	//            function onSubmit() {
	////                if (valid_form('mission') && valid_form('goal')) {
	//                    if (document.getElementById('project_img_editor').lastElementChild.innerHTML == 'image widget')
	//                        document.getElementById('project_img_editor').lastElementChild.remove();
	//                    document.getElementById('project_img').value = document.getElementById('project_img_editor').innerHTML;
	//                    if (document.getElementById('project_img_editor').getElementsByTagName('figure').length == 0)
	//                        document.getElementById('project_img').value = '<div></div>';
	////                }
	//            }
	//        </script>";
	//    echo $content;
}

function maenna_aboutcontent($Row = '', $button = false) {
    //for highlights creation  link
    global $redirect;
    $data_type = 'highlights';
    $section = 'highlights';
    $panel = "two_columns";


	$template = "<h2>Why We Like This Opportunity</h2><p><br></p><h3>Low Leverage</h3><p>Discuss the amount of loans and the terms of such loans (interest rate, duration, collateral if any etc.) as well as the amount of equity capital the business has on its balance sheet.</p><h3>Cash Flows</h3><p>Discuss the size of free cash flows the business generates. Also discuss the annual cash requirements of the business for operations, capital expenditures and similar uses.</p><h3>Collateral, Returns</h3><p>What is the range of interest or return investors expect to make?</p><h3>Growth</h3><p>What is the company’s revenue and profit growth rate for the last year and the last 5 years? How is this expected to change in the next year and 5 years?</p><h3>Business Model</h3><p>Describe what the company offers, to whom and why. Also describe how the company makes money and the size of expenses it incurs to generate that revenue.&nbsp; Discuss what the company does better than other similar companies in its market that gives the company an advantage to sell more of its products/services?</p><h3>Experienced Team</h3><p>Discuss the backgrounds of the people running the company</p><p><br></p><h2>Risks</h2><p><br></p><ul><li>Default risks</li></ul><p>What is the risk that the company may be unable to meet its debt obligations and how does the company mitigate the risk?</p><ul><li>Operational risks</li></ul><p>What are the risks of providing the company’s products and services at the highest quality standard and the fastest speed customers demand and how does the company manage this risk?</p><ul><li>Competition</li></ul><p>How does the company mitigate the risk of other companies providing the same service or product or customers not moving to a replacement product?</p><ul><li>Market risks</li></ul><p>Discuss how the value of the business may be adversely impacted by the market and the economic condition you operate in</p><ul><li>Other uncontrollable risks</li></ul><p>Discuss operating and non-operating risks that the company is unable to control and what may protect investors from such risks.</p><p><br></p><h2>Deal Summary</h2><p><br></p><p>Discuss what the investment is, who the issuer is and if such investment is funded or not. If the investment is a loan, discuss the total loan amount, interest rate, the expected repayment date, collateral if any and the value of such collateral.&nbsp; If the investment is equity, discuss if it is dividend paying, frequency of such dividends, how the investor is to be compensated for his/her investment. If the investment is pooled with other investors, discuss how this is done, how many investors, how such investors are represented and the monitoring and reporting of performance during the term of the investment.&nbsp; Please discuss details of the investment and the mechanics for return distribution as clearly as possible while maintaining the actual name of the company for privacy.</p><p><br></p><h3>What protects my interest</h3><p>Discuss the value of the collaterals and the specific description of each collateral and its value. If the investment is to be protected by the value of the company and its assets, discuss the value of these assets, what lien such assets may have on them, the market value of these assets and how such value is determined.</p><h3>Payment mechanism</h3><p>Discuss how investors are scheduled to receive regular monthly interest or dividend payments, the annualized target rate for such interest or dividend over the investment’s expected term of x months / years. Also discuss the maturity date of the loan when the Principal is expected to be repaid or the exit mechanism to provide liquidity and return of capital.&nbsp; In the event payments are transferable to investor’s related party pursuant to specific deal terms, discuss how such transfers may occur as applicable.</p><h3>Originator background</h3><p>Discuss the background of the company, the originator of the deal and his/her track record and how such track record is established to vet this opportunity.</p>";
	$template = require __DIR__ . '/templates/about.php';
	$content = "<input type='hidden' id='project_id' value='". $Row->project_id . "'>
	<div class='act-content'>
        <form method='post' action='' enctype='mutipart/form-data' onsubmit='onSubmit()'>
    		<div class='content_box' style='margin-top:20px;margin-left:-10px;'><div class='box_title shaded_title'>ADD PUBLIC PITCH OR TEASER</div></div>
    		<div class='entry'><div class='entry-content'>
                <div class=\"document-editor\">
                    <div class=\"toolbar-container1\"></div>
                        <div class=\"content-container\">
                            <input type='hidden' name='mission' id='mission'>";
	if($Row->mission_temp){
	    $content .= "<div id=\"mission_editor\" style='height: 350px; overflow-y: auto;'>" . $Row->mission_temp . "</div>";
	} else {
        if ($Row->mission == null || $Row->mission == '<p><br data-cke-filler="true"></p>')
            $content .= "<div id=\"mission_editor\" style='height: 350px; overflow-y: auto;'>" . $template . "</div>";
        else
            $content .= "<div id=\"mission_editor\" style='height: 350px; overflow-y: auto;'>" . str_ireplace('<br data-cke-filler="true">', '', $Row->mission) . "</div>";
    }
	$content .= "</div>
                    </div>
                </div>
            </div>
            <p>&nbsp;</p>
    		<div class='content_box' style='width:auto;'><div class='box_title shaded_title' style='margin-left:-10px;'>ADD PRIVATE INFORMATION FOR SELECT INTERESTED PARTIES</div></div>
    		<div class='entry'><div class='entry-content'>
                <div class=\"document-editor\">
                    <div class=\"toolbar-container2\"></div>
                        <div class=\"content-container\">
                            <input type='hidden' name='goal' id='goal'>";
    if($Row->goal_temp){
        $content .= "<div id=\"goal_editor\" style='height: 350px; overflow-y: auto;'>" . str_ireplace('<br data-cke-filler="true">', '', $Row->goal_temp) . "</div>";
    } else {
        $content .= "<div id=\"goal_editor\" style='height: 350px; overflow-y: auto;'>" . str_ireplace('<br data-cke-filler="true">', '', $Row->goal) . "</div>";
    }
    $content .=      "</div>
                    </div>
                </div>
            </div>
            <input type='hidden' value='0' id='temp_save' name='temp_save'>
            <div class='changing-link'>
                <a href='$redirect&panel=${panel}&section=${section}&view=add&datatype=$data_type'>Add Highlights</a>
            </div
            <div class='changing-link'>
                <a href='$redirect&panel=${panel}&section=${section}&view=list&datatype=$data_type'>List Highlights</a>
            </div>
    		<div style=\"margin-top:8px;\" align='center'>
    			<input type='button' value='Save' onclick='onlySave(this)' class='button'>
    			<input type='submit' value='Publish' class='button'>";
    if(!$button) $content .= "<input type='button' value='Cancel' onclick='redirect();' class='button'>";
    $content .= "</div>
    	</form>
    </div>";
	$content .= "<style>
        p {
            font-size: 16px;
        }
        h2 {
            font-size: 24px;
            font-family: Lato Black;
            color: #929497;
        }
        h3 {
            font-size: 20px;
            font-family: Lato Light;
            color: #929497;
        }
        h4 {
            font-size: 12px;
            font-family: Lato Regular;
            color: #929497;
        }
        #mission_editor, #goal_editor{
            border: 1px solid #c4c4c4;
			border-top:none;
        }
        #mission_editor ol, #goal_editor ol {
            list-style: none;
			counter-reset: item;
			color: #686b83 !important;
        }
        #mission_editor ol li, #goal_editor ol li {
            counter-increment: item;
            margin-bottom: 5px;
            margin-left: 10px;
            font-size: 15px;
			font-family: Lato Light;
			color: #686b83 !important;
        }
        #mission_editor ol li:before, #goal_editor ol li:before {
            content: counter(item);
            color: #00A2BF;
            font-family: Lato Black;
            font-size: 17px;
            width: 1.2em;
            display: inline-block;
            font-weight: bold;
        }
        #mission_editor ul li, #goal_editor ul li {
            margin-left: 10px;
            padding: 0 0 .2em 0;
            background: none;
            list-style: none;
            font-size: 15px;
			font-family: Lato Light;
			color: #686b83 !important;
        }
        #mission_editor ul li:before, #goal_editor ul li:before {
              content: '•';
              color: #00A2BF;
              font-weight: bold;
			font-size: 17px;
			font-family: Lato Black;
				 
        }
        .ck.ck-custom-heading, .ck.ck-heading_heading-h1, .ck.ck-heading_heading-h2, .ck.ck-heading_heading-h4, .ck.ck-heading_paragraph-p-1, .ck.ck-heading_paragraph-p-2, .about-company-h1, .about-company-h2, .about-company-h4, .about-company-p1, .about-company-p2 {
            color: #929497;
        }
        .ck.ck-heading_heading-h1, .about-company-h1 {
            font-family: Lato Black !important;
            font-size: 22pt !important;
			font-weight: normal !important;
			line-height: 35px;
        }
        .ck.ck-heading_heading-h2, .about-company-h2 {
            font-family: 'Lato Light';
            font-size: 22pt !important;
        	font-weight: normal !important;
        	line-height: 32px;
        }
        .ck.ck-heading_heading-h4, .about-company-h4 {
			color: #686b83 !important;
            font-family: 'Lato Light';
			font-size: 12pt !important;
			
        }
        .ck.ck-heading_paragraph-p-1, .about-company-p1, #mission_editor p:not([class]), #goal_editor p:not([class]) {
            font-family: 'LatoRegular';
            font-size: 15px !important;
        	// font-weight: bold !important;
			line-height: 28px;
			color: #686b83 !important;
        }
        .ck.ck-heading_paragraph-p-2, .about-company-p2 {
            font-family: Lato Light !important;
            font-size: 15px !important;
            font-weight: normal !important;
		}
		.ck.ck-heading_paragraph-p-3, .about-company-p3 {
            font-family: 'LatoRegular'!important;
            font-size: 15pt !important;
            font-weight: normal !important;
        }
        /*br[data-cke-filler] {
            display: none;
            visibility: hidden;
        }*/

    </style>";
	$content .= "<script>
            var pTags = document.getElementById('mission_editor').getElementsByTagName('p');
            for (var i = 0; i < pTags.length; i ++) {
                if (pTags[i].firstChild != null && pTags[i].firstChild.nodeName == 'BR') {
                    pTags[i].innerHTML = '';
                    // console.log(pTags[i])
                    }
            }
            var project_id = document.getElementById('project_id').value;
            var fontOptions = {
                options: [

                    { model: 'heading1', view: 'h1', title: 'Heading 1', class: 'ck-heading_heading-h1', view: {
                        name: 'h1',
                        classes: 'about-company-h1'
                    } },
                    { model: 'heading2', view: 'h2', title: 'Heading 2', class: 'ck-heading_heading-h2', view: {
                        name: 'h2',
                        classes: 'about-company-h2'
                    } },
                    { model: 'heading3', title: 'Paragraph 1', class: 'ck-heading_paragraph-p-1', view: {
                        name: 'p',
                        classes: 'about-company-p1'
                    } },
                    { model: 'heading4', title: 'Paragraph 2', class: 'ck-heading_paragraph-p-2', view: {
                        name: 'p',
                        classes: 'about-company-p2'
					}, converterPriority: 'high' },
					// { model: 'heading5', title: 'Paragraph 3', class: 'ck-heading_paragraph-p-3', view: {
                    //     name: 'p',
                    //     classes: 'about-company-p3'
                    // }, converterPriority: 'high' },
                    { model: 'footnote', title: 'Footnote', class: 'ck-heading_heading-h4', view: {
                        name: 'h4',
                        classes: 'about-company-h4'
                    }}
                ]
            };
            var fontSize = {
                options: [12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32]
            };
            DecoupledEditor
                .create( document.querySelector( '#mission_editor' ), {
                     // fontSize: fontSize,
                     heading: fontOptions,
                     toolbar: [ 'heading', '|', 'bold', 'italic', '|', 'alignment', '|', 'numberedlist', 'bulletedlist', '|', 'imageupload', 'mediaembed' ],
//                     simpleUpload: {
//                        // The URL that the images are uploaded to.
//                        uploadUrl: '/themes/maennaco/images/project/',
//
//                        // Headers sent along with the XMLHttpRequest to the upload server.
//                        headers: {
//                        }
//                    }
                        ckfinder: {
                            uploadUrl: '/themes/maennaco/includes/cropper.php/?command=uploadImage&id='+project_id,

                            options: {
                                resourceType: 'Images'
                            }
                    }
                } )
                .then( editor => {
//                    window.editor = editor;

                    const toolbarContainer = document.querySelector( '.toolbar-container1' );

                    toolbarContainer.prepend( editor.ui.view.toolbar.element );

                    window.editor = editor;
                } )
                .catch( err => {
                    console.error( err.stack );
                } );

            DecoupledEditor
                .create( document.querySelector( '#goal_editor' ), {
                     // fontSize: fontSize,
                     heading: fontOptions,
                     toolbar: [ 'heading', '|', 'bold', 'italic', '|', 'alignment', '|', 'numberedlist', 'bulletedlist', '|', 'imageupload', 'mediaembed' ]
                } )
                .then( editor => {
//                    window.editor = editor;

                    const toolbarContainer = document.querySelector( '.toolbar-container2' );

                    toolbarContainer.prepend( editor.ui.view.toolbar.element );

                    window.editor = editor;
                } )
                .catch( err => {
                    console.error( err.stack );
                } );


            function onSubmit() {
//                if (valid_form('mission') && valid_form('goal')) {
                    if (document.getElementById('mission_editor').lastElementChild.innerHTML == 'media widget')
                        document.getElementById('mission_editor').lastElementChild.remove();
                    document.getElementById('mission').value = document.getElementById('mission_editor').innerHTML;

                    if (document.getElementById('goal_editor').lastElementChild.innerHTML == 'media widget')
                        document.getElementById('goal_editor').lastElementChild.remove();
                    document.getElementById('goal').value = document.getElementById('goal_editor').innerHTML;
//                }
            }
        </script>";
	echo $content;
}

function maenna_mission($Row = '') {
	$param = '"mission"';
	if (!empty($Row->mission)) {
		$delete = "<input type='button' value='Delete' onclick='delete_record($param);' class='button'>";
	}
	$content = "<div class='act-content'>";
	$content .= "<form method='post' action='' enctype='mutipart/form-data' onsubmit='return valid_form($param);'>
					<div class='content_box'><div class='box_title shaded_title'>Edit About</div></div>
					<div class='entry'><div class='entry-content'><textarea name='mission' id='mission' style='width: 99%;'>$Row->mission</textarea></div></div>
					<div align='center'>
						<input type='submit' value='Submit' class='button'>
						$delete
						<input type='button' value='Cancel' onclick='redirect();' class='button'>
					</div>
				</form>";
	$content .= "</div>";
	echo $content;
}

function maenna_goal($Row = '') {
	$param = '"goal"';
	if (!empty($Row->goal)) {
		$delete = "<input type='button' value='Delete' onclick='delete_record($param);' class='button'>";
	}
	$content = "<div class='act-content'>";
	$content .= "<form method='post' action='' enctype='mutipart/form-data' onsubmit='return valid_form($param);'>
					<div class='content_box'><div class='box_title shaded_title'>Edit Goal</div></div>
					<div class='entry'><div class='entry-content'><textarea name='goal' id='goal' style='width: 99%;'>$Row->goal</textarea></div></div>
					<div align='center'>
						<input type='submit' value='Submit' class='button'>
						$delete
						<input type='button' value='Cancel' onclick='redirect();' class='button'>
					</div>
				</form>";
	$content .= "</div>";
	echo $content;
}

function maenna_founded($Row = '') {
	$param = '"founded"';
	if (!empty($Row->founded)) {
		$delete = "<input type='button' value='Delete' onclick='delete_record($param);' class='button'>";
	}
	$content = "<div class='act-content'>";
	$content .= "<form method='post' action='' enctype='mutipart/form-data' onsubmit='return valid_form($param);'>
					<div class='content_box'><div class='box_title shaded_title'>Edit Founded</div></div>
					<div class='entry'><div class='entry-content'>
						<input type='text' name='founded' id='founded' value='$Row->founded' />
					</div></div><br />
					<div>
						<input type='submit' value='Submit' class='button'>
						$delete
						<input type='button' value='Cancel' onclick='redirect();' class='button'>
					</div>
				</form>";
	$content .= "</div>";
	echo $content;
}

function maenna_industry($Row = '') {
	$param = '"industry"';
	$test = industry_options($Row->industry);
	$select = "<select name='industry' id='industry'>" . $test . "</select>";
	if (!empty($Row->industry)) {
		$delete = "<input type='button' value='Delete' onclick='delete_record($param);' class='button'>";
	}
	$content = "<div class='act-content'>";
	$content .= "<form method='post' action='' enctype='mutipart/form-data' onsubmit='return valid_form($param);'>
					<div class='content_box'><div class='box_title shaded_title'>Edit Industry</div></div>
					<div class='entry'><div class='entry-content'>
						$select
					</div></div><br />
					<div>
						<input type='submit' value='Submit' class='button'>
						$delete
						<input type='button' value='Cancel' onclick='redirect();' class='button'>
					</div>
				</form>";
	$content .= "</div>";
	echo $content;
}

function arrangeValues($value) {
	// remove CKEditor br
	// $value = str_ireplace('<br data-cke-filler="true">', '', $value);
	// NON UTF
	$value = preg_replace('/\s+/', ' ', $value);
	$value = preg_replace('/[\x00-\x1F\x7F-\xFF]/', '', $value);
	// Also see https://stackoverflow.com/questions/1176904/php-how-to-remove-all-non-printable-characters-in-a-string
	// 8 bit extended ASCII
	// $string = preg_replace('/[\x00-\x1F\x7F]/', '', $string);
	return mysql_real_escape_string(nl2br($value));
}

function industry_values() {
	$Data = array(
		"Basic Materials"    => array(
			'Agriculture',
			'Building Materials',
			'Chemicals',
			'Coal',
			'Forest Products',
			'Metals & Mining',
			'Steel',
		),
		"Consumer Cyclical"  => array(
			'Advertising & Marketing Svcs',
			'Autos',
			'Entertainment',
			'Homebuilding & Construction',
			'Manuf. - Apparel & Furniture',
			'Packaging & Containers',
			'Personal Services',
			'Publishing',
			'Restaurants',
			'Retail - Apparel & Specialty',
			'Travel & Leisure',
		),
		"Consumer Defensive" => array(
			'Beverages - Alcoholic',
			'Beverages - Non-Alcoholic',
			'Consumer Packaged Goods',
			'Education',
			'Retail - Defensive',
			'Tobacco Products',
		),
		"Financial Services" => array(
			'Asset Management',
			'Banks',
			'Brokers & Exchanges',
			'Credit Services',
			'Insurance',
			'Insurance - Life',
			'Insurance - Property & Casualty',
			'Insurance - Specialty',
		),
		"Real Estate"        => array(
			'Real Estate Services',
			'REITs',
		),
		"Healthcare"         => array(
			'Biotechnology',
			'Drug Manufacturers',
			'Health Care Plans',
			'Health Care Providers',
			'Medical Devices',
			'Medical Diagnostics & Research',
			'Medical Distribution',
			'Medical Instruments & Equipment',
			'Utilities - Independent Power Producers',
			'Utilities - Regulated',
			'Communication Services',
		),
		"Energy"             => array(
			'Oil & Gas - Drilling',
			'Oil & Gas - E&P',
			'Oil & Gas - Integrated',
			'Oil & Gas - Midstream',
			'Oil & Gas - Refining & Marketing',
			'Oil & Gas - Services',
		),
		"Industrial"         => array(
			'Aerospace & Defense',
			'Airlines',
			'Business Services',
			'Conglomerates',
			'Consulting & Outsourcing',
			'Employment Services',
			'Engineering & Construction',
			'Farm & Construction Machinery',
			'Industrial Distribution',
			'Industrial Products',
			'Transportation & Logistics',
			'Truck Manufacturing',
			'Waste Management',
		),
		"Technology"         => array(
			'Application Software',
			'Communication Equipment',
			'Computer Hardware',
			'Online Media',
			'Semiconductors',
		)
	);
	$Output = array();
	foreach ($Data as $categoryTitle => $CategoryArray) {
		$M = array();
		foreach ($CategoryArray as $val) {
			$mKey = preg_replace('/[^a-z0-9]/i', '', $val);
			$M["$mKey"] = $val;
		}
		$Output["$categoryTitle"] = $M;
	}
	//echo "Values<pre>" ;print_r($Output); echo "</pre>";
	return $Output;
}

function industry_options($selected = null) {
	$industry_options = '';
	$industry_values = industry_values();
	foreach ($industry_values as $key => $array) {
		$industry_options .= "<optgroup label='$key'>";
		foreach ($array as $kkey => $val) {
			$sel = '';
			if (($selected == $kkey) && (!empty($selected))) $sel = 'selected';
			$industry_options .= "<option value='$kkey' $sel>$val</option>";
		}
		$industry_options .= "</optgroup>";
	}
	return $industry_options;
}

?>
<?php $redirect_path = $base_url . $redirect; ?>
<script>
	function redirect() {
		window.location.href = '<?php echo $redirect_path; ?>';
	}
	function onlySave(self) {
	    $("#temp_save").val(1);
        $(self).parent().parent().submit();
    }
	function delete_record(param) {
		if (confirm('Continue to remove record')) {
			window.location.href = '<?php echo $redirect_path;?>&view=delete&param=' + param;
		}
		else {
			return false
		}
	}
	function valid_form_testing(field_id) {
		var field = '';
		field = document.getElementById(field_id).value;
		if (field == '') {
			alert('Please fill the field values and submit');
			return false;
		}
		else {
			return true;
		}
	}

	<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

	function like_company(type, project_id, user_id) {
		if (type == 'like') {
			var status = 1;
		}
		else {
			var status = 0;
		}
		$.ajax({
			type: 'get',
			url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
				'type=like_company&' +
				'project_id=' + project_id + '&' +
				'user_id=' + user_id + '&' +
				'status=' + status + "&" +
				"u=<?php echo $u; ?>&" +
				"m=<?php echo $m; ?>",
			data: '',
			beforeSend: function () {
			},
			success: function () {
				counter = $('#like_company_id').next();
				if (type == 'like') {
					$('#like_company_id').html("<a class='ablike' style='cursor:pointer;border:none;' onclick='like_company(\"unlike\", " + project_id + "," + user_id + ");'>Unlike</a>");
					counter.html(parseInt(counter.html(), 10) + 1);
				}
				else {
					$('#like_company_id').html("<a class='ablike' style='cursor:pointer;border:none;' onclick='like_company(\"like\", " + project_id + "," + user_id + ");'>Like</a>");
					counter.html(parseInt(counter.html(), 10) - 1);
				}
			}
		});
	}
</script>
