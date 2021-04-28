<?php
namespace Clewed;

use Clewed\Insights\InsightRepository;
use Clewed\Insights\InsightService;
use Clewed\Notifications\NotificationService;
use Clewed\Insights\InsightEntity;

/**
 * @author Dmytro Vovk <dmitry.vovk@gmail.com>
 */
class PaypalIpnProcessor {

    /** @var Db */
    protected $db;
    protected $data = array();
    protected $custom = array();

    protected $validStatus = array('Completed', 'Pending');

    public function __construct() {
        $this->db = Db::get_instance();
        if (ENVIRONMENT === PROD) {
            $this->validStatus = array('Completed');
        }
    }

    /**
     * @param array $data
     *
     * @return bool|int
     */
    public function process(array $data) {
        $this->data = $data;
        if ($this->valid()) {
            if ($this->receiverValid()) {
                if (!in_array($this->data['payment_status'], $this->validStatus, true)) {
                    error_log(sprintf('Payment status was "%s"', $this->data['payment_status']));
                    return true;
                }
                if ($this->hasTransaction()) {
                    error_log('Duplicate transaction ' . $this->data['txn_id']);
                    return false;
                }
                $this->extractCustom();

                $insight = $this->attendToInsight();
                $result = $this->createPaymentRecord($insight);
                return $result;
            } else {
                error_log(
                    sprintf(
                        'Receiver not valid. Expected "%s" got "%s"',
                        PAYPAL_ACCOUNT,
                        $this->data['receiver_email']
                    )
                );
            }
        } else {
            error_log('Did not validate IPN');
            error_log(print_r($this->data, true));
        }
        return false;
    }

    protected function extractCustom() {
        if (!empty($this->data['custom'])) {
            $this->custom = unserialize(base64_decode($this->data['custom']));
        }
    }

    /**
     * @return bool
     */
    protected function valid() {
        header($_SERVER['SERVER_PROTOCOL'] . ' 200 OK');
        $url = PAYPAL_URL . '?cmd=_notify-validate';
        foreach ($_POST as $key => $value) {
            $url .= sprintf('&%s=%s', $key, urlencode(stripslashes($value)));
        }
        $c = curl_init($url);
        curl_setopt_array(
            $c,
            array(
                CURLOPT_HEADER            => false,
                CURLOPT_RETURNTRANSFER    => true,
                CURLOPT_FOLLOWLOCATION    => true,
                CURLOPT_MAXREDIRS         => 5,
                CURLOPT_TIMEOUT           => 30,
                CURLOPT_CONNECTTIMEOUT    => 10,
                CURLOPT_DNS_CACHE_TIMEOUT => 86400,
                CURLOPT_SSL_VERIFYPEER    => false,
            )
        );
        $response = curl_exec($c);
        $httpCode = curl_getinfo($c, CURLINFO_HTTP_CODE);
        return $httpCode == 200 && trim($response) === 'VERIFIED';
    }

    protected function receiverValid() {
        return $this->data['receiver_email'] === PAYPAL_ACCOUNT;
    }

    protected function hasTransaction() {
        return 0 != $this->db->get_column(
                        'SELECT COUNT(*) FROM `maenna_professional_payments` WHERE `transaction_id` = :tx_id LIMIT 1',
                        array(':tx_id' => $this->data['txn_id'])
        );
    }

    /**
     * Attending to insight
     * When attending to private one insight should be duplicated
     * @see InsightService::attend()
     * @return Insights\InsightEntity|null
     */
    protected function attendToInsight()
    {
        $insightRepository = new InsightRepository();
        $insightService = new InsightService();

        $insight = $insightRepository->findById($this->custom['pro_id']);
        if ($insight) {
            $insight = $insightService->attend($this->custom['user_id'], $insight, false);
        }
        return $insight;
    }

    /**
     * @param InsightEntity|null $insight
     * @return bool|int
     */
    protected function createPaymentRecord($insight = null) {
        $timestamp = time();
        $paymentId = $this->db->run(
                        'INSERT INTO `maenna_professional_payments` (
                            `user_id`, `pro_id`, `transaction_id`, `amount`,`discount_rate`, `status`, `date_created`
                        ) VALUES (
                            :user_id, :pro_id, :tx_id, :amount, :rate, 1, :date_created
                        )',
                        array(
                            ':user_id' => $this->custom['user_id'],
                            ':pro_id'  => $insight ? $insight->id : $this->custom['pro_id'],
                            ':tx_id'   => $this->data['txn_id'],
                            ':amount'  => $this->data['mc_gross'],
                            ':rate'  => $this->custom['rate'],
                            ':date_created'  => $timestamp
                        )
        );

        if(!empty($paymentId)) {
            $insightId = $insight ? $insight->id : (int) $this->custom['pro_id'];
            $userId = (int) $this->custom['user_id'];
            if(!empty($insightId) && !empty($userId)) {
                $offerRepository = new InsightRepository();
                $offer = $offerRepository->findById($insightId);
                if(!empty($offer) && $offer->type == InsightEntity::TYPE_GROUP_INSIGHT) {
                    $notificationService = new NotificationService();
                    $notificationService->registerEvent(
                        'insight_review_requested',
                        $insightId,
                        $userId
                    );
                }
            }
        }

        return $paymentId;
    }
}
