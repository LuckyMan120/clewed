<?php
/**
 * Container of companies business logic
 *
 * @author oleg bursacovschi <o.bursacovschi@gmail.com>
 */
namespace Clewed\Company;

use Clewed\Db;
use Clewed\Notifications\NotificationService;
use Clewed\User\Service as UserService;

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
     * Checks if a company is approved
     *
     * @param $companyId
     * @return array
     */
    public function isCompanyApproved($companyId)
    {
        return $this->areCompaniesApproved(array($companyId));
    }

    /**
     * Checks if companies are approved
     *
     * @param $companyIds
     * @return array
     */
    public function areCompaniesApproved($companyIds)
    {

        if (empty($companyIds))
            return array();

        $companyIdsPlaceholder = rtrim(str_repeat('?, ', count($companyIds)), ', ');
        $rows = $this->db->get_array("
            SELECT companyid as id, active
            FROM maenna_company
            WHERE companyid IN($companyIdsPlaceholder)",
            $companyIds
        );

        $companies = array();
        foreach ($rows as $row)
            $companies[$row['id']] = !!$row['active'];

        return $companies;
    }

    /**
     * Toggles a company's featured state
     *
     * @param $companyId
     * @return bool
     */
    public function toggleCompanyFeaturedState($companyId)
    {
        return $this->toggleCompaniesFeaturedState(array($companyId));
    }

    /**
     * Toggles companies featured state
     *
     * @param $companyIds
     * @return bool
     */
    public function toggleCompaniesFeaturedState($companyIds)
    {

        if (empty($companyIds))
            return false;

        $companyIdsPlaceholder = rtrim(str_repeat('?, ', count($companyIds)), ', ');
        $this->db->run("
            UPDATE maenna_company
            SET featured = 1 - featured
            WHERE companyid IN($companyIdsPlaceholder)",
            $companyIds
        );

        return true;
    }

    /**
     * Toggles companies shareable state
     *
     * @param $companyIds
     * @return bool
     */
    public function toggleCompaniesShareableState($companyIds)
    {

        if (empty($companyIds))
            return false;

        $companyIdsPlaceholder = rtrim(str_repeat('?, ', count($companyIds)), ', ');
        $this->db->run("
            UPDATE maenna_company
            SET shareable = 1 - shareable
            WHERE companyid IN($companyIdsPlaceholder)",
            $companyIds
        );

        return true;
    }

    public function toggleCompaniesOpenState($companyIds){
        if (empty($companyIds))
            return false;
        $companyIdsPlaceholder = rtrim(str_repeat('?, ', count($companyIds)), ', ');
        $this->db->run("
            UPDATE maenna_company
            SET stateable = CASE
                WHEN stateable = 1 THEN 0
                ELSE 1
            END 
            WHERE companyid IN($companyIdsPlaceholder)",
            $companyIds
        );
        return true;
    }
    public function toggleCompaniesComingState($companyIds){
        if (empty($companyIds))
            return false;

        $companyIdsPlaceholder = rtrim(str_repeat('?, ', count($companyIds)), ', ');
        $this->db->run("
            UPDATE maenna_company
            SET stateable = CASE
                WHEN stateable = 2 THEN 0
                ELSE 2
            END 
            WHERE companyid IN($companyIdsPlaceholder)",
            $companyIds
        );

        return true;
    }
    public function toggleCompaniesPastState($companyIds){
        if (empty($companyIds))
            return false;

        $companyIdsPlaceholder = rtrim(str_repeat('?, ', count($companyIds)), ', ');
        $this->db->run("
            UPDATE maenna_company
            SET stateable = CASE
                WHEN stateable = 3 THEN 0
                ELSE 3
            END 
            WHERE companyid IN($companyIdsPlaceholder)",
            $companyIds
        );

        return true;
    }

    /**
     * Returns current count of featured companies
     */
    public function getFeaturedCount()
    {

        $row = $this->db->get_row("
            SELECT COUNT(*) as count
            FROM maenna_company
            WHERE active = 1
            AND featured = 1
        ");

        return $row['count'];
    }

    /**
     * Fetch featured companies
     *
     * @param $limit
     * @return array
     */
    public function getFeatured($limit = 8)
    {

        $rows = $this->db->get_array("
            SELECT
                mc.companyid as id,
                mc.*,
                ma.*
            FROM maenna_company mc
            LEFT JOIN maenna_about ma ON ma.project_id = mc.companyid
            WHERE mc.featured = 1
            AND mc.active = 1
            LIMIT {$limit}"
        );

        $companies = array();
        foreach ($rows as $row) {
            $id = (int)$row['id'];
            $companies[$id] = $row;
        }

        // fetch extra data
        $companyIds = array_keys($companies);
        if (!empty($companyIds)) {

            $companyIdsPlaceholder = rtrim(str_repeat('?, ', count($companyIds)), ', ');
            $rows = $this->db->get_array("
                SELECT mcd.companyid AS id, mcd.*
                FROM maenna_company_data mcd
                WHERE mcd.companyid IN($companyIdsPlaceholder)",
                $companyIds
            );

            $extras = array();
            foreach ($rows as $row) {
                $companyId = (int)$row['id'];
                $key = $row['data_type'];
                $attr = $row['data_attr'];

                unset($row['data_value_save']);
                $values = $this->buildValues($row, 'data_value');
                if (empty($attr))
                    $extras[$companyId][$key] = $values;
                else
                    $extras[$companyId][$key][$attr] = reset($values);
            }

            foreach ($companies as $companyId => $company)
                foreach ($extras[$companyId] as $key => $values)
                    $companies[$companyId][$key] = $values;
        }

        return $companies;
    }

    /**
     * Builds a string from numbered columns
     * @param $data
     * @param $needle
     * @return array
     */
    protected function buildValues($data, $needle)
    {
        $values = array();
        foreach ($data as $key => $value)
            if (false !== strpos($key, $needle) && !empty($value))
                $values[] = $value;
        return $values;
    }

    /**
     * Fetch companies
     *
     * @param $companyIds
     * @return array
     */
    public function get($companyIds)
    {
        if (empty($companyIds))
            return array();

        $companyIdsPlaceholder = rtrim(str_repeat('?, ', count($companyIds)), ', ');
        $rows = $this->db->get_array("
            SELECT
                mc.companyid as id,
                mc.*,
                ma.*
            FROM maenna_company mc
            LEFT JOIN maenna_about ma ON ma.project_id = mc.companyid
            WHERE mc.companyid IN($companyIdsPlaceholder)",
            $companyIds
        );

        $companies = array();
        foreach ($rows as $row) {
            $id = (int)$row['id'];
            $companies[$id] = $row;
        }

        $rows = $this->db->get_array("
            SELECT mcd.companyid AS id, mcd.*
            FROM maenna_company_data mcd
            WHERE mcd.companyid IN($companyIdsPlaceholder)",
            $companyIds
        );

        $extras = array();
        foreach ($rows as $row) {
            $companyId = (int)$row['id'];
            $key = $row['data_type'];
            $attr = $row['data_attr'];

            unset($row['data_value_save']);
            $values = $this->buildValues($row, 'data_value');
            if (empty($attr))
                $extras[$companyId][$key] = $values;
            else
                $extras[$companyId][$key][$attr] = reset($values);
        }

        foreach ($companies as $companyId => $company)
            foreach ($extras[$companyId] as $key => $values)
                $companies[$companyId][$key] = $values;

        return $companies;
    }

    /**
     * @param $serviceIds
     * @return array
     */
    public function getServices($serviceIds)
    {
        if (empty($serviceIds))
            return array();

        $serviceIdsPlaceholder = rtrim(str_repeat('?, ', count($serviceIds)), ', ');
        $rows = $this->db->get_array("
            SELECT *, eventid as id
            FROM maenna_company_events
            WHERE eventid IN({$serviceIdsPlaceholder})
            ORDER BY eventid DESC",
            $serviceIds
        );

        $services =
        $companyIds = array();
        foreach ($rows as $row) {
            $companyId = (int)$row['companyid'];
            $companyIds[$companyId] = $companyId;
            $serviceId = (int)$row['id'];
            $services[$serviceId] = $row;
            $services[$serviceId]['createDate'] = date('m/d/y', $row['created']);
            $services[$serviceId]['dueDate'] = date('Y', $row['datetime']) > 2000 ? date('m/d/y', $row['datetime']) : '';
            $services[$serviceId]['isCompleted'] = $isCompleted = !empty($row['delivery_date']) && (time() - strtotime($row['delivery_date']) > 3 * 24 * 60 * 60);
            $services[$serviceId]['isFunded'] = $isFunded = !empty($row['payment_id']);
            $services[$serviceId]['isApprovedByClient'] = $isApprovedByClient = 'wire' == $row['payment_method'];
            $services[$serviceId]['isDelivered'] = $isDelivered = !empty($row['delivery_date']);
            $services[$serviceId]['isApproved'] = $isApproved = $row['approved'];
        }

        if (!empty($companyIds)) {
            $userService = new UserService();
            $users = $userService->get(array_keys($companyIds));
            foreach ($services as $serviceId => $service) {
                $companyId = (int)$service['companyid'];
                if (!empty($users[$companyId]))
                    $services[$serviceId]['company'] = $users[$companyId];
            }
        }

        return $services;
    }

    /**
     * @param $expertId
     * @return array
     */
    public function getServicesByConfirmedLeadId($expertId)
    {
        if (empty($expertId))
            return array();

        $rows = $this->db->get_array("
            SELECT *
            FROM maenna_company_events
            WHERE executor_id = ?
            AND executor_status = 'confirmed'
            AND approved = 1
            ORDER BY eventid DESC",
            array($expertId)
        );

        if (empty($rows))
            return array();

        $serviceIds = array();
        foreach ($rows as $row)
            $serviceIds[$row['eventid']] = (int)$row['eventid'];

        return $this->getServices(array_keys($serviceIds));
    }

    /**
     * @param $serviceIds
     * @return bool|int
     */
    public function toggleServiceApproval($serviceIds)
    {
        if (empty($serviceIds))
            return false;

        $serviceIdsPlaceholder = rtrim(str_repeat('?, ', count($serviceIds)), ', ');
        return $this->db->run("
            UPDATE maenna_company_events
            SET approved = NOT(approved)
            WHERE eventid IN({$serviceIdsPlaceholder})",
            $serviceIds
        );
    }

    /**
     * @param $companyId
     * @return array
     */
    public function getColleagueIds($companyId)
    {
        if (empty($companyId))
            return array();

        $rows = $this->db->get_array("
            SELECT assignee_uid
            FROM maenna_connections
            WHERE target_uid = ?
            AND conntype = 'client'
            AND status = 'active'",
            array($companyId)
        );

        $colleagueIds = array();
        foreach ($rows as $row)
            $colleagueIds[$row['assignee_uid']] = (int)$row['assignee_uid'];

        return array_values($colleagueIds);
    }

    /**
     * @param $serviceId
     * @return array
     */
    public function getInvitedExpertIds($serviceId)
    {
        if (empty($serviceId))
            return array();

        $rows = $this->db->get_array("
            SELECT uid
            FROM maenna_company_events_inv
            WHERE eventid = ?
            AND status='sent'",
            array($serviceId)
        );

        $expertIds = array();
        foreach ($rows as $row)
            $expertIds[$row['uid']] = (int)$row['uid'];

        return array_values($expertIds);
    }

    /**
     * @param $serviceId
     * @return array
     */
    public function getConfirmedInvitedExpertIds($serviceId)
    {
        if (empty($serviceId))
            return array();

        $rows = $this->db->get_array("
            SELECT uid
            FROM maenna_company_events_inv
            WHERE eventid = ?
            AND status='confirmed'",
            array($serviceId)
        );

        $expertIds = array();
        foreach ($rows as $row)
            $expertIds[$row['uid']] = (int)$row['uid'];

        return array_values($expertIds);
    }

    /**
     * @param $serviceId
     * @return array
     */
    public function getExpertIds($serviceId)
    {
        if (empty($serviceId))
            return array();

        $rows = $this->db->get_array("
            SELECT uid
            FROM maenna_company_events_inv
            WHERE eventid = ?",
            array($serviceId)
        );

        $expertIds = array();
        foreach ($rows as $row)
            $expertIds[$row['uid']] = (int)$row['uid'];

        return array_values($expertIds);
    }

    /**
     * @param $serviceId
     * @return null|int
     */
    public function getServiceLeadExpertId($serviceId)
    {
        $services = $this->getServices(array($serviceId));
        $service = $services[$serviceId];
        if (empty($service))
            return null;

        return $service['executor_id'];
    }

    /**
     * @param $serviceId
     * @return bool
     */
    public function isServiceLeadExpertStatusConfirmed($serviceId)
    {
        $services = $this->getServices(array($serviceId));
        $service = $services[$serviceId];
        if (empty($service))
            return false;

        return 'confirmed' === $service['executor_status'];
    }

    /**
     * @param $txnId
     * @return mixed
     */
    public function getServicePayment($txnId)
    {
        $txnId = filter_var($txnId, FILTER_SANITIZE_STRING);
        return $this->db->get_row("
            SELECT *
            FROM maenna_service_payments
            WHERE txn_id = ?",
            array($txnId)
        );
    }

    /**
     * @param $payment
     * @return bool
     */
    public function processServicePayment($payment)
    {
        $txnId = filter_var($payment['txn_id'], FILTER_SANITIZE_STRING);
//        $existingPayment = $this->getServicePayment($txnId);
//        if (!empty($existingPayment))
//            return false;

        $this->db->run("
            INSERT IGNORE INTO maenna_service_payments (txn_id, user_id, service_id, amount, status, datetime)
            VALUES (?, ?, ?, ?, ?, ?)",
            array(
                $txnId,
                $payment['user_id'],
                $payment['service_id'],
                $payment['amount'],
                1,
                $payment['datetime']
            )
        );

        $notificationService = new NotificationService();
        $notificationService->registerEvent(
            'company_service_purchased',
            $payment['service_id'],
            $payment['user_id']
        );

        $fees = $this->getServicePaymentMethodFees();
        if (empty($payment['method']))
            $payment['method'] = 'wire';

        return $this->db->run("
            UPDATE maenna_company_events
            SET
                payment_id = ?,
                payment_amount = ?,
                payment_date = ?,
                payment_method = ?,
                processing_fee = ?
            WHERE eventid = ?",
            array(
                $txnId,
                $payment['amount'],
                $payment['datetime'],
                $payment['method'],
                $fees[$payment['method']],
                $payment['service_id'],
            )
        );
    }

    /**
     * Retrieve expert ids that are invited to participate into a discussion
     *
     * @param $discussionId
     * @return array
     */
    public function getDiscussionInvitedExpertIds($discussionId)
    {
        $rows = $this->db->get_array("
            SELECT user_id
            FROM discussion_invites
            WHERE discussion_id = ?",
            array($discussionId)
        );

        $expertIds = array();
        foreach ($rows ?: array() as $row)
            $expertIds[$expertId] = $expertId = (int)$row['user_id'];

        return array_keys($expertIds);
    }

    /**
     * Replaces expert ids that are invited to participate into a discussion
     *
     * @param $discussionId
     * @param $expertIds
     * @param $options
     * @return bool
     */
    public function replaceDiscussionInvitedExpertIds($discussionId, $expertIds, $options = array())
    {
        if (empty($discussionId))
            return false;

        $discussions = $this->getDiscussions(array($discussionId));
        $discussion = $discussions[$discussionId];
        if (empty($discussion))
            return false;

        $currentIds = $this->getDiscussionInvitedExpertIds($discussionId);
        $toRemove = array_diff($currentIds, $expertIds);
        if (!empty($toRemove)) {
            $placeholder = str_repeat('?, ', count($toRemove) - 1) . '?';
            $params = array_merge(array($discussionId), $toRemove);
            $this->db->run("
                DELETE FROM discussion_invites
                WHERE discussion_id = ?
                AND user_id IN ($placeholder)",
                $params
            );
        }

        $toInsert = array_diff($expertIds, $currentIds);
        if (!empty($toInsert)) {

            $params =
            $values = array();
            foreach ($toInsert as $expertId) {
                $params[] = $discussionId;
                $params[] = $expertId;
                $values[] = '(?, ?)';
            }

            $this->db->run("
                INSERT INTO discussion_invites (discussion_id, user_id)
                VALUES " . implode(',', $values) . ";",
                $params
            );

            $notificationService = new NotificationService();
            $options = array_merge(array(
                'companyId' => $discussion['companyid'],
                'expertIds' => $toInsert
            ), $options);

            $notificationService->registerEvent(
                'project_discussion_experts_invited',
                $discussionId,
                0,
                $options
            );
        }

        return true;
    }

    /**
     * Retrieve discussions
     *
     * @param $discussionIds
     * @return array
     */
    public function getDiscussions($discussionIds)
    {
        if (empty($discussionIds))
            return array();

        $placeholder = str_repeat('?, ', count($discussionIds) - 1) . '?';
        $rows = $this->db->get_array("
            SELECT *
            FROM maenna_company_data
            WHERE dataid IN($placeholder)",
            $discussionIds
        );

        $discussions = array();
        foreach ($rows as $row)
            $discussions[$row['dataid']] = $row;

        return $discussions;
    }

    /**
     * Retrieve company team sizes
     *
     * @param $companyIds
     * @return array indexed by company id
     */
    public function getTeamSize($companyIds)
    {
        if (empty($companyIds))
            return array();

        $placeholder = str_repeat('?, ', count($companyIds) - 1) . '?';
        $rows = $this->db->get_array("
            SELECT
                target_uid,
                COUNT(DISTINCT(assignee_uid)) as size
            FROM maenna_connections
            WHERE target_uid IN($placeholder)
            AND status = 'active'
            AND conntype IN('advisor', 'client', 'collaborator','visible')
            GROUP BY target_uid",
            $companyIds
        );

        $teamSizes = array();
        foreach ($rows as $row)
            $teamSizes[$row['target_uid']] = $row['size'];

        return $teamSizes;
    }

    /**
     * Get all service payment methods
     *
     * @return array
     */
    public function getServicePaymentMethods()
    {
        return array(
            'paypal',
            'wire'
        );
    }

    /**
     * Get all fees related to payment methods
     *
     * @return array
     */
    public function getServicePaymentMethodFees()
    {
        return array(
            'paypal' => 3,
            'wire' => 1
        );
    }

    /**
     * Sets a payment method for a service
     *
     * @param $serviceId
     * @param $method
     * @return bool|int
     */
    public function setServicePaymentMethod($serviceId, $method)
    {
        $fees = $this->getServicePaymentMethodFees();
        return $this->db->run("
            UPDATE maenna_company_events
            SET
                payment_method = ?,
                processing_fee = ?
            WHERE eventid = ?",
            array(
                $method,
                $fees[$method],
                $serviceId
            )
        );
    }

    /**
     * Updates wire reference
     *
     * @param $serviceId
     * @param $reference
     * @return bool|int
     */
    public function setServiceWireTransferReference($serviceId, $reference)
    {
        return $this->db->run("
            UPDATE maenna_company_events
            SET wire_reference = ?
            WHERE eventid = ?",
            array(
                $reference,
                $serviceId
            )
        );
    }

    /**
     * Mark service
     *
     * @param $serviceId
     * @return bool|int
     */
    public function markServiceComplete($serviceId)
    {
        return $this->db->run("
            UPDATE maenna_company_events
            SET delivery_date = ?
            WHERE eventid = ?",
            array(
                date('Y-m-d'),
                $serviceId
            )
        );
    }

    /**
     * Mark service started
     *
     * @param $serviceId
     * @return bool|int
     */
    public function markServiceStarted($serviceId)
    {
        return $this->db->run("
            UPDATE maenna_company_events
            SET start_date = ?
            WHERE eventid = ?",
            array(
                date('c'),
                $serviceId
            )
        );
    }

    /**
     * Retrieve company permissions
     *
     * @param $companyId
     * @return array
     */
    public function getPermissions($companyId)
    {
        $companies = $this->get(array($companyId));
        $company = $companies[$companyId];
        if (empty($company))
            return array();

        $membership = 'cu_' . $company['membership'];
        $access = new \Maenna_access();
        $permissions = $access->load_permission($membership);
        if (empty($permissions))
            return array();

        $rows = $this->db->get_array("
            SELECT
                ms.sectionid as id,
                ms.maenna_name as name,
                msp.maenna_name as parent
            FROM maenna_sections ms
            LEFT JOIN maenna_sections msp ON msp.sectionid = ms.parentid
        ");

        foreach ($rows as $row) {
            $id = $row['id'];
            $name = $row['name'];
            $parent = $row['parent'];

            if (!empty($parent) && empty($permissions[$parent]))
                $permissions[$parent] = 'hide';

            if (!empty($permissions[$id])) {
                $permissions[$name] = $permissions[$id];
                unset($permissions[$id]);

                if (!empty($parent) && 'hide' != $permissions[$name])
                    $permissions[$parent] = 'read';
            }
        }

        return $permissions;
    }

    /**
     * @param $serviceId
     * @return array
     */
    public function getServiceMilestones($serviceId)
    {
        return $this->db->get_array("
            SELECT *
            FROM project_service_milestones
            WHERE service_id = ? ORDER BY duration ASC",
            array(
                (int)$serviceId
            )
        );
    }

    /**
     * @param $serviceId
     * @param $milestones
     */
    public function replaceServiceMilestones($serviceId, $milestones = array())
    {
        $milestoneIds = array();
        array_walk($milestones, function ($milestone) use (&$milestoneIds) {
            $milestoneId = (int)$milestone['id'];
            if (!empty($milestoneId))
                $milestoneIds[$milestoneId] = $milestoneId;
        });

        $currentMilestones = $this->getServiceMilestones($serviceId);
        $currentMilestoneIds = array();
        array_walk($currentMilestones, function ($milestone) use (&$currentMilestoneIds) {
            $currentMilestoneIds[$milestoneId] = $milestoneId = (int)$milestone['id'];
        });

        foreach ($milestones as $milestone)
            $this->db->run("
                INSERT INTO project_service_milestones (id, service_id, due_date, description)
                VALUES (?, ?, ?, ?)
                ON DUPLICATE KEY UPDATE due_date = ?, description = ?",
                array(
                    (int)$milestone['id'],
                    (int)$serviceId,
                    date('c', $milestone['due_date']),
                    str_replace("\n", '<br />', $milestone['description']),
                    date('c', $milestone['due_date']),
                    str_replace("\n", '<br />', $milestone['description'])
                )
            );

        $milestoneIdsToRemove = array_values(array_diff_key($currentMilestoneIds, $milestoneIds));
        if (!empty($milestoneIdsToRemove)) {
            $milestoneIdsPlaceholder = rtrim(str_repeat('?, ', count($milestoneIdsToRemove)), ', ');
            $this->db->run("
                DELETE FROM project_service_milestones
                WHERE id IN($milestoneIdsPlaceholder)",
                $milestoneIdsToRemove
            );
        }
    }

    /**
     * @param $serviceId
     * @param array $milestones
     */
    public function createServiceMilestones($serviceId, $milestones = array()) {
        foreach ($milestones as $milestone) {
            if (empty($milestone['duration']) || empty($milestone['description'])) {
                continue;
            }
            $query = "INSERT INTO project_service_milestones (service_id, description, duration)
                VALUES (?, ?, ?)";
            $params = array(
                (int)$serviceId,
                str_replace("\n", '<br />', $milestone['description']),
                (int)$milestone['duration']
            );
            $this->db->run($query, $params);
        }
    }

    /**
     * @param $serviceId
     * @param array $milestones
     */
    public function updateServiceMilestones($serviceId, $milestones = array()) {
        foreach ($milestones as $milestone) {
            if (!array_key_exists('id', $milestone)) {
                // create new milestone
                $this->createServiceMilestones($serviceId, array($milestone));
                continue;
            }
            if (empty($milestone['duration']) || empty($milestone['description'])) {
                $query = "DELETE FROM `project_service_milestones` WHERE `id` = " . $milestone['id'] . ";";
                $this->db->run($query);
                continue;
            }
            $query = "UPDATE project_service_milestones
                SET description = ?, duration = ?
                WHERE service_id = ? AND id = ?";
            $params = array(
                str_replace("\n", '<br />', $milestone['description']),
                (int)$milestone['duration'],
                (int)$serviceId,
                (int)$milestone['id']
            );
            $this->db->run($query, $params);
        }
    }

    public function getConnectors($companyId)
    {

        $connectors = array();

        $rows = $this->db->get_array("
            SELECT
                mc.assignee_uid as id
            FROM maenna_connections mc
            WHERE mc.status = 'active'
            AND mc.conntype = 'visible'
            AND target_uid = $companyId"
        );
        foreach ($rows as $row) {
            $connectors[] = $row['id'];
        }
        return $connectors;
    }

    public function getInterested($companyId)
    {

        $interested = array();

        $rows = $this->db->get_array("
            SELECT
                mc.assignee_uid as id
            FROM maenna_connections mc
            WHERE mc.status = 'active'
            AND mc.conntype = 'collaborator'
            AND target_uid = $companyId"
        );
        foreach ($rows as $row) {
            $interested[] = $row['id'];
        }
        return $interested;
    }
}