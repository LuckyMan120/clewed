<?php

global $redirect;
$redirect = rebuild_url(array('tab', 'page', 'mtab', 'id'));
function company_name($op = null) {
    global $user, $AccessObj;
    $editorid = $user->uid;
    $companyid = get_var('id');
    $Block['title'] = "";
    $content = '';
    $redirect = rebuild_url(array('tab', 'page', 'mtab', 'id'));
    $section = __FUNCTION__;
    $panel = 'edit_company_info';
    if (($op != 'update') && $op) {
        $sql = "SELECT * FROM maenna_company WHERE companyid  = %d ";
        $result = db_query($sql, array($companyid));
        $Data = db_fetch_object($result);
        $projName = $Data->projname;
        if (empty($projName)) {
            $projName = getProjectName($companyid);
        }
        $_title = "" . htmlentities(strtoupper($projName), ENT_QUOTES | ENT_IGNORE, "UTF-8") . "";
//Get this years revenue and profit
        $sql1 = "SELECT data_value,data_value2 FROM maenna_company_data WHERE data_type = 'financial' AND companyid = %d ORDER BY data_attr DESC LIMIT 1";
        $result1 = db_query($sql1, array($companyid));
        $Data1 = db_fetch_object($result1);
        $founded = 'n/a';
        $foundedid = "";
        $numOfEmp = 'n/a';
        $numOfEmpid = "";
        $numOfEmp = $Data->empnum;
        if ($numOfEmp) $numOfEmp = number_format($numOfEmp);
        $founded = $Data->founded;
        $foundedid = $Data->data_id;
        $industry = $Data->sector;
        $revenue = $Data1->data_value;
        if ($revenue == '') $revenue = "N/A";
        $profit = $Data1->data_value2;
        if ($profit == '') $profit = "N/A";
        $phone = !empty($Data->phone) ? $Data->phone : 'n/a';
        $street1 = !empty($Data->address1) ? $Data->address1 : '';
        $street2 = !empty($Data->address2) ? $Data->address2 : '';
        $city = !empty($Data->city) ? $Data->city : '';
        $state = !empty($Data->state) ? $Data->state : '';
        $zip = !empty($Data->zip) ? $Data->zip : '';
        $address = '';
        if ($street1) $address = "$street1 <br>";
        if ($street2) $address .= "$street2 <br>";
        if ($city) $address .= "$city <br>";
        if ($state . $zip) $address .= "$state, $zip";
        $website = !empty($Data->web) ? $Data->web : 'n/a';
        if ($website && strlen($website) > 14) {
            $website = "<a href='$website' target='_blank'>" . substr($website, 0, 14) . "</a>";
        }
        $logo = $Date->logo;

        $about = \Clewed\Db::get_instance()->get_row("
            SELECT *
            FROM maenna_about
            WHERE project_id = ?",
            array($companyid)
        );


        $src = '';
        $path = ROOT . '/themes/maennaco/images/project/';
        if (!empty($about['project']) && file_exists($path . $about['project'])){
            $src = "/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/" . urlencode($about['project']) . "&zc=1&w=70&h=70";
        }elseif (empty($logo)){
            $src = getAvatarUrl($_REQUEST['id']);
        } else{
            $src = "./" . file_directory_path() . "/" . $logo;
        }

        $edit = '';
        $follow_link = $request_link = '';
        if ($AccessObj->user_type == 'people') {
            /*            if(in_array($AccessObj->_relation, array('pu_analyst','pu_expert', 'pu_visible', 'pu_other')))
                        {
                            if(! Connections::is_following($companyid, $AccessObj->uid))
                            {
                                $follow_link = "<div style='padding:0;margin:0;' class='inline-btn'><a href='$redirect&update_section=follow'>FOLLOW</a></div>";
                            }else{
                                $follow_link = "FOLLOWING";
                            }

                        }*/
            if (in_array($AccessObj->_relation, array('pu_visible'))) {
                if (Connections::allow_to_request($companyid, $AccessObj->uid)) {
                    //$request_link = "<div style='padding:0;margin:0;' class='inline-btn'><a href='$redirect&update_section=request_join_an'>REQUEST TO JOIN ADVISORY NETWORK</a></div>";
                } else {
                    $request_link = "<div>REQUESTED</div>";
                }
            }
        }
        $Block['title'] = "<span style='color:#284B54;font-family:Lato Bold;' class='proj_name'>" . $_title . "</span>";
        if ($op == 'write') {
            $Block['title'] .= "&nbsp;<div class=editbtn>
                        <a href='/account?tab=companies&page=company_detail&id=$companyid&section=company_name&panel=$panel' >Edit</a></div>";
        }

        $hideAvatarStyle = 'about' === $_REQUEST['mtab'] || empty($_REQUEST['mtab']) ? 'display:none;' : '';


        $content .= <<< END
    <!--div style='margin-top:-3px;padding-bottom:3px;'>
        <table cellpadding=0 cellspacing=0 class='no-border' style='padding:0;margin:0;'>
            <tr>
                <td colspan=2>
                    <div style='width:100%;height:80px;background:#f8f8f8 url(/themes/maennaco/images/pro_bg1.png) no-repeat;font:bold 16px Signika; color:white;text-align:center;line-height:80px;overflow:hidden'>
                        $_title
                    </div>
                </td>
            </tr>
            <tr>
                <td style='padding:5px 0; font: 12px Signika'>Founded: $founded</td>
                <td style='padding:5px 0; font: 12px Signika'>Employees: $numOfEmp</td>
            </tr>
        </table>

    </div-->
    <table class='no-border' style='margin-top:0;'>
        <tr>
            <td style='width:130px;'>
                <div  class='blue-border' style='margin-left:20px;height: 60px;$hideAvatarStyle'><img style="margin-top:-15px;" height="70" width="70" src='$src'/> </div>
            </td>
        </tr>
END;
        $content .= '<tr><td>';
        $sectorRev = array();
        if (!empty($industry)) {
            array_push($sectorRev, preg_replace('/(?<=\\w)(?=[A-Z])/', ' $1', $industry));
        }
        if (!empty($revenue) && $revenue != 'N/A') {
            $rev_Text = "";
            $rev_Num = (int)$revenue;
            if($rev_Num >= 1000000000)
            {
                $rev_Text = (int)($rev_Num / 1000000000) . " Billion";
            }
            else
            {
                $rev_Text = (int)($rev_Num / 1000000) . " Million";
            }
            array_push($sectorRev, '$' . $rev_Text);
        }
        $cityState = array();
        if (!empty($city)) array_push($cityState, ucwords($city));
        if (!empty($state)) array_push($cityState, strtoupper($state));
        if (count($cityState) != 0) $content .= '<div class=row0>' . implode(', ', $cityState) . '</div>';
        if ($founded) {
            $content .= "<div class=row0 >Founded: $founded</div>";
        }
        if (count($sectorRev)) {
            $content .= "<div class=row0>" . implode(' - ', $sectorRev) . "</div>";
        }
        // if ($AccessObj->user_type == 'people') {
        //     $content .= "<style>.sidebar_box .wrapper {background-color: #FFF;</style><div class=row0 style='border-bottom: none;margin-top: 60px;font-family: Lato Light,sans-serif;color:#929497; font-size: 18px;font-style: normal;line-height:22px;'>" . $Data->deal_summary_statement . "</div>";
        // }
        $content .= '</td></tr>
</table><br/>' . $follow_link . $request_link;
        $Block['body'] = sidebar_box($Block['title'], $content);
    } elseif ($op == 'update') {
        $founded = sget($_POST, 'founded');
        $numOfEmp = sget($_POST, 'empnum');
        $street1 = sget($_POST, 'street1');
        $street2 = sget($_POST, 'street2');
        $city = sget($_POST, 'city');
        $state = sget($_POST, 'state');
        $website = sget($_POST, 'website');
        $phone = sget($_POST, 'phone');
        $zip = sget($_REQUEST, 'zip');
        $time = time();
        if ($numOfEmp) $numOfEmp = preg_replace("/[^0-9]/", '', $numOfEmp);
        if ($phone) $phone = preg_replace("/[^0-9]/", '', $phone);
        $companyid = sget($_POST, 'id');
        $sql = "UPDATE maenna_company SET phone = '%s',
                                            web = '%s',
                                            address1='%s',
                                            address2 = '%s',
                                            city='%s',
                                            state='%s',
                                            zip='%s',
                                            empnum=%d,
                                            founded='%s',
                                            access='%s'
                                            WHERE companyid = %d LIMIT 1";
        if (db_query($sql, array($phone, $website, $street1, $street2, $city, $state, $zip, $numOfEmp, $founded, '$time', $companyid))) {
            drupal_set_message("Company information is updated");
        } else {
            drupal_set_message("Failed to update company info", 'error');
        }
        //$_SESSION['messages']['status'] = 'Cour company information is updated';
        //drupal_goto("account","a=edit_company_detail");
    }
    return $Block;
}

function follow($op = null) {
    global $AccessObj;
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
    $tab = sget($_REQUEST, 'tab');
    $id = sget($_REQUEST, 'id');
    if ($id && $AccessObj && ($op == 'update')) {
        if ($tab == 'companies') {
            if (Connections::set_connection($id, $AccessObj->uid, '', 'follow')) {
                drupal_set_message("Operation Successful");
                return;
            }
        } elseif ($tab == 'professionals') {
            if (Connections::set_pro_connection($AccessObj->uid, $id, '', 'follow')) {
                drupal_set_message("Operation Successful");
                return;
            }
        }
    }
    drupal_set_message('Invalid Operation', 'error');
    return;
}

function request_join_an($op = null) {
    global $AccessObj;
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
    $tab = sget($_REQUEST, 'tab');
    $id = sget($_REQUEST, 'id');
    if ($id && $AccessObj && ($op == 'update')) {
        if ($tab == 'companies') {
            if (Connections::set_pro_connection($id, $AccessObj->uid, '', 'request')) {
                drupal_set_message("Operation Successful");
                return;
            }
        } elseif ($tab == 'professionals') {
            if (Connections::set_connection($AccessObj->uid, $id, '', 'request')) {
                drupal_set_message("Operation Successful");
                return;
            }
        }
    }
    drupal_set_message('Invalid Operation', 'error');
    return;
}

function what_we_do($op = null) {
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "What We Do";
    $tab = sget($_REQUEST, 'tab');
    $panel = "wwd_panel";
    $data_type = 'what_we_do';
    global $redirect;
    ///
    $content = '';
    if ($op) {
        $sql = "SELECT * FROM maenna_company_data WHERE companyid = %d AND data_type = '%s'";
        $result = db_query($sql, array($companyid, $data_type));
        $Row = db_fetch_object($result);
        if ($Row) {
            $content = strip_tags($Row->data_value2);
            // $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
//            $content = sideBarExcerpt($content, $redirect . "&panel=${panel}&view=detail");
            $content = sideBarMonitoringExcerpt($content, $redirect . "&panel=${panel}&view=detail");
        }
        if ($op == 'write') $Block['title'] .= "<div class='editbtn'><a href='${redirect}&panel=${panel}&view=edit&section=" . __FUNCTION__ . "' class='tool'>Edit</a></div>";
    }
    $Block['body'] = sidebar_box($Block['title'], $content, 'sidebar_info');
    return $Block;
}

function wwd_panel($op = null) {
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "What we do";
    $panel = "wwd_panel";
    $data_type = 'what_we_do';
    global $redirect;
    $dataid = '';
    $content = '';
    if ($op && $op != 'update') {
        $view = sget($_REQUEST, 'view');
        $sql = "SELECT * FROM maenna_company_data WHERE companyid = %d AND data_type = '%s'";
        $result = db_query($sql, array($companyid, $data_type));
        $Row = db_fetch_object($result);
        if ($Row) {
            $content = $Row->data_value2;
            $dataid = $Row->dataid;
        }
        if ($view == 'edit' && $op == 'write') {
            $Block['title'] = "Edit - " . $Block['title'];
            $content = "<form method=post action='/account'>
                        <table class='no-border'>
                            <tr>
                                <td><textarea name='$data_type' style='width:98%;height:400px;'>$content</textarea></td>
                            </tr>
                            <tr>
                                <td><input type=submit name=submit value=submit class=button /> <a href='$redirect' class=button>Cancel</a></td>
                            </tr>
                        </table>
                            <input type=hidden name=dataid value='$dataid' />
                            <input type=hidden name=update_section value='$panel' />
                            <input type=hidden name=view value='detail' />
                        ";
            $content .= hidden_post_values(array('tab', 'page', 'mtab', 'id', 'panel', 'section'));
            $content .= "</form>";
        } elseif ($view == 'detail') {
            $content = nl2br(strip_tags($content));
            // $content = nl2br(htmlentities($content, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $content = "<div class=entry><div class=entry-content>$content</div></div><div class=backbtn><a href='$redirect' class='button'>back</a></div>";
        }
    } elseif ($op == 'update') {
        $data_value2 = sget($_REQUEST, $data_type);
        $companyid = sget($_REQUEST, 'id');
        $dataid = sget($_REQUEST, 'dataid');
        if (empty($companyid)) {
            drupal_set_message("Invalid company id", 'error');
        } else {
            if ($dataid) {
                $sql = "UPDATE maenna_company_data SET data_value2 = '%s', access='%s',editorid=%d WHERE data_type = '%s' AND dataid = %d LIMIT 1";
                $DBValues = array($data_value2, $dataid, $time, $editorid, $data_type);
            } else {
                $sql = "INSERT INTO maenna_company_data (companyid, access, data_type, data_value2,editorid ) VALUES (%d, '%s','%s','%s',%d)";
                $DBValues = array($companyid, $time, $data_type, $data_value2, $editorid);
            }
            if (db_query($sql, $DBValues)) {
                drupal_set_message("Data record is updated");
            } else drupal_set_message("Failed to update record", 'error');
        }
        return;
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

///////////////////////////////////////////////////
function who_we_are($op = null) {
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "Who We Are";
    $tab = sget($_REQUEST, 'tab');
    $panel = "wwa_panel";
    $data_type = 'who_we_are';
    global $redirect;
    ///
    $content = '';
    if ($op) {
        $sql = "SELECT * FROM maenna_company_data WHERE companyid = %d AND data_type = '%s'";
        $result = db_query($sql, array($companyid, $data_type));
        $Row = db_fetch_object($result);
        if ($Row) {
            $content = strip_tags($Row->data_value2);
            // $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $more_link = $redirect . "&panel=${panel}&view=detail";
//            $content = sideBarExcerpt($content, $more_link);
            $content = sideBarMonitoringExcerpt($content, $more_link);
        }
        if ($op == 'write') $Block['title'] .= "<div class='editbtn'><a href='${redirect}&panel=${panel}&view=edit&section=" . __FUNCTION__ . "' class='tool'>Edit</a></div>";
    }
    $Block['body'] = sidebar_box($Block['title'], $content, 'whoweare sidebar_info');
    return $Block;
}

function wwa_panel($op = null) {
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Who We Are";
    $panel = "wwa_panel";
    $data_type = 'who_we_are';
    global $redirect;
    $dataid = '';
    $content = '';
    if ($op && $op != 'update') {
        $view = sget($_REQUEST, 'view');
        $sql = "SELECT * FROM maenna_company_data WHERE companyid = %d AND data_type = '%s'";
        $result = db_query($sql, array($companyid, $data_type));
        $Row = db_fetch_object($result);
        if ($Row) {
            $content = $Row->data_value2;
            $dataid = $Row->dataid;
        }
        if ($view == 'edit' && $op == 'write') {
            $Block['title'] = "Edit - " . $Block['title'];
            $content = "<form method=post action='/account'>
                        <table class='no-border'>
                            <tr>
                                <td><textarea name='$data_type' style='width:98%;height:400px;'>$content</textarea></td>
                            </tr>
                            <tr>
                                <td><input type=submit name=submit value=submit class=button />&nbsp;&nbsp;<a href='$redirect' class=tool>Cancel</a></td>
                            </tr>
                        </table>
                            <input type=hidden name=dataid value='$dataid' />
                            <input type=hidden name=update_section value='$panel' />
                            <input type=hidden name=view value='detail' />
                        ";
            $content .= hidden_post_values(array('tab', 'page', 'mtab', 'id', 'panel', 'section'));
            $content .= "</form>";
        } elseif ($view == 'detail') {
            $content = nl2br(strip_tags($content));
            // $content = nl2br(htmlentities($content, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $content = "<div class=entry>
                            <div class=entry-content>$content</div>
                        </div>
                        <div class=backbtn><a href='$redirect' class='button'>back</a></div>";
        }
    } elseif ($op == 'update') {
        $data_value2 = sget($_REQUEST, $data_type);
        $companyid = sget($_REQUEST, 'id');
        $dataid = sget($_REQUEST, 'dataid');
        if (empty($companyid)) {
            drupal_set_message("Invalid company id", 'error');
        } else {
            if ($dataid) {
                $sql = "UPDATE maenna_company_data SET data_value2 = '%s', access='%s',editorid=%d WHERE data_type = '%s' AND dataid = %d LIMIT 1";
                $DBValues = array($data_value2, $dataid, $time, $editorid, $data_type);
            } else {
                $sql = "INSERT INTO maenna_company_data (companyid, access, data_type, data_value2,editorid ) VALUES (%d, '%s','%s','%s',%d)";
                $DBValues = array($companyid, $time, $data_type, $data_value2, $editorid);
            }
            if (db_query($sql, $DBValues)) {
                drupal_set_message("Data record is updated");
            } else drupal_set_message("Failed to update record", 'error');
        }
        return;
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

//////////////////////////////////////////////////////
function key_markets($op = null) {
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "Our Markets";
    $tab = sget($_REQUEST, 'tab');
    $panel = "key_markets_panel";
    $data_type = 'marketinfo';
    global $redirect;
    ///
    $content = '';
    if ($op) {
        $sql = "SELECT * FROM maenna_company_data WHERE companyid = %d AND data_type = '%s' ORDER BY (data_value3 * 1) DESC";
        $result = db_query($sql, array($companyid, $data_type));
        while ($Row = db_fetch_object($result)) {
            $market = $Row->data_value2;
            $share = $Row->data_value3;
            if ($market) {
                $content .= "<div class=\"row markets\">$market <span class=right style='text-align:right;'>$share</span> </div>";
            }
        }
        if ($op == 'write') $Block['title'] .= "<div class='editbtn'><a href='${redirect}&panel=${panel}&view=edit&section=key_markets' class='tool'>Edit</a></div>";
    }
    $Block['body'] = sidebar_box($Block['title'], $content, 'sidebar_info');
    return $Block;
}

function key_markets_panel($op = null) {
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "Edit - Our Markets";
    $tab = sget($_REQUEST, 'tab');
    $panel = "key_markets_panel";
    $data_type = 'marketinfo';
    global $redirect;
    $content = '';
    if ($op == 'write') {
        $sql = "SELECT * FROM maenna_company_data WHERE companyid = %d AND data_type = '%s' ORDER BY (data_value3 * 1) DESC";
        $result = db_query($sql, array($companyid, $data_type));
        $content = "<form method=post action='/account'>
                    <table class='edit_table no-border'><tr>
                                <td style='width:20px;'></td>
                                <td  style='width:200px;font-weight:bold'>Our Markets</td>
                                <td style='font-weight:bold'>Share</td></tr>";
        $counter = 1;
        for ($i = 1; $i <= 5; $i++) {
            $Row = db_fetch_array($result);
            if (empty($Row)) $Row = array();
            $market = sget($Row, 'data_value2');
            $share = sget($Row, 'data_value3');
            $dataid = sget($Row, 'dataid');
            $content .= "\n<tr> <td >$counter.</td>
                                <td><input type=text name='market_$i' value='$market' style='width:180px;' /> </td>
                                <td><input type='text' name='share_$i' value='$share' style='width:80px;' />
                                    <input type=hidden name='dataid_$i' value='$dataid' /></td></tr>";
            $counter++;
        }
        $content .= "<tr><td></td><td colspan=2 style='padding-top:8px;'>
                        <input type=submit name=submit value='submit' class=button />
                        &nbsp;
                        <a href='$redirect' class=button>Cancel</a>
                    </td></tr></table>";
        $content .= hidden_post_values(array('tab', 'page', 'mtab', 'id', 'panel', 'view'));
        $content .= "<input type=hidden name='update_section' value='key_markets_panel' /></form>";
    } elseif ($op == 'update') {
        for ($i = 1; $i <= 5; $i++) {
            $market = sget($_REQUEST, "market_$i");
            $share = sget($_REQUEST, "share_$i");
            $dataid = sget($_REQUEST, "dataid_$i");
            if (empty($dataid)) {
                $sql = "insert into maenna_company_data (companyid, data_type, data_attr, data_value2,data_value3,editorid, access) values(%d, 'marketinfo', '$i', '%s','%s',%d, %s)";
                $sql_data = array($companyid, $market, $share, $editorid, $time);
            } else {
                $sql = "UPDATE maenna_company_data SET data_value2 = '%s',data_value3 = '%s', editorid = %d, access='%s' WHERE dataid = %d AND companyid = %d LIMIT 1";
                $sql_data = array($market, $share, $editorid, $time, $dataid, $companyid);
            }
            if (!db_query($sql, $sql_data)) {
                drupal_set_message("Failed to update market info. Please try again later", 'error');
                return;
            }
        }
        drupal_set_message("Company market information is updated.");
        return;
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

//////////////////////////////////////////////////////////////////
function milestones($op = null) {
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "Our Milestones";
    $tab = sget($_REQUEST, 'tab');
    $section = __FUNCTION__;
    $panel = "milestones_panel";
    $data_type = 'keymilestone';
    global $redirect;
    $content = '';
    if ($op) {
        $sql = "SELECT * FROM maenna_company_data WHERE companyid = %d AND data_type = 'keymilestone' AND deleted != 1 ORDER BY dataid DESC LIMIT 3";
        $result = db_query($sql, array($companyid));
        $counter = 0;
        $viewall_link = $redirect . "&panel=${panel}&view=listview&section=$section";
        while (($Row = db_fetch_object($result)) !== false) {
            $counter++;
            $title = htmlentities($Row->data_attr, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $text = strip_tags($Row->data_value2);
            // $text =     htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
            $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
//            $text = sideBarExcerpt($text);
            $text = sideBarMonitoringExcerpt($text, $viewall_link);
            $last = '';
            $content .= "<div class='entry'>
                            <div class=title>$title

                            </div>
                        <div class=content>$text</div></div>";
            ////    <div class=editbtn><a href='$edit_link' class=tool>Edit</a></div>
        }
        $url = $redirect . "panel=milestones_panel&view=add";
        $add_link = $redirect . "&panel=${panel}&view=add&section=$section";
        $edit_link = $redirect . "&panel=${panel}&view=listview&section=$section";
        if ($op == 'write') {
            $Block['title'] .= "<div class=editbtn>
                                    <a href='$add_link' class=tool>Add</a>
                            </div>";
        }
//        $content .= "\n<div class='viewallbtn'><a href='$viewall_link' class=tool>More</a></div>";
    }
    $Block['body'] = sidebar_box($Block['title'], $content, 'sidebar_info');
    return $Block;
}

function milestones_panel($op = null) {
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "Our Milestones";
    $tab = sget($_REQUEST, 'tab');
    $panel = __FUNCTION__;
    $data_type = 'keymilestone';
    $content = '';
    $redirect = rebuild_url(array('tab', 'page', 'id', 'mtab'));
    $back = rebuild_url(array('tab', 'page', 'id', 'mtab'));
    $add_link = $redirect . "&panel=${panel}&view=add&section=milestones";
    if ($op == 'write') $Block['title'] .= "<div class=editbtn><a href='$add_link' class=tool>Add</a></div>";
    if ($op && $op != 'update') {
        $view = sget($_REQUEST, 'view');
        if ($view == 'detail' || $view == 'edit' || $view == 'add') {
            $dataid = sget($_REQUEST, 'dataid');
            $Row = array();
            if ($dataid) {
                $sql = "SELECT * FROM maenna_company_data WHERE dataid = %d LIMIT 1";
                $result = db_query($sql, array($dataid));
                $Row = db_fetch_array($result);
            } elseif (empty($dataid) && ($view == 'detail')) {
                $sql = "SELECT * FROM maenna_company_data WHERE companyid = %d AND data_type = '%s' ORDER BY dataid DESC LIMIT 1";
                $result = db_query($sql, array($companyid, $data_type));
                $Row = db_fetch_array($result);
            }
            if ($Row !== false) {
                if ($view == 'detail') {
                    $dataid = sget($Row, 'dataid');
                    $year = ucwords(sget($Row, 'data_attr'));
                    $year = htmlentities($year, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $milestone = sget($Row, 'data_value2');
                    // $milestone = nl2br(htmlentities($milestone, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                    $milestone = nl2br(strip_tags($milestone));
                    $content = "<div class=entry>
                                    <div class=entry-title>$year</div>
                                    <div class=entry-content>$milestone</div>
                                </div>";
                    $rem_link = $redirect . "&panel=${panel}&view=listview&update_section=${panel}&do=remove&dataid=$dataid";
                    $content .= "<div class=backbtn>";
                    if ($op == 'write') $content .= "<div class='rembtn'><a href='$rem_link' class=button onclick='return confirm(\"Continue to remove record\")'>Delete</a></div>";
                    $content .= "<a href='$redirect&panel=${panel}&view=listview' class=button>back</a>
                                </div>";
                    $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
                    if ($op == 'write') $Block['title'] .= "<div class=editbtn><a href='$edit_link' class=tool>Edit</a></div>";
                } elseif ($view == 'edit' || $view == 'add' && $op == 'write') {
                    $year = ucwords(sget($Row, 'data_attr'));
                    $dataid = sget($Row, 'dataid');
                    $milestone = sget($Row, 'data_value2');
                    if ($view == 'edit') {
                        $title = "Edit Milestone";
                        $do = 'update';
                        $rem_link = $redirect . "&panel=${panel}&view=listview&update_section=${panel}&do=remove&dataid=$dataid&section=milestones";
                    } else {
                        $title = "Add Milestone";
                        $do = 'insert';
                        $rem_link = '';
                    }
                    $Block['title'] = $title;
                    $hv = hidden_post_values(array('tab', 'page', 'id', 'mtab', 'panel', 'section'));
                    $content .= <<< END
                    <form action='/account' method='post' onsubmit='return check_input();'>
                        <div class=entry>
                            <div class=entry-content>
                                <div>Year:<br /><input type=text name=year value='$year' class='require_string' /></div><br />
                                <div>Milestone:<br /><textarea name='milestone' style='width:99%;height:400px;' class='require_string' >$milestone</textarea>
                                <div class=backbtn>
                                    <input style="margin-right:-10px;" type=submit name=submit value=Submit class=button />
                                    &nbsp;&nbsp;
                                    <a href='$back' style='margin-left: 10px; background: none repeat scroll 0 0 #43a0c1 !important;' class=button>Cancel</a>
                                    &nbsp;&nbsp;
                                    <a ref='$rem_link' style='margin-left: -3px; background: none repeat scroll 0 0 #43a0c1 !important;' class='button' onclick='return confirm("Continue to remove record")'>Delete</a>
                                </div>
                            </div>
                        </div>
                        $hv
                        <input type='hidden' name=dataid value='$dataid' />
                        <input type='hidden' name=view value='detail' />
                        <input type='hidden' name=do value='$do' />
                        <input type='hidden' name=update_section value='$panel' />
                    </form>
END;
                }
            }
        } elseif ($view == 'listview') {
            $limit = 5;
            $_page = sget($_REQUEST, '_page');
            if (empty($_page)) $_page = 1;
            $start = ($_page - 1) * $limit;
            $sql = "SELECT count(*) AS total FROM maenna_company_data WHERE data_type = '%s' AND companyid = %d AND deleted != 1";
            $result = db_query($sql, array($data_type, $companyid));
            $Row = db_fetch_object($result);
            $total = $Row->total;
            $sql = "select * from maenna_company_data where data_type = '%s' and companyid = %d and deleted != 1 order by dataid desc limit $start, $limit";
            $result = db_query($sql, array($data_type, $companyid));
            while (($Row = db_fetch_object($result)) !== false) {
                $access = date('m/d/Y', $Row->access);
                $title = htmlentities($Row->data_attr, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                $text = nl2br(strip_tags($Row->data_value2));
                // $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                $dataid = $Row->dataid;
                $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
                $text = contentExcerpt($text, $more_link);
                $content .= "<div class=entry>";
                if ($op == 'write') $content .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid' class='tool' >Edit</a></div>";
                $content .= "<div class='entry-title'>$title</div><div class='entry-content'>" . $text . "</div></div><hr class='line' />";
            }
            $Pagenation = array(
                'total'        => $total,
                'limit'        => 5,
                'baseurl'      => '/account?',
                'num_of_links' => 8,
            );
            $content .= pagination($Pagenation);
        }
    } elseif ($op == 'update') {
        $do = sget($_REQUEST, 'do');
        $Correct = false;
        if ($do == 'update' || $do == 'insert') {
            $dataid = sget($_REQUEST, 'dataid');
            $year = sget($_REQUEST, 'year');
            $milestone = sget($_REQUEST, 'milestone');
            if (empty($year) || empty($milestone)) {
                drupal_set_message("Please complete the form and try again", 'error');
            } elseif ($dataid && ($do == 'update')) {
                $DBValues = array(
                    'access'      => $time,
                    'data_attr'   => $year,
                    'data_value2' => $milestone,
                    'editorid'    => $editorid
                );
                foreach ($DBValues as $key => $val) {
                    $SQL_STR["$key"] = "$key = '%s'";
                }
                $SQL_STR["editorid"] = "editorid=%d";
                $sql = "UPDATE maenna_company_data SET " . implode(',', $SQL_STR) . " WHERE dataid = " . ((int) $dataid) . " LIMIT 1";
                if (db_query($sql, $DBValues)) $Correct = true;
            } elseif ($do == 'insert') {
                $DBKeys = array(
                    'companyid',
                    'access',
                    'data_type',
                    'data_attr',
                    'data_value2',
                    'editorid'
                );
                $DBValues = array($companyid, $time, $data_type, $year, $milestone, $editorid);
                $SQL_STR = array("%d", "'%s'", "'%s'", "'%s'", "'%s'", "%d");
                $sql = "INSERT INTO maenna_company_data (" . implode(',', $DBKeys) . ") VALUES(" . implode(',', $SQL_STR) . ")";
                if (db_query($sql, $DBValues)) $Correct = true;
            }
        } elseif ($do == 'remove') {
            $dataid = sget($_REQUEST, 'dataid');
            if ($dataid) {
                $sql = "UPDATE maenna_company_data SET access = '%s', editorid = %d, deleted = 1 WHERE dataid = %d LIMIT 1";
                if (db_query($sql, array($time, $editorid, $dataid))) $Correct = true;
            }
        }
        if ($Correct) {
            drupal_set_message("Operation Successful");
        } else {
            drupal_set_message("Operation Failed", 'error');
        }
        return;
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

//////  connection widget for company and pro
function followers($op = null) {
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = '';
    $data_type = '';
    global $redirect;
    $content = '';
    global $AccessObj;
    require_once("." . base_path() . path_to_theme() . "/includes/classes/.inc");
    if ($op && ($op != 'update')) {
        $box_content = '';
        $advisor_active = 'team_active';
        $box_title = "Followers";
        $all_folls = getFollowing($companyid);
        $box_title .= " (" . count($all_folls) . ")";
        foreach ($all_folls as $pro_uid) {
            //Get user gender
            $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = " . ((int) $pro_uid));
            $gender_tmp = mysql_fetch_array($q1);
            $gender = $gender_tmp['gender'];
            //Check if user have a profile picture
            if (file_exists('sites/default/images/profiles/50x50/' . $pro_uid . '.jpg')) {
                $avatar = 'sites/default/images/profiles/50x50/' . $pro_uid . '.jpg';
            } else {
                if ($gender == 'm' || $gender == '') {
                    $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                } else {
                    $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                }
            }
            $pro_maeid = getProId($pro_uid);
            $rid = userRoleId($pro_uid);
            $pro_type = Options_proType($rid, 1);
            $pro_exp = getProExpertise($pro_uid);
            if ($AccessObj->Com_sections[$_REQUEST['mtab']]['sections']['followers']['access'] == 'read') {
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:'LatoRegular'; text-transform:capitalize;margin-top:20px;\">$pro_maeid, $pro_exp</p></div>";
            } else {
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1'> $pro_maeid,  $pro_exp</a></div>";
            }
        }
        $content = <<< END
        <table class='team_stats' style='padding-bottom:0;margin-bottom:0;'>
            <tr>
                <td style='border:none;hieght:8px;line-height:8 px;' colspan=3>&nbsp;</td>
            </tr>
            <tr>
                <td colspan=3 style='border-color:#dce6f5;border-bottom:none'>
                    <div class="team_stat_list" style="border-bottom:solid 2px #DCE6F5;">
                        <div class="title" id="team_stat_title">
                            $box_title

                        </div>

                    </div>
                </td>
            </tr>
            <tr>
                <td colspan=3 style='border-top:none;'>
                    <div class="team_stat_list">
                        <div class="list coll_list" id="team_stat_list">
                            $box_content
                        </div>
                    </div>
                </td>
            </tr>
        </table>

END;
    }
    $Block['body'] = sidebar_box('', $content);
    return $Block;
}

function collaborators($op = null) {

    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = '';
    $data_type = '';
    global $redirect;
    $content = '';
    global $AccessObj;
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
    if ($op && ($op != 'update')) {
        $box_content = '';
        $perm = $AccessObj->Com_sections[$_REQUEST['mtab']]['sections']['connections']['access'];
        $requestPerm = $AccessObj->Com_sections['share_knowledge']['sections']['collaborators']['access'];
        $all_colls = getCollaborators($companyid);
        $box1_cnt = count($all_colls['active']);
        $all_folls = getFollowing($companyid);
        $box2_cnt = count($all_folls);
        $ptype = '';
        $ptype = $_REQUEST['ptype'];
        //echo $collaborator_active1."sdfsdf";
        if ($ptype != '') {
            $box_content = '';
            $box_title = '';
            $style = 'style=display:block;';
        } else {
            $box_content = '';
            $box_title = '';
            $style = 'style=display:none;';
        }
        if ($ptype == 'collaborators') {
            $collaborator_active1 = 'team_active';
        } elseif ($ptype == 'following') {
            $follower_active1 = 'team_active';
        } else {
            $collaborator_active1 = '';
            $follower_active1 = '';
        }
        if (empty($ptype) || $ptype == 'collaborators') {
            //$collaborator_active1 = 'team_active';
            $box_title = "Interested";
            foreach ($all_colls['active'] as $pro_uid) {
                //Get user gender
                $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = " . ((int) $pro_uid));
                $gender_tmp = mysql_fetch_array($q1);
                $gender = $gender_tmp['gender'];
                //Check if user have a profile picture
                if (file_exists('sites/default/images/profiles/50x50/' . $pro_uid . '.jpg')) {
                    $avatar = 'sites/default/images/profiles/50x50/' . $pro_uid . '.jpg';
                } else {
                    if ($gender == 'm' || $gender == '') {
                        $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                    } else {
                        $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                    }
                }
                $pro_maeid = getProId($pro_uid);
                $rid = userRoleId($pro_uid);
                $pro_type = Options_proType($rid, 1);
                $pro_exp = getProExpertise($pro_uid);
                if ($AccessObj->Com_sections[$_REQUEST['mtab']]['sections']['collaborators']['access'] == 'read') {
                    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:Helvetica; text-transform:uppercase;margin-top:20px;\">$pro_maeid, $pro_exp</p></div>";
                } else {
                    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1'> $pro_maeid, $pro_exp</a></div>";
                }
            }
        } elseif ($ptype == 'following') {
            //die('proba');
            //$follower_active1 = 'team_active';
            $box_title = "Following";
            foreach ($all_folls as $pro_uid) {
                //Get user gender
                $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = " . ((int) $pro_uid));
                $gender_tmp = mysql_fetch_array($q1);
                $gender = $gender_tmp['gender'];
                //Check if user have a profile picture
                if (file_exists('sites/default/images/profiles/50x50/' . $pro_uid . '.jpg')) {
                    $avatar = 'sites/default/images/profiles/50x50/' . $pro_uid . '.jpg';
                } else {
                    if ($gender == 'm' || $gender == '') {
                        $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                    } else {
                        $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                    }
                }
                $pro_maeid = getProId($pro_uid);
                $rid = userRoleId($pro_uid);
                $pro_type = Options_proType($rid, 1);
                $pro_exp = getProExpertise($pro_uid);
                if ($AccessObj->Com_sections[$_REQUEST['mtab']]['sections']['followers']['access'] == 'read') {
                    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:Helvetica; text-transform:uppercase;margin-top:20px;\">$pro_maeid, $pro_exp</p></div>";
                } else {
                    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1'> $pro_maeid, $pro_exp</a></div>";
                }
            }
        }
        $content = <<< END

		<div id="advisor" style="margin: -17px 0px 0px 0px;">
		<!--<div class="adtitle">Collaborators</div><br/>-->

		<div class="adcoments"><a id="selFoll" style="width:120px;cursor:pointer" class='$follower_active1' href="$redirect&ptype=following" >Following<span>$box2_cnt</span></a> </div>

	</div><br/>

        <table class='team_stats' id='coll' style='padding-bottom:0;margin-bottom:0;'>
           <!-- <tr class=link>
            <td name="selType" ><a style="width:120px;cursor:pointer" class='$collaborator_active'  href="$redirect&ptype=collaborators">
                <span class='num'>$box1_cnt</span>
                <span class=type>COLLABORATORS -</span></a>
            </td>
            <td name="selType" ><a style="width:120px;cursor:pointer" class='$follower_active'  href="$redirect&ptype=following" >
                <span class='num'>$box2_cnt</span>
                <span class=type>FOLLOWING -</span></a>
            </td>

            </tr>-->
            <tr>
                <td style='border:none;hieght:8px;line-height:8 px;' colspan=3>&nbsp;</td>
            </tr>
            <tr $style>
                <td colspan=3 style='border-color:#dce6f5;border-bottom:none'>
                    <div class="team_stat_list" style="border-bottom:solid 2px #DCE6F5;">
                        <div class="title" id="team_stat_title">
                            $box_title

                        </div>

                    </div>
                </td>
            </tr>
            <tr $style>
                <td colspan=3 style='border-top:none;'>
                    <div class="team_stat_list">
                        <div class="list coll_list" id="team_stat_list">
                            $box_content
                        </div>
                    </div>
                </td>
            </tr>
        </table>

END;
        if (in_array($requestPerm, array('read', 'write')) || $AccessObj->user_type == 'super' || Connections::admin_of($companyid)) {
            if($AccessObj->user_type != 'people'){
                $content .= '<br><div class="team_stat_list">
                        <div class="title" id="coll_req" style="background:none; padding-left:0px;" >
                            Requests
                        </div>
</div>
                    ';
            }
            if (count($all_colls['pending']) != 0) {
                $content .= 'Connection requests';
            }
            if ($all_colls['pending'][0] != null) {
                foreach ($all_colls['pending'] as $pro_uid) {
                    $pro_maeid = getProId($pro_uid);
                    $rid = userRoleId($pro_uid);
                    $pro_type = Options_proType($rid, 1);
                    $content .= "
                        <div class=\"row singlerow\" style=\"white-space:normal; padding:5px 0\">
                            <a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1'> $pro_maeid</a>
                            <a style='" . ('write' != $requestPerm ? 'display:none;':'') . "float:right; cursor:pointer;' cnf_type='cnfColl' uname='" . $pro_maeid . "' type='cnf_req' uid='" . $pro_uid . "' proj_name='" . getProjectName($companyid) . "' class='tool'>CONFIRM</a>
                        </div>";
                }
            }
            $q = mysql_query("SELECT DISTINCT assignee_uid FROM maenna_connections WHERE target_uid= " . ((int) $companyid) . " AND conntype='client' AND status = 'pending'") or die(mysql_error());
            if (mysql_num_rows($q) > 0) $content .= 'Management';
            while ($row = mysql_fetch_array($q)) {
                $content .= "<div class=\"row singlerow\" style=\"white-space:normal; padding:5px 0\">
	 <a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$row[assignee_uid]&closebtn=1'>" . getProId($row[assignee_uid]) . "</a>
	                             <a style='float:right;  cursor:pointer;' uname='" . getProId($row[assignee_uid]) . "' type='cnf_req' uid='" . $row['assignee_uid'] . "' class='tool' proj_name='" . getProjectName($companyid) . "' cnf_type='cnfMgmnt'>CONFIRM</a>
	                         </div>";
            }
            $content .= '<div id="dialog-confirm" title="Confirm request?" style="display:none;"><p class="dialog_text"></p>
	     <input type="hidden" cid="' . $companyid . '" cnf_type=""  uname=""  uid="" id = "cnf_uid">
	 </div>

    <style>

    .ui-dialog .ui-dialog-buttonpane .ui-dialog-buttonset {
      float:left;
      margin-left: 10px;
    }

    </style>
	<script type="text/javascript">


	$(document).ready(function(){

        $( "#dialog-confirm" ).dialog({
            resizable: false,
            height:150,
            width: 350,
            modal: true,
            autoOpen: false,
            buttons: {
                "Confirm": function() {

                    uid = $("#cnf_uid").attr("uid");
                    cnf_type = $("#cnf_uid").attr("cnf_type");
	                     $.post("/themes/maennaco/includes/collaborator.php?type="+cnf_type, {companyId: ' . $companyid . ',pid:uid,status: "active"},

				       function(response){

                                       $("#cnf_uid").attr("uid","");
                                       $("div.coll_list").append(response);
                                       $(\'a[uid=\'+uid+\']\').parent().remove();

                                       if (cnf_type == "cnfColl") $(".coll_num").html(parseInt($(".coll_num").html())+1);
                                       else $(".client_num").html(parseInt($(".client_num").html())+1);


                                       });
                    $( this ).dialog( "close" );
                },
                "Decline": function() {
                    uid = $("#cnf_uid").attr("uid");
                    cnf_type = $("#cnf_uid").attr("cnf_type");
	                     $.post("/themes/maennaco/includes/collaborator.php?type="+cnf_type, {companyId: ' . $companyid . ',pid:uid,status: "deactivated"},

				       function(response){

                                       $("#cnf_uid").attr("uid","");
                                       $(\'a[uid=\'+uid+\']\').parent().remove();


                                       });
                    $( this ).dialog( "close" );
                },
                "Cancel": function() {
                    $( this ).dialog( "close" );
                },

            }
        });
        init_cnf_collaborate();
    });


        </script>';
        }
    }
    $Block['body'] = sidebar_box('', $content);
    return $Block;
}

function connections($op = null) {
//    print "<pre>".print_r(debug_backtrace(),true)."</pre>";
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
    $editorid = $user->uid;
    $userid = sget($_REQUEST,'userid');
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $companyName = getProId($companyid);
    $Block['title'] = '';
    $data_type = '';
    global $redirect;
    $content = '';
    global $AccessObj;
    $ref = '';
    $content_title = 'Team';
    if ($AccessObj->user_type == 'people' || $userid) {
        $quest_mark = '';
        if ($_REQUEST['ref']) $ref = '&ref=' . $_REQUEST['ref'];
    } else {
        $quest_mark = '<span id="questmark" style="float: right"><a href="#" id="get-help-btn"><img src="/themes/maennaco/images/questionmark.png" style="padding-top:2px;" /></a></span>';
    }
    if ($op && ($op != 'update')) {
        $Conns = Connections::Com_conns($companyid);
//die("<pre>".print_r($Conns,true)."</pre>");
        $box0_cnt = count($Conns['Partner']);
        $box1_cnt = count($Conns['Advisor']);
        $box2_cnt = count($Conns['Visible']);
        $box3_cnt = count($Conns['Client']);
		$box4_cnt = count($Conns['Investor']);
        $all_colls = getCollaborators($companyid);
        $coll_cnt = count($all_colls['active']);
        $advisor_active = $following_active = $conn_active = '';
        $ctype = sget($_REQUEST, 'ctype');
        $LIST = '';
        $box_content = '';
        //if (!isset($_REQUEST['mtab'])) $_REQUEST['mtab'] = 'questionnaire';
        $perm = $AccessObj->Com_sections[$_REQUEST['mtab']]['sections']['connections']['access'];
        $requestPerm = $AccessObj->Com_sections['share_knowledge']['sections']['collaborators']['access'];
        $requestPermA = $AccessObj->Com_sections['share_knowledge']['sections']['share_knowledge_add_advisor']['access'];
        $requestPermX = $AccessObj->Com_sections['share_knowledge']['sections']['share_knowledge_remove_interested']['access'];
        if (empty($ctype) || $ctype == 'advisor') {
            $advisor_active = 'team_active';
            $box_title = "Active";
            foreach ($Conns['Advisor'] as $Pro) {
                $pro_uid = $Pro->assignee_uid;
                //Get user gender
                $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = " . ((int) $pro_uid));
                $gender_tmp = mysql_fetch_array($q1);
                $gender = $gender_tmp['gender'];
                //Check if user have a profile picture
                if (file_exists('sites/default/images/profiles/50x50/' . $pro_uid . '.jpg')) {
                    $avatar = 'sites/default/images/profiles/50x50/' . $pro_uid . '.jpg';
                } else {
                    if ($gender == 'm' || $gender == '') {
                        $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                    } else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                }
                $pro_maeid = getProId($pro_uid);
                $rid = userRoleId($pro_uid);
                $pro_type = Options_proType($rid, 1);
                $pro_exp = getProExpertise($pro_uid);
                if ($AccessObj->Com_sections[$_REQUEST['mtab']]['sections']['connections']['access'] == 'read') {
                    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left;               margin-right:5px; width:50px; height:50px;\">
                 <p style=\" font-size:11px; color:#666; font-family:'LatoRegular'; text-transform:capitalize;margin-top:20px;\">$pro_maeid, $pro_exp</p></div>";
                } else {
                    $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\">
<a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1'> $pro_maeid, $pro_exp</a></div>";
                }
            }
        } elseif ($ctype == 'following') {
            $following_active = 'team_active';
            $box_title = "Management";
            foreach ($Conns['Client'] as $Pro) {
                $pro_uid = $Pro->target_uid;
                //Get user gender
                $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = " . ((int) $pro_uid));
                $gender_tmp = mysql_fetch_array($q1);
                $gender = $gender_tmp['gender'];
                //Check if user have a profile picture
                if (file_exists('sites/default/images/profiles/50x50/' . $pro_uid . '.jpg')) {
                    $avatar = 'sites/default/images/profiles/50x50/' . $pro_uid . '.jpg';
                } else {
                    if ($gender == 'm' || $gender == '') {
                        $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                    } else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                }
                $pro_maeid = getProId($pro_uid);
                $rid = userRoleId($pro_uid);
                $pro_type = Options_proType($rid, 1);
                $pro_exp = getProExpertise($pro_uid);
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . "  style=\"float:left; margin-right:5px; width:50px; height:50px;\"><a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1'>$pro_maeid, $pro_exp</a></div>";
            }
        } elseif ($ctype == 'connections') {
            $pro_uid = $Pro->target_uid;
            //Get user gender
            $q1 = mysql_query("SELECT gender FROM maenna_people WHERE pid = " . ((int) $pro_uid));
            $gender_tmp = mysql_fetch_array($q1);
            $gender = $gender_tmp['gender'];
            //Check if user have a profile picture
            if (file_exists('sites/default/images/profiles/50x50/' . $pro_uid . '.jpg')) {
                $avatar = 'sites/default/images/profiles/50x50/' . $pro_uid . '.jpg';
            } else {
                if ($gender == 'm' || $gender == '') {
                    $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                } else $avatar = '/themes/maennaco/images/prof-avatar-female.png';
            }
            $conn_active = 'team_active';
            $box_title = "Connected";
            foreach ($Conns['Visible'] as $Pro) {
                $pro_maeid = getProId($pro_uid);
                $rid = userRoleId($pro_uid);
                $pro_type = Options_proType($rid, 1);
                $pro_exp = getProExpertise($pro_uid);
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=" . $avatar . " style=\"float:left; margin-right:5px; width:50px; height:50px;\"><a style=\"margin-top:20px;\" href='/account?tab=professionals&page=pro_detail&id=$pro_uid&closebtn=1'> $pro_maeid, $pro_exp</a></div>";
            }
        }
        $content = <<< END
        <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
        <script type="text/javascript">

    function addAdvisor(cid, uid) {
        if (confirm("Do you really want to add this expert to Active section?")) {
            $.post("/themes/maennaco/includes/collaborator.php?type=moveToAdvisors", { companyId: cid, pid: uid},
            function(response){
                if(response == 'success') {
                    var counter = $("#selColl .coll_num").text();
                    if(counter > 0)
                        $("#selColl .coll_num").text(counter - 1);
                    var adviseSection = $('.adcoments.advisors'),
                        counterSpan = adviseSection.find('span'),
                        counter = counterSpan.text();
                    counterSpan.text(counter - 0 + 1);
                    adviseSection.find('a').click();
                }
            });
        }
    };

	$(document).ready(function(){

        $('#get-help-btn').click(function(e) {
            e.preventDefault();
          $("#get-help-dialog").dialog({
            modal: true,
            autoOpen: true,
            resizable: false,
            width: 600,
            title: 'Your connections',
          });
        });

    remCollab = function(cid, uid) {
        if (confirm("Do you really want to uncollaborate?")) {
            $.post("/themes/maennaco/includes/collaborator.php?type=uncollaborate", { companyId: cid, pid: uid},
            function(response){
                if(response == 'Disconnect succeeded') {
                    var counter = $("#selColl .coll_num").text();
                    if(counter > 0)
                        $("#selColl .coll_num").text(counter - 1);
                    $("#selColl").click();
                    $("#selColl").click();
                }
            });
        }
    }

    $("#selColl").livequery("click", function(event){

        event.preventDefault();

        var thisObj = $(this);
        var perm = "$perm";
        var requestPerm = "$requestPerm";
        var requestPermA = "$requestPermA";
        var requestPermX = "$requestPermX";

        if(thisObj.hasClass('team_active')) {
            thisObj.removeClass('team_active');
            $('.team1').hide();
            return true;
        }

        $.post("/themes/maennaco/includes/fetch.php?type=getCollaborators$ref", {
            companyId: '$companyid',
            perm:perm,
            requestPerm:requestPerm,
            requestPermA:requestPermA,
            requestPermX:requestPermX
            }, function(response){
                $('a[id^="sel"]').removeClass('team_active');
                thisObj.addClass('team_active');
                $("#team_stat_title").html("Collaborators");
                $("#team_stat_list").html($(response).fadeIn('slow'));
                var ltdh = 0;
                $("td.left-td .left-td-wrapper").children().each(function(){
                    ltdh = ltdh + $(this).outerHeight();
                });
                var mtdh = $("td.act-content").height();
                if(ltdh+260 >= mtdh){
                  $("td.left-td .left-td-wrapper").css('padding-bottom',260);
                }
                if($("#coll_req").length > 0){
                  //$("#coll_req").parent().css("margin-top",260);
                } else {
                  $("td.left-td .left-td-wrapper").css('padding-bottom',340);
                  //$(".whoweare").css("margin-top",340);
                }

                $('.team1').insertAfter(thisObj.parent());
                $('.team1').show();
            });

    });
    
    $("#selInvestor").livequery("click", function(event){

        event.preventDefault();

        var thisObj = $(this);
        var perm = "$perm";
        var requestPerm = "$requestPerm";
        var requestPermA = "$requestPermA";
        var requestPermX = "$requestPermX";

        if(thisObj.hasClass('team_active')) {
            thisObj.removeClass('team_active');
            $('.team1').hide();
            return true;
        }

        $.post("/themes/maennaco/includes/fetch.php?type=getProInvestors$ref", {
            companyId: $companyid,
            perm:perm,
            requestPerm:requestPerm,
            requestPermA:requestPermA,
            requestPermX:requestPermX
            }, function(response){
                $('a[id^="sel"]').removeClass('team_active');
                thisObj.addClass('team_active');
                $("#team_stat_title").html("Investors");
                $("#team_stat_list").html($(response).fadeIn('slow'));
                var ltdh = 0;
                $("td.left-td .left-td-wrapper").children().each(function(){
                    ltdh = ltdh + $(this).outerHeight();
                });
                var mtdh = $("td.act-content").height();
                if(ltdh+260 >= mtdh){
                  $("td.left-td .left-td-wrapper").css('padding-bottom',260);
                }
                if($("#coll_req").length > 0){
                  //$("#coll_req").parent().css("margin-top",260);
                } else {
                  $("td.left-td .left-td-wrapper").css('padding-bottom',340);
                  //$(".whoweare").css("margin-top",340);
                }

                $('.team1').insertAfter(thisObj.parent());
                $('.team1').show();
            });

    });

        $("#selFoll").livequery("click", function(event){

        event.preventDefault();


        var thisObj = $(this);

        if(thisObj.hasClass('team_active')) {
            thisObj.removeClass('team_active');
            $('.team1').hide();
            return true;
        }

        var perm = "$perm";

        $.post("/themes/maennaco/includes/fetch.php?type=getFollowing$ref", {companyId: $companyid,perm:perm},

            function(response){

                $('a[id^="sel"]').removeClass('team_active');
                thisObj.addClass('team_active');
                $("#team_stat_title").html("Following");
                $("#team_stat_list").html($(response).fadeIn('slow'));
                var ltdh = 0;
                $("td.left-td .left-td-wrapper").children().each(function(){
                    ltdh = ltdh + $(this).outerHeight();
                });
                var mtdh = $("td.act-content").height();
                if(ltdh+260 >= mtdh){
                  $("td.left-td .left-td-wrapper").css('padding-bottom',260);
                }

                if($("#coll_req").length > 0){
                  //$("#coll_req").parent().css("margin-top",260);
                } else {
                  $("td.left-td .left-td-wrapper").css('padding-bottom',340);
                  //$(".whoweare").css("margin-top",340);
                }


                $('.team1').insertAfter(thisObj.parent());
                $('.team1').show();
            });

    });


        $("#selAdv").livequery("click", function(){

	var thisObj = $(this);

	if(thisObj.hasClass('team_active')) {
        thisObj.removeClass('team_active');
        $('.team1').hide();
        return true;
    }

        var perm = "$perm";

        $.post("/themes/maennaco/includes/posts.php?type=teamStats&ctype=advisor$ref", {companyId: $companyid,perm:perm},

				       function(response){
                                       $('a[id^="sel"]').removeClass('team_active');
                                       thisObj.addClass('team_active');
                                       $("#team_stat_title").html("Active");
                                       $("#team_stat_list").html($(response).fadeIn('slow'));
                                       var ltdh = 0;
                                       $("td.left-td .left-td-wrapper").children().each(function(){
                                           ltdh = ltdh + $(this).outerHeight();
                                       });
                                       var mtdh = $("td.act-content").height();
                                       if(ltdh+260 >= mtdh){
                                         $("td.left-td .left-td-wrapper").css('padding-bottom',260);
                                       }
                                       if($("#coll_req").length > 0){
                                        //$("#coll_req").parent().css("margin-top",260);
                                      } else {
                                        $("td.left-td .left-td-wrapper").css('padding-bottom',340);
                                        //$(".whoweare").css("margin-top",340);
                                      }


                                      $('.team1').insertAfter(thisObj.parent());
                                      $('.team1').show();
                                       });

        });
         $("#selPar").livequery("click", function(){

        	var thisObj = $(this);

        	if(thisObj.hasClass('team_active')) {
                thisObj.removeClass('team_active');
                $('.team1').hide();
                return true;
            }

                var perm = "$perm";

                $.post("/themes/maennaco/includes/posts.php?type=teamStats&ctype=partner$ref", {companyId: $companyid,perm:perm},

        				       function(response){
                                               $('a[id^="sel"]').removeClass('team_active');
                                               thisObj.addClass('team_active');
                                               $("#team_stat_title").html("Active");
                                               $("#team_stat_list").html($(response).fadeIn('slow'));
                                               var ltdh = 0;
                                               $("td.left-td .left-td-wrapper").children().each(function(){
                                                   ltdh = ltdh + $(this).outerHeight();
                                               });
                                               var mtdh = $("td.act-content").height();
                                               if(ltdh+260 >= mtdh){
                                                 $("td.left-td .left-td-wrapper").css('padding-bottom',260);
                                               }
                                               if($("#coll_req").length > 0){
                                                //$("#coll_req").parent().css("margin-top",260);
                                              } else {
                                                $("td.left-td .left-td-wrapper").css('padding-bottom',340);
                                                //$(".whoweare").css("margin-top",340);
                                              }


                                              $('.team1').insertAfter(thisObj.parent());
                                              $('.team1').show();
                                               });

                });

                $("#selConn").livequery("click", function(){

	var thisObj = $(this);

	if(thisObj.hasClass('team_active')) {
        thisObj.removeClass('team_active');
        $('.team1').hide();
        return true;
    }

        var perm = "$perm";
        $.post("/themes/maennaco/includes/posts.php?type=teamStats&ctype=connections$ref", {companyId: $companyid,perm:perm},

				       function(response){
                                       $('a[id^="sel"]').removeClass('team_active');
                                       thisObj.addClass('team_active');
                                       $("#team_stat_title").html("Connected");
                                       $("#team_stat_list").html($(response).fadeIn('slow'));
                                        var ltdh = 0;
                                        $("td.left-td .left-td-wrapper").children().each(function(){
                                            ltdh = ltdh + $(this).outerHeight();
                                        });
                                       var mtdh = $("td.act-content").height();
                                       if(ltdh+260 >= mtdh){
                                         $("td.left-td .left-td-wrapper").css('padding-bottom',260);
                                       }

                                       if($("#coll_req").length > 0){
                                        //$("#coll_req").parent().css("margin-top",260);
                                      } else {
                                        $("td.left-td .left-td-wrapper").css('padding-bottom',340);
                                        //$(".whoweare").css("margin-top",340);
                                      }

                                      $('.team1').insertAfter(thisObj.parent());
                                      $('.team1').show();
                                       });

        });

                $("#selAct").livequery("click", function(){

	var thisObj = $(this);

	if(thisObj.hasClass('team_active')) {
        thisObj.removeClass('team_active');
        $('.team1').hide();
        return true;
    }

        var perm = "$perm";
        $.post("/themes/maennaco/includes/posts.php?type=teamStats&ctype=following$ref", {companyId: $companyid,perm:perm},

				       function(response){
                                       $('a[id^="sel"]').removeClass('team_active');
                                       thisObj.addClass('team_active');
                                       $("#team_stat_title").html("Management");
                                       $("#team_stat_list").html($(response).show());
                                        var ltdh = 0;
                                        $("td.left-td .left-td-wrapper").children().each(function(){
                                            ltdh = ltdh + $(this).outerHeight();
                                        });
                                       var mtdh = $("td.act-content").height();
                                       if(ltdh+260 >= mtdh){
                                         $("td.left-td .left-td-wrapper").css('padding-bottom',260);
                                       }

                                       if($("#coll_req").length > 0){
                                          //$("#coll_req").parent().css("margin-top",260);
                                        } else {
                                          $("td.left-td .left-td-wrapper").css('padding-bottom',340);
                                          //$(".whoweare").css("margin-top",340);
                                        }


                                        $('.team1').insertAfter(thisObj.parent());
                                        $('.team1').show();
                                       });

        });



        });
        </script>

		<div id="advisor">
		<div class="adtitle people"><span data-tooltip="This section represents professional resources Clewed has screened and connected to you. Click the question icon to read how it works.">$content_title</span>$quest_mark</div><br/>
		<div class="adcoments" style="padding: 4px 0px 4px 20px;"><a id="selAct" style="cursor:pointer" oldhref="$redirect&section=connections&ctype=following" >Project Owners <span>$box3_cnt</span> </a></div>
		<div class="adcoments partners"><a id="selPar" style="cursor:pointer" oldhref="$redirect&section=connections&ctype=partners">Introductions <span>$box0_cnt</span></a> </div>
		<div class="adcoments proinvestors"><a id="selInvestor" style="cursor:pointer" oldhref="$redirect&section=connections&ctype=investor">Investors <span>$box4_cnt</span></a> </div>
		<div class="adcoments advisors"><a id="selAdv" style="cursor:pointer" oldhref="$redirect&section=connections&ctype=advisor">Advisors <span>$box1_cnt</span></a> </div>
				<div class="adcoments" ><a id="selColl" style="width:120px;cursor:pointer" class='$collaborator_active1' href="$redirect&ptype=collaborators">Interested<span class="coll_num">$coll_cnt</span></a> </div>
		<div class="adcoments" style=""><a id="selConn" style="cursor:pointer"  oldhref="$redirect&section=connections&ctype=connections" >Connectors <span>$box2_cnt</span></a> </div>
		


	</div><br/>


		<div id="get-help-dialog" style="display:none">
		  <div style="width:530px;padding:20px;">
		  <p>Your team includes verified colleagues you invite, and advisors approved to help with services you buy, connectors who introduce you to opportunities, and professionals interested to help and have the track record related to your project or opportunity. All members are vetted for their talents generally and for their track record in your project specifically.  Clewed manages your connections to ensure quality and save time for all parties. Only people connected to you here have access to your information organized in tabs as follows:</p>

		  <table style="font-family: LatoRegular !important;">
		    <thead style="text-decoration: underline; font-weight: bold;">
		        <tr>
		            <td width="150">Connections</td><td>Information and tabs</td>
		        </tr>
		    </thead>
		    <tbody>
		    <tr>
	            <td>Project Owners</td><td>About, Vetting, Diligence, Analysis, Monitoring, Data Room, Services for topics they help with.</td>
	        </tr>
		     <tr>
	            <td>Advisors</td><td>About, Vetting, Services for topics they participate. They may also access Analysis and Monitoring pages as needed</td>
	        </tr>
	        <tr>
	            <td>Interested</td><td>About, Vetting, for messages they exchanged with you or Clewed</td>
	        </tr>
	        <tr>
	            <td>Connectors</td><td>About, Vetting, Analysis as needed</td>
	        </tr>
	        
	        <tr>
	            <td>Following</td><td>About</td>
	        </tr>
		    </tbody>
		  </table>


		  <p>Note: Your connections can access or add information in your Data Room tab related to the above topics for items they need to know to provide their services.</p>
          </div>
        </div>

        <div class="team_stat_list team1" style="display: none;">
            <div class="list" id="team_stat_list">
                $box_content
            </div>
        </div>

END;
    }
    $Block['body'] = sidebar_box('', $content);
    return $Block;
}

function our_clients($op = null) {
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "Our Clients";
    $tab = sget($_REQUEST, 'tab');
    $panel = "oc_panel";
    $data_type = 'ourclients';
    global $redirect;
    ///
    $content = '';
    if ($op) {
        $sql = "SELECT * FROM maenna_company_data WHERE companyid = %d AND data_type = '%s'";
        $result = db_query($sql, array($companyid, $data_type));
        $Row = db_fetch_object($result);
        if ($Row) {
            $content = strip_tags($Row->data_value2);
            // $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
//            $content = "<div class='limitwidth'>" . sideBarExcerpt($content, $redirect . "&panel=${panel}&view=detail") ."</div>";
            $content = "<div class='limitwidth'>" . sideBarMonitoringExcerpt($content, $redirect . "&panel=${panel}&view=detail") . "</div>";
        }
        if ($op == 'write') $Block['title'] .= "<div class='editbtn'><a href='${redirect}&panel=${panel}&view=edit&section=" . __FUNCTION__ . "' class='tool'>Edit</a></div>";
    }
    $Block['body'] = sidebar_box($Block['title'], $content, 'sidebar_info');
    return $Block;
}

function oc_panel($op = null) {
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Our Clients";
    $panel = "oc_panel";
    $data_type = 'ourclients';
    global $redirect;
    $dataid = '';
    $content = '';
    $text = '';
    if ($op && $op != 'update') {
        $view = sget($_REQUEST, 'view');
        $sql = "SELECT * FROM maenna_company_data WHERE companyid = %d AND data_type = '%s'";
        $result = db_query($sql, array($companyid, $data_type));
        $Row = db_fetch_object($result);
        if ($Row) {
            $text = $Row->data_value2;
            $dataid = $Row->dataid;
        }
        if ($view == 'edit') {
            $Block['title'] = "Edit - " . $Block['title'];
            $content = "<form method=post action='/account'>
                        <table class='no-border'>
                            <tr>
                                <td><textarea name='$data_type' style='width:98%;height:400px;'>$text</textarea></td>
                            </tr>
                            <tr>
                                <td><input type=submit name=submit value=submit class=button /> <a href='$redirect' class=button>Cancel</a></td>
                            </tr>
                        </table>
                            <input type=hidden name=dataid value='$dataid' />
                            <input type=hidden name=update_section value='$panel' />
                            <input type=hidden name=view value='detail' />
                        ";
            $content .= hidden_post_values(array('tab', 'page', 'mtab', 'id', 'panel', 'section'));
            $content .= "</form>";
        } elseif ($view == 'detail') {
            $text = nl2br(strip_tags($text));
            // $text = nl2br(htmlentities($text, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $content = "<div class=entry><div class=entry-content>$text</div></div><div class=backbtn><a href='$redirect' class='button'>back</a></div>";
        }
    } elseif ($op == 'update') {
        $data_value2 = sget($_REQUEST, $data_type);
        $companyid = sget($_REQUEST, 'id');
        $dataid = sget($_REQUEST, 'dataid');
        if (empty($companyid)) {
            drupal_set_message("Invalid company id", 'error');
        } else {
            if ($dataid) {
                $sql = "UPDATE maenna_company_data SET data_value2 = '%s', access='%s',editorid=%d WHERE data_type = '%s' AND dataid = %d LIMIT 1";
                $DBValues = array($data_value2, $time, $editorid, $data_type, $dataid);
            } else {
                $sql = "INSERT INTO maenna_company_data (companyid, access, data_type, data_value2,editorid ) VALUES (%d, '%s','%s','%s',%d)";
                $DBValues = array($companyid, $time, $data_type, $data_value2, $editorid);
            }
            if (db_query($sql, $DBValues)) {
                drupal_set_message("Data record is updated");
            } else drupal_set_message("Failed to update record", 'error');
        }
        return;
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function pro_project_box($op) {
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
    global $AccessObj;
    $user_id = sget($_REQUEST,'userid');
    $id = $editorid = $AccessObj->uid;
    if($user_id) $id = $user_id;
    $time = time();
    $cmp_id = sget($_REQUEST, 'id');
    $Block['title'] = '';
    $data_type = '';
    $redirect = rebuild_url(array('tab', 'page', 'id'));
    $content = '';
    $Conns = Connections::Pro_conns($editorid);
    //hide a pro's teambox
    //test_array($AccessObj);
    //test_array($Conns);
    if ($AccessObj->user_type == 'company') return '';
    if ($op && ($op != 'update')) {
        $coll_companies = getCollaborationCompanies($id);
        $foll_companies = getFollowers($id);
        $relPros = getRelated($id);
        //test_array($relPros);
        $cnt_box_0 = count($Conns['Partner']);
        $cnt_box_1 = count($Conns['Advisor']);
        $cnt_box_2 = count($Conns['Visible']);
        $cnt_box_3 = count($coll_companies['active']);
        $cnt_box_4 = count($foll_companies);
        $cnt_box_5 = count($relPros);
        $cnt_box_6 = count($Conns['Client']);
        $active_box_1 = $active_box_2 = $active_box_3 = $active_box_4 = $active_box_5 = $active_box_6 = '';
        $ctype = sget($_REQUEST, 'ctype');
        $ptype = sget($_REQUEST, 'ptype');
        $LIST = '';
        $box_content = '';
        if ($ctype != '') {
            $box_content = '';
            $box_title = '';
            $style = 'style=display:block;';
            $style2 = 'style=display:none;';
        } elseif ($ptype != '') {
            $box2_content = '';
            $box2_title = '';
            $style2 = 'style=display:block;';
            $style = 'style=display:none;';
        } else {
            $box_content = '';
            $box_title = '';
            $box2_content = '';
            $box2_title = '';
            $style = 'style=display:none;';
            $style2 = 'style=display:none;';
        }
        if ($ctype != 'projects') {
            $active_box_1 = '';
        } else {
            $active_box_1 = 'activebox';
        }
        if ($ptype != 'followers') {
            $active_box_4 = '';
        } else {
            $active_box_4 = 'activebox';
        }
        // {
        // $active_box_3 = 'active';
        // $box_title = "SELECTIONS";
        // foreach($Conns['Follow'] as $Proj)
        // {
        // $target_uid = $Proj->target_uid;
//
        // //Get cmp_role
        // $q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $target_uid") or die(mysql_error());
        // $cmp_role_tmp = mysql_fetch_array($q1);
        // $cmp_role = $cmp_role_tmp['company_type'];
        // //Check if user have a profile picture
        // if (file_exists('sites/default/images/company/50x50/'.$target_uid.'.jpg')) {$avatar = 'sites/default/images/company/50x50/'.$target_uid.'.jpg';}
        // else
        // if ($cmp_role == 'service') $avatar =' /themes/maennaco/images/cmp-avatar-service.png';
        // else $avatar =' /themes/maennaco/images/cmp-avatar-product.png';
//
        // ;
//
        // $maenna_id = getProjectName($target_uid);
        // $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src=".$avatar." style=\"float:left; margin-right:5px; width:50px; height:50px;\"><a style=\"margin-top:20px;\" href='/account?tab=companies&page=company_detail&id=$target_uid&closebtn=1' >$maenna_id</a></div>";
        // }
        // }
        $management_div = "";
        $msql = "SELECT * FROM maenna_people WHERE pid = %d";
        $mresult = db_query($msql, $_REQUEST['id']);
        $mRow = db_fetch_object($mresult);
        if ($mRow && $mRow->protype) {
            $protype = $mRow->protype;
        }
        //if ($protype == 'client') {
            $management_div = <<< END
              <div class="adcoments"><a id="proselManagement" class='$active_box_6' style="width:120px;cursor:pointer" href="$redirect&section=connections&ctype=management">Own projects <span>$cnt_box_6</span></a> </div>
END;
        //}
        
        $content = <<< END
        <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
        <script type="text/javascript">
        	$(document).ready(function(){

                $('#get-help-btn').click(function(e) {
                    e.preventDefault();
                  $("#get-help-dialog").dialog({
                    modal: true,
                    autoOpen: true,
                    resizable: false,
                    title: 'Your connections',
                    width: 560,
                  });
                });

    $("a[id^='prosel']").livequery("click", function(event){

        event.preventDefault();
        var thisObj = $(this);

        if(thisObj.hasClass('team_active')) {
            thisObj.removeClass('team_active');
            $('.team1').hide();
            return true;
        }

        type = $(this).attr('id').substr(6);

        $.post("/themes/maennaco/includes/fetch.php?type=get"+type, {proId: $id},

            function(response){

                console.log(thisObj.parent());

                $('a[id^="prosel"]').removeClass('team_active');
                thisObj.addClass('team_active');

                if(!$(".team1").length) {

              thisObj.parent().parent().parent().append("<div class='team_stat_list team1' style='display:none;'><div id='team_stat_list' class='list'></div></div>");

                }

                $("#team_stat_title").html(type);
                $("#team_stat_list").html($(response).show());



                $('.team1').insertAfter(thisObj.parent());
                $('.team1').show();

            });

    });
    });
    </script>

		<div id="advisor">
        <div class="adtitle"><span data-tooltip="Your connections information is private visible only to you. Click on the list below to access details about each of your connections. For easy access to the latest activity related to your connections, refer to your notifications on top right menu.">My Project</span><span id="questmark" style="float: right"><a href="#" id="get-help-btn"><img src="/themes/maennaco/images/questionmark.png" style="padding-top:2px;"></a></span></div><br/>
		$management_div
            <div class="adcoments"><a id="proselPartners" class='$active_box_1' style="width:120px;cursor:pointer" href="$redirect&section=connections&ctype=projects">Introductions <span>$cnt_box_0</span></a> </div>
		<div class="adcoments"><a id="proselProjects" class='$active_box_1' style="width:120px;cursor:pointer" href="$redirect&section=connections&ctype=projects">Advising <span>$cnt_box_1</span></a> </div>
				<div class="adcoments"><a id="proselCollaborating" class='$active_box_3' style="width:120px;cursor:pointer" href="$redirect&section=connections&ctype=collaborating">Interested <span>$cnt_box_3</span></a> </div>
		<div class="adcoments"><a id="proselConnected" class='$active_box_2' style="width:120px;cursor:pointer" href="$redirect&section=connections&ctype=connected" >Connector <span>$cnt_box_2</span></a> </div>
		<div class="adcoments"><a id="proselFollowers" style="width:120px;cursor:pointer" class='$active_box_4' href="$redirect&section=connections&ptype=followers">Followers <span>$cnt_box_4</span></a> </div>

	</div><br/>


	<div id="get-help-dialog" style="display:none">
	  <div style="width:500px;padding:20px;">
	  <p>Clewed connects professionals to projects and opportunities through the Connections section. This section gives you access to information based on your connection level as follows:</p>

	  <table style="font-family: LatoRegular !important;">
	    <thead style="text-decoration: underline; font-weight: bold;">
	        <tr>
	            <td width="150">Connection</td><td>Available Information</td>
	        </tr>
	    </thead>
	    <tbody>
        <tr>
            <td>My Company</td><td>Access all tabs for projects and opportunities you bring.</td>
        </tr>
        <tr>
            <td>Introductions</td><td>About, Due Diligence, Vetting, Analysis, Data Room, Services, Monitoring tabs on an as needed basis</td>
        </tr>
        <tr>
            <td>Advising</td><td>About, Vetting, Services for basic clients; Analysis and Monitoring tabs as applicable</td>
        </tr>
        <tr>
            <td>Interested</td><td>About, Vetting</td>
        </tr>
        <tr>
            <td>Connector</td><td>About, Vetting, Analysis, Data Room as applicable</td>
        </tr>
        <tr>
            <td>Followers</td><td>About, Profiles</td>
        </tr>
                <tr>
            <td>Following</td><td>About, Profiles</td>
        </tr>
	    </tbody>
	  </table>

	  <p>Note: All company connections are initiated by Clewed based on client needs except ?Interested?. You can proactively build relationships with companies by requesting to join their Interested team.</p>


	  </div>

    </div>
END;
    }
    $Block['body'] = sidebar_box('', $content);
    return $Block;
}


/**
 * @param null $op
 * @return mixed
 * **** MANAGEMENT DISCUSSION *****
 */
function management_discussion($op = null){

    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Management Interviews";
    $data_type = 'management_discussion';
    $panel = "audio_upload";
    global $redirect;
    $content = '';
    $section = __FUNCTION__;
    $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
    $access = access_information_tables($companyid,'management_discussion');

    if ($access['write']){
        $Block['title'] .= "<div class='editbtn text-uppercase'><a href='$redirect&panel=${panel}&section=${section}&view=add' class=tool>Add</a></div>";
    }

    if ($access['read']){
        $db = \Clewed\Db::get_instance();

        $audios = $db->get_array(
            "SELECT * FROM discussion_audio as da where da.company_id = :company",
            array(':company' => $companyid)
        );


        if (count($audios) > 0){
            foreach ($audios as $audio){
                $content .= discussion_audio_item($audio['name']);
            }
        }
    }


    if ($content != ''){
        $viewall_link = $redirect . "&panel=${panel}&section=${section}&view=detail";

        $content .= "
                <div class='more-btn-content'>
                     <div class='viewbtn'>
                     <a href='$viewall_link' class=tool>More</a>
                     </div>
                </div>";

        $Block['body'] = sidebar_box($Block['title'], $content, 'management_discussion');
    }

    return $Block;
}

/**
 * @param null $op
 * @return mixed
 */
function audio_upload($op = null){

    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $tab = sget($_REQUEST, 'tab');
    $page = sget($_REQUEST, 'page');
    $panel = "audio_upload";
    //$data_type = sget($_REQUEST, 'datatype');
    global $redirect;
    $section = sget($_REQUEST, 'section');
    $access = access_information_tables($companyid,'management_discussion');

    $queries = [
        'tab' => $tab,
        'page' => $page,
        'id' => $companyid
    ];

    $detailQuery = [
        'section' => $section,
        'panel' => $panel,
        'view' => 'detail'
    ];

    $url = http_build_query($queries, '', '&');
    $detailUrl = http_build_query(array_merge($queries,$detailQuery), '', '&');

    $content = '';
    $HV =  hidden_post_values(array('tab', 'page', 'mtab', 'id', 'section', 'panel'));
    $Block['title'] = "MANAGEMENT INTERVIEWS";
    $view = sget($_REQUEST, 'view');
    $db = \Clewed\Db::get_instance();

    if ($op && $op !== 'update') {

        if ( $access['write'] && $view == 'add'){

            $content = '
            <form action="/account" method="post">
                <div class="interview-audio-preview-uploader">       
                    <noscript>
                        <p>Please enable JavaScript to use file uploader.</p>
                    </noscript>
                    <script type="text/javascript">
                        (function() {
                            var $uploader = $(".interview-audio-preview-uploader");
                            
                            if($uploader.length > 0) {
                                new qq.FileUploader({
                                    params: {
                                        name: '.$companyid.' ,
                                        itype: "discussion_audio",                 
                                    },
                                    label: `<span class="uploader-btn"><img src="./themes/maennaco/images/project/upload.png" >Upload mp3 file</span>`,
                                    multiple: false,
                                    element: $uploader[0],
                                    action: "/themes/maennaco/includes/file_upload.php",
                                    allowedExtensions: ["mp3"],
                                    onComplete: function(id, fileName, responseJSON){      
                                        console.log(responseJSON)
                                        if(responseJSON.success) {    
                                            $uploader.parent().find("#file_name").val(responseJSON.name)
                                            
                                            let infoList = $uploader.parent().find(".qq-upload-success"); 
                                            if(infoList.length >1){
                                                infoList.first().remove();                               
                                            }      
                                            if(infoList.length > 0){
                                                $(".qq-upload-file").html(responseJSON.name);
                                            }
                                        }
                                    }, 
                                    onSubmit: function(id, fileName){},
                                    fileTemplate: `<li>
                                    <span class="qq-upload-file"></span>
                                    <span class="qq-upload-spinner"></span>
                                    <span class="qq-upload-size"></span><a class="qq-upload-remove"></span>
                                    &nbsp;<a class="qq-upload-cancel" href="#"></a>
                                    <span class="qq-upload-failed-text">Failed</span>
                                    </li>`
                                });
                            }
                        }());
                    </script>
                </div>
                '.$HV.'
                <input type="hidden" name="file_name" id="file_name"/>
                <input type="hidden" name="view" value="detail"/>
                <input type="hidden" name="do" value="insert" />
                <input type="hidden" name=update_section value="'.$panel.'" />
                <div class="audio-buttons-block" >
                    <input type="submit" name="submit" value="Submit"  class="button" /> 
                    <a href="'.$redirect.'" class="button">Cancel</a>
                </div>
            </form>
            ';

        }
        elseif ($view = 'detail' && $access['read']){

            if ($access['write']){
                $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&section=${section}&view=add' class=tool>Add</a></div>";
            }

            $db = \Clewed\Db::get_instance();

            $audios = $db->get_array(
                "SELECT * FROM discussion_audio as da where da.company_id = :company",
                array(':company' => $companyid)
            );

            if (count($audios) > 0){
                foreach ($audios as $audio){
                    $content .= discussion_audio_item($audio['name'],$op ,
                        [
                            'company_id' => $audio['company_id'] ,
                            'id' => $audio['id'],
                            'access'=> $access['write']
                        ]);
                }

                if($content != '' && $access['write']){
                    $content .= '       
                   <script type="text/javascript">
                        $(document).ready(function(){
                            $(".audio-preview-remove").click(function(el) {
                             
                                var $el = $(this),
                                    id = $el.data("id"),
                                    m = $el.data("m"),
                                    companyId = $el.data("companyId"),
                                    file_name = $el.data("name");
                                    
                                if(confirm("Are you sure you want to delete the audio preview file ?"))
                                    $.ajax({
                                        type: "get",
                                        url: "./themes/maennaco/includes/delete.php?type=discussion-audio-preview&id=" + id + "&m=" + m+ "&companyId=" + companyId + "&file_name="+file_name,
                                        success: function (msg) {
                                            if(msg === "success") {
                                                $el.parent(".audio-preview").remove();
                                            }
                                        }
                                    });

                            });
                        });
                    </script>
                    ';
                }
            }


        }
        else{
            drupal_goto('account', $url);
        }

    }
    elseif ($op && $op == 'update'){

        $do = sget($_REQUEST, 'do');
        if ($do == "insert" ){
            $file_name = sget($_REQUEST, 'file_name');

            if ($file_name && $companyid){
                $insertAudio = $db->run('
                        INSERT INTO `discussion_audio` (`name`, `company_id`,`created`)
                        VALUES (:fileName,:company,:created)',
                    array(
                        ':fileName' => $file_name,
                        ':company' => $companyid,
                        ':created' => $time
                    )
                );
            }

            if ($insertAudio) drupal_set_message("Operation Successful");
            else drupal_set_message("Operation failed", 'error');

            drupal_goto('account', $detailUrl);

        }
    }
    else{
        drupal_goto('account', $url);
    }

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;

}

/**
 * @param null $op
 * @return mixed
 ****** HIGH LIGHTS *******
*/
function highlights($op = null){

    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Highlights";
    $Block['count'] = 0;
    $data_type = 'highlights';
    $panel = "two_columns";
    global $redirect;
    $content = '';
    $section = __FUNCTION__;

        $db = \Clewed\Db::get_instance();

        $highlights = $db->get_array(
            "SELECT cd.dataid as id , cd.data_value as title  FROM maenna_company_data as cd where cd.data_type = :data_type and cd.companyid = :companyid ",
            array(
                ':data_type' => 'highlights',
                ':companyid' => $companyid,
            )
        );

        $count = count($highlights);
        if ( $count > 0){
            $Block['count'] = $count;
            $highlights_id = sget($_REQUEST,'highlights');
            if (!$highlights_id && !sget($_REQUEST, 'panel')){
                $highlights_id = $highlights[0]['id'];
            }

            $content = ' <div class="tab">';
            foreach ($highlights as $key => $highlight){
                $active = "";
                if ($key==0){
                    $active="active";
                }
                $content .= '<button class="tablinks '. $active.'" 
                            onclick="changeTab(event, '.("tab_".$highlight['id']).')">
                                <div class="title">'.$highlight["title"].'</div>
                             </button>';
            }
            $content .= '</div>';
        }

    $Block['body'] = sidebar_box($Block['title'], $content, 'highlights_nav');

    return $Block;
}

/**
 * @param null $op
 * @return mixed
 */
function two_columns($op = null){
    global $user, $AccessObj;

    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $tab = sget($_REQUEST, 'tab');
    $page = sget($_REQUEST, 'page');
    $panel = "two_columns";
    $data_type = sget($_REQUEST, 'datatype');
    $data_id = sget($_REQUEST,'data_id');
    global $redirect;
    $db_request = null;
    $section = sget($_REQUEST, 'section');
    $queries = [
        'tab' => $tab,
        'page' => $page,
        'id' => $companyid,
    ];
    $listQuery = [
        'view' => 'list',
        'panel' => $panel,
        'section' => $section,
        'datatype' => $data_type,
    ];

    $content = '';
    $hv      = hidden_post_values(array('tab', 'page', 'section','panel','id', 'mtab'));

    $Block['title'] = "Add Highlights";

    $view = sget($_REQUEST, 'view');
    $db = \Clewed\Db::get_instance();
    $url = http_build_query($queries, '', '&');
    $listURL =  http_build_query(array_merge($queries,$listQuery), '', '&');

    if ($op && $op !== 'update') {

        if ($op == 'write' && ($view == 'add' || $view == 'edit') ){

            $Block['title'] = "Add Highlights";
            $action = 'insert';
            $data = [];
            if ($view == 'edit' && isset($data_id) && !empty($data_id)){
                $data = $db->get_row(
                    'select cd.dataid as id , cd.data_value as title , cd.data_value2 as content  from maenna_company_data as cd
                            where cd.companyid = :companyid and cd.data_type = :data_type  and  cd.dataid = :dataid',
                    array(
                       ':companyid'=>$companyid,
                       ':data_type'=>$data_type,
                       ':dataid'=>$data_id,
                    )
                );
                //if exist data row
                if ($data){
                    $Block['title'] = "Edit High Lights";
                    $action = 'edit';

                }else{
                    drupal_goto('account',$url);
                }

            }

            $content .= "
                      <form action='/account' method='post' onsubmit='form_validation(event)'>
                        <div class=rt-block1>
                            <div class=rt-title>      
                                <div>Title:<br />
                                    <input  placeholder='' type=text name=title value='".$data['title']."' maxlength='200' class='require_string'/>
                                </div>
                                <div>Content:<br />
                                    <label for='tinymce' class='textarea-label'> </label>
                                    <textarea name='content' style='width:99%;height:300px;' class='require_string' >".$data['content']."</textarea>
                                </div>
 
                                <div class=backbtn>
                                <div>
                                    <input type='submit' name='submit' value='Submit'  class='button' />                              
                                    <a href='/account?$listURL' class='button'>Cancel</a>
                                </div>
                            </div>
                        </div>
                        $hv
                        <input type='hidden' name=data_id value='".$data['id']."' />
                        <input type='hidden' name=datatype value='$data_type' />
                        <input type='hidden' name=do value='$action' />
                        <input type='hidden' name=update_section value='$panel' />
                    </form>";
        }
        elseif ($view == 'list'){
            $Block['title'] = "List Highlights";
            if ($op == 'write'){
                $Block['title'] .= "<div class='editbtn text-uppercase'><a href='$redirect&panel=${panel}&section=${section}&view=add&datatype=$data_type' class=tool>Add</a></div>";
            }

            $highlights = $db->get_array(
                "SELECT cd.dataid as id , cd.data_value as title , cd.data_value2 as content   FROM maenna_company_data as cd where cd.data_type = :data_type and cd.companyid = :companyid limit 10",
                array(
                    ':data_type' => 'highlights',
                    ':companyid' => $companyid,
                )
            );

            $count = count($highlights);

            if ($count > 0){
                $content .='<div class="custom-accordion-content">';
                foreach ($highlights as $highlight){

                    $content .='
                            <button class="accordion">'.$highlight["title"];

                    if ($op == 'write'){
                       $content .= '
                                <div class="accordion-actions-box">                       
                                    <a href="/account?tab=companies&page=company_detail&mtab=about&id='.$companyid.'&panel=two_columns&section=highlights&view=edit&datatype=highlights&data_id='.$highlight['id'].'">Edit</a>
                                    <a href="/account?tab=companies&page=company_detail&mtab=about&id='.$companyid.'&panel=two_columns&section=highlights&view=delete&datatype=highlights&data_id='.$highlight['id'].'">Remove</a>
                                </div>';
                    }
                    $content.='</button>
                            <div class="panel">
                              <p>'.$highlight["content"].'</p>
                            </div>
                    ';
                }
                $content .='</div>';
            }

        }
        elseif ($op == 'write' && $view == 'delete' && ($AccessObj->user_type =='super' || $editorid == $companyid)){

            $db_request = $db->run(
                'delete FROM maenna_company_data  where dataid = :dataid and data_type = :data_type and companyid = :companyid',
                array(
                    ':data_type' => 'highlights',
                    ':companyid' => $companyid,
                    ':dataid' => $data_id,
                )
            );

            if ($db_request){
                drupal_set_message("Operation Successful");
            }else{
                drupal_set_message("Operation failed", 'error');
            }

            drupal_goto('account', $listURL);
        }
        else{
            drupal_goto('account',$url);
        }

    }
    elseif ($op && $op == 'update'){

        $do = sget($_REQUEST, 'do');
        $data_type = sget($_REQUEST, 'datatype');
        $title     = sget($_REQUEST, 'title');
        $content   = sget($_REQUEST, 'content');
        $data_id   = sget($_REQUEST, 'data_id');

        $DBValues = array($companyid,$time,$data_type,$title,$content,$editorid,1);

        if ($data_type && $title && $content){


                if ($do == "insert" ){

                    $db_request = $db->run('
                            INSERT INTO `maenna_company_data` (companyid,access,data_type,data_value,data_value2,editorid,status)
                            VALUES (?,?,?,?,?,?,?)',
                            $DBValues
                    );
                }
                elseif ($do == "edit" && $data_id ){

                    $db_request = $db->run('
                            UPDATE maenna_company_data SET access = :access,data_value = :data_value ,data_value2 = :data_value2 ,editorid = :editorid 
                            WHERE dataid = :data_id',
                        array(
                            ':data_id'=>$data_id,
                            ':access'=>$time,
                            ':data_value'=>$title,
                            ':data_value2'=>$content,
                            ':editorid'=>$editorid
                        )
                    );

                }

                if ($db_request){
                    drupal_set_message("Operation Successful");
                }else{
                    drupal_set_message("Operation failed", 'error');
                }





                drupal_goto('account', $listURL);

        }

    }

    $Block['body'] = content_box($Block['title'], $content,$panel);
    return $Block;

}


/* EOF */
