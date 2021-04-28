<link rel="stylesheet" href="/themes/maennaco/custom-styles/custom-style.css">

<div class="navbar-menu">
    <div class="navbar-logo">
        <a href="/">
            <img src="<?php echo $base_url; ?>/themes/maennaco/images/index_logo_1.png" alt="website logo" border="0">
        </a>
    </div>
    <div class="custom-nav-top">
        <?php $currentPage = $_SERVER["REQUEST_URI"]; ?>
        <div class="navbar-item-content">
            <ul class="nav-item-content">
                <li class="nav-item first ">
                    <a href="#!">
                        What
                        <br>
                        we do
                    </a>
                </li>
                <li class="nav-item first  <?php if ($currentPage == "/companies") echo "selected"; ?>">
                    <a href="/companies">
                        Discover
                        <br>
                        Opportunities
                    </a>
                </li>
                <li class="nav-item last <?php if ($currentPage == "/buy-services") echo "selected"; ?>">
                    <a href="/buy-services">
                        Buy
                        <br>
                        Services
                    </a>
                </li>
                <li class="nav-item last <?php if ($currentPage == "/join-insights") echo "selected"; ?>">
                    <a href="/join-insights">
                        Join
                        <br>
                        Insights
                    </a>
                </li>
                <li class="nav-item last <?php if ($currentPage == "/professionals") echo "selected"; ?>">
                    <a href="/professionals">
                        For
                        <br>
                        Professional
                    </a>
                </li>
                <li class="nav-item last <?php if ($currentPage == "/about/the-company") echo "selected"; ?>">
                    <a href="/about/the-company">
                        Learn
                        <br>
                        More
                    </a>
                </li>
            </ul>
        </div>
        <div class="navbar-login-register">
            <div style="display:none">
                <?php get_user_status_home($user); ?>
            </div>
            <div class="l-r-content">
                <div class="nav-login">LOG IN</div>
                <div class="nav-register ml-10-px">Sign Up</div>
            </div>
            <div class="loginstuff_div">
                <a class="a-child-1" href="<?php echo $base_url; ?>/user/password">Forgot password?</a>
                <a class="a-child-2" href='/about/contact-us'> feedback?</a>
            </div>
        </div>
    </div>
    <div class="custom-open-close">
        <img src="/themes/maennaco/custom-images/menu.svg" alt="open-close" class="menu-svg">
        <img src="/themes/maennaco/custom-images/cancel.svg" alt="open-close" class="menu-svg menu-close-svg d-n">
    </div>
    <div class="custom-nav-bottom ">
        <?php $currentPage = $_SERVER["REQUEST_URI"]; ?>
        <div class="navbar-item-content">
            <ul class="nav-item-content">
                <li class="nav-item first "><a href="#!">What We Do</a></li>
                <li class="nav-item first  <?php if ($currentPage == "/companies") echo "selected"; ?>"><a
                            href="/companies">Discover Opportunities </a></li>
                <li class="nav-item last <?php if ($currentPage == "/buy-services") echo "selected"; ?>"><a href="/buy-services"> Buy Services </a></li>
                <li class="nav-item last <?php if ($currentPage == "/join-insights") echo "selected"; ?>"><a
                            href="/join-insights">Join Insights </a></li>
                <li class="nav-item last <?php if ($currentPage == "/professionals") echo "selected"; ?>"><a
                            href="/professionals">For Professional</a></li>
                <li class="nav-item last <?php if ($currentPage == "/about/the-company") echo "selected"; ?>"><a
                            href="/about/the-company">Learn More</a></li>
            </ul>
        </div>
        <div class="navbar-login-register">
            <div style="display:none">
                <?php get_user_status_home($user); ?>
            </div>
            <div class="l-r-content">
                <div class="nav-login n-r-hide-show">LOG IN</div>
                <div class="nav-register n-r-hide-show ml-10-px">Sign Up</div>
            </div>
            <div class="loginstuff_div">
                <a class="a-child-1" href="<?php echo $base_url; ?>/user/password">Forgot password?</a>
                <a class="a-child-2" href='/about/contact-us'> feedback?</a>
            </div>
        </div>
    </div>
</div>

<!--<div id="header" style="background:#ffffff!important;">-->
<!--	<div class="logo">-->
<!--		<a href="/">-->
<!--			<img src="--><?php //echo $base_url; ?><!--/themes/maennaco/images/index_logo_1.png" alt="website logo" border="0">-->
<!--		</a>-->
<!--	</div>-->
<!--	--><?php //$currentPage = $_SERVER["REQUEST_URI"];?>
<!---->
<!--	<div class="homemenu">-->
<!--		<div class="item first --><?php //if($currentPage=="/about/the-company") echo "selected"; ?><!--"><a href="/about/the-company">Learn <br/>More</a></div>-->
<!--		<div style="border-left:none;" class="item last --><?php //if($currentPage=="/professionals") echo "selected"; ?><!--"><a href="/professionals">Join as <br/>professional</a></div>-->
<!--		<div class="item last --><?php //if($currentPage=="/insights") echo "selected"; ?><!--"><a href="/insights">Insights &<br>Services</a></div>-->
<!--        <div style="border-left:none;" class="item last --><?php //if($currentPage=="/companies") echo "selected"; ?><!--"><a href="/companies">Discover<br/>Opportunities </a></div>-->
<!--	</div>-->
<!---->
<!--	<div class="loginStuff">-->
<!--		--><?php //if ($user->uid) { ?>
<!--			<a id="my_account" href="/account"-->
<!--			   style="font-family:'Lato Regular', sans-serif; font-size:10px; font-weight:bold;cursor:pointer; margin-left:20px;">-->
<!--				MY ACCOUNT >-->
<!--			</a>-->
<!--			<a href="/logout">-->
<!--				<input type="submit"-->
<!--					   style="display:inline-block; margin:0 0 0 20px; width:97px;font-size:15px;font-family: 'Lato Black', sans-serif;height:32px;"-->
<!--					   name="op" id="edit-submit" value="LOG OUT" class="form-submit">-->
<!--			</a>-->
<!--		--><?php //} ?>
<!--	</div>-->
<!--	<div id="user-status" class="loginStuff">-->
<!--		<div style="display:none">-->
<!--			--><?php //get_user_status_home($user); ?><!--	-->
<!--		</div>-->
<!--		<div>-->
<!--			<a style="cursor: pointer;text-decoration: none;" class='header-login'>LOG IN</a>-->
<!--			<a style="cursor: pointer;text-decoration: none;" class='header-register'>REGISTER</a>-->
<!--		</div>-->
<!--		<br style="clear:both;">-->
<!--		<div class="loginstuff_div">-->
<!--			<a href="--><?php //echo $base_url;?><!--/user/password">Forgot password?</a>-->
<!--			<a href='/about/contact-us'><span class="loginStuff_inner">In beta,</span> feedback?</a>&nbsp;&nbsp;&nbsp;&nbsp;-->
<!--		</div>-->
<!--	</div>-->
<!--	--><?php ////if (!$is_front) get_topnavmenu(); ?>
<!--</div>-->

<!--<style>-->
<!--	.loginstuff_div{-->
<!--		display: flex;-->
<!--		justify-content: space-between;-->
<!--	}-->
<!--	.loginStuff_inner{-->
<!--		color: #898B8E;-->
<!--		font-family: 'Lato Italic'; -->
<!--		font-size: 13px; -->
<!--		font-weight: normal; -->
<!--		padding: 5px 3px 0 0;-->
<!--	}-->
<!--</style>-->
<script>
    let count_open = 0;

    $('.n-r-hide-show').click(function () {
        $('.menu-svg').removeClass('d-n');
        $('.menu-close-svg').addClass('d-n');
        if (count_open === 0) {
            $('.custom-nav-bottom').slideDown(0);
            count_open++;
        } else {
            $('.custom-nav-bottom').slideUp(0);
            count_open = 0;
        }
    });
</script>
<script src="/themes/maennaco/custom-script/custom-script.js"></script>