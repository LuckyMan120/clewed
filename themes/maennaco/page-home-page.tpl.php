<?php
global $base_url;
global $user;
// $Id: page.tpl.php,v 1.18.2.1 2009/04/30 00:13:31 goba Exp $
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN""http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>"
      lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
<head>
    <title><?php print $head_title ?></title>
    <?php print str_replace('?Q', '?I', $styles); ?>
    <?php print str_replace('?Q', '?I', $scripts); ?>
    <!--[if lt IE 7]>
    <?php print phptemplate_get_ie_styles(); ?>
    <![endif]-->
    <?php if ($user->uid > 1) : ?>
        <script type="text/javascript">
            window.location = '/account';
        </script>
    <?php endif; ?>
    <link type="text/css" rel="stylesheet" href="/css/homepage.css"/>
    <link type="text/css" rel="stylesheet"
          href="/themes/maennaco/jui/css/redmond/homepagecss/jquery-ui-1.8.15.custom.css"/>
    <link type="text/css" rel="stylesheet" href="/themes/maennaco/jquery.rating.css"/>
    <link rel="stylesheet" type="text/css" href="/js/slick/slick.css"/>
    <link rel="stylesheet" type="text/css" href="/js/slick/slick-theme.css"/>
    <link rel="stylesheet" href="/themes/maennaco/custom-styles/owl.carousel.min.css">
    <!--    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-1.6.2.min.js"></script>-->
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.8.1/jquery.min.js"></script>
    <script src="/themes/maennaco/custom-script/owl.carousel.min.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/js/jquery-ui-1.9.2.custom.min.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js"></script>
    <script type="text/javascript" src="/js//slick/slick.js"></script>
    <script type="text/javascript" src="/js/homepage.js"></script>
    <script type="text/javascript" src="/js/jquery.rating.js"></script>
    <script type="text/javascript" src="/js/register-dialog.js"></script>
</head>
<body<?php print phptemplate_body_class($left, $right); ?>>

    <div id="fb-root"></div>
    <script type="text/javascript">
        /* start => owl carousel connect*/
        $(document).ready(function () {
            // console.log($('.owl-carousel'));
            $('.owl-carousel').owlCarousel({
                items: 1,
                loop: true,
                nav: true,
                autoplay: true,
                autoplayTimeout: 5000,
                autoplayHoverPause: true
            });
            ;
        });
        /* end => owl carousel connect*/

        (function (d, s, id) {
            var js, fjs = d.getElementsByTagName(s)[0];
            if (d.getElementById(id)) return;
            js = d.createElement(s);
            js.id = id;
            js.src = "//connect.facebook.net/en_GB/all.js#xfbml=1&appId=267809316574109";
            fjs.parentNode.insertBefore(js, fjs);
        }(document, 'script', 'facebook-jssdk'));

        function show_hide(id_show, id_hide, bidblue, bidblack) {
            if (id_show == 'tabs-1') {
                window.utype = 'company';
            } else if (id_show == 'tabs-2') window.utype = 'professional';
            document.getElementById(id_show).style.display = 'block';
            document.getElementById(id_hide).style.display = 'none';
            document.getElementById(bidblue).style.backgroundColor = '#00a2bf';
            document.getElementById(bidblack).style.backgroundColor = '#D0D2D2';
            document.getElementById('form-submit1').style.display = 'block';
            document.getElementById('form-submit2').style.display = 'block';
        }


    </script>

    <div id="wrapper">
        <div id="container" class="clear-block" style="margin-top:0px !important;">
            <!-- start => header -->
            <?php require_once("header-regular.php"); ?>
            <!-- end => header -->
        </div>

        <!-- start => section 1 --- let your business snowball  -->
        <div class="custom-let-your-business">
            <div class="c-container">
                <div class="custom-let-your-title">
                    <h1>
                        Let your business
                        <b>snowball.</b>
                    </h1>
                    <p>
                        A new way to accelerate, transform and fund
                        more companies at low cost.
                    </p>
                    <div class="custom-started-demo">
                        <div class="get-started nav-register">get started</div>
                        <a href="/how-it-works.php">
                            <div class="get-a-demo ml-10-px">get a demo</div>
                        </a>
                    </div>
                </div>
            </div>
        </div>
        <!-- end => section 1 --- let your business snowball  -->

        <!-- start => section 2 --- a whole new way content  -->
        <div class="custom-whole-content">
            <div class="c-container">
                <div class="whole-title">
                    <h1>
                        <b>Unlocking</b> the service and capital markets
                    </h1>
                </div>
                <div class="whole-point">
                    <ul>
                        Clewed is a new type of service and capital infrastructure for lower middle-market companies and
                        investors. Our platform combines technology with actual human assistance to improve the knowledge,
                        performance, and purchasing power of traditionally underserved, smaller companies.
                        <br>
                        For investors,
                        our software reduces the expensive yet necessary diligence and risk management costs while creating
                        transparency that builds trust. This approach enables us to provide smaller companies with low-cost
                        capital from a larger pool of traditionally unavailable investors, while offering many investors
                        opportunities formerly reserved for bigger institutions.
                    </ul>
                </div>
            </div>
        </div>
        <!-- end => section 2 --- a whole new way content  -->

        <!-- start => section 3 --- explore sample project   -->
        <div class="custom-explore-sample">
            <div class="c-container">
                <div class="custom-sample-projects">
                    <div class="open-title">
                        <h1>
                            Get managed services at <b class="bold-black">low cost</b>
                        </h1>
                        <p>
                            Create a project account and complete five simple questions.
                        </p>
                        <p>
                            A talented analyst will help you scope, manage and get work done.
                        </p>
                        <h1 class="pt-15pt f-20-px">
                            <b>Browse sample projects</b>
                        </h1>
                    </div>
                </div>
                <?php include "companies/featured-grid.php";?>
            </div>
        </div>
        <!-- end => section 3 --- explore sample project   -->

        <!-- start => section 4 --- company register content   -->
        <div class="custom-register-company">
            <div class="c-container">
                <div class="custom-company-title">
                    <div class="open-title">
                        <h1>
                            A world of top
                            <b>financial & </b>
                        </h1>
                        <h1>
                            <b>strategic</b>
                            services at your fingertips
                        </h1>
                    </div>
                </div>
                <div class="register-wrapper-bottom">
                    <div class="custom-register-point">
                        <ul>
                            <li>
                                <div class="title-content-point">
                                    <div class="custom-style-img">
                                        <img src="/themes/maennaco/custom-images/yes.png" alt="okay">
                                    </div>
                                    <h1>Clear price, low-cost</h1>
                                </div>

                                <p>Our platform simplifies and reduces fees.
                                    No hourly rates. Clear, project-based pricing.
                                </p>
                            </li>
                            <li>
                                <div class="title-content-point">
                                    <div class="custom-style-img">
                                        <img src="/themes/maennaco/custom-images/yes.png" alt="okay">
                                    </div>
                                    <h1>Quality people and work</h1>
                                </div>
                                <p>
                                    Work with curated professionals hard to access
                                    traditionally.
                                </p>
                            </li>
                            <li>
                                <div class="title-content-point">
                                    <div class="custom-style-img">
                                        <img src="/themes/maennaco/custom-images/yes.png" alt="okay">
                                    </div>
                                    <h1>Your payments, protected</h1>
                                </div>
                                <p>
                                    You’ll know what you pay upfront. We release
                                    your payments when you approve the work.
                                </p>
                            </li>
                            <li>
                                <div class="title-content-point">
                                    <div class="custom-style-img">
                                        <img src="/themes/maennaco/custom-images/yes.png" alt="okay">
                                    </div>
                                    <h1>Fast, human support</h1>
                                </div>
                                <p>
                                    Our analysts know business and are available
                                    to help anytime, anywhere.
                                </p>
                            </li>
                        </ul>
                    </div>
                    <div class="custom-register-form">
                        <div class="custom-reg-form">
                            <div class="custom-form-content" id="custom-register-company-form-content">
                                <div class="first-last-name-content">
                                    <input class="required first-last-name" type="text" placeholder="First Name"
                                           id="company-firstname"/>
                                    <input class="required first-last-name" type="text" placeholder="Last Name"
                                           id="company-lastname"/>
                                </div>
                                <div class="reg-input-content">
                                    <input class="required reg-input" type="text" placeholder="Your Email Address"
                                           id="company-email"/>
                                    <input class="required reg-input m-10-0" type="password" placeholder="Your Password"
                                           id="company-password"/>
                                    <input class="required reg-input" type="text" placeholder="Company Name"
                                           id="company-name"/>
                                </div>
                                <div class="custom-reg-select">
                                    <div class="reg-sel-arrow">
                                        <select name="company-revenue" id="company-revenue" class="reg-select">
                                            <option value="revenue">Revenue</option>
                                            <?php
                                                $Revenus = RevenuesData();
                                                foreach ($Revenus as $key => $val) {
                                                    echo "\n<option value='$key'>$val</option>";
                                                }
                                            ?>
                                        </select>
                                    </div>
                                    <div class="reg-sel-arrow reg-sel-arrow-two">
                                        <select name="company-industry" id="company-industry" class="reg-select">
                                            <option value="industry">Industry</option>
                                            <?php
                                                $Sectors = _INDUSTRY();
                                                foreach ($Sectors as $key => $value) {
                                                    echo "<optgroup label=\"$key\">";
                                                    foreach ($value as $key1 => $value1) echo "<option value=\"$key1\">$value1</option>";
                                                    echo '</optgroup>';
                                                }
                                            ?>
                                        </select>
                                    </div>
                                </div>
                                <span class="custom-agree-terms">
                                    <div class="round">
                                      <input type="checkbox" id="cmp-agree"/>
                                      <label for="cmp-agree"></label>
                                        <span class="custom-span">
                                            I agree with Clewed's
                                            <a href="#" class="show_terms" type="terms"
                                               style="color:#00a1be; font-family:'Lato Italic', sans-serif;">
                                                Terms
                                            </a>
                                            and
                                            <a href="#" class="show_terms" type="privacy"
                                               style="color:#00a1be; font-family:'Lato Italic', sans-serif;">
                                                Privacy Policy
                                            </a>
                                        </span>
                                    </div>
                                </span>
                                <div class="custom-button-by-show-more">
                                    <div class="form-submit-register-company custom-show_more_cmp" rel="show" type="pastable">
                                        get started
                                    </div>
                                </div>
                                <div class="already-have-account">
                                    Already have account?
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        <!-- end => section 4 --- company register content   -->

        <!-- start => section 5 --- insight and research   -->
        <div class="custom-insight-and-research">
            <div class="c-container">
                <div class="custom-research-projects">
                    <div class="open-title">
                        <h1>
                            Simply buy pre-packaged services
                            <b>hassle-free</b>
                        </h1>
                        <p>
                            Do I have the right KPI? Is my capital structure optimal?
                        </p>
                        <p>
                            Why is my margin low? Buy clear, simplified services by vetted top professionals.
                        </p>
                    </div>
                </div>
                <?php include "insight/featured-grid-service.php";?>
            </div>
        </div>
        <!-- end => section 5 --- insight and research   -->

        <!-- start => section 6 --- simple buy pre packaged -->
        <div class="custom-insights-and-expert">
            <div class="c-container">
                <div class="custom-expert-projects">
                    <div class="open-title">
                        <h1>
                            Access <b>insights and expert</b> analysis on market changes
                        </h1>
                        <p>
                            Experts on our platform host insight discussions to help you research.
                        </p>
                        <p>
                            Join live calls, listen to past topics, request custom insights and more.
                        </p>
                    </div>
                </div>
                <?php include "insight/featured-grid-insight.php";?>
            </div>
        </div>
        <!-- end => section 6 --- simple buy pre packaged   -->

        <!-- start => section 7 --- carousel section -->
        <div class="custom-carousel-content">
            <div class="c-container">
                <div class="slider-content">
                    <div class="owl-carousel owl-theme">
                        <div class="custom-slider-item">
                            <div class="open-title">
                                “Truly fantastic work. Clewed has become an extension of our business.”
                            </div>
                            <div class="custom-slider-bottom-content">
                                <h1>
                                    MIDDLE MARKET PRIVATE EQUITY FIRM, CEO
                                </h1>
                                <p>
                                    Acquisition diligence, risk analysis, research, portfolio support
                                </p>
                            </div>
                        </div>
                        <div class="custom-slider-item">
                            <div class="open-title">
                                “Within six months, we’ve gotten a year’s value and our team has become smarter.”
                            </div>
                            <div class="custom-slider-bottom-content">
                                <h1>
                                    $20 MILLION BUILDING MATERIALS COMPANY, COO
                                </h1>
                                <p>
                                    Growth advisory: Strategy, product pricing, balance sheet improvements, KPIs, more
                                </p>
                            </div>
                        </div>
                        <div class="custom-slider-item">
                            <div class="open-title">
                                “High quality work, fast delivery. We won a mandate over best known, established firms in
                                our industry by leveraging Clewed.”
                            </div>
                            <div class="custom-slider-bottom-content">
                                <h1>
                                    $50 MILLION DEFENCE CONSULTING FIRM.SENIOR EXECUTIVE
                                </h1>
                                <p>
                                    Defense industrial base supply chain research on 15 countries
                                </p>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        <!-- end =>  section 7 --- carousel section   -->

        <!-- start => section 8 --- unlock opportunities section -->
        <div class="custom-unlock-opportunities">
            <div class="c-container">
                <div class="custom-unlock-projects">
                    <div class="open-title">
                        Access intelligence.
                        <b> Unlock opportunities.</b>
                        Accelerate growth.
                    </div>
                </div>
                <div class="custom-card-content">
                    <div class="custom-completed-card">
                        <div class="image-content">
                            <img src="/themes/maennaco/custom-images/puzl1.jpg" alt="pazl">
                        </div>
                        <div class="text-content">
                            <h1>The power of intelligence</h1>
                            <p>
                                At Clewed, we vet and organize your data and convert it to actionable intelligence that
                                fuels your business, harmonizes your people, and connects you to the professionals and
                                partners you need to achieve results.
                            </p>
                        </div>
                    </div>
                    <div class="custom-completed-card">
                        <div class="image-content">
                            <img src="/themes/maennaco/custom-images/diagram1.jpg" alt="diagram">
                        </div>
                        <div class="text-content">
                            <h1>Gain purchasing power</h1>
                            <p>
                                Our platform enables talented people to serve multiple companies in one place from anywhere,
                                without wasting time. This allows clients to gain purchasing power and save money, sharing
                                top resources they couldn’t access before.
                            </p>
                        </div>
                    </div>
                    <div class="custom-completed-card">
                        <div class="image-content">
                            <img src="/themes/maennaco/custom-images/partner1.jpg" alt="partner">
                        </div>
                        <div class="text-content">
                            <h1>Attract the right capital</h1>
                            <p>
                                Our solutions reduce the traditional diligence and risk management costs while creating the
                                transparency you need to develop trust and attract a broader network of owner-aligned
                                investors.
                            </p>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        <!-- end => section 8 --- unlock opportunities section   -->

        <!-- start => section 9 --- join the hundreds section -->
        <?php require 'companies/companies-join.php'; ?>
        <!-- end => section 9 --- join the hundreds section   -->

        <!-- start => footer -->
        <?php require_once("footer-regular.php"); ?>
        <!-- end => footer -->

    </div>
    <script type="text/javascript">
        (function (i, s, o, g, r, a, m) {
            i['GoogleAnalyticsObject'] = r;
            i[r] = i[r] || function () {
                (i[r].q = i[r].q || []).push(arguments)
            }, i[r].l = 1 * new Date();
            a = s.createElement(o),
                m = s.getElementsByTagName(o)[0];
            a.async = 1;
            a.src = g;
            m.parentNode.insertBefore(a, m)
        })(window, document, 'script', '//www.google-analytics.com/analytics.js', 'ga');

        ga('create', 'UA-43665911-1', 'clewed.com');
        ga('send', 'pageview');

    </script>

</body>
</html>
