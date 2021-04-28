<?php

// $Maenna_page->setBeforeContent(pro_DiscussionTopFilters());

function pro_DiscussionMyInsights($op = null) {
  global $user;
  $editorid = $user->uid;
  $pid = sget($_REQUEST, 'id');
  $pro_id = sget($_REQUEST, 'pro_id');
//    $Block['title'] = 'Calendar';
  $redirect = rebuild_url(array('tab', 'page', 'id'));
  //if($op == 'write')$Block['title'] .= "<div class=editbtn><a href='$redirect&panel=${panel}&view=edit' class=tool>EDIT</a></div>";
  $content = '';

  if (empty($op))
    return '';

  ob_start();
  $url = "http://" . $_SERVER['HTTP_HOST'];
  $editorname = $user->name;
  require_once("pro_myinsights_block.php");
  $content = ob_get_contents();
  ob_end_clean();

  //$content = sget($_REQUEST, 'file');

  $Block['body'] = content_box($Block['title'], $content);
  return $Block;
}

function pro_DiscussionTopFilters($op = NULL) {
  ob_start();

  require_once('pro_myinsights_top_filter.php');

  $content = ob_get_contents();

  ob_end_clean();
  return $content;
}