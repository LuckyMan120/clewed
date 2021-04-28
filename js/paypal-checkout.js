(function ($) {
    "use strict";
    $(function () {
        var discount = $('.ui-dialog #discount-code'),
            checkEvent = function (callBack) {
                var eventId = discount.data('event');
                $.ajax(
                    {
                        url: '/check-event.php',
                        type: 'post',
                        cache: false,
                        data: {
                            discountCode: discount.val(),
                            eventId: eventId,
                            id: $('#payment').data('id')
                        },
                        success: function (r) {
                            if (r.error) {
                                switch (true) {
                                case r.errors['invalid-code']:
                                    alert('No discount available with this code');
                                    break;
                                }
                            } else {
                                if (r.price === 'discount') {
                                    var cost = $('#total-cost');
                                    cost.text(r.cost + ' (with ' + r.rate + '% discount)');
                                    var sButton = $("#payment-submit");

                                    if (r.cost == 0) {
                                        sButton.val("Join Free");
                                        sButton.unbind('click').click(function() {
                                            sButton.prop('disabled', true);
                                            sButton.val("Joining..");
                                            joinnow2(eventId,$("#buyer_uid").data('uid'),"false","false");
                                        });
                                    }

                                    sButton.show();
                                }
                                $('.ui-dialog #pp-form-container').html(r.form);
                                if (typeof callBack === 'function') {
                                    callBack();
                                }
                            }
                        }
                    }
                );
            };

        //discount.bind('keyup', function () {
        //    if (discount.val().length === 7) {
        //        checkEvent();
        //    }
        //});

        $('#payment-apply-discount').click(checkEvent);

        var paypalEvent = function (callBack) {
            var eventId = $('#event').val();
            var amount = $('#amount').val();
            $.ajax(
                {
                    url: '/invest-paypal.php',
                    type: 'post',
                    cache: false,
                    data: {
                        eventId: eventId,
                        amount: amount,
                        paypal_link: $('#paypal_link').val(),
                    },
                    success: function (r) {
                        if (r.error) {
                            switch (true) {
                                case r.errors['invalid-code']:
                                    alert('No discount available with this code');
                                    break;
                            }
                        } else {
                            // if (r.price === 'discount') {
                            //     var cost = $('#total-cost');
                            //     cost.text(r.cost + ' (with ' + r.rate + '% discount)');
                            //     var sButton = $("#payment-submit");
                            //
                            //     if (r.cost == 0) {
                            //         sButton.val("Join Free");
                            //         sButton.unbind('click').click(function() {
                            //             sButton.prop('disabled', true);
                            //             sButton.val("Joining..");
                            //             joinnow2(eventId,$("#buyer_uid").data('uid'),"false","false");
                            //         });
                            //     }
                            //
                            //     sButton.show();
                            // }
                            $('#pp-form-container').html(r.form);
                            if (typeof callBack === 'function') {
                                callBack();
                            }
                        }
                    }
                }
            );
        };

        $('#payment_method').change(function() {
            $('#paypal').attr('style', 'display: none;');
<<<<<<< HEAD
=======
            // $('#ach_wire').attr('style', 'display: none;');
>>>>>>> dev
            if ($(this).val() != '' && $('#amount').val() == '') {
                $('#payment_method').val('');
                alert('Please input an amount');
                return;
            }
            if ($(this).val() == 'paypal') {
                $('#paypal').attr('style', 'display: inline;');
                paypalEvent(function () {
                    $('#pp-form-container').find('form').attr('target', '_blank')
                    $('#pp-form-container').find('form').submit();
                });
                getAmount();
            }
            // else if ($(this).val() == 'ach_wire'){
            //     $('#ach_wire').attr('style', 'display: inline;');
            //     getAmount();
            // }
        });
        //discount.bind('keyup', function () {
        //    if (discount.val().length === 7) {
        //        checkEvent();
        //    }
        //});

        $('#payment-apply-discount').click(paypalEvent);

        $('#enter-discount-code').bind('click', function () {
            var form = $('#discount-code-container');
            form.show();
            form.find('input[name="code"]').focus();
            form.find('#check-discount-code').bind('click', function (evt) {
                evt.preventDefault();
                var input = $('#discount-code-value'),
                    code = input.val();
                if (code.length === 0) {
                    alert('Please enter discount code');
                    form.find('input[name="code"]').focus();
                    return;
                }
                $.ajax(
                    {
                        url: '/check-event.php',
                        type: 'post',
                        cache: false,
                        data: {
                            discountCode: code,
                            eventId: input.data('insight'),
                            id: $('#payment').data('id'),
                            type: 'insight-page'
                        },
                        success: function (r) {
                            console.dir(r);
                            if (r.error) {
                                $('#discount-code-error').text('No discount available with this code');
                            } else {
                                if (r.price === 'discount') {
                                    if (r.cost === 0) {
                                        $('#joinnow').replaceWith('<p style="text-align: center;"><input type="button" onclick="joinnow2(' + input.data('insight') + ', ' + input.data('uid') + ', false, false);" class="join" value="Join Free"></p>');
                                    } else {
                                        $('#joinnow').replaceWith(r.form);
                                    }
                                    $('#discount-code-ok').text('Discount applied');
                                } else {
                                    $('#discount-code-error').text('Discount code not recognized');
                                }
                            }
                        }
                    }
                );
            });
        });
        $('.ui-dialog #payment-submit').bind('click', function () {
            checkEvent(function () {
                $('.ui-dialog #payment-submit').prop('disabled', true).val('Redirecting you to PayPal...');
                $('.ui-dialog #pp-form-container').find('form').submit();
            });
        });
        $('div#payment').bind('dialogopen', function(event) {
            $("#discount-code").val('');
        });
    });
})(jQuery);
