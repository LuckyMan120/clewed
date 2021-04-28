<?php
/**
 * @author Dmytro Vovk <dmitry.vovk@gmail.com>
 */
if (!array_key_exists('hash', $_GET)) {
    header($_SERVER['SERVER_PROTOCOL'] . ' 404 Not Found', true, 404);
    die;
}
$hash = $_GET['hash'];
if (strlen($hash) !== 40) {
    header($_SERVER['SERVER_PROTOCOL'] . ' 404 Not Found', true, 404);
    die;
}
// Check referrer
if (!array_key_exists('HTTP_REFERER', $_SERVER)) {
    header($_SERVER['SERVER_PROTOCOL'] . ' 404 Not Found', true, 404);
    die;
}
if (false === strpos($_SERVER['HTTP_REFERER'], $_SERVER['SERVER_NAME'])) {
    header($_SERVER['SERVER_PROTOCOL'] . ' 404 Not Found', true, 404);
    die;
}
// All is good, proceed with the file delivery
require 'lib/init.php';
$db = \Clewed\Db::get_instance();
$file = $db->get_column('SELECT `file` FROM `audio_files` WHERE `hash` = :hash LIMIT 1', array(':hash' => $hash));
if (empty($file)) {
    header($_SERVER['SERVER_PROTOCOL'] . ' 404 Not Found', true, 404);
    die;
}
$file = 'sites/default/files/events_tmp/' . $file;
if (!is_readable($file)) {
    header($_SERVER['SERVER_PROTOCOL'] . ' 404 Not Found', true, 404);
    die;
}
$size = filesize($file);
header('Cache-Control: private', false);
header('Accept-Ranges: bytes');
header('Content-Type: audio/mpeg');
$ranges = array();
if (isset($_SERVER['HTTP_RANGE'])) {
    // ranged download
    $rawRanges = explode(',', str_replace('bytes=', '', $_SERVER['HTTP_RANGE']));
    $contentRanges = array();
    $contentLength = 0;
    foreach ($rawRanges as $range) {
        list($start, $end) = explode('-', $range);
        if ('' == $start) { // "Range: -500"
            $start = $size - $end;
            $end = $size - 1;
        } else { // "Range: 500-" or "Range: 500-600"
            if ('' == $end) { // "Range: 500-"
                $end = $size - 1;
            }
            $start = (int) $start;
        }
        if (($start < $end) && ($start < $size)) {
            // range is valid
            $ranges[] = array(
                'start' => $start,
                'end'   => $end,
                'size'  => $end - $start + 1,
            );
            $contentLength += $end - $start + 1;
            $contentRanges[] = $start . '-' . $end;
        }
    }
    // no valid ranges
    if (0 == count($ranges)) {
        header($_SERVER['SERVER_PROTOCOL'] . ' 416 Range Not Satisfiable', true, 416);
        exit;
    }
    header($_SERVER['SERVER_PROTOCOL'] . ' 206 Partial Content');
    header('Content-Range: bytes ' . implode(',', $contentRanges) . '/' . $size);
    header('Content-Length: ' . $contentLength);
} else {
    // nonranged download
    $ranges[] = array(
        'start' => 0,
        'end'   => $size - 1,
        'size'  => $size,
    );
    header($_SERVER['SERVER_PROTOCOL'] . ' 200 OK');
    header('Content-Length: ' . $size);
}
// The url must be single use
//$db->run('DELETE FROM `audio_files` WHERE `hash` = :hash LIMIT 1', array(':hash' => $hash));
$f = fopen($file, 'r');
foreach ($ranges as $range) {
    fseek($f, $range['start']);
    echo fread($f, $range['size']);
    ob_flush();
    flush();
    if (feof($f)) break;
}
fclose($f);
