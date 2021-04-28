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

if ($_REQUEST['type'] == 'getPayments') {

    if ($_REQUEST['m'] != md5("kyarata75:" . $_REQUEST['uid'] . ':' . $_REQUEST['user_type'] . ':' . $_REQUEST['ins_id'])) return "false";
    else {
        $pro_id = (int) $_REQUEST['ins_id'];
        $db = \Clewed\Db::get_instance();

        $owner = $db->get_row("
            SELECT mpl.*
            FROM maenna_people mpl
            INNER JOIN maenna_professional mp ON mp.postedby = mpl.pid
            WHERE mp.id = :pro_id",
            array(':pro_id' => $pro_id)
        );

        $ownerId = $owner['pid'];
        $ownerReferralCode = $owner['code_to_refer'];

        if ($_REQUEST['utype'] != 'user') {

            $payments = $db->get_array("
                SELECT
                    (
                        SELECT SUM(amount)
                        FROM maenna_professional_payments
                        WHERE pro_id = :pro_id
                        AND status = 1
                    ) AS total,
                    mpp.*,
                    IF (
                        mc.companyid IS NULL,
                        IF (
                            mp.username_type = 1,
                            mp.firstname,
                            CONCAT(mp.firstname, ' ', mp.lastname)
                        ),
                        IF (
                            mc.projname <> '',
                            mc.projname,
                            CONCAT('PROJECT ', mc.companyid + 100)
                        )
                    ) AS display_name,
                    IF(mc.companyid IS NULL, 'people', 'company') AS user_type,
                    IF(mc.companyid IS NULL, '',ma.project) AS project,
                    IF(mc.companyid IS NULL, mp.gender,'') AS gender,
                    u.mail AS email,
                    IF(mc.companyid IS NULL, mp.referral_code, mc.referral_code) AS referral_code
                FROM maenna_professional_payments mpp
                LEFT JOIN maenna_people mp ON mpp.user_id = mp.pid
                LEFT JOIN maenna_company mc ON mpp.user_id = mc.companyid
                LEFT JOIN maenna_about ma ON mpp.user_id = ma.project_id
                LEFT JOIN users u ON mpp.user_id = u.uid
                WHERE pro_id = :pro_id AND mpp.status = 1",
                array(':pro_id' => $pro_id)
            );
        }
        else {
            $payments = $db->get_array("
                SELECT
                    (
                        SELECT SUM(amount)
                        FROM maenna_professional_payments
                        WHERE user_id = :user_id
                        AND status = 1
                    ) as total,
                    mpp.*,
                    IF (
                        mp.username_type = 1,
                        mp.firstname,
                        CONCAT(mp.firstname, ' ', mp.lastname)
                    ) as display_name,
                    mps.title,
                    mp.gender,
                    mps.postedby,
                    u.mail as email
                FROM maenna_professional_payments mpp
                LEFT JOIN maenna_professional mps ON mpp.pro_id = mps.id
                LEFT JOIN maenna_people mp ON mps.postedby = mp.pid
                LEFT JOIN users u ON mp.pid = u.uid
                WHERE user_id = :user_id and mpp.status = 1",
                array(':user_id' => $pro_id)
            );
        }
        if ($_REQUEST['utype'] != 'user') {
            $content = "<table style=width:800px;><thead><tr><th style='width:66px'>User</th><th style='width:160px;'>Name</th><th style='width:230px;max-width:230px;'>E-mail</th><th style='width:80px;padding-left:50px;'>Date</th><th style='width:50px;'>Discount</th><th style='width:60px;text-align:right;'>Amount</th></tr></thead>";
            foreach ($payments as $payment) {
                $timestamp = date('M d,y', $payment['date_created']);
                if ($payment['user_type'] == 'company') {
                    if (!empty($payment['project']) && file_exists('./themes/maennaco/images/project/' . $payment['project'])) {
                        $avatarFileName = "project/" . $payment['project'];
                        $avatar = "/themes/maennaco/phpthumb/phpThumb.php?src=../images/" . $avatarFileName . "&zc=1&w=40&h=40";
                    } else {
                        $avatar = ' /themes/maennaco/images/cmp-avatar-product.png';
                    }
                } else {
                    if (file_exists('sites/default/images/profiles/150x150/' . $payment['user_id'] . '.jpg')) {
                        $avatar = 'sites/default/images/profiles/150x150/' . $payment['user_id'] . '.jpg';
                    } else {
                        if ($payment['gender'] == 'm' || $payment['gender'] == '') {
                            $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                        } else
                            $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                    }
                }
                if ($payment['user_type'] == 'company')
                    $link = '/account?tab=companies&page=company_detail&id=' . $payment['user_id'];
                else
                    $link = '/account?tab=professionals&page=pro_detail&id=' . $payment['user_id'];
                /*if ($payment['user_type'] == 'people') $display_name = "<a href='#' id='pro_id" . $payment['user_id'] . "' ref='pro_id' style='color:#00a2bf;' class='profile_details'>" . ucfirst($payment['display_name']) . "</a>";
                else $display_name = ucfirst($payment['display_name']);*/
                //$display_name = "<span style='color:#00a2bf;'>".ucfirst($payment['display_name'])."</span>";

                $email = $payment['email'];
                if('people' == $_REQUEST['user_type'] || 'company' == $_REQUEST['user_type'])
                    $email = '';

                if($ownerId == $_REQUEST['uid'] && !empty($ownerReferralCode) && $ownerReferralCode == $payment['referral_code'])
                    $email = $payment['email'];

                $content .= "<tr style='height:70px;border-bottom:solid 1px #d0d2d3;'><td style='vertical-align:middle;'><a target='_blank' title='" . $payment['display_name'] . "' href='" . $link . "'>
                    <img src='" . $avatar . "' width='40px' height='40px'></a></td><td>" . $payment['display_name'] . "</td><td style='max-width:230px;' >" . $email . "</td><td style='padding-left:50px;vertical-align:middle;'>" . $timestamp . "</td><td>" . $payment['discount_rate'] . "%</td><td style='vertical-align:middle;text-align:right;padding-right:10px;'>$" . number_format($payment['amount'], 1) . "</td></tr>";

            }
            $content .= "<tr style='height:70px;border-bottom:solid 1px #d0d2d3;'><td style='vertical-align:middle;'>Total:</td><td></td><td></td><td></td><td></td><td style='vertical-align:middle;text-align:right;padding-right:10px;'>$" . number_format($payment['total'], 1) . "</td></tr></table>";
        }

        else {
            $content = "<table style=width:800px;><thead><tr><th style='width:66px'>User</th><th style='width:210px'>Title</th><th style='max-width:230px'>E-mail</th><th style='width:130px;padding-left:50px;'>Date</th><th style='width:50px'>Discount</th><th style='text-align:right;width:80px;'>Amount</th></tr></thead>";
            foreach ($payments as $payment) {
                if (strlen($payment['title']) > 30) $title = substr($payment['title'],0,30)."...";
                else $title = $payment['title'];
                $timestamp = date('M d,y', $payment['date_created']);
                    if (file_exists('sites/default/images/profiles/150x150/' . $payment['postedby'] . '.jpg')) {
                        $avatar = 'sites/default/images/profiles/150x150/' . $payment['postedby'] . '.jpg';
                    } else {
                        if ($payment['gender'] == 'm' || $payment['gender'] == '') {
                            $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                        } else
                            $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                    }

                $link = '/account?tab=professionals&page=pro_detail&id=' . $payment['postedby'];
                $ins_link = '/account?tab=professionals&page=pro_detail&id=' . $payment['postedby'].'&section=pro_industry_view&type=details&pro_id='.$payment['pro_id'];

                $content .= "<tr style='height:70px;border-bottom:solid 1px #d0d2d3;'><td style='vertical-align:middle;'><a target='_blank' title='" . $payment['display_name'] . "' href='" . $link . "'>
                    <img src='" . $avatar . "' width='40px' height='40px'></a></td><td><a target='_blank' title='" . $payment['title'] . "' href='" . $ins_link . "'>" . $title . "</a></td><td style='overflow-x:scroll;max-width:230px;' >" . $payment['email'] . "</td><td style='padding-left:50px;vertical-align:middle;'>" . $timestamp . "</td><td>" . $payment['discount_rate'] . "%</td><td style='padding-right:10px;text-align:right;vertical-align:middle;'>$" . number_format($payment['amount'], 1) . "</td></tr>";

            }
            $content .= "<tr style='height:70px;border-bottom:solid 1px #d0d2d3;'><td style='vertical-align:middle;'>Total:</td><td></td><td></td><td></td><td></td><td style='text-align:right;padding-right:10px;vertical-align:middle;'>$" . number_format($payment['total'], 1) . "</td></tr></table>";


        }

        die ($content);
    }

}

if ($_REQUEST['type'] == 'getReviews') {

    function ifAdmin ($crypt_uid) {

        $uid = decrypt_string($crypt_uid);

        $sql = "SELECT * FROM users u WHERE u.uid = ".(int) $uid." AND EXISTS (SELECT * FROM users_roles WHERE uid = u.uid AND rid IN (6,10))";

        $result = db_query($sql);

        if (mysql_num_rows($result) > 0 ) {return true;} else return false;
    }

    function getUserById($id)
    {
        if(empty($id)) return 'invalid id';
        $sql = mysql_query("SELECT rid FROM users_roles WHERE uid = '".$id."' LIMIT 1 ");
        $ridn = mysql_fetch_array($sql);
        if ($ridn['rid'] == '3') {

            $sql = "select users_roles.*, maenna_company.projname from users_roles, maenna_company where users_roles.uid = '".$id."' and maenna_company.companyid = '".$id."' limit 1";


        }
        else {

            $sql = "select users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname from users_roles, maenna_people where users_roles.uid = '".$id."' and maenna_people.pid = '".$id."' limit 1";

        }
        $result = mysql_query($sql);
        $Row = mysql_fetch_assoc($result);
        $rid = $ridn['rid'];
        $firstname = ucwords(strtolower($Row['firstname']));
        if ($rid == 6) $output = 'Admin';
        elseif ($rid == 10) $output = 'Clewed';
        elseif ($rid == "3") {
            if ($Row['projname'] != '')
                $output = strtoupper($Row['projname']);
            else $output = 'Project '.(string) ($id + 100);

        }
        else
        {
            $output = $firstname;
        }
        return $output;
    }

    $content = '<div style="margin-top:20px;" class="user_reviews">';
    $sql = "SELECT
                  ur.*,
                  mcv.title AS service_title,
                  mp.type as mp_type,
                  mp.title  AS insight_title
                FROM user_rating ur LEFT JOIN maenna_company_events mcv ON ur.service_id = mcv.eventid
                LEFT JOIN maenna_professional mp ON ur.insight_id = mp.id
                WHERE target_uid = " . (int)$_REQUEST['uid'] . "
                ORDER BY created DESC";
    $result = db_query($sql);
    while ($Data = db_fetch_object($result)) {

        $editor = getUserById($Data->editor_uid);

        $content .= "<div style='padding-bottom:20px; border-bottom:1px solid #F2F2F2;margin-left:15px;min-height:60px;margin-top:20px;' class='each_rating'>
<div class='rateit_rates' style='margin-top:4px;margin-right:20px;float:left;' data-tooltip='".$Data->rate_overall." star rating' data-rateit-readonly='true' data-rateit-starwidth='12' data-rateit-starheight='12'  data-rateit-value='".$Data->rate_overall."'></div>
";

        //Prepare title if rating is for service or isnight
        if (!is_null($Data->service_id) && $Data->service_id != 0)
        {
            $service_title = "<span style='line-height:20px;'>Service: &nbsp;<strong>$Data->service_title</strong></span><br>";
        }
        else if (!is_null($Data->insight_id) && $Data->insight_id != 0) {
            $post_type = ($Data->mp_type == 0) ? 'Insight' : 'Service';
            $service_title = "<span style='line-height:20px;'>$post_type: &nbsp;<strong>$Data->insight_title</strong></span><br>";
        }
        else $service_title = '';

        if ($Data->comment != '')
            $content .="<div style='float:left;width:520px;'>
$service_title
<span style='line-height:20px;' ".(ifAdmin($_REQUEST['user']) ? 'data-tooltip=\'Click to edit.\'' : '')." data-rid='$Data->id' data-m='".md5($Data->id."kyarata75")."' class='rate_comment'><span class='rate_comment_text'>".$Data->comment."</span></span><br>";
        $content .= "<span><strong>".$editor."</strong><br>".date("D, M j, Y g:i A T ",$Data->created)."</span></div><div style='clear:both;'></div></div>
                    ";
    }

    $content .= "</div>";
    if (ifAdmin($_REQUEST['user'])) {
        $content .= "<script type=\"text/javascript\">
                 $(document).ready(function(){

                    $(\".rate_comment\").click(function() {
                                        
                            if($(this).find('textarea').length > 0){
                            
                            return false;

                            }
                        
                            var name = $(this).text();
                            $(this).find('span').hide();
                            $('<textarea></textarea>')
                                .attr({
                                    'name': 'fname',
                                    'class': 'rate_comment_edit',
                                    'value': name,
                                    'style':'width:530px!important;height:auto !important;font-family:LatoRegular !important;font-size:0.9em !important;'
                                }).appendTo($(this));
                                
                            $('<a>Save</a>').attr({
                                    'class': 'tool save-btn',
                                    'style': 'float:right;margin-left:20px;',
                                    'href': '#'

                            }).click(function(e) {
                            
                            e.stopPropagation();
                        
                            cont = $(this).parent();
                            text = cont.find('textarea');
                                           
                            if (text.val().trim().length == 0) {
                                alert('Please help the community by sharing your experience in the comment section.');
                                cont.find('.cancel-btn').click();
                                }
                             else {
                                    
                                    		$.post('/themes/maennaco/includes/like.php?type=update_comment_rate_user', {
                                                    rid: cont.data('rid'),
                                                    m: cont.data('m'),
                                                    text: text.val()
                                                    
                                                },function(response){
                                                
                                                if (response == 'success') {
                                                
                                                cont.find('.rate_comment_text').html(text.val());
                                                cont.find('a.cancel-btn').click();
                                                
                                                }
                                                
                                                else alert('There was a problem. Please contact our administrator.');
                                                   
                                                });
                             
                             
                             }
   
                            }).appendTo($(this)).show('slow');
                            
                            $('<a>Cancel</a>').attr({
                                    'class': 'tool cancel-btn',
                                    'style': 'float:right;color:#00a2bf !important;',
                                    'href': '#'
                            
                            }).click(function(e) {
                            
                            e.stopPropagation();
 
                            cont = $(this).parent();
                            
                            cont.find('textarea').remove();
                            cont.find('a.tool').remove();
                            cont.find('.rate_comment_text').show();
                            })
                            .appendTo($(this)).show('slow');
                                
                            $(this).find('textarea').focus(); 
                        }); 
                        
                $('rate_comment').delegate('textarea','click',function() {
                                alert('click');
                                });
                     
                });

</script>";
    }
    die($content);


}

if (substr($_REQUEST['type'], 0, 3) == 'get') {
    function getProjectName($id, $isadmin = '') {
        if (empty($id)) return '';
        $id = (int) $id;
        $sql = "select * from maenna_company where companyid = $id";
        $result = db_query($sql);
        $Data = db_fetch_object($result);
        $projName = $Data->projname;
        if (empty($projName)) {
            $projName = "Project" . sprintf("%05s", $id + 100);
        }
        return strtoupper($projName);
    }

    function getUserById($id) {
        if (empty($id)) return 'invalid id';
        $id = (int) $id;
        $sql = mysql_query("SELECT rid FROM users_roles WHERE uid = '" . $id . "' LIMIT 1 ");
        $ridn = mysql_fetch_array($sql);
        if ($ridn['rid'] == '3') {
            $sql = "select users_roles.*, maenna_company.projname from users_roles, maenna_company where users_roles.uid = '" . $id . "' and maenna_company.companyid = '" . $id . "' limit 1";
        } else {
            $sql = "select users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname,maenna_people.lastname from users_roles, maenna_people where users_roles.uid = '" . $id . "' and maenna_people.pid = '" . $id . "' limit 1";
        }
        $result = mysql_query($sql);
        $Row = mysql_fetch_assoc($result);
        $rid = $ridn['rid'];
        $firstname = ucfirst($Row['firstname']);
        if (in_array($rid, array(6, 10))) {
            $output = "clewed";
        } elseif ($rid == "3") {
            $output = strtoupper($Row['projname']);
        } else {
            $output = ${firstname}; //.sprintf("%04s", $id +100);
        }
        return $output;
    }

    function getProExpertise($uid) {
        $sql = "select experties from maenna_people where pid = %d";
        $result = db_query($sql, array($uid));
        $Row = db_fetch_object($result);
        //die(test_array($Row));
        return preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $Row->experties);
    }

    function getUserTypeById($uid) {
        $query = "SELECT rid FROM users_roles WHERE uid = %d ";
        $result = db_query($query, array($uid));
        $Row = db_fetch_object($result);
        $rid = $Row->rid;
        if (in_array($rid, array(4, 5, 7, 8, 11, 12, 13, 14))) {
            return 'people';
        } elseif ($rid == 6) return 'admin';
        elseif ($rid == 10) return 'super_admin';
        elseif ($rid == 3) return 'company';
        return "invalid uid";
    }

    function getAvatarUrl($uid) {
        $usrType = getUserTypeById($uid);
        if ($usrType == 'people') {
            //Get user gender
            $query = "SELECT gender FROM maenna_people WHERE pid = %d";
            $result = db_query($query, array($uid));
            $gender_tmp = db_fetch_object($result);
            $gender = $gender_tmp->gender;
            //Check if user have a profile picture
            if (file_exists('sites/default/images/profiles/50x50/' . $uid . '.jpg')) {
                $avatar = 'sites/default/images/profiles/50x50/' . $uid . '.jpg';
            } else {
                if ($gender == 'm' || $gender == '') {
                    $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                } else {
                    $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                }
            }
        } elseif ($usrType == 'admin' || $usrType == 'super_admin') {

            $avatar = '/themes/maennaco/images/discussion_logo.png';
        }

        else {
            //Get cmp_role
            $query = "SELECT company_type,project  FROM maenna_company mc LEFT JOIN maenna_about ma
             ON mc.companyid = ma.project_id WHERE companyid = %d";
            $result = db_query($query, array($uid));
            $cmp_role_tmp = db_fetch_object($result);
            $cmp_role = $cmp_role_tmp->company_type;
            //Check if user have a profile picture
            if ($cmp_role_tmp->project != '' && file_exists('themes/maennaco/images/project/'.$cmp_role_tmp->project)) {
                $avatar = "/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/".$cmp_role_tmp->project."&zc=1&w=45&h=45";
            } else if ($cmp_role == 'service') {
                $avatar = ' /themes/maennaco/images/cmp-avatar-service.png';
            } else $avatar = ' /themes/maennaco/images/cmp-avatar-product.png';
        }
        return $avatar;
    }


    if ($_REQUEST['type'] == 'getCollaborators' || $_REQUEST['type'] == 'getFollowing') {
        function getFollowing($cid) {
            $q = mysql_query("SELECT * FROM maenna_followers WHERE cid = '" . ((int) $cid) . "'");
            $colls = array();
            if ($q) {
                while ($r = mysql_fetch_array($q)) $colls[] = $r['uid'];
                return $colls;
            }
        }

        function getCollaborators($cid) {
            $q = mysql_query("SELECT * FROM maenna_connections WHERE target_uid = '" . ((int) $cid) . "' and conntype='collaborator' and status <> 'deactivated'");
            $tmp1 = array();
            $tmp2 = array();
            $colls = array();
            if ($q) {
                while ($r = mysql_fetch_array($q)) if ($r['status'] != 'deactivated') $colls[$r['status']][] = $r['assignee_uid'];
                return $colls;
            }
        }

        if ($_REQUEST['type'] == 'getCollaborators') {
            $pros = getCollaborators($_REQUEST['companyId']);
            $pros = $pros['active'];
            if ($_REQUEST['ref'] != '') $ref2 = "&ref=" . $_REQUEST['ref'] . "&ref2=Collaborators";
        } elseif ($_REQUEST['type'] == 'getFollowing') {
            $pros = getFollowing($_REQUEST['companyId']);
            if ($_REQUEST['ref'] != '') $ref2 = "&ref=" . $_REQUEST['ref'] . "&ref2=Following";
        }
        foreach ($pros as $Pro) {
            $pro_uid = $Pro;
            $avatar = getAvatarUrl($pro_uid);
            $pro_maeid = getUserById($pro_uid);
            $pro_exp = getProExpertise($pro_uid);
            if ($_REQUEST['perm'] == 'read') {
                $box_content .= "
                    <div style=\"height:55px;\" class='row singlerow'>
                        <img src=\"" . $avatar . "\" style=\"float:left;margin-right:5px; width:50px; height:50px;\">
                        <p style=\"font-size:11px!important;margin-top:9px !important; float:left; margin-right:7px; height:auto;width:100px;overflow:hidden;text-overflow: ellipsis;\">$pro_maeid<br> $pro_exp</p>";
            } else {
                $box_content .= "
                    <div style=\"height:55px;\" class='row singlerow'>
                        <img src=\"" . $avatar . "\" style=\"float:left; margin-right:5px; width:50px; height:50px;\">
                        <a style=\"cursor: pointer; font-size:11px!important;margin-top:9px !important; float:left; margin-right:7px; height:auto;width:100px;overflow:hidden;text-overflow: ellipsis;\" onclick='showExpertInfo($pro_uid);'> $pro_maeid<br>$pro_exp</a>";
            }

            if('write' == $_REQUEST['requestPermA'] && $_REQUEST['type'] == 'getCollaborators') {
                $box_content .= "<a style='margin-top:0; color:#00a3bf; font-size:12px;text-transform: uppercase;margin-right:5px;height:auto;display: inline-block;'
                                        title='Approved experts can create and post services in the service page of this project as active team members. Click to approve.'
                                        href='javascript:addAdvisor({$_REQUEST['companyId']}, $pro_uid);'>a</a>";
            }

            if('write' == $_REQUEST['requestPermX'] && $_REQUEST['type'] == 'getCollaborators') {
                $box_content .= "<a style='margin-top:0; color:red; font-size:15px; height:auto;display: inline-block;'
                                            title='Use this tool to delete this expert from Interested section. Please confirm to proceed.'
                                            href='javascript:remCollab({$_REQUEST['companyId']}, $pro_uid);'>x</a>";
            }

            $box_content .= '</div>';

        }
        if ($box_content == '') $box_content = 'No users';
        die($box_content);
    } elseif ($_REQUEST['type'] == 'getProjects' || $_REQUEST['type'] == 'getConnected' || $_REQUEST['type'] == 'getManagement') {

        try {
            $id = $_REQUEST['proId'];
            require_once("themes/maennaco/includes/classes/connections.inc");
            $Conns = Connections::Pro_conns($id);
            ini_set('display_errors', '1');
            ini_set('display_startup_errors', '1');
            error_reporting(E_ALL);

            // Class 'Twig_Environment' not found
//            $twig = new Twig_Environment( new Twig_Loader_Filesystem(ROOT . '/templates'));

            if ($_REQUEST['type'] == 'getProjects') {
                $cmps = $Conns['Advisor'];
                $ref = 'Active';
            } elseif ($_REQUEST['type'] == 'getManagement') {
                $cmps = $Conns['Client'];
            } else {
                $cmps = $Conns['Visible'];
                $ref = 'Connected';
            }
            foreach ($cmps as $Proj) {
                $target_uid = $Proj->target_uid;
                $avatar = getAvatarUrl($target_uid);
                $maenna_id = getProjectName($target_uid);
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'>
            <a class='pro-card-tool' pro-card-tooltip=\"$target_uid\" style=\"margin - top:20px;\" href='/account?tab=companies&page=company_detail&id=$target_uid&closebtn=1&ref=$ref&show_type=1'><img src=\"" . $avatar . "\" style=\"float:left; margin-right:5px; width:50px; height:50px;\">
            " . ucwords(strtolower($maenna_id)) . "</a>
            </div>";
            }
            if ($box_content == '') $box_content = 'No users';

        } catch (Exception $exception){
            echo $exception->getMessage();
        }
        die($box_content);
    } elseif ($_REQUEST['type'] == 'getCollaborating' || $_REQUEST['type'] == 'getFollowers' || $_REQUEST['type'] == 'getRelated' || $_REQUEST['type'] == 'getProFollowing' || $_REQUEST['type'] == 'getInvited' || $_REQUEST['type'] == 'getPartners' || $_REQUEST['type'] == 'getInvestors'|| $_REQUEST['type'] == 'getProInvestors') {
        $id = $_REQUEST['proId'];

		function getInvestors($id) {
			$investorPros = array();
			$sql = "select * from maenna_connections where (target_uid = '%s' or assignee_uid = '%s') and conntype='investor' and status = 'active'";
			$result = db_query($sql, $id, $id);
			while ($Row = db_fetch_object($result)) {
				if ($Row->target_uid == $id) {
					$investorPros[] = $Row->assignee_uid;
				} else $investorPros[] = $Row->target_uid;
			}
			return $investorPros;
		}

        function getInvited($uid) {
            $q = mysql_query("
                select uid
                FROM users u
                LEFT JOIN maenna_people mp on u.uid = mp.pid
                LEFT JOIN maenna_company mc ON u.uid = mc.companyid
                WHERE (
                    mp.referral_code = (
                        SELECT code_to_refer
                        FROM maenna_people
                        WHERE pid = '" . ((int) $uid) . "')
                    OR mc.referral_code = (
                        SELECT code_to_refer
                        FROM maenna_people
                        WHERE pid = '" . ((int) $uid) . "')
                )
                and u.status = 1
                and NOW() < DATE_ADD(FROM_UNIXTIME(u.created), INTERVAL 1 YEAR)
                ");
            $colls = array();
            if ($q) {
                while ($r = mysql_fetch_array($q)) $colls[] = $r['uid'];
                return $colls;
            }
        }
        function getFollowers($uid) {
            $q = mysql_query("SELECT * FROM maenna_followers WHERE uid = '" . ((int) $uid) . "'");
            $colls = array();
            if ($q) {
                while ($r = mysql_fetch_array($q)) $colls[] = $r['cid'];
                return $colls;
            }
        }

        function getRelated($id) {
            $relatedPros = array();
            $sql = "select * from maenna_connections where (target_uid = '%s' or assignee_uid = '%s') and conntype='related' and status = 'active'";
            $result = db_query($sql, $id, $id);
            while ($Row = db_fetch_object($result)) {
                if ($Row->target_uid == $id) {
                    $relatedPros[] = $Row->assignee_uid;
                } else $relatedPros[] = $Row->target_uid;
            }
            return $relatedPros;
        }

        function getCollaborationCompanies($uid) {
            $q = mysql_query("SELECT * FROM maenna_connections WHERE assignee_uid = '" . ((int) $uid) . "' and status <> 'deactivated' and conntype='collaborator'");
            $tmp1 = array();
            $tmp2 = array();
            $colls = array();
            if ($q) {
                while ($r = mysql_fetch_array($q)) if ($r['status'] != 'deactivated') $colls[$r['status']][] = $r['target_uid'];
                return $colls;
            }
        }

        if ($_REQUEST['type'] == 'getProFollowing') {
            function getFollowing($cid) {
                $q = mysql_query("SELECT * FROM maenna_followers WHERE cid = '" . ((int) $cid) . "'");
                $colls = array();
                if ($q) {
                    while ($r = mysql_fetch_array($q)) $colls[] = $r['uid'];
                    return $colls;
                }
            }
            $pro_exp = getProExpertise($_REQUEST['proId']);

            $pros = getFollowing($_REQUEST['proId']);

            foreach ($pros as $Pro) {
                $ref = 'pro_following';
                $target_uid = $Pro;
                $avatar = getAvatarUrl($target_uid);
                $maenna_id = getUserById($target_uid);
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src='" . $avatar . "' style=\"float:left; margin-right:5px; width:50px; height:50px;\"><a style=\"cursor: pointer;margin-top:20px;\" onclick='showExpertInfo($target_uid);'>" . ucwords(strtolower($maenna_id)) . "<br>".$pro_exp."</a></div>";
            }
        }

		if ($_REQUEST['type'] == 'getProInvestors') {
			function getCompanyInvestors($cid) {
				$q = mysql_query("SELECT * FROM maenna_connections WHERE target_uid = '" . ((int) $cid) . "' and conntype='investor' and status <> 'deactivated'");
				$tmp1 = array();
				$tmp2 = array();
				$colls = array();
				if ($q) {
					while ($r = mysql_fetch_array($q)) if ($r['status'] != 'deactivated') $colls[$r['status']][] = $r['assignee_uid'];
					return $colls;
				}
			}

			$pros = getCompanyInvestors($_REQUEST['companyId']);
			$pros = $pros['active'];

			foreach ($pros as $Pro) {
				$ref = 'pro_investors';
				$target_uid = $Pro;
				$pro_exp = getProExpertise($target_uid);
				$avatar = getAvatarUrl($target_uid);
				$maenna_id = getUserById($target_uid);
				$box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src='" . $avatar . "' style=\"float:left; margin-right:5px; width:50px; height:50px;\"><a style=\"cursor: pointer;margin-top:20px;\" onclick='showExpertInfo($target_uid);'>" . ucwords(strtolower($maenna_id)) . "<br>".$pro_exp."</a></div>";
			}
		}

        if ($_REQUEST['type'] == 'getPartners') {
            function getPartners($pid) {
                $q = mysql_query("SELECT * FROM maenna_connections WHERE assignee_uid = '" . ((int) $pid) . "' and conntype = 'partner' and status = 'active' ");
                $colls = array();
                if ($q) {
                    while ($r = mysql_fetch_array($q)) $colls[] = $r['target_uid'];
                    return $colls;
                }
            }
            $pro_exp = getProExpertise($_REQUEST['proId']);

            $pros = getPartners($_REQUEST['proId']);

            foreach ($pros as $Pro) {
                $ref = 'pro_partner';
                $target_uid = $Pro;
                $avatar = getAvatarUrl($target_uid);
                $maenna_id = getUserById($target_uid);
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src='" . $avatar . "' style=\"float:left; margin-right:5px; width:50px; height:50px;\"><a href=\"/account?tab=companies&page=company_detail&id=$target_uid&closebtn=1&ref=\"style=\"cursor: pointer;margin-top:20px;\">" . ucwords(strtolower($maenna_id)) . "</a></div>";
            }
        }

        if ($_REQUEST['type'] == 'getCollaborating') {
            $cmps = getCollaborationCompanies($id);
            $cmps = $cmps['active'];
            $ref = 'Contributing';
        } elseif ($_REQUEST['type'] == 'getFollowers') {
            $cmps = getFollowers($id);
            $ref = 'Following';
        } elseif ($_REQUEST['type'] == 'getInvited')
		{
			$cmps = getInvited($id);
			$ref = 'Invitations';
		}elseif ($_REQUEST['type'] == 'getInvestors') {
			$cmps = getInvestors($id);
			$ref = 'Investors';
        } elseif ($_REQUEST['type'] == 'getRelated') $cmps = getRelated($id);

        foreach ($cmps as $Proj) {
            $target_uid = $Proj;
            $usr_type = getUserTypeById($target_uid);
            $avatar = getAvatarUrl($target_uid);
            echo "d".$target_uid;
            if ($usr_type != 'people') {
                $maenna_id = getProjectName($target_uid);

                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'>
                <a style=\"margin - top:20px;\" href='/account?tab=companies&page=company_detail&id=$target_uid&closebtn=1&ref=$ref'>
                <img src='" . $avatar . "' style=\"float:left; margin-right:5px; width:50px; height:50px;\">" . ucwords(strtolower($maenna_id)) . "</a></div>";
            }
            else {
                $maenna_id = getUserById($target_uid);
                $pro_exp = getProExpertise($target_uid);
                $box_content .= "\n<div style=\"height:55px;\" class='row singlerow'><img src='" . $avatar . "' style=\"float:left; margin-right:5px; width:50px; height:50px;\"><a style=\"cursor: pointer; margin-top:20px;\" onclick='showExpertInfo($target_uid);'>" . ucwords(strtolower($maenna_id)) . "<br>".$pro_exp."</a></div>";
            }
        }
        if ($box_content == '') $box_content = 'No users';
        die($box_content);
    }
}
if ($_REQUEST['type'] == 'changeProjectName') {
    if (md5($_REQUEST['cid'] . 'kyarata75') == $_REQUEST['m']) {
        $sql = "select projname from maenna_company WHERE projname = '%s'";
        $result = db_query($sql, array($_REQUEST['proj_name']));
        if ($result) {
            if (mysql_num_rows($result) == 0) {
                $sql = "update maenna_company SET projname = '%s' WHERE companyid = %d ";
                $result = db_query($sql, array($_REQUEST['proj_name'], $_REQUEST['cid']));
                if ($result) {
                    die('Project name changed successfully');
                } else die('Error happened.Please try again');
            } else die('Project name is already taken. Try another one');
        }
    } else die('Authentication problem!');
}
if ($_REQUEST['type'] == 'checkApproved') {
    $selected_uid = substr($_REQUEST['uids'], 1);
    $sql = "select pid FROM maenna_people WHERE approved <> 1 and pid in($selected_uid)";
    $result = mysql_query($sql);
    while ($row = mysql_fetch_array($result)) {
        $notApp[] = $row['pid'];
    }
    if (count($notApp) != 0) $status = 'false'; else $status = true;
    $notApp['status'] = $status;
    die(json_encode($notApp));
}
//todo: remove this check
if ($_REQUEST['type'] == 'checkCompleted') {

    if ($_REQUEST['utype'] == 'people') {
        die('true');
    } elseif ($_REQUEST['utype'] == 'company') {
        die('true');
    } else {
        die("Admin cannot join Insights. Create a profile to join.");
    }
}
