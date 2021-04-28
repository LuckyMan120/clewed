<?php
/**
 * Model class to manipulate insights
 *
 * @author Vladimir Sergeev <v.s.sergeev@gmail.com>
 */
namespace Clewed\Insights;

use Clewed\Db;

class InsightRepository {

    /** @var Db */
    protected $db;

    public function __construct() {
        $this->db = Db::get_instance();
    }

    /**
     * @param int $insightId
     * @return InsightEntity|null
     */
    public function findById($insightId)
    {
        $insightId = intval($insightId);
        $sql = 'SELECT * FROM `maenna_professional` WHERE `id` = ?';

        return $this->getOneByQuery($sql, array($insightId));
    }

    /**
     * @param $templateInsightId
     * @param $attendeeUid
     * @return InsightEntity|null
     */
    public function findAttendedByTemplateInsight($templateInsightId, $attendeeUid)
    {
        $sql = 'SELECT mp.* FROM
                    `maenna_professional` AS mp JOIN maenna_professional_payments AS mpp ON(mp.id=mpp.pro_id)
                WHERE mp.template_insight_id = ? AND mpp.user_id = ? ORDER BY mp.id DESC LIMIT 1';
        return $this->getOneByQuery($sql, array($templateInsightId, $attendeeUid));
    }

    /**
     * @param int $insightId
     * @return int|null
     */
    public function findAttendeeIdPurchasedPrivateInsight($insightId)
    {
        $sql = 'SELECT * FROM maenna_professional_payments
                WHERE pro_id = ?
                ORDER BY id ASC LIMIT 1';
        if ($row = $this->db->get_row($sql, array($insightId))) {
            return $row['user_id'];
        }
        return null;
    }
    /**
     * @param int $insightId
     * @return array|null
     * Returns an array of attending user id`s
     */
    public function findAttendees($insightId)
    {
        $returnArr = array();
        $sql = 'SELECT user_id FROM maenna_professional_payments
                WHERE pro_id = ?
                ';
        if ($row = $this->db->get_array($sql, array($insightId))) {

            foreach($row as $value)
                $returnArr[] = $value['user_id'];
            return $returnArr;
        }
        return null;
    }

    /**
     * @param int $insightId
     * @return array|null
     * Returns an array of users who rated for particular insight
     */
    public function findUsersWhoRated($insightId)
    {
        $returnArr = array();
        $sql = "SELECT editor_uid FROM user_rating WHERE insight_id = :insight_id";

        $result = $this->db->get_array($sql,array('insight_id' => $insightId));

        if (count($result) > 0) {
            foreach ($result as $value) {

                $returnArr[] = $value['editor_uid'];

            }
            return $returnArr;
        }
        else return null;
    }
    
    

    /**
     * @param string $sql
     * @param array $params
     * @return InsightEntity|null
     */
    private function getOneByQuery($sql, $params)
    {
        if ($row = $this->db->get_row($sql, $params)) {
            $insight = new InsightEntity();
            $insight->populate($row);
            return $insight;
        }
        return null;
    }
}
