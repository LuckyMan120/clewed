<?php
//$maenna_page['right']= array('upload_box','maenna_selection', 'recent_topics');
global $redirect;
$redirect = rebuild_url(array('tab', 'page'));

$maenna_page['right']= array('info_tools','post_project','advisory_openings', 'project_openings');
function post_project($op = null){
    global $user, $redirect;
    $editorid = $user->uid;
    $Block['title'] = "<div style='padding:5px 0; '><a href='$redirect&panel=posting_panel&postingtype=council'>Post advisory council opening</a></div>" .
                        "<div style='padding:5px 0; '><a href='$redirect&panel=posting_panel&postingtype=project'>Post a speical project</a></div>";
    $content = '';
    $time = time();
    
    $Block['body'] = sidebar_box($Block['title'],$content);
    return $Block;
}

function posting_panel($op = null){
    global $user, $redirect;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    
    
    $content = '';
    $time = time();
    
    $view = sget($_REQUEST, 'view');
    $do = sget($_REQUEST, 'do');
    
    $postingtype = sget($_REQUEST, 'postingtype');
    if($postingtype == 'council')$Block['title'] = 'Post Advisory Council Opening';
    elseif($postingtype == 'project') $Block['title'] = 'Post Special Project';
    else{return '';}
    $message = $startdate = $enddate = $esthr = '';
    $option_proType = Options_proType();
    $option_industry = Options_industry();
    $option_expertise = Options_experties();
    $startdate = $enddate = $esthr = '';
    $maennaid = '';
    if(sget($_REQUEST, 'companyid'))$maennaid = getProjectName(sget($_REQUEST, 'companyid'));
    if(sget($_REQUEST, 'postingid')){
        $postingid = sget($_REQUEST, 'postingid');
        $sql = "select * from maenna_postings where postingid = %d";
        $result = db_query($sql, array($postingid));
        $Row = db_fetch_object($result);
        if(empty($Row)) return "invalid posting id";
        if($Row->postedby != $editorid){
            $isAdmin = false;
            foreach($user->roles as $key => $title){
                if($key == 6 || $key == 10){
                    $isAdmin = true;
                }
            }
            if(! $isAdmin) {
                return "you are not allowed to edit this posting";
            }
        }
        $maennaid = getProjectName($Row->companyid);
        $message = $Row->message;
        $option_proType = Options_proType($Row->protype , 1);
        $option_industry = Options_industry($Row->industry, 1);
        $option_expertise = Options_experties($Row->expertise, 1);
        $startdate = date('m/d/Y', $Row->startdate);
        $enddate = date('m/d/Y', $Row->enddate);
        $esthr = $Row->esthr;
    
    }
    
    
    
    if($op && $op != 'update'){
        $content = <<< END
        <form action='/account' method=post onsubmit='return check_input();' >
        <table class='edit_table' style='width:450px;'>
            
            <tr><td style='width:200px;'>Project Name</td><td><input type=text name=name value='$maennaid' class='require_string'/></td></tr>
            <tr><td>Message:</td><td><textarea name='message' style='width:300px;height:100px;'>$message</textarea></td></tr>
            <tr><td>Professional Type</td><td><select name='protype' style='width:200px;'  class='require_string'><option value=''></option>$option_proType</select></td></tr>
            <tr><td>Expertise</td><td><select name='expertise' style='width:200px;'  class='require_string'><option value=''></option>$option_expertise</select></td></tr>
            <tr><td>Industry</td><td><select name='industry' style='width:200px;'><option value=''></option>$option_industry</select></td></tr>
            <tr><td>Start Date</td><td><input type=text name=startdate value='$startdate' class='datepicker require_string' /></td></tr>
            <tr><td>Est. Hrs</td><td><input type=text name=esthr value='$esthr'  /> <span class='note'>(please enter numbers only, eg. 5)</span></td></tr>
            <tr><td>Deadline</td><td><input type=text name=enddate value='$ebddate'   class='datepicker require_string' /></td></tr>
            <tr><td></td><td><input type=submit name=submit value='submit' class=button /></td></tr>
        </table>
            <input type=hidden name=tab value=information />
            <input type=hidden name=page value=postings />
            <input type=hidden name=update_section value=posting_panel />
            <input type=hidden name=panel value=posting_panel />
            <input type=hidden name=postingtype value='$postingtype' />
            <input type=hidden name=postingid value='$postingid' />
        </form>
        <script type='text/javascript'>
        $(document).ready(function(){
            init_datepicker();
        })
        </script>
END;
    
    }elseif($op == 'update'){
        
        $DBKeys = array('postingtype',
                        'companyid',
                        'projectname',
                        'message',
                        'protype',
                        'expertise',
                        'industry',
                        'startdate',
                        'esthr',
                        'enddate',
                        'created',
                        'postedby');
        $SQL_STR = array();
        $DBValues = array();
        foreach($DBKeys as $key){
            $SQL_STR[] = "'%s'";
            $DBValues["$key"] = sget($_REQUEST, $key);
        }
        $maennaid = sget($_REQUEST,'name');
        $companyid = realid($maennaid);
        $projectname = sget($_REQUEST, 'name');
        if(empty($maennaid) || empty($projectname) || empty($companyid)){
            drupal_set_message("Invalid maenna id or project name values", 'error');
            return;
        }
        $DBValues['companyid'] = $companyid;
        $DBValues['projectname'] = $projectname;
        $DBValues['created'] = $time;
        $DBValues['postedby'] = $editorid;
        $DBValues['startdate'] = strtotime($DBValues['startdate']);
        $DBValues['enddate'] = strtotime($DBValues['enddate']);
       
        $postingid = sget($_REQUEST, 'postingid');
        
        $sql = "insert into maenna_postings (".implode(',', $DBKeys). ") values (".implode(',',$SQL_STR).")";
        if(db_query($sql, $DBValues)){
            drupal_set_message("Advisory councol is posted successfully");
            return;
        }else{
            drupal_set_message("Failed to add advisory council", 'error');
            return; 
        }
        
    }
    
    $Block['body'] = sidebar_box($Block['title'],$content);
    return $Block;
}

/* EOF */