<div id="center">
    <div id="squeeze">
        <div class="right-corner">
            <div class="left-corner" style='padding:0;margin:0'>
                <div class="clear-block">
                    <?/*= str_replace('MAENNA', 'clewed', $content) */?>
                    <div id="node-76" class="node" style="margin: auto;">
                        <div class="content clear-block">
                            <!-- <div class="index-text-a" style="width:880px !important;padding-top:50px !important;">Invest to grow with promising private companies</div> -->
                        </div>
                    </div>
                    <div
                        style="display:none;position:relative;margin-left:auto; margin-right:auto; width:263px;height:40px;">
                        <div style="position:absolute;width:100px;top:0;left:0;margin:0;padding:0;">
                            <a href="https://twitter.com/share" class="twitter-share-button" data-count="none">Tweet</a>
                            <script type="text/javascript">!function (d, s, id) {
                                    var js, fjs = d.getElementsByTagName(s)[0];
                                    if (!d.getElementById(id)) {
                                        js = d.createElement(s);
                                        js.id = id;
                                        js.src = "//platform.twitter.com/widgets.js";
                                        fjs.parentNode.insertBefore(js, fjs);
                                    }
                                }(document, "script", "twitter-wjs");</script>
                        </div>
                        <div style="position:absolute;width:100px;top:10px;left:80px;">
                            <script src="//platform.linkedin.com/in.js" type="text/javascript"></script>
                            <script type="IN/Share" data-url="www.maennaco.com" data-counter="right"></script>
                        </div>
                        <div style="position:absolute;width:100px;top:10px;left:150px;">
                            <iframe
                                src="//www.facebook.com/plugins/like.php?href=http%3A%2F%2Fwww.maennaco.com&amp;send=false&amp;layout=button_count&amp;width=450&amp;show_faces=false&amp;action=like&amp;colorscheme=light&amp;font&amp;height=21&amp;appId=267809316574109"
                                scrolling="no" frameborder="0"
                                style=" margin-left:30px;border:none; overflow:hidden; width:80px; height:21px;"
                                allowTransparency="true"></iframe>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>
<!-- /.left-corner, /.right-corner, /#squeeze, /#center -->

<br style="clear:both;">

<?php
    function get_all_number($flag){
        $sql_all = "SELECT COUNT(*) as cnt FROM `maenna_company` mc LEFT JOIN `users` u ON `mc`.`companyid` = `u`.`uid`
                    WHERE `u`.`status` = 1 AND `mc`.`public` = 1 AND `mc`.`stateable` = $flag";
        if (!empty($_REQUEST['industry'])) {
            $industries = "'" . implode("', '", array_keys($_Industry[$_REQUEST['industry']])) . "'";
            $sql_all .= " AND `sector` IN($industries)";
        }

        if (!empty($_REQUEST['revenue'])) {
            $sql_all .= " AND (`revenue` IS NULL OR `revenue` <= " . ((int) $_REQUEST['revenue']) . ")";
        }
        $result_all = db_query($sql_all);
        $cnt_all = db_fetch_object($result_all);
        $cnt_all = $cnt_all->cnt;
        return $cnt_all;
    }

    function get_projects($flag){
        $start = 0;
        $limit = 3;
        $_Industry = _INDUSTRY();
        if (sget($_REQUEST, '_page')) $start = (sget($_REQUEST, '_page') - 1) * $limit;
        $financial_table = 'LEFT JOIN (SELECT `companyid` AS financial_id,`data_value` AS revenue  FROM `maenna_company_data` WHERE `dataid` = (
                                SELECT `dataid` FROM `maenna_company_data` AS temp
                                    WHERE
                                        `temp`.`companyid` = `maenna_company_data`.`companyid`
                                    AND
                                        `temp`.`data_type` = "financial"
                                    AND
                                        `temp`.`data_value` > 0
                                    AND
                                        `temp`.`data_value` IS NOT NULL
                                    ORDER BY `data_attr` DESC LIMIT 1
                                )
                            ) AS financial_tbl ON `financial_tbl`.`financial_id` = `maenna_company`.`companyid`';

        $abt_tbl = 'LEFT JOIN `maenna_about` ON `maenna_company`.`companyid` = `maenna_about`.`project_id`';

        $SQL = "SELECT
                    CAST(`financial_tbl`.`revenue` AS UNSIGNED) AS revenue,
                    `maenna_company`.*,
                    `maenna_about`.`mission`,
                    `maenna_about`.`project`,
                    IFNULL(`likes`.`amount`, 0) AS likes,
                    `likes`.`users` AS liking_users
                FROM
                    `users`,
                    `maenna_company`
                    $financial_table
                    $abt_tbl
                LEFT JOIN (
                    SELECT
                    `project_id`,
                    COUNT(`la_id`) AS amount,
                    CONCAT(`user_id`) AS users
                    FROM `like_company`
                    GROUP BY `project_id`
                ) AS likes
                ON
                    `likes`.`project_id` = `maenna_company`.`companyid`
                WHERE
                    `users`.`uid` = `maenna_company`.`companyid`
                AND
                    `users`.`status` = 1
                AND
                    `public` = 1
                AND `maenna_company`.`stateable` = $flag";


        if (!empty($_REQUEST['industry'])) {
            $industries = "'" . implode("', '", array_keys($_Industry[$_REQUEST['industry']])) . "'";
            $SQL .= " AND `sector` IN($industries)";
        }

        if (!empty($_REQUEST['revenue'])) {
            $SQL .= " AND (`revenue` IS NULL OR `revenue` <= " . ((int) $_REQUEST['revenue']) . ")";
        }

        $SQL .= ' GROUP BY `maenna_company`.`companyid`';

        if (empty($_REQUEST['likes'])) {
            $SQL .= ' ORDER BY `companyid` DESC';
        } else {
            $SQL .= ' ORDER BY `likes` ' . (($_GET['likes'] == 'ASC') ? 'ASC' : 'DESC');
        }

        $SQL .= ' LIMIT ' . $start . ', ' . $limit;
        $result = db_query($SQL);
        if(mysql_num_rows($result)>0){
            mysql_data_seek($result, 0);
            return $result;
        }
    }
    function get_team_size($flag){
        $result = get_projects($flag);
        if($result){
            $projectIds = array();
            while (($row = db_fetch_array($result)) !== false)
                $projectIds[$projectId] = $projectId = (int) $row['companyid'];

            $companyService = new \Clewed\Company\Service();
            $teamSizes = $companyService->getTeamSize(array_keys($projectIds));
            return $teamSizes;
        }
    }
?>
<div id="center" class="custom-center-content" style='margin-top:-80px;'>
<!--    <p>-->
<!--        Access unique offerings across a diverse set of alternative asset classes.-->
<!--    </p>-->
    <div id="squeeze">
        <div class="account-content-box">
            <div class="custom_content_box">
                <div class="box_content main_content custom-box-content">
                    <?php
                        $i = 0;
                        $open_result = get_projects($flag="1");
                        $cnt_all = get_all_number($flag="1");
                        $cmp_cnt = mysql_num_rows($open_result);
                        if ($cmp_cnt > 0) {
                            echo '
                                  <div class="open-opportunities-content">
                                      <div class="open-title">
                                          <b>Open</b> Opportunities
                                      </div>
                                  </div>
                            <div id="custom-cardholder" class="openable">';
                            $teamSizes = get_team_size($flag="1");
                            while (($row = db_fetch_array($open_result)) !== false) {
                                $i++;
                                $recordid = sget($row, 'companyid');
                                $sql = "SELECT data_value FROM maenna_company_data as t
                                        LEFT JOIN maenna_company as t1 ON t.companyid = t1.companyid
                                        WHERE t.data_type = 'financial' AND t.companyid = $recordid AND t.data_attr = YEAR(NOW()) AND t1.openable = 1";
                                $rev = mysql_query($sql);
                                $rev = mysql_fetch_array($rev);
                                if (!empty($row['project']) && file_exists('themes/maennaco/images/project/' . $row['project'])) {
                                    $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/' . urlencode($row['project']);   // 270*202  . '&zc=1&w=270&h=202'
//                                      $avatar = '/themes/maennaco/custom-images/default_avatar.png';
                                } else {
//                                    $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/big-' . str_replace(' /themes/maennaco/images/', '', getAvatarUrl($row['companyid'])); //. '&zc=1&w=270&h=202'
                                      $avatar = '/themes/maennaco/custom-images/default_avatar.png';
                                }
                                ?>
                                <?php if($row['stateable']):?>
                                    <div class="custom-card-wrap">
                                        <div class="custom-card-inner">
<!--                                            <div class="projectName" style="text-align: left;display: block;">--><?//= $row['projname'] ?><!--</div>-->
                                            <a style="cursor:pointer;" href="/companies?id=<?= $row['companyid'];?>">
                                                <img src="<?= $avatar ?>" alt="<?= $row['projname'] ?>" width="100%" height="100%"  style="object-fit: cover"/>
                                            </a>
                                            <?php
                                                $cityState = array();
                                                if (!empty($row['city'])) array_push($cityState, ucwords($row['city']));
                                                if (!empty($row['state'])) array_push($cityState, strtoupper($row['state']));
                                                $liking_users = explode(',', $row['liking_users']);
                                            ?>
<!--                                            <div class="city-state" style="color:#898B8E;">--><?//= implode(', ', $cityState) ?><!--</div>-->
                                        </div>
                                        <?php
                                            $sectorRev = array();
                                            if (!empty($row['sector'])) {
                                                array_push($sectorRev, preg_replace('/(?<=\\w)(?=[A-Z])/', ' $1', $row['sector']));
                                                $sectorStr = implode(' - ', $sectorRev);
                                            }

                                            if (!empty($rev['data_value'])) {
                                                if($rev['data_value'] >= 1000000000 ) {
                                                    $revenue = "Rev : $" . (int)($rev['data_value'] / 1000000000) . " B";
                                                    $revenueTwo = "$".(int)($rev['data_value'] / 1000000000) . " B";
                                                }
                                                else {
                                                    $revenue = "Rev : $" . (int)($rev['data_value'] / 1000000) . " M";
                                                    $revenueTwo = "$".(int)($rev['data_value'] / 1000000) . " M";
                                                    }
                                            } else {
                                                $revenue = "Rev : $".(int)$rev['data_value']."";
                                                $revenueTwo = "$".(int)$rev['data_value']."";
                                            }
                                        ?>
                                        <div class='custom-card-inner-right'>
                                            <div class="custom-card-inner-right-top-content">
                                                <ul class="custom-card-inner-wrapper pr-15">
                                                    <li class=" ml-0 pl-0 fs-12 custom-card-inner-item">
                                                        <a class="text-d-n" style="cursor:pointer;" href="/companies?id=<?= $row['companyid']; ?>"><?= $row['projname'] ?></a>
                                                    </li>
                                                    <li class="pl-30 fs-12 custom-card-inner-item">
                                                        <?= implode(', ', $cityState); ?>
                                                    </li>
                                                    <li class="pl-30 fs-12 custom-card-inner-item">
                                                        <span class="custom-categories-span">Industry:</span> <?= $sectorStr; ?>
                                                    </li>

                                                    <li class="pl-30 ml-0-460 fs-12 custom-card-inner-item">
                                                        <?php if ($revenueTwo != null){?><span class="custom-categories-span">Rev: </span><?= $revenueTwo;  }?>
                                                    </li>

                                                    <li class="ml-auto fs-12 custom-card-inner-item">
                                                        <?php if($row['shareable']):?>
                                                            <span onclick='showContMessage()'><a data-tooltip="
                                                                This tool allows you to connect to learn more and explore fit/qualifications privately. You must have related industry and/or operating expertise for services."
                                                                                                 type='collaborate' style='cursor:pointer; color: #00A2BF;'>Connect</a></span>                                                       </span>
                                                            <span><?= (int) $teamSizes[$recordid]; ?></span>
                                                        <?php endif; ?>
                                                    </li>
                                                </ul>
<!--                                                <div class="sector-revenue" style="color:#898B8E; width:30%; margin-right:15%;">-->
<!--                                                        <span class="sector">Indudtry: --><?//= $sectorStr ?><!--</span>-->
<!--                                                </div>-->
<!--                                                <div style="max-width: 30%;margin-right: 30%;color:#898B8E;"> -->
<!--                                                    <span class="revenue">--><?//=$revenue?><!--</span>-->
<!--                                                </div>-->
<!--                                                <div style="max-width: 30%;color:#00A2BF;"> -->
<!--                                                    --><?php //if($row['shareable']):?>
<!--                                                        <span onclick='showContMessage()'><a data-tooltip="-->
<!--                                                                This tool allows you to connect to learn more and explore fit/qualifications privately. You must have related industry and/or operating expertise for services."-->
<!--                                                            type='collaborate' style='cursor:pointer; color: #00A2BF;'>Connect</a></span>                                                       </span>-->
<!--                                                        <span>--><?//= (int) $teamSizes[$recordid]; ?><!--</span>-->
<!--                                                    --><?php //endif; ?>
<!--                                                </div>-->
                                            </div>
                                            <a class="custom-card-inner-text"  href="/companies?id=<?= $row['companyid'];?>">
                                                <div class="custom-card-inner-text-child" >
                                                    <h5 class="custom-subtitile" ><?=$row['deal_summary_title']?></h5>
                                                    <div class="custom-mission" >
                                                        <?= substr(strip_tags($row['deal_summary_statement']), 0, 230) . (!empty($row['deal_summary_statement']) && strlen(strip_tags($row['deal_summary_statement'])) > 230 ? '&hellip;' : '') ?>
                                                    </div>
                                                </div>
                                            </a>
                                            <div class="custom-card-inner-bottom-content">
                                                <?php
                                                    $sql = "select sum(amount) sum, COUNT(*) as counts from maenna_professional_investment where company_id=%d and status=3";
                                                    $result = db_query($sql, array($recordid));
                                                    $row_amount = db_fetch_array($result);
                                                    $total_amount = is_null($row_amount['sum']) ? 0 : $row_amount['sum'];
                                                    $committed_count = $row_amount['counts'];

                                                    $sql_round_amount = "select round_amount_raising as rmount,close_date,security_type from maenna_company_data where companyid=%d and data_type='financial' and data_attr = ".date('Y')."";
                                                    $result_amount = db_query($sql_round_amount, array($recordid));
                                                    $row_round = db_fetch_array($result_amount);
//                                                    dump($row_round);
                                                    $goal_amount = is_null($row_round['rmount']) ? 0 : $row_round['rmount'];
                                                    $close_date = $row_round['close_date'];
//                                                    dump($close_date);
                                                    $data_style = date("d/m/y", time($close_date));
//                                                    dump($data_style);

                                                $amount_raising_percent = $total_amount / $goal_amount * 100;
                                                    if($amount_raising_percent>20) $possible_percent = $amount_raising_percent;
                                                    if($goal_amount>0){
                                                        $gola_amount_style = '$' . number_format($goal_amount, 0, '.', ',');
                                                    }
                                                    $seq_type = $row_round['security_type'];
                                                if($row['fundraising'] == '1'){
                                                    echo('     
                                                    <div class="custom_status_bar_wrapper-content custom-flex-460">
                                                          <div class="custom-content-icon">');
                                                                if ($goal_amount>0)
                                                                    echo('
                                                                            <div class="custom-media-mini">
                                                                               <div class="custom-style-img">
                                                                                   <img src="/themes/maennaco/custom-images/money.png" alt="Raising">
                                                                               </div>
                                                                               <div class="custom-media-body-mini pl-2">
                                                                                    <h6>'.$gola_amount_style.'</h6>
                                                                                    <p>Amount Raising</p>
                                                                               </div>
                                                                            </div>
                                                                            ');
                                                                if($revenue != null)
                                                                    echo('
                                                                            <div class="custom-media-mini ml-30">
                                                                                  <div class="custom-style-img ">
                                                                                      <img src="/themes/maennaco/custom-images/yes.png" alt="Revivee">
                                                                                  </div>
                                                                                  <div class="custom-media-body-mini pl-2">
                                                                                       <h6>Amount : $</h6>
                                                                                       <p>Minimum</p>
                                                                                  </div>
                                                                            </div>
                                                                           ');
                                                                if(($revenue == null && $goal_amount<=0) && $data_style != null)
                                                                    echo('
                                                                            <div class="custom-media-mini">
                                                                                   <div class="custom-style-img ">
                                                                                       <img src="/themes/maennaco/custom-images/calendar.png" alt="Launch">
                                                                                   </div>
                                                                                   <div class="custom-media-body-mini pl-2">
                                                                                        <h6>'.$data_style.'</h6>
                                                                                        <p>Launch Date</p>
                                                                                   </div>
                                                                            </div>
                                                                           ');
                                                            echo(' 
                                                          </div>
                                                                ');
                                                     echo('
                                                        <div class="custom-view-opportunity">
                                                           <a href="/companies?id='.$row['companyid'].'" class="view-opportunity">
                                                                View Opportunity
                                                           </a>
                                                        </div>
                                                       ');
                                             echo('
                                                    </div>
                                                    ');
                                            if (!empty($amount_raising_percent))
                                                echo('
                                                    <div class="custom-status-bar-wrapper">
                                                        <div class="custom-status-bar">
                                                            <div class="custom-status-bar-progress" style="width: '.number_format($amount_raising_percent, 0, '.', ',').'%"></div>
                                                        </div>
                                                    </div>');
                                                      } else {
                                                    echo('
                                                     <div class="custom_status_bar_wrapper-content ">
                                                     ');
                                                        if($data_style != null)
                                                            echo('
                                                            <div class="custom-content-icon">
                                                                <div class="custom-media-mini">
                                                                    <div class="custom-media-body-mini pl-2">
                                                                        <p>Advisory Project</p>
                                                                    </div>
                                                                </div>
                                                            </div>    
                                                            ');
                                                           echo('
                                                            <div class="custom-view-opportunity m-10-0">
                                                                <a href="/companies?id='.$row['companyid'].'" class="view-opportunity">
                                                                    View Opportunity
                                                                </a>
                                                            </div>
                                                     </div>
                                                     ');
                                                      }?>
                                            </div>
                                        </div>
                                    </div>
                                <?php endif; ?>
                                <?php
                                $page = sget($_REQUEST, '_page');
                            }
                        }
                            ?>
                    </div>
                    <?php
                        $i = 0;
                        $open_result = get_projects($flag="2");
                        $cnt_all = get_all_number($flag="2");
                        $cmp_cnt = mysql_num_rows($open_result);
                        if ($cmp_cnt > 0) {
                            echo '
                            <div class="custom-coming-soon" >
                                 <div class="open-title">
                                   <b>Coming</b> soon Project
                               </div>
                            </div>
                            <div id="custom-cardholder" class="comingable" >';
                            $teamSizes = get_team_size($flag="2");
                            while (($row = db_fetch_array($open_result)) !== false) {
                                $i++;
                                $recordid = sget($row, 'companyid');
                                $sql = "SELECT data_value FROM maenna_company_data as t
                                        LEFT JOIN maenna_company as t1 ON t.companyid = t1.companyid
                                        WHERE t.data_type = 'financial' AND t.companyid = $recordid AND t.data_attr = YEAR(NOW()) AND t1.comingable = 1";
                                $rev = mysql_query($sql);
                                $rev = mysql_fetch_array($rev);
                                if (!empty($row['project']) && file_exists('themes/maennaco/images/project/' . $row['project'])) {
                                    $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/' . urlencode($row['project']);   // 270*202 . '&zc=1&w=270&h=202'
//                                      $avatar = '/themes/maennaco/custom-images/default_avatar.png';
                                } else {
//                                    $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/big-' . str_replace(' /themes/maennaco/images/', '', getAvatarUrl($row['companyid']));// . '&zc=1&w=270&h=202'
                                      $avatar = '/themes/maennaco/custom-images/default_avatar.png';
                                }
                                ?>
                                <?php if($row['stateable']):?>
                                    <div class="custom-card-wrap">
                                        <div class="custom-card-inner">
                                            <!--                                            <div class="projectName" style="text-align: left;display: block;">--><?//= $row['projname'] ?><!--</div>-->
                                            <a style="cursor:pointer;" href="/companies?id=<?= $row['companyid'];?>">
                                                <img src="<?= $avatar ?>" alt="<?= $row['projname'] ?>" width="100%" height="100%" style="object-fit: cover"/>
                                            </a>
                                            <?php
                                            $cityState = array();
                                            if (!empty($row['city'])) array_push($cityState, ucwords($row['city']));
                                            if (!empty($row['state'])) array_push($cityState, strtoupper($row['state']));
                                            $liking_users = explode(',', $row['liking_users']);
                                            ?>
                                            <!--                                            <div class="city-state" style="color:#898B8E;">--><?//= implode(', ', $cityState) ?><!--</div>-->
                                        </div>

                                        <?php
                                        $sectorRev = array();
                                        if (!empty($row['sector'])) {
                                            array_push($sectorRev, preg_replace('/(?<=\\w)(?=[A-Z])/', ' $1', $row['sector']));
                                            $sectorStr = implode(' - ', $sectorRev);
                                        }

                                        if (!empty($rev['data_value'])) {
                                            if($rev['data_value'] >= 1000000000 )
                                                $revenue = "Rev : $" . (int) ($rev['data_value'] / 1000000000) . " B";
                                            else
                                                $revenue = "Rev : $" . (int) ($rev['data_value'] / 1000000) . " M";
                                        } else $revenue = "Rev : $".(int)$rev['data_value']."";
                                        ?>
                                        <div class='custom-card-inner-right'>
                                            <div class="custom-card-inner-right-top-content">
                                                <ul class="custom-card-inner-wrapper pr-15">
                                                    <li class=" ml-0 pl-0 fs-12 custom-card-inner-item">
                                                        <a class="text-d-n" style="cursor:pointer;" href="/companies?id=<?= $row['companyid']; ?>"><?= $row['projname'] ?></a>
                                                    </li>
                                                    <li class="pl-30 fs-12 custom-card-inner-item">
                                                        <?= implode(', ', $cityState); ?>
                                                    </li>
                                                    <li class="pl-30 fs-12 custom-card-inner-item">
                                                        <span class="custom-categories-span">Industry:</span> <?= $sectorStr; ?>
                                                    </li>
                                                    <li class="ml-auto fs-12 custom-card-inner-item">
                                                        <?php if($row['shareable']):?>
                                                            <span onclick='showContMessage()'><a data-tooltip="
                                                                This tool allows you to connect to learn more and explore fit/qualifications privately. You must have related industry and/or operating expertise for services."
                                                                                                 type='collaborate' style='cursor:pointer; color: #00A2BF;'>Connect</a></span>                                                       </span>
                                                            <span><?= (int) $teamSizes[$recordid]; ?></span>
                                                        <?php endif; ?>
<!--                                                        <span class="revenue"><a href="/companies?id=--><?//= $row['companyid']; ?><!--">Open</a></span>-->
                                                    </li>
                                                </ul>
                                            </div>
                                            <a class="custom-card-inner-text"  href="/companies?id=<?= $row['companyid'];?>">
                                                <div class="custom-card-inner-text-child" >
                                                    <h5 class="custom-subtitile" ><?=$row['deal_summary_title']?></h5>
                                                    <div class="custom-mission" >
                                                        <?= substr(strip_tags($row['deal_summary_statement']), 0, 230) . (!empty($row['deal_summary_statement']) && strlen(strip_tags($row['deal_summary_statement'])) > 230 ? '&hellip;' : '') ?>
                                                    </div>
                                                </div>
                                            </a>
                                            <div class="custom-card-inner-bottom-content">
                                                <?php
                                                $sql = "select sum(amount) sum, COUNT(*) as counts from maenna_professional_investment where company_id=%d and status=3";
                                                $result = db_query($sql, array($recordid));
                                                $row_amount = db_fetch_array($result);
                                                $total_amount = is_null($row_amount['sum']) ? 0 : $row_amount['sum'];
                                                $committed_count = $row_amount['counts'];

                                                $sql_round_amount = "select round_amount_raising as rmount,close_date,security_type from maenna_company_data where companyid=%d and data_type='financial' and data_attr = ".date('Y')."";
                                                $result_amount = db_query($sql_round_amount, array($recordid));
                                                $row_round = db_fetch_array($result_amount);
                                                $goal_amount = is_null($row_round['rmount']) ? 0 : $row_round['rmount'];
                                                $close_date = $row_round['close_date'];
                                                $data_style = date("d/m/y", time($close_date));
                                                $amount_raising_percent = $total_amount / $goal_amount * 100;
                                                if($amount_raising_percent>20) $possible_percent = $amount_raising_percent;
                                                if($goal_amount>0){
                                                    $gola_amount_style = '$' . number_format($goal_amount, 0, '.', ',');
                                                }
                                                $seq_type = $row_round['security_type'];
                                                if($row['fundraising'] == '1'){
                                                    echo('     
                                                    <div class="custom_status_bar_wrapper-content">
                                                          <div class="custom-content-icon">
                                                                <div class="custom-media-mini">
                                                                    <div class="custom-media-body-mini pl-2">
                                                                        <p>Advisory Project</p>
                                                                    </div>
                                                                </div>
                                                          </div>    
                                                            ');
                                                    echo('
                                                           <div class="custom-view-opportunity m-10-0">
                                                               <a href="/companies?id='.$row['companyid'].'" class="view-opportunity">
                                                                   View Opportunity
                                                               </a>
                                                           </div>
                                                    </div>
                                                     ');
                                                } ?>
                                            </div>
                                        </div>
                                    </div>

                                <?php endif; ?>
                                <?php
                                $page = sget($_REQUEST, '_page');
                            }
                        }
                    ?>
                    </div>
                    <?php
                        $i = 0;
                        $open_result = get_projects($flag="3");
                        $cmp_cnt = mysql_num_rows($open_result);
                        $cnt_all = get_all_number($flag="3");
                        if ($cmp_cnt > 0) {
                            echo '
                                    <div class="custom-completed-projects" >
                                         <div class="open-title">
                                          explore completed <b>projects</b>
                                       </div>
                                    </div>
                          <div id="custom-cardholder" class="pastable">';
                            $teamSizes = get_team_size($flag="3");
                            while (($row = db_fetch_array($open_result)) !== false) {
                                $i++;
                                $recordid = sget($row, 'companyid');
                                $sql = "SELECT data_value FROM maenna_company_data as t
                                        LEFT JOIN maenna_company as t1 ON t.companyid = t1.id
                                        WHERE t.data_type = financial AND t.companyid = $recordid AND t.data_attr = YEAR(NOW()) AND t1.pastable = 1";
                                $rev = mysql_query($sql);
                                $rev = mysql_fetch_array($rev);
                                if (!empty($row['project']) && file_exists('themes/maennaco/images/project/' . $row['project'])) {
                                    $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/' . urlencode($row['project']);   // 270*202  . '&zc=1&w=270&h=202'
                                } else {
//                                    $avatar = '/themes/maennaco/phpthumb/phpThumb.php?src=../images/big-' . str_replace(' /themes/maennaco/images/', '', getAvatarUrl($row['companyid'])); // . '&zc=1&w=270&h=202'
                                    $avatar = '/themes/maennaco/custom-images/default_avatar.png';

                                }
                                ?>
                                <?php if($row['stateable']):?>
                                    <div class="custom-completed-card<?php (!($i % 3) ? ' last' : '') ?>">
                                        <a class="custom-completed-href" href="/companies?id=<?= $row['companyid']?>">
                                            <div class="custom-completed-project-name text-d-n"><?= $row['projname'] ?></div>
                                            <div class="custom-completed-avatar">
                                                <img src="<?= $avatar ?>" alt="<?= $row['projname'] ?>" width ="100%;" style="object-fit: cover"/>
                                            </div>
                                            <?php
                                                $cityState = array();
                                                if (!empty($row['city'])) array_push($cityState, ucwords($row['city']));
                                                if (!empty($row['state'])) array_push($cityState, strtoupper($row['state']));
                                                $liking_users = explode(',', $row['liking_users']);
                                            ?>
<!--                                            <div class="city-state">--><?//= implode(', ', $cityState) ?><!--</div>-->
                                            <?php
                                                $sectorRev = array();
                                                if (!empty($row['sector'])) {
                                                    array_push($sectorRev, preg_replace('/(?<=\\w)(?=[A-Z])/', ' $1', $row['sector']));
                                                    $sectorStr = implode(' - ', $sectorRev);
                                                    if (strlen($sectorStr) > 12) {
                                                        $sectorStr = substr($sectorStr, 0, 10) . '&hellip;';
                                                    }
                                                }

                                                if (!empty($rev['data_value'])) {
                                                    if($rev['data_value'] >= 1000000000 )
                                                        $revenue = "Rev - $" . (int) ($rev['data_value'] / 1000000000) . " B";
                                                    else
                                                        $revenue = "Rev - $" . (int) ($rev['data_value'] / 1000000) . " M";
                                                } else $revenue = "Rev - $".(int)$rev['data_value']."";
                                            ?>
                                            <div class="custom-sector-revenue">
                                                <span class="custom-sector"><?= $sectorStr ?></span>
                                                <span class="custom-revenue"><?= $revenue ?></span>
                                            </div>
                                            <div
                                                class='custom-mission' ><?= substr(strip_tags($row['mission']), 0, 228) . (!empty($row['mission']) && strlen(strip_tags($row['mission'])) > 228 ? '&hellip;' : '') ?>
                                            </div>
                                        </a>
                                    </div>
                                <?php endif; ?>
                                <?php if ($i % 3 == 0) { ?>
                                    <br style='clear: both;'>
                                <?php
                                }
                                $page = sget($_REQUEST, '_page');
                            }
                        } else {
                            ?>
                            <!-- <span style="color: #00A2BF;font: 14px 'Lato Bold';margin-top: 20px;margin-left: 20px;display: block;">No visible companies</span> -->
                            <?php
                            // exit;
                        }
                        if ($cnt_all > 3 && $cmp_cnt>0) {
                            ?>
                            <div class="custom-button-by-show-more">
                                <div class="show_more_cmp custom-show_more_cmp" rel="show" type="pastable">see more</div>
                            </div>
                        </div>
                    <?php } ?>
                    </div>
                </div>
</div>
</div>
</div>


