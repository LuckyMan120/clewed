<?php
/**
 * Created by PhpStorm.
 * User: Ihor Borysyuk
 * Date: 7/26/15
 * Time: 7:46 PM
 */

require_once __DIR__."/insight_preview.php";

function renderInsightsPreview($user, $sort, $sortmonth, $sortdate, $start = 0, $limit = -1, $type = '')
{

    $fields = 'maenna_professional.*, maenna_people.pid as uid, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname," ", maenna_people.lastname)) as firstname, maenna_people.experties as pexperties, maenna_people.protype';
    $joins = 'LEFT JOIN maenna_people ON maenna_people.pid = maenna_professional.postedby';
    $sql_result = "SELECT $fields FROM  `maenna_professional` $joins WHERE maenna_professional.approve_status = 1 AND maenna_professional.type IN (0,1) ";
    if ($sort != '') {
        $sql_result .= " AND maenna_professional.tags LIKE '%" . mysql_real_escape_string($sort) . "%'";
    }

    if(!empty($type)) {
        if('insight' === $type)
            $sql_result .= " AND type = 0 ";
        elseif('service' === $type)
            $sql_result .= " AND type <> 0 ";
    }

    if ($sortmonth != '') {
        $sql_result .= " AND MONTH(FROM_UNIXTIME(maenna_professional.datetime)) = " . mysql_real_escape_string($sortmonth);
    }
    if ($sortdate != '') {
        $sql_result .= " AND DATE(FROM_UNIXTIME(maenna_professional.datetime)) = '" . mysql_real_escape_string($sortdate) . "'";
    }
    $sql_result .= " ORDER BY maenna_professional.datetime DESC";


    if ($limit != -1) {
        $sql_result .= ' LIMIT '.$start.", ".$limit;
    }


    $result1 = mysql_query($sql_result) or die(mysql_error());

    if (mysql_num_rows($result1) == 0) {
        echo('No Insight scheduled.');
        $cnt = 0;
    } else {
        $cnt = mysql_num_rows($result1);
    }

    while ( $row1 = mysql_fetch_array($result1) ) {
        renderInsightPreview($row1, $user);
    }

    return $cnt;
}
