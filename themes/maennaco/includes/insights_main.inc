<?php
// $maenna_page['content'] = array('info_list','related_list');

$maenna_page['content'] = array('infoTabs','info_list','related_list');

function info_list($op = null){
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
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
                               /* array('label'=> 'type', 'style' => 'width:120px'),
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
                                    array('label'=>'more &raquo;','url'=>'/account?a=information&tab=insights&panel=insights_panel&section=advisory' )
                                    ),'right');
    
        $content .= js_init('init_ajaxTrigger(); ');
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}


function related_list($op = null){
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();
    
    if($op == 'view' || $op == null)
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
                                    array('label'=>'more &raquo;','url'=>'/account?a=information&tab=insights&panel=insights_panel&section=research' )
                                    ),'right');
        
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;

}

function insights_panel($op = null)
{
    global $user;
    $editorid = $user->uid;
    $pid = sget($_REQUEST, 'pid');
    $Block['title'] = '';
    $content = '';
    $time = time();


    if($op == null || $op == 'view')
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
                                      array('label'=>'back to Insights', 'url' => '/account?a=information&tab=insights')
                                      ));
        
        
        
        $Block['body'] = content_box($Block['title'], $content);
    }
    return $Block;
}


        
?>
