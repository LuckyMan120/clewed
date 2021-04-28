<?php
//error_reporting(0);
chdir('../../../');
require_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
error_reporting(E_ALL);
ini_set('display_errors', true);
ini_set('display_startup_errors', true);
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

define('__ACCOUNT__', 1);
require_once 'new_functions.inc';

if ($_REQUEST['type'] == 'pro_detail_short') {

    $sql = "SELECT profile FROM maenna_people where pid = %d LIMIT 1";
    $result = db_query ($sql,$_REQUEST['uid']);
    $Row = db_fetch_object($result);
    die($Row->profile);


}


if ($_REQUEST['type'] == 'pro_detail') {

    function getUserType($uid) {

        $q = mysql_query("SELECT pid FROM maenna_people WHERE pid = '".$uid."' ");

        if (mysql_num_rows($q) > 0 ) return 'people';

        else {
            $q1 = mysql_query("SELECT companyid FROM maenna_company WHERE companyid = '".$uid."' ");
            if (mysql_num_rows($q1) > 0 ) return 'company';
            else return 'admin';
        }
    }


    $base_url = $_REQUEST['base_url'];
    $user_id = $_REQUEST['uid'];

    $utype = getUserTypeById($_REQUEST['uid']);

    $user_sql = mysql_query ("SELECT * FROM maenna_people WHERE pid = '".$user_id."'");
    $user_data = mysql_fetch_array($user_sql);
    $people_pid = $user_data['pid'];
    $people_sql = mysql_query("select * from maenna_people_data where pid = '$people_pid' and data_type = 'addinfo' and data_attr = 'experties'");
    $people_data = mysql_fetch_array($people_sql);

    if ($user_data['username_type'] == 1) $username = ucfirst($user_data['firstname']);
    else $username = ucfirst($user_data['firstname']) . ' ' . ucfirst($user_data['lastname']);

    //$Id = $_REQUEST['id'];
    //$q1 = mysql_query("SELECT company_type FROM maenna_company WHERE companyid = $crId") or die(mysql_error());
    //$cmp_role_tmp = mysql_fetch_array($q1);
    //$cmp_role = $cmp_role_tmp['company_type'];
    //Check if user have a profile picture
    //$thumb = $base_url.'/sites/default/images/profiles/117x117/'.$user_id.'.jpg';
    //if (file_exists($thumb)) {
    $avatar = getAvatarUrl($user_id,"150");//$base_url.'/sites/default/images/profiles/117x117/'.$user_id.'.jpg'; //}
    //else if ($cmp_role == 'service') $avatar =$base_url.'/themes/maennaco/images/cmp-avatar-service.png';
    //else $avatar =$base_url.'/themes/maennaco/images/cmp-avatar-product.png';

    ?>
    <table width="100%">
        <tr>
            <?php
            $p_id = $user_id;
            $sql_profdetails = mysql_query ("SELECT * FROM  `maenna_people_data` WHERE `pid` = '" .$p_id ."' AND `data_attr` = 'experties'");
            $pdetails_res = mysql_fetch_array($sql_profdetails);
            $sql_inddetail = mysql_query ("SELECT * FROM  `maenna_people_data` WHERE `pid` = '" .$p_id ."' AND `data_attr` = 'industryview'");
            $pind_res = mysql_fetch_array($sql_inddetail);
            $sql_managdetail = mysql_query ("SELECT * FROM  `maenna_people_data` WHERE `pid` = '" .$p_id ."' AND `data_attr` = 'mgmtview'");
            $pmanage_res = mysql_fetch_array($sql_managdetail);

            $sql_edu = mysql_query ("SELECT data_value,data_value3 FROM  `maenna_people_data` WHERE `pid` = ".$p_id." and data_type = 'education' order by dataid desc limit 1");
            $edu_res = mysql_fetch_array($sql_edu);

            $graduate = $edu_res['data_value3'];
            $undergraduate = $edu_res['data_value'];

            if ($user_data['protype'] == 'other') $user_data['protype'] = 'Other Expert';
            else {
                if ($user_data['protype'] == 'executive'){
                    $user_data['protype'] = 'Operator';
                }
            }

            if ($undergraduate != '') $school = $undergraduate."<br>";
            if ($graduate != '') $school .= $graduate;
            ?>
            <td width="45%" valign="top">
                <!-- <img width="90px" height="90px" src="./sites/default/132076369_factory.jpg" /> -->
                <img src="<?php echo $avatar; ?>" width="90" height="90">
                <div style="font-family:Lato Italic; font-size:14px;" class="poptitle"><?php echo $username ?>, <?php echo ucwords($user_data['protype']);?></div>
                <div style="font-family:Lato Italic; font-size:14px;"><?=preg_replace('/(?<!\ )[A-Z]/', ' $0', $user_data['experties'])?></div>
                <div style="font-family:Lato Italic; font-size:14px;"><?=$school;?></div>
                </td>
                <br />
            <td>
                <?php
                //echo "<b>ABOUT</b><br />";
                if(!empty($user_data['profile']))
                {
                    echo "<b><div style='margin-bottom:10px;'>Summary</b></div>";
                    echo '<p id="info" style="line-height:20px;margin-top:0 !IMPORTANT;margin-right:10px;">';
                    echo $user_data['profile'];

                }
                if(!empty($pdetails_res['data_value2']))
                {
                    echo "<b><div style='margin-bottom:10px;'>Why you should listen to $username </div> </b>";
                    echo '<p id="info" style="line-height:20px;margin-top:0 !IMPORTANT;margin-right:10px;">';
                    $pdetails_res['data_value2'] = strip_tags($pdetails_res['data_value2'],"<strong><em>");
                    $pdetails_res['data_value2'] = html_entity_decode($pdetails_res['data_value2'],ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    echo $pdetails_res['data_value2'];
                    echo "</p>";

                }
                //echo "<b>ABOUT</b><br />";
                if(!empty($pind_res['data_value2']))
                {
                    echo "<b><div style='margin-bottom:10px;'>Industry view </div></b>";
                    echo '<p id="info" style="line-height:20px;margin-top:0 !IMPORTANT;margin-right:10px;">';
                    $pind_res['data_value2'] = strip_tags( $pind_res['data_value2'],"<strong><em>");
                    $pind_res['data_value2'] = html_entity_decode( $pind_res['data_value2'],ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    echo $pind_res['data_value2'];
                    echo "</p>";

                }
                if(!empty($pmanage_res['data_value2']))
                {
                    echo "<b><div style='margin-bottom:10px;'>Management view </div></b>";
                    echo '<p id="info" style="line-height:20px;margin-top:0 !IMPORTANT;margin-right:10px;">';
                    $pmanage_res['data_value2'] = strip_tags( $pmanage_res['data_value2'],"<strong><em>");
                    $pmanage_res['data_value2'] = html_entity_decode( $pmanage_res['data_value2'],ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    echo $pmanage_res['data_value2'];
                    echo "</p>";

                }
                ?>
                </p>


            </td>
        </tr>
    </table>

    <?php

    die();
} else {

    require_once __DIR__ . '/../blocks/insights/insights.php';

    $start = 0;
    $limit = 3;
    if ($_REQUEST['rel'] == 'show'){
        $start = $limit;
    }

    $cnt = renderInsightsPreview($user, $_REQUEST['category'], $_REQUEST['month'], $_REQUEST['sortdate'], 0, -1, $_REQUEST['offer-type']);

 if ($cnt > 0) echo '<a  class="show_more_cmp" style="width:155px !important;" onclick="showLoginDlg();">Login or register to see more</a>';
}
?>