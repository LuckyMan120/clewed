<?php
/**
 * Container of user business logic
 *
 * @author oleg bursacovschi <o.bursacovschi@gmail.com>
 */
namespace Clewed\User;

use Clewed\Db;
use Clewed\Company\Service as CompanyService;
use Clewed\Professional\Service as ProService;

class Service
{
    protected $db;

    /**
     * Constructor
     */
    public function __construct()
    {
        $this->db = Db::get_instance();
    }

    /**
     * Get user type by id
     *
     * @param $userId
     * @return string
     */
    public function getUserType($userId)
    {
        $row = $this->db->get_row("SELECT pid FROM maenna_people WHERE pid = :pid", array(':pid' => $userId));
        if (!empty($row))
            return 'people';

        $row = $this->db->get_row("SELECT companyid FROM maenna_company WHERE companyid = :cid", array(':cid' => $userId));
        if (!empty($row))
            return 'company';

        return 'admin';
    }

    /**
     * Retrieve user data
     *
     * @param $userIds
     * @return array
     */
    public function get($userIds)
    {
        if (empty($userIds))
            return array();

        $userIdPlaceholders = str_repeat('?, ', count($userIds) - 1) . '?';
        $rows = $this->db->get_array("
            SELECT
                u.*,
                mc.*,
                mp.*,
                ma.*,
                IF (mc.companyid IS NULL, 0, 1) AS is_company,
                IF (mp.pid IS NULL, 0, 1) AS is_professional
            FROM users u
            LEFT JOIN maenna_company mc ON mc.companyid = u.uid
            LEFT JOIN maenna_people mp ON mp.pid = u.uid
            LEFT JOIN maenna_about ma ON ma.project_id = u.uid
            WHERE u.uid IN ({$userIdPlaceholders})",
            $userIds
        );

        $users = array();
        $companies = array();
        $professionals = array();
        foreach ($rows as $row) {
            $userId = (int) $row['uid'];

            if ($row['is_professional']) {
                $row['full_name'] = ucfirst($row['firstname']) . ' ' . (1 == $row['username_type'] ? strtoupper(substr($row['lastname'], 0, 1)) : ucfirst($row['lastname']));
                $professionals[$userId] = $userId;
            } elseif ($row['is_company']) {
                $row['full_name'] = ucfirst($row['projname']) ?: 'Project ' . $row['pseudo_name'];
                $companies[$userId] = $userId;
            } else
                $row['is_admin'] = 1;

            $row['avatar'] = $this->buildAvatarPath($row);

            $users[$userId] = $row;
        }

        if (!empty($companies)) {
            $companyService = new CompanyService();
            $companies = $companyService->get(array_keys($companies));
            foreach ($companies as $companyId => $company)
                $users[$companyId] = array_merge($users[$companyId], $company);
        }

        if (!empty($professionals)) {
            $proService = new ProService();
            $professionals = $proService->get(array_keys($professionals));
            foreach ($professionals as $proId => $professional)
                $users[$proId] = array_merge($users[$proId], $professional);
        }

        return $users;
    }

    /**
     * Builds avatar path
     *
     * @param $user
     * @return string
     */
    protected function buildAvatarPath($user)
    {
        $url = '/themes/maennaco/images/cmp-avatar-product.png';
        if ($user['is_company']) {

            if (!empty($user['project']) && is_readable(ROOT . '/themes/maennaco/images/project/' . $user['project']))
                $url = '/themes/maennaco/images/project/' . urlencode($user['project']);

            elseif (is_readable(ROOT . '/sites/default/images/company/50x50/' . $user['uid'] . '.jpg'))
                $url = '/sites/default/images/company/50x50/' . $user['uid'] . '.jpg';

            elseif ('service' == $user['company_type'])
                $url = '/themes/maennaco/images/cmp-avatar-service.png';

        } elseif ($user['is_professional']) {

            $url = 'f' == $user['gender']
                ? '/themes/maennaco/images/prof-avatar-female.png'
                : '/themes/maennaco/images/prof-avatar-male.png';

            if (is_readable(ROOT . '/sites/default/images/profiles/50x50/' . $user['uid'] . '.jpg'))
                $url = '/sites/default/images/profiles/50x50/' . $user['uid'] . '.jpg';
        } elseif ($user['is_admin'] == 1) {
            $url = '/themes/maennaco/images/discussion_logo.png';
        } else {

            $url = '/themes/maennaco/images/prof-avatar-male.png';
        }

        return $url;
    }

    /**
     * @param $userId
     * @return bool
     */
    public function isCompany($userId)
    {
        return 'company' == $this->getUserType($userId);
    }

    /**
     * @param $userId
     * @return bool
     */
    public function isProfessional($userId)
    {
        return 'people' == $this->getUserType($userId);
    }

    /**
     * @param $userId
     * @param $companyId
     * @return bool
     */
    public function isColleague($userId, $companyId)
    {
        $connections = $this->getConnections($companyId);
        foreach ($connections['client'] ?: array() as $connection)
            if ($connection['assignee_uid'] == $userId)
                return true;

        return false;
    }

    /**
     * @param $userId
     * @param $companyId
     * @return bool
     */
    public function isAdmin($userId, $companyId)
    {
        $connections = $this->getConnections($companyId);
        foreach ($connections['admin'] ?: array() as $connection)
            if ($connection['assignee_uid'] == $userId)
                return true;

        return false;
    }

    /**
     * @param $userId
     * @return bool
     */
    public function isSuperAdmin($userId)
    {
        $found = $this->db->get_column("
            SELECT uid
            FROM users_roles
            WHERE uid = ?
            AND rid = ?
            LIMIT 1",
            array(
                $userId,
                10
            )
        );

        return !empty($found);
    }

    /**
     * @param $companyId
     * @return array
     */
    public function getConnections($companyId)
    {
        $connections = array(
            'admin' => array(),
            'follower' => array(),
            'follow' => array(),
            'advisor' => array(),
            'watchlist' => array(),
            'visible' => array(),
            'propose' => array(),
            'collaborator' => array(),
            'investor' => array(),
            'client' => array(),
        );

        if (empty($companyId))
            return $connections;

        $rows = $this->db->get_array("
            SELECT *, conntype as type
            FROM maenna_connections
            WHERE status = 'active' 
            AND (assignee_uid = ? OR target_uid = ?)
            ORDER BY conntype",
            array(
                $companyId,
                $companyId
            )
        );

        foreach ($rows as $row) {
            switch ($row['type']) {
                case 'follow':
                    $connections['follow' . ($companyId == $row['target_uid'] ? 'er' : '')][] = $row;
                    break;
                case 'watchlist':
                    $companyId != $row['target_uid'] || $connections['watchlist'][] = $row;
                    break;
                case 'visible':
                    $companyId != $row['target_uid'] || $connections['visible'][] = $row;
                    break;
                case 'collaborator':
                    $companyId != $row['target_uid'] || $connections['collaborator'][] = $row;
                    break;
                default:
                    $connections[$row['type']][] = $row;
            }
        }

        return $connections;
    }
}