<?php
//mgmt_discussion', 'financial_data', 'council_advice



///////////////////
function mgmt_analysis($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = "MANAGEMENT DISCUSSION & ANALYSIS";
    $tab = sget($_REQUEST,'tab');
    $panel = "multi";
    $data_type = 'mgmtanalysis';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    $content = '';
    

    if($op == 'view' || $op == 'null')
    {
        if($op == 'view' || $op == null){
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and deleted != 1 order by dataid desc limit 2";
        $result = db_query($sql, array($companyid, $data_type));
        $counter = 0;
        while(($Row = db_fetch_object($result)) !== false)
        {
            $counter++;
            $title = htmlentities(strtoupper($Row->data_value),ENT_QUOTES | ENT_IGNORE, "UTF-8");
                if(empty($title))$title = "&nbsp;";
             $text =     htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
            $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
            $text = contentExcerpt($text, '', 1500);
            $last = '';
            
            $content .= "\n<div class='entry'>
                            \n<div class=entry-title>$title".
                                //<div class=editbtn><a href='$edit_link' class=tool>EDIT</a></div>
                           " </div>
                            \n<div class=entry-content>$text</div>
                        \n</div>";
        }
    
        $add_link = $redirect . "&panel=${panel}&view=add&datatype=${data_type}";
        $viewall_link = $redirect . "&panel=${panel}&view=listview&datatype=${data_type}";
        $Block['title'] .= "<div class=editbtn>
                                <a href='$add_link' class=tool>ADD</a>
                            </div>";
        $content .= "\n<div class=viewallbtn><a href='$viewall_link' class=tool>MORE</a></div>";
    }
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

/////////////////////

function financial_data($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = "FINANCIAL DATA";
    $tab = sget($_REQUEST,'tab');
    $panel = "financial_data_panel";
    $data_type = 'financial';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    $content = '';
    
    if($op == null || $op == 'view')
    {
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and deleted != 1 order by data_attr desc";
        $result = db_query($sql, array($companyid, $data_type));
        
        $Data = array();
        $Chart_data = array();
        $str_chart_data = '';
        
         $content .= "<div id='chart'></div>";
        
        $content .= "<div style='position:relative'>
            <div style='position:relative;height:12px;'>
                <div class='editbtn'><a href='#' class='tool openclose' boxid='chart_table' >MORE</a></div>
            </div>
            <div style='margin-top:6px;display:none' id='chart_table'>
        <table cellspacing=0 cellpadding=0 class='blue-border'>";
        while(($Row = db_fetch_array($result) ) !== false)
        {
            $year = sget($Row, 'data_attr');
            $revenue = sget($Row, 'data_value', 'int');
            $earning =  sget($Row, 'data_value2', 'int');
            $working_capt = sget($Row, 'data_value3', 'int');
            $capex = sget($Row, 'data_value4', 'int');
            $dc =  sget($Row, 'data_value5');
            if($earning && $revenue)
            {
                $earning_p = number_format((($earning / $revenue) * 100), 1) . '%';
            }else{
                $earning_p = '';
            }
        
            $Data[] = array(
                            'year' => $year,
                            'earning_p' => $earning_p, 
                            'revenue' => $revenue,
                            'earning' => $earning,
                            'working_capt' => ($working_capt ) ? '$' . number_format($working_capt) : '',
                            'capex' => ($capex) ? '$' . number_format($capex) : '',
                            'dc' => $dc
                        );
        }
        $thisYear = date("Y");
 
        $Table_rows = array(
                            'year' => array(),
                            'Earnings %' => array(),
                            'Revenue Growth' => array(),
                            'Earnings Growth' => array(),
                            'Working Cap.' => array(),
                            'Capex' => array(),
                            'Debt' => array()
                            );
        if(count($Data) == 0)
        {
            for($i = 0; $i < 5; $i++)
            {
                $Table_rows["year"][$i] = $thisYear - $i;
            }
        }else
        {
            // column by column, 
            $This_year = array_shift($Data);
    
            while(count($Data) > 0)
            {
                $Past_year =array_shift($Data);
                
                $Rows = array();
                $Rows[0] = $this_year = $This_year['year'];
                $Rows[1] = $This_year['earning_p'];
                $this_revenue = $This_year['revenue'];
                $this_earning = $This_year['earning'];
                $past_revenue = $Past_year['revenue'];
                $past_earning = $Past_year['earning'];
                $Chart_data[] = array("'$this_year'",  $this_revenue, $this_earning);
                if($this_revenue && $past_revenue){
                    $Rows[2] = number_format((($this_revenue - $past_revenue) / $past_revenue) * 100, 1) . '%';
                }else{
                    $Rows[2] = '';
                }
                if($past_earning && $this_earning){
                    $Rows[3] = number_format((($this_earning - $past_earning) / $past_earning) * 100, 1) . '%';
                }else{
                    $Rows[3] = '';
                }
                $Rows[4] = $This_year['working_capt'];
                $Rows[5] = $This_year['capex'];
                $Rows[6] = $This_year['dc'];
                
                $m = 0;
                foreach($Table_rows as &$Column)
                {
                    array_push($Column, $Rows[$m]);
                    $m++;
                }
                
                //loop
                $This_year = $Past_year;
            }
            $m = 0;
            $Rows = array();
            $Rows[0] = $this_year = $This_year['year'];
            $Rows[1] = $Rows[2] = $Row[3] ='-';
            $Rows[1] = $This_year['earning_p'];
            $Rows[4] = $This_year['working_capt'];
            $Rows[5] = $This_year['capex'];
            $Rows[6] = $This_year['dc'];
            $Chart_data[] = array("'$this_year'",   $This_year['revenue'], $This_year['earning']);
            foreach($Table_rows as &$Column)
            {
                
                array_push($Column, $Rows[$m]);
                $m++;
            }
        }
        foreach($Table_rows as $title => &$Row)
        {
            //$Row = array_pad($Row, 5, '-');
            if($title == 'year'){
                $class= 'align-center bold';
                $title = '';
            }else{
                $class = 'align-right';
            }
            $content .= "\n<tr>
                            <td>$title</td>
                            <td style='width:90px;font-size:11px;' class='$class'>".$Row[0]."</td>
                            <td style='width:90px;font-size:11px;' class='$class'>".$Row[1]."</td>
                            <td style='width:90px;font-size:11px;' class='$class'>".$Row[2]."</td>
                            <td style='width:90px;font-size:11px;' class='$class'>".$Row[3]."</td>
                            <td style='width:90px;font-size:11px;' class='$class'>".$Row[4]."</td>
                        </tr>";
            
        }
        $content .= "\n</table></div></div>";
        foreach($Chart_data as &$M)
        {
            foreach($M as &$i)
            {
                if(empty($i))$i = 0;
            }
            $M = "[" .implode(",", $M) . "]";
        }
    
        $Chart_data= array_pad($Chart_data, 5, "[' ',0,0]");
        $Chart_data = array_reverse($Chart_data, true);
        $str_chart_data = implode(",", $Chart_data) ;
        if($str_chart_data)
                $content .=  <<< END
<script type="text/javascript" src="https://www.google.com/jsapi"></script>
<script type="text/javascript">
        
        
                // Load the Visualization API and the piechart package.
              google.load('visualization', '1.0', {'packages':['corechart']});
        
              // Set a callback to run when the Google Visualization API is loaded.
              google.setOnLoadCallback(drawChart);
             
              function drawChart() {
                var data = new google.visualization.DataTable();
                data.addColumn('string', 'Year');
                data.addColumn('number', 'Rev');
                data.addColumn('number', 'Earnings');
                data.addRows([
                  $str_chart_data
                ]);
        
                var options = {
                  width: 570, height: 240,
                  title: '',
                  //hAxis: {title: 'Year', titleTextStyle: {color: 'red'}}
                };
        
                var chart = new google.visualization.ColumnChart(document.getElementById('chart'));
                chart.draw(data, options);
        
                
                }
</script>
END;
    }
    $edit_link = $redirect . "&panel=${panel}";
    $Block['title'] .= "<div class=editbtn><a href='$edit_link' class=tool>EDIT</a></div>";
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}
function financial_data_panel($op = null)
{
    global $user;
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = "FINANCIAL DATA";
    $tab = sget($_REQUEST,'tab');
    $panel = "financial_data_panel";
    $data_type = 'financial';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    $HV = hidden_post_values(array('a','type','tab', 'companyid'));
    $content = '';
   
    if($op == 'title') return $Block['title'];
    if($op == null || $op == 'view')
    {
        $view = sget($_REQUEST, 'view');
        if(empty($view))
        {
            $Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=add' class=tool>ADD</a></div>";
            $sql = "select * from maenna_company_data where data_type = '%s' and companyid = %d and deleted != 1 order by data_attr desc";
            $result = db_query($sql, array($data_type, $companyid));
      
            $content .= "<table class='report with-border'><tr style='background:#ebebeb'>
                <td style='width:40px;font-weight:bold;text-align:center;vertical-align:middle'>YEAR</td>
                <td style='width:90px;font-weight:bold;text-align:center;vertical-align:middle'>REVENUE($)</td>
                <td style='width:90px;font-weight:bold;text-align:center;vertical-align:middle'>EARNING($)</td>
                <td style='width:90px;font-weight:bold;text-align:center;vertical-align:middle'>WORKING CAP.($)</td>
                <td style='width:90px;font-weight:bold;text-align:center;vertical-align:middle'>CAPEX($)</td>
                <td style='width:60px;font-weight:bold;text-align:center;vertical-align:middle'>DEBT</td>
                <td style=''></td>
            </tr>";
            while($Row = db_fetch_array($result))
            {
                $year = sget($Row, 'data_attr');
                $revenue = sget($Row, 'data_value', 'int'); if($revenue)$revenue =  number_format($revenue);
                $earning = sget($Row, 'data_value2', 'int');if($earning)$earning = number_format($earning);
                $capital = sget($Row, 'data_value3', 'int');;if($capital)$capital = number_format($capital);
                $capex = sget($Row, 'data_value4', 'int');if($capex)$capex = number_format($capex);
                $dc = sget($Row, 'data_value5');
                $dataid = sget($Row, 'dataid');
                $content .= "<tr>
                                <td>$year</td>
                                <td style='text-align:right;'>$revenue</td>
                                <td style='text-align:right;'>$earning</td>
                                <td style='text-align:right;'>$capital</td>
                                <td style='text-align:right;'>$capex</td>
                                <td style='text-align:right;'>$dc</td>
                                <td><a href='$redirect&panel=${panel}&view=edit&dataid=${dataid}'>edit</a>&nbsp;&nbsp;
                                    <a href='$redirect&panel=${panel}&do=delete&dataid=${dataid}&update_section=${panel}' onclick='return confirm(\"Continute to remove the record\")'>del</a>
                                </td>
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
            $revenue = sget($Row, 'data_value', 'int');
            $earning = sget($Row, 'data_value2', 'int');
            $capital = sget($Row, 'data_value3', 'int');
            $capex = sget($Row, 'data_value4', 'int');
            $dc = sget($Row, 'data_value5');
            $this_year = date("Y");
            for($i = $this_year; $i >= ($this_year - 70); $i--)
            {
                $Years["$i"] = $i; 
            }
            $option_year = option_code($Years,$year);
            $content .= "
                <form method=post action='/account' >
                    <table class='report with-border'>
                        <tr style='background:#ebebeb'>
                            <td style='width:40px;font-weight:bold'>YEAR</td>
                            <td style='width:90px;font-weight:bold'>REVENUE($)</td>
                            <td style='width:90px;font-weight:bold'>EARNING($)</td>
                            <td style='width:90px;font-weight:bold'>WORKING CAPITAL($)</td>
                            <td style='width:90px;font-weight:bold'>CAPEX($)</td>
                            <td style='width:50px;font-weight:bold'>DEBT</td>
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
    $editorid = $user->uid;
    $time = time();
    $companyid = sget($_REQUEST, 'companyid');
    $Block['title'] = "COUNCIL ADVICE AND ANALYSIS";
    $tab = sget($_REQUEST,'tab');
    $panel = "multi";
    $data_type = 'advice';
    $redirect = rebuild_url(array('a','type','tab', 'companyid'));
    $content = '';

    if($op == 'view' || $op == 'null')
    {
        if($op == 'view' || $op == null){
        $sql = "select * from maenna_company_data where companyid = %d and data_type = '%s' and deleted != 1 order by dataid desc limit 2";
        $result = db_query($sql, array($companyid, $data_type));
        $counter = 0;
        while(($Row = db_fetch_object($result)) !== false)
        {
            $counter++;
                $title = htmlentities(strtoupper($Row->data_value),ENT_QUOTES | ENT_IGNORE, "UTF-8");
                if(empty($title))$title = "&nbsp;";
             $text =     htmlentities($Row->data_value2,ENT_QUOTES | ENT_IGNORE, "UTF-8");
            $dataid = $Row->dataid;
            $more_link = $redirect . "&panel=${panel}&view=detail&dataid=$dataid";
            $edit_link = $redirect . "&panel=${panel}&view=edit&dataid=$dataid";
            $text = contentExcerpt($text, '', 1500);
            $last = '';
            
            $content .= "\n<div class='entry' >
                            \n<div class=entry-title>$title".
                              //  <div class=editbtn><a href='$edit_link' class=tool>EDIT</a></div>
                            "</div>
                            \n<div class=entry-content>$text</div>
                        \n</div>";
        }
    
        $add_link = $redirect . "&panel=${panel}&view=add&datatype=${data_type}";
        $viewall_link = $redirect . "&panel=${panel}&view=listview&datatype=${data_type}";
        $Block['title'] .= "<div class=editbtn>
                                <a href='$add_link' class=tool>ADD</a>
                            </div>";
        $content .= "\n<div class=viewallbtn><a href='$viewall_link' class=tool>MORE</a></div>";
    }
    }
    
    $Block['body'] = content_box($Block['title'], $content);
    return $Block;
}

/* EOF */