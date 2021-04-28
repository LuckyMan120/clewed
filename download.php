<?php
$filename = $_REQUEST['file_name']; // of course find the exact filename....
// Check for allowed symbols. Normal file name would not contain "/." sequence.
if (strpos($filename, '/.')) {
    header($_SERVER['SERVER_PROTOCOL'] . ' 404 Not Found', true, 404);
    die;
}
if (isset($_REQUEST['tab']) && $_REQUEST['tab'] == 'file') $filename = 'sites/default/files/' . $filename;
else $filename = 'sites/default/files/events_tmp/' . $filename;
// File either not there or not readable
if (!is_readable($filename)) {
    header($_SERVER['SERVER_PROTOCOL'] . ' 404 Not Found', true, 404);
    die;
}
header('Pragma: public');
header('Expires: 0');
header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
header('Cache-Control: private', false); // required for certain browsers
//header('Content-Type: ' . mime_content_type($filename));
header('Content-Disposition: attachment; filename="' . basename($filename) . '";');
header('Content-Transfer-Encoding: binary');
header('Content-Length: ' . filesize($filename));
readfile($filename);
exit;