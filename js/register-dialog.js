'use strict';
function checkString(obj, watermark) {
	if (obj.val() == ' ' || obj.val() == watermark) {
		obj.css('border', '1px solid #C52020');
		obj.focus();
		return false;
	} else {
		obj.css('border', 'none');
		return true;
	}
}
$(function () {
	var emailRegex = new RegExp(/^([\w\.\-]+)@([\w\-]+)((\.[\w\-]+)+)$/i);
	var registerDialog = $("#register-dialog");
	$('.show_terms').click(function () {
		$.post(
			'/themes/maennaco/includes/homepage_posts.php?type=policy&req_type=' + $(this).attr('type'),
			function (response) {
				$("#policy").html(response);
			}
		);
		$("#policy").dialog("open");
		return false;
	});
	//$("#company-email").bind('focusout', function () {
	//	var com = $("#company-email").val();
	//	if (com != '') {
	//		$.post("check_email_exists.php",
	//			{email: $("#company-email").val()},
	//			function (response) {
	//				if (response == 'false') {
	//					alert("User with this email already exists. Please use another one.");
	//					$("#company-email").val('').focus();
	//				}
	//			}
	//		);
	//		var valid = emailRegex.test($("#company-email").val());
	//		if (!valid) {
	//			alert("Format of the e-mail address is not correct");
	//			$("#company-email").val('').focus();
	//		}
	//	} else {
	//		return false;
	//	}
	//});
	//$("#pro-email").bind('focusout', function () {
	//	var pro = $("#pro-email").val();
	//	if (pro != '') {
	//		$.post("check_email_exists.php",
	//			{email: $("#pro-email").val()},
	//			function (response) {
	//				if (response == 'false') {
	//					alert("User with this email already exists. Please use another one.");
	//					$("#pro-email").val('').focus();
	//				}
	//			}
	//		);
	//		var valid = emailRegex.test($("#pro-email").val());
	//		if (!valid) {
	//			alert("Format of the e-mail address is not correct");
	//			$("#pro-email").val('').focus();
	//		}
	//	} else {
	//		return false;
	//	}
	//});
	window.showRegisterDialog = function() {
		registerDialog.dialog('open');
		registerDialog.parent().children(".ui-dialog-titlebar").remove();
		if ($('#company').hasClass('active')) {
			show_hide('tabs-1', 'tabs-2', 'b-1', 'b-2');
		} else if ($('#pro').hasClass('active')) {
			show_hide('tabs-2', 'tabs-1', 'b-2', 'b-1');
		} else {
			$("#b-2").css('background-color', '#D0D2D2');
		}
	};
	$('#home-register').click(function (e) {
		e.preventDefault();
		window.showRegisterDialog();
		return false;
	});
	registerDialog.dialog({
		modal:     true,
		autoOpen:  false,
		draggable: true,
		resizable: false,
		width:     350,
		buttons:   [
			{
				text:    "Register",
				click:   function (e) {
					var clicked = $(e.target);
					var fValid = true;
					var selTab = window.utype;
					if (selTab == 'company') {
						fValid = fValid && checkString($("#company-firstname"), 'First Name');
						fValid = fValid && checkString($("#company-lastname"), 'Last Name');
						fValid = fValid && checkString($("#company-name"), 'Company Name');
						fValid = fValid && checkString($("#company-email"), 'Email');
						fValid = fValid && checkString($("#company-password"), 'Password');
						fValid = fValid && checkString($("#company-industry"), 'industry');
						fValid = fValid && checkString($("#company-revenue"), 'revenue');
						if (!fValid) {
							alert('Please fill in the required fields.');
						}
						if (fValid && !$('#cmp-agree').is(':checked')) {
							fValid = false;
							alert('You have to read and agree with terms and privacy policy!');
						}
						if (fValid) {
							clicked.parent().attr('disabled', true);
							$.post(
								'register_new_user.php?type=company',
								{
									firstname: $("#company-firstname").val(),
									lastname:  $("#company-lastname").val(),
									name:      $("#company-name").val(),
									email:     $("#company-email").val(),
									password:  $("#company-password").val(),
									industry:  $("#company-industry").val(),
									revenue:   $("#company-revenue").val()
								},
								function (response) {
									alert(response);
									if (response != 'Security code is not correct!') {
										location.reload();
									}
								}
							);
						}
					} else {
						fValid = fValid && checkString($("#pro-type"), 'iam');
						fValid = fValid && checkString($("#pro-firstname"), 'First Name');
						fValid = fValid && checkString($("#pro-lastname"), 'Last Name');
						fValid = fValid && checkString($("#pro-email"), 'Email');
						fValid = fValid && checkString($("#pro-password"), 'Password');
						fValid = fValid && checkString($("#pro-experties"), 'experties');
						if (!fValid) {
							alert('Please fill in the required fields.');
						}
						if (fValid && !$('#pro-agree').is(':checked')) {
							fValid = false;
							alert('You have to read and agree with terms and privacy policy!');
						}
						if (fValid) {
							clicked.parent().attr('disabled', true);
							$.post(
								'register_new_user.php?type=proffesional',
								{
									firstname: $("#pro-firstname").val(),
									lastname:  $("#pro-lastname").val(),
									experties: $("#pro-experties").val(),
									email:     $("#pro-email").val(),
									password:  $("#pro-password").val(),
									pro_type:  $("#pro-type").val()
								},
								function (response) {
									alert(response);
									if (response != 'Security code is not correct!') {
										location.reload();
									}
								}
							);
						}
					}
				},
				"class": "form-submit1"
			},
			{
				text:    "Cancel",
				click:   function () {
					$(this).dialog("close");
				},
				"class": "form-submit2"
			}
		]
	});
	//start = > custom-form-home-page-register-company
	$('.form-submit-register-company').click(function () {

		var fValid = true;

		fValid = checkString($("#company-firstname"), 'First Name') && fValid;
		fValid = checkString($("#company-lastname"), 'Last Name') && fValid;
		fValid = checkString($("#company-name"), 'Company Name') && fValid;
		fValid = checkString($("#company-email"), 'Email') && fValid;
		fValid = checkString($("#company-password"), 'Password') && fValid;
		fValid = checkString($("#company-industry"), 'industry') && fValid;
		fValid = checkString($("#company-revenue"), 'revenue') && fValid;
		if (!fValid) {
			alert('Please fill in the required fields.');
		}
		if (fValid && !$('#cmp-agree').is(':checked')) {
			fValid = false;
			alert('You have to read and agree with terms and privacy policy!');
		}
		if (fValid) {
			// clicked.parent().attr('disabled', true);
			$.post(
				'register_new_user.php?type=company',
				{
					firstname: $("#company-firstname").val(),
					lastname:  $("#company-lastname").val(),
					name:      $("#company-name").val(),
					email:     $("#company-email").val(),
					password:  $("#company-password").val(),
					industry:  $("#company-industry").val(),
					revenue:   $("#company-revenue").val()
				},
				function (response) {
					alert(response);
					if (response != 'Security code is not correct!') {
						location.reload();
					}
				}
			);

		}
	});
	//end = > custom-form-home-page-register-company

	$('#tabs').tabs();
/*	//$('#company-firstname').Watermark('First Name');
	$('#company-lastname').Watermark('Last Name');
	$('#company-name').Watermark('Company Name');
	$('#company-email').Watermark('Email');
	$('#company-password').Watermark('Password');
	$('#pro-firstname').Watermark('First Name');
	$('#pro-lastname').Watermark('Last Name');
	$('#pro-email').Watermark('Email');*/
	//$('#pro-password').Watermark('Password');
	$('#edit-email').Watermark('Email');
	//$('#edit-password').Watermark('Password');

});

function show_hide(id_show, id_hide, bidblue, bidblack) {
	if (id_show == 'tabs-1') {
		window.utype = 'company';
	} else if (id_show == 'tabs-2') {
		window.utype = 'professional';
	}
	document.getElementById(id_show).style.display = 'block';
	document.getElementById(id_hide).style.display = 'none';
	document.getElementById(bidblue).style.backgroundColor = '#00a2bf';
	document.getElementById(bidblack).style.backgroundColor = '#D0D2D2';
}
