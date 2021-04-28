<?php

if( !isset($_GET['q']) || empty($_GET['q']) ){
    exit;
}

require_once 'dbcon.php';
$queryString = mysql_real_escape_string($_GET['q'], $conn);
$queryRoles =  mysql_real_escape_string($_GET['roles'], $conn);
$queryUid = mysql_real_escape_string($_GET['uid'], $conn);
$queryMd5 = mysql_real_escape_string($_GET['md5'], $conn);

if (md5($queryRoles.$queryUid."bljoKica75")!=$queryMd5) { print "security error!"; exit; }

$roles = explode(",", $queryRoles);



echo '<table border="0" cellpadding="0" cellspacing="0" id="listOfPeople">';
if( strpos('maennaco admin', strtolower($_GET['q'])) !== false ){
    $limit = 12;
    // I have to put uid 60 + 100 = 160
    if (array_search("Maennaco admin", $roles)) 
	    echo '<tr>
		    <td bgcolor="#FFFFFF" uid="160" onclick="putNameInsideToField(this);">Maennaco Admin</td>
		  </tr>';   // Super Admin uid is 60, for now
        /* if Super Admin uid is not 60, then do
            SELECT uid
              FROM users_roles
             WHERE rid IN (SELECT rid
                             FROM role
                            WHERE name = 'Super admin');
        */
} else {
    $limit = 12;
}

if(preg_match('/0*[1-9]+$/', $queryString) > 0){ // if $queryString end on number like 'aena02'
    preg_match('/^(\D*)(\d+)$/', $queryString, $matches);   // split it on number part and
    $numberPart = (int)$matches[2];
    $stringPart = strtoupper($matches[1]);
    $sql = "SELECT users_roles.uid + 100 AS uid,
                   users_roles.rid,
                   CASE ".
                      /*WHEN (    CONCAT(maenna_people.firstname, 'MAEADM') LIKE '%" . $stringPart . "'
                            AND users_roles.rid IN (6, 10))
                      THEN
		      CONCAT(UPPER(maenna_people.firstname), 'MAEADM')*/
		   "
                      WHEN (    CONCAT(maenna_people.firstname, 'MAE') LIKE '%" . $stringPart . "'
                            AND users_roles.rid NOT IN (6, 10, 3))
                      THEN
                         CONCAT(UPPER(maenna_people.firstname), 'MAE')
                      WHEN (    CONCAT('ADMIN_',LEFT(users_extend.first_name,1),LEFT(users_extend.last_name,1)) LIKE '%" . $stringPart . "'
                            AND users_roles.rid IN (6, 10))
                      THEN
                         CONCAT('ADMIN_',LEFT(users_extend.first_name,1),LEFT(users_extend.last_name,1))
                   END
                      AS name
			  FROM    users
			  INNER JOIN maenna_mail_connections mmc
			  ON (mmc.uid = {$queryUid} AND mmc.target=users.uid)
			  LEFT JOIN
			  maenna_people 
			  ON (users.uid = maenna_people.pid)
			  LEFT JOIN
			  maenna_company
			  ON (maenna_company.companyid = users.uid)
			  LEFT JOIN
			  users_extend
			  ON (users.uid = users_extend.uid)
                   LEFT JOIN
                      users_roles
                   ON (users_roles.uid = users.uid)
             WHERE     users_roles.uid + 100 REGEXP '^" . $numberPart . "'
                   AND (   (    CONCAT('ADMIN_',LEFT(users_extend.first_name,1),LEFT(users_extend.last_name,1)) LIKE '%" . $stringPart . "'
                            AND users_roles.rid IN (6, 10))
                        OR (    CONCAT(maenna_people.firstname, 'MAE') LIKE '%" . $stringPart . "'
                            AND users_roles.rid NOT IN (6, 10, 3)))
             LIMIT {$limit}";
} else {
    $queryString = rtrim($queryString, '0');
    $sql = "SELECT users_roles.uid + 100 AS uid,
                   users_roles.rid,
                       CASE
                      WHEN (    CONCAT('ADMIN_',LEFT(users_extend.first_name,1),LEFT(users_extend.last_name,1)) LIKE '%" . $queryString . "%'
                            AND users_roles.rid IN (6, 10))
                      THEN
		      CONCAT('ADMIN_',LEFT(users_extend.first_name,1),LEFT(users_extend.last_name,1))
                      "./*    WHEN (    CONCAT(maenna_people.firstname, 'MAEADM') LIKE '%" . $queryString . "%'
                                AND users_roles.rid IN (6, 10))
                          THEN
			  CONCAT(UPPER(maenna_people.firstname), 'MAEADM')*/
			"
                          WHEN (    CONCAT(maenna_people.firstname, 'MAE') LIKE '%" . $queryString . "%'
                                AND users_roles.rid NOT IN (6, 10, 3))
                          THEN
                             CONCAT(UPPER(maenna_people.firstname), 'MAE')
			 WHEN (maenna_company.projname LIKE '%". $queryString . "%' AND users_roles.rid = 3)
			 THEN maenna_company.projname
                       END
                          AS name
			  FROM    users
			  INNER JOIN maenna_mail_connections mmc
			  ON (mmc.uid = {$queryUid} AND mmc.target=users.uid)
			  LEFT JOIN
			  maenna_people 
			  ON (users.uid = maenna_people.pid)
			  LEFT JOIN
			  maenna_company
			  ON (maenna_company.companyid = users.uid)
			  LEFT JOIN
			  users_extend
			  ON (users.uid = users_extend.uid)
                       LEFT JOIN
                          users_roles
                       ON (users_roles.uid = users.uid)
		       WHERE    users.status=1 and (
			       (    CONCAT('ADMIN_',LEFT(users_extend.first_name,1),LEFT(users_extend.last_name,1)) LIKE '%" . $queryString . "%'
                           AND users_roles.rid IN (6, 10))
                       OR (    CONCAT(maenna_people.firstname, 'MAE') LIKE '%" . $queryString . "%'
                           AND users_roles.rid NOT IN (6, 10, 3))
			OR ( maenna_company.projname LIKE '%". $queryString . "%' AND users_roles.rid = 3)
			)
                 LIMIT {$limit}";
}

updateConnectedUsers($conn, $roles, $queryUid);
$result = mysql_query($sql, $conn);

if (empty($result)) {
	echo '<tr>
		            <td bgcolor="#FFFFFF">No Connected Users</td>
			    </tr>';
} else {

	while ($row = mysql_fetch_array($result)) {
	    $firstname = strtoupper($row['name']);
	    if(in_array($row['rid'], array(6, 10))){
	       $uid = sprintf("%04s", $row['uid']);
	    } else {
		$uid = sprintf("%04s", $row['uid']);
	    }
	    if ($row['rid']==3) $output = $firstname;
	    else $output = $firstname . $uid;
	    echo '<tr>
		    <td bgcolor="#FFFFFF" uid="' . $row['uid'] . '" onclick="putNameInsideToField(this);">' . $output . '</td>
		  </tr>';
	}
}
echo '</table>';

/*

function intersect_split($left, $right, $greedy = true){
    for($i = 0, $m = strlen($left); $i <= $m; $i++){
        $chunk = substr($left, $i * (int) ($greedy ?: -1));
        if(substr($right, 0, strlen($chunk)) == $chunk){
            return array(
                (string) substr($left, 0, $m - strlen($chunk)),
                (string) substr($right, strlen($chunk)),
            );
        }
    }
    return array($left, $right);
}
*/

function updateConnectedUsers ($active_db, $roles, $uid) {

    foreach($roles  as $rid => $role ){
        if($role == 'authenticated user'){
            continue;
        }

        if($role == 'Company'){//- message between companies and their admin
            /*
            [14:21:57] Nick Cosic: select assignee_uid from maenna_connections where target_uid='{$company_id}'
            [14:21:57] Nick Cosic: related - to su svi assignee_uid koji su povezani na isti target_uid;
            [14:22:45] Nick Cosic: a da bi naso admina u ono gore moras da dodas and conntype='admin';
            [14:22:55] Nick Cosic: inace ti samo daje sve vrste veza
            [14:46:12] Nick Cosic: ukljuci samo u related i taj target_uid
            [14:46:44] Nick Cosic: znaci sve target_uidove koji su vezani za nekog usera i onda sve assignee_uid koji su vezani za te target_idove
            */
            $sql = "SELECT " . mysql_real_escape_string($uid, $active_db) . ", assignee_uid
                      FROM maenna_connections
                     WHERE target_uid='" . mysql_real_escape_string($uid, $active_db) . "'
                       AND conntype = 'admin'";
            mysql_query("INSERT IGNORE INTO maenna_mail_connections(uid, target) ".$sql, $active_db);
        }

        if($role == 'Super admin'){ //- message between superadmin and admins
		$sql = "SELECT 60, uid FROM users WHERE status=1";
            mysql_query("INSERT IGNORE INTO maenna_mail_connections(uid, target) ".$sql, $active_db);
        }

        if($role == 'Analyst' //message between professionals connected to a company (listed in team box) and that company's admin
        || $role == 'Investor'
        || $role == 'Other Expert'
        || $role == 'Executive'
        ){
            $sql = "SELECT " . mysql_real_escape_string($uid, $active_db) . ", mc.assignee_uid
                      FROM maenna_connections mc
                           INNER JOIN users_roles ur
                              ON (    mc.assignee_uid = ur.uid
                                  AND ur.rid = (SELECT role.rid
                                                  FROM role
                                                 WHERE role.name = 'Company'))
                           INNER JOIN maenna_connections mc2
                              ON (    mc.assignee_uid = mc2.target_uid
                                  AND mc2.conntype = 'admin'
                                  )
                     WHERE mc.target_uid = '" . mysql_real_escape_string($uid, $active_db) . "'";
            mysql_query("INSERT IGNORE INTO maenna_mail_connections(uid, target) ".$sql, $active_db);

            //message between related professionals
            $sql = "SELECT " . mysql_real_escape_string($uid, $active_db) . ",mc2.assignee_uid
                      FROM maenna_connections mc1, maenna_connections mc2
		      WHERE mc1.target_uid=mc2.target_uid 
		       AND mc1.assignee_uid = '" . mysql_real_escape_string($uid, $active_db) . "'";
            mysql_query("INSERT IGNORE INTO maenna_mail_connections(uid, target) ".$sql, $active_db);
        }

        /*
        * Now I have to do checks in the opposite direction, for example if super admins are allowed to send
        * messages to admins, then I should allow admins to reply and send messages to super admins too
        */

        if($role == 'Maennaco admin') {
            // message between company's admin and professionals connected to that company
            $sql = "SELECT " . mysql_real_escape_string($uid, $active_db) . ", mc2.assignee_uid
                      FROM maenna_connections mc
                           INNER JOIN users_roles ur
                              ON (    mc.target_uid = ur.uid
                                  AND ur.rid = (SELECT role.rid
                                                  FROM role
                                                 WHERE role.name = 'Company'))
                           INNER JOIN maenna_connections mc2
                              ON (    mc.target_uid = mc2.target_uid
                                  )
                     WHERE mc.assignee_uid = '" . mysql_real_escape_string($uid, $active_db) . "' AND mc.conntype = 'admin'";
            mysql_query("INSERT IGNORE INTO maenna_mail_connections(uid, target) ".$sql, $active_db);

            // message between admins and superadmin
            $sql = "SELECT " . mysql_real_escape_string($uid, $active_db) . ", 60
                                        FROM users_roles
                                       WHERE     rid = (SELECT role.rid
                                                          FROM role
                                                         WHERE role.name = 'Super admin')
                                             ";
            mysql_query("INSERT IGNORE INTO maenna_mail_connections(uid, target) ".$sql, $active_db);

            // company admin and his company
            $sql ="SELECT  " . mysql_real_escape_string($uid, $active_db) . ", target_uid 
                              FROM maenna_connections
                             WHERE assignee_uid ='" . mysql_real_escape_string($uid, $active_db) . "'";
            mysql_query("INSERT IGNORE INTO maenna_mail_connections(uid, target) ".$sql, $active_db);
        }




    }// end foreach
}
