<?php

class Connections {

    public static function conntype() {
        return array('admin', 'follow', 'follower', 'advisor', 'visible');
    }

    public static function DBKeys() {
        return array(
            'connid',
            'conntype',
            'target_uid',
            'assignee_uid',
            'role',
            'access',
            'status', // possible values: pending, active, removed
            'editorid',
            'edittime'
        );
    }

    public static function sload_conn($connid) {
        if (empty($connid)) return false;
        $sql = "select *
                from maenna_connections
                left join maenna_conn_approve
                on maenna_conn_apporve.ap_connid = maenna_connections.connid
                where maenna_connections.connid = %d limit 1";
        $result = db_query($sql, $connid);
        $Row = db_fetch_object($result);
        if ($Row) {
            return $Row;
        } else return false;
    }

    public static function set_connection($target_uid, $assignee_uid, $connid = '', $conntype = '') {
        global $AccessObj;
        $uniqueConn = array('client','advisor','partner','visible','collaborator','propose','investor');
        var_dump($conntype, strcasecmp($conntype, 'none') == 0);
        if (strcasecmp($conntype, 'none') == 0) {
            if ($connid) {
                return self::deactivate_conn($connid);
            }
            return true;
        } elseif (in_array($conntype,$uniqueConn)) {
            if ($connid) {
                return self::change_conntype($connid, $conntype);
            }
            else {
                $Existing_conns = self::load_conn_by_uids($target_uid, $assignee_uid);
                if ($Existing_conns) {
                    foreach ($Existing_conns as $Conn) {
                        if (in_array($Conn['conntype'],$uniqueConn)) {
                            return self::change_conntype($Conn['connid'], $conntype);
                        }
                    }
                }
            }
        }


        elseif (strcasecmp($conntype, 'watchlist') == 0) {
            if ($connid) {
                return self::change_conntype($connid, $conntype);
            } else {
                return self::new_connection(
                    array(
                        'target_uid'   => $target_uid,
                        'assignee_uid' => $assignee_uid,
                        'conntype'     => 'watchlist',
                        'editorid'     => $AccessObj->uid,
                        'edittime'     => time(),
                        'status'       => 'active'
                    )
                );
            }
        }/* elseif (strcasecmp($conntype, 'client') == 0) {
            if ($connid) {
                return self::change_conntype($connid, $conntype);
            } else {
                return self::new_connection(
                    array(
                        'target_uid'   => $target_uid,
                        'assignee_uid' => $assignee_uid,
                        'conntype'     => 'client',
                        'editorid'     => $AccessObj->uid,
                        'edittime'     => time(),
                        'status'       => 'active'
                    )
                );
            }
        }*/ elseif (strcasecmp($conntype, 'related') == 0) {
            if ($connid) {
                return self::change_conntype($connid, $conntype);
            } else {
                return self::new_connection(
                    array(
                        'target_uid'   => $target_uid,
                        'assignee_uid' => $assignee_uid,
                        'conntype'     => 'related',
                        'editorid'     => $AccessObj->uid,
                        'edittime'     => time(),
                        'status'       => 'active'
                    )
                );
            }
        } /*elseif (strcasecmp($conntype, 'advisor') == 0) {
            $Existing_conns = self::load_conn_by_uids($target_uid, $assignee_uid);
            if ($Existing_conns) {
                foreach ($Existing_conns as $Conn) {
                    if ($Conn['conntype'] == 'propose' || $Conn['conntype'] == 'visible' || $Conn['conntype'] == 'advisor' || $Conn['conntype'] == 'partner' || $Conn['conntype'] == 'client') {
                        return self::change_conntype($Conn['connid'], 'advisor');
                    }
                }
            }
            return self::new_connection(
                array(
                    'target_uid'   => $target_uid,
                    'assignee_uid' => $assignee_uid,
                    'conntype'     => 'advisor',
                    'editorid'     => $AccessObj->uid,
                    'edittime'     => time(),
                    'status'       => 'active'
                )
            );
        } elseif (strcasecmp($conntype, 'partner') == 0) {
            $Existing_conns = self::load_conn_by_uids($target_uid, $assignee_uid);
            if ($Existing_conns) {
                foreach ($Existing_conns as $Conn) {
                    if ($Conn['conntype'] == 'propose' || $Conn['conntype'] == 'visible' || $Conn['conntype'] == 'advisor' || $Conn['conntype'] == 'partner' || $Conn['conntype'] == 'client') {
                        return self::change_conntype($Conn['connid'], 'partner');
                    }
                }
            }
            return self::new_connection(
                array(
                    'target_uid'   => $target_uid,
                    'assignee_uid' => $assignee_uid,
                    'conntype'     => 'partner',
                    'editorid'     => $AccessObj->uid,
                    'edittime'     => time(),
                    'status'       => 'active'
                )
            );
        } elseif (strcasecmp($conntype, 'propose') == 0) {
            $Existing_conns = self::load_conn_by_uids($target_uid, $assignee_uid);
            if ($Existing_conns) {
                foreach ($Existing_conns as $Conn) {
                    if ($Conn['conntype'] == 'visible') {
                        if (self::change_conntype($Conn['connid'], 'propose')) {
                            return self::new_conn_approve($connid);
                        } else return false;
                    } elseif ($Conn['conntype'] == 'advisor') {
                        drupal_set_message("Operation Failed - cannot propose an user who's an advisor", 'error');
                        return false;
                    } elseif ($Conn['conntype'] == 'propose') return true;
                }
            }
            //db_last_insert_id('table_name', 'name');
            if (self::new_connection(
                array(
                    'target_uid'   => $target_uid,
                    'assignee_uid' => $assignee_uid,
                    'conntype'     => 'propose',
                    'editorid'     => $AccessObj->uid,
                    'edittime'     => time(),
                    'status'       => 'active'
                )
            )
            ) {
                $connid = db_last_insert_id('maenna_connections', 'connid');
                return self::new_conn_approve($connid);
            } else return false;
        } elseif (strcasecmp($conntype, 'visible') == 0) {
            $Existing_conns = self::load_conn_by_uids($target_uid, $assignee_uid);
            if ($Existing_conns) {
                foreach ($Existing_conns as $Conn) {
                    if ($Conn['conntype'] == 'propose' || $Conn['conntype'] == 'visible' || $Conn['conntype'] == 'advisor' || $Conn['conntype'] == 'partner' ) {
                        return self::change_conntype($Conn['connid'], 'visible');
                    }
                }
            }
            return self::new_connection(
                array(
                    'target_uid'   => $target_uid,
                    'assignee_uid' => $assignee_uid,
                    'conntype'     => 'visible',
                    'editorid'     => $AccessObj->uid,
                    'edittime'     => time(),
                    'status'       => 'active'
                )
            );
        } */elseif (strcasecmp($conntype, 'follow') == 0) {
            $Existing_conns = self::load_conn_by_uids($target_uid, $assignee_uid);
            if ($Existing_conns) {
                foreach ($Existing_conns as $Conn) {
                    if ($Conn['conntype'] == 'follow') {
                        return false;
                    }
                }
            }
            return self::new_connection(
                array(
                    'target_uid'   => $target_uid,
                    'assignee_uid' => $assignee_uid,
                    'conntype'     => 'follow',
                    'editorid'     => $AccessObj->uid,
                    'edittime'     => time(),
                    'status'       => 'active'
                )
            );
        } /*elseif (strcasecmp($conntype, 'collaborator') == 0) {
            if ($connid) {
                return self::change_conntype($connid, $conntype);
            } else {
                return self::new_connection(
                    array(
                        'target_uid'   => $target_uid,
                        'assignee_uid' => $assignee_uid,
                        'conntype'     => 'collaborator',
                        'editorid'     => $AccessObj->uid,
                        'edittime'     => time(),
                        'status'       => 'active'
                    )
                );
            }
        }*/
        return false;
    }

    public static function deactivate_conn($connid) {
        if (empty($connid)) {
            return false;
        }
        global $AccessObj;
        if (!in_array($AccessObj->user_type, array('admin', 'super'))) return false;
        if ($AccessObj->user_type == 'admin') {
            $Conn = self::load_conn($connid);
            if (!$Conn) return false;
            $target_uid = $Conn->target_uid;
            if (!self::admin_of($target_uid)) return false;
        }
//        $sql = "update maenna_connections set status = 'deactiveated', edittime = '%s', editorid = '%s' where connid = %d limit 1";
        $sql = "update maenna_connections set status = 'deactivated', edittime = '%s', editorid = '%s' where connid = %d";
        return db_query($sql, array(time(), $AccessObj->uid, $connid));
    }

    public static function load_conn($connid) {
        if (empty($connid)) return false;
        $sql = "select  *
                from    maenna_connections
                where maenna_connections.connid = %d limit 1";
        $result = db_query($sql, $connid);
        $Row = db_fetch_object($result);
        if ($Row) {
            return $Row;
        } else return false;
    }

    public static function admin_of($id) {
        global $AccessObj;
        $sql = "select count(*) as cnt from maenna_connections where target_uid = '%s' and assignee_uid = '%s' and conntype = 'admin' and status = 'active'";
        $result = db_query($sql, array($id, $AccessObj->uid));
        $Row = db_fetch_object($result);
        if ($Row && $Row->cnt == 1) {
            return true;
        } else return false;
    }

    public static function change_conntype($connid, $conntype) {
        if (empty($connid) || empty($conntype)) return false;
        return db_query("update maenna_connections set conntype = '%s' where connid = %d", array($conntype, $connid));
    }

    public static function new_connection($Param) {
        global $user;
        $Param['editorid'] = $user->uid;
        $Param['edittime'] = $Param['access'] = time();
        $Param_Keys = array_keys($Param);
        $SQL_STR = array();
        foreach ($Param_Keys as $m) $SQL_STR[] = "'%s'";
        $sql = "insert into maenna_connections (" . implode(',', $Param_Keys) . ") values (" . implode(',', $SQL_STR) . ") ";
        $DBValues = $Param;
        return db_query($sql, $DBValues);
    }

    public static function load_conn_by_uids($target_uid, $assignee_uid) {
        if (empty($target_uid) || empty($assignee_uid)) return false;
        $Output = array();
        $sql = "select  conntype,
                        connid
                from    maenna_connections
                where   (target_uid = '%s' and assignee_uid = '%s') and
                        status = 'active'";
        $result = db_query($sql, array($target_uid, $assignee_uid, $assignee_uid, $target_uid));
        while ($Row = db_fetch_array($result)) $Output[] = $Row;
        return $Output;
    }

    public static function new_conn_approve($connid) {
        global $AccessObj;
        $time = time();
        if ($AccessObj->user_type == 'admin' || $AccessObj->user_type == 'super') {
            $sql = "insert into maenna_conn_approve (ap_connid, admin_approval, admin_time) values(%d, '%s', '%s')
                    on duplicate key update admin_approval = '1' , admin_time = '%s'";
            return db_query($sql, array($connid, '1', $time, $time));
        }
        return false;
    }

    public static function set_pro_connection($pro_id, $com_id, $connid = '', $conntype = '') {
        global $AccessObj;
        if (strcasecmp($conntype, 'advisor') == 0) {
            if ($tmp_connid = self::conn_pro_visible($pro_id, $com_id)) {
                self::deactivate_conn($tmp_connid);
            } elseif ($tmp_connid = self::conn_pro_propose($pro_id, $com_id)) {
                self::deactivate_conn($tmp_connid);
            } elseif ($tmp_connid = self::conn_pro_advisor($pro_id, $com_id)) {
                return true;
            }
            return self::new_connection(
                array(
                    'target_uid'   => $com_id,
                    'assignee_uid' => $pro_id,
                    'conntype'     => 'advisor',
                    'editorid'     => $AccessObj->uid,
                    'edittime'     => time(),
                    'status'       => 'active'
                )
            );
        } elseif (strcasecmp($conntype, 'propose') == 0) {
            if ($tmp_connid = self::conn_pro_visible($pro_id, $com_id)) {
                self::deactivate_conn($tmp_connid);
            } elseif ($tmp_connid = self::conn_pro_propose($pro_id, $com_id)) {
                return true;
            } elseif ($tmp_connid = self::conn_pro_advisor($pro_id, $com_id)) {
                drupal_set_message("can not change an advisor connection type to be propose type", 'error');
                return false;
            }
            if (self::new_connection(
                array(
                    'target_uid'   => $com_id,
                    'assignee_uid' => $pro_id,
                    'conntype'     => 'propose',
                    'editorid'     => $AccessObj->uid,
                    'edittime'     => time(),
                    'status'       => 'active'
                )
            )
            ) {
                $connid = db_last_insert_id('maenna_connections', 'connid');
                return self::new_conn_approve($connid);
            } else return false;
        } elseif (strcasecmp($conntype, 'visible') == 0) {
            if ($tmp_connid = self::conn_pro_visible($pro_id, $com_id)) {
                return true;
            } elseif ($tmp_connid = self::conn_pro_propose($pro_id, $com_id)) {
                self::deactivate_conn($tmp_connid);
            } elseif ($tmp_connid = self::conn_pro_advisor($pro_id, $com_id)) {
                self::deactivate_conn($tmp_connid);
            }
            return self::new_connection(
                array(
                    'target_uid'   => $com_id,
                    'assignee_uid' => $pro_id,
                    'conntype'     => 'visible',
                    'editorid'     => $AccessObj->uid,
                    'edittime'     => time(),
                    'status'       => 'active'
                )
            );
        } elseif (strcasecmp($conntype, 'related') == 0) {
            $pro2_id = $com_id;
            if ($tmp_connid = self::conn_pro_related($pro2_id, $pro_id)) {
                return true;
            } else {
                return self::new_connection(
                    array(
                        'target_uid'   => $pro_id,
                        'assignee_uid' => $pro2_id,
                        'conntype'     => 'related',
                        'editorid'     => $AccessObj->uid,
                        'edittime'     => time(),
                        'status'       => 'active'
                    )
                );
            }
        } elseif (strcasecmp($conntype, 'none') == 0) {
            if ($tmp_connid = self::conn_pro_visible($pro_id, $com_id)) {
                self::deactivate_conn($tmp_connid);
            } elseif ($tmp_connid = self::conn_pro_propose($pro_id, $com_id)) {
                self::deactivate_conn($tmp_connid);
            } elseif ($tmp_connid = self::conn_pro_advisor($pro_id, $com_id)) {
                self::deactivate_conn($tmp_connid);
            } elseif ($tmp_connid = self::conn_pro_related($pro_id, $com_id)) {
                self::deactivate_conn($tmp_connid);
            }
            return true;
        } elseif (strcasecmp($conntype, 'follow') == 0) {
            $Existing_conns = self::load_conn_by_uids($pro_id, $com_id);
            if ($Existing_conns) {
                foreach ($Existing_conns as $Conn) {
                    if ($Conn['conntype'] == 'follow') {
                        return false;
                    }
                }
            }
            return self::new_connection(
                array(
                    'target_uid'   => $pro_id,
                    'assignee_uid' => $com_id,
                    'conntype'     => 'follow',
                    'editorid'     => $AccessObj->uid,
                    'edittime'     => time(),
                    'status'       => 'active'
                )
            );
        } elseif (strcasecmp($conntype, 'request') == 0) {
            $Existing_conns = self::load_conn_by_uids($pro_id, $com_id);
            if ($Existing_conns) {
                foreach ($Existing_conns as $Conn) {
                    if ($Conn['conntype'] == 'request') {
                        return false;
                    }
                }
            }
            return self::new_connection(
                array(
                    'target_uid'   => $pro_id,
                    'assignee_uid' => $com_id,
                    'conntype'     => 'request',
                    'editorid'     => $AccessObj->uid,
                    'edittime'     => time(),
                    'status'       => 'active'
                )
            );
        } else {
            var_dump($conntype);
            return false;
        }
    }

    public static function conn_pro_visible($pro_id, $com_id) {
        $sql = "select * from maenna_connections where target_uid = '%s' and assignee_uid = '%s' and conntype = 'visible'  and status = 'active'";
        $result = db_query($sql, array($com_id, $pro_id));
        while ($Row = db_fetch_object($result)) {
            return $Row->connid;
        }
        return false;
    }

    public static function conn_pro_propose($pro_id, $com_id) {
        $sql = "select * from maenna_connections where target_uid = '%s' and assignee_uid = '%s' and conntype = 'propose' and status = 'active'";
        $result = db_query($sql, array($com_id, $pro_id));
        while ($Row = db_fetch_object($result)) {
            return $Row->connid;
        }
        return false;
    }

    public static function conn_pro_advisor($pro_id, $com_id) {
        $sql = "select * from maenna_connections where target_uid = '%s' and assignee_uid = '%s' and conntype = 'advisor'  and status = 'active'";
        $result = db_query($sql, array($com_id, $pro_id));
        while ($Row = db_fetch_object($result)) {
            return $Row->connid;
        }
        return false;
    }

    public static function conn_pro_related($pro_id, $pro2_id) {
        $sql = "select * from maenna_connections where ((target_uid = '%s' and assignee_uid = '%s') or (target_uid = '%s' and assignee_uid = '%s')) and conntype = 'related' and status = 'active'";
        $result = db_query($sql, array($pro_id, $pro2_id, $pro2_id, $pro_id));
        while ($Row = db_fetch_object($result)) {
            return $Row->connid;
        }
        return false;
    }

    public static function assign_admin($target_uid, $assignee_uid) {
        global $user;
        $editorid = $user->uid;
        $time = time();
        $sql = "select * from maenna_connections where status='active' and target_uid = '%s' and conntype = 'admin'";
        $result = db_query($sql, array($target_uid));
        $Row = db_fetch_object($result);
        if ($Row) {
            $connid = $Row->connid;
            $sql = "update maenna_connections set assignee_uid = '%s' where connid = %d";
            return db_query($sql, array($assignee_uid, $connid));
        } else {
            $user_rid = userRoleId($target_uid);
            return self::new_connection(
                array(
                    'target_uid'   => $target_uid,
                    'assignee_uid' => $assignee_uid,
                    'conntype'     => 'admin',
                    'role'         => $user_rid,
                    'editorid'     => $editorid,
                    'edittime'     => $time,
                    'status'       => 'active'
                )
            );
        }
    }

    public static function remove_assigned_admin($target_uid) {
        global $user;
        $editorid = $user->uid;
        $time = time();
        $sql = "update  maenna_connections
                set     status = 'deactivated',
                        editorid='%s',
                        edittime='%s'
                where   target_uid = '%s' and conntype='admin'";
        return db_query($sql, array($editorid, $time, $target_uid));
    }

    public static function _relation() {
        global $user;
        require_once("." . base_path() . path_to_theme() . "/includes/classes/maenna_users.inc");
        $id = sget($_REQUEST, 'id');
        if ($id == '') $id = $user->uid;
        if (empty($id)) return false;
        global $AccessObj;
        if ($AccessObj->user_type == 'admin') {
            $sql = "select * from maenna_connections where target_uid = '%s' and assignee_uid = '%s'";
            $result = db_query($sql, array($id, $AccessObj->uid));
            $Row = db_fetch_object($result);
            if ($Row->conntype == 'admin') {
                return 'au_assign';
            } else return false;
        } elseif ($AccessObj->user_type == 'company') {
            if ($AccessObj->uid == $id) {
                $q = mysql_query("SELECT membership FROM maenna_company WHERE companyid = " . $id);
                $membership = mysql_fetch_array($q);
                return 'cu_' . $membership['membership'];
            } else {
                $Owner_conns = self::Com_conns($id);
                if ($Owner_conns['Inwatchlist']) {
                    foreach ($Owner_conns['Inwatchlist'] as $_Conn) {
                        if ($_Conn->target_uid = $AccessObj->uid) {
                            return 'cu_visible';
                        }
                    }
                }
                return false;
            }
        } elseif ($AccessObj->user_type == 'people') {
            $Owner_conns = self::Com_conns($id);
            if (count($Owner_conns['Advisor']) > 0) {
                foreach ($Owner_conns["Advisor"] as $_Conn) {
                    if ($_Conn->assignee_uid == $AccessObj->uid) {
                        switch ($AccessObj->role_id) {
                            case 4:
                                return 'pu_analyst';
                                break;
                            case 5:
                                return 'pu_executive';
                                break;
                            case 7:
                                return 'pu_other';
                                break;
/*                            case 11:
                                return 'pu_partner';
                                break;*/
                            case 13:
                                return 'pu_other';
                                break;
                            case 14:
                                return 'pu_other';
                                break;
                        }
                    }
                }
            }
            if ($Owner_conns["Visible"]) {
                foreach ($Owner_conns["Visible"] as $_Conn) {
                    if ($_Conn->assignee_uid == $AccessObj->uid) return 'pu_visible';
                }
            }
            if (count($Owner_conns['Follower']) > 0) {
                foreach ($Owner_conns['Follower'] as $_Conn) {
                    if ($_Conn->assignee_uid == $AccessObj->uid) {
                        return 'pu_visible';
                    }
                }
            }
            if ($Owner_conns['Propose']) {
                foreach ($Owner_conns['Propose'] as $_Conn) {
                    if ($_Conn->assignee_uid == $AccessObj->uid) {
                        return 'pu_visible';
                    }
                }
            }
            if ($Owner_conns['Client']) {
                foreach ($Owner_conns['Client'] as $_Conn) {
                    if ($_Conn->assignee_uid == $AccessObj->uid) {
                        return 'pu_client';
                    }
                }
            }
            if ($Owner_conns['Partner']) {
                foreach ($Owner_conns['Partner'] as $_Conn) {
                    if ($_Conn->assignee_uid == $AccessObj->uid) {
                        return 'pu_partner';
                    }
                }
            }
            //CP DEV - Check if user is collaborator and load those permissions,it will load collaborators permission only if user is none of the above.
            if (in_array($id, $AccessObj->coll_companies)) return 'pu1_collaborators';
            return false;
        }
    }

    public static function Com_conns($id) {
        $Conns = array(
            'Admin'        => array(),
            'Follower'     => array(),
            'Follow'       => array(),
            'Advisor'      => array(),
            'Watchlist'    => array(),
            'Inwatchlist'  => array(),
            'Visible'      => array(), //
            'Propose'      => array(),
            'Collaborator' => array(),
            'Investor'     => array(),
            'Client'       => array(),
            'Partner'      => array()
        );
        if (empty($id)) return $Conns;
        $sql = "select  *
                from    maenna_connections
                where   status = 'active' and
                        ((assignee_uid = '%s') or (target_uid = '%s'))
                order by conntype
                ";
        $result = db_query($sql, array($id, $id));
        while ($Row = db_fetch_object($result)) {
            $conntype = ucwords($Row->conntype);
            if ($Row->conntype == 'follow') {
                if ($Row->target_uid == $id) {
                    $Conns['Follower'][] = $Row;
                } else $Conns['Follow'][] = $Row;
            } elseif ($Row->conntype == 'watchlist') {
                if ($Row->target_uid == $id) {
                    $Conns['Watchlist'][] = $Row;
                } // target com is the one being watched.
                else $Conns['Inwatchlist'] = $Row;
            } elseif ($Row->conntype == 'visible') {
                if ($Row->target_uid == $id) $Conns['Visible'][] = $Row; // target com is the one being watched.
            } elseif ($Row->conntype == 'collaborator') {
                if ($Row->target_uid == $id) $Conns['Collaborator'][] = $Row;
            } elseif ($Row->conntype == 'partner') {
                if ($Row->target_uid == $id) $Conns['Partner'][] = $Row;
            } else {
                $Conns["$conntype"][] = $Row;
            }
        }
        return $Conns;
    }

    public static function admin_connections($admin_id) {
        if (empty($admin_id)) return array();
        $companyid = sget($_REQUEST, 'id');
        //get companies
        $Conns = array(
            'Com' => array(),
            'Pro' => array()
        );
        $sql = "select  *
                from    maenna_connections
                where   conntype='admin' and
                        status = 'active' and
                        assignee_uid = %d";
        $result = db_query($sql, array($admin_id, $companyid));
        while ($Row = db_fetch_object($result)) {
            if ($Row->role == 3) {
                $Conns['Com'][] = $Row;
            } else $Conns['Pro'][] = $Row;
        }
        return $Conns;
    }

    public static function Connected_pros($id) {
        if (empty($id)) return array();
        $sql = "select  *
                from    maenna_connections
                where  status = 'active' and target_uid = %d
                and (conntype = 'advisor' or conntype = 'visible' or conntype = 'client' or conntype = 'propose' or conntype='collaborator' or conntype = 'partner' or conntype = 'investor')";
        $result = db_query($sql, array($id));
        while ($Row = db_fetch_object($result)) $Conns[] = $Row;
        return $Conns;
    }

    public static function Pro_conns($id) {
        if (empty($id)) return array();

        //get companies
        $Connections = array(
            'Admin'    => array(), // only one admin for each com. and pro. user
            'Follow'   => array(), // can only follow either com. or pro. type users
            'Follower' => array(), // can only be followed by com or pro
            'Related'  => array(), // visible relation among pros. equiv. watchlist for com.
            'Advisor'  => array(), // pro is is always the assignee_id
            'Propose'  => array(), // has a separate approval setting table maenna_conn_approve
            'Visible'  => array(), //
            'Following' => array(),
            'Invitations' => array(),
			'Investor' => array(),
        );
        $sql = "select *
                from maenna_connections
                where status = 'active' and ((assignee_uid = '%s') or (target_uid = '%s'))
                order by conntype
                ";
        $result = db_query($sql, array($id, $id));
        while ($Row = db_fetch_object($result)) {
            $conntype = ucwords($Row->conntype);
            if ($Row->conntype == 'follow') {
                if ($Row->target_uid == $id) {
                    $Conns['Follower'][] = $Row;
                } else $Conns['Follow'][] = $Row;
            } elseif ($Row->conntype == 'related') {
                $Conns['Related'][] = $Row;
            }elseif ($Row->conntype == 'visible') {
                if ($Row->assignee_uid == $id) $Conns['Visible'][] = $Row;
            } else {
                $Conns["$conntype"][] = $Row;
            }
        }
        $sql = "select  uid
                from    maenna_followers
                where
                        cid = %d
                ";
        $result = db_query($sql, $id);
        while ($Row = db_fetch_object($result)) {
            $Conns['Following'][] = $Row->uid;
        }
        $sql = "select  code_to_refer
                from    maenna_people
                where
                        pid = %d
                ";
        $result = db_query($sql, $id);
        $code_to_refer = db_fetch_object($result);
        $code_to_refer = $code_to_refer->code_to_refer;

        $sql = "
            select uid
            FROM users u
            LEFT JOIN maenna_people mp on u.uid = mp.pid
            LEFT JOIN maenna_company mc ON u.uid = mc.companyid
            WHERE (
                mp.referral_code = '%s'
                OR mc.referral_code = '%s'
            )
            and u.status = 1
            and NOW() < DATE_ADD(FROM_UNIXTIME(u.created), INTERVAL 1 YEAR)
            ";
        $result = db_query($sql, $code_to_refer,$code_to_refer);
        while ($Row = db_fetch_object($result)) {
            $Conns['Invitations'][] = $Row->uid;
        }
        return $Conns;
    }

    public static function is_following($target_uid, $assignee_uid) {
        if ($target_uid && $assignee_uid) {
            $sql = "select * from maenna_connections where conntype='follow' and target_uid = '%s' and assignee_uid = '%s' and status = 'active'";
            $result = db_query($sql, array($target_uid, $assignee_uid));
            $Row = db_fetch_object($result);
            if ($Row) return true;
        }
        return false;
    }

    public static function allow_to_request($com_id, $pro_id) {
        if (empty($com_id) || empty($pro_id)) return false;
        $sql = "select     * from maenna_connections
                where       status = 'active' and
                            ((target_uid = '%s' and assignee_uid == '%s') or
                            (assignee_uid = '%s' and target_uid == '%s'))";
        $result = db_query($sql, array($com_id, $pro_id, $pro_id, $com_id));
        while ($Row = db_fetch_object($result)) {
            if ($Row->conntype == 'advisor' || $Row->conntype == 'propose' || $Row->conntype == 'request') {
                echo $Row->connid;
                return false;
            }
        }
        return true;
    }
    // returns true if
    // for com user: target pro connection type is visible, advisor, proposed or requested.
    // for pro user: target pro connection type is related
    function pro_is_visible($pro_id) {
        global $AccessObj;
        // one way
        $sql = "select *
                from    maenna_connections
                where   (target_uid = '%s' and assignee_uid = '%s') or
                        (target_uid = '%s' and assignee_uid = '%s')";
        $result = db_query($sql, $pro_id, $AccessObj->uid, $AccessObj->uid, $pro_id);
        while ($Row = db_fetch_object($result)) {
            if (in_array($Row->conntype, array('visible', 'advisor', 'request', 'propose', 'related', 'client'))) return true;
        }
        //Another way, if a user has a write permission on connection section on any of the tabs.
        //This creates a backdoor but Hiwot wanted it this way.
        //Since in this moment we do not have Com_sections array formatted we have to check permissions by section id directly from database.
        switch ($AccessObj->role_id) {
            case 4:
                $relation_type = 'pu_analyst';
                break;
            case 5:
                $relation_type = 'pu_executive';
                break;
            case 7:
                $relation_type = 'pu_other';
                break;
            case 11:
                $relation_type = 'pu_partner';
                break;
            case 11:
                $relation_type = 'pu_client';
                break;
        }
        $Perm = $this->load_permission($relation_type);
        $perm_array = Maenna_page::mae_com_sections();
        foreach ($perm_array as $tab_name => &$Tab_data) {
            $tab_access = 'hide';
            foreach ($Tab_data['sections'] as $section_name => &$Section_data) {
                $section_id = $Section_data['id'];
                $Section_data['access'] = $Perm["$section_id"];
                if ($Perm["$section_id"] != 'hide') $tab_access = 'read';
                if ($section_name == 'connections') {
                    if ($Perm["$section_id"] == 'write') return true;
                }
            }
            $Tab_data['access'] = $tab_access;
        }
        //Checking if logged in user is company and if is following proffesional whose profile is being questioned
        if ($AccessObj->user_type == 'company') {
            global $user;
            $sql = "select *
                from maenna_followers
                where cid = %d and uid = %d";
            $result = db_query($sql, array($user->uid, $pro_id), $die_on_error = true);
            if (mysql_num_rows($result)) return true;
        }
        return false;
    }

    public static function get_fundraising_statistics_area_visible($target_uid) {
        $sql = "select * from maenna_connections where status='active' and target_uid = '%s' and conntype = 'admin'";
        $result = db_query($sql, array($target_uid));
        $Row = db_fetch_object($result);

        return $Row;
    }
}
