<style type="text/css">
    .content_box {
        margin-top: 14px;
    }

    .input_error {
        border: 1px solid #ff0000 !important;
    }

    .ui-dialog .ui-dialog-buttonpane {
        border-width: 0 !important;
    }

    .professionals .act-content div.content_box .box_title.shaded_title {
        height: 32px;
        line-height: 32px;
        color: #fff;
        background-color: #94c9da;
        padding-left: 15px;
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

    select.discuss option {
        padding: 4px;
        font-size: 14px;
    }

    .diss {
        margin-left: 2px;
    }
    .diss input.small.button {
        line-height: 22px;
    }

    .manage-insight-link {
        float:left;
        margin-top:10px;
        padding:0px 7px 0px 7px;
        line-height:12px;
        font-size:12px;
        font-weight:bold;
        border-left:1px solid #76787f;
        color: #00A2BF !important;
    }

    .manage-insight-link.done {
        color: #76787f !important;
        font-weight:normal;
    }

</style>

<link rel="stylesheet" type="text/css" href="/themes/maennaco/jui/comments/css/jquery.simple-dtpicker.css"/>
<script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.simple-dtpicker.js"></script>
<?php
global $base_url;
global $user;
global $ifAdmin;

use Clewed\Insights\InsightEntity;

date_default_timezone_set('EST');
include('dbcon.php');
$id = (int) mysql_real_escape_string(sget($_REQUEST, 'id'));
$sql_schedule = "SELECT discussion_status FROM maenna_people WHERE pid = %d";
if ($_REQUEST['id'] != '') {
    $dis_query = db_query($sql_schedule, array($id));
} else {
    $user_id   = $user->uid;
    $dis_query = db_query($sql_schedule, array($user_id));
}

$result_dis = mysql_fetch_object($dis_query);
$usertype = $AccessObj->user_type;


$sql = "SELECT * FROM maenna_professional WHERE datetime >= %d and postedby = %d ORDER BY datetime ASC";
$result = db_query($sql, array(time() - 3600, $_REQUEST['id'])); //todo: 1 hour from settings
$insights_future = array();
while ($row = db_fetch_object($result)) {
    $insights_future[$row->id] = array(date('Y-n-j', $row->datetime), $row->approve_status, $row->title);
}
?>


<div id='docprev'>

<script type="text/javascript">

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
    init_rsvp_invite();
    init_follow();
    $('#cost').blur(function () {
        $(this).formatCurrency({roundToDecimalPlace: -2, eventOnDecimalsEntered: true});
    }).bind('decimalsEntered', function (e, cents) {
            var errorMsg = 'Please do not enter any cents (.' + cents + ')';
            alert(errorMsg);
    });

    var currAtt = '';
    var availableTags = {items: [ //Get the advisors and connected users for the autocomplete feature; $companyid was gotten in the earlier phase in new_company_detail_left.php
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
        $q = "SELECT `projname` FROM `maenna_company` WHERE `companyid` = %d";

        $q = db_query($q,array($_REQUEST['id']));

        $q = mysql_fetch_array($q);
        echo '{value: "' . $_REQUEST['id'] . '", name: "' . $q['projname'] . ' "}';
        ?>
    ]};

    $('[id^=rmFile]').livequery("click", function () {
        fileid = $(this).attr('id').replace("rmFile", "");
        $.post("/themes/maennaco/includes/posts.php?type=removeFile&fileid=" + fileid, {
        }, function (response) {
            $("#file" + fileid).remove();
        });
    });
    $('input[type="file"]').click(function () {
    });
    $("#showEventForm").click(function () {
        var discussion_status = "<?php echo $result_dis->discussion_status; ?>";
        var usertype = '<?php echo $usertype; ?>';
        if (discussion_status == 1 || usertype == 'admin' || usertype == 'super') {
            $("#eventFormDiv").show();
            $(this).parent().parent().hide();
        } else {
            alert("Organizing a discussion requires screening and approval. Please complete your profile to get started.");
            return false;
        }
    });

    $("#showEventFormInsight").click(function () {
        var discussion_status = "<?php echo $result_dis->discussion_status; ?>";
        var usertype = '<?php echo $usertype; ?>';
        if (discussion_status == 1 || usertype == 'admin' || usertype == 'super') {
            $('#scheduleTitle').text('Schedule Insights');
            // add after '#scheduleTitle', delete '#addFileLink'
            var addFile = '<div id="addFileIcon" class="dedit">' +
                '<a style="float:right; margin-left:10px;" href="#"' +
                    'id="file_id<?php echo $row1['id'] ?>"' +
                    'alt="<?php echo md5($row1['id'] . $user->name . "kyarata75") ?>"' +
                    'name="<?= $user->name; ?>" delType="event" class="tool file"' +
                    'title="Add materials: Files / Links">' +
                        '<img src="/themes/maennaco/images/upload_bk.png">' +
                '</a>' +
            '</div>';
            $('#addFileLink').remove();
            $(addFile).insertAfter('#scheduleTitle');
            $('#addMilestonesLink').hide();
            $('#newMilestonesList').empty();

            $('#eventLoc').prop('type', 'text');
            $('#insightType').find('option').remove().end().append('<option value="0" selected="selected">Insight</option>').val('0');
            $('#eventFormDiv').show();
            $('#insightTab').hide();
            $('#serviceTab').show();

            $('[js-type-private="true"]').hide();
            $('#eventDate').prop({'required': true, 'type': 'text', 'value': ''});
            $('#eventServiceDuration').hide();

            $('#eventType').attr('placeholder', 'Insight topic (max 120 characters)');
            $('#eventDesc').prop({'required': true})
            $('#eventDesc').show();
            $('#whyattend').attr('placeholder', 'Insight description');

            $('#pro_cancel_insight').prop('type', 'submit');
            $('#pro_cancel_service').prop('type', 'hidden');
        } else {
            alert("Organizing a discussion requires screening and approval. Please complete your profile to get started.");
            return false;
        }
    });

    $("#showEventFormService").click(function () {
        var discussion_status = "<?php echo $result_dis->discussion_status; ?>";
        var usertype = '<?php echo $usertype; ?>';
        if (discussion_status == 1 || usertype == 'admin' || usertype == 'super') {
            $('#scheduleTitle').text('Create a Services');
            // add after '#newMilestonesList', delete '#addFileIcon'
            var addFile = '<div id="addFileLink" style="text-align:left;">' +
                '<a style="color: #00A3BF;" href="#" ' +
                   'id="file_id<?= $row1['id']; ?>" ' +
                   'alt="<?= md5($row1['id'] . $user->name . "kyarata75"); ?>" ' +
                   'name="<?= $user->name; ?>" delType="event" class="file" ' +
                   'title="Add materials: Files / Links">' +
                    'Add Files' +
                '</a>' +
            '</div>';
            $('#addFileIcon').remove();
            $(addFile).insertAfter('#newMilestonesList');
            $('#addMilestonesLink').show();
            $('#newMilestonesList').show();

            $('#eventLoc').prop('type', 'hidden');
            $('#insightType').find('option').remove().end().append('<option value="1" selected="selected">Service</option>').val('1');
            $('#eventFormDiv').show();
            $('#serviceTab').hide();
            $('#insightTab').show();

            $('[js-type-private="true"]').show();
            $('#eventDate').prop({'required': false, 'type': 'hidden', 'value': ''});
            $('#eventServiceDuration').show();

            $('#eventType').attr('placeholder', 'I will help you with... (max 120 characters)');
            $('#eventDesc').removeAttr('required');
            $('#eventDesc').hide();
            $('#whyattend').attr('placeholder', 'What will buyers get from this service');

            $('#pro_cancel_service').prop('type', 'submit');
            $('#pro_cancel_insight').prop('type', 'hidden');
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
        } else {
            eventid = 'eventid=' + $(this).attr('id').replace('edit_id', '');
        }
        uid = <?= $user->uid; ?>;
        $.post("/themes/maennaco/includes/pro_posts.php?type=profileInfo&display=true&" + eventid + "&uid=" + uid + "&base_url=<?php echo $base_url; ?>", {
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
                }, closeOnEscape: true,
                modal: true
            }).html(response);
        });
    });
    $("a[id^='editmat_id']").livequery("click", function (e) {
        e.preventDefault();
        eventid = $(this).attr('id').replace('editmat_id', '');
        $.post("/themes/maennaco/includes/pro_posts_files.php?type=attachFile&reason=edit",
            function (response) {
                $("#editmatdlg").dialog({
                    autoOpen: true,
                    width: 500,
                    title: 'File attachment',
                    resizable: false,
                    draggable: false,
                    height: 310,
                    buttons: {"Save": function () {
                        $.post("/themes/maennaco/includes/pro_posts_files.php?type=editInsight", {eventid: eventid, str: $(editEventForm).serialize()
                        }, function (response) {
                            alert(response);
                        });
                        $(this).dialog("close");
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
                    action: '/themes/maennaco/includes/file_upload.php',
                    onComplete: function (id, fileName, responseJSON) {
                        if (responseJSON['success']) {
                            $("#editEventForm").append('<input name="fileupl" type="hidden" value="' + responseJSON['timestamp'] + "_" + fileName + '"  class="fileInfo" path="' + responseJSON['timestamp'] + "_" + fileName + '" filetitle="' + $("#fileTitleEdit").val() + '" timestamp = "' + responseJSON['timestamp'] + '">');
                        }
                    }
                });
            });
    });

    $(".evedit").livequery("click", function (e) {
        e.preventDefault();
        var eventid = $(this).attr('id').replace('edit_id', '');
        $.post("/themes/maennaco/includes/pro_posts.php?type=eventEdit&display=true&eventid=" + eventid,
            function (response) {
                $("#eveditdlg").dialog({
                    autoOpen: true,
                    width: 500,
                    title: 'Event edit',
                    resizable: false,
                    draggable: false,
                    open: function () {
                        $(this).closest(".ui-dialog").find(".ui-button:first").next().find(".ui-button-text").addClass("uicancel");
                    },
                    height: 600,
                    buttons: {
                        "Save": function () {
                            links = '';
                            name = '';
                            title = $("#editEventForm").find("#eventType").val();
                            var name = new Array();
                            var links = new Array();
                            $("#text input[name=name]").each(function () {
                                name.push($(this).val());
                            });
                            var taskArray = new Array();
                            $("#text input[name=link]").each(function () {
                                links.push($(this).val());
                            });
                            names = name;
                            datetime = $("#editEventForm").find("#date").val();
                            loc = $("#editEventForm").find("#eventLoc").val();
                            desc = $("#editEventForm").find("#eventDesc").val();
                            whyattend = $("#editEventForm").find("#whyattend").val();
                            eventcost = $("#editEventForm").find("#eventCost").val();
                            eventcapacity = $("#editEventForm").find("#eventCapacity").val();
                            eventtags = $("#editEventForm").find("#eventTags").val();
                            eventindustry = $("#editEventForm").find("#eventIndustry").val();
                            buyer_requirement = $("#editEventForm").find("#buyer_requirement").val();
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
                            } else {
                                notif = false;
                            }
                            invitees = $("#editEventForm").find(".as-selections").children(".as-original").children(".as-values").val();
                            $(".fileInfo").each(function () {
                                var tmpArr1 = {'path': $(this).attr('path'), 'title': $(this).attr('filetitle'), 'timestamp': $(this).attr('timestamp')};
                                filesEdit.push(tmpArr1);
                            });
                            //
                            var milestones = [];
                            // getting milestones data
                            $('.milestone-data').each(function() {
                                var id = $(this).attr('id'),
                                    duration = parseInt($(this).find('select').val()),
                                    desc = $(this).find('input').val();
                                if (isNaN(duration)) {
                                    return;
                                }
                                var currentObject = {
                                    description: desc,
                                    duration: duration
                                }
                                if (id) {
                                    currentObject.id = parseInt(id.replace('ms_id', ''))
                                }
                                milestones.push(currentObject);
                            });
                            // getting service duration
                            var serviceDuration = parseInt($('#eventEditServiceDuration').val());
                            if (isNaN(serviceDuration)) {
                                serviceDuration = '';
                            }
                            $.post(
                                '/themes/maennaco/includes/pro_posts.php?type=eventEdit',
                                {
                                    eventid: eventid,
                                    title: title,
                                    loc: loc,
                                    whyattend: whyattend,
                                    desc: desc,
                                    date: datetime,
                                    buyer_requirement: buyer_requirement,
                                    cost: eventcost,
                                    capacity: eventcapacity,
                                    tags: eventtags,
                                    industry: eventindustry,
                                    u: uname,
                                    cid: cid,
                                    uid: uid,
                                    files: filesEdit,
                                    notif: notif,
                                    links: links,
                                    names: names,
                                    approve_discount: $('input[name="approve_discount"]').prop('checked') ? 1 : 0,
                                    discount_enabled: $('input[name="use_discount"]').prop('checked') ? 1 : 0,
                                    discount_rate: $('input[name="rate"]').val(),
                                    milestones: milestones,
                                    duration: serviceDuration
                                },
                                function (response) {
                                    if (response.replace(/\s/g, "") == 'true') {
                                        alert('Your update was sucessfully saved');
                                        location.reload();
                                    } else {
                                        alert('Something went wrong. Some of your data may not be saved.');
                                    }
                                }
                            );
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
                    action: '/themes/maennaco/includes/file_upload.php',
                    onComplete: function (id, fileName, responseJSON) {
                        if (responseJSON['success']) {
                            $("#eventType").before('<input type="hidden" class="fileInfo" path="' + fileName + '" filetitle="' + $("#fileTitleEdit").val() + '" timestamp = "' + responseJSON['timestamp'] + '">');
                            $("#fileTitleEdit").val('');
                        }
                    }
                });
            }
        );
    });
    /********* Attach Files  *********/
    $(".file").livequery("click", function (e) {
        e.preventDefault();
        fileid = $(this).attr('id').replace('file_id', '');
        $.post("/themes/maennaco/includes/pro_posts_files.php?type=attachFile",
            function (response) {
                $("#eveditdlg").dialog({
                    autoOpen: true,
                    width: 500,
                    title: 'File Attachment',
                    height: 320,
                    buttons: {
                        "Save": function () {
                            title = $("#editEventForm #event").children("#eventType").val();
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
                            } else {
                                notif = false;
                            }
                            invitees = $("#editEventForm").children(".as-selections").children(".as-original").children(".as-values").val();
                            $(".fileInfo").each(function () {
                                var tmpArr1 = {'path': $(this).attr('path'), 'title': $(this).attr('filetitle'), 'timestamp': $(this).attr('timestamp')};
                                filesEdit.push(tmpArr1);
                            });

                            $.post("/themes/maennaco/includes/pro_posts_files.php?type=attachFile", {title: title, datetime: datetime, loc: loc, agenda: agenda, invitees: invitees, u: uname, cid: cid, uid: uid, files: filesEdit, notif: notif, names: name, links: links},
                                function (response) {
                                    $('#eveditdlg').dialog("close");
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
                    params: {
                        pro_id: $("#file-uploader1").data('pro-id')
                    },
                    // path to server-side upload script
                    action: '/themes/maennaco/includes/file_upload.php',
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
        $.post("/themes/maennaco/includes/posts.php?type=confirmAtt&status=" + status + "&uid=" + uid + "&eventid=" + eventid,
            function (response) {
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
            $.post("/themes/maennaco/includes/pro_posts_files.php?type=eventcom&u=" + u + "&m=" + m + "&value=" + a + "&eventid=" + eventid + "&uid=" + uid,
                function (response) {
                    tarea.parent().parent().parent().parent().after($(response).show());
                    tarea.val("Share ideas on this topic");
                    $('textarea').elastic();
                    $(".commentMark").Watermark("Got advice / question on this topic?");
                    tarea.Watermark("watermark", "#369");
                    $(".commentMark").Watermark("watermark", "#EEEEEE");
                });
        }
    });
    <?php
    if (in_array($AccessObj->user_type,array('super','admin'))) {
        if ($_REQUEST['id']) {
            $uid = $_REQUEST['id'];
        } else {
            $uid = $user->uid;
        }
    } else {
        $uid = $user->uid;
    } ?>
    $('#addEvent').click(function () {
        var valid = true;
        var uid = <?= $uid; ?>;
        var m = '<?= md5($uid . "kyarata75"); ?>';
        var usr_type = '<?= $AccessObj->user_type; ?>';
        <?php
        if ($_REQUEST['id'] == '') {
          $user_id = $user->uid;
        } else {
          $user_id = $_REQUEST['id'];
        }
        ?>
        $("form[id='addEventForm'] :input").each(function () {
            if ($(this).attr('required') == "required") {
                if ($(this).val() == '') {
                    valid = false;
                    $(this).addClass('input_error');
                } else {
                    $(this).removeClass('input_error');
                }
            }
        });
        if (!valid) {
            alert("Please insert required fields");
            return false;
        }
        var cid = <?= $user_id; ?>;
        var event1 = $('#eventType').val();
        var type = $('#insightType').val();
        if (type == '') {
            alert('Please select a type of Insight');
            $("#insightType").focus();
            return false;
        }
        var loc = $('#eventLoc').val();
        var desc = $('#eventDesc').val();
        var datetime = $('#eventDate').val();
        var whyattend = $('#whyattend').val();
        var buyer_requirement = $('#buyer_requirement').val();
        var cost = $('#cost').val();
        var capacity = $('#capacity').val();
        var tags = $('#tags').val();
        var invitees = $('.as-values').val();
        var uname = '<?= $user->name; ?>';
        var urole = '<?= end($user->roles); ?>';
        if (tags == '') {
            alert('Please select a Category');
            $("#tags").focus();
            return false;
        }
        var files = [];
        $(".fileInfo").each(function () {
            var tmpArr = {'path': $(this).attr('path'), 'title': $(this).attr('filetitle'), 'timestamp': $(this).attr('timestamp')};
            files.push(tmpArr);
        });

        var milestones = [],
            serviceDuration = '';
        if (type == '1') {
            // getting milestones data
            $('.milestone-data').each(function() {
                var duration = parseInt($(this).find('select').val()),
                    desc = $(this).find('input').val();
                if (isNaN(duration)) {
                    return;
                }
                milestones.push({
                    description: desc,
                    duration: duration
                });
            });
            // getting service duration
            serviceDuration = parseInt($('#eventServiceDuration').val());
            if (isNaN(serviceDuration)) {
                serviceDuration = '';
            }
        }
        $.post(
            '/themes/maennaco/includes/pro_posts.php?type=addEvent',
            {
                uid: uid,
                insightType: type,
                title: event1,
                loc: loc,
                desc: desc,
                date: datetime,
                whyattend: whyattend,
                buyer_requirement: buyer_requirement,
                cost: cost,
                capacity: capacity,
                tags: tags,
                u: uname,
                files: files,
                cid: cid,
                urole: urole,
                use_discount: $('#use-discount').is(':checked'),
                rate: $('#discount-rate').val(),
                code: $('#discount-code').val(),
                milestones: milestones,
                duration: serviceDuration
            },
            function (response) {
                alert(('0' == type ? 'Insight' : 'Service') + ' has been created successfully');
                $('#eventFormDiv').hide();
                $('.fileInfo').remove();
                $('.watermark').Watermark('Share ideas on this topic');
                $('#eventType').val('');
                $('#eventLoc').val('');
                $('#eventDesc').val('');
                $('#eventDate').val('');
                $('#whyattend').val('');
                $('#buyer_requirement').val('');
                $('#cost').val('');
                $('#capacity').val('');
                $('#tags').val('');
                var url = 'account?tab=professionals&page=pro_detail&id=' + cid + '&section=pro_industry_view&type=discussion';
                if (type == 1) {
                    url += '&private=1'
                }
                window.location.href = url;
            }
        );
    });
    $("#pro_cancel").click(function () {
        $("#showEventForm").parent().parent().show();
        $("#eventFormDiv").hide();
        $("#eventType").val('');
        $("#eventLoc").val('');
        $("#eventDesc").val('');
        $("#eventDate").val('');
        $("#whyattend").val('');
        $("#cost").val('');
        $("#capacity").val('');
        $("#tags").val('');
        return false;
    });
    $("#pro_cancel_insight").click(function () {
        $("#insightTab").show();
        $("#eventFormDiv").hide();
        $("#eventType").val('');
        $("#eventLoc").val('');
        $("#eventDesc").val('');
        $("#eventDate").val('');
        $("#whyattend").val('');
        $("#cost").val('');
        $("#capacity").val('');
        $("#tags").val('');
        return false;
    });
    $("#pro_cancel_service").click(function () {
        $("#serviceTab").show();
        $("#eventFormDiv").hide();
        $("#eventType").val('');
        $("#eventLoc").val('');
        $("#eventDesc").val('');
        $("#eventDate").val('');
        $("#whyattend").val('');
        $("#cost").val('');
        $("#capacity").val('');
        $("#tags").val('');
        return false;
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
        var uid = '<?= $user->uid; ?>';
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
        $(this).children("a.delete").show();
    });
    $('.event').livequery("mouseleave", function (e) {
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
            url: '/themes/maennaco/includes/delete.php?type=professional&id=' + temp + "&u=" + u + "&m=" + m,
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

    $('textarea').elastic();
    jQuery(function ($) {
        $(".watermark").Watermark("Share ideas on this topic");
        $("#eventLoc").Watermark("Clewed call in").addClass('watermark');
        $("#eventInv").Watermark("+ Add people to notify");
        $(".commentMark").Watermark("Got advice / question on this topic?");

    });
    function UseData() {
        $.Watermark.HideAll();
        //Do Stuff
        $.Watermark.ShowAll();
    }

    $('#get-help-btn2').click(function () {
        $("#get-help-dialog2").dialog({
            modal: true,
            height: "450",
            autoOpen: true,
            title: 'Share Your Knowledge',
            resizable: false,
            buttons: {
                Close: function () {
                    $(this).dialog("close");
                }
            },
            open: function (event, ui) {
                $(this).scrollTop(0);
            }

        });
    });

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

});

function showprodetails(id) {
    $("#showpro" + id).show();
}

function discussion(id, proid) {
    window.location.href = "./account?tab=professionals&page=pro_detail&id=" + id + "&section=pro_industry_view&type=details&pro_id=" + proid;
}

function like_discussion(type, prof_id, userid) {
    if (type == 'like') {
        var status = 1;
    } else {
        var status = 0;
    }

    <?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

    $.ajax({
        type: 'get',
        url: '/themes/maennaco/includes/delete.php?' +
            'type=like_discussion&' +
            'prof_id=' + prof_id + '&' +
            'userid=' + userid + '&' +
            'status=' + status + "&" +
            "u=<?php echo $u; ?>&" +
            "m=<?php echo $m; ?>",
        success: function (msg) {
            if (type == 'like') {
                $('#likepost' + prof_id).html("<a style='cursor:pointer;color:#00A3BF;font-weight: bold;' onclick='like_discussion(\"unlike\", " + prof_id + "," + userid + ");'>Unlike</a>&nbsp;" + msg + "");
            } else {
                $('#likepost' + prof_id).html("<a style='cursor:pointer;color:#00A3BF;font-weight: bold;' onclick='like_discussion(\"like\", " + prof_id + "," + userid + ");'>Like</a>&nbsp;" + msg + "");
            }
        }
    });
}

<?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75');?>

function follow_discussion(type, prof_id, user_id) {
    if (type == 'follow') {
        var status = 1;
    } else {
        var status = 0;
    }
    $.ajax({
        type: 'get',
        url: '/themes/maennaco/includes/delete.php?' +
        'type=followdis&' +
        'prof_id=' + prof_id + "&" +
        "user_id=" + user_id + "&" +
        "status=" + status + "&" +
        "u=<?php echo $u;?>&" +
        "m=<?php echo $m;?>",
        success: function (msg) {
            if (type == 'follow') {
                $('#follow_dis' + prof_id).html("<a style='float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f;' title='Unfollow' onclick='follow_discussion(\"unfollow\", " + prof_id + "," + user_id + ");' delType = 'event' class='tool follow'><strong>Following</strong></a>");
            } else {
                $('#follow_dis' + prof_id).html("<a style='float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f;' title='Follow' onclick='follow_discussion(\"follow\", " + prof_id + "," + user_id + ");' delType = 'event' class='tool follow'><strong>Follow</strong></a>");
            }
        }
    });
}

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
                $('#approve_dis' + prof_id).html("<a style='float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f;' title='Disapprove' onclick='approve_discussion(\"disapprove\", " + prof_id + "," + user_id + ");' delType = 'event' class='tool follow'><strong>Disapprove</strong></a>");
                $('.featured-toggle-wrapper[data-id=' + prof_id + ']').removeClass('hidden');
            } else {
                $('#approve_dis' + prof_id).html("<a style='float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f;' title='Approve' onclick='approve_discussion(\"approve\", " + prof_id + "," + user_id + ");' delType = 'event' class='tool follow'><strong>Approve</strong></a>");
                $('.featured-toggle-wrapper[data-id=' + prof_id + ']')
                    .addClass('hidden')
                    .find('a')
                    .html('<strong>Feature</strong>')
                    .data('value', 0)
                    .removeClass('done');
            }
        }
    });
}

function isNumberKey(evt) {
    var charCode = (evt.which) ? evt.which : event.keyCode;
    return !(charCode != 46 && charCode > 31
        && (charCode < 48 || charCode > 57));
}


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
                foreach ($insights_future as $pid => $val_arr) {
                     echo "{date: '$val_arr[0]',approved: '$val_arr[1]',title: '".addslashes($val_arr[2])."'},";
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
                $.post("/themes/maennaco/includes/fetch_insights_calendar.php", {date: date, creator: creator
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
                });
            }
            $(".datepicker").live('focus', function () {
                if ($("#ins_status").val() == '1') {
                    if (!confirm("It is not safe to change a live insight. Please cancel this request unless you are sure there are no attendees and it is safe to change the time.")) return;
                }
//                        initDatePicker(); //TODO: will ve usefull later
                $('#date-picker').val($(this).val());
                title = $(this).parent().find("#eventType").val();
                if (title == '' || title == 'undefined') title = 'Create Insight';
                $("#picker").dialog('option', 'title', title);
                $date = $(this).val().split(' ')[0];
                $creator = $(this).attr('creator');
                currentDate = '<?=date('m-d-Y H:i');?>';
                if ($('#date-picker').val().length) {
                    $id = getParameterByName('id');
                    if ($.trim($id) == '') {
                        $.post("/themes/maennaco/includes/fetch_insights_calendar.php?type=getProInsights", {id: $creator}, function (response) {
                            $('#date-picker').handleDtpicker('setDateStr', $('#date-picker').val(), response);
                        }, "json");
                    } else {
                        $('#date-picker').handleDtpicker('setDateStr', $('#date-picker').val());
                    }
                }
                $(".timeTable").html('<img style="margin-top:28px;" src="themes/maennaco/images/ajax-loader.gif">');
                if ($date == '' || $date == 'undefined') $date = currentDate;
                $.post("/themes/maennaco/includes/fetch_insights_calendar.php", {date: $date, creator: $creator
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
                        $(this).dialog("close");
                    }
                }
            });
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

            $('a.featured-toggle').livequery('click', function() {
                var $this = $(this),
                    value = $this.data('value'),
                    insightId = $this.data('id'),
                    currentText = 1 == value ? 'Unfeature' : 'Feature',
                    toggledText = 1 == value ? 'Feature' : 'Unfeature';

                <?php $u = time(); $m = md5('delete.php:' . $u . ':kyarata75')?>
                if(confirm('Are you sure you want to ' + currentText.toLowerCase() + ' this insight?')) {
                    $.ajax({
                        type: 'GET',
                        url: '/themes/maennaco/includes/delete.php?' +
                            'type=feature-insight&' +
                            'id=' + insightId + '&' +
                            'u=<?php echo $u;?>&' +
                            'm=<?php echo $m;?>',
                        success: function (msg) {
                            if('error' !== msg) {
                                $this.html('<strong>' + toggledText + '</strong>');
                                $this.data('value', Math.abs(value - 1));
                                if(1 == value)
                                    $this.removeClass('done');
                                else
                                    $this.addClass('done');
                                alert(msg + ' insight' + (1 == msg ? ' ' : 's ') + (1 == msg ? 'is' : 'are') + ' now marked as featured');
                            }
                        }
                    });
                }
            });


            $('a.delivered-toggle').livequery('click', function() {
                var $this = $(this),
                    value = $this.data('value'),
                    m = $this.data('m'),
                    insightId = $this.data('id');

                if(confirm('Are you sure you want to deliver this insight?')) {

                    $.post('/themes/maennaco/includes/delete.php?type=deliver-insight',
                        {
                            id: insightId,
                            m: m
                        },
                        function (response) {
                            if (response == 'success') {
                                $this.parent().addClass('manage-insight-link done');
                                $this.parent().html('Delivered');
                                alert('Insight is now marked as delivered');
                            }
                        }
                    );
                }
            });

            $('#addMilestonesLink a').click(function () {
                var durations = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 23, 25, 27, 30];
                var singleMilestoneInputs = '<div class="milestone-data"><select style="margin: -5px 0 0 -9px; padding: 6px; width: 150px !important; height: 25px !important; box-sizing: content-box; font-size: 14px !important;">' +
                    '<option value="" selected="selected">Milestone Deliverable</option>';
                for (var i in durations) {
                    singleMilestoneInputs += '<option value="' + durations[i] + '">In ' + durations[i] + ' days</option>';
                }
                singleMilestoneInputs += '</select>' +
                    '<input type="text" placeholder="Deliverable description" style="margin: 5px 0 0 10px; width: 345px !important; height: 37px !important;"></div>';
                $('#newMilestonesList').append(singleMilestoneInputs);
            });
        });
        $('#use-discount').click(function () {
            if ($(this).prop('checked')) {
                $('#discount-properties').css('display', 'inline-block');
                $('#discount-rate').focus();
                $('#discount-tip').css('display', 'inline-block');
                setTimeout(function () {
                    $('#discount-tip').hide(500);
                }, 30000);
            } else {
                $('#discount-properties').css('display', 'none');
                $('#discount-tip').hide();
            }
        });
    });
</script>

<?php
$service_duration = array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 23, 25, 27, 30);

$open_insights = (!empty($_POST['openinsights']) || !empty($_GET['openinsights'])) ? true : false;
$open_services = (!empty($_POST['openservices']) || !empty($_GET['openservices'])) ? true : false;

$display_form_div = ($open_insights || $open_services) ? '' : 'display:none;';
$schedule_title = $open_services ? 'Create a Services' : 'Schedule Insights';
if ($open_insights) {
    $insight_type_option = '<option value="0" selected="selected">Insight</option>';
} else if ($open_services) {
    $insight_type_option = '<option value="1" selected="selected">Services</option>';
} else {
    $insight_type_option = '<option value="" selected="selected">Choose your Insight type</option>';
}
$event_type_placeholder = $open_insights ? 'Insight topic (max 120 characters)' : ($open_services ? 'I will help you with... (max 120 characters)' : 'Insight topic');
$event_date_props = $open_insights ? 'required="true" type="text"' : 'type="hidden"';
$why_attend_placeholder = $open_insights ? 'Insight description' : ($open_services ? 'What will buyers get from this service' : 'Insight/Service description');
?>
<div>
<div id="eveditdlg"></div>
<div style="display:none;" id="editmatdlg"></div>
<div id="profiledlg"></div>
<div style="clear:both;"></div>
<div id="eventFormDiv" style="margin-top:10px;<?= $display_form_div ?>">
    <form action="" method="post" name="addEvent" id="addEventForm">
        <div class="dischead">
            <div id="scheduleTitle" class="dtitle"><?= $schedule_title; ?></div>
            <?php if ($open_insights) : ?>
            <div id="addFileIcon" class="dedit">
                <a style="float:right; margin-left:10px;" href="#"
                   id="file_id<?php echo $row1['id'] ?>"
                   alt="<?php echo md5($row1['id'] . $user->name . "kyarata75") ?>"
                   name="<?= $user->name; ?>" delType="event" class="tool file"
                   title="Add materials: Files / Links">
                    <img src="/themes/maennaco/images/upload_bk.png">
                </a>
            </div>
            <?php endif; ?>
            <div style="clear:both"></div>
        </div>

        <select required="required" class="discuss" id="insightType" name="insightType"
                style="margin: 20px 10px 0 0; padding: 6px; height: 29px; font-size: 14px; width: 516px !important; box-sizing: content-box; -webkit-appearance: none; display: none;">
            <?= $insight_type_option; ?>
        </select>

        <input required="required" class="discuss" id="eventType" placeholder="<?= $event_type_placeholder; ?>" maxlength="120"
               data-maxsize="120" name="eventType" type="text" style="margin-bottom:0px; width:516px !important;"/>
        <input class="input" id="eventLoc" placeholder="Clewed call in" name="eventLoc" cols="20" type="<?= $open_services ? 'hidden' : 'text'; ?>">
        <input <?= $event_date_props; ?> id="eventDate" placeholder="When?" name="eventDate"
                                           creator="<?= intval($_REQUEST['id']); ?>" class='datepicker ins_picker'/>
        <select class="discuss" id="eventServiceDuration" name="eventServiceDuration" style="margin: 5px 10px 5px 0;padding: 6px;height: 29px;box-sizing: content-box;width: 516px !important;font-size: 14px !important;float: right;<?= $open_insights ? 'display:none;' : 'display:inline-block;'; ?>">
            <option value="" selected="selected">Choose Duration</option>
            <?php foreach ($service_duration as $duration) : ?>
                <option value="<?= $duration; ?>">In <?= $duration; ?> days</option>
            <?php endforeach; ?>
        </select>
        <br>

        <div id="picker" title="">
            <table cellspacing="0" cellpadding="0" border="0px">
                <tr>
                    <td style="vertical-align: top;"><br/><input type="text" disabled="disabled" id="date-picker"/></td>
                    <td class="timeTable" style="width:100%;vertical-align: top;"></td>
                </tr>
            </table>
        </div>

        <textarea <?= $open_insights ? 'required="true"' : ''; ?> class="input" id="eventDesc" placeholder="Target audience" name="eventDesc" cols="60"
                  style="height:95px;<?= $open_services ? 'display:none;' : ''; ?>"></textarea>
        <br/>
        <textarea style="min-height:400px;" required="true" class="input" id="whyattend" placeholder="<?= $why_attend_placeholder; ?>"
                  name="whyattend" cols="60"></textarea><br/>
        <div id="addMilestonesLink" style="text-align: left;<?= !$open_services ? 'display:none;' : ''; ?>">
            <a style="color: #00A3BF; cursor: pointer;">Add Milestones</a>
        </div>
        <div id="newMilestonesList"></div>
        <?php if ($open_services) : ?>
        <div id="addFileLink" style="text-align: left;">
            <a style="color: #00A3BF;" href="#"
               id="file_id<?php echo $row1['id'] ?>"
               alt="<?php echo md5($row1['id'] . $user->name . "kyarata75") ?>"
               name="<?= $user->name; ?>" delType="event" class="file"
               title="Add materials: Files / Links">
                Add Files
            </a>
        </div>
        <?php endif; ?>
        <div style="text-align:left;<?= !$open_services ? 'display:none;' : ''; ?>" js-type-private="true">
            <a href="#" onclick="$('#buyer_requirement').toggle(); return false;" style="color:#00A3BF;">Buyer Requirement</a>
            <textarea style="display:none; min-height:200px; margin-left:0; font-size: 14px" class="input" id="buyer_requirement" placeholder="Describe buyer requirement"
                      name="buyer_requirement" cols="60"></textarea>
        </div>

        <input required="true" class="input" type='text' placeholder="Cost" id="cost" name="cost" style="margin-left:0px;"/>

        <input class="input" type='text' placeholder="Capacity" id="capacity" onkeypress="return isNumberKey(event)"
               name="capacity"/>

        <select required='true' class="discuss" id="tags" name="tags" style="margin-top:1px;height:33px;font-size:14px;">
            <option value="">Choose a Category</option>
            <?= OPTION_TAGS(_categories()) ?>
        </select>
        <br/>

        <div style="text-align: left; clear: both">
            <label style="width: 167px; display: inline-block; margin-top: 7px;">
                <input type="checkbox"
                       value="1"
                       name="use-discount"
                       id="use-discount"/>
                Offer discount
            </label>

            <div id="discount-properties" style="display: none">
                <input type="text"
                       size="5"
                       style="width: 70px !important; padding: 0 3px; text-align: center; display: inline-block; margin-right: 65px"
                       name="discount-code"
                       id="discount-code"
                       value="<?= Clewed\Insights\DiscountModel::generateCode() ?>"
                       readonly
                       title="Copy this discount code"
                       onfocus="this.select()"
                />
                <label>
                    Discount rate is
                    <input type="text"
                           name="discount-rate"
                           id="discount-rate"
                           data-tooltip="Amount will be rounded to the nearest dollar."
                           value="50"
                           style="width: 25px !important; text-align: center"
                           onkeypress="return isNumberKey(event)"/>%
                </label>
            </div>
            <div id="discount-tip" style="display: none; width: auto; padding: 4px 8px; font-family: 'Lato Regular', sans-serif; font-size: 11px; margin-top: 10px; background-color: #003241; color: white; line-height: 14px;">
                Allowed one discount code per Insight.
            </div>
        </div>

        <div id='evFile' style='text-align:left; display:none'>
            <input style="background:none;float:left;border:solid 2px #DCE6F5;" type=text id="fileTitle"/>

            <div id="file-uploader">
                <noscript>
                    <p>Please enable JavaScript to use file uploader.</p>
                    <!-- or put a simple form for upload here -->
                </noscript>

            </div>
            <div style="clear:both"></div>
        </div>
        <?= js_init("init_openbox();init_hidebox();"); ?>

        <div class="diss">
            <input type="button" id="addEvent" class="small button" value="Submit"/>
            <input type="<?= $open_insights ? 'submit' : 'hidden' ?>" class="small button" value="Cancel" id="pro_cancel_insight">
            <input type="<?= $open_services ? 'submit' : 'hidden' ?>" class="small button" value="Cancel" id="pro_cancel_service"/>
            <a <?php echo $result_dis->discussion_status ? 'href="/Clewed_Insights_Content_Guidelines_final_102314.pdf"' : 'href="#" onclick="alert(\'This file is private for approved professionals.\');return false;"' ?>
                    target="_blank" style="color: #00A3BF; margin-left: 10px;">Download content guidelines</a>
        </div>

    </form>
</div>
<div id="posting" align="left">
<input id="attSpan" type="hidden" value="">

<?php
$page = mysql_real_escape_string(sget($_REQUEST, 'page'));
$type = mysql_real_escape_string(sget($_REQUEST, 'type'));
$user_idd = $user->uid;
$tab = sget($_REQUEST, 'tab');

$ifInsightAdmin = ($usertype == 'super' || $usertype == 'admin');
$ifOwnerOrAdmin = ($id == $user->uid || $ifInsightAdmin);

$fields = '
    maenna_professional.*,
    maenna_people.pid as uid,
    
        	(	SELECT COUNT(DISTINCT(maenna_professional_payments.user_id)) as count
                    FROM maenna_professional_payments
                    LEFT JOIN maenna_people ON maenna_professional_payments.user_id = maenna_people.pid
                    LEFT JOIN maenna_company ON maenna_professional_payments.user_id = maenna_company.companyid
                    WHERE maenna_professional_payments.pro_id = maenna_professional.id) as count
	,
    
    IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname," ", maenna_people.lastname)) as firstname,
    maenna_people.experties as pexperties,
    maenna_people.protype as protype,
    (SELECT COUNT(*) FROM maenna_professional_payments AS mpp WHERE mpp.pro_id = maenna_professional.id and mpp.user_id = ' . $user_idd . ') as attending,
    (SELECT COUNT(*) FROM maenna_professional_payments AS mpp WHERE mpp.pro_id = maenna_professional.id) as attendees,
    (SELECT COUNT(id) FROM like_discussion_professional WHERE prof_id = maenna_professional.id) as likes,
    (SELECT COUNT(id) FROM like_discussion_professional WHERE prof_id = maenna_professional.id AND user_id = ' . $user_idd . ') as likepost';
if ($tab == 'insights' || $tab == 'services' || $_REQUEST['id'] == '') {
    $fields .= ', (SELECT COUNT(id) FROM maenna_professional_payments WHERE user_id = maenna_professional.postedby AND pro_id = maenna_professional.id) as remaining_spots';
} else {
    $fields .= ', (SELECT COUNT(id) FROM maenna_professional_payments WHERE user_id = ' . (int) mysql_real_escape_string(sget($_REQUEST, 'id')) . ' AND pro_id = maenna_professional.id) as remaining_spots';
}
$joins = 'LEFT JOIN maenna_people ON maenna_people.pid = maenna_professional.postedby';
$sql_where = array();
$sql_result = "SELECT $fields FROM `maenna_professional` $joins ";
if ($ifInsightAdmin && ($tab == 'insights' || $tab == 'services' || $_REQUEST['id'] == '')) {

    if ($_REQUEST['ftype'] == 'live') $sql_where[] = ' maenna_professional.approve_status = 1';
    elseif ($_REQUEST['ftype'] == 'progressing') $sql_where[] = ' maenna_professional.approve_status = 0';

} elseif ($tab == 'insights'|| $tab == 'services' || $_REQUEST['id'] == '') {
    $sql_where[] = ' maenna_professional.approve_status = 1';
} elseif ($ifOwnerOrAdmin) {
    if (isset($_REQUEST['add']) && substr( $_REQUEST['add'], 0, 13 ) == "guest_expert_")
        $sql_where[] = ' (maenna_professional.postedby = ' . $id.' OR maenna_professional.postedby = ' .substr($_REQUEST['add'],13).')';
    else $sql_where[] = ' maenna_professional.postedby = ' . $id;
} else {
    $sql_where[] = 'maenna_professional.approve_status = 1';
    if (isset($_REQUEST['add']) && substr( $_REQUEST['add'], 0, 13 ) == "guest_expert_")
        $sql_where[] = ' (maenna_professional.postedby = ' . $id.' OR maenna_professional.postedby = ' .substr($_REQUEST['add'],13).')';
    else $sql_where[] = ' maenna_professional.postedby = ' . $id;
}

//for navigation under "My" menu
$ifRequestingOwnInsights = ($id == $user->uid);
if ($ifRequestingOwnInsights) {
    if (isset($_REQUEST['private']) && $_REQUEST['private']) {
        $sql_where[] = " maenna_professional.type IN (1,2) ";
    } elseif (isset($_REQUEST['active']) && $_REQUEST['active']) {
        $sql_where[] = ' maenna_professional.approve_status = 1';
    } elseif (isset($_REQUEST['public']) && $_REQUEST['public']) {
        $sql_where[] = " maenna_professional.type = 0 ";
    } // else: insights of both public and private types will be listed
} else {
    if (!$ifInsightAdmin) {
        $sql_where[] = " maenna_professional.type IN (0,1) ";
    }
}


if ($_REQUEST['sort'] != '') {
     $sql_where[] = " maenna_professional.tags LIKE '%" . mysql_real_escape_string($_REQUEST['sort']) . "%'";
}
if ($_REQUEST['sortmonth'] != '' && $_REQUEST['id'] == '') {
    $sql_where[] = " MONTH(FROM_UNIXTIME(maenna_professional.datetime)) = " . mysql_real_escape_string($_REQUEST['sortmonth']);
}
if ($_REQUEST['sortmonth'] != '') {
    $sql_where[] = " MONTH(FROM_UNIXTIME(maenna_professional.datetime)) = " . mysql_real_escape_string($_REQUEST['sortmonth']);
}
if ($_REQUEST['sortdate'] != '') {
    $sql_where[] = " maenna_professional.datetime = " . mysql_real_escape_string($_REQUEST['sortdate']);
}

if (count($sql_where) > 0) $total_sql_where = ' WHERE ' . implode(" and ", $sql_where);
$sql_result .= $total_sql_where;

if ($ifRequestingOwnInsights) {
    if (isset($_REQUEST['active']) && $_REQUEST['active']) {
        $sql_result .= ' HAVING attendees > 0 ';
    } else {
        $sql_result .= ' HAVING attendees = 0 ';
    }
} else {
    if (!$ifOwnerOrAdmin && ($tab == 'insights' || $tab == 'services' || $_REQUEST['id'] == '')) {
        #$sql_result .= ' HAVING attending = 0 ';
    }
}

$sort_style = 'DESC';
$sql_result .= " ORDER BY maenna_professional.datetime " . $sort_style;
$result1 = mysql_query($sql_result);
$i = 0;
$insightRepository = new \Clewed\Insights\InsightRepository();
while ($row1 = mysql_fetch_array($result1)) {


    $pageType = 'insights';

    $count = (int) $row1['count'];
    $attendeeId = null;
    $soldOut = $row1['capacity'] <= $count;


  //check service tag
  if ($row1['capacity'] > $count && $row1['type'] != InsightEntity::TYPE_GROUP_INSIGHT){
      if ($row1['type'] == InsightEntity::TYPE_PRIVATE_INSIGHT) {
          $attendeeId = $insightRepository->findAttendeeIdPurchasedPrivateInsight($row1['id']);
          if (!$attendeeId) {
              $pageType =  'services';
          }
      }else {
          $pageType = 'services';
      }
  }



    if ($tab === $pageType){

        if ($tab == 'insights'|| $tab == 'services' || $_REQUEST['id'] == '') {
            $id = $row1['postedby'];
        }
        else {
            $id = sget($_REQUEST, 'id');
        }
        $utype  = getUserTypeById($id);
        $userId = $row1['uid'];
        $isPrivateInsight = ($row1['type'] == InsightEntity::TYPE_PRIVATE_INSIGHT);
        if ($utype == 'admin') {
            $P_username = 'Admin';
        }
        elseif ($utype == 'super_admin') {
            $P_username = 'Clewed';
        }
        else {
            $P_username = trim(ucfirst($row1['firstname']));
        }
        /*    if (str_word_count($P_username) == 2 && $P_username != 'Super admin') {
                $tmp        = explode(" ", $P_username);
                $P_username = ucwords($tmp[0] . " " . $tmp[1][0]);
            }*/
        $sql_likes_result          = (int) $row1['likes'];
        $sql_remaining_spots_count = (int) $row1['remaining_spots'];
        $likepost                  = (int) $row1['likepost'];
        date_default_timezone_set('EST');
        if ($i == 0) {
            $border = 'style="border:none;"';
            $i++;
        }
        $avatar            = getAvatarUrl($userId, "150");
        $sql               = "select user_id from maenna_discussion_moderator where discussion_id = '" . $row1['id'] . "' limit 1";
        $moderatorResource = mysql_query($sql);
        $moderatorId       = 0;
        $response          = mysql_fetch_assoc($moderatorResource);
        $moderatorAvatar   = false;
        if ($response) {
            $moderatorId     = $response['user_id'];
            $moderatorAvatar = getAvatarUrl($moderatorId, "150");
        }

        $ifActiveModerator = false;
        $ifInsightOwner = ($row1['postedby'] == $user->uid);
        $ifInsightOwnerOrAdmin = ($ifInsightOwner || $ifInsightAdmin);
        $ifCanWrite = $AccessObj->Com_sections['advice']['sections']['maenna_events']['access'] == 'write';

        $attending = $insightRepository->findAttendees($row1['id']);
        $rated = $insightRepository->findUsersWhoRated($row1['id']);
        $ifAttending = in_array($user_idd,$attending);
        $ifRated = in_array($user_idd,$rated);
        $ifInProjectBox = inProjectBox($row1['postedby'], $user_idd);
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
                       href="/account?tab=professionals&page=pro_detail&id=<?= $id ?>&section=pro_industry_view&type=details&pro_id=<?= $row1['id'] ?>"><span
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

                    <div id="clear" style="clear:both"></div>
                    <div>
                        <span style="color:#91939E !important">By:</span><a href="#" id="edit_id<?php echo $row1['id']; ?>" style="color:#00a2bf;" class="profile_details"
                                                                              name="<?= $user->name; ?>">
                            <?php echo $P_username; ?>, <?= ucfirst($row1['pexperties']); ?></a>
                        <?php
                        $res = db_query("SELECT mdm.*,IF (mp.username_type = 1,mp.firstname,CONCAT(mp.firstname,' ', mp.lastname)) as firstname,mp.protype as protype,mp.experties as experties FROM maenna_discussion_moderator mdm join maenna_people mp ON mdm.user_id = mp.pid WHERE discussion_id = %d ORDER BY id LIMIT 1", $row1['id']);
                        if ($res) $moderator = db_fetch_array($res);
                        if ($moderator['status'] == 'invited') {
                            $status = '<span  style="color:#91939E !important" class="inv_status">Invited: </span>';
                        } elseif ($moderator['status'] == 'active') $status = '<span  style="color:#91939E !important" class="inv_status">Guest expert: </span>';
                        else $status = '';
                        if ($moderator) echo '<a href="#" id="pro_id' . $moderator['user_id'] . '" ref="pro_id" style="margin-left:20px;color:#00a2bf;" class="profile_details">' . $status . trim(ucfirst($moderator['firstname'])) . ", " . ucfirst($moderator['experties']) . "</a> ";
                        if ($moderator['status'] == 'invited' && $moderator['user_id'] == $user->uid) {
                            echo "<span class='rsvp_inv'><a style='margin-left:10px;cursor:pointer;' iid='" . $moderator['id'] . "' rel='accept' class='invitation'>Accept<a> / <a iid='" . $moderator['id'] . "' style='cursor:pointer;' rel='reject' class='invitation'>Reject</a></span>";
                        }

                        $ifActiveModerator = ($moderator['status'] == 'active' && $moderator['user_id'] == $user->uid);
                        ?>
                    </div>
                    <div id="clear" style="clear:both"></div>
                    <?php
                    if (strlen($row1['whyattend']) > 150) $more_link = '...<a onclick="discussion(\'' . $id . '\', \'' . $row1['id'] . '\', \'' . $row1['title'] . '\');" href="/account?tab=professionals&page=pro_detail&id=' . $id . '&section=pro_industry_view&type=details&pro_id=' . $row1['id'] . '" class="tool">More</a>'; else $more_link = '';
                    ?>
                    <span
                        style="color:#686b83;float: left; margin: 0; text-align: left;"><?= substr($row1['whyattend'], 0, 150) . $more_link; ?>
                </span>

                    <div id="clear" style="clear:both"></div>
                    <div id="clear" style="clear:both"></div>
                    <?php


                    if ($row1['capacity'] > $count) {
                        ?>
                        <span style="float:left; margin-top:10px; padding-right: 7px; border-right: 1px solid #76787f; line-height: 12px; font-size: 12px; color: #76787F;">
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
                        }
                        elseif ($row1['type'] == InsightEntity::TYPE_PRIVATE_INSIGHT) {
                            $attendeeId = $insightRepository->findAttendeeIdPurchasedPrivateInsight($row1['id']);
                            if ($attendeeId) {
                                $attendeeName = getUserById($attendeeId);
                                echo 'For ' . $attendeeName;
                            } else {
                                echo 'Service';
                            }
                        }
                        else {
                            echo 'Service';
                        }
                        ?>
                    </span>
                        <?php
                    } else {
                        echo '<span class="sold-out" style="padding-right: 7px; border-right: 1px solid #76787f;">Sold out</span>';
                    }


                    if ( $ifInsightOwnerOrAdmin || $ifActiveModerator || $ifCanWrite ) { ?>
                    <span class='discussion_span_edit <?= $moderator['user_id']; ?>'>
                    <?php if ($ifInsightOwner || $ifActiveModerator) { ?>
                        <a style="float:left;margin-top:10px; padding: 0 8px 0 8px; line-height:12px;cursor:text; border-right: 1px solid #76787f;"
                           onclick="return false;" href="#" class="tool"><strong
                                style="color:#76787f; font-weight:normal!important; "><?= $row1['approve_status'] ? 'Approved' : 'Pending Approval' ?></strong></a>
                    <?php } ?>
                        <?php if ($ifInsightOwnerOrAdmin || $ifActiveModerator) { ?>
                            <a style="float:left; margin-top:10px; line-height:12px; padding-right: 7px; padding-left: 7px;"
                               href="#" id="edit_id<?= $row1['id'] ?>"
                               alt="<?= md5($row1['id'] . $user->name . "kyarata75") ?>" name="<?= $user->name; ?>"
                               delType='event' class="tool evedit">
                            <strong style="font-weight:normal!important;">
                                Edit
                            </strong>
                        </a>
                        <?php } ?>
                        <?php } ?>
                        <?php if (($ifInsightOwnerOrAdmin || $ifCanWrite) && !$row1['approve_status']) { ?>
                            <a style="float:left;margin-top:10px; padding:0 8px;border-left:1px solid #76787f; line-height:12px;"
                               href="#" id="remove_id<?= $row1['id'] ?>"
                               alt="<?= md5($row1['id'] . $user->name . "kyarata75") ?>" name="<?= $user->name; ?>"
                               delType='event' class="tool evdelete">
                        Delete
                    </a>
                        <?php } ?>

                        <?php if (!$ifAdmin && ($ifActiveModerator || $ifInsightOwner || $attendeeId == $user->uid)) { ?>
                            <a style="float:left;margin-top:10px; padding:0 8px;border-left:1px solid #76787f;line-height:12px;" href="#"
                               id="editmat_id<?= $row1['id'] ?>" alt="<?= md5($row1['id'] . "kyarata75") ?>"
                               name="<?= $user->name; ?>" delType='event' class="tool">
                            Add Files
                        </a>
                        <?php } ?>

                        <?php if ($row1['postedby'] == $user->uid && $AccessObj->role_id == 4) : ?>
                            <?php if (!$moderator) : ?>
                                <?php
                                /**
                                 * https://clewed.codebasehq.com/projects/clewedcom/tickets/244
                                 *
                                 * "Invite professionals" - this should be only for admin for now.
                                 * please hide this link from professional user account.
                                 */
                                if ($row1['postedby'] != $user->uid) { ?>
                                    <span>
                                <a style="color: #00A2BF; float:left;margin-top:10px; padding:0 7px;line-height:12px; border-left:1px solid #76787f;"
                                   href="/account?tab=professionals&id=<?= $row1['id'] . "&m=" . md5($row1['id'] . "kyarata75"); ?>&pid=<?= $row1['postedby']; ?>"
                                   style="text-transform:none;" class="tool follow">
                                    <strong>Invite Moderator</strong>
                                </a>
                            </span>
                                <?php } ?>

                            <?php else : ?>
                                <span>
                      <a style="color: #00A2BF; float:left;margin-top:10px; padding:0 7px;line-height:12px; border-left:1px solid #76787f;"
                         href="/account?tab=professionals&id=<?= $row1['id'] . '&m=' . md5($row1['id'] . 'kyarata75'); ?>&edit=edit&pid=<?= $row1['postedby']; ?>"
                         data-tooltip="This tool invites another moderator to insight, and rejects previous invitation."
                         rel="edit" style="text-transform:none;" pid="<?= $_REQUEST['id'] ?>" class="tool follow">
                          <strong>Edit moderator</strong>
                      </a>
                    </span>
                            <?php endif; ?>
                        <?php endif; ?>

                        <?php if ($usertype == 'super' || $ifAdmin): ?>

                            <?php if ($row1['approve_status'] == 1): ?>
                                <span id="approve_dis<?= $row1['id'] ?>">
                      <a style="float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f;"
                         onclick="approve_discussion('disapprove', '<?= $row1['id'] ?>', '<?= $id ?>');"
                         title="Disapprove" delType='event'
                         class="tool follow">
                          <strong>
                              Unlist
                          </strong>
                      </a>
                    </span>
                            <?php
                            else:
                                if ($row1['location'] != '') $oncl = "onclick=\"approve_discussion('approve', '" . $row1['id'] . "', '" . $id . "');\""; else $oncl = "onclick=\"alert('You need to enter Clewed Call in to approve insight.');\"";
                                ?>
                                <span id="approve_dis<?= $row1['id'] ?>">
                      <a style="float:left;margin-top:10px; padding:0px 7px 0px 7px;line-height:12px; border-left:1px solid #76787f; color: #00A2BF;"
                         <?= $oncl; ?> title="Approve" delType='event'
                         class="tool follow"> <strong>List</strong></a>
                    </span>
                            <?php endif;?>

                        <?php $_insight = $row1;?>
                        <?php if(!$soldOut):?>
                                <?php if($_insight['featured']):?>
                                    <span data-id="<?php echo $_insight['id'];?>"
                                          class="featured-toggle-wrapper <?php if(!$_insight['approve_status']) echo 'hidden';?>">
                                    <a class="tool manage-insight-link featured-toggle done"
                                       data-id="<?php echo $_insight['id']; ?>"
                                       data-value="1"><strong>Unfeature</strong></a>
                                </span>
                                <?php else:?>
                                    <span data-id="<?php echo $_insight['id'];?>"
                                          class="featured-toggle-wrapper <?php if(!$_insight['approve_status']) echo 'hidden';?>">
                                    <a class="tool manage-insight-link featured-toggle"
                                       data-id="<?php echo $_insight['id']; ?>"
                                       data-value="0"><strong>Feature</strong></a>
                                </span>
                                <?php endif;?>
                            <?php endif;
                        endif;

                        if ($usertype == 'super' || $ifAdmin || ($row1['postedby'] == $user->uid)) {
                            ?>

                            <?php if (!$moderator) {
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
                        }
                        ?>

                        <?php if($ifInsightOwnerOrAdmin && $isPrivateInsight) { ?>
                            <?php if($row1['delivered']) { ?>
                                <span class="manage-insight-link done">
                                Delivered
                            </span>
                            <?php } else { ?>
                                <span>
                                <a class="tool manage-insight-link delivered-toggle"
                                   data-id="<?= $row1['id']; ?>"
                                   data-m="<?= md5($row1['id'] . "kyarata75"); ?>"
                                   data-value="1"><strong>Deliver</strong></a>
                            </span>
                            <?php } ?>
                        <?php }
                        if ($user_idd != $row1['postedby'] && ($ifAttending || $ifInsightAdmin || $ifInProjectBox) ) {

                            if ($ifRated) {

                                echo "<span style='color:#666670!important;' class=\"manage-insight-link\">Review added</span>";

                            }
                            else  {
                                echo "<a class=\"tool manage-insight-link\"
                               href=\"account?tab=professionals&page=pro_detail&id=".$row1['postedby']."&section=pro_industry_view&type=details&pro_id=".$row1['id']."&review=1\"
                               style=\"text-transform:none;cursor:pointer;\">
                                Add expert review
                            </a>";
                            }


                        }


                        ?>


                    <div style="color: #76787F; font-size: 12px; float: right; margin-top: 4px;">
                        <?= $row1['views'] > 1 ? $row1['views'] . ' views |': '' ?>
                        <?= $row1['tags'] ?>
                    </div>
                <div style="clear:both"></div>
              </span>
                </div>
                <span class="attFiles" style="float:left;font-size:10px;">
            </span>
            </div>
        </div>
        <div id="clear" style="clear:both"></div>
        <hr style="margin-top:23px;background:#d0d2d3;">

<?php  }
} ?>

</div>

</div>
</div>

<?php require ROOT . '/themes/maennaco/dialogs/create-insights-help.php' ?>
