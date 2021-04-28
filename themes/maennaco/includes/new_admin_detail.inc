<?php
function admin_user_info($op = null)
{
    $Block['title'] = ("Admin User Info");
    $uid = (int)sget($_REQUEST, 'id');
    
    if($op && $op != 'update')
    {
        $first_name = $last_name = $email = $phone = $street1 = $street2 = $city = $state = $zip = '';
        if($uid > 0)
        {
            $sql = "select * from users, users_extend, users_roles where users.uid = users_extend.uid and users.uid = $uid and users.uid = users_roles.uid limit 1";
            $result = db_query($sql);
            $data = db_fetch_object($result);
            
            $redirect = rebuild_url(array('tab', 'page', 'id'));
            
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
            $role = $data->rid;
            
            $title = ucwords($first_name . ', ' . $last_name);
            $Block['title'] = check_plain($title);
        }else
        {
            $Block['title'] = ("ADD NEW ADMIN");
        }
    
        $view = sget($_REQUEST, 'view');
        if(empty($view))
        {
            $content .= "<br><b>Email: </b>" . check_plain($email);
            $content .= "<br><b>Account created on: </b>" . $created;
            if($phone) $content .= "<br><b>Phone: </b>" . $phone;
            if($street1) $content .= "<br>$street1";
            if($street2) $content .= "<br>$street2";
            if($city) $content .= "<br>$city";
            if($state) $content .= ", $state";
            if($zip) $content .= " $zip";
            $content .= "<br><br><a href='$redirect&view=edit' class='button'>Edit</a>";
        }elseif($view == 'edit' || $view == 'new')
        {
            $HV = hidden_post_values(array("tab", 'page', 'id'));
            if($view == 'new')
            {
                $redirect = rebuild_url(array('tab'));
                $HV = hidden_post_values(array('tab', 'page'));
            }
            $section = __FUNCTION__;
            $options_state = Options_state($state);
            $admin_types = Options_admin_types_($role);
            $content = <<< END
            <form method=post action='/account' onsubmit='return check_input();'>
                <table>
                     <tr>
                        <td>Admin type</td>
                        <td><select name='admin_type' class='require_select'><option></option>$admin_types</select></td>
                    </tr>
                    <tr>
                        <td style='width:130px;'>First Name</td>
                        <td><input type=text name='first_name' value='$first_name' class='require_string' /></td>
                    </tr>
                    <tr>
                        <td>Last Name</td>
                        <td><input type=text name='last_name' value='$last_name' class='require_string'  /></td>
                    </tr>
                    <tr>
                        <td>Email</td>
                        <td><input type=text name='email' value='$email' class='require_email'  /></td>
                    </tr>
                    <tr>
                        <td>Phone</td>
                        <td><input type=text name='phone' value='$phone' /></td>
                    </tr>
                    <tr>
                        <td>Street Address</td>
                        <td><input type=text name='street1' value='$street1' /></td>
                    </tr>
                    <tr>
                        <td>Apt/Suite</td>
                        <td><input type=text name='street2' value='$street2' /></td>
                    </tr>
                    <tr>
                        <td>City</td>
                        <td><input type=text name='city' value='$city' /></td>
                    </tr>
                    <tr>
                        <td>State</td>
                        <td><select name='state'><option></option>$options_state</select></td>
                    </tr>
                    <tr>
                        <td>Zip</td>
                        <td><input type=text name='zip' value='$zip' /></td>
                    </tr>
                    <tr>
                        <td>New Password</td>
                        <td><input type=password name='password1' value='' autocomplete=off id='password1' /></td>
                    </tr>
                    <tr>
                        <td>Confirm Password</td>
                        <td><input type=password name='password2' value='' autocomplete=off  id='password2' /></td>
                    </tr>
                </table>
                $HV
                <input type=hidden name=update_section value='$section' />
                <input type=hidden name=view value='$view' />
                <input type=submit name=submit value='submit' class=button />  &nbsp;&nbsp;<a href='$redirect' class='button'>cancel</a>
            </form>
END;
        }
        
        
    }elseif($op == 'update')
    {
        $firstName = sget($_POST, 'first_name');
        $lastName = sget($_POST, 'last_name');
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
        $admin_type = sget($_REQUEST,'admin_type');
        
        $view = sget($_REQUEST, 'view');
        if($view == 'edit')
        {
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
            
            
            $sql = "select count(*) as total from users where mail = '%s' and uid != $uid and status = 1";
            $sql_data = array($email);
            
            $result = db_query($sql, $sql_data);
            $row = db_fetch_object($result);
            if($row->total >0 )
            {
                drupal_set_message('The email address, '."$email".', is already used by other member', 'error');
                return;
            }
            
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
        }elseif($view == 'new')
        {            
            if(empty($firstName ) || empty($lastName) || empty($email))
            {
                drupal_set_message('Please complete new user form and try again', 'error');
                return;
            }
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
            
            $pass = md5($password1);
            
            $sql = "select count(*) as total from users where mail = '%s' and status = 1";
            $sql_data = array($email);
            
            $result = db_query($sql, $sql_data);
            $row = db_fetch_object($result);
            if($row->total >0 )
            {
                drupal_set_message('The email address is already used by another member', 'error');
                return;
            }

            if ($admin_type == 6) $roles = array( 6 => 'Maennaco admin' );
            elseif ($admin_type == 15) $roles = array( 15 => 'Sales' );
            
            $New_user = array(
                'name'              => "Temp" . date('s'),
                'mail'             => $email,
                'init'              => $email,
                'roles'             => $roles,
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
            
            if ( db_query($SQL, $Values) == false)
            {
                drupal_set_message( t("There was an error when updateing user database. Operation aborted."));
       
                if($uid){
                    $SQL = "delete from users where uid = $uid limit 1";
                    db_query($SQL);
                }
                
            }else{
                //$SQL = "insert into maenna_people (uid,  first_name, last_name, street1, street2, city, state, zip,phone) values(%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')";
                //db_query($SQL, $Values);
                drupal_set_message( t("You have created a new admin user account"));
                $Param = array('to'=>$email, 'from'=>'info@clewed.com', 'firstname'=>$firstName,'lastname'=>$lastName, 'pass'=>$pass);
                notify_user('new_user_account', $Param);
            }
        }
        return;
        
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
/* EOF */