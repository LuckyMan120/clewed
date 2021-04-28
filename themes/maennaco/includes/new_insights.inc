<?php
global $redirect;
$redirect = rebuild_url(array('tab', 'page'));
function pro_tabs()
{
	global $base_url;
	$page = sget($_REQUEST, 'page');
	$type = sget($_REQUEST, 'type');
	$id = sget($_REQUEST, 'id');	
	$content = '<div class="account-section-tabs">
            <ul>
            </ul>
        </div>';
   echo $content;
}
function list_insights($op = null)
{
	global $user;
    global $AccessObj;
	$user_type = $AccessObj->user_type;
    $editorid = $user->uid; // the currently logged in user, print_r this object to see all data available
    $companyid = sget($_REQUEST, 'id');
    $content = '';
    global $redirect;
    $data_type = 'discussions';
    $panel  = 'multi';
    $section=__FUNCTION__;
    $previleges = array('super', 'admin');
	if(in_array($user_type, $previleges)) {
		$Block['title'] = 'INSIGHTS';
	} elseif($user_type == 'people') {
        $open_insights = (!empty($_POST['openinsights']) || !empty($_GET['openinsights'])) ? true : false;
        $open_services = (!empty($_POST['openservices']) || !empty($_GET['openservices'])) ? true : false;
		$Block['title'] = 'INSIGHTS 
        <div class="schedule">
            <div class="sched">
                <span id="questmark" class="insights" style="float: right;">
                    <a href="#" id="get-help-btn2">
                        <img src="/themes/maennaco/images/questionmark_white.png" style="padding-top:5px; margin-left: 6px;">
                    </a>
                </span>
            </div>
        </div>
      
        <div class="schedule" id="insightTab" ' . ($open_insights ? 'style="display: none;"' : '') . '>
            <div class="sched">
                <div class="discmnts"></div>
                <a href="/account?tab=professionals&page=pro_detail&id='.$AccessObj->uid.'&section=pro_industry_view&type=discussion&openinsights=1">
                    <span style="text-transform:none !important;" data-tooltip="Share your expertise by creating your own Insights. Click the ? on the right for guidelines.">
                        Create Insight
                    </span>
                </a>
            </div>
        </div>';
	} else {
		$Block['title'] = 'INSIGHTS';
	}
		
		
    if (sget($_REQUEST, 'name')) {
	    $Block['title'] = sget($_REQUEST, 'name');
		$fname = sget($_REQUEST, 'name');
    }

	ob_start();
        $url = "http://" .$_SERVER['HTTP_HOST'];
	$editorname = $user->name;
	require_once("pro_comments.php");
	$content = ob_get_contents();
	ob_end_clean();
	
    //$content = sget($_REQUEST, 'file');

    $Block['body'] = content_box($Block['title'], $content, 'blue_bar');
    return $Block;
}

function myfollowing($op = null)
{
	global $user;
    global $AccessObj;
	$user_type = $AccessObj->user_type;
    $editorid = $user->uid; // the currently logged in user, print_r this object to see all data available
    $companyid = sget($_REQUEST, 'id');
    $content = '';
    global $redirect;
    $data_type = 'discussions';
    $panel  = 'multi';
    $section=__FUNCTION__;
	$page = sget($_REQUEST, 'page');
	$type = sget($_REQUEST, 'type');
	$id = sget($_REQUEST, 'id');
	if($type == 'following')
		$title = 'FOLLOWING';  
	else if($type == 'registered')
		$title = 'MY ORDERS';
	else if($type == 'moderated')
		$title = 'Guest expert';

		$Block['title'] = $title;	
    if (sget($_REQUEST, 'name')) {
	    $Block['title'] = sget($_REQUEST, 'name');
		$fname = sget($_REQUEST, 'name');
    }

	ob_start();
        $url = "http://" .$_SERVER['HTTP_HOST'];
	$editorname = $user->name;
	require_once("myfollowing.php");
	$content = ob_get_contents();
	ob_end_clean();

    $Block['body'] = content_box($Block['title'], $content,"blue_bar");
    return $Block;
}
function info_list($op = null){
    global $user,$redirect;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();
    
    if($op && $op != 'update')
    {
        $sql = "select * from maenna_insights  where infotype != 'researchart' and status = 'approved' order by created desc limit 3";
        $result = db_query($sql);
        $Table = array('title' => 'Shared Advisory Information ',
                       'class' => 'report',
                       'thead'=> array(),
                       'tbody'=> array());
        $Table['thead'] = array(
                                array('label'=> 'title', 'style' => ''),
                                array('label'=> 'topic', 'style' => 'width:200px'),
                                array('label'=> 'date', 'style' => 'width:70px'),
                                array('label'=> 'source', 'style' => 'width:80px'),
                                array('label'=> 'rating', 'style' => 'width:40px'),
                                array('label'=> 'selection', 'style' => 'width:60px'),
                                );
        
        while(($Row = db_fetch_array($result))){
            extract($Row);
            
            $title = htmlentities($title, ENT_QUOTES);
            if($inputtype == 'file'){
                $data = "/" . file_directory_path() . "/" . $data;
                $title = "<a href='$data' target='_blank'>$title</a>";
            }elseif($inputtype == 'url'){
                $title = "<a href='$data' target='_blank'>$title</a>";
            }
            $topic = Options_infoTopic($topic, 1);
            $type = Options_infoType($infotype, 1);
            $industry = Options_industry($industry, 1);
            $created = date('m/d/Y', $created);
            $postedby = getProId($postedby);
            $option_rating = Options_rating($rating);
            $option_rating = "<select name='rating' class='ajaxTrigger' table='maenna_insights' column='rating' idname='insightid' idval='$insightid'><option></option>$option_rating</select>";
            $option_selection = Options_selection($selection);
            $option_selection = "<select name='selection' class='ajaxTrigger' table='maenna_insights' column='selection' idname='insightid' idval='$insightid' >$option_selection</select>";
            $Table['tbody'][] = array(
                                      'title' => $title,
                                      'topic' => $topic,
                                      'type'  => $type,
                                      'industry' => $industry,
                                      'date' => $created,
                                      'source' => $postedby,
                                      'rating' => $option_rating,
                                      'selection' => $option_selection
                                      );
        }
        
        $content = render_table($Table);
        $content .= button_link(array(
                                    array('label'=>'more &raquo;','url'=>"$redirect&panel=insights_panel&section=advisory" )
                                    ),'right');
    
        $content .= js_init('init_ajaxTrigger(); ');
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function related_list($op = null){
    global $user,$redirect;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();
    
    if($op && $op != 'update')
    {
        $sql = "select * from maenna_insights  where infotype = 'researchart' and status = 'approved' order by created desc limit 3";
        $result = db_query($sql);
        $Table = array('title' => 'Contributed articles & research',
                       'class' => 'report',
                       'thead'=> array(),
                       'tbody'=> array());
        $Table['thead'] = array(
                                array('label'=> 'title', 'style' => ''),
                                array('label'=> 'topic', 'style' => 'width:200px'),
                                /*array('label'=> 'type', 'style' => 'width:120px'),
                                array('label'=> 'industry', 'style' => 'width:120px'),*/
                                array('label'=> 'date', 'style' => 'width:70px'),
                                array('label'=> 'source', 'style' => 'width:80px'),
                                array('label'=> 'rating', 'style' => 'width:40px'),
                                array('label'=> 'selection', 'style' => 'width:60px'),
                                );
        
        while(($Row = db_fetch_array($result))){
            extract($Row);
            
            $title = htmlentities($title, ENT_QUOTES);
            if($inputtype == 'file'){
                $data = "/" . file_directory_path() . "/" . $data;
                $title = "<a href='$data' target='_blank'>$title</a>";
            }elseif($inputtype == 'url'){
                $title = "<a href='$data' target='_blank'>$title</a>";
            }
            $topic = Options_infoTopic($topic, 1);
            $type = Options_infoType($infotype, 1);
            $industry = Options_industry($industry, 1);
            $created = date('m/d/Y', $created);
            $postedby = getProId($postedby);
            $option_rating = Options_rating($rating);
            $option_rating = "<select name='rating'         class='ajaxTrigger'
                                                            table='maenna_insights'
                                                            column='rating'
                                                            idname='insightid'
                                                            idval='$insightid' ><option></option>$option_rating</select>";
            $option_selection = Options_selection($selection);
            $option_selection = "<select name='selection'   class='ajaxTrigger'
                                                            table='maenna_insights'
                                                            column='selection'
                                                            idname='insightid'
                                                            idval='$insightid' >$option_selection</select>";
            $Table['tbody'][] = array(
                                      'title' => $title,
                                      'topic' => $topic,
                                      'type'  => $type,
                                      'industry' => $industry,
                                      'date' => $created,
                                      'source' => $postedby,
                                      'rating' => $option_rating,
                                      'selection' => $option_selection
                                      );
        }
        
        $content = render_table($Table);
        $content .= button_link(array(
                                    array('label'=>'more &raquo;','url'=>"$redirect&panel=insights_panel&section=research" )
                                    ),'right');
        
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}
function insights_panel($op = null)
{
    global $user,$redirect;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();
    if($op && $op != 'update')
    {
        // section values: selection, advisory, research
        
        $section = sget($_REQUEST, 'section'); 
        $tableTitle = '';
        $SearchKeys = array('section','selection', 'topic', 'infotype', 'industry');
        $SQL_WHERE = array();
        $ORDER_BY = array();
        foreach($SearchKeys as $key){
            $val = sget($_REQUEST, $key);
            if(! empty($val))
            {
                if($key == 'section')
                {
                    if($val == 'advisory') {
                        $SQL_WHERE["infotype"] = "(infotype ='advisorytra' or infotype = 'prefessionalint')";
                        $tableTitle = "Shared Advisory Information ";
                    }
                    elseif($val == 'research')
                    {
                        $SQL_WHERE["infotype"] = "infotype = 'researchart'";
                        $tableTitle = "Contributed articles & research ";
                    }
                    elseif($val == 'selection') {
                        $SQL_WHERE["selection"] = "selection=1";
                        $tableTitle = "Maenna Selections";
                        $ORDER_BY[] = 'selectiontime desc';
                    }
                        
                }else{
                    $SQL_WHERE["$key"] = "infotype = '$val'";
                }
            }
        }
        $SQL_WHERE["status"] = "status = 'approved'";
        $Table = array('title' => $tableTitle,
                       'class' => 'report',
                       'thead'=> array(),
                       'tbody'=> array());
        $Table['thead'] = array(
                                array('label'=> 'title', 'style' => ''),
                                array('label'=> 'topic', 'style' => 'width:120px'),
                                array('label'=> 'type', 'style' => 'width:120px'),
                                array('label'=> 'industry', 'style' => 'width:120px'),
                                array('label'=> 'date', 'style' => 'width:70px'),
                                array('label'=> 'source', 'style' => 'width:80px'),
                                array('label'=> 'rating', 'style' => 'width:40px'),
                                array('label'=> 'selection', 'style' => 'width:60px'),
                                array('label'=> 'status', 'style' => 'width:80px'),
                                );
        
        $ORDER_BY[] = "created desc";
        $strOrderby = implode(',', $ORDER_BY);
        if(count($SQL_WHERE) > 0){
            $strSqlWhere = ' where ' . implode(' and ', $SQL_WHERE);
        }
        $sql = "select * from maenna_insights $strSqlWhere order by $strOrderby";
        $result = db_query($sql);
        while(($Row = db_fetch_array($result)) != false)
        {
            extract($Row);
            $title = htmlentities($title, ENT_QUOTES);
            if($inputtype == 'file'){
                $data = "/" . file_directory_path() . "/" . $data;
                $title = "<a href='$data' target='_blank'>$title</a>";
            }elseif($inputtype == 'url'){
                $title = "<a href='$data' target='_blank'>$title</a>";
            }
            $topic = Options_infoTopic($topic, 1);
            $type = Options_infoType($infotype, 1);
            $industry = Options_industry($industry, 1);
            $created = date('m/d/Y', $created);
            $postedby = getProId($postedby);
            $option_rating = Options_rating($rating);
            $option_rating = "<select name='rating' class='ajaxTrigger' table='maenna_insights' column='rating' idname='insightid' idval='$insightid' ><option></option>$option_rating</select>";
            $option_selection = Options_selection($selection);
            $option_selection = "<select name='selection' class='ajaxTrigger' table='maenna_insights' column='selection' idname='insightid' idval='$insightid' >$option_selection</select>";
            $status = "<select name='status' class='ajaxTrigger'
                                                table='maenna_insights'
                                                column='status'
                                                idname='insightid'
                                                idval='$insightid' >" . Options_status($status) . "</select>";
            $Table['tbody'][] = array(
                                      'title' => $title,
                                      'topic' => $topic,
                                      'type'  => $type,
                                      'industry' => $industry,
                                      'date' => $created,
                                      'source' => $postedby,
                                      'rating' => $option_rating,
                                      'selection' => $option_selection,
                                      'status' => $status
                                      );
        }
        $content = render_table($Table);
        $content .= js_init("init_ajaxTrigger();");
        $content .= button_link(array(
                                      array('label'=>'back to Insights', 'url' => "$redirect")
                                      ));
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}  
?>
