<?php
/**
 * Model class to manipulate discounts
 *
 * @author Dmitry Vovk <dmitry.vovk@gmail.com>
 */
namespace Clewed\Insights;

use Clewed\Db;

class DiscountModel {

    /** @var \Clewed\Db */
    protected $db;
    const CODE_LEN = 7;
    const CHARS = 'ABCDEFGH123456789XYZPQRSTUVW';

    public function __construct() {
        $this->db = Db::get_instance();
    }

    public function create($insightId, $rate = 100, $code = null) {
        assert(is_numeric($insightId));
        $discount = new Discount;
        $discount->insightId = $insightId;
        $discount->rate = $rate;
        $discount->code = $code ?: self::generateCode();
        return $discount;
    }

    /**
     * @return string
     */
    public static function generateCode() {
        $code = '';
        for ($i = 0; $i < self::CODE_LEN; $i++) {
            $code .= substr(self::CHARS, rand(0, strlen(self::CHARS) - 1), 1);
        }
        return $code;
    }

    /**
     * @param int $insightId
     *
     * @return bool|Discount
     */
    public function getInsightDiscount($insightId) {
        assert(is_numeric($insightId));
        $sql = 'SELECT `discount_id`, `insight_id`, `code`, `rate`, `approved` FROM `insight_discount` WHERE `insight_id` = :insight_id';
        if ($row = $this->db->get_row($sql, array(':insight_id' => $insightId))) {
            $discount = new Discount;
            $discount->discountId = $row['discount_id'];
            $discount->insightId = $insightId;
            $discount->code = $row['code'];
            $discount->rate = $row['rate'];
            $discount->approved = (bool) $row['approved'];
            return $discount;
        }
        return false;
    }

    /**
     * @param Discount $discount
     *
     * @return bool|int
     */
    public function save(Discount $discount) {
        $data = array(
            ':insight_id' => $discount->insightId,
            ':code'       => $discount->code,
            ':rate'       => $discount->rate,
            ':approved'   => $discount->approved,
        );
        if (null === $discount->discountId) {
            $sql = 'INSERT INTO `insight_discount` (`insight_id`,`code`,`rate`,`approved`) VALUES (:insight_id,:code,:rate,:approved)';
        } else {
            $sql = 'UPDATE `insight_discount` SET `insight_id`=:insight_id,`code`=:code,`rate`=:rate,`approved`=:approved WHERE `discount_id` = :discount_id LIMIT 1';
            $data[':discount_id'] = $discount->discountId;
        }
        return $this->db->run($sql, $data);
    }

    /**
     * @param string|int|Discount $discount
     *
     * @return bool|int
     */
    public function delete($discount) {
        if ($discount instanceof $discount) {
            $id = $discount->discountId;
        } else if (is_numeric($discount)) {
            $id = $discount;
        } else {
            return false;
        }
        $sql = 'DELETE FROM `insight_discount` WHERE `discount_id` = :discount_id LIMIT 1';
        return $this->db->run($sql, array(':discount_id' => $id));
    }
}
