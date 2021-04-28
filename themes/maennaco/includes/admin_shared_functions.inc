<?php
function admin_user_info($op = null)
{
    $Block['title'] = ("Admin User Info");
    $uid = (int)sget($_REQUEST, 'id');
    if($uid <= 0){
        $Block['body'] = content_box($Block['title'], "");
        return $Block;
    }
    $sql = "select * from users, users_extend where users.uid = users_extend.uid and users.uid = %d limit 1";
    $result = db_query($sql, array($uid));
    $data = db_fetch_object($result);
    
    
    $email = $data->mail;
    $created = ($data->created) ? date("m/d/Y",$data->created) : "n/a";
    $first_name = $data->first_name;
    $last_name = $data->last_name;
    $phone = $data->phone;
    $street1 = $data->street1;
    $street2 = $data->street2;
    $city = $data->city;
    $state = $data->state;
    $zip  = $data->zip;
    
    $title = ucwords($first_name . ', ' . $last_name);
    $Block['title'] = check_plain($title);
    
    if($op == 'view' || $op == null)
    {
     
        $content .= "<br><b>Email: </b>" . check_plain($email);
        $content .= "<br><b>Account created on: </b>" . $created;
        if($street1) $content .= "<br>$street1";
        if($street2) $content .= "<br>$street2";
        if($city) $content .= "<br>$city";
        if($state) $content .= ", $state";
        if($zip) $content .= " $zip";
        //$content .= "<div style='text-align:right'><a href='#' class='edit_wnd' rel='edit_company_description'>edit company description</a></div>";
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function admin_update_info($op = null)
{
    $Block['title'] =  'Update Admin User Info'; 
    if($op == 'view' || $op == '') {
        $uid = sget($_REQUEST,'id');
        if($uid)$Block['body']  =  "<div class='pagelink'><a href='#' class='edit_wnd' rel='admin_update_info' data='$uid'>Update Admin User Info</a></div>";
    }elseif($op == 'update'){
        $firstName = sget($_POST, 'fname');
        $lastName = sget($_POST, 'lname');
        $email = sget($_POST, 'email');
        $phone = sget($_POST, 'phone');
        $street1 = sget($_POST, 'street1');
        $street2 = sget($_POST, 'street2');
        $city = sget($_POST, 'city');
        $state = sget($_POST, 'state');
        $zip = sget($_POST, 'zip');
        $password1 = sget($_REQUEST,'password1');
        $password2 = sget($_REQUEST,'password2');
        $uid = sget($_REQUEST,'id');
        
        if(empty($uid))
        {
            drupal_set_message('invalid admin user id', 'error');
            return;
        }
        if(empty($firstName ) || empty($lastName) || empty($email))
        {
            drupal_set_message('Please complete new user form and try again', 'error');
            return;
        }
        $m = $password1 . $password2;
        if(! empty($m))
        {
            if(empty($password1) || empty($password2))
            {
                drupal_set_message('Please confirm the new password', 'error');
                return;
            }
            if(strlen($password1) < 8)
            {
                drupal_set_message('The new password must consist of minimum eight characters', 'error');
                return;
            }
            if($password1 != $password2){
                drupal_set_message("The passwords don't match", 'error');
                return;
            }
        }
        
        
        $sql = "select count(*) as total from users where mail = '%s' and uid != %d";
        $sql_data = array($email, $uid);
        
        $result = db_query($sql, $sql_data);
        $row = db_fetch_object($result);
        if($row->total >0 )
        {
            drupal_set_message('The email address, '."$email".', is already used by other member', 'error');
            return;
        }
        
        /*
        $New_user = array(
            'name'              => "Temp" . date('s'),
            'mail'             => $email,
            'init'              => $email,
            'roles'             => array( 6 => 'Maennaco admin' ),
            'pass'              => $pass,
            'status'            => 1
        );
        $userObj = user_save(null, $New_user);
        $SQL = 'update {users} set name = "%s" where uid = %d';
        db_query($SQL, "Admin_" . $userObj->uid, $userObj->uid);
        $m = db_affected_rows();
        if($m != 1) alert_admin("new admin user db query error");
        $uid = $userObj->uid;
        $SQL = "insert into {users_extend} (uid,  first_name, last_name, street1, street2, city, state, zip,phone) values(%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')";

        $Values = array($uid, $firstName, $lastName, $street1, $street2, $city, $state, $zip, $phone);
        */
        if(! empty($password1))
        {
            $password = md5($password1);
            $sql = "update users set mail = '%s', init = '%s', pass = '%s' where uid = %d limit 1";
            $sql_data = array($email, $email, $password, $uid);
        }else{
            $sql = "update users set mail = '%s', init = '%s' where uid = %d limit 1";
            $sql_data = array($email, $email, $uid);
        }
        if(db_query($sql, $sql_data) === false){
            drupal_set_message('Failed to update user email', 'error');
            return;
        }
        
        
        $user_extend_keys = array('first_name' => '',
                                  'last_name' => '',
                                  'phone' => '',
                                  'street1' => '',
                                  'street2' => '',
                                  'city' => '',
                                  'state' => '',
                                  'zip' => '');
        foreach($user_extend_keys as $key=>$val)
        {
            $user_extend_keys["$key"] = "$key = '%s'";
        }
        $sql = "update users_extend set " . implode(',', $user_extend_keys) ." where uid = %d limit 1";
        $sql_data = array($firstName, $lastName, $phone, $street1, $street2, $city, $state, $zip, $uid);

        if ( db_query($sql, $sql_data) == false)
        {

            drupal_set_message( t("There was an error when updating user database. Operation aborted."), 'error');
        }else{
            drupal_set_message( t("You have updated admin user account info"));
            $Param = array('to'=>$email, 'from'=>'info@clewed.com', 'firstname'=>$firstName,'lastname'=>$lastName, 'pass'=>$password1);
            notify_user('new_user_account', $Param);
        }
        
        
    }
    return $Block;
}
/* EOF */