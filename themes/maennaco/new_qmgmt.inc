<?php
// $maenna_page['content'] = array('info_list','related_list');

function qmgmt($op = null)
{
    global $user;
    $editorid = $user->uid;
    $id = sget($_REQUEST, 'id');
    
    $target = sget($_REQUEST, 'target');
    if(empty($target)) $target = 'company';
    if($target == 'pro')
    {
        $Block['title'] = 'Professional Questionnaire';
    }
    else
    {
        $Block['title'] = 'Company Questionnaire';
    }
    $content = '';
    $time = time();
    $redirect = rebuild_url(array('tab','page'));
    
    if($op && $op != 'update')
    {
        /*
         $sql = "select     parent.*,
                            count(child.recordid) as cnt,
                            max(uidcnt)  as uids
                from        maenna_questionair as parent
                left join   (SELECT *
                                FROM    `maenna_questionair` 
                                left join(  select  qid ,
                                                    count(uid) as uidcnt
                                            from    maenna_qa
                                            group by qid
                                        ) as qa on qa.qid = maenna_questionair.recordid
                                WHERE `type` = 'question'
                            )as child
                on          parent.recordid = child.parentid 
                where       parent.type = 'version' and
                            parent.target = '$target'
                group by    parent.recordid
                order by    parent.created desc";
        */
        $sql = "select  parent.*,
                        cnt,
                        uids
                from    maenna_questionair as parent
                left join   (
                                select  parentid,
                                        count(distinct recordid) as cnt
                                from    maenna_questionair
                                where   type = 'question'  
                                group by parentid
                            ) as child on child.parentid = parent.recordid
                left join   (
                                select  count(distinct uid) as uids, parentid
                                from    maenna_qa,
                                        maenna_questionair
                                where   maenna_qa.qid = maenna_questionair.recordid
                                group by parentid
                            ) as Answers on Answers.parentid = parent.recordid
                where   parent.type = 'version' and
                        parent.target = '%s'
                order by parent.created desc";
                //echo $sql;
        $result = db_query($sql, array($target));
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
    $redirect=rebuild_url(array('tab','page'));
    if(empty($target)) $target='company';
    $_Status = array('0' => 'pending', '1'=>'live');
    while($Row = db_fetch_object($result))
    {
        $recordid = $Row->recordid;
        $version = $Row->content;
        $created = date('m/d/Y', $Row->created);
        $details = "<a href='$redirect&target=$target&panel=qdetail&id=$recordid'>details</a>";
        $edit = "<a href=''>edit</a>";
        $remove = "<a href=''>remove</a>";
        
       
        if(empty($Row->cnt)) $numOfQ = 0;
        else $numOfQ = $Row->cnt;
        $option_status = "<select name='status' table='maenna_question1' idname='recordid' idval='$recordid' column='status' class='ajaxTrigger'>" .option_code($_Status, $Row->status) . "</select>";
        $answered = $Row->uids;
        if($answered > 0){
            $answered = "<a href='$redirect&panel=answeredlist&target=$target&id=$recordid'>$answered</a>";
        }
if ($target == 'company') {
	 $cmp_type= Options_cmpType($Row->usertype,1);
        $Versions[] = array(
                            'version' => $version,
				'company type' => $cmp_type,
                            '# of questions' => $numOfQ,
                            'created on' => $created,
                            'details' => $details,
                            'edit' => "<a href='$redirect&panel=new_questionnaire&target=$target&id=$recordid'>edit</a>",
                            'remove' => "<a onclick='return confirm(\"Continue to remove selected questionnaire template\")' href='$redirect&panel=qmgmt&update_section=questionnaire&id=$recordid&target=$target'>remove</a>",
                            'status' => $option_status,
                            'preview' => "<center><a href='$redirect&panel=preview&_page=1&id=$recordid'>go &#187;<center></a>",
                            'answered' => "<center>$answered</center>"
                            ); 
			      }
else {
	 $pro_type= Options_proType($Row->usertype,1);
	 $Versions[] = array(
                            'version' => $version,
				'pro type' => $pro_type,
                            '# of questions' => $numOfQ,
                            'created on' => $created,
                            'details' => $details,
                            'edit' => "<a href='$redirect&panel=new_questionnaire&target=$target&id=$recordid'>edit</a>",
                            'remove' => "<a onclick='return confirm(\"Continue to remove selected questionnaire template\")' href='$redirect&panel=qmgmt&update_section=questionnaire&id=$recordid&target=$target'>remove</a>",
                            'status' => $option_status,
                            'preview' => "<center><a href='$redirect&panel=preview&_page=1&id=$recordid'>go &#187;<center></a>",
                            'answered' => "<center>$answered</center>"
                            ); 
			      }

    }
    
    if(count($Versions) == 0)
    {
        
        $html = "No Questionnaire was found. ";
    }else{
        $Table = array('title' => 'Questionnaire List',
                       'class' => 'report',
                       'thead'=> array(),
                       'tbody'=> array());
	if ($target == 'company') {
        $Table['thead'] = array(
                            array('label'=> 'version', 'style' => ''),
				array('label'=> 'company type', 'style' => ''),
                            array('label'=> '# of questions', 'style' => 'width:100px'),
                            array('label'=> 'created on', 'style' => 'width:60px'),
                            array('label'=> 'details', 'style' => 'width:60px'),
                            array('label'=> 'edit', 'style' => 'width:40px'),
                            array('label'=> 'remove', 'style' => 'width:50px'),
                            array('label'=> 'status', 'style' => 'width:60px'),
                            array('label'=> 'preview', 'style' => 'width:50px;text-align:center'),
                            array('label'=> 'answered', 'style' => 'width:50px;text-align:center'),
                            );
} else  {
        $Table['thead'] = array(
                            array('label'=> 'version', 'style' => ''),
                            array('label'=> 'Pro type', 'style' => ''),
                            array('label'=> '# of questions', 'style' => 'width:100px'),
                            array('label'=> 'created on', 'style' => 'width:60px'),
                            array('label'=> 'details', 'style' => 'width:60px'),
                            array('label'=> 'edit', 'style' => 'width:40px'),
                            array('label'=> 'remove', 'style' => 'width:50px'),
                            array('label'=> 'status', 'style' => 'width:60px'),
                            array('label'=> 'preview', 'style' => 'width:50px;text-align:center'),
                            array('label'=> 'answered', 'style' => 'width:50px;text-align:center'),
                            );
}
        $Table['tbody'] = $Versions;
        $html = render_table($Table);
        $html .= js_init("init_ajaxTrigger();");
    }
    $html .= "<br>" .
        "Click <a href='$redirect&panel=new_questionnaire&target=$target'>here</a> to add one.";
    
    return $html;
}

function new_questionnaire($op)
{
    global $user;
    $editorid = $user->uid;
    $redirect = rebuild_url(array('tab','page'));
    
    $target = sget($_REQUEST, 'target');
    if(empty($target)) $target='company';
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
    
    if($op && $op != 'update')
    {
	 if ($target == 'company') {

	$option_cmptype = Options_cmpType();

	$type_str = " <div class=row style='padding:5px 0;border:none;'>
                
                Company Type: <select name=usrtype><option></option>$option_cmptype</select>
            </div>";
}
	 else {
		$option_usertype = Options_proType();
		$type_str = " <div class=row style='padding:5px 0;border:none;'>User Type: <select name=usrtype><option></option>$option_usertype</select>
            </div>";
}
        $content = <<< END
        <form action='/account' method='post'>
            <div class=row style='padding:5px 0;border:none'>
                <label>Please enter a Questionnaire version. (eg a year)</label><br>
            </div>
            <div class=row style='padding:5px 0;border:none;'>
                <input type=text name=version value='$version' style='width:300px;' />
            </div>
	     $type_str
            <div class=row>
                <input type=submit name=submit value='submit' class='button' />
            </div>
            <input type=hidden name=tab value=information />
            <input type=hidden name=page value=qmgmt />
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
        if(empty($target)) $target='company';
        $content = sget($_REQUEST, 'version');
        $created = time();
        $recordid = sget($_REQUEST, 'id');
	 $usertype = sget($_REQUEST, 'usrtype');        
	
	if(empty($type) || empty($target) || empty($content) || empty($created))
        {
            drupal_set_message("missing information", 'error');
        }else{
          
            if(empty($recordid))
            {
                $sql = "insert into maenna_questionair (target, type, usertype, content, created, editorid) values ('%s','%s','%s','%s','%s', %d)";
                $DBValues = array($target, $type, $usertype, $content, $created, $editorid);
            }else{
                $sql = "update maenna_questionair set content = '%s',editorid = %d, usertype='%s' where recordid = %d limit 1";
                $DBValues = array($content, $editorid,$usertype,$recordid);
            }
            if(db_query($sql,$DBValues) !== false){
			 drupal_set_message("Questionnaire template was successfully edited");

            } else{
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
    if(empty($target))$target='company';
    $redirect = rebuild_url(array('tab','page'));
    $parentid = sget($_REQUEST, 'id');
    if(empty($parentid)) return '';
    
    if($op && $op != 'update')
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
                        <a href='$redirect&panel=qmgmt&target=$target' class='button'>back to list</a>
                            &nbsp;&nbsp;&nbsp;
                        <a href='$redirect&target=$target&panel=qdetail&id=$parentid' class='button'>refresh</a>
                            &nbsp;&nbsp;&nbsp;
                        <a href='$redirect&panel=preview&page=1&id=$parentid' class='button'>preview</a>
                    </div>
                    <div style='position:relative;width:100%;text-align:right'>
                    
                    <a href='$redirect&target=$target&panel=qdetail&id=$parentid&add=question' class=button>add question</a>
                        &nbsp;&nbsp;&nbsp;
                    <a href='$redirect&target=$target&panel=qdetail&id=$parentid&add=title' class=button>add subtitle</a>
                        &nbsp;&nbsp;&nbsp;
                    <a href='$redirect&target=$target&panel=qdetail&id=$parentid&add=instruction' class=button>add instruction</a>
                        &nbsp;&nbsp;&nbsp;
                    <a href='$redirect&target=$target&panel=qdetail&id=$parentid&add=fileuploader' class=button>add file uploader</a>
                        &nbsp;&nbsp;&nbsp;
                    <a href='$redirect&target=$target&panel=qdetail&id=$parentid&do=pagedivider&update_section=qdetail' class=button>add page divider</a>
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
        if(empty($target)) $target='company';
        $type= sget($_REQUEST, 'type'); // question or title
        $weight = sget($_REQUEST, 'weight');
        if (isset($_REQUEST['usertype'])) { $usertype = sget($_REQUEST, 'usertype');}
        elseif (isset($_REQUEST['cmptype'])) { $usertype = sget($_REQUEST, 'cmptype');}
        else $usertype = '0';
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
            $sql_str = "%d, '%s', '%s', '%s', '%s','%s', %d, %d";
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
    if(empty($target)) $target='company';
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
//   if($target == 'pro')
//        {
//            $option_usertype = Options_proType($usertype);
//            $tr2 = "\n<tr><td>User Type:</td><td><select name=usertype><option></option>$option_usertype</select></td></tr>";
//        }
//	 else 
//        {
//                $option_cmptype = Options_cmpType($usertype);
//                $tr2 = "\n<tr><td>Company Type:</td><td><select name=cmptype><option></option>$option_cmptype</select></td></tr>";  
//        }
    } else {
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
//                    if($target == 'pro')
//        {
//            $option_usertype = Options_proType($usertype);
//            $tr2 = "\n<tr><td>User Type:</td><td><select name=usertype><option></option>$option_usertype</select></td></tr>";
//        }
//	 else 
//        {
//                $option_cmptype = Options_cmpType($usertype);
//                $tr2 = "\n<tr><td>Company Type:</td><td><select name=cmptype><option></option>$option_cmptype</select></td></tr>";  
//        }    
}
    $sql = "select weight from maenna_questionair where parentid = %d order by weight desc limit 1";
    $result = db_query($sql, array($parentid));
    $Row = db_fetch_object($result);
    $weight = $Row->weight;
    if(empty($weight)) $weight = 1;
    $weight += 5;
    $html = <<< END
    <div style='font:italic bold 14px Helvetica'>$title</div>
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
        <input type=hidden name=tab value=information />
        <input type=hidden name=page value='qmgmt' />
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
    $redirect = rebuild_url(array('tab','page'));
    $Table = array('title' => '',
                       'class' => 'report',
                       'thead'=> array(),
                       'tbody'=> array());
    $Table['thead'] = array(
                            array('label'=> 'Num.', 'style' => 'width:20px;'),
                            array('label'=> 'type', 'style' => 'width:100px'),
				array('label'=> 'company type', 'style' => 'width:60px'),
                            array('label'=> 'content', 'style' => ''),
                            array('label'=> 'edit', 'style' => 'width:60px'),
                            array('label'=> 'remove', 'style' => 'width:60px'),
                            array('label'=> 'weight', 'style' => 'width:60px'),
                            
                            );
    $target = sget($_REQUEST, 'target');
    if(empty($target)) $target='company';
    if($target == 'pro')
    {
        $Table['thead'] = array(
                            array('label'=> 'Num.', 'style' => 'width:20px;'),
                            array('label'=> 'type', 'style' => 'width:60px'),
                            array('label'=> 'user type', 'style' => 'width:90px'),
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
	else {
		
		$companytype = Options_cmpType($Row->usertype,1);
	}
        $edit = "<a href='$redirect&target=$target&panel=qdetail&id=$parentid&edit=$type&recordid=$recordid'>edit</a>";
        if($Row->type == 'pagedivider')$edit = '';
        $Tbody[] = array(
                         'num.' => $num,
                         'type' => $Row->type,
                         'user type' => $usertype,
			    'company type' => $companytype,
                         'content' => htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8"),
                         'edit' => $edit,
                         'remove' => "<a href='$redirect&target=$target&panel=qdetail&id=$parentid&update_section=qdetail&recordid=$recordid&do=remove' onclick='confirm(\"Continue to remove the record\");'>remove</a>",
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
    $redirect = rebuild_url(array('tab','page'));
    $parentid = sget($_REQUEST, 'id');
    $usertype = sget($_REQUEST, 'usertype');
    if(empty($parentid)) return '';
    
    
    if($op && $op != 'update')
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
        
        $_page = sget($_REQUEST, '_page');
        if(empty($_page)) $_page = 1;
        $morePage = false;
        $pageCnt = 1;
        $curPage = $_page;
        $preType = '';
        $counter = 1;
        while($Row = db_fetch_object($result)){
            $type = $Row->type;
            
            $questionid = $Row->recordid;
            if($type == 'pagedivider'){
                if($preType != 'pagedivider')$pageCnt++;
                
            }elseif($type == 'title'){
                if($pageCnt == $_page)
                {
                    $subtitle = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $content .= "\n<div style='margin-top:25px;font:bold italic 13px tahoma;'>$subtitle</div>";
                }
            }elseif($type == 'question')
            {
                if($pageCnt == $_page)
                {
                    
                    $question = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $answer = $Row->answer;
                    $content .= "\n<div style='margin-top:20px;'>";
                    $content .= "\n<div style='font:bold 12px Helvetica;'>$counter. $question</div>";
                    $content .= "\n<div style='margin-top:8px;'><textarea name='q_$questionid' style='width:99%;height:90px;'>$answer</textarea></div>";
                    $content .= "\n</div>";
                    $counter++;
                }
            }elseif($type == 'instruction'){
                if($pageCnt == $_page)
                {
                    $instruction = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $content .= "\n<div style='margin-top:8px;font:italic 12px Helvetica'>$instruction</div>";
                }
            }elseif($type == 'fileuploader')
            {
                if($pageCnt == $_page)
                {
                    $loaderTitle = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $content .= "\n<div style='margin-top:20px;'>";
                    $content .= "\n<div style='font:bold 12px Helvetica;'>$counter. $loaderTitle</div>";
                    $content .= "\n<div style='margin-top:8px;'><input type=file name='f_$questionid' /></div>";
                    $content .= "\n</div>";
                    $counter++;
                }
            }
            $preType = $type;
        }
        // Page navigation by divider
        $prePage = $nextPage = $submit = '';
        if($_page > 1 && $pageCnt > 1){
            $pre= $_page -1;
            $prePage = "<a href='$redirect&panel=preview&id=$parentid&page=$pre' class=button>previous page</a> &nbsp;&nbsp;&nbsp;";
        }
        if($pageCnt > 1 && $_page < $pageCnt ){
            $next = $_page + 1;
            $nextPage = "<a href='$redirect&panel=preview&id=$parentid&page=$next' class=button>next page</a>&nbsp;&nbsp;&nbsp;";
        }
        if($pageCnt == $_page){
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
                    <input type=hidden name=tab value=information />
                    <input type=hidden name=page value=qmgmt />
                    <input type=hidden name=panel value=preview />
                    <input type=hidden name='_page' value=1 />
                    <input type=hidden name=id value='$parentid' />
                </form>
                
            </div>
END;
        }else $userSelect = '';
        
        $content = <<< END
        $userSelect
        <form method='post' action='/account' enctype='multipart/form-data'>
            <div style='position:absolute;width:150px;right:0;top:0;text-align:right;'>
                <a href='$redirect&target=$target&panel=qdetail&id=$parentid' class=button>back to details</a>
            </div>
            <div style='position:relative;width:100%;'>
            $content
            <input type=hidden name=tab value=information />
            <input type=hidden name=page value=qmgmt />
            <input type=hidden name=panel value='preview' />
            <input type=hidden name=id value='$parentid' />
            <input type='hidden' name='_page' value='$next' />
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
    $redirect = rebuild_url(array('tab','page'));
    $Block['title'] = '';
    $content = '';
    
    if($op && $op != 'update')
    {
        $parentid = sget($_REQUEST, 'id');
        $target = sget($_REQUEST, 'target');
        if(empty($target)) $target='company';
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
                            array('label'=> 'View/Edit', 'style' => 'width:80px'),
                            );
            if($target == 'company')
            {
                 $Table['title'] = 'Company List';
                 $baseUrl = "/account?tab=companies&type=company_detail&companyid=";
            }
            
            $Tbody = array();
            // retrieve user list that has 
            $sql = "select  maenna_qa.*
                    from    maenna_qa,
                            maenna_questionair
                    where   maenna_qa.qid = maenna_questionair.recordid and
                            maenna_questionair.parentid = %d
                    group by maenna_qa.uid
                    order by created desc";
            $result= db_query($sql, array($parentid));
            
            while($Row = db_fetch_object($result))
            {
                $uid = $Row->uid;
                if(empty($uid)) continue;
                $created = $Row->created;
                $url = "<a href='$redirect&panel=answerdetail&target=$target&id=$parentid&uid=$uid' >go</a>";
                if($target == 'company'){
                    $maennaid = getProjectName($uid);
                }else{
                    $maennaid = getProId($uid);
                }
                $Tbody[] = array(
                                 'maenna id' => $maennaid,
                                 'accessed' => date('m/d/Y', $created),
                                 'view/edit' => $url
                                 );
            }
            $Table['tbody'] = $Tbody;
            $content .= "<div style='position:absolute;width:100px;right:0;top:0;text-align:right'>
                                <a href='$redirect&panel=qmgmt&target=$target' class=button>back</a>
                        </div>";
            $content .= render_table($Table);
        }
    }
    
    
    $Block['body']= content_box($Block['title'], $content);
    return $Block;
}


function answerdetail($op = null)
{
    require_once("." . base_path() . path_to_theme() . "/includes/classes/connections.inc");
    global $user, $AccessObj;
    $editorid = $user->uid;
    $time = time();
    $content = '';
    $Block['title'] = "";
    $redirect = rebuild_url(array('tab','page'));
    $parentid = sget($_REQUEST, 'id');
    $target = sget($_REQUEST, 'target');
    if(empty($target)) $target='company';
    $maennaUid = sget($_REQUEST, 'uid');
    $maennaRid = userRoleId($maennaUid);
    
    if($AccessObj->user_type == 'admin')
    {
       if(! Connections::admin_of($maennaUid)) return '';
    }
    if(empty($parentid) || empty($maennaUid)) return '';
    
    
    if($op && $op != 'update')
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
            $sql = "select Q.*, A.answer,A.qaid from maenna_questionair as Q
                    left join maenna_qa as A on
                        Q.recordid = A.qid and
                        A.uid = %d
                    where parentid = %d order by weight, recordid";
            $DBValues = array($maennaUid, $parentid);
        }else{
            $sql = "select Q.*, A.answer ans,A.qaid  from maenna_questionair as Q
                    left join maenna_qa as A on
                        Q.recordid = A.qid and
                        A.uid = %d
                    where parentid = %d
                    order by weight, recordid";
            $DBValues = array($maennaUid, $parentid);
           //$Block['title'] .= " (" . Options_proType($usertype, 1) . " view)";
        }
        
        $result = db_query($sql, $DBValues);
        
        $_page = sget($_REQUEST, '_page');
        if(empty($_page)) $_page = 1;
        $morePage = false;
        $pageCnt = 1;
        $curPage = $_page;
        $preType = '';
        $counter = 1;
        
        $edit_link = rebuild_url(array('tab','page','id', 'uid', 'target'));
        while($Row = db_fetch_object($result)){
            $type = $Row->type;
            
            $questionid = $Row->recordid;
            if($type == 'pagedivider'){
                if($preType != 'pagedivider')$pageCnt++;
                
            }elseif($type == 'title'){
                if($pageCnt == $_page)
                {
                    $subtitle = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $content .= "\n<div class='subtitle'>$subtitle</div>";
                }
            }elseif($type == 'question')
            {
                if($pageCnt == $_page)
                {
                    $Qid = $Row->recordid;
                    $Aid = $Row->qaid;
                    $question = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $answer = nl2br(htmlentities($Row->ans,ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                    $content .= "\n<div class='entry'>";
                    $content .= "\n<div class='entry-title'>
                                        <a name='t_$Qid'></a>
                                        <div style='float:right;
                                                    margin-left:20px;
                                                    margin-top:4px;
                                                    width:30px;
                                                    border-left:solid 1px #666;
                                                    height:10px;
                                                    line-height:8px;
                                                    padding-left:8px;'>
                                                    <a href='$edit_link&panel=qqedit&qid=$Qid&aid=$Aid' class=tool>
                                                        EDIT
                                                    </a>
                                        </div>
                                        $counter. $question
                                    </div>";
                    $content .= "\n<div class='entry-content' style='clear:both'><div style='border:solid 1px #f3f3f3;padding:5px;'><p style='font-family:Helvetica'>$answer</p></div></div>";
                    $content .= "\n</div>";
                    
                }
                $counter++;
            }elseif($type == 'instruction'){
                if($pageCnt == $_page)
                {
                    $instruction = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $content .= "\n<div style='margin-top:8px;font: 13px Helvetica'>$instruction</div>";
                }
            }elseif($type == 'fileuploader' && 0)
            {
                if($pageCnt == $_page)
                {
                    $loaderTitle = htmlentities($Row->content, ENT_QUOTES | ENT_IGNORE, "UTF-8");
                    $filename = $Row->answer;
                    if(empty($filename))
                    {
                        $file = "N/A"; continue;
                    }
                    else {
                        $path =  "./" . file_directory_path() . "/" . $filename;
                        $file = "<a href='$path' target='_blank'>$filename</a>";
                    }
                    $content .= "\n<div style='margin-top:20px;'>";
                    $content .= "\n<div style='font:bold 12px Helvetica;'>$counter. $loaderTitle</div>";
                    $content .= "\n<div style='margin-top:8px;padding:5px;border:solid 1px #f3f3f3;width:550px;overflow:hidden;'>$file</div>";
                    $content .= "\n</div>";
                    
                }
                $counter++;
            }
            $preType = $type;
        }
        $sql = "select * from maenna_qa where qid = %d and uid = %d and qtype != 'question' order by qaid";
       
        $result = db_query($sql, array($parentid, $maennaUid));

        while($Row = db_fetch_object($result) && 0)
        {
            $file_title = htmlentities(strtoupper($Row->filename), ENT_QUOTES);
            
            $file = $Row->answer;
            $path =  "./" . file_directory_path() . "/" . $file;
            $content .= "\n<div style='margin-top:20px;'>";
                    $content .= "\n<div style='font:bold 12px Helvetica;'>$counter</div>";
                    $content .= "\n<div style='margin-top:8px;padding:5px;border:solid 1px #f3f3f3;width:550px;overflow:hidden;'>
                                        <a href='$path' target='_blank'>$file_title</a></div>";
                    $content .= "\n</div>";
        }
        // Page navigation by divider
        $prePage = $nextPage = $submit = '';
        if($_page > 1 && $pageCnt > 1){
            $pre= $_page -1;
            $prePage = "<a href='$redirect&panel=preview&id=$parentid&page=$pre' class=button>previous page</a> &nbsp;&nbsp;&nbsp;";
        }
        if($pageCnt > 1 && $_page < $pageCnt ){
            $next = $_page + 1;
            $nextPage = "<a href='$redirect&panel=preview&id=$parentid&page=$next' class=button>next page</a>&nbsp;&nbsp;&nbsp;";
        }
        if($pageCnt == $_page){
            $submit = "<input type=submit name=submit value='Submit' class=button />";
        }else{
            $submit = "<input type=submit name=submit value='save and continue' class=button />";
        }
        // Show view as userType for Pro
        if($AccessObj->user_type == 'super')
        {
            $back_url = "$redirect&target=$target&panel=answeredlist&id=$parentid";
        }elseif($AccessObj->user_type = 'admin')
        {
            if($target == 'company')
                $back_url = "/account?tab=companies&page=company_detail&id=$maennaUid";
            else{
                $back_url = "/account?tab=professionals&page=pro_detail&id=$maennaUid";
            }
        }
        
        $content = <<< END
    
        <form method='post' action='/account' enctype='multipart/form-data'>
            <div style='position:absolute;width:150px;right:0;top:0;text-align:right;'>
                <a href='$back_url' class=button>back</a>
            </div>
            <div style='position:relative;width:100%;'>
            $content
            <input type=hidden name=tab value=information />
            <input type=hidden name=page value=qmgmt />
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

function qqedit($op = null)
{
    global $user, $AccessObj;
    $editorid = $user->uid;
    $time = time();
    $content = '';
    $Block['title'] = "";
    //tab=information&page=qmgmt&panel=answerdetail&target=company&id=29&uid=175
    $redirect = rebuild_url(array('tab','page','target','id','uid'));
   
    
    //tab=information&page=qmgmt&id=29&uid=175&target=company&panel=qqedit&qid=31&aid=16
    $uid = sget($_REQUEST, 'uid');
    $qid = sget($_REQUEST, 'qid');
    $qaid = sget($_REQUEST, 'aid');
    $target = sget($_REQUEST, 'target');
    if($op && $op != 'update')
    {
        $Block['title'] = 'EDIT QUESTIONNAIRE - ';
        if($target == 'company')$Block['title'] .= getProjectName($uid);
        else $Block['title'] .= getProId($uid);
        
        
        
        $sql = "select * from maenna_questionair where recordid = %d";
        $result = db_query($sql, array($qid));
        $Row = db_fetch_object($result);
        if($Row)
        {
            $question = $Row->content;
            $question =  htmlentities(strtoupper($Row->content), ENT_QUOTES | ENT_IGNORE, "UTF-8");
        }else
        {
            drupal_set_message("Invalid QID");
            return;
        }
        $answer = '';
        if($qaid)
        {
            $sql = "select * from maenna_qa where qaid = %d";
            $result = db_query($sql, array($qaid));
            $Row = db_fetch_object($result);
            if($Row)
            {
                $answer = $Row->answer;
            }
        }
        $HV = hidden_post_values(array('tab','page', 'id', 'uid', 'target', 'panel', 'qid', 'aid'));
        $content = <<< END
            <div class=row>$question</div>
            <form method=post action='/account'>
                <table class='table-list'>
                    <tr>
                        <td>
                            <textarea name='answer' style='width:100%;height:200px;'>$answer</textarea>
                        </td>
                    </tr>
                </table>
                $HV
                <input type=hidden name=update_section value=qqedit />
                <input type=submit name=submit value=submit class=button />  <a href='$redirect&panel=answerdetail#t_$qid' class=button>back</a>
            </form>
END;
    }elseif($op == 'update')
    {
        $answer = sget($_REQUEST, 'answer');
        if($qaid)
        {
            $sql = "update maenna_qa set answer = '%s' where qaid = %d limit 1";
            $DBValues = array($answer, $qaid);
        }else
        {
            $sql = "insert into maenna_qa (qid,qtype, uid, answer,created ) values (%d, '%s', %d, '%s', '%s')";
            $DBValues = array($qid, 'question', $uid, $answer, $time);
        }
        if(db_query($sql, $DBValues))
        {
            drupal_set_message("Record updated");
        }else
        {
            drupal_set_message("Operation Failed",'error');
        }
        return;
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}


/* EOF */