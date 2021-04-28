<?php
$maenna_page['right'] = array('');

$section_tab = sget($_REQUEST, 'tab');

switch($section_tab)
{
    case 'questionnaire':
        $maenna_page['right'] = array('submit_financials','submit_otherfiles');
        break;
    case 'analysis':
        $maenna_page['right'] = array('files');
        break;
    case 'plan':
        $maenna_page['right'] = array('files');
        break;
    case 'monitoring':
        $maenna_page['right'] = array(
                                      'admin_tools',
                                      'admin_section',
                                      'company_resources',
                                      'upload_photo',
                                      'competitive_advantage',
                                      'earning_source',
                                      'priority',
                                      'metrics',
                                      'watch_list',
                                      'files',
                                      );
        break;
    case 'minutes':
        $maenna_page['right'] = array('files');
        break;
    default:
        $maenna_page['right'] = array('submit_financials','submit_otherfiles');
        break;
}


function questionnaire_fileuploader($op = null)
{
    global $user;
    $companyid = $user->uid;

    $Block['title'] = '';
    $content = '';

    if($op == null || $op == 'view'){
        $sql = "select * from maenna_questionair where parentid is null and target = 'company' and status = 1";
        $result = db_query($sql);
        $Row = db_fetch_object($result);
        if($Row)
        {

            $Block['title'] = 'UPLOAD FILES';//htmlentities(ucwords($Row->content), ENT_QUOTES) . " - upload files";
            $parentid = $Row->recordid;

            $sql = "select Q.*, A.answer from maenna_questionair as Q
                        left join maenna_qa as A on
                            Q.recordid = A.qid and
                            A.uid = %d
                        where Q.parentid = %d and type='fileuploader' order by weight, recordid";
            $result = db_query($sql, array($companyid,$parentid));
            $counter = 1;
            while(($Row = db_fetch_object($result)) )
            {
                $questionid=$Row->recordid;
                $title = $counter . ". " . htmlentities(ucwords($Row->content), ENT_QUOTES) . "";
                $file = $Row->answer;
                if($file){
                    $path =  "./" . file_directory_path() . "/" . $file;
                    $file = preg_replace("/^[0-9]*_/i", '', $file);
                    $filetitle = $file;
                    $filename = $file;
                    if(strlen($filetitle) > 25) $filetitle = substr($file, 0, 25) . "...";
                    $file = "(<a href='$path' target='_blank' class='blue'>$filetitle</a>)";
                }
                $content .= "\n<div class=row style='padding:7px 0'>$title<br />
                                <div style='margin-left:10px;margin-top:5px;'>
                                    <input type=file name='f_$questionid' style='width:100px;' /><br />
                                    <div style='margin-top:5px;width:200px;overflow:hidden;' title='$filename'>$file</div>
                                </div>
                            </div>";
                $counter++;
            }

            $content = <<< END
                <script src="/themes/maennaco/jquery.filestyle.js" type="text/javascript"></script>
                <form method='post' action='/account' enctype='multipart/form-data'  onsubmit="return confirm('Please make sure your have saved your answers before continue to upload files.'+'\\n'+ 'Select OK to upload files, or CANCAL to return to the questionnaire.')">

                    $content
                    <div style='margin:10px;'><input type=submit name=submit value='Upload' /></div>
                    <input type=hidden name=a value=questionnaire />
                    <input type=hidden name=parentid value='$parentid' />
                    <input type=hidden name=update_section value='questionnaire' />
                    <input type='hidden' name='page' value='$next' />
                </form>
                <script type="text/javascript">
                jQuery(document).ready(function($){
                    jQuery("input[type=file]").filestyle({
                        image: "/themes/maennaco/choose-file.gif",
                        imageheight : 22,
                        imagewidth : 82,
                        width : 80
                    });
                });
                </script>
END;
        }
    }

    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

function financials($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $companyid = $user->uid;
    $Block['title'] = "FILES";
    $tab = sget($_REQUEST,'tab');
    $panel = "editor_panel";
    $data_type = 'financials';
    $redirect = rebuild_url(array('tab'));
    ///
    $content = '';
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type'";
        $result = db_query($sql);
        $Row = db_fetch_object($result);
        if($Row)
        {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = sideBarExcerpt($content, $redirect);

        }
        if ($write) {
          $Block['title'] .= "<div class='editbtn'><a href='${redirect}&panel=${panel}&view=edit&data_type=${data_type}' class='tool'>edit</a></div>";
        }
    }
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}
function files($op)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $companyid = $user->uid;
    $time = time();
    $Block['title'] = "FILES";
    $tab = sget($_REQUEST,'tab');
    $data_type = sget($_REQUEST, 'tab') . "_files";
    $redirect = rebuild_url(array('tab'));
    $content = '';
    $HPV = hidden_post_values(array('tab'));

    if ($write) {
      $Block['title'] .= "<div class='editbtn'><a href='#' class='tool openbox' boxid='box1'>Edit</a></div>";
    }
    if(!$write || $op == null || $op == 'view')
    {
        if ($write) {
          $content = "<div id='box1' style='display:none'>
                        <form method=post action='/account' enctype='multipart/form-data'>
                            <div class=row>Name:<br><input type=text name=name value='' /></div>
                            <div class=row>File: <input type=file name='file' /></div>
                            <div class=row><input type=submit name=submit value='submit'  class=button /> &nbsp;
                                    <a href='#' class='hidebox button' boxid=box1>Close</a>
                            </div>
                            $HPV
                            <input type='hidden' name=update_section value='files' />
                            <input type='hidden' name=do value='new' />
                        </form>
                    </div>";
          $content .= js_init("init_openbox();init_hidebox();");
        }

        // list files
        $sql = "select * from maenna_company_data where data_type = '%s' and companyid = %d and deleted != 1 order by dataid desc";
        $result = db_query($sql, array($data_type, $companyid));
        while($Row = db_fetch_object($result))
        {
            $name = htmlentities(strtoupper($Row->data_value),ENT_QUOTES);
            $filename = $Row->data_value2;
            $dataid = $Row->dataid;
            $path =  "./" . file_directory_path() . "/" . $filename;
            $content .= "\n<div class=row style='width:100%;overflow:hidden;'>
                        <a href='$path' target='_blank'>$name</a>";
            if ($write) {
              $content .= "
                            <div class='entry_tool'><a href='$redirect&update_section=files&do=del&dataid=$dataid'
                                                    onclick='return confirm(\"Continue to delete this file?\")'>del</a>
                            </div>";
            }
            $content .= "
                        </div>";
        }

    }elseif($op == 'update')
    {
        $do = sget($_REQUEST, 'do');
        $dataid = sget($_REQUEST, 'dataid');
        if($do == 'new')
        {
            $name = sget($_REQUEST, 'name');

            if((! empty($name )) && ($_FILES['file']['error'] == 0))
            {
                $File = upload_file($_FILES['file']);
                if( $File)
                {
                    $filename= $File['name'];
                    $type = $File['type'];
                    $sql = "insert into maenna_company_data (companyid, access, data_type,data_value, data_value2, data_value3, editorid )
                                                        values(%d,'%s','%s','%s','%s','%s',%d)";
                    $DBValues = array($companyid, $time, $data_type, $name, $filename, $type, $editorid);
                    if(db_query($sql, $DBValues)) drupal_set_message("File is added");
                    else{
                        drupal_set_message("Failed to upload file. Please try again later", 'error');
                    }

                }
                return;
            }

            drupal_set_message("Please complete the form and try again", 'error');

        }elseif($do == 'del')
        {
            $dataid = sget($_REQUEST, 'dataid');
            if($dataid)
            {
                $sql = "update maenna_company_data set deleted = 1 where companyid = %d and dataid = %d limit 1";
                if(db_query($sql, array($companyid, $dataid))) drupal_set_message("The file is deleted");
                else drupal_set_message("Failed to delete the file", 'error');
            }
        }
        return;
    }


    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}
function admin_tools($op = null)
{
    $Block['title'] = '';
    $content = "
        <div class=row><a href=''>clewed Note</a></div>
        <div class=row><a href=''>Progress</a></div>
        <div class=row><a href=''>Request Professionals</a></div>
        <div class=row><a href=''>Suggest Related Companies</a></div>
    ";
    $Block['body'] = sidebar_box('', $content);
    return $Block;
}

function competitive_advantage($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $companyid = $user->uid;
    $time = time();
    $Block['title'] = "DURABLE COMPETITIVE ADVANTAGE";
    $tab = sget($_REQUEST,'tab');
    $data_type = 'advantage';
    $redirect = rebuild_url(array('tab'));
    $content = '';
    $panel = 'single';
    if ($write) {
      $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit&key=${data_type}' class=tool>Edit</a></div>";
    }
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' ";
        $result = db_query($sql, array($companyid, $data_type));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = sideBarExcerpt($content, $redirect . "&panel=${panel}&view=detail&key=${data_type}");
        }
    }else{

    }

    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}


function earning_source($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $companyid = $user->uid;
    $time = time();
    $Block['title'] = "SOURCE OF EARNINGS<br>GROWTH";
    $tab = sget($_REQUEST,'tab');
    $data_type = 'earningsource';
    $redirect = rebuild_url(array('tab'));
    $content = '';
    $panel = 'single';
    if ($write) {
      $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit&key=${data_type}' class=tool>Edit</a></div>";
    }
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' ";
        $result = db_query($sql, array($companyid, $data_type));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $content = htmlentities($Row->data_value2, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $content = sideBarExcerpt($content, $redirect . "&panel=${panel}&view=detail&key=${data_type}");
        }
    }else{

    }

    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}
function single($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $companyid = $user->uid;
    $time = time();

    $panel = "single";
    $data_type = sget($_REQUEST, 'key');
    $redirect = rebuild_url(array('tab','key'));
    $dataid = '';
    $content = '';

    switch($data_type)
    {
        case "advantage":
            $title = "DURABLE COMPETITIVE ADVANTAGE";
            break;
        case "earningsource":
            $title = "SOURCE OF EARNINGS GROWTH";
            break;
    }
    $Block['title'] = $title;

    if(!$write || $op == null || $op == 'view'){
        $view = sget($_REQUEST, 'view');
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type'";
        $result = db_query($sql, array($companyid));
        $Row = db_fetch_object($result);
        if($Row){
            $content = $Row->data_value2;
            $dataid = $Row->dataid;
        }
        if($write && $view == 'edit'){
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
            $content .= hidden_post_values(array('tab','panel','key'));
            $content .= "</form>";
        }elseif($view == 'detail'){
            $content = nl2br(htmlentities($content, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
            $content = "<div class=entry>
                            <div class=entry-content>$content</div>
                        </div>
                        <div class=backbtn><a href='$redirect' class='button'>back</a></div>";
        }
    }elseif($op == 'update'){

        $data_value2 = sget($_REQUEST, $data_type);
        $companyid = $user->uid;
        $dataid = sget($_REQUEST, 'dataid');

        if(empty($companyid)) drupal_set_message("Invalid company id",'error');
        else{
            if($dataid){
                $sql = "update maenna_company_data set data_value2 = '%s', access='$time',editorid=$editorid where data_type = '$data_type' and dataid = %d limit 1";
                $DBValues = array($data_value2, $dataid);
            }else{
                $sql = "insert into maenna_company_data (companyid, access, data_type, data_value2,editorid ) values (%d, '%s','%s','%s',%d)";
                $DBValues = array($companyid, $time, $data_type, $data_value2, $editorid);
            }
            if(db_query($sql, $DBValues))drupal_set_message("Data record is updated");
            else drupal_set_message("Failed to update record", 'error');
        }
        return ;
    }
     $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}




function priority($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $time = time();
    $companyid = $user->uid;
    $Block['title'] = "PRIORITIES";
    $tab = sget($_REQUEST,'tab');
    $panel = "multi";
    $data_type = 'priority';
    $redirect = rebuild_url(array('tab'));
    $content = '';

    if($op == 'view' || $op == null){
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '${data_type}' and deleted != 1 order by dataid desc limit 3";
        $result = db_query($sql, array($companyid));
        $counter = 0;
        while(($Row = db_fetch_object($result)) !== false)
        {
            $counter++;

            $title = strtoupper($Row->data_value);
            if($title)
            $title = htmlentities($title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            else $title = "&nbsp;";

            //$title =    htmlentities($Row->data_value,ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $text =     htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid&datatype=${data_type}";
            $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid&datatype=${data_type}";
            //$text = sideBarExcerpt($text, $more_link);
            $text = sideBarExcerpt($text);
            $last = '';


            $content .= "<div class='entry'>
                            <div class=title>$title

                            </div>
                        <div class=content>$text</div></div>";
                        //<div class=editbtn><a href='$edit_link' class=tool>Edit</a></div>
        }

        $viewall_link = $redirect . "&panel=${panel}&view=listview&datatype=${data_type}";
        if ($write) {
          $add_link = $redirect . "&panel=${panel}&view=add&datatype=${data_type}";
          $Block['title'] .= "<div class=editbtn>
                                    <a href='$add_link' class=tool>Add</a>
                            </div>";
        }
        $content .= "\n<div class='viewallbtn'><a href='$viewall_link' class=tool>View ALL</a></div>";
    }


    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}
function multi($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $time = time();
    $companyid = $user->uid;
    $tab = sget($_REQUEST,'tab');
    $panel = "multi";
    $data_type = sget($_REQUEST, 'datatype');
    $redirect = rebuild_url(array('tab', 'datatype'));
    $content = '';
    switch($tab)
    {
        case "priority":
            $title = "PRIORITIES";
            break;
        case "minutes":
            $title = "MINUTES";
            break;
        case "analysis":
            $title = "ANALYSIS";
            break;
        case "plan":
            $title = "Plan";
            break;
    }
    $Block['title'] = $title;
    if ($write) {
      $add_link = $redirect . "&panel=${panel}&view=add&datatype=${data_type}";
      $Block['title'] .= "<div class=editbtn>
                                    <a href='$add_link' class=tool>Add</a>
                            </div>";
    }

    if(!$write || $op == null || $op == 'view')
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
                if(!$write || $view == 'detail'){
                    $dataid = sget($Row, 'dataid');
                    $sub_title = strtoupper(sget($Row,'data_value'));
                    $sub_title = htmlentities($sub_title, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $text = sget($Row, 'data_value2');
                    $text = nl2br(htmlentities($text, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                    $content = "<div class=entry>
                                    <div class=entry-title>$sub_title</div>
                                    <div class=entry-content>$text</div>
                                </div>";
                    if ($write) {
                      $rem_link = $redirect . "&panel=${panel}&view=listview&update_section=${panel}&do=remove&dataid=$dataid";
                      $content .= "<div class=backbtn>
                                    <div class='rembtn'><a href='$rem_link' class=button onclick='return confirm(\"Continue to remove record\")'>Delete</a></div>
                                    <a href='$redirect&panel=${panel}&view=listview' class=button>back</a>
                                </div>";
                      $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
                      $Block['title'] .= "<div class=editbtn>
                                            <a href='$edit_link' class=tool>Edit</a></div>";
                    }
                }elseif($view == 'edit' || $view == 'add')
                {
                    $sub_title = strtoupper(sget($Row,'data_value'));
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
                    $hv = hidden_post_values(array('tab','panel','datatype'));
                    $content .= <<< END
                    <form action='/account' method='post' onsubmit='return check_input();'>
                        <div class=entry>
                            <div class=entry-content>
                                <div>Title:<br /><input type=text name=title value='$sub_title' class='require_string' style='width:400px;' /></div><br />
                                <div>Content:<br /><textarea name='content' style='width:99%;height:400px;' class='require_string' >$text</textarea>
                                <div class=backbtn>
                                    <div class='rembtn'><a href='$rem_link' class=button onclick='return confirm("Continue to remove record")'>Delete</a></div>
                                    <input type=submit name=submit value=submit class=button />
                                    &nbsp;&nbsp;
                                    <a href='$redirect&panel=${panel}&view=listview' class=button>Cancel</a>
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
            $limit = 5;
            $page = sget($_REQUEST,'page');
            if(empty($page)) $page = 1;
            $start = ($page - 1) * $limit;

            $sql = "select count(*) as total from maenna_company_data where data_type = '$data_type' and companyid = %d and deleted != 1";
            $result = db_query($sql, array($companyid));
            $Row = db_fetch_object($result);
            $total = $Row->total;

            $sql = "select * from maenna_company_data where data_type = '$data_type' and companyid = %d and deleted != 1 order by dataid desc limit $start, $limit";
            $result = db_query($sql, array($companyid));
            while(($Row = db_fetch_object($result)) !== false)
            {
                $access = date('m/d/Y',$Row->access);
                $sub_title = htmlentities($Row->data_value, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                $dataid = $Row->dataid;
                $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
                $text = contentExcerpt($text, $more_link);
                $content .= "<div class=entry>";
                if ($write) {
                  $content .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid' class='tool' >edit</a></div>";
                }
                $content .= "<div class='entry-title'>$sub_title</div><div class='entry-content'>" . $text ."</div></div><hr class='line' />";
            }
            $Pagenation = array('total' => $total,
                'limit' =>5,
                'baseurl' => $redirect,
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

            if(empty($sub_title) || empty($text))
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
}

function metrics($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $companyid = $user->uid;
    $time = time();
    $Block['title'] = "METRICS";
    $tab = sget($_REQUEST,'tab');
    $data_type = 'metrics';
    $panel = "columns";
    $redirect = rebuild_url(array('tab'));
    $content = '';
    $HPV = hidden_post_values(array('tab'));

    if ($write) {
      $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&datatype=${data_type}&view=edit' class=tool>Edit</a></div>";
    }
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and deleted != 1 order by data_attr, dataid ";
        $result = db_query($sql, array($companyid, $data_type));
        while($Row = db_fetch_object($result))
        {
            $title = $Row->data_value;
            if($title)
            $title = htmlentities($title,ENT_QUOTES);
            else $title = "&nbsp;";

            $col2 = htmlentities($Row->data_value2,ENT_QUOTES);
            $col3 = htmlentities($Row->data_value3,ENT_QUOTES);
            $content .= "<div class=row>
                <div style='overflow:hidden;width:110px;'>$title</div>
                <div style='width:20%;position:absolute;top:0;left:120px;overflow:hidden;padding:inherit;'>$col2</div>
                <div style='width:20%;position:absolute;top:0;left:180px;overflow:hidden;padding:inherit;'>$col3</div>
            </div>";

        }
        if($content != '')
        {

            $viewall_link = $redirect . "&panel=${panel}&view=detail&datatype=${data_type}";

            $content .= "\n<div class='viewbtn'><a href='$viewall_link' class=tool>View</a></div>";

        }
    }

    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}
function columns($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $time = time();
    $companyid = $user->uid;
    $tab = sget($_REQUEST,'tab');
    $panel = "columns";
    $data_type = sget($_REQUEST, 'datatype');
    $redirect = rebuild_url(array('tab','datatype'));
    $content = '';
    $HV = hidden_post_values(array('tab', 'datatype', 'panel'));
    switch($data_type)
    {
        case "metrics":
            $title = "METRICS";
            break;
        case "watchlist":
            $title = "WATCH LIST";
            break;
    }
    $Block['title'] = $title;

    if(!$write || $op == null || $op == 'view')
    {
        $view = sget($_REQUEST, 'view');

        if(!$write || $view == 'edit' || $view == 'detail')
        {
            if ($write) {
              $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>Add</a></div>";
            }

            $Row = array();

            $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and deleted != 1 order by data_attr, dataid";
            $result = db_query($sql, array($companyid, $data_type));


            if(!$write || $view == 'detail')
            {
                $content = "<table class='with-border' style='width:500px;'>";
                while($Row = db_fetch_object($result))
                {
                    $dataid = $Row->dataid;
                    $column1 = htmlentities($Row->data_value, ENT_QUOTES);
                    $column2 = htmlentities($Row->data_value2, ENT_QUOTES);
                    $column3 = htmlentities($Row->data_value3, ENT_QUOTES);
                    $position= $Row->data_attr;
                    $content .= "\n<tr><td>$column1</td><td>$column2</td><td>$column3</td></tr>";
                }
                $content .= "</table>";
                if ($write) {
                  $content .= "<br><a href='$redirect&panel=${panel}&view=edit' class=button>Edit</a>";
                }
            }elseif($view == 'edit')
            {
                $content = "<form method='/account' method='post'><table class=report style='width:600px'>" .
                            "<tr><td >Delete</td>
                                <td>Ttitle</td>
                                <td>Value 1</td>
                                <td>Value 2</td>
                                <td>Position</td>
                            </tr>";
                while($Row = db_fetch_object($result))
                {
                    $dataid = $Row->dataid;
                    $column1 = $Row->data_value;
                    $column2 = $Row->data_value2;
                    $column3 = $Row->data_value3;
                    $position= $Row->data_attr;
                    $content .= "<tr>
                                    <td><input type=checkbox name='delete[]' value='del_$dataid' /></td>
                                    <td><input type=text name='col1_$dataid' value='$column1' /></td>
                                    <td><input type=text name='col2_$dataid' value='$column2' style='width:80px' /></td>
                                    <td><input type=text name='col3_$dataid' value='$column3' style='width:80px' /></td>
                                    <td><input type=text name='pos_$dataid' value='$position' style='width:40px' /></td>
                                </tr>";
                }
                $content .= "</table>";
                $content .= "
                    <div><input type=submit name=submit value='Submit'  class=button /> <a href='$redirect&panel=${panel}&view=detail' class=button>Cancel</a>
                    </div>
                    $HV
                    <input type=hidden name=view value='detail' />
                    <input type=hidden name=do value='update' />
                    <input type='hidden' name=update_section value='$panel' />
                </form>";
            }
        }elseif(strcasecmp($view, 'add') == 0 )
        {

            $content = "<form action='/account' method=post>
                <table class='report-table' style='width:400px'>
                    <tr>
                        <td>Title</td>
                        <td style='width:100px;'>Column 1</td>
                        <td style='width:100px;'>Column 2</td>
                    </tr>
                    <tr>
                        <td><input type=text name='title' value=''  style='' /> </td>
                        <td><input type=text name='val1' value=''  style='width:80px;' /></td>
                        <td><input type=text name='val2' value=''  style='width:80px;' /></td>
                    </tr>
                </table>
                <div>
                    <input type=submit name=submit value=submit class=button /> <a href='$redirect&panel=${panel}&view=detail' class=button>Cancel</a>
                    $HV
                    <input type=hidden name=view value='detail' />
                    <input type=hidden name=do value='insert' />
                    <input type='hidden' name=update_section value='$panel' />
                </div>
            </form>";
        }
    }elseif($op == 'update')
    {
        $do = sget($_REQUEST, 'do');
        $Correct = false;
        if($do == 'insert')
        {
            $title = sget($_REQUEST, 'title');
            $val1 = sget($_REQUEST, 'val1');
            $val2 = sget($_REQUEST, 'val2');

            if(empty($title) )
            {
                drupal_set_message('Please enter a title', 'error');
                return;
            }
            $sql = "insert into maenna_company_data (companyid,access, data_type, data_value, data_value2, data_value3,editorid) values(%d, '%s', '%s', '%s', '%s', '%s', %d)";
            $DBValues = array($companyid, $time, $data_type, $title, $val1, $val2, $editorid);
            if(db_query($sql, $DBValues))
            {
                drupal_set_message('New record added');
            }else{
                drupal_set_message('Operation failed');
            }
            return;
        }
        elseif($do == 'update')
        {
            //process delete first
            $DEL = array();
            if(isset($_REQUEST['delete']) && (count($_REQUEST['delete']) > 0))
            {
                foreach($_REQUEST['delete'] as $Item)
                {
                    $del_dataid = str_replace('del_', '', $Item);
                    if(intval($del_dataid) > 0)
                    {
                        $sql = "update maenna_company_data set deleted = 1, access = '%s', editorid = %d where dataid = %d limit 1";
                        $DBValues = array($time, $editorid, $del_dataid);
                        $Correct = db_query($sql, $DBValues);
                        $DEL[] = $del_dataid;
                    }
                }
            }
            // update record
            foreach($_REQUEST as $key => $val)
            {
                if(substr($key, 0, 5) == 'col1_')
                {
                    $dataid = str_replace('col1_', '', $key);
                    if(in_array($dataid, $DEL))continue;
                    $data_value = $val;
                    $data_value2 = sget($_REQUEST, 'col2_'.$dataid);
                    $data_value3 = sget($_REQUEST, 'col3_'.$dataid);
                    $data_attr = sget($_REQUEST, 'pos_'.$dataid);
                    $sql = "update maenna_company_data set data_attr = '%s', data_value = '%s', data_value2='%s', data_value3='%s', editorid=%d, access='%s' where dataid = %d limit 1";
                    $DBValues = array($data_attr, $data_value, $data_value2, $data_value3, $editorid, $time, $dataid);
                    $Correct = db_query($sql, $DBValues);
                    if(! $Correct) break;
                }
            }

        }
        if($Correct)drupal_set_message("Operation Successful");
        else drupal_set_message("Operation failed", 'error');
        return ;
    }


    $Block['body'] = content_box($Block['title'],$content);
    return $Block;
}

function watch_list($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $companyid = $user->uid;
    $time = time();
    $Block['title'] = "WATCH LIST";
    $tab = sget($_REQUEST,'tab');
    $data_type = 'watchlist';
    $panel = "columns";
    $redirect = rebuild_url(array('tab'));
    $content = '';
    $HPV = hidden_post_values(array('tab'));

    if ($write) {
      $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&datatype=${data_type}&view=edit' class=tool>Edit</a></div>";
    }
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and deleted != 1 order by data_attr, dataid ";
        $result = db_query($sql, array($companyid, $data_type));
        $header = "<div class=row>
                <div style='overflow:hidden;width:110px;'>&nbsp;</div>
                <div style='width:20%;position:absolute;top:0;left:120px;overflow:hidden;padding:inherit;'>REV</div>
                <div style='width:20%;position:absolute;top:0;left:180px;overflow:hidden;padding:inherit;'>ROE</div>
            </div>";
        while($Row = db_fetch_object($result))
        {
            $title = $Row->data_value;
            if($title)
            $title = htmlentities($title,ENT_QUOTES);
            else $title = "&nbsp;";
            $col2 = htmlentities($Row->data_value2,ENT_QUOTES);
            $col3 = htmlentities($Row->data_value3,ENT_QUOTES);
            $content .= "<div class=row>
                <div style='overflow:hidden;width:110px;'>$title</div>
                <div style='width:20%;position:absolute;top:0;left:120px;overflow:hidden;padding:inherit;'>$col2</div>
                <div style='width:20%;position:absolute;top:0;left:180px;overflow:hidden;padding:inherit;'>$col3</div>
            </div>";

        }
        if($content != '')
        {

            $viewall_link = $redirect . "&panel=${panel}&view=detail&datatype=${data_type}";

            $content .= "\n<div class='viewbtn'><a href='$viewall_link' class=tool>View</a></div>";

        }
        $content = $header . $content;
    }

    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}


/* EOF */
