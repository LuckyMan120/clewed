<?php

function questionnaire_fileuploader($op = null)
{
    global $user;
    $uid = $user->uid;
    $editorid = $uid;
    $rid = userRoleId($uid);
    $Block['title'] = '';
    $content = '';
    
    if($op == null || $op == 'view'){
        $sql = "select * from maenna_questionair where parentid is null and target = 'pro' and status = 1";
        $result = db_query($sql);
        $Row = db_fetch_object($result);
        if($Row)
        {
            
            $Block['title'] = "Upload Files";
            $parentid = $Row->recordid;
            
            $sql = "select Q.*, A.answer from maenna_questionair as Q
                    left join maenna_qa as A on
                        Q.recordid = A.qid and
                        A.uid = %d    
                    where Q.parentid = %d and
                        (type = 'fileuploader') and
                        ((usertype = 0) or (usertype = $rid))
                    order by weight, recordid";
            $result = db_query($sql, array($editorid, $parentid));
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
                    if(strlen($filetitle) > 25)$filetitle = substr($filetitle, 0, 25) . "...";
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

/* EOF */