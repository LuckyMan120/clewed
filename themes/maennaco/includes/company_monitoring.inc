<?php
//mgmt_discussion', 'financial_data', 'council_advice



///////////////////
function mgmt_analysis($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $time = time();
    $companyid = $user->uid;
    $Block['title'] = "MANAGEMENT DISCUSSION & ANALYSIS";
    $tab = sget($_REQUEST,'tab');
    $panel = "mgmt_analysis_panel";
    $data_type = 'analysis';
    $redirect = rebuild_url(array('tab'));
    $content = '';
    

    if($op == 'view' || $op == 'null')
    {
        if($op == 'view' || $op == null)
        {
          $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type' and deleted != 1 order by dataid desc limit 2";
          $result = db_query($sql, array($companyid));
          $counter = 0;
          while(($Row = db_fetch_object($result)) !== false)
          {
            $counter++;
            $text =     htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
            $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
            $text = contentExcerpt($text, '', 1500);
            $last = '';
            
            $content .= "\n<div class='entry' style='margin-top:0px;padding-bottom:0px;'>
                            \n<div class=entry-title>&nbsp;".
                                //<div class=editbtn><a href='$edit_link' class=tool>EDIT</a></div>
                           " </div>
                            \n<div class=entry-content>$text</div>
                        \n</div>";
          }
    
          $viewall_link = $redirect . "&panel=${panel}&view=listview";
          if ($write) {
            $add_link = $redirect . "&panel=${panel}&view=add";
            $Block['title'] .= "<div class=editbtn>
                                <a href='$add_link' class=tool>ADD</a>
                            </div>";
          }
          $content .= "\n<div class=viewallbtn><a href='$viewall_link' class=tool>VIEW ALL</a></div>";
      }
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function mgmt_analysis_panel($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $time = time();
    $companyid = $user->uid;
    $Block['title'] = "MANAGEMENT DISCUSSION & ANALYSIS";
    $tab = sget($_REQUEST,'tab');
    $panel = "mgmt_analysis_panel";
    $data_type = 'analysis';
    $redirect = rebuild_url(array('tab'));
    $content = '';
    
    if($op == 'title'){
        return $Block['title'];
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
                    
                    $text = sget($Row, 'data_value2');                  
                    $text = nl2br(htmlentities($text, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                    $content = "<div class=entry>
                    
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
                                            <a href='$edit_link' class=tool>EDIT</a></div>";
                    }
                }elseif($view == 'edit' || $view == 'add')
                {
                   
                    $dataid = sget($Row, 'dataid');
                    $text = sget($Row, 'data_value2');
                    if($view == 'edit'){
                        $title = "EDIT " . $Block['title'];
                        $do = 'update';
                        $rem_link = $redirect . "&panel=${panel}&view=listview&update_section=${panel}&do=remove&dataid=$dataid";
                    }else{
                        $title = "ADD " . $Block['title'];
                        $do = 'insert';
                        $rem_link = '';
                    }
                    $Block['title'] = $title;
                    $hv = hidden_post_values(array('tab','panel'));
                    $content .= <<< END
                    <form action='/account' method='post' onsubmit='return check_input();'>
                        <div class=entry>
                            <div class=entry-content>
                    
                                <div>CONTENT:<br /><textarea name='content' style='width:99%;height:400px;' class='require_string' >$text</textarea>
                                <div class=backbtn>
                                    <div class='rembtn'><a href='$rem_link' class=button onclick='return confirm("Continue to remove record")'>Delete</a></div>
                                    <input type=submit name=submit value=submit class=button />
                                    &nbsp;&nbsp;
                                    <a href='${redirect}&panel=${panel}&view=listview' class=button>Cancel</a>
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
                $title = $Row->data_attr;
                if($title )
                {
                    $title = htmlentities(strtoupper($title), ENT_QUOTES | ENT_IGNORE, "UTF-8");
                }else
                $title = "&nbsp;";
                $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES  | ENT_IGNORE, "UTF-8"));
                $dataid = $Row->dataid;
                $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
                $text = contentExcerpt($text, $more_link);
                $content .= "<div class=entry>";
                if ($write) {
                  $content .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid' class='tool' >edit</a></div>";
                }
                $content .= "<div class='entry-title'>$title</div><div class='entry-content'>" . _filter_autop(html_entity_decode($text)) ."</div></div><hr class='line' />";
            }
            $Pagenation = array('total' => $total,
                'limit' =>5,
                'baseurl' => $redirect,
                'num_of_links' => 8,
            );
            if ($write) {
              $add_link = $redirect . "&panel=${panel}&view=add";
              $Block['title'] .= "<div class=editbtn>
                                <a href='$add_link' class=tool>ADD</a>
                            </div>";
            }
            $content .= pagination($Pagenation);
        }
    }elseif($op == 'update')
    {
        $do = sget($_REQUEST, 'do');
        $Correct = false;
        if($do == 'update' || $do == 'insert')
        {
            $dataid = sget($_REQUEST,'dataid');
           
            $content = sget($_REQUEST, 'content');
                
            if( empty($content))
            {
                
            }
            elseif($dataid && ($do == 'update'))
            {
                $DBValues = array('access' => $time,
                        
                                  'data_value2' => $content,
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
                                'data_value2',
                                'editorid');
                $DBValues = array($companyid, $time, $data_type, $content, $editorid);
                $SQL_STR = array("%d", "'%s'","'%s'","'%s'","%d");
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

/////////////////////

function financial_data($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $time = time();
    $companyid = $user->uid;
    $Block['title'] = "FINANCIAL DATA";
    $tab = sget($_REQUEST,'tab');
    $panel = "financial_data_panel";
    $data_type = 'financial';
    $redirect = rebuild_url(array('tab'));
    $content = '';
    
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company_data where companyid = $companyid and data_type = '$data_type' order by data_attr desc";
        $result = db_query($sql);
        $Data = array();
        $content = "<div><table cellspacing=0 cellpadding=0 class='dotted-border'>";
        if(db_affected_rows($result) > 0)
        {
            while(($row = db_fetch_object($result) ) !== false)
            {
                $year = $row->data_attr;
                $Data["$year"] = $row;
            }
            $row1 = "<tr><td></td>";
            $row2 = "<tr><td >Revenue G%</td>";
            $row3 = "<tr><td>Earnings G%</td>";
            $row4 = "<tr><td >Current Assets</td>";
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
                    $row1 .= "\n<td align=center  class='bold' style='width:16%;'>$year</td>";
                    $revenue = '';
                    $earnings = '';
                    $assets = '';
                    if(isset($Data["$year"])){
                        $row = $Data["$year"];
                        if($row->data_value)$revenue = "\$" .number_format($row->data_value);
                        if($row->data_value2)$earnings ="\$" . number_format($row->data_value2);
                        if($row->data_value3)$assets ="\$" . number_format($row->data_value3);
                    }
                    $row2 .= "<td align=right>$revenue</td>";
                    $row3 .= "<td align=right>$earnings</td>";
                    $row4 .= "<td align=right>$assets</td>";
                }
            }
            $row1 .= "</tr>";
            $row2 .= "</tr>";
            $row3 .= "</tr>";
            $row4 .= "</tr>";
        }
        $content .= "\n $row1 \n $row2 \n $row3\n $row4 \n</table></div>";
    }
    if ($write) {
      $edit_link = $redirect . "&panel=${panel}";
      $Block['title'] .= "<div class=editbtn><a href='$edit_link' class=tool>EDIT</a></div>";
    }
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

function financial_data_panel($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $time = time();
    $companyid = $user->uid;
    $Block['title'] = "FINANCIAL DATA";
    $tab = sget($_REQUEST,'tab');
    $panel = "financial_data_panel";
    $data_type = 'financial';
    $redirect = rebuild_url(array('tab'));
    $HV = hidden_post_values(array('tab'));
    $content = '';
   
    if($op == 'title') return $Block['title'];
    if(!$write || $op == null || $op == 'view')
    {
        $view = sget($_REQUEST, 'view');
        if(!$write || empty($view))
        {
            if ($write) {
              $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>ADD</a></div>";
            }
            $sql = "select * from maenna_company_data where data_type = '%s' and companyid = %d and deleted != 1 order by data_attr desc";
            $result = db_query($sql, array($data_type, $companyid));
            $content = "<table class='report with-border'><tr style='background:#ebebeb'>
                <td style='width:40px;font-weight:bold;text-align:center;vertical-align:middle'>YEAR</td>
                <td style='width:90px;font-weight:bold;text-align:center;vertical-align:middle'>REVENUE($)</td>
                <td style='width:90px;font-weight:bold;text-align:center;vertical-align:middle'>EBIT($)</td>
                <td style='width:90px;font-weight:bold;text-align:center;vertical-align:middle'>WORKING CAP.($)</td>
                <td style='width:90px;font-weight:bold;text-align:center;vertical-align:middle'>CAPEX($)</td>
                <td style='width:50px;font-weight:bold;text-align:center;vertical-align:middle'>Debt</td>";
            if ($write) {
              $content .= "
                <td style=''></td>";
            }
            $content .= "
            </tr>";
            while($Row = db_fetch_object($result))
            {
                $year = $Row->data_attr;
                $revenue = $Row->data_value; if($revenue)$revenue =  number_format($revenue);
                $earning = $Row->data_value2;if($earning)$earning = number_format($earning);
                $capital = $Row->data_value3;if($capital)$capital = number_format($capital);
                $capex = $Row->data_value4;if($capex)$capex = number_format($capex);
                $dc = $Row->data_value5;
                $dataid = $Row->dataid;
                $content .= "<tr>
                                <td>$year</td>
                                <td style='text-align:right;'>$revenue</td>
                                <td style='text-align:right;'>$earning</td>
                                <td style='text-align:right;'>$capital</td>
                                <td style='text-align:right;'>$capex</td>
                                <td style='text-align:right;'>$dc</td>";
                if ($write) {
                  $content .= "
                                <td><a href='$redirect&panel=${panel}&view=edit&dataid=${dataid}'>edit</a>&nbsp;&nbsp;
                                    <a href='$redirect&panel=${panel}&do=delete&dataid=${dataid}&update_section=${panel}' onclick='return confirm(\"Continute to remove the record\")'>del</a>
                                </td>";
                }
                $content .= "
                            </tr>";
            }
            $content .= "</table>";
        }elseif($view == 'edit' || $view == 'add')
        {
            $dataid = sget($_REQUEST, 'dataid');
            if($dataid)
            {
                $sql = "select * from maenna_company_data where dataid = %d";
                $result = db_query($sql, array($dataid));
                $Row = db_fetch_array($result);
            }
            if(empty($Row)) $Row = array();
            
            $year = sget($Row, 'data_attr');
            $revenue = sget($Row, 'data_value');
            $earning = sget($Row, 'data_value2');
            $capital = sget($Row, 'data_value3');
            $capex = sget($Row, 'data_value4');
            $dc = sget($Row, 'dc');
            $this_year = date("Y");
            for($i = $this_year; $i >= ($this_year - 70); $i--)
            {
                $Years["$i"] = $i; 
            }
            $option_year = option_code($Years,$year);
            $content .= "
                <form>
                    <table class='report with-border'>
                        <tr style='background:#ebebeb'>
                            <td style='width:40px;font-weight:bold'>YEAR</td>
                            <td style='width:90px;font-weight:bold'>REVENUE</td>
                            <td style='width:90px;font-weight:bold'>EARNING</td>
                            <td style='width:90px;font-weight:bold'>WORKING CAPITAL</td>
                            <td style='width:90px;font-weight:bold'>CAPEX</td>
                            <td style='width:50px;font-weight:bold'>Debt</td>
                        </tr>
                        <tr>
                            \n<td ><select name=year><option></option>$option_year</select></td>
                            <td>$<input type=text name=revenue value='$revenue' style='width:80px;'></td>
                            <td>$<input type=text name=earning value='$earning' style='width:80px;'></td>
                            <td>$<input type=text name=capital value='$capital' style='width:80px;'></td>
                            <td>$<input type=text name=capex value='$capex' style='width:80px;'></td>
                            <td><input type=text name=dc value='$dc' style='width:30px;'></td>
                        </tr>
                    </table>
                    <div class=row style='border:none;margin-top:9px;'>
                        <input type=submit name=submit value=submit class=button />
                        <a href='$redirect&panel=${panel}' class=button>cancel</a>
                    </div>
                    $HV
                    <input type='hidden' name=panel value='$panel' />
                    <input type='hidden' name=update_section value='$panel' />
                    <input type=hidden name=do value='update' />
                    <input type=hidden name=dataid value='$dataid' />
                </form>
            ";
        }
        
    }
    elseif($op == 'update')
    {
        
        $Correct = false;
        $do = sget($_REQUEST, 'do');
        if($do == 'update')
        {
            $DBKeys = array('year' => 'data_attr',
                            'revenue' => 'data_value',
                            'earning' => 'data_value2',
                            'capital' => 'data_value3',
                            'capex' => 'data_value4',
                            'dc' => 'data_value5');
            $DBValues = $SQL_STR_NEW = $SQL_UPDATE = array();
            foreach($DBKeys as $post_key => $dbkey)
            {
                $DBValues["$dbkey"] = sget($_REQUEST, $post_key);
                $SQL_STR_NEW[] = "'%s'";
                $SQL_STR_UPDATE[] = "$dbkey = '%s'";
            }
            if(sget($_REQUEST, 'year') == '')
            {
                drupal_set_message("Please specify a year", 'error');
                return;
            }
            $dataid = sget($_REQUEST, 'dataid');
            if(empty($dataid))
            {
                
                $sql = "insert into maenna_company_data (companyid, data_type, access, editorid, ". implode(',',$DBKeys).") values(%d, '%s', '%s',%d ,".implode(',', $SQL_STR_NEW).")";
                array_unshift($DBValues, $companyid, $data_type, $time, $editorid);
               
            }else{
                $sql = "update maenna_company_data set  ".implode(',',$SQL_STR_UPDATE)." , editorid = %d, access = '%s' where dataid = %d limit 1";
                array_push($DBValues, $editorid, $time, $dataid);
            }
            $Correct = db_query($sql, $DBValues);
        }elseif($do = 'delete')
        {
            $dataid = sget($_REQUEST, 'dataid');
            if($dataid)
            {
                $sql = "update maenna_company_data set deleted = 1 where dataid = %d limit 1";
                $DBValues = array($dataid);
                $Correct = db_query($sql, $DBValues);
            }
        }
        if($Correct) drupal_set_message("Financial information is updated");
        else drupal_set_message("Operation failed", 'error');

        return;
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

///////////////////
function council_advice($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $time = time();
    $companyid = $user->uid;
    $Block['title'] = "COUNCIL ADVICE AND ANALYSIS";
    $tab = sget($_REQUEST,'tab');
    $panel = "council_advice_panel";
    $data_type = 'advice';
    $redirect = rebuild_url(array('tab'));
    $content = '';

    if($op == 'view' || $op == 'null')
    {
        if($op == 'view' || $op == null)
        {
          $sql = "select * from maenna_company_data where companyid = %d and data_type = '$data_type' and deleted != 1 order by dataid desc limit 2";
          $result = db_query($sql, array($companyid));
          $counter = 0;
          while(($Row = db_fetch_object($result)) !== false)
          {
            $counter++;
            $text =     htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
            $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
            $text = contentExcerpt($text, '', 1500);
            $last = '';
            
            $content .= "\n<div class='entry' style='margin-top:0;padding-bottom:0'>
                            \n<div class=entry-title>&nbsp;".
                              //  <div class=editbtn><a href='$edit_link' class=tool>EDIT</a></div>
                            "</div>
                            \n<div class=entry-content>$text</div>
                        \n</div>";
          }
    
          $viewall_link = $redirect . "&panel=${panel}&view=listview";
          if ($write) {
            $add_link = $redirect . "&panel=${panel}&view=add";
            $Block['title'] .= "<div class=editbtn>
                                <a href='$add_link' class=tool>ADD</a>
                            </div>";
          }
          $content .= "\n<div class=viewallbtn><a href='$viewall_link' class=tool>VIEW ALL</a></div>";
        }
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function council_advice_panel($op = null)
{
    global $user;
    global $permit;

    $write = $permit->check(__FUNCTION__, 'write');
    $editorid = $user->uid;
    $time = time();
    $companyid = $user->uid;
    $Block['title'] = "COUNCIL ADVICE AND ANALYSIS";
    $tab = sget($_REQUEST,'tab');
    $panel = "council_advice_panel";
    $data_type = 'advice';
    $redirect = rebuild_url(array('tab'));
    $content = '';
    if($op == 'title'){
        return $Block['title'];
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
                    
                    $text = sget($Row, 'data_value2');                  
                    $text = nl2br(htmlentities($text, ENT_QUOTES | ENT_IGNORE, "UTF-8"));
                    $content = "<div class=entry>
                    
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
                                            <a href='$edit_link' class=tool>EDIT</a></div>";
                    }
                }elseif($view == 'edit' || $view == 'add')
                {
                   
                    $dataid = sget($Row, 'dataid');
                    $text = sget($Row, 'data_value2');
                    if($view == 'edit'){
                        $title = "EDIT " . $Block['title'];
                        $do = 'update';
                        $rem_link = $redirect . "&panel=${panel}&view=listview&update_section=${panel}&do=remove&dataid=$dataid";
                    }else{
                        $title = "ADD " . $Block['title'];
                        $do = 'insert';
                        $rem_link = '';
                    }
                    $Block['title'] = $title;
                    $hv = hidden_post_values(array('tab','panel'));
                    $content .= <<< END
                    <form action='/account' method='post' onsubmit='return check_input();'>
                        <div class=entry>
                            <div class=entry-content>
                    
                                <div>CONTENT:<br /><textarea name='content' style='width:99%;height:400px;' class='require_string' >$text</textarea>
                                <div class=backbtn>
                                    <div class='rembtn'><a href='$rem_link' class=button onclick='return confirm("Continue to remove record")'>Delete</a></div>
                                    <input type=submit name=submit value=submit class=button />
                                    &nbsp;&nbsp;
                                    <a href='${redirect}&panel=${panel}&view=listview' class=button>Cancel</a>
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
                $title = $Row->data_attr;
                if($title )
                {
                    $title = htmlentities(strtoupper($title), ENT_QUOTES | ENT_IGNORE, "UTF-8");
                }else
                $title = "&nbsp;";
                $text = nl2br(htmlentities($Row->data_value2,ENT_QUOTES  | ENT_IGNORE, "UTF-8"));
                $dataid = $Row->dataid;
                $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
                $text = contentExcerpt($text, $more_link);
                $content .= "<div class=entry>";
                if ($write) {
                  $content .= "<div class=editbtn><a href='${redirect}&panel=${panel}&view=edit&dataid=$dataid' class='tool' >edit</a></div>";
                }
                $content .= "<div class='entry-title'>$title</div><div class='entry-content'>" . $text ."</div></div><hr class='line' />";
            }
            $Pagenation = array('total' => $total,
                'limit' =>5,
                'baseurl' => $redirect,
                'num_of_links' => 8,
            );
            if ($write) {
              $add_link = $redirect . "&panel=${panel}&view=add";
              $Block['title'] .= "<div class=editbtn>
                                <a href='$add_link' class=tool>ADD</a>
                            </div>";
            }
            $content .= pagination($Pagenation);
        }
    }elseif($op == 'update')
    {
        $do = sget($_REQUEST, 'do');
        $Correct = false;
        if($do == 'update' || $do == 'insert')
        {
            $dataid = sget($_REQUEST,'dataid');
           
            $content = sget($_REQUEST, 'content');
                
            if( empty($content))
            {
                
            }
            elseif($dataid && ($do == 'update'))
            {
                $DBValues = array('access' => $time,
                        
                                  'data_value2' => $content,
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
                                'data_value2',
                                'editorid');
                $DBValues = array($companyid, $time, $data_type, $content, $editorid);
                $SQL_STR = array("%d", "'%s'","'%s'","'%s'","%d");
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

/* EOF */
