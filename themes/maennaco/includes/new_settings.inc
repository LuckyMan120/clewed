<?php
function settings_instruction($op = null) {
    $Block['title'] = "SETTINGS";
    $content = 'Please select an option';
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function account($op = null) {
    require_once("." . base_path() . path_to_theme() . "/includes/classes/maenna_users.inc");
    global $AccessObj;
    $uid = $AccessObj->uid;
    $time = time();
    $Block['title'] = "ACCOUNT SETTINGS";
    $redirect = rebuild_url(array('tab', 'page'));
    $content = '';
    $section = __FUNCTION__;
    if ($op && $op != 'update') {
        $view = sget($_REQUEST, 'view');
        if (in_array($AccessObj->user_type, array('super', 'admin'))) {
            $sql = "select  users.mail,
                                concat_ws(' ', first_name, last_name) as name,
                                phone,
                                notifications,
                                notif_industries,
                                notif_categories,
                                discussion_rupdated,
                                discussion_fupdated,
                                professional_discussion,
                                company_update,
                                company_comment
                        from    users,
                                users_extend
                        where   users.uid = users_extend.uid and
                                users.uid = %d";
        } elseif ($AccessObj->user_type == 'company') {
            $sql = "select  users.mail,
                                contact as name,
                                phone,
                                notifications,
                                notif_industries,
                                notif_categories,
                                discussion_rupdated,
                                discussion_fupdated,
                                professional_discussion,
                                company_update,
                                company_comment
                        from    users,
                                maenna_company
                        where   users.uid = maenna_company.companyid and
                                users.uid = %d";
        } elseif ($AccessObj->user_type == 'people') {
            $sql = "select  users.mail,
                                concat_ws(' ', real_first_name, real_last_name) as name,
                                phone,
                                notifications,
                                notif_industries,
                                notif_categories,
                                discussion_rupdated,
                                discussion_fupdated,
                                professional_discussion,
                                company_update,
                                company_comment,
                                cmp_member_email
                        from    users,
                                maenna_people
                        where   users.uid = maenna_people.pid and
                                users.uid = %d";
        } else {
            return '';
        }
        $result = db_query($sql, $uid);
        $Row = db_fetch_array($result);
        $notifications_status = $Row['notifications'];
        $selected_categories = explode('|', $Row['notif_categories']);
        $selected_industries = explode('|', $Row['notif_industries']);
        $discussion_rupdated = ($Row['discussion_rupdated']) ? ' checked=checked ' : '';
        $discussion_fupdated = ($Row['discussion_fupdated']) ? ' checked=checked ' : '';
        $professional_discussion = ($Row['professional_discussion']) ? ' checked=checked ' : '';
        $company_update = ($Row['company_update']) ? ' checked=checked ' : '';
        $company_comment = ($Row['company_comment']) ? ' checked=checked ' : '';
        $notif_messages = array(
            'discussion_rupdated'     => 'A discussion I registered for is updated',
            'discussion_fupdated'     => 'A discussion I am following is updated',
            'professional_discussion' => 'A professional I am following holds a discussion',
            'company_update'          => 'When pages of companies I am advising update',
            'company_comment'         => 'When a company invites me to comment'
        );
        if (empty($view)) {
            if ($Row) {
                $email = sget($Row, 'mail');
                $name = ucwords(sget($Row, 'name'));
                $phone = sget($Row, 'phone');
                $cmpemail = sget($Row, 'cmp_member_email');
                $dailyNotifs = '';
                foreach ($notif_messages as $id => $message) {
                    if ($Row[$id]) $dailyNotifs .= $message . "<br />";
                }
                $content = '<div class="view-settings">';
                $content .= '<div class="section-title">Account Settings</div>';
                $content .= '<div id="edit-settings"><a style="border-left:solid 1px #006274; padding-left:2px;" href="/account?tab=settings&section=' . $section . '&panel=account&view=edit">Edit</a></div>';
                $content .= '<div class="setting-label">Account Owner:</div>';
                $content .= "<div class='setting-content'>$name</div>";
                $content .= '<div class="setting-label">Primary Email (for login):</div>';
                $content .= "<div class='setting-content'>$email</div>";
                $content .= '<div class="setting-label">Password:</div>';
                $content .= "<div class='setting-content'>*******</div>";
                $content .= '<div class="setting-label">Phone:</div>';
                $content .= "<div class='setting-content'>$phone</div>";
                if ($AccessObj->user_type == 'people') {
                    $content .= '<div class="setting-label">Secondary Email (work verification):</div>';
                    $content .= "<div class='setting-content'>$cmpemail</div>";
                }
                if ($AccessObj->user_type == 'company') {
                    $content .= '<div class="section-title">Notification Settings</div>';
                    $content .= '<div class="setting-label">Notify me about new insights and services:</div>';
                    $content .= "<div class='setting-content no-margin multi'>" . ($Row['discussion_rupdated'] ? 'Weekly' : 'Not at all') . "</div>";
                }
                elseif ($AccessObj->user_type == 'people') {
                    $content .= '<div class="section-title">Notification Settings</div>';
                    $content .= '<div class="setting-label">Notify me about new insights and services:</div>';
                    $content .= "<div class='setting-content no-margin multi'>" . ($Row['discussion_rupdated'] ? 'Weekly' : 'Not at all') . "</div>";
                    $content .= '<div class="setting-label">Notify me about new companies or projects:</div>';
                    $content .= "<div class='setting-content no-margin multi'>" . ($Row['company_update'] ? 'Weekly' : 'Not at all') . "</div>";
                }
                $content .= '</div><!-- view-settings -->';
            }
        } elseif ($view == 'edit') {
            $Block['title'] = "EDIT - ACCOUNT SETTINS";
            if ($Row) {
                $email = sget($Row, 'mail');
                $name = ucwords(sget($Row, 'name'));
                $N = explode(' ', $name);
                $N = array_values(array_filter($N));
                $firstname = $name;
                $lastname = '';
                if (count($N) == 2) {
                    $firstname = trim($N[0]);
                    $lastname = trim($N[1]);
                }
                $phone = sget($Row, 'phone');
                $cmpemail = sget($Row, 'cmp_member_email');
                $HV = hidden_post_values(array('tab', 'panel', 'section'));
                $notifyes = ($notifications_status) ? 'checked=checked' : '';
                $notifno = !($notifications_status) ? 'checked=checked' : '';
                $industries = array_keys(_INDUSTRY());
                array_shift($industries);
                foreach ($industries as $industry) {
                    $indLabel = str_replace(' ', '', $industry);
                    $indList .= '<div class="inputblock"><input type="checkbox" name="industries[]" id="' . $indLabel . '" value="' . $industry . '" ' . (in_array($industry, $selected_industries) ? 'checked="checked"' : '') . '/> <label for="' . $indLabel . '">' . $industry . "</label></div>";
                }
                $categories = _categories();
                foreach ($categories as $category) {
                    $catLabel = str_replace(' ', '', $category);
                    $catList .= '<div class="inputblock"><input type="checkbox" name="categories[]" id="' . $catLabel . '" value="' . $category . '"' . (in_array($category, $selected_categories) ? 'checked="checked"' : '') . '/> <label for="' . $catLabel . '">' . $category . "</label></div>";
                }
                $content = '<div class="edit-settings">';
                $content .= "<form method=post action='/account' onsubmit='return check_input();'>";
                $content .= '<div class="section-title">Account Settings</div>';
                $content .= '<div class="setting-label">First name:</div>';
                $content .= "<div class='setting-content'><input type=text name=firstname value='$firstname' class='require_string' /><span class=warning>*</span></div>";
                $content .= '<div class="setting-label">Last name:</div>';
                $content .= "<div class='setting-content'><input type=text name=lastname value='$lastname'  class='require_string' ><span class=warning>*</span></div>";
                $content .= '<div class="setting-label">Primary Email (for login):</div>';
                $content .= "<div class='setting-content'><input id='edit-email' type=text name=email value='$email' style='width:270px;'  class='require_email' ><span class=warning>*</span></div>";
                $content .= '<div class="setting-label">Password:</div>';
                $content .= "<div style='height:80px;' class='setting-content multi'>
                                <input class='settings-password' type='password' name='password' value=''  autocomplete='off' />
                                <span class='note'>(new password)</span>
                                <br>
                                <input class='settings-password-match' type='password' name='password1' value=''  autocomplete='off' />
                                <span class='note'>(re-type new password)</span><br>
                                <span class='note'>Password must be at least 8 characters,containg at least one number,<br> and no special characters.</span>

                            </div>";
                $content .= '<div class="setting-label">Phone:</div>';
                $content .= "<div class='setting-content'><input type=text name=phone value='$phone' /></div>";

                if ($AccessObj->user_type == 'people') {
                    $content .= '<div class="setting-label">Secondary Email (work verification):</div>';
                    $content .= "<div class='setting-content'><input id='edit-cmp-member-email' type=text name=cmp_member_email value='$cmpemail' /></div>";
                }

                if ($AccessObj->user_type == 'company') {
                    $content .= '<div class="section-title">Notification Settings</div>';
                    $content .= '<div class="setting-label">Notify me about new insights and services:</div>';

                    $on = $Row['discussion_rupdated'] ? ' checked=checked ' : '';
                    $off = !$Row['discussion_rupdated'] ? ' checked=checked ' : '';

                    $content .= "<div class='setting-content no-margin multi semifull gray'>
                                    <div class='inputblock yes'>
                                        <input id='notifson' type='radio' name='discussion_rupdated' value='on' $on />
                                        <label for='notifson'>Weekly</label>
                                    </div>
                                    <div class='inputblock no'>
                                        <input id='notifsoff' type='radio' name='discussion_rupdated' value='' $off />
                                        <label for='notifsoff'>Not at all</label>
                                    </div>
                                </div>";
                }
                elseif ($AccessObj->user_type == 'people') {
                    $content .= '<div class="section-title">Notification Settings</div>';
                    $content .= '<div class="setting-label">Notify me about new insights and services:</div>';

                    $on = $Row['discussion_rupdated'] ? ' checked=checked ' : '';
                    $off = !$Row['discussion_rupdated'] ? ' checked=checked ' : '';

                    $content .= "<div class='setting-content no-margin multi semifull gray'>
                                    <div class='inputblock yes'>
                                        <input id='notifson' type='radio' name='discussion_rupdated' value='on' $on />
                                        <label for='notifson'>Weekly</label>
                                    </div>
                                    <div class='inputblock no'>
                                        <input id='notifsoff' type='radio' name='discussion_rupdated' value='' $off />
                                        <label for='notifsoff'>Not at all</label>
                                    </div>
                                </div>";

                    $content .= '<div class="setting-label">Notify me about new companies or projects:</div>';

                    $on = $Row['company_update'] ? ' checked=checked ' : '';
                    $off = !$Row['company_update'] ? ' checked=checked ' : '';

                    $content .= "<div class='setting-content no-margin multi semifull gray'>
                                    <div class='inputblock yes'>
                                        <input id='notifson' type='radio' name='company_update' value='on' $on />
                                        <label for='notifson'>Weekly</label>
                                    </div>
                                    <div class='inputblock no'>
                                        <input id='notifsoff' type='radio' name='company_update' value='' $off />
                                        <label for='notifsoff'>Not at all</label>
                                    </div>
                                </div>";
                }
                $content .= '<div class="setting-label bottomcontent">
                                <input type=submit name=submit value=Submit class=button onClick="return check_input();"/>
                                <input type="button" class="button" style="background:#2c4658 !important;" name="cancel" id="cancel" value="Cancel" onclick="javascript:window.location.href = \'/account?tab=settings&panel=account\'; return false;" class="tool" />
                            </div>';
                $content .= '<input type=hidden name="tab" value="settings" />
                             <input type=hidden name="panel" value="account" />
                             <input type=hidden name="section" value="account" />
                             <input type=hidden name="update_section" value="account" />
                            </form></div><!-- view-settings -->';
            }
        }
    } elseif ($op == 'update') {
        $firstname = sget($_REQUEST, 'firstname');
        $lastname = sget($_REQUEST, 'lastname');
        $password = sget($_REQUEST, 'password');
        $password1 = sget($_REQUEST, 'password1');
        $email = trim(sget($_REQUEST, 'email'));
        $phone = sget($_REQUEST, 'phone');
        $cmpemail = trim(sget($_REQUEST, 'cmp_member_email'));
        $notifications = (sget($_REQUEST, 'notifications') == 'on' ? 1 : 0);
        $industries = $_REQUEST['industries'];
        $categories = $_REQUEST['categories'];
        $discussion_rupdated = (sget($_REQUEST, 'discussion_rupdated') == 'on' ? 1 : 0);
        $discussion_fupdated = (sget($_REQUEST, 'discussion_fupdated') == 'on' ? 1 : 0);
        $professional_discussion = (sget($_REQUEST, 'professional_discussion') == 'on' ? 1 : 0);
        $company_update = (sget($_REQUEST, 'company_update') == 'on' ? 1 : 0);
        $company_comment = (sget($_REQUEST, 'company_comment') == 'on' ? 1 : 0);
        if (empty($firstname) || empty($lastname) || empty($email)) {
            drupal_set_message("Please complete the form and try again", 'error');
        } else {
            if (((!empty($password)) || (!empty($passowrd1))) && $password !== $password1) {
                drupal_set_message("Passwords don't match. Please try again", 'error');
            } else {
                $Param = array(
                    'first_name' => $firstname,
                    'last_name'  => $lastname,
                    'password'   => $password,
                    'email'      => $email,
                    'phone'      => $phone,
                    'cmp_member_email'      => $cmpemail
                );

                $isEmailDuplicated = false;
                $isEmailValid = !empty($email) && false !== filter_var($email, FILTER_VALIDATE_EMAIL);
                if($isEmailValid) {

                    //check for email duplicate
                    $_sql =  "SELECT uid FROM users WHERE mail = '%s' AND uid <> '%s' LIMIT 1";
                    $resource = db_query($_sql, array($email, $uid));
                    if(!empty($resource)) {
                        $data = mysql_fetch_array($resource);
                        if(!empty($data['uid']))
                            $isEmailDuplicated = true;
                    }
                }

                if(!$isEmailValid || $isEmailDuplicated) {
                    $_sql =  "SELECT mail FROM users WHERE uid = '%s' LIMIT 1";
                    $resource = db_query($_sql, array($uid));
                    if(!empty($resource)) {
                        $data = mysql_fetch_array($resource);
                        $Param['email'] = $data['mail'];
                    }
                }



                $return = Mae_users::update_account($uid, $AccessObj->user_type, $Param);
                if ($return) {

                    $hash = null;
                    $emailError = false;
                    $secondaryEmailError = false;
                    if(!$isEmailValid) {
                        $emailError = true;
                        drupal_set_message("Invalid email", 'error');
                    }
                    elseif($isEmailDuplicated) {
                        $emailError = true;
                        drupal_set_message(sprintf("User with email %s already exists", $email), 'error');
                    }
                    Mae_users::update_notifications($uid, $notifications, $industries, $categories, $discussion_rupdated, $discussion_fupdated, $professional_discussion, $company_update, $company_comment);

                    if(!$emailError && !$secondaryEmailError)
                        drupal_set_message("Your account settings is updated");

                    if($emailError && $secondaryEmailError)
                        $hash = 'invalid-emails';
                    elseif($emailError)
                        $hash = 'invalid-email';
                    elseif($secondaryEmailError)
                        $hash = 'invalid-secondary-email';

                    if($emailError || $secondaryEmailError)
                        drupal_goto('account', 'tab=settings&section=account&panel=account&view=edit', $hash);

                } else {
                    drupal_set_message("Failed to update account settings.", 'error');
                }
            }
        }
    }
    $Block['body'] = content_box(null, $content);
    return $Block;
}

function ccom_permissions($op = null) {
    require_once("." . base_path() . path_to_theme() . "/includes/classes/maenna_users.inc");
    global $AccessObj, $Maenna_page;
    $editorid = $AccessObj->uid;
    $time = time();
    $Block['title'] = "PERMISSION SETTINGS";
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $content = '';
    $section = __FUNCTION__;
    $type = sget($_REQUEST, 'type');
    $P = array();
    $accessid = '';
    $companyid = sget($_REQUEST, 'id');
    // prof users have no access
    if (!in_array($AccessObj->user_type, array('super', 'admin', 'company'))) {
        $Block['body'] = content_box($Block['title'], $content);
        return $Block;
    }
    $Sections = $Maenna_page->mae_com_sections();
    if ($op && $op != 'update') {
        if ($AccessObj->user_type == 'admin' || $AccessObj->user_type == 'company') {
            $companyid = sget($_REQUEST, 'id');
            if (empty($companyid)) {
                drupal_set_message("Access Error - 03", 'error');
            } else {
                $sql = "select * from maenna_access where editor_utype = 'super'";
                $result = db_query($sql);
                $P = array();
                while ($Row = db_fetch_object($result)) {
                    $Temp = unserialize($Row->settings);
                    $P = array_merge_recursive($P, $Temp);
                }
                $sql = "select * from maenna_access where companyid = '%s' ";
                $result = db_query($sql, array($companyid));
                $Row = db_fetch_object($result);
                if ($Row) {
                    $accessid = $Row->accessid;
                    $Temp = unserialize($Row->settings);
                    $P = MergeArrays($P, $Temp);
                }
                $User_group = Mae_users::com_group();
                $Rights = array('hide' => 'no', 'read' => 'yes');
                unset($Sections['analysis']);
                $Table = com_perm_table($AccessObj->user_type, $User_group, $Sections, $Rights, $P);
            }
        } else return '';
        $HV = hidden_post_values(array('tab', 'section', 'panel', 'type', 'id'));
        $content = <<< END
        <form method=post action='/account'>

            $Table
            $HV
            <input type=checkbox name=revert value='1'> Remove custom settings and restore default <br />
            <input type=hidden name=accessid value='$accessid' />
            <input type=hidden name=update_section value='$section' />
            <input type=submit name=submit value=submit class=button />
        </form>
END;
    } elseif ($op == 'update') {
        $revert = sget($_REQUEST, 'revert');
        $Permissions = $_POST['perm'];
        $DBKeys = array(
            'accessid',
            'usergroup',
            'settings',
            'editorid',
            'editor_utype',
            'companyid',
            'access'
        );
        $companyid = sget($_REQUEST, 'id');
        $Param = array(
            'accessid'     => sget($_REQUEST, 'accessid'),
            'usergroup'    => 'com',
            'settings'     => serialize($Permissions),
            'editorid'     => $AccessObj->uid,
            'editor_utype' => $AccessObj->user_type,
            'companyid'    => $companyid,
            'access'       => time()
        );
        $return = '';
        if ($AccessObj->user_type == 'company') {
            if ($AccessObj->uid != $companyid) {
                drupal_set_message("Access Error - 01", 'error');
            } else {
                if ($revert) {
                    $return = $AccessObj->remove_access($companyid);
                } else {
                    $return = $AccessObj->update_access($Param);
                }
            }
        } elseif ($AccessObj->user_type == 'admin') {
            if (empty($companyid)) {
                drupal_set_message('Invalid company id', 'error');
            } else {
                if ($AccessObj->admin_of($companyid)) {
                    if ($revert) {
                        $return = $AccessObj->remove_access($companyid);
                    } else {
                        $return = $AccessObj->update_access($Param);
                    }
                } else {
                    drupal_set_message("Access Error - 02", 'error');
                }
            }
        }
        if ($return) {
            drupal_set_message("Settings updated");
        } else drupal_set_message("Failed to update settings", 'error');
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function com_perm_table($user_type, $Groups, $Sections, $Rights, $Saved_settings) {
    $Table = "<table cellspacing=0 cellpadding=0 class='table-list'>";
    $Table .= "<tr><td style='width:200px;'><!-- section column --></td>";
    foreach ($Groups as $group_key => $group_title) {
        $title = strtoupper($group_title);
        $Table .= "<td  align=center><b>$title</b></td>";
    }
    $Table .= "</tr>";
    //test_array($Sections);
    foreach ($Sections as $Section) {
        $title = strtoupper(sget($Section, 'title'));
        $maenna_name = sget($Section, 'maenna_name');
        if (in_array($maenna_name, array('questionnaire')) || empty($maenna_name)) continue;
        $id = sget($Section, 'id');
        $Table .= "\n<tr style='background:#f3f3f3;'><td><b>$title</b></td>";
        foreach ($Groups as $group_key => $group_title) {
            $Table .= "\n<td align=center>&nbsp;</td>";
        }
        $Table .= "</tr>";
        foreach ($Section['sections'] as $S) {
            $cu_edit = sget($S, 'cu_edit');
            if (empty($cu_edit)) continue;
            $CU_EDIT = explode(',', $cu_edit);
            foreach ($CU_EDIT as &$D) {
                $D = trim($D);
            }
            $id = sget($S, 'id');
            $title = ucwords(strtolower(sget($S, 'title')));
            $Table .= "\n<tr><td style='text-align:left;padding-left:10px;'>- $title</td>";
            foreach ($Groups as $group_key => $group_title) {
                if (!in_array($group_key, $CU_EDIT)) {
                    $Table .= "\n<td align=center>&nbsp;</td>";
                    continue;
                }
                $setting = 'hide';
                if (isset($Saved_settings["$group_key"]) && isset($Saved_settings["$group_key"]["$id"])) {
                    $setting = $Saved_settings["$group_key"]["$id"];
                }
                $tag_name = "perm[$group_key][$id]";
                $otions_rights = Options_rights($Rights, $setting, $tag_name);
                $Table .= "\n<td align=center>$otions_rights</td>";
            }
            $Table .= "</tr>";
        }
    }
    $Table .= "</table>";
    return $Table;
}

function com_permissions($op = null) {
    require_once("." . base_path() . path_to_theme() . "/includes/classes/maenna_users.inc");
    global $AccessObj, $Maenna_page;
    $editorid = $AccessObj->uid;
    $time = time();
    $Block['title'] = "PERMISSION SETTINGS";
    $redirect = rebuild_url(array('tab', 'page'));
    $content = '';
    $section = __FUNCTION__;
    $type = sget($_REQUEST, 'type');
    $P = array();
    $accessid = '';
    // prof users have no access
    if (!in_array($AccessObj->user_type, array('super', 'admin', 'company'))) {
        $Block['body'] = content_box($Block['title'], $content);
        return $Block;
    }
    $ptab = $_REQUEST['ptab'];
    $Sections = $Maenna_page->mae_com_sections();
    if ($op && $op != 'update') {
        if ($AccessObj->user_type == 'super') {
            $tab1_active = $tab2_active = '';
            if (empty($ptab) || $ptab == 'team') {
                $tab1_active = 'active';
                $ptab = 'team';
            } else $tab2_active = 'active';
            if (empty($type)) $type = 'cu';
            if ($type == 'cu') {
                $User_group = Mae_users::cu_group();
            } elseif ($type == 'pu') {
                $User_group = Mae_users::pu_group($ptab);
            } elseif ($type == 'au') {
                $User_group = Mae_users::au_group();
            } else {
                return '';
            }
            //Changing user type becaouse of the two tabs in permission table.Second tab uses type pu1 as professional user 1st level
            if ($ptab == 'coll') $type = 'pu1';
            $sql = "select * from maenna_access where usergroup = '%s' and editor_utype ='super'";
            $result = db_query($sql, array($type, $AccessObj->user_type));
            //echo $sql; test_array(array($type, $AccessObj->user_type));
            $Row = db_fetch_object($result);
            if ($Row) {
                $P = unserialize($Row->settings);
                $accessid = $Row->accessid;
            }

            $Rights = array('hide' => 'no access', 'read' => 'read', 'write' => 'write');
            $Table = perm_table($AccessObj->user_type, $User_group, $Sections, $Rights, $P);
        } elseif ($AccessObj->user_type == 'admin' || $AccessObj->user_type == 'company') {
            $companyid = sget($_REQUEST, 'id');
            if (empty($companyid)) {
                drupal_set_message("Access Error - 03", 'error');
            } else {
                $sql = "select * from maenna_access where companyid = '%s' ";
                $result = db_query($sql, array($companyid));
                $Row = db_fetch_object($result);
                if ($Row) {
                    $accessid = $Row->accessid;
                    $P = unserialize($Row->settings);
                }
                $User_group = Mae_users::com_group();
                $Rights = array('hide' => 'hidden', 'read' => 'visible');
                $Table = perm_table($AccessObj->user_type, $User_group, $Sections, $Rights, $P);
            }
        } else return '';
        //test_array($User_group);
        //test_array($Sections);
        $HV = hidden_post_values(array('tab', 'section', 'panel', 'type', 'id'));
        if ($type == 'pu' || $type == 'pu1') {
            $content .= "<form method=post action='/account'>
                <div class=\"searchtab\">
                <a href=\"$redirect&section=com_permissions&panel=com_permissions&type=pu&ptab=team\" class=\"info_tab_btn $tab1_active\" >FIRST TAB</a>&nbsp;&nbsp;&nbsp;
                <a href=\"$redirect&section=com_permissions&panel=com_permissions&type=pu&ptab=coll\" class=\"info_tab_btn $tab2_active\" >SECOND TAB</a>&nbsp;&nbsp;&nbsp;
        </div>";
        } else $content .= "<form method=post action='/account'>";
        $content .= <<< END

            $Table
            $HV
            <input type=hidden name=ptab value='$ptab' class=button />
            <input type=hidden name=accessid value='$accessid' />
            <input type=hidden name=update_section value='$section' />
            <input type=submit name=submit value=submit class=button />

        </form>
END;
    } elseif ($op == 'update') {
        $Permissions = $_POST['perm'];
        $DBKeys = array(
            'accessid',
            'usergroup',
            'settings',
            'editorid',
            'editor_utype',
            'companyid',
            'access'
        );
        $companyid = sget($_REQUEST, 'id');
        //Change type of permissions if the second tab is on. Than type is pu1
        if ($ptab == 'coll') $type = 'pu1';
        $Param = array(
            'accessid'     => sget($_REQUEST, 'accessid'),
            'usergroup'    => $type,
            'settings'     => serialize($Permissions),
            'editorid'     => $AccessObj->uid,
            'editor_utype' => $AccessObj->user_type,
            'companyid'    => $companyid,
            'access'       => time()
        );
        $return = '';
        if ($AccessObj->user_type == 'company') {
            if ($AccessObj->uid != $companyid) {
                drupal_set_message("Access Error - 01", 'error');
            } else {
                $return = $AccessObj->update_access($Param);
            }
        } elseif ($AccessObj->user_type == 'admin') {
            if (empty($companyid)) {
                drupal_set_message('Invalid company id', 'error');
            } else {
                if ($AccessObj->admin_of($companyid)) {
                    $return = $AccessObj->update_acces($Param);
                } else {
                    drupal_set_message("Access Error - 02", 'error');
                }
            }
        } elseif ($AccessObj->user_type == 'super') {
            $return = $AccessObj->update_access($Param);
        }
        if ($return) {
            drupal_set_message("Settings updated");
        } else drupal_set_message("Failed to update settings", 'error');
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function perm_table($user_type, $Groups, $Sections, $Rights, $Saved_settings) {
    //Pronik2009 #874 task4 p1 (stikcy titles)
    switch (sget($_REQUEST, 'type')) {
        case 'au':
        $pageName = 'admin_users_page';
            break;
        case 'pu':
        $pageName = 'professional_users_page';
            break;
        case 'cu':
        $pageName = 'company_users_page';
            break;
        case '':
            if (sget($_REQUEST, 'tab')=='settings'&&sget($_REQUEST, 'section')=='com_permissions'&&sget($_REQUEST, 'panel')=='com_permissions')
                $pageName = 'company_users_page';
            break;
        default:
        $pageName = 'other_page';
            break;
    }
    $Table = "<table cellspacing=0 cellpadding=0 class='table-list ".$pageName."'>";
    $Table .= "<tr><td style='width:200px;'><!-- section column --></td>";
    $td_class = '';
    foreach ($Groups as $group_key => $group_title) {
        $title = strtoupper($group_title);
        if ($title=="EXECUTIVES") $title="ACTIVE<br/>EXECUTIVES";
        $Table .= "<td align=center><b>".$title."</b></td>";
    }
    $Table .= "</tr>";


    foreach ($Sections as $key => $Section) {
        $title = strtoupper(sget($Section, 'title'));
        $maenna_name = sget($Section, 'maenna_name');
        $id = sget($Section, 'id');
        if ($title == 'FILE LIST') {
            $regularSections = array('connections' => $Section['sections']['connections'], 'collaborators' => $Section['sections']['collaborators'], 'maenna_file' => $Section['sections']['maenna_file'], 'files' => $Section['sections']['files']);
            $Table .= "\n<tr style='background:#f3f3f3;'><td><b>$title</b></td>";
            foreach ($Groups as $group_key => $group_title) {
                $Table .= "\n<td  align=center>&nbsp;</td>";
            }
            $Table .= "</tr>";
            foreach ($regularSections as $S) {
                $id = sget($S, 'id');
                $title = ucwords(strtolower(sget($S, 'title')));
                $Table .= "\n<tr><td style='text-align:left;padding-left:10px;'>- $title</td>";
                foreach ($Groups as $group_key => $group_title) {
                    $setting = 'hide';
                    if (isset($Saved_settings["$group_key"]) && isset($Saved_settings["$group_key"]["$id"])) {
                        $setting = $Saved_settings["$group_key"]["$id"];
                    }
                    $tag_name = "perm[$group_key][$id]";
                    $otions_rights = Options_rights($Rights, $setting, $tag_name);
                    $Table .= "\n<td align=center>$otions_rights</td>";
                }
                $Table .= "</tr>";
            }
            $_Tags = _TAGS();
            foreach ($_Tags as $key => $value) {
                $Table .= "\n<tr ref='proba' style='background:#DDE6F5;'><td><b>$value</b></td>";
                foreach ($Groups as $group_key => $group_title) {
                    $Table .= "\n<td  align=center>&nbsp;</td>";
                }
                $Table .= "</tr>";
                for ($i = 1; $i < 3; $i++) {
                    $S = $Section['sections'][$key . '_' . $i];
                    $id = sget($S, 'id');
                    $title = ucwords(strtolower(sget($S, 'title')));
                    $Table .= "\n<tr><td style='text-align:left;padding-left:10px;'>- $title</td>";
                    foreach ($Groups as $group_key => $group_title) {
                        $setting = 'hide';
                        if (isset($Saved_settings["$group_key"]) && isset($Saved_settings["$group_key"]["$id"])) {
                            $setting = $Saved_settings["$group_key"]["$id"];
                        }
                        $tag_name = "perm[$group_key][$id]";
                        if ($i == 1) {
                            $File_Rights = array('hide' => 'no access', 'all' => 'All Access');
                        } else $File_Rights = array('hide' => 'no access', 'view' => 'View', 'download' => 'Download');
                        $otions_rights = Options_rights($File_Rights, $setting, $tag_name);
                        $Table .= "\n<td align=center>$otions_rights</td>";
                    }
                    $Table .= "</tr>";
                }
            }
        } else {
            if('advice' == strtolower($title))
                $title = 'Services';

            $Table .= "\n<tr style='background:#f3f3f3;'><td><b>$title</b></td>";
            foreach ($Groups as $group_key => $group_title) {
                $Table .= "\n<td  align=center>&nbsp;</td>";
            }
            $Table .= "</tr>";
            foreach ($Section['sections'] as $S) {
                $id = sget($S, 'id');
                $title = ucwords(strtolower(sget($S, 'title')));
                $Table .= "\n<tr><td style='text-align:left;padding-left:10px;'>- $title</td>";
                foreach ($Groups as $group_key => $group_title) {
                    $setting = 'hide';
                    if (isset($Saved_settings["$group_key"]) && isset($Saved_settings["$group_key"]["$id"])) {
                        $setting = $Saved_settings["$group_key"]["$id"];
                    }
                    $tag_name = "perm[$group_key][$id]";
                    $Secondary_Rights = array("hide" => "no access", "read" => "read", "edit" => "read&add", "write" => "all access");
                    if ($id == '22' || $id == '24' || $id == '25' || $id == '23' || $id == '70' || $id == '71' || $id == 'analysis_comment' || $id == 'minutes_comment' || $id == 'monitoring_comment' || $id == 'performance_comment') {
                        $otions_rights = Options_rights($Secondary_Rights, $setting, $tag_name);
                    } else $otions_rights = Options_rights($Rights, $setting, $tag_name);

                    if(false !== strpos(strtolower($S['title']), 'discuss project')) {
                        $otions_rights = Options_rights(array(
                            "hide" => "no access",
                            "read" => "read",
                            "write" => "write",
                            "read_own" => "read own",
                            "write_own" => "write own",
                            "read_all_write_own" => "write own +",
                        ), $setting, $tag_name);
                    }

                    if('share_knowledge_add_advisor' == $id) {
                        $otions_rights = Options_rights(array(
                            "hide" => "no access",
                            "write" => "write"
                        ), $setting, $tag_name);
                    }

                    if('share_knowledge_remove_interested' == $id) {
                        $otions_rights = Options_rights(array(
                            "hide" => "no access",
                            "write" => "write"
                        ), $setting, $tag_name);
                    }

                    $Table .= "\n<td align=center>$otions_rights</td>";
                }
                $Table .= "</tr>";
            }
        }
    }
    $Table .= "</table>";
    return $Table;
}
