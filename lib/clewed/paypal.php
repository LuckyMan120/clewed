<?php
namespace Clewed;

use Clewed\Insights\InsightEntity;
use Clewed\Insights\InsightRepository;
use Clewed\Notifications\NotificationService;
use PayPal\IPN\PPIPNMessage;
use PayPal\Rest\ApiContext;
use PayPal\Auth\OAuthTokenCredential;
use PayPal\Api\Payer;
use PayPal\Api\Item;
use PayPal\Api\ItemList;
use PayPal\Api\Details;
use PayPal\Api\Amount;
use PayPal\Api\Transaction;
use PayPal\Api\RedirectUrls;
use PayPal\Api\Payment;
use PayPal\Exception\PPConnectionException;

/**
 * @author Dmytro Vovk <dmitry.vovk@gmail.com>
 */
class Paypal {

    const PAYPAL_CLIENT_ID = 'AfjfuRD8DYfIsLjdyzB2kWAQzK-FoODCNytU0TVbcttA_d3fxN1DTmHEvqrA';
    const PAYPAL_SECRET = 'EGYMvBCuY3qrL4fBVA_7hsENeO1zjb2NIea1e0A11OfxzpDp7f5FZa63v2ke';
    const REDIR_SUCCESS = '/paypal-callback.php?success=true&id=';
    const REDIR_FAILURE = '/paypal-callback.php?success=false&id=';
    /** @var string */
    public $redirectUrl;
    /** @var PPConnectionException */
    public $error;
    /** @var string */
    public $paymentId;
    /** @var ApiContext */
    protected $apiContext;

    public function __construct() {
        $this->apiContext = new ApiContext(new OAuthTokenCredential(self::PAYPAL_CLIENT_ID, self::PAYPAL_SECRET));
        $this->apiContext->setConfig(
                         array(
                             'mode'                   => ENVIRONMENT === DEV ? 'sandbox' : 'live',
                             'http.ConnectionTimeOut' => 30,
                             'log.LogEnabled'         => true,
                             'log.FileName'           => 'PayPal.log',
                             'log.LogLevel'           => 'FINE',
                             'custom'                 => 'custom field value',
                         )
        );
        $this->db = Db::get_instance();
    }

    /**
     * @return string
     */
    protected function getCustomInfo() {
        global $user;
        $customInfo = array(
            'user_id' => $user->id,
            'pro_id' => ''
        );
        return base64_encode(serialize($customInfo));
    }

    /**
     * @param string $encoded
     *
     * @return mixed
     */
    protected function extractCustomInfo($encoded) {
        return unserialize(base64_decode($encoded));
    }

    public function buyItem($name, $price, $id) {
        $payer = new Payer;
        $payer->setPaymentMethod('paypal');
        $item = new Item;
        $item->setCurrency('USD');
        $item->setName($name);
        $item->setPrice($price);
        $item->setQuantity(1);
        $item->setSku($id);
        $itemList = new ItemList;
        $itemList->setItems(array($item));
        $details = new Details;
        $details->setShipping('0')
                ->setTax('0')
                ->setSubtotal($price);
        $amount = new Amount;
        $amount->setCurrency('USD')
               ->setTotal($price)
               ->setDetails($details);
        $transaction = new Transaction;
        $transaction->setItemList($itemList);
        $transaction->setAmount($amount);
        $redirectUrls = new RedirectUrls;
        $redirectUrls->setReturnUrl(PROTO . HOST . self::REDIR_SUCCESS)
                     ->setCancelUrl(PROTO . HOST . self::REDIR_FAILURE);
        $payment = new Payment;
        $payment->setIntent('sale')
                ->setPayer($payer)
                ->setRedirectUrls($redirectUrls)
                ->setTransactions(array($transaction));
        try {
            $payment->create($this->apiContext);
        } catch (PPConnectionException $ex) {
            $this->error = $ex;
            return false;
        }
        foreach ($payment->getLinks() as $link) {
            if ($link->getRel() === 'approval_url') {
                $this->redirectUrl = $link->getHref();
                break;
            }
        }
        $this->paymentId = $payment->getId();
        return true;
    }

    public function parseResponse() {
        $ipn = new PPIPNMessage;
        $data = $ipn->getRawData();
        if (!$ipn->validate()) {
            // IPN not valid
            return false;
        }
        if ($data['payment_status'] !== 'Completed') {
            // Transaction not completed
            return false;
        }
        $tx_id = $ipn->getTransactionId();
        $txc = $this->db->get_column(
            'SELECT COUNT(*) FROM `maenna_professional_payments` WHERE `transaction_id` = :tx_id LIMIT 1',
            array(':tx_id' => $tx_id)
        );
        if ($txc != 0) {
            // Transaction already processed
            return false;
        }
        $amount = $data['mc_gross'];
        $customInfo = $this->extractCustomInfo($data['custom']);
        $paymentId = $this->db->run(
            'INSERT INTO `maenna_professional_payments` (
                "user_id", "pro_id", "transaction_id", "amount", "status", "date_created"
            ) VALUES (
                :user_id, :pro_id, :tx_id, :amount, 1, NOW()
            )',
            array(
             ':user_id' => $customInfo['user_id'],
             ':pro_id'  => $customInfo['pro_id'],
             ':tx_id'   => $tx_id,
             ':amount'  => $amount
            )
        );

        if(!empty($paymentId)) {
            $insightId = (int) $customInfo['pro_id'];
            $userId = (int) $customInfo['user_id'];
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

        /*
        Array
        (
            [residence_country] => US
            [invoice] => abc1234
            [address_city] => San Jose
            [first_name] => John
            [payer_id] => TESTBUYERID01
            [shipping] => 3.04
            [mc_fee] => 0.44
            [txn_id] => 878813798
            [receiver_email] => seller@paypalsandbox.com
            [quantity] => 1
            [custom] => xyz123
            [payment_date] => 12:37:32 23 Jun 2014 PDT
            [address_country_code] => US
            [address_zip] => 95131
            [tax] => 2.02
            [item_name] => something
            [address_name] => John Smith
            [last_name] => Smith
            [receiver_id] => seller@paypalsandbox.com
            [item_number] => AK-1234
            [verify_sign] => AFcWxV21C7fd0v3bYYYRCpSSRl31ABA5-j.LhQx8uTJ60zJfTNvmVlPM
            [address_country] => United States
            [payment_status] => Completed
            [address_status] => confirmed
            [business] => seller@paypalsandbox.com
            [payer_email] => buyer@paypalsandbox.com
            [notify_version] => 2.1
            [txn_type] => web_accept
            [test_ipn] => 1
            [payer_status] => verified
            [mc_currency] => USD
            [mc_gross] => 12.34
            [address_state] => CA
            [mc_gross1] => 9.34
            [payment_type] => instant
            [address_street] => 123, any street
        )*/
    }
}

?>
<script type="text/javascript">
    var i = {
        "id": "PAY-6U290923N9074603TKOUIHOQ",
        "create_time": "2014-06-23T19:44:58Z",
        "update_time": "2014-06-23T19:44:59Z",
        "state": "created",
        "intent": "sale",
        "payer": {
            "payment_method": "paypal",
            "payer_info": {
                "shipping_address": {}
            }
        },
        "transactions": [
            {
                "amount": {
                    "total": "125.40",
                    "currency": "USD",
                    "details": {
                        "subtotal": "125.40"
                    }
                },
                "item_list": {
                    "items": [
                        {
                            "name": "Insight",
                            "sku": "ins-3",
                            "price": "125.40",
                            "currency": "USD",
                            "quantity": "1"}
                    ]
                }
            }
        ],
        "links": [
            {
                "href": "https://api.sandbox.paypal.com/v1/payments/payment/PAY-6U290923N9074603TKOUIHOQ",
                "rel": "self",
                "method": "GET"
            },
            {
                "href": "https://www.sandbox.paypal.com/cgi-bin/webscr?cmd=_express-checkout&token=EC-6XV47315A16239237",
                "rel": "approval_url",
                "method": "REDIRECT"
            },
            {
                "href": "https://api.sandbox.paypal.com/v1/payments/payment/PAY-6U290923N9074603TKOUIHOQ/execute",
                "rel": "execute",
                "method": "POST"
            }
        ]
    };
</script>
