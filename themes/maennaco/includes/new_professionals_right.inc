<?php
/*
pro_types
pro_memberstats
mlvl_stats
sector_stats
expertis_stats
*/
function pro_filter() {
    $content = '    <div id="cmp-filter-cont">

    <span class="cmp-filter">Filter by</span>

    <span class="filter-parent" rel="type-filter">Professional Type</span><div style="height:1px;clear:both;"></div>';
    if ($_REQUEST['type'] == '') {
        $type_display = 'style="display:none"';
    } else {
        $type_display = 'class="open"';
    }
    if ($_REQUEST['expertise'] == '') {
        $exp_display = 'style="display:none"';
    } else {
        $exp_display = 'class="open"';
    }
    $content .= ' <div id="type-filter" ' . $type_display . '>';
    $_ProType = _pro_roles();
    unset($_ProType['12']);//12 => 'Company Member' #389 issue
    $type_keys = implode(',', array_keys($_ProType));
    $sql = "SELECT COUNT(`users_roles`.`rid`) AS cnt,
                            rid
                    FROM    users_roles,
                            maenna_people,
                            users
                    WHERE   users_roles.rid IN ($type_keys)
                    AND     users_roles.uid = maenna_people.pid
                    AND     users.uid = users_roles.uid
                    AND     users.status = 1
                    AND     maenna_people.public = 1
                    GROUP BY users_roles.rid";
    $result = db_query($sql);
    $url = rebuild_url(array('tab', 'id', 'm', 'edit', 'pid'));
    $content .= "<span class='filter-entry " . ($_GET['type'] == '' ? 'filter-active' : '') . "' filter='industry'><a href='" . $url . "&expertise=" . $_GET['expertise'] . "'>All</a></span>";
    while ($Row = db_fetch_object($result)) {
        $cnt = $Row->cnt;
        $rid = ($Row->rid);
        $type = ucfirst($_ProType["$rid"]);
        if ($_REQUEST['type'] == $type) $selected = 'filter-active'; else $selected = '';
        $content .= "<span class='filter-entry " . $selected . "' filter='industry'><a href='" . $url . "&expertise=" . $_GET['expertise'] . "&type=" . $type . "'>" . $type . "</a></span>";
    }
    $content .= '</div>
        <span class="filter-parent filterParent" rel="expertise-filter">Expertise</span>
        <div style="height:1px;clear:both;"></div>
        <div id="expertise-filter" ' . $exp_display . '>';
    $content .= "
    <span class='filter-entry " . ($_GET['expertise'] == '' ? 'filter-active' : '') . "' filter='industry'>
        <a href='" . $url . "&type=" . $_GET['type'] . "'>
            All
        </a>
    </span>";
    $expertise = _experties();
    foreach ($expertise as $key => $value) {
        if ($_REQUEST['expertise'] == $key) $selected = 'filter-active'; else $selected = '';
        if ($key != '') {
            $content .= "
            <span class='filter-entry " . $selected . "' filter='industry'>
                <a href='" . $url . "&expertise=" . $key . "&type=" . $_GET['type'] . "'>" . (strlen($key) > 25 ? substr($key, 0, 25) . '...' : $key) . "</a>
            </span>";
        }
    }
    echo $content;
    ?>
    </div>
    </div>
    </div>
    <script lang="javascript">
        $(document).ready(function () {
            $(".filter-parent").click(function () {
                rel = $("#" + $(this).attr('rel'));
                rel_str = $(this).attr('rel').substring(0, $(this).attr('rel').indexOf('-'));
                if (rel.hasClass('open')) {
                    var regex = new RegExp("&?" + rel_str + "=([^&]$|[^&]*)", "i");
                    location.href = location.href.replace(regex, "");
                } else {
                    rel.show();
                    rel.addClass('open');
                }
            });
        });
    </script>
    <style>
        span.filter-entry a {
            color: inherit !important;
        }
    </style>
<?php
}

function pro_types($op = null) {
    global $user;
    global $redirect;
    global $AccessObj;
    $editorid = $user->uid;
    $Block['title'] = 'Professional Type';
    $content = '';
    $time = time();
    $_ProType = _pro_roles();
    $type_keys = implode(',', array_keys($_ProType));
    if ($op) {
        if ($AccessObj->user_type == 'super') {
            $sql = "select  count(users_roles.rid) as cnt,
                            rid
                    from    users_roles,
                            maenna_people,
                            users
                    where   users_roles.rid in ($type_keys) and
                            users_roles.uid = maenna_people.pid and
                            users.uid = users_roles.uid
                            and users.status = 1
                    group by users_roles.rid";
        } elseif ($AccessObj->user_type == 'admin' || $AccessObj->user_type == 'sales') {
            $sql = "select  count(rid) as cnt,
                            rid
                    from    users_roles,
                            maenna_people,
                            maenna_connections,
                            users
                    where   rid in ($type_keys) and
                            users_roles.uid = pid and
                            users_roles.uid = users.uid and
                            users.status = 1 and
                            maenna_connections.target_uid = users_roles.uid and
                            maenna_connections.conntype='admin' and
                            maenna_connections.status = 'active' and
                            maenna_connections.assignee_uid = '" . $AccessObj->uid . "'
                            group by rid";
        } else {
            return '';
        }
        $result = db_query($sql);
        while ($Row = db_fetch_object($result)) {
            $cnt = $Row->cnt;
            $rid = ($Row->rid);
            $type = strtoupper($_ProType["$rid"]);
            $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=professionals&proTypeSel=" . $rid . "'><div style='position:absolute;text-align:right;width:100%'>$cnt</div>$type</div></a>";
        }
//        $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
//                    "<a href='/account?a=professionals&panel=pro_types_panel'>more</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}

function pro_status($op = null) {
    global $user;
    global $redirect;
    global $AccessObj;
    $editorid = $user->uid;
    $Block['title'] = 'User Status';
    $content = '';
    $time = time();
    $_ProType = _pro_roles();
    $type_keys = implode(',', array_keys($_ProType));
    if ($op) {
        if ($AccessObj->user_type == 'super') {
            $sql0 = "SELECT DISTINCT pid FROM maenna_people mp JOIN users u ON mp.pid = u.uid WHERE u.status = 1 AND mp.approved = 1";
            $app = db_query($sql0);
            $app_cnt = mysql_num_rows($app);
            $sql1 = "SELECT COUNT(*) as rej_cnt FROM rejected_users WHERE user_type = '%s'";
            $rej = db_query($sql1, 'professional');
            $rej_cnt = db_fetch_object($rej);
            $sql2 = "SELECT DISTINCT mp.postedby FROM maenna_professional mp JOIN users u on mp.postedby = u.uid
                     where u.status = 1
                     and exists (SELECT pid FROM maenna_people WHERE pid = mp.postedby);
                     ";
            $ins = db_query($sql2);
            $ins_cnt = mysql_num_rows($ins);
            $sql3 = "SELECT DISTINCT assignee_uid from maenna_connections mc JOIN users u ON u.uid = mc.assignee_uid   where conntype = '%s' and mc.status = '%s' and u.status = %d";
            $coll = db_query($sql3, array('collaborator', 'active', 1));
            $coll_cnt = mysql_num_rows($coll);
            $sql4 = "SELECT COUNT(*) as deact_cnt FROM maenna_people mp JOIN users u ON mp.pid = u.uid WHERE mp.active = %d and u.status = %d ";
            $deact = db_query($sql4, 0, 1);
            $deact_cnt = db_fetch_object($deact);
            $sql5 = "SELECT COUNT(*) as visible FROM maenna_people mp JOIN users u ON mp.pid = u.uid WHERE mp.public = %d and u.status = %d ";
            $visible = db_query($sql5, 1, 1);
            $vis_cnt = db_fetch_object($visible);
        } elseif ($AccessObj->user_type == 'admin') {
            $sql0 = "SELECT DISTINCT pid FROM maenna_people mp JOIN users u ON mp.pid = u.uid WHERE u.status = %d AND mp.approved = %d

                    and (exists (select target_uid FROM maenna_connections where target_uid = mp.pid and assignee_uid = %d and conntype = '%s' and status = %d) OR mp.approved = 1)
            ";
            $app = db_query($sql0, 1, 1, (int) $AccessObj->uid, 'admin', 'active');
            $app_cnt = mysql_num_rows($app);
            $sql1 = "SELECT COUNT(*) as rej_cnt FROM rejected_users WHERE user_type = '%s'";
            $rej = db_query($sql1, 'professional');
            $rej_cnt = db_fetch_object($rej);
            $sql2 = "SELECT DISTINCT mp.postedby FROM maenna_professional mp JOIN users u on mp.postedby = u.uid
                     where u.status = %d
                     and exists (SELECT pid FROM maenna_people WHERE pid = mp.postedby)
                     and (
                     exists (select target_uid FROM maenna_connections where target_uid = mp.postedby and assignee_uid = %d and conntype = '%s' and status = %d)
                     or exists (select pid FROM maenna_people where approved = 1 and pid = mp.postedby))

                     ";
            $ins = db_query($sql2, 1, $AccessObj->uid, 'admin', 'active');
            $ins_cnt = mysql_num_rows($ins);
            $sql3 = "SELECT DISTINCT assignee_uid from maenna_connections mc JOIN users u ON u.uid = mc.assignee_uid   where conntype = '%s' and mc.status = '%s' and u.status = %d
            and (
            exists (select target_uid FROM maenna_connections where target_uid = mc.assignee_uid and assignee_uid = %d and conntype = '%s' and status = %d)
            or exists (SELECT pid from maenna_people where pid = mc.assignee_uid and approved = 1))

            ";
            $coll = db_query($sql3, array('collaborator', 'active', 1, $AccessObj->uid, 'admin', 'active'));
            $coll_cnt = mysql_num_rows($coll);
            $sql4 = "SELECT COUNT(*) as deact_cnt FROM maenna_people mp JOIN users u ON mp.pid = u.uid WHERE mp.active = %d and u.active = %d ";
            $deact = db_query($sql4, 0, 1);
            $deact_cnt = db_fetch_object($deact);
            $sql5 = "SELECT COUNT(*) as visible FROM maenna_people mp JOIN users u ON mp.pid = u.uid WHERE mp.public = %d and u.status = %d ";
            $visible = db_query($sql5, 1, 1);
            $vis_cnt = db_fetch_object($visible);
        } else {
            return '';
        }
        $content = "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=professionals&ustatus=approved'><div style='position:absolute;text-align:right;width:100%'>$app_cnt</div>APPROVED</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=professionals&ustatus=visible'><div style='position:absolute;text-align:right;width:100%'>$vis_cnt->visible</div>VISIBLE</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=professionals&ustatus=insights'><div style='position:absolute;text-align:right;width:100%'>$ins_cnt</div>STARTED INSIGHTS</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=professionals&ustatus=contributing'><div style='position:absolute;text-align:right;width:100%'>$coll_cnt</div>CONNECTED</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=professionals&ustatus=deactivated'><div style='position:absolute;text-align:right;width:100%'>$deact_cnt->deact_cnt</div>DEACTIVATED</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><div style='position:absolute;text-align:right;width:100%'>$rej_cnt->rej_cnt</div><a href='#'>DISQUALIFIED</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}

function pro_memberstats($op = null) {
    global $user;
    global $redirect;
    $editorid = $user->uid;
    $Block['title'] = 'Member Statistics';
    $content = '';
    $time = time();
    if ($op) {
        $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>" .
            "<a href='$redirect&panel=pro_memberstats_panel'>more</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}

function mlvl_stats($op = null) {
    global $user;
    global $redirect;
    $editorid = $user->uid;
    $Block['title'] = 'Background Check';
    $content = '';
    $time = time();
    if ($op) {
        $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>" .
            "<a href='$redirect&panel=mlvl_stats_panel'>more</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}

function sector_stats($op = null) {
    global $user;
    global $redirect;
    $editorid = $user->uid;
    $Block['title'] = 'Sector Statistics';
    $content = '';
    $time = time();
    if ($op) {
        $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>" .
            "<a href='$redirect&panel=sector_stats_panel'>more</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}

function expertis_stats($op = null) {
    global $user;
    global $redirect;
    $editorid = $user->uid;
    $Block['title'] = 'Expertise Statistics';
    $content = '';
    $time = time();
    if ($op) {
        $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>" .
            "<a href='$redirect&panel=expertis_stats_panel'>more</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}


/* EOF */
