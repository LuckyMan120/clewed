function putNameInsideToField(el) {
    //debugger;
    document.getElementById('searchFieldUid').value = el.getAttribute("uid");
    document.getElementById('searchField').value = el.innerHTML;
    $('#jquery-live-search').hide();
}