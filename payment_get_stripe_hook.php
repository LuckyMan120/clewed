<?php
    require 'lib/init.php';
    include('themes/maennaco/includes/dbcon.php');

    \Stripe\Stripe::setApiKey('sk_test_51HSqP9C2Wih1cFYGyidNE5BdaQjwwE3nKe4nrhvcCqFPAYGUZJaQtfkEFk10mQgKDUnXEySwa8CyaGr9Chquie2k00leJ7IB2Q');
    
    $payload = file_get_contents('php://input');
    $event_json = json_decode($payload);
    $event_id = $event_json->id;
    
    $event = \Stripe\Event::retrieve($event_id);

    if($event->type == 'customer.created') {
       echo("success");
    }
    if ($event->type == 'charge.succeeded') {
        $json_data = $event->data->object;
        $description =$json_data->description;
        $temp_des = explode('-',$description);
        $proj_name = $temp_des[0]; $companyid = $temp_des[1];
        $amount =$json_data->amount;
        $customer_id =$json_data->customer;
        $bank_info_sql = "UPDATE users_bank_info SET companyid='".$companyid."', status='1' where customerid='".$customer_id."'";
        mysql_query($bank_info_sql);

        $get_uid_sql = "SELECT uid from users_bank_info WHERE customerid='".$customer_id."'";
        $user_data_res = mysql_query($get_uid_sql);
        $user_data = mysql_fetch_object($user_data_res);
        $uid = $user_data->uid;
        $update_amount_sql = "UPDATE maenna_professional_investment SET amount='".$amount."',status='3' WHERE prof_id='".$uid."' and company_id='".$companyid."'";
        mysql_query($update_amount_sql);

        echo("success");
        http_response_code(200);
    }
?>