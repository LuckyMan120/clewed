<?php
/**
 * @var array $attendees
 */
error_reporting(E_ALL);
global $base_url;
global $user;
global $ifAdmin;
global $AccessObj;

$user_id = $userId = (int) $user->uid;
$pro_id = $offerId = (int) sget($_REQUEST, 'pro_id');
$ifModerator = ifDiscussionModerator($pro_id, $user_id);
$ifAttended = !empty($attendees[$user_id]);
$ifAdmin = $ifAdmin || $AccessObj->user_type == 'super';
$ifOwner = $_REQUEST['id'] == $user_id;

$fileDirectAccessAttempt = 0;
$twig = new Twig_Environment(new Twig_Loader_Filesystem(ROOT . '/templates'));

$companyViewHelper = new Clewed\Company\Featured\View\Helper();

$personsToShow = 60; ?>

<style type="text/css">
    div.content_box {
        z-index: 10 !important;
    }

    div#docprev {
        text-align: left !important;
    }

    #cardholder .card {
        margin-bottom: 0 !important;
    }

    .pro-participations {
        float:right;
        border-left: solid 1px #006274;
        height: 15px;
        padding-left: 3px;
        margin-top: 15px;
        margin-right:25px;
        padding-bottom: 4px;
    }
</style>
<div class='month_2' style='border-bottom:none;'>
    <?php if ($count > 0): ?>
        <?php $counter = 0;?>
        <?php foreach ($attendees as $attendee): ?>
            <?php include "pro_participation_item.php"; ?>
            <?php if(++$counter >= $personsToShow) break; ?>
        <?php endforeach;?>
    <?php endif; ?>
</div>
<?php if($count > $personsToShow):?>
    <div><a href="#" class="tool pro-participations">More</a></div>
    <?php echo js_init('init_pro_participations();init_pro_participations_dialog("Joined (' . $count . ')");');?>
    <div id='pro-participations-dialog' style='display:none;'>
    <?php foreach ($attendees as $attendee):?>
        <?php include "pro_participation_item.php";?>
    <?php endforeach;?>
    </div>
<?php endif; ?>
<div style="height:40px;clear:both;"></div>
