<?php
global $base_url;
global $user;
// $Id: page.tpl.php,v 1.18.2.1 2009/04/30 00:13:31 goba Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>"
      lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
<head>
    <title><?php print $head_title ?></title>
    <?php print $styles ?>
    <?php print $scripts ?>    
    <!--[if lt IE 7]><link type="text/css" rel="stylesheet" media="all" href="/themes/maennaco/fix-ie.css"/><![endif]-->
    <link type="text/css" rel="stylesheet" href="/css/how-it-works.css"/>
    <link type="text/css" rel="stylesheet" href="/themes/maennaco/jui/css/redmond/homepagecss/jquery-ui-1.8.15.custom.css"/>
    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-1.6.2.min.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-ui-1.8.15.custom.min.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js"></script>
    <script type="text/javascript" src="/js/how-it-works.js"></script>
    <script type="text/javascript" src="/js/register-dialog.js"></script>
    <?php  include_once 'templates/header.php'?>
	<style type="text/css">
		div.imageBlock h2{
			padding:35px 0px;
		}
		div.imageBlock p{
			font-size: 24px !important;
			font-family: 'Lato Light Italic', sans-serif  !important;
			line-height:26px;
			color: #898B8E;
		}
		h1{
			padding:22px 0px;
			font-family: "LatoRegular", sans-serif;
			font-size: 32px;
			color: #547381;
			display: block;
			text-align: center;
			/*margin-bottom: 28px;*/
		}
		.clear-block {
			min-height: 60px;
		}
	</style>
</head>
<body  style="background:white">
<div class="hiw-line" id="hiw-top-line"></div>
<div class="hiw-line" id="hiw-bottom-line" style="display:none;top:255px"></div>


<div id="wrapper" style="background:white">
    <div id="container" class="clear-block" style="margin-top:0px !important;">

        <?php require_once("header-regular.php");?>
        <!-- /header -->

        <br style="clear:both;"><div id="center">
            <div id="squeeze">
                <div class="right-corner">
                    <div class="left-corner" style='padding:0;margin:0'>
                        <div id="hiw-banner" style="background-image:none; height:1px">                            
                            <div id="hiw-banner-text" style="width:100%">
                               
                            </div>
							<br>							
                        </div>
                        <div class="imageBlock" style="margin-top:30px;margin-bottom:25px;">
                        <p>Clewed is changing the way businesses plan, execute and achieve results. By seamlessly connecting companies with leading experts through our platform, we make building a scalable business easier, cheaper and faster – opening up more possibilities for companies to lower their cost of capital and improve their purchasing power. Companies can now capitalize on large-scale market opportunities quickly while experts can better monetize their skills.</p>
                            </div>
						<div class="clear-block">                            
                            <div class="links">
                                <a id="company"<?php if (empty($_REQUEST['pro'])) echo ' class="active"' ?>>
                                    For Companies</a>
                                <a id="pro"<?php if (!empty($_REQUEST['pro'])) echo ' class="active"' ?>>
                                    For Professionals</a>								
                            </div>
                        </div>
						<div id="companiesblock" class="imageBlock jsblock"<?php if (!empty($_REQUEST['pro'])) echo ' style="display: none;"' ?>>
							<p>Despite spending over $400B each year on consulting services, companies continue to ask two fundamental questions: 1) Are we focused on the right priorities; and 2) Do we have the capacity to execute against those priorities? The traditional model of delivering value to the marketplace takes too long, is incomplete and costs too much.</p>
							<div style="text-align:center"><h1>The Traditional Model</h1></div>
							<img src="<?php echo $base_url; ?>/themes/maennaco/images/project/venn_diag_1.jpg" alt="The Traditional Model" width="940" style="margin-bottom:22px">
							<p>Clewed offers a new model that more efficiently allocates expertise to its highest and best use. The platform is a transparent marketplace where relevant experts can be applied to help companies improve business outcomes. Unlike the traditional model, Clewed incorporates processes and toolsets, which integrate strategic formulation and implementation advisory to accelerate time to value.</p>

							<div style="text-align:center"><h1>The Clewed Model</h1></div>
							<img src="<?php echo $base_url; ?>/themes/maennaco/images/project/venn_diag_3.jpg" alt="The Clewed Model" width="940" style="margin-bottom:22px">
							<p>This new delivery model is an approach applicable to all companies seeking a better way to plan and execute, regardless of the desired outcomes or where they are in their journey to building a scalable and sustainable business. Whether you want to enter new markets, increase your profit margin, or manage risks, Clewed can help you do so like never before.</p>

							<div class="index-text-a-blue" style="margin-top:22px">
								<a data-type="company" class="request_demo" rel="request">» Request Demo</a>&nbsp;
								<?php /*<a style="float: left;margin-left:50px;" href="/about/what-we-belive">» Learn More</a> */?>
							</div>
						</div>
						<div id="problock" class="imageBlock jsblock"<?php if (empty($_REQUEST['pro'])) echo ' style="display: none;"' ?>>
							<p>Today, highly qualified professionals are spending too much time trying to find clients and differentiate their services. Pressure to close can often lead to poor alignment between capabilities and client requirements. The only means to maintain margins and recoup these inefficiencies is by charging higher prices.</p>
							<div style="text-align:center"><h1>The Traditional Model</h1></div>
							<img src="<?php echo $base_url; ?>/themes/maennaco/images/project/venn_diag_2.jpg" alt="The Traditional Model" width="940" style="margin-bottom:22px">
							<p>The Clewed platform is a transparent marketplace that increases the efficiency by which professionals serve companies. The process and toolsets integrated in the platform help you communicate and sell your expertise more easily so you can quickly add value and reach more companies.</p>

							<div style="text-align:center"><h1>The Clewed Model</h1></div>
							<img src="<?php echo $base_url; ?>/themes/maennaco/images/project/venn_diag_3.jpg" alt="The Clewed Model" width="940" style="margin-bottom:22px">
							<p>If you are an industry or investment analyst, operating executive, subject matter expert or consultant, join Clewed to reach a broader audience and monetize your expertise like never before.</p>

							<div class="index-text-a-blue" style="margin-top:22px">
								<a data-type="professional" class="request_demo" rel="request">» Request Demo</a>&nbsp;
								<?php /*<a style="float: left;margin-left:50px;" href="/about/what-we-belive">» Learn More</a> */?>
							</div>
						</div>						
                    </div>
                </div>
            </div>
        </div>
        <?php //require 'themes/maennaco/dialogs/register.php'; ?>
    </div>
    <!-- /container -->
    <?php require_once("themes/maennaco/footer-regular.php");?>
</div>
<div id="policy" style="font-family: 'Lato Regular'; font-size:12px; display:none;"></div>
</body>
</html>
