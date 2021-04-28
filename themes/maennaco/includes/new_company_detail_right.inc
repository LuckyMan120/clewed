<?php

/**
 * @param Maenna_access $AccessObj
 * @param array $selectedOptions
 * @param $company_id
 * @return array
 */
function fundraising_statistics(Maenna_access $AccessObj, array $selectedOptions, $company_id)
{
    $output = '<input type="hidden" id="user_id" value="' . $AccessObj->uid . '">';
    $output .= '<div class="sidebar_box company_analysis">';
    // $output .= '<div class="wrapper" style="margin-bottom: -20px;">';
    $output .= '<div class="wrapper">';

    // Raised amount
    if (!empty($selectedOptions[0]['round_amount_raising'])) {
        $amount_raising = '$' . number_format($selectedOptions[0]['round_amount_raising'], 0, '.', ',');
        $output .= '<div class="fieldset">';
        //        $goal_amount = 1e8;
        $goal_amount = $selectedOptions[0]['round_amount_raising'];
        //    $str_goal_amount = '$'.number_format($goal_amount, 0, '.', ',');

        $sql = "select sum(amount) sum from maenna_professional_investment where company_id=%d and status=3";
        $result = db_query($sql, array($company_id));
        $row = db_fetch_array($result);
        $total_amount = is_null($row['sum']) ? 0 : $row['sum'];
        //        $amount_raising_percent = $selectedOptions[0]['round_amount_raising'] / $goal_amount * 100;
        $amount_raising_percent = $total_amount / $goal_amount * 100;
        $output .= '<div class="amount raised-amount">' . $amount_raising . '</div>';
        $output .= '<div class="status_bars_wrapper"><div class="status-bar-wrapper"><div class="status-bar"></div><div class="status-bar-progress" style="width: ' . $amount_raising_percent . '%"></div></div></div>';
        //    $output .=     '<div class="status-label">As of '.$str_goal_amount.' Goal</div>';
        $output .= '<div class="status-label">Amount raising in this round</div>';
        $output .= '</div>';
    }

    // Offered in this round
    if (!empty($selectedOptions[0]['percent_ownership_offered'])) {
        $percent_ownership_offered = (float)$selectedOptions[0]['percent_ownership_offered'];
        $output .= '<div class="fieldset">';
        $output .= '<div class="amount percentage-offered">' . number_format($amount_raising_percent, 0, '.', ',') . '%</div>';
        $output .= '<div class="status-label">Offered in this round</div>';
        $output .= '</div>';
    }

    // Days to go
    $close_date = $selectedOptions[0]['close_date'];

    if (!empty($close_date)) {
        $date2 = date_create($close_date);
        $date1 = date_create('now');

        $interval = date_diff($date1, $date2);
        // + or -
        $days_to_go_symbol = $interval->format('%R');
        $days_to_go = $interval->format('%a');
        if ($days_to_go_symbol == '+') {
            $output .= '<div class="fieldset">';
            $output .= '<div class="amount days-to-go">' . $days_to_go . '</div>';
            $output .= '<div class="status-label">Days to go</div>';
            $output .= '</div>';
        } else {
            // Now it is already pst the Close Date
            //        $percent_ownership_offered = (float)$selectedOptions[0]['percent_ownership_offered'];
            //
            //        $output .=   '<div class="fieldset">';
            //        $output .=     '<div class="amount percentage-offered">'.number_format($percent_ownership_offered, 0, '.', ',').'%</div>';
            //        $output .=     '<div class="status-label">Offered in this round</div>';
            //        $output .=   '</div>';
            $date = date($close_date);
            $str = date("M d, Y", strtotime($date));
            $output .= '<div class="fieldset">';
            $output .= '<div class="amount days-to-go">' . $str . '</div>';
            $output .= '<div class="status-label" style="color: #929497;">Close Date</div>';
            $output .= '</div>';
        }
    }

    // Security type
    $security_type = $selectedOptions[0]['security_type'];
    if (!empty($security_type)) {
        $output .= '<div class="fieldset">';
        $output .= '<div class="amount percentage-offered">' . $security_type . '</div>';
        $output .= '<div class="status-label">Security type</div>';
        $output .= '</div>';
    }

    // Interest rate
    $interest_rate = $selectedOptions[0]['interest_rate'];
    if (!empty($interest_rate)) {
        $output .= '<div class="fieldset">';
        $output .= '<div class="amount percentage-offered">' . number_format($interest_rate, 2, '.', ',') . '%</div>';
        $output .= '<div class="status-label">Interest rate</div>';
        $output .= '</div>';
    }

    // Funding Purpose
    if (!empty($selectedOptions[0]['funding_purpose'])) {
        $funding_purposes = get_funding_purposes_list();
        $key = $selectedOptions[0]['funding_purpose'];
        $str_funding_purpose = 'N/A';
        if ($key && isset($funding_purposes[$key])) {
            $str_funding_purpose = $funding_purposes[$key];
        }

        $label = 'Funding Purpose';
        $output .= '<div class="fieldset">';
        $output .= '<div class="status-label funding-purpose--label">' . $label . '</div>';
        $output .= '<div class="status-label funding-purpose--value">' . $str_funding_purpose . '</div>';
        $output .= '</div>';
    }
    return $output;
}

function fundraising_section_pro($op = null)
{
    global $AccessObj;
    $company_id = $_REQUEST['id'];
    $selectedOptions = getSelectedCompanyPage2($company_id);
    if (empty($selectedOptions)) {
        // ERROR
        return '';
    }

    // If this section is disabled in the admin section, do not show this.
    $is_fundraising_enabled = $selectedOptions['fundraising'];
    if (!$is_fundraising_enabled) {
        // Fundraising section is disabled for this company
        return '';
    }
    require_once 'themes/maennaco/includes/company_fundraising_analysis.inc';
    echo fundraising_statistics($AccessObj, $selectedOptions, $company_id) . render_company_fundraising_analysis();
}

function fundraising_section_cmp($op = null)
{
    global $AccessObj;
    $company_id = $_REQUEST['id'];
    $selectedOptions = getSelectedCompanyPage2($company_id);
    if (empty($selectedOptions)) {
        // ERROR
        return '';
    }

    // If this section is disabled in the admin section, do not show this.
    $is_fundraising_enabled = $selectedOptions['started_fundraising'];
    if (!$is_fundraising_enabled) {
        // Fundraising section is disabled for this company
        return '';
    }
    require_once 'themes/maennaco/includes/explore_capital_raising.inc';
    echo fundraising_statistics($AccessObj, $selectedOptions, $company_id) .explore_capital_raising();
}

function cmp_GettingStarted($op = null)
{

    require_once 'themes/maennaco/includes/get_started.inc';
    echo get_started();

    global $user;
    if ($user->uid == $_REQUEST['id'] && !isset($_SESSION['q-popup']) && $user->first_time == '1') {
        $_SESSION['q-popup'] = 1;
        user_save($user, array('first_time' => '0'));
        $js_popup = <<< END
          <div id="q-popup">
          Please complete the about section of your profile to get started.
          </div>
          <script type="text/javascript">
            $('#q-popup').dialog({
              autoOpen: 1,
              modal: 1,
              draggable: true,
              resizable: false
            });
          </script>
END;
        print $js_popup;
    }
}

function renderTopicBlock($Row, $mtab, $base_url, $companyid, $an_id)
{
    $title = trim($Row->data_value);

    if (empty($title)) {
        return;
    }

    $title_limit = 75;
    $long_title = strlen($title) > $title_limit ? '...' : '';

    $dataid = $Row->dataid;

    $title = ucwords(strtolower(htmlentities(substr($title, 0, $title_limit), ENT_QUOTES | ENT_IGNORE, "UTF-8")));
    $title1 = renderTopicUrl($mtab, $base_url, $companyid, $dataid, $title . $long_title);

    $bg_color = '';
    if (isset($an_id) && $an_id == $dataid) {
        $bg_color = 'background-color:#e3eef2!important';
    }
    return "\n<div class='row' style='padding:15px 0; border-bottom: solid 1px #f0f2f3; font-family: \"LatoRegular\"; " . $bg_color . "'>$title1</div>";
}

function sections($op = null)
{
    global $user;
    global $AccessObj;
    global $redirect;
    global $base_url;
    $editorid       = $user->uid;
    $companyid      = sget($_REQUEST, 'id');
    $Block['title'] = '<span style="font-family: Lato Bold Italic !important;">Topics</span>';
    $content        = '';
    $mtab           = sget($_REQUEST, 'mtab');

    if (empty($mtab)) {
        foreach ($AccessObj->Com_sections as $tab_name => $Tab_data) {
            if ($Tab_data['access'] != 'hide' && $tab_name != 'common') {
                $mtab = $tab_name;
                break;
            }
        }
    }

    $redirect = rebuild_url(array('tab', 'page', 'id')) . "&mtab=$mtab";

    //test_array($AccessObj->Com_sections);
    //echo userType($AccessObj->uid);
    $orderby = 'desc';

    switch ($mtab) {
        case "analysis":
            $data_type = 'analysis';
            $orderby   = '';
            break;
        case "share_knowledge":
            $data_type = 'share_knowledge';
            $orderby   = 'desc';
            break;
        case "plan":
            $data_type = 'plan';
            $orderby   = '';
            break;
        case 'minutes':
            $an_id = sget($_REQUEST, 'dataid');
            $data_type = 'minutes';
            break;
        case "performance":
            $data_type = "plan";
            break;
        case "advice":
            $an_id = sget($_REQUEST, 'view_id');
            $data_type = 'advice';
            break;
        case "summary":
            $an_id = sget($_REQUEST, 'analysis_id');
            if (!empty($an_id)) {
                $data_type = "analysis";
                $orderby   = '';
            } else {
                echo 'no an id';
                return;
            }
            break;
        case "share_knowledge_details":
            $an_id = sget($_REQUEST, 'data_id');
            if (!empty($an_id)) {
                $data_type = "share_knowledge";
                $orderby   = 'desc';
            } else {
                echo 'no an id';
                return;
            }
            break;
        default:
            //            echo "invalid mtab";
            return;
    }

    if ($op && $op != 'update' and $data_type != 'advice') {

        $authorIsCurrentUser = " editorid='{$user->uid}' ";
        $authorIsCompany = " editorid='{$companyid}' ";
        $authorIsAdmin = " editorid IN(SELECT DISTINCT(uid) FROM users_roles WHERE rid IN(6, 10)) ";
        $noInvites = " (SELECT COUNT(*) FROM discussion_invites WHERE discussion_id = dataid) <= 0 ";
        $isBroadcast = " ({$authorIsCompany} OR {$authorIsAdmin}) AND {$noInvites} ";
        $isAdmin = " (SELECT uid FROM users_roles WHERE rid IN(6, 10) AND uid={$user->uid} LIMIT 1) IS NOT NULL ";
        $isInvited = " (SELECT user_id FROM discussion_invites WHERE discussion_id = dataid AND user_id='{$user->uid}') IS NOT NULL ";

        $isCompany = $companyid == $user->uid;
        $isColleague = false;
        $connections = Connections::Com_conns($companyid);
        foreach ($connections['Client'] as $connection)
            if ($connection->assignee_uid == $user->uid)
                $isColleague = true;

        $isCompanyStaff = ' 2=1 ';
        if ($isCompany || $isColleague)
            $isCompanyStaff = " {$noInvites} ";

        $contentReducer = " AND (
            {$isAdmin} OR
            {$authorIsCurrentUser} OR
            {$isBroadcast} OR
            {$isInvited} OR
            {$isCompanyStaff}
        )";

        $sql    = "select * from maenna_company_data where companyid = %d $contentReducer and data_type = '%s' and deleted != 1 order by dataid $orderby limit 10";
        $result = db_query($sql, array($companyid, $data_type));

        $counter = 1;
        while ($Row = db_fetch_object($result)) {
            $content .= renderTopicBlock($Row, $mtab, $base_url, $companyid, $an_id);
            $counter++;
        }

        /**
         * START "More block" logic
         * TODO: Refactor to make single DB query
         */
        if ($counter >= 11) {
            $content .= "<span id='showmoretopics'><a href='#' onclick='showmoretopics();return false;' style='color:#00A3BF;'>More</a></span><span id='showlesstopics' style='display:none;'><a href='#' onclick='showlesstopics();return false;' style='color:#00A3BF;'><strong>Less</strong></a></span>";
        }

        $content .= "<div id='showmore' style='display:none;'>";
        $sql     = "select * from maenna_company_data where companyid = %d and data_type = '%s' and deleted != 1 order by dataid $orderby limit 10,99";
        $result  = db_query($sql, array($companyid, $data_type));
        $counter = 1;

        while ($Row = db_fetch_object($result)) {
            $content .= renderTopicBlock($Row, $mtab, $base_url, $companyid, $an_id);
            $counter++;
        }
        /**
         * END "More block" logic
         */

        $content .= "</div>";
    }
    elseif ($data_type == 'advice') {

        if ($AccessObj->user_type == 'super') {
            $result1 = mysql_query("SELECT * FROM maenna_company_events mce WHERE  companyid = '" . $companyid . "' ORDER BY datetime DESC");
        } else {

            $companyService = new Clewed\Company\Service();

            $result1 = mysql_query("
                SELECT *
				FROM  `maenna_company_events` mce
				WHERE (
                  " . (int) $user->uid . " IN (
                        SELECT uid
                        FROM maenna_company_events_inv
                        WHERE eventid = mce.eventid
                        AND status <> 'declined'
                    )
                    AND 1 = CASE
                                WHEN ('" . (int)$user->uid . "' IN (" . implode(',', $companyService->getColleagueIds($_REQUEST['id'])) . ") AND mce.approved = 0) THEN 0
                                ELSE 1
                            END
                    AND companyid = '{$companyid}'
                ) OR (
                    postedby = '{$user->uid}'
                    AND companyid = '{$companyid}'
                )
                OR (
                    executor_id = '{$user->uid}'
                    AND (executor_status <> 'declined' OR executor_status IS NULL)
                    AND companyid = '{$companyid}'
                    AND approved = 1
                )
                OR (
                    companyid = '{$companyid}'
                    AND companyid = '{$user->uid}'
                    AND approved = 1
                )
                ORDER BY datetime DESC ");
        }

        while ($Row = db_fetch_object($result1)) {
            $Row->dataid = $Row->eventid;
            $Row->data_value = $Row->title;
            $content .= renderTopicBlock($Row, $mtab, $base_url, $companyid, $an_id);
        }
        $content .= "</div>";
    }

    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

/**
 * @param $mtab
 * @param $base_url
 * @param $companyid
 * @param $dataid
 * @param $title
 * @return string
 */
function renderTopicUrl($mtab, $base_url, $companyid, $dataid, $title)
{
    $section = $mtab;

    switch ($mtab) {
        case "analysis":
        case "summary":
            $title = "<a href='$base_url/account?tab=companies&page=company_detail&id=" . $companyid . "&mtab=summary&type=details&analysis_id=" . $dataid . "'>" . $title . "</a>";
            break;
        case "share_knowledge":
        case "share_knowledge_details":
            $title = "<a href='$base_url/account?tab=companies&page=company_detail&id=" . $companyid . "&mtab=share_knowledge_details&type=details&data_id=" . $dataid . "'>" . $title . "</a>";
            break;
        case 'minutes':
        case "performance":
            $title = "<a href='$base_url/account?tab=companies&page=company_detail&id=" . $companyid . "&mtab=$mtab&panel=multi&view=detail&dataid=" . $dataid . "'>" . $title . "</a>";
            break;
        case "advice":
            $title = "<a href='$base_url/account?tab=companies&page=company_detail&id=" . $companyid . "&mtab=advice&view_id=" . $dataid . "'>" . $title . "</a>";
            break;
        default:
            $title = "<a href='$base_url/account?tab=companies&page=company_detail&id=" . $companyid . "&mtab=$mtab&panel=multi&view=edit&dataid=" . $dataid . "&section=$section'>" . $title . "</a>";
            break;
    }
    return $title;
}

function sortanalysis($op = null)
{
    require('analysis_sort.php');
}

function share_knowledge_sort($op = null)
{
    require('share_knowledge_sort.php');
}

function financials($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "Files";
    $tab = sget($_REQUEST, 'tab');
    $panel = "editor_panel";
    $data_type = 'financials';
    global $redirect;
    ///
    $content = '';
    if ($op == null || $op == 'view') {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s'";
        $result = db_query($sql, array($data_type));
        $Row = db_fetch_object($result);
        if ($Row) {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            //            $content = sideBarExcerpt($content, $redirect);
            $content = sideBarMonitoringExcerpt($content, $redirect);
        }
        $Block['title'] .= "<div class='editbtn'><a href='${redirect}&panel=${panel}&view=edit&data_type=${data_type}' class='tool'>edit</a></div>";
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function diligence($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "<span style='font-size:14px;color: #284B54;font-family: Lato Bold Italic;text-transform:capitalize !important;'>Sections</span>";
    $content = '';
    global $redirect;
    if ($op) {

        //Get company type

        $q = mysql_query("SELECT membership FROM maenna_company WHERE companyid = '" . ((int) $companyid) . "' LIMIT 1");
        $cmp_type_tmp = mysql_fetch_array($q);
        $cmp_type = $cmp_type_tmp['membership'];

        $sql = "select * from maenna_questionair where parentid is null and target = 'company' and status = 1 and usertype = '%s'";
        $result = db_query($sql, array($cmp_type));
        $Row = db_fetch_object($result);
        if ($Row) {
            $Block['title'] = "<span style='font-size:14px;color: #00A2BF;font-family: Lato Bold Italic;text-transform:capitalize !important;'>Topics</span>"; // htmlentities(ucwords($Row->content), ENT_QUOTES);
            $parentid = $Row->recordid;



            $sql = "select * from maenna_questionair where parentid = %d and (type = 'title') order by weight, recordid";
            $result = db_query($sql, array($parentid));
            $counter = 1;
            while (($Row = db_fetch_object($result))) {
                $questionid = $Row->recordid;
                $title =   "<a href='{$redirect}#t_$questionid'>" . htmlentities(ucwords($Row->content), ENT_QUOTES | ENT_IGNORE, "UTF-8") . "</a>";
                $content .= "\n<div class=row style='padding:7px 0; text-transform: capitalize;font-size: 14px;'>" . ucwords(strtolower($title)) . "</div>";
                $counter++;
            }
        }
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function files($op)
{
    global $user;
    global $AccessObj;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    // list files
    if (isset($_REQUEST['file'])) {
        $Block['title'] = "Files";

        //$Block['title'] .= "<span style='line-height:15px;border-left:thin solid #666;padding-left:20px; float:right;display:inline-block;font-weight:bold; font-family:\"HelveticaNeue-CondensedBold\", \"Helvetica Neue\"; font-size:13px;'>Sort By<img panelid='file_sort' style='width:20px; margin-left:20px;' src='themes/maennaco/images/arrow_down.png'></span>";

        $data_type = sget($_REQUEST, 'mtab') . "_files";
        $section = __FUNCTION__;
        global $redirect;
        $content = '';
        $HPV =  hidden_post_values(array('tab', 'page', 'mtab', 'id'));

        //if($op == 'write') $Block['title'] .= "<div class='editbtn'><a href='#' class='tool openbox' boxid='box1'>Edit</a></div>";
        if ($op && $op != 'update') {
            //$content = "<div id='box1' style='display:none'>
            //                <form method=post action='/account' enctype='multipart/form-data'>
            //                    <div class=row>Name:<br><input type=text name=name value='' /></div>
            //                    <div class=row>File: <input type=file name='file' /></div>
            //                    <div class=row><input type=submit name=submit value='submit'  class=button /> &nbsp;
            //                            <a href='#' class='hidebox button' boxid=box1>Close</a>
            //                    </div>
            //                    $HPV
            //                    <input type='hidden' name=section value='files' />
            //                    <input type='hidden' name=update_section value='files' />
            //                    <input type='hidden' name=do value='new' />
            //                </form>
            //            </div>";
            //if (sget($_REQUEST,'mtab') == 'file' && $AccessObj->Com_sections['file']['sections']['file_man']['access'] == 'replace')
            //
            //$content .= "<div id='box2' style='display:none'>
            //                <form method=post action='/account' enctype='multipart/form-data'>
            //                    <div class=row>Name:<br><input type=text name=name value='' /></div>
            //                    <div class=row>File: <input type=file name='file' /></div>
            //                    <div class=row><input type=submit name=submit value='submit'  class=button /> &nbsp;
            //                            <a href='#' class='hidebox button' boxid=box2>Close</a>
            //                    </div>
            //                    $HPV
            //                    <input type='hidden' name=section value='files' />
            //                    <input type='hidden' name=update_section value='files' />
            //                    <input type='hidden' name=do value='update' />
            //                    <input type='hidden' name=dataid value='".sget($_REQUEST,'dataid')."' />
            //                </form>
            //            </div>";
            $i = 0;

            foreach (_TAGS() as $key => $value) {

                if ($tag == $key) $font_style = 'color:#00a2bf;';
                else $font_style = 'font-style:italic;';

                $sortLinks .= "<a href='${redirect}&section=$section&tag=$key' style='$font_style font-size:10px;cursor:pointer;";
                if ($i == 0)  $sortLinks .= "margin-right:10px;'>$value</a>";
                else $sortLinks .= "margin-left:10px;margin-right:10px;'>$value</a>";

                if (++$i != $tag_no) $sortLinks .= '|';
            }

            $content .= '<div id="file_sort" style="display:none;line-height:30px;height:30px;width:100%; background-color:#f2f2f2"><span style="margin-top:0; display:inline-block;vertical-align:middle;margin-left:15px;">';



            $content .= $sortLinks . '</span></div>';
            $content .= js_init("init_openbox();init_hidebox();");


            $sql = "select * from maenna_company_data where data_type like '%_files' and companyid = %d and deleted != 1 order by dataid desc";
            $result = db_query($sql, array($companyid));
            while ($Row = db_fetch_object($result)) {

                $nameLong = htmlentities(ucwords($Row->data_value), ENT_QUOTES);
                if (strlen($nameLong) > 15) $name = substr($nameLong, 0, 15) . "...";
                else $name = $nameLong;
                $filename = $Row->data_value2;
                $dataid = $Row->dataid;
                $path =  "./" . file_directory_path() . "/" . $filename;
                $ext = pathinfo($filename, PATHINFO_EXTENSION);
                $editorid = $Row->editorid;
                $access = $Row->access;

                if (in_array($ext, array('doc', 'xls', 'pdf', 'ppt', 'tiff', 'odt', 'txt')))
                    $type_icon = '<img width="15px" height="15px" src="./themes/maennaco/images/' . $ext . '_icon.png">';

                $arr = explode("_", $Row->data_type, 2);
                $cat = $arr[0];

                $catName = getFileCategoryName($cat);

                $tmpRow = '';

                $tmpRow .= "\n<div class = 'row singlerow' style='overflow:hidden;text-align:left;height:25px;' title='$name'>
                         $type_icon <span title = '" . date('d/m/Y', $Row->access) . " - " . getUserById($Row->editorid) . "'><a style='float:left; margin-left:20px;' href = '/account?tab=companies&page=company_detail&eid=$Row->editorid&dataid=$Row->dataid&datatype=$Row->data_type&id=$companyid&mtab=file&file=" . urlencode($path) . "&name=" . urlencode($name) . "'>$name</a></span>
                            </div>";

                if ($catName == 'Other') $cat = 'Other';
                $fileList[$cat][] = $tmpRow;

                //if (strlen($Row->data_value) > 23) $name = substr(htmlentities(ucwords($Row->data_value),ENT_QUOTES),0,23)."..."; else  $name = htmlentities(ucwords($Row->data_value),ENT_QUOTES);
                //$filename = $Row->data_value2;
                //$dataid = $Row->dataid;
                //$path =  "./" . file_directory_path() . "/" . $filename;
                //$rem_link = '';
                //if($op == 'write' ) $rem_link = "<div class='entry_tool' style='width:30px;text-align:left;'><a href='$redirect&update_section=files&do=del&dataid=$dataid&section=$section'
                //                                        onclick='return confirm(\"Continue to delete this file?\")'>DEL</a>
                //                </div>";
                //$content .= "\n<div class='row singlerow' style='padding:5px 0'>
                //            <a target='_blank' href='/account?tab=companies&page=company_detail&eid=$Row->editorid&dataid=$Row->dataid&id=$companyid&mtab=file&file=".urlencode($path)."&name=".urlencode($name)."' title = '".date('d/m/Y',$Row->access)." - ".getUserById($Row->editorid)."'>$name</a>
                //                $rem_link
                //            </div>";
            }

            $fileNo = 1;

            foreach (_TAGS() as $key => $value) {


                if (count($fileList[$key]) > 0) {

                    $content .= "<div style=' font-style:italic;background:#f2f2f2;' class='row singlerow'>$value</div>";

                    foreach ($fileList[$key] as $value1)

                        if ($fileNo >= 11) break;

                        else {
                            $content .= $value1;
                            $fileNo++;
                        }
                }
            }

            if (!isset($tag)) {

                $content .= "<div style='background:#f2f2f2;' class='row singlerow'>Other</div>";

                foreach ($fileList['Other'] as $value)

                    if ($fileNo >= 11) break;
                    else {
                        $content .= $value;
                        $fileNo++;
                    }
            }
        }

        if ($fileNo == 11) $content .= "<a style='border-left: 1px solid;padding-left:5px;line-height: 1.1em;'class = 'tool' href='account?tab=companies&page=company_detail&id=" . $companyid . "&mtab=file'>More</a>";
    } elseif ($op == 'update') {
        $do = sget($_REQUEST, 'do');
        $dataid = sget($_REQUEST, 'dataid');
        if ($do == 'new') {
            $name = sget($_REQUEST, 'name');

            $tag = sget($_REQUEST, 'tag');

            $tag .= "_files";

            if (!empty($tag)) {

                if ((!empty($name)) && ($_FILES['file']['error'] == 0)) {
                    set_time_limit(0);
                    $File = upload_file($_FILES['file']);
                    if ($File) {
                        $filename = $File['name'];
                        $type = $File['type'];
                        $sql = "insert into maenna_company_data (companyid, access, data_type,data_value, data_value2, data_value3, editorid )
                                                        values(%d,'%s','%s','%s','%s','%s',%d)";
                        $DBValues = array($companyid, $time, $tag, $name, $filename, $type, $editorid);
                        if (db_query($sql, $DBValues)) drupal_set_message("File is added");
                        else {
                            drupal_set_message("Failed to upload file. Please try again later", 'error');
                        }
                    }
                    return;
                }
            } else {

                if ((!empty($name)) && ($_FILES['file']['error'] == 0)) {
                    set_time_limit(0);
                    $File = upload_file($_FILES['file']);
                    if ($File) {
                        $filename = $File['name'];
                        $type = $File['type'];
                        $sql = "insert into maenna_company_data (companyid, access, data_type,data_value, data_value2, data_value3, editorid )
                                                        values(%d,'%s','%s','%s','%s','%s',%d)";
                        $DBValues = array($companyid, $time, $data_type, $name, $filename, $type, $editorid);
                        if (db_query($sql, $DBValues)) drupal_set_message("File is added");
                        else {
                            drupal_set_message("Failed to upload file. Please try again later", 'error');
                        }
                    }
                    return;
                }

                drupal_set_message("Please complete the form and try again", 'error');
            }
        } elseif ($do == 'del') {
            $dataid = sget($_REQUEST, 'dataid');
            if ($dataid) {
                $sql = "update maenna_company_data set deleted = 1 where companyid = %d and dataid = %d limit 1";
                if (db_query($sql, array($companyid, $dataid))) drupal_set_message("The file is deleted");
                else drupal_set_message("Failed to delete the file", 'error');
            }
        } elseif ($do == 'update') {
            $dataid = sget($_REQUEST, 'dataid');
            $name = sget($_REQUEST, 'name');

            if ((!empty($name)) && ($_FILES['file']['error'] == 0)) {
                set_time_limit(0);
                $File = upload_file($_FILES['file']);
                $time = date();
                if ($File) {
                    $filename = $File['name'];
                    $type = $File['type'];
                    $sql = "UPDATE maenna_company_data  SET access = '%s', data_value = '%s', data_value2 = '%s', data_value3 = '%s', editorid = '%s'  WHERE dataid = '%s'";
                    $DBValues = array($time, $name, $filename, $type, $user->uid, $dataid);
                    if (db_query($sql, $DBValues)) drupal_set_message("File has been updated");
                    else {
                        drupal_set_message("Failed to upload file. Please try again later", 'error');
                    }
                }
                return;
            }

            drupal_set_message("Please complete the form and try again", 'error');
        }
        return;
    }


    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function prof_invest($op)
{
    global $user;
    global $AccessObj;
    $profid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $proj_name = getProjectName($companyid);
    $time = time();
    $date = date('d/m/Y');

    if ($op == 'update') {
        $do = sget($_REQUEST, 'do');
        if ($do == 'new') {
            $type = sget($_REQUEST, 'investor_type');
            $amount = sget($_REQUEST, 'amount');
            $amount = str_replace('$', '', $amount);
            $amount = str_replace(',', '', $amount);

            $sql = "insert into maenna_professional_investment (prof_id, company_id, proj_name, type, ioi, created, status, date)
                                            values(%d, %d, '%s', '%s', %d, '%s', '%s', '%s')";
            $DBValues = array($profid, $companyid, $proj_name, $type, $amount, $time, '0', $date);
            if (db_query($sql, $DBValues)) {
                drupal_set_message("Added successfully");
                drupal_goto('account','tab=companies&page=company_detail&id='.$companyid.'&mtab=about');
            }
            else drupal_set_message("Failed to Add. Please try again later", 'error');
        } elseif ($do == 'del') {
            $dataid = sget($_REQUEST, 'dataid');

            $sql = "update maenna_professional_investment set deleted = 1 where id = %d limit 1";
            if (db_query($sql, array($dataid))) drupal_set_message("The investment is deleted");
            else drupal_set_message("Failed to delete the investment", 'error');
        } elseif ($do == 'edit') {
            $dataid = sget($_REQUEST, 'dataid');
            $type = sget($_REQUEST, 'investor_type');
            $amount = sget($_REQUEST, 'amount');
            $amount = str_replace('$', '', $amount);
            $amount = str_replace(',', '', $amount);
            $reference = sget($_REQUEST, 'reference');
            $refdate = sget($_REQUEST, 'refdate');
            $reftime = time($refdate);
            $pay_status = sget($_REQUEST, 'pay_status');
            if ($reference != null && $AccessObj->user_type != 'people') {
                if ($pay_status == '1' || $pay_status == '4') {
                    drupal_set_message("Failed to update the investment", 'error');
                } else {
                    $sql = "update maenna_professional_investment set type = '%s', amount = %d, reference = %d, created = '%s',date = '%s' where id = %d limit 1";
                    if (db_query($sql, array($type, $amount, $reference, $refdate,$date, $dataid))) {
                        drupal_set_message("The investment is updated");
                        drupal_goto('account','tab=companies&page=company_detail&id='.$companyid.'&mtab=about');
                    }
                    else drupal_set_message("Failed to update the investment", 'error');
                }
            } else {
                $sql = "update maenna_professional_investment set type = '%s', ioi = %d, created = '%s', date = '%s' where id = %d limit 1";
                if (db_query($sql, array($type, $amount, $date, $date, $dataid))) {
                    drupal_set_message("The investment is updated");
                    drupal_goto('account','tab=companies&page=company_detail&id='.$companyid.'&mtab=about');
                }
                else drupal_set_message("Failed to update the investment", 'error');
            }
        }
    }
    if (sget($_REQUEST, 'user_type') != 'admin')
        drupal_goto('account', 'tab=companies&page=company_detail&id=' . $companyid . '&section=company_name&panel=prof_explore_investment&mode=show');
    else
        drupal_goto('account', 'tab=companies&page=company_detail&id=' . $companyid . '&mtab=about&section=fundraising_administration&panel=fap&type=committed_capital');


    //    $Block['body'] = sidebar_box($Block['title'], $content);
    //    return $Block;
}

function prof_invest_now($op)
{
    global $user;
    global $AccessObj;
    $profid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $update_company_sql = "update users_bank_info set companyid='" . $companyid . "' where uid='" . $profid . "'";
    mysql_query($update_company_sql);

    if ($op == 'update') {
        $paymentmethod = sget($_REQUEST, 'paymentmethod');
        $amount = sget($_REQUEST, 'amount');
        $amount = str_replace('$', '', $amount);
        $amount = str_replace(',', '', $amount);
        $total = sget($_REQUEST, 'total');
        $total = str_replace('$', '', $total);
        $total = str_replace(',', '', $total);
        $total = (int)$total;
        $clewedFee = sget($_REQUEST, 'clewedfee');
        $processingFee = sget($_REQUEST, 'processingfee');
        if ($total == 0 || $total == null) {
            drupal_set_message("Investment Amount field was empty. Please insert value.", 'error');
            drupal_goto('account', 'tab=companies&page=company_detail&id=' . $companyid . '&section=company_name&panel=invest_now');
            return false;
        }
        if ($paymentmethod == 'linked_bank') {
            /*********** Stripe ***********/
            $customerid = sget($_REQUEST, 'customerid');
            /***********
                @@ Get connectid
                @@ Author : Monster
                @@ Created By : 2020-09-20
                $get_conid_sql = "select connectid from users_bank_info where companyid='" . $companyid . "'";
                $con_data_res = mysql_query($get_conid_sql);
                $con_data = mysql_fetch_array($con_data_res);
                $connectid = $con_data['connectid'];
             */

            \Stripe\Stripe::setApiKey('sk_test_51HSqP9C2Wih1cFYGyidNE5BdaQjwwE3nKe4nrhvcCqFPAYGUZJaQtfkEFk10mQgKDUnXEySwa8CyaGr9Chquie2k00leJ7IB2Q');

            /***********
                @@ For transfer connect account
                @@ Author : Monster
                @@ Created By : 2020-09-20
                $charge = \Stripe\Charge::create([
                    'amount' => $total,
                    'currency' => 'usd',
                    'customer' => $customerid,
                    'transfer_data' => [
                        'amount' => $amount,
                        'destination' => $connectid,
                    ],
                ]);
             */
            /***********
                @@ Customer Charge in Stripe
                @@ Author : Monster
                @@ Created By : 2020-09-20
             */
            $get_project_sql = "SELECT proj_name FROM maenna_professional_investment WHERE prof_id = '" . $profid . "' AND company_id = '" . $companyid . "'";
            $project_data_res = mysql_query($get_project_sql);
            $project_data = mysql_fetch_object($project_data_res);
            $proj_name = $project_data->proj_name;

            $get_bank_sql = "SELECT bankname,banknumber from users_bank_info WHERE customerid='".$customerid."' AND status='1'";
            $bank_data_res = mysql_query($get_bank_sql);
            $bank_data = mysql_fetch_object($bank_data_res);
            $bankname = $bank_data->bankname;
            $banknumber = $bank_data->banknumber;

            $user_sql = "SELECT * FROM users WHERE uid = '" . $profid . "'";
            $user_res = mysql_query($user_sql);
            $users = mysql_fetch_object($user_res);
            $user_name = $users->name;
            // $charge_total = $total * 100;
            $charge = \Stripe\Charge::create([
                'amount' => $total,
                'currency' => 'usd',
                'customer' => $customerid,
                'description' => $proj_name.'-'.$companyid.'-'.$bankname.'-'.$user_name,
            ]);
            if ($charge) {
                $sql = "update maenna_professional_investment set amount = '" . $total . "',clewed_fee = '" . $clewedFee . "', processing_fee= '" . $processingFee . "' where prof_id = '" . $profid . "' and company_id = '" . $companyid . "'";
                if (mysql_query($sql)) {
                    drupal_set_message("Successfully invest");
                    drupal_goto('account','tab=companies&page=company_detail&id='.$companyid.'&mtab=about');
                }
            } else drupal_set_message("Failed to invest", 'error');
        }
    }
       if (sget($_REQUEST, 'user_type') != 'admin')
           drupal_goto('account', 'tab=companies&page=company_detail&id=' . $companyid . '&section=company_name&panel=prof_explore_investment&mode=show');
       else
           drupal_goto('account', 'tab=companies&page=company_detail&id=' . $companyid . '&mtab=about&section=fundraising_administration&panel=fap&type=committed_capital');

       $Block['body'] = sidebar_box($Block['title'], $content);
       return $Block;
}

function admin_tools($op = null)
{
    /** @to-do: Focus current selected page in the right sidebar **/
    $Block['title'] = '';
    $content = "
        <div class=row><a href='#'>Clewed Note</a></div>
        <div class=row><a href='#'>Progress</a></div>
        <div class=row><a href='#'>Request Professional</a></div>
        <div class=row><a href='#'>Suggest a Related Company</a></div>
    ";
    $Block['body'] = sidebar_box('', $content, 'css_admin_box');
    return $Block;
}

function nav_menu_right($op = null)
{
    /*    $Block['title'] = '';

		$content = '<a id="showEventForm" style="padding:0; background:none;color:#00a2bf;cursor:pointer; font-size:14px;">Schedule a discussion</a>';
    $Block['body'] = sidebar_box('', $content);
    return $Block;*/
}

function download_link($op = null)
{
    global $AccessObj;
    if (isset($_REQUEST['file'])) {
        if ($AccessObj->Com_sections[file][sections][download_link][access] == 'write' || $AccessObj->Com_sections[file][sections][download_link][access] == 'read') {

            $Block['title'] = 'DOWNLOAD FILE';

            $content = '<a href="' . "http://" . $_SERVER['HTTP_HOST'] . sget($_REQUEST, 'file') . '" style="color:#00a2bf;padding:0;">Download file</a>';
            $Block['body'] = sidebar_box('', $content);
            return $Block;
        }
    }
}
function competitive_advantage($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Durable Competitive Advantage";

    $data_type = 'advantage';
    global $redirect;
    $content = '';
    $panel = 'single';
    $section = __FUNCTION__;
    if ($op == 'write') $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit&section=$section' class=tool>Edit</a></div>";
    if ($op) {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' ";
        $result = db_query($sql, array($companyid, $data_type));
        $Row = db_fetch_object($result);
        if ($Row) {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            // $content = _filter_autop(html_entity_decode($content));
            $more_link = $redirect . "&panel=${panel}&view=detail&section=$section";
            $content = sideBarMonitoringExcerpt($content, $more_link);
        }
    }
    $Block['body'] = sidebar_box($Block['title'], $content, 'comp-adv');
    return $Block;
}


function earning_source($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Source of Earnings Growth";
    $tab = sget($_REQUEST, 'tab');
    $data_type = 'earningsource';
    global $redirect;
    $content = '';
    $panel = 'single';
    $section = __FUNCTION__;
    if ($op == 'write') $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit&section=$section' class=tool>Edit</a></div>";
    if ($op) {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' ";
        $result = db_query($sql, array($companyid, $data_type));
        $Row = db_fetch_object($result);
        if ($Row) {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = _filter_autop(html_entity_decode($content));
            $more_link = $redirect . "&panel=${panel}&view=detail&section=$section";
            $content = sideBarMonitoringExcerpt($content, $more_link);
        }
    }

    $Block['body'] = sidebar_box($Block['title'], $content, 'earning-src');
    return $Block;
}
function single($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();

    $panel = "single";
    //$data_type = sget($_REQUEST, 'key');
    global $redirect;
    $dataid = '';
    $content = '';
    $section = sget($_REQUEST, 'section');
    switch ($section) {
        case "competitive_advantage":
            $title = "Durable Competitivity Advantage";
            $data_type = 'advantage';
            break;
        case "earning_source":
            $title = "Source of Earnings Growth";
            $data_type = 'earningsource';
            break;
    }
    $Block['title'] = $title;

    if ($op && $op != 'update') {
        $view = sget($_REQUEST, 'view');
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s'";
        $result = db_query($sql, array($companyid, $data_type));
        $Row = db_fetch_object($result);
        if ($Row) {
            $content = $Row->data_value2;
            $dataid = $Row->dataid;
        }
        if ($view == 'edit') {
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
            $content .= hidden_post_values(array('tab', 'page', 'id', 'mtab', 'panel', 'section'));
            $content .= "</form>";
        } elseif ($view == 'detail') {
            $content = nl2br(htmlentities($content, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $content = _filter_autop(html_entity_decode($content));
            $content = "<div class=entry>
                            <div class=entry-content>$content</div>
                        </div>
                        <div class=backbtn><a href='$redirect&section=section' class='button'>back</a></div>";
        }
    } elseif ($op == 'update') {

        $data_value2 = sget($_REQUEST, $data_type);
        $companyid = sget($_REQUEST, 'id');
        $dataid = sget($_REQUEST, 'dataid');

        if (empty($companyid)) drupal_set_message("Invalid company id", 'error');
        else {
            if ($dataid) {
                $sql = "update maenna_company_data set data_value2 = '%s', access='%s',editorid=%d where data_type = '%s' and dataid = %d limit 1";
                $DBValues = array($data_value2, $time, $editorid, $data_type, $dataid);
            } else {
                $sql = "insert into maenna_company_data (companyid, access, data_type, data_value2,editorid ) values (%d, '%s','%s','%s',%d)";
                $DBValues = array($companyid, $time, $data_type, $data_value2, $editorid);
            }
            if (db_query($sql, $DBValues)) drupal_set_message("Data record is updated");
            else drupal_set_message("Failed to update record", 'error');
        }
        return;
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function priority($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $Block['title'] = "Priorities";
    $panel = "multi";
    $data_type = 'priority';
    global $redirect;
    $content = '';
    $section = __FUNCTION__;
    if ($op) {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and deleted != 1 order by dataid limit 3";
        $result = db_query($sql, array($companyid, $data_type));
        $counter = 0;
        $viewall_link = $redirect . "&panel=${panel}&view=listview&section=${section}";
        while (($Row = db_fetch_object($result)) !== false) {
            $counter++;

            $title = ucwords($Row->data_value);
            if ($title)
                $title = htmlentities($title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            else $title = "&nbsp;";

            //$title =    htmlentities($Row->data_value,ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $text =     htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $text = _filter_autop(html_entity_decode($text));
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid&section=${section}";
            $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid&section=${section}";
            //$text = sideBarExcerpt($text, $more_link);
            $text = sideBarMonitoringExcerpt($text, $viewall_link);
            $last = '';


            $content .= "<div class='entry'>
                            <div class=title>$title</div>
                            <div class=content>$text</div>
                        </div>";
        }

        $url = $redirect . "panel=milestones_panel&view=add&section=${section}";
        $add_link = $redirect . "&panel=${panel}&view=add&section=${section}";

        if ($op == 'write') $Block['title'] .= "<div class=editbtn><a href='$add_link' class=tool>Add</a></div>";
        //        $content .= "\n<div class='viewallbtn'><a href='$viewall_link' class=tool>More</a></div>";
    }


    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function multi($op = null)
{
    global $user, $AccessObj, $base_url;

    $editorid  = $user->uid;
    $time      = time();
    $companyid = sget($_REQUEST, 'id');
    $section   = sget($_REQUEST, 'section');
    $panel     = "multi";

    $content = '';
    $orderby = 'desc';
    $mtab    = sget($_REQUEST, 'mtab');
    if (empty($mtab)) {
        foreach ($AccessObj->Com_sections as $tab_name => $Tab_data) {
            if ($Tab_data['access'] != 'hide' && $tab_name != 'common') {
                $mtab = $tab_name;
                break;
            }
        }
    }
    if (empty($section)) $section = $mtab;
    if (empty($section)) {
        echo "Invalid section - multi";
        return;
    }
    $length_limit = "";
    $redirect     = rebuild_url(array('tab', 'page', 'id')) . "&mtab=$mtab&section=$section";
    $back         = rebuild_url(array('tab', 'page', 'id')) . "&mtab=$mtab";
    switch ($section) {
        case "highlights":
            $title        = "High Lights";
            $data_type    = 'highlights';
            $orderby   = '';
            break;
        case "minutes":
            $title        = "ADVICE";
            $data_type    = 'minutes';
            $length_limit = "maxlength=3000";
            break;
        case "maenna_analysis":
        case 'analysis':
            $title     = "ANALYSIS";
            $data_type = 'analysis';
            $orderby   = '';
            break;
        case 'share_knowledge':
        case 'maenna_share_knowledge':
            $title     = "DISCUSS THE PROJECT";
            $data_type = 'share_knowledge';
            $orderby   = '';
            break;
        case "maenna_plan":
            $title     = "Plan";
            $data_type = 'plan';
            $orderby   = '';
            break;
        case 'mgmt_analysis':
            $title     = "MANAGEMENT DISCUSSION & ANALYSIS";
            $data_type = 'mgmtanalysis';
            break;
        case 'council_advice':
            $title     = "COUNCIL ADVICE AND ANALYSIS";
            $data_type = 'advice';
            break;
        case 'priority':
            $title     = "PRIORITY";
            $data_type = 'priority';
            $orderby   = '';
            break;
    }
    $Block['title'] = $title;

    if ($op == 'write' || $op == 'write_own' || $op == 'read_all_write_own' || ($op == 'edit' && $editorid == $companyid)) {
        $add_link = $redirect . "&panel=${panel}&view=add";
        if ($data_type != 'share_knowledge')
            $Block['title'] .= "<div class=editbtn><a href='$add_link' class=tool>ADD</a></div>";
    }

    /*$res = mysql_query('DESCRIBE maenna_company_data');
while($row = mysql_fetch_array($res)) {
    echo "{$row['Field']} - {$row['Type']} <br>";
}*/

    if ($op && ($op != 'update')) {
        $view = sget($_REQUEST, 'view');
        $do = sget($_REQUEST, 'do');

        if ($view == 'detail' || $view == 'edit' || $view == 'add') {

            $dataid = sget($_REQUEST, 'dataid');

            if ($do == 'remove') {
                if ($dataid) {
                    $sql = "update maenna_company_data set access = '%s', editorid = %d, deleted = 1 where dataid = %d limit 1";
                    if (db_query($sql, array($time, $editorid, $dataid))) $Correct = true;

                    if ($Correct) drupal_set_message("Operation Successful");
                    else {
                        drupal_set_message("Operation Failed", 'error');
                    }
                    header("location:" . $redirect);
                }
            }
            $Row    = array();
            if ($dataid) {
                $sql    = "select * from maenna_company_data where dataid = %d limit 1";
                $result = db_query($sql, array($dataid));
                $Row    = db_fetch_array($result);
            } elseif (empty($dataid) && ($view == 'detail')) {
                $sql    = "select * from maenna_company_data where companyid = %d and data_type = '%s' order by dataid desc limit 1";
                $result = db_query($sql, array($companyid, $data_type));
                $Row    = db_fetch_array($result);
            }

            //print_r($Row);
            //mysql_query("update maenna_company_data set status=1");

            if ($Row !== false) {

                if ($view == 'detail') {

                    $dataid    = sget($Row, 'dataid');
                    $sub_title = strtoupper(sget($Row, 'data_value'));
                    if ($sub_title)
                        $sub_title = htmlentities($sub_title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    else
                        $sub_title = "&nbsp;";
                    $text     = sget($Row, 'data_value2');
                    $text     = nl2br(htmlentities($text, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                    $rem_link = '';
                    if ($op == 'write' || ($op == 'edit' && $editorid == $Row['editorid'])) {
                        $rem_link  = $redirect . "&update_section=${panel}&do=remove&dataid=$dataid";
                        $rem_link  = "<a href='$rem_link' class=tool onclick='return confirm(\"Continue to remove record\")'>Delete</a>";
                        $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
                        $edit_link = "<div class=editbtn><a href='$edit_link' class=tool>EDIT</a></div>";
                    } else {
                        $rem_link  = '';
                        $edit_link = '';
                    }

                    $content = "<div class=entry>
                                    <div class=entry-title>$sub_title &nbsp;$edit_link</div>
                                    <div class=entry-content>" . _filter_autop(html_entity_decode($text)) . "</div>
                                </div>";

                    $content .= "<div class=backbtn>

                                    <a href='$back' class=button>back</a>&nbsp;&nbsp;
                                    $rem_link
                                </div>";
                } elseif (($view == 'edit' || $view == 'add') && ($op == 'write' || $op == 'write_own' || $op == 'read_all_write_own') || $op == 'edit') {
                    $dataid = sget($Row, 'dataid');
                    $status = sget($Row, 'status');
                    if ($status == 0) {
                        $sub_title  = strtoupper(sget($Row, 'data_value_save'));
                        $text       = sget($Row, 'saved_content');
                        $short_desc = sget($Row, 'short_description2');
                        $tags       = sget($Row, 'tags2');
                    } else {
                        $sub_title  = strtoupper(sget($Row, 'data_value'));
                        $text       = sget($Row, 'data_value2');
                        $short_desc = sget($Row, 'short_description');
                        $tags       = sget($Row, 'tags');
                    }

                    if ($view == 'edit') {
                        $title    = "Edit " . $Block['title'];
                        $do       = 'update';
                        $rem_link = $redirect . "&panel=${panel}&view=listview&update_section=${panel}&do=remove&dataid=$dataid";
                    } else {
                        $title    = "Add " . $Block['title'];
                        $do       = 'insert';
                        $rem_link = '';
                    }
                    if (!in_array($data_type, array('mgmtanalysis', 'plan'))) {
                        $save_link = '<input type=submit name=submit  style="cursor:pointer;" value=Save class=button_add />';
                    }
                    $Block['title'] = $title;
                    if ('share_knowledge' == $data_type)
                        $Block['title'] = substr($Block['title'], 4);

                    $hv             = hidden_post_values(array('tab', 'page', 'id', 'mtab', 'panel', 'section'));
                    $rr             = '<div>Tags:<br /><select class="discuss" id="tags" name="tags"><option value="">Choose a Category</option>';
                    $rr .= OPTION_TAGS(_categories(), $tags);
                    $rr .= '</select></div><br />';

                    $titlePlaceholder = '';
                    if ('share_knowledge' == $data_type)
                        $titlePlaceholder = 'Type the subject for your message';

                    $contentPlaceholder = '';
                    if ('share_knowledge' == $data_type)
                        if ($user->uid == $_REQUEST['id'])
                            $contentPlaceholder = 'If you are the administrator of this project, <br>you can use this page to post project\'s expert requirements.';
                        elseif (in_array($AccessObj->user_type, array('super', 'admin')))
                            $contentPlaceholder = 'Briefly discuss your background and interest.<br> Include any relevant related work as you see fit.<br><br> If you are the administrator of this project, <br>you can use this page to post project\'s expert requirements.';
                        else
                            $contentPlaceholder = 'Briefly discuss your background and interest.<br> Include any relevant related work as you see fit.';

                    //                        if('share_knowledge' == $data_type)
                    //                            $rr = '';

                    $invitation = "<br>";
                    if ('share_knowledge' == $data_type) {
                        $invitedExpertIdsString = '';
                        $companyService = new \Clewed\Company\Service();
                        if (!empty($dataid)) {

                            $invitedExpertIds = $companyService->getDiscussionInvitedExpertIds($dataid);
                            $invitedExpertIdsString = implode(',', $invitedExpertIds);
                        }

                        $Conns = Connections::Com_conns($companyid);
                        $companyName = getProId($companyid);
                        $tags = array(
                            '{value: "' . $companyid . '", name: "' . $companyName . '"}'
                        );
                        if ($AccessObj->user_type == 'people' && in_array($AccessObj->uid, $companyService->getConnectors($companyid))) {
                            foreach ($Conns['Visible'] as $Pro) {
                                $pro_uid = $Pro->assignee_uid;
                                $pro_maeid = getProId($pro_uid);
                                $tags[] = '{value: "' . $pro_uid . '", name: "' . $pro_maeid . '"}';
                            }
                        } elseif ($AccessObj->user_type == 'people' && in_array($AccessObj->uid, $companyService->getInterested($companyid))) {
                            foreach ($Conns['Collaborator'] as $Pro) {
                                $pro_uid = $Pro->assignee_uid;
                                $pro_maeid = getProId($pro_uid);
                                $tags[] = '{value: "' . $pro_uid . '", name: "' . $pro_maeid . '"}';
                            }
                        } else {

                            foreach ($Conns['Client'] as $Pro) {
                                $pro_uid = $Pro->assignee_uid;
                                $pro_maeid = getProId($pro_uid);
                                $tags[] = '{value: "' . $pro_uid . '", name: "' . $pro_maeid . '"}';
                            }

                            foreach ($Conns['Advisor'] as $Pro) {
                                $pro_uid = $Pro->assignee_uid;
                                $pro_maeid = getProId($pro_uid);
                                $tags[] = '{value: "' . $pro_uid . '", name: "' . $pro_maeid . '"}';
                            }

                            foreach ($Conns['Collaborator'] as $Pro) {
                                $pro_uid = $Pro->assignee_uid;
                                $pro_maeid = getProId($pro_uid);
                                $tags[] = '{value: "' . $pro_uid . '", name: "' . $pro_maeid . '"}';
                            }
                            foreach ($Conns['Visible'] as $Pro) {
                                $pro_uid = $Pro->assignee_uid;
                                $pro_maeid = getProId($pro_uid);
                                $tags[] = '{value: "' . $pro_uid . '", name: "' . $pro_maeid . '"}';
                            }
                        }

                        $tags = array_unique($tags);

                        $mode = "add-mode";
                        if (!empty($dataid))
                            $mode = "edit-mode";

                        if (!empty($tags))
                            $invitation = "
                                    <div class=\"discussion-invited-experts-container $mode\">
                                    To:
                                        <input type='text'
                                               name='invited_experts'
                                               placeholder=\"Skip this section to message only the project administrator or add user names from the Project team to add more\"
                                               data-tooltip=\"Skip this section to message only the project administrator or add user names from the Project team to add more\"
                                               value=\"$invitedExpertIdsString\"
                                               style=\" height: 14px !important;
                                                        font-family: LatoRegular !important;
                                                        font-style: italic !important;
                                                        font-size: 14px !important;
                                                        padding-left:2px !important;\"/>
                                        <script type='text/javascript'>
                                            var invited = (\"$invitedExpertIdsString\").split(','),
                                                tags = [" . implode(',', $tags) . "];

                                            $(function(){
                                                $(\".discussion-invited-experts-container input\").autoSuggest(tags, {
                                                    preFill: tags.filter(function(el){
                                                        return invited.indexOf(el.value) >= 0;
                                                    }),
                                                    selectedItemProp: \"name\",
                                                    searchObjProps: \"name\"
                                                });
                                                $(\"input[name='submit]\")
                                            });

                                            function inject_invited_experts(form) {
                                                var invitations = $(form).find('input.as-values').val();
                                                $(form).find('input[name=invited_expert_ids]').val(invitations);
                                            };
                                        </script>
                                    </div>
                                    <input type='hidden' name='invited_expert_ids' />
                                ";
                    }

                    $content .= <<< END
                    <form action='$base_url/account' method='post' name='addanalysis' onsubmit='inject_invited_experts(this);return check_input();'>
                        <div class=rt-block1>
                            <div class=rt-title style='margin-right: -7px;'>
                                $invitation
                                <div>Subject:<br /><input style='width:573px !important;' placeholder='$titlePlaceholder' type=text name=title value='$sub_title' maxlength="55" /></div>
                                <div>Message:<br />
                                <label for='tinymce' style="font-style:italic;text-align:center;top:320px;left: 150px;z-index: 1;color: #999999;position: absolute;cursor:text;">$contentPlaceholder</label>
                                <textarea name='content' style='width:99%;height:300px;' class='require_string' $length_limit>$text</textarea></div>



								$rr

                                <div class=backbtn>
                                <input type=submit name=submit style="cursor:pointer;" value=Publish class=button_add />
                                $save_link
                                <!--<a href='$rem_link' class=tool onclick='return confirm("Continue to remove record")'>Delete</a>-->
									<a href='$redirect' class=tool><input type=button name=submit value=Cancel class=button_add /></a>
                                </div>
                            </div>
                        </div>
                        $hv
                        <input type='hidden' name=dataid value='$dataid' />
                        <input type='hidden' name=view value='detail' />
                        <input type='hidden' name=do value='$do' />
                        <input type='hidden' name=update_section value='$panel' />
                    </form>
<script type="text/javascript">
                    $(document).ready(function(){

                    $(".button_add").click(function(ev) {

                    if ($(this).val() != 'Cancel') {
                    if ($('input[name=title]').val().trim() == '') {
                    ev.preventDefault();
                    $('input[name=title]').addClass('error_select');
                    $(this).removeAttr("disabled");
                    }
                    else {
                    $('input[name=title]').removeClass('error_select');
                    }

                    if ($('#tags option:selected').val() == '') {
                    ev.preventDefault();
                    $('#tags').addClass('error_select');
                    $(this).removeAttr("disabled");
                    }
                    else { $('#tags').removeClass('error_select');}
                    }

                    });

                     });
                     </script>
                     <style>
                     .discussion-invited-experts-container .as-selections {
                     padding-top:28px !important;
                     min-height:2px !important;
                     margin-top:0px !important;
                     }
                     discussion-invited-experts-container {
                     margin-bottom:8px !important;
                     }
                     </style>
END;
                }
            }
        }
        elseif ($view == 'listview') {
            $limit = 3;
            $_page = sget($_REQUEST, '_page');
            if (empty($_page)) $_page = 1;
            $start = ($_page - 1) * $limit;

            $sql    = "select count(*) as total from maenna_company_data where data_type = '%s' and companyid = %d and deleted != 1";
            $result = db_query($sql, array($data_type, $companyid));
            $Row    = db_fetch_object($result);
            $total  = $Row->total;

            $sql    = "select * from maenna_company_data where data_type = '%s' and companyid = %d and deleted != 1 order by dataid $orderby limit $start, $limit";
            $result = db_query($sql, array($data_type, $companyid));

            while (($Row = db_fetch_object($result)) !== false) {
                $access    = date('m/d/Y', $Row->access);
                $sub_title = htmlentities(strtoupper($Row->data_value), ENT_QUOTES | ENT_IGNORE, "UTF-8");
                if (empty($sub_title)) $sub_title = "&nbsp;";

                $text      = nl2br(htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                $dataid    = $Row->dataid;

                $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid&section=${section}";
                // $text = contentExcerpt($text, $more_link);

                $content .= "\n<div class=entry>";
                if ($op == 'write' || ($op == 'edit' && $user->uid == $Row->editorid))
                    $content .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid&section=${section}' class='tool' >edit</a></div>";

                $rem_link = $redirect . "&update_section=${panel}&do=remove&dataid=$dataid";

                //                    $content .= "<div style='float: right'><a href='$rem_link' class=tool onclick='return confirm(\"Continue to remove record\")'>Delete</a> | ";
                //                    $content .= "<a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid&section=${section}' class='tool' >edit</a></div>";
                $content .= "<div class='entry-title'>$sub_title &nbsp;</div><div class='entry-content'>" . _filter_autop(html_entity_decode($text)) . "</div></div><hr class='line' />";
            }
            $Pagenation = array(
                'total'        => $total,
                'limit'        => $limit,
                'baseurl'      => '/account?',
                'num_of_links' => 8,
            );
            $content .= pagination($Pagenation);
        }
    } elseif ($op == 'update') {
        date_default_timezone_set('EST');
        $do      = sget($_REQUEST, 'do');
        $Correct = false;

        if ($do == 'update' || $do == 'insert') {
            $dataid            = sget($_REQUEST, 'dataid');
            $sub_title         = sget($_REQUEST, 'title');
            $text              = sget($_REQUEST, 'content');
            $short_description = sget($_REQUEST, 'short_desc');
            $tags              = sget($_REQUEST, 'tags');

            $invitedExpertIdsString = trim($_REQUEST['invited_expert_ids'] ?: '', ', ');
            $invitedExpertIds = array();
            if (!empty($invitedExpertIdsString))
                $invitedExpertIds = explode(',', $invitedExpertIdsString);

            if (empty($text)) {
            } elseif ($dataid && ($do == 'update')) {

                if (sget($_REQUEST, 'submit') == 'Save') {
                    $status   = 0;
                    $DBValues = array(
                        'access'             => $time,
                        'short_description2' => $short_description,
                        'tags2'              => $tags,
                        'data_value_save'    => $sub_title,
                        'saved_content'      => $text,
                        'status'             => $status,
                        'editorid'           => $editorid
                    );
                } else {
                    $status   = 1;
                    $DBValues = array(
                        'access'            => $time,
                        'short_description' => $short_description,
                        'tags'              => $tags,
                        'data_value'        => $sub_title,
                        'data_value2'       => $text,
                        'status'            => $status,
                        'editorid'          => $editorid
                    );
                }


                foreach ($DBValues as $key => $val) {
                    $SQL_STR["$key"] = "$key = '%s'";
                }
                $SQL_STR["editorid"] = "editorid=%d";

                if ('share_knowledge' == $mtab) {
                    unset($SQL_STR["editorid"]);
                    unset($DBValues["editorid"]);
                }

                $sql                 = "update maenna_company_data set " . implode(',', $SQL_STR) . " where dataid = " . ((int)$dataid) . " limit 1";
                if (db_query($sql, $DBValues)) $Correct = true;

                if ($Correct && 'share_knowledge' == $mtab) {
                    $companyService = new \Clewed\Company\Service();
                    $companyService->replaceDiscussionInvitedExpertIds($dataid, $invitedExpertIds);
                }

                header('location:' . $base_url . '/account?tab=companies&page=company_detail&id=' . $companyid . '&mtab=' . $mtab);
            }
              elseif ($do == 'insert') {
                if (sget($_REQUEST, 'submit') == 'Save') {
                    $status = 0;
                } else {
                    $status = 1;
                }

                $DBKeys = array(
                    'companyid',
                    'access',
                    'data_type',
                    'short_description2',
                    'tags2',
                    'data_value_save',
                    'saved_content',
                    'short_description',
                    'tags',
                    'data_value',
                    'data_value2',
                    'editorid',
                    'status'
                );

                $DBValues = array($companyid, $time, $data_type, $short_description, $tags, $sub_title, $text, $short_description, $tags, $sub_title, $text, $editorid, $status);
                $SQL_STR  = array("%d", "'%s'", "'%s'", "'%s'", "'%s'", "'%s'", "'%s'", "'%s'", "'%s'", "'%s'", "'%s'", "%d", "'%s'");
                $sql      = "insert into maenna_company_data (" . implode(',', $DBKeys) . ") values(" . implode(',', $SQL_STR) . ")";

                if (db_query($sql, $DBValues)) $Correct = true;

                $inserted_id = mysql_insert_id();

                if (true === $Correct && 'share_knowledge' == $mtab) {
                    $userService = new Clewed\User\Service();
                    $author = $userService->get(array($editorid));
                    $author = $author[$editorid];
                    if (!empty($author) && empty($invitedExpertIds)) {
                        $eventType = 'project_proposal_added';
                        if ($editorid == $companyid || $author['is_admin'])
                            $eventType = 'project_broadcast_added';

                        $notificationService = new Clewed\Notifications\NotificationService();
                        $notificationService->registerEvent($eventType, $companyid, $editorid);
                    }
                    $options = array(
                        'title' => strip_tags($sub_title),
                        'message' => strip_tags($text),
                    );
                    $companyService = new \Clewed\Company\Service();
                    $companyService->replaceDiscussionInvitedExpertIds($inserted_id, $invitedExpertIds, $options);
                }

                if ($mtab == 'analysis') {
                    $mtab        = 'summary&type=details&analysis_id=' . $inserted_id;
                }

                if ($Correct == true) {
                    //                        $q1        = mysql_query("SELECT IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname, gender FROM maenna_people WHERE pid = " . $user->uid);
                    //                        $pro_data  = mysql_fetch_array($q1);
                    //                        $comp      = mysql_query("SELECT first_name, mail FROM users LEFT JOIN users_extend USING(uid) WHERE uid = " . ((int)$companyid));
                    //                        $comp_data = mysql_fetch_array($comp);
                    //
                    //                        $compname = $comp_data['first_name'];
                    //                        $compmail = $comp_data['mail'];
                    //                        $proname  = $pro_data['firstname'];

                    //                        $content = <<< END
                    //Hi $compname,
                    //
                    //$proname added information on your analysis page on Clewed. Login at www.clewed.com to review.
                    //
                    //The clewed team!
                    //END;
                    //                        $headers = 'From: noreply@clewed.com' . "\r\n";
                    //mail($compmail, 'New information on analysis page on Clewed!', $content, $headers);
                }

                header('location:' . $base_url . '/account?tab=companies&page=company_detail&id=' . $companyid . '&mtab=' . $mtab);
            }
            } elseif ($do == 'remove') {
            $dataid = sget($_REQUEST, 'dataid');
            if ($dataid) {
                $sql = "update maenna_company_data set access = '%s', editorid = %d, deleted = 1 where dataid = %d limit 1";
                if (db_query($sql, array($time, $editorid, $dataid))) $Correct = true;
            }
        }
        if ($Correct) drupal_set_message("Operation Successful");
        else {
            drupal_set_message("Operation Failed", 'error');
        }
        return;
    }

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
/*function multi($op = null)
{
    global $user,$AccessObj;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $section = sget($_REQUEST,'section');
    $panel = "multi";

    $content = '';
    $orderby = 'desc';
    $mtab = sget($_REQUEST, 'mtab');
    if(empty($mtab))
    {
        foreach($AccessObj->Com_sections as $tab_name => $Tab_data)
        {
            if($Tab_data['access'] != 'hide' && $tab_name != 'common')
            {
                $mtab = $tab_name;
                break;
            }
        }
    }
    if(empty($section))$section = $mtab;
    if(empty($section)) {echo "Invalid section - multi";return;}
    $length_limit = "";
    $redirect = rebuild_url(array('tab','page', 'id')) . "&mtab=$mtab&section=$section";
    $back = rebuild_url(array('tab','page', 'id')) . "&mtab=$mtab";
    switch($section)
    {
        case "minutes":
            $title = "ADVICE";
            $data_type = 'minutes';
            $length_limit = "maxlength=3000";
            break;
        case "maenna_analysis":
        case 'analysis':
            $title = "ANALYSIS";
            $data_type = 'analysis';
            $orderby = '';
            break;
        case "maenna_plan":
            $title = "Plan";
            $data_type = 'plan';
            $orderby = '';
            break;
        case 'mgmt_analysis':
            $title = "Management Discussion & Analysis";
            $data_type = 'mgmtanalysis';
            break;
        case 'council_advice':
            $title = "Council Advice and Analysis";
            $data_type = 'advice';
            break;
        case 'priority':
            $title = "Priority";
            $data_type = 'priority';
            $orderby = '';
            break;
    }
    $Block['title'] = $title;

    if($op == 'write')
    {
        $add_link = $redirect . "&panel=${panel}&view=add";
        $Block['title'] .= "<div class=editbtn><a href='$add_link' class=tool>Add</a></div>";
    }

    if($op && ($op != 'update'))
    {
        $view = sget($_REQUEST, 'view');

        if($view == 'detail' || $view == 'edit' || $view == 'add')
        {

            $dataid = sget($_REQUEST, 'dataid');
            $Row = array();
            if($dataid)
            {
                $sql = "select * from maenna_company_data where dataid = %d limit 1";
                $result = db_query($sql, array($dataid));
                $Row = db_fetch_array($result);
            }elseif(empty($dataid) && ($view == 'detail')){
                $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' order by dataid desc limit 1";
                $result = db_query($sql, array($companyid, $data_type));
                $Row = db_fetch_array($result);
            }

            if($Row !== false)
            {
                if($view == 'detail'){
                    $dataid = sget($Row, 'dataid');
                    $sub_title = ucwords(sget($Row,'data_value'));
                    if($sub_title)
                        $sub_title = htmlentities($sub_title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    else
                        $sub_title = "&nbsp;";
                    $text = sget($Row, 'data_value2');
                    $text = nl2br(htmlentities($text, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                    $rem_link = '';
                     if($op == 'write'){
                        $rem_link = $redirect . "&update_section=${panel}&do=remove&dataid=$dataid";
                        $rem_link = "<a href='$rem_link' class=tool onclick='return confirm(\"Continue to remove record\")'>Delete</a>";
                        $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
                        $edit_link = "<div class=editbtn><a href='$edit_link' class=tool>Edit</a></div>";
                   }else{$rem_link = '';$edit_link = '';}

                    $content = "<div class=entry>
                                    <div class=entry-title>$sub_title &nbsp;$edit_link</div>
                                    <div class=entry-content>" . _filter_autop(html_entity_decode($text)) . "</div>
                                </div>";

                    $content .= "<div class=backbtn>

                                    <a href='$back' class=button>back</a>&nbsp;&nbsp;
                                    $rem_link
                                </div>";

                }elseif(($view == 'edit' || $view == 'add') && $op == 'write')
                {
                    $sub_title = ucwords(sget($Row,'data_value'));
                    $dataid = sget($Row, 'dataid');
                    $text = sget($Row, 'data_value2');
                    if($view == 'edit'){
                        $title = "Edit " . $Block['title'];
                        $do = 'update';
                        $rem_link = $redirect . "&panel=${panel}&view=listview&update_section=${panel}&do=remove&dataid=$dataid";
                    }else{
                        $title = "Add " . $Block['title'];
                        $do = 'insert';
                        $rem_link = '';
                    }
                    $Block['title'] = $title;
                    $hv = hidden_post_values(array('tab','page','id', 'mtab','panel','section'));
                    $content .= <<< END
                    <form action='/account' method='post' onsubmit='return check_input();'>
                        <div class=entry>
                            <div class=entry-content>
                                <div>Title:<br /><input type=text name=title value='$sub_title' class='require_string' style='width:400px;' /></div><br />
                                <div>Content:<br /><textarea name='content' style='width:99%;height:500px;'  $length_limit>$text</textarea>
                                <div class=backbtn>
                                <input type=submit name=submit value=submit class=button />&nbsp;&nbsp;
                                <a href='$rem_link' class=tool onclick='return confirm("Continue to remove record")'>Delete</a>

                                    &nbsp;&nbsp;
                                    <a href='$redirect' class=tool>Cancel</a>
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


        }
        elseif($view == 'listview')
        {
            $limit = 3;
            $_page = sget($_REQUEST,'_page');
            if(empty($_page)) $_page = 1;
            $start = ($_page - 1) * $limit;

            $sql = "select count(*) as total from maenna_company_data where data_type = '$data_type' and companyid = %d and deleted != 1";
            $result = db_query($sql, array($companyid));
            $Row = db_fetch_object($result);
            $total = $Row->total;

            $sql = "select * from maenna_company_data where data_type = '$data_type' and companyid = %d and deleted != 1 order by dataid $orderby limit $start, $limit";
            $result = db_query($sql, array($companyid));
            while(($Row = db_fetch_object($result)) !== false)
            {
                $access = date('m/d/Y',$Row->access);
                $sub_title = htmlentities(ucwords($Row->data_value), ENT_QUOTES | ENT_IGNORE, "UTF-8");
                if(empty($sub_title))$sub_title = "&nbsp;";

                $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                $dataid = $Row->dataid;
                $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid&section=${section}";
               // $text = contentExcerpt($text, $more_link);
                $content .= "\n<div class=entry>";
                $content .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid&section=${section}' class='tool' >edit</a></div>";
                $content .= "<div class='entry-title'>$sub_title &nbsp;</div><div class='entry-content'>" . _filter_autop(html_entity_decode($text)) ."</div></div><hr class='line' />";
            }
            $Pagenation = array('total' => $total,
                'limit' =>$limit,
                'baseurl' => '/account?',
                'num_of_links' => 8,
            );
            $content .= pagination($Pagenation);
        }
    }elseif($op == 'update')
    {
        $do = sget($_REQUEST, 'do');
        $Correct = false;
        if($do == 'update' || $do == 'insert')
        {
            $dataid = sget($_REQUEST,'dataid');
            $sub_title = sget($_REQUEST, 'title');
            $text = sget($_REQUEST, 'content');

            if( empty($text))
            {

            }
            elseif($dataid && ($do == 'update'))
            {
                $DBValues = array('access' => $time,
                                  'data_value' => $sub_title,
                                  'data_value2' => $text,
                                  'editorid' => $editorid);
                foreach($DBValues as $key => $val)
                {
                    $SQL_STR["$key"] = "$key = '%s'";
                }
                $SQL_STR["editorid"] = "editorid=%d";
                $sql = "update maenna_company_data set " . implode(',', $SQL_STR) . " where dataid = $dataid limit 1";
                if(db_query($sql, $DBValues))$Correct = true;

            }elseif($do == 'insert')
            {
                $DBKeys = array('companyid',
                                'access',
                                'data_type',
                                'data_value',
                                'data_value2',
                                'editorid');
                $DBValues = array($companyid, $time, $data_type, $sub_title, $text, $editorid);
                $SQL_STR = array("%d", "'%s'","'%s'","'%s'","'%s'","%d");
                $sql = "insert into maenna_company_data (".implode(',',$DBKeys).") values(".implode(',',$SQL_STR).")";
                if(db_query($sql, $DBValues)) $Correct = true;
            }
        }elseif($do == 'remove')
        {
            $dataid = sget($_REQUEST, 'dataid');
            if($dataid)
            {
                $sql = "update maenna_company_data set access = '%s', editorid = %d, deleted = 1 where dataid = %d limit 1";
                if(db_query($sql, array($time, $editorid, $dataid))) $Correct = true;
            }
        }
        if($Correct)drupal_set_message("Operation Successful");
        else{ drupal_set_message("Operation Failed", 'error'); }
        return ;
    }

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}*/









/***
 * Two new Columns
 * Key finance
 * Use of funds
 **/
function key_finance($op = null){
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Key Financials";
    $data_type = 'key_finance';
    $panel = "information_table";
    global $redirect;
    $content = '';
    $section = __FUNCTION__;
    $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));

    $access = access_information_tables($companyid,'key_finance');

    if ($access["write"])
        $Block['title'] .= "<div class='editbtn text-uppercase'><a href='$redirect&panel=${panel}&section=${section}&view=edit' class=tool>Edit</a></div>";


    if ($access["read"]) {
        $db = \Clewed\Db::get_instance();


        $result = $db->get_array(
            "select * from maenna_company_data where companyid = :company and data_type = :data_type and deleted != 1 order by data_attr*1 asc, dataid asc limit 6",
            array(
                ':company' => $companyid,
                ':data_type' => $data_type,
            )
        );

        if (count($result) > 0){
            $content = " <table class='lateral-table'>";

            foreach($result as $Row) {
                $title = $Row['data_value'];
                if ($title) $title = htmlentities($title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                else $title = "&nbsp;";

                $value = htmlentities(str_replace('Actual', 'Act.', $Row['data_value3']), ENT_QUOTES);

                $content .= "
                <tr>
                    <td class='col col-first'>$title</td>
                    <td class='col col-second'>$value</td>
                </tr>
            ";



            }
            $content .= "</table>";
        }

    }
    if ($content != '') {
        $viewall_link = $redirect . "&panel=${panel}&view=detail&section=${section}";
        $content .= "\n<div class='viewbtn'><a href='$viewall_link' class=tool>More</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content, 'table_two_column');
    }
    return $Block;
}

function use_of_funds($op = null){
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Use Of Funds";
    $data_type = 'use_of_funds';
    $panel = "information_table";
    global $redirect;
    $content = '';
    $section = __FUNCTION__;
    $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
    $access = access_information_tables($companyid,'use_of_funds');

    if ($access["write"])
    $Block['title'] .= "<div class='editbtn text-uppercase'><a href='$redirect&panel=${panel}&section=${section}&view=edit' class=tool>Edit</a></div>";

    if ($access["read"]) {
        $db = \Clewed\Db::get_instance();

        $result = $db->get_array(
            "select * from maenna_company_data where companyid = :company and data_type = :data_type and deleted != 1 order by data_attr*1 asc, dataid asc limit 6",
            array(
                ':company' => $companyid,
                ':data_type' => $data_type,
            )
        );

        if (count($result) > 0){
            $content = " <table class='lateral-table'>";
            foreach ($result as $Row) {
                $title = $Row['data_value'];
                if ($title) $title = htmlentities($title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                else $title = "&nbsp;";

                $value = htmlentities(str_replace('Actual', 'Act.', $Row['data_value3']), ENT_QUOTES);

                $content .= "
                <tr>
                    <td class='col col-first'>$title</td>
                    <td class='col col-second'>$value</td>
                </tr>
            ";

            }
            $content .= "</table>";
        }

    }

    if ($content != '') {
        $viewall_link = $redirect . "&panel=${panel}&view=detail&section=${section}";
        $content .= "\n<div class='viewbtn'><a href='$viewall_link' class=tool>More</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content, 'table_two_column');
    }

    return $Block;
}

function information_table($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $tab = sget($_REQUEST, 'tab');
    $page = sget($_REQUEST, 'page');
    $panel = __FUNCTION__;
    //$data_type = sget($_REQUEST, 'datatype');
    global $redirect;
    $section = sget($_REQUEST, 'section');
    $queries = [
        'tab' => $tab,
        'page' => $page,
        'id' => $companyid,
    ];
    $url = http_build_query($queries, '', '&');

    $content = '';
    $HV =  hidden_post_values(array('tab', 'page', 'mtab', 'id', 'section', 'panel'));

    switch ($section) {
        case "use_of_funds":
            $title = "USE OF FUNDS";
            $data_type = 'use_of_funds';
            break;
        case "key_finance":
            $title = "Key Financials";
            $data_type = 'key_finance';
            break;
    }
    $Block['title'] = $title;
    $access = access_information_tables($companyid,$data_type);

    if ($op && $op != 'update' && $access['read']) {
        $view = sget($_REQUEST, 'view');

        if ($view == 'edit' || $view == 'detail') {

            if ($access['write']){
                $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add&section=$section' class=tool>Add</a></div>";
            }

            $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and deleted != 1 order by data_attr * 1 asc, dataid asc";
            $result = db_query($sql, array($companyid, $data_type));

            if ($view == 'detail') {
                $content = "<table class='metrics with-border' class='table-list'>";
                while ($Row = db_fetch_object($result)) {
                    $dataid = $Row->dataid;
                    $column1 = htmlentities($Row->data_value, ENT_QUOTES);
                    $url = $Row->data_value2;
                    $column2 = htmlentities($Row->data_value3, ENT_QUOTES);
                    $column3 = htmlentities($Row->data_value4, ENT_QUOTES);
                    $position = $Row->data_attr;
                    if (empty($url))
                        $content .= "\n<tr><td>$column1</td><td>$column2</td><td>$column3</td></tr>";
                    else
                        $content .= "\n<tr><td><a href='$url' target='_blank'>$column1</a></td><td>$column2</td><td>$column3</td></tr>";
                }
                $content .= "</table><br><a href='$redirect&panel=${panel}&view=edit&section=$section' class=button>Edit</a>";
            }
            elseif (($view == 'edit') && ($access['write'])) {

                $content = "<form action='/account' method='post'><table class=report style='width:600px'>" .
                    "<tr>
                         <td >Delete</td>
                         <td>Title</td>
                         <td>Value 1</td>
                     </tr>";

                while ($Row = db_fetch_object($result)) {
                    $dataid = $Row->dataid;
                    $column1 = $Row->data_value;
                    $column2 = $Row->data_value3;
                    $URL = '';

                    $content .= "<tr>
                                    <td><input type=checkbox name='delete[]' value='del_$dataid' /></td>
                                    <td><input type=text name='col1_$dataid' value='$column1' /><br />
                                        $URL
                                    </td>
                                     <td><input type=text name='col2_$dataid' value='$column2' style='width:80px !important' /></td>
                                </tr>";

                }
                $content .= "</table>";
                $content .= "
                    <div><input type=submit name=submit value='Submit'  class=button /> <a href='$redirect' class=button>Cancel</a>
                    </div>
                    $HV
                    <input type=hidden name=view value='detail'/>
                    <input type=hidden name=do value='update' />
                    <input type='hidden' name=update_section value='$panel' />
                </form>";
            }

        }
        elseif (strcasecmp($view, 'add') == 0) {
            $URL = '';

                $content = "<form action='/account' method='post'>
                <div class=row>
                    <label>Title</labe><br / ><input type=text name='title' value=''  style='' />
                </div>
                $URL
                <div class=row>
                    <label>Column 1</labe><br / ><input type=text name='val1' value=''  style='width:80px;' />
                </div>

                <div>
                    <input type=submit name=submit value=submit class=button /> <a href='$redirect&panel=${panel}&view=detail&section=$section' class=button>Cancel</a>
                    $HV
                    <input type=hidden name=view value='detail' />
                    <input type=hidden name=do value='insert' />
                    <input type=hidden name=position value='100' />
                    <input type='hidden' name=update_section value='$panel' />
                </div>
            </form>";

        }
        else{
            drupal_goto('account',$url);
        }
    }
    elseif ($op == 'update') {
        $do = sget($_REQUEST, 'do');
        $Correct = false;
        if ($do == 'insert') {
            $title = sget($_REQUEST, 'title');
            $url = sget($_REQUEST, 'url');
            $val1 = sget($_REQUEST, 'val1');
            $val2 = sget($_REQUEST, 'val2');

            if (empty($title)) {
                drupal_set_message('Please enter a title', 'error');
                return;
            }
            $sql = "insert into maenna_company_data (companyid,access, data_type,data_attr, data_value, data_value2, data_value3,data_value4, editorid) values(%d, '%s', '%s','%s', '%s', '%s', '%s','%s', %d)";
            $DBValues = array($companyid, $time, $data_type, '100', $title, $url, $val1, $val2, $editorid);

            if (db_query($sql, $DBValues)) {
                drupal_set_message('New record added');
            } else {
                drupal_set_message('Operation failed');
            }
            return;
        }
        elseif ($do == 'update') {
            //process delete first
            $DEL = array();
            if (isset($_REQUEST['delete']) && (count($_REQUEST['delete']) > 0)) {
                foreach ($_REQUEST['delete'] as $Item) {
                    $del_dataid = str_replace('del_', '', $Item);
                    if (intval($del_dataid) > 0) {
                        $sql = "update maenna_company_data set deleted = 1, access = '%s', editorid = %d where dataid = %d limit 1";
                        $DBValues = array($time, $editorid, $del_dataid);
                        $Correct = db_query($sql, $DBValues);
                        $DEL[] = $del_dataid;
                    }
                }
            }
            // update record
            foreach ($_REQUEST as $key => $val) {
                if (substr($key, 0, 5) == 'col1_') {
                    $dataid = str_replace('col1_', '', $key);
                    if (in_array($dataid, $DEL)) continue;
                    $data_value = $val;
                    $data_value2 = sget($_REQUEST, 'url_' . $dataid);
                    $data_value3 = sget($_REQUEST, 'col2_' . $dataid);
                    $data_value4 = sget($_REQUEST, 'col3_' . $dataid);
                    $data_attr = sget($_REQUEST, 'pos_' . $dataid);
                    $sql = "update maenna_company_data set data_attr = '%s', data_value = '%s', data_value2='%s', data_value3='%s', data_value4 = '%s', editorid=%d, access='%s' where dataid = %d limit 1";
                    $DBValues = array($data_attr, $data_value, $data_value2, $data_value3, $data_value4, $editorid, $time, $dataid);
                    $Correct = db_query($sql, $DBValues);
                    if (!$Correct) break;
                }
            }
        }
        elseif ($do == 'remove') {
            //test_array($_POST);
        }
        if ($Correct) drupal_set_message("Operation Successful");
        else drupal_set_message("Operation failed", 'error');
        return;
    }
    else{
        drupal_goto('account',$url);
    }

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

/******end******/



function metrics($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Metrics";
    $data_type = 'metrics';
    $panel = "columns";
    global $redirect;
    $content = '';
    $section = __FUNCTION__;
    $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));

    if ($op == 'write')
        $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&section=${section}&view=edit' class=tool>Edit</a></div>";

    if ($op) {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and deleted != 1 order by data_attr*1 asc, dataid asc limit 6";
        $result = db_query($sql, array($companyid, $data_type));
        while ($Row = db_fetch_object($result)) {
            $title = $Row->data_value;
            if ($title) $title = htmlentities($title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            else $title = "&nbsp;";

            $col2 = htmlentities(str_replace('Actual', 'Act.', $Row->data_value3), ENT_QUOTES);
            $col3 = htmlentities($Row->data_value4, ENT_QUOTES);
            $content .= "<div class='row singlerow metrics'>
                <div style='overflow:hidden;width:145px;'>$title &nbsp;</div>
                <div style='width:35px;position:absolute;top:0;left:155px;overflow:hidden;padding:inherit;text-align:right;'>$col2</div>
                <div style='width:35px;position:absolute;top:0;left:195px;overflow:hidden;padding:inherit;text-align:right;'>$col3</div>
            </div>";
        }
        if ($content != '') {

            $viewall_link = $redirect . "&panel=${panel}&view=detail&section=${section}";

            $content .= "\n<div class='viewbtn'><a href='$viewall_link' class=tool>More</a></div>";
        }
    }

    $Block['body'] = sidebar_box($Block['title'], $content, 'metrics');

    return $Block;
}

function columns($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'id');
    $tab = sget($_REQUEST, 'tab');
    $panel = "columns";
    //$data_type = sget($_REQUEST, 'datatype');
    global $redirect;
    $section = sget($_REQUEST, 'section');

    $content = '';
    $HV =  hidden_post_values(array('tab', 'page', 'mtab', 'id', 'section', 'panel'));

    switch ($section) {
        case "metrics":
            $title = "Metrics";
            $data_type = 'metrics';
            break;
        case "watch_list":
            $title = "Watch List";
            $data_type = 'watchlist';
            break;
    }
    $Block['title'] = $title;

    if ($op && $op != 'update') {
        $view = sget($_REQUEST, 'view');

        if ($view == 'edit' || $view == 'detail') {

            if ($op == 'write')
                $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add&section=$section' class=tool>Add</a></div>";

            $Row = array();

            $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and deleted != 1 order by data_attr * 1 asc, dataid asc";
            $result = db_query($sql, array($companyid, $data_type));


            if ($view == 'detail') {
                $content = "<table class='metrics with-border' class='table-list'>";
                while ($Row = db_fetch_object($result)) {
                    $dataid = $Row->dataid;
                    $column1 = htmlentities($Row->data_value, ENT_QUOTES);
                    $url = $Row->data_value2;
                    $column2 = htmlentities($Row->data_value3, ENT_QUOTES);
                    $column3 = htmlentities($Row->data_value4, ENT_QUOTES);
                    $position = $Row->data_attr;
                    if (empty($url))
                        $content .= "\n<tr><td>$column1</td><td>$column2</td><td>$column3</td></tr>";
                    else
                        $content .= "\n<tr><td><a href='$url' target='_blank'>$column1</a></td><td>$column2</td><td>$column3</td></tr>";
                }
                $content .= "</table><br><a href='$redirect&panel=${panel}&view=edit&section=$section' class=button>Edit</a>";
            }

            elseif (($view == 'edit') && ($op == 'write')) {

                $content = "<form action='/account' method='post'><table class=report style='width:600px'>" .
                    "<tr>
                         <td >Delete</td>
                         <td>Title</td>
                         <td>Value 1</td>
                         <td>Value 2</td>
                         <td>Position</td>
                     </tr>";

                while ($Row = db_fetch_object($result)) {
                    $dataid = $Row->dataid;
                    $column1 = $Row->data_value;
                    $url = $Row->data_value2;
                    $column2 = $Row->data_value3;
                    $column3 = $Row->data_value4;
                    $position = $Row->data_attr;
                    $URL = '';
                    if ($data_type == 'watchlist') {
                        $URL = "<input type=text name='url_$dataid' value='$url' />";
                    }else{
                    $content .= "<tr>
                                    <td><input type=checkbox name='delete[]' value='del_$dataid' /></td>
                                    <td><input type=text name='col1_$dataid' value='$column1' /><br />
                                        $URL
                                    </td>
                                    <td><input type=text name='col2_$dataid' value='$column2' style='width:80px !important' /></td>
                                    <td><input type=text name='col3_$dataid' value='$column3' style='width:80px !important' /></td>
                                    <td><input type=text name='pos_$dataid' value='$position' style='width:40px !important' /></td>
                                </tr>";
                    }
                }
                $content .= "</table>";
                $content .= "
                    <div><input type=submit name=submit value='Submit'  class=button /> <a href='$redirect' class=button>Cancel</a>
                    </div>
                    $HV
                    <input type=hidden name=view value='detail'/>
                    <input type=hidden name=do value='update' />
                    <input type='hidden' name=update_section value='$panel' />
                </form>";
            }

        }
            elseif (strcasecmp($view, 'add') == 0) {
            $URL = '';
            if ($data_type == 'watchlist') {
                $URL = "<div class=row>
                    <label>URL</labe><br / ><input type=text name='url' value=''  style='' />
                </div>";

                $content = "<form action='/account' method='post'>
                <div class=row>
                    <label>Title</labe><br / ><input type=text name='title' value=''  style='' />
                </div>
                $URL
                <div class=row>
                    <label>Revenue</labe><br / ><input type=text name='val1' value=''  style='width:80px;' />
                </div>
                <div class=row>
                    <label>ROE</labe><br / ><input type=text name='val2' value=''  style='width:80px;' />
                </div>


                <div>
                    <input type=submit name=submit value=submit class=button /> <a href='$redirect&panel=${panel}&view=detail&section=$section' class=button>Cancel</a>
                    $HV
                    <input type=hidden name=view value='detail' />
                    <input type=hidden name=do value='insert' />
                    <input type=hidden name=position value='100' />
                    <input type='hidden' name=update_section value='$panel' />
                </div>
            </form>";
            }
            else {
                $content = "<form action='/account' method='post'>
                <div class=row>
                    <label>Title</labe><br / ><input type=text name='title' value=''  style='' />
                </div>
                $URL
                <div class=row>
                    <label>Column 1</labe><br / ><input type=text name='val1' value=''  style='width:80px;' />
                </div>
                <div class=row>
                    <label>Column 2</labe><br / ><input type=text name='val2' value=''  style='width:80px;' />
                </div>


                <div>
                    <input type=submit name=submit value=submit class=button /> <a href='$redirect&panel=${panel}&view=detail&section=$section' class=button>Cancel</a>
                    $HV
                    <input type=hidden name=view value='detail' />
                    <input type=hidden name=do value='insert' />
                    <input type=hidden name=position value='100' />
                    <input type='hidden' name=update_section value='$panel' />
                </div>
            </form>";
            }
        }
    }

    elseif ($op == 'update') {
        $do = sget($_REQUEST, 'do');
        $Correct = false;
        if ($do == 'insert') {
            $title = sget($_REQUEST, 'title');
            $url = sget($_REQUEST, 'url');
            $val1 = sget($_REQUEST, 'val1');
            $val2 = sget($_REQUEST, 'val2');

            if (empty($title)) {
                drupal_set_message('Please enter a title', 'error');
                return;
            }
            $sql = "insert into maenna_company_data (companyid,access, data_type,data_attr, data_value, data_value2, data_value3,data_value4, editorid) values(%d, '%s', '%s','%s', '%s', '%s', '%s','%s', %d)";
            $DBValues = array($companyid, $time, $data_type, '100', $title, $url, $val1, $val2, $editorid);

            if (db_query($sql, $DBValues)) {
                drupal_set_message('New record added');
            } else {
                drupal_set_message('Operation failed');
            }
            return;
        }
        elseif ($do == 'update') {
            //process delete first
            $DEL = array();
            if (isset($_REQUEST['delete']) && (count($_REQUEST['delete']) > 0)) {
                foreach ($_REQUEST['delete'] as $Item) {
                    $del_dataid = str_replace('del_', '', $Item);
                    if (intval($del_dataid) > 0) {
                        $sql = "update maenna_company_data set deleted = 1, access = '%s', editorid = %d where dataid = %d limit 1";
                        $DBValues = array($time, $editorid, $del_dataid);
                        $Correct = db_query($sql, $DBValues);
                        $DEL[] = $del_dataid;
                    }
                }
            }
            // update record
            foreach ($_REQUEST as $key => $val) {
                if (substr($key, 0, 5) == 'col1_') {
                    $dataid = str_replace('col1_', '', $key);
                    if (in_array($dataid, $DEL)) continue;
                    $data_value = $val;
                    $data_value2 = sget($_REQUEST, 'url_' . $dataid);
                    $data_value3 = sget($_REQUEST, 'col2_' . $dataid);
                    $data_value4 = sget($_REQUEST, 'col3_' . $dataid);
                    $data_attr = sget($_REQUEST, 'pos_' . $dataid);
                    $sql = "update maenna_company_data set data_attr = '%s', data_value = '%s', data_value2='%s', data_value3='%s', data_value4 = '%s', editorid=%d, access='%s' where dataid = %d limit 1";
                    $DBValues = array($data_attr, $data_value, $data_value2, $data_value3, $data_value4, $editorid, $time, $dataid);
                    $Correct = db_query($sql, $DBValues);
                    if (!$Correct) break;
                }
            }
        } elseif ($do == 'remove') {
            //test_array($_POST);
        }
        if ($Correct) drupal_set_message("Operation Successful");
        else drupal_set_message("Operation failed", 'error');
        return;
    }


    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function watch_list($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Watch List";
    $data_type = 'watchlist';
    $panel = "columns";
    $section = __FUNCTION__;
    global $redirect;
    $content = '';
    $HPV =  hidden_post_values(array('tab', 'page', 'mtab', 'id'));

    //if($op == 'write')$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&section=${section}&view=edit' class=tool>Edit</a></div>";
    if ($op) {
        $content = "\n<div class=row>
                <div style='overflow:hidden;width:110px;'>&nbsp;</div>
                <div style='width:20%;position:absolute;top:0;left:120px;overflow:hidden;padding:inherit;'>REV</div>
                <div style='width:30;position:absolute;top:0;left:200px;overflow:hidden;padding:inherit;'>ROE</div>
            </div>";

        $sql = "select  maenna_connections.*,
                        financial_table.*
                from    maenna_connections
                left join   (select companyid as com_id,
                                    data_value as revenue,
                                    data_value2 as earnings,
                                    data_value3 as equity,
                                    data_attr
                            from    maenna_company_data
                            inner join  (   select companyid as newid,
                                            max(data_attr) as recent_year
                                            from maenna_company_data
                                            where data_type = 'financial'
                                            and deleted != 1
                                            group by companyid
                                        )as  grouped
                                    on  maenna_company_data.companyid = grouped.newid and
                                        maenna_company_data.data_attr = grouped.recent_year
                            where   data_type = 'financial' and
                                    deleted != 1
                            ) as financial_table on maenna_connections.assignee_uid = financial_table.com_id
                where   conntype = 'watchlist' and
                        status = 'active' and
                        target_uid = '%s'";
        $result = db_query($sql, array($companyid));
        while ($Row = db_fetch_array($result)) {
            //test_array($Row);
            $com_id = $Row['assignee_uid'];
            $com_maennaid = "<a href='/account?tab=companies&page=company_detail&id=$com_id&closebtn=1'>" . getProjectName($com_id) . "</a>";
            $revenue = sget($Row, 'revenue', 'int');
            $earnings = sget($Row, 'earnings', 'int');
            //var_dump($earings);
            $equity = sget($Row, 'equity', 'int');
            $col2 = $col3 = '-';
            if ($revenue) {
                $col2 = '$' . number_format($revenue);
            }
            if ($equity && $earnings) {

                $col3 = number_format(($earnings / $equity) * 100, 2) . "%";
            }
            $content .= "\n<div class=row>
                <div style='overflow:hidden;width:110px;background:none'>$com_maennaid</div>
                <div style='width:70px;position:absolute;top:0;left:110px;overflow:hidden;padding:inherit;background:none'>$col2</div>
                <div style='width:50px;position:absolute;top:0;left:186px;overflow:hidden;padding:inherit;background:none'>$col3</div>
            </div>";
        }
    }

    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function com_premium($op = null)
{
    require_once("." . base_path() . path_to_theme() . "/includes/classes/maenna_users.inc");
    global $AccessObj;
    global $redirect;
    if ($AccessObj->user_type != 'super') return '';
    $editorid = $AccessObj->uid;
    $companyid = sget($_REQUEST, 'id');
    $section = __FUNCTION__;
    $time = time();
    $Block['title'] = "Account Status";
    $Block['title'] .= "<div class=editbtn><a href='$redirect&section=${section}&view=edit_acct' class=tool>Edit</a></div>";
    $panel = "columns";
    $section = __FUNCTION__;
    $redirect = rebuild_url(array('tab', 'page', 'id', 'mtab'));
    $content = '';
    $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
    if ($op && $op !== 'update') {
        $view = sget($_REQUEST, 'view');
        $M = Mae_users::is_premium_acct($companyid);
        $content = sget($M, 'detail');
        if ($view == 'edit_acct' && sget($_REQUEST, 'section') == $section) {
            $content .= "<div class='row' style='margin-top:15px;'>
                <b>Change Account Status</b>
                <form method='post' action='/account'>
                <table>
                    <tr>
                        <td>Status:</td>
                        <td>
                            <select name=status style='width:80px;'>
                                <option value=regular>Regular</option>
                                <option value=premium>Premium</option>
                                <option value=level1>Level 1</option>
                                <option value=level2>Level 2</option>
                                <option value=level3>Level 3</option>
                                <option value=level4>Level 4</option>
                                <option value=level5>Level 5</option>
                                <option value=level6>Level 6</option>
                                <option value=level7>Level 7</option>
                                <option value=level8>Level 8</option>
                                <option value=level9>Level 9</option>
                                <option value=level10>Level 10</option>
                            </select>
                        </td>
                    </tr>
                    <tr>
                        <td>Expires on:</td>
                        <td><input type=text name='date' value='' class='datepicker' style='width:80px;' /></td>
                    </tr>
                    <tr>
                        <td></td>
                        <td><input type=submit name=submit value=submit class=button /> <a href='$redirect&section=com_premium' class=button>cancel</a></td>
                    </tr>
                </table>
                $HPV
                \n<input type=hidden name='section' value='$section' />
                \n<input type=hidden name='update_section' value='$section' />
                </form>
            </div><script type='text/javascript'>init_datepicker();</script>";
        }
    } elseif ($op == 'update') {
        $status = sget($_REQUEST, 'status');
        $date = sget($_REQUEST, 'date');
        if (empty($companyid)) {
            drupal_set_message("Invalid company id", 'error');
        } elseif (empty($status)) {
            drupal_set_message("Please select a status", 'error');
        } elseif ($status == 'premium' && empty($date)) {
            drupal_set_message("Please select a date", 'error');
        } else {
            if ($date) {
                $dt = @strtotime($date);
            } else $dt = '';
            if ($status == 'premium' && empty($dt)) {
                drupal_set_message("Invalid date", 'error');
            } else {
                if (Mae_users::set_premium($companyid, $status, $dt)) {
                    drupal_set_message("Account status updated");
                } else {
                    drupal_set_message("Failed to update account status", 'error');
                }
            }
        }
        return '';
    }
    $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
    return $Block;
}


function manage_connections($op = null)
{
    global $AccessObj;
    $id = sget($_REQUEST, 'id');
    if (empty($id)) return '';
    if ($AccessObj->user_type != 'super' && (!Connections::admin_of($id))) return '';

    $section = __FUNCTION__;
    $editorid = $AccessObj->uid;

    $time = time();
    $redirect = rebuild_url(array('tab', 'page', 'id', 'mtab'));
    $HV =  hidden_post_values(array('tab', 'page', 'mtab', 'id'));

    $Block['title'] = "Manage Connections";
    ///$Block['title'] .= "<div class=editbtn><a href='$redirect&section=${section}&panel=mcp' class=tool>Edit</a></div>";

    $panel = get_var('panel');
    $type = get_var('type');

    $classes = [
        'com' => '',
        'pro' => ''
    ];

    if ($panel == 'mcp') {
        if ($type == 'com' || $type == 'pro') {
            $classes[$type] = ' active';
        }
    }

    $content = "<div class='row{$classes['com']}'><a href='$redirect&section=${section}&panel=mcp&type=com'>Company Connections</a></div>
                <div class='row{$classes['pro']}'><a href='$redirect&section=${section}&panel=mcp&type=pro'>Professional Connections</a></div>
                ";


    $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
    return $Block;
}

function mcp($op = null)
{
    require_once("." . base_path() . path_to_theme() . "/includes/classes/maenna_users.inc");
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
    global $AccessObj;
    $id = sget($_REQUEST, 'id');
    if (empty($id)) return '';


    $type = sget($_REQUEST, 'type');
    $section = __FUNCTION__;
    $redirect = rebuild_url(array('tab', 'page', 'id', 'mtab'));
    $HV =  hidden_post_values(array('tab', 'page', 'mtab', 'id'));
    $Block['title'] = "Manage Connections";
    if ($op && $op != 'update') {
        if ($type == 'com') $Block['title'] .= " - Company Connections";
        else $Block['title'] .= " - Professional Connections";

        $Admin_conns    =   Connections::admin_connections($AccessObj->uid);
        $User_conns     =   Connections::Com_conns($id);
        $Pro_conns      =   Connections::Connected_pros($id);
        $Matched_pros   =   getMatchedProfessionals($id);
        //test_array($Admin_conns);
        //test_array($Pro_conns);
        //test_array($Matched_pros);

        $TR = '';
        if ($type == 'com') {
            $HEADER = "<tr style='background:#f2f2f2;'><td>Maenna ID</td><td>Connection Type</td></tr>";
            $Options = array(
                'none' => '----',
                'watchlist' => 'Visible'
            );
            foreach ($Admin_conns['Com'] as $Conn) {
                $com_id = $Conn->target_uid;
                if ($com_id == $id) continue;
                $com_maennaid = getProjectName($com_id);
                $conn_id = '';
                $relation = 'none';
                foreach ($User_conns['Watchlist'] as $User_conn) {
                    if ($User_conn->assignee_uid == $com_id) {
                        $conn_id = $User_conn->connid;
                        $relation = 'watchlist';
                        break;
                    }
                }
                $select = "<select name='REL[$com_id][conntype]'>" . option_code($Options, $relation) . " </select>";
                $TR .= "\n<tr><td>$com_maennaid <input type='hidden' name='REL[$com_id][connid]' value='$conn_id' /></td><td>$select</td></tr>";
            }
        } elseif ($type == 'pro') {
            $stab = sget($_REQUEST, 'stab');
            if ($stab == '' || $stab == 'connected') {
                $stab = 'connected';
                $conn_active = 'active';
                $HEADER = "<tr style='background:#f2f2f2;'><td>Maenna ID</td><td>Connection Type</td><td>Connection date</td></tr>";
            } else {
                $match_active = 'active';
                $HEADER = "<tr style='background:#f2f2f2;'><td>Maenna ID</td><td>Connection Type</td><td>Matched date</td><td>Unmatch</td></tr>";
            }
            $content = "<div class='searchtab'>
                <a href='$redirect&section=${section}&panel=mcp&type=pro&stab=connected' class='info_tab_btn  $conn_active'  >Connected</a>&nbsp;&nbsp;&nbsp;
                <a href='$redirect&section=${section}&panel=mcp&type=pro&stab=matched' class='info_tab_btn $match_active'  >Matched</a>&nbsp;&nbsp;&nbsp;
            </div>";

            $Options1 = array(
                'none' => '----',
                'visible' => 'Connectors',
                'partner' => 'Partner',
				'investor' => 'Investor',
                'advisor' => 'Advisor',
                'collaborator' => 'Interested',
                'propose' => 'Propose as advisor'
            );

            $Options2 = array(
                'none' => '----',
                'visible' => 'Connectors',
                'partner' => 'Partner',
                'advisor' => 'Advisor',
                'collaborator' => 'Interested',
            );
            if ($stab == 'connected' || $stab == '') {
                $i = 1;
                //die(test_array($Pro_conns));
                foreach ($Pro_conns as $Conn) {
                    //if ($Conn->conntype == 'match') $pro_id = $Conn->assignee_uid;
                    //else $pro_id = $Conn->target_uid; //
                    $pro_id = $Conn->assignee_uid;
                    $pro_maennaid = getProId($pro_id);
                    $conn_id = '';
                    $relation = 'none';
                    $Options = $Options1;
                    $access = showTime($Conn->edittime);
                    //$allProsId[] = $pro_id;
                    foreach ($User_conns as $key => $User_conn) {
                        $found = false;
                        if (in_array($key, array('Advisor', 'Partner', 'Propose', 'Visible', 'Client', 'Collaborator','Investor'))) {
                            foreach ($User_conn as $Conn1) {
                                if ($Conn1->assignee_uid == $pro_id && $Conn->conntype == $Conn1->conntype) {
                                    $conn_id = $Conn1->connid;
                                    $relation = $Conn1->conntype;
                                    if (strcasecmp($relation, 'advisor') == 0) {
                                        $Options = $Options2;
                                    }
                                    $found = true;
                                    break;
                                }
                            }
                        }
                        if ($found) break;
                    }
                    //                if (getRoleId($pro_id) == '12') {
                    $Options += array('client' => 'Colleague');
                    //                }

                    $select = "<select name='REL[$pro_id][conntype]'>" . option_code($Options, $relation) . " </select>";
                    $TR .= "\n<tr><td>" . $i++ . ". &nbsp; $pro_maennaid <input type='hidden' name='REL[$pro_id][connid]' value='$conn_id' /></td><td>$select</td><td>$access</td></tr>";
                }
                /*                $colls = getCollaborators($id);
                foreach ($colls['active'] as $pro_id) {

                    $pro_maennaid = getProId($pro_id);
                    $Options = $Options1;
                    $relation = 'collaborator';
                    $select = "<select name='REL[$pro_id][conntype]'>" .option_code($Options, $relation). " </select>";
                    $TR .= "\n<tr><td>".$i++.". &nbsp; $pro_maennaid <input type='hidden' name='REL[$pro_id][connid]' value='$conn_id' /></td><td>$select</td><td></td></tr>";
                }*/
            } else {
                $i = 1;
                foreach ($Matched_pros as $Conn) {
                    $pro_id = $Conn->assignee_uid; //
                    $pro_maennaid = getProId($pro_id);
                    $conn_id = $Conn->connid;
                    $relation = 'none';
                    $Options = $Options1;
                    $access = showTime($Conn->edittime);

                    foreach ($User_conns as $key => $User_conn) {
                        $found = false;
                        if (in_array($key, array('Advisor', 'Propose', 'Visible', 'Client', 'Collaborator','Investor'))) {
                            foreach ($User_conn as $Conn1) {
                                if ($Conn1->assignee_uid == $pro_id && $Conn->conntype == $Conn1->conntype) {
                                    $conn_id = $Conn1->connid;
                                    $relation = $Conn1->conntype;
                                    if (strcasecmp($relation, 'advisor') == 0) {
                                        $Options = $Options2;
                                    }
                                    $found = true;
                                    break;
                                }
                            }
                        }
                        if ($found) break;
                    }
                    //                if (getRoleId($pro_id) == '12'){
                    $Options += array('client' => 'Colleague');
                    //                }

                    $select = "<select name='REL[$pro_id][conntype]'>" . option_code($Options, $relation) . " </select>";
                    $TR .= "\n<tr><td>" . $i++ . ". &nbsp; $pro_maennaid <input type='hidden' name='REL[$pro_id][connid]' value='$conn_id' /></td><td>$select</td><td>$access</td><td data-cid='$conn_id' data-hash='" . md5($conn_id . "kyarata75") . "' class='unmatch_pro'>Unmatch</td></tr>";
                }
            }
        }

        $content .= <<< END
        <form method=post action='/account'>
            <table cellspacing=0 cellpadding=0 class='table-list'>
                $HEADER
                $TR
            </table>
            <input type=submit name=submit value=submit class=button /> <a href='$redirect' class=button>cancel</a>
            $HV
            <input type=hidden name=type value='$type' />
            <input type=hidden name=section value='manage_connections' />
            <input type=hidden name=panel value='$section' />
            <input type=hidden name=update_section value='$section' />
            <input type='hidden' name='stab' value='$stab'/>
        </form>
       <script type="text/javascript">
       $(document).ready(function(){
           $(".unmatch_pro").click(function(ev) {

               if (confirm("Are You sure to unmatch professional from company?")) {
                ev.preventDefault();
                thisObj = $(this);

                $.post("/themes/maennaco/includes/pro_posts.php?type=unmatch_pro", {
                    m: thisObj.data('hash'),
                    cid: thisObj.data('cid')
                },function(response){
                    if (response == "true") {

                        thisObj.parent().hide("slow");

                    }

                    else alert("Authentication problem!");

		});
                }

	});

        });
      </script>
END;
    } elseif ($op == 'update') {
        $REL = $_POST['REL'];
        if ($type == 'com') {
            foreach ($REL as $com_id => $Rel) {
                $connid = sget($Rel, 'connid');
                $conntype = sget($Rel, 'conntype');
                if (!Connections::set_connection($id, $com_id, $connid, $conntype)) {
                    drupal_set_message("Failed to set connections", 'error');
                    return;
                }
            }
            drupal_set_message("Operation Successful");
            drupal_goto('account', array(
                'tab' => 'companies',
                'page' => 'company_detail',
                'id' => $id,
                'mtab' => 'about',
                'section' => 'manage_connections',
                'panel' => 'mcp',
                'type' => 'com',
            ));
            return;
        } elseif ($type == 'pro') {
            $stab = $_POST['stab'];
            foreach ($REL as $pro_id => $Rel) {
                $connid = sget($Rel, 'connid');
                $conntype = sget($Rel, 'conntype');
                if ($stab == 'matched' && $conntype == 'none') continue;
                if ($stab == 'connected' && $conntype == 'none') {
                    Connections::change_conntype($connid, 'match');
                    continue;
                }
                //                Connections::deactivate_conn($connid);
                if (!Connections::set_connection($id, $pro_id, $connid, $conntype)) {
                    //test_array(array($target_uid, $id, $connid, $conntype));
                    drupal_set_message("Failed to set connections", 'error');
                    return;
                }
            }
            drupal_set_message("Operation Successful");
            drupal_goto('account', array(
                'tab' => 'companies',
                'page' => 'company_detail',
                'id' => $id,
                'mtab' => 'about',
                'section' => 'manage_connections',
                'panel' => 'mcp',
                'type' => 'pro',
            ));
            die;
            return;
        }
    }

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function mcp_old($op = null)
{
    die();
    require_once("." . base_path() . path_to_theme() . "/includes/classes/maenna_users.inc");
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
    global $AccessObj;
    $id = sget($_REQUEST, 'id');
    if (empty($id)) return '';
    if ($AccessObj->user_type != 'super' && (!Connections::admin_of($id))) return '';

    $type = sget($_REQUEST, 'type');
    $section = __FUNCTION__;
    $redirect = rebuild_url(array('tab', 'page', 'id', 'mtab'));
    $HV =  hidden_post_values(array('tab', 'page', 'mtab', 'id'));
    $Block['title'] = "Manage Connections";

    if ($op && $op != 'update') {
        if ($type == 'com') $Block['title'] .= " - Company Connections";
        else $Block['title'] .= " - Professional Connections";

        $Admin_conns    =   Connections::admin_connections($AccessObj->uid);
        $User_conns     =   Connections::Com_conns($id);

        $TR = '';
        if ($type == 'com') {

            $Options = array(
                'none' => '----',
                'watchlist' => 'Visible'
            );
            foreach ($Admin_conns['Com'] as $Conn) {
                $com_id = $Conn->target_uid;
                if ($com_id == $id) continue;
                $com_maennaid = getProjectName($com_id);
                $conn_id = '';
                $relation = 'none';
                foreach ($User_conns['Watchlist'] as $User_conn) {
                    if ($User_conn->assignee_uid == $com_id) {
                        $conn_id = $User_conn->connid;
                        $relation = 'watchlist';
                        break;
                    }
                }
                $select = "<select name='REL[$com_id][conntype]'>" . option_code($Options, $relation) . " </select>";
                $TR .= "\n<tr><td>$com_maennaid <input type='hidden' name='REL[$com_id][connid]' value='$conn_id' /></td><td>$select</td></tr>";
            }
        } elseif ($type == 'pro') {
            $Options1 = array(
                'none' => '----',
                'visible' => 'Visible',
                'advisor' => 'Advisor',
                'propose' => 'Propose as advisor'
            );
            $Options2 = array(
                'none' => '----',
                'visible' => 'Visible',
                'advisor' => 'Advisor',
            );
            foreach ($Admin_conns['Pro'] as $Conn) {
                $pro_id = $Conn->target_uid; //
                $pro_maennaid = getProId($pro_id);
                $conn_id = '';
                $relation = 'none';
                $Options = $Options1;
                foreach ($User_conns as $key => $User_conn) {
                    $found = false;
                    if (in_array($key, array('Advisor', 'Propose', 'Visible'))) {
                        foreach ($User_conn as $Conn) {
                            if ($Conn->assignee_uid == $pro_id) {
                                $conn_id = $Conn->connid;
                                $relation = $Conn->conntype;
                                if (strcasecmp($relation, 'advisor') == 0) {
                                    $Options = $Options2;
                                }
                                $found = true;
                                break;
                            }
                        }
                    }
                    if ($found) break;
                }
                $select = "<select name='REL[$pro_id][conntype]'>" . option_code($Options, $relation) . " </select>";
                $TR .= "\n<tr><td>$pro_maennaid <input type='hidden' name='REL[$pro_id][connid]' value='$conn_id' /></td><td>$select</td></tr>";
            }
        }
        $content = <<< END
        <form method=post action='/account'>
            <table cellspacing=0 cellpadding=0 class='table-list'>
                <tr style='background:#f2f2f2;'><td>Maenna ID</td><td>Connection Type</td></tr>
                $TR
            </table>
            <input type=submit name=submit value=submit class=button /> <a href='$redirect' class=button>cancel</a>
            $HV
            <input type=hidden name=type value='$type' />
            <input type=hidden name=section value='manage_connections' />
            <input type=hidden name=panel value='$section' />
            <input type=hidden name=update_section value='$section' />
        </form>
END;
    } elseif ($op == 'update') {
        $REL = $_POST['REL'];
        //test_array($REL);
        if ($type == 'com') {
            foreach ($REL as $com_id => $Rel) {
                $connid = sget($Rel, 'connid');
                $conntype = sget($Rel, 'conntype');
                if (!Connections::set_connection($id, $com_id, $connid, $conntype)) {
                    //test_array($Rel);
                    //test_array(array($target_uid, $id, $connid, $conntype));
                    drupal_set_message("Failed to set connections", 'error');
                    return;
                }
            }
            drupal_set_message("Operation Successful");
            return;
        } elseif ($type == 'pro') {
            foreach ($REL as $pro_id => $Rel) {
                $connid = sget($Rel, 'connid');
                $conntype = sget($Rel, 'conntype');
                if (!Connections::set_connection($id, $pro_id, $connid, $conntype)) {
                    //test_array($Rel);
                    //test_array(array($target_uid, $id, $connid, $conntype));
                    drupal_set_message("Failed to set connections", 'error');
                    return;
                }
            }
            drupal_set_message("Operation Successful");
            return;
        }
    }

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function com_questionnaire($op = null)
{
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
    global $user, $AccessObj;
    $editorid = $user->uid;
    $id = sget($_REQUEST, 'id');
    $time = time();
    $Block['title'] = "Edit Questionnaire";


    global $user, $AccessObj;

    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($id))) {
        //continue;
    } else {
        return '';
    }
    if (empty($op)) return '';

    $content = '';

    $sql = "select recordid from maenna_questionair where target='company' and status = 1 and type = 'version'";
    $result = db_query($sql);
    $Row = db_fetch_object($result);
    if ($Row) {
        $parentid = $Row->recordid;
    } else return '';

    $Block['title'] .= "<div class=editbtn><a href='/account?tab=information&page=qmgmt&panel=answerdetail&target=company&id=$parentid&uid=$id' class=tool>View</a></div>";

    $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
    return $Block;
}

function q_files($op)
{
    global $user, $AccessObj;
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");

    $companyid = sget($_REQUEST, 'id');

    $editorid = $user->uid;
    $time = time();
    $Block['title'] = "Files";

    $section = __FUNCTION__;

    $content = '';
    $redirect = rebuild_url(array('tab', 'page', 'mtab', 'id'));
    $parentid = '';




    $id = sget($_REQUEST, 'id');
    if ($AccessObj->user_type == 'admin') {
        if (!Connections::admin_of($uid)) return '';
    }

    if ($op && $op != 'update') {

        $sql = "select * from maenna_questionair where status = 1 and target='company' and parentid is null order by recordid desc";
        $result = db_query($sql);
        $Row = db_fetch_object($result);
        if ($Row) {
            $parentid = $Row->recordid;

            // list files
            $sql = "select Q.*, A.answer,A.qaid from maenna_questionair as Q
                        left join maenna_qa as A on
                            Q.recordid = A.qid and
                            A.uid = %d
                        where Q.parentid = %d and type='fileuploader' order by weight, recordid";
            $result = db_query($sql, array($id, $parentid));
            $counter = 1;
            while (($Row = db_fetch_object($result))) {
                $questionid = $Row->recordid;
                $qaid = $Row->qaid;
                $file = $Row->answer;
                if ($file) {
                    $path =  "./" . file_directory_path() . "/" . $file;
                    $file = preg_replace("/^[0-9]*_/i", '', $file);
                    $filetitle = ucwords($file);
                    $filename = $file;
                    if (strlen($filetitle) > 35) $filetitle = substr($file, 0, 35) . "...";
                    $file = "<a href='/account?tab=companies&page=company_detail&id=$companyid&mtab=file&file=" . urlencode($path) . "&name=" . urlencode($filename) . "' target='_blank' class='blue'>$filename</a>";
                } else {
                    continue;
                }
                $content .= "\n<div class=row style='clear:both'>
                                    <div style='width:240px;overflow:hidden;white-space:nowrap;' title='$filename'>$counter. $file</div>

                            </div>";
                $counter++;
            }
            $sql = "select * from maenna_qa where qid = %d and uid = %d order by qaid ";
            $result = db_query($sql, array($parentid, $id));
            while ($Row = db_fetch_object($result)) {
                $qaid = $Row->qaid;
                $file = $Row->answer;
                if ($file) {
                    $path =  "./" . file_directory_path() . "/" . $file;
                    $file = preg_replace("/^[0-9]*_/i", '', $file);

                    $filename = htmlentities(ucwords($Row->filename), ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    if (strlen($filetitle) > 25) $filetitle = substr($file, 0, 25) . "...";
                    $file = "<a href='/account?tab=companies&page=company_detail&id=$companyid&mtab=file&file=" . urlencode($path) . "&name=" . urlencode($filename) . "' target='_blank' class='blue'>$filename</a>";
                } else {
                    continue;
                }
                $content .= "\n<div class=row>
                                   <div style='width:250px;overflow:hidden;' title='$filename'>$counter.  $file</div>
                            </div>";
                $counter++;
            }
        }
    }


    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function cmp_insights_stats($op = null)
{
    global $user, $AccessObj;
    $pid = sget($_REQUEST, 'id');
    if ($AccessObj->user_type == 'super' || ($AccessObj->user_type == 'admin' && Connections::admin_of($pid)) || $AccessObj->uid == $_REQUEST['id']) {
        $Block['title'] = 'Insights Attending Summary';
        $editorid = $user->uid;
        $db = \Clewed\Db::get_instance();

        $payments = $db->get_array(
            "SELECT (SELECT SUM(amount) FROM maenna_professional_payments WHERE user_id = :user_id and status = 1) as total, mpp.*,
IF (mp.username_type = 1,mp.firstname,CONCAT(mp.firstname,' ', mp.lastname)) as display_name,
mp.gender,mps.postedby
FROM maenna_professional_payments mpp
  LEFT JOIN maenna_professional mps ON mpp.pro_id = mps.id
  LEFT JOIN maenna_people mp ON mps.postedby = mp.pid
WHERE user_id = :user_id and status = 1 LIMIT 5",
            array(':user_id' => $pid)
        );
        if (count($payments) == 0) $content = 'No Insights attended yet.';
        else {
            $content = "<table class='payment_stats' style='margin:0 !important;'>";
            foreach ($payments as $payment) {
                $timestamp = date('n/j/y', $payment['date_created']);

                if (file_exists('sites/default/images/profiles/150x150/' . $payment['postedby'] . '.jpg')) {
                    $avatar = 'sites/default/images/profiles/150x150/' . $payment['postedby'] . '.jpg';
                } else {
                    if ($payment['gender'] == 'm' || $payment['gender'] == '') {
                        $avatar = ' /themes/maennaco/images/prof-avatar-male.png';
                    } else
                        $avatar = '/themes/maennaco/images/prof-avatar-female.png';
                }

                /*if ($payment['user_type'] == 'people') $display_name = "<a href='#' id='pro_id" . $payment['user_id'] . "' ref='pro_id' style='color:#00a2bf;' class='profile_details'>" . ucfirst($payment['display_name']) . "</a>";
 else $display_name = ucfirst($payment['display_name']);*/
                /*$display_name = "<span style='color:#00a2bf;'>".ucfirst($payment['display_name'])."</span>";*/
                $content .= "<tr style='height:70px;border-bottom:solid 1px #d0d2d3;'><td data-tooltip='" . $payment['display_name'] . "' style='vertical-align:middle;'><img src='" . $avatar . "' width='40px' height='40px'></td><td style='vertical-align:middle;padding-left:10px;'>" . $timestamp . "</td><td style='vertical-align:middle;text-align:right;padding-right:20px;'>$" . number_format($payment['amount'], 1) . "</td></tr>";
            }
            $content .= "<tr style='height:70px;border-bottom:solid 1px #d0d2d3;'><td style='vertical-align:middle;'>Total:</td><td></td><td style='vertical-align:middle;text-align:right;padding-right:20px;'>$" . number_format((float)$payment['total'], 1) . "</td></tr></table><div style='background-color:#FAFBFB'><a href='#' style='float:right;border-left: solid 1px #006274;height: 15px;padding-left: 3px;margin-top: 10px;margin-right:9px;' data-ins_id = '" . $payment['user_id'] . "' data-hash='" . md5('kyarata75:' . $user->uid . ':' . $AccessObj->user_type . ':' . $payment['user_id']) . "' data-utype='user' data-uid='" . $user->uid . "' data-user-type='" . $AccessObj->user_type . "' class='tool payment_details'>More</a></div>";
            $content .= js_init('init_payment_details();init_payment_details_dialog();');
            $content .= "<div id='payment_details_dialog' style='display:none;'></div>";
        }
        $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
        return $Block;
    } else {
        return '';
    }
}

function fundraising_administration($op = null)
{
    global $AccessObj;
    $id = sget($_REQUEST, 'id');
    if (empty($id)) {
        return '';
    }

    if ($AccessObj->user_type != 'super' && (!Connections::admin_of($id))) {
        return '';
    }

    $section = __FUNCTION__;
    $editorid = $AccessObj->uid;

    $time = time();
    $redirect = rebuild_url(array('tab', 'page', 'id', 'mtab'));
    $HV =  hidden_post_values(array('tab', 'page', 'mtab', 'id'));

    $panel = get_var('panel');
    $type = get_var('type');

    $classes = [
        'company_info' => '',
        'committed_capital' => '',
        'turn_invest_now' => ''
    ];

    if ($panel == 'fap') {
        if (isset($classes[$type])) {
            $classes[$type] = ' active';
        }
    }

    $Block['title'] = "Fundraising Administration";
    ///$Block['title'] .= "<div class=editbtn><a href='$redirect&section=${section}&panel=mcp' class=tool>Edit</a></div>";

    $content = "<div class='row${classes['company_info']}'><a href='$redirect&section=${section}&panel=fap&type=company_info'>Company Information</a></div>
                <div class='row${classes['committed_capital']}'><a href='$redirect&section=${section}&panel=fap&type=committed_capital'>Committed Capital</a></div>
                <div class='row${classes['turn_invest_now']}'><a href='$redirect&section=${section}&panel=fap&type=turn_invest_now'>Turn on \"Invest Now\" button</a></div>";

    $Block['body'] = sidebar_box($Block['title'], $content, 'css_admin_box');
    return $Block;
}

function fap($op = null)
{
    require_once("." . base_path() . path_to_theme() . "/includes/classes/maenna_users.inc");
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
    global $AccessObj;
    $id = $companyId = sget($_REQUEST, 'id');
    $get_companyinfo_sql = "SELECT * FROM maenna_company WHERE companyid = '".$id."'";
    $company_res = mysql_query($get_companyinfo_sql);
    $company_data = mysql_fetch_object($company_res);
    $company_mail = $company_data->email;
    if (empty($id)) return '';


    $type = sget($_REQUEST, 'type');
    $panel = __FUNCTION__;
    $redirect = rebuild_url(array('tab', 'page', 'id', 'mtab'));
    $HV =  hidden_post_values(array('tab', 'page', 'mtab', 'id'));
    if ($op && $op != 'update') {
        switch ($type) {
            case "company_info": {
                    $Block['title'] = "Company Fundraising Approval Checklist";
                    $content = "";
                    $content .= "<font face='Lato Regular'>" . drupal_get_form('maenna_forms_company_fap_checklist_form') . "</font>";
                    $content .= '<script src="https://cdn.plaid.com/link/v2/stable/link-initialize.js"></script>';
                    $content .= "<script>";
                    $content .= "$(document).ready(function(){
                        (async function() {
                            async function fetchLinkToken() {
                                let response = await fetch('/payment_create_link_token.php',{
                                    method: 'POST',
                                    headers: {
                                        'Content-Type': 'application/json',
                                    },
                                    body: JSON.stringify({'customerinfo':'".$company_mail."'}),
                                });
                                let responseJSON  = await response.json();
                                return responseJSON.link_token;
                            }
                    
                            const configs = {
                                token: await fetchLinkToken(),
                                env: 'sandbox',
                                onSuccess: async function(public_token, metadata){
                                    console.log(public_token);
                                    account_id = metadata.accounts[0].id
                                    user_email = '$company_mail'
                                    companyid = '$companyId'
                                    description = 'Clewed Company Client';
                                    $.post( '/payment_process_plaid_token.php', {public_token,account_id,companyid,user_email,description}, function( data ) {                        
                                        if (data != null || data != ''){
                                            alert('Successed!');
                                        }else{
                                            alert('Failed!');
                                        }
                                    });
                                },
                                onExit: async function(err, metadata) {
                                    if (err != null) {
                                        console.log(err);
                                        console.log(metadata);   
                                    }
                                }
                            }
                    
                            var linkHandler = Plaid.create(configs);        
                            $('#edit-banklink-approved').click(function(){
                                var flag = $('#edit-banklink-approved').attr('checked');
                                if(flag == 'checked'){
                                    linkHandler.open();
                                }
                            });
                        })();
                    });";
                    $content .= "</script>";
                    break;
                }
            case "committed_capital": {
                    if (isset($_REQUEST['sortby'])) $sortby = $_REQUEST['sortby'];
                    $Block['title'] = "COMMITTED CAPITAL";
                    $_Tags = ADMIN_INVEST_SORT();
                    $tag_no = count($_Tags);
                    $investor = $_REQUEST['investor'];
                    $status = $_REQUEST['status'];
                    $i = 0;

                    $sortLinks = '';
                    foreach ($_Tags as $key => $value) {
                        if ($sortby == $key) $font_style = 'color:#00a2bf;';
                        else $font_style = 'font-style:italic;';
                        $sortLinks .= "<a href='${redirect}&section=fundraising_administration&panel=$panel&type=committed_capital&sortby=$key' style='$font_style font-size:12px;cursor:pointer;";

                        if ($i == 0)
                            $sortLinks .= "margin-right:10px;'>{$value}</a>";
                        else
                            $sortLinks .= "margin-left:10px;margin-right:10px;'>{$value}</a>";

                        if (++$i != $tag_no)
                            $sortLinks .= '|';
                    }
                    if (!empty($sortLinks))
                        $sortLinks = rtrim($sortLinks, '|');

                    //          $status = array('<span class="color-swatch" data-tooltip="Pending" style="background-color:#FFE940; border:1px solid #FFE940; padding-right: 10px;"></span>',
                    //              '<span class="color-swatch" data-tooltip="Accepted" style="background-color:#00A2BF; border:1px solid #00A2BF; padding-right: 10px;"></span>',
                    //              '<span class="color-swatch" data-tooltip="Declined" style="background-color:#DF4000; border:1px solid #DF4000; padding-right: 10px;"></span>');

                    //          $Block['title'] .= '<a class= "openbox" boxid="box1" data-tooltip="' . $tooltip . '" style="color:#fff;cursor:pointer;display:inline-block;margin-left:120px; margin-top:5px;height:20px;line-height:22px;border-left:thin solid #fff;padding-left:10px;font-family:LatoRegular; font-size:13px; text-transform: none;" onclick="add_invest()">Add</a>';

                    //        $Block['title'] .= '<a class= "openbox" boxid="box2" data-tooltip="' . $tooltip . '" style="color:#fff;cursor:pointer;display:inline-block;margin-left:10px; margin-top:5px;height:20px;line-height:22px;border-left:thin solid #fff;padding-left:10px;font-family:LatoRegular; font-size:13px; text-transform: none;">Edit</a>';

                    //        $Block['title'] .= '<span class="color-swatch" data-tooltip="Accepted" style="background-color:#00A2BF; border:1px solid #00A2BF; margin-left: 20px; padding-left: 4px; padding-right: 13px;"></span>';
                    //        $Block['title'] .= '<span class="color-swatch" data-tooltip="Pending" style="background-color:#FFE940; border:1px solid #FFE940; margin-left: 10px; padding-left: 4px; padding-right: 13px;"></span>';
                    //        $Block['title'] .= '<span class="color-swatch" data-tooltip="Declined" style="background-color:#DF4000; border:1px solid #DF4000; margin-left: 10px; padding-left: 4px; padding-right: 13px;"></span>';

                    $Block['title'] .= "<span style='margin-top:5px;height:20px;line-height:22px;border-left:thin solid #fff;padding-left:10px; float:right;display:inline-block;font-weight:bold; font-family:LatoRegular; font-size:13px; text-transform:none;'>Sort By <img class='openSortPanel' panelid='file_sort' style='width:20px; margin-left:20px;' src='themes/maennaco/images/arrow_down.png'></span>";

                    $content = '<input type="hidden" id="redirect" value=' . ${redirect} . '>';
                    //Set this page styles
                    $content .= "<style>
                        .shaded_title {

                            background:#ffffff;
							font-size: 16px;
                            height:30px;
                            line-height:30px;
                        }
                        .main_content {
                            padding-top:0 !important;
                        }
            </style>";
                    $content .= '<div id="file_sort" style="display:none;line-height:30px;height:75px;width:100%; background-color:#f2f2f2; padding-bottom: 20px;"><span style="margin-top:0; display:inline-block;vertical-align:middle;margin-left:15px;">';
                    $content .= $sortLinks . '</span><select id="investor_type" style="width:100px;height:28px;margin-top:10px; margin-bottom:10px; margin-left: 80px;" name="investor_type"><option value="">Investor Type:</option>';
                    foreach (INVESTOR_TYPE() as $key => $value) {
                        if ($investor != '' && $key == $investor)
                            $content .= "<option value=$key selected>$value</option>";
                        else
                            $content .= "<option value=$key>$value</option>";
                    }
                    $content .= "</select><select id='status_type' style='width:100px;height:28px;margin-top:10px; margin-bottom:10px; margin-left: 20px;' name='status_type'><option value=''>Status:</option>";
                    foreach (INVEST_STATUS() as $key => $value) {
                        if ($status != '' && $key == $status)
                            $content .= "<option value=$key selected>$value</option>";
                        else
                            $content .= "<option value=$key>$value</option>";
                    }
                    $content .= "</select><input type='submit' class='button' value=Filter id='filter_btn' style='margin-left: 15px; margin-top: 10px;'/></div>";

                    $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
                    $content .= "<div id='box1' style='margin-bottom:15px;display:none;'>
                            <form id='addInvest_admin' method=post action='/account' enctype='multipart/form-data' onsumbit=''>
                                <div><label>Type:</label></div><select id='investor_type_admin' style='width:207px;height:28px;margin-top:10px; margin-bottom:10px;' name='investor_type'><option value=''>Investor Type:</option>";
                    foreach (INVESTOR_TYPE() as $key => $value)
                        $content .= "<option value='" . $key . "'>" . $value . "</option>";
                    $content .= "</select>";
                    $content .= "<br> <div><label>Funded$:</label></div><input style='width:197px!important;height:22px;margin-bottom:10px;' id='amount' type=text name='amount' placeholder='Funded$' />";
                    $content .= "<br> <div><label>Reference#:</label></div><input style='width:197px!important;height:22px;margin-bottom:10px;' id='reference' type=text name='reference' placeholder='Reference#' />";
                    $content .= "<br> <div><label>Date:</label></div><input style='width:197px!important;height:22px;margin-bottom:10px;' id='refdate' type=date name='refdate' placeholder='Date' />";
                    $content .= "<div id='agree'><br><input style='margin-left:10px; margin-top:7px;' type='checkbox' id='cmp-agree'><label style='margin-bottom: 10px;' for='cmp-agree'>I agree with <a class='show_confidentiality' type='terms' target='_blank' style='font-family:Lato Regular;font-style: italic;color:#00a1be;'>confidentiality</a> to access fundraising information </label></div>";
                    $content .= "<input type='hidden' name='pay_status' id='pay_status'>";
                    $content .= "<br><input type=submit name=submit value='Submit' class='button' /> &nbsp;
                    <a href='#' class='hidebox-1 button' boxid=box1>Close</a>
            </div>
            $HPV
            <input type='hidden' name=section value='prof_invest' />
            <input type='hidden' name=update_section value='prof_invest' />
            <input type='hidden' id='do' name=do value='new' />
            <input type='hidden' id='dataid' name=dataid value='-1' />
            <input type='hidden' id='adminOrProf' name=user_type value='prof' />
            </form>";

                    if ($investor == '' && $status == '') {
                        if ($sortby == '')
                            $sql = "select a.*, p.firstname name from maenna_professional_investment a join maenna_people p on a.prof_id=p.pid where a.deleted!=1 and company_id = " . $companyId . " order by a.id asc";
                        else
                            $sql = "select a.*, p.firstname name  from maenna_professional_investment a join maenna_people p on a.prof_id=p.pid where a.deleted!=1 and company_id = " . $companyId . " order by " . $sortby . " desc";
                        $result = db_query($sql);
                    } else {
                        $sql = "select a.*, p.firstname name from maenna_professional_investment a join maenna_people p on a.prof_id=p.pid where ";
                        if ($investor != '')
                            $sql .= "a.type='" . $investor . "' and ";
                        if ($status != '')
                            $sql .= "a.status=" . $status . " and ";
                        $sql .= "a.deleted!=1 and company_id = " . $companyId . " order by a.id asc";
                        $result = db_query($sql);
                    }
                    while ($Row = db_fetch_object($result)) {
                        $id = $Row->id;
                        $tmpRow = '';
                        $tmpRow .= "\n<tr style='height: 32px;'>
                <td style='padding-left:15px;padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:left;height:32px;'><span><a style='color:black;' href='/account?tab=investments&id=$Row->prof_id'><u>$Row->name</u></a></span></div></td>
                <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:left;height:32px;'>" . strtoupper($Row->type) . "</div></td>
                <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:right;height:32px;margin-right: 10px;'>$" . number_format($Row->ioi) . "</div></td>
                <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:right;height:32px;margin-right:10px;'>$" . number_format($Row->amount) . "</div></td>
                <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:left;height:32px;'>" . date("d/m/y", $Row->created) . "</div></td>
                <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:left;height:32px;'><select class='invest_status' style='width:100px;' name='invest_$Row->id' invest-id='$Row->id'>";
                        foreach (INVEST_STATUS() as $key => $value)
                            if ($key == $Row->status)
                                $tmpRow .= "<option value='" . $key . "' selected>" . $value . "</option>";
                            else
                                $tmpRow .= "<option value='" . $key . "'>" . $value . "</option>";
                        $tmpRow .= "</select></div></td>";
                          $amount_style = '$' . number_format($Row->amount);
                        // $amount_style = '$' . number_format($Row->ioi);
                        //   $tmpRow .= "<td style='padding-top:5px;font-size:11px;'>
                        //         <a href='#' onclick='edit_invest($Row->id, \"$Row->type\", \"$amount_style\")' style='display: inline-block;border-left: 1px solid;line-height: 1.1em;padding-left: 5px;' class='tool'>Edit</a>
                        //         <a href='.$redirect&update_section=prof_invest&user_type=admin&do=del&dataid=" . $id . "' onclick='return confirm(\"Continue to delete this file?\")' style='margin-left: 5px;display: inline-block;border-left: 1px solid;line-height: 1.1em;padding-left: 5px;' class='tool'>Del</a></td>";
                        $date_style =  date("m/d/yy", $Row->created);
                        $tmpRow .= "<td style='padding-top:5px;font-size:11px;'>
                                    <a href='#' onclick='edit_invest($Row->id, \"$Row->type\", \"$amount_style\", \"$Row->reference\", \"$date_style\", \"$Row->status\")' style='display: inline-block;border-left: 1px solid;line-height: 1.1em;padding-left: 5px;' class='tool'>Edit</a></td>";
                        if(!$Row->amount) $check = null;
                        else $check = 'checked';
                        $tmpRow .= "<td style='padding-top:5px;font-size:11px;'><input type='checkbox' ".$check."></td>";
                        $tmpRow .= "</tr>";
                        $fileList[] = $tmpRow;
                    }
                    if ($investor == '' && $status == '') {
                        $sql = "select sum(amount) as total_amount_funded,  sum(ioi) as total_amount_ioi from maenna_professional_investment where deleted!=1 and company_id = " . $companyId;
                    } else {
                        $sql = "select sum(amount) as total_amount_funded,  sum(ioi) as total_amount_ioi from maenna_professional_investment where ";
                        if ($investor != '')
                            $sql .= "type='" . $investor . "' and ";
                        if ($status != '')
                            $sql .= "status=" . $status . " and ";
                        $sql .= "deleted!=1 and company_id = " . $companyId;
                    }
                    $result = db_query($sql);
                    $Row = db_fetch_object($result);
                    $get_decline_total_sql = "SELECT SUM(amount) AS total_decline FROM maenna_professional_investment WHERE company_id = $companyId AND STATUS = 2";
                    $decline_total_res = db_query($get_decline_total_sql);
                    $Decline_Row = db_fetch_object($decline_total_res);
                    $tmpRow = "\n<tr style='height: 32px;'>
                        <td style='padding-left:15px;padding-top:5px;font-size:11px;'>Total Declined</td>
                        <td style='padding-left:15px;padding-top:5px;font-size:11px;'></td>
                        <td style='padding-left:15px;padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:right;margin-right:10px;height:32px;font-weight:bold;'>$" . number_format($Decline_Row->total_decline) . "</div></td></tr>";
                    $get_pending_total_sql = "SELECT SUM(amount) AS total_pending FROM maenna_professional_investment WHERE company_id = $companyId AND STATUS = 0";
                    $pending_total_res = db_query($get_pending_total_sql);
                    $Pending_Row = db_fetch_object($pending_total_res);
                    $tmpRow .= "\n<tr style='height: 32px;'>
                        <td style='padding-left:15px;padding-top:5px;font-size:11px;'>Total Pending</td>
                        <td style='padding-left:15px;padding-top:5px;font-size:11px;'></td>
                        <td style='padding-left:15px;padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:right;margin-right:10px;height:32px;font-weight:bold;'>$" . number_format($Pending_Row->total_pending) . "</div></td></tr>";
                    $get_approve_total_sql = "SELECT SUM(amount) AS total_approve FROM maenna_professional_investment WHERE company_id = $companyId AND STATUS = 1";
                    $approve_total_res = db_query($get_approve_total_sql);
                    $Approve_Row = db_fetch_object($approve_total_res);
                    $tmpRow .= "\n<tr style='height: 32px;'>
                        <td style='padding-left:15px;padding-top:5px;font-size:11px;' colspan='2'>Total LOI Approved</td>
                        <td style='padding-left:15px;padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:right;margin-right:10px;height:32px;font-weight:bold;'>$" . number_format($Approve_Row->total_approve) . "</div></td></tr>";
                    $get_funded_total_sql = "SELECT SUM(amount) AS total_funded FROM maenna_professional_investment WHERE company_id = $companyId AND STATUS = 3";
                    $funded_total_res = db_query($get_funded_total_sql);
                    $Aunded_Row = db_fetch_object($funded_total_res);
                    $tmpRow .= "\n<tr style='height: 32px;'>
                        <td style='padding-left:15px;padding-top:5px;font-size:11px;' colspan='3'>Total Funded</td>
                        <td style='padding-left:15px;padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:right;margin-right:10px;height:32px;font-weight:bold;'>$" . number_format($Aunded_Row->total_funded) . "</div></td></tr>";
                    $total_lol_amount = $Decline_Row->total_decline + $Pending_Row->total_pending + $Approve_Row->total_approve;
                    $tmpRow .= "\n<tr style='height: 32px;'>
            <td style='padding-left:15px;padding-top:5px;font-size:11px;'  colspan='2'><div style='overflow:hidden;text-align:left;height:32px;'>Grand Total</div></td>
            <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:right;margin-right:10px;height:32px;font-weight:bold;'>$" . number_format($total_lol_amount) . "</div></td>
            <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:right;margin-right:10px;height:32px;font-weight:bold;'>$" . number_format($Aunded_Row->total_funded) . "</div></td>
            <td style='padding-top:5px;font-size:11px;'></td>";
                    $tmpRow .= "<td style='padding-top:5px;font-size:11px;'></td>";
                    $tmpRow .= "</tr>";
                    $fileList[] = $tmpRow;

                    $content .= '<table style="margin:0; cellspacing="0" cellpadding="0" border="0" width="100%" class="left-align">
                    <thead style="background:#F9FAFC;">
                        <tr>
                            <td style="font-weight:normal;width:20%; padding-left:15px;">NAME</td>
                            <td style="font-weight:normal;width:18%">TYPE</td>
                            <td style="font-weight:normal;width:12%">IOI$</td>
                            <td style="font-weight:normal;width:12%">FUNDED$</td>
                            <td style="font-weight:normal;width:12%">DATE</td>
                            <td style="font-weight:normal;width:25%">STATUS</td>
                            <td style="font-weight:normal;width:*">ACTION</td>

            </tr></thead>';
                    foreach ($fileList as $key => $value)
                        $content .= $value;
                    $content .= "</table>
            <script type='text/javascript' src='/themes/maennaco/jui/comments/js/jquery.livequery.js'></script>
            <script type='text/javascript'>
                function add_invest() {
                    $('#investor_type').val('');
                    $('#amount').val('');
                    $('#do').val('new');
                    $('#dataid').val('-1');
                    
                    $('#agree').attr('style', 'display: inline');
                }
                function edit_invest(id, type, amount, reference , refdate, status) {
                    $('#investor_type_admin').val(type);
                    $('#amount').val(amount);
                    $('#reference').val(reference);
                    $('#refdate').val(refdate);
                    $('#pay_status').val(status);
                    $('#do').val('edit');
                    $('#dataid').val(id);
                    $('#adminOrProf').val('admin');
                    
                    $('#box1').attr('style', 'display: inline');
                    $('#agree').attr('style', 'display: none');
                }
                $(document).ready(function(){
                    var investor = $('#investor_type').val();
                    var status = $('#status_type').val();
                    if (investor != '' || status != '')
                        $('#file_sort').attr('style', 'display: inline');
                    $('.account-section-tabs').attr('style', 'display: none');
                    $('#squeeze').attr('style', 'margin-top: -60px');
                    $('.left-td-wrapper').attr('style', 'margin-top: 0px');
    
                    init_openbox();
                    init_hidebox();
                    init_sortPanel();
                    
                    var old_select_value = '';
                
                    $('form#addInvest_admin').submit(function(event) {
    
                        if ($('#investor_type_admin').val() == '') {
    
                           $('#investor_type').addClass('inerror');
                           event.preventDefault();
    
                        }
    
                        else $('#investor_type').removeClass('inerror');
                        
                        
                        if ($('#amount').val() == '') {
    
                           $('#amount').addClass('inerror');
                           event.preventDefault();
                           return;
    
                        }
    
                        else $('#amount').removeClass('inerror');
     
                        if ($('#do').val() == 'new') {
                            if ($('#cmp-agree').is(':checked') == false) {
                                alert('You have to read confidentiality!');
                                event.preventDefault();
                            }
                        }
    
                    });
                    
                    $('.invest_status').click(function(){
                        old_select_value = $(this).val();
                    });
                    
                    $('.invest_status').change(function(){
                        sel_obj = $(this).parent();
                        var selected_status = $(this).val();
                        if (selected_status == '1')
                            if (confirm('Confirm you have adjusted this professional\'s connection level to permit access to investor files.') == false) {
                                $(this).val(old_select_value);
                                return;
                            }
                        var uid = $(this).attr('invest-id');
                        POSTDATA = 'action=admin_change_invest_status&target_uid='+uid+'&status='+selected_status;
                        ajax_update(POSTDATA, tr_bg_color_green);
                    });
                    
                    $('#filter_btn').click(function() {
                        var investor = $('#investor_type').val();
                        var status = $('#status_type').val();

                        window.location.href = $('#redirect').val()+'&section=fundraising_administration&panel=fap&type=committed_capital&investor='+investor+'&status='+status;
                    });
                    
                });
            </script>";

                    if ($sortby != '') {
                        $sql = "select " . $sortby . " sortby,SUM(amount) as funded_sum, SUM(ioi) as ioi_sum, COUNT(*) count from maenna_professional_investment where deleted!=1 and company_id = " . $companyId . " group by " . $sortby . " order by " . $sortby . " desc";
                        $result = db_query($sql);
                        $content .= '<div style="height: 60px;"></div>';
                        $content .= '<div class="box_title shaded_title"><span>SUMMARY</span></div><div style="margin: auto; margin-top: -28px;"><table cellspacing="0" cellpadding="0" class="blue-border monitoring-values">
            <thead><tr><th>' . strtoupper($sortby) . '</th>';
                        if ($sortby == 'ioi' || $sortby == 'amount')
                            $columns = array('COUNT' => 'count');
                        else $columns = array(
                            'IOI' => 'ioi', 'FUNDED' => 'amount',
                            'COUNT' => 'count'
                        );
                        foreach ($columns as $column_name => $column)
                            $content .= "<th width='35%'>$column_name</th>\n";
                        $content .= '</tr></thead><tbody>';

                        while (($Row = db_fetch_array($result)) !== false) {
                            $content .= '<tr>';
                            switch ($sortby) {
                                case 'type':
                                    $data = strtoupper($Row[sortby]);
                                    break;
                                case 'ioi':
                                    $data = strtoupper($Row[sortby]);
                                    break;
                                case 'amount':
                                    $data = strtoupper($Row[sortby]);
                                    break;
                                case 'date':
                                    $data = $Row[sortby];
                                    break;
                                case 'status':
                                    if ($Row[sortby] == 0)
                                        $data = 'PENDING';
                                    else if ($Row[sortby] == 1)
                                        $data = 'ACCEPTED';
                                    else if ($Row[sortby] == 2)
                                        $data = 'DECLINED';
                                    else if ($Row[sortby] == 3)
                                        $data = 'FUNDED';
                                    break;
                            }
                            $content .= "<td>$data</td>";
                            if ($sortby != 'ioi' && $sortby != 'amount') {
                                $content .= "<td>$" . number_format($Row[ioi_sum]) . "</td>";
                                $content .= "<td>$" . number_format($Row[funded_sum]) . "</td>";
                            }
                            $content .= "<td>$Row[count]</td>";
                            $content .= '</tr>';
                        }
                        $content .= '</tbody>';
                        $content .= '</table></div>';
                        $content .= '</div></div>';
                    }
                    break;
                }
            case "turn_invest_now": {

                    break;
                }
            default: {
                    break;
                }
        }
    }
    elseif ($op == 'update') {

        switch ($type) {
            case "company_info": {

                    break;
                }
            case "committed_capital": {

                    break;
                }
            case "turn_invest_now": {

                    break;
                }
            default: {
                    break;
                }
        }
    }

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
/* EOF */


///account?tab=companies&page=company_detail&id=845&mtab=about&section=fundraising_administration&panel=fap&type=company_info