<?php

function investments($op = null)
{

    global $user;
    global $AccessObj;
    global $redirect;
    $section = 'company_name';
    $panel = __FUNCTION__;
    $username = null;
    if ($_REQUEST['id'] && ($AccessObj->user_type == 'super' || $AccessObj->user_type == 'admin'))
        $userid = $_REQUEST['id'];
    else
        $userid = $user->uid;
//    $companyid = sget($_REQUEST, 'id');

    /*if($_REQUEST['mode'] == 'edit') {
        $uid = isset($_REQUEST[id])?$_REQUEST[invest_id]:'-1';
        $type = isset($_REQUEST[type])?$_REQUEST[type]:'';
        $amount = isset($_REQUEST[amount])?$_REQUEST[amount]:'';
        if (isset($_REQUEST[type]))
            $op = 'edit';
        else
            $op = 'new';

        $Block['title'] = "Input - EDIT";
        $content = '<input type="hidden" id="userId" value="' . $AccessObj->uid . '">';
        $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
        $content .= "<div id='box1' style='margin-bottom:15px;'>
                        <form id='addInvest' method=post action='/account' enctype='multipart/form-data' onsumbit=\"return validateFileForm();\">
                            <select id='investor_type' style='width:207px;height:28px;margin-top:10px; margin-bottom:10px;' name='investor_type'><option value=''>Investor Type:</option>";
        foreach (INVESTOR_TYPE() as $key => $value)
            if ($key != $type)
                $content .= "<option value='" . $key . "'>" . $value . "</option>";
            else
                $content .= "<option value='" . $key . "' selected>" . $value . "</option>";
        $content .= "</select>";
        $content .= "<br><input style='width:197px!important;height:22px;margin-bottom:10px;' id='amount' type=text name='amount' placeholder='Amount' value='$amount' />";
        $content .= "<div id='agree'><br><input style='margin-left:10px; margin-top:7px;' type='checkbox' id='cmp-agree'><label style='margin-bottom: 10px;' for='cmp-agree'>I agree with <a class='show_confidentiality' type='terms' target='_blank' style='font-family:Lato Regular;font-style: italic;color:#00a1be;'>confidentiality</a> to access fundraising information </label></div>";
        $content .= "<br><input type=submit name=submit value='Submit' class='button' /> &nbsp;
                <a href='' class='hidebox button' boxid=box1>Close</a>
        </div>
        $HPV
        <input type='hidden' name=section value='prof_invest' />
        <input type='hidden' name=update_section value='prof_invest' />
        <input type='hidden' id='do' name=do value=$op />
        <input type='hidden' id='dataid' name=dataid value=$uid />
        </form>";

        $content .= "<script type='text/javascript' src='/themes/maennaco/jui/comments/js/jquery.livequery.js'></script>
    <script type='text/javascript'>
        function add_invest() {
            $('#investor_type').val('');
            $('#amount').val('');
            $('#do').val('new');
            $('#dataid').val('-1');

            $('#agree').attr('style', 'display: inline');
        }
        function edit_invest(id, type, amount) {
            $('#investor_type').val(type);
            $('#amount').val(amount);
            $('#do').val('edit');
            $('#dataid').val(id);

            $('#box1').attr('style', 'display: inline');
            $('#agree').attr('style', 'display: none');
        }
        $(document).ready(function(){
            $('.account-section-tabs').attr('style', 'display: none');
            $('#squeeze').attr('style', 'margin-top: -40px');
            $('.left-td-wrapper').attr('style', 'margin-top: -20px');

            init_openbox();
            init_hidebox();
            init_sortPanel();

            $('#addInvest').submit(function(event) {

                if ($('#investor_type').val() == '') {

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

//                if ($('#do').val() == 'new') {
                    if ($('#cmp-agree').is(':checked') == false) {
                        alert('You have to read confidentiality!');
                        event.preventDefault();
                    }
                    $('a.blue_button div.main-text').html('LOI Submitted');
//                }

            });
        });
    </script>";
    }
    else */
    {
        if (isset($_REQUEST['sortby'])) $sortby = $_REQUEST['sortby'];
        $Block['title'] = "<span style='margin-left: 15px; color: #ffffff'>Investment Activity</span>";
        $File_Perm = $AccessObj->Com_sections['file']['sections'];
        $_Tags = PROF_INVEST_SORT();
        $tag_no = count($_Tags);
        $company_id = sget($_REQUEST, 'id');
        $i = 0;

        $sortLinks = '';
        foreach ($_Tags as $key => $value) {
            if ($sortby == $key) $font_style = 'color:#00a2bf;'; else $font_style = 'font-style:italic;';
            $sortLinks .= "<a href='${redirect}&section=$section&panel=$panel&sortby=$key' style='$font_style font-size:12px;cursor:pointer;";

            if ($i == 0)
                $sortLinks .= "margin-right:10px;'>{$value}</a>";
            else
                $sortLinks .= "margin-left:10px;margin-right:10px;'>{$value}</a>";

            if (++$i != $tag_no)
                $sortLinks .= '|';
        }
        if(!empty($sortLinks))
            $sortLinks = rtrim($sortLinks, '|');

        $status = array('<span class="color-swatch" data-tooltip="Pending" style="background-color:#FFE940; border:1px solid #FFE940; padding-right: 15px;"></span>',
            '<span class="color-swatch" data-tooltip="Accepted" style="background-color:#00A2BF; border:1px solid #00A2BF; padding-right: 15px;"></span>',
            '<span class="color-swatch" data-tooltip="Declined" style="background-color:#DF4000; border:1px solid #DF4000; padding-right: 15px;"></span>',
            '<span class="color-swatch" data-tooltip="Funded" style="background-color:#6ddf45; border:1px solid #6ddf45; padding-right: 15px;"></span>');

//    $Block['title'] .= '<a class= "openbox" boxid="box1" data-tooltip="' . $tooltip . '" style="color:#fff;cursor:pointer;display:inline-block;margin-left:120px; margin-top:5px;height:20px;line-height:22px;border-left:thin solid #fff;padding-left:10px;font-family:LatoRegular; font-size:13px; text-transform: none;" onclick="add_invest()">Add</a>';

//        $Block['title'] .= '<a class= "openbox" boxid="box2" data-tooltip="' . $tooltip . '" style="color:#fff;cursor:pointer;display:inline-block;margin-left:10px; margin-top:5px;height:20px;line-height:22px;border-left:thin solid #fff;padding-left:10px;font-family:LatoRegular; font-size:13px; text-transform: none;">Edit</a>';

//        $Block['title'] .= '<span class="color-swatch" data-tooltip="Accepted" style="background-color:#00A2BF; border:1px solid #00A2BF; margin-left: 20px; padding-left: 4px; padding-right: 13px;"></span>';
//        $Block['title'] .= '<span class="color-swatch" data-tooltip="Pending" style="background-color:#FFE940; border:1px solid #FFE940; margin-left: 10px; padding-left: 4px; padding-right: 13px;"></span>';
//        $Block['title'] .= '<span class="color-swatch" data-tooltip="Declined" style="background-color:#DF4000; border:1px solid #DF4000; margin-left: 10px; padding-left: 4px; padding-right: 13px;"></span>';

//        $Block['title'] .= "<span style='margin-top:5px;height:20px;line-height:22px;border-left:thin solid #fff;padding-left:10px; float:right;display:inline-block;font-weight:bold; font-family:LatoRegular; font-size:13px; text-transform:none; color: #ffffff'>Sort By <img class='openSortPanel' panelid='file_sort' style='width:20px; margin-left:20px;' src='themes/maennaco/images/arrow_down.png'></span>";

        $content = '';
        //Set this page styles
        $content .= "<style>
                .shaded_title {

                    background:#94c9da;
                    font-size: 16px;
                    height:30px;
                    line-height:30px;
                }
                .main_content {
                    padding-top:0 !important;
                }
                table.account-table td.right-td {
                    border-left:none !important;
                }
            </style>";
        $content .= '<div id="file_sort" style="display:none;line-height:30px;height:30px;width:100%; background-color:#f2f2f2"><span style="margin-top:0; display:inline-block;vertical-align:middle;margin-left:15px;">';
        $content .= $sortLinks . '</span></div>';
        $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
        $content .= "<div id='box1' style='margin-bottom:15px;display:none;'>
                            <form id='addInvest' method=post action='/account' enctype='multipart/form-data' onsumbit=\"return validateFileForm();\">
                                <select id='investor_type' style='width:207px;height:28px;margin-top:10px; margin-bottom:10px;' name='investor_type'><option value=''>Investor Type:</option>";
        foreach (INVESTOR_TYPE() as $key => $value)
            $content .= "<option value='" . $key . "'>" . $value . "</option>";
        $content .= "</select>";
        $content .= "<br><input style='width:197px!important;height:22px;margin-bottom:10px;' id='amount' type=text name='amount' placeholder='Amount' />";
        $content .= "<div id='agree'><br><input style='margin-left:10px; margin-top:7px;' type='checkbox' id='cmp-agree'><label style='margin-bottom: 10px;' for='cmp-agree'>I agree with <a class='show_confidentiality' type='terms' target='_blank' style='font-family:Lato Regular;font-style: italic;color:#00a1be;'>confidentiality</a> to access fundraising information </label></div>";
        $content .= "<br><input type=submit name=submit value='Submit' class='button' /> &nbsp;
                    <a href='#' class='hidebox button' boxid=box1>Close</a>
            </div>
            $HPV
            <input type='hidden' name=section value='prof_invest' />
            <input type='hidden' name=update_section value='prof_invest' />
            <input type='hidden' id='do' name=do value='new' />
            <input type='hidden' id='dataid' name=dataid value='-1' />
            </form>";

        $IOI = 0;
        $Funded = 0;
        if ($sortby == '') {
                $sql = "select a.*, p.firstname name from maenna_professional_investment a join maenna_people p on a.prof_id=p.pid where a.prof_id=%d AND a.deleted!=1 order by a.created desc";
        }
        else {
                $sql = "select a.*, p.firstname name from maenna_professional_investment a join maenna_people p on a.prof_id=p.pid where a.prof_id=%d AND a.deleted!=1 order by " . $sortby . " desc";
        }
        $result = db_query($sql, array($userid));
        while ($Row = db_fetch_object($result)) {
            if(!$username) $username = $Row->name;
            $id = $Row->id;
            $tmpRow = '';
            $tmpRow .= "\n<tr style='height: 32px;'>
                <td style='padding-top:5px;font-size:14px;'><div style='overflow:hidden;text-align:left;height:32px;font-family: lato light;color: #929497;font-size: 14px;'>" . date("d/m/Y", $Row->created) . "</div></td>
                <td style='padding-left:15px;padding-top:5px;font-size:14px;'><a href='/account?tab=companies&page=company_detail&id=" . $Row->company_id . "&userid=".$userid."' style='overflow:hidden;text-align:left;height:32px;font-family: lato light;color: #929497;font-size: 14px;'>$Row->proj_name</a></td>";
//                if ($Row->status != '3') {
//                    $IOI += $Row->amount;
//                    $tmpRow .= "<td style='padding-top:5px;font-size:14px;'><div style='overflow:hidden;text-align:right;height:32px;font-family: lato light;color: #929497;font-size: 14px;'>$" . number_format($Row->amount) . "</div></td>
//                    <td style='padding-top:5px;font-size:11px;'><div style='overflow:hidden;text-align:right;height:32px;font-family: lato light;color: #929497;font-size: 14px;'>$0</div></td>";
//                }
//                else {
//                    $Funded += $Row->amount;
//                    $tmpRow .= "<td style='padding-top:5px;font-size:14px;'><div style='overflow:hidden;text-align:right;height:32px;font-family: lato light;color: #929497;font-size: 14px;'>$0</div></td>
//                    <td style='padding-top:5px;font-size:14px;'><div style='overflow:hidden;text-align:right;height:32px;font-family: lato light;color: #929497;font-size: 14px;'>$" . number_format($Row->amount) . "</div></td>";
//                }
                $IOI += $Row->ioi;
                $Funded += $Row->amount;
                $tmpRow .= "<td style='padding-top:5px;font-size:14px;'><div style='overflow:hidden;text-align:right;height:32px;font-family: lato light;color: #929497;font-size: 14px;'>$" . number_format($Row->ioi) . "</div></td>
                    <td style='padding-top:5px;font-size:14px;'><div style='overflow:hidden;text-align:right;height:32px;font-family: lato light;color: #929497;font-size: 14px;'>$" . number_format($Row->amount) . "</div></td>";
                $tmpRow .= "<td style='padding-top:5px;font-size:14px;'><div style='overflow:hidden;text-align:right;height:32px;font-family: lato light;color: #929497;font-size: 14px;'>" . $status[$Row->status] . "</div></td>";
                $amount_style = '$' . number_format($Row->amount);
            //If file file author is currently logged in user
//            $tmpRow .= "<td style='padding-top:5px;font-size:11px;'>
//                    <a href='account?tab=companies&page=company_detail&id=$company_id&section=company_name&panel=prof_explore_investment&mode=edit&invest_id=$Row->id&type=$Row->type&amount=$Row->amount' style='display: inline-block;border-left: 1px solid;line-height: 1.1em;padding-left: 5px;' class='tool'>Edit</a>
//                    <a href='.$redirect&update_section=prof_invest&do=del&dataid=" . $id . "' onclick='return confirm(\"Continue to delete this file?\")' style='margin-left: 5px;display: inline-block;border-left: 1px solid;line-height: 1.1em;padding-left: 5px;' class='tool'>Del</a></td>";
            $tmpRow .= "</tr>";
            $fileList[] = $tmpRow;
        }

        $tmpRow = "\n<tr style='height: 32px;'>
            <td style='padding-left:15px;padding-top:5px;font-size:14px;'><div style='overflow:hidden;text-align:left;height:32px;font-family: lato light;color: #929497;font-size: 14px;'>Total</div></td>
            <td style='padding-top:5px;font-size:14px;'></td>
            <td style='padding-top:5px;font-size:14px;'><div style='overflow:hidden;text-align:right;height:32px;font-family: lato light;color: #929497;font-size: 14px;'>$" . number_format($IOI) . "</div></td>
            <td style='padding-top:5px;font-size:14px;'><div style='overflow:hidden;text-align:right;height:32px;font-family: lato light;color: #929497;font-size: 14px;'>$" . number_format($Funded) . "</div></td>
            <td style='padding-top:5px;font-size:14px;'></td></tr>";
        $fileList[] = $tmpRow;

        $content .= '<table style="margin:0; cellspacing="0" cellpadding="0" border="0" width="100%" class="left-align">
                    <thead style="background:#F9FAFC;">
                        <tr>
                            <td style="font-weight:normal;width:12%;">DATE</td>
                            <td style="font-weight:normal;width:20%;padding-left:8px;">PROJECT NAME</td>
                            <!--<td style="font-weight:normal;width:20%">TYPE</td>-->
                            <!--<td style="font-weight:normal;width:12%">AMOUNT</td>-->
                            <td style="font-weight:normal;width:15%;padding-left:8px;">IOI $</td>
                            <td style="font-weight:normal;width:15%;padding-left:8px;">FUNDED $</td>
                            <td style="font-weight:normal;width:10%;padding-left:8px;">STATUS</td>
                            <!--<td style="font-weight:normal;width:*">ACTION</td>-->

            </tr></thead>';
//        if ($tag != 'other') {
        foreach ($fileList as $key => $value) {
//                if (count($fileList[$key]) > 0 && ($File_Perm[$key . "_1"]['access'] != 'hide' || $File_Perm[$key . "_2"]['access'] != 'hide')) {
//                    foreach ($fileList[$key] as $value1) {
            $content .= $value;
//                    }
//                }
        }
//        }
//        if (!isset($tag) || $tag == 'other') {
////            if (count($fileList['Other']) > 0 && ($File_Perm["other_1"]['access'] != 'hide' || $File_Perm["other_2"]['access'] != 'hide')) {
//                $content .= "<tr><td style='padding-left:15px; background:#f2f2f2;' colspan = '6'>Other</td></tr>";
////                foreach ($fileList['Other'] as $value) {
//                    $content .= $value;
////                }
////            }
//        }

        $content .= "</table>

        <div id='uploadDlg' style='display:none;'></div>

        <script type='text/javascript' src='/themes/maennaco/jui/comments/js/jquery.livequery.js'></script>
        <script type='text/javascript'>                
//            function add_invest() {
//                $('#investor_type').val('');
//                $('#amount').val('');
//                $('#do').val('new');
//                $('#dataid').val('-1');
//                
//                $('#agree').attr('style', 'display: inline');
//            }
//            function edit_invest(id, type, amount) {
//                $('#investor_type').val(type);
//                $('#amount').val(amount);
//                $('#do').val('edit');
//                $('#dataid').val(id);
                
//                $('#box1').attr('style', 'display: inline');
//                $('#agree').attr('style', 'display: none');
//                window.location.href = $('#redirect').val()+'&section=company_name&panel=prof_explore_investment&id='+id+'&type='+type+'&amount='+amount;
//            }
            $(document).ready(function(){
                $('.account-section-tabs').attr('style', 'display: none');
                $('#squeeze').attr('style', 'margin-top: -40px');
                $('.left-td-wrapper').attr('style', 'margin-top: -20px');
            
                init_openbox();
                init_hidebox();
                init_sortPanel();
            
                $('#addInvest').submit(function(event) {

                    if ($('#investor_type').val() == '') {

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
                        $('a.blue_button div.main-text').html('LOI Submitted');
                    }

                });
                
//                setInterval(function () {
//                    var path = window.location.href, company_id, userId;
//                    
//                    userId = $('#userId').val();
//                    company_id = path.substring(path.indexOf('company_detail&id=') + 18, path.indexOf('&section'));
//                    sel_obj = $('a.blue_button div.main-text');
//                    POSTDATA = 'action=check_invest_status&userId='+userId+'&companyId='+company_id;
//                    ajax_update(POSTDATA, admin_invest_loi_approved);        
//                }, 5000);
            });
        </script>";

//    else {
//        $Block['title'] = "File View";
//        if (sget($_REQUEST, 'name')) {
//            $Block['title'] = sget($_REQUEST, 'name');
//            $fname = sget($_REQUEST, 'name');
//        }
//        $file_array = pathinfo(sget($_REQUEST, 'name'));
//        $f_extension = $file_array['extension'];
//        if (in_array($f_extension, array('doc', 'xls', 'pdf', 'ppt', 'tiff'))) {
//            $arr = explode("_", sget($_REQUEST, 'datatype'), 2);
//        }
//        $cat = $arr[0];
//        $catName = getFileCategoryName($cat);
//        $tmpRow = '<div style="margin-right:18px;float:right;">';
//        $sql = "select * from maenna_company_data WHERE deleted != 1 and dataid = %d";
//        $result = db_query($sql, array(sget($_REQUEST, 'dataid')));
//        if ($Row = db_fetch_object($result)) {
//            $url = "http://" . $_SERVER['HTTP_HOST'] . "/" . file_directory_path() . "/" . $Row->data_value2;
//            $res  = mysql_query("SELECT d_id FROM wall_documents where document_name = '" . basename($url) . "'");
//            $red  = mysql_fetch_array($res);
//            $f_id = $red['d_id'];
//        }
//        $editorname = $user->name;
//        $editorid = $Row->editorid;
//        //If file file author is currently logged in user
//        if ($userid == $editorid || $AccessObj->user_type == 'super') {
//            $tmpRow .= "
//
//                                    <a data-tooltip='Replace file' title='REPLACE' class= 'openbox fileReplace' boxid='box2' style='display:inline-block;margin-right: 5px;' ref='" . sget($_REQUEST, 'dataid') . "' fileName = '" . sget($_REQUEST, 'name') . "' href = '#'><img src = '/themes/maennaco/images/replace_file_icon.png'></a>
//                                    <a data-tooltip='Download file' title='DOWNLOAD' href = '/download.php?tab=file&file_name=" . rawurlencode($Row->data_value2) . "' style='display:inline-block;margin-right: 5px;'><img src = '/themes/maennaco/images/download_file_icon.png'></a>
//                                    <a data-tooltip='Delete file' title='DELETE' href='.$redirect&update_section=files&do=del&dataid=" . sget($_REQUEST, 'dataid') . "'
//                                                    onclick='return confirm(\"Continue to delete this file?\")' style='display: inline-block;border-left: 1px solid;line-height: 1.1em;padding-left: 5px;' class='tool'>Delete</a>
//                                    ";
//        } else {
//            if ($File_Perm[$cat . "_2"]['access'] == 'hide') {
//                $tmpRow .= "";
//            } elseif ($File_Perm[$cat . "_2"]['access'] == 'view') {
//                $tmpRow .= "";
//            } else {
//                $tmpRow .= "<a  style='display:inline-block;margin-right: 5px;' href='/download.php?tab=file&file_name=" . $Row->data_value2 . "'><img src = '/themes/maennaco/images/download_file_icon.png'></a>
//                                    ";
//            }
//        }
//        $tmpRow .= '</div>';
//        $HPV = hidden_post_values(array('tab', 'page', 'mtab', 'id'));
//        $Block['title'] .= $tmpRow;
//        $content .= "<div id='box2' style='margin-bottom:15px;display:none;'>
//                        <form id='fileReplace' method=post action='/account' enctype='multipart/form-data'>
//                            <br><input style='width:155px;height:22px;margin-bottom:4px;' type=text name=name value='' />
//                            <br><input type=file name='file' /></br><br><input style='padding-top:3px;width:75px; height:28px;' type=submit name=submit value='submit'  class=buttonReplace /> &nbsp;
//                                    <a href='#' style='width: 53px;height: 20px;display: inline-block;text-align: center;' class='hidebox buttonReplace' boxid=box2>Close</a>
//                            </div>
//                            $HPV
//                            <input type='hidden' name=section value='files' />
//                            <input type='hidden' name=update_section value='files' />
//                            <input type='hidden' name=do value='update' />
//                            <input type='hidden' name='dataid' value='".$f_id."'/>
//                        </form>
//                    ";
//        ob_start();
//        require_once("file_comments.php");
//        $content .= ob_get_contents();
//        $content .= "
//                <script type='text/javascript'>
//$(document).ready(function(){
//    init_openbox();
//    init_hidebox();
//
//    $('.fileReplace').click(function() {
//
//    $(\"#fileReplace input[name='name']\").val($(this).attr('fileName'));
//    $(\"input[name='dataid']\").val($(this).attr('ref'));
//
//    });
//});
//</script>
//
//        <style>
//
//                        .buttonReplace {
//                            font-family: 'Lato Regular' !important;
//                            padding: 2px 9px;
//                            background: #41454F;
//                            color: white !important;
//                            border: 2px outset buttonface;
//                            text-transform: capitalize;
//                            display:inline-block;
//                            text-align:center;
//                            font-size:12px;
//                        }
//                        .buttonReplace:hover {
//                            background: #00a2bf;
//                            text-decoration: none;
//                            padding: 2px 9px;
//                        }
//                        .shaded_title {
//
//                            background:#ffffff;
//                            height:30px;
//                            line-height:30px;
//                            padding-left:15px;
//
//                        }
//
//                        .main_content {
//
//                            padding-top:0 !important;
//
//                        }
//
//                    </style>";
//        ob_end_clean();
//    }
    }
    $Block['body'] = sprintf('<h2 style="margin:10px;"><a href="/account?tab=professionals&page=pro_detail&id=%d">%s</a></h2>', $userid, $username) . content_box($Block['title'], $content);
    return $Block;
}

/* EOF */
