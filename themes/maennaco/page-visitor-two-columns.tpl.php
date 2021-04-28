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
        <script type="text/javascript" src="/<?php global $theme_path; echo $theme_path?>/forms.js"></script>
    </head>
    <body<?php print phptemplate_body_class($left, $right); ?>>
        <div id="wrapper">
            <div id="container" class="clear-block"> 
                <div id="header" >
                    <div style="width:540px;">
                        <a href="/account"><img src="/<?php echo path_to_theme()  ;?>/logo_1.png" alt="website logo" border=0></a>
                    </div>
                    <div id="user-status">
                        <?php global $user; get_user_status($user); ?>
                    </div>
                    <?php if(! $is_front) get_topnavmenu(); ?>
                </div> <!-- /header -->
                <div class='divider' style='margin-top:15px;'></div>
                
                <div id="center" >
                    <div id="squeeze">
                        <?php
                                if(isset($node)){
                                    if(in_array($node->nid, array(15,16,17) )) get_people_menu(1);
                                    elseif(in_array($node->nid, array(8,13))) get_people_menu(2);
                                    elseif(in_array($node->nid, array(25, 27,29, 30,26,35))) get_people_menu(3);
                                }
                        ?>
                        <div class='visitor-content-box' style="margin-top:0;">
                            
                            
                            <table cellpadding=0 cellspacing=0 border=0 class="two-column-tbl">
                                <tr>
                                    <td class="left-td"  >
                                        <div style="width:95%;padding-right:5%;">
                                            <?php if ($title): ?>
                                                <h1 class="title" id="page-title">
                                                    <?php if ($title): print '<h2'. ($tabs ? ' class="with-tabs"' : '') .'>'. $title .'</h2>'; endif; ?>
                                                </h1>
                                            <?php endif; ?>
                                            <?php print $content ?>
                                        </div>
                                    </td>
                                    <td
                                        <?php
                                            if(in_array($node->nid, array(27,25,29))) echo 'style="padding:0;"';
                                            else{ echo 'style="background:url(/themes/maennaco/images/bbbg.jpg) repeat;padding:0;padding-top:30px;"';}
                                        ?>
                                        class="right-td"
                                    >
                                        <div <?php if(in_array($node->nid, array(35))) echo 'style="width:100%;margin:0 auto;margin-top:-8px;padding;0;background:transparent;"'; else echo 'style="width:90%;margin:0 auto;" ';  ?> >
                                            <?php if ($messages): ?>
                                                <div id="messages"><div class="section clearfix">
                                                  <?php print $messages; ?>
                                                </div></div> <!-- /.section, /#messages -->
                                            <?php endif; ?>
                                            <?php
                                            
                                                    $node_array = array(8, 10,13,14,15,16,17);
                                                    if (  isset($node) && in_array($node->nid, $node_array ))
                                                    {
                                                        //echo $node->nid;
                                                        switch($node->nid)
                                                        {
                                                            case 13: case 8:
                                                                $form  = drupal_get_form('maenna_forms_company_signup_form');
                                                                break;
                                                            case 15: case 16: case 17: case 14:
                                                            
                                                                $form = drupal_get_form('maenna_forms_pro_form');
                                                                break;
                                                            case 10:
                                                                $form = drupal_get_form('maenna_forms_investor_form');
                                                                break;
                                                        }
                                                        echo  theme_status_messages();
                                                        echo $form;
                                                       
                                                    }elseif(isset($node) && $node->nid == 35)
                                                    {
                                                        $maenna_block = node_load(31);
                                                        echo $maenna_block->body;
                                                    }
                                                    elseif(isset($node) && $node->nid == 26)
                                                    {
                                                        $maenna_block = node_load(36);
                                                        echo $maenna_block->body;
                                                    }
                                                    elseif(isset($node) && $node->nid == 25)
                                                    {
                                                        $maenna_block = node_load(36);
                                                        //echo $maenna_block->body;
                                                    }
                                                    else
                                                    {
                                            ?>
                        
                                                        
                                                        <!--h3>Collaborative platform for talent</h3>
                                                        <p>
                                                          If you have a passion for business with an intimate knowledge of industry sectors, markets and the operational challenges shared by businesses, Maenna is the right partner for you. We offer a convenient vehicle for you to share information, analyze, refer, and advise leading smaller companies and share in their upside. Learn more through the <b><a href="/people">People</a> section.</b> 
                                                        </p-->
                                            <?php   } ?>
                                            <span class='clear-fix'></span>
                                        </div>
                                    </td>
                                </tr>
                            </table>
                            
                
                        </div><!-- visitor-content-box -->
                        
                        <div id="footer" style="padding-top:0;clear:both;">
                            <div class="divider">&nbsp;</div>
                           
                            <?php if (isset($primary_links)) : ?>
                                <div id="main-menu" class="text-13 navigation font-myriad">
                                 <?php print theme('links', $primary_links, array('class' => 'main-menu-links') ) ?>
                                   </div>
                            <?php endif; ?>
                             <?php if( ! $is_front){ ?>
                                <div class="center-text clearfix text-13 font-myriad link-grey" style="padding-top:4px;">
                                &copy; 2011 ClewedCo. All Rights Reserved.
                            </div>
                            <?php } ?>
                        </div><!-- footer -->
                        
                    </div><!-- squeeze -->
                </div><!-- center -->
            </div><!-- container -->
        </div><!-- wrapper -->
    </body>
</html>
