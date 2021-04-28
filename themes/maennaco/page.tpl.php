<?php
// $Id: page.tpl.php,v 1.18.2.1 2009/04/30 00:13:31 goba Exp $

global $user;
global $base_url;
$urlid = explode("/",$_SERVER['REQUEST_URI']);

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
if($urlid[3] == 'edit') {
		//echo $head_title;
		$head2 = explode("|",$head_title);
$userid2 = str_replace("MU_","",$head2[0]);
		//echo "SELECT * FROM maenna_people where pid=$userid";
		$result = mysql_query("SELECT * FROM maenna_people where pid=".$userid2."");
		$role = mysql_fetch_assoc($result);
		$head_title22 = $role['firstname']." ".$role['lastname'];
		$title = $head_title22;
		//$content = '';
}
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
  <head>
    <?php print $head ?>
   <title><?php if($_REQUEST['q'] == 'user/password') { echo "User Password Reset"; }
	elseif($urlid[3] == 'edit') {
		echo $head_title22;
	}
	else print $head_title ?></title>
    <?php print $styles ?>
    <?php print $scripts ?>
    <!--[if lt IE 7]>
      <?php print phptemplate_get_ie_styles(); ?>
    <![endif]-->
      <style type="text/css">
          .page-wrapper {
              height: 100%;
          }

          .page-container {
              box-sizing: border-box;
              min-height: 100%;
              padding-bottom: 80px;
          }

          .page-footer {
              margin-top: -86px !important;
          }
      </style>
  </head>
  <body<?php print phptemplate_body_class($left, $right); ?>>

<!-- Layout -->
  <!--div id="header-region" class="clear-block"><?php print $header; ?></div-->

    <div id="wrapper" class="page-wrapper">
    <div id="container" class="clear-block page-container">

      <div id="header" style="padding: 3px 0px 17px 0px;">
              <div style="width:220px;position:relative; padding-top:12px;" >
                <a href="/account"><img src="<?php echo $base_url; ?>/themes/maennaco/images/index_logo.png" alt="website logo" border=0></a>
                <!--<div style="position:absolute;top:-9px; left:254px; z-index:10"><img src="/<?php echo path_to_theme()  ;?>/images/beta.png" width='22'></div>-->
              </div>
         
            <div id="user-status">
                <?php get_user_status($user); ?>
            </div>
        
        <?php if(false) get_topnavmenu(); ?> 
      </div> <!-- /header -->
<?php  //echo "<div class='divider' style='margin-top:10px;'></div>"; ?>  
     

      <div id="center"><div id="squeeze" ><div class="right-corner"><div class="left-corner" style='padding:0;margin:0'>
          <?php if ($show_messages && $messages): print $messages; endif; ?>
          <div class="clear-block">
            <?php
               
                  echo "<div class='page-content' >";
                  
                  //$title_color = ' nav-title';
                  $title_color = '';
                  $tab_box = false;
                  if(isset($node)){
                    if(in_array($node->nid, array(44,45) )) { get_people_menu(1);  $title_color = '';$tab_box = 1;}
                    elseif(in_array($node->nid, array(41,42,43,49,64))) { get_people_menu(2); $title_color = '';$tab_box = 1;}
                    elseif(in_array($node->nid, array(37,38,39,40,50))) { get_people_menu(3); $title_color = '';$tab_box = 1;}
                    elseif(in_array($node->nid, array(46,47))) { get_people_menu(4); $title_color = '';$tab_box = 1;}
                    elseif($node->nid == 65){
                      $n = node_load(54); echo $n->body ."<br><br>";
                    }
                  }
                  if($tab_box) echo "<div class='tab-box'>";
                  if ($title && ! in_array($node->nid, array(41,45,47)))
                      { print '<h2'. /*($tabs ? ' class="with-tabs ' . $title_color. '" ' : '') . */'  >'. $title .'</h2>'; }
                  print $content;
                  if($tab_box)echo "</div>";
                  echo "</div>";
               
            ?>
          </div>
          <?php print $feed_icons ?>
    
          
          <?php /*?><div id="footer">
              <div class="divider">&nbsp;</div>
             
              <?php if (false) : ?> 
                <div id="main-menu" class="text-13 navigation font-myriad <?php  if($is_front) print "fontpage" ;?>">
                  <?php print theme('links', $primary_links, array('class' => 'main-menu-links') ) ?>
                </div>
              <?php endif; ?>
            <?php if( ! $is_front){ ?>
            <div class="center-text clearfix text-13 font-myriad link-grey" style="padding-top:4px;">
              &copy; 2013 clewed. All Rights Reserved.
            </div>
            <?php  } // user->uid?>
         
          </div><?php */?>
      </div></div></div></div> <!-- /.left-corner, /.right-corner, /#squeeze, /#center -->

      <?php if ($right): ?>
        <div id="sidebar-right" class="sidebar">
         
          <?php if (!$left && $search_box): ?><div class="block block-theme"><?php print $search_box ?></div>
        
          <?php endif; ?>
          <?php print $right ?>
        </div>
      <?php endif; ?>

    </div> <!-- /container -->
	<div id="footer" style="padding-top:20px;clear:both;" class="page-footer">
                            <div class="divider">
							<?php if (false) : ?> // isset($primary_links) ovo je stajalo ranije
                               <!-- <div id="main-menu" class="text-12 navigation font-myriad"> 
                                    <?php print theme('links', $primary_links, array('class' => 'main-menu-links') ) ?>
                                </div>
                            <?php endif; ?>
                             <?php if( ! $is_front){ ?>
                                <?php require_once("footer-maennaco.php");?>
                            </div>
                            <?php } ?>
							
							</div>
                           
                            
                        </div><!-- footer -->
                        
                    </div> </div>
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
<!--    <script type="text/javascript">

//  var _gaq = _gaq || [];
//  _gaq.push(['_setAccount', 'UA-23393148-1']);
//  _gaq.push(['_trackPageview']);
//
//  (function() {
//    var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
//    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
//    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
//  })();

</script>-->
<script>
  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
  })(window,document,'script','//www.google-analytics.com/analytics.js','ga');

  ga('create', 'UA-43665911-1', 'clewed.com');
  ga('send', 'pageview');

</script>
  </body>
</html>
