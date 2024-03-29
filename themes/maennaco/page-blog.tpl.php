<?php
// $Id: page.tpl.php,v 1.18.2.1 2009/04/30 00:13:31 goba Exp $

global $user;
if(isset($node) && in_array($node->nid, array(46,47)))
{
  
  $userRole = -1;
  $user_roles = array_keys($user->roles);
  if(is_array($user_roles))
  {
    foreach($user_roles as $K)
    {
      if(in_array($K, array(1,3,4,5,7)) && sget($_SESSION,'investor_access') != 1) {
        $_SESSION['page_history'] = "investors";
        drupal_goto('account');
      }
    }
  }
  //drupal_goto('account');
}
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

      <div id="header" >
              <div style="width:280px;position:relative;" >
                <a href="/account"><img src="/<?php echo path_to_theme()  ;?>/images/index_logo.png" alt="website logo" border=0 width="274"></a>
                <!--<div style="position:absolute;top:-9px; left:254px; z-index:10"><img src="/<?php echo path_to_theme()  ;?>/images/beta.png" width='22'></div>-->
              </div>
         
            <div id="user-status">
                <?php get_user_status($user); ?>
            </div>
        
        <?php if(! $is_front) get_topnavmenu(); ?>
      </div> <!-- /header -->
<?php  echo "<div class='divider' style='margin-top:8px;'></div>"; ?> 
     

      <div id="center"><div id="squeeze" ><div class="right-corner"><div class="left-corner" style='padding:0;margin:0'>

          <?php if ($show_messages && $messages): print $messages; endif; ?>
          <div class="clear-block">
            <?php
               
                  echo "<div class='blog-content' ><table cellspacing=0 cellpadding=0 border=0 width='100%'><tr><td class='left-box'>";
                  
                  //$title_color = ' nav-title';
                  $title_color = '';
                  
                  if ($title && ! in_array($node->nid, array(41,45,47)) )
                    {
                       // print '<h2'. /*($tabs ? ' class="with-tabs ' . $title_color. '" ' : '') . */' style="color:#666;margin-top:-20px;">'. /*$title*/ "" .'</h2><br>';
                    }
                  print $content;
                  echo "</div>";
               
            ?>
            </td>
            <td class="right-box">
        
        <div class="box">
                <h3>Categories</h3>
                <?php
                      $result = db_query("SELECT term_data.tid, term_data.name, COUNT(*) AS count FROM {vocabulary_node_types} INNER JOIN  {term_data} USING (vid) INNER JOIN {term_node} USING (tid) INNER JOIN {node} USING (nid) WHERE node.status = 1 and vocabulary_node_types.type = 'blog' GROUP BY term_data.tid, term_data.name ORDER BY term_data.weight");
                      $items = array();
                      while ($category = db_fetch_object($result)) {
                       
                        $items[] = l($category->name .' ('. $category->count .')', 'taxonomy/term/'. $category->tid);
                      }
                        
                      echo  "<div class='innerbox'>" . theme('item_list', $items) . "</div>";
                      
                  ?>
        </div>
                <?php print $feed_icons ?>
            </td>
            </table>
          </div>
          
    
          
          <div id="footer">
              <div class="divider">&nbsp;</div>
             
              <?php if (isset($primary_links)) : ?>
                <div id="main-menu" class="text-13 navigation font-myriad <?php  if($is_front) print "fontpage" ;?>">
                  <?php print theme('links', $primary_links, array('class' => 'main-menu-links') ) ?>
                </div>
              <?php endif; ?>
            <?php if( ! $is_front){ ?>
            <div class="center-text clearfix text-13 font-myriad link-grey" style="padding-top:4px;">
              &copy; 2013 clewed. All Rights Reserved.
            </div>
            <?php  } // user->uid?>
         
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
    <div id='popup_layer' class='hide'>
        <div id="dialog" style="padding:20px;">
          <p>
            
            <?php
              $terms_node =  node_load(11);
              echo $terms_node->body;
            /*
                $array = array(
                    array('src' => 'companies.png', 'href' =>'/company-apply-form', 'alt'=>'companies'),
                    array('src' => 'professionals.png', 'href' =>'/people-apply-form', 'alt'=>'professionals'),  
                );
                button_links($array);
            */
            ?>
          </p>
        </div>
    </div>
    <script type="text/javascript">

  var _gaq = _gaq || [];
  _gaq.push(['_setAccount', 'UA-23393148-1']);
  _gaq.push(['_trackPageview']);

  (function() {
    var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
  })();

</script>
  </body>
</html>
