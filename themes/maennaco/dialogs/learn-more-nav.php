<script type="text/javascript">
    $(".dialog-left-column a").click(function () {
        var id = $(this).attr('id');
        if (id == 'wwa') {
            $(".dialog-right-column").height(1550);
            $(".verticaldivider").height(1550);
        } else if (id == 'wwd') {
            $(".dialog-right-column").height(534);
            $(".verticaldivider").height(534);
        } else {
            $(".dialog-right-column").height(534);
            $(".verticaldivider").height(534);
        }
        $(".dialog-left-column a").removeClass('dialog-selected');
        $(this).addClass('dialog-selected');
        $(".dialog-right-column").fadeOut('fast');
        $.post("/themes/maennaco/includes/homepage_posts.php?type=" + id, {
        }, function (response) {
            $(".dialog-right-column").html(response).fadeIn("slow");
            if (id == 'partners') {
                $('#partner-name').Watermark('NAME');
                $('#partner-email').Watermark('EMAIL');
                $('#partner-phone').Watermark('PHONE #');
                $('#partner-comments').Watermark('COMMENTS');
            } else if (id == 'contactus') {
                $('#contact-name').Watermark('Name');
                $('#contact-email').Watermark('Email');
                $('#contact-subject').Watermark('Subject');
                $('#contact-message').Watermark('Message');
            }
        });
    });
</script>

<div class="dialog-left-column">
    <a id="wwb" class="dialog-left-column-title dialog-selected" style="cursor:pointer;">What We Believe</a><br><br>
    <a id="ost" class="dialog-left-column-title" style="cursor:pointer;">Our Story</a><br>
    <a id="wsa" class="indented" style="cursor:pointer;">&#8226; What Sets Us Apart</a><br><br>
    <a id="companies" class="dialog-left-column-title" style="cursor:pointer;">Our Clients</a><br><br>
    <a id="wwd" class="dialog-left-column-title" style="cursor:pointer;">Our Services</a><br>
    <a id="cser" class="indented" style="cursor:pointer;">&#8226; Advisory Services</a><br>
    <a id="inv" class="indented" style="cursor:pointer; ">&#8226; Clewed Insights</a><br><br>
    <a id="wwa" class="dialog-left-column-title" style="cursor:pointer;">Who We Are</a><br><br>
    <a id="pron" class="dialog-left-column-title" style="cursor:pointer;">Professional Network</a><br><br>
    <a id="investors" class="dialog-left-column-title" style="cursor:pointer;">Investors</a><br><br>
    <a id="contactus" class="dialog-left-column-title" style="cursor:pointer;">Contact Us</a><br>
</div>
<div class="verticaldivider"></div>
<div class="dialog-right-column">
    <span class="dialog-right-column-title">What We Believe</span><br><br>
    Small fast-growing companies typically offer the best long-term returns when owners compound their growth with a
    minimal interruption and fees.
</div>
