<?php


function qfiles($op)
{
    global $user, $AccessObj;
    $editorid = $user->uid;
    
    $time = time();
    $Block['title'] = "Upload Files";
    
    $redirect = rebuild_url(array('tab'));
    $section = __FUNCTION__;
    $HV =  hidden_post_values(array('tab'));
    $content = '';
    
    $parentid = '';
    
    
    if($AccessObj->user_type == 'people')
    {
        $target = 'pro';
    }else{
        $target = 'company';
    }
    $sql = "select  *
            from    maenna_questionair
            where   status = 1 and
                    target='%s' and
                    parentid is null
            order by recordid desc";
    $result = db_query($sql,array($target));
    $Row = db_fetch_object($result);
    if($Row)
    {
        $parentid = $Row->recordid;
    }

    $Block['title'] .= "<div class='editbtn'><a href='#' class='tool openbox' boxid='box1'>Add</a></div>";

    if($op && $op != 'update')
    {
        $content = "<div id='box1' style='display:none'>
                        <form method=post action='/account' enctype='multipart/form-data'>
                            <div class=row>Name:<br><input type=text name=name value='' maxlength=30 /></div>
                            <div class=row>File: <input type=file name='file' /></div>
                            <div class=row><input type=submit name=submit value='submit'  class=button /> &nbsp;
                                    <a href='#' class='hidebox button' boxid=box1>Close</a>
                            </div>
                            $HV
                            <input type='hidden' name=section value='qfiles' />
                            <input type='hidden' name=update_section value='qfiles' />
                            <input type='hidden' name=do value='new' />
                        </form>
                    </div>";
        $content .= js_init("init_openbox();init_hidebox();");
        
        // list files
        $sql = "select Q.*, A.answer,A.qaid from maenna_questionair as Q
                    left join maenna_qa as A on
                        Q.recordid = A.qid and
                        A.uid = %d
                    where Q.parentid = %d and type='fileuploader' order by weight, recordid";
        $result = db_query($sql, array($AccessObj->uid,$parentid));
        $counter = 1;
        while(($Row = db_fetch_object($result)) )
        {
            $qaid = $Row->qaid;
            die("D".$qaid);
            $questionid=$Row->recordid;
            //$title = $counter . ". " . htmlentities(ucwords($Row->content), ENT_QUOTES) . "";
            $file = $Row->answer;
            if($file){
                $path =  "./" . file_directory_path() . "/" . $file;
                $file = preg_replace("/^[0-9]*_/i", '', $file);
                $filetitle = ucwords($file);
                $filename = $file;
                if(strlen($filetitle) > 18) $filetitle = substr($file, 0, 18) . "...";
                $file = "<a href='$path' target='_blank' class='blue'>$filetitle</a>";
            }else{
                continue;
            }
            $content .= "\n<div class=row style='padding:7px 0'>
                            <div style='margin-left:10px;margin-top:5px;'>
                                <div style='margin-top:5px;width:200px;overflow:hidden;' title='$filename'>$counter. $file</div>
                                <div class=editbtn><a href='$redirect&update_section=$section&qaid=".$qaid."&do=del' onclick='return confirm(\"Continue to delete the file\")'>Del</a></div>
                            </div>
                        </div>";
            $counter++;
        }
        $sql = "select * from maenna_qa where qid = %d and uid = %d order by qaid ";
        $result = db_query($sql, array($parentid, $AccessObj->uid));
        while($Row = db_fetch_object($result))
        {
            $qaid = $Row->qaid;
            $file = $Row->answer;
            if($file){
                $path =  "./" . file_directory_path() . "/" . $file;
                $file = preg_replace("/^[0-9]*_/i", '', $file);
                
                $filename = $Row->filename;
                $filetitle = htmlentities($filename, ENT_QUOTES);
                
                if(strlen($filename) > 18) $filename = substr($filename, 0, 18) . "...";
               
                $filename = htmlentities(ucwords($filename), ENT_QUOTES);
                 
                $filename = "<a href='$path' target='_blank' class='blue'>$filename</a>";
            }else{
                continue;
            }
            $content .= "\n<div class=row style='padding:7px 0'>
                            <div style='margin-left:10px;margin-top:5px;'>
                               <div style='margin-top:5px;width:200px;overflow:hidden;' title='$filetitle'>$counter.  $filename</div>
                               <div class=editbtn><a href='$redirect&update_section=$section&qaid=$qaid&do=del' onclick='return confirm(\"Continue to delete the file\")'>Del</a></div>
                            </div>
                        </div>";
            $counter++;
        }
        
    }elseif($op == 'update')
    {
        
            $do = sget($_REQUEST, 'do');
        
            if($do == 'new')
            {
                $name = sget($_POST, 'name');
            $file = '';
            if($_FILES['file'] && $_FILES['file']['err'] == 0)
            {
                $file = $_FILES['file']['name'];
            }
            if(empty($file) || empty($name))
            {
                drupal_set_message('Please complete the file form and try again', 'error');
                return;
            }
            
            
            if(empty($parentid))
            {
                drupal_set_message("Invalid questionnaire id - file uploader", 'error');
                return;
            }
            $filename = time() . '_' . clearString($file);
            $type = $_FILES['file']['type'];
            $path = "./" . file_directory_path() . "/" . $filename;
            $tmp = $_FILES['file']['tmp_name'];
            if( move_uploaded_file($tmp, $path))
            {
                $sql_new = "insert into maenna_qa
                            (qid, qtype, uid, answer, created,filename)
                            values (%d, '%s', %d, '%s', '%s','%s')";
                
                $DBValues = array($parentid, $type, $AccessObj->uid, $filename, $time, $name);
                
                if( ! db_query($sql_new, $DBValues)){
                    drupal_set_message("Failed to upload file", 'error');
                    return;
                }else{
                    drupal_set_message("A new file is added");
                }
            }else
            {
                
               drupal_set_message("Error: the file $file wasn't uploaded", 'error');
            }
            return ;
        }elseif($do == 'del')
        {
            $qaid = sget($_REQUEST, 'qaid');
            if($qaid)
            {
                $sql = "delete from maenna_qa where qaid = %d and uid = %d limit 1";
                if(db_query($sql, array($qaid, $AccessObj->uid)))
                {
                    drupal_set_message("The file is deleted");
                }
                else
                {
                    drupal_set_message("Failed to delete the file", 'error');
                }
            }
        }
        return;
    }
    
    
    $Block['body'] = sidebar_box($Block['title'], $content);
    return $Block;
}

/* EOF */