<?php

chdir('../../../');
require_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

$return = menu_execute_active_handler();

if (is_int($return)) {
    switch ($return) {
        case MENU_NOT_FOUND:
            drupal_not_found();
            break;
        case MENU_ACCESS_DENIED:
            drupal_access_denied();
            break;
        case MENU_SITE_OFFLINE:
            drupal_site_offline();
            break;
    }
}

define("__ACCOUNT__",1);
require_once("themes/maennaco/includes/new_functions.inc");

if ($_REQUEST['type'] == 'pro_follow') {
    require_once("dbcon.php");

    $page = $_REQUEST['page'];
    $total = $_REQUEST['total'];

    $limit = "limit ".(($page -1)*6).", 6";

    $sql = "select  users.created as register,
                    users.uid,
                    maenna_people.*,
                    users_roles.rid,
                    mpd.expertise,
                    mc.target_uid as connected_company_id,
                    mcomp.projname

            from    users,
                    maenna_people LEFT JOIN (SELECT pid,data_value2 as expertise FROM maenna_people_data mp1d where data_type='addinfo' and data_attr = 'experties' and dataid = (SELECT dataid FROM maenna_people_data where data_type='addinfo' and data_attr = 'experties' and pid = mp1d.pid order by dataid DESC limit 1)

                    ) mpd ON maenna_people.pid = mpd.pid

                    LEFT JOIN  maenna_connections mc ON mc.connid = (SELECT connid FROM maenna_connections WHERE conntype = 'client' and status = 'active' and assignee_uid = mpd.pid LIMIT 1)
                    LEFT JOIN  maenna_company mcomp ON mcomp.companyid = mc.target_uid
                    ,
                    users_roles
                    where users.uid = maenna_people.pid and users.uid = users_roles.uid and users.status = 1 and maenna_people.public = 1 order by users.uid DESC
                   ";

    $sql .= " ".$limit;

    $result = db_query($sql);
//    echo '<pre>'; print_r($result); die;
    $_ProType = _pro_roles();
    // numeric index array
    $_Industry = _INDUSTRY();
    $_mLvl = _managementLevel();

    $iid = $_REQUEST['iid'];
    $itopic = $_REQUEST['itopic'];
    $itype = $_REQUEST['itype'];

    if (!empty($result)) {

        $i = 1;

        while (($row = db_fetch_array($result)) !== false) {

            $pid = sget($row, 'pid');
            $created = sget($row, 'register');
            $uid = $user -> uid;
            $foll_status = followerStatus($pid, $uid);
            $usrType = strtoupper(userType($pid));

            if ($i++ % 2 != 0) $first = 'first'; else $first = '';
            $avatar = getAvatarUrl($pid,"150");

            $row['experties'] = preg_replace('/(?<! )(?<!^)[A-Z]/',' $0', $row['experties']);
            $row['experties2'] = preg_replace('/(?<! )(?<!^)[A-Z]/',' $0', $row['experties2']);
            $row['experties3'] = preg_replace('/(?<! )(?<!^)[A-Z]/',' $0', $row['experties3']);

            //Get education data as it was too complicated to get it in parent query

            $sql1 = "SELECT data_value,data_value3 FROM maenna_people_data WHERE data_type = '%s' and pid = %d order by dataid DESC LIMIT 1";
            $result1 = db_query($sql1,array('education',$pid));
            $row1 = db_fetch_object($result1);

            $row['undergraduate'] = $row1->data_value3;
            $row['graduate'] = $row1->data_value;

            if (strlen($row['expertise']) >= 209 ) $row['expertise'] = substr(strip_tags($row['expertise']),0,209)."...";
            else $row['expertise'] = strip_tags($row['expertise']);

            if (strlen($row['profile']) >= 110 ) $row['profile'] = substr(strip_tags($row['profile']),0,110)."...";
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



            $content .=  '<div class="pro_card '.$first.'">

                    <a rel = "'.$pid.'" class="pro_popup" style="cursor:pointer;"><img width="50" height="50" src ="'.$avatar.'">

                    <div style="float:left;width:340px;"><div class="name_title">'.strtoupper($row['firstname']).', '.$usrType. $projectLink .'</div><br style="clear:both;"><div class="pro_summary">'.$row['profile'].'
                    </div></div>
                      <br style="clear:both">

<hr style="width:97%; background-color:#D0D2D3; margin-left:5px;margin-top:11px;">
<div class="pro_exp">'.($row['experties'] != '' ? $row['experties'] : '').($row['experties2'] != '' ? ", ".$row['experties2'] : '').($row['experties3'] != '' ? ", ".$row['experties3'] : '').'</div><div class="pro_exp">';
            if ($row['graduate'] != '') $content .= $row['graduate'];
            elseif ($row['undergraduate'] != '') $content .= $row['undergraduate'];
            $content .= '</div>
<hr style="width:97%; background-color:#D0D2D3; margin-left:5px;margin-top:11px;margin-bottom:11px;">
<div class="pro_summary" style="height:50px;margin-left:20px;">'.$row['expertise'].'
                    </div></a>';

            if ($_REQUEST['iid'] != '') {

                $value = '<a class="invite" pid = "'.$_REQUEST['pid'].'" itopic = "'.$itopic.'" uid = "' . $pid . '" pname="'.ucfirst($row['firstname']).'" type="'.$itype.'" m="'.md5($iid."kyarata75").'" iid="'.$iid.'"  class="tool">Invite</a>';
            } else {
            if ($foll_status)
                $value = '<a class="follow" title="Unfollow" type="unfollow" cid="' . $uid . '" uid = "' . $pid . '" class="tool">Following</a>';
            else
                $value = '<a class="follow" type="follow" cid="' . $uid . '" uid = "' . $pid . '"  class="tool">Follow</a>';
            }
            $content .= $value;
            if (proHasInsights($pid)) $content .='
<a class="insights" href="account?tab=professionals&page=pro_detail&id='.$pid.'&section=pro_industry_view&type=discussion" style="float:right !important;">Insights</a>';
            $content .= '</div>';
            if ($first == '') $content .= '<br style="clear:both;">';
        }



}

    if (($page)*6 < $total ) {

        if ($_REQUEST['iid'] != '') $invite_data = 'iid="'.$_REQUEST['iid'].'" itopic="'.$_REQUEST['itopic'].'" type="'.$_REQUEST['itype'].'" pid="'.$_REQUEST['pid'].'"'; else $invite_data = '';

        $content .= "<span ".$invite_data." total='".$total."' page='".++$page."' rel='show' class='show_more_pro'>view more</span>";
    }

   die($content);

}
?>