<?php
/**
 * Container of professionals business logic
 *
 * @author oleg bursacovschi <o.bursacovschi@gmail.com>
 */
namespace Clewed\Professional;

use Clewed\Db;

class Service
{

    protected $db;

    /**
     * Constructor
     */
    public function __construct()
    {
        $this->db = Db::get_instance();
    }

    /**
     * Fetch professionals
     *
     * @param $proIds
     * @return array
     */
    public function get($proIds)
    {
        if (empty($proIds))
            return array();

        $proIdsPlaceholder = rtrim(str_repeat('?, ', count($proIds)), ', ');

        $rows = $this->db->get_array("
            SELECT
                mp.pid as id,
                mp.*
            FROM maenna_people mp
            WHERE mp.pid IN({$proIdsPlaceholder})",
            $proIds
        );

        $professionals = array();
        foreach ($rows as $row) {
            $id = (int) $row['id'];

            $row['profile'] = strip_tags($row['profile']);
            if (strlen($row['profile']) > 139)
                $row['profile'] = rtrim(substr($row['profile'], 0, 139), ',. ') . '...';

            $row['experties'] = preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $row['experties']);
            $row['experties2'] = preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $row['experties2']);
            $row['experties3'] = preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $row['experties3']);

            $professionals[$id] = $row;
        }

        $rows = $this->db->get_array("
            SELECT
                pid as id,
                data_value2 AS expertise
            FROM
            maenna_people_data
            WHERE data_type = 'addinfo'
            AND data_attr = 'experties'
            AND pid IN({$proIdsPlaceholder})
            ORDER BY dataid ASC",
            $proIds
        );

        foreach ($rows as $row) {
            $id = (int) $row['id'];

            $row['expertise'] = strip_tags($row['expertise']);
            if (strlen($row['expertise']) > 160)
                $row['expertise'] = rtrim(substr($row['expertise'], 0, 160), ',. ') . '...';

            $professionals[$id]['expertise'] = $row['expertise'];
        }

        $rows = $this->db->get_array("
            SELECT
                pid as id,
                data_value as undergraduate,
                data_value3 as graduate
            FROM maenna_people_data
            WHERE data_type = 'education'
            AND pid IN({$proIdsPlaceholder})
            ORDER BY dataid ASC",
            $proIds
        );

        foreach ($rows as $row) {
            $id = (int) $row['id'];

            $row['graduate'] = strip_tags($row['graduate']);
            $professionals[$id]['graduate'] = $row['graduate'];

            $row['undergraduate'] = strip_tags($row['undergraduate']);
            $professionals[$id]['undergraduate'] = $row['undergraduate'];
        }

        return $professionals;
    }
}