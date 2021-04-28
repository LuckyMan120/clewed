'use strict';
$(function () {
	$(document).tooltip({
		items:   "a[data-tooltip], span[data-tooltip], div[data-tooltip],li[data-tooltip],td[data-tooltip]",
		content: function () {
			return $(this).attr('data-tooltip')
		}
	});
	$(".card-tool").tooltip({ items: "[card-tooltip]", tooltipClass: "card_tip", content: function () {
		return $("div[uid='" + $(this).attr("card-tooltip") + "']").html();
	}});
    $(".profile_details").live("click", function (e) {
		e.preventDefault();
		var uid = $(this).attr('id').replace('pro_id', '');
		$.post(
			"/themes/maennaco/includes/fetch_ins.php?type=pro_detail&uid=" + uid,
			function (response) {
				$("#eveditdlg").dialog({
					autoOpen:      true,
					width:         650,
					resizable:     false,
					draggable:     false,
					height:        400,
					closeText:     'hide',
					buttons:       {},
					closeOnEscape: true,
					modal:         true
				}).html(response);
			}
		);
	});
	$('#calendar').fullCalendar({
		header:      {
			left:   'prev ',
			center: 'title',
			right:  'next'
		},
		editable:    false,
		events:      "/themes/maennaco/includes/json-events.php?pagename=insights",
		eventRender: function (event, element, view) {
			return event.start.getMonth() == view.start.getMonth();
		},
		eventDrop:   function (event, delta) {
			alert(event.title + ' was moved ' + delta + ' days\n' +
				'(should probably update your database)');
		},
		loading:     function (bool) {
			if (bool) {
				$('#loading').show();
			} else {
				$('#loading').hide();
			}
			$('.fc-event-hori').css({height: '35px', top: '-=24'});
		}
	});
	$("#tab2").click(function () {
		cmpClick();
	});
	$(".filter-parent").click(function () {
		//$("div[id$='-filter']").hide();
		var rel = $("#" + $(this).attr('rel'));
		if (rel.hasClass('open')) {
			rel.hide();
			rel.removeClass('open')
		} else {
			rel.show();
			rel.addClass('open');
		}
	});

	$(".filter-entry").click(function () {
        var $row = $(this),
            $filter = $row.parent(),
            filterType = $row.attr('filter'),
            value = $row.attr('rel'),
            filterState = $filter.parent().data('state') || {},
            url = '/themes/maennaco/includes/fetch_ins.php?';

		filterState['offer-type'] = $('#offerType').val();

        $filter.find('.filter-entry').each(function () {
			$(this).removeClass('filter-active');
		});

		$row.addClass('filter-active');
        filterState[filterType] = value;
        $filter.parent().data('state', filterState);

        var query = [];
        $.each(Object.keys(filterState), function(i, v){
            if(filterState[v])
                query.push(v + '=' + filterState[v]);
        });

        url += query.join('&');
		$.post(
            url,
            {},
            function (response) {
                $("#posting").fadeTo('fast', 0, function () {
                    $(this).html(response)
                }).fadeTo('slow', 100);
            }
        );
	});

	$(".contrib").click(function () {
		//$("#contrib-dialog").dialog("open");
        showLoginDlg();
	});
	$(".login-start").click(function () {
		$("#contrib-dialog").dialog("close");
		//$("#login-dialog").dialog("open");
		//$("#login-email").blur();
		$(".header-login").trigger( "click" );
	});
	$(".show_more_cmp").click(function () {
		var cat = '';
		var month = '';
		$.post("/themes/maennaco/includes/fetch_ins.php?rel=" + $(this).attr('rel') + cat + month, {
		}, function (response) {
			$("#posting").fadeTo('fast', 0,function () {
				$(this).append(response)
			}).fadeTo('slow', 100);
		});
		$(this).fadeTo('slow', 0, function () {
			$(this).remove()
		});
	});
	window.utype = 'professional';
	var emailRegex = new RegExp(/^([\w\.\-]+)@([\w\-]+)((\.[\w\-]+)+)$/i);
	window.loadMoreCmp = function (el) {
		var cat = el.getAttribute('category');
		var month = el.getAttribute('month');
		var ind;
		if (cat !== null) ind = '&category=';
		$.post("/themes/maennaco/includes/fetch_ins.php?_page=" + el.getAttribute('page') + ind + month, {
		}, function (response) {
			$("#posting").fadeTo('fast', 0,function () {
				$(this).append(response)
			}).fadeTo('slow', 100);
		});
		el.remove();
	};
	window.showContMessage = function () {
		//$("#contrib-dialog").dialog("open");
        showLoginDlg();
	};
	window.cmpClick = function () {
		//$("#comp-dialog").dialog("open");
		showLoginDlg();
	};
	window.showFullRegistrationDialog = function () {
		$("#login-dialog").dialog("close");
		$(".header-register").trigger( "click" );
		//window.showRegisterDialog();
	};
	window.showLoginDlg = function () {
		console.log("asdsdf");
		$("#login-dialog").dialog("open");
		$("#login-email").blur();
	};

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

	var companyPassword = $('#company-password');
	var proPassword = $('#pro-password');
	var companyEmail = $('#company-email');
	var proEmail = $('#pro-email');
	companyPassword.focusout(function () {
		password_type($('#company-password'));
	});
	proPassword.focusout(function () {
		password_type($('#pro-password'));
	});
/*	$('#company-firstname').Watermark('First Name');
	$('#company-lastname').Watermark('Last Name');
	$('#company-name').Watermark('Company Name');
	companyEmail.Watermark('Email');
	companyPassword.Watermark('Password');
	$('#pro-firstname').Watermark('First Name');
	$('#pro-lastname').Watermark('Last Name');
	proEmail.Watermark('Email');
	proPassword.Watermark('Password');*/
	$('#edit-email').Watermark('Email');
	//$('#edit-password').Watermark('Password');
	$('#login-email').Watermark('Email');
	$('#login-password').Watermark('Password');
	$("#feedback").click(function (event) {
		event.preventDefault();
		$.post(
			'/themes/maennaco/includes/homepage_posts.php?type=contactus',
			function (response) {
				$(".dialog-left-column a").removeClass('dialog-selected');
				$('#contactus').addClass('dialog-selected');
				$(".dialog-right-column").html(response).fadeIn("slow");
				$('#contact-name').Watermark('Name');
				$('#contact-email').Watermark('Email');
				$('#contact-subject').Watermark('Subject');
				$('#contact-message').Watermark('Message');
				$("#learn-more-dialog").dialog("open");
			}
		);
	});
	var body = $("body");
	body.delegate("button.contactus", "click", function () {
		var fValid = true;
		fValid = fValid && checkString($("#contact-name"), 'Name');
		fValid = fValid && checkString($("#contact-email"), 'Email');
		fValid = fValid && checkString($("#contact-subject"), 'Subject');
		fValid = fValid && checkString($("#contact-message"), 'Message');
		if (fValid) {
			$.post("/themes/maennaco/includes/homepage_posts.php?type=feedback",
				{name: $("#contact-name").val(), email: $("#contact-email").val(), subject: $("#contact-subject").val(), message: $("#contact-message").val() },
				function (response) {
					if (response == 0) {
						$("#contact-name").val('');
						$("#contact-email").val('');
						$("#contact-subject").val('');
						$("#contact-message").val('');
						alert("Your information was successfully sent");
					} else {
						$("#contact-name").val('');
						$("#contact-email").val('');
						$("#contact-subject").val('');
						$("#contact-message").val('');
						alert("Message couldn't be send. Please try again!");
					}
					$("#learn-more-dialog").dialog("open");
				}
			);
		}
	});
	body.delegate("a.pagree", "click", function () {
		$.post("/themes/maennaco/includes/homepage_posts.php?type=terms", {
		}, function (response) {
			$("#right-column-left").html(response).fadeIn("slow");
		});
	});
	$(".show_terms").click(function () {
		$.post("/themes/maennaco/includes/homepage_posts.php?type=policy&req_type=" + $(this).attr('type'), {
		}, function (response) {
			$("#policy").html(response);
		});
		$("#policy").dialog("open");
		return false;
	});
	$("#tabs").tabs();
	//companyEmail.bind('focusout', function () {
	//	var com = companyEmail.val();
	//	if (com != '') {
	//		$.post("/check_email_exists.php",
	//			{email: companyEmail.val()},
	//			function (response) {
	//				if (response == 'false') {
	//					alert("User with this email already exists. Please use another one.");
	//					$("#company-email").val('').focus();
	//				}
	//			});
	//		var valid = emailRegex.test(companyEmail.val());
	//		if (!valid) {
	//			alert("Format of the e-mail address is not correct");
	//			$("#company-email").val('').focus();
	//		}
	//	}
	//});
	//proEmail.bind('focusout', function () {
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
	//	}
	//});
	$("#learn-more-dialog").dialog({
		modal:     true,
		autoOpen:  false,
		width:     920,
		height:    620,
		draggable: false
	});
	$("#policy").dialog({
		modal:     true,
		autoOpen:  false,
		width:     920,
		height:    620,
		draggable: false
	});
	$("#contrib-dialog").dialog({
		modal:     true,
		autoOpen:  false,
		draggable: true,
		resizable: false,
		width:     350
	});
	$("#comp-dialog").dialog({
		modal:     true,
		autoOpen:  false,
		draggable: true,
		resizable: false,
		width:     350
	});
	$("#login-dialog").dialog({
		modal:     true,
		autoOpen:  false,
		draggable: true,
		resizable: false,
		width:     350,
		buttons:   [
			{
				text:    "Log in",
				click:   function () {
					$("#login-dialog").find('form').submit();
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
	$('#learn-more').click(function () {
		$('#learn-more-dialog').dialog('open');
		return false;
	});
	$('.reg-but1').click(function () {
		$(".ui-dialog-buttonset").show();
	});
	$('.reg-but2').click(function () {
		$(".ui-dialog-buttonset").show();
	});
/*	$("input:text").focus(function () {
		$(this).val('');
	});*/
	var page_height = $(window).height();
	var diff = parseInt(page_height) - 480;
	diff = parseInt(diff / 2);
	if (diff) $('#container').css('margin-top', diff);
	var a = 0;
	if (diff) $('#container').css('margin-top', diff);
	$('#show_overlay').click(function () {
		$('#olay').fadeIn("fast");
	});
	$('#olay').bind("mouseenter",function () {
		a = 1;
	}).bind("mouseleave", function () {
		a = 0;
		$(this).fadeOut();
	});
	$('#closeolay').click(function () {
		$('#olay').fadeOut();
	});

    $('.offer-join-btn').click(function(){
        location.hash = 'join';
        var $footerLoginDialog = $('#footer-login-dialog'),
            $footerLoginDialogUri = $footerLoginDialog.find('input[name=uri]'),
            footerLoginDialogUri = $footerLoginDialogUri.val(),
            $loginDialog = $('#login-dialog'),
            $loginDialogUri = $loginDialog.find('input[name=uri]'),
            loginDialogUri = $loginDialogUri.val();

        if(-1 === footerLoginDialogUri.indexOf('#join'))
            $footerLoginDialogUri.val(footerLoginDialogUri + '#join');

        if(-1 === loginDialogUri.indexOf('#join'))
            $loginDialogUri.val(loginDialogUri + '#join');
    });
});
