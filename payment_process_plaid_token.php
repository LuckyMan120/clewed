<?php
    require 'lib/init.php';
    include('themes/maennaco/includes/dbcon.php');

    $plaid_client_id = '5f64c507166e6d0012449b6c';
    $public_token = $_REQUEST['public_token'];
    $plaid_secret = '3132debb49806b95f75a35e1e1bdc5';
    $env = 'sandbox';
    $account_id = $_REQUEST['account_id'];
    $prouserid = $_REQUEST['prouserid'];
    $user_email = $_REQUEST['user_email'];
    $description = $_REQUEST['description'];
    $companyid = $_REQUEST['companyid'];
    $headers[] = 'Content-Type: application/json';
    $params = array(
        "client_id"     => $plaid_client_id,
        "secret"        => $plaid_secret,
        "public_token"  => $public_token,
    );
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "https://" . $env . ".plaid.com/item/public_token/exchange");
    curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($params));
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 30);
    curl_setopt($ch, CURLOPT_TIMEOUT, 80);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);

    if (!$result = curl_exec($ch)) {
        trigger_error(curl_error($ch));
    }
    curl_close($ch);

    $accessTokenJsonParsed = json_decode($result);

    /****************************

    Get stripe_bank_account_token 

    ******************************/
    $btok_params = array(
        'client_id'     => $plaid_client_id,
        'secret'        => $plaid_secret,
        'access_token'  => $accessTokenJsonParsed->access_token,
        'account_id'    => $account_id,
    );

    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "https://" . $env . ".plaid.com/processor/stripe/bank_account_token/create");
    curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($btok_params));
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 30);
    curl_setopt($ch, CURLOPT_TIMEOUT, 80);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);

    if (!$result = curl_exec($ch)) {
        trigger_error(curl_error($ch));
    }
    curl_close($ch);

    $btoken = json_decode($result);

    $stripe_bank_account_token = $btoken->stripe_bank_account_token;

    /********* Create bank accoun and verify *********/
    \Stripe\Stripe::setApiKey('sk_test_51HSqP9C2Wih1cFYGyidNE5BdaQjwwE3nKe4nrhvcCqFPAYGUZJaQtfkEFk10mQgKDUnXEySwa8CyaGr9Chquie2k00leJ7IB2Q');
    
    $customer = \Stripe\Customer::create ([
        'email' => $user_email,
        'description' => ".$description.-.$prouserid.",
        'source' => $stripe_bank_account_token,
    ]);

    $bank_account = \Stripe\Customer::retrieveSource ($customer->id,$customer->default_source);
    $bankname = $bank_account->bank_name;
    $bankcountry = $bank_account->country;
    $banknumber = $bank_account->last4;
    $bankverifyStatus = $bank_account->status;

    // verify the account
    // if($bankverifyStatus != 'verified');
    //     $bank_account->verify(['amounts' => [32, 45]]);

    /*****
        @@ Create Connected Account
        @@ Author : Monster
        @@ Created By : 2020-09-20
        $conected_account = \Stripe\Account::create([
            'country' => 'US',
            'type' => 'express',
            'email'=> $user_email,
        ]);
        $connect_id = $conected_account->id;
        $conected_account_link = \Stripe\AccountLink::create([
            'account' => $connect_id,
            'refresh_url' => 'https://dev:private@dev.clewed.com/account?tab=linkbank',
            'return_url' => 'https://dev:private@dev.clewed.com/account?tab=linkbank',
            'type' => 'account_onboarding',
        ]);
        exit ($conected_account_link->url);
    **/

    if($prouserid != null){
        $user_data_sql = "SELECT * FROM users_bank_info WHERE uid = '" . $prouserid . "'";
        $user_data_res = mysql_query($user_data_sql);
        $user_data = mysql_fetch_object($user_data_res);
        if ($user_data == null) {
            $insert_sql = "INSERT INTO users_bank_info SET uid='".$prouserid."', bankname='".$bankname."',bankcountry='".$bankcountry."',banknumber='".$banknumber."',customerid='".$customer->id."',status='1'";
            mysql_query($insert_sql);
        } else {
            if($bankname != $user_data->bankname){
                $insert_sql = "INSERT INTO users_bank_info SET uid='".$prouserid."', bankname='".$bankname."',bankcountry='".$bankcountry."',banknumber='".$banknumber."',customerid='".$customer->id."',status='1'";
                mysql_query($insert_sql);
                $update_sql = "UPDATE users_bank_info SET status='0' WHERE uid='".$prouserid."' AND bankname != '".$bankname."'";
                mysql_query($update_sql);
            } else{
                $update_sql = "UPDATE users_bank_info SET customerid='".$customer->id."' ,bankname='".$bankname."',bankcountry='".$bankcountry."',banknumber='".$banknumber."' WHERE uid='".$prouserid."'";
                mysql_query($update_sql);
            }
        }
    }
    if($companyid != null){
        $user_data_sql = "SELECT * FROM companys_bank_info WHERE cid = '" . $companyid . "'";
        $user_data_res = mysql_query($user_data_sql);
        $user_data = mysql_fetch_object($user_data_res);
        if ($user_data == null) {
            $insert_sql = "INSERT INTO companys_bank_info SET cid='".$companyid."', bankname='".$bankname."',bankcountry='".$bankcountry."',banknumber='".$banknumber."',customerid='".$customer->id."',status='1'";
            mysql_query($insert_sql);
        } else {
            if($bankname != $user_data->bankname){
                $insert_sql = "INSERT INTO companys_bank_info SET cid='".$companyid."', bankname='".$bankname."',bankcountry='".$bankcountry."',banknumber='".$banknumber."',customerid='".$customer->id."',status='1'";
                mysql_query($insert_sql);
                $update_sql = "UPDATE companys_bank_info SET status='0' WHERE cid='".$companyid."' AND bankname != '".$bankname."'";
                mysql_query($update_sql);
            } else{
                $update_sql = "UPDATE companys_bank_info SET customerid='".$customer->id."' ,bankname='".$bankname."',bankcountry='".$bankcountry."',banknumber='".$banknumber."' WHERE cid='".$companyid."'";
                mysql_query($update_sql);
            }
        }
    }
    exit("OK");
?>
