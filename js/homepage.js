$(document).ready(function () {
  window.utype = "company";
  var body = $("body");
  var emailRegex = new RegExp(/^([\w\.\-]+)@([\w\-]+)((\.[\w\-]+)+)$/i);

  function password_type(value) {
    if (value.val().length < 8) {
      alert("Your password needs to contain 8 characters minimum!");
      value.focus();
      return false;
    }
    re = /^[A-Za-z]+$/;
    if (re.test(value.val())) {
      alert("Your password needs to contain at least one number!");
      value.focus();
      return false;
    }
    return true;
  }

  function checkString(obj, watermark) {
    if (obj.val() == "" || obj.val() == watermark) {
      obj.css("border", "1px solid #C52020");
      obj.focus();
      return false;
    } else {
      obj.css("border", "none");
      return true;
    }
  }

  /*	//$('#company-firstname').Watermark('First Name');
	$('#company-lastname').Watermark('Last Name');
	$('#company-name').Watermark('Company Name');
	$('#company-email').Watermark('Email');
	$('#company-password').Watermark('Password');
	$('#pro-firstname').Watermark('First Name');
	$('#pro-lastname').Watermark('Last Name');
	$('#pro-email').Watermark('Email');
	$('#pro-password').Watermark('Password');*/
  $("#edit-email").Watermark("Email");
  //$('#edit-password').Watermark('Password');
  $("#feedback").click(function (event) {
    event.preventDefault();
    $.post(
      "/themes/maennaco/includes/homepage_posts.php?type=contactus",
      {},
      function (response) {
        $(".dialog-left-column a").removeClass("dialog-selected");
        $("#contactus").addClass("dialog-selected");
        $(".dialog-right-column").html(response).fadeIn("slow");
        $("#contact-name").Watermark("Name");
        $("#contact-email").Watermark("Email");
        $("#contact-subject").Watermark("Subject");
        $("#contact-message").Watermark("Message");
        $("#learn-more-dialog").dialog("open");
      }
    );
  });

  body.delegate("button.contactus", "click", function () {
    var fValid = true;
    fValid = fValid && checkString($("#contact-name"), "Name");
    fValid = fValid && checkString($("#contact-email"), "Email");
    fValid = fValid && checkString($("#contact-subject"), "Subject");
    fValid = fValid && checkString($("#contact-message"), "Message");
    if (fValid) {
      $.post(
        "/themes/maennaco/includes/homepage_posts.php?type=feedback",
        {
          name: $("#contact-name").val(),
          email: $("#contact-email").val(),
          subject: $("#contact-subject").val(),
          message: $("#contact-message").val(),
        },
        function (response) {
          if (response == 0) {
            $("#contact-name").val("");
            $("#contact-email").val("");
            $("#contact-subject").val("");
            $("#contact-message").val("");
            alert("Your information was successfully sent");
          } else {
            $("#contact-name").val("");
            $("#contact-email").val("");
            $("#contact-subject").val("");
            $("#contact-message").val("");
            alert("Message couldn't be send. Please try again!");
          }
          $("#learn-more-dialog").dialog("open");
        }
      );
    }
  });
  body.delegate("a.pagree", "click", function () {
    $.post(
      "/themes/maennaco/includes/homepage_posts.php?type=terms",
      {},
      function (response) {
        $("#right-column-left").html(response).fadeIn("slow");
      }
    );
  });
  $(".show_terms").click(function () {
    $.post(
      "/themes/maennaco/includes/homepage_posts.php?type=policy&req_type=" +
        $(this).attr("type"),
      {},
      function (response) {
        $("#policy").html(response);
      }
    );
    $("#policy").dialog("open");
    return false;
  });
  $("#tabs").tabs();
  //$("#company-email").bind('focusout', function () {
  //	var com = $("#company-email").val();
  //	if (com != '') {
  //		$.post("/check_email_exists.php",
  //			{email: $("#company-email").val()},
  //			function (response) {
  //				if (response == 'false') {
  //					alert("User with this email already exists. Please use another one.");
  //					$("#company-email").val('').focus();
  //				}
  //			});
  //		var valid = emailRegex.test($("#company-email").val());
  //		if (!valid) {
  //			alert("Format of the e-mail address is not correct");
  //			$("#company-email").val('').focus();
  //		}
  //	} else {
  //		return false;
  //	}
  //});
  //
  //$('#pro-email').bind('focusout', function () {
  //	var proEmail = $("#pro-email");
  //	var pro = proEmail.val();
  //	if (pro != '') {
  //		$.post("/check_email_exists.php",
  //			{email: proEmail.val()},
  //			function (response) {
  //				if (response == 'false') {
  //					alert("User with this email already exists. Please use another one.");
  //					$("#pro-email").val('').focus();
  //				}
  //			});
  //		var valid = emailRegex.test(proEmail.val());
  //		if (!valid) {
  //			alert("Format of the e-mail address is not correct");
  //			$("#pro-email").val('').focus();
  //		}
  //	} else {
  //		return false;
  //	}
  //});
  $("#learn-more-dialog").dialog({
    modal: true,
    autoOpen: false,
    width: 920,
    height: 620,
    draggable: false,
  });
  $("#policy").dialog({
    modal: true,
    autoOpen: false,
    width: 920,
    height: 620,
    draggable: false,
  });
  $("#register-dialog").dialog({
    modal: true,
    autoOpen: false,
    draggable: true,
    resizable: false,
    width: 350,
    buttons: [
      {
        text: "Register",
        click: function (e) {
          var clicked = $(".form-submit1");
          var fValid = true;
          selTab = window.utype;
          if (selTab == "company") {
            fValid =
              checkString($("#company-firstname"), "First Name") && fValid;
            fValid = checkString($("#company-lastname"), "Last Name") && fValid;
            fValid = checkString($("#company-name"), "Company Name") && fValid;
            fValid = checkString($("#company-email"), "Email") && fValid;
            fValid = checkString($("#company-password"), "Password") && fValid;
            fValid = password_type($("#company-password")) && fValid;
            fValid = checkString($("#company-industry"), "industry") && fValid;
            fValid = checkString($("#company-revenue"), "revenue") && fValid;
            if (!fValid) {
              alert("Please fill in the required fields.");
            }
            if (fValid && !$("#cmp-agree").is(":checked")) {
              fValid = false;
              alert(
                "You have to read and agree with terms and privacy policy!"
              );
            }
            if (fValid) {
              clicked.attr("disabled", true);
              if (clicked.attr("rel") == "request") {
                ajax_path = "register_new_user.php?type=company&request=true";
              } else ajax_path = "register_new_user.php?type=company";
              $.post(
                ajax_path,
                {
                  firstname: $("#company-firstname").val(),
                  lastname: $("#company-lastname").val(),
                  name: $("#company-name").val(),
                  email: $("#company-email").val(),
                  password: $("#company-password").val(),
                  industry: $("#company-industry").val(),
                  revenue: $("#company-revenue").val(),
                },
                function (response) {
                  alert(response);
                  if (response != "Security code is not correct!") {
                    location.reload();
                  }
                }
              );
            }
          } else {
            fValid = checkString($("#pro-type"), "iam") && fValid;
            fValid = checkString($("#pro-firstname"), "First Name") && fValid;
            fValid = checkString($("#pro-lastname"), "Last Name") && fValid;
            fValid = checkString($("#pro-email"), "Email") && fValid;
            fValid = checkString($("#pro-password"), "Password") && fValid;
            fValid = checkString($("#pro-experties"), "experties") && fValid;
            if (!fValid) {
              alert("Please fill in the required fields.");
            }
            if (fValid && !$("#pro-agree").is(":checked")) {
              fValid = false;
              alert(
                "You have to read and agree with terms and privacy policy!"
              );
            }
            if (fValid) {
              clicked.parent().attr("disabled", true);
              $.post(
                "register_new_user.php?type=proffesional",
                {
                  firstname: $("#pro-firstname").val(),
                  lastname: $("#pro-lastname").val(),
                  experties: $("#pro-experties").val(),
                  email: $("#pro-email").val(),
                  password: $("#pro-password").val(),
                  pro_type: $("#pro-type").val(),
                },
                function (response) {
                  alert(response);
                  if (response != "Security code is not correct!") {
                    location.reload();
                  }
                }
              );
            }
          }
        },
        class: "form-submit1",
      },
      {
        text: "Cancel",
        click: function () {
          $(this).dialog("close");
        },
        class: "form-submit2",
      },
    ],
  });
  $("#learn-more").click(function () {
    $("#learn-more-dialog").dialog("open");
    return false;
  });
  $(".request_demo").click(function () {
    /*        $("#register-dialog #tabs-2").hide();
        $("#register-dialog").parent().children(".ui-dialog-titlebar").remove();
        $(".reg-choose").hide();*/
    $("#register-dialog").children(".reg-top").html("Schedule a Demo");
    $(".form-submit1").children(".ui-button-text").html("Request");
    $(".agree_terms").hide();
    $(".form-submit1").attr("rel", "request");
    $("#register-dialog").dialog("open");
    return false;
  });

  $("#home-register").click(function () {
    $("#register-dialog")
      .children(".reg-top")
      .html("JOIN CLEWED, IT'S FREE TO START");
    $(".form-submit1").children(".ui-button-text").html("Register");
    $(".agree_terms").show();
    $(".form-submit1").attr("rel", "");
    if ($(this).data("type") == "company") {
      $("#register-dialog #tabs-2").hide();
      $("#register-dialog").parent().children(".ui-dialog-titlebar").remove();
      $(".reg-choose").hide();
      $("#register-dialog").dialog("open");
    } else {
      $("#register-dialog").dialog("open");
      $("#register-dialog").parent().children(".ui-dialog-titlebar").remove();
      $("#register-dialog #tabs-1").hide();
      $("#register-dialog #tabs-2").hide();
      $(".ui-dialog-buttonset").hide();
      $(".reg-but1").css("background-color", "#D0D2D2");
      $(".reg-but2").css("background-color", "#D0D2D2");
    }
    return false;
  });
  $(".reg-but1").click(function () {
    $(".ui-dialog-buttonset").show();
  });
  $(".reg-but2").click(function () {
    $(".ui-dialog-buttonset").show();
  });
  /*	$("input:text").focus(function () {
		$(this).val('');
	});*/
  var page_height = $(window).height();
  var diff = parseInt(page_height) - 480;
  diff = parseInt(diff / 2);
  if (diff) $("#container").css("margin-top", diff);
  var a = 0;
  if (diff) $("#container").css("margin-top", diff);
  $("#show_overlay").click(function () {
    $("#olay").fadeIn("fast");
  });
  $("#olay")
    .bind("mouseenter", function () {
      a = 1;
    })
    .bind("mouseleave", function () {
      a = 0;
      $(this).fadeOut();
    });
  $("#closeolay").click(function () {
    $("#olay").fadeOut();
  });

  $(document).tooltip({
    items: "div[data-tooltip]",
    content: function () {
      return $(this).attr("data-tooltip");
    },
  });

  $(".rateit").rateit();

  $(function () {
    $("#slider").slick({
      autoplay: true,
      autoplaySpeed: 3000,
      dots: true,
      arrows: false,
      fade: true,
      cssEase: "linear",
    });
  });
});
