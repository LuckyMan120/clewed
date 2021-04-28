$(function () {
	var emailRegex = new RegExp(/^([\w\.\-]+)@([\w\-]+)((\.[\w\-]+)+)$/i);

	function password_type(value) {

        if (value.hasClass('settings-password') && value.val() == '') return true;
		if (value.val().length < 8) {
			alert('Your password needs to contain 8 characters minimum!');
			value.focus();
			return false;
		}
		re = /^[A-Za-z]+$/;
		if (re.test(value.val())) {
			alert('Your password needs to contain at least one number!');
			value.focus();
			return false;
		}
		return true;
	}

	function checkStringNew(obj, watermark) {		
		if (obj.val() == '' || obj.val() == watermark) {
			obj.css('border', '1px solid #C52020');
			obj.focus();
			return false;
		} else {
			obj.css('border', 'none');
			return true;
		}
	}
    $(".settings-password").focusout(function() {

        password_type($(this));

    });
    $(".settings-password-match").focusout(function() {

        if ($(".settings-password").val() !== $(this).val()) {
            alert("Your passwords need to match.");
            $(this).focus();
        }

    });
    $(".show_terms").click(function () {
        $.post("/themes/maennaco/includes/homepage_posts.php?type=policy&req_type=" + $(this).attr('type'), {
        }, function (response) {
            $("#policy").html(response);
        });
        $("#policy").dialog("open");
        return false;
    });
    $(".show_confidentiality").click(function () {
        $("#policy").html("WE WILL ADD THE ACTUAL AGREEMENT TO LINK TO LATER.");
        $("#policy").dialog("open");
        return false;
    });
    $('#policy').dialog({
        modal:     true,
        autoOpen:  false,
        width:     920,
        height:    620,
        draggable: false
    });

	$('#pro-email').bind('focusout', function (e) {
        if($('#footer-tabs-2').is(':visible')) {
            e.preventDefault();
            var proEmail = $("#pro-email");
            var pro = proEmail.val();
            if (pro != '') {
                $.post("/check_email_exists.php",
                    {email: proEmail.val()},
                    function (response) {
                        if (response == 'false') {
                            alert("User with this email already exists. Please use another one.");
                            $("#pro-email").val('').focus();
                        }
                    });
                var valid = emailRegex.test(proEmail.val());
                if (!valid) {
                    alert("Format of the e-mail address is not correct");
                    $("#pro-email").val('').focus();
                }
            } else {
                return false;
            }
        }
	});
    $('#company-email').bind('focusout', function (e) {
        if($('#footer-tabs-1').is(':visible')){
            var cmpEmail = $("#company-email");
            var cmp = cmpEmail.val();
            if (cmp != '') {

                $.post("/check_email_exists.php",
                    {email: cmpEmail.val()},
                    function (response) {
                        if (response == 'false') {
                            alert("User with this email already exists. Please use another one.");
                            $("#company-email").val('').focus();
                        }
                        else if (response == 'duplicate domain') {
                            alert("There is a Clewed account for your company. Please register as a professional with your company email and indicate you are a company member to join your colleagues.");
                            $("#company-email").val('').focus();
                        }
                    });
                var valid = emailRegex.test(cmpEmail.val());
                if (!valid) {
                    alert("Format of the e-mail address is not correct");
                    $("#company-email").val('').focus();
                }
            } else {
                return false;
            }
        }
    });

/*	//$('#company-firstname').Watermark('First Name');
	//$('#company-lastname').Watermark('Last Name');
	$('#company-name').Watermark('Company Name');
	$('#company-email').Watermark('Email');
	$('#company-password').Watermark('Password');
	$('#pro-firstname').Watermark('First Name');
	$('#pro-lastname').Watermark('Last Name');
	$('#pro-email').Watermark('Email');
	$('#pro-password').Watermark('Password');*/
	$('#edit-email').Watermark('Email');
	//$('#edit-password').Watermark('Password');

	$("#footer-tabs").tabs();

    $("input:checkbox[data-group='cmp-member']").click(function() {
        // in the handler, 'this' refers to the box clicked on
        var $box = $(this);
        if ($box.is(":checked")) {
            // the group of the box is retrieved using the .attr() method
            // as it is assumed and expected to be immutable
            var group = "input:checkbox[data-group='" + $box.data("group") + "']";
            // the checked state of the group/box on the other hand will change
            // and the current value is retrieved using .prop() method
            $(group).prop("checked", false);
            $box.prop("checked", true);
        } else {
            $box.prop("checked", false);
        }
        if ($box.attr('id') == 'pro-member' && $box.is(':checked')) $("#"+$box.data('box')).show();
        else if ($box.attr('id') == 'pro-member' && !$box.is(':checked')) $("#"+$box.data('box')).hide();
        else if ($box.attr('id') == 'pro-no-member') $("#"+$box.data('box')).hide();
    });

    /* start => new click login and register*/

    $(".nav-login").click(function () {
        $("#footer-login-dialog").parent().children(".ui-dialog-titlebar").remove();
        $("#footer-login-dialog").dialog("open");
        $("#login-email").blur();
    });

    $(".nav-register").click(function () {
        //$("#footer-register-dialog #footer-tabs-2").hide();
        //$("#footer-register-dialog #footer-tabs-1").show();
        //$("#footer-tabs #b-2").css('background', '#D0D2D2');
        //$("#footer-tabs #b-1").css('background', '#00a2bf');
        $("#footer-register-dialog").parent().children(".ui-dialog-titlebar").remove();
        $("#footer-register-dialog").dialog("open");
        //window.utype = 'company';
        $(".agree_terms").show();
        if(location.hash == '#register-step-1') {
            init_register_step_1();
        } else if(location.hash == '#register-step-2')
            init_register_step_2();
        else
            init_register_step_1();
    });
    $(".custom-get-button").click(function(){
        $("#footer-register-dialog").parent().children(".ui-dialog-titlebar").remove();
        $("#footer-register-dialog").dialog("open");
        //window.utype = 'company';
        $(".agree_terms").show();
        if(location.hash == '#register-step-1') {
            init_register_step_1();
        } else if(location.hash == '#register-step-2')
            init_register_step_2();
        else
            init_register_step_1();
    });
    /* end => new click login and register*/


	$(".header-login").click(function () {
        $("#footer-login-dialog").parent().children(".ui-dialog-titlebar").remove();
        $("#footer-login-dialog").dialog("open");
        $("#login-email").blur();
    });

    //Submit login form on Enter button

    $("#login-email").keyup(function (e) {
        if (e.keyCode == 13) {
            $(this).closest('form').submit();
        }
    });

    $("#login-password").keyup(function (e) {
        if (e.keyCode == 13) {
            $(this).closest('form').submit();
        }
    });

	$(".header-register").click(function () {
        $("#footer-register-dialog").parent().children(".ui-dialog-titlebar").remove();
        $("#footer-register-dialog").dialog("open");
		//window.utype = 'company';
        $(".agree_terms").show();
        if(location.hash == '#register-step-1') {
            init_register_step_1();
        } else if(location.hash == '#register-step-2')
            init_register_step_2();
        else
            init_register_step_1();
    });

	$("#home-register-new").click(function () {
        //if($(this).attr("data-type")=="company"){
			////$("#footer-register-dialog #footer-tabs-1").show();
			////$("#footer-register-dialog #footer-tabs-2").hide();
			////$("#footer-tabs #b-2").css('background', '#D0D2D2');
			////$("#footer-tabs #b-1").css('background', '#00a2bf');
        //    show_hide_new('footer-tabs-1','footer-tabs-2');
        //}else{
			////$("#footer-register-dialog #footer-tabs-2").show();
			////$("#footer-register-dialog #footer-tabs-1").hide();
			////$("#footer-tabs #b-1").css('background', '#D0D2D2');
			////$("#footer-tabs #b-2").css('background', '#00a2bf');
        //    show_hide_new('footer-tabs-2','footer-tabs-1');
        //}
        //$("#footer-register-dialog").parent().children(".ui-dialog-titlebar").remove();
        //$("#footer-register-dialog").dialog("open");
        showRegDlg();
        init_register_step_1();
    });

    $(".pre-footer-wrapper a").click(function(){
        $("#home-register-new").trigger('click');
    });

	$(".request_demo").click(function () {
        $(".ui-dialog-titlebar").remove();

		if($(this).attr("data-type")=="company"){
			//$("#footer-register-dialog #footer-tabs-1").show();
			//$("#footer-register-dialog #footer-tabs-2").hide();
			//$("#footer-tabs #b-2").css('background', '#D0D2D2');
			//$("#footer-tabs #b-1").css('background', '#00a2bf');
			//window.utype = 'company';
            show_hide_new('footer-tabs-1','footer-tabs-2');
		}else{
			//$("#footer-register-dialog #footer-tabs-2").show();
			//$("#footer-register-dialog #footer-tabs-1").hide();
			//$("#footer-tabs #b-1").css('background', '#D0D2D2');
			//$("#footer-tabs #b-2").css('background', '#00a2bf');
			//window.utype = 'professional';
            show_hide_new('footer-tabs-2','footer-tabs-1');
		}
        //$(".reg-hint").hide();
        //$(".reg-choose").hide();
		//$(".reg-but1").hide();
		//$(".reg-but2").hide();
		//$(".reg-choose-btn-divider").hide();
        $("#footer-register-dialog").children(".reg-top").html("Schedule a Demo");
        $(".form-submit1").children(".ui-button-text").html("Request");
        $('.agree_terms').hide();
        $(".form-submit1").attr('rel','request');
        $("#footer-register-dialog").dialog("open");
        return false;
    });

	window.showRegDlg = function () {
        $("#footer-login-dialog").dialog("close");
		$(".header-register").trigger( "click" );
    };
    

	$('#login-email').Watermark('Email');
    $('#login-password').Watermark('Password');
	
	$("#footer-login-dialog").dialog({
        modal: true,
        autoOpen: false,
        draggable: true,
        resizable: false,
        width: "350",
        open: function() {
            $(".form-submit1").children(".ui-button-text").html("Log in");
        },
        buttons: [
            {
                text: "Log in",
                click: function () {
                    $("#footer-login-dialog").find('form').trigger('submit');
                },
                "class": "form-submit1"
            },
            {
                text: "Cancel",
                click: function () {
                    $(this).dialog("close");
                },
                "class": "form-submit2"
            }
        ]
    });
	$("#footer-register-dialog").dialog({
        modal: true,
        autoOpen: false,
        draggable: true,
        resizable: false,
        width: "730",
        open: function() {
            $(".form-submit1").children(".ui-button-text").html("Register");
        },
        buttons: [
            {
                text: "Register",
                click: function (e) {
                    var clicked = $(e.target).parent();
                    var abs_url = window.location.protocol + "//" + window.location.host + "/";
                    var that = this;
                    fValid = true;
                    selTab = window.utype;
                    if (selTab == 'company') {
                        fValid = checkStringNew($("#company-firstname"), 'First Name') && fValid;
                        fValid = checkStringNew($("#company-lastname"), 'Last Name') && fValid;
                        fValid = checkStringNew($("#company-name"), 'Company Name') && fValid;
                        fValid = checkStringNew($("#company-email"), 'Email') && fValid;
                        fValid = checkStringNew($("#company-password"), 'Password') && fValid;
                        fValid = password_type($('#company-password')) && fValid;
                        fValid = checkStringNew($("#company-industry"), 'industry') && fValid;
                        //fValid = checkStringNew($("#company-revenue"), 'revenue') && fValid;
                        if (!fValid) {
                            alert('Please fill in the required fields.');
                        }
                        if (clicked.attr('rel') == 'request') {

                            ajax_path = abs_url+"register_new_user.php?type=company&request=true";

                        }

                        else
                        {
                            if (fValid && !$('#cmp-agree').is(':checked')) {
                                fValid = false;
                                alert('You have to read and agree with terms and privacy policy!');
                            }
                            ajax_path = abs_url+"register_new_user.php?type=company";
                        }

                        if (fValid) {
                            clicked.attr('disabled', true);

                            $.post(ajax_path,
                                {firstname: $("#company-firstname").val(), lastname: $("#company-lastname").val(), name: $("#company-name").val(), email: $("#company-email").val(), password: $("#company-password").val(), industry: $("#company-industry").val(), revenue: $("#company-revenue").val(), cmp_referral: $("#company-referral").val() },
                                function (response) {
                                    alert(response);
                                    if (-1 != response.indexOf('Login to get started')) {
                                        $(that).dialog("close");
                                        $(".header-login").trigger('click');
                                    }
                                    else if(-1 != response.indexOf('company already has an account')) {
                                        $(that).find('.reg-but2').trigger('click');
                                    }
                                    else if(response != 'Security code is not correct!') {
                                        location.reload();
                                    }

                                    clicked.attr('disabled', false);
                                });
                        }
                    }
                    else {
                        fValid = checkStringNew($("#pro-type"), 'iam') && fValid;
                        fValid = checkStringNew($("#pro-firstname"), 'First Name') && fValid;
                        fValid = checkStringNew($("#pro-lastname"), 'Last Name') && fValid;
                        fValid = checkStringNew($("#pro-email"), 'Email') && fValid;
                        fValid = checkStringNew($("#pro-password"), 'Password') && fValid;
                        fValid = password_type($('#pro-password')) && fValid;
                        fValid = checkStringNew($("#pro-experties"), 'experties') && fValid;
                        if ($("input[name='pro-member']").is(":checked")) {
                            fValid = checkStringNew($("#pro-cmp-email"), 'Company email') && fValid;
                        }
                        if (!fValid) {
                            alert('Please fill in the required fields.');
                        }
                        if (clicked.attr('rel') == 'request') {

                            ajax_path = abs_url+"register_new_user.php?type=professional&request=true";

                        }

                        else {
                            if (fValid && !$('#pro-agree').is(':checked')) {
                                fValid = false;
                                alert('You have to read and agree with terms and privacy policy!');
                            }
                            ajax_path = abs_url+"register_new_user.php?type=professional";
                        }
                        if (fValid) {
                            clicked.attr('disabled', true);
                            $.post(ajax_path,
                                {firstname: $("#pro-firstname").val(), lastname: $("#pro-lastname").val(), experties: $("#pro-experties").val(), email: $("#pro-email").val(), password: $("#pro-password").val(), pro_type: $("#pro-type").val(),pro_member: $("#pro-member").is(":checked"),pro_referral:$("#pro-referral").val(),pro_cmp_email: $("#pro-cmp-email").val() },
                                function (response) {
                                    alert(response);
                                    if (-1 != response.indexOf('Login to get started')) {
                                        $(that).dialog("close");
                                        $(".header-login").trigger('click');
                                    }
                                    else if(response != 'Security code is not correct!') {
                                        location.reload();
                                    }

                                    clicked.attr('disabled', false);
                                });
                        }
                    }
                },
                "class": "form-submit1"
            },
            {
                text: "Cancel",
                click: function () {
                    $(this).dialog("close");
					$("#footer-register-dialog").children(".reg-top").html("JOIN CLEWED, IT'S FREE TO START");
					//$(".reg-choose").show();
                    //$(".reg-hint").show();
					//$(".reg-but1").show();
					//$(".reg-but2").show();
					$(".form-submit1").attr('rel','');
                    location.hash = "";
                },
                "class": "form-submit2"
            }
        ]        
	});

	/*$("input:text").focus(function () {
		$(this).val('');
	});*/

    if('#register-step-1' == location.hash) {
        showRegDlg();
        init_register_step_1();
    } else if('#register-step-2' == location.hash) {
        showRegDlg();
        init_register_step_1();
    }

    $(window).bind('hashchange', function() {
        if('#register-step-1' == location.hash) {
            showRegDlg();
            init_register_step_1();
        } else if('#register-step-2' == location.hash) {
            showRegDlg();
            init_register_step_2();
        }
    });
});

function show_hide_new(id_show, id_hide, bidblue, bidblack) {
    if (id_show == 'footer-tabs-1') {
        window.utype = 'company';
    } else if (id_show == 'footer-tabs-2') {
        window.utype = 'professional';
    }
    document.getElementById(id_show).style.display = 'block';
    document.getElementById(id_hide).style.display = 'none';
    //document.getElementById(bidblue).style.backgroundColor = '#00a2bf';
    //document.getElementById(bidblack).style.backgroundColor = '#D0D2D2';
    init_register_step_2();
}

function init_register_step_1() {
    var $dialog = $('#footer-register-dialog');
    $dialog.find('.register-step-1-buttons').show();
    $dialog.find('#footer-tabs-1').hide();
    $dialog.find('#footer-tabs-2').hide();
    $dialog.parent().find('.ui-dialog-buttonset').hide();
    $dialog.dialog('option', 'width', 730);
    $dialog.dialog('option', 'position', 'center');
    if(location.hash != '#register-step-1') {
        location.hash = '#register-step-1';
    }
}

function init_register_step_2() {
    var $dialog = $('#footer-register-dialog');
    $dialog.find('.register-step-1-buttons').hide();
    $dialog.parent().find('.ui-dialog-buttonset').show();
    $dialog.dialog('option', 'width', 480);
    $dialog.dialog('option', 'position', 'center');
    if(location.hash != '#register-step-2') {
        location.hash = '#register-step-2';
    }
    if(window.utype == 'professional')
        $dialog.find('#footer-tabs-2').show();
    else
        $dialog.find('#footer-tabs-1').show();
}

