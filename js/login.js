$(function () {
    'use strict';
    var login = function () {
        return {
            'formSelector': '#maenna-login-form-home',
            'handler': function () {
                $(this.formSelector).live('submit', function () {
                    var $form = $(this),
                    result = false;
                    var abs_url = window.location.protocol + "//" + window.location.host + "/";
                    $.ajax({
                        url: abs_url+'lib/clewed/user/actions/login.php',
                        async: false,
                        dataType: 'json',
                        data: $form.serialize(),
                        type: $form.attr('method'),
                        success: function (response) {
                            result = response.success;
                            if (!response.success) {
                                alert('Invalid email and password. Please try again');
                            }
                        }
                    });
                    return result;
                });
            },
            'init': function () {
                this.handler();
            }
        };
    }();
    login.init();
});
