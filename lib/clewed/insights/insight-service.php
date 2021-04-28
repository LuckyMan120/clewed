<?php
/**
 * Container of insights business logic
 *
 * @author Vladimir Sergeev <v.s.sergeev@gmail.com>
 */
namespace Clewed\Insights;

use Clewed\Db;
use Clewed\Notifications\NotificationService;

class InsightService
{

    /** @var Db */
    protected $db;

    public function __construct()
    {
        $this->db = Db::get_instance();
        $this->model = new InsightModel();
    }

    /**
     * @param int $attendeeId
     * @param InsightEntity $insight
     * @param bool $insertFakePayment
     * @return InsightEntity
     */
    public function attend($attendeeId, InsightEntity $insight, $insertFakePayment = false)
    {
        if ($insight->isPrivateTemplateInsight()) {

            $privateInsight = $this->duplicatePrivateInsightForAttending($insight);
            $this->duplicateInsightDiscount($insight, $privateInsight);

            if ($insertFakePayment) {
                $this->insertFakePayment($attendeeId, $privateInsight->id);
            }

            $this->duplicateInsightRequirementQuestions($insight, $privateInsight);
            $this->duplicateInsightMilestones($insight, $privateInsight);

            return $privateInsight;
        } else {
            if ($insertFakePayment) {
                $this->insertFakePayment($attendeeId, $insight->id);
            }
            $insight->spots = $insight->spots >= 1 ? $insight->spots - 1 : 0;
            $this->model->save($insight);

            return $insight;
        }
    }

    /**
     * Register new insight view
     *
     * @param int $insightId
     *
     * @return bool|int
     */
    public function addView($insightId)
    {
        assert('is_int($insightId)');
        return $this->db->run(
            'UPDATE `maenna_professional` SET `views` = `views` + 1 WHERE `id` = :id LIMIT 1',
            array(':id' => $insightId)
        );
    }

    /**
     * @param InsightEntity $insight
     * @return InsightEntity
     */
    private function duplicatePrivateInsightForAttending(InsightEntity $insight)
    {
        $newInsight = clone $insight;
        $newInsight->id = null;
        $newInsight->type = InsightEntity::TYPE_PRIVATE_INSIGHT;
        $newInsight->spots = $insight->capacity;
        $newInsight->views = 0;
        $newInsight->created = time();
        $newInsight->template_insight_id = $insight->id;

        $this->model->save($newInsight);
        return $newInsight;
    }

    /**
     * @param InsightEntity $oldInsight
     * @param InsightEntity $newInsight
     */
    private function duplicateInsightRequirementQuestions(InsightEntity $oldInsight, InsightEntity $newInsight) {
        $sql = "SELECT * FROM `maenna_questionair` WHERE `parentid` = {$oldInsight->id} AND `target` = 'service' AND `editorid` = {$oldInsight->postedby};";
        $rows = $this->db->get_array($sql);

        foreach ($rows as $row) {
            $query = "INSERT INTO `maenna_questionair` (`parentid`, `target`, `type`, `content`, `created`, `editorid`) VALUES (:parent_id, 'service', 'question', :content, :created_at, :editor_id);";
            $values = array(':parent_id' => $newInsight->id, ':content' => $row['content'], ':created_at' => time(), ':editor_id' => $row['editorid']);
            $this->db->run($query, $values);
        }
        return true;
    }

    /**
     * @param InsightEntity $oldInsight
     * @param InsightEntity $newInsight
     */
    private function duplicateInsightMilestones(InsightEntity $oldInsight, InsightEntity $newInsight) {
        $sql = "SELECT * FROM `project_service_milestones` WHERE `service_id` = {$oldInsight->id}";
        $rows = $this->db->get_array($sql);

        foreach ($rows as $row) {
            $query = "INSERT INTO `project_service_milestones` (`service_id`, `description`, `duration`) VALUES (:new_insight_id, :description, :duration)";
            $values = array(':new_insight_id' => $newInsight->id, ':description' => $row['description'], ':duration' => $row['duration']);
            $this->db->run($query, $values);
        }
        return true;
    }

    /**
     * @param InsightEntity $oldInsight
     * @param InsightEntity $newInsight
     * @return bool|Discount
     */
    private function duplicateInsightDiscount(InsightEntity $oldInsight, InsightEntity $newInsight)
    {
        $discountModel = new DiscountModel;
        $oldDiscount = $discountModel->getInsightDiscount($oldInsight->id);
        $newDiscount = clone $oldDiscount;
        $newDiscount->discountId = null;
        $newDiscount->insightId = $newInsight->id;

        $discountModel->save($newDiscount);
        return $newDiscount;
    }

    /**
     * @param $attendeeId
     * @param $insightId
     */
    private function insertFakePayment($attendeeId, $insightId)
    {
        mysql_query("
            INSERT INTO maenna_professional_payments  (user_id,pro_id,transaction_id,amount,discount_rate,status, date_created)
            VALUES('" . $attendeeId . "','" . $insightId . "','','0','100','1', '" . time() . "')");

        $insightId = (int) $insightId;
        $userId = (int) $attendeeId;
        if (!empty($insightId) && !empty($userId)) {
            $offerRepository = new InsightRepository();
            $offer = $offerRepository->findById($insightId);
            if (!empty($offer) && $offer->type == InsightEntity::TYPE_GROUP_INSIGHT) {
                $notificationService = new NotificationService();
                $notificationService->registerEvent(
                    'insight_review_requested',
                    $insightId,
                    $userId
                );
            }
        }
    }

    /**
     * Fetch featured insights
     * @param $limit
     * @return array
     */
    public function getFeatured($limit = 12)
    {

        $rows = $this->db->get_array("
            SELECT
                mp.*,
                p.*,
                mp.industry as pindustry,
                CASE r.reviews IS NULL WHEN TRUE THEN 0 ELSE r.reviews END as reviews,
                CASE j.oined IS NULL WHEN TRUE THEN 0 ELSE j.oined END as joined
            FROM maenna_professional mp
            INNER JOIN maenna_people p ON p.pid = mp.postedby
            LEFT JOIN (
                SELECT
                    pro_id,
                    COUNT(DISTINCT(user_id)) as oined
                FROM maenna_professional_payments
                GROUP BY pro_id
            ) j ON j.pro_id = mp.id
            LEFT JOIN (
                SELECT
                    target_uid,
                    COUNT(*) as reviews
                FROM user_rating
                GROUP BY target_uid
            ) r ON r.target_uid = mp.postedby
            WHERE mp.featured = 1
            AND mp.approve_status = 1
            AND (j.oined IS NULL OR j.oined < mp.capacity)
            LIMIT {$limit}"
        );

        $professionalIds = array();
        $insights = array();
        foreach ($rows as $row) {
            $insight = array(
                'id' => $insightId = (int) $row['id'],
                'title' => $row['title'],
                'type' => 0 == $row['type'] ? 'Insight' : 'Service',
                'views' => $row['views'],
                'price' => $row['cost'],
                'category' => $row['tags'],
                'industry' => $row['pindustry'],
                'author' => array(
                    'id' => $professionalId = (int) $row['postedby'],
                    'first-name' => $row['firstname'],
                    'last-name' => $row['lastname'],
                    'full-name' => $row['firstname'] . ' ' . (1 == $row['username_type'] ? substr($row['lastname'], 0, 1) : $row['lastname']),
                    'expertise' => array(
                        $row['experties'],
                        $row['experties2']
                    ),
                    'education' => ucfirst($row['trained_at'] ? $row['trained_at'] : 'Undergrad'),
                    'rating' => number_format((float) $row['rating'], 2, '.', ''),
                    'reviews-count' => $row['reviews'],
                    'work-years' => (int) $row['yearwork'] ? (date("Y") - $row['yearwork']) : 0,
                    'tag' => $row['title_tag'],
                    'trained-at' => $row['trained_at']
                )
            );

            $insights[$insightId] = $insight;
            $professionalIds[$professionalId] = $professionalId;
        }

        // fetch author's extra data
        if (!empty($professionalIds)) {

            $placeholders = rtrim(str_repeat('?, ', count($professionalIds)), ', ');
            $rows = $this->db->get_array("
                SELECT *
                FROM maenna_people_data
                WHERE pid IN($placeholders)",
                array_values($professionalIds)
            );

            $professionalExtras = array();
            foreach ($rows as $row) {
                $professionalId = (int) $row['pid'];
                $key = $row['data_type'];
                $value = $this->buildValues($row, 'data_value');
                $professionalExtras[$professionalId][$key] = $value;
            }

            foreach ($insights as $insightId => $insight) {
                $professionalId = $insight['author']['id'];
                foreach ($professionalExtras[$professionalId] as $key => $value) {
                    if ('education' === $key)
                        continue;
                    $insights[$insightId]['author'][$key] = $value;
                }
            }
        }

        return $insights;
    }

    /**
     * Builds a string from numbered columns
     * @param $data
     */
    protected function buildValues($data, $needle)
    {
        $values = array();
        foreach ($data as $key => $value)
            if (false !== strpos($key, $needle) && !empty($value))
                $values[] = $value;
        return implode(', ', $values);
    }

    /**
     * Retrieve offer attendee ids
     *
     * @param $offerIds
     * @return array indexed by offer id
     */
    public function getAttendeeIds($offerIds)
    {
        if (empty($offerIds))
            return array();

        $offerIdPlaceholders = str_repeat('?, ', count($offerIds) - 1) . '?';
        $rows = $this->db->get_array("
            SELECT pro_id, user_id
            FROM maenna_professional_payments
            WHERE pro_id IN({$offerIdPlaceholders})",
            $offerIds
        );

        $attendeeIds = array();
        foreach ($rows as $row) {
            $offerId = (int) $row['pro_id'];
            $attendeeId = (int) $row['user_id'];
            $attendeeIds[$offerId][$attendeeId] = $attendeeId;
        }

        return $attendeeIds;
    }
}
