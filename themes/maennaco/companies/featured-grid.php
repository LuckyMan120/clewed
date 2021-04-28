
<?php defined('ROOT') || die;

require_once __DIR__ . '/../../../lib/init.php';

use Clewed\Company\Service;
use Clewed\Company\Featured\View\Helper;

$service = new Service();
$companies = $service->getFeatured();
$_helper = new Helper();

include __DIR__ . '/../../../templates/companies/featured-list.tpl.php';
