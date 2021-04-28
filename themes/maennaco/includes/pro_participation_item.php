<?php

if (!isset($fileDirectAccessAttempt) || !isset($AccessObj) || !isset($twig) || !isset($companyViewHelper))
    die;

$baseUrl = isset($baseUrl) ? $baseUrl : '';
$attendee = isset($attendee) ? $attendee : array();

/** @var Clewed\Company\Featured\View\Helper $companyViewHelper */
$companyViewHelper = isset($companyViewHelper) ? $companyViewHelper : null;

$ifAdmin = isset($ifAdmin) ? $ifAdmin : false;
$ifModerator = isset($ifModerator) ? $ifModerator : false;
$ifAttended = isset($ifAttended) ? $ifAttended : false;
$ifOwner = isset($ifOwner) ? $ifOwner : false;

$hasProfile = false;
$class = "";
$attrs = " uid=\"{$userId}\" ";
$style = "";
$href = "";
$id = "attendee-" . $attendee['uid'];
if ($attendee['is_professional']) {
    $class = "pro-card-tool";
    $hasProfile = strlen(trim($attendee['profile'])) > 0;
    if ($hasProfile && ($ifAdmin || $ifModerator || $ifOwner || $ifAttended)) {
        $class .= " profile_details";
        $attrs = " ref=\"pro_id\" pro-card-tooltip=\"{$attendee['uid']}\"";
        $style = "style=\"cursor:pointer;\"";
        $id = 'pro_id' . $attendee['uid'];
    }
} elseif ($attendee['is_company']) {
    $class = "card-tool";
    $hasProfile = strlen(trim($attendee['mission'])) > 0;
    if ($hasProfile && ($ifAdmin || $ifModerator || $ifOwner || $ifAttended)) {
        $href = "href=\"" . $baseUrl . '/account?tab=companies&page=company_detail&id=' . $attendee['uid'] . "\"";
        $attrs = "card-tooltip=\"{$attendee['uid']}\"";
    }
}

$title = htmlspecialchars($attendee['full_name'], ENT_QUOTES, 'utf-8');
$avatar = $baseUrl . '/themes/maennaco/phpthumb/phpThumb.php?src=' . $attendee['avatar'] . '&zc=1&w=40&h=41';

?>

<li style="list-style: none; background: none; margin: 0 10px 10px 0; padding: 0; width: 35px; height: 35px; float: left;">
    <a id="<?= $id;?>" class="<?= $class; ?>" <?= $style; ?> <?= $href; ?> title="<?= $title; ?>" <?= $attrs; ?>>
        <img src="<?= $avatar; ?>" width="40" height="40"/>
    </a>
</li>

<?php if ($hasProfile): ?>
<div uid="<?= $attendee['uid']; ?>" style="display:none">
    <?php if ($attendee['is_company']):

        $company = $attendee;
        $company['sector'] = $companyViewHelper->buildSectorString($attendee);
        $company['revenue'] = $companyViewHelper->buildRevenueString($attendee);
        $company['city_state'] = $companyViewHelper->buildCityStateString($attendee);
        $company['mission'] = strlen($company['mission']) > 100 ? (substr($company['mission'], 0, 100) . '...') : $company['mission'];
        $company['mission'] = strip_tags($company['mission']);

        echo $twig->render('cards/company.twig', array(
            'company' => $company,
            'baseUrl' => $baseUrl,
        ));

    elseif ($attendee['is_professional']):

        $pro = $attendee;
        $pro['exp'] = rtrim(
            ($pro['experties']  != '' ? $pro['experties']  . ', ' : '') .
            ($pro['experties2'] != '' ? $pro['experties2'] . ', ' : '') .
            ($pro['experties3'] != '' ? $pro['experties3'] . ', ' : ''),
            ', '
        );

        $pro['education'] = rtrim(
            ($pro['graduate']      != '' ? $pro['graduate'] . '; ' : '') .
            ($pro['undergraduate'] != '' ? $pro['undergraduate'] . '; ' : ''),
            '; '
        );

        echo $twig->render('cards/pro.twig', array(
            'pro' => $pro,
            'baseUrl' => $baseUrl,
        ));

    endif; ?>
</div>
<?php endif; ?>