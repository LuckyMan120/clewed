$(document).ready(function(){
    $('.gray_input_text > div > input').bind('focus', function(){
        if($(this).val() == $(this).parent().parent().attr('dv')){
            $(this).val("").css('color','black');
        }
    }).bind('blur',function(){
        if($(this).val() == $(this).parent().parent().attr('dv') || $(this).val() == '' ){
            $(this).val($(this).parent().parent().attr('dv')).css('color','#757474');
        }
    });
})