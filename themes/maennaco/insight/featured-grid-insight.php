<?php defined('ROOT') || die;

require_once __DIR__ . '/../../../lib/init.php';

use Clewed\Insights\InsightService;
use Clewed\Insights\Featured\View\Helper;

$service = new InsightService();
$insights = $service->getFeatured();

$_helper = new Helper();

include __DIR__ . '/../../../templates/insights/featured-list-insight.tpl.php';
