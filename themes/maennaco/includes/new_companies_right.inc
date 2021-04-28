<?php

function cmp_type($op = null) {
    global $user;
    global $redirect;
    global $AccessObj;
    $editorid = $user->uid;

    $Block['title'] = 'Company Type';
    $content = '';
    $time = time();

    $_ProType = _cmp_types();
    $type_keys = implode(',',array_keys($_ProType));

    if($op || true )
    {
        if($AccessObj->user_type == 'super')
        {
            $sql = "select  count(maenna_company.companyid) as cnt,
                            membership
                    from    maenna_company,
                            users
                    where
                            users.uid = maenna_company.companyid
                            and users.status = 1
                    group by membership";

        }
        elseif($AccessObj->user_type == 'admin')
        {
            $sql = "select  count(maenna_company.companyid) as cnt,
                            membership
                    from
                            maenna_company,
                            maenna_connections,
                            users
                    where   users.uid = maenna_company.companyid
                            and users.status = 1 and
                            maenna_connections.target_uid = maenna_company.companyid and
                            maenna_connections.conntype='admin' and
                            maenna_connections.status = 'active' and
                            maenna_connections.assignee_uid = '". $AccessObj->uid ."'
                            group by membership";

        }else { return ''; }

        $result = db_query($sql);
        while($Row = db_fetch_object($result))
        {
            $cnt = $Row->cnt;
            $cmp_type =  ($Row->membership);
            $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=companies&cmpType=".$cmp_type."'><div style='position:absolute;text-align:right;width:100%'>$cnt</div>".strtoupper($cmp_type)."</div></a>";
        }
//        $content .= "<div style='border-top:solid 1px #ebebeb;text-align:right;'>".
//                    "<a href='/account?a=professionals&panel=pro_types_panel'>more</a></div>";
        $Block['body'] = sidebar_box($Block['title'], $content);
    }
    return $Block;
}

function cmp_status($op = null) {
    global $user;
    global $redirect;
    global $AccessObj;
    $editorid = $user->uid;

    $Block['title'] = 'User Status';
    $content = '';
    $time = time();

    $_ProType = _pro_roles();
    $type_keys = implode(',',array_keys($_ProType));

    if($op || true)
    {
        if($AccessObj->user_type == 'super')
        {
            $sql1 = "SELECT COUNT(*) as rej_cnt FROM rejected_users WHERE user_type = '%s'";
            $rej = db_query($sql1,'company');
            $rej_cnt = db_fetch_object($rej);

            $sql2 = "select DISTINCT user_id from maenna_professional_payments mpp
				JOIN users u ON mpp.user_id = u.uid
				where u.status = 1 and exists (select companyid FROM maenna_company WHERE companyid = mpp.user_id);
                     ";
            $ins = db_query($sql2);
            $ins_cnt = mysql_num_rows($ins);

            $sql3 = "SELECT COUNT(*) as vis_cnt FROM maenna_company mc JOIN users u ON mc.companyid = u.uid where  u.status = 1 and mc.public = 1";
            $vis = db_query($sql3);
            $vis_cnt = db_fetch_object($vis);

            $sql4 = "SELECT COUNT(*) as reg_cnt FROM maenna_company mc JOIN users u ON mc.companyid = u.uid where u.status = 1";
            $reg = db_query($sql4);
            $reg_cnt = db_fetch_object($reg);

            $sql5 = "SELECT COUNT(*) as act_cnt FROM maenna_company mc JOIN users u ON mc.companyid = u.uid where u.status = 1 and mc.active = 0";
            $act = db_query($sql5);
            $act_cnt = db_fetch_object($act);

            $sql6 = "SELECT COUNT(*) as ftd_cnt FROM maenna_company mc JOIN users u ON mc.companyid = u.uid where u.status = 1 and mc.featured = 1";
            $ftd = db_query($sql6);
            $ftd_cnt = db_fetch_object($ftd);


            $sql7 = "SELECT COUNT(CASE WHEN mc.fundraising = 1 THEN 1 END)         AS fnd_cnt,
                            COUNT(CASE WHEN mc.started_fundraising = 1 THEN 1 END) AS started_fnd_cnt
                    FROM maenna_company mc JOIN users u ON mc.companyid = u.uid
                    WHERE u.status = 1";
            $fnd = db_query($sql7);
            $fnd_cnt = db_fetch_object($fnd);

            $sql8 = "SELECT COUNT(*) as fop_cnt FROM maenna_company mc JOIN users u ON mc.companyid = u.uid where u.status = 1 and mc.stateable = 1";
            $fop = db_query($sql8);
            $fop_cnt = db_fetch_object($fop);

            $sql9 = "SELECT COUNT(*) as fcp_cnt FROM maenna_company mc JOIN users u ON mc.companyid = u.uid where u.status = 1 and mc.stateable = 2";
            $fcp = db_query($sql9);
            $fcp_cnt = db_fetch_object($fcp);

            $sql10 = "SELECT COUNT(*) as fpp_cnt FROM maenna_company mc JOIN users u ON mc.companyid = u.uid where u.status = 1 and mc.stateable = 3";
            $fpp = db_query($sql10);
            $fpp_cnt = db_fetch_object($fpp);

        }

        elseif($AccessObj->user_type == 'admin') {



        $sql1 = "SELECT COUNT(*) as rej_cnt FROM rejected_users WHERE user_type = '%s'";
        $rej = db_query($sql1,'company');
        $rej_cnt = db_fetch_object($rej);

        $sql2 = "select DISTINCT user_id from maenna_professional_payments mpp
				JOIN users u ON mpp.user_id = u.uid
				where u.status = %d and exists (select companyid FROM maenna_company WHERE companyid = mpp.user_id)
				and exists(select target_uid FROM maenna_connections where target_uid = mpp.user_id and assignee_uid = %d and conntype = '%s' and status = %d )

                     ";
        $ins = db_query($sql2,1,$AccessObj->uid,'admin','active');
        $ins_cnt = mysql_num_rows($ins);

        $sql3 = "SELECT COUNT(*) as vis_cnt FROM maenna_company mc JOIN users u ON mc.companyid = u.uid where  u.status = %d and mc.public = %d
                and exists (select target_uid FROM maenna_connections where target_uid = mc.companyid and assignee_uid = %d and conntype = '%s' and status = %d)
        ";
        $vis = db_query($sql3,1,1,$AccessObj->uid,'admin','active');
        $vis_cnt = db_fetch_object($vis);

        $sql4 = "SELECT COUNT(*) as reg_cnt FROM maenna_company mc JOIN users u ON mc.companyid = u.uid where u.status = %d
                and exists (select target_uid FROM maenna_connections where target_uid = mc.companyid and assignee_uid = %d and conntype = '%s' and status = %d)
        ";
        $reg = db_query($sql4,1,$AccessObj->uid,'admin','active');
        $reg_cnt = db_fetch_object($reg);

        $sql5 = "SELECT COUNT(*) as act_cnt FROM maenna_company mc JOIN users u ON mc.companyid = u.uid where u.status = %d and mc.active = %d
                and exists(select target_uid FROM maenna_connections where target_uid = mc.companyid and assignee_uid = %d and conntype = '%s' and status = %d)
        ";
        $act = db_query($sql5,1,0,$AccessObj->uid,'admin','active');
        $act_cnt = db_fetch_object($act);
            
        $sql6 = "SELECT COUNT(*) as ftd_cnt FROM maenna_company mc JOIN users u ON mc.companyid = u.uid where u.status = %d and mc.featured = %d
                and exists(select target_uid FROM maenna_connections where target_uid = mc.companyid and assignee_uid = %d and conntype = '%s' and status = %d)
        ";
            $ftd = db_query($sql6,1,1,$AccessObj->uid,'admin','active');
            $ftd_cnt = db_fetch_object($ftd);


    }

        else { return ''; }

        $content = "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=companies&cmpStatus=featured'><div style='position:absolute;text-align:right;width:100%'>$ftd_cnt->ftd_cnt</div>FEATURED</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=companies'><div style='position:absolute;text-align:right;width:100%'>$reg_cnt->reg_cnt</div>REGISTERED</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=companies&cmpStatus=visible'><div style='position:absolute;text-align:right;width:100%'>$vis_cnt->vis_cnt</div>VISIBLE</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><div style='position:absolute;text-align:right;width:100%'>$rej_cnt->rej_cnt</div><a href='#'>DISQUALIFIED</a></div>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=companies&cmpStatus=deactivated'><div style='position:absolute;text-align:right;width:100%'>$act_cnt->act_cnt</div>DEACTIVATED</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=companies&cmpStatus=insight'><div style='position:absolute;text-align:right;width:100%'>$ins_cnt</div>ATTENDING INSIGHTS</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=companies&cmpStatus=started_fundraising'><div style='position:absolute;text-align:right;width:100%'>$fnd_cnt->started_fnd_cnt</div>FUNDRAISING</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=companies&cmpStatus=fundraising'><div style='position:absolute;text-align:right;width:100%'>$fnd_cnt->fnd_cnt</div>INVESTING</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=companies&cmpStatus=opening'><div style='position:absolute;text-align:right;width:100%'>$fop_cnt->fop_cnt</div>OPEN</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=companies&cmpStatus=coming'><div style='position:absolute;text-align:right;width:100%'>$fcp_cnt->fcp_cnt</div>COMING</div></a>";
        $content .= "<div class=row style='padding:2px 0;position:relative;'><a href='/account?tab=companies&cmpStatus=pasting'><div style='position:absolute;text-align:right;width:100%'>$fpp_cnt->fpp_cnt</div>PAST</div></a>";

        $Block['body'] = sidebar_box($Block['title'], $content);


    return $Block;
}

}
