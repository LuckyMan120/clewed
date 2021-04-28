<?php
// $maenna_page['content'] = array('info_list','related_list');

function questionnaire($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    
    $target = sget($_REQUEST, 'target');
    if($target == 'pro')$Block['title'] = 'Professional Questionnaire';
    else $Block['title'] = 'Company Questionnaire';
    $content = '';
    $time = time();
    
    if($op == view || $op == null)
    {
        /*
                $sql = "select parent.*, count(child.recordid) as cnt from maenna_questionair as parent
                            left join maenna_questionair as child on
                                parent.recordid = child.parentid and child.type='question' 
                            where parent.type = 'version' and parent.target = '$target' group by parent.recordid order by parent.created desc";
        */
         $sql = "select parent.*, count(child.recordid) as cnt,  count(distinct maenna_qa.uid ) as uids
                            from maenna_questionair as parent
                            left join maenna_questionair as child on
                                parent.recordid = child.parentid and child.type='question'
                left join maenna_qa on child.recordid = maenna_qa.qid and maenna_qa.uid > 0 
                            where parent.type = 'version' and parent.target = '$target' group by parent.recordid order by parent.created desc";
        $result = db_query($sql);
        $content .= tpl_render_version($result);
    }elseif($op == 'update')
    { // remove op
        $recordid = intval(sget($_REQUEST, 'id'));
        if($recordid > 0)
        {
            $sql = "delete from maenna_questionair where recordid = %d limit 1";
            db_query($sql, array($recordid));
            drupal_set_message("Questionnaire Template Removed");
        }
    }
    
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function tpl_render_version($result)
{
    $html = '';
    $Versions = array();
    $target = sget($_REQUEST, 'target');
    $_Status = array('0' => 'pending', '1'=>'live');
    while($Row = db_fetch_object($result))
    {
        $recordid = $Row->recordid;
        $version = $Row->content;
        $created = date('m/d/Y', $Row->created);
        $details = "<a href='/account?a=information&tab=questionnaire&target=$target&panel=qdetail&id=$recordid'>details</a>";
        $edit = "<a href=''>edit</a>";
        $remove = "<a href=''>remove</a>";
        
       
        if(empty($Row->cnt)) $numOfQ = 0;
        else $numOfQ = $Row->cnt;
        $option_status = "<select name='status' table='maenna_question1' idname='recordid' idval='$recordid' column='status' class='ajaxTrigger'>" .option_code($_Status, $Row->status) . "</select>";
        $answered = $Row->uids;
        if($answered > 0){
            $answered = "<a href='/account?a=information&tab=questionnaire&panel=answeredlist&target=$target&id=$recordid'>$answered</a>";
        }
        $Versions[] = array(
                            'version' => $version,
                            '# of questions' => $numOfQ,
                            'created on' => $created,
                            'details' => $details,
                            'edit' => "<a href='/account?a=information&tab=questionnaire&panel=new_questionnaire&target=$target&id=$recordid'>edit</a>",
                            'remove' => "<a onclick='return confirm(\"Continue to remove selected questionnaire template\")' href='/account?a=information&tab=questionnaire&panel=questionnaire&update_section=questionnaire&id=$recordid&target=$target'>remove</a>",
                            'status' => $option_status,
                            'preview' => "<center><a href='/account?a=information&tab=questionnaire&panel=preview&page=1&id=$recordid'>go &#187;<center></a>",
                            'answered' => "<center>$answered</center>"
                            );
    }
    
    if(count($Versions) == 0)
    {
        
        $html = "No Questionnaire was found. ";
    }else{
        $Table = array('title' => 'Questionnaire List',
                       'class' => 'report',
                       'thead'=> array(),
                       'tbody'=> array());
        $Table['thead'] = array(
                            array('label'=> 'version', 'style' => ''),
                            array('label'=> '# of questions', 'style' => 'width:100px'),
                            array('label'=> 'created on', 'style' => 'width:60px'),
                            array('label'=> 'details', 'style' => 'width:60px'),
                            array('label'=> 'edit', 'style' => 'width:40px'),
                            array('label'=> 'remove', 'style' => 'width:50px'),
                            array('label'=> 'status', 'style' => 'width:60px'),
                            array('label'=> 'preview', 'style' => 'width:50px;text-align:center'),
                            array('label'=> 'answered', 'style' => 'width:50px;text-align:center'),
                            );
        $Table['tbody'] = $Versions;
        $html = render_table($Table);
        $html .= js_init("init_ajaxTrigger();");
    }
    $html .= "<br>" .
        "Click <a href='/account?a=information&tab=questionnaire&panel=new_questionnaire&target=$target'>here</a> to add one.";
    
    return $html;
}

function new_questionnaire($op)
{
    global $user;
    $editorid = $user->uid;
    
    
    $target = sget($_REQUEST, 'target');
    $recordid = sget($_REQUEST, 'id');
    $version = '';
    if(empty($recordid))
    {
        if($target == 'pro')$Block['title'] = 'Add New Professional Questionnaire';
        else $Block['title'] = 'Add New Company Questionnaire';
    }else{
        if($target == 'pro')$Block['title'] = 'Edit Professional Questionnaire';
        else $Block['title'] = 'Edit Company Questionnaire';
        
        $sql = "select content from maenna_questionair where recordid = %d limit 1";
        $result = db_query($sql, array($recordid));
        $Row = db_fetch_object($result);
        $version = $Row->content;
    }
    
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
    {
        $content = <<< END
        <form action='/account' method='post'>
            <div class=row style='padding:5px 0;border:none'>
                <label>Please enter a Questionnaire version. (eg a year)</label><br>
            </div>
            <div class=row style='padding:5px 0;border:none;'>
                <input type=text name=version value='$version' style='width:300px;' />
            </div>
            <div class=row>
                <input type=submit name=submit value='submit' class='button' />
            </div>
            <input type=hidden name=a value=information />
            <input type=hidden name=tab value=questionnaire />
            <input type=hidden name=panel value=questionnaire />
            <input type=hidden name=update_section value=new_questionnaire />
            <input type=hidden name=target value='$target' />
            <input type=hidden name=id value='$recordid' />
        </form>
END;
        
    }elseif($op == 'update')
    {
        $type = 'version';
        $target = sget($_REQUEST, 'target');
        $content = sget($_REQUEST, 'version');
        $created = time();
        $recordid = sget($_REQUEST, 'id');
        if(empty($type) || empty($target) || empty($content) || empty($created))
        {
            drupal_set_message("missing information", 'error');
        }else{
          
            if(empty($recordid))
            {
                $sql = "insert into maenna_questionair (target, type, content, created, editorid) values ('%s','%s','%s','%s', %d)";
                $DBValues = array($target, $type, $content, $created, $editorid);
            }else{
                $sql = "update maenna_questionair set content = '%s',editorid = %d where recordid = %d limit 1";
                $DBValues = array($content, $editorid, $recordid);
            }
            if(db_query($sql,$DBValues) !== false){
                drupal_set_message("A new Questionnaire template is created");
            }else{
                drupal_set_message("Failed to add new template", 'error');
            }
        }
        return '';
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}


function qdetail($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    
    $target = sget($_REQUEST, 'target');
    $parentid = sget($_REQUEST, 'id');
    if(empty($parentid)) return '';
    
    if($op == null || $op == 'view')
    {
        $add = sget($_REQUEST, 'add');
        $edit = sget($_REQUEST, 'edit');
        $submit = sget($_REQUEST, 'submit');
        if($add && empty($submit))
        {
            $content .= tpl_single_form('add');
        }elseif($edit && empty($submit)){
            $content .= tpl_single_form('edit');
        }else
        {
    
            $sql = "select * from maenna_questionair where recordid = %d limit 1";
            $result = db_query($sql, array($parentid));
            $version = '';
            $Row = db_fetch_object($result);
        
            $version = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            if($target == 'pro')$Block['title'] = 'Professional Questionnaire -- ' . $version;
            else $Block['title'] = 'Company Questionnaire -- ' . $version;
                   
            
            $content .= "
                    <div style='position:absolute;with:200px;top:0;right:0;text-align:right;'>
                        <a href='/account?a=information&tab=questionnaire&panel=questionnaire&target=$target' class='button'>back to list</a>
                            &nbsp;&nbsp;&nbsp;
                        <a href='/account?a=information&tab=questionnaire&target=$target&panel=qdetail&id=$parentid' class='button'>refresh</a>
                            &nbsp;&nbsp;&nbsp;
                        <a href='/account?a=information&tab=questionnaire&panel=preview&page=1&id=$parentid' class='button'>preview</a>
                    </div>
                    <div style='position:relative;width:100%;text-align:right'>
                    
                    <a href='/account?a=information&tab=questionnaire&target=$target&panel=qdetail&id=$parentid&add=question' class=button>add question</a>
                        &nbsp;&nbsp;&nbsp;
                    <a href='/account?a=information&tab=questionnaire&target=$target&panel=qdetail&id=$parentid&add=title' class=button>add subtitle</a>
                        &nbsp;&nbsp;&nbsp;
                    <a href='/account?a=information&tab=questionnaire&target=$target&panel=qdetail&id=$parentid&add=instruction' class=button>add instruction</a>
                        &nbsp;&nbsp;&nbsp;
                    <a href='/account?a=information&tab=questionnaire&target=$target&panel=qdetail&id=$parentid&add=fileuploader' class=button>add file uploader</a>
                        &nbsp;&nbsp;&nbsp;
                    <a href='/account?a=information&tab=questionnaire&target=$target&panel=qdetail&id=$parentid&do=pagedivider&update_section=qdetail' class=button>add page divider</a>
                        </div>";
                        
            $sql = "select * from maenna_questionair where parentid = %d order by weight asc, recordid asc";
            $result = db_query($sql, array($parentid));
            $content .= tpl_render_qlist($result);
            $content .= js_init("init_ajaxTrigger();");
        }
    }elseif($op == 'update'){
        $do = sget($_REQUEST, 'do');
        $parentid = sget($_REQUEST, 'id');
        $target = sget($_REQUEST, 'target');
        $type= sget($_REQUEST, 'type'); // question or title
        $weight = sget($_REQUEST, 'weight');
        $usertype = sget($_REQUEST, 'usertype');
        $content = sget($_REQUEST, 'content');
        $recordid = sget($_REQUEST, 'recordid');
        if(empty($parentid)){
            drupal_set_message("invalid parent id",'error');
            return;
        }
        
        if($do == 'add'){
            if(empty($content)){
                drupal_set_message('empty message or title','error');
                return;
            }
            $keys = 'parentid, target, type, usertype, content, created, editorid, weight';
            $sql_str = "%d, '%s', '%s', %d, '%s','%s', %d, %d";
            $DBValues = array($parentid, $target, $type, $usertype, $content, $time, $editorid, $weight);
            $sql = "insert into maenna_questionair ($keys) values ($sql_str)";
        }elseif($do == 'remove'){
            $recordid = sget($_REQUEST, 'recordid');
            if(empty($recordid)) {
                drupal_set_message("invalid id", 'error');
                return;
            }
            $sql = "delete from maenna_questionair where recordid = %d limit 1";
            $DBValues = array($recordid);
        }elseif($do == 'pagedivider'){
            $id = sget($_REQUEST, 'id');
            $type = 'pagedivider';
            $sql = "insert into maenna_questionair (parentid, type,created, editorid) values(%d, '%s','%s',%d)";
            $DBValues = array($id, $type, $time, $editorid);
        }else{ // edit title or question
            if(empty($recordid)){
                drupal_set_message('invalid record id', 'error');
                return;
            }
            $sql = "update maenna_questionair set content = '%s', usertype = '%s', created = '%s', editorid = %d where recordid = %d limit 1";
            $DBValues = array($content, $usertype, $time, $editorid, $recordid);
        }
        if(isset($DBValues) && is_array($DBValues) && db_query($sql, $DBValues)){
            drupal_set_message("Operation Successful");
        }else{
            drupal_set_message("Failed to complete the changes",'error');
        }
        
    }
    

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function tpl_single_form($mode)
{
    $parentid = sget($_REQUEST, 'id');
    $target = sget($_REQUEST, 'target');
    $recordid = '';
    $tr1 = $tr2 = '';
    
    if($mode == 'add')
    {
        $title = "Add " . sget($_REQUEST, 'add');
        $type = sget($_REQUEST, 'add');
        
        if($type == 'fileuploader')
        {
            $tr1 = "<tr><td style='width:160px'>Enter a title for file uploader:</td><td><input name=content style='width:300px;' /></td></tr>";
            
        }else
        {
            $tr1 = "<tr><td>Enter a $type:</td><td><textarea name=content style='width:300px;height:80px;'></textarea></td></tr>";
            
        }
        if($target == 'pro')
        {
                $option_usertype = Options_proType();
                $tr2 = "\n<tr><td>User Type:</td><td><select name=usertype><option></option>$option_usertype</select></td></tr>";  
        }
    }else{
        $recordid = sget($_REQUEST, 'recordid');
        if(empty($recordid)) return "Invalid record id";
        $sql = "select * from maenna_questionair where recordid = %d limit 1";
        $result = db_query($sql, array($recordid));
        $Row = db_fetch_object($result);
        $content = $Row->content;
        $usertype = $Row->usertype;
        $title = "Edit " .sget($_REQUEST, 'edit');
        $type = sget($_REQUEST, 'edit');
        $tr1 = "<tr><td>$type:</td><td><textarea name=content style='width:300px;height:80px;'>$content</textarea></td></tr>";
        if($target == 'pro')
        {
            $option_usertype = Options_proType($usertype);
            $tr2 = "\n<tr><td>User Type:</td><td><select name=usertype><option></option>$option_usertype</select></td></tr>";
        }
    }
    $sql = "select weight from maenna_questionair where parentid = %d order by weight desc limit 1";
    $result = db_query($sql, array($parentid));
    $Row = db_fetch_object($result);
    $weight = $Row->weight;
    if(empty($weight)) $weight = 1;
    $weight += 5;
    $html = <<< END
    <div style='font:italic bold 14px Signika'>$title</div>
    <form method=post action='/account'>
    <table>
        $tr1
        $tr2
        <tr><td></td><td><input type=submit name=submit value='submit' /> </td></tr>
    </table>
        <input type=hidden name=id value='$parentid' />
        <input type=hidden name=target value='$target' />
        <input type=hidden name=type value='$type' />
        <input type=hidden name=weight value='$weight' />
        <input type=hidden name=a value=information />
        <input type=hidden name=tab value='questionnaire' />
        <input type=hidden name=panel value='qdetail' />
        <input type=hidden name=update_section value='qdetail' />
        <input type=hidden name=do value='$mode' />
        <input type=hidden name=recordid value='$recordid' />
    </form>
END;
    return $html;
}

function tpl_render_qlist($result)
{
    if(empty($result)) return '';
    
    $Table = array('title' => '',
                       'class' => 'report',
                       'thead'=> array(),
                       'tbody'=> array());
    $Table['thead'] = array(
                            array('label'=> 'Num.', 'style' => 'width:20px;'),
                            array('label'=> 'type', 'style' => 'width:100px'),
                            array('label'=> 'content', 'style' => ''),
                            array('label'=> 'edit', 'style' => 'width:60px'),
                            array('label'=> 'remove', 'style' => 'width:60px'),
                            array('label'=> 'weight', 'style' => 'width:60px'),
                            
                            );
    $target = sget($_REQUEST, 'target');
    if($target == 'pro')
    {
        $Table['thead'] = array(
                            array('label'=> 'Num.', 'style' => 'width:20px;'),
                            array('label'=> 'type', 'style' => 'width:60px'),
                            array('label'=> 'user type', 'style' => 'width:60px'),
                            array('label'=> 'content', 'style' => ''),
                            array('label'=> 'edit', 'style' => 'width:30px'),
                            array('label'=> 'remove', 'style' => 'width:30px'),
                            array('label'=> 'weight', 'style' => 'width:40px'),
                            
                            );
    }
    $Tbody = array();
    $counter = 0;
    while($Row = db_fetch_object($result))
    {
        $parentid = $Row->parentid;
        $recordid = $Row->recordid;
        $target= $Row->target;
        $type = $Row->type;
        if( ($Row->type != 'question')  && ($Row->type != 'fileuploader'))
        {
            $num = '';
        }else{
            $counter++;
            $num = $counter;
        }
       
        $option_weight = Options_weight($Row->weight);
        $selectWeight = "\n<select name='weight' class='ajaxTrigger' table='maenna_questionair' idname='$parentid' idval='$recordid' column='$target'>$option_weight</select>";
        $usertype = '';
        if($target == 'pro'){
            $usertype = Options_proType($Row->usertype,1);
        }
        $edit = "<a href='/account?a=information&tab=questionnaire&target=$target&panel=qdetail&id=$parentid&edit=$type&recordid=$recordid'>edit</a>";
        if($Row->type == 'pagedivider')$edit = '';
        $Tbody[] = array(
                         'num.' => $num,
                         'type' => $Row->type,
                         'user type' => $usertype,
                         'content' => htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8"),
                         'edit' => $edit,
                         'remove' => "<a href='/account?a=information&tab=questionnaire&target=$target&panel=qdetail&id=$parentid&update_section=qdetail&recordid=$recordid&do=remove' onclick='confirm(\"Continue to remove the record\");'>remove</a>",
                         'weight' => $selectWeight,
                         );
    }
    $Table['tbody'] = $Tbody;
    $return = render_table($Table);
    return $return;
}

function preview($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $content = '';
    $Block['title'] = "Preview - ";
    
    $parentid = sget($_REQUEST, 'id');
    $usertype = sget($_REQUEST, 'usertype');
    if(empty($parentid)) return '';
    
    
    if($op == 'view' || empty($op))
    {
        $sql = "select * from maenna_questionair where recordid = %d";
        $result = db_query($sql, array($parentid));
        $Row = db_fetch_object($result);
       
        $Block['title'] .= htmlentities($Row->content | ENT_IGNORE, "UTF-8");
        $target = $Row->target;
        
       
        if(empty($usertype))
        {
            $sql = "select * from maenna_questionair where parentid = %d order by weight";
            $DBValues = array($parentid);
        }else{
            $sql = "select * from maenna_questionair where parentid = %d and (usertype = '%s' || usertype = 0)  order by weight";
            $DBValues = array($parentid, $usertype);
            $Block['title'] .= " (" . Options_proType($usertype, 1) . " view)";
        }
        
        $result = db_query($sql, $DBValues);
        
        $page = sget($_REQUEST, 'page');
        if(empty($page)) $page = 1;
        $morePage = false;
        $pageCnt = 1;
        $curPage = $page;
        $preType = '';
        $counter = 1;
        while($Row = db_fetch_object($result)){
            $type = $Row->type;
            
            $questionid = $Row->recordid;
            if($type == 'pagedivider'){
                if($preType != 'pagedivider')$pageCnt++;
                
            }elseif($type == 'title'){
                if($pageCnt == $page)
                {
                    $subtitle = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $content .= "\n<div style='margin-top:25px;font:bold italic 13px tahoma;'>$subtitle</div>";
                }
            }elseif($type == 'question')
            {
                if($pageCnt == $page)
                {
                    
                    $question = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $answer = $Row->answer;
                    $content .= "\n<div style='margin-top:20px;'>";
                    $content .= "\n<div style='font:bold 12px Signika;'>$counter. $question</div>";
                    $content .= "\n<div style='margin-top:8px;'><textarea name='q_$questionid' style='width:99%;height:90px;'>$answer</textarea></div>";
                    $content .= "\n</div>";
                    $counter++;
                }
            }elseif($type == 'instruction'){
                if($pageCnt == $page)
                {
                    $instruction = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $content .= "\n<div style='margin-top:8px;font:italic 12px Signika'>$instruction</div>";
                }
            }elseif($type == 'fileuploader')
            {
                if($pageCnt == $page)
                {
                    $loaderTitle = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $content .= "\n<div style='margin-top:20px;'>";
                    $content .= "\n<div style='font:bold 12px Signika;'>$counter. $loaderTitle</div>";
                    $content .= "\n<div style='margin-top:8px;'><input type=file name='f_$questionid' /></div>";
                    $content .= "\n</div>";
                    $counter++;
                }
            }
            $preType = $type;
        }
        // Page navigation by divider
        $prePage = $nextPage = $submit = '';
        if($page > 1 && $pageCnt > 1){
            $pre= $page -1;
            $prePage = "<a href='/account?a=information&tab=questionnaire&panel=preview&id=$parentid&page=$pre' class=button>previous page</a> &nbsp;&nbsp;&nbsp;";
        }
        if($pageCnt > 1 && $page < $pageCnt ){
            $next = $page + 1;
            $nextPage = "<a href='/account?a=information&tab=questionnaire&panel=preview&id=$parentid&page=$next' class=button>next page</a>&nbsp;&nbsp;&nbsp;";
        }
        if($pageCnt == $page){
            $submit = "<input type=submit name=submit value='Submit' class=button />";
        }else{
            $submit = "<input type=submit name=submit value='save and continue' class=button />";
        }
        // Show view as userType for Pro
        if($target == 'pro'){
            $option_protype = Options_proType($usertype);
            $userSelect = <<< END
            <div style='padding-bottom:10px;'>
                <form method=post action='/account'>
                View As: <select name='usertype'>
                    <option value=''>All Pro Types</option>
                    $option_protype
                </select> &nbsp&nbsp<input type=submit name=submit value='go' />
                    <input type=hidden name=a value=information />
                    <input type=hidden name=tab value=questionnaire />
                    <input type=hidden name=panel value=preview />
                    <input type=hidden name=page value=1 />
                    <input type=hidden name=id value='$parentid' />
                </form>
                
            </div>
END;
        }else $userSelect = '';
        
        $content = <<< END
        $userSelect
        <form method='post' action='/account' enctype='multipart/form-data'>
            <div style='position:absolute;width:150px;right:0;top:0;text-align:right;'>
                <a href='/account?a=information&tab=questionnaire&target=$target&panel=qdetail&id=$parentid' class=button>back to details</a>
            </div>
            <div style='position:relative;width:100%;'>
            $content
            <input type=hidden name=a value=information />
            <input type=hidden name=tab value=questionnaire />
            <input type=hidden name=panel value='preview' />
            <input type=hidden name=id value='$parentid' />
            <input type='hidden' name='page' value='$next' />
            $prePage $nextPage $submit
            </div>
        </form>
END;
    }
    
    
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}


function answeredlist($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    
    $Block['title'] = '';
    $content = '';
    
    if($op == null || $op == 'view')
    {
        $parentid = sget($_REQUEST, 'id');
        $target = sget($_REQUEST, 'target');
        if($parentid)
        {
            $sql = "select * from maenna_questionair where recordid = %d";
            $result = db_query($sql, array($parentid));
            $Row = db_fetch_object($result);
            if($Row)
            {
                $Block['title'] = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
            }
            $target = $Row->target;
            
            $Table = array('title' => 'Professional User List',
                       'class' => 'report',
                       'thead'=> array(),
                       'tbody'=> array());
            $Table['thead'] = array(
                            array('label'=> 'MAENNA ID', 'style' => ''),
                            array('label'=> 'Accessed', 'style' => 'width:100px'),
                            array('label'=> 'View', 'style' => 'width:60px'),
                            
                            );
            if($target == 'company')
            {
                 $Table['title'] = 'Company List';
                 $baseUrl = "/account?a=companies&type=company_detail&companyid=";
            }
            
            $Tbody = array();
            // retrieve user list that has 
            $sql = "select maenna_qa.* from maenna_qa, maenna_questionair where maenna_qa.qid = maenna_questionair.recordid and maenna_questionair.parentid = %d group by maenna_qa.uid order by created desc";
            $result= db_query($sql, array($parentid));
            
            while($Row = db_fetch_object($result))
            {
                $uid = $Row->uid;
                if(empty($uid)) continue;
                $created = $Row->created;
                $url = "<a href='/account?a=information&tab=questionnaire&panel=answerdetail&target=pro&id=$parentid&uid=$uid' >go</a>";
                if($target == 'company'){
                    $maennaid = getProjectName($uid);
                }else{
                    $maennaid = getProId($uid);
                }
                $Tbody[] = array(
                                 'maenna id' => $maennaid,
                                 'accessed' => date('m/d/Y', $created),
                                 'view' => $url
                                 );
            }
            $Table['tbody'] = $Tbody;
            $content .= "<div style='position:absolute;width:100px;right:0;top:0;text-align:right'>
                                <a href='/account?a=information&tab=questionnaire&panel=questionnaire&target=$target' class=button>back</a>
                        </div>";
            $content .= render_table($Table);
        }
    }
    
    
    $Block['body']= content_box($Block['title'], $content);
    return $Block;
}


function answerdetail($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $content = '';
    $Block['title'] = "";
    
    $parentid = sget($_REQUEST, 'id');
    $target = sget($_REQUEST, 'target');
    $maennaUid = sget($_REQUEST, 'uid');
    $maennaRid = userRoleId($maennaUid);
    
    
    if(empty($parentid) || empty($maennaUid)) return '';
    
    
    if($op == 'view' || empty($op))
    {
        $sql = "select * from maenna_questionair where recordid = %d";
        $result = db_query($sql, array($parentid));
        $Row = db_fetch_object($result);
       
        $Block['title'] .= htmlentities($Row->content,ENT_QUOTES | ENT_IGNORE, "UTF-8");
        $target = $Row->target;
    
        if($target == 'company')$Block['title'] .= " (" . getProjectName($maennaUid) . ")";
        else $Block['title'] .=  " (". getProId($maennaUid) . " - " . Options_proType($maennaRid, 1). ")";
       
        if(empty($maennaRid))
        {
            $sql = "select Q.*, A.answer from maenna_questionair as Q
                    left join maenna_qa as A on
                        Q.recordid = A.qid and
                        A.uid = %d
                    where parentid = %d order by weight, recordid";
            $DBValues = array($maennaUid, $parentid);
        }else{
            $sql = "select Q.*, A.answer from maenna_questionair as Q
                    left join maenna_qa as A on
                        Q.recordid = A.qid and
                        A.uid = %d
                    where parentid = %d and
                        (usertype = %d || usertype = 0)
                    order by weight, recordid";
            $DBValues = array($maennaUid, $parentid, $maennaRid);
           //$Block['title'] .= " (" . Options_proType($usertype, 1) . " view)";
        }
        
        $result = db_query($sql, $DBValues);
        
        $page = sget($_REQUEST, 'page');
        if(empty($page)) $page = 1;
        $morePage = false;
        $pageCnt = 1;
        $curPage = $page;
        $preType = '';
        $counter = 1;
        while($Row = db_fetch_object($result)){
            $type = $Row->type;
            
            $questionid = $Row->recordid;
            if($type == 'pagedivider'){
                if($preType != 'pagedivider')$pageCnt++;
                
            }elseif($type == 'title'){
                if($pageCnt == $page)
                {
                    $subtitle = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $content .= "\n<div class='subtitle'>$subtitle</div>";
                }
            }elseif($type == 'question')
            {
                if($pageCnt == $page)
                {
                    
                    $question = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $answer = nl2br(htmlentities($Row->answer,ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                    $content .= "\n<div class='entry'>";
                    $content .= "\n<div class='entry-title'>$counter. $question</div>";
                    $content .= "\n<div class='entry-content'><div style='border:solid 1px #f3f3f3;padding:5px;'><p style='font-family:Signika'>$answer</p></div></div>";
                    $content .= "\n</div>";
                    
                }
                $counter++;
            }elseif($type == 'instruction'){
                if($pageCnt == $page)
                {
                    $instruction = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $content .= "\n<div style='margin-top:8px;font: 13px Signika'>$instruction</div>";
                }
            }elseif($type == 'fileuploader')
            {
                if($pageCnt == $page)
                {
                    $loaderTitle = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $filename = $Row->answer;
                    if(empty($filename)) $file = "N/A";
                    else {
                        $path =  "./" . file_directory_path() . "/" . $filename;
                        $file = "<a href='$path' target='_blank'>$filename</a>";
                    }
                    $content .= "\n<div style='margin-top:20px;'>";
                    $content .= "\n<div style='font:bold 12px Signika;'>$counter. $loaderTitle</div>";
                    $content .= "\n<div style='margin-top:8px;padding:5px;border:solid 1px #f3f3f3'>$file</div>";
                    $content .= "\n</div>";
                    
                }
                $counter++;
            }
            $preType = $type;
        }
        // Page navigation by divider
        $prePage = $nextPage = $submit = '';
        if($page > 1 && $pageCnt > 1){
            $pre= $page -1;
            $prePage = "<a href='/account?a=information&tab=questionnaire&panel=preview&id=$parentid&page=$pre' class=button>previous page</a> &nbsp;&nbsp;&nbsp;";
        }
        if($pageCnt > 1 && $page < $pageCnt ){
            $next = $page + 1;
            $nextPage = "<a href='/account?a=information&tab=questionnaire&panel=preview&id=$parentid&page=$next' class=button>next page</a>&nbsp;&nbsp;&nbsp;";
        }
        if($pageCnt == $page){
            $submit = "<input type=submit name=submit value='Submit' class=button />";
        }else{
            $submit = "<input type=submit name=submit value='save and continue' class=button />";
        }
        // Show view as userType for Pro

        
        $content = <<< END
    
        <form method='post' action='/account' enctype='multipart/form-data'>
            <div style='position:absolute;width:150px;right:0;top:0;text-align:right;'>
                <a href='/account?a=information&tab=questionnaire&target=$target&panel=answeredlist&id=$parentid' class=button>back</a>
            </div>
            <div style='position:relative;width:100%;'>
            $content
            <input type=hidden name=a value=information />
            <input type=hidden name=tab value=questionnaire />
            <input type=hidden name=panel value='preview' />
            <input type=hidden name=id value='$parentid' />
            <input type='hidden' name='page' value='$next' />
                <div style='margin-top:15px;'>$prePage $nextPage</div>
            </div>
        </form>
END;
    }
    
    
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

/* EOF */