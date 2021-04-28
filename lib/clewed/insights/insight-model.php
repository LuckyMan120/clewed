<?php
/**
 * Model class to manipulate insights
 *
 * @author Vladimir Sergeev <v.s.sergeev@gmail.com>
 */
namespace Clewed\Insights;

use Clewed\Db;
use Clewed\Notifications\NotificationService;

class InsightModel
{

    /** @var Db */
    protected $db;

    public function __construct()
    {
        $this->db = Db::get_instance();
    }

    /**
     * @param InsightEntity $insight
     * @return bool|int
     */
    public function save(InsightEntity $insight)
    {
        if ($insight->id) {
            $this->update($insight);
        } else {
            $this->create($insight);
        }
    }

    /**
     * @param InsightEntity $insight
     * @return int
     */
    public function create(InsightEntity $insight)
    {
        $sql = 'INSERT INTO
                        `maenna_professional`
                    (
                      `id`, `username`, `postedby`, `type`, `title`, `description`, `whyattend`, `buyer_requirement`,
                      `location`, `tags`, `industry`,`datetime`, `created`, `cost`, `hours`, `hourlyrate`, `capacity`, `spots`, `approve_status`,
                      `views`, `template_insight_id`, `featured`, `delivered`, `duration`
                    )
                    VALUES
                    (
                      NULL, :username, :postedby, :type, :title, :description, :whyattend, :buyer_requirement,
                      :location, :tags, :industry, :datetime, :created, :cost, :hours, :hourlyrate, :capacity, :spots,
                      :approve_status, :views, :template_insight_id, :featured, :delivered, :duration
                    )';

        $placeholders = array(
            ':username' => $insight->username,
            ':postedby' => $insight->postedby,
            ':type' => $insight->type,
            ':title' => $insight->title,
            ':description' => $insight->description,
            ':whyattend' => $insight->whyattend,
            ':buyer_requirement' => $insight->buyer_requirement,
            ':location' => $insight->location,
            ':tags' => $insight->tags,
            ':industry' => (string) $insight->industry,
            ':datetime' => $insight->datetime,
            ':created' => $insight->created,
            ':cost' => $insight->cost,
            ':hours' => $insight->hours,
            ':hourlyrate' => $insight->hourlyrate,
            ':capacity' => $insight->capacity,
            ':spots' => $insight->spots,
            ':approve_status' => $insight->approve_status,
            ':views' => $insight->views,
            ':template_insight_id' => $insight->template_insight_id,
            ':featured' => 0,
            ':delivered' => $insight->delivered,
            ':duration' => (int) $insight->duration
        );

        $this->db->run($sql, $placeholders);
        $insight->id = $this->db->lastInsertId();
        if (!!$insight->approve_status) {
            $notificationService = new NotificationService();
            $notificationService->registerEvent('offer_approved', $insight->id, 0);
        }


        return $insight->id;
    }

    /**
     * @param InsightEntity $insight
     */
    public function update(InsightEntity $insight)
    {
        $sql = "UPDATE
                        maenna_professional
                    SET
                        `type` = :type,
                        `title` = :title,
                        `description` = :description,
                        `whyattend` = :whyattend,
                        `buyer_requirement` = :buyer_requirement,
                        `location` = :location,
                        `tags` = :tags,
                        `industry` = :industry,
                        `datetime` = :datetime,
                        `cost` = :cost,
                        `capacity` = :capacity,
                        `spots` = :spots,
                        `approve_status` = :approve_status,
                        `views` = :views,
                        `delivered` = :delivered,
                        `duration` = :duration
                    WHERE id = :id
                    LIMIT 1";

        $placeholders = array(
            ':id' => $insight->id,
            ':type' => $insight->type,
            ':title' => $insight->title,
            ':description' => $insight->description,
            ':whyattend' => $insight->whyattend,
            ':buyer_requirement' => $insight->buyer_requirement,
            ':location' => $insight->location,
            ':tags' => $insight->tags,
            ':industry' => (string) $insight->industry,
            ':datetime' => $insight->datetime,
            ':cost' => $insight->cost,
            ':capacity' => $insight->capacity,
            ':spots' => $insight->spots,
            ':approve_status' => $insight->approve_status,
            ':views' => $insight->views,
            ':delivered' => $insight->delivered,
            ':duration' => $insight->duration
        );

        $approve = false;
        if (!!$insight->approve_status) {

            $approved = $this->db->get_array("
                SELECT approve_status
                FROM maenna_professional
                WHERE id = :id
                LIMIT 1",
                array(':id' => $insight->id)
            );

            $approved = !!$approved[0]['approve_status'];
            if (!$approved)
                $approve = true;
        }

        $updated = $this->db->run($sql, $placeholders);
        if($updated) {
            //$notificationService = new NotificationService();
            //$notificationService->registerEvent('offer_updated', $insight->id, 0);
        }

        if ($approve) {
            $notificationService = new NotificationService();
            $notificationService->registerEvent('offer_approved', $insight->id, 0);
        }
    }

}
