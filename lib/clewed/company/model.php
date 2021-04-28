<?php

namespace Clewed\Company;

use Clewed\Db;

class Model {

    protected $db;

    public function __construct() {
        $this->db = Db::get_instance();
    }

    protected $template = array();

    public function  getCompanyInfo() {
        if (isset($_REQUEST['company_id'])) {
            $companyId = $_REQUEST['company_id'];
//            $path = './themes/maennaco/images/project/';
            if ($companyId) {
                $sql = 'SELECT `companyid`, `company_type`, `founded`, `projname`, `sector`, `pseudo_name`,`city`, `state`, `shareable`, `deal_summary_title`,`deal_summary_statement` FROM `maenna_company` where companyid = :id ';
                $company = $this->db->get_row($sql, array(':id' => $companyId));
                $sql = "SELECT `data_value`,`max_per_investor`,`min_per_investor`,`estimated_share_price`,`term`,`round_amount_raising`,`security_type`,`close_date`,`interest_rate`,`funding_purpose` FROM `maenna_company_data` WHERE `data_type` = 'financial' and `companyid` = :id AND `data_attr` = YEAR(NOW())";
                $companyRev = $this->db->get_row($sql, array(':id' => $companyId));
                $sql = 'SELECT `goal`, `mission`, `project` FROM `maenna_about` WHERE project_id='.$companyId;
                $companyAbout =  $this->db->get_row($sql);

                $sql = "SELECT sum(amount) AS sum FROM maenna_professional_investment WHERE company_id = :id and status=3";
                $result = $this->db->get_row($sql, array(':id' => $companyId));
                $total_amount = is_null($result['sum']) ? 0 : $result['sum'];
                $goal_amount = $companyRev['round_amount_raising'];
                $amount_raising_percent = $total_amount / $goal_amount * 100;

                if (!$company['projname']) {
                    $company['projname'] = "Project" . sprintf("%05s", $companyId + 100);
                }

                $company['pseudo_name'] = 'PROJECT ' . $company['pseudo_name'];

                if ($companyAbout){
                    $company['mission'] = $companyAbout['mission'];
                    $company['goal'] = $companyAbout['goal'];
                    $company['project'] = $companyAbout['project'];
                }
                $company['avatar'] = $this->getAvatar($company);
                $company['term'] = $companyRev['term'];
                $company['security_type'] = $companyRev['security_type'];
                $date2 = date_create($companyRev['close_date']);
                $date1 = date_create('now');
                $interval = date_diff($date1, $date2);
                $days_to_go_symbol = $interval->format('%R');
                $days_to_go = $interval->format('%a');
                $company['date_symbol'] = $days_to_go_symbol;
                $company['close_date'] = $days_to_go;
                $company['interest_rate'] = is_null($companyRev['interest_rate']) ? 0 : $companyRev['interest_rate'];
                $company['amount_raising_percent'] = $amount_raising_percent;
                $company['funding_purpose'] = $companyRev['funding_purpose'];
                $company['revenue'] = (int) ($companyRev['data_value'] / 1000000);
                $company['max_per_investor'] = number_format($companyRev['max_per_investor'], 0, '.', ',');
                $company['min_per_investor'] = number_format($companyRev['min_per_investor'], 0, '.', ',');
                $company['estimated_share_price'] = number_format($companyRev['estimated_share_price'], 0, '.', ',');
                $company['round_amount_raising'] = number_format($companyRev['round_amount_raising'], 0, '.', ',');
//
//                 if (!empty($company['project']) && file_exists($path . $company['project'])) {
//                    $company['project'] = '<div class="abt-content" style="height: 442px !important;width: 590px !important;"><img src="/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/' . urlencode($company['project']) . '&zc=1&w=590&h=442" /></div>';
//                } else {
//                    $company['project'] =
//                        '<div class="abt-content" style="text-align:center;"><div class="abtadd"><span>Add a photo that<br />tells your story.</span></div></div>';
//                }

                return $company;
            }
        }
    }

    protected function getAvatar($company) {
        if (!empty($company['project']) && file_exists('themes/maennaco/images/project/' . $company['project'])) {
            return '/themes/maennaco/phpthumb/phpThumb.php?src=../images/project/' . urlencode($company['project']) . '&zc=1&w=150&h=150';
        }
        if ('service' === $company['company_type']) {
            return ' /themes/maennaco/images/cmp-avatar-service.png';
        } else {
            return '/themes/maennaco/images/cmp-avatar-product.png';
        }
    }
}

