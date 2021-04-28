<?php

//$maenna_page['right']= array('upload_box','maenna_selection', 'recent_topics');

$maenna_page['right'] = array('info_tools', 'upload_box', 'maenna_selection', 'recent_topics', 'admin_approve');
global $redirect;
$redirect = rebuild_url(array('tab', 'page'));

function pro_DiscussionCalendar($op = null) {
  global $user;
  $editorid = $user->uid;
  $pid = sget($_REQUEST, 'id');
  $pro_id = sget($_REQUEST, 'pro_id');
  //$Block['title'] = 'Calendar';
  $redirect = rebuild_url(array('tab', 'page', 'id'));
  //if($op == 'write')$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>EDIT</a></div>";
  $content = '';

  if (empty($op))
    return '';

  ob_start();
  $url = "http://" . $_SERVER['HTTP_HOST'];
  $editorname = $user->name;
  require_once("pro_calendar_new.php");
  $content = ob_get_contents();
  ob_end_clean();

  //$content = sget($_REQUEST, 'file');

  $Block['body'] = content_box($Block['title'], $content);
  return $Block;
}

function upload_box($op = null) {
  global $user, $redirect;
  $editorid = $user->uid;
  $pid = sget($_REQUEST, 'pid');
  $Block['title'] = "<div style='padding:5px 0; text-align:center'><a href='$redirect&panel=upload_box_panel'>Add Information</a></div>";
  $content = '';
  $time = time();

  $Block['body'] = sidebar_box($Block['title'], $content);
  return $Block;
}

function upload_box_panel($op = null) {
  global $user, $redirect;
  $editorid = $user->uid;
  $pid = sget($_REQUEST, 'pid');
  $Block['title'] = 'Add Information';
  $content = '';
  $time = time();

  $view = sget($_REQUEST, 'view');
  $do = sget($_REQUEST, 'do');


  $options_topic = Options_infoTopic();
  $options_infoType = Options_infoType();
  $option_industry = Options_industry();


  if ($op && $op != 'update') {
    $content = <<< END
        <form action='/account' method=post enctype='multipart/form-data' >
        <table class='edit_table' style='width:400px;'>
            <tr><td style='width:100px;'>Topic</td><td><select name='topic' style='width:200px;' class='required'><option value=''></option>$options_topic</select></td></tr>
            <tr><td>Information Type</td><td><select name='infotype' style='width:200px;'  class='required'><option value=''></option>$options_infoType</select></td></tr>
            <tr><td>Industry</td><td><select name='industry' style='width:200px;'><option value=''></option>$option_industry</select></td></tr>
            <tr><td>Title</td><td><input type=text name=title value='' style='width:200px' /></td></tr>
            <tr><td colspan=2 style='text-align:left;padding-top:10px;'><span class=note>Please choose to upload a file or enter a web URL</span></td></tr>
            <tr><td><input type=radio name=inputtype value='file' /> File</td><td><input type=file name=file /></td></tr>
            <tr><td><input type=radio name=inputtype value='url' /> Web URL</td><td><input type=text name=url value=''  style='width:200px;' /></td></tr>
            <tr><td></td><td><input type=submit name=submit value='submit' class=button /></td></tr>
        </table>
            <input type=hidden name=tab value=information />
            <input type=hidden name=page value=insights />
            <input type=hidden name=update_section value=upload_box_panel />
            <input type=hidden name=panel value=upload_box_panel />
        </form>
END;
  } elseif ($op == 'update') {
    $DBKeys = array('topic', 'infotype', 'industry', 'inputtype', 'title', 'data', 'created', 'postedby', 'status');
    $DBValues = array();
    $SQL_STR = array();
    foreach ($DBKeys as $key) {
      $DBValues["$key"] = sget($_POST, $key);
      $SQL_STR[] = "'%s'";
    }
    extract($DBValues);
    $Correct = true;
    if (empty($topic) || empty($infotype)) {
      drupal_set_message('Please choose a topic and information type', 'error');
      $Correct = false;
    } elseif (empty($inputtype)) {
      drupal_set_message('Please choose if it is a file or an URL', 'error');
      $Correct = false;
    } elseif (empty($title)) {
      drupal_set_message('Please enter a title', 'error');
      $Correct = false;
    } elseif ($inputtype == 'file') {

      if (empty($_FILES['file']) || $_FILES['file']['error'] != UPLOAD_ERR_OK) {
        drupal_set_message('Please select a file to upload', 'error');
        $Correct = false;
      }
      $DBValues['data'] = sget($_FILES, 'name');
    } elseif ($inputtype == 'url') {
      if (empty($_POST['url'])) {
        drupal_set_message('Please enter a web URL', 'error');
        $Correct = false;
      } else {
        $DBValues['data'] = sget($_REQUEST, 'url');
      }
    }
    if (!$Correct)
      return;

    if ($inputtype == 'file') {
      $path = "./" . file_directory_path() . "/";
      $filename = "";
      if ($ext = get_filetype($_FILES["file"]["name"])) {
        $filename = ranStr() . "." . $ext;
        if (!move_uploaded_file($_FILES["file"]["tmp_name"], $path . $filename)) {
          drupal_set_message("Failed to upload new file", 'error');
          $Correct = false;
        }
      } else {
        drupal_set_message("The file type is not allowed", 'error');
        $Correct = false;
      }
      $DBValues['data'] = $filename;
    }
    if (!$Correct)
      return;

    $DBValues['created'] = $time;
    $DBValues['postedby'] = $editorid;
    $DBValues['status'] = 'pending';
    $sql = "insert into maenna_insights (" . implode(',', $DBKeys) . ") values (" . implode(',', $SQL_STR) . ") ";

    if (db_query($sql, $DBValues)) {
      drupal_set_message("The new information is added to database. It will be reviewed by Maenna Admin and listed in Insights section after approved");
    } else {
      drupal_set_message("Failed to upload information. Please try again later", 'error');
    }
  }

  $Block['body'] = sidebar_box($Block['title'], $content);
  return $Block;
}

function maenna_selection($op = null) {
  global $user, $redirect;
  $editorid = $user->uid;
  $pid = sget($_REQUEST, 'pid');
  $Block['title'] = 'Maenna Selections';
  $content = '';
  $time = time();
  $sql = "select * from maenna_insights where selection = 1 order by selectiontime desc limit 5";
  $result = db_query($sql);
  while (($Row = db_fetch_array($result))) {
    extract($Row);
    $title = htmlentities($title, ENT_QUOTES);
    if ($inputtype == 'file') {
      $data = "/" . file_directory_path() . "/" . $data;
      $title = "<a href='$data' target='_blank'>$title</a>";
    } elseif ($inputtype == 'url') {
      $title = "<a href='$data' target='_blank'>$title</a>";
    }
    $type = Options_infoType($infotype, 1);
    $content .= "\n<div class=row style='position:relative;padding:3px 0;width:100%;'>
            <div style='position:absolute;width:50%;right:0;text-align:right'>$type</div>
            <div style='width:60%;overflow:hidden'>$title</div>
        </div>";
  }
  if (!empty($content)) {
    $content .= text_link(array(
        array('label' => 'MORE &raquo;', 'url' => "$redirect&panel=insights_panel&section=selection")
            ), 'right');
  }
  $Block['body'] = sidebar_box($Block['title'], $content);
  return $Block;
}

function recent_topics($op = null) {
  global $user, $redirect;
  $editorid = $user->uid;
  $pid = sget($_REQUEST, 'pid');
  $Block['title'] = 'Most Recent';
  $content = '';
  $time = time();



  $Block['body'] = sidebar_box($Block['title'], $content);
  return $Block;
}

function admin_approve($op = null) {
  global $user, $redirect;
  $editorid = $user->uid;
  $pid = sget($_REQUEST, 'pid');
  $Block['title'] = 'Approve Information(admin only)';
  $content = '';
  $time = time();

  $sql = "select count(*) as cnt from maenna_insights where status = 'pending'";
  $result = db_query($sql);
  $Row = db_fetch_object($result);
  $count = 0;
  if ($Row->cnt)
    $count = $Row->cnt;
  $content = "<div class=row style='padding:2px 0;position:relative'><div style='position:absolute;width:100%;text-align:right'>$count</div>Pending Entries: </div>";

  if ($count)
    $content .= text_link(array(
        array('label' => 'edit &raquo;', 'url' => "$redirect&panel=admin_approve_panel")
            ), 'right');

  $Block['body'] = sidebar_box($Block['title'], $content);
  return $Block;
}

function admin_approve_panel($op = null) {
  global $user, $redirect;
  $editorid = $user->uid;
  $Block['title'] = 'Approve Information(admin only)';
  $content = '';
  $time = time();

  if ($op && $op != 'update') {
    $sql = "select * from maenna_insights where status = 'pending' order by created desc";
    $result = db_query($sql);
    while (($Row = db_fetch_array($result))) {
      extract($Row);
      $title = htmlentities($title, ENT_QUOTES);
      if ($inputtype == 'file') {
        $data = "/" . file_directory_path() . "/" . $data;
        $title = "<a href='$data' target='_blank'>$title</a>";
      } elseif ($inputtype == 'url') {
        $title = "<a href='$data' target='_blank'>$title</a>";
      }
      $topic = Options_infoTopic($topic, 1);
      $type = Options_infoType($infotype, 1);
      $industry = Options_industry($industry, 1);
      $created = date('m/d/Y', $created);
      $postedby = getProId($postedby);

      $option_rating = Options_rating($rating);
      $option_status = Options_status($status);
      $content .= "\n<tr>
                <td>$title</td>
                <td>$topic</td>
                <td>$type</td>
                <td>$industry</td>
                <td>$created</td>
                <td>$postedby</td>
                <td><select name=rating class='ajaxTrigger' table='maenna_insights'
                                                            idval='$insightid'
                                                            idname='insightid' 
                                                            column='rating'><option></option>$option_rating</select></td>
                <td><select name=status class='ajaxTrigger' table='maenna_insights'
                                                            idval='$insightid'
                                                            idname='insightid' 
                                                            column='status'>$option_status</select></td>
            </tr>";
    }
    $content = <<< END
    <div>
        <table  class='report'>
            <thead>
                <tr><td colspan=8 style='padding:4px; font:italic bold 14px arial;'>Contributed articles & research</td>
                <tr><td>Title</td>
                    <td style='width:120px;'><b>Topic</b></td>
                    <td style='width:120px;'><b>Type</b></td>
                    <td style='width:120px;'><b>Industry</b></td>
                    <td style='width:70px;'><b>Date</b></td>
                    <td style='width:80px;'><b>Source</b></td>
                    <td style='width:50px;'><b>Rating</b></td>
                    <td style='width:80px;'><b>Status</b></td>
                </tr>
            </thead>
            <tbody>
                $content
               
            </tbody>
        </table>
        <div class='pagenav'>
            <a href='$redirect&panel=admin_approve_panel' class=button>Refresh Page</a>&nbsp;&nbsp;
            <a href='$redirect' class=button>Back to insights</a>
        </div>
    </div>
    <script type='text/javascript'>
    $(document).ready(function(){init_ajaxTrigger();})
    </script>
END;
  } elseif ($op == 'update') {
    
  }

  $Block['body'] = sidebar_box($Block['title'], $content);
  return $Block;
}

/* EOF */