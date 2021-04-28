
$('.menu-svg').click(function () {
    $('.menu-svg').removeClass('d-n');
    $(this).addClass('d-n');
    if (count_open === 0) {
        $('.custom-nav-bottom').slideDown();
        count_open++;
    } else {
        $('.custom-nav-bottom').slideUp();
        count_open = 0;
    }
});

