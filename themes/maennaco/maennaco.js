$(document).ready(function(){

  //init_disableClickedButtons();
  init_datepicker();
  init_image_rollover();
  init_collapsible();
  init_overlay();
  init_td_hilightinput();
  init_editwnd();
  init_inputLength();
  init_researchEdit();
  inti_table_collapse();
  init_morediv();
  init_tinymce();
  init_home_video();
  init_number_format_profile_form();
  init_percent_format_profile_form();
  init_decimal_format_profile_form();
  init_featured_insight_title();
  init_featured_company_mission();
  init_company_cover_image_uploader();

  init_tooltips();
  init_fundraising_administration();
  init_show_more();
});
function init_show_more() {
  $('a.collapse-icon').click(function (e) {
      e.preventDefault();
      var me = this;
      var toToggle = $($(this).data("for"));
      if (toToggle) {
        toToggle.toggle(400, function() {
          const sign = toToggle.is(':visible')  ? '&#8211;' : '+';
          me.innerHTML = sign;
        });
      }
  });
}
function init_number_format_profile_form() {
    $('input[data-type="profile_form_number"]').blur(function () {
        $(this).formatCurrency({roundToDecimalPlace: -2, eventOnDecimalsEntered: true});
    }).bind('decimalsEntered', function (e, cents) {
        var errorMsg = 'Please do not enter any cents (.' + cents + ')';
        alert(errorMsg);
    });
}

function init_decimal_format_profile_form() {
    $('input[data-type="profile_form_decimal"]').blur(function () {
        $(this).formatCurrency({roundToDecimalPlace: 2, eventOnDecimalsEntered: true});
    }).bind('decimalsEntered', function (e, cents) {
        var errorMsg = 'Please use three digits after comma.';
        alert(errorMsg);
    });
}

function init_percent_format_profile_form() {
    $('input[data-type="percent_form_number"]').blur(function () {
        $(this).formatCurrency({symbol: '%',positiveFormat: '%n%s', roundToDecimalPlace: 2, eventOnDecimalsEntered: true});
    }).bind('decimalsEntered', function (e, cents) {
        var errorMsg = 'Please use two digits after comma.';
        alert(errorMsg);
    });
}

function calculateOverall() {
  var overall = 0;
  var i = 0;
  $(".individual").each(function() {
    currValue = $(this).rateit("value");
    overall = overall + currValue;
    if (currValue != 0) i++;
  });
  return parseFloat(overall/i).toFixed(2);
}

function init_payment_details () {

  $(".payment_details").click(function(ev) {

    ev.preventDefault();
    thisObj = $(this);
        utype = thisObj.data('utype');

    $.post("/themes/maennaco/includes/fetch.php?type=getPayments&utype="+utype, {
      ins_id: thisObj.data('ins_id'),
      m: thisObj.data('hash'),
      user_type: thisObj.data('user-type'),
      uid: thisObj.data('uid')
    },function(response){
      if (response != "false") {

        $("#payment_details_dialog").html(response);
        $("#payment_details_dialog").dialog("open");

      }

      else alert("Authentication problem!");

    });

  });
}

function init_payment_details_dialog() {

  $('#payment_details_dialog').dialog({
    width: 850,
    title: "Payment Summary",
    height: 500,
    modal:true,
    autoOpen: false,
    close: function() {
      $(this).dialog("close");
    }
  });

}

function init_pro_participations_dialog(title) {
  $('#pro-participations-dialog').dialog({
    width: 480,
    title: title,
    modal:true,
    autoOpen: false,
    close: function() {
      $(this).dialog("close");
    }
  });
}

function init_pro_participations () {
    $(".pro-participations").click(function(ev) {
        ev.preventDefault();
        $("#pro-participations-dialog").dialog("open");
    });
}

function init_rate() {
  /*$("img.rate_user").click(function(e) {
   if($(this).data('rate') == '1')
   {
   $("#rate_user_dialog").find('div').css('background-image','url(/themes/maennaco/images/th_up.png)');
   $("#rate_user_dialog").find('#rate_type').val('1');
   }
   else {

   $("#rate_user_dialog").find('div').css('background-image','url(/themes/maennaco/images/th_down.png)');
   $("#rate_user_dialog").find('#rate_type').val('0');
   }
   $("#rate_user_dialog").dialog('open');

   });*/

  $(".add_review").click(function(e) {

    $("#rate_user_dialog").dialog("open");

  });
  $('.rateit').bind('rated reset', function (e) {
    var ri = $(this);


    //if the use pressed reset, it will get value: 0 (to be compatible with the HTML range control), we could check if e.type == 'reset', and then set the value to  null .
    var value = ri.rateit('value');


    $("#rate_user_dialog").find('#rate_'+ri.data("rate-type")).val(value);

    $(".overall").rateit('value', calculateOverall());
  });

  $(".rateit").mouseleave(function() {

    $('.rate_hint').html('Roll over stars. Then click to rate.');
  });

  $(".overall").mouseenter(function() {

    $(this).attr('title',$(this).rateit("value")+" star rating");

  });

  $(".rateit").bind('over', function (event,value) {

    var hint = '';
    switch(value) {
      case 0:
        hint = 'Roll over stars. Click to rate.';
        break;
      case 1:
        hint = 'Not good.';
        break;
      case 2:
        hint = 'Can be better.';
        break;
      case 3:
        hint = 'A - OK. ';
        break;
      case 4:
        hint = 'Great.';
        break;
      case 5:
        hint = 'Awesome.';
        break;
    }
    $('.rate_hint').html(hint);

  });


  $(".get_reviews").click(function(e) {

    thisObj = $(this);

    $.post("/themes/maennaco/includes/fetch.php?type=getReviews", {
      uid: thisObj.data('uid'),
      user: thisObj.data('user')

    },function(response){

      if (response != "fail") {

        $("#get_reviews_dialog").html(response);
        $("#get_reviews_dialog").dialog("open");
        $(".rateit_rates").rateit();

      }

    });
  });
}

function init_home_video(){

  var video = "<iframe width=\"800\" height=\"450\" src=\"//www.youtube-nocookie.com/embed/LB57oHQ8XtU?rel=0\" frameborder=\"0\" allowfullscreen></iframe>";

  $('#watch-video-link').click(function(e){
    e.preventDefault();
    $('#home-video').html(video);
    $('#home-video').dialog({
      width: 'auto',
      height: 514,
      modal:true,
      dialogClass: 'home-video-dialog',
      close: function() {
        $('#home-video').html("");
      }
    });
  });
}

function init_disableClickedButtons() {

  $('input[name=submit]').click(function() {
    $(this).hide('slow');
    //$(this).parents('form').submit();
  })

}
function init_morediv(){
  $('.morediv').each(function(){
    var pixelPerLine = 18;
    var numOfLines = $(this).attr('lines');
    if(numOfLines > 0){
      height = $(this).height();
      $(this).attr('trueH',height);
      pixels = pixelPerLine * numOfLines;
      $(this).attr('sH',pixels);

      $(this).css('height', pixels).css('overflow','hidden');
    }
  });
  $('.morediv_link').bind('click',function(evt){
    evt.preventDefault();
    if($(this).attr('closed') == '1'){
      H = $(this).parent().prev().attr('trueH');
      $(this).parent().prev().animate({height: H });
      $(this).html("Close").attr('closed', '0');
    }else{
      sH = $(this).parent().prev().attr('sH');
      $(this).parent().prev().animate({height: sH });
      $(this).html("More >>").attr('closed', '1');
    }
  })
}
function init_inputLength(){
  $(":text").each(function(){
    var len = $(this).attr('strlen');
    if(typeof(len) != 'undefined')
    {
      $(this).keyup(function(evt){
        if($(this).val().length > $(this).attr('strlen'))
        {
          var text = $(this).val();
          var limit = $(this).attr('strlen');
          text = text.substring(0, limit);
          $(this).val(text);
          alert("Max input length reached");

        }
      })
    }
  })
}
var sel_obj = {};
function tr_bg_color_green(res)
{
  if(res.status == 'ok')
  {
    if(sel_obj)
    {
      sel_obj.parent().parent().css('backgroundColor', '#20b61f');
    }
  }else{
    alert(res.content);
  }
}
function admin_invest_loi_approved(res)
{
  if(res.status == 'ok')
  {
    if(sel_obj)
    {
      var info = res.content.split('_');
      if (info[0] != '0') {
        sel_obj.html('Invest Now');
        $('#pre_invest').remove();
        sel_obj.parent().after('<div id="pre_invest" style="text-align: center;width: 120%; margin-left: -30px; margin-top: -20px;">Access to private company information granted, ' +
            'review before investing.</div>');
      }
      else {
        sel_obj.html('EXPLORE INVESTMENT');
        $('#pre_invest').remove();
      }
    }
  }else{
    alert(res.content);
  }
}
function init_assign_admin()
{
  $(".select_assign_admin").change(function(){
    sel_obj = $(this);
    var selected_admin_uid = $(this).val();
    var target_uid = $(this).attr('target_uid');
    if(selected_admin_uid && target_uid)
    {
      POSTDATA = "action=assign_admin&target_uid=" + target_uid + "&admin=" + selected_admin_uid;
      ajax_update(POSTDATA, tr_bg_color_green);
    }else
    {
      alert('Error');

    }
  });
}
function inti_table_collapse(){
  $('#table_collapse').bind('click',function(evt){
    evt.preventDefault();
    var str = $(this).text();
    if(str == 'more info')
    {
      $(this).text('hide');
      $('.hidea').each(function(){$(this).css('display','none');});
      $('.hideb').each(function(){$(this).css('display','table-cell');});
    }else{
      $(this).text('more info');
      $('.hidea').each(function(){$(this).css('display','table-cell');});
      $('.hideb').each(function(){$(this).css('display','none');});
    }
  })
}
function overlay_show(res)
{
  if(res.status != 'ok') return;

  html = (res.content);
  //$('#easyOverlay').append( html);
  document.getElementById('overlay_content').innerHTML += "<br><br><h2></h2>" + html + "<br><br><a href='javascript:void(0)' id='closeit2' >Close Window</a>";
  var wide = parseInt(($(window).width() / 2) - (($('#overlay_content').width() ) / 2));
  var high = parseInt(($(window).height() / 2) - (($('#overlay_content').height() ) / 2));
  var scrollTop = $(window).scrollTop();
  var pos = ($('#easyOverlay').offset());
  var W = $('#overlay_content').width();
  var PH = $(window).height();
  $("#easyOverlay").css({
    top: high + scrollTop + "px",
    left: wide + "px",
    width: W+"px",
    visibility: "visible"
  })
  $("#overlay").css({ height: PH + scrollTop+ "px", display: 'none',visibility: "visible"}).fadeIn('slow');
  $("#closeit2").bind("click", function(){
    $("#overlay").fadeOut();
    $("#financial_chart").css("display",'block');
  });
}
function init_datepicker()
{
  if($('.datepicker').length > 0 ){
    $('.datepicker').each(function(){
      $(this).datepicker();
    })
  }
  if($('.monthpicker').length > 0 ){
    $('.monthpicker').each(function(){
      var D = new Date();
      var year = D.getFullYear();
      var str = (year - 50) + ':' + (year);

      $(this).datepicker({ changeYear: true,changeMonth:true,yearRange: str});
    })
  }
  if($('.monthyearpicker').length > 0 ){
    $('.monthyearpicker').datepicker( {
      changeMonth: true,
      changeYear: true,
      showButtonPanel: true,
      dateFormat: 'MM yy',
      onClose: function(dateText, inst) {
        var month = $("#ui-datepicker-div .ui-datepicker-month :selected").val();
        var year = $("#ui-datepicker-div .ui-datepicker-year :selected").val();
        $(this).datepicker('setDate', new Date(year, month, 1));
      }
    });
  }

}
function editwnd_show(res)
{
  if(res.status != 'ok') return;

  html = (res.content);
  if($("#overlay")) { $("#overlay").remove(); }
  $("body").append("<div id='overlay'><div id='overlay_bg'></div><div id='easyOverlay'><a href='javascript:void(0)' id='closeit'>X</a><div id='overlay_content'></div></div></div>");
  $("#closeit").bind("click", function(){
    $("#overlay").fadeOut();
    $("#financial_chart").css("display",'block');
  });
  document.getElementById('overlay_content').innerHTML +=("<br><br><h2>" + res.title + "</h2>" + html + "<br><br><a href='javascript:void(0)' id='closeit2' >Close Window</a>");
  init_datepicker();
  init_inputLength();
  var wide = parseInt(($(window).width() / 2) - (($('#overlay_content').width() ) / 2));
  var high = parseInt(($(window).height() / 2) - (($('#overlay_content').height() ) / 2));
  var scrollTop = $(window).scrollTop();
  var pos = ($('#easyOverlay').offset());
  var W = $('#overlay_content').width();
  var PH = $(window).height();
  $("#easyOverlay").css({
    top: high + scrollTop + "px",
    left: wide + "px",
    width: W+"px",
    visibility: "visible"
  })
  $("#overlay").css({ height: PH + scrollTop+ "px", display: 'none',visibility: "visible"}).fadeIn('slow');
  $("#closeit2").bind("click", function(){
    $("#overlay").fadeOut();
    $("#financial_chart").css("display",'block');
  });
}
function ajax_update(POSTDATA,callback){
  $.ajax({
    url: 'maennaco_ajax',
    dataType:'json',
    type:'post',
    data:POSTDATA,
    success: function(response){
      callback (response);
    },
    error :function (xhr){
      alert('Error!  Status = ' + xhr.status + xhr.responseText);
    }
  })
}


function init_td_hilightinput()
{
  $("td.td_hilightinput > div > input").each(function(){
    $(this).bind('focus', function(){
      $(this).select();
    });
  });
  $("a[id=reload_captcha]").bind('click',function(evt){
    evt.preventDefault();
    $("div[id=captcha_img]").html('<img height="39px" width="110px" src="/captcha/captcha.php?' + Math.random() + '">');
  })
}
function init_image_rollover()
{
  $('.button-links > a > img').each(function(){
    var the_src = $(this).attr('src');

    var rollover_src = the_src.replace('.png', '_over.png');
    if($(this).parent().hasClass('active-trail')){
      $(this).attr('src', rollover_src);
      return;
    }
    if(rollover_src.search(/http/i) < 0 ) rollover_src = 'http://' + location.host + '/' + rollover_src;
    LoadImg(rollover_src);
    $(this).bind('mouseover', function(){
      the_src = $(this).attr('src');
      rollover_src = the_src.replace('.png', '_over.png');
      if(rollover_src.search(/http/i) < 0 ) rollover_src = 'http://' + location.host + '/' + rollover_src;
      $(this).attr('src', rollover_src);
    }).bind('mouseout',  function(){
      the_src = $(this).attr('src');
      rollover_src =  the_src.replace('_over.png', '.png');
      if(rollover_src.search(/http/i) < 0 ) rollover_src = 'http://' + location.host + '/' + rollover_src;
      $(this).attr('src', rollover_src);
    })
  })
}

function LoadImg(imgSrc)
{
  var ImgObj = new Image();
  ImgObj.src = imgSrc;
}

function init_collapsible()
{
  $(".collapsible").siblings('p').css('display','none');
  $(".collapsible").each(function(){
    $(this).css('cursor','pointer').addClass('blue').css('text-decoration','underline').bind('mouseover',function(){
      $(this).removeClass('blue').css('text-decoration','none');
    }).bind('mouseout',function(){
      $(this).addClass('blue').css('text-decoration','underline');
    }).bind('click',function(){
      if($(this).hasClass('dark-gray')) $(this).removeClass('dark-gray');
      else $(this).addClass('dark-gray');
      $(this).siblings('p').animate({height:'toggle'},500)
    });
  })
  $('.popup').bind('click',function(evt){
    evt.preventDefault();
    if($(this).attr('rel') == 'terms') POSTDATA = "action=overlay_terms";
    ajax_update(POSTDATA, overlay_show);
  });
  $('#fundrising_link').bind('click',function(e){
    e.preventDefault();
    if($('.funding-data').hasClass('hidden')){
      $('.funding-data').removeClass('hidden');
    }
  });
}

function check_input()
{
  tinyMCE.triggerSave();
  var Correct = true;

  $(".require_select").each(function(){
    if ($(this).find('option:selected').text() == '') {
      Correct = false;
            $(this).css("border","1px solid #ff0000");
    } else {
            $(this).css("border","1px solid #f2f2f2");
    }
  });
  $(".require_string").each(function(){

    if ($(this).val() == null || $(this).val() == '') {

      Correct = false;
      $(this).css("border","1px solid #ff0000");
    } else {
            $(this).css("border","1px solid #f2f2f2");
    }
  });
  $(".require_email").each(function() {
    email = $(this).val();
    if( email == null || email == '' || (! check_email( email ) ) ){
      Correct = false;
            $(this).css("border","1px solid #ff0000");
    }else{
            $(this).css("border","1px solid #f2f2f2");
    }
  });
  var password1 = $("#password1").val();
  var password2 = $("#password2").val();
  if(password1 != '' || password2 != '')
  {
    if(password1 != password2)
    {
      Correct = false;
      $("#password1").css("border","1px solid #ff0000");
      $("#password2").css("border","1px solid #ff0000");
      alert("Passwords don't match");
      return false;
    }else
    {
      $("#password1").css("border","1px solid #f2f2f2");
      $("#password2").css("border","1px solid #f2f2f2");
    }
  }
  if( ! Correct ) {
    alert("Please enter the required info and try again");
    $("input[name='submit']").removeAttr('disabled');
  }
  return Correct;
}
function check_email(email)
{
  var filter = /^([a-zA-Z0-9_\.\-])+\@(([a-zA-Z0-9\-])+\.)+([a-zA-Z0-9]{2,4})+$/;
  return filter.test(email);
}
function init_overlay()
{
  $.fn.easyOverlay = function(){
    $(this).click(function(e){
      if($("#overlay")) { $("#overlay").remove(); }
      $("body").append("<div id='overlay'><div id='overlay_bg'></div><div id='easyOverlay'><a href='javascript:void(0)' id='closeit'>X</a><div id='overlay_content'></div></div></div>");
      $("#closeit").bind("click", function(){
        $("#overlay").fadeOut();
        $("#financial_chart").css("display",'block');
      });
    });
  };
  $(".popup").easyOverlay();
}
function init_editwnd()
{
  $('.edit_wnd').bind('click',function(evt){
    evt.preventDefault();
    $("#financial_chart").css("display",'none');
    POSTDATA = "action=" + $(this).attr('rel') + "&ajax=yes";
    if($(this).attr('data') != '') POSTDATA += "&data=" + $(this).attr('data');
    ajax_update(POSTDATA, editwnd_show);
  });
}
function reload_page(res){
  alert(res);
  window.location.href=location.href;
}
function init_researchEdit()
{
  $('.research-edit-icon').bind('click',function(evt){
    //evt.preventDefault();
    POSTDATA = "action=research_edit_company&ajax=yes&page=" + $(this).attr('page');
    if($(this).attr('data') != '') POSTDATA += "&data=" + $(this).attr('data');
    ajax_update(POSTDATA, editwnd_show);
  });
  $('.company-edit-icon').bind('click',function(evt){
    //evt.preventDefault();
    POSTDATA = "action=company_edit_company&ajax=yes&page=" + $(this).attr('page');
    if($(this).attr('data') != '') POSTDATA += "&data=" + $(this).attr('data');
    ajax_update(POSTDATA, editwnd_show);
  });
}
function init_visibility() {

  $(".select_set_public").change(function(){
    sel_obj = $(this);
    var target_publicity = $(this).val();
    var target_uid = $(this).attr('target_uid');
    if(target_uid)  $.post("/themes/maennaco/includes/follower.php?type=setPublic", { uid: target_uid, target_publicity: target_publicity },function(response){});

    else alert('error');


  });
}

function init_set_public() {

  $(".select_set_public").change(function(){
    sel_obj = $(this);
    var target_publicity = $(this).val();
    var target_cid = $(this).attr('target_uid');
    if(target_cid)  $.post("/themes/maennaco/includes/collaborator.php?type=setPublic", { cid: target_cid, target_publicity: target_publicity },function(response){});

    else alert('error');


  });
}

function init_changeProjName() {

  $("#submitProjName").click(function() {

    if (confirm("Please confirm you would like \"" + $('#edit-projnameinitial').val() +  "\" to represent your account on Clewed for confidentiality. All materials you submit should have this name, not your real company name.")) {

      sel_obj = $(this);
      proj_name = $("#edit-projnameinitial").val();
      no = $(this).attr('cid');
      m = $(this).attr('m');

      $.post("/themes/maennaco/includes/fetch.php?type=changeProjectName", { cid: no,proj_name: proj_name,m: m},

        function(response){
          alert(response);
          if (response == 'Project name changed successfully') {

            sel_obj.parent().parent().parent().parent().hide();
            $(".proj_name").html(proj_name.toUpperCase());

          }


        });

    }

  });
}
function init_collaborate() {

  $('a[type="no_coll"]').livequery("click", function() {

    alert("Only approved professionals can contribute. Please complete your profile if you have not done so to proceed.");

  });

  $('a[type="collaborate"]').livequery("click", function() {
    sel_obj = $(this);
    cid = sel_obj.attr("cid");
    uid = sel_obj.attr("uid");
    if (confirm("Confirm your connection request!")) {
      $.post("/themes/maennaco/includes/collaborator.php", { companyId: cid,pid: uid},
        function(response){
          sel_obj.attr('type','uncollaborate');
          sel_obj.attr('data-tooltip', 'Uncollaborate');
          sel_obj.html('Pending');
          alert(response);
        });
    }
  });

  $('a[type="uncollaborate"]').livequery("click", function() {
    sel_obj = $(this);
    cid = sel_obj.attr("cid");
    uid = sel_obj.attr("uid");
    if (confirm("Do you really want to disconnect?")) {
      $.post("/themes/maennaco/includes/collaborator.php?type=uncollaborate", { companyId: cid,pid: uid},
        function(response){
          sel_obj.attr('type','collaborate');
          sel_obj.attr('data-tooltip', 'This tool allows you to communicate with the project owner about your interest/qualifications. You must have deep knowledge of the business model of successful companies in this industry to connect.');
          sel_obj.html("Connect");
          alert(response);
        });
    }
  });
}

function init_contribute() {
  $('a[type="no_coll"]').livequery("click", function () {
    alert("Only approved professionals can connect. Please complete your profile for approval.");
  });
  $('a[type="contribute"]').livequery("click", function () {
    sel_obj = $(this);
    cid = sel_obj.attr("cid");
    uid = sel_obj.attr("uid");
    if (confirm("Confirm your connection request!")) {
      $.post(
        "/themes/maennaco/includes/collaborator.php",
        {
          companyId: cid,
          pid: uid
        },
        function (response) {
          sel_obj.css('color', '#9F9F9F');
          sel_obj.attr('type', 'Disconnect');
          sel_obj.html('Pending');
          alert(response);
        }
      );
    }
  });
  $('a[type="uncontribute"]').livequery("click", function () {
    sel_obj = $(this);
    cid = sel_obj.attr("cid");
    uid = sel_obj.attr("uid");
    if (confirm("Do you really want to disconnect?")) {
      $.post(
        "/themes/maennaco/includes/collaborator.php?type=uncollaborate",
        {
          companyId: cid,
          pid: uid
        },
        function (response) {
          sel_obj.attr('type', 'collaborate');
          sel_obj.css('color', '#00a2bf');
          sel_obj.html('Request connection');
          alert(response);
        }
      );
    }
  });
}


function init_follow() {

  $('a[type="follow"]').livequery("click",function() {
    sel_obj = $(this);
    cid = sel_obj.attr("cid");
    uid = sel_obj.attr("uid");
    if (confirm("Confirm your following request!")) {

      $.post("/themes/maennaco/includes/follower.php?type=follow", { companyId: cid,pid: uid},

        function(response){

          sel_obj.attr('type','unfollow');
          sel_obj.html('Following');
          sel_obj.attr('title','Unfollow');
          sel_obj.parent().find('.foll_cnt').html(parseInt(sel_obj.parent().find('.foll_cnt').html())+1);
          alert(response);


        });
    }

  });

  $('a[type="unfollow"]').livequery("click",function() {
    sel_obj = $(this);
    cid = sel_obj.attr("cid");
    uid = sel_obj.attr("uid");
    if (confirm("Do you really want to unfollow!")) {

      $.post("/themes/maennaco/includes/follower.php?type=unfollow", { companyId: cid,pid: uid},

        function(response){
          sel_obj.attr('type','follow');
          sel_obj.html('Follow');
          sel_obj.parent().find('.foll_cnt').html(parseInt(sel_obj.parent().find('.foll_cnt').html())-1);
          alert(response);


        });
    }

  });

}

function init_rsvp_invite() {

  $('.invitation').click(function(){

    inv = $(this);
    rel = inv.attr('rel');
    inv_id = inv.attr('iid');

    if (confirm('Are you sure you want to ' + rel + " invitation?")) {

      $.post("/themes/maennaco/includes/add_moderator.php?type=invitation&inv_id="+inv_id+"&status="+rel, {

      }, function(response){

        if ($.trim(response) == 'OK') {

          alert("You have successfully "+rel+"ed invitation.");
          if (rel == 'accept') {
            inv.parent().parent().find("span.inv_status").html("Guest expert:");
            inv.parent().parent().find("span.rsvp_inv").remove();
          }

          else {

            inv.parent().parent().find("a[id^='pro_id']").remove();
            inv.parent().parent().find("span.rsvp_inv").remove();

          }

        } else alert('There was an error.Please try again.');

      });

    }


  });

}

function init_invite() {

  $('a.invite').livequery("click",function() {
    sel_obj = $(this);
    iid = sel_obj.attr("iid");
    uid = sel_obj.attr("uid");
    pname = sel_obj.attr("pname");
    itopic = sel_obj.attr("itopic");
    bEdit = sel_obj.attr("type");
    m = sel_obj.attr("m");
    pid = sel_obj.attr('pid');
    if (confirm("Invite " + pname + " to your Insight " + itopic)) {

      $.post("/themes/maennaco/includes/add_moderator.php?type=invite", { discid: iid,proid: uid,modname: pname,bEdit: bEdit,m:m,itopic:itopic},

        function(response){

          if ($.trim(response) == 'OK') {
            alert("Invitation sent");
            window.location.href = 'account?tab=professionals&id='+pid+'&page=pro_detail&section=pro_industry_view&type=details&pro_id='+iid;
          }
          else if ($.trim(response) == 'duplicate')
            alert("You have already invited that professional. Try inviting someone else.");
        });
    }

  });

}


function init_sortPanel() {

  $(".openSortPanel").livequery("click",function() {

    $("#"+$(this).attr("panelid")).show();
    $(this).removeClass("openSortPanel");
    $(this).addClass("closeSortPanel");
    $(this).attr("src", "/themes/maennaco/images/arrow_up.png");

  });

  $(".closeSortPanel").livequery("click",function() {

    $("#"+$(this).attr("panelid")).hide();
    $(this).removeClass("closeSortPanel");
    $(this).addClass("openSortPanel");
    $(this).attr("src", "/themes/maennaco/images/arrow_down.png");

  });

}

function init_cnf_collaborate() {

  $('a[type="cnf_req"]').click(function() {

    selObj = $(this);
    req_type = '';

    if ((selObj).attr('cnf_type') == 'cnfColl') req_type = 'collaborators'; else req_type = 'management';


    $("#cnf_uid").attr('uid', selObj.attr('uid'));
    $("#cnf_uid").attr('cnf_type', selObj.attr('cnf_type'));
    $(".dialog_text").html('Please confirm '+selObj.attr('uname')+' is not connected as any team member of "'+selObj.attr('proj_name')+'" before approving this request to connect');

    $("#dialog-confirm").dialog('open');
    $("#dialog-confirm").dialog('option','height',200);

  });

}

function init_search_selector(){
  $(".letter_selector").each(function(){
    $(this).bind('click',function(evt){
      evt.preventDefault();
      var letter = $(this).attr('data');
      $("#letter").val(letter);
      $("#search_form").submit();
    })
  });
}
var str_selected = '';
function init_select_all()
{
  $("#select_all").bind('click',function(){
    if($("#select_all").attr('checked')){
      $(".records").each(function(){$(this).attr('checked', true);});
    }else{
      $(".records").each(function(){$(this).attr('checked', false);});
    }
  });

  $('#public_btn').livequery('click',function(){

    if ($(this).attr('user') == 'company') mess = 'Make selected companies visible to professionals.Confirm?';
    else mess = "Make selected professional visible to companies.Confirm?"
    if(confirm(mess))
    {
      str_selected = '';
      $('#admin_action').val('action_public');
      $('.cb_record').each(function(){
        if($(this).attr('checked') == 'checked' || $(this).attr('checked') == true)
        {
          str_selected  += ',' +  $(this).val();
        }

      });
      //alert(str_selected);
      if(str_selected == ',') str_selected = '';
      if(str_selected != ''){
        if ($(this).attr('user') != 'company') {
          $.post("/themes/maennaco/includes/fetch.php?type=checkApproved", { uids: str_selected},function(response){

            jobj = $.parseJSON(response);
            str_arr = str_selected.split(',');



            if (jobj.status == 'false') {

              alert("Only approved professionals will be made public!");

              $.each(jobj,function(i,item) {

                if (i != 'status') {

                  str_arr = jQuery.grep(str_arr, function(value) {
                    return value != item;
                  });
                }
              });
              str_selected = str_arr.join();
            }


            $('#selected_records').val(str_selected);
            $("#search_form").submit();


          });

        }

        else {

          $('#selected_records').val(str_selected);
          $("#search_form").submit();


        }

      }
      else{
        alert("Please select records to toggle visibility");
      }
    }
  });

  $('#approve_btn').livequery('click',function(){
    if(confirm('Continue to approve selected professionals'))
    {
      str_selected = '';
      $('#admin_action').val('action_approve');
      $('.cb_record').each(function(){
        if($(this).attr('checked') == 'checked' || $(this).attr('checked') == true)
        {
          str_selected  += ',' +  $(this).val();
        }

      });
      //alert(str_selected);
      if(str_selected == ',') str_selected = '';
      if(str_selected != ''){
        $('#selected_records').val(str_selected);
        $("#search_form").submit();
      }
      else{
        alert("Please select records to approve");
      }
    }
  });

  /* Start Fundraising */
  $('#start_fundraising_btn').livequery('click',function(){
    if(confirm('Continue to start fundraising for selected users?'))
    {
      str_selected = '';
      $('#admin_action').val('action_start_fundraising');
      $('.cb_record').each(function(){
        if($(this).attr('checked') == 'checked' || $(this).attr('checked') == true)
        {
          str_selected  += ',' +  $(this).val();
        }

      });

      if(str_selected == ',') str_selected = '';
      if(str_selected != ''){
        $('#selected_records').val(str_selected);
        $("#search_form").submit();
      }
      else{
        alert("Please select records to start fundraising for.");
      }
    }
  });

  /* Toggle Investing */
  $('#fundraising_btn').livequery('click',function(){
      if(confirm('Continue to toggle investing for selected users?'))
      {
          str_selected = '';
          $('#admin_action').val('action_fundraising');
          $('.cb_record').each(function(){
              if($(this).attr('checked') == 'checked' || $(this).attr('checked') == true)
              {
                  str_selected  += ',' +  $(this).val();
              }

          });

          if(str_selected == ',') str_selected = '';
          if(str_selected != ''){
              $('#selected_records').val(str_selected);
              $("#search_form").submit();
          }
          else{
              alert("Please select records to toggle investing for.");
          }
      }
  });

  /* Toggle Insight */
  $('#insight_btn').livequery('click',function(){
      if(confirm('Continue to mark users as if they came to attend Insights.'))
      {
          str_selected = '';
          $('#admin_action').val('action_insights');
          $('.cb_record').each(function(){
              if($(this).attr('checked') == 'checked' || $(this).attr('checked') == true)
              {
                  str_selected  += ',' +  $(this).val();
              }

          });

          if(str_selected == ',') str_selected = '';
          if(str_selected != ''){
              $('#selected_records').val(str_selected);
              $("#search_form").submit();
          }
          else{
              alert("Please select records to mark as Insights attendees.");
          }
      }
  });

  /* Delete */
  $('#delete_btn').livequery('click',function(){
    if(confirm('Continue to remove the selected records and all associated connections?'))
    {
      str_selected = '';
      $('#admin_action').val('action_delete');
      $('.cb_record').each(function(){
        if($(this).attr('checked') == 'checked' || $(this).attr('checked') == true)
        {
          str_selected  += ',' +  $(this).val();
        }

      });
      //alert(str_selected);
      if(str_selected == ',') str_selected = '';
      if(str_selected != ''){
        $('#selected_records').val(str_selected);
        $("#search_form").submit();
      }
      else{
        alert("Please select records to delete");
      }
    }
  });

  $('#reject_btn').livequery('click',function(){

    if ($(this).attr('user') == 'company') mess = 'This user does not meet clewed service criteria. This action deletes selected users from the system. Confirm deletion?';
    else mess = 'This user does not meet clewed professional criteria. Reject to delete user from professional table';
    if(confirm(mess))
    {
      str_selected = '';
      $('#admin_action').val('action_delete');
      $('#reject_company').val('reject');
      $('.cb_record').each(function(){
        if($(this).attr('checked') == 'checked' || $(this).attr('checked') == true)
        {
          str_selected  += ',' +  $(this).val();
        }

      });
      //alert(str_selected);
      if(str_selected == ',') str_selected = '';
      if(str_selected != ''){
        $('#selected_records').val(str_selected);
        $("#search_form").submit();
      }
      else{
        alert("Please select records to reject");
      }
    }
  });

  /* Toggle Deactive */
  $('#active_btn').livequery('click',function(){
    if(confirm('Deactivated users will be listed at the end of the user table. Deactivate?'))
    {
      str_selected = '';
      $('#admin_action').val('action_activate');
      $('.cb_record').each(function(){
        if($(this).attr('checked') == 'checked' || $(this).attr('checked') == true)
        {
          str_selected  += ',' +  $(this).val();
        }

      });
      //alert(str_selected);
      if(str_selected == ',') str_selected = '';
      if(str_selected != ''){
        $('#selected_records').val(str_selected);
        $("#search_form").submit();
      }
      else{
        alert("Please select records to change active status");
      }
    }
  });
  $('#filter_btn').bind('click',function(){
    $("#admin_action").val(' ');
  });

  $('#match_btn').bind('click', function(){
    if($('#select_adminid').val() == ''){
      alert('Please select an admin to continue');
    }
    else
    {
      var cmp_id = $("#select_cmpid").val();
      $("#cmp_id").val(cmp_id);
      if(confirm('Continue to match selected professionals to company?'))
      {
        str_selected = '';
        $('#admin_action').val('action_match');
        $('.cb_record').each(function(){
          if($(this).attr('checked') == 'checked' || $(this).attr('checked') == true)
          {
            str_selected  += ',' +  $(this).val();
          }

        });
        str_selected = str_selected.substr(1);
        if(str_selected == ',') str_selected = '';
        if(str_selected != ''){
          $('#selected_records').val(str_selected);
          $("#search_form").submit();
        }
        else{
          alert("Please select records to continue");
        }
      }
    }

  });
  $('#assign_admin_btn').livequery('click', function(){

    var admin_id = $("#select_adminid").val();

    // if ( admin_id == 'none')
    //   admin_id = $("div#sticky-head table.actions select#select_adminid").val();
    // else if ($("div#sticky-head table.actions select#select_adminid").val() != 'none') admin_id = $("div#sticky-head table.actions select#select_adminid").val();
    if(admin_id == 'none'){
      alert('Please select an admin to continue');
    }
    else
    {
      $("#admin_id").val(admin_id);
      if(confirm('Continue to assign admin to the selected records?'))
      {
        str_selected = '';
        $('#admin_action').val('action_assign_admin');
        $('.cb_record').each(function(){
          if($(this).attr('checked') == 'checked' || $(this).attr('checked') == true)
          {
            str_selected  += ',' +  $(this).val();
          }

        });
        //alert(str_selected);
        if(str_selected == ',') str_selected = '';
        if(str_selected != ''){
          $('#selected_records').val(str_selected);
          $("#search_form").submit();
        }
        else{
          alert("Please select records to continue");
        }
      }
    }

  });
  $('.info_tab_btn').bind('click',function(evt){
    evt.preventDefault();
    var tab_id = $(this).attr('tab');
    $("#info_tab").val(tab_id);
    $("#admin_action").val(' ');
        //$("#update_section").val(' ');
    $("#search_form").submit();
  })
}
function init_search_pagination()
{
  $(".pagination_btn").bind("click", function(evt){
    evt.preventDefault();
    var pageid = $(this).attr('page');
    $("#_page").val(pageid);
    $("#admin_action").val(' ');
    $("#search_form").submit();
  });
}
function wordcounter(inp)
{

  var limitNum = inp.getAttribute('limit');
  var counter = document.getElementById('wordcounter');
  if (inp.value.length > limitNum) {
    inp.value = inp.value.substring(0, limitNum);
  } else {
    counter.innerHTML = limitNum - inp.value.length;
  }
}
function toggleDiv(ID,obj){
  if(ID){
    if($('#'+ID).hasClass('hide')){
      $('#'+ID).show().removeClass('hide');
      obj.innerHTML = 'hide';
    }else{
      $('#'+ID).hide().addClass('hide');
      obj.innerHTML = 'more';
    }
  }
}
function doNothing(res){
  //alert(res.content);
  i = 1;
}
var ajaxClickObj = {};
function postingapply(res)
{
  if(res.content == 'apply'){

    ajaxClickObj.removeClass('button_off').text('apply');
  }else if(res.content == 'cancel'){

    ajaxClickObj.addClass('button_off').text('cancel');
  }else{
    alert(res.content);
  }
}
function eventapply(res)
{
  if(res.content == 'register'){

    ajaxClickObj.removeClass('button_off').text('register');
  }else if(res.content == 'cancel'){

    ajaxClickObj.addClass('button_off').text('cancel');
  }else{
    alert(res.content);
  }
}
function rediretPage(res)
{
  if(res.status == 'ok'){
    alert(res.content);
    window.location.href = 'http://' + res.content;
  }else{
    alert(res.content);
  }
}
function init_ajaxTrigger()
{
  $(".ajaxTrigger").each(function(){
    if($(this).is('select')){
      $(this).change(function(){
        table = $(this).attr('table');
        idval = $(this).attr('idval');
        idname = $(this).attr('idname');
        column = $(this).attr('column');
        val = $(this).val(); // it maybe empty??
        if( (table == '') || (idval == '') || (idname == '') ){//|| (column == '') ){
          alert("Invalid data");
          return ;
        }else{
          POSTDATA = "a=maennaco_ajax&action=ajaxTrigger&table=" + table +"&column=" + column +"&idname=" + idname + "&idval=" + idval + "&val=" + val;
          ajax_update(POSTDATA, doNothing);
        }
      })
    }else{
      table = $(this).attr('table');
      if(table == 'maenna_postings_data')
      {
        $(this).bind('click', function(evt){
          evt.preventDefault();
          ajaxClickObj = $(this);
          postingid=$(this).attr("postingid");
          dataid = $(this).attr("dataid");

          if(postingid)
          {
            POSTDATA = "a=maennaco_ajax&action=ajaxTrigger&table=" + table +"&postingid=" + postingid + "&dataid=" + dataid;
            ajax_update(POSTDATA, postingapply);
          }

        })
      }else if(table == 'maenna_events_data')
      {
        $(this).bind('click', function(evt){
          evt.preventDefault();
          ajaxClickObj = $(this);
          eventid=$(this).attr("eventid");
          dataid = $(this).attr("dataid");

          if(eventid)
          {
            POSTDATA = "a=maennaco_ajax&action=ajaxTrigger&table=" + table +"&eventid=" + eventid + "&dataid=" + dataid;
            ajax_update(POSTDATA, redirectPage);
          }
        });
      }else{
        i = 1;
      }
    }
  })
}
function init_openbox()
{
  $(".openbox").each(function(){
    $(this).bind('click',function(evt){
      evt.preventDefault();
      var boxid = $(this).attr("boxid");
      $("#"+boxid).css("display","block");
    })
  });

  $(".openclose").each(function(){
    $(this).bind('click',function(evt){

      evt.preventDefault();
      var boxid = $(this).attr("boxid");

      if($("#"+boxid).css("display") == 'block')
      {
        $("#"+boxid).css("display","none");
        $(this).text("MORE");
      }else{
        $("#"+boxid).css("display","block");
        $(this).text("CLOSE");
      }

    })
  })
}
function init_hidebox(){
  $(".hidebox").each(function(){
    $(this).bind("click",function(evt){
      evt.preventDefault();
      // var boxid = $(this).attr("boxid");
      // $("#"+boxid).css("display","none");
      window.history.back();
    })
  })
  $("a.hidebox-1").each(function(){
    $(this).bind("click",function(evt){
      evt.preventDefault();
      var boxid = $(this).attr("boxid");
      $("#"+boxid).css("display","none");
    })
  })
}

function init_select_goto()
{
  $(".select_goto").change(function(){
    $param = $(this).val();
    $url = $(this).attr('baseurl') + $param;
    location.href = $url;
  })
}

function init_tinymce()
{
  // initialize tinymce editor
  // but do not apply on all textareas
  if (($('#docprev').size() == 0 || $('.minutes_tinymce_editor').size() > 0) &&
    $('#company-questions').size() == 0 &&
    $('#qmgmt-questions').size() == 0 &&
    $('#pro-questionnaire').size() == 0 &&
    $('#maenna-forms-company-signup-form-page2').size() == 0 &&
    $('#maenna-forms-pro-form-page2').size() == 0 ) {
    tinyMCE.init({
      // mode : "textareas",
      mode : "specific_textareas",
      editor_selector : "own_text_editor",
      theme : "advanced",
      valid_styles : { '*' : 'color,font-weight,text-decoration' },
      plugins : "paste,spellchecker",
      theme_advanced_buttons1 : "bold,italic,underline,separator,strikethrough,justifyleft,justifycenter,justifyright,justifyfull,bullist,numlist,|,undo,redo,|,link,unlink,|,cut,copy,paste,pastetext,pasteword,spellchecker",
      paste_auto_cleanup_on_paste : true,
      theme_advanced_buttons2 : "",
      theme_advanced_buttons3 : "",
      theme_advanced_toolbar_location : "top",
      theme_advanced_toolbar_align : "left",
      theme_advanced_path : false,
      editor_deselector: "mceNoEditor",
      init_instance_callback : function() {
        tinymce.execCommand('mceSpellCheck', true);
      },
      oninit : "setPlainText",
      setup : function(ed) {

        ed.onKeyDown.add(function (ed, evt) {
          var max_words = $(tinyMCE.get(tinyMCE.activeEditor.id).getElement()).attr('max_words');
          if (!max_words) {return;}
          //if ( $(ed.getBody()).text().length+1 > ed.getParam('max_chars')){
          var words = $(ed.getBody()).text().split(' ');
          if ( words.length > max_words)
          {
            if (evt.keyCode != 8 && evt.keyCode != 46){

              evt.preventDefault();
              evt.stopPropagation();

            }
          }

        });

        ed.onKeyUp.add(function(ed, evt) {
          var keycode = (evt.keyCode ? evt.keyCode : evt.which);

          var max_words = $(tinyMCE.get(tinyMCE.activeEditor.id).getElement()).attr('max_words');
          if (!max_words) {return;}
          //if ( $(ed.getBody()).text().length+1 > ed.getParam('max_chars')){
          var words = $(ed.getBody()).text().split(' ');
          if ( words.length > max_words)
          {
            evt.preventDefault();
            alert('You`ve reached maximum allowed words.Please delete some words.');
            evt.stopPropagation();

            return false;
          }
        });

                ed.onInit.add(function() {
                    if(ed.getContent() != '') {
                        $('label[for="tinymce"]').hide();
                    }

                    $(ed.getDoc()).contents().find('body').focus(function(){
                        $('label[for="tinymce"]').hide();
                    });

                    $(ed.getDoc()).contents().find('body').blur(function(){
                        if(ed.getContent() == '') {
                            $('label[for="tinymce"]').show();
                        }
                    });
                });
      }
    });
  }
}

function setPlainText() {
  var editors = tinyMCE.EditorManager.editors;

  for (var key in editors)
  {
    editor = tinyMCE.get(key);
    editor.pasteAsPlainText = true;
    if (tinymce.isOpera || /Firefox\/2/.test(navigator.userAgent)) {
      editor.onKeyDown.add(function (editor, e) {
        if (((tinymce.isMac ? e.metaKey : e.ctrlKey) && e.keyCode == 86) || (e.shiftKey && e.keyCode == 45))
          editor.pasteAsPlainText = true;
      });
    } else {
      editor.onPaste.addToTop(function (editor, e) {
        editor.pasteAsPlainText = true;
      });
    }
  }
}

function pro_profile_more_link() {
  $('.main_content .tool').click(function(e){
    e.preventDefault();
    var grand = $(this).parent().parent();
    grand.next("div:not(:visible)").show();
    grand.hide();
  });
}

function init_featured_insight_title() {
  $('.featured-insight-title').each(function(i, el) {
    var keep = el.innerHTML;
    while(el.scrollHeight > el.offsetHeight) {
      el.innerHTML = keep;
      el.innerHTML = el.innerHTML.substring(0, el.innerHTML.length-1);
      keep = el.innerHTML;
      el.innerHTML = el.innerHTML + "...";
    }
  });
}

function init_featured_company_mission() {
  $('.featured-company-mission').each(function(i, el) {
    var keep = el.innerHTML;
    while(el.scrollHeight > el.offsetHeight) {
      el.innerHTML = keep;
      el.innerHTML = el.innerHTML.substring(0, el.innerHTML.length-1);
      keep = el.innerHTML;
      el.innerHTML = el.innerHTML + "...";
    }
  });
}
function validate_video_url() {
  var url = $("#pitch_video_url").val();
  if (url == "") {
    return true;
  }
  var embedUrl = prepareEmbededUrl(url);
  return embedUrl != null;
}

function prepareEmbededUrl(url) {
   let regex = /^(?:https?:\/\/)?(?:www\.)?/i;
   let domainUrl = url.replace(regex,"");

   let providers =  [{
      name: "dailymotion",
      url: [/^dailymotion\.com\/video\/(\w+)/],
      embedUrl: "https://www.dailymotion.com/embed/video/"
    }, {
      name: "spotify",
      url: [/^open\.spotify\.com\/(artist\/\w+)/, /^open\.spotify\.com\/(album\/\w+)/, /^open\.spotify\.com\/(track\/\w+)/],
      embedUrl: "https://open.spotify.com/embed/"
    }, {
      name: "youtube",
      url: [/^(?:m\.)?youtube\.com\/watch\?v=([\w-]+)/, /^(?:m\.)?youtube\.com\/v\/([\w-]+)/, /^youtube\.com\/embed\/([\w-]+)/, /^youtu\.be\/([\w-]+)/],
      embedUrl: "https://www.youtube.com/embed/"
    }, {
      name: "vimeo",
      url: [/^vimeo\.com\/(\d+)/, /^vimeo\.com\/[^/]+\/[^/]+\/video\/(\d+)/, /^vimeo\.com\/album\/[^/]+\/video\/(\d+)/, /^vimeo\.com\/channels\/[^/]+\/(\d+)/, /^vimeo\.com\/groups\/[^/]+\/videos\/(\d+)/, /^vimeo\.com\/ondemand\/[^/]+\/(\d+)/, /^player\.vimeo\.com\/video\/(\d+)/],
      embedUrl: "https://player.vimeo.com/video/"
    }
  ];

   var embededUrl = null;

   $.each(providers, function(index, value) {
     $.each(value.url,function(indexs, values) {
        let match = values.exec(domainUrl);
        if (match) embededUrl =  value.embedUrl + match[1] + "?transparent=0";
     });
   });
   return embededUrl;
}

function init_company_cover_image_uploader() {

  var $uploader = $('.company-cover-image-uploader'),
    companyId = $uploader.data('id'),
    u = $uploader.data('u'),
    m = $uploader.data('m');

  if ($uploader.length > 0) {
    var qquploader = new qq.FileUploader({
      params: {
        company_id: companyId,
        itype: 'company-cover-image'
      },
      text: "UPLOAD IMAGE",
      multiple: false,
      element: $uploader[0],
      action: '/themes/maennaco/includes/file_upload.php',
      allowedExtensions: ["jpg", "jpeg", "png"],
      onComplete: function (id, fileName, responseJSON) {
        if (responseJSON.success) {
          var $c = $uploader.parent().find('.company-cover-image'),
            $parent = $c.parent(),
            $img = $c.find('img'),
            requiredHeight = 442,
            requiredWidth = 590,
            ratio = requiredWidth / requiredHeight,
            width = requiredWidth,
            selectionHeight = 50,
            selectionWidth = selectionHeight * ratio,
            displaySizeWarning = false,
            realImageSizeAvailable = false;

          if (responseJSON.width && responseJSON.width > 0 && responseJSON.height && responseJSON.height > 0) {

            realImageSizeAvailable = true;

            if (responseJSON.width < requiredWidth || responseJSON.height < requiredHeight)
              displaySizeWarning = true;

            var realRatio = responseJSON.width / responseJSON.height,
              scale = responseJSON.width / width;

            if (realRatio > ratio) {
              selectionHeight = responseJSON.height / scale;
              selectionWidth = selectionHeight * ratio;
            }
            else {
              selectionWidth = width;
              selectionHeight = selectionWidth / ratio;
            }
          }

          $parent.find('input[name=project]').val(responseJSON.name);

          if ($img.length > 0)
            $img.remove();

          $c.append('<img />');
          $c.find('img')
            .attr('src', responseJSON.url + '?' + ((new Date()).getTime()))
            .css('width', width)
            .css('height', '100%')
            .Jcrop({
              aspectRatio: ratio,
              resizable: false,
              minSize: [selectionWidth, selectionHeight],
              setSelect: [0, 0, selectionHeight, selectionWidth],
              onChange: function (c) {
                $c.find('.company-cover-image-crop-btn a')
                  .data('x', c.x)
                  .data('y', c.y)
                  .data('w', c.w)
                  .data('h', c.h)
              }
            });

          if (displaySizeWarning) {
            $c.append('<div class="company-cover-image-size-warning">' +
              (realImageSizeAvailable ? 'The size of uploaded image is ' + responseJSON.width + ' x ' + responseJSON.height + '. ' : '') +
              'The quality of the resulting image can be poor. <br> ' +
              'Please, upload another image of size not less than 590 x 440</div>');
            $c.append('<div class="company-cover-image-upload-another-image-btn"><a>upload another image</a></div>');
            $c.append('<div class="company-cover-image-crop-btn"><a>ignore and crop</a></div>');
          }
          else {
            $c.append('<div class="less-text company-cover-image-upload-another-image-btn"><a>upload another image</a></div>');
            $c.append('<div class="company-cover-image-crop-btn"><a>crop</a></div>');
          }

          $c.find('.company-cover-image-upload-another-image-btn a').click(function () {
            $c.dialog("close");
            $parent.append('<div class="company-cover-image"></div>');
            $uploader.find('input[type=file]').trigger('click');
          });

          $c.find('.company-cover-image-crop-btn a').click(function () {
            var $this = $(this),
              x = $this.data('x'),
              y = $this.data('y'),
              w = $this.data('w'),
              h = $this.data('h'),
              progress = $this.data('progress');

            if (w == 0 || h == 0)
              return alert('Please, select the image area to crop');

            if (progress)
              return false;

            $(this).text('cropping...');
            $(this).data('progress', 1);

            $.ajax({
              url: '/themes/maennaco/includes/cropper.php',
              type: 'post',
              data: {
                x: x,
                y: y,
                w: w,
                h: h,
                relativeWidth: width,
                type: 'company-cover-image',
                company: companyId,
                filename: responseJSON.name,
                u: u,
                m: m
              },
              dataType: 'json',
              success: function (r) {
                if (r.success) {
                  $parent.append(
                    '<div class="company-cover-image">' +
                    '   <img src="' + responseJSON.url + '?' + ((new Date()).getTime()) + '"/>' +
                    '</div>');
                }
                else {
                  alert('Operation failed. Try again');
                  $parent.append(
                    '<div class="company-cover-image"></div>');
                }

                $c.dialog("close");
              }
            });
          });

          $c.dialog({
            width: width + 30,
            position: ['middle', 100],
            title: "Please, select the image area to crop",
            modal: true,
            autoOpen: true,
            closeOnEscape: false,
            close: function () {
              $(this).dialog("close");
            }
          });

          $c.parent().find('.ui-dialog-titlebar-close').hide();

          if ($uploader.parent().find('.qq-upload-success').length > 1)
            $uploader.parent().find('.qq-upload-success').first().remove();
        }

      },
      onSubmit: function (id, fileName) {
      },
      fileTemplate: '<li>' +
      '<span class="qq-upload-file"></span>' +
      '<span class="qq-upload-spinner"></span>' +
      '<span class="qq-upload-size"></span>' + '<a class="qq-upload-remove"></span>' +
      '&nbsp;<a class="qq-upload-cancel" href="#"></a>' +
      '<span class="qq-upload-failed-text">Failed</span>' +
      '</li>'
    });


    let uploadButton = $uploader.parent().find('.qq-upload-button')[0];
    uploadButton.style.width = "135px";
  }
}

function init_tooltips() {
  $(document).tooltip({
    items: '[data-tooltip]',
    content: function () {
      return $(this).data('tooltip');
    }
  });
}

function init_fundraising_administration() {
  var $form = $('#maenna-forms-company-fap-checklist-form');
  if ($form.length == 0) {
    return;
  }

  $form.find('input[type="checkbox"]').change(function() {
    if (this.name == 'fundraising_declined' && $(this).is(':checked')) {
      $form.find('input[name="fundraising_approved"]').prop('checked', false);
    } else if (this.name == 'fundraising_approved' && $(this).is(':checked')) {
      $form.find('input[name="fundraising_declined"]').prop('checked', false);
    }
  });
}


/**
 * form validation
 */
function form_validation(event){

  tinyMCE.triggerSave();
  var Correct = true;

  $(".require_select").each(function(){
    if ($(this).find('option:selected').text() == '') {
      Correct = false;
      $(this).css("border","1px solid #ff0000");
    } else {
      $(this).css("border","1px solid #f2f2f2");
    }
  });
  $(".require_string").each(function(){

    if ($(this).val() == null || $(this).val() == '') {

      Correct = false;
      $(this).css("border","1px solid #ff0000");
    } else {
      $(this).css("border","1px solid #f2f2f2");
    }
  });
  $(".require_email").each(function() {
    email = $(this).val();
    if( email == null || email == '' || (! check_email( email ) ) ){
      Correct = false;
      $(this).css("border","1px solid #ff0000");
    }else{
      $(this).css("border","1px solid #f2f2f2");
    }
  });

  if( ! Correct ) {
    alert("Please enter the required info and try again");
    $("input[name='submit']").removeAttr('disabled');
    event.preventDefault();
  }
  return Correct;
}


document.addEventListener("DOMContentLoaded", function(event) {
  initAccordion()
});

/**
 * custom accordion
 */
let initAccordion = () => {
  let accordionContent = document.querySelector(".custom-accordion-content");
  if (accordionContent){
    let acc = accordionContent.querySelectorAll(".accordion");
    let i;

    for (i = 0; i < acc.length; i++) {
      acc[i].addEventListener("click", function() {
        this.classList.toggle("active");
        var panel = this.nextElementSibling;
        if (panel.style.maxHeight) {
          panel.style.maxHeight = null;
        } else {
          panel.style.maxHeight = panel.scrollHeight + "px";
        }
      });
    }
  }
}

