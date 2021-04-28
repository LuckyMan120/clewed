<?php
/**
 * @author Dmitry Vovk <dmitry.vovk@gmail.com>
 */
namespace Clewed\Insights;

class Discount {

    /** @var int */
    public $discountId;
    /** @var int */
    public $insightId;
    /** @var int */
    public $rate = 100;
    /** @var string */
    public $code = '';
    /** @var bool */
    public $approved = false;
}
