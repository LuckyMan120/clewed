<?php
define("__ACCOUNT__", 1);
$timestamp = time();

/**
 * Handle file uploads via XMLHttpRequest
 */
class qqUploadedFileXhr {

    /**
     * Save the file to the specified path
     *
     * @return boolean TRUE on success
     */
    function save($path) {
        $input = fopen("php://input", "r");
        $temp = tmpfile();
        $realSize = stream_copy_to_stream($input, $temp);
        fclose($input);
        if ($realSize != $this->getSize()) {
            return false;
        }
        $target = fopen($path, "w");
        fseek($temp, 0, SEEK_SET);
        stream_copy_to_stream($temp, $target);
        fclose($target);
        return true;
    }

    function getName() {
//        return str_replace(' ', '-', $_GET['qqfile']);
        return $_GET['qqfile'];
    }

    function getFileName() {
        return $_GET['filename'];
    }

    function getSize() {
        if (isset($_SERVER["CONTENT_LENGTH"])) {
            return (int) $_SERVER["CONTENT_LENGTH"];
        } else {
            throw new Exception('Getting content length is not supported.');
        }
    }
}

class qqUploadedFileForm {

    private $inputName;

    /**
     * @param string $inputName ; defaults to the javascript default: 'qqfile'
     */
    public function __construct($inputName = 'qqfile') {
        $this->inputName = $inputName;
    }

    /**
     * Save the file to the specified path
     *
     * @return boolean TRUE on success
     */
    public function save($path) {
        return move_uploaded_file($_FILES[$this->inputName]['tmp_name'], $path);
    }

    /**
     * Get the original filename
     *
     * @return string filename
     */
    public function getName() {
        return $_FILES[$this->inputName]['name'];
    }

    /**
     * Get the file size
     *
     * @return integer file-size in byte
     */
    public function getSize() {
        return $_FILES[$this->inputName]['size'];
    }
}

class qqFileUploader {

    private $allowedExtensions = array();
    private $sizeLimit = 1048576000;
    /** @var qqUploadedFileXhr|qqUploadedFileForm */
    private $file;

    function __construct(array $allowedExtensions = array(), $sizeLimit = 1048576000) {
        $this->allowedExtensions = array_map('strtolower', $allowedExtensions);
        $this->sizeLimit = $sizeLimit;
        $this->checkServerSettings();
        if (isset($_GET['qqfile'])) {
            $this->file = new qqUploadedFileXhr();
        } elseif (isset($_FILES['qqfile'])) {
            $this->file = new qqUploadedFileForm();
        } else {
            $this->file = false;
        }
    }

    private function checkServerSettings() {
        $postSize = $this->toBytes(ini_get('post_max_size'));
        $uploadSize = $this->toBytes(ini_get('upload_max_filesize'));
    }

    private function toBytes($str) {
        $val = trim($str);
        $last = strtolower($str[strlen($str) - 1]);
        switch ($last) {
            case 'g':
                $val *= 1024;
                break;
            case 'm':
                $val *= 1024;
                break;
            case 'k':
                $val *= 1024;
                break;
        }
        return $val;
    }

    /**
     * Returns array('success'=>true) or array('error'=>'error message')
     */
    function handleUpload($uploadDirectory, $replaceOldFile = false, $newFileName = null) {
        global $timestamp;
        if (!is_writable($uploadDirectory)) {
            return array('error' => "Server error. Upload directory isn't writable.");
        }
        if (!$this->file) {
            return array('error' => 'No files were uploaded.');
        }
        $size = $this->file->getSize();
        if ($size == 0) {
            return array('error' => 'File is empty');
        }
        if ($size > $this->sizeLimit) {
            return array('error' => 'File is too large (> ' . $this->toBytes($this->sizeLimit) . ')');
        }
        $pathinfo = pathinfo($this->file->getName());
        if ($_GET['filename'] != '' && strtolower(trim($_GET['filename'])) != 'undefined') {
            $filename = $_GET['filename'];
        } else {
            $filename = $pathinfo['filename'];
        }
        //$filename = md5(uniqid());
        $ext = strtolower($pathinfo['extension']);
        if ($this->allowedExtensions && !in_array(strtolower($ext), $this->allowedExtensions)) {
            $these = implode(', ', $this->allowedExtensions);
            return array('error' => 'File has an invalid extension, it should be one of ' . $these . '.');
        }
        if (!$replaceOldFile) {
            /// don't overwrite previous files that were uploaded
            while (file_exists($uploadDirectory . $filename . '.' . $ext)) {
                $filename .= rand(10, 99);
            }
        }
        //$this->file->save($uploadDirectory .$timestamp.'_'. $filename . '.' . $ext

        $filename = $timestamp . '_' . $filename . '.' . $ext;
        if(null !== $newFileName)
            $filename = $newFileName . '.' . $ext;

        $path = $uploadDirectory . $filename;
        if ($this->file->save($path)) {
            return array('success' => true, 'timestamp' => $timestamp, 'name' => $filename);
        } else {
            return array(
                'error' => 'Could not save uploaded file.' .
                    'The upload was cancelled, or server error encountered'
            );
        }
    }

}

// list of valid extensions, ex. array("jpeg", "xml", "bmp")
$allowedExtensions = array(
    "txt", "csv", "htm", "html",
    "doc", "docx", "xls", "xlsx", "rtf", "ppt", "pptx", "pdf", "swf", "flv", "avi",
    "wmv", "mov", "jpg", "jpeg", "gif", "png"
);
$allowedExtensions = array("txt", "xml", "doc", "docx", "xls", "xlsx", "rtf", "ppt", "pptx", "pdf", "jpg", "jpeg", "png", "gif", 'mp3');
// max file size in bytes
$sizeLimit = 100 * 1024 * 1024;
$uploader = new qqFileUploader($allowedExtensions, $sizeLimit);

if('insight-cover-image' === $_REQUEST['itype']) {
    $insightId = (int) $_REQUEST['pro_id'];
    $result = $uploader->handleUpload("../../../sites/default/images/insights/original/", true, $insightId);
    $result['url'] = "/sites/default/images/insights/original/" . $insightId . '.jpg';
}
elseif('company-cover-image' === $_REQUEST['itype']) {

    $companyId = (int) $_REQUEST['company_id'];
    foreach (glob(dirname(dirname(__FILE__)) . "/images/project/{$companyId}-tmp.*") as $f)
        unlink($f);

    $result = $uploader->handleUpload("../../../themes/maennaco/images/project/", true, $companyId . '-tmp');
    if($result['success']) {
        $result['url'] = "/themes/maennaco/images/project/" . $result['name'];
        list($result['width'], $result['height']) = getimagesize("../../../themes/maennaco/images/project/" . $result['name']);
    }
}
elseif('insight-audio-preview' === $_REQUEST['itype']) {
    $insightId = (int) $_REQUEST['pro_id'];
    $result = $uploader->handleUpload("../../../sites/default/files/events_tmp/insights/audio-previews/", true, $insightId);
}
elseif('discussion_audio' == $_REQUEST['itype']) {
    $file_name = time().'_'.$_REQUEST['name'];

    $result = $uploader->handleUpload("../../../sites/default/files/events_tmp/discussion/audio-previews/", true, $file_name);
}
else
    $result = $uploader->handleUpload("../../../sites/default/files/events_tmp/");

$offerId = (int) $_REQUEST['pro_id'];
if($offerId && $result['success']) {

    require_once __DIR__ . '/../../../lib/init.php';

    $notificationService = new Clewed\Notifications\NotificationService();
    $notificationService->registerEvent('offer_updated', $offerId, 0);
}

// to pass data through iframe you will need to encode all html tags
echo htmlspecialchars(json_encode($result), ENT_NOQUOTES);
