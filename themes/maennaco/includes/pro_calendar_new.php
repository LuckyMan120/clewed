<?php
error_reporting(0);
global $base_url;
include('dbcon.php');
global $user;
global $AccessObj;
$usertype = $AccessObj->user_type;
$tab = sget($_REQUEST, 'tab');
//$user_id = $user->uid;
if ($_REQUEST['id'] == '') {
    $user_id = $user->uid;
    $pagename = 'insights';
    $uid = '';
} else {
    $user_id = $_REQUEST['id'];
    $pagename = '';
    $uid = $_REQUEST['id'];
}
if ($tab != 'insights' && $tab != 'services') {
    ?>
    <link rel='stylesheet' type='text/css' href='<?php echo $base_url; ?>/themes/maennaco/fullcalendar.css'/>
    <script type='text/javascript' src='<?php echo $base_url; ?>/themes/maennaco/fullcalendar.js'></script>
    <script type='text/javascript'>

        $(document).ready(function () {

            $('#calendar').fullCalendar({
                header: {
                    left: 'prev ',
                    center: 'title',
                    right: 'next'
                },
                editable: false,

                events: "<?php echo $base_url; ?>/themes/maennaco/includes/json-events.php?user_id=<?php echo $user_id;?>&pagename=<?php echo $pagename; ?>",
                eventRender: function (event, element, view) {
                    if (event.start.getMonth() != view.start.getMonth())
                        return false;
                },

                eventDrop: function (event, delta) {
                    alert(event.title + ' was moved ' + delta + ' days\n' +
                    '(should probably update your database)');
                },

                loading: function (bool) {
                    if (bool) $('#loading').show();
                    else $('#loading').hide();

                    $('.fc-event-hori').css({height: '35px', top: '-=24'});
                }

            });

        });

    </script>
    <style type='text/css'>
        #loading {
            position: absolute;
            top: 5px;
            right: 5px;
        }

        #calendar {
            background-color: #fafbfb;
            width: 265px;
            margin: 0px auto 20px -10px;
        }
    </style>


    <div id='loading' style='display:none'></div>
    <div id='calendar'></div>
<?php } ?>

<script type="text/javascript"
        src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>

<script type="text/javascript">
    $(".category").livequery("click", function (e) {
        e.preventDefault();
        $('#categories').toggle();
    });

    $(".month").livequery("click", function (e) {
        e.preventDefault();
        $('#months').toggle();
    });

    $(".prof").livequery("click", function (e) {
        e.preventDefault();
        $('#professional').toggle();
    });

    $("#myinsights").livequery("click", function (e) {
        e.preventDefault();
        $('#showinsights').toggle();
    });
</script>

<style>
    .Categories a:hover{ color:#686b72; text-decoration:none!important; }

    .Categories_2{ margin:0px; padding:0px;}

    .month_2{ margin:0px; padding:0px;}
    .right-td{
        display: table-cell!important;
    }
</style>

<div class="categories">
    <div class="box_title shaded_title"><span id="rghttd_title">Filter</span></div>
    <?php if ($usertype == 'super' || $usertype == 'admin') {

        echo '<a href="/account?tab=insights&ftype=live"><strong style="color:';
        if ($_REQUEST['ftype'] == 'live') {
            echo '#00aad6;';
        } else {
            echo '#686b72;';
        }
        echo 'font-style:italic;">Live</strong></a><br><a href="/account?tab=insights&ftype=progressing"><strong style="color:';
        if ($_REQUEST['ftype'] == 'progressing') {
            echo '#00aad6;';
        } else {
            echo '#686b72;';
        }
        echo 'font-style:italic;">Progressing</strong></a>';
    } ?>

    <div class="Categories">
        <div class="left_border"><a class="category" style="display: block; cursor:pointer;">
                <strong>Categories</strong></a></span>
        </div>
        <div id="categories" style="<?php if (!isset($_GET['sort']) || $_GET['sort'] == "") {echo "display:none;";} ?>">
            <div class="Categories_2">
                <ul class="right_filter">
                  <?php

                    if ( $tab == 'insights' || $tab == 'services' ) {
                        $categories = mysql_query("SELECT * FROM maenna_professional group by tags") or die(mysql_error());
                    } else {
                        $cat_sql = "SELECT * FROM maenna_professional where postedby = %d group by tags";
                        $categories = db_query($cat_sql, array($user_id));
                    }
                    while ($resCategories = mysql_fetch_array($categories)) {
                        if ($resCategories['tags'] == 'Choose a Category' || $resCategories['tags'] == '') {
                            continue;
                        } ?>
                        <li<?php echo ($_GET['sort'] == $resCategories['tags'])? ' style="background-color:#e3eef2;"':'';?>>
                            <?php
                            if ($tab == 'insights') { ?>
                                <a href="<?= $base_url ?>/account?tab=insights&sort=<?= $resCategories['tags'] ?><?= empty($_REQUEST['sortmonth']) ? '' : '&sortmonth=' . $_REQUEST['sortmonth'] ?>"><?= $resCategories['tags'] ?></a>
                            <?php } elseif ($tab == 'services') { ?>
                                <a href="<?= $base_url ?>/account?tab=services&sort=<?= $resCategories['tags'] ?><?= empty($_REQUEST['sortmonth']) ? '' : '&sortmonth=' . $_REQUEST['sortmonth'] ?>"><?= $resCategories['tags'] ?></a>
                            <?php } else { ?>
                                <a href="<?= $base_url ?>/account?tab=professionals&page=pro_detail&id=<?= $resCategories['postedby'] ?>&section=pro_industry_view&type=discussion&sort=<?= $resCategories['tags'] ?><?= empty($_REQUEST['sortmonth']) ? '' : '&sortmonth=' . $_REQUEST['sortmonth'] ?>"><?= $resCategories['tags'] ?></a>
                            <?php } ?>
                        </li>
                    <?php }
                    ?></ul>
            </div>

        </div>

        <div class="left_border">
            <span>
                <a class="month" style="cursor:pointer;">
                    <strong>Month</strong>
                </a>
            </span>
        </div>
        <div id="months" style="<?php if (!isset($_GET['sortmonth']) || $_GET['sortmonth'] == "") {echo "display:none;";} ?>">
            <div class="month_2">
                <ul class="right_filter">
                    <?php
                    $months = array(
                        '01' => 'January',
                        '02' => 'February',
                        '03' => 'March',
                        '04' => 'April',
                        '05' => 'May',
                        '06' => 'June',
                        '07' => 'July',
                        '08' => 'August',
                        '09' => 'September',
                        '10' => 'October',
                        '11' => 'November',
                        '12' => 'December'
                    );
                    foreach ($months as $key => $value) { ?>
                        <li <?php echo ($_GET['sortmonth'] == $key)? ' style="background-color:#e3eef2;"': ''?>>
                        <?php
                        if ( $tab == 'insights' ) { ?>
                            <a href="<?= $base_url ?>/account?tab=insights&sortmonth=<?= $key ?><?= empty($_REQUEST['sort']) ? '' : '&sort=' . $_REQUEST['sort'] ?>"><?= $value ?></a></li>
                        <?php } elseif ($tab == 'services') { ?>
                            <a href="<?= $base_url ?>/account?tab=services&sortmonth=<?= $key ?><?= empty($_REQUEST['sort']) ? '' : '&sort=' . $_REQUEST['sort'] ?>"><?= $value ?></a></li>
                        <?php } else { ?>
                            <a href="<?= $base_url ?>/account?tab=professionals&page=pro_detail&id=<?= $uid ?>&section=pro_industry_view&type=discussion&sortmonth=<?= $key ?><?= empty($_REQUEST['sort']) ? '' : '&sort=' . $_REQUEST['sort'] ?>"><?= $value ?></a></li>
                        <?php } ?>

                    <?php
                    }
                    ?>
                </ul>
            </div>
        </div>
    </div>