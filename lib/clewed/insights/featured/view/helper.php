<?php
/**
 * @author oleg bursacovschi <o.bursacovschi@gmail.com>
 */
namespace Clewed\Insights\Featured\View;

class Helper {

    /**
     * Builds correct formatted expertise string
     *
     * @param $insight
     * @return string
     */
    public function buildExpertiseString($insight)
    {
        if (empty($insight['author']['expertise']))
            return '';

        $data = array();
        foreach($insight['author']['expertise'] as $expertise)
            if(!empty($expertise))
                $data[] = trim($expertise);

        return $this->e(implode(', ', $data));
    }

    /**
     * Builds correct formatted price string
     *
     * @param $insight
     * @return string
     */
    public function buildPriceString($insight)
    {
        if(empty($insight['price']))
            return '$0';

        if($insight['price'] < 1000)
            return '$' . $insight['price'];

        $short = number_format($insight['price'] / 1000, '1', '.', '');
        if('0' == substr($short, -1, 1))
            $short = substr($short, 0, -2);

        return $this->e('$' . $short . 'K');
    }

    /**
     * Builds correct formatted views string
     *
     * @param $insight
     * @return string
     */
    public function buildViewsString($insight)
    {
        if(empty($insight['views']))
            return '0';

        if($insight['views'] < 1000)
            return $insight['views'];

        $short = number_format($insight['views'] / 1000, '1', '.', '');
        if('0' == substr($short, -1, 1))
            $short = substr($short, 0, -2);

        return $this->e($short . 'K');
    }

    /**
     * Escape short syntax
     *
     * @param $string
     * @return string
     */
    public function e($string)
    {
        return $this->escape($string);
    }

    /**
     * Escape function
     *
     * @param $string
     * @return string
     */
    public function escape($string)
    {
        return htmlspecialchars($string, ENT_QUOTES, 'utf-8');
    }
}