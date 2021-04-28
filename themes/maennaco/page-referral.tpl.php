<?php
// $Id: page.tpl.php,v 1.18.2.1 2009/04/30 00:13:31 goba Exp $

global $user;
if(isset($node) && in_array($node->nid, array(46,47)))
{
  
  $userRole = -1;
  $user_roles = array_keys($user->roles);
  
  //drupal_goto('account');
}

$submitted = sget($_REQUEST,'submit');
if($submitted == 'submit')
{
  $option  = sget($_REQUEST, 'program_option');
  $accept_policy = sget($_REQUEST,'accept_policy');
  $name =sget($_REQUEST,'name');
  $email =sget($_REQUEST,'email');
  $phone =sget($_REQUEST,'phone');
  $referral = check_plain(sget($_REQUEST,'referral'));
  
  $M = <<< HTML
  <table border=1 cellpadding=4 cellspacing=2>
    <tr><td>Program Option:</td><td>$option</td></tr>
    <tr><td>Accepted Policy:</td><td>Yes</td></tr>
    <tr><td>Name</td><td>$name</td></tr>
    <tr><td>Email</td><td>$email</td></tr>
    <tr><td>Phone</td><td>$phone</td></tr>
    <tr><td>Referrals:</td><td>$referral</td></tr>
  </table>
HTML;

  $Param = array('to'=>"neongeo@gmail.com", 'from'=>'info@clewed.com', 'message'=>$M);
  notify_user('new_referral', $Param);
  $Param = array('to'=>"hnega@clewed.com", 'from'=>'info@clewed.com', 'message'=>$M);
  notify_user('new_referral', $Param);
  $Param = array('to'=>"chief@pillarcc.com", 'from'=>'info@clewed.com', 'message'=>$M);
  notify_user('new_referral', $Param);
  drupal_goto("http://www.clewed.com/referral-form-submitted");
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
  <script type="text/javascript">
function check_referral_form() {
  var Correct = true;
  var M = ['program_option', 'accept_policy', 'name', 'email', 'phone', 'referral_box'];
  for(i = 0; i < M.length -1 ; i++)
  {
    var tId = M[i];
    if($("#" + tId).val() == ''){
      $("#" + tId).css("borderTopColor",'red').css("borderBottomColor",'red').css("borderLeftColor",'red').css("borderRightColor",'red');
      Correct = false;
    }else{
      $("#" + tId).css("borderTopColor",'#e8e8e8').css("borderBottomColor",'#e8e8e8').css("borderLeftColor",'#e8e8e8').css("borderRightColor",'#e8e8e8');
    }
  }
  if($('#accept_policy').attr('checked')){
    $('#accept_policy').css("borderTopColor",'#e8e8e8').css("borderBottomColor",'#e8e8e8').css("borderLeftColor",'#e8e8e8').css("borderRightColor",'#e8e8e8');
  }else{
    Correct = false;
    alert("Please read and accept Terms of Referral Program before submit");
  }
  if($('#referral_box').val() == '' || $('#').val() == "Please use this space to provide contact information for companies you would like to refer (company name, contact name, email, phone number)"){
    Correct = false;
    $('#referral_box').css("borderTopColor",'#e8e8e8').css("borderBottomColor",'#e8e8e8').css("borderLeftColor",'#e8e8e8').css("borderRightColor",'#e8e8e8');
  }else{
    $("#referral_box").css("borderTopColor",'red').css("borderBottomColor",'red').css("borderLeftColor",'red').css("borderRightColor",'red');
  }
  return Correct;
}
$(document).ready(function(){
  $('#referral_box').css('color','gray').bind('focus', function(){
    $(this).val("");
  }).bind('blur',function(){
    if($(this).val() == ''){
      $(this).val('Please use this space to provide contact information for companies you would like to refer (company name, contact name, email, phone number)');
    }
  });
});  
  </script>
  </head>
  <body<?php print phptemplate_body_class($left, $right); ?> style='font:12px arial'>

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
<?php  //echo "<div class='divider' style='margin-top:8px;'></div>"; ?> 
     

      <div id="center"><div id="squeeze" ><div class="right-corner"><div class="left-corner" style='padding:0;margin:0'>
<form method=post action="" onsubmit="return check_referral_form();">
          <?php if ($show_messages && $messages): print $messages; endif; ?>
          <div class="clear-block">
            <div class='page-content' >
              <h2 class="with-tabs" >CLEWED Referral Partner Program</h2>
              <div class="referral-content" style="margin-top:10px;line-height:18px;text-align:justify">
                <p>CLEWED's Referral Partners introduce CLEWED to prospective small to mid-size companies that are searching for an efficient system to help manage their growth and might benefit from CLEWED’s integrated advisory council solution. There's no cost for membership in the CLEWED Referral Partner Program. </p>
				<p>
				Referral Partners may choose to receive a referral fee of 10% of the first year’s subscription revenues or if desired, no payment. Many Referral Partners come from CLEWED’s Advisory Network Members or complementary service providers to prospective clients. CLEWED’s Advisory Network Members are selected for their commitment to the long-term success of our clients business, their expertise in our client’s business and the quality of their service. Network Members may qualify to serve as council members or participate in CLEWED’s “Special Projects” program and gain additional clients for their services.
                </p>
                <p>
                Please select your choice of option:
				</p>
                <select name="program_option" style='width:180px' id="program_option"><option></option>
                  <option>Cash Payment</option>
                  <option>No Payment</option>
                 
                </select>                        

                
                
                <h3 style='margin-top:15px;'>
                  <div style="width:200px;float:right;text-align:right;"><a href='http://www.clewed.com/REFERRAL-AGREEMENT' target='_blank' style="font-size:12px;font-weight:normal;">Printable version</a></div>
                  Terms of Referral Agreement</h3>
                 
                <div style="position:relative; margin-top:12px;width:100%;padding:16px;padding-left:0;">
                  <div style="height:160px;overflow-y:scroll;padding-right:10px;">
                    <?php $terms = node_load(72); echo nl2br($terms->body);?>
                  </div>
                </div>
                <div style='margin-top:8px;position:relative;'>
                 
                  <input type=checkbox name='accept_policy' id="accept_policy" value=1> I agree.  Please check the box to indicate your agreement to the terms and complete the form below to register.
                </div>
                <div style="margin-top:15px;">
                  <h3>Partner Contact</h3>
                  <table style="margin-top:12px;">
                    <tr><td style="width:40px;padding-right:15px;">Name:</td><td><input type=text name=name value="" id="name" /></td></tr>
                    <tr><td style="padding-right:15px;">Email:</td><td><input type=text name=email value=""  id="email" /></td></tr>
                    <tr><td style="padding-right:15px;">Phone</td><td><input type=text name=phone value="" id="phone" /></td></tr>
                    <tr><td colspan=2>I would like to refer the following companies:</td></tr>
                    <tr><td colspan=2><textarea name=referral style="width:600px;height:100px;" id="referral_box">Please use this space to provide contact information for companies you would like to refer (company name, contact name, email, phone number)</textarea></td></td></tr>
                  </table>
                  
                  <input type="image" name="submit" value="submit" id="edit-next"  class="form-submit" src="/themes/maennaco/images/submit.png" />
                  
                  
                </form>
                  
                </div>


                
              </div><!-- referral content -->
              
              
              
            </div>
            
          
        
        
          </div>
         
    
          
          <?php /*?><div id="footer">
              <div class="divider">&nbsp;</div>
             
              <?php if (isset($primary_links)) : ?>
                <div id="main-menu" class="text-13 navigation font-myriad <?php  if($is_front) print "fontpage" ;?>">
                  <?php print theme('links', $primary_links, array('class' => 'main-menu-links') ) ?>
                </div>
              <?php endif; ?>
            <?php if( ! $is_front){ ?>
            <div class="center-text clearfix text-13 font-myriad link-grey" style="padding-top:4px;">
              &copy; 2013 Clewed. All Rights Reserved.
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
	
	<div id="footer" style="padding-top:0;clear:both;">
                            <div class="divider">
							<?php if (false) : ?> // isset($primary_links) ovo je stajalo ranije
                               <!-- <div id="main-menu" class="text-12 navigation font-myriad"> 
                                    <?php print theme('links', $primary_links, array('class' => 'main-menu-links') ) ?>
                                </div>
                            <?php endif; ?>
                             <?php if( ! $is_front){ ?>
                                <div class="center-text clearfix text-12 font-myriad" style="padding-top:10px;color:#ffffff;">
                                &copy; 2013 Clewed. All Rights Reserved.
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
