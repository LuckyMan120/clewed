<?php include __DIR__ . '/../../../lib/init.php';

error_reporting(0);

if (isset($_REQUEST['command']) && $_REQUEST['command'] == "uploadImage") {
    // Check if file was uploaded without errors
    $allowed = array("jpg" => "image/jpg", "jpeg" => "image/jpeg", "gif" => "image/gif", "png" => "image/png");
    $filename = $_FILES["upload"]["name"];
    $filetype = $_FILES["upload"]["type"];
    $filesize = $_FILES["upload"]["size"];
    $time = time();

    // Verify file extension
    $ext = pathinfo($filename, PATHINFO_EXTENSION);
    if(!array_key_exists($ext, $allowed)) die("Error: Please select a valid file format.");

    // Verify file size - 5MB maximum
    $maxsize = 20 * 1024 * 1024;
    if($filesize > $maxsize) die("Error: File size is larger than the allowed limit.");

    // Verify MYME type of the file
    sleep(1);
    if(in_array($filetype, $allowed)){
        move_uploaded_file($_FILES["upload"]["tmp_name"], "../../../themes/maennaco/images/project/". $time . "_" . basename($_FILES["upload"]["name"]));
//        echo "Your file was uploaded successfully.";

        die(htmlspecialchars(json_encode(array(
            'uploaded' => true,
            "url" => '../../../themes/maennaco/images/project/'. $time . '_' . basename($_FILES["upload"]["name"])
        )), ENT_NOQUOTES));
    }
}

if(isset($_REQUEST['type']) && 'insight-cover-image' == $_REQUEST['type']) {

    $insightId = (int) $_REQUEST['insight'];
    $time = (int) $_REQUEST['time'];
    $m = $_REQUEST['m'];

    if($m === md5('cropper.php:' . $time . ':' . $insightId . ':kyarata75')) {

        if(is_readable($path = ROOT . 'sites/default/images/insights/original/' . $insightId . '.jpg')) {
            $x = (int) $_REQUEST['x'];
            $y = (int) $_REQUEST['y'];
            $w = (int) $_REQUEST['w'];
            $h = (int) $_REQUEST['h'];
            $relativeWidth = (int) $_REQUEST['relativeWidth'];

            if(0 !== $relativeWidth) {

                $initialSize = getimagesize($path);

                $ratio = $initialSize[0] / $relativeWidth;
                $x *= $ratio;
                $y *= $ratio;
                $w *= $ratio;
                $h *= $ratio;

                $cropped = imagecreatetruecolor($w, $h);
                $original = imagecreatefromstring(file_get_contents($path));

                imagecopyresampled ($cropped, $original, 0, 0, $x, $y, $w, $h, $w, $h);
                imagejpeg($cropped, $path, 80);

                $result = array(
                    'success' => true
                );

                die(htmlspecialchars(json_encode($result), ENT_NOQUOTES));
            }
        }
    }
}

elseif(isset($_REQUEST['type']) && 'company-cover-image' == $_REQUEST['type']) {

    $companyId = (int) $_REQUEST['company'];
    $u = (int) $_REQUEST['u'];
    $m = $_REQUEST['m'];
    $filename = preg_replace('|[^0-9A-Za-z\.\-]|', '', $_REQUEST['filename']);

    if($m === md5('cropper.php:' . $u . ':' . $companyId . ':kyarata75')) {

        if(is_readable($path = ROOT . 'themes/maennaco/images/project/' . $filename)) {
            $x = (int) $_REQUEST['x'];
            $y = (int) $_REQUEST['y'];
            $w = (int) $_REQUEST['w'];
            $h = (int) $_REQUEST['h'];
            $relativeWidth = (int) $_REQUEST['relativeWidth'];

            if(0 !== $relativeWidth) {

                $initialSize = getimagesize($path);

                $ratio = $initialSize[0] / $relativeWidth;
                $x *= $ratio;
                $y *= $ratio;
                $w *= $ratio;
                $h *= $ratio;

                $cropped = imagecreatetruecolor($w, $h);
                $original = imagecreatefromstring(file_get_contents($path));

                imagecopyresampled ($cropped, $original, 0, 0, $x, $y, $w, $h, $w, $h);
                imagejpeg($cropped, $path, 80);

                $result = array(
                    'success' => true
                );

                die(htmlspecialchars(json_encode($result), ENT_NOQUOTES));
            }
        }
    }
}

die(htmlspecialchars(json_encode(array(
    'success' => false
)), ENT_NOQUOTES));