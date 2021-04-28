<?php
error_reporting(0);
global $base_url;
global $user;

global $ifAdmin;

use Clewed\Insights\InsightEntity;

include('dbcon.php');
$id = sget($_REQUEST, 'id');

if ($_REQUEST['id'] != '') {
    $sql_schedule = "SELECT discussion_status FROM maenna_people WHERE pid=" . $_REQUEST['id'];
} else {
    $user_id = $user->uid;
    $sql_schedule = "SELECT discussion_status FROM maenna_people WHERE pid=" . $user_id;
}
$dis_query = mysql_query($sql_schedule);
$result_dis = mysql_fetch_object($dis_query);
?>
<script type="text/javascript"
        src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
<link rel="stylesheet" type="text/css" href="/themes/maennaco/jui/comments/css/jquery.simple-dtpicker.css"/>
<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.simple-dtpicker.js"></script>
<script type="text/javascript" src="<?php echo $base_url; ?>/themes/maennaco/jui/js/jquery.formatCurrency.js"></script>

<script type="text/javascript"
        src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.autosuggest.js"></script>
<link href="<?php /*echo $base_url; */ ?>/themes/maennaco/jui/comments/css/screen.css?as" type="text/css"
      rel="stylesheet"/>
<link href="<?php echo $base_url; ?>/themes/maennaco/jui/comments/css/autosuggest.css" type="text/css"
      rel="stylesheet"/>
<link href="<?php echo $base_url; ?>/themes/maennaco/jui/comments/css/fileuploader.css" type="text/css"
      rel="stylesheet"/>

<script src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.elastic.js" type="text/javascript"
        charset="utf-8"></script>
<script src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/fileuploader.js" type="text/javascript"
        charset="utf-8"></script>
<script src="<?php echo $base_url; ?>/themes/maennaco/jui/comments/js/jquery.watermarkinput.js"
        type="text/javascript"></script>

<style type="text/css">
    div.content_box .box_title {
        margin-top: 14px;
    }
    .sold-out {
        font-style: italic;
        font-weight: bold;
        color: #00A2BF;
        float: left;
        margin-top: 10px;
        line-height: 12px;
        font-size: 12px;
    }
</style>

<div id='docprev'>
<?php
?>
<script type="text/javascript">

    <?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

    function approve_discussion(type, prof_id, user_id) {
        if (type == 'approve') {
            var status = 1;
            if (!confirm("Are you sure you want to approve and make this Insight visible?")) return false;
        } else {
            var status = 0;
            if (!confirm("Are you sure you want to disapprove and make this Insight hidden?")) return false;
        }
        $.ajax({
            type: 'get',
            url: '/themes/maennaco/includes/delete.php?' +
            'type=approvedis&' +
            'prof_id=' + prof_id + "&" +
            "status=" + status + "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",
            success: function (msg) {
                if (type == 'approve') {
                    $('#approve_dis' + prof_id).html("<a style='float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f;' title='Disapprove' onclick='approve_discussion(\"disapprove\", " + prof_id + "," + user_id + ");' delType = 'event' class='tool follow'><strong>Unlist</strong></a>");
                    $('.featured-toggle-wrapper[data-id=' + prof_id + ']').removeClass('hidden');
                } else {
                    $('#approve_dis' + prof_id).html("<a style='float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f;' title='Approve' onclick='approve_discussion(\"approve\", " + prof_id + "," + user_id + ");' delType = 'event' class='tool follow'><strong>List</strong></a>");
                    $('.featured-toggle-wrapper[data-id=' + prof_id + ']')
                        .addClass('hidden')
                        .find('a')
                        .html('<strong>Feature</strong>')
                        .data('value', 0)
                        .removeClass('done');
                }
            }
        });
    };

function openDialog(boxtitle, boxcontent, spanAtt) {

    $("#dialog").dialog({
        autoOpen: true,
        width: 500,
        title: boxtitle,
        height: 490,
        buttons: {"Close": function () {
            $(this).dialog("close");
        }},
        closeOnEscape: true,
        modal: true
    }).html(boxcontent);

}

$(document).ready(function () {

    init_follow();

    $('body').append("<div id=\"add_mod_dialog\" style=\"display:none\"></div>");

    $('#add_mod_dialog').dialog({
        autoOpen: false,
        title: 'Add Collaborator',
        buttons: {
            'Submit': function () {
                var modname = $('input[name="modname"]').val();
                var modurl = $('input[name="modurl"]').val();
                var discid = $('input[name="discid"]').val();
                var bEdit = $('input[name="edit"]').val();
                var m = $('input[name="_token"]').val();
                var proid = $('input[name="proid"]').val();
                var notify = 0;
                if ($('#notify').prop('checked')) notify = 1;
                if (modname !== 'Add moderator name' && modurl !== 'Add link to moderator\'s Clewed profile') {
                    $.ajax({
                        type: 'POST',
                        url: '/themes/maennaco/includes/add_moderator.php',
                        data: {
                            modname: modname,
                            modurl: modurl,
                            m: m,
                            discid: discid,
                            proid: proid,
                            bEdit: bEdit,
                            notify: notify
                        }
                    }).done(function (data) {
                        if (data.replace(/\s+/g, '') !== 'OK') {
                            alert(data);
                        } else {
                            location.reload(false);
                        }
                    });
                } else {
                    alert("Please fill in both.");
                }
            },
            'Close': function () {
                $(this).dialog('close');
            }
        },
        modal: true
    });

    $('.add_moderator_btn').click(function () {
        var discid = $(this).attr('id').replace('add_mod', '');
        var rel = '';
        if ($(this).find("a").attr('rel') == 'edit') rel = '&type=edit';
        $.ajax({
            type: 'GET',
            url: '/themes/maennaco/includes/add_moderator.php?discid=' + discid + rel
        }).done(function (data) {
            $('#add_mod_dialog').html(data);
            $('#add_mod_dialog').dialog('open');
        });
    });

    $("a[id^='editmat_id']").livequery("click", function (e) {

        e.preventDefault();

        eventid = $(this).attr('id').replace('editmat_id', '');

        $.post("<?php echo $base_url; ?>/themes/maennaco/includes/pro_posts_files.php?type=attachFile&reason=edit", {
        }, function (response) {

            $("#editmatdlg").dialog({
                autoOpen: true,
                width: 500,
                title: 'File attachment',
                resizable: false,
                draggable: false,
                open: function () {
                    //$(this).closest(".ui-dialog").find(".ui-button:first").next().find(".ui-button-text").addClass("uicancel");
                },
                height: 400,
                buttons: {"Save": function () {

                    $.post("<?php echo $base_url; ?>/themes/maennaco/includes/pro_posts_files.php?type=editInsight", {eventid: eventid, str: $(editEventForm).serialize()
                    }, function (response) {

                        alert(response);

                    });

                },
                    "Cancel": function () {
                        $(this).dialog("close");
                    }
                }

            }).html(response);
            var uploader2 = new qq.FileUploader({
                // pass the dom node (ex. $(selector)[0] for jQuery users)
                element: $("#file-uploader2")[0],
                dataType: 'json',
                params: {
                    pro_id: eventid
                },
                // path to server-side upload script
                action: '<?php echo $base_url; ?>/themes/maennaco/includes/file_upload.php',
                onComplete: function (id, fileName, responseJSON) {

                    if (responseJSON['success']) {

                        $("#editEventForm").append('<input name="fileupl" type="hidden" value="' + responseJSON['timestamp'] + "_" + fileName + '"  class="fileInfo" path="' + responseJSON['timestamp'] + "_" + fileName + '" filetitle="' + $("#fileTitleEdit").val() + '" timestamp = "' + responseJSON['timestamp'] + '">');

                    }

                }
            });

        });
    });

    $('#cost').blur(function () {
        $(this).formatCurrency({roundToDecimalPlace: -2, eventOnDecimalsEntered: true});
    }).bind('decimalsEntered', function (e, cents) {
        var errorMsg = 'Please do not enter any cents (.' + cents + ')';
        alert(errorMsg);
    });

    var currAtt = '';
    var availableTags = {items: [
        //Get the advisors and connected users for the autocomplete feature; $companyid was gotten in the earlier phase in new_company_detail_left.php
        <?php
        $Conns = Connections::Com_conns($companyid);
        foreach ($Conns['Advisor'] as $Pro) {
          $pro_uid = $Pro->assignee_uid;
          $pro_maeid = getProId($pro_uid);
          echo '{value: "' . $pro_uid . '", name: "' . $pro_maeid . '"},';
        }

        foreach ($Conns['Visible'] as $Pro) {
          $pro_uid = $Pro->assignee_uid;
          $pro_maeid = getProId($pro_uid);
          echo '{value: "' . $pro_uid . '", name: "' . $pro_maeid . '"},';

          foreach ($Conns['Client'] as $Pro) {
            $pro_uid = $Pro->assignee_uid;
            $pro_maeid = getProId($pro_uid);
            echo '{value: "' . $pro_uid . '", name: "' . $pro_maeid . '"},';
          }
        }
        $q = mysql_query("SELECT projname FROM maenna_company WHERE companyid = '" . $_REQUEST['id'] . "'");
        $q = mysql_fetch_array($q);
        echo '{value: "' . $_REQUEST['id'] . '", name: "' . $q['projname'] . ' "}';
        ?>
    ]};

    $('[id^=rmFile]').livequery("click", function () {

        fileid = $(this).attr('id').replace("rmFile", "");

        $.post("<?php echo $base_url; ?>/themes/maennaco/includes/posts.php?type=removeFile&fileid=" + fileid, {
        }, function (response) {

            $("#file" + fileid).remove();

        });

    });

    $('input[type="file"]').click(function () {

    });
    $("#showEventForm").click(function () {

        var discussion_status = "<?php echo $result_dis->discussion_status; ?>";
        if (discussion_status == 1) {
            $("#eventFormDiv").show();
        } else {
            alert("Organizing a discussion requires screening and approval. Please complete your profile to get started.");
            return false;
        }

    });

    $("#dialog").dialog({
        autoOpen: false
    });
    $(".profile_details").livequery("click", function (e) {

        e.preventDefault();
        var eventid = '';

        if ($(this).attr('ref') == 'pro_id') {
            eventid = 'pro_id=' + $(this).attr('id').replace('pro_id', '');
        }
        else {
            eventid = 'eventid=' + $(this).attr('id').replace('edit_id', '');
        }
        uid = <?= $user->uid; ?>;
        $.post("<?php echo $base_url; ?>/themes/maennaco/includes/pro_posts.php?type=profileInfo&display=true&" + eventid + "&uid=" + uid + "&base_url=<?php echo $base_url; ?>", {
        }, function (response) {

            $("#eveditdlg").dialog({
                autoOpen: true,
                width: 650,
                title: 'PROFILE',
                resizable: false,
                draggable: false,
                height: 400,
                closeText: 'hide',
                buttons: {
                    /*"Close": function() { $(this).dialog("close"); }*/

                }, closeOnEscape: true,
                modal: true
            }).html(response);
        });
    });

    $(".evedit").livequery("click", function (e) {

        e.preventDefault();

        var eventid = $(this).attr('id').replace('edit_id', '');

        $.post("<?php echo $base_url; ?>/themes/maennaco/includes/pro_posts.php?type=eventEdit&display=true&eventid=" + eventid, {
        }, function (response) {

            $("#eveditdlg").dialog({
                autoOpen: true,
                width: 650,
                title: 'Event edit',
                resizable: false,
                draggable: false,
                open: function () {
                    $(this).closest(".ui-dialog").find(".ui-button:first").next().find(".ui-button-text").addClass("uicancel");
                },
                height: 600,
                buttons: {
                    "Save": function () {
                        var theForm = $('#editEventForm');
                        links = '';
                        name = '';
                        var title = theForm.children("#eventType").val();
                        var name = new Array();
                        var links = new Array();
                        $("#text input[name=name]").each(function () {
                            name.push($(this).val());
                        });

                        var taskArray = new Array();
                        $("#text input[name=link]").each(function () {
                            links.push($(this).val());
                        });

                        var names = name;
                        var datetime = theForm.children("#date").val();
                        var loc = theForm.children("#eventLoc").val();
                        var agenda = theForm.children("#eventDesc").val();
                        var whyattend = theForm.children("#whyattend").val();
                        var eventcost = theForm.children("#eventCost").val();
                        var eventcapacity = theForm.children("#eventCapacity").val();
                        var eventtags = theForm.children("#eventTags").val();
                        var uname = '<?= $user->name; ?>';
                        <?php
                        if ($_REQUEST['id'] == '') {
                          $user_id = $user->uid;
                        } else {
                          $user_id = $_REQUEST['id'];
                        }
                        ?>
                        var cid = <?= $user_id; ?>;
                        var uid = <?= $user->uid; ?>;
                        var filesEdit = [];
                        if ($("#chkNot").is(":checked")) {
                            var notif = 'true';
                        } else {
                            var notif = false;
                        }
                        var invitees = theForm.children(".as-selections").children(".as-original").children(".as-values").val();
                        $(".fileInfo").each(function () {
                            var tmpArr1 = {'path': $(this).attr('path'), 'title': $(this).attr('filetitle'), 'timestamp': $(this).attr('timestamp')};
                            filesEdit.push(tmpArr1);
                        });

                        $.post(
                            '/themes/maennaco/includes/pro_posts.php?type=eventEdit',
                            {
                                eventid: eventid,
                                title: title,
                                datetime: datetime,
                                loc: loc,
                                agenda: agenda,
                                whyattend: whyattend,
                                cost: eventcost,
                                capacity: eventcapacity,
                                tags: eventtags,
                                u: uname,
                                cid: cid,
                                uid: uid,
                                files: filesEdit,
                                notif: notif,
                                links: links,
                                names: names,
                                use_discount: theForm.children("#use_discount").prop('checked'),
                                approve_discount: theForm.children("#approve_discount").prop('checked')
                            },
                            function (response) {
                                location.reload();
                            });

                    },
                    "Close": function () {
                        if (confirm('Are you sure you want to close without saving?')) $(this).dialog("close");
                    }
                },
                closeOnEscape: true,
                modal: true
            }).html(response);

            $("#eventInv").autoSuggest(availableTags.items, {selectedItemProp: "name", searchObjProps: "name"});
            $("#eventDesc").elastic();
            $(".as-selections").width(600);
            var uploader1 = new qq.FileUploader({
                // pass the dom node (ex. $(selector)[0] for jQuery users)
                element: $("#file-uploader1")[0],
                dataType: 'json',
                params: {
                    pro_id: eventid
                },
                // path to server-side upload script
                action: '<?php echo $base_url; ?>/themes/maennaco/includes/file_upload.php',
                onComplete: function (id, fileName, responseJSON) {

                    if (responseJSON['success']) {

                        $("#eventType").before('<input type="hidden" class="fileInfo" path="' + fileName + '" filetitle="' + $("#fileTitleEdit").val() + '" timestamp = "' + responseJSON['timestamp'] + '">');
                        $("#fileTitleEdit").val('');

                    }

                }
            });

        });

    });

    /********* Attach Files  *********/

    $(".file").livequery("click", function (e) {

        e.preventDefault();

        fileid = $(this).attr('id').replace('file_id', '');

        $.post("<?php echo $base_url; ?>/themes/maennaco/includes/pro_posts_files.php?type=attachFile", {
        }, function (response) {

            $("#eveditdlg").dialog({
                autoOpen: true,
                width: 650,
                title: 'File Attachment',
                height: 300,
                buttons: {
                    "Save": function () {

                        title = $("#editEventForm #event").children("#eventType").val();
                        /*var title = '';
                         $("#event textarea").each(function () {
                         title += $(this).val() + '###';
                         });*/
                        var name = new Array();
                        var links = new Array();
                        $("#text input[name=name]").each(function () {
                            name.push($(this).val());
                        });

                        var taskArray = new Array();
                        $("#text input[name=link]").each(function () {
                            links.push($(this).val());
                        });

                        datetime = $("#editEventForm").children("#eventDate").val();
                        loc = $("#editEventForm").children("#eventLoc").val();
                        agenda = $("#editEventForm").children("#eventDesc").val();
                        uname = '<?= $user->name; ?>';
                        <?php
                        if ($_REQUEST['id'] == '') {
                          $user_id = $user->uid;
                        } else {
                          $user_id = $_REQUEST['id'];
                        }
                        ?>

                        cid = <?= $user_id; ?>;
                        uid = <?= $user->uid; ?>;
                        filesEdit = [];
                        if ($("#chkNot").is(":checked")) {
                            notif = 'true';
                        }
                        else {
                            notif = false;
                        }

                        invitees = $("#editEventForm").children(".as-selections").children(".as-original").children(".as-values").val();
                        $(".fileInfo").each(function () {
                            var tmpArr1 = {'path': $(this).attr('path'), 'title': $(this).attr('filetitle'), 'timestamp': $(this).attr('timestamp')};
                            filesEdit.push(tmpArr1);
                        });

                        $.post("<?php echo $base_url; ?>/themes/maennaco/includes/pro_posts_files.php?type=attachFile", {title: title, datetime: datetime, loc: loc, agenda: agenda, invitees: invitees, u: uname, cid: cid, uid: uid, files: filesEdit, notif: notif, names: name, links: links},
                            function (response) {
                                //location.reload();
                                $('#eveditdlg').dialog("close");

                            });

                    },
                    "Close": function () {
                        $(this).dialog("close");
                    }

                },
                closeOnEscape: true,
                modal: true
            }).html(response);

            $("#eventInv").autoSuggest(availableTags.items, {selectedItemProp: "name", searchObjProps: "name"});
            $("#eventDesc").elastic();
            $(".as-selections").width(600);
            var uploader1 = new qq.FileUploader({
                // pass the dom node (ex. $(selector)[0] for jQuery users)
                element: $("#file-uploader1")[0],
                // path to server-side upload script
                params: {
                    pro_id: $("#file-uploader1").data('pro-id')
                },
                action: '<?php echo $base_url; ?>/themes/maennaco/includes/file_upload.php',
                onComplete: function (id, fileName, responseJSON) {

                    if (responseJSON['success']) {
                        $("#eventType").before('<input type="hidden" class="fileInfo" path="' + fileName + '" filetitle="' + $("#fileTitleEdit").val() + '" timestamp = "' + responseJSON['timestamp'] + '">');
                        $("#fileTitleEdit").val('');

                    }

                }
            });

        });

    });

    /******** End Attach Files **********/

    $("#rsvp-button").livequery("click", function () {

        status = $('input[name=invStatus]:checked', '#invrsvp').val();
        uid = '<?= $user->uid; ?>';
        eventid = $("#dlgInfo").attr('eventid').replace('event', '');

        $.post("<?php echo $base_url; ?>/themes/maennaco/includes/posts.php?type=confirmAtt&status=" + status + "&uid=" + uid + "&eventid=" + eventid, {
        }, function (response) {

            if (response.trim() == 'overlap') {
                alert("You are already attending event at that time!");
                return;
            }

            $("#" + $("#attSpan").val()).html(response);

            $("#dialog").dialog("close");

        });

    });

    $(".eventTitle").livequery("click", function (evt) {

        evt.preventDefault();
        eventDiv = $(this).parent().parent().parent();
        eventForm = $(this).parent();
        ccfiles = $(this).parent().parent().find("<:nth-child(3)");

        boxtitle = eventForm.find(">:first-child").html();
        boxcontent = '<span id="dlgInfo" eventid = "' + eventDiv.attr('id') + '" style="float:left; font-size:15px;"><strong>' + eventForm.find("<:first-child").html() + '</strong></span><div id="clear" style="clear:both"></div><span style="float:left;">' + eventForm.find("<:nth-child(3)").html() + '</span><br><span style="float:left;">&nbsp;' + eventForm.find("<:nth-child(5)").html() + '</span><br></div>';

        boxcontent = boxcontent + '<div id="clear" style="clear:both"><br>';

        if (eventForm.children(".invatt").length > 0) {
            boxcontent = boxcontent + '<div style="float:left;">' + ccfiles.find("<:nth-child(1)").html().replace("rsvp", "") + '</div>';
        }

        boxcontent = boxcontent + '<div id="clear" style="clear:both"><br><span style="float:left; ">AGENDA:<div style="margin-left:15px;">' + eventForm.find("<:nth-child(7)").html() + '</div> </span><div id="clear" style="clear:both"><br><span style="float:left;">FILES:</span><br><div style="margin-left:15px;>"' + ccfiles.children(".attFiles").html().replace("Attached files:", "") + '</div></span>';

        boxcontent = boxcontent + '<div id="clear" style="clear:both"><br><span style="float:left;">ATTENDING?</span><form style="float:left;" id="invrsvp" action=""><label style="margin-left:20px;"><input type="radio" name="invStatus" value="confirmed" class="styled"><strong>Yes</strong></label><label style="margin-left:20px;"><input type="radio" name="invStatus" value="declined" class="styled"><strong>No</strong></label><label style="margin-left:20px;"><input type="radio" name="invStatus" value="maybe" class="styled"><strong>May be</strong></label><a id="rsvp-button" style="margin-left:30px;" class="small button"> SAVE </a></form>';

        if (boxcontent != '') {
            boxcontent = boxcontent.replace(/\\n/g, '<br />');
        }
        $("#attSpan").val("att" + eventDiv.attr('id').replace('event', ''));
        openDialog(boxtitle, boxcontent, $(this).parent().attr('id'));

    });

    $(".rsvp").livequery("click", function (evt) {

        evt.preventDefault();
        eventDiv = $(this).parent().parent().parent().parent().parent();
        eventForm = $(this).parent().parent().parent().parent().find("<:first-child");
        ccfiles = $(this).parent().parent().parent();

        boxtitle = eventForm.find(">:first-child").html();
        boxcontent = '<span id="dlgInfo" eventid = "' + eventDiv.attr('id') + '" style="float:left; font-size:15px;"><strong>' + eventForm.find("<:first-child").html() + '</strong></span><div id="clear" style="clear:both"></div><span style="float:left; color:#6792D0;">' + eventForm.find("<:nth-child(3)").html() + '</span><span style="float:left; color:#6792D0;">' + eventForm.find("<:nth-child(5)").html() + '</span><br></div>';

        boxcontent = boxcontent + '<div id="clear" style="clear:both"><br><span style="float:left;">ATTENDES:</span> <div style="margin-left:15px;float:left;">' + ccfiles.find("<:first-child").html().replace("rsvp", "") + '</div>';

        boxcontent = boxcontent + '<div id="clear" style="clear:both"><br><span style="float:left; ">AGENDA:<div style="margin-left:15px;">' + eventForm.find("<:nth-child(7)").html() + '</div> </span><div id="clear" style="clear:both"><br><span style="float:left;">FILES:</span><br><div style="margin-left:15px;>"' + ccfiles.children(".attFiles").html().replace("Attached files:", "") + '</div></span>';

        boxcontent = boxcontent + '<div id="clear" style="clear:both"><br><span style="float:left;">ATTENDING?</span><form style="float:left;" id="invrsvp" action=""><label style="margin-left:20px;"><input type="radio" name="invStatus" value="confirmed" class="styled"><strong>Yes</strong></label><label style="margin-left:20px;"><input type="radio" name="invStatus" value="declined" class="styled"><strong>No</strong></label><label style="margin-left:20px;"><input type="radio" name="invStatus" value="maybe" class="styled"><strong>May be</strong></label><a id="rsvp-button" style="margin-left:30px;" class="small button"> SAVE </a></form>';

        if (boxcontent != '') {
            boxcontent = boxcontent.replace(/\\n/g, '<br />');
        }
        $("#attSpan").val($(this).parent().attr('id'));
        openDialog(boxtitle, boxcontent, $(this).parent().attr('id'));

    });

    $("#eventInv").autoSuggest(availableTags.items, {selectedItemProp: "name", searchObjProps: "name"});

    $('.qq-upload-remove').live("click", function () {

        var fileToRemove = $(this).parent().children('.qq-upload-file').html();
        $("input[path='" + fileToRemove + "']").remove();
        $(this).parent().remove();
    });
    $('[id^=shareButton]').livequery("click", function () {

        tarea = $('textarea[eventid=' + $(this).attr('textref') + ']');
        var a = encodeURIComponent(tarea.val());
        var m = tarea.attr("alt");
        var u = tarea.attr("name");
        var uid = '<?= $user->uid; ?>';
        var eventid = tarea.attr('eventid');

        if (a != "Discuss a topic or ask a question on this file ...") {

            $.post("<?php echo $base_url; ?>/themes/maennaco/includes/pro_posts_files.php?type=eventcom&u=" + u + "&m=" + m + "&value=" + a + "&eventid=" + eventid + "&uid=" + uid, {
            }, function (response) {

                tarea.parent().parent().parent().parent().after($(response).show());
                tarea.val("Share ideas on this topic");

                $('textarea').elastic();
                $(".commentMark").Watermark("Got advice / question on this topic?");
                tarea.Watermark("watermark", "#369");

                $(".commentMark").Watermark("watermark", "#EEEEEE");

            });
        }
    });

    $('.watermark').livequery("focus", function (e) {

        sbmBtt = $(this).attr('eventid');

        $('a[textref=' + sbmBtt + ']').show();
    });

    //$('.watermark').livequery("focusout", function(e){
    //
    //	sbmBtt = $(this).attr('eventid');
    //
    //	$('a[textref='+sbmBtt+']').hide('slow');
    //});

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
        var uid = '<?= $user->uid; ?>';

        if (comment_text != "Got advice / question on this topic?") {
            $.post("<?php echo $base_url; ?>/themes/maennaco/includes/add_comment.php?u=" + u + "&comment_text=" + comment_text + "&post_id=" + getpID + "&m=" + m + "&uid=" + uid, {
            }, function (response) {

                $('#CommentPosted' + getpID).append($(response).show());
                $("#commentMark-" + getpID).val("Got advice / question on this topic?");

            });
        }

    });

    //more records show
    $('a.more_records').livequery("click", function (e) {

        var next = $('a.more_records').attr('id').replace('more_', '');

        $.post("<?php echo $base_url; ?>/themes/maennaco/includes/posts.php?show_more_post=" + next, {
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
            url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete_comment.php?c_id=' + c_id + "&u=" + u + "&m=" + m,
            data: '',
            beforeSend: function () {

            },
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
            url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?type=professional&id=' + temp + "&u=" + u + "&m=" + m,
            data: '',
            beforeSend: function () {

            },
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
                url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?type=event&id=' + temp + "&u=" + u + "&m=" + m,
                data: '',
                beforeSend: function () {

                },
                success: function () {

                    $('#event' + temp).remove();

                }

            });
            return true;

        }

        $.ajax({
            type: 'get',
            url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?id=' + temp + "&u=" + u + "&m=" + m,
            data: '',
            beforeSend: function () {

            },
            success: function () {

                $('#record-' + temp).remove();

            }

        });

    });

    $('textarea').elastic();

    jQuery(function ($) {

        $(".watermark").Watermark("Share ideas on this topic");
        $("#eventDate").Watermark("When?").addClass('watermark');
        ;
        //$("#eventType").Watermark("TITLE").addClass('watermark');;
        $("#eventLoc").Watermark("Where?").addClass('watermark');
        ;
        $("#eventDesc").Watermark("Agenda?").addClass('watermark');
        ;
        $("#whyattend").Watermark("Insight description").addClass('watermark');
        ;
        //$("#cost").Watermark("COST").addClass('watermark');;
        $("#capacity").Watermark("Capacity").addClass('watermark');
        ;
        $("#eventInv").Watermark("+ Add people to notify");
        $(".commentMark").Watermark("Got advice / question on this topic?");

    });

    jQuery(function ($) {

        /*
         $(".watermark").Watermark("watermark","#369");
         $("#eventDate").Watermark("watermark","#369");
         $("#eventType").Watermark("watermark","#369");
         $("#eventLoc").Watermark("watermark","#369");
         $("#eventDesc").Watermark("watermark","#369");
         $("#eventInv").Watermark("watermark","#369");
         $("#cost").Watermark("watermark","#369");
         $("#capacity").Watermark("watermark","#369");
         $("#whyattend").Watermark("watermark","#369");
         $(".commentMark").Watermark("watermark","#EEEEEE");
         */
    });

    function UseData() {

        $.Watermark.HideAll();

        //Do Stuff

        $.Watermark.ShowAll();

    }

});

// ]]>

function showprodetails(id) {
    $("#showpro" + id).show();
}

function discussion(id, proid) {

    window.location.href = "./account?tab=professionals&page=pro_detail&id=" + id + "&section=pro_industry_view&type=details&pro_id=" + proid;
}

function like_discussion(type, prof_id, userid) {

    if (type == 'like') {
        var status = 1;
    }
    else {
        var status = 0;
    }

    <?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

    $.ajax({
        type: 'get',
        url: '<?php echo $base_url; ?>/themes/maennaco/includes/delete.php?' +
            'type=like_discussion&' +
            'prof_id=' + prof_id + '&' +
            'userid=' + userid + '&' +
            'status=' + status + "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",
        data: '',
        beforeSend: function () {

        },
        success: function () {

            location.reload();
            /*if(type == 'like')
             {
             $('#likepost').html("<a style='cursor:pointer;' onclick='like_discussion(\"unlike\", "+prof_id+","+userid+");'>Unlike</a>");
             }
             else
             {
             $('#likepost').html("<a style='cursor:pointer;' onclick='like_discussion(\"like\", "+prof_id+","+userid+");'>Like</a>");
             }*/

        }

    });
}
function isNumberKey(evt) {
    var charCode = (evt.which) ? evt.which : event.keyCode;
    if (charCode != 46 && charCode > 31
        && (charCode < 48 || charCode > 57)) {
        return false;
    }

    return true;
}

</script>

<div> <!--align="center"-->
<?php
/* if ($_REQUEST['id'] == $user->uid || end($user->roles) == 'Super admin' || end($user->roles) == 'Maennaco admin') { */

//if ($AccessObj->Com_sections['discussions']['sections']['nav_menu_right']['access'] == 'write') {
?>

      <!--<a id="showEventForm" class = "small button" style=" font-size:12px;float:right; margin-top:10px;">SCHEDULE A DISCUSSION</a>-->

<div id="eveditdlg"></div>
<div id="editmatdlg"></div>
<div id="profiledlg"></div>
<div style="clear:both;"></div>
<div id="eventFormDiv" style="display:none;">
<form action="" method="post" name="addEvent" id="addEventForm">
<div class="dischead">
    <div class="dtitle">schedule discussion</div>
    <!-- <div class="dedit"><a href='#'>ATTACH FILE</a></div><div style="clear:both"></div>-->
    <div class="dedit"><!--<a href='#'>--><a style="float:right; margin-left:10px;" href="#"
                                             id="file_id<?php echo $row1['id'] ?>"
                                             alt="<?php echo md5($row1['id'] . $user->name . "kyarata75") ?>"
                                             name="<?= $user->name; ?>" delType='event' class="tool file"
                                             title="Add materials: Files / Links"><img
                src="<?php echo $base_url; ?>/themes/maennaco/images/upload_bk.png"></a></div>
    <div style="clear:both"></div>
</div>

<input class="discuss" id="eventType" placeholder="Title" name="eventType" type="text"/>
<!--<textarea class="input" id="eventType" name="eventType" cols="60" placeholder="TITLE" ></textarea>-->
<input class="input" id="eventLoc" placeholder="Where?" name="eventLoc" cols="20">
<input required="true" type='text' id="eventDate" placeholder="When?" name="eventDate" class='datepicker'
       style="15px/160% Verdana, sans-serif;"/><br>

<div id="picker" title="">
    <table cellspacing="0" cellpadding="0" border="0px">
        <tr>
            <td style="vertical-align: top;"><br/>
                <input disabled="disabled" type="text" id="date-picker"/></td>
            <td class="timeTable" style="width:100%;vertical-align: top;"></td>
        </tr>
    </table>
</div>
<style>
    /*.ui-dialog{width: 1050px !important;}*/
    .datepicker {
        width: 238px !important;
    }

    .datepicker_timelist {
        width: 65px !important;
        height: 180px !important;
    }

    .datepicker_calendar {
        width: 168px !important;
    }

    .tableTime {
        text-align: center;
        margin-top: 0px;
    }

    .tableTime td {
        border: 1px solid #E8E9EA;
        width: 7px;
    }

    .tableTime td.approved {
        background-color: #686B72 !important;
    }

    .bGreen {
        background-color: green;
    }

    .bGray {
        background-color: #E8E9EA;
    }

    .bCurrent {
        background-color: #e3eef2
    }

    .bRed {
        background-color: red;
    }

    .datepicker > .datepicker_inner_container > .datepicker_calendar > .datepicker_table > tbody > tr > td.wday_sat:not(.day_in_past) {
        color: #43a0c1;
    }

    td[rel^=time] {
        cursor: pointer;
    }

    td[rel^=time]:hover {
        background-color: #E3EEF2;
    }

    td.disabled[rel^=time] {
        background-color: #E8E9EA;
        cursor: default;
    }

    td[rel^=time][data-disabledbyme = true] {
        background-color: #E8E9EA;
        cursor: default;
    }

    td[rel^=time].active {
        background-color: #686B72 !important;
        cursor: default;
    }
</style>

<script type='text/javascript'>
jQuery(function ($) {
    $(document).ready(function () {
        var days = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
        var month = [
            'January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October',
            'November', 'December'
        ];
        var createDateString = function (year, month, day, hour, minute) {
            month++;
            return (month >= 10 ? month : "0" + month) + "-" + (day >= 10 ? day : "0" + day) + "-" + year + " " + (hour >= 10 ? hour : "0" + hour) + ":" + (minute >= 10 ? minute : "0" + minute);
        };

        var insights = {items: [
            <?php
            foreach($insights_future as $pid => $val_arr)
                 {echo "{date: '$val_arr[0]',approved: '$val_arr[1]',title: '".addslashes($val_arr[2])."'},";
}

            ?>

        ]};

        window.getParameterByName = function (name) {
            name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
            var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
                results = regex.exec(location.search);
            return results == null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
        };

        window.changeTimeTable = function (date, creator) {

            $(".timeTable").html('<img style="margin-top:28px;" src="themes/maennaco/images/ajax-loader.gif">');

            $.post("<?php echo $base_url; ?>/themes/maennaco/includes/fetch_insights_calendar.php", {date: date, creator: creator
            }, function (response) {
                $(".timeTable").html(response);

            });
        };

        var initDatePicker = function () {

            var picker = $('#date-picker');
            if (!picker.data('pickerId')) {
                picker.appendDtpicker({
                    'inline': true,
                    "locale": "en",
                    "futureOnly": true,
                    'dateFormat': 'MM-DD-YYYY hh:mm',
                    'minuteInterval': 15,
                    'insights': insights.items

                });

            }

        };

        if ($('#date-picker')) {

            initDatePicker();
            $('#date-picker').val('');
            //$('#date-picker').change(changeInsight);

            $('td[rel^=time]').live("click", function () {
                var item = parseInt($(this).attr('rel').split('time')[1]);
                if (isNaN(item)) {
                    return;
                }

                if ($(this).hasClass('disabled') || $(this).data('disabledbyme') == true) {
                    return;
                }
                for (i = 0; i < 4; i++) {

                    if ($("td[rel=time" + (item + i) + "]").hasClass('approved')) {
                        alert('Please select another time. You have a conflict with a live insight at the selected time.');
                        return;
                    }

                }
                var itemHours = parseInt(item / 4),
                    itemMinutes = (item * 15) % 60,
                    dtObj = $('#date-picker'),
                    currentDate = new Date('<?=date('Y/m/d H:i');?>:00');
                if (dtObj.val()) {
                    currentDate = new Date((dtObj.val() + ":00").replace(/\-/g, '/'));
                }
                var oldVal = dtObj.val();
                dtObj.val(createDateString(currentDate.getFullYear(), currentDate.getMonth(), currentDate.getDate(), itemHours, itemMinutes));

                $(".bCurrent").removeClass('bCurrent');

                for (i = 0; i < 4; i++) { //TODO: 1 hour duration from settings

                    $("td[rel=time" + (item + i) + "]").addClass('bCurrent');

                }

                $(".timelist_item").removeClass('active');
                /*                    if (changeInsight()==false){
                 dtObj.val(oldVal);
                 changeInsight();
                 }*/
            });
        }

        $(".datepicker").live('focus', function () {
            if ($("#ins_status").val() == '1') {
                if (!confirm("This insight is live. Moving the time for this insight may cause cause trouble for attendees. Are you sure you want to change this time?")) return;
            }
//                        initDatePicker(); //TODO: will ve usefull later
            $('#date-picker').val($(this).val());
            //changeInsight();
            title = $(this).parent().find("#eventType").val();
            if (title == '' || title == 'undefined') title = 'Create Insight';

            $("#picker").dialog('option', 'title', title);

            $date = $(this).val().split(' ')[0];
            $creator = $(this).attr('creator');

            if ($('#date-picker').val().length) {

                $id = getParameterByName('id');

                if ($.trim($id) == '') {

                    $.post("<?php echo $base_url; ?>/themes/maennaco/includes/fetch_insights_calendar.php?type=getProInsights", {id: $creator}, function (response) {

                        $('#date-picker').handleDtpicker('setDateStr', $('#date-picker').val(), response);

                    }, "json");
                }
                else {
                    $('#date-picker').handleDtpicker('setDateStr', $('#date-picker').val());
                }
            }

            $(".timeTable").html('<img style="margin-top:28px;" src="themes/maennaco/images/ajax-loader.gif">');

            $.post("<?php echo $base_url; ?>/themes/maennaco/includes/fetch_insights_calendar.php", {date: $date, creator: $creator
            }, function (response) {
                $(".timeTable").html(response);

            });

            $("#picker").dialog("open");
        });

        $("#picker").dialog({
            modal: true,
            autoOpen: false,
            width: 1050,
            height: 550,
            buttons: {
                "Save": function () {

                    $('.datepicker').val($('#date-picker').val());
                    $(this).dialog("close");
                },
                Cancel: function () {
                    var selectedDate = $('.datepicker').val();
                    if (Date.parse(selectedDate)) {
                        $('#date-picker').val(selectedDate);
                    }
//                                $('#date-picker').handleDtpicker('destroy'); //TODO: will be usefull later

                    $(this).dialog("close");
                }
            }
        });

//              $(".datepicker").datetimepicker({dateFormat: 'M d'});
//					$("#eventDate").datetimepicker({dateFormat: 'M d'});
        //$("#test").datetimepicker({dateFormat: 'yy-mm-dd'});
        var uploader = new qq.FileUploader({
            // pass the dom node (ex. $(selector)[0] for jQuery users)
            element: $("#file-uploader")[0],
            // path to server-side upload script
            action: '/themes/maennaco/includes/file_upload.php',
            onComplete: function (id, fileName, responseJSON) {

                if (responseJSON['success']) {
                    $("#eventType").before('<input type="hidden" class="fileInfo" path="' + fileName + '" filetitle="' + $("#fileTitle").val() + '" timestamp = "' + responseJSON['timestamp'] + '">');
                    $("#fileTitle").val('');

                }

            }
        });
    });
});
</script>
<textarea class="input" id="eventDesc" placeholder="Agenda" name="eventDesc" cols="60"
          style="height:95px;"></textarea><br>
<textarea class="input" id="whyattend" placeholder="Insight description" name="whyattend" cols="60"></textarea><br>
<input class="input" type='text' placeholder="Cost" id="cost" name="cost"/>
<input class="input" type='text' placeholder="Capacity" id="capacity" onkeypress="return isNumberKey(event)"
       name="capacity"/>
<!--<input  class="input" type='text' placeholder="ADD LINK" id="link" onkeypress="return isNumberKey(event)" name="link" style="width:207px!important;" />


<div id="tags">-->
<select class="discuss" id="tags" name="tags">
    <option>Choose a Category</option>
    <?php

    echo OPTION_TAGS(_categories());

    ?>
</select>
<!--</div>--><br>

<!--<textarea class="input" id="eventInv" name="eventInv" style="height:20px" cols="60"></textarea><br>-->

<div id='evFile' style='text-align:left; display:none'>
    <input style="background:none;float:left;border:solid 2px #DCE6F5;" type=text id="fileTitle"/>

    <div id="file-uploader">
        <noscript>
            <p>Please enable JavaScript to use file uploader.</p>
            <!-- or put a simple form for upload here -->
        </noscript>

    </div>
    <!--<a style="pointer:cursor; float:left; padding:0;" class='hidebox button' boxid=evFile>CLOSE</a>-->
    <div style="clear:both"></div>
</div>
<?= js_init("init_openbox();init_hidebox();"); ?>

<div class="diss">

    <input type="button" id="addEvent" class="small button" value="Submit"/>
    <input type="submit" class="small button" value="Cancel" id="pro_cancel"/>
    <!--				<a class="small button"> CANCEL </a>
            <a id="addEvent" class="small button"> SUBMIT </a>
    -->
</div>
<!--
<br><hr style="color: #E7E6E7;background-color: #E7E6E7;height: 5px;">
<br>-->

</form>
</div>
<?php //}  ?>

<div id="posting" align="left">
<input id="attSpan" type="hidden" value="">

<?php
//require('pro_comments_list.php');
?>
<?php
$page = sget($_REQUEST, 'page');
$type = sget($_REQUEST, 'type');
$idd = $user->uid;
$tab = sget($_REQUEST, 'tab');

if ($type == 'registered') {
    $sql_result = "SELECT mp.*, mpp.pro_id FROM  maenna_professional as mp,
          maenna_professional_payments as mpp WHERE mp.id = mpp.pro_id  and
          mpp.user_id =" . $idd;
} elseif ($type == 'moderated') {
    $sql_result = "SELECT mp.*, mpp.discussion_id FROM  maenna_professional as mp,
          maenna_discussion_moderator as mpp WHERE mp.id = mpp.discussion_id  and
          mpp.user_id =" . $idd;
} else {
    $sql_result = "SELECT mp.*, mdf.prof_id FROM  maenna_professional as mp,
          maennaco_discussion_follow as mdf WHERE mp.id = mdf.prof_id and
          mdf.user_id =" . $idd;
}

/*if ($_REQUEST['include_archive'] == '1') {
    $sort_style = 'DESC';
} else $sort_style = 'ASC';*/
$sort_style = 'DESC';
$sql_result .= " ORDER BY mp.datetime " . $sort_style;


$result1 = mysql_query($sql_result);

while ($row1 = mysql_fetch_array($result1)) {
    if ($page == 'myinsights' || $_REQUEST['id'] == '') {
        $id = $row1['postedby'];
    } else {
        $id = sget($_REQUEST, 'id');
    }
    $sql_expertise = mysql_query("SELECT * FROM  `maenna_people` mp JOIN users_roles ur ON mp.pid = ur.uid WHERE mp.pid = '" . (int) $row1['postedby'] . "'");
    $sql_exp_result = mysql_fetch_array($sql_expertise);
    if ($sql_exp_result['username_type'] == 1) {
        $P_username = ucfirst($sql_exp_result['firstname']);
    } else $P_username = ucfirst($sql_exp_result['firstname']) . ' ' . ucfirst($sql_exp_result['lastname']);
    if ($sql_exp_result['rid'] == 10)
        $P_username = 'Super_admin';
    elseif ($sql_exp_result['rid'] == 6)
        $P_username = 'Clewed admin';
    $sql_likes = mysql_query("SELECT * FROM  `like_discussion_professional` WHERE `user_id` = '" . $id . "' AND `prof_id` = '" . $row1['id'] . "' ORDER BY id DESC");
    $sql_likes_result = mysql_num_rows($sql_likes);
    $sql_remaining_spots = mysql_query("SELECT * FROM  `maenna_professional_payments` WHERE `user_id` = '" . $id . "' AND `pro_id` = '" . $row1['id'] . "' ORDER BY id DESC");
    $sql_remaining_spots_count = mysql_num_rows($sql_remaining_spots);
    $result3 = mysql_query(
        "SELECT *
                                 FROM  `like_discussion_professional`
                                 WHERE  prof_id = '" . $row1['id'] . "' and user_id = '" . $id . "'"
    );
    $likepost = mysql_num_rows($result3);
    $likepostArray = mysql_fetch_array($result3);
    date_default_timezone_set('EST');

    $userId = $row1['postedby'];
    $avatar = getAvatarUrl($userId, "150");

    $sql               = "select user_id from maenna_discussion_moderator where discussion_id = '" . $row1['id'] . "' limit 1";
    $moderatorResource = mysql_query(
        $sql
    );
    $moderatorId       = 0;
    $response          = mysql_fetch_assoc($moderatorResource);
    $moderatorAvatar   = false;
    if ($response) {
        $moderatorId     = $response['user_id'];
        $moderatorAvatar = getAvatarUrl($moderatorId, "150");
    }
    ?>

    <div class="event" <?= $border; ?> id="event<?= $row1['id']; ?>">
        <div id="clear" style="clear:both"></div>
        <div
            style="float:left;<?php if ($moderatorAvatar): ?>width: 0px;<?php endif; ?>margin: 0px 0px 0px 0px;padding:3px 0px 0px 0px;height: 38px;">
            <img src="<?= $avatar; ?>" alt="" width="35px" class="grayscale"/>
        </div>
        <?php if ($moderatorAvatar): ?>
            <div style="float:left;height: 38px;margin-top:38px;">
                <img src="<?= $moderatorAvatar; ?>" alt="" width="35px" class="grayscale"/>
            </div>
        <?php endif; ?>
        <!-- calendar -->

        <div class="event-info">
            <div style="margin-left:10px; float:left; width:525px;margin-top:5px;">
                <a onclick="discussion('<?= $id ?>', '<?= $row1['id'] ?>');"
                   href="<?php echo $base_url; ?>/account?tab=professionals&page=pro_detail&id=<?= $id ?>&section=pro_industry_view&type=details&pro_id=<?= $row1['id'] ?>"><span
                        class="eventTitle"
                        style="width:465px;float:left;font-size:18px;font-weight:bold; cursor:pointer; text-decoration:none;white-space:nowrap;overflow:hidden !important;text-overflow: ellipsis;"><?= replace_email(substr($row1['title'], 0, 80)); ?></span></a>
              <span
                  style="float:right; padding:0px; margin:-3px 0px 0px 0px; font-size:10px; color:#00A3BF; font-weight:normal; font-family:'LatoRegular';">
                <?php if ($likepost) { ?>
                    <span id="likepost<?= $row1['id'] ?>"
                          style="font-size: 14px;font-style:normal;font-family: 'Lato Bold';"><a
                            style="cursor:pointer; color:#00A3BF;font-style:normal;font-size:14px"
                            onclick="like_discussion('unlike', '<?= $row1['id'] ?>', '<?= $user->uid ?>');">Unlike</a> &nbsp;<?php echo ($sql_likes_result != '0') ? $sql_likes_result : '0'; ?></span>
                <?php } else { ?>
                    <span id="likepost<?= $row1['id'] ?>"
                          style="font-size: 14px;font-style:normal;font-family: 'Lato Bold';"><a
                            style="cursor:pointer;color:#00A3BF;font-style:normal;font-size:14px"
                            onclick="like_discussion('like', '<?= $row1['id'] ?>', '<?= $user->uid ?>');">Like</a>&nbsp; <?php echo ($sql_likes_result != '0') ? $sql_likes_result : '0'; ?></span>
                <?php } ?>

              </span>
                <!--<div id="clear" style="clear:both"></div>-->
                <!--<span class="prodatetime"><? /*= date("l, M j, Y g:i A T ", $row1['datetime']); */ ?></span>-->
                <div id="clear" style="clear:both"></div>
                <div>
                    <span style="color:#91939E !important" class="inv_status">By: </span><a href="#" id="edit_id<?php echo $row1['id']; ?>" style="color:#00a2bf;" class="profile_details"
                       name="<?= $user->name; ?>">
                        <?php echo $P_username; ?>, <?= ucfirst($sql_exp_result['experties']); ?></a>
                    <?php
                    $res = db_query("SELECT mdm.*,IF (mp.username_type = 1,mp.firstname,CONCAT(mp.firstname,' ', mp.lastname)) as firstname,mp.protype as protype,mp.experties FROM maenna_discussion_moderator mdm join maenna_people mp ON mdm.user_id = mp.pid WHERE discussion_id = %d ORDER BY id LIMIT 1", $row1['id']);
                    if ($res) $moderator = db_fetch_array($res);

                    if ($moderator['status'] == 'invited') $status = '<span  style="color:#91939E !important" class="inv_status">Invited: </span>';
                    elseif ($moderator['status'] == 'active') $status = '<span  style="color:#91939E !important" class="inv_status">Collaborator: </span>';
                    else $status = '';
                    if ($moderator) echo '<a href="#" id="pro_id' . $moderator['user_id'] . '" ref="pro_id" style="margin-left:20px;color:#00a2bf;" class="profile_details">' . $status . trim(ucfirst($moderator['firstname'])) . ", " . ucfirst($moderator['experties']) . "</a> ";
                    if ($moderator['status'] == 'invited' && $moderator['user_id'] == $user->uid)
                        echo "<span class='rsvp_inv'><a style='margin-left:10px;cursor:pointer;' iid='" . $moderator['id'] . "' rel='accept' class='invitation'>Accept<a> / <a iid='" . $moderator['id'] . "' style='cursor:pointer;' rel='reject' class='invitation'>Reject</a></span>";
                    /*                    if (!empty($row1['pexperties'])) {
                                          echo preg_replace('/(?<! )(?<!^)[A-Z]/', ' $0', $row1['pexperties']);
                                        }
                                        */
                    ?>
                    </a>

                </div>
                <div id="clear" style="clear:both"></div>
                <?php

                if (strlen($row1['whyattend']) > 150) $more_link = '...<a onclick="discussion(\'' . $id . '\', \'' . $row1['id'] . '\', \'' . $row1['title'] . '\');" href="/account?tab=professionals&page=pro_detail&id=' . $id . '&section=pro_industry_view&type=details&pro_id=' . $row1['id'] . '" class="tool">More</a>';
                else $more_link = '';

                ?>

                <span
                    style="float:left; margin:0px 0px 0px 0px; text-align:left;"><?= substr($row1['whyattend'], 0, 150) . $more_link; ?></span>

                <div id="clear" style="clear:both"></div>
                <!-- <span style="float:left; display:none;"><? /*= $row1['description']; */ ?></span>-->
                <div id="clear" style="clear:both"></div>

                <?php
                $pro_sql = '
                    SELECT COUNT(DISTINCT(maenna_professional_payments.user_id)) as count
                    FROM maenna_professional_payments
                    LEFT JOIN maenna_people ON maenna_professional_payments.user_id = maenna_people.pid
                    LEFT JOIN maenna_company ON maenna_professional_payments.user_id = maenna_company.companyid
                    WHERE maenna_professional_payments.pro_id = %d';
                $pro_result = db_query($pro_sql, array((int) $row1['id']));
                $count = mysql_fetch_assoc($pro_result);
                $count = (int) $count['count'];
                $attendeeId = null;
                $soldOut = $row1['capacity'] <= $count;
                $insightRepository = new \Clewed\Insights\InsightRepository();
                if ($row1['capacity'] > $count) {
                    ?>
                    <span style="float:left; margin-top:10px; padding-right: 7px; line-height: 12px; font-size: 12px; color: #76787F;">
                        <?php
                        if ($row1['type'] == InsightEntity::TYPE_GROUP_INSIGHT) {
                            $spotsLeft = (int)($row1['capacity'] - $count);
                            if ($count >= 10) {
                                if ($spotsLeft <= 5) {
                                    echo  $spotsLeft . (($spotsLeft > 1) ? ' Spots Left' : ' Spot Left');
                                } else {
                                    echo (int)($count) . ' Joined';
                                }
                            } else {
                                echo  'Insight';
                            }
                        } elseif ($row1['type'] == InsightEntity::TYPE_PRIVATE_INSIGHT) {
                            $attendeeId = $insightRepository->findAttendeeIdPurchasedPrivateInsight($row1['id']);
                            if ($attendeeId) {
                                $attendeeName = getUserById($attendeeId);
                                echo $attendeeName . "'s Service";
                            } else {
                                echo 'Service';
                            }
                        } else {
                            echo 'Service';
                        }
                        ?>
                    </span>
                    <?php
                } else {
                    echo '<span class="sold-out" style="float:left; padding-right: 7px; ">Sold out</span>';
                }?>

                <?php if ($row1['postedby'] == $user->uid || $AccessObj->Com_sections['advice']['sections']['maenna_events']['access'] == 'write' || $usertype == 'super' || $ifAdmin || ($moderator['status'] == 'active' && $moderator['user_id'] == $user->uid)) { ?>
                <span class='discussion_span_edit'>

                <?php if ($row1['postedby'] == $user->uid || ($moderator['status'] == 'active' && $moderator['user_id'] == $user->uid)) : ?>
                    <a style="float:left;margin-top:10px; padding:0 8px; line-height:12px;cursor:text; border-left: 1px solid #76787f;"
                       onclick="return false;" href="#" class="tool"><strong
                            style="color:#76787f; font-weight:normal!important; "><?php echo $row1['approve_status'] ? 'Approved' : 'Pending Approval' ?></strong></a>
                <?php endif;

                if ($row1['approve_status'] == 0 && (($moderator['status'] == 'active' && $moderator['user_id'] == $user->uid) || $row1['postedby'] == $user->uid) || $usertype == 'super' || $ifAdmin) {
                    ?>

                    <a style="border-left: 1px solid #76787f;float:left;margin-top:10px; padding:0 8px; line-height:12px;"
                       href="#" id="edit_id<?php echo $row1['id'] ?>"
                       alt="<?php echo md5($row1['id'] . $user->name . "kyarata75") ?>" name="<?= $user->name; ?>"
                       delType='event' class="tool evedit"><strong style="font-weight:normal!important; ">Edit</strong></a>

                <?php }
                }
                if (($row1['postedby'] == $user->uid || $AccessObj->Com_sections['advice']['sections']['maenna_events']['access'] == 'write' || $usertype == 'super' || $ifAdmin) && !$row1['approve_status']) { ?>
                    <a style="float:left;margin-top:10px; padding:0px 8px 0px 8px;border-left:1px solid #76787f; line-height:12px;"
                       href="#" id="remove_id<?php echo $row1['id'] ?>"
                       alt="<?php echo md5($row1['id'] . $user->name . "kyarata75") ?>" name="<?= $user->name; ?>"
                       delType='event' class="tool evdelete"> Delete</a>
                <?php } ?>

                    <?php if (($usertype != 'super' && !$ifAdmin && ($moderator['status'] == 'active' && $moderator['user_id'] == $user->uid || $row1['postedby'] == $user->uid || ($type == 'registered' && $row1['type'] == 2)))) { ?>
                        <a style="border-left: 1px solid #76787f;float:left;margin-top:10px; padding:0 8px 0 8px;line-height:12px;" href="#"
                           id="editmat_id<?php echo $row1['id'] ?>" alt="<?php echo md5($row1['id'] . "kyarata75") ?>"
                           name="<?= $user->name; ?>" delType='event' class="tool"> Add Files</a>
                    <?php } ?>

                    <?php if ($row1['postedby'] == $user->uid && $AccessObj->role_id == 4) :
                        if (!$moderator) {
                            ?>
                            <span>
                      <a style="color: #00A2BF; float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f;"
                         href="/account?tab=professionals&id=<?= $row1['id'] . "&m=" . md5($row1['id'] . "kyarata75"); ?>"
                         style="text-transform:none;" class="tool follow"> <strong>Invite Moderator</strong></a>
                    </span>
                        <?php } else { ?>
                            <span>
                      <a style="color: #00A2BF; float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f;"
                         href="/account?tab=professionals&id=<?= $row1['id'] . "&m=" . md5($row1['id'] . "kyarata75"); ?>&edit=edit"
                         data-tooltip="This tool invites another moderator to insight, and rejects previous invitation."
                         rel="edit" style="text-transform:none;" class="tool follow"> <strong>Edit Moderator</strong></a>
                    </span>

                        <?php
                        }
                    endif; ?>

                    <?php if ($usertype == 'super' || $ifAdmin): ?>

                        <?php if ($row1['approve_status'] == 1): ?>
                            <span id="approve_dis<?= $row1['id'] ?>">
                      <a style="float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f;"
                         onclick="approve_discussion('disapprove', '<?= $row1['id'] ?>', '<?= $id ?>');"
                         title="Disapprove" delType='event'
                         class="tool follow"> <strong>Unlist</strong></a>
                    </span>
                        <?php
                        else:
                            if ($row1['location'] != '') $oncl = "onclick=\"approve_discussion('approve', '" . $row1['id'] . "', '" . $id . "');\"";
                            else $oncl = "onclick=\"alert('You need to enter Clewed Call in to approve insight.');\"";
                            ?>
                            <span id="approve_dis<?= $row1['id'] ?>">
                      <a style="float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f; color: #00A2BF;"
                          <?= $oncl; ?> title="Approve" delType='event'
                         class="tool follow"> <strong>List</strong></a>
                    </span>
                        <?php endif;
                        if (!$moderator) {
                            ?>
                            <span class="add_moderator_btn" id="add_mod<?= $row1['id'] ?>">
                      <a style="color: #00A2BF; float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f;"
                         title="This tool adds moderator to the insight." style="text-transform:none;"
                         class="tool follow"> <strong>Add colloaborator</strong></a>
                    </span>
                        <?php } else { ?>
                            <span class="add_moderator_btn" id="add_mod<?= $row1['id'] ?>">
                      <a style="color: #00A2BF; float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f;"
                         data-tooltip="This tools changes moderator of the insight." rel="edit"
                         style="text-transform:none;" class="tool follow"> <strong>Edit colloaborator</strong></a>
                    </span>

                        <?php
                        }
                    endif;

                    ?>
                    <div
                        style="color:#76787F;font-size:12px;float:right;margin-top:4px;">
                        <?= $row1['views'] > 1 ? $row1['views'] . ' views |': '' ?>
                        <?php print $row1['tags']; ?></div>
                <div style="clear:both"></div>
              </span>

            </div>

            <span class="attFiles" style="float:left;font-size:10px;">

            </span>
        </div>

    </div>

    <div id="clear" style="clear:both"></div>
    <hr style="margin-top:23px;background:#d0d2d3;">

<?php } ?>

</div>

</div>
</div>
