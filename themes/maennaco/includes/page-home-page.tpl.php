<?php
// $Id: page.tpl.php,v 1.18.2.1 2009/04/30 00:13:31 goba Exp $
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>"
	lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">

<head>
	<?php print $head ?>
	<title>
		<?php print $head_title ?>
	</title>
	<?php print $styles ?>
	<?php print $scripts ?>
	<!--[if lt IE 7]>
      <?php print phptemplate_get_ie_styles(); ?>
    <![endif]-->

	<style>
		.ui-dialog .ui-dialog-titlebar {
			padding: .4em 1em;
			position: relative;
			background: none;
			border-top: none;
			border-left: none;
			border-bottom: none;
			border-right: none;
			color: #231F20;
		}

		.ui-dialog .ui-dialog-title {
			float: left;
			margin: .1em 16px .1em 0;
		}

		.ui-dialog .ui-dialog-titlebar-close {
			display: block !important;
			background-image: url('themes/maennaco/images/close-icon-off.png') !important;
			width: 33px;
			height: 32px;
		}

		.ui-dialog .ui-dialog-titlebar-close span {
			display: block;
			margin: 1px;
		}

		.ui-dialog .ui-dialog-titlebar-close:hover {
			background-image: url('themes/maennaco/images/close-icon-on.png') !important;
		}

		.ui-dialog .ui-dialog-titlebar-close:focus {
			padding: 0;
		}

		.ui-dialog .ui-dialog-titlebar-close {
			position: absolute;
			top: 23% !important;
			right: -0.2em;
			margin: -10px 0 0 0;
			padding: 1px;
		}

		#wrapper {
			background: url("/themes/maennaco/images/homepage-bg.jpg") repeat-y;
			margin-top: -20px;
		}

		p {
			font-family: 'Lato Regular';
			font-size: 14px;
		}


		ul li,
		ul.menu li,
		.item-list ul li,
		li.leaf {
			height: 30px;
			background: #E8E8E9 !important;
		}

		ul li,
		ul.menu li,
		.item-list ul li,
		li.list-a {
			background: none !important;
			height: auto;
		}

		.ui-dialog .ui-dialog-content {
			padding: 0 !important;
		}

		.ui-dialog {
			padding: 0 !important;
		}

		.ui-widget-overlay {
			opacity: 0.6 !important;
		}
	</style>
</head>
<body<?php print phptemplate_body_class($left, $right); ?>>
	<link type="text/css" href="/themes/maennaco/jui/css/redmond/homepagecss/jquery-ui-1.8.15.custom.css"
		rel="stylesheet" />
	<script type="text/javascript" src="/themes/maennaco/jui/js/jquery-1.6.2.min.js"></script>
	<script type="text/javascript" src="/themes/maennaco/jui/js/jquery-ui-1.8.15.custom.min.js"></script>
	<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
	<script src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js" type="text/javascript"></script>

	<div id="fb-root"></div>
	<script>(function (d, s, id) {
			var js, fjs = d.getElementsByTagName(s)[0];
			if (d.getElementById(id)) return;
			js = d.createElement(s); js.id = id;
			js.src = "//connect.facebook.net/en_GB/all.js#xfbml=1&appId=267809316574109";
			fjs.parentNode.insertBefore(js, fjs);
		}(document, 'script', 'facebook-jssdk'));</script>

	<!-- Layout -->
	<!--div id="header-region" class="clear-block"><?php print $header; ?></div-->

	<div id="wrapper">
		<div id="container" class="clear-block" style="margin-top:20px !important;">

			<div id="header">
				<?php 
            global $user;
           ?>
				<div style="position:relative;width:220px;float:left;">
					<a href="/"><img src="/<?php echo path_to_theme()  ;?>/images/index_logo.png" alt="website logo"
							border=0 width=220></a>

				</div>

				<div style="position:relative;width:220px;margin-right:50px;float:right;">
					<?php if (!$user->uid) echo '<a id="learn-more" style="font-family:Lato Regular; font-size:12px; font-weight:bold;cursor:pointer; color:#06333b;">LEARN MORE > </a>';

                            else  echo '<a id="my_account" href="/account" style="font-family:Lato Regular; font-size:12px; font-weight:bold;cursor:pointer; color:#11a7c2;">MY ACCOUNT > </a>';?>
				</div>


				<div id="user-status" style="float:left;">
					<?php get_user_status_home($user); ?>
				</div>

				<?php if(! $is_front) get_topnavmenu(); ?>
			</div> <!-- /header -->

			<br style="clear:both;"><br>
			<div id="center" style='margin-top:55px;'>
				<div id="squeeze">
					<div class="right-corner">
						<div class="left-corner" style='padding:0;margin:0'>

							<div class="clear-block">
								<?php
                print $content;
		  //echo '<a href="#" class="home-play"></a>';
		  echo '<a href="#" id="home-register" class="home-register"></a>';
		  
		  echo '<div style="margin-left:auto; margin-right:auto; width:330px">';
		  echo '<a href="https://twitter.com/share" class="twitter-share-button">Tweet</a>
<script>!function (d, s, id) { var js, fjs = d.getElementsByTagName(s)[0]; if (!d.getElementById(id)) { js = d.createElement(s); js.id = id; js.src = "//platform.twitter.com/widgets.js"; fjs.parentNode.insertBefore(js, fjs); } }(document, "script", "twitter-wjs");</script>';
                 echo '<script src="//platform.linkedin.com/in.js" type="text/javascript"></script>
<script type="IN/Share" data-url="www.clewed.com" data-counter="right"></script>';
echo '<iframe src="//www.facebook.com/plugins/like.php?href=http%3A%2F%2Fwww.clewed.com&amp;send=false&amp;layout=button_count&amp;width=450&amp;show_faces=false&amp;action=like&amp;colorscheme=light&amp;font&amp;height=21&amp;appId=267809316574109" scrolling="no" frameborder="0" style=" margin-left:30px;border:none; overflow:hidden; width:80px; height:21px;" allowTransparency="true"></iframe>';
		  echo '</div>';	             
?>
							</div>



						</div>
					</div>
				</div>
			</div> <!-- /.left-corner, /.right-corner, /#squeeze, /#center -->



		</div> <!-- /container -->
		<div id="footer-new">
			<div id="footer-wrapper">
				<!--		   </span>		   			<div id="footer-left-column">
<span>We believe</span>&nbsp; small to mid-size companies have the highest unrecognized potential. They must connect more seamlessly with the skills and resources they need to
unlock hidden value and compound high growth. We created our platform to allow clients
to do that far more efficiently and inexpensively than any other alternative because doing
so is far greater than any short-term fix.
			</div> 
	<div class="headerdivider"></div>-->
				<div id="footer-right-column">
					<div id="rc-icon"><img src="themes/maennaco/images/research-icon.png"></div>
					<br>
					<span class="title">RESEARCH</span>
					<div id="rc-content">We start with a long-term mindset and a rigorous analysis to generate a plan
						that enables clients to win markets. </div>
				</div>
				<div class="headerdivider"></div>
				<div id="footer-right-column">
					<div id="rc-icon"><img src="themes/maennaco/images/advice-icon.png"></div>
					<br>
					<span class="title">ADVICE</span>
					<div id="rc-content"> A customized advisory team of leading executives and analysts infuses
						strategic insights and provides innovative problem-solving.</div>
				</div>
				<div class="headerdivider"></div>
				<div id="footer-right-column">
					<div id="rc-icon"><img src="themes/maennaco/images/monitor-icon.png"></div>
					<br>
					<span class="title">MONITOR</span>
					<div id="rc-content"> We continuously measure risks and performance and iterate advice for new
						learning to ensure we compound high growth.</div>
				</div>
				<div style="float:right; margin-top:50px;width:420px; font-family:'Lato Regular';"><a class="show_terms"
						type="terms" target="_blank" style="cursor:pointer; font-family:'Lato Regular';">Terms of
						Use</a><a class="show_terms" type="privacy"
						style=" cursor:pointer;margin-left:20px;font-family:'Lato Regular';">Privacy Policy</a>
					<span style="margin-left:20px;">&#169; 2020 clewed. All Rights Reserved.</span>
				</div>



			</div>
		</div>



		<script type="text/javascript">
			$(document).ready(function () {

				function checkString(obj, watermark) {


					if (obj.val() == ' ' || obj.val() == watermark) {

						obj.css('border', '1px solid #C52020');
						return false;
					}
					else {
						obj.css('border', '1px solid #CCCCCC');
						return true;
					}
				}

				$('#company-firstname').Watermark('FIRST NAME');
				$('#company-lastname').Watermark('LAST NAME');
				$('#company-name').Watermark('COMPANY NAME');
				$('#company-email').Watermark('EMAIL');
				$('#company-password').Watermark('PASSWORD');
				$('#company-captcha').Watermark('SECURITY CODE');

				$('#pro-firstname').Watermark('FIRST NAME');
				$('#pro-lastname').Watermark('LAST NAME');
				$('#pro-email').Watermark('EMAIL');
				$('#pro-password').Watermark('PASSWORD');
				$('#pro-captcha').Watermark('SECURITY CODE');

				$('#edit-email').Watermark('USER NAME');
				//$('#edit-password').Watermark('PASSWORD');

				$("#feedback").click(function (event) {

					event.preventDefault();
					$(".dialog-right-column").html(response).fadeIn("slow");
					$("#learn-more-dialog").dialog("open");
					$.post("/themes/maennaco/includes/homepage_posts.php?type=contactus", {

					}, function (response) {

						$('#contact-name').Watermark('NAME');
						$('#contact-email').Watermark('EMAIL');
						$('#contact-subject').Watermark('SUBJECT');
						$('#contact-message').Watermark('MESSAGE');




					});
				});

				$("body").delegate("a.contactus", "click", function () {

					alert('damjan');

				});


				$("#partner_feed").click(function (event) {

					event.preventDefault();
					$(".dialog-right-column").html(response).fadeIn("slow");
					$("#learn-more-dialog").dialog("open");
					$.post("/themes/maennaco/includes/homepage_posts.php?type=partners", {

					}, function (response) {

						$('#partner-name').Watermark('NAME');
						$('#partner-email').Watermark('EMAIL');
						$('#partner-phone').Watermark('PHONE');
						$('#partner-comments').Watermark('COMMENTS');




					});
				});

				$("body").delegate("a.psubmit", "click", function () {

					alert('damjan');

				});


				$("body").delegate("a.pagree", "click", function () {

					$.post("/themes/maennaco/includes/homepage_posts.php?type=terms", {

					}, function (response) {

						$("#right-column-left").html(response).fadeIn("slow");

					});


				});


				$(".show_terms").click(function () {

					$.post("/themes/maennaco/includes/homepage_posts.php?type=policy&req_type=" + $(this).attr('type'), {

					}, function (response) {

						$("#policy").html(response);


					});
					$("#policy").dialog("open");
					return false;
				});





				$("#tabs").tabs();

				$(".dialog-left-column a").click(function () {


					var id = $(this).attr('id');


					if (id == 'wwa') {

						$(".dialog-right-column").height(2550);
						$(".verticaldivider").height(2530);

					} else if (id == 'wwd') {

						$(".dialog-right-column").height(810);
						$(".verticaldivider").height(790);
					}

					else {

						$(".dialog-right-column").height(550);
						$(".verticaldivider").height(550);

					}

					$(".dialog-left-column a").removeClass('dialog-selected');
					$(this).addClass('dialog-selected');

					$(".dialog-right-column").fadeOut('fast');


					$.post("/themes/maennaco/includes/homepage_posts.php?type=" + id, {

					}, function (response) {

						$(".dialog-right-column").html(response).fadeIn("slow");

						if (id == 'partners') {

							$('#partner-name').Watermark('NAME');
							$('#partner-email').Watermark('EMAIL');
							$('#partner-phone').Watermark('PHONE');
							$('#partner-comments').Watermark('COMMENTS');

						}
						else if (id == 'contactus') {

							$('#contact-name').Watermark('NAME');
							$('#contact-email').Watermark('EMAIL');
							$('#contact-subject').Watermark('SUBJECT');
							$('#contact-message').Watermark('MESSAGE');

						}

					});




				});



				$("#learn-more-dialog").dialog({
					modal: true,
					autoOpen: false,
					width: "920",
					height: "620",
				});

				$("#policy").dialog({
					modal: true,
					autoOpen: false,
					width: "920",
					height: "620",
				});

				$("#register-dialog").dialog({
					modal: true,
					autoOpen: false,
					width: "300",
					buttons: [
						{
							text: "REGISTER",
							click: function () {

								fValid = true;

								selTab = $("#tabs").tabs('option', 'selected');

								if (selTab == 0) {

									fValid = fValid && checkString($("#company-firstname"), 'FIRST NAME');
									fValid = fValid && checkString($("#company-lastname"), 'LAST NAME');
									fValid = fValid && checkString($("#company-name"), 'COMPANY NAME');
									fValid = fValid && checkString($("#company-email"), 'EMAIL');
									fValid = fValid && checkString($("#company-password"), 'PASSWORD');
									fValid = fValid && checkString($("#company-industry"), 'industry');

									if (!$('#cmp-agree').is(':checked')) { fValid = false; alert('You have to read and agree with terms and privacy policy!'); }







									if (fValid) {


										$.post("register_new_user.php?type=company",
											{ firstname: $("#company-firstname").val(), lastname: $("#company-lastname").val(), name: $("#company-name").val(), email: $("#company-email").val(), password: $("#company-password").val(), industry: $("#company-industry").val(), captcha: $("#company-captcha").val() },
											function (response) {
												alert(response);
												if (response != 'Security code is not correct!')
													location.reload();

											});
									}

								}
								else {
									fValid = fValid && checkString($("#pro-type"), 'iam');
									fValid = fValid && checkString($("#pro-firstname"), 'FIRST NAME');
									fValid = fValid && checkString($("#pro-lastname"), 'LAST NAME');
									fValid = fValid && checkString($("#pro-email"), 'EMAIL');
									fValid = fValid && checkString($("#pro-password"), 'PASSWORD');
									fValid = fValid && checkString($("#pro-experties"), 'experties');

									if (!$('#pro-agree').is(':checked')) { fValid = false; alert('You have to read and agree with terms and privacy policy!'); }







									if (fValid) {


										$.post("register_new_user.php?type=proffesional",
											{ firstname: $("#pro-firstname").val(), lastname: $("#pro-lastname").val(), experties: $("#pro-experties").val(), email: $("#pro-email").val(), password: $("#pro-password").val(), captcha: $("#pro-captcha").val(), pro_type: $("#pro-type").val() },
											function (response) {
												alert(response);
												if (response != 'Security code is not correct!')
													location.reload();

											});
									}


								}



							},
							"class": "form-submit"
						},
						{
							text: "CANCEL",
							click: function () {
								$(this).dialog("close");
							},
							"class": "form-submit"
						}
					]
				});

				$("#learn-more").click(function () {
					$("#learn-more-dialog").dialog("open");
					return false;
				});

				$("#home-register").click(function () {
					$("#register-dialog").dialog("open");
					$("#register-dialog").parent().children(".ui-dialog-titlebar").remove();
					return false;
				});

				$("input:text").focus(function () {
					$(this).val('');
				});

				var page_height = $(window).height();

				var container_height = $('#container').css('height');

				var diff = parseInt(page_height) - 480;
				diff = parseInt(diff / 2);
				if (diff) $('#container').css('margin-top', diff);
				var a = 0;
				if (diff) $('#container').css('margin-top', diff);
				$('#show_overlay').click(function () {
					$('#olay').fadeIn("fast");
				});
				$('#olay').bind("mouseenter", function () {
					a = 1;
				}).bind("mouseleave", function () {
					a = 0;
					$(this).fadeOut();
				});
				$('#closeolay').click(function () {
					$('#olay').fadeOut();
				})
			})
		</script>

		<div style="clear:both;text-align:left;display:none">
			<a href=""><img src="/<?php echo path_to_theme();?>/images/facebook-icons.jpg" style="display:inline"></a>
			<a href=""><img src="/<?php echo path_to_theme();?>/images/twitter-icons.jpg" style="display:inline"></a>
		</div>

	</div>
	<!-- /layout -->

	<?php print $closure ?>
	<div id='apply_account_type' class="hide">
		<div id="dialog" style="padding:0px 30px 0px 10px;height:20px;line-height:20px;">
			I represent a <a href="/company-apply-form">company</a> or a <a href="/people-apply-form">professional</a>.
			<?php
            /*
                $array = array(
                    array('src' => 'companies.png', 'href' =>'/company-apply-form', 'alt'=>'companies'),
                    array('src' => 'professionals.png', 'href' =>'/people-apply-form', 'alt'=>'professionals'),  
                );
                button_links($array);
            */
            ?>
		</div>
	</div>
	<script type="text/javascript">

		var _gaq = _gaq || [];
		_gaq.push(['_setAccount', 'UA-23393148-1']);
		_gaq.push(['_trackPageview']);

		(function () {
			var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
			ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
			var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
		})();

	</script>
	<div id="learn-more-dialog" style="display:none;">
		<div class="dialog-left-column">
			<a id="wwb" class="dialog-left-column-title dialog-selected" style="cursor:pointer;">What We Believe</a><br>
			<a id="wwd" class="dialog-left-column-title" style="cursor:pointer;">What We Do</a><br>
			<!--<a id="wwd_overview" class="indented" style="cursor:pointer;">- Overview</a><br>
<a id="services" style="cursor:pointer;" class="indented">- Services</a><br>
<a id="investment" style="cursor:pointer;" class="indented">- Investment</a><br>-->
			<a id="wsa" class="indented" style="cursor:pointer;">- What Sets Us Apart</a><br>
			<a id="companies" class="indented" style="cursor:pointer;">- Company Criteria</a><br><br>
			<a id="wwa" class="dialog-left-column-title" style="cursor:pointer;">Who We Are</a><br>
			<!--<a id="wwa_overview" style="cursor:pointer;" class="indented">- Overview</a><br>
<a id="maenna" style="cursor:pointer;" class="indented">- MAENNA Team</a><br>
<a id="ad_network" style="cursor:pointer;" class="indented">- Advisory Network</a><br>-->
			<a id="advisors" class="indented" style="cursor:pointer;">- Advisor Criteria</a><br><br>
			<a id="investors" class="dialog-left-column-title" style="cursor:pointer;">Investors</a><br>
			<a id="partners" class="dialog-left-column-title" style="cursor:pointer;">Partners</a><br>
			<a id="contactus" class="dialog-left-column-title" style="cursor:pointer;">Contact Us</a><br>
		</div>
		<div class="verticaldivider"></div>
		<div class="dialog-right-column">
			<span class="dialog-right-column-title">What We Believe</span><br><br>

			We believe small to mid-size companies have the highest unrecognized potential and
			must connect more seamlessly with leading owner-minded professionals to realize their
			long-term potential. We created our platform to allow clients to compound maximum
			growth for as long as possible because doing so generates far greater wealth than any
			short-term fix.
		</div>
		<div id="register-dialog" style="display:none;">
			<div id="tabs">
				<ul>
					<li id="reg-company"><a href="#tabs-1">COMPANY</a></li>
					<li id="reg-company"><a id="reg-proffesional" href="#tabs-2">PROFESSIONAL</a></li>
				</ul>
				<div id="tabs-1">
					<div id="reg-form" style="color:#00a1be;">
						* <input class="required" type="text" id="company-firstname">
						* <input class="required" type="text" id="company-lastname">
						* <input class="required" type="text" id="company-name">
						* <input class="required" type="text" id="company-email">
						* <input class="required" type="password" id="company-password">
						<span style="margin-left:10px;font-size:10px; color:#00a1be;"> 8 character minimum, at least one
							number</span><br>

						<select name="company-industry" id="company-industry">
							<option value="industry">INDUSTRY</option>
							<?php
			$Sectors            = _INDUSTRY();
			foreach ($Sectors as $key=>$value ) {
				
				echo "<optgroup label=\"$key\">";
				
				foreach($value as $key1=>$value1) echo "<option value=\"$key1\">$value1</option>";

			echo "</optgroup>";
			}

			echo "</select>";

 ?>
							<input style="margin-left:20px; margin-top:7px;" type="checkbox" id="cmp-agree">
							<label style="font-family:'Lato Regular'; font-size:9px; color:#2E2F34;" for="cmp-agree">I
								agree with clewed's <a class="show_terms" type="terms" target="_blank"
									style="color:#00a1be;">Terms</a> and <a target="_blank" class="show_terms"
									type="privacy" style="color:#00a1be;">Privacy Policy</a></label>

							<br>
							<div style="float:left;" id='captcha_img' style='height:39px;width:110px;'><img
									height="39px" width="110px" src='/captcha/captcha.php'></div>
							<div></div><input class="required" style="float:left;width:125px;" type="text"
								id="company-captcha">
					</div>
					<br style="clear:both;"><a href="#" id="reload_captcha" style="font-size:11px;">Not readable? Change
						text.</a>
				</div>
				<div id="tabs-2">




					<div id="reg-form" style="color:#00a1be;">
						<select name="pro-type" id="pro-type">
							<option value="iam">I AM</option>
							<?php
			$ProType = _pro_types();
			foreach ($ProType as $key=>$value ) {
				
				echo "<option value=\"$key\">$value</option>";
								}

			echo "</select>";
?>
							* <input class="required" type="text" id="pro-firstname">
							* <input class="required" type="text" id="pro-lastname">
							* <input class="required" type="text" id="pro-email">
							* <input class="required" type="password" id="pro-password">
							<span style="margin-left:10px;font-size:10px; color:#00a1be;"> 8 character minimum, at least
								one number</span><br>

							<select name="pro-experties" id="pro-experties">
								<option value="experties">EXPERTIES</option>
								<?php
    			$Experties = _experties();
			foreach ($Experties as $key=>$value ) {
				
				echo "<optgroup label=\"$key\">";
				
				foreach($value as $key1=>$value1) echo "<option value=\"$key1\">$value1</option>";

			echo "</optgroup>";
			}

			echo "</select>";

 ?>
								<input style="margin-left:20px; margin-top:7px;" type="checkbox" id="pro-agree">
								<label style="font-family:'Lato Regular'; font-size:9px; color:#2E2F34;"
									for="pro-agree">I agree with clewed's <a class="show_terms" type="terms"
										target="_blank" style="color:#00a1be;">Terms</a> and <a class="show_terms"
										type="terms" target="_blank" style="color:#00a1be;">Privacy Policy</a></label>

								<br>
								<div style="float:left;" id='captcha_img' style='height:39px;width:110px;'><img
										height="39px" width="110px" src='/captcha/captcha.php'></div>
								<div></div><input class="required" style="float:left;width:125px;" type="text"
									id="pro-captcha">
					</div>
					<br style="clear:both;"><a href="#" id="reload_captcha" style="font-size:11px;">Not readable? Change
						text.</a>
				</div>
			</div>

		</div>

	</div>

	<div id="policy" style="font-family: 'Lato Regular'; font-size:12px; display:none;"></div>


	</body>

</html>