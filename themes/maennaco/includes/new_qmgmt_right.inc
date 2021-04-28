<?php
//$maenna_page['right']= array('upload_box','maenna_selection', 'recent_topics');

$maenna_page['right']= array('info_tools');

// similar function in new_questionnaire_right.inc
function qmgmt_files($op)
{
    global $user, $AccessObj;
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
    

    $editorid = $user->uid;
    $time = time();
    $Block['title'] = "FILES";
    
    $section = __FUNCTION__;

    $content = '';
    $HV =  hidden_post_values(array('tab','page','target', 'id', 'uid','panel' ));
    $redirect = rebuild_url(array('tab','page','target', 'id', 'uid', 'panel'));
    $parentid = '';
    


    $parentid = sget($_REQUEST, 'id');
    $uid = sget($_REQUEST, 'uid');
    if($AccessObj->user_type == 'admin')
    {
        if(! Connections::admin_of($uid)) return '';
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
                           
                            <input type='hidden' name=update_section value='$section' />
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
        $result = db_query($sql, array($uid,$parentid));
        $counter = 1;
        while(($Row = db_fetch_object($result)) )
        {
            $questionid=$Row->recordid;
            $qaid = $Row->qaid;
            $file = $Row->answer;
            if($file){
                $path =  "./" . file_directory_path() . "/" . $file;
                $file = preg_replace("/^[0-9]*_/i", '', $file);
                $filetitle = ucwords($file);
                $filename = $file;
                if(strlen($filetitle) > 40) $filetitle = substr($file, 0, 40) . "...";
                $file = "<a href='$path' target='_blank' class='blue'>$filetitle</a>";
            }else{
                continue;
            }
            $content .= "\n<div class=row style='clear:both'>
                            
                                <div style='width:30px;float:right;'>
                                    <a href='$redirect&update_section=qmgmt_files&aid=$qaid&do=del'
                                            onclick='return confirm(\"Continue to remove the file?\")'
                                        >Del</a>
                                </div>
                                <div style='width:240px;overflow:hidden;white-space:nowrap;' title='$filename'>$counter. $file</div>
                           
                        </div>";
            $counter++;
        }
        $sql = "select * from maenna_qa where qid = %d and uid = %d order by qaid ";
        $result = db_query($sql, array($parentid, $uid));
        while($Row = db_fetch_object($result))
        {
            $qaid = $Row->qaid;
            $file = $Row->answer;
            if($file){
                $path =  "./" . file_directory_path() . "/" . $file;
                $file = preg_replace("/^[0-9]*_/i", '', $file);
                
                $filename = htmlentities(ucwords($Row->filename), ENT_QUOTES | ENT_IGNORE , "UTF-8");
                if(strlen($filetitle) > 25) $filetitle = substr($file, 0, 25) . "...";
                $file = "<a href='$path' target='_blank' class='blue'>$filename</a>";
            }else{
                continue;
            }
            $content .= "\n<div class=row>
                                <div style='width:30px;float:right;'>
                                    <a href='$redirect&update_section=qmgmt_files&aid=$qaid&do=del'
                                            onclick='return confirm(\"Continue to remove the file?\")'
                                        >Del</a>
                                </div>
                               <div style='width:250px;overflow:hidden;' title='$filename'>$counter.  $file</div>
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
                
                $DBValues = array($parentid, $type, $uid, $filename, $time, $name);
                
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
            $aid = sget($_REQUEST, 'aid');
            $uid = sget($_REQUEST, 'uid');
            $new_uid = $editorid * 10000 + $uid;
            if($aid)
            {
                $sql = "update maenna_qa set uid = %d where qaid = %d and uid = %d limit 1";
                if(db_query($sql, array($new_uid, $aid, $uid)))
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