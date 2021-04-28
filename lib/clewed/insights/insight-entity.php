<?php

namespace Clewed\Insights;

class InsightEntity {

    const TYPE_GROUP_INSIGHT = 0;
    const TYPE_PRIVATE_TEMPLATE = 1;
    const TYPE_PRIVATE_INSIGHT = 2;

    const STATUS_APPROVED = 1;
    const STATUS_NOT_APPROVED = 0;

    /**
     * @var int
     */
    public $id;

    /**
     * @var  string
     */
    public $username = '';

    /**
     * @var int
     */
    public $postedby = 0;

    /**
     * @var int
     */
    public $type = 0;

    /**
     * @var string
     */
    public $title = '';

    /**
     * @var string
     */
    public $description = '';

    /**
     * @var string
     */
    public $whyattend = '';

    /**
     * @var string
     */
    public $buyer_requirement;

    /**
     * @var string
     */
    public $location = '';

    /**
     * @var string
     */
    public $tags = '';

    /**
     * @var string
     */
    public $industry = '';

    /**
     * Timestamp of insights beginning
     * @var int
     */
    public $datetime = 0;

    /**
     * Timestamp when insight was created
     * @var int
     */
    public $created = 0;

    /**
     * @var float
     */
    public $cost = 0;

    /**
     * @var float
     */
    public $hourlyrate = 0;

    /**
     * @var float
     */
    public $hours = 0;

    /**
     * @var int
     */
    public $capacity = 0;

    /**
     * @var int
     */
    public $spots = 0;

    /**
     * @var int
     */
    public $approve_status = 0;

    /**
     * @var int
     */
    public $views = 0;

    /**
     * ID of template (parent) insight which this inside was copied from
     * @var int
     */
    public $template_insight_id = 0;

    /**
     * Delivered state used for private insights (services)
     * @var int
     */
    public $delivered = 0;

    /**
     * Duration of service
     * @var int
     */
    public $duration = 0;

    /**
     * @param $properties
     * @throws \Exception
     */
    public function populate($properties)
    {
        foreach ($properties as $key => $value) {
            if (property_exists($this, $key)) {
                $this->$key = $value;
            }
        }
    }

    /**
     * Populate some common data from creation and edition forms
     * @param array $requestData
     */
    public function populateFromRequest($requestData)
    {
        $this->title          = $requestData['title'];
        $this->description    = $requestData['desc'];
        $this->location       = $requestData['loc'];
        if (strlen(trim($requestData['date'])) > 0) {
            $this->setDatetimeFromUsaDateFormat($requestData['date']);
        } else {
            $this->datetime = time();
        }
        $this->whyattend      = $requestData['whyattend'];
        $this->buyer_requirement = $requestData['buyer_requirement'];
        $this->capacity       = (int)$requestData['capacity'];
        $this->cost           = str_replace(array('$', ',', ' '), '', $requestData['cost']);
        $this->tags           = $requestData['tags'];
        $this->industry       = $requestData['industry'];
        if (array_key_exists('duration', $requestData)) {
            $this->duration = $requestData['duration'];
        } else {
            $this->duration = 0;
        }
    }
    
    /**
     * @return array
     */
    public function export()
    {
        return array(
            'id'            => $this->id,
            'username'      => $this->username,
            'postedby'      => $this->postedby,
            'type'          => $this->type,
            'title'         => $this->title,
            'description'   => $this->description,
            'whyattend'     => $this->whyattend,
            'buyer_requirement' => $this->buyer_requirement,
            'location'      => $this->location,
            'tags'          => $this->tags,
            'industry'      => $this->industry,
            'datetime'      => $this->datetime,
            'created'       => $this->created,
            'cost'          => $this->cost,
            'hourlyrate'    => $this->hourlyrate,
            'hours'         => $this->hours,
            'capacity'      => $this->capacity,
            'spots'         => $this->spots,
            'approve_status' => $this->approve_status,
            'views'         => $this->views,
            'delivered'     => $this->delivered,
            'duration'      => $this->duration
        );
    }

    /**
     * @return array
     */
    public function exportAsPlaceholders()
    {
        $placeholders = array();
        foreach ($this->export() as $property => $value) {
            $placeholders[':' . $property] = $value;
        }
        return $placeholders;
    }

    /**
     * Fill datetime timestamp property from date representation like "mm-dd-YYYY HH:ii"
     *
     * @param $dateTime
     */
    public function setDatetimeFromUsaDateFormat($dateTime)
    {
        $this->datetime = strtotime(str_replace('-', '/', $dateTime));
    }

    /**
     * @return bool
     */
    public function isGroupInsight()
    {
        return $this->type == self::TYPE_GROUP_INSIGHT;
    }

    /**
     * @return bool
     */
    public function isPrivateTemplateInsight()
    {
        return $this->type == self::TYPE_PRIVATE_TEMPLATE;
    }

    /**
     * @return bool
     */
    public function isPrivateInsight()
    {
        return $this->type == self::TYPE_PRIVATE_INSIGHT;
    }

    /**
     * @return bool
     */
    public function isApproved()
    {
        return $this->approve_status == 1;
    }
}
