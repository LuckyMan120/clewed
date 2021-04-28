<?php
include('dbcon.php');
error_reporting(0);

if ($_REQUEST['type'] == 'edit') {

    $db = \Clewed\Db::get_instance();


    if ($_REQUEST['display'] == 'true') {

        function timerFormat($start_time, $end_time)
        {
            $results = array();
            $total_time = $end_time - $start_time;
            $results['days']       = floor($total_time /86400);
            $results['hours']     = floor($total_time /3600);
            $results['minutes']   = intval(($total_time/60) % 60);
            $results['seconds']   = intval($total_time % 60);

            return $results;
        }

        $serviceId = (int) $_REQUEST['eventid'];
        $uid = (int) $_POST['uid'];
        $time = (int) $_POST['time'];
        $hash = $_POST['hash'];
        $computedHash = md5($serviceId . ':' . $uid . ':' . $time . ':kyarata75');

        if ($hash !== $computedHash)
            die();

        $companyService = new Clewed\Company\Service();
        $services = $companyService->getServices(array($serviceId));
        $service = $services[$serviceId];

        $user = new Clewed\User\Service();
        $usrType = $user->getUserType($uid);

        $sql2 = "
              SELECT 
                  mcd.*,
                  CASE mp.firstname IS NULL WHEN TRUE THEN mc.projname ELSE mp.firstname END as uploaded_by,
                  CASE mp.pid IS NULL AND mc.companyid IS NULL WHEN TRUE THEN 1 ELSE 0 END as uploaded_by_clewed
              FROM maenna_company_data mcd
              LEFT JOIN maenna_people mp ON mp.pid = mcd.editorid 
              LEFT JOIN maenna_company mc ON mc.companyid = mcd.editorid 
              WHERE (mcd.deleted <> 1 || (mcd.deleted = 1 && (UNIX_TIMESTAMP(NOW()) - mcd.access) < (31*24*60*60) ))
              AND mcd.data_type = 'service-file' 
              AND mcd.data_value6 = ?
              ORDER BY mcd.dataid DESC";
        $file = $db->get_array($sql2, array($_REQUEST['eventid']));
        ?>

        <div style="clear:both;"></div>
        <style>
            #editEventForm div strong {
                font-family: 'LatoRegular', serif; }
        </style>
        <form action="" method="post" class="edit-service-form edit-service-files-form">
            <div id='evFileEdit' class="service-files-uploader-container" style='text-align:left;' title="File names can not contain odd characters or symbols">
                <br>
                <input style="width:100%!important;background:none;float:left;"
                       type=text
                       placeholder="Click Upload to select file to add"
                       title="File names can not contain odd characters or symbols"
                       id="fileTitleEdit"/>

                <div id="file-uploader1">
                    <noscript>
                        <p>Please enable JavaScript to use file uploader.</p>
                        <!-- or put a simple form for upload here -->
                    </noscript>

                </div>
                <!--<a style="pointer:cursor; float:left; padding:0;" class='hidebox button' boxid=evFile>CLOSE</a>-->
            </div>
            <div style="float: left;width: 100%;font-size: 12px;line-height: 15px;" title="File names can not contain odd characters or symbols">
                <p style="margin-bottom:11px !important; font-weight:bold;">Previous files:</p>
                <?php
                foreach ($file as $files) {

                    if ($usrType != 'admin' && $files['deleted'] == 1) continue;
                    if ($usrType != 'admin' && $files['editorid'] != $uid) continue;



                    $stripfilename = preg_replace('/^\d+_/', '', $files['data_value2']);
                    if (strlen($stripfilename) > 40)
                        $stripfilename = substr($stripfilename, 0, 40) . '...';

                    $uploadedOn = date('m/d/y', $files['access']);
                    $uploadedBy = $files['uploaded_by'];
                    if($files['uploaded_by_clewed'])
                        $uploadedBy = 'Clewed';

                    echo "<div id=\"file{$files['dataid']}\">"
                        . "<div style='width:240px;float:left;'>{$stripfilename}</div>"
                        . "<div style='width:150px;float:left;margin-left:10px;'>uploaded by {$uploadedBy}</div>"
                        . "<div style='width:90px;float:left;margin-left:10px;'>{$uploadedOn}</div>";

                    $timestamp = time();
                    $access = $files['access'];
                    $diff = timerFormat($access,$timestamp);

                    if (($uid == $files['editorid'] && $diff['hours'] < 4) || $usrType == 'admin' )
                        echo "<a id=\"rmFile" . $files['dataid'] . "\" data-tooltip = \"To avoid delays and disruption, removing files is restricted for error correction purposes within 3 hours of your upload. Use the service discussion tab to request clewed to remove files if you need to do so after such period\" class=\"small-remove-link\">Remove</a>";
                    else echo "<a class='small-remove-link'></a>";
                    if ($files['deleted'] == 1) echo "<a id='unDelFile" . $files['dataid'] . "' style='margin-left:15px;color:red !important;' class=\"small-remove-link\" data-tooltip='This file is deleted. It will remain visible for admin for one month after uploading.'>Deleted</a></div>"; else echo "</div>";
                }
                ?>
            </div>
        </form>

        <?php die();

    } else {

        $serviceId = (int) $_REQUEST['eventid'];
        $uid = (int) $_POST['uid'];
        $time = (int) $_POST['time'];
        $hash = $_POST['hash'];
        $computedHash = md5($serviceId . ':' . $uid . ':' . $time . ':kyarata75');

        if ($hash !== $computedHash)
            die();

        $companyService = new Clewed\Company\Service();
        $services = $companyService->getServices(array($serviceId));
        $service = $services[$serviceId];

        $isApproved = $service['approved'];
        $notificationService = new Clewed\Notifications\NotificationService();

        foreach ($_REQUEST['files'] as $value) {

            $fileName = $value['path'];
            if ($value['title'] == '')
                $value['title'] = substr($value['path'], 0, strrpos($value['path'], '.'));

            if(is_readable(ROOT . 'sites/default/files/events_tmp/' . $fileName))
                rename(ROOT . 'sites/default/files/events_tmp/' . $fileName, ROOT . 'sites/default/files/' . $fileName) or die('problem');

            $sql = "
                INSERT INTO maenna_company_data (companyid, access, data_type,data_value, data_value2,data_value6, editorid )
	            VALUES(
	                '" . $_REQUEST['cid'] . "',
	                '" . time() . "',
	                'service-file',
	                '" . $value['title'] . "',
	                '" . $fileName . "',
	                '" . $_REQUEST['eventid'] . "',
	                " . $_REQUEST['uid'] . "
                )";

            mysql_query($sql) or die(mysql_error());

            if($isApproved)
                $notificationService->registerEvent(
                    'company_service_file_added',
                    (int) $_REQUEST['eventid'],
                    (int) $_REQUEST['uid'],
                    array(
                        'id' => mysql_insert_id(),
                        'fileName' => $fileName,
                        'authorId' => (int) $_REQUEST['uid']
                    )
                );
        }
    }
}

