<?php

namespace Clewed\Company;

use Clewed\Company\Service as CompanyService;
use Clewed\Db as ClewedDb;
use Clewed\Notifications\NotificationService;

/**
 * Class Controller
 * @package Clewed\Company
 */
class Controller
{
    /**
     * @var \Clewed\Db
     */
    public $db;

    /**
     * Controller constructor
     */
    public function __construct()
    {
        $this->db = ClewedDb::get_instance();
    }

    /**
     * Toggles service approval
     */
    public function toggleServiceApproval()
    {
        $id = (int) $_POST['id'];
        $uid = (int) $_POST['uid'];
        if (empty($id))
            return json_encode(array(
                'success' => false,
                'error' => array('message' => 'Invalid service id')
            ));

        $notificationService = new NotificationService();

        $companyService = new CompanyService();
        $companyService->toggleServiceApproval(array($id));

        $services = $companyService->getServices(array($id));
        $service = $services[$id];
        if (!empty($service) && $service['approved']) {

            $leadExpertId = $service['executor_id'];
            if (!empty($leadExpertId) && empty($service['executor_status']))
                $notificationService->registerEvent(
                    'company_service_invite_sent',
                    $id,
                    $uid,
                    array(
                        'invitedUserId' => (int) $leadExpertId,
                        'lead' => true
                    )
                );

            $options = array('authorId' => $service['postedby']);
            if (!empty($leadExpertId) && 'confirmed' == $service['executor_status'])
                $options['leadId'] = $leadExpertId;

            $projectOwnerId = $service['companyid'];
            if (!empty($projectOwnerId))
                $options['ownerId'] = $projectOwnerId;

            $notificationService->registerEvent(
                'company_service_approved',
                $id,
                $uid,
                $options
            );

            $invitedExpertIds = $companyService->getInvitedExpertIds($id);
            $colleagueIds = $companyService->getColleagueIds($service['companyid']);
            foreach ($invitedExpertIds as $invitedExpertId)
                $notificationService->registerEvent(
                    'company_service_invite_sent',
                    $id,
                    $uid,
                    array(
                        'invitedUserId' => (int) $invitedExpertId,
                        'lead' => false,
                        'colleague' => in_array($invitedExpertId, $colleagueIds)
                    )
                );
        }

        return json_encode(array(
            'success' => true,
            'data' => $services
        ));
    }

    /**
     * Confirms wire transfer for a service
     */
    public function confirmWireTransfer()
    {
        $id = (int) $_POST['id'];
        if (empty($id))
            return json_encode(array(
                'success' => false,
                'error' => array('message' => 'Invalid service id')
            ));

        $uid = (int) $_POST['uid'];
        $method = $_POST['method'];
        $companyService = new CompanyService();
        $methods = $companyService->getServicePaymentMethods();
        if (!in_array($method, $methods))
            return json_encode(array(
                'success' => false,
                'error' => array('message' => 'Invalid type')
            ));

        $success = $companyService->setServicePaymentMethod($id, $method);
        if ('wire' === $method && $success) {
            $notificationService = new NotificationService();
            $notificationService->registerEvent(
                "wire_transfer_confirmed",
                $id,
                $uid
            );
        }

        return json_encode(array(
            'success' => $success
        ));
    }

    /**
     * Confirms wire transfer reference for a service
     */
    public function confirmWireTransferReference()
    {
        $id = (int) $_POST['id'];
        if (empty($id))
            return json_encode(array(
                'success' => false,
                'error' => array('message' => 'Invalid service id')
            ));

        $uid = (int) $_POST['uid'];
        $reference = filter_var($_GET['ref'], FILTER_SANITIZE_STRING);
        $companyService = new CompanyService();
        $success = $companyService->setServiceWireTransferReference($id, $reference);
        if ($success) {
            $notificationService = new NotificationService();
            $notificationService->registerEvent(
                "wire_transfer_reference_confirmed",
                $id,
                $uid
            );
        }

        return json_encode(array(
            'success' => $success
        ));
    }

    /**
     * Marks a service complete
     */
    public function markServiceComplete()
    {
        $id = (int) $_POST['id'];
        if (empty($id))
            return json_encode(array(
                'success' => false,
                'error' => array('message' => 'Invalid service id')
            ));

        $uid = (int) $_POST['uid'];
        $companyService = new CompanyService();
        $success = $companyService->markServiceComplete($id);
        if ($success) {
            $notificationService = new NotificationService();
            $notificationService->registerEvent(
                "project_service_delivered",
                $id,
                $uid
            );

            $notificationService->registerEvent(
                "project_service_expert_review_requested",
                $id,
                $uid
            );
        }

        return json_encode(array(
            'success' => $success
        ));
    }

    /**
     * Marks a service started
     */
    public function markServiceStarted()
    {
        $id = (int) $_POST['id'];
        if (empty($id))
            return json_encode(array(
                'success' => false,
                'error' => array('message' => 'Invalid service id')
            ));

        $companyService = new CompanyService();
        $success = $companyService->markServiceStarted($id);
        return json_encode(array(
            'success' => $success
        ));
    }
}