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
$sql = "select * from users,users_extend where name like 'sid%' and users.uid = users_extend.uid";

$result = mysql_query($sql);
$Param = array();
while(($row = mysql_fetch_assoc($result)) !== false)
{
    $Param = array();
    $Param['companyid'] = sget($row,'uid');
    $Param['email'] = sget($row,'mail');
    $Param['company'] = sget($row,'company_name');
	$Param['firstname'] = sget($row,'first_name');
	$Param['lastname'] = sget($row,'last_name');
    $Param['address1'] = sget($row,'stree1');
    $Param['address2'] = sget($row,'stree2');
    $Param['city'] = sget($row,'city');
    $Param['state'] = sget($row,'state');
    $Param['sector'] = sget($row,'sector');
    $Param['hear_about'] = sget($row,'hear_about');
    $Param['referral_code'] = sget($row,'referral_code');
    $Param['description'] = sget($row,'description');
    $Param['founded'] = date("Y") - (sget($row,'year_in_biz')) * 1;
    $Param['contact'] = sget($row,'first_name') . ' ' . sget($row,'last_name');
    $Param['web'] = sget($row,'website');
    
    
    $KEYS = array_keys($Param);
    $M = array();
    foreach($Param as $key => $val)
    {
        $M[] = "'%s'";
    }
    $m = implode(',', $M);
    $k = implode(',', $KEYS);
    $sql = "insert into maenna_company ($k) values($m)";
    $sql = vsprintf($sql, $Param);
    if(mysql_query($sql))
    {
        echo "OK";
    }else{
        echo mysql_error();
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