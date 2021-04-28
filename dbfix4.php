<?php
error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('display_startup_errors', TRUE);

$name = "maennaco_";
$pass = "hcfVr46t146G6";
$host = "localhost";
$db = "maennaco";

$link = mysql_connect($host, $name,$pass);
mysql_select_db($db);
$sql = "select * from users,users_data where name like 'mu%' and users.uid = users_data.uid";

$result = mysql_query($sql);
$Param = array();
while(($row = mysql_fetch_assoc($result)) !== false)
{
    $Param = array();
    $Param['pid'] = sget($row,'uid');
    $Param['dataid'] = sget($row,'dataid');
    $Param['data_type'] = sget($row,'data_type');
    $Param['data_value'] = sget($row,'data_value');
    $Param['data_value2'] = sget($row,'data_value2');
    $Param['data_attr'] = sget($row,'data_attr');
    
    
    //echo "<pre>";
    //print_r($Param);
    //echo "</pre>"; exit;
    
    $KEYS = array_keys($Param);
    $M = array();
    foreach($Param as $key => $val)
    {
        $M[] = "'%s'";
    }
    $m = implode(',', $M);
    $k = implode(',', $KEYS);
    $sql = "insert into maenna_people_data ($k) values($m)";
    $sql = vsprintf($sql, $Param);
    if(mysql_query($sql))
    {
        echo "OK";
    }else{
        echo $sql . mysql_error();
        exit;
    }
    
    
    echo "<pre>";
   // print_r($row);
    print_r($Param);
    echo "</pre>";
}

function sget($array, $key, $flag = "")
{
    if( ( ! isset($array)) || (! isset($key)))
    {
        echo "invalid parameters";
        return false;
    }
    if(isset($array["$key"])){
        if( $flag == "string" ) return trim($array["$key"]);
        elseif($flag == 'ini') return (int)preg_replace("/[^0-9\.\-]/", '', $array["$key"]);
        elseif( $flag == "decimal" ) return preg_replace("/[^0-9\.\-]/", '', $array["$key"]);
        else return $array["$key"];
    }else{
        return "";
    }
}
?>