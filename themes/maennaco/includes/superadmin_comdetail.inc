<?php
$maenna_page['content'] = array(    
                                            'maenna_company_name',
                                            'maenna_company_financials',
                                            'maenna_analysis',
                                            'advisory_advice',
                                            'relavent_info',
                                            'superadmin_comprogress');
$tab = sget($_REQUEST, 'tab');
if($tab == 'analysis') {
    $maenna_page['left'] = array('');
    $maenna_page['content'] = array('maenna_analysis');
}elseif($tab == 'plan'){
    $maenna_page['left'] = array('');
    $maenna_page['content'] = array('maenna_analysis');
}elseif($tab == 'monitoring'){
    $maenna_page['left'] = array('');
    $maenna_page['content'] = array('maenna_analysis');
}elseif($tab == 'files'){
    $maenna_page['left'] = array('');
    $maenna_page['content'] = array('maenna_analysis');
}elseif($tab == 'watchlist'){
    $maenna_page['left'] = array('');
    $maenna_page['content'] = array('maenna_analysis');
}
  
function company_section_tabs($op = null)
{
    global $maenna_page;
    $companyid = sget($_REQUEST, 'companyid');
    $tab = sget($_REQUEST, 'tab');
    if(empty($tab)) $tab = 'analysis';
    $Tabs = array('analysis' => 'analysis',
                  'plan'  => 'plan',
                  'monitoring' => 'monitoring',
                  'files' => 'Files/Minutes',
                  'watchlist'=> 'Watch List');
    $i = 0;
    foreach($Tabs as $key => $title){
        $active = ''; $class = '';
        if($key == $tab)$active = ' active-trail active';
        if($i == 0)$class = 'first';
        elseif($i == (count($Tabs) - 1))$class = 'last';
        $content .= "\n<li class=' $class $active '><a href='/account?a=companies&type=company_detail&tab=$key&companyid=$companyid' class='$active'>".ucwords($title)."</a></li>";
        $i++;
    }
    $content = <<< END
    <div style='position:absolute;background:blue;height:45px;top:0;'>
        &nbsp;
    </div>
END;
    $Block['body'] = content_box($Block['title'], $content);
    
    
    
    return $Block;
}

function superadmin_comprogress($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = 'Progress (super admin only) ';
    $content = '';
    $time = time();
    
    $content = '';
    
    if($op == null || $op == 'view')
    {
        if($companyid){
            $sql = "select * from maenna_company_data where data_type = 'progress' and companyid = %d order by access desc limit 3";
            $result = db_query($sql, array($companyid));
            while(($Row = db_fetch_object($result)) !== false)
            {
                $date = date('m/d/Y', $Row->access);
                $title = htmlentities($Row->data_value, ENT_QUOTES);
                $text = htmlentities(contentExcerpt($Row->data_value2), ENT_QUOTES);
                $followup = $Row->data_attr;
                if($followup) $followup = "(follwup: " . date('m/d/Y', $followup) . ")";
                $access = date('m/d/Y', $Row->access);
                $content .= "\n<div class=row><b>$access - $title $followup</b><p>$text</p></div>";
            }
        }
    }
    $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
    "<a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&view=add&companyid=$companyid'>add</a>&nbsp;".
    "<a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&companyid=$companyid'>more</a></div>";
       
    
    $Block['body'] = content_box($Block['title'], $content);
    
    return $Block;
}

function superadmin_comprogress_panel($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = 'Progress (super admin only)';
    $content = '';
    $time = time();
    
    if($op == null || $op == 'view')
    {
        $view = sget($_REQUEST, 'view');
        if(empty($view))
        {
            $sql = "select * from maenna_company_data where companyid = %d and data_type = 'progress' order by access desc";
            $result = db_query($sql, array($companyid));
            while(($Row = db_fetch_object($result)) !== false)
            {
                $subject = htmlentities($Row->data_value,ENT_QUOTES);
                $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES));
                $dataid = $Row->dataid;
                $followup = $Row->data_attr;
                if($followup) $followup = "(follwup: " . date('m/d/Y', $followup) . ")";
                $access = date('m/d/Y', $Row->access);
            
                $content .= "\n<div style='position:relative;border-bottom:solid 1px #ebebeb;'>" .
                                "<div style='position:absolute;height:20px;width:50px;top:8px;right:10px'>".
                                "\n<a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&view=edit&dataid=$dataid&companyid=$companyid' class=edit-icon>edit</a>".
                                "\n<a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&do=delete&update_section=superadmin_comprogress_panel&dataid=$dataid&companyid=$companyid' class='delete-icon' onclick='return confirm(\"Continue to remove the record\")'>delete</a>".
                                "</div>" .
                                "\n<label><b>$access - $subject $followup </b></labe><p>$text</p></div>";
            }
        }elseif($view == 'add' || $view == 'edit')
        {
            $subject = $detail = $followup = '';
            $dataid = sget($_REQUEST, 'dataid');
            if($dataid){
                $sql = "select * from maenna_company_data where dataid = %d and data_type = 'progress' limit 1";
                $result = db_query($sql, array($dataid)); $Row = db_fetch_object($result);
                if($Row) {
                    $detail = $Row->data_value2;
                    $subject = $Row->data_value;
                    $followup = date('m/d/Y', $Row->access);
                }
            }
            $content .= <<< END
            <form method='post' action='/account' onsubmit='return check_input();'>
                <table class='edit_table'>
                    <tr><td>Subject</td><td><input type=text name=subject value='$subject' style='width:500px;'> </td></tr>
                    <tr><td>Detail</td><td><textarea name='detail' style='width:500px;height:240px;'>$detail</textarea></td></tr>
                    <tr><td>Followup</td><td><input type='text' name=followup value='$followup' class='datepicker' /></td></tr>
                    <tr><td></td><td><input type=submit value='submit' class=button ></td></tr>
                </table>
                <input type=hidden name=a  value=companies >
                <input type=hidden name=type  value=company_detail >
                <input type=hidden name=panel  value=superadmin_comprogress_panel >
                <input type=hidden name=update_section  value='superadmin_comprogress_panel' >
                <input type=hidden name=companyid  value='$companyid'>
                <input type=hidden name=dataid  value='$dataid'>
            </form>
            <script type='text/javascript'>
            $(document).ready(function(){
                init_datepicker();
            })
            </script>
END;
        }
    }elseif($op == 'update')
    {
        $dataid = sget($_REQUEST, 'dataid');
        $subject = sget($_REQUEST, 'subject');
        $detail = sget($_REQUEST, 'detail');
        $followup = sget($_REQUEST, 'followup');
        if($followup) $followup = strtotime($followup);
        $do = sget($_REQUEST, 'do');
        
        if(empty($do))
        {
            if($subject)
            {
                if($dataid)
                {
                    $sql = "update maenna_company_data set data_value = '%s',data_value2 = '%s',access='%s',data_attr='%s', editorid=%d where dataid = %d and data_type = 'progress' limit 1";
                    $DBValues = array($subject, $detail, $time, $followup, $editorid, $dataid);
                }else{
                    $sql = "insert into maenna_company_data (companyid, access, data_type,data_attr, data_value,data_value2, editorid) values(%d,'%s','%s','%s','%s','%s',%d)";
                    $DBValues = array($companyid, $time, 'progress',$followup,$subject,$detail, $editorid);
                }
                if(db_query($sql, $DBValues)) drupal_set_message("the record is updated.");
                else drupal_set_message("failed to save changes.", 'error');
            }
        }elseif($do == 'delete')
        {
            if($dataid)
            {
                $sql = "delete from maenna_company_data where data_type = 'progress' and dataid = %d limit 1";
                if(db_query($sql, array($dataid))) drupal_set_message("the record is removed.");
                else drupal_set_message("failed to remove record.", 'error');
            }
        }
        
    }
    $content .= "<div class='align-right edit-link'><a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&view=add&companyid=$companyid'>add</a></div>";
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function advisory_KPI($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = 'KPI';
    $content = '';
    $time = time();
    
    $content = '';

    $KPI_Values = array();
    if($op == null || $op == 'view')
    {
        if($companyid){
            $sql = "select * from maenna_company_data where data_type = 'kpi' and companyid = %d";
            $result = db_query($sql, array($companyid));
            $Row = db_fetch_object($result);
            if($Row)
            {
                $KPI_Values = unserialize($Row->data_value2);
            }
            foreach($KPI_Values as $key => $Item){
                if($Item && is_array($Item) )
                {
                    $title = htmlentities(sget($Item, 'title'),ENT_QUOTES);
                    if(empty($title))continue;
                    if($key !== 'file')
                    {
                        
                        $text = htmlentities(sget($Item, 'text'), ENT_QUOTES);
                        $content .= "\n<div class=row style='position:relative;padding:2px 0;'><div style='position:absolute;text-align:right;width:100%'>$text</div>$title</div>";
                    }
                    else{
                        $text = sget($Item, 'text');
                        $path = "./" . file_directory_path() . "/" . $text;
                        $content .= "\n<div class=row style='position:relative;padding:2px 0;'><div style='position:absolute;text-align:right;width:100%'><a href='${path}' target='_blank'>view</a></div>$title</div>";
                    }
                }
            }
        }
    }
    $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
    "<a href='/account?a=companies&type=company_detail&panel=advisory_KPI_panel&companyid=$companyid&view=edit'>edit</a></div>";
       
    
    $Block['body'] = sidebar_box($Block['title'], $content);
    
    return $Block;
}

function advisory_KPI_panel($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = 'KPI';
    $content = '';
    $time = time();

    if($op == null || $op == 'view')
    {
        $view = sget($_REQUEST, 'view');
        if($view == 'add' || $view == 'edit')
        {
            $sql = "select * from maenna_company_data where companyid = %d and data_type = 'kpi'";
            $result = db_query($sql, array($companyid));
            $dataid = '';
            $oldfile = '';
            $fileTitle = '';
            $Data = array();
            $Row = db_fetch_object($result);
            if($Row)
            {
                $data_value2 = $Row->data_value2;
                $Data = unserialize($data_value2);
                $dataid = $Row->dataid;
                $Item = sget($Data, 'file');
                if($Item && is_array($Item))
                {
                    $oldfile = sget($Item, 'text');
                    $fileTitle = sget($Item, 'title');
                }
                
            }

            for($i = 1; $i <= 7; $i ++)
            {
                $Item = sget($Data, $i);
                if($Item && is_array($Item))
                {
                    $title = sget($Item, 'title');
                    $text = sget($Item, 'text');
                }
              
                if($key != 'file') $content .= "\n<tr><td>${i}. </td><td><input type=text name='title_$i' value = '$title' /></td>
                                                        <td><input type=text name='text_$i' value='$text' /></td></tr>";
            }
            
            $content = <<< END
            <div class=row>Title examples: <br>Net Margin, Gross Margin, ROE, Revenue Growth, Earnings Growth, New Markets</div>
            <form method='post' action='/account' onsubmit='return check_input();' enctype='multipart/form-data'>
                <table class='edit_table'>
                    <tr><td></td><td>Title</td><td>Value</td></tr>
                    $content
                    <tr><td>Upload File</td><td><input type=text name=fileTitle value='$fileTitle' /></td><td><input type=file name=file /></td></tr>
                    <tr><td></td><td><input type=submit value='submit' class=button ></td></tr>
                </table>
                <input type=hidden name=oldfile value='$oldfile' />
                <input type=hidden name=a  value=companies >
                <input type=hidden name=type  value=company_detail >
                <input type=hidden name=view  value=edit >
                <input type=hidden name=panel  value=advisory_KPI_panel >
                <input type=hidden name=update_section  value='advisory_KPI_panel' >
                <input type=hidden name=companyid  value='$companyid'>
                <input type=hidden name=dataid  value='$dataid'>
            </form>
            
END;
        }
    }elseif($op == 'update')
    {
        
        $dataid = sget($_REQUEST, 'dataid');

      
        $do = sget($_REQUEST, 'do');
        
        if(empty($do))
        {
        
            $filename = '';
            $fileTitle = sget($_REQUEST, 'fileTitle');
            if(isset($_FILES['file']) && $_FILES['file']['error'] == 0)
            {
                $ext = get_filetype($_FILES["file"]["name"]);
                $filename = ranStr() . "." . $ext;
                $path = "./" . file_directory_path() . "/";
                if(move_uploaded_file($_FILES["file"]["tmp_name"], $path . $filename)){
                    $KPI_Values["file"] = $filename;    
                }else{
                    druapl_set_message("Filed to save uploaded file", 'error');
                }
            }
            $Data = array();
            for($i = 1; $i <= 7 ; $i ++)
            {
                $title = sget($_POST, "title_$i");
                $text = sget($_POST, "text_$i");
                $Data["$i"] = array('title' => $title, 'text' => $text);
            }
            if($fileTitle)
            {
                $oldfile = sget($_REQUEST, 'oldfile');
                if($filename) $Data['file'] = array('title' => $fileTitle, 'text' => $filename);
                elseif($oldfile) $Data['file'] = array('title' => $fileTitle, 'text' => $oldfile);
            }
            
            $data_value2 = serialize($Data);
            if($dataid)
            {
                $sql = "update maenna_company_data set data_value2 = '%s',access='%s', editorid=%d where dataid = %d and data_type = 'kpi' limit 1";
                $DBValues = array($data_value2, $time, $editorid, $dataid);
            }else{
                $sql = "insert into maenna_company_data (companyid, access, data_type,data_value2, editorid) values(%d,'%s','%s','%s',%d)";
                $DBValues = array($companyid, $time, 'kpi',$data_value2, $editorid);
            }
            if(db_query($sql, $DBValues)) drupal_set_message("the record is updated.");
            else drupal_set_message("failed to save changes.", 'error');
            
        }
        
    }
    //$content .= "<div class='align-right edit-link'><a href='/account?a=companies&type=company_detail&panel=superadmin_comprogress_panel&view=add&companyid=$companyid'>add</a></div>";
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}


/*EOF */