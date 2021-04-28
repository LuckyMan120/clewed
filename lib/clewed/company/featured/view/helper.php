<?php
/**
 * @author oleg bursacovschi <o.bursacovschi@gmail.com>
 */
namespace Clewed\Company\Featured\View;

class Helper
{
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

    /**
     * Builds correct formatted sector string
     *
     * @param $company
     * @return string
     */
    public function buildSectorString($company)
    {
        if (empty($company['sector']))
            return '';

        $data = array();
        array_push($data, preg_replace('/(?<=\\w)(?=[A-Z])/', ' $1', $company['sector']));

        return $this->e(implode(' - ', $data));
    }

    /**
     * Builds correct formatted revenue string
     *
     * @param $company
     */
    public function buildRevenueString($company)
    {
        //custom this return 'REV: $0';
        $y = date('Y');
        if (empty($company['financial'][$y]))
            return 'REV: $0';

        $revenue = '';
        if ($company['financial'][$y] >= 1000000000)
            $revenue .= "Rev < $" . (int) ($company['financial'][$y] / 1000000000) . " B";
        else
            $revenue .= "Rev < $" . (int) ($company['financial'][$y] / 1000000) . " M";

        return $this->e($revenue);
    }

    /**
     * Builds correct formatted City, State string
     *
     * @param $company
     * @return string
     */
    public function buildCityStateString($company)
    {
        $data = array();
        if (!empty($company['city']))
            array_push($data, ucwords($company['city']));

        if (!empty($company['state']))
            array_push($data, strtoupper($company['state']));

        return $this->e(implode(', ', $data));
    }

    /**
     * Builds correct image url
     *
     * @param $company
     * @return string
     */
    public function buildImageUrlString($company)
    {
        $url = '';
        if (!empty($company['project']) && file_exists('themes/maennaco/images/project/' . $company['project'])) {
            $url .= '/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/' . urlencode($company['project']) . '&zc=1&w=191&h=143';
        }
        elseif (file_exists('sites/default/images/company/50x50/' . $company['id'] . '.jpg')) {
            $url .= '/themes/maennaco/phpthumb/phpThumb.php?src=sites/default/images/company/50x50/' . $company['id'] . '.jpg';
        }
        elseif('service' == $company['company_type'])
            $url .= '/themes/maennaco/images/cmp-avatar-service.png';
        else
            $url .= '/themes/maennaco/images/cmp-avatar-product.png';

        return $url;
    }
}