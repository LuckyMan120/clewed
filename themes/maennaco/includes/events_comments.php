<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery-ui-timepicker-addon.js"></script>
<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.autosuggest.js"></script>
<link href="/themes/maennaco/jui/comments/css/screen.css?as" type="text/css" rel="stylesheet"/>
<link href="/themes/maennaco/jui/comments/css/autosuggest.css" type="text/css" rel="stylesheet"/>
<link href="/themes/maennaco/jui/comments/css/fileuploader.css" type="text/css" rel="stylesheet"/>

<!--<script src="/themes/maennaco/jui/comments/js/jquery.elastic.js" type="text/javascript" charset="utf-8"></script>-->
<script src="/themes/maennaco/jui/comments/js/fileuploader.js" type="text/javascript" charset="utf-8"></script>
<script src="/themes/maennaco/jui/comments/js/jquery.watermarkinput.js" type="text/javascript"></script>

<style>
    .form-submit2.ui-button-text-only span.ui-button-text {
        background-color: #06333b !important;
    }
</style>
<div id='docprev'>
<script type="text/javascript">

function openDialog(boxtitle, boxcontent, spanAtt) {

    $("#dialog").dialog({
        autoOpen: true,
        width: 500,
        title: boxtitle,
        height: 490,
        buttons: { "Close": function () {
            $(this).dialog("close");
        }},
        closeOnEscape: true,
        modal: true
    }).html(boxcontent);

}

$(document).ready(function () {

    var currAtt = '';
    var availableTags = {items: [
        //Get the advisors and connected users for the autocomplete feature; $companyid was gotten in the earlier phase in new_company_detail_left.php
        <?php
        $Conns = Connections::Com_conns($companyid);
        $Actives = array();
        foreach($Conns['Advisor'] as $Pro) {
            $pro_uid = $Pro->assignee_uid;
            $pro_maeid = getProId($pro_uid);
            $Actives[] = array("value" => $pro_uid,"name" => $pro_maeid,"type" => "advisor");
            echo '{value: "'.$pro_uid.'", name: "'.$pro_maeid.'", type: "advisor"},';
        }
        foreach($Conns['Client'] as $Pro) {
            $pro_uid = $Pro->assignee_uid;
            $pro_maeid = getProId($pro_uid);
            echo '{value: "'.$pro_uid.'", name: "'.$pro_maeid.'", type: "client"},';
        }
//        foreach($Conns['Visible'] as $Pro) {
//            $pro_uid = $Pro->assignee_uid;
//            $pro_maeid = getProId($pro_uid);
//            echo '{value: "'.$pro_uid.'", name: "'.$pro_maeid.'"},';
//
//        }
//        $q = mysql_query("SELECT projname FROM maenna_company WHERE companyid = '".((int) $_REQUEST['id'])."'");
//        $q = mysql_fetch_array($q);
//        echo '{value: "'.$_REQUEST['id'].'", name: "'.$q['projname'].' "}';
        ?>
    ]};

    var availableLeads =  {items: [
        <?php
        foreach($Actives as $val) {
            echo '{value: "'.$val['value'].'", name: "'.$val['name'].'", type: "advisor"},';
        }
        ?>

    ]};

    if(availableTags.items.length <= 0)
        $('.add-service-form').addClass('no-users-available');
    else
        $('.add-service-form').removeClass('no-users-available');

    $('[id^=rmFile]').livequery("click", function () {
        fileid = $(this).attr('id').replace("rmFile", "");
        $.post(
            "/themes/maennaco/includes/posts.php?type=removeFile&fileid=" + fileid,
            function (response) {
                $("#file" + fileid).remove();
            }
        );
    });

    $('input[type="file"]').click(function () {
    });
    $("#showEventForm").click(function () {

        $("#eventFormDiv").show();
    });

    $("#serviceType").change(function() {

        var selected = $(this ).val();

        <?php
        if(in_array($AccessObj->user_type, array('super', 'admin'))) {
            echo "var usrType = 'super';";
        }
        elseif (('company' == $AccessObj->user_type && $_REQUEST['id'] == $user->uid) || in_array($AccessObj->uid,$companyService->getColleagueIds($companyid))) {
            echo "var usrType = 'colleague';";
        }
        else echo "var usrType = 'people';";

        ?>

        if (selected == 'service') {
            $("#eventDate").attr('placeholder','Deadline');
            $("#eventLoc").attr('data-tooltip','');
            $("#eventLoc").attr('placeholder','How is service delivered? (eg: Online files, Call, In person)');
            $("input[name='budget']").attr('placeholder','Net Expert Fee $');
            $("input[name='budget']").attr('data-tooltip','Gross expert fee will be +25%');
            $("input[name='total']").attr('data-tooltip','');



        }
        else if (selected == 'meeting') {
            $("#eventDate").attr('placeholder','Date');
            $("#eventLoc").attr('data-tooltip', 'Using Clewed\'s number is recommended for efficiency tracking and improvement for all users. Add your call in number if you prefer.');
            $("#eventLoc").attr('placeholder','Where: Clewed will add call in shortly (Optional)');
            $("input[name='budget']").attr('placeholder','Add fee or $0.00');
            if (usrType == 'people')
                $("input[name='budget']").attr('data-tooltip','Unless this is a seperate service call not discussed in scope, calls related to projects are generally free. Use your judgment to avoid unnecessary FREE calls/discussions by clarifying items through discussions.');
            else if (usrType == 'colleague') {$("input[name='total']").attr('data-tooltip','Enter $0.00 if this call is part of the scope of an existing service or a brief vetting call.');}
        }
        else $("#eventFormDiv").hide();
    });

    $("#dialog").dialog({
        autoOpen: false
    });

    $(".evedit").livequery("click", function (e) {
        e.preventDefault();
        var eventid = $(this).attr('id').replace('edit_id', ''),
            filterColleagues = $(this).data('filter-colleagues');

        $.post("/themes/maennaco/includes/posts.php?type=eventEdit&display=true&eventid=" + eventid, {
            uid: '<?php echo $user->uid;?>',
            time: '<?php echo $time = time();?>',
            hash: '<?php echo md5($user->uid . ':' . $time.  ':kyarata75')?>'
        },
        function (response) {
            $("#eveditdlg").dialog({
                autoOpen: true,
                width: 650,
                title: 'Service editor',
                height: 600,
                buttons: {
                    "Save": {
                        text: "Save",
                        'class': "form-submit1",
                        click: function () {
                            var editEventForm = $("#editEventForm");
                            var $form = editEventForm;
                            var budget = $form.find('input[name=budget]').val();
                            var total = $form.find('input[name=total]').val();
                            var fee = $form.find('input[name=clewed_fee]').val();
                            var title = editEventForm.children("#eventType").val();
                            var datetime = editEventForm.children("#eventDateEdit").val();
                            var loc = editEventForm.children("#eventLoc").val();
                            var agenda = editEventForm.children("textarea[name=eventDesc]").val();
                            var adjustments = editEventForm.children("textarea[name=subsequent_adjustments]").val();
                            var uname = '<?=$user->name;?>';
                            var cid = <?=$_REQUEST['id'];?>;
                            var uid = <?=$user->uid;?>;
                            var time = <?php echo $time = time();?>;
                            var hash = '<?php echo md5($user->uid . ':' . $time . ':kyarata75');?>';
                            var mainExpert = $form.find('.main-expert-container .as-values').val();
                            var invitedExperts = $form.find('.invited-experts-container .as-values').val();
                            var filesEdit = [];

                            if ($("#chkNot").is(":checked")) notif = 'true'; else notif = 'false';
                            //invitees = editEventForm.children(".as-selections").children(".as-original").children(".as-values").val();
                            $(".fileInfo").each(function () {
                                var tmpArr1 = {'path': $(this).attr('path'), 'title': $(this).attr('filetitle'), 'timestamp': $(this).attr('timestamp')};
                                filesEdit.push(tmpArr1);
                            });

                            var milestones = [];
                            editEventForm
                                .find('.milestones-container .milestone')
                                .each(function(i, m){
                                    var id = $(m).data('id'),
                                        dueDate = $(m).find('.due-date').val(),
                                        description = $(m).find('.description').val();
                                    if(dueDate !== void 0 && dueDate.length > 0) {
                                        var splitted = dueDate.split('/'),
                                            year = '20' + splitted[2],
                                            month = splitted[0] - 1,
                                            day = splitted[1];
                                        milestones.push({
                                            id: id,
                                            due_date: new Date(year, month, day, 12).getTime() / 1000,
                                            description: description
                                        });
                                    }
                                });

                            $.post(
                                "/themes/maennaco/includes/posts.php?type=eventEdit",
                                {
                                    eventid: eventid,
                                    title: title,
                                    datetime: datetime,
                                    loc: loc,
                                    agenda: agenda,
                                    invitees: invitedExperts,
                                    u: uname,
                                    fee: fee,
                                    budget: budget,
                                    total: total,
                                    time: time,
                                    executor_id: mainExpert,
                                    hash: hash,
                                    cid: cid,
                                    uid: uid,
                                    files: filesEdit,
                                    notif: notif,
                                    subsequent_adjustments: adjustments,
                                    milestones: milestones
                                },
                                function () {
                                    location.reload();
                                });
                        }},
                    "Close": {
                        'class': "form-submit2",
                        text: "Close",
                        click: function () {
                            $(this).dialog("close");
                        }
                    }
                },
                closeOnEscape: true,
                modal: true
            }).html(response);
            var invited = $(response).find(".invited-experts-container input").val().split(',');

            if(availableTags.items.length <= 0) {
                $('.edit-service-form').addClass('no-users-available');
                $(".edit-service-form input[name=executor_id]").val('');
            }
            else
                $('.edit-service-form').removeClass('no-users-available');

            var preFill = availableTags.items.filter(function(item){
                return invited.indexOf(item.value) >= 0;
            });

            var executorId = $(response).find("input[name=executor_id]").val();
            $("#eveditdlg input[name=executor_id]").autoSuggest(availableLeads.items, {
                preFill: availableTags.items.filter(function(el){
                    return el.value == executorId;
                }),
                selectionLimit: 1,
                selectedItemProp: "name",
                searchObjProps: "name"
            });

            if(filterColleagues)
                availableTags.items = availableTags.items.filter(function(item){
                    return 'client' == item.type || invited.indexOf(item.value) >= 0;
                });

            $("#eventInv").autoSuggest(availableTags.items, {
                preFill: preFill,
                selectedItemProp: "name",
                searchObjProps: "name"
            });

            //$("#eventDesc").elastic();
            $(".edit-service-form .as-selections").width(290);
            $(".datepicker").datetimepicker(
                {
                    dateFormat: 'yy-mm-dd',
                    timeFormat: 'hh:mm'
                }
            );
        });
    });

    $(".service-add-files-btn").livequery("click", function (e) {
        e.preventDefault();
        var $btn = $(this),
            $container = $("#service-file-upload-dlg"),
            eventId = $btn.data('id'),
            uid = $btn.data('uid'),
            time = $btn.data('time'),
            hash = $btn.data('hash');

        $.post("/themes/maennaco/includes/manage-service-files.php?type=edit&display=true&eventid=" + eventId, {
                uid: uid,
                time: time,
                hash: hash
            },
            function (response) {
                $container.dialog({
                    autoOpen: true,
                    width: 650,
                    title: 'Service files editor',
                    buttons: {
                        "Save": {
                            text: "Save",
                            'class': "form-submit1",
                            click: function () {
                                var files = [];
                                $container.find(".fileInfo").each(function () {
                                    var tmp = {'path': $(this).attr('timestamp') + '_' + $(this).attr('path'), 'title': $(this).attr('filetitle'), 'timestamp': $(this).attr('timestamp')};
                                    files.push(tmp);
                                });

                                $.post(
                                    "/themes/maennaco/includes/manage-service-files.php?type=edit&eventid=" + eventId,
                                    {
                                        uid: uid,
                                        time: time,
                                        hash: hash,
                                        files: files
                                    },
                                    function (response) {
                                        location.reload();
                                    }
                                );
                            }},
                        "Close": {
                            'class': "form-submit2",
                            text: "Close",
                            click: function () {
                                $(this).dialog("close");
                            }
                        }
                    },
                    closeOnEscape: true,
                    modal: true
                }).html(response);

                var uploader1 = new qq.FileUploader({
                    // pass the dom node (ex. $(selector)[0] for jQuery users)
                    element: $("#file-uploader1")[0],
                    // path to server-side upload script
                    action: '/themes/maennaco/includes/file_upload.php',
                    onComplete: function (id, fileName, responseJSON) {
                        if (responseJSON['success']) {
                            fileName = responseJSON['name'].replace(responseJSON['timestamp'] + '_', '');
                            $container.append('<input type="hidden" class="fileInfo" path="' + fileName + '" filetitle="' + $("#fileTitleEdit").val() + '" timestamp = "' + responseJSON['timestamp'] + '">');
                            $("#fileTitleEdit").val('');
                        }
                    }
                });
            });
    });

    $("#rsvp-button").livequery("click", function () {
        var status = $('input[name=invStatus]:checked', '#invrsvp').val();
        var uid = '<?=$user->uid;?>';
        var eventid = $("#dlgInfo").attr('eventid').replace('event', '');
        $.post(
            "/themes/maennaco/includes/posts.php?type=confirmAtt&status=" + status + "&uid=" + uid + "&eventid=" + eventid,
            function (response) {
                if (response.trim() == 'overlap') {
                    alert("You are already attending event at that time!");
                    return;
                }
                $("#" + $("#attSpan").val()).html(response);
                $("#dialog").dialog("close");
            }
        );
    });

    $(".eventTitle").livequery("click", function (evt) {
        evt.preventDefault();
        var eventDiv = $(this).parent().parent().parent();
        var eventForm = $(this).parent();
        var ccfiles = $(this).parent().parent().find("<:nth-child(3)");
        var boxtitle = eventForm.find(">:first-child").html();
        var boxcontent = '<span id="dlgInfo" eventid = "' + eventDiv.attr('id') + '" style="float:left; font-size:15px;"><strong>' + eventForm.find("<:first-child").html() + '</strong></span><div id="clear" style="clear:both"></div><span style="float:left;">' + eventForm.find("<:nth-child(3)").html() + '</span><br><span style="float:left;">&nbsp;' + eventForm.find("<:nth-child(5)").html() + '</span><br></div>';
        boxcontent = boxcontent + '<div id="clear" style="clear:both"><br>';
        if (eventForm.children(".invatt").length > 0) boxcontent = boxcontent + '<div style="float:left;">' + ccfiles.find("<:nth-child(1)").html().replace("rsvp", "") + '</div>';
        boxcontent = boxcontent + '<div id="clear" style="clear:both"><br><span style="float:left; ">AGENDA:<div style="margin-left:15px;">' + eventForm.find("<:nth-child(7)").html() + '</div> </span><div id="clear" style="clear:both"><br><span style="float:left;">FILES:</span><br><div style="margin-left:15px;>"' + ccfiles.children(".attFiles").html().replace("Attached files:", "") + '</div></span>';
        boxcontent = boxcontent + '<div id="clear" style="clear:both"><br><span style="float:left;">ATTENDING?</span><form style="float:left;" id="invrsvp" action=""><label style="margin-left:20px;"><input type="radio" name="invStatus" value="confirmed" class="styled"><strong>Yes</strong></label><label style="margin-left:20px;"><input type="radio" name="invStatus" value="declined" class="styled"><strong>No</strong></label><label style="margin-left:20px;"><input type="radio" name="invStatus" value="maybe" class="styled"><strong>May be</strong></label><a id="rsvp-button" style="margin-left:30px;" class="small button"> SAVE </a></form>';
        if (boxcontent != '') boxcontent = boxcontent.replace(/\\n/g, '<br />');
        $("#attSpan").val("att" + eventDiv.attr('id').replace('event', ''));
        openDialog(boxtitle, boxcontent, $(this).parent().attr('id'));
    });

    $(".rsvp").livequery("click", function (evt) {
        evt.preventDefault();
        var eventDiv = $(this).parent().parent().parent().parent().parent();
        var eventForm = $(this).parent().parent().parent().parent().find("<:first-child");
        var ccfiles = $(this).parent().parent().parent();
        var boxtitle = eventForm.find(">:first-child").html();
        var boxcontent = '<span id="dlgInfo" eventid = "' + eventDiv.attr('id') + '" style="float:left; font-size:15px;"><strong>' + eventForm.find("<:first-child").html() + '</strong></span><div id="clear" style="clear:both"></div><span style="float:left; color:#00a2bf;">' + eventForm.find("<:nth-child(3)").html() + '</span><span style="float:left; color:#00a2bf;">' + eventForm.find("<:nth-child(5)").html() + '</span><br></div>';
        boxcontent = boxcontent + '<div id="clear" style="clear:both"><br><span style="float:left;">ATTENDES:</span> <div style="margin-left:15px;float:left;">' + ccfiles.find("<:first-child").html().replace("rsvp", "") + '</div>';
        boxcontent = boxcontent + '<div id="clear" style="clear:both"><br><span style="float:left; ">AGENDA:<div style="margin-left:15px;">' + eventForm.find("<:nth-child(7)").html() + '</div> </span><div id="clear" style="clear:both"><br><span style="float:left;">FILES:</span><br><div style="margin-left:15px;>"' + ccfiles.children(".attFiles").html().replace("Attached files:", "") + '</div></span>';
        boxcontent = boxcontent + '<div id="clear" style="clear:both"><br><span style="float:left;">ATTENDING?</span><form style="float:left;" id="invrsvp" action=""><label style="margin-left:20px;"><input type="radio" name="invStatus" value="confirmed" class="styled"><strong>Yes</strong></label><label style="margin-left:20px;"><input type="radio" name="invStatus" value="declined" class="styled"><strong>No</strong></label><label style="margin-left:20px;"><input type="radio" name="invStatus" value="maybe" class="styled"><strong>May be</strong></label><a id="rsvp-button" style="margin-left:30px;" class="small button"> SAVE </a></form>';
        if (boxcontent != '') boxcontent = boxcontent.replace(/\\n/g, '<br />');
        $("#attSpan").val($(this).parent().attr('id'));
        openDialog(boxtitle, boxcontent, $(this).parent().attr('id'));
    });

    $("#eventInv").autoSuggest(availableTags.items, {selectedItemProp: "name", searchObjProps: "name"});
    $("input[name=executor_id]").autoSuggest(availableLeads.items, {
        <?php
            if(!empty($user->uid) && !in_array($AccessObj->user_type, array('admin', 'super', 'company')))
                echo "preFill: availableTags.items.filter(function(el){
                    return el.value == {$user->uid} && el.type != 'client';
                }),";
        ?>
        selectionLimit: 1,
        selectedItemProp: "name",
        searchObjProps: "name"
    });
    $('.qq-upload-remove').livequery("click", function () {
        var fileToRemove = $(this).parent().children('.qq-upload-file').html();
        $("input[path='" + fileToRemove + "']").remove();
        $(this).parent().remove();
    });
    $('[id^=shareButton]').livequery("click", function () {
        tarea = $('textarea[eventid=' + $(this).attr('textref') + ']');
        var a = encodeURIComponent(tarea.val());
        var m = tarea.attr("alt");
        var u = tarea.attr("name");
        var uid = '<?=$user->uid;?>';
        var eventid = tarea.attr('eventid');
        if (a != "Discuss a topic or ask a question on this file ...") {
            $.post("/themes/maennaco/includes/posts.php?type=eventcom&u=" + u + "&m=" + m + "&value=" + a + "&eventid=" + eventid + "&uid=" + uid, {
            }, function (response) {
                tarea.parent().parent().parent().parent().after($(response).show());
                tarea.val("Share ideas on this topic");
                //$('textarea').elastic();
                $(".commentMark").Watermark("Got advice / question on this topic?");
                tarea.Watermark("watermark", "#369");
                $(".commentMark").Watermark("watermark", "#EEEEEE");
            });
        }
    });

    $('#addEvent').click(function () {
        var $form = $('#addEventForm');
        var uid = <?=$user->uid;?>;
        var m = '<?=md5($user->uid."kyarata75");?>';
        var cid = <?=$_REQUEST['id'];?>;
        var event = $("#eventType").val();
        var loc = $("#eventLoc").val();
        var desc = $("#eventDesc").val();
        var datetime = $("#eventDate").val();
        var invitees = $form.find(".invited-experts-container .as-values").val();
        var mainExpert = $form.find('.main-expert-container .as-values').val();
        var budget = $form.find('input[name=budget]').val();
        var fee = $form.find('input[name=clewed_fee]').val();
        var total = $form.find('input[name=total]').val();
        var tags = $("#tags").val();
        var uname = '<?=$user->name;?>';
        var urole = '<?=end($user->roles);?>';
        var files = [];
        $(".fileInfo").each(function () {
            var tmpArr = {'path': $(this).attr('path'), 'title': $(this).attr('filetitle'), 'timestamp': $(this).attr('timestamp')};
            files.push(tmpArr);
        });

        var milestones = [];
        var serviceType = $("#serviceType").val();

        $("#eventType").removeClass("error_select");
        $("#eventDesc").removeClass("error_select");
        $("#serviceType").removeClass("error_select");

        var errors = false;

        if (serviceType.trim() == ''){
            errors = true;
            $("#serviceType").addClass("error_select");
        }
        if (event.trim() == ''){
            errors = true;
            $("#eventType").addClass("error_select");
        }
        if (desc.trim() == ''){
            errors = true;
            $("#eventDesc").addClass("error_select");
        }


        $form
            .find('.milestones-container .milestone')
            .each(function(i, m){
                var id = $(m).data('id'),
                    dueDate = $(m).find('.due-date').val(),
                    description = $(m).find('.description').val();
                if(dueDate !== void 0 && dueDate.length > 0) {
                    var splitted = dueDate.split('/'),
                        year = '20' + splitted[2],
                        month = splitted[0] - 1,
                        day = splitted[1];
                    milestones.push({
                        id: id,
                        due_date: new Date(year, month, day, 12).getTime() / 1000,
                        description: description
                    });
                }
            });

        if( !errors ){
            $.post("/themes/maennaco/includes/posts.php?type=addEvent", {
                    uid: uid,
                    event: event,
                    loc: loc,
                    desc: desc,
                    date: datetime,
                    invitees: invitees,
                    u: uname,
                    files: files,
                    cid: cid,
                    urole: urole,
                    m: m,
                    tags: tags,
                    budget: budget,
                    fee: fee,
                    total: total,
                    executor_id: mainExpert,
                    milestones: milestones,
                    serviceType: serviceType
                },
                function (response) {
                    $('.fileInfo').remove();
                    $(".watermark").Watermark("Share ideas on this topic");
                    //$("#eventDate").Watermark("Deadline");
                    //$("#eventType").Watermark("Subject?");
                    //$("#eventLoc").Watermark("How is service delivered? (eg: Online files, Call, In person)");
                    //$("#eventDesc").Watermark("Describe service and deliverables. Add timeline for key milestones in your description");
                    //$("input[name=budget]").Watermark("Expert Fee $", "#369");
                    //$("input[name=clewed_fee]").Watermark("Clewed Fee %", "#369");
                    //$(".watermark").Watermark("watermark", "#369");
                    //$("#eventDate").Watermark("watermark", "#369");
                    //$("#eventType").Watermark("watermark", "#369");
                    //$("#eventLoc").Watermark("watermark", "#369");
                    //$("#eventDesc").Watermark("watermark", "#369");
                    //$("#eventInv").Watermark("watermark", "#369");
                    $("#eventFormDiv").hide();
                    document.location.reload(true);
                }
            );
        }
    });
    $('.watermark').livequery("focus", function (e) {
        sbmBtt = $(this).attr('eventid');
        $('a[textref=' + sbmBtt + ']').show();
    });
    $('.commentMark').livequery("focus", function (e) {
        var parent = $('.commentMark').parent();
        $(".commentBox").children(".CommentImg").hide();
        var getID = parent.attr('id').replace('record-', '');
        $("#commentBox-" + getID).children("a#SubmitComment").show();
        $("#commentBox-" + getID).children(".CommentImg").show();
    });
    //showCommentBox
    $('a.showCommentBox').livequery("click", function (e) {
        var getpID = $(this).attr('id').replace('post_id', '');
        $("#commentBox-" + getpID).css('display', '');
        $("#commentMark-" + getpID).focus();
        $("#commentBox-" + getpID).children("CommentImg").show();
        $("#commentBox-" + getpID).children("a#SubmitComment").show();
    });

    //SubmitComment
    $('a.comment').livequery("click", function (e) {
        var getpID = $(this).parent().attr('id').replace('commentBox-', '');
        var comment_text = encodeURIComponent($("#commentMark-" + getpID).val());
        var m = $(this).parent().attr("alt");
        var u = $(this).parent().attr("name");
        var uid = '<?=$user->uid;?>';
        if (comment_text != "Got advice / question on this topic?") {
            $.post("/themes/maennaco/includes/add_comment.php?u=" + u + "&comment_text=" + comment_text + "&post_id=" + getpID + "&m=" + m + "&uid=" + uid, {
            }, function (response) {
                $('#CommentPosted' + getpID).append($(response).show());
                $("#commentMark-" + getpID).val("Got advice / question on this topic?");
            });
        }
    });
    //more records show
    $('a.more_records').livequery("click", function (e) {
        var next = $('a.more_records').attr('id').replace('more_', '');
        $.post("/themes/maennaco/includes/posts.php?show_more_post=" + next, {
        }, function (response) {
            $('#bottomMoreButton').remove();
            $('#posting').append($(response).show());
        });
    });

    //deleteComment
    $('a.c_delete').livequery("click", function (e) {
        if (confirm('Are you sure you want to delete this comment?') == false) {
            return false;
        }
        e.preventDefault();
        var c_id = $(this).attr('id').replace('CID-', '');
        var u = $(this).attr("name");
        var m = $(this).attr("alt");
        $.ajax({
            type: 'get',
            url: '/themes/maennaco/includes/delete_comment.php?c_id=' + c_id + "&u=" + u + "&m=" + m,
            success: function () {
                $('#comment-' + c_id).remove();
            }
        });
    });

    /// hover show remove button
    $('.friends_area').livequery("mouseenter", function (e) {
        $(this).children("label.name").children("a.delete").show();
    });
    $('.friends_area').livequery("mouseleave", function (e) {
        $(this).children("label.name").children("a.delete").hide();
    });
    $('.event').livequery("mouseenter", function (e) {
        //$(this).children("span").children("a.delete").show();
        $(this).children("a.delete").show();
    });
    $('.event').livequery("mouseleave", function (e) {
        //$(this).children("span").children("a.delete").hide();
        $(this).children("a.delete").hide();
    });
    /// hover show remove button
    $('a.evdelete').livequery("click", function (e) {
        if (confirm('Are you sure you want to delete this discussion?') == false) {
            return false;
        }
        e.preventDefault();
        var temp = $(this).attr('id').replace('remove_id', '');
        var u = $(this).attr("name");
        var m = $(this).attr("alt");
        $.ajax({
            type: 'get',
            url: '/themes/maennaco/includes/delete.php?type=event&id=' + temp + "&u=" + u + "&m=" + m,
            success: function () {
                $('#event' + temp).remove();
            }
        });
        return true;
    });

    $('a.delete').livequery("click", function (e) {
        if (confirm('Are you sure you want to delete this post?') == false) {
            return false;
        }
        e.preventDefault();
        var parent = $('a.delete').parent();
        var temp = $(this).attr('id').replace('remove_id', '');
        var u = $(this).attr("name");
        var m = $(this).attr("alt");
        var main_tr = $('#' + temp).parent();
        if ($(this).attr('delType') == 'event') {
            $.ajax({
                type: 'get',
                url: '/themes/maennaco/includes/delete.php?type=event&id=' + temp + "&u=" + u + "&m=" + m,
                success: function () {
                    $('#event' + temp).remove();
                }
            });
            return true;
        }
        $.ajax({
            type: 'get',
            url: '/themes/maennaco/includes/delete.php?id=' + temp + "&u=" + u + "&m=" + m,
            success: function () {
                $('#record-' + temp).remove();
            }
        });
    });
    //$('textarea').elastic();
    jQuery(function ($) {
        $(".watermark").Watermark("Share ideas on this topic");
//        $("#eventDate").Watermark("Deadline");
//        $("#eventType").Watermark("Subject?");
//        $("#eventLoc").Watermark("How is service delivered? (eg: Online files, Call, In person)");
//        $("#eventDesc").Watermark("Describe service and deliverables. Add timeline for key milestones in your description");
//        $("#eventInv").Watermark("+ Add people to notify");
//        $(".commentMark").Watermark("Got advice / question on this topic?");
//        $("input[name=budget]").Watermark("Expert Fee $", "#rgb(143, 144, 149)");
//        $("input[name=clewed_fee]").Watermark("Clewed Fee %", "#rgb(143, 144, 149)");
    });

    jQuery(function ($) {
//        $(".watermark").Watermark("watermark", "#369");
//        $("#eventDate").Watermark("watermark", "#369");
//        $("#eventType").Watermark("watermark", "#369");
//        $("#eventLoc").Watermark("watermark", "#369");
//        $("#eventDesc").Watermark("watermark", "#369");
//        $("#eventInv").Watermark("watermark", "#369");
        $(".commentMark").Watermark("watermark", "#EEEEEE");
    });

    function UseData() {
        $.Watermark.HideAll();
        $.Watermark.ShowAll();
    }

});

</script>

<div>

    <?php
    if ($AccessObj->Com_sections['advice']['sections']['nav_menu_right']['access'] == 'write') {
        ?>
        <div id="eveditdlg" style="display:none;overflow-x:hidden;overflow-y:scroll;"></div>
        <div id="service-file-upload-dlg"></div>
        <div style="clear:both;"></div>
        <div id="eventFormDiv" style="display:none;">
            <script type='text/javascript'>
                $(document).ready(function () {
                    $("#eventDate").datetimepicker({dateFormat: 'yy-mm-dd',minDate:0});
                });
            </script>
            <?php
                $createServiceToolCaption = 'Create service';
                if('company' == $AccessObj->user_type && $_REQUEST['id'] == $user->uid || 'super' == $AccessObj->user_type || 'admin' == $AccessObj->user_type)
                    $createServiceToolCaption = 'Order service';
            ?>
            <select required="true" class="discuss" id="serviceType" name="serviceType"
                    style="margin:20px 0 10px 0;padding:6px;height:29px;font-size:14px;width:97% !important;box-sizing:content-box;display:inline-block;">
                <option value="" selected="selected">Choose the type</option>
                <option value="service">Create Service</option>
                <option value="meeting">Schedule a meeting / call</option>
            </select>
            <form action="" method="post" name="addEvent" id="addEventForm" class="add-service-form">

                <?= js_init("init_openbox();init_hidebox();"); ?>
                <input type="text"
                       class="input"
                       id="eventType"
                       name="eventType"
                       placeholder="Subject"
                       style="height:46px!important;width:100%!important;box-sizing: border-box;margin-bottom: 10px;">

                <input type='text'
                       id="eventDate"
                       name="eventDate"
                       class='datepicker'
                       placeholder="Deadline"
                       style="15px/160% Verdana, sans-serif;float:left;margin-right:0;width:30%!important;height: 46px!important;box-sizing: border-box;"/>

                <input type="text"
                       class="input"
                       id="eventLoc"
                       name="eventLoc"
                       placeholder="How is service delivered? (eg: Online files, Call, In person)"
                       style="float:right;width: 69%!important;box-sizing: border-box;height:46px!important;">

                <textarea class="input"
                          id="eventDesc"
                          name="eventDesc"
                          cols="60"
                          placeholder="Describe service and deliverables. Add timeline for key milestones in your description"
                          style="width: 100%!important;"></textarea>
<div id="serviceSection">
                <div class="milestones-section" style="margin-bottom:35px;">
                    <div class="milestones-container">
                    </div>
                    <div class="milestones-controls">
                        <div class="add-milestone-btn" style="float:right;color: #00a2bf;line-height: 35px;cursor: pointer;clear:both;">Add milestones</div>
                    </div>
                    <script type="text/javascript">

                        function addServiceFormRenderMilestone(id, date, description, readonly) {
                            return $(
                                "<div data-id=\"" + (id ? id : "") + "\" class=\"milestone\">" +
                                "   <input " +
                                "       class=\"due-date\"" +
                                "       type=\"text\"" +
                                "       placeholder=\"Due date\"" +
                                "       value=\"" + (date ? date : "") + "\"" +
                                (true === readonly ? ' readonly=\"readonly\" ' : '') +
                                (true === readonly ? ' <?= $blockedTooltip;?> ' : '') +
                                "       style=\"width: 33%!important;height: 46px!important;font-size:14px;\"/>" +
                                "   <textarea " +
                                "        class=\"description\"" +
                                "        type=\"text\"" +
                                "        placeholder=\"Description\"" +
                                "        title=\"" + (description ? description : "") + "\"" +
                                "        value=\"" + (description ? description : "") + "\"" +
                                (true === readonly ? ' readonly=\"readonly\" ' : '') +
                                "        style=\"width: 65%!important;min-height: 46px!important;height:46px !important; margin-top:0px !important; font-size:14px;float:right\"></textarea>" +
                                "</div>"
                            );
                        }

                        function addServiceFormAddMilestone(id, date, description, readonly) {
                            var $milestone = addServiceFormRenderMilestone(id, date, description, readonly);
                            $('.add-service-form .milestones-container').append($milestone);
                            if(!readonly)
                                $milestone.find('.due-date').datepicker({
                                    dateFormat: 'mm/dd/y',
                                    minDate: 0
                                });
                        }

                        $(function(){
                            $('.add-service-form .add-milestone-btn').attr('onclick', 'addServiceFormAddMilestone();return false;');
                        });

                    </script>
                </div>

                <?php if(in_array($AccessObj->user_type, array('super', 'admin'))) : ?>

                    <script type="text/javascript">
                        $(function(){
                            var $form = $('#addEventForm'),
                                recalculateTotal = function(){
                                    var budget = $form.find('input[name=budget]').val(),
                                        clewedFee = $form.find('input[name=clewed_fee]').val(),
                                        $total = $('.add-service-total-fee');

                                    if(!clewedFee.trim().length)
                                        clewedFee = 0;

                                    if(!budget.trim().length)
                                        budget = 0;

                                    if(0 == budget && 0 == clewedFee)
                                        return $total.val('0.00');

                                    if(Number(budget) == budget && Number(clewedFee) == clewedFee)
                                        $total.val((budget * (100 + (clewedFee - 0)) / 100).toFixed(2));

                                    $form.find('input[name=budget]').attr('data-tooltip', 'Gross expert fee will be $' + $total.val());
                                };

                            $form.find('input[name=budget]').keyup(recalculateTotal);
                            $form.find('input[name=clewed_fee]').keyup(recalculateTotal);
                        });

                    </script>

                    <table cellpadding="0" cellspacing="0" class="box-content">
                        <tr>
                            <td align="left">
                                Net Expert fee:
                            </td>
                            <td align="left">
                                Clewed fee:
                            </td>
                            <td align="left">
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <input type="text"
                                       name="budget"
                                       placeholder="Net Expert Fee $"
                                       data-tooltip="Gross expert fee will be +25%"
                                       style="width:98%!important;float:left;height:46px!important;font-size:14px;margin-right:5px;margin-top:3px;" />
                            </td>
                            <td>
                                <input type="text"
                                       name="clewed_fee"
                                       placeholder="Clewed Fee %"
                                       value="62"
                                       style="width:98%!important;float:left;height:46px!important;font-size:14px;margin-right:5px;margin-top:3px;" />
                            </td>
                            <td>
                                <input type="text"
                                       class="add-service-total-fee"
                                       readonly="readonly"
                                       placeholder="Total Fee $"
                                       style="width:99%!important;float:right;height:46px!important;font-size:14px;margin-top: 3px;" />
                            </td>
                        </tr>
                    </table>

                <?php else : ?>

                    <?php if (('company' == $AccessObj->user_type && $_REQUEST['id'] == $user->uid) || in_array($AccessObj->uid,$companyService->getColleagueIds($companyid))): ?>
                        <input type="text"
                               name="total"
                               placeholder="Total Fee $"
                               style="width:100%!important; height: 46px!important; font-size:14px;" />
                    <?php else : ?>
                        <input type="text"
                               name="budget"
                               placeholder="Enter your net fee for this project"
                               data-tooltip="Your fee after clewed charges is $"
                               style="width:100%!important; height: 46px!important; font-size:14px;" />
                        <script type="text/javascript">
                            $(function(){
                                var $form = $('#addEventForm'),
                                  recalculateTotal = function(){
                                      var budget = $form.find('input[name=budget]').val();

                                      if(!budget.trim().length)
                                          budget = 0;

                                      $form.find('input[name=budget]').attr('data-tooltip', 'Your fee after clewed charges is $' + (budget * 1.25).toFixed());
                                  };

                                recalculateTotal();
                                $form.find('input[name=budget]').keyup(recalculateTotal);
                            });
                        </script>
                    <?php endif;?>

                <?php endif;?>

                <?php if ('people' == $AccessObj->user_type && !in_array($AccessObj->uid,$companyService->getColleagueIds($companyid))) {
                        $blockControls = true;
                        $blockedTooltip = " data-tooltip=\"Experts can only create a service as lead experts and invite others to collaborate.\" ";
                }
                else {
                    $blockControls = false;
                    $blockedTooltip = "";
                }
                ?>

                <div class="main-expert-container" style="float:left;width: 250px;margin-top: 7px;">
                    <input type='text'
                           name="executor_id"
                        <?= $blockedTooltip;?>
                        <?= $blockControls ? 'readonly="readonly"' : ''?>

                           placeholder="Lead expert"
                           style="color:#8f9095 !important;font-family: 'Lato Italic',serif!important;font-size: 14px!important;"/>
                </div>

                <div style="float:right;margin-top: 7px;">
                    <select class="discuss"
                            id="tags"
                            name="tags"
                            style="color:#AFAFAF;height: 46px;font-size:14px;width:286px!important;padding-left: 15px!important;line-height: 40px;">
                        <option value="">Choose a Category</option>
                        <?= OPTION_TAGS(_categories()) ?>
                    </select>
                </div>
            </div>
                <div class="invited-experts-container" style="float:left;width: 100%;">
                    <input type='text'
                           id="eventInv"
                           name="eventInv"
                           placeholder="Invite expert collaborators or client colleagues"
                           style="width:100%!important;color:#8f9095!important;font-family: 'Lato Italic',serif!important;font-size: 14px!important;"/>
                </div>

                <div align="right" style="margin-right: 10px;clear: both;">
                    <a id="addEvent" class="tool"> SUBMIT </a>
                </div>
                <br>
                <?php if($blockControls):?>
                    <div class="click-blocker"
                        <?= $blockedTooltip ?>
                         style="z-index:1000;width: 41%;position: absolute;height: 46px;margin-top: -160px;"></div>
                    <script>
                        $(function(){
                            $('.click-blocker').click(function(e){
                                e.stopPropagation();
                                e.preventDefault();
                                return false;
                            });
                        });
                    </script>
                <?php endif;?>
            </form>
        </div>
    <?php } ?>

    <div id="posting" align="left">
        <input id="attSpan" type="hidden" value="">
        <?php include_once 'new_events_comments_list.php' ?>
    </div>

</div>
</div>
