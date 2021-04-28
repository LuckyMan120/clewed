<?php
global $redirect;
$redirect = rebuild_url(array('tab'));
function pro_list($op = null) {
    global $AccessObj;
    global $user;
    global $redirect;
    $_Experties = _experties();
    //$Block['title'] = 'Professionals';
    if ($op) {
        if (in_array($AccessObj->user_type, array('admin', 'super', 'sales'))) {
            $content = get_maenna_tabs($AccessObj->user_type);
            $content .= get_maenna_people($AccessObj->user_type);
        } elseif ($AccessObj->user_type == 'company' ||
            ($AccessObj->user_type == 'people'
                && isset($_REQUEST['id']) && isset($_REQUEST['m'])
                && $_REQUEST['m'] == md5($_REQUEST['id'] . "kyarata75")
            )
        ) {
            $detail_array = array('proid' => array('PRO ID', 'tab_a', 300), 'rid' => array('TYPE', 'tab_a', 150), 'industry' => array('INDUSTRY', 'tab_a', 200), 'experties' => array('EXPERTIES', 'tab_a', 300), 'status' => array('STATUS', 'tab_a', 250),);
            if ($_REQUEST['id'] && $AccessObj->user_type == 'people') {
                $ins_topic = getInsightTopic($_REQUEST['id']);
            }
            $sql_total = "SELECT count(*) AS total  FROM  users,maenna_people
                    WHERE users.uid = maenna_people.pid AND users.status = 1 AND public = 1";
            $result_total = db_query($sql_total);
            $m = db_fetch_object($result_total);
            if ($m->total <= 0) {
                $Block['body'] = "No visible professionals";
                return $Block;
            }
            $start = 0;
            $limit = 6;
            $SearchKeys = array('letter', 'name', 'proid', 'industry', 'experties', 'state', 'proTypeSel', 'managementLevel', 'plres', 'rating', 'eventFrom', 'eventTo', 'managerid',);
            $filter_cnt = 0;
            foreach ($SearchKeys as $key) {
                if (sget($_REQUEST, $key) === '') {
                    continue;
                }
                $filter_cnt++;
            }
            if (sget($_REQUEST, '_page')) {
                if ($filter_cnt > sget($_REQUEST, 'filterCnt')) {
                    $start = 0;
                } else $start = (sget($_REQUEST, '_page') - 1) * $limit;
            }
            $sql = "SELECT  users.created AS register,
                    users.uid,
                    maenna_people.*,
                    users_roles.rid,
                    mpd.expertise,
                    mc.target_uid as connected_company_id,
                    mcomp.projname
            FROM    users,
                    maenna_people LEFT JOIN (SELECT pid,data_value2 AS expertise FROM maenna_people_data mp1d WHERE data_type='addinfo' AND data_attr = 'experties' AND dataid = (SELECT dataid FROM maenna_people_data WHERE data_type='addinfo' AND data_attr = 'experties' AND pid = mp1d.pid ORDER BY dataid DESC LIMIT 1)

                    ) mpd ON maenna_people.pid = mpd.pid
                    LEFT JOIN  maenna_connections mc ON mc.connid = (SELECT connid FROM maenna_connections WHERE conntype = 'client' and status = 'active' and assignee_uid = mpd.pid LIMIT 1)
                    LEFT JOIN  maenna_company mcomp ON mcomp.companyid = mc.target_uid
                    ,
                    users_roles
                    WHERE users.uid = maenna_people.pid AND users.uid = users_roles.uid AND users.status = 1 AND maenna_people.public = 1
                   ";
            //die($sql);
            if ($_REQUEST['expertise'] != '') {
                $expertise = "'" . implode("', '", array_keys($_Experties[$_REQUEST['expertise']])) . "'";
                if ($expertise == "''") $expertise = "'" . $_REQUEST['expertise'] . "'";
                $sql .= " AND (experties IN($expertise)
                            OR experties2 IN($expertise)
                            OR experties3 IN($expertise)
                          )";
            }


            $sql .= "order by users.uid desc";
            if ($_REQUEST['expertise'] == '' && $_REQUEST['type'] == '') $sql .= " limit 6";
            $result = db_query($sql);
            //var_dump($result);
            $_ProType = _pro_roles();
            // numeric index array
            $_Industry = _INDUSTRY();
            $_mLvl = _managementLevel();
            if (!empty($result)) {
                $content = "<div class='pro_follow'>";
                $content .= "<div id='pro_popup' style='display:none;'></div>";
                if ($AccessObj->user_type == 'company') {
                    $content .= '<div id="cardstitle">Follow professionals; discover their Insights</div>';
                } else {
                    $content .= '<div id="cardstitle">Invite moderator for Insight topic ' . $ins_topic . '</div>';
                }
                $i = 1;
                while (($row = db_fetch_array($result)) !== false) {
//                    echo '<pre>'; print_r($row);

                    $pid = sget($row, 'pid');
                    $created = sget($row, 'register');
                    $uid = $user->uid;
                    $followers = getFollowers($pid);
                    $usrType = strtoupper(userType($pid));
                    if ($_REQUEST['type'] != '' && $usrType != strtoupper($_REQUEST['type'])) continue;
                    if ($i++ % 2 != 0) $first = 'first'; else $first = '';
                    $avatar = getAvatarUrl($pid, "150");
                    $row['experties'] = preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $row['experties']);
                    $row['experties2'] = preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $row['experties2']);
                    $row['experties3'] = preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $row['experties3']);
                    //Get education data as it was too complicated to get it in parent query
                    $sql1 = "SELECT data_value,data_value3 FROM maenna_people_data WHERE data_type = '%s' AND pid = %d ORDER BY dataid DESC LIMIT 1";
                    $result1 = db_query($sql1, array('education', $pid));
                    $row1 = db_fetch_object($result1);
                    $row['undergraduate'] = $row1->data_value3;
                    $row['graduate'] = $row1->data_value;
                    if (strlen($row['expertise']) >= 209) {
                        $row['expertise'] = substr(strip_tags($row['expertise']), 0, 209) . "...";
                    } else $row['expertise'] = strip_tags($row['expertise']);
                    if (strlen($row['profile']) >= 110) $row['profile'] = substr(strip_tags($row['profile']), 0, 110) . "...";
                    $row['profile'] = strip_tags($row['profile']);


                    if (!empty($row['projname'])){
                        if (strlen(strtoupper($row['firstname']).', '.$usrType. $row['projname']) > 32){
                            $shortProjName = substr($row['projname'], 0, 32 - strlen($row['firstname'].', '.$usrType)) .'...';
                        } else {
                            $shortProjName = $row['projname'];
                        }
                        $projectLink = ', <a data-tooltip="'.htmlentities($row['projname']).'" style="color:#00A2BF !important;font-family:"Lato Bold Italic";font-size: 14px;cursor: pointer;" href="/account?tab=companies&page=company_detail&id='.$row['connected_company_id'].'&mtab=about">'.$shortProjName.'</a>';
                    } else {
                        $projectLink = '';
                    }

                    $content .= '<div class="pro_card ' . $first . '">

                    <a rel = "' . $pid . '" class="pro_popup" style="cursor:pointer;"><img width="50" height="50" src ="' . $avatar . '">

                    <div style="float:left;width:340px;"><div class="name_title">' . strtoupper($row['firstname']) . ', ' . $usrType. $projectLink . '</div><br style="clear:both;"><div class="pro_summary">' . $row['profile'] . '
                    </div></div>
                      <br style="clear:both">

<hr style="width:97%; background-color:#D0D2D3; margin-left:5px;margin-top:11px;">
<div class="pro_exp">' . ($row['experties'] != '' ? $row['experties'] : '') . ($row['experties2'] != '' ? ", " . $row['experties2'] : '') . ($row['experties3'] != '' ? ", " . $row['experties3'] : '') . '</div><div class="pro_exp">';
                    if ($row['graduate'] != '') {
                        $content .= $row['graduate'];
                    } elseif ($row['undergraduate'] != '') $content .= $row['undergraduate'];
                    $content .= '</div>
<hr style="width:97%; background-color:#D0D2D3; margin-left:5px;margin-top:11px;margin-bottom:11px;">
<div class="pro_summary" style="height:50px;margin-left:20px;">' . $row['expertise'] . '
                    </div></a>';
                    if ($AccessObj->user_type == 'company') {
                        if (in_array($AccessObj->uid, $followers)) {
                            $value = '<a class="follow" title="Unfollow" type="unfollow" cid="' . $uid . '" uid = "' . $pid . '" class="tool">Following</a>';
                        } else {
                            $value = '<a data-tooltip="Be the first to know ' . ucfirst($row['firstname']) . '\'s next Insight"  class="follow" type="follow" cid="' . $uid . '" uid = "' . $pid . '"  class="tool">Follow</a>';
                        }
                        $value .= "<div style='color: #00a2bf !important;font-family: \"Lato Bold Italic\";font-size: 14px;margin-top:22px;margin-left:3px;float:left;'class='foll_cnt'>" . count($followers) . "</div>";
                    } else {
                        $value = '<a class="invite" pid= "' . $_REQUEST['pid'] . '"itopic = "' . $ins_topic . '" uid = "' . $pid . '" pname="' . ucfirst($row['firstname']) . '" type="' . $_REQUEST['edit'] . '" m="' . $_REQUEST['m'] . '" iid="' . $_REQUEST['id'] . '"  class="tool">Invite</a>';
                    }
                    $content .= $value;
                    if (proHasInsights($pid)) {
                        $content .= '
<a class="insights" href="account?tab=professionals&page=pro_detail&id=' . $pid . '&section=pro_industry_view&type=discussion" style="float:right !important;">Insights</a>';
                    }
                    $content .= '</div>';
                    if ($first == '') $content .= '<br style="clear:both;">';
                    /*					foreach ($detail_array as $key => $Column) {

                                            $value = sget($row, $key);
                                            if ($key == 'proid') {
                                                $value = $title = getProId($pid);
                                                $value = ucwords(strtolower($value));
                                                if ($foll_status)$value = "<a href='$redirect&page=pro_detail&id=$pid'>$value</a>";
                                                else $value = "<a style='color:gray;'>$value</a>";
                                                $content .= "\n<td style='padding-left:15px;'><div style='width:$width;overflow:hidden;' title='$title'>$value</div></td>";
                                                continue;
                                            } elseif ($key == 'rid') {
                                                $value = $_ProType["$value"];
                                                $content .= "\n<td><div  style='width:$width;overflow:hidden;'>$value</div></td>";
                                                continue;
                                            } elseif ($key == 'industry') {
                                                if ($value && ($created > '1322005040')) {
                                                    foreach ($_Industry as $SubArray) {
                                                        foreach ($SubArray as $index => $val) {
                                                            if (strcasecmp($index, $value) == 0) {
                                                                $value = $val;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                }
                                                $content .= "<td><div  style='width:$width;overflow:hidden;height:20px;' title='$title'>" . ($value) . "</div></td>";
                                                continue;
                                            } elseif ($key == 'experties') {

                                                if ($value && ($created > '1322005040')) {
                                                    foreach ($_Experties as $SubArray) {
                                                        foreach ($SubArray as $index => $val) {
                                                            if (strcasecmp($index, $value) == 0) {
                                                                $value = $val;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    $value = preg_replace('/\B([A-Z])/', ' $1', $value);
                                                }
                                                $content .= "<td><div  style='width:$width;overflow:hidden;height:20px;'>" . ($value) . "</div></td>";
                                                continue;
                                            } elseif ($key == 'status') {

                                                $txt_align = 'style="text-align:center;"';

                                                if ($foll_status)
                                                    $value = '<a style="color:#00a2bf;" title="UNFOLLOW" type="unfollow" cid="' . $uid . '" uid = "' . $pid . '" class="tool">FOLLOWING</a>';
                                                else
                                                    $value = '<a style="color:#006274;" type="follow" cid="' . $uid . '" uid = "' . $pid . '"  class="tool">FOLLOW</a>';
                                                $content .= "<td><div  style='width:$width;overflow:hidden;height:20px;'>" . ($value) . "</div></td>";
                                                continue;
                                            }

                                        }*/
                }
                if ($_REQUEST['type'] == '' && $_REQUEST['expertise'] == '') {
                    if ($_REQUEST['id'] != '' && isset($_REQUEST['m'])) $invite_data = 'iid="' . $_REQUEST['id'] . '" itopic="' . $ins_topic . '" type="' . $_REQUEST['edit'] . '" pid="' . $_REQUEST['pid'] . '"'; else $invite_data = '';
                    $content .= "<span total='" . $m->total . "' page='2' rel='show' class='show_more_pro' " . $invite_data . ">view more</span>";
                }
                $content .= "</div>";
                $Pagenation = array('total' => $m->total, 'limit' => $limit, 'baseurl' => "/account?", 'num_of_links' => 8,);
                /*				$content .= '<table style="margin:0;width:60%;float:right;" class="no-border"><tr border=0>
                            <td border=0>' .
                            "<div style='width:100%;'>" .
                            pagination($Pagenation) . "</div>" .
                            '</td></tr></table>';*/
            }
            $content .= '
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
<script type="text/javascript">
$(document).ready(function(){

    init_follow();
    init_invite();
    init_sortPanel();

    $(".show_more_pro").livequery("click",function(e){

    e.preventDefault();

    page = $(this).attr("page");
    total = $(this).attr("total");
    iid = $(this).attr("iid");
    itopic = $(this).attr("itopic");
    type = $(this).attr("type");
    pid = $(this).attr("pid");

    if (iid != "" && iid) invite_data = "&iid=" + iid + "&itopic=" + itopic + "&itype=" + type + "&pid="+pid; else invite_data = "";

        $.post("/themes/maennaco/includes/fetch_pro.php?type=pro_follow&page=" + page + "&total="+total + invite_data, {
        }, function(response) {
        $(".show_more_pro").remove();
        $(".pro_follow").append(response);
        });


    });

   $(".pro_popup").livequery("click", function(e) {


        e.preventDefault();
        var pro_id = $(this).attr("rel");
        uid = "' . $user->uid . '";
        bInv = "' . $_REQUEST['id'] . '";
        if (bInv != "") inv = "&inv=true&edit=' . $_REQUEST['edit'] . '&iid=" + bInv + "&itopic=' . $ins_topic . '&pid=' . $_REQUEST['pid'] . '"; else inv = "";
        $.post("/themes/maennaco/includes/pro_posts.php?type=profileInfo&display=true&pro_id=" + pro_id + "&uid=" + uid + inv, {
        }, function(response) {

          $("#pro_popup").dialog({
            autoOpen: true,
            width: 650,
            title: "Profile",
            resizable: false,
            draggable: false,
            height: 400,
            closeText: "hide",
            buttons: {
              /*"Close": function() { $(this).dialog("close"); }*/

            }, closeOnEscape: true,
            modal: true
          }).html(response);
        });
      });


})
</script>
        <style>
    .box_title {

        height:30px;
        line-height:30px;
        padding-left:15px;

    }

    .main_content {

        padding-top:0 !important;

    }
    </style>';
        }
        /*        elseif ($AccessObj->user_type == 'people'){


                }*/
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}

function get_maenna_tabs($utype) {
    $ptab = sget($_REQUEST, 'ptab');
    $SearchKeys = array('letter', 'name', 'proid', 'industry', 'experties', 'state', 'proTypeSel', 'managementLevel', 'plres', 'rating', 'eventFrom', 'eventTo', 'managerid',);
    $filter_cnt = 0;
    foreach ($SearchKeys as $key) {
        if (sget($_REQUEST, $key) === '') {
            continue;
        }
        $filter_cnt++;
    }
    if ($utype == 'admin') {
        if ($ptab == '' || $ptab == '2') {
            $active1 = 'first active-trail active';
            $ptab = '2';
        } else {
            $active = 'first active-trail active';
        }
        $html = '<div class="account-section-tabs" style="margin:20px 0px 0px 0px;width:250px;">
            <ul>
<li class="' . $active . '"><a class="" href="/account?tab=professionals&ptab=1">Registered professionals</a></li>
<li class="' . $active1 . '"><a style="text-transform:none;" class="" href="/account?tab=professionals&ptab=2">Assigned professionals</a></li>';
        $html .= '</ul></div><br><br>';
    }
    require_once("." . base_path() . path_to_theme() . "/includes/classes/maenna_users.inc");
    global $AccessObj;
    $Prospects = array('high', 'medium', 'low', 'unlikely');
    //test_array($_REQUEST);
    if (sget($_REQUEST, 'name')) {
        $name = sget($_REQUEST, 'name');
    } else $name = '';
    $td_name = "<input type=text name=name placeholder='Name' value='$name'>";
    $filterCode = preg_replace('|[^0-9A-Za-z]|', '', sget($_REQUEST, 'ref_code'));
    $td_ref_filter = "<input type=\"text\" name=\"ref_code\" placeholder='Referral Code' value='$filterCode'>";
    $Letters = "abcdeghijklmnopqrstuvw";
    $td_letters = "";
    for ($i = 0; $i < strlen($Letters); $i++) {
        $l = strtoupper($Letters[$i]);
        $td_letters .= "<a href='#' class='letter_selector' data=$l>$l</a>&nbsp;";
    }
    $letter = sget($_REQUEST, 'letter') ? sget($_REQUEST, 'letter') : '';
    $td_letters .= "<input type=hidden name=letter value='' id='letter' value='$letter' /><a href='#' class='letter_selector' data=x style='letter-spacing:0;'>XYZ</a>";
    //// Second Row //////////////////////
    $proid = sget($_REQUEST, 'proid');
    $td_proid = "<input type=text name=proid value='$proid' placeholder='Professional ID' />";
    $industry = (sget($_REQUEST, 'industry')) ? sget($_REQUEST, 'industry') : 'Industry';
    $td_industry = "<select name=industry><option value=''>Industry</option>" . Options_industry($industry) . "</select>";
    // Experties
    /*
     $_Roles = _ROLES();
     $role = (sget($_REQUEST,'role')) ? sget($_REQUEST,'role') : 'Role';
     $option_role = '';
     foreach($_Roles as $r)
     {
     $selected = '';
     if($r == $role) $selected = 'selected';
     $option_role .= "\n<option $selected>$r</option>";
     }
     */
    $experties = sget($_REQUEST, 'experties');
    $td_experties = "\n<select name=experties><option value=''>Experties</option>" . Options_experties($experties) . "</select>";
    $state = sget($_REQUEST, 'state');
    $td_state = "\n<select name=state><option value=''>State</option>" . Options_state($state) . "</select>";
    $eventFrom = sget($_REQUEST, 'eventFrom');
    $td_event_from = "<input type=text name=eventFrom placeholder='Advisory Event From' value='$eventFrom' class='datepicker' />";
    /// Row 3 - 1 professional type///////////
    $proType = sget($_REQUEST, 'proTypeSel');
    $td_protype = "\n<select name='proTypeSel' id='proTypeSel'><option value=''>Professional Type</option>" . Options_proType($proType) . "</select>";
    // Row 3 - management level
    $_ManagementLevel = _managementLevel();
    $mLvl = sget($_REQUEST, 'managementLevel');
    $option_mLvl = '';
    foreach ($_ManagementLevel as $key => $val) {
        $selected = '';
        if ($key == '') {
            continue;
        }
        if ($mLvl === $key) {
            $selected = 'selected';
        }
        $option_mLvl .= "\n<option value='$key' $selected>$val</option>";
    }
    $td_mlevl = "\n<select name=managementLevel><option value=''>Background Check</option>$option_mLvl</select>";
    // Row3  - PL Responsibility
    $_PLRes = _plResponsibility();
    $plres = sget($_REQUEST, 'plres');
    $option_plres = '';
    foreach ($_PLRes as $key => $val) {
        $selected = '';
        if ($key == '') {
            continue;
        }
        if ($key == $plres) {
            $selected = 'selected';
        }
        $option_plres .= "\n<option value='$key' $selected>$val</option>";
    }
    $td_plres = "<select name='plres'><option value=''>PL Responsibility</option>$option_plres</select>";
    //$section3 = "\n<td><!--input type=text name=experience_high value='$experience_high' --></td>";
    $rating = sget($_REQUEST, 'rating');
    $sel_rating = "<select name=rating><option value=''>Rating</option>";
    $array = array('1' => '0 - 1', '2' => '1 - 2', '3' => '2 - 3', '4' => '3 - 4', '5' => '4 - 5');
    foreach ($array as $key => $val) {
        $selected = '';
        if ($rating == $key) {
            $selected = 'selected';
        }
        $sel_rating .= "\n<option value=$key $selected>$val</option>";
    }
    $sel_rating .= "</select>";
    $td_rating = "\n$sel_rating";
    $eventTo = sget($_REQUEST, 'eventTo');
    $td_event_to = "<input type=text name=eventTo value='$eventTo' placeholder='Advisory Event To' class='datepicker' />";
    if ($utype == 'super') {
        $Managers = Mae_users::all_admins();
        $options_manager = '';
        $posted_managerid = sget($_REQUEST, 'managerid');
        foreach ($Managers as $Admin) {
            $selected = '';
            $admin_id = sget($Admin, 'uid');
            $name = ucwords(sget($Admin, 'first_name')) . " " . ucwords(sget($Admin, 'last_name'));
            if (($admin_id == $posted_managerid) && $posted_managerid) {
                $selected = "selected";
            }
            $options_manager .= "<option $selected value='$admin_id'>$name</option>\n";
        }
        $td_rel_manager .= "<select name=managerid><option  value=''>Relationship Manager</option><option></option>$options_manager</select>";
    } else {
        $td_rel_manager = "";
    }
    $info_tab = sget($_REQUEST, 'info_tab');
    if (empty($info_tab)) {
        $info_tab = 'tab_a';
    }
    $_page = sget($_REQUEST, '_page');
    if ($filter_cnt > sget($_REQUEST, 'filterCnt')) $_page = 1;
    if (empty($_page)) {
        $_page = 1;
    }
    $ustatus = sget($_REQUEST, 'ustatus');
    $html .= <<< END
        <div class="pro-fixed-filters"><form action='/account' method='get' id='search_form'>

            <table cellspaing=0 cellpadding=0 class='search-tabs' style='width:80%;'>
                <tr>
                    <td>$td_name</td>
                    <td colspan=4>$td_letters</td>
                    <td>$td_ref_filter</td>
                </tr>
                <tr>
                    <td>$td_proid</td>
                    <td>$td_industry</td>
                    <td>$td_experties</td>
                    <td>$td_state</td>
                    <td style='text-align:right;padding-right:0'>$td_event_from</td>
                    <td>$td_rel_manager</td>
                </tr>
                <tr>
                    <td>$td_protype</td>
                    <td>$td_mlevl</td>
                    <td>$td_plres</td>
                    <td>$td_rating</td>
                    <td style='text-align:right;padding-right:0'>$td_event_to</td>
                    <td style=''>
                        <input type=submit value=Filter class="button" id = "submit_btn" >
                        </td>
                </tr>
            </table>
            <input type=hidden name=tab value=professionals>
            <input type=hidden name='admin_action' id='admin_action' value='' />
            <input type=hidden name='reject_company' id='reject_company' value='' />
            <input type=hidden name='selected_records' id='selected_records' value='' />
            <input type=hidden name='admin_id' id='admin_id' value='' />
            <input type=hidden name='cmp_id' id='cmp_id' value='' />
            <input type=hidden name='info_tab' id='info_tab' value='$info_tab' />
            <input type=hidden name='ustatus' value='$ustatus' />
            <input type=hidden name='proType' value='$proType' />
            <input type=hidden name='filterCnt' value='$filter_cnt' />
            <input type=hidden name='_page' id='_page' value='$_page' />
            <input type=hidden name='update_section' id='update_section' value='pro_update_section' />
        </form>

<script type='text/javascript'>
$(document).ready(function(){
    init_search_selector();
    init_select_all();
    init_datepicker();
    init_search_pagination();
    init_assign_admin();
    init_visibility();
    init_follow();
})
</script>
END;
    return $html;
}

function get_maenna_people($user_type) {
    //$time_start = microtime(true);
    global $redirect, $AccessObj;
    $companyid = sget($_REQUEST, 'id');
    $ptab = sget($_REQUEST, 'ptab');
    if ($user_type == 'admin') {
        if ($ptab == '') {
            $ptab = '2';
        }
    }
    $SearchKeys = array('letter', 'name', 'proid', 'industry', 'experties', 'state', 'proTypeSel', 'managementLevel', 'plres', 'rating', 'eventFrom', 'eventTo', 'managerid', 'ref_code');
    $SearchValues = array();
    foreach ($SearchKeys as $key) {
        if (sget($_REQUEST, $key) === '') {
            continue;
        }
        $SearchValues["$key"] = sget($_REQUEST, $key);
    }
    $filter_cnt = count($SearchValues);
    $SQL_Where = array();
    $thisYear = date("Y");
    //test_array($SearchValues);
    foreach ($SearchValues as $key => $val) {
        switch ($key) {
            case 'name' :
                if (strcasecmp($val, 'name') == 0) {
                    $val = '';
                } else {
                    $val = "(
                        (maenna_people.firstname like '%%" . ($val) . "%%') or
                        (maenna_people.real_first_name like '%%" . ($val) . "%%') or
                        (maenna_people.real_last_name like '%%" . ($val) . "%%') or
                        (maenna_people.email like '%%" . ($val) . "%%')
                    )";
                }
                break;
            case 'industry' :
                $val = "maenna_people.industry = '$val'";
                break;
            case 'experties' :
                $val = "maenna_people.experties = '$val'";
                break;
            case 'proTypeSel' :
                $val = "(users_roles.rid = '$val')";
                break;
            case 'managementLevel' :
                $val = "maenna_people.mlvl = '$val'";
                break;
            case 'plres' :
                $val = "(maenna_people.plres = '$val' and maenna_people.plres is not null)";
                break;
            case 'experience' :
                $year5 = $thisYear - 5;
                $year10 = $thisYear - 10;
                $year15 = $thisYear - 15;
                if ($val == 1) {
                    $val = "maenna_people.yearwork >= $year5";
                } elseif ($val == 2) {
                    $val = "(maenna_people.yearwork >= $year10 and maenna_people.yearwork <= $year5)";
                } elseif ($val == 3) {
                    $val = "(maenna_people.yearwork >= $year15 and maenna_people.yearwork <= $year10)";
                } elseif ($val == 4) {
                    $val = "maenna_people.yearwork <= $year15";
                } else {
                    break;
                }
                break;
            case 'state' :
                $val = "maenna_people.state = '$val'";
                break;
            case 'letter' :
                $val = "maenna_people.real_last_name like '" . check_plain($val) . "%'";
                break;
            case 'rating' :
                $val = "(maenna_people.rating is not null and maenna_people.rating >= " . ($val - 1) . " and maenna_people.rating < " . $val . ")";
                break;
            case 'eventFrom' :
                if (strcasecmp('', $val) == 0) {
                    $val = '';
                } else {
                    $val = '';
                }
                break;
            case 'eventTo' :
                if (strcasecmp('', $val) == 0) {
                    $val = '';
                } else {
                    $val = '';
                }
                break;
            case 'proid' :
                if (strcasecmp('', $val) == 0) {
                    $val = '';
                } else {
                    $proid = intval(filter_num($val));
                    $val = "maenna_people.pid = $proid";
                }
                break;
            case 'managerid' :
                $val = "managerid = '$val'";
                break;
            case 'ref_code':
                $refCode = preg_replace('|[^0-9A-Za-z]|', '', $val);
                if(!empty($refCode))
                    $val = "(referral_code = '{$refCode}' OR code_to_refer = '{$refCode}')";
                break;
            default :
                continue;
                break;
        }
        if (!empty($val)) {
            $SQL_Where["$key"] = $val;
        }
    }
    //$SQL_Where[] = " (delete_marker != 1 OR delete_marker is NULL)";
    $SQL_Where[] = " users.uid = maenna_people.pid";
    $SQL_Where[] = " users.uid = users_roles.uid and users.status = 1";
    if ($_REQUEST['ustatus'] == 'approved') $SQL_Where[] = " maenna_people.approved = 1";
    if ($_REQUEST['ustatus'] == 'visible') $SQL_Where[] = " maenna_people.public = 1";
    if ($_REQUEST['ustatus'] == 'contributing') $SQL_Where[] = " EXISTS (SELECT assignee_uid FROM maenna_connections WHERE conntype = 'collaborator' and status = 'active' and assignee_uid = maenna_people.pid)";
    if ($_REQUEST['ustatus'] == 'insights') $SQL_Where[] = " exists (SELECT postedby FROM maenna_professional WHERE postedby = maenna_people.pid)";
    if ($_REQUEST['ustatus'] == 'deactivated') $SQL_Where[] = " maenna_people.active = 0";
    $str_sqlWhere = ' where ' . implode(" and ", $SQL_Where);
    $manager_table = " left join( select target_uid,
                                        assignee_uid as managerid
                                    from maenna_connections
                                    where
                                        maenna_connections.conntype = 'admin' and
                                        maenna_connections.status = 'active'
                                ) as manager_table on manager_table.target_uid = maenna_people.pid";
    if ($user_type != 'super') {
        if ($user_type == 'admin' && $ptab == '2') {
            $str_sqlWhere .= " AND (approved = 1 OR manager_table.managerid = $AccessObj->uid)";
        } else $str_sqlWhere .= " AND maenna_people.active = 1";
        /*$str_sqlWhere .= " AND (
                            (
                            managerid =  '".$AccessObj->uid."'
                            AND manager_table.conntype =  'admin'
                            )
                            OR (
                            manager_table1.target_uid
                            IN (

                            SELECT target_uid
                            FROM maenna_connections
                            WHERE STATUS =  'active'
                            AND conntype =  'admin'
                            AND assignee_uid =  '".$AccessObj->uid."'
                            AND role =  '3'
                            )
                            AND manager_table1.conntype <>  'admin'
                            ) OR approved = 1
                            )";*/
        /*$table_manager = " left join (select target_uid,
         assignee_uid as managerid
         from maenna_connections
         where maenna_connections.conntype ='proadmin' and
         maenna_connections.status ='active'
         ) as table_manager on table_manager.target_uid = users_roles.uid";
         */
        $manager_table = " left join( select target_uid,
                                        assignee_uid as managerid,conntype
                                    from maenna_connections
                                    where
                                        maenna_connections.status = 'active'
                                        and maenna_connections.conntype = 'admin'
                                ) as manager_table on manager_table.target_uid = maenna_people.pid

                                left join( select target_uid,
                                        assignee_uid,conntype
                                    from maenna_connections
                                    where
                                        maenna_connections.status = 'active'
                                ) as manager_table1 on manager_table1.assignee_uid = maenna_people.pid";
    }
    $sql_total = "select count(*) as total from (select distinct users.created as register,
                    users.uid,
                   maenna_people.*,
                    users_roles.rid
            from    maenna_people $manager_table
                    LEFT JOIN users ON maenna_people.pid = users.uid
                    LEFT JOIN users_roles ON maenna_people.pid = users_roles.uid
                    " . $str_sqlWhere . ") as tmp_table";
    $result = db_query($sql_total);
    $row = db_fetch_object($result);
    $total = $row->total;
    //echo($sql_total);
    // finialize SQL before sql_total returns
    $_page = sget($_REQUEST, '_page');
    if ($filter_cnt > sget($_REQUEST, 'filterCnt')) $_page = 1;
    if (empty($_page)) {
        $_page = 1;
    }
    //$limit = 20;
    $limit = 50;
    $start = ($_page - 1) * $limit;
    $thisYear = intval(date("Y"));
    $sql = "SELECT DISTINCT
  users.created                                                                                AS register,
  users.uid,
  IF(maenna_people.username_type = 1, maenna_people.firstname, CONCAT(maenna_people.firstname, ' ',
                                                                      maenna_people.lastname)) AS firstname,
  (SELECT ifnull(NULLIF(data_value, ''), data_value3)
   FROM maenna_people_data
   WHERE data_type = 'education' AND pid = users.uid
   ORDER BY dataid DESC
   LIMIT 1)                                                                                    AS education,
  maenna_people.*,
  manager_table.managerid,
  users_roles.rid,
  profiles.count                                                                               AS count,
  referal_tbl.referralscnt,
  follower_tbl.followerscnt,
  advisor_tbl.advisorscnt,
  discussion_tbl.discussionscnt,
  collaboration_tbl.collaborationcnt,
  owners_tbl.owneventscnt
FROM maenna_people  $manager_table
  LEFT JOIN users ON maenna_people.pid = users.uid
  LEFT JOIN users_roles ON maenna_people.pid = users_roles.uid
  LEFT JOIN (SELECT
               mf.uid   AS fid,
               count(*) AS followerscnt
             FROM maenna_followers mf LEFT JOIN users u ON u.uid = mf.cid
             WHERE u.status = 1
             GROUP BY mf.uid) AS follower_tbl ON follower_tbl.fid = maenna_people.pid
  LEFT JOIN (SELECT
               mc.assignee_uid AS aid,
               count(*)        AS advisorscnt
             FROM `maenna_connections` mc LEFT JOIN users u ON mc.target_uid = u.uid
             WHERE mc.conntype = 'advisor' AND mc.status = 'active' AND u.status = 1
             GROUP BY aid) AS advisor_tbl ON advisor_tbl.aid = maenna_people.pid
  LEFT JOIN (SELECT
               uid      AS mceid,
               count(*) AS discussionscnt
             FROM `maenna_company_events_inv` mcei
             WHERE mcei.status = 'confirmed'
             GROUP BY mceid) AS discussion_tbl ON discussion_tbl.mceid = maenna_people.pid
  LEFT JOIN (SELECT
               mc.assignee_uid AS cid,
               count(*)        AS collaborationcnt
             FROM `maenna_connections` mc LEFT JOIN users u ON mc.target_uid = u.uid
             WHERE mc.conntype = 'collaborator' AND mc.status = 'active' AND u.status = 1
             GROUP BY cid) AS collaboration_tbl ON collaboration_tbl.cid = maenna_people.pid
  LEFT JOIN (SELECT
               mp.postedby AS proid,
               count(*)    AS owneventscnt
             FROM maenna_professional mp
             GROUP BY proid) AS owners_tbl ON owners_tbl.proid = maenna_people.pid
  LEFT JOIN (SELECT
               pid,
               SUM(ccnt) AS referralscnt
             FROM (
                    SELECT mp.pid,COUNT(mc.referral_code) as ccnt
                    FROM maenna_people mp
                      LEFT JOIN maenna_company mc ON mp.code_to_refer = mc.referral_code
                      INNER JOIN users u ON u.uid = mc.companyid
                    WHERE u.status = 1 and (mc.referral_code is not null and mc.referral_code <> '')
                    GROUP BY mp.pid

                    UNION ALL

                    SELECT mp.pid,COUNT(mp2.referral_code) as ccnt
                    FROM maenna_people mp
                      LEFT JOIN maenna_people mp2 ON mp.code_to_refer = mp2.referral_code
                      INNER JOIN users u ON u.uid = mp2.pid
                    WHERE  u.status = 1 and (mp2.referral_code is not null and mp2.referral_code <> '')
                    GROUP BY mp.pid
                  ) AS a
             GROUP BY pid) AS referal_tbl ON referal_tbl.pid = maenna_people.pid
  LEFT JOIN (SELECT
               tmp.pid,
               COUNT(*) AS count
             FROM (SELECT
                     mp.pid         AS pid,
                     mp.data_value3 AS status
                   FROM maenna_people_data mp INNER JOIN (SELECT *
                                                          FROM maenna_people_data
                                                          WHERE data_type = 'addinfo' AND
                                                                data_attr IN ('experties', 'industryview', 'mgmtview')
                                                          ORDER BY access DESC) a ON mp.dataid = a.dataid
                   GROUP BY mp.data_attr, mp.pid
                   HAVING status = 'saved' OR status = 'submitted') AS tmp
             GROUP BY tmp.pid) AS profiles ON profiles.pid = maenna_people.pid
            " . $str_sqlWhere . " order by active DESC,created DESC limit $start, $limit";
    //echo $sql;
    //die($sql);
    // total_sql  return if 0
    if ($total == 0) {
        return "Found 0 Result";
    }
    $info_tab = sget($_REQUEST, 'info_tab');
    if (empty($info_tab)) {
        $info_tab = 'tab_a';
    }
    $class_tab_a = $class_tab_b = $class_tab_c = '';
    if ($info_tab == 'tab_a') {
        $class_tab_a = 'active';
    } elseif ($info_tab == 'tab_b') {
        $class_tab_b = 'active';
    } elseif ($info_tab == 'tab_c') {
        $class_tab_c = 'active';
    }
    if ($user_type == 'super' || ($user_type == 'admin' && $ptab == '2')) {
        $detail_array = array(
            'checkbox'        => array('', '', 25),
            'rownum'          => array('', '', 25),
            'proid'           => array('User name', '', 120),
            'real_first_name' => array('First Name', '', 60),
            'real_last_name' => array('Last Name', '', 60),
            'public'          => array('<div data-tooltip="Visible to companies" style="vertical-align:middle;display:inline-block; margin-right:15px; margin-left:15px;  background-color:#00a3bf;width:12px;height:12px;border:1px solid #00a3bf;"></div>', '', 20),
            'approved'        => array('<div data-tooltip="Visible to Admins" style="vertical-align:middle;display:inline-block; margin-right:15px; margin-left:15px;  background-color:#527382;width:12px;height:12px;border:1px solid #527382;"></div>', '', 20),
            'insight'         => array('<div data-tooltip="Insights" style="vertical-align:middle;display:inline-block; margin-right:15px; margin-left:15px;  background-color:#013241;width:12px;height:12px;border:1px solid #013241;"></div>', '', 20),
            'deactivated'     => array('<div data-tooltip="Deactivated professionals" style="vertical-align:middle;display:inline-block; margin-right:15px; margin-left:15px;  background-color:#939597;width:12px;height:12px;border:1px solid #939597;"></div>', '', 20),
            'rid'             => array('User role', 'tab_a', 80),
            'experties'       => array('Experties', 'tab_a', 200),
            'proj_name'       => array('Company', 'tab_c', 200),
            'yearwork'        => array('Yrs', 'tab_a', 30),
            'rating'          => array('R', 'tab_a', 20),
            'code_to_refer'   => array('Ref', 'tab_c', 200),
            'referral_code'   => array('Inv code', 'tab_c', 200),
            'advising'        => array('<div data-tooltip="Active" style="vertical-align:middle;display:inline-block; margin-right:15px; margin-left:15px;  background-color:#00234D;width:12px;height:12px;border:1px solid #00234D;"></div>', 'tab_a', 20),
            'discussions'     => array('<div data-tooltip="Project Services" style="vertical-align:middle;display:inline-block; margin-right:15px; margin-left:15px;  background-color:#366092;width:12px;height:12px;border:1px solid #366092;"></div>', 'tab_a', 20),
            'collaborating'   => array('<div data-tooltip="Collaborating" style="vertical-align:middle;display:inline-block; margin-right:15px; margin-left:15px;  background-color:#95b3d7;width:12px;height:12px;border:1px solid #95b3d7;"></div>', 'tab_a', 20),
            'followercnt'     => array('<div data-tooltip="Following" style="vertical-align:middle;display:inline-block; margin-right:15px; margin-left:15px;  background-color:#b8cce4;width:12px;height:12px;border:1px solid #b8cce4;"></div>', 'tab_a', 20),
            'ownevents'       => array('<div data-tooltip="Insights held" style="vertical-align:middle;display:inline-block; margin-right:15px; margin-left:15px;  background-color:#DDEDFF;width:12px;height:12px;border:1px solid #DDEDFF;"></div>', 'tab_a', 20),
            'referralscnt'    => array('<div data-tooltip="Referrals" style="vertical-align:middle;display:inline-block; margin-right:15px; margin-left:15px;  background-color:#d8dd4f;width:12px;height:12px;border:1px solid #d8dd4f;"></div>', 'tab_a', 20),
            //'assign_admin' =>array('Assign Admin',      'tab_a',160),
            /*'uname' => array('User Name', 'tab_b', 90),*/
            'register'        => array("Registered", 'tab_b', 90),
            'industry'        => array('Industry', 'tab_c', 150),
            'education'       => array('Education', 'tab_c', 150),
            'mlvl'            => array('Level', 'tab_c', 160),
            'specialprojects' => array('Sp Projects', 'tab_c', 160),
            /*'status' => array('Status', 'tab_b', 60),*/
            'email'           => array('Email', 'tab_b', 200),
            'advisorydate'    => array('Adv Date', 'tab_c', 90),
            'state'           => array('State', 'tab_b', 60),
            'managerid'       => array('Relationship Manager', 'tab_b', 160),
        );
    } else {
        $detail_array = array(
            'checkbox'  => array('', '', 25),
            'rownum'    => array('', '', 25),
            'proid'     => array('Pro ID', '', 140),
            'experties' => array('Experties', 'tab_a', 200),
            'register'  => array("Registered", 'tab_a', 90),
            'industry'  => array('Industry', 'tab_a', 150),
            'education' => array('Education', 'tab_a', 150),
            'state'     => array('State', 'tab_a', 60)
        );
    }
    $html = <<<DNE
            <script type="text/javascript">
            $('document').ready(function(){
              $("body").append('<div id="sticky-head"><table class="no-border actions">'+$(".actions").html()+'</table><table id="sticky-head1"><thead>'+$("table.account-table table.left-align thead").html()+'</thead></table></div>');
              var offset  = $("table.account-table table.left-align thead").offset();
              $('#sticky-head').css({
                'border-collapse': 'collapse',
                 width: $("table.account-table table.left-align").width(),
                 position:'fixed',
                 top: 0,
                 left: offset.left,
                 display: 'none',
                 margin: 0});
               $("#sticky-head1").css({background: '#94c9da','margin':'0'});
               $("div#sticky-head table.actions").css({
                  'font-size': '10px',
                  'font-weight': 'bold',
                  'color':'#76787F',
                  'text-transform': 'uppercase',
                  'text-decoration': 'none',
                  'font-family': 'LatoRegular',
                  'background-color':'#fff',
                  'margin':'0',
                  'cursor':'pointer'
              });
              $('div#sticky-head table.actions a').hover( function(){

                    $(this).css({'color':'#00a2bf','text-decoration':'none'});


                },
                function(){

                    $(this).css('color', '#76787F');

                });

              $("div#sticky-head table.actions a").click(function(){

                id = $(this).attr('id');
                $("#"+id).click();

              });

              $("div#sticky-head table.actions select").css({

              'height':'34px',
              'text-transform':'none',
              'font-style':'italic',
              'font-size':'14px'
              });

              $('#sticky-head1 thead').css({width: $("table.account-table table.left-align thead").width()});
              $("table.account-table table.left-align thead th").css('padding-right',0);
              $("table.account-table table.left-align thead th.no-padd").css('padding-right',5);
              $("#sticky-head1 thead th").css('padding-right',0);
              $("#sticky-head1 thead th.no-padd").css('padding-right',5);
              $("#sticky-head1 thead th").css('height',32);
              $("table.account-table table.left-align thead th").each(function(i){
                $('#sticky-head1 thead th').eq(i).css('font-size', $(this).css('font-size'));
              });

              $(window).resize(function(){
                offset = $("table.account-table table.left-align thead").offset();
                $('#sticky-head').css({
                  left: offset.left
                });
              });

              $(window).scroll(function(){
                if($(window).scrollTop() > (offset.top*1)){
                  $('#sticky-head').show();
                }else{
                  $('#sticky-head').hide();
                }
              });
            /*
              var filtersOffset = $(".pro-fixed-filters").offset();
              $(window).scroll(function(){
                if($(window).scrollTop() >= filtersOffset.top){
                  $(".pro-fixed-filters").css({'position':'fixed', 'top':0,'width':1069,'background-color':'#fff','z-index':1});
                  $("#filter-table-spacer").css({'height':$(".pro-fixed-filters").height()});
                } else {
                  $(".pro-fixed-filters").css({'position':'static'});
                  $("#filter-table-spacer").css({'height':0});
                }
              });
            */
            });
            </script>
DNE;
    if ($user_type == 'super' || ($user_type == 'admin' && $ptab == '2')) {
        $html .= "<div class='searchtab'>
                 <a href='#' class='info_tab_btn $class_tab_a' tab='tab_a'  >STATISTICS</a>&nbsp;&nbsp;&nbsp;
				<a href='#' class='info_tab_btn $class_tab_b' tab='tab_b'  >CONTACTS</a>&nbsp;&nbsp;&nbsp;
                <a href='#' class='info_tab_btn $class_tab_c' tab='tab_c'  >BACKGROUND</a>
            </div>";
    }
    if ($AccessObj->user_type == 'super') {
        $delete_btn = '<a data-tooltip="This tool will permanently delete selected users from database." class="tool" type="button" id="delete_btn" >delete</a>&nbsp;&nbsp;';
        $vis_btn = '<a data-tooltip="This tool will make professional visible to company users who can follow." type=button class=tool id="public_btn" >make visible</a>&nbsp;&nbsp;';
        $rej_btn = '<a data-tooltip="This tool will delete the professional user as rejected and keep his email account to ensure same email is not registered again." type=button class=tool id="reject_btn" >disqualify</a>&nbsp;&nbsp;';
        $app_btn = '<a data-tooltip=" This tool will make this professional visible to admins for administration." type=button class=tool id="approve_btn" > approve proffesionals</a>&nbsp;&nbsp;';
        $ins_btn = '<a data-tooltip = "This tool will mark the user as a professional who registered to attend Insights."type=button class=tool id="insight_btn" >toggle insight</a>&nbsp;&nbsp;';
        $act_btn = '<a data-tooltip = "This tool will list the user at the bottom of the professional list due to lack of activity."type=button class=tool id="active_btn" >toggle deactive</a>&nbsp;&nbsp;';
        $adm_btn = '<a type=button class=tool id="assign_admin_btn" >submit</a>';
        $option_admins = option_code($AccessObj->All_admins);
        $value_adm = "\n<select id='select_adminid' style='width:333px;'>
          <option value='none'>Choose admin to assign project</option>
                $option_admins</select>";
        $value = '';
    } elseif ($user_type == 'admin' && $ptab == '2') {

        $match_btn = '<a type=button class=tool id="match_btn" >submit to match</a>';
        $option_companies = option_code(getAdminCompanies($AccessObj->uid));
        $value = "\nSelect professionals &nbsp;<select id='select_cmpid' style='width:200px;'><option style='color:grey' value='none'>Choose Projects</option>$option_companies</select>";
    }
    $html .= '<table class="no-border actions"><tr border=0><td style="vertical-align:middle">
            ' . $delete_btn . $vis_btn . $rej_btn . $app_btn . $ins_btn . $act_btn . $value . ' &nbsp;&nbsp' . $match_btn . '&nbsp;&nbsp' . $value_adm . '&nbsp;&nbsp' . $adm_btn . '</td></tr></table>';
    $html .= "</div> <!-- end of #pro-fixed-filters -->";
    $html .= "<div id=\"filter-table-spacer\"></div>";
    $html .= "<table cellspacing=0 cellpadding=0 border=0 style='width:1069px;'  class='result-table left-align'>
                <thead style='background:#94c9da;'>
                    <th style='width:25px;text-align:left;' >
                        <input type=checkbox id='select_all' >
                    </th>";
    foreach ($detail_array as $key => $Column) {
        if ($key == 'checkbox' || ($key == 'managerid' && $AccessObj->user_type != 'super')) {
            continue;
        }
        if (($key == 'name') && $AccessObj->user_type != 'super') {
            continue;
        }
        if ($key == 'rownum') {
            $html .= "<th style=\"width:25px;text-align:left;\">#</th>";
            continue;
        }
        $class = '';
        if ($Column[1] == '' || $info_tab == $Column[1]) {
            $width = '';
            $name = $Column[0];
            if ($Column[2]) {
                $width = "width:" . $Column[2] . 'px;';
            }
            if ($key == 'rating') {
                $width .= " text-align:center;";
            }
            if ($key == 'public') $padd_class = 'no-padd'; else $padd_class = '';
            $html .= "<th class='$padd_class' style='$width'>$name</th>";
        }
    }
    $html .= "</tr></thead><tbody>";
    //die($sql);
    $result = db_query($sql);
    //var_dump($result);
    $_ProType = _pro_roles();
    // numeric index array
    $_Industry = _INDUSTRY();
    //$_Experties = _experties();
    $_mLvl = _managementLevel();
    if (!empty($result)) {
        if ($_page != 1) {
            $rownum = $start;
        } else $rownum = 0;
        while (($row = db_fetch_array($result)) !== false) {
            $pid = sget($row, 'pid');
            $published = sget($row, 'published');
            foreach ($detail_array as $key => $Column) {
                if ($Column[1] == '' || $info_tab == $Column[1]) {
                    $width = '';
                    $name = $Column[0];
                    if ($Column[2]) {
                        $width = "width:" . $Column[2] . 'px';
                    }
                } else {
                    continue;
                }
                $created = sget($row, 'register');
                $value = sget($row, $key);
                if ($key != 'email') {
                    $value = ucwords($value);
                }
                // if($user_type == 'admin' && sget($row,'managerid') == $AccessObj->uid)
                //  $trbg = 'style="background-color:#E5E5E5"';
                if ($key == 'rownum') {
                    $rownum++;
                    $html .= "<td>$rownum</td>";
                    continue;
                } elseif ($key == 'checkbox') {
                    $html .= "<tr $trbg><td><input type=checkbox name=records[] value='$pid' class='cb_record records' /></td>";
                    continue;
                } elseif ($key == 'register') {
                    $value = date('m/d/Y', $value);
                } elseif ($key == 'proid') {
                    $link_color = 'gray';
                    if (!empty($row['profile'])) $link_color = 'black';
                    //$value = $title = getProId($pid);
                    $value = $title = $row['firstname'];
                    $value = ucwords(strtolower($value));
                    if ($row['count'] > 0) $value .= '*';
                    $value = "<a style='color:$link_color;' href='$redirect&page=pro_detail&id=$pid'>$value</a>";
                    $html .= "<td><div style='width:$width;overflow:hidden;' title='$title'>$value</div></td>";
                    continue;
                } elseif ($key == 'rid') {
                    $value = $_ProType["$value"];
                    if ($value == 'Other Expert') {
                        $value = 'Other';
                    } elseif ($value == 'Company Member') {
                        $value = 'Member';
                    } elseif ($value == 'Operating Executive') {
                        $value = 'Operator';
                    }
                    if (get_pro_accreditation_status($pid) != 0) {
                        $value .= ' (A';
                        $accData = getAccreditationVerData($pid);
                        if ($accData) $value .= 'V)'; else $value .= ')';
                    }
                    $html .= "<td><div  style='width:$width;overflow:hidden;'>$value</div></td>";
                    continue;
                } elseif ($key == 'industry') {
                    if ($value && ($created > '1322005040')) {
                        foreach ($_Industry as $SubArray) {
                            foreach ($SubArray as $index => $val) {
                                if (strcasecmp($index, $value) == 0) {
                                    $value = $val;
                                    break;
                                }
                            }
                        }
                    }
                    $html .= "<td><div  style='width:$width;overflow:hidden;height:20px;' title='$title'>" . ($value) . "</div></td>";
                    continue;
                } elseif ($key == 'education') {
                    if ($value) {
                        $html .= "<td><div  style='width:$width;overflow:hidden;height:20px;'>" . ($value) . "</div></td>";
                    }
                    continue;
                } elseif ($key == 'experties') {
                    /*					if ($value && ($created > '1322005040')) {
                                            foreach ($_Experties as $SubArray) {
                                                foreach ($SubArray as $index => $val) {
                                                    if (strcasecmp($index, $value) == 0) {
                                                        $value = $val;
                                                        break;
                                                    }
                                                }
                                            }
                                        }*/
                    $value = preg_replace('/(?<!\ )[A-Z]/', ' $0', $value);
                    $html .= "<td><div  style='width:$width;overflow:hidden;height:20px;'>" . ($value) . "</div></td>";
                    continue;
                } elseif ($key == 'proj_name') {
                    if ($value == '') {
                        $value = '-';
                    }
                } elseif ($key == 'mlvl') {
                    if ($value && ($created > '1322005040')) {
                        foreach ($_mLvl as $index => $val) {
                            if (strcasecmp($index, $value) == 0) {
                                $value = $val;
                                break;
                            }
                        }
                    }
                    $html .= "<td><div  style='width:$width;overflow:hidden;height:20px;text-align:left;' title='$value'>" . ($value) . "</div></td>";
                    continue;
                } elseif ($key == 'yearwork') {
                    if ((int) $value > 0) {
                        $value = $thisYear - $value;
                    }
                    $html .= "<td align=right>$value</td>";
                    continue;
                } elseif ($key == 'full_name') {

                    $value = sget($row, 'real_first_name')." ".sget($row, 'real_last_name');
                    $html .= "<td><div style='width:$width;overflow:hidden'>" . htmlentities(ucwords($value)) . "</div></td>";
                    continue;
                } elseif ($key == 'real_last_name') {

                    $value = sget($row, 'real_last_name');
                    $html .= "<td><div style='width:$width;overflow:hidden'>" . htmlentities(ucwords($value)) . "</div></td>";
                    continue;
                } elseif ($key == 'email') {
                    if ($user_type == 'super' || ($user_type == 'admin' && ifAssignedAdmin($row['pid'], $AccessObj->uid))) {
                        $html .= "<td><div style='width:$width;overflow:hidden'>" . htmlentities($value) . "</div></td>";
                    } else {
                        $html .= "<td><div style='width:$width;overflow:hidden'></div></td>";
                    }
                    continue;
                } elseif ($key == 'managerid') {
                    if ($user_type != 'super') {
                        continue;
                    }
                    $admin_id = sget($row, 'managerid');
                    $option_admins = option_code($AccessObj->All_admins, $admin_id);
                    $html .= "<td><select class='select_assign_admin' name='admin_$pid' style='width:130px;' target_uid='$pid'><option value='none'></option>$option_admins</select></td>";
                    continue;
                } elseif ($key == 'advising') {
                    if ($user_type != 'super' && $user_type != 'admin') {
                        continue;
                    }
                    $adCnt = getAdvisorsNum($pid);
                    //$adCnt = sget($row,'advisorscnt');
                    if ($adCnt == 0) {
                        $adCnt = '-';
                    }
                    $html .= '<td style="text-align:center;">' . $adCnt . '</td>';
                    continue;
                } elseif ($key == 'public') {
                    if ($user_type != 'super' && $user_type != 'admin') {
                        continue;
                    }
                    if (sget($row, 'public') == 1) {
                        $html .= '<td><div style="vertical-align:middle; display:inline-block; margin-right:15px; margin-left:15px;  background-color:#00a3bf;width:12px;height:12px;border:1px solid #00a3bf;"></div></td>';
                    } else {
                        $html .= '<td></td>';
                    }
                    continue;
                } elseif ($key == 'approved') {
                    if ($user_type != 'super' && $user_type != 'admin') {
                        continue;
                    }
                    if (sget($row, 'approved') == 1) {
                        $html .= '<td><div style="vertical-align:middle; display:inline-block; margin-right:15px; margin-left:15px;  background-color:#527382;width:12px;height:12px;border:1px solid #527382;"></div></td>';
                    } else {
                        $html .= '<td></td>';
                    }
                    continue;
                } elseif ($key == 'insight') {
                    if ($user_type != 'super' && $user_type != 'admin') {
                        continue;
                    }
                    if (sget($row, 'insight_came_flag') == 1) {
                        $html .= '<td><div style="vertical-align:middle; display:inline-block; margin-right:15px; margin-left:15px;  background-color:#013241;width:12px;height:12px;border:1px solid #013241;"></div></td>';
                    } else {
                        $html .= '<td></td>';
                    }
                    continue;
                } elseif ($key == 'deactivated') {
                    if ($user_type != 'super' && $user_type != 'admin') {
                        continue;
                    }
                    if (sget($row, 'active') == 0) {
                        $html .= '<td><div style="vertical-align:middle; display:inline-block; margin-right:15px; margin-left:15px;  background-color:#939597;width:12px;height:12px;border:1px solid #939597;"></div></td>';
                    } else {
                        $html .= '<td></td>';
                    }
                    continue;
                } elseif ($key == 'discussions') {
                    if ($user_type != 'super' && $user_type != 'admin') {
                        continue;
                    }
                    //$DisCnt = getDiscussionsNum($pid);
                    $DisCnt = sget($row, 'discussionscnt');
                    if ($DisCnt == 0) {
                        $DisCnt = '-';
                    }
                    $html .= '<td style="text-align:center;">' . $DisCnt . '</td>';
                    continue;
                } elseif ($key == 'collaborating') {
                    if ($user_type != 'super' && $user_type != 'admin') {
                        continue;
                    }
                    //$collCnt = getCollaborationNum($pid);
                    $collCnt = sget($row, 'collaborationcnt');
                    if ($collCnt == 0) {
                        $collCnt = '-';
                    }
                    $html .= '<td style="text-align:center;">' . $collCnt . '</td>';
                    continue;
                } elseif ($key == 'ownevents') {
                    //$sql_total = "select count(*) as total  from  maenna_professional where postedby = %d";
                    //$result_total = db_query($sql_total,array($pid));
                    //$m = db_fetch_object($result_total);
                    $m = sget($row, 'owneventscnt');
                    $html .= '<td style="text-align:center;">' . ($m == 0 ? '-' : $m) . '</td>';
                    continue;
                } elseif ($key == 'followercnt') {
                    if ($user_type != 'super' && $user_type != 'admin') {
                        continue;
                    }
                    //$follCnt = getFollowersNum($pid);
                    $follCnt = sget($row, 'followerscnt');
                    if ($follCnt == 0) {
                        $follCnt = '-';
                    }
                    $html .= '<td style="text-align:center;">' . $follCnt . '</td>';
                    continue;
                } elseif ($key == 'referralscnt') {
                    if ($user_type != 'super' && $user_type != 'admin') {
                        continue;
                    }
                    $refCnt = sget($row, 'referralscnt');
                    if ($refCnt == 0 || $refCnt == '') {
                        $refCnt = '-';
                    }
                    $html .= '<td style="text-align:center;">' . $refCnt . '</td>';
                    continue;
                } elseif ($key == 'specialprojects') {
                    if ($user_type != 'super' && $user_type != 'admin') {
                        continue;
                    }
                    //$html .= '<td style="text-align:left;">COMING SOON!</td>';
                    continue;
                } elseif ($key == 'assign_admin') {
                    if (!in_array($user_type, array('super', 'admin'))) continue;
                    $admin_id = sget($row, 'managerid');
                    $option_admins = option_code($AccessObj->All_admins, $admin_id);
                    if ($user_type == 'super') {
                        $value = "<select class='select_assign_admin' name='admin_$recordid' style='width:130px;' target_uid='$pid'><option value='none'></option>$option_admins</select>";
                    } elseif ($admin_id) {
                        $value = getAdminName($admin_id);
                    }
                }
                //if(empty($value))$value = '-';
                $html .= "<td ref=" . $key . ">" . $value . "</td>";
            }
            $html .= "</tr>";
        }
        if ((sget($_REQUEST, 'group_result') == 'batch assign')) {
            perform_grouping('people', $sql_where);
        }
    }
    $html .= "</tbody></table>";
    $Pagenation = array('total' => $total, 'limit' => $limit, 'baseurl' => "/account?", 'num_of_links' => 8, '_page' => $_page);
//	if ($AccessObj -> user_type == 'super') {
//		$delete_btn = '<a href="#" class="tool" id="delete_btn" >delete</a>&nbsp;&nbsp;';
//		$vis_btn = '<a type=button class=tool id="public_btn" >add to follower table</a>&nbsp;&nbsp;';
//        $app_btn = '<a type=button class=tool id="approve_btn" >toggle approve proffesionals</a>&nbsp;&nbsp;';
//        $adm_btn = '<a type=button class=tool id="assign_admin_btn" >assign admin</a>';
//        $option_admins = option_code($AccessObj->All_admins);
//        $value_adm = "\n<select id='select_adminid' style='width:130px;'><option value='none'></option>$option_admins</select>";
//        $value = '';
//	} else {
//		$match_btn = '<a type=button class=tool id="match_btn" >submit to match</a>';
//        $option_companies = option_code(getAdminCompanies($AccessObj->uid));
//        $value = "\nSelect professionals &nbsp;<select id='select_cmpid' style='width:200px;'><option style='color:grey' value='none'>Choose Projects</option>$option_companies</select>";
//    }
    $html .= '<table class="no-border"><tr border=0><td style="vertical-align:middle">&nbsp;</td><td border=0>' . "<div style='width:100%;'>" . pagination($Pagenation) . "</div>" . '</td></tr></table>';
    $html .= '<span style="float:left;">

            <!--<div title="Visibility" style=\'display:inline-block; margin-right:7px; margin-left:7px;  background-color:#ED0000;width:12px;height:12px;border:1px solid #ED0000;\'></div>

            <div title="Approved" style=\'display:inline-block; margin-right:7px; margin-left:7px;  background-color:#ed934c;width:12px;height:12px;border:1px solid #ed934c;\'></div>

            <div title="Advising" style=\'display:inline-block; margin-right:7px;margin-left:7px; background-color:#00234D;width:12px;height:12px;border:1px solid #00234D;\'></div>

            <div title="Discussions" style=\'display:inline-block; margin-right:7px;margin-left:7px; background-color:#366092;width:12px;height:12px;border:1px solid #366092;\'></div>

            <div title="Collaborating" style=\'display:inline-block; margin-right:7px;margin-left:7px; background-color:#95B3D7;width:12px;height:12px;border:1px solid #95B3D7;\'></div>

            <div title="Followers" style=\'display:inline-block; margin-right:7px;margin-left:7px; background-color:#B8CCE4;width:12px;height:12px;border:1px solid #B8CCE4;\'></div>
            <div title="Own events - coming soon" style=\'display:inline-block; margin-right:7px;margin-left:7px; background-color:#DDEDFF;width:12px;height:12px;border:1px solid #DDEDFF;\'></div>
            <div title="Referals" style=\'display:inline-block; margin-right:7px;margin-left:7px; background-color:#D8DD4F;width:12px;height:12px;border:1px solid #D8DD4F;\'></div>-->

            <p style="color: black;">Profile created</p>
            <p>* Profile submitted</p>

            </span>';
    //$time_end = microtime(true);
    //$html.= $time_end - $time_start;
    return $html;
}

function pro_update_section($op = null) {
    global $AccessObj;
    if (!in_array($AccessObj->user_type, array('super', 'admin'))) {
        drupal_set_message("You don`t have enough privileges");
        return '';
    }
    if ($op == 'update') {
        $admin_actions = $_REQUEST['admin_action'];
        $selected_uid = sget($_REQUEST, 'selected_records');
        if ($selected_uid == ',') {
            $selected_uid = '';
        }
        $UID = array();
        if ($selected_uid) {
            $A = explode(',', $selected_uid);
            foreach ($A as $id) {
                $UID[] = trim($id);
            }
        }
        if ($admin_actions == 'action_delete') {
            if (count($UID) > 0) {
                foreach ($UID as $uid) {
                    if ($uid == '') continue;
                    $sql = "SELECT mail FROM users WHERE uid = %d";
                    $result = db_query($sql, $uid);
                    $mail = db_fetch_object($result);
                    $sql0 = "DELETE FROM maenna_followers WHERE uid = %d;";
                    $sql1 = "DELETE FROM maenna_connections WHERE target_uid = %d OR assignee_uid = %d;";
                    $sql2 = "DELETE FROM users WHERE uid = %d";
                    if (!db_query($sql0, array($uid)) || !db_query($sql1, array($uid, $uid)) || !db_query($sql2, array($uid))) {
                        drupal_set_message("Error");
                        return;
                    }
                    if ($_REQUEST['reject_company'] == 'reject') {
                        $now = time();
                        $sql3 = "INSERT INTO rejected_users (user_email,rejected,rejected_by,user_type) VALUES ('%s','%s',%d,'%s')";
                        if (!db_query($sql3, array($mail->mail, $now, $AccessObj->uid, 'professional'))) {
                            drupal_set_message("Error");
                            return;
                        }
                    }
                }
                drupal_set_message("Operation Successful");
            }
        }
        if ($admin_actions == 'action_activate') {
            if (count($UID) > 0) {
                foreach ($UID as $uid) {
                    $sql = "UPDATE maenna_people SET active = NOT(active) WHERE pid = %d LIMIT 1";
                    if (!db_query($sql, array($uid))) {
                        drupal_set_message("Error");
                        return;
                    }
                }
                drupal_set_message("Operation Successful");
            }
        }
        if ($admin_actions == 'action_public') {
            if (count($UID) > 0) {
                $sql_where = implode(' or pid = ', $UID);
                $sql = "UPDATE maenna_people SET public = NOT(public) WHERE pid = NULL";
                $sql .= $sql_where;
                if (!db_query($sql)) {
                    drupal_set_message("Error");
                    return;
                }
                drupal_set_message("Operation Successful");
            }
        }
        if ($admin_actions == 'action_approve') {
            if (count($UID) > 0) {
                foreach ($UID as $uid) {
                    $sql = "UPDATE maenna_people SET approved = NOT(approved) WHERE pid = %d LIMIT 1";
                    if (!db_query($sql, array($uid))) {
                        drupal_set_message("Error");
                        return;
                    }
                }
                drupal_set_message("Operation Successful");
            }
        }
        if ($admin_actions == 'action_insights') {
            if (count($UID) > 0) {
                foreach ($UID as $uid) {
                    $sql = "UPDATE maenna_people SET insight_came_flag = NOT(insight_came_flag) WHERE pid = %d LIMIT 1";
                    if (!db_query($sql, array($uid))) {
                        drupal_set_message("Error");
                        return;
                    }
                }
                drupal_set_message("Operation Successful");
            }
        }
        if ($admin_actions == 'action_assign_admin') {
            require_once("classes/connections.inc");
            $admin = sget($_REQUEST, 'admin_id');
            if (count($UID) > 0) {
                foreach ($UID as $uid) {
                    if ($admin) {
                        if ($admin == 'none') {
                            $return = Connections::remove_assigned_admin($uid);
                        } else {
                            $return = Connections::assign_admin($uid, $admin);
                        }
                    }
                }
                if ($return) {
                    drupal_set_message("Operation Successful");
                } else drupal_set_message("Error Occurred");
            }
        }
        if ($admin_actions == 'action_match') {
            if (count($UID) > 0) {
                $cmp_id = sget($_REQUEST, 'cmp_id');
                foreach ($UID as $uid) {
                    $sql = "SELECT * FROM maenna_connections
                                WHERE conntype in ('match','visible','partner','advisor','collaborator','propose','client') AND target_uid = %d AND assignee_uid = %d and status = 'active'";
                    $db_query = db_query($sql, array($cmp_id, $uid));
                    if (db_fetch_object($db_query)) {
                        drupal_set_message('Professional is already matched or connected. Ignoring.');
                        continue;
                    }
                    
                    $sql = "INSERT INTO maenna_connections (conntype, target_uid, assignee_uid, access,status, editorid, edittime)
                                VALUES ('match', %d, %d, UNIX_TIMESTAMP(NOW()), 'active', %d, UNIX_TIMESTAMP(NOW()))";
                    
                    if (! db_query($sql, array($cmp_id, $uid, $AccessObj->uid))) {
                        drupal_set_message('An error occurred when trying to insert connection.', 'error');
                        return;
                    }
/*                    $sql = "INSERT INTO maenna_connections (conntype,target_uid,assignee_uid,access,status,editorid,edittime)
                                        VALUES ('match','" . (int) $cmp_id . "','" . (int) $uid . "',unix_timestamp(now()),'active','" . $AccessObj->uid . "',unix_timestamp(now()))";
                    if (!mysql_query($sql)) {
                        drupal_set_message("Error");
                        return;
                    }*/
                }
                drupal_set_message("Operation Successful");
            }
        }
        if ($_REQUEST['admin_action'] != ' ' && $_REQUEST['admin_action'] != '') header("Location: /account?tab=professionals");
    }
}

/* EOF */
