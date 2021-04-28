<?php
/* Fundraising analysis */
function render_company_fundraising_analysis()
{
    global $user, $AccessObj;

    if ($_REQUEST['tab'] != 'companies') {
        return '';
    }
    $company_id = $_REQUEST['id'];
    $userid = $_REQUEST['userid'];

    $selectedOptions = getSelectedCompanyPage2($company_id);
    if (empty($selectedOptions)) {
        // ERROR
        return '';
    }

    // Invest Now - button
    $str_minimum = '$' . number_format($selectedOptions[0]['min_per_investor'], 0, '.', ',');
    $output = '';
    $buttonLabel = 'EXPLORE INVESTMENT';
    $buttonStatusLabel = $str_minimum.' Minimum';
    $output .=   '<div class="fieldset">';
    $output .=     '<div class="blue_button_box with-status-label">';
    $companyid = get_var('id');
        $flag_userid = $AccessObj->uid;
        if($userid) $flag_userid = $userid;
        $sql = "select status, ioi, amount, date from maenna_professional_investment where prof_id=%d and company_id=%d and deleted!=1 order by date desc limit 1";
        $result = db_query($sql, array($flag_userid, $company_id));
        $Row = db_fetch_array($result);
        $submitedValueLabel = '$' . number_format($Row['amount'], 0, '.', ',');
        $submitedIoiLabel = '$' . number_format($Row['ioi'], 0, '.', ',');
        $cdate = $Row['date'];
        $show_date = date("d/m/y", time($cdate));
        if ($Row) {
            switch ($Row['status']) {
                case "0":
                    $output .= '<a class="blue_button" href="account?tab=companies&amp;page=company_detail&amp;mtab=about&amp;id=' . $companyid . '&amp;panel=prof_explore_investment&mode=add" style="cursor:pointer; width: 100%;" cid="'.$companyid.'" uid="'.$user->uid.'">
                                <div class="main-text" style="width: 120%; margin-top:7px;margin-left: -20px; font-size: 22px;">IOI submitted '.$show_date.'</div><div class="button-status-label">'.$submitedIoiLabel.'</div></a>
                                <div class="status-label funding-purpose--label" style="text-align:center;font-size:14px;">Pending approval</div>
                    <!--<script type="text/javascript">
                        $(document).ready(function(){
                            setInterval(function () {
                                var path = window.location.href, company_id, userId;
            
                                userId = $("#user_id").val();
                                company_id = path.substring(path.indexOf("company_detail&id=") + 18, path.length);
                                sel_obj = $("a.blue_button div.main-text");
                                POSTDATA = "action=check_invest_status&userId="+userId+"&companyId="+company_id;
                                ajax_update(POSTDATA, admin_invest_loi_approved);
                            }, 5000);
                        });
                    </script>-->';
                    break;
                case "1":
                    if ($Row['amount'] == 0)
                        $output .= '<a class="blue_button" 
                                       href="account?tab=linkbank&companyid=' . $companyid . '"
                                       style="cursor:pointer; width: 100%;" cid="'.$companyid.'" uid="'.$user->uid.'">
                                       <div class="main-text" style="width: 120%; margin-left: -20px; font-size: 22px;">Invest Now</div>
                                       <div class="button-status-label">' . $buttonStatusLabel . '</div>
                                    </a>';
                    else
                        $output .= '<a class="blue_button" 
                                       href="#" onclick="return false;"
                                       style="cursor:pointer; width: 100%;">
                                       <div class="main-text" style="width: 120%; margin-left: -20px; font-size: 22px;" cid="'.$companyid.'" uid="'.$user->uid.'">'.$submitedValueLabel.' Submitted</div>
                                       <div class="button-status-label">' . $buttonStatusLabel . '</div>
                                       </a>';
                        // $output .= '<div class="status-label funding-purpose--label" style="font-size:14px;text-align: center;">Pending receipt confirmation</div>';

                    break;
                case "2":
                    $detail_message = "You might have reached the non-accredited investor allowable limit. Check your limit in your account or email clewed@clewed.com to discuss further.";
                    $output .= '<a data-tooltip="'.$detail_message.'"
                                   onclick="return false;"  
                                   class="blue_button" 
                                   href="#" 
                                   style="cursor:pointer; width: 100%;" cid="'.$companyid.',uid="'.$user->uid.'">
                                   <div class="main-text" style="width: 120%; margin-left: -20px; font-size: 22px;">Declined</div>
                                   <div class="button-status-label">Declined investment</div>
                                   </a>';
                    break;
                case "3":
                    $output .= '<a class="blue_button" 
                                       href="#" onclick="return false;"
                                       style="cursor:pointer; width: 100%;">
                                       <div class="main-text" style="width: 120%; margin-left: -20px; font-size: 22px;" cid="'.$companyid.'" uid="'.$user->uid.'">'.$submitedValueLabel.' Submitted</div>
                                       <div class="button-status-label">' . $buttonStatusLabel . '</div>
                                       </a>';
                        // $output .= '<div class="status-label funding-purpose--label" style="font-size:14px;text-align: center;">Pending receipt confirmation</div>';
                    break;
                case "4":
                    $output .= '<a class="blue_button" href="#" onclick="return false;" style="cursor:pointer; width: 100%;" cid="'.$companyid.'" uid="'.$user->uid.'">
                                    <div class="main-text" style="width: 120%; margin-left: -20px; font-size: 22px;">Invested</div>
                                    <div class="button-status-label">$' . number_format($Row['amount']) . '</div>
                                </a>';
                    break;
            }

        }
        else
            $output .= '<a class="blue_button" href="account?tab=companies&page=company_detail&id=' . $companyid . '&section=company_name&panel=prof_explore_investment&mode=add" style="cursor:pointer; width: 100%;" cid="'.$companyid.'" uid="'.$user->uid.'" flag=1><div class="main-text" style="width: 120%; margin-left: -20px; font-size: 16px;">' . $buttonLabel . '</div><div class="button-status-label">' . $buttonStatusLabel . '</div></a>';
    $output .=     '</div>';
    $output .=   '</div>';

    $output.= fundraising_additional_info($selectedOptions);

    return $output;
}
?>
<script>
    $(document).ready(function () {
        $('.blue_button').click(function(){
            sel_obj = $(this);
            cid = sel_obj.attr("cid");
            uid = sel_obj.attr("uid");
            flag = sel_obj.attr("flag");
            if(flag == 1){
                console.log(cid,uid);
                uid = sel_obj.attr("uid");
                if (confirm("Confirm your connection request!")) {
                $.post("/themes/maennaco/includes/collaborator.php", { companyId: cid,pid: uid},
                    function(response){
                        alert(response);
                    });
                }
            }
        });
    });
</script>
