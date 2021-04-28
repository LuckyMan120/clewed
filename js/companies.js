$(function () {
    $(document).tooltip({ items: "a[data-tooltip], span[data-tooltip]", content: function () {
        return $(this).attr('data-tooltip')
    } });
    $(".filter-parent").click(function () {
        $("div[id$='-filter']").hide();
        rel = $("#" + $(this).attr('rel'));
        if (rel.hasClass('open')) {
            rel.hide();
            rel.removeClass('open')
        }
        else {
            rel.show();
            rel.addClass('open');
        }
    });
    $(".filter-entry").click(function () {
        $(".filter-entry").each(function () {
            $(this).removeClass('filter-active');
        });
        $(this).addClass('filter-active');
        $.post("/themes/maennaco/includes/fetch_cmp.php?" + $(this).attr('filter') + "=" + $(this).attr('rel'),
            function (response) {
                $("#cardholder").fadeTo('fast', 0,function () {
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
        /*$("#login-dialog").dialog("open");
        $("#login-email").blur();*/
		$(".header-login").trigger( "click" );
    });
    $('.show_more_cmp').click(function () {
        var ind = '';
        var rev = '';
        var type = $(this).attr('type');
        console.log(type);
        if ($(this).is("[industry]")) {
            ind = '&industry=' + $(this).attr('industry');
        }
        if ($(this).is("[revenue]")) {
            rev = '&revenue=' + $(this).attr('revenue');
        }
        $.post(
            '/themes/maennaco/includes/fetch_cmp.php?rel=' + $(this).attr('rel') + ind + rev+'&type='+$(this).attr('type'),
            function (response) {
                $('.'+type).fadeTo('fast', 0,function () {
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
        ind = el.getAttribute('industry');
        rev = el.getAttribute('revenue');
        if (ind !== null) {
            ind = '&industry=' + ind;
        }
        if (rev !== null) {
            rev = '&revenue=' + rev;
        }
        $.post("/themes/maennaco/includes/fetch_cmp.php?_page=" + el.getAttribute('page') + ind + rev,
            function (response) {
                $("#cardholder").fadeTo('fast', 0,function () {
                    $(this).append(response)
                }).fadeTo('slow', 100);
            }
        );
        el.remove();
    }
    window.showContMessage = function () {
        //$("#contrib-dialog").dialog("open");
        showLoginDlg();
    };
    window.cmpClick = function (rel) {
        $("input[name='ref']").val(rel);
        //$("#comp-dialog").dialog("open");
        showLoginDlg();
    };
    window.showRegDlg = function () {
        $("#register-dialog #tabs-1").hide();
        $("#register-dialog #tabs-2").show();
        $("#register-dialog").parent().children(".ui-dialog-titlebar").remove();
        $("#register-dialog").dialog("open");
    };
    window.showLoginDlg = function () {
        /*$("#login-dialog").dialog("open");
        $("#login-email").blur();*/
		// $(".header-login").trigger( "click" );
    };
    function password_type(value) {
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

    function checkString(obj, watermark) {
        if (obj.val() == '' || obj.val() == watermark) {
            obj.css('border', '1px solid #C52020');
            obj.focus();
            return false;
        } else {
            obj.css('border', 'none');
            return true;
        }
    }

/*    $('#company-firstname').Watermark('First Name');
    $('#company-lastname').Watermark('Last Name');
    $('#company-name').Watermark('Company Name');
    $('#company-email').Watermark('Email');
    $('#company-password').Watermark('Password');
    $('#pro-firstname').Watermark('First Name');
    $('#pro-lastname').Watermark('Last Name');
    $('#pro-email').Watermark('Email');
    $('#pro-password').Watermark('Password');*/
    $('#edit-email').Watermark('Email');
    //$('#edit-password').Watermark('Password');
    $('#login-email').Watermark('Email');
    $('#login-password').Watermark('Password');
    $("#feedback").click(function (event) {
        event.preventDefault();
        $.post("/themes/maennaco/includes/homepage_posts.php?type=contactus",
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
    $("body").delegate("button.contactus", "click", function () {
        fValid = true;
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
                });
        }
    });
    $("body").delegate("a.pagree", "click", function () {
        $.post("/themes/maennaco/includes/homepage_posts.php?type=terms",
            function (response) {
                $("#right-column-left").html(response).fadeIn("slow");
            }
        );
    });
    $(".show_terms").click(function () {
        $.post("/themes/maennaco/includes/homepage_posts.php?type=policy&req_type=" + $(this).attr('type'),
            function (response) {
                $("#policy").html(response);
            }
        );
        $("#policy").dialog("open");
        return false;
    });
    $("#tabs").tabs();
    //$("#company-email").bind('focusout', function () {
    //    var com = $("#company-email").val();
    //    if (com != '') {
    //        $.post("/check_email_exists.php",
    //            {email: $("#company-email").val()},
    //            function (response) {
    //                if (response == 'false') {
    //                    alert("User with this email already exists. Please use another one.");
    //                    $("#company-email").val('').focus();
    //                }
    //            });
    //        var valid = emailRegex.test($("#company-email").val());
    //        if (!valid) {
    //            alert("Format of the e-mail address is not correct");
    //            $("#company-email").val('').focus();
    //        }
    //    }
    //    else {
    //        return false;
    //    }
    //});
    //$("#pro-email").bind('focusout', function () {
    //    var pro = $("#pro-email").val();
    //    if (pro != '') {
    //        $.post("/check_email_exists.php",
    //            {email: $("#pro-email").val()},
    //            function (response) {
    //                if (response == 'false') {
    //                    alert("User with this email already exists. Please use another one.");
    //                    $("#pro-email").val('').focus();
    //                }
    //            });
    //        var valid = emailRegex.test($("#pro-email").val());
    //        if (!valid) {
    //            alert("Format of the e-mail address is not correct");
    //            $("#pro-email").val('').focus();
    //        }
    //    }
    //    else {
    //        return false;
    //    }
    //});
    $("#learn-more-dialog").dialog({
        modal: true,
        autoOpen: false,
        width: "920",
        height: "620",
        draggable: false
    });
    $("#policy").dialog({
        modal: true,
        autoOpen: false,
        width: "920",
        height: "620",
        draggable: false
    });
    $("#contrib-dialog").dialog({
        modal: true,
        autoOpen: false,
        draggable: true,
        resizable: false,
        width: "350"
    });
    $("#comp-dialog").dialog({
        modal: true,
        autoOpen: false,
        draggable: true,
        resizable: false,
        width: "350"
    });
    $("#login-dialog").dialog({
        modal: true,
        autoOpen: false,
        draggable: true,
        resizable: false,
        width: "350",
        buttons: [
            {
                text: "Log in",
                click: function () {
                    $("#login-dialog").find('form').trigger('submit');
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
    $("#register-dialog").dialog({
        modal: true,
        autoOpen: false,
        draggable: true,
        resizable: false,
        width: "350",
        buttons: [
            {
                text: "Register",
                click: function (e) {
                    var clicked = $(e.target);
                    fValid = true;
                    selTab = window.utype;
                    if (selTab == 'company') {
                        fValid = checkString($("#company-firstname"), 'First Name') && fValid;
                        fValid = checkString($("#company-lastname"), 'Last Name') && fValid;
                        fValid = checkString($("#company-name"), 'Company Name') && fValid;
                        fValid = checkString($("#company-email"), 'Email') && fValid;
                        fValid = checkString($("#company-password"), 'Password') && fValid;
                        fValid = password_type($('#company-password')) && fValid;
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
                            clicked.parent().attr('disabled', true);
                            $.post("register_new_user.php?type=company",
                                {firstname: $("#company-firstname").val(), lastname: $("#company-lastname").val(), name: $("#company-name").val(), email: $("#company-email").val(), password: $("#company-password").val(), industry: $("#company-industry").val(), revenue: $("#company-revenue").val() },
                                function (response) {
                                    alert(response);
                                    if (response != 'Security code is not correct!') {
                                        location.reload();
                                    }
                                });
                        }
                    }
                    else {
                        fValid = checkString($("#pro-type"), 'iam') && fValid;
                        fValid = checkString($("#pro-firstname"), 'First Name') && fValid;
                        fValid = checkString($("#pro-lastname"), 'Last Name') && fValid;
                        fValid = checkString($("#pro-email"), 'Email') && fValid;
                        fValid = checkString($("#pro-password"), 'Password') && fValid;
                        fValid = checkString($("#pro-experties"), 'experties') && fValid;
                        if (!fValid) {
                            alert('Please fill in the required fields.');
                        }
                        if (fValid && !$('#pro-agree').is(':checked')) {
                            fValid = false;
                            alert('You have to read and agree with terms and privacy policy!');
                        }
                        if (fValid) {
                            clicked.parent().attr('disabled', true);
                            $.post("register_new_user.php?type=proffesional",
                                {firstname: $("#pro-firstname").val(), lastname: $("#pro-lastname").val(), experties: $("#pro-experties").val(), email: $("#pro-email").val(), password: $("#pro-password").val(), pro_type: $("#pro-type").val() },
                                function (response) {
                                    alert(response);
                                    if (response != 'Security code is not correct!') {
                                        location.reload();
                                    }
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
                },
                "class": "form-submit2"
            }
        ]        });
    $("#learn-more").click(function () {
        $("#learn-more-dialog").dialog("open");
        return false;
    });
    $('.reg-but1').click(function () {
        $(".ui-dialog-buttonset").show();
    });
    $('.reg-but2').click(function () {
        $(".ui-dialog-buttonset").show();
    });
/*    $("input:text").focus(function () {
        $(this).val('');
    });*/
    var page_height = $(window).height();
    var diff = parseInt(page_height) - 480;
    diff = parseInt(diff / 2);
    if (diff) {
        $('#container').css('margin-top', diff);
    }
    var a = 0;
    if (diff) {
        $('#container').css('margin-top', diff);
    }
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
    })
});
