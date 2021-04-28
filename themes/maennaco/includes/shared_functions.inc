<?php


function view_company_logo($op = null)
{
    $Block['title'] = 'Company Logo';
    $company_id = (int)sget($_REQUEST, 'id');
    if($company_id <= 0){
        $Block['body'] = sidebar_box($Block['title'], "");
        return $Block;
    }
    
    if($op == 'view' || $op == null){
        
        $sql = "select * from users_data where uid = %d and data_type = 'logo'";
        $result = db_query( $sql, array($company_id) );
        if( db_affected_rows($result) > 0 )
        {
            $Data = db_fetch_object($result);
            $img = $Data->data_value;
                $src = "/" . file_directory_path() . "/" . $img;
                //echo $src;
                $content = "<div class='logo'><img src='$src' style='width:130px;'></div>";
        }else{
            $content = "n/a<br>";
        }
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}

function maenna_company_logo($op = null)
{
    global $user;
    $companyid = sget($_REQUEST,'companyid');
    $Block['title'] = 'Logo';
    if($op == 'view' || $op == null){
        $sql = "select * from maenna_company_data where companyid = %d and data_type = 'logo'";
        $result = db_query( $sql , array($companyid));
        if( db_affected_rows($result) > 0 )
        {
            
            $Data = db_fetch_object($result);
            $dataid = $Data->dataid;
            $img = $Data->data_value;
            $src = "/" . file_directory_path() . "/" . $img;
            //echo $src;
            $content = "<div class='logo'><img src='$src' style='width:130px;'></div>";
            $content .= "<div class='edit-link'><a href='#' class='edit_wnd' rel='maenna_company_logo' data='$companyid'>edit</a></div>";
        }else{
            
            $content = "<div class='edit-link'><a href='#' class='edit_wnd' rel='maenna_company_logo' data='$companyid'>upload a logo</a></div>";
        }
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        
      
            if ($_FILES["logo"]["error"] > 0)
            {
                drupal_set_message( $_FILES["logo"]["error"]);
            }
            else
            {
                $path = "./" . file_directory_path() . "/";
                $filename = "";
                if($ext = get_filetype($_FILES["logo"]["name"]))
                {
                    $filename = ranStr() . "." . $ext;
                    if(move_uploaded_file($_FILES["logo"]["tmp_name"], $path . $filename))
                    {
                        
                        $dataid = sget($_REQUEST,'dataid');
                        $editorid = $user->uid;
                        $time = time();
                        if($dataid)
                        {
                            $sql = "update maenna_company_data set data_value = '%s', editorid = %d, access = '%s' where dataid = %d limit 1";
                            db_query($sql, array($filename, $editorid, $time, $dataid));
                        }else{
                            $sql = "insert into maenna_company_data (companyid, data_type, data_value, access, editorid) values (%d, 'logo', '%s', '%s', %d)";
                            db_query($sql, array($companyid, $filename, $time, $editorid));
                        }
                        drupal_set_message("Your new logo has been submitted.");
                    }else{
                        drupal_set_message("Failed to upload new file",'error');
                    }
                }else{
                    drupal_set_message("The file type is not allowed",'error');
                }
            }
    }
    return $Block;
}


function advisory_advice($op=null){
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $time = time();
    
    $Block['title'] = "Advisory Council Advice";
    $content = '';
    if($op == 'view' || $op == null)
    {
        
        $sql = "select * from maenna_company_data where data_type = 'advice' and companyid = %d order by access desc limit 5";
        $result = db_query($sql, array($companyid));
        while(($Row = db_fetch_object($result)) !== false)
        {
            $access = date('m/d/Y',$Row->access);
            $text = $Row->data_value2;
            
            $text = contentExcerpt($text);
            $content .= "<div class=row><p>".$access . ' - ' . $text ."</p></div>";
        }    
    }
    
    $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
    "<a href='/account?a=companies&type=company_detail&panel=advisory_advice_panel&view=add&companyid=$companyid'>add</a>" .
                "&nbsp; <a href='/account?a=companies&type=company_detail&panel=advisory_advice_panel&companyid=$companyid'>more</a></div>";
   
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function advisory_advice_panel($op=null){
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = 'Advisory Council Advice';
    $content = '';
    $time = time();
    
    if($op == null || $op == 'view')
    {
        $view = sget($_REQUEST, 'view');
        if(empty($view))
        {
            $sql = "select * from maenna_company_data where companyid = %d and data_type = 'advice' order by access desc";
            $result = db_query($sql, array($companyid));
            while(($Row = db_fetch_object($result)) !== false)
            {
                $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES));
                $dataid = $Row->dataid;
                
                $access = date('m/d/Y', $Row->access);
                $content .= "\n<div style='position:relative;border-bottom:solid 1px #ebebeb;'>" .
                                "<div style='position:absolute;height:20px;width:50px;top:8px;right:10px'>".
                                "\n<a href='/account?a=companies&type=company_detail&panel=advisory_advice_panel&view=edit&dataid=$dataid&companyid=$companyid' class=edit-icon>edit</a>".
                                "\n<a href='/account?a=companies&type=company_detail&panel=advisory_advice_panel&do=delete&update_section=advisory_advice_panel&dataid=$dataid&companyid=$companyid' class='delete-icon' onclick='return confirm(\"Continue to remove the record\")'>delete</a>".
                                "</div>" .
                                "\n<label><b>$access</b></labe><p>$text</p></div>";
            }
        }elseif($view == 'add' || $view == 'edit')
        {
            $text = '';$month=''; $dataid = sget($_REQUEST, 'dataid');
            if($dataid){
                $sql = "select * from maenna_company_data where dataid = %d and data_type = 'advice' limit 1";
                $result = db_query($sql, array($dataid)); $Row = db_fetch_object($result);
                if($Row) {
                    $text = $Row->data_value2;
                    $access = date('m/d/Y', $Row->access);
                }
            }
            $content .= <<< END
            <form method='post' action='/account' onsubmit='return check_input();'>
                <table class='edit_table'>
                   
                    <tr><td>Detail</td><td><textarea name='text' style='width:500px;height:240px;'>$text</textarea></td></tr>
                    <tr><td></td><td><input type=submit value='submit' class=button ></td></tr>
                </table>
                <input type=hidden name=a  value=companies >
                <input type=hidden name=type  value=company_detail >
                <input type=hidden name=panel  value=advisory_advice_panel >
                <input type=hidden name=update_section  value='advisory_advice_panel' >
                <input type=hidden name=companyid  value='$companyid'>
                <input type=hidden name=dataid  value='$dataid'>
            </form>
END;
        }
    }elseif($op == 'update')
    {
        $dataid = sget($_REQUEST, 'dataid');
        $text = sget($_REQUEST, 'text');
        $do = sget($_REQUEST, 'do');
       
        
        if(empty($do))
        {
            if($text)
            {
                if($dataid)
                {
                    $sql = "update maenna_company_data set data_value2 = '%s',access='%s', editorid=%d where dataid = %d and data_type = 'advice' limit 1";
                    $DBValues = array($text, $time, $editorid, $dataid);
                }else{
                    $sql = "insert into maenna_company_data (companyid, access, data_type, data_value2, editorid) values(%d,'%s','%s','%s',%d)";
                    $DBValues = array($companyid, $time, 'advice', $text, $editorid);
                }
                if(db_query($sql, $DBValues)) drupal_set_message("the record is updated.");
                else drupal_set_message("failed to save changes.", 'error');
            }
        }elseif($do == 'delete')
        {
            if($dataid)
            {
                $sql = "delete from maenna_company_data where data_type = 'advice' and dataid = %d limit 1";
                if(db_query($sql, array($dataid))) drupal_set_message("the record is removed.");
                else drupal_set_message("failed to remove record.", 'error');
            }
        }
        
    }
    $content .= "<div class='align-right edit-link'><a href='/account?a=companies&type=company_detail&panel=advisory_advice_panel&view=add&companyid=$companyid'>add</a></div>";
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function relavent_info($op=null){
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $time = time();
    
    $Block['title'] = "Research and relevant information";
    $content = '';
    if($op == 'view' || $op == null)
    {
        
        $sql = "select * from maenna_company_data where data_type = 'relaventinfo' and companyid = %d order by access desc limit 3";
        $result = db_query($sql, array($companyid));
        while(($Row = db_fetch_object($result)) !== false)
        {
            $access = date('m/d/Y',$Row->access);
            $data_attr = $Row->data_attr;
            $title = htmlentities($Row->data_value,ENT_QUOTES);
            if($data_attr == 'link'){
                $link = $Row->data_value2;
                $content .= "\n<div class='row'><p>".$access . " - <a href='$link' target='_blank'>$title</a></p></div>";
            }else{
                $filename = $Row->data_value2;
                $destination = "./" . file_directory_path() . "/$filename";
                $content .= "\n<div class='row'><p>".$access . " - <a href='$destination' target='_blank'>$title</a></p></div>";
            }
        }
    }
    
    $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
    "<a href='/account?a=companies&type=company_detail&panel=relavent_info_panel&view=add&companyid=$companyid'>add</a>&nbsp;&nbsp;" .
                "<a href='/account?a=companies&type=company_detail&panel=relavent_info_panel&companyid=$companyid'>more</a></div>";
    $content .= "<div class='align-right edit-link'></div>";
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function relavent_info_panel($op=null){
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = 'Research and relevant information';
    $content = '';
    $time = time();
    
    if($op == null || $op == 'view')
    {
        $view = sget($_REQUEST, 'view');
        if(empty($view))
        {
            $sql = "select * from maenna_company_data where companyid = %d and data_type = 'relaventinfo' order by access desc";
            $result = db_query($sql, array($companyid));
            while(($Row = db_fetch_object($result)) !== false)
            {
                $title  =(htmlentities($Row->data_value,ENT_QUOTES));
                $type = $Row->data_attr;
                $value = $Row->data_value2;
                $dataid = $Row->dataid;
                if($type == 'link'){
                    $text = "<a href='$value' target='_blank'>$title</a>";
                }else{
                    $file = "./" . file_directory_path() . "/" . $value;
                    $text = "<a href='$file' target='_blank'>$title</a>";
                }
                $access = date('m/d/Y', $Row->access);
                $content .= "\n<div style='position:relative;border-bottom:solid 1px #ebebeb;'>" .
                                "<div style='position:absolute;height:20px;width:50px;top:8px;right:10px'>".
                                "\n<a href='/account?a=companies&type=company_detail&panel=relavent_info_panel&view=edit&dataid=$dataid&companyid=$companyid' class=edit-icon>edit</a>".
                                "\n<a href='/account?a=companies&type=company_detail&panel=relavent_info_panel&do=delete&update_section=relavent_info_panel&dataid=$dataid&companyid=$companyid' class='delete-icon' onclick='return confirm(\"Continue to remove the record\")'>delete</a>".
                                "</div>" .
                                "\n<label><b>$access</b></labe><p>$text</p></div>";
            }
        }elseif($view == 'add' || $view == 'edit'){
            
            $dataid = sget($_REQUEST, 'dataid');
            $linkTitle = $fileTitle = $value = '';
            if($dataid){
                $sql = "select * from maenna_company_data where dataid = %d and data_type = 'relaventinfo' limit 1";
                
                $result = db_query($sql, array($dataid)); $Row = db_fetch_object($result);
                if($Row) {
                    $type = $Row->data_attr;
                    if($type == 'link'){
                        $linkTitle = nl2br(htmlentities($Row->data_value,ENT_QUOTES));
                        $value = $Row->data_value2;
                    }else{
                        $fileTitle = nl2br(htmlentities($Row->data_value,ENT_QUOTES));
                    }
                }
            }
            $content = <<< END
<table style='width:100%;' border=0>
<tr>
    <td style='width:50%; border:none'>
        <form  method='post' action='/account' onsubmit='return check_input();' enctype='multipart/form-data'>
        <b>Add New File</b><br>
        <div class='form-row'><label>Title:</label><input type=text name=title value='$fileTitle' /></div>
        <div class='form-row'><label>File:</label> <input type=file name=file value='' /></div>
        <div class='form-row'><input type=submit name=submit value=submit class=button /></div>
        <input type=hidden name=datatype value=file />
        <input type=hidden name=a  value=companies >
        <input type=hidden name=type  value=company_detail >
        <input type=hidden name=panel  value=relavent_info_panel >
        <input type=hidden name=update_section  value='relavent_info_panel' >
        <input type=hidden name=companyid  value='$companyid'>
        <input type=hidden name=dataid  value='$dataid'>
        </form>
    </td>
    <td style=' border:none'>
        <form method='post' action='/account' onsubmit='return check_input();'>
        <b>Add A Web Link</b><br>
        <div class='form-row'><label>Title:</label> <input type=text name=title value='$linkTitle' /></div>
        <div class='form-row'><label>URL:</label> <input type=text name=link value='$value' /></div>
        <div class='form-row'><input type=submit name=submit value=submit class=button /></div>
        <input type=hidden name=datatype value=link />
        <input type=hidden name=a  value=companies >
        <input type=hidden name=type  value=company_detail >
        <input type=hidden name=panel  value=relavent_info_panel >
        <input type=hidden name=update_section  value='relavent_info_panel' >
        <input type=hidden name=companyid  value='$companyid'>
        <input type=hidden name=dataid  value='$dataid'>
        </form>
    </td>
</tr>
</table>
END;
    
        }
          
    }elseif($op == 'update')
    {
        $dataid = sget($_REQUEST, 'dataid');
        $datatype = sget($_REQUEST, 'datatype');
        $do = sget($_REQUEST, 'do');
        $correct = true;
    
        if(empty($do))
        {
            if($datatype)
            {
                if($dataid)
                {
                    $title = sget($_REQUEST, 'title');
                    if($datatype=='file'){
                        $data_attr = 'file';
                        if($_FILES['file']['error'] == 0)
                        {
                            $path = "./" . file_directory_path() . "/";
                            $ext =  get_filetype($_FILES['file']['name']);
                            $filename = ranStr() . "." . $ext;
                            $data_value2 = $filename;
                            if(! move_uploaded_file($_FILES['file']['tmp_name'], $path . $filename)){
                                drupal_set_message("failed to save file.", 'error');
                                $correct = false;
                            }else{
                                $sql = "update maenna_company_data set data_value = '%s', data_value2 = '%s',access='%s', editorid=%d where dataid = %d and data_type = 'relaventinfo' limit 1";
                                $DBValues = array($title, $data_value2, $time, $editorid, $dataid);
                            }
                        }else{
                            $sql = "update maenna_company_data set data_value = '%s', access='%s', editorid=%d where dataid = %d and data_type = 'relaventinfo' limit 1";
                    $DBValues = array($title, $time, $editorid, $dataid);
                        }
                    }else{
                        $data_attr = 'link';
                        $title = sget($_REQUEST, 'title');
                        $data_value2 = sget($_REQUEST,'link');
                        $sql = "update maenna_company_data set data_value = '%s', data_value2 = '%s',access='%s', editorid=%d where dataid = %d and data_type = 'relaventinfo' limit 1";
                        $DBValues = array($title, $data_value2, $time, $editorid, $dataid);
                    }
                    
                    
                }else{
                    $title = sget($_REQUEST, 'title');
                    if($datatype=='file'){
                        $data_attr = 'file';
                        if($_FILES['file']['error'] == 0)
                        {
                            $path = "./" . file_directory_path() . "/";
                            $ext =  get_filetype($_FILES['file']['name']);
                            $filename = ranStr() . "." . $ext;
                            $data_value2 = $filename;
                            if(! move_uploaded_file($_FILES['file']['tmp_name'], $path . $filename)){
                                drupal_set_message("failed to save file.", 'error');
                                $correct = false;
                            }
                        }else{
                            drupal_set_message("failed to upload file.", 'error');
                            $correct = false;
                        }
                    }else{
                        $data_attr = 'link';
                        $title = sget($_REQUEST, 'title');
                        $data_value2 = sget($_REQUEST,'link');
                    }
                    $sql = "insert into maenna_company_data (companyid, access, data_type, data_attr, data_value, data_value2, editorid) values(%d,'%s','%s','%s','%s','%s',%d)";
                    $DBValues = array($companyid, $time, 'relaventinfo', $data_attr, $title, $data_value2, $editorid);
                    
                }
                if($correct){
                    if(db_query($sql, $DBValues)) drupal_set_message("the record is updated.");
                    else drupal_set_message("failed to save changes.", 'error');
                }
                
            }
        }elseif($do == 'delete')
        {
            if($dataid)
            {
                $sql = "delete from maenna_company_data where data_type = 'relaventinfo' and dataid = %d limit 1";
                if(db_query($sql, array($dataid))) drupal_set_message("the record is removed.");
                else drupal_set_message("failed to remove record.", 'error');
            }
        }
        
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function maenna_company_name($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'companyid');
    $time = time();
   
    $sql = "select company, description from maenna_company where companyid = %d";
    $result = db_query($sql, array($companyid));
    $row = db_fetch_object($result);
    $company_name = strtoupper($row->company);
    $company_desc = nl2br(htmlentities($row->description, ENT_QUOTES));
    
    $Block['title'] = "<b>" . getProjectName($companyid) . " (" .  $company_name .")</b>";
    if($op == 'view' || $op == null){
        
        $content = "<div style='font:12px arial;line-height:1.5em' lines=3 trueH=0 sH=0 class='morediv'><b>About</b><br>$company_desc</div>";
        
        $content .= "</ul><div class='align-right more-link'><a href='#' class='edit_wnd' rel='edit_maenna_company_name' data='$companyid'>edit</a>";
        if(strlen($company_desc) > 300)
        {
            $content .= "&nbsp; <a href='#' class='morediv_link' closed=1>More >></a> ";
        }
        $content .= "</div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
       
        $company_name = sget($_POST,'company');
        $company_desc = sget($_POST,'desc');
        $sql = "update maenna_company set company = '%s', description = '%s',access='%s' where companyid = %d limit 1";
        $sql_data = array($company_name, $company_desc,$time, $companyid);
        if(db_query($sql, $sql_data)) drupal_set_message("Company information is updated.");
        else drupal_set_message("Failed to update company information.",'error');
    }
    return $Block;
}

function maenna_company_links($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid  = sget($_REQUEST,'companyid');
    $time = time();
    $correct = true;
    $Block['title'] = 'Links';
    if($op == 'view' || $op == null){
        $content = "<ul>";
        $sql = "select * from maenna_company_data where companyid = %d and data_type = 'links' order by dataid";
       
        $result = db_query($sql, array($companyid));
        $counter = 1;
        while(($data = db_fetch_object($result)) !== false)
        {
            $link = $data->data_value2;
            if(! $link ) continue;
            $content .= "<li><a href='$link' target='_blank'>" . check_plain($data->data_value) . "</a></li>";
            $counter++;
        }
        if($content == ''){$content = '<li>n/a</li>';}
        
        $content .= "</ul><div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_maenna_company_links' data='$companyid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        for($i = 1; $i <= 5; $i++)
        {
            $title = sget($_POST, "name_$i");
            $url = sget($_POST, "link_$i");
            $dataid = sget($_POST,"dataid_$i");
            if($dataid)
            {
                $sql = "update maenna_company_data set data_value = '%s', data_value2 = '%s', access='%s', editorid='%s' where dataid = %d limit 1 ";
                $sql_data = array($title, $url, $time, $editorid, $dataid);
            }else{
                $sql = "insert into maenna_company_data (companyid, access, data_type, data_value, data_value2, editorid) values('%d','%s','%s','%s','%s','%d')";
                $sql_data = array($companyid, $time, 'links', $title, $url, $editorid);
            }
            if( ! db_query($sql, $sql_data)){
                drupal_set_message("Error: failed to update company links.",'error');
                $correct = false;
                break;
            }
        }
        if($correct) drupal_set_message("Company  information is updated.");
    }
    return $Block;
}
function maenna_company_financials($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'companyid');
    $time = time();
    $Block['title'] = 'Company Financials';
    if($op == 'view' || $op == null){
        
        /*include_once '/var/www/vhosts/clewed.com/httpdocs/php-ofc-library/open_flash_chart_object.php';
        $content = "<div style='position:relative;z-index:2;' id='financial_chart'>";
        $content .= open_flash_chart_object( 500, 250, 'http://'. $_SERVER['SERVER_NAME'] .'/open_chart?companyid=' . $uid , false );
        $content .= "</div>";
        */
        $sql = "select * from maenna_company_data where companyid = %d and data_type = 'financial' order by data_attr desc";
        $result = db_query($sql, array($companyid));
        $Data = array();
        $content = "<div><table cellspacing=0 cellpadding=0><tr><td></td><td></td><td></td><td></td><td></td><td></td></tr>";
        if(db_affected_rows($result) > 0)
        {
            while(($row = db_fetch_object($result) ) !== false)
            {
                $year = $row->data_attr;
                $Data["$year"] = $row;
            }
            $row1 = "<tr><td width=100></td>";
            $row2 = "<tr><td>Revenues</td>";
            $row3 = "<tr><td>After-Tax Earnings</td>";
            $thisYear = date("Y");
            $Years = array();
            for($i = $thisYear; $i >= $thisYear - 4; $i--)
            {
                $Years[] = $i;
            }
            if(count($Data) > 0)
            {
                foreach($Years as $year)
                {
                    $row1 .= "\n<td align=right>$year</td>";
                    $revenue = '';
                    $earnings = '';
                    if(isset($Data["$year"])){
                        $row = $Data["$year"];
                        if($row->data_value)$revenue = "\$" .number_format($row->data_value);
                        if($row->data_value2)$earnings ="\$" . number_format($row->data_value2);
                    }
                    $row2 .= "<td align=right>$revenue</td>";
                    $row3 .= "<td align=right>$earnings</td>";
                }
            }
            $row1 .= "</tr>";
            $row2 .= "</tr>";
            $row3 .= "</tr>";
        }
        $content .= "\n $row1 \n $row2 \n $row3 \n</table></div>";
        
        $content .= "<div class='more-link'><a href='#' class='edit_wnd' rel='edit_maenna_company_financial' data='$companyid'>edit</a></div>";
        $Block['body'] = content_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        
        $thisYear = date("Y");
        $Years = array();
        for($i = 0; $i < 5 ; $i++)
        {
            $theYear = $thisYear - $i;
            $revenue = sget($_POST,'revenue_'. $theYear,'decimal');
            $earnings = sget($_POST,'earnings_'. $theYear,'decimal');
            $equity = sget($_POST,'equity_'. $theYear,'decimal');
            $dataid = (int)sget($_POST,'dataid_'.$theYear);
            if($dataid > 0){
                $sql = "update maenna_company_data set data_value = '%s', data_value2 = '%s',data_value3= '%s',access='%s',editorid='%d' where dataid = %d and data_type = 'financial' and data_attr = '%s'";
                $sql_data = array($revenue, $earnings,$equity, $time, $editorid, $dataid, $theYear);
            }else{
                $sql = "insert into maenna_company_data (companyid, data_type, data_attr, data_value, data_value2,data_value3,access,editorid) values (%d, 'financial', '%s', '%s','%s','%s', '%s','%s')";
                $sql_data = array($companyid, $theYear, $revenue, $earnings,$equity, $time, $editorid);
            }
           
            if(db_query($sql, $sql_data) === false)
            {
                //echo $sql,
                //test_array($sql_data);
                //echo mysql_error();
                drupal_set_message("Faild to update company finacial information. Please try again later",'error');
                
            }
        }
        drupal_set_message("Your company financial information is updated");
    }
    return $Block;
}

function maenna_progress_status($op = null)
{
    global $user;
    $editorid = $user->uid;
    $companyid = sget($_REQUEST,'companyid');
    $time = time();

    $Block['title'] = 'Progress Status';
    if($op == 'view' || $op == null){
        $content = "<div><table cellspacing=0 cellpadding=0><tr><td width=70>Date</td><td width=100>Contact Name</td><td>Message</td><td width=100 align=right>Follow-Up Date</td></tr>";
        $sql = "select * from maenna_company_data  where companyid = %d and data_type = 'progress' order by dataid desc ";
        $result = db_query($sql, array($companyid));
        $counter = 1;
        
        while(($row = db_fetch_object($result)) !== false)
        {
            $date = '';
            $followup = '';
            if($row->data_attr) $followup = date('m/d/Y', $row->data_attr);
            if($row->data_value3) $date = date('m/d/Y', $row->data_value3);
           
            $contact = $row->data_value;
            $message = htmlentities( $row->data_value2,ENT_QUOTES);
            
            $content .= "<tr><td>$date</td><td>$contact</td><td>$message</td><td align=right>$followup</td></tr>\n";
        }
        $content .= "</table></div>";
        
        $content .= "</ul><div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_maenna_company_progress' data='$companyid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        $date = sget($_REQUEST,'date');
        if($date ) $date = strtotime($date);
        $contact = sget($_REQUEST,'contact');
        $message = sget($_REQUEST, 'message');
        $followup = sget($_REQUEST,'followup');
        if($followup)$followup = strtotime($followup);
        $sql = "insert into maenna_company_data (companyid, editorid, access, data_type, data_attr, data_value, data_value2, data_value3) values('%d','%d','%s','%s','%s','%s','%s','%s')";
        $sql_data = array($companyid, $editorid, $time, 'progress', $followup, $contact, $message,$date);
        if(db_query($sql, $sql_data) === false)
        {
                //echo $sql,
                //test_array($sql_data);
                //echo mysql_error();
                drupal_set_message("Faild to update progress status. Please try again later",'error');
                
        }else
        drupal_set_message("Progress status is updated");
    }
    return $Block;
}

function maenna_links_group($op = null)
{
    $companyid = sget($_REQUEST, 'companyid');

    $Block['title'] =  'Request a professional'; 
    if($op == 'view' || $op == '')
    {
        $Block['body']  =  "<div class='pagelink'><a href='#' class='edit_wnd' rel='edit_maenna_company_financial' data='$companyid'>Ask a question</a></div>" .
                           "<div class='pagelink'><a href='#' class='edit_wnd' rel='edit_maenna_company_financial' data='$companyid'>Invite Collaborator</a></div>" .
                           "<div class='pagelink'><a href='#' class='' rel='edit_maenna_company_financial' data='$companyid'>Follow this company</a></div>" .
                           "<div class='pagelink'><a href='#' class='edit_wnd' rel='edit_maenna_company_financial' data='$companyid'>Join Relationship Team</a></div>" ;
    }
    return $Block;
}


function maenna_followup_priority($op = null)
{
    $companyid = sget($_REQUEST,'companyid');
    $Block['title'] = "Follow-Up Priorities";
    global $user;
    $editorid = $user->uid;
    $time = time();
    
    if($op == null || $op == 'view')
    {
        
        $sql = "select * from maenna_company_data where data_type = 'followup' and companyid = %d order by data_attr asc limit 6";
        $result = db_query($sql, array($companyid));
        $content = "\n<table><tr><td></td><td></td></tr>";
        if(db_affected_rows($result) > 0)
        {
            while(($row = db_fetch_object($result)) !== false)
            {
                
                $date = date('m/d/Y', $row->data_attr);
                $note = htmlentities($row->data_value);
                $content .= "<tr><td colspan=2>$date<br>$note    </td></tr>\n";
            }
        }
        

        $content .= "</table>";
        $content .= "\n<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_maenna_followup_priority' data='$companyid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        
        $companyid = sget($_REQUEST,'companyid');
        $note = sget($_REQUEST,'note');
        $date = sget($_REQUEST,'date');
        $date_stamp = strtotime($date);
        
            $sql = "insert into maenna_company_data(companyid, data_type, data_attr, data_value, access, editorid) values('%d','%s','%s','%s','%s','%d') ";
            $sql_data = array($companyid, 'followup', $date_stamp, $note, $time, $editorid);
        
        if(db_query($sql, $sql_data))
        {
            drupal_set_message("Prospect setting updated");
        }else{
            drupal_set_message("Error: failed to update prospect setting",'error');
        }
        return;
    }
    return $Block;
}

function maenna_companies_to_follow($op = null)
{
    $companyid = sget($_REQUEST,'companyid');
    $Block['title'] = "Companies To Follow";
    global $user;
    $editorid = $user->uid;
    $time = time();
    $content = 'n/a';
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function maenna_company_articles($op = null)
{
    $companyid = sget($_REQUEST,'companyid');
    $Block['title'] = "Articles";
    global $user;
    $editorid = $user->uid;
    $time = time();
    
    if($op == null || $op == 'view')
    {
        
        $sql = "select * from maenna_company_data where data_type = 'article' and companyid = %d order by dataid asc limit 6";
        $result = db_query($sql, array($companyid));
        $content = "\n<table><tr><td></td><td></td></tr>";
        if(db_affected_rows($result) > 0)
        {
            while(($row = db_fetch_object($result)) !== false)
            {
                
                $data_attr = $row->data_attr;
                if($data_attr == 'webpage')
                $content .= "<tr><td><a href='".$row->data_value2."' target='_blank'>" . htmlentities($row->data_value) ."</a></td></tr>\n";
                else{
                    $src = "/" . file_directory_path() . "/" . $row->data_value;
                    $content .= "<tr><td><a href='$src' target='_blank'>" . htmlentities($row->data_value) ."</a></td></tr>\n";
                }
            }
        }
        

        $content .= "</table>";
        $content .= "\n<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_maenna_company_article' data='$companyid'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }elseif($op == 'update')
    {
        $title = sget($_REQUEST,'title');
        $url = sget($_REQUEST,'url');
        $article_type = '';
        $data_type = 'article';
        
        if( (! empty($title ))  && (! empty($url)))
        {
            $data_attr = "webpage";
            $sql = "insert into maenna_company_data (companyid, data_type, data_attr, data_value, data_value2, access, editorid)
                                                        values('%d','%s','%s','%s','%s','%s','%d')";
            $sql_array = array($companyid, $data_type, $data_attr, $title, $url, $time, $editorid);
        }
        if(isset($_FILES['file']) && $_FILES['error'] == 0)
        {
                $path = "./" . file_directory_path() . "/";
                $filename = "";
                if($ext = get_filetype($_FILES["file"]["name"]))
                {
                    $filename = ranStr() . "." . $ext;
                    if(move_uploaded_file($_FILES["file"]["tmp_name"], $path . $filename))
                    {
                        drupal_set_message("A new file has been uploaded.");
                        $recordid = sget($_REQUEST,'companyid');
                        $sql = "select * from maenna_company_data where companyid = %d and data_type = 'logo'";
                        $result = db_query($sql, array($companyid));
                        $count = db_affected_rows($result);
                        $editorid = $user->uid;
                        $time = time();
                        
                            $sql = "insert into maenna_company_data (companyid, data_type, data_value, access, editorid) values (%d, 'article', '%s', '%s', %d)";
                            $sql_array = array($companyid, $filename, $time, $editorid);
                            
                        
                    }else{
                        drupal_set_message("Failed to upload new file",'error');
                    }
                }else{
                    drupal_set_message("The file type is not allowed",'error');
                }
        }
        
        
        if(db_query($sql, $sql_array))
        {
            drupal_set_message("A new article record is added");
        }else{
            drupal_set_message("Error: failed to update article record",'error');
        }
        return;
    }
    return $Block;
    
}
function view_company_market($op = null)
{
    $Block['title'] = 'Existing Market';
    $company_id = (int)sget($_REQUEST, 'id');
    if($company_id <= 0){
        $Block['body'] = sidebar_box($Block['title'], "");
        return $Block;
    }
    if($op == 'view' || $op == null){
        $content = "";
        $sql = "select * from users_data where uid = %d and data_type = 'marketinfo' and data_attr = 'market' order by data_id";
        $result = db_query($sql, array($company_id));
        $counter = 1;
        while(($data = db_fetch_object($result)) !== false)
        {
            $content .= "${counter}. " . check_plain($data->data_value2) . "<br>";
        }
        if($content == ''){$content = 'n/a';}
        //$content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_company_market'>edit markets</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}
function view_company_milestone($op = null)
{
    $Block['title'] = 'Milestones';
    $company_id = (int)sget($_REQUEST, 'id');
    if($company_id <= 0){
        $Block['body'] = sidebar_box($Block['title'], "");
        return $Block;
    }
    if($op == 'view' || $op == null){
        $content = "";
        $sql = "select * from users_data where uid = %d and data_type = 'milestone' order by data_id";
        $result =  db_query($sql, array($company_id));
        $counter = 1;
        while(($data = db_fetch_object($result)) !== false)
        {
            $milestone = check_plain($data->data_value2);
            $content .= "<div style='width:100%;height:40px;overflow:hidden'>${counter}. " . $milestone . "</div>";
            $counter++;
        }
        if($content == '')$content = 'n/a';
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}
function view_company_name($op = null)
{
    $Block['title'] = ("Company Name");
    $company_id = (int)sget($_REQUEST, 'id');
    if($company_id <= 0){
        $Block['body'] = sidebar_box($Block['title'], "");
        return $Block;
    }
    $sql = "select ta.mail,ta.created, concat(tb.first_name ,', ',tb.last_name) as username,tb.company_name,tb.description from users_extend as tb, users as ta where tb.uid = %d and ta.uid = %d limit 1";
    $result = db_query($sql, array($company_id, $company_id));
    $data = db_fetch_object($result);
    $company_name = ucwords($data->company_name);
    $Block['title'] = check_plain($company_name);
    $desc = ($data->description) ? $data->description : "n/a";
    $name = ($data->username) ? $data->username : "n/a";
    $mail = ($data->mail) ? $data->mail : "n/a";
    $created = ($data->created) ? date("m/d/Y",$data->created) : "n/a";
    if($op == 'view' || $op == null)
    {
        $content .= "<b>User Name: </b>" . check_plain($name);
        $content .= "<br><b>Email: </b>" . check_plain($mail);
        $content .= "<br><b>Account created on: </b>" . $created;
        $content .= "<h6>Company Description</h6>" . check_plain($desc);
        //$content .= "<div style='text-align:right'><a href='#' class='edit_wnd' rel='edit_company_description'>edit company description</a></div>";
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function view_company_financial( $op = null )
{
    $Block['title'] = ("Company Financial");
    $company_id = (int)sget($_REQUEST, 'id');
    if($company_id <= 0){
        $Block['body'] = sidebar_box($Block['title'], "");
        return $Block;
    }
    if($op == 'view' || $op == null){
        $content = "";
        //$content .= "<div style='text-align:right'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function view_company_market_info($op = null)
{

    $Block['title'] = 'Market Information';
    $company_id = (int)sget($_REQUEST, 'id');
    if($company_id <= 0){
        $Block['body'] = sidebar_box($Block['title'], "");
        return $Block;
    }
    $array = array("competitor","market","explansion");
    if($op == 'view' || $op == null){
        $content = "";
        
        $sql = "select * from users_data where uid = %d and data_type = 'marketinfo' order by data_id";
        $result = db_query($sql, array($company_id));
        
        $c = ''; $m = ''; $e = '';
        
        for($i = 1; $i <=3 ; $i++)
        { 
            if(($data = db_fetch_object($result)) !== false)
            {
                $c .= check_plain($data->data_value2) . "<br>"; 
            }
        }
        if($c == '') $c = 'n/a <br>';
        for($i = 1; $i <=3 ; $i++)
        { 
            if(($data = db_fetch_object($result)) !== false)
            {
                $m .= check_plain($data->data_value2) . "<br>"; 
            }
        }
        if($m == '') $m = 'n/a <br>';
        for($i = 1; $i <=3 ; $i++)
        { 
            if(($data = db_fetch_object($result)) !== false)
            {
                $e .= check_plain($data->data_value2) . "<br>"; 
            }
        }
        if($e == '') $e = 'n/a <br>';
        $content .= "<b>Top three competitors</b><br>" .
                    "$c" .
                    "<b>Top three markets you currently sell in</b><br>" .
                    "$m" .
                    "<b>Top three priority expansion markets</b><br>" .
                    "$e" ;
        
            
       
        
       // $content .= "<div class='align-right edit-link'><a href='#' class='edit_wnd' rel='edit_company_market_info'>edit</a></div>";
        
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function view_company_analyst($op = null)
{
    $Block['title'] = ("Company Analyst");
    $company_id = (int)sget($_REQUEST, 'id');
    if($company_id <= 0){
        $Block['body'] = sidebar_box($Block['title'], "");
        return $Block;
    }
    if($op == 'view' || $op == null){
        $content = "";
        //$content .= "<div style='text-align:right'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function view_company_follower($op = null)
{
    $Block['title'] = ("Followers");
    $company_id = (int)sget($_REQUEST, 'id');
    if($company_id <= 0){
        $Block['body'] = sidebar_box($Block['title'], "");
        return $Block;
    }
    if($op == 'view' || $op == null){
        $content = "followers";
        //$content .= "<div style='text-align:right'><a href='#' class='edit_wnd' rel='edit_company_financial'>edit</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}


/* EOF */