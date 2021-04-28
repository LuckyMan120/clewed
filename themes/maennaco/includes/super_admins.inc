<?php

// center 
function admin_list($op = null)
{
    $Block['title'] = 'Admin Users';
    if($op == 'view' || $op == null){
        $content = '';
        
        
        
        $content .= admin_search();
        
        
        
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function admin_search()
{
    $Search_parameter = array();
    $Selection = array();
    $Sectors = _sectors();
    $PRoles = _pro_roles();
    $States = get_us_states();
    $Search_parameter = array(
                                  
    );
    $Search_result  = get_search_result('admin');
    $paginationBaseURL = '/account?a=admins&';
    $block = render_people_search($Search_parameter, $Search_result, $paginationBaseURL);
    return $block;
}
function remove_admins($op = null)
{
    if(isset($_POST['uids']) && (count($_POST['uids'] > 0)))
    {
        $sql_where = implode(',', $_POST['uids']);
        $SQL = "update users set status = 0 where uid in ($sql_where)";
        if(db_query($SQL) !== false)
        {
          drupal_set_message( t("Admin user records updated."));
          
        }
        else{
           drupal_set_message( t("Failed to update user record."),'error');
        }
    }
}
// right side bar
function add_admin_user($op = null)
{
    $Block['title'] =  'Add Admin User'; 
    if($op == 'view' || $op == '') {
        $Block['body']  =  "<div class='pagelink'><a href='#' class='edit_wnd' rel='add_admin_user'>Add Admin User</a></div>";
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
        $pass = createRandomPassword();
        
        if(empty($firstName ) || empty($lastName) || empty($email))
        {
            drupal_set_message('Please complete new user form and try again', 'error');
            return;
        }
        
        $sql = "select count(*) as total from users where mail = '%s' and status = 1";
        $sql_data = array($email);
        
        $result = db_query($sql, $sql_data);
        $row = db_fetch_object($result);
        if($row->total >0 )
        {
            drupal_set_message('The email address is already used by other member', 'error');
            return;
        }
        
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
        
        if ( db_query($SQL, $Values) == false)
        {
            drupal_set_message( t("There was an error when updateing user database. Operation aborted."));
   
            if($uid){
                $SQL = "delete from users where uid = %d limit 1";
                db_query($SQL, array($uid));
            }
            
        }else{
            $SQL = "insert into maenna_people (uid,  first_name, last_name, street1, street2, city, state, zip,phone) values(%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')";
            db_query($SQL, $Values);
            drupal_set_message( t("You have created a new admin user account"));
            $Param = array('to'=>$email, 'from'=>'info@clewed.com', 'firstname'=>$firstName,'lastname'=>$lastName, 'pass'=>$pass);
            notify_user('new_user_account', $Param);
        }
        
        
    }
    return $Block;
}


/* EOF */