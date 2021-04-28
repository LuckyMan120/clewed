$(function () {
	$('#pro').click(function (e) {
		$('div.links a').removeClass('active');
		if (!$(e.currentTarget).hasClass('active')) {
			$(e.currentTarget).addClass('active');
			$('.jsblock').hide();
			$('#problock').show();
		}
	});
	$('#company').click(function (e) {
		$('div.links a').removeClass('active');
		if (!$(e.currentTarget).hasClass('active')) {
			$(e.currentTarget).addClass('active');
			$('.jsblock').hide();
			$('#companiesblock').show();
		}
	});
	$('#policy').dialog({
		modal:     true,
		autoOpen:  false,
		width:     920,
		height:    620,
		draggable: false
	});
	$('#learn-more-dialog').dialog({
		modal:     true,
		autoOpen:  false,
		width:     920,
		height:    620,
		draggable: false
	});
	$('#learn-more').click(function () {
		$('#learn-more-dialog').dialog('open');
		return false;
	});
	//window.showRegisterDialogNew = function() {
	//	$("#footer-register-dialog").dialog('open');
	//	$("#footer-register-dialog").parent().children(".ui-dialog-titlebar").remove();
	//	if ($('#company').hasClass('active')) {
	//		show_hide('footer-tabs-1', 'footer-tabs-2', 'b-1', 'b-2');
	//	} else if ($('#pro').hasClass('active')) {
	//		show_hide('footer-tabs-2', 'footer-tabs-1', 'b-2', 'b-1');
	//	} else {
	//		$("#b-2").css('background-color', '#D0D2D2');
	//	}
	//};
	$('#home-register-new1').click(function (e) {
		e.preventDefault();
		showRegDlg();
		init_register_step_1();
	});
});
