<?php

$name = "maennaco_";
$pass = "hcfVr46t146G6";
$host = "localhost";
$db = "maennaco";

$link = mysql_connect($host, $name, $pass);
mysql_select_db($db);
$sql = "select * from research_people_data where data_type = 'progress'";

$result = mysql_query($sql);

while(($row = mysql_fetch_assoc($result)) !== false)
{
    $data = '';
    $followup = '';
    $id = $row['dataid'];
    $data_attr = $row['data_attr'];
 
    list($date, $followup) = split("\+", $data_attr);
    if($date) $date = strtotime($date);
    else{$date = '';}
    if($followup)$followup = strtotime($followup);
    else{$followup = '';}
    echo "<br> id - $date - $followup";
    if($id)
    {
        $sql = "update research_people_data set data_attr = '$followup', data_value3 = '$date' where dataid = $id";
        mysql_query($sql) or die(mysql_error());
    }
}

?>