<?php
    require 'lib/init.php';
    include('themes/maennaco/includes/dbcon.php');
    $bank_name = trim($_POST['bank_name']);
    $userid = $_POST['userid'];
    $bank_name_arr = explode('••••',$bank_name);
    $bank_name = trim($bank_name_arr[0]);
    $bank_number = trim($bank_name_arr[1]);
    $bank_info_sql = "UPDATE users_bank_info SET status='1' where bankname='".$bank_name."' and banknumber='".$bank_number."' and uid = '".$userid."' ";
    mysql_query($bank_info_sql);
    $bank_info_sql = "UPDATE users_bank_info SET status='0' where bankname!='".$bank_name."' and uid = '".$userid."' ";
    mysql_query($bank_info_sql);
    $get_customer_id = "SELECT * FROM users_bank_info WHERE uid = '" . $userid . "' and status = 1";
    $customer_res = mysql_query($get_customer_id);
    $coutomer_data = mysql_fetch_object($customer_res);
    exit($coutomer_data->customerid);
?>