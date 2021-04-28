<?php
/**
 * @author Dmytro Vovk <dmitry.vovk@gmail.com>
 */
require_once 'includes/bootstrap.inc';
require 'lib/init.php';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
ini_set('display_errors', true);
error_reporting(E_ALL);
global $user;

class PaymentChecker {

    protected $user;
    protected $data;
    protected $response = array(
        'error' => false,
    );

    public function __construct($user, array $data) {
        $this->user = $user;
        $this->data = $data;
    }

    public function validate() {
        if (!is_object($this->user)) {
            $this->response['error'] = true;
            $this->response['errors']['not-logged-in'] = true;
        }
        if (empty($this->data['eventId'])) {
            $this->response['error'] = true;
            $this->response['errors']['empty-event-id'] = true;
        } else {
            $this->response['event-id'] = (int) $this->data['eventId'];
        }
        $this->get_event();
        $this->response['price'] = 'full';
        if (!empty($this->data['discountCode'])) {
            $p = new \Clewed\Insights\DiscountModel(\Clewed\Db::get_instance());
            $discount = $p->getInsightDiscount($this->response['event-id']);
            if ($discount->approved) {
                if (strtolower($discount->code) === strtolower($this->data['discountCode'])) {
                    $this->response['price'] = 'discount';
                    $this->response['rate'] = (int) $discount->rate;
                    if ($this->response['rate'] === 100) {
                        $this->response['cost'] = 0;
                    } else {
                        $this->response['cost'] = number_format($this->response['cost'] - $this->response['cost'] / 100 * $discount->rate);
                    }
                } else {
                    $this->response['error'] = true;
                    $this->response['errors']['invalid-code'] = true;
                }
            }
        }
        else $this->response['rate'] = 0;
    }

    protected function get_event() {

        $db = \Clewed\Db::get_instance();
        $eventCost = $db->get_column(
            'SELECT `cost` FROM `maenna_professional` WHERE `id` = :id LIMIT 1',
            array(':id' => $this->response['event-id'])
        );
        $this->response['cost'] = $this->response['full-cost'] = (int) str_replace(',', '', $eventCost);
    }

    /**
     * @return array
     */
    public function getResult() {
        if (!$this->response['error']) {
            $this->response['form'] = $this->createForm();
        }
        return $this->response;
    }

    /**
     * @return string
     */
    public function createForm() {
        global $user;
        $sql = 'SELECT `users`.`mail`, `firstname`, `lastname` FROM `users`, `maenna_people` WHERE `users`.`uid` = `maenna_people`.`pid` AND `users`.`uid` = :uid LIMIT 1';
        list($email, $firstname, $lastname) = \Clewed\Db::get_instance()->get_row($sql, array(':uid' => $user->uid));

        $custom = base64_encode(
            serialize(
                array(
                    'user_id' => $user->uid,
                    'pro_id'  => $this->response['event-id'],
                    'rate'    => $this->response['rate']
                )
            )
        );
        $itemName = 'Insight';
        $itemNumber = $this->response['event-id'];
        $amount = $this->response['full-cost'];
        $rate = $this->response['rate'];
        if ($this->response['price'] === 'discount') {
            $discountAmount = $this->response['cost'];
        }
        $returnUrl = sprintf(
            '/account?tab=professionals&page=pro_detail&id=%d&section=pro_industry_view&type=details&pro_id=%d',
            $this->data['id'],
            $itemNumber
        );
        ob_start();
        require ROOT . '/templates/payment-form.php';
        return ob_get_clean();
    }
}

$pc = new PaymentChecker($user, $_POST);
$pc->validate();
header('Content-type: text/json');
echo json_encode($pc->getResult());
