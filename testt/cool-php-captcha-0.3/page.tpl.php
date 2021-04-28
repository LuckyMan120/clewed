<?php
// $Id: page.tpl.php,v 1.18.2.1 2009/04/30 00:13:31 goba Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
  <head>
    <?php print $head ?>
    <title><?php print $head_title ?></title>
    <?php print $styles ?>
    <?php print $scripts ?>
    <!--[if lt IE 7]>
      <?php print phptemplate_get_ie_styles(); ?>
    <![endif]-->
  </head>
  <body<?php print phptemplate_body_class($left, $right); ?>>

<!-- Layout -->
  <!--div id="header-region" class="clear-block"><?php print $header; ?></div-->

    <div id="wrapper" >
    <div id="container" class="clear-block">

      <div id="header">
         <?php
            global $user;
            if ( $user->uid ) { ?>
              <div style="width:540px;">
                <a href="/account"><img src="/<?php echo path_to_theme()  ;?>/logo_1.png" alt="website logo" border=0></a>
              </div>
            <?php } ?>
            <div id="user-status">
                <?php get_user_status($user); ?>
            </div>
        
        <?php if(! $is_front) get_topnavmenu(); ?>
      </div> <!-- /header -->
<?php  echo "<div class='divider' style='margin-top:15px;'></div>"; ?> 
      <?php if ($left): ?>
        <div id="sidebar-left" class="sidebar" >
          <?php if ($search_box): ?><div class="block block-theme"><?php print $search_box ?></div><?php endif; ?>
          <?php print $left ?>
        </div>
      <?php endif; ?>

      <div id="center"><div id="squeeze" ><div class="right-corner"><div class="left-corner" style='padding:0;margin:0'>
          <?php //print $breadcrumb; ?>
          <?php if ($show_messages && $messages): print $messages; endif; ?>
          <div class="clear-block">
            <?php
                if($is_front)  print $content;
                else{
                  echo "<div class='page-content' >";
                  
                  $title_color = ' nav-title';
                  if(isset($node)){
                    if(in_array($node->nid, array(44,45) )) { get_people_menu(1);  $title_color = '';}
                    elseif(in_array($node->nid, array(41,42,43,,49))) { get_people_menu(2); $title_color = '';}
                    elseif(in_array($node->nid, array(37,38,39,40))) { get_people_menu(3); $title_color = '';}
                    elseif(in_array($node->nid, array(46,47))) { get_people_menu(4); $title_color = '';}
                  }
                  if ($title && ! in_array($node->nid, array(41,45,47)))
                      { print '<h2'. ($tabs ? ' class="with-tabs ' . $title_color. '" ' : '') .'  >'. $title .'</h2>'; }
                  print $content;
                  echo "</div>";
                }
            ?>
          </div>
          <?php print $feed_icons ?>
          <?php if ( $user->uid ) { ?>
          
          <div id="footer">
              <div class="divider">&nbsp;</div>
             
              <?php if (isset($primary_links)) : ?>
                <div id="main-menu" class="text-13 navigation font-myriad <?php  if($is_front) print "fontpage" ;?>">
                  <?php print theme('links', $primary_links, array('class' => 'main-menu-links') ) ?>
                </div>
              <?php endif; ?>
            <?php if( ! $is_front){ ?>
            <div class="center-text clearfix text-13 font-myriad link-grey" style="padding-top:4px;">
              &copy; 2011 MaennaCo. All Rights Reserved.
            </div>
            <?php  } // user->uid?>
            <?php } ?>
          </div>
      </div></div></div></div> <!-- /.left-corner, /.right-corner, /#squeeze, /#center -->

      <?php if ($right): ?>
        <div id="sidebar-right" class="sidebar">
         
          <?php if (!$left && $search_box): ?><div class="block block-theme"><?php print $search_box ?></div>
        
          <?php endif; ?>
          <?php print $right ?>
        </div>
      <?php endif; ?>

    </div> <!-- /container -->
  </div>
<!-- /layout -->

  <?php print $closure ?>
  </body>
</html>
