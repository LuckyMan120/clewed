<?php
error_reporting(0);
require_once('dbcon.php');
date_default_timezone_set('EST');

if (array_key_exists('type', $_REQUEST) && $_REQUEST['type'] === 'getProInsights') {
    $id = (int) $_REQUEST['id'];
    $sql = mysql_query("SELECT `id`,`datetime`,`approve_status`,`title` FROM `maenna_professional` WHERE `postedby` = " . $id . " AND `datetime` >= '" . (time() - 3600) . "'");
    $tmp = array();
    while ($row = mysql_fetch_object($sql)) {
        array_push($tmp, array('date' => date('Y-n-j', $row->datetime), 'approved' => $row->approve_status, 'title' => $row->title));
    }
    die(json_encode($tmp));
}

$date = mysql_real_escape_string($_REQUEST['date']);
$date_arr = explode('-', $date);
$date_arr = explode('-', $date);
$creator = (int) $_REQUEST['creator'];

function getProId($id) {
    if (empty($id)) return 'invalid id';
    $sql = "select users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname from users_roles, maenna_people where users_roles.uid = '" . $id . "' and maenna_people.pid = '" . $id . "' limit 1";
    $result1 = mysql_query($sql);
    $Row = mysql_fetch_assoc($result1);
    if (empty($Row)) return "invalid user role setting - $id";
    $firstname = ucwords($Row['firstname']);
    return $firstname;
}

$sql = mysql_query("SELECT `id`,`datetime`,`approve_status`,`postedby` FROM `maenna_professional` WHERE DATE_FORMAT(FROM_UNIXTIME(`datetime`), '%m-%d-%Y') = '" . $date . "'");
$insights = array();
while ($row = mysql_fetch_object($sql)) {
    $quater = date('H', $row->datetime) * 4 + floor(date('i', $row->datetime) / 15);
    for ($i = 0; $i < 4; $i++) { //todo: 4 = one hour duration from setting
        if ($row->postedby == $creator) {
            $me[$quater + $i] = $row->approve_status;
        } else {
            $insights[$row->postedby][$quater + $i] = $row->approve_status;
        }
    }
}
?>

<div id="insightTable" style="overflow-y: scroll;height: 390px" class="insightTables">
                        <h1 style="font-size:14px;text-align: center;">Select time for <?php echo date('l F j, Y',  strtotime($date_arr[1].'-'.$date_arr[0].'-'.$date_arr[2]))?></h1>
<table class="tableTime">
    <tr style="font-size: 11px;height: 32px"><td>Time</td>
        <?php $hoursInDay = 0;
        for($i=0;$i<12;$i++) {
//                            if ($date == $curDate && $i < $curHour) continue;
            $time = ($i==0)?12:$i;
            echo '<td colspan="4">'. $time.'am</td>';
            $hoursInDay++;
        }
        for($i=0;$i<12;$i++) {
//                            if ($date == $curDate && $i+12 < $curHour) continue;
            $time = ($i==0)?12:$i;
            echo '<td colspan="4">'. $time.'pm</td>';
            $hoursInDay++;
        }
        ?>
    </tr>
    <?php
    $i = 1;
    echo '<tr><td data-tooltip="' . getProId($creator) . '">Me</td>';
    for ($j = (24 - $hoursInDay) * 4; $j < 24 * 4; $j++) {
        if (isset($me[$j])) {
            if ($me[$j] == 1) {
                $app_class = 'approved';
                $tooltip = 'This time has live insight, please select open times.';
            } else {
                $app_class = '';
                $tooltip = 'You have Insight pending at this time. Please edit the time for that insight to select this time withiout a conflict.';
            }
            $class = 'class="disabled ' . $app_class . '" data-tooltip="' . $tooltip . '" data-disabletime="' . $date . '"';
        } else {
            $class = '';
        }
        $CurrId = date('H') * 4 + floor(date('i') / 15);
        $currDate = date("m-d-Y");
        $date1 = explode(" ", $date);
        if ($currDate == $date1[0] && $j <= $CurrId) {
            $class = 'class="disabled" data-tooltip="You cannot set Insight time to past. Please select future time."';
        }
        echo "<td rel='time$j' {$class} data-disabledbyme='false'></td>";
    }
    echo '</tr>';
    $app_class = '';
    $b = 1;
    foreach ($insights as $id => $row) {
        echo '<tr><td data-tooltip="' . getProId($id) . '">' . $b++ . '</td>';
        for ($j = (24 - $hoursInDay) * 4; $j < 24 * 4; $j++) {
            if (isset($row[$j])) {
                if ($row[$j] == 1) $app_class = 'approved'; else $app_class = '';
                $class = ' class="bGray ' . $app_class . '"';
            } else {
                $class = '';
            }
            echo "<td $class></td>";
        }
        echo '</tr>';
        $i++;
    }
    ?>
</table>
</div>
