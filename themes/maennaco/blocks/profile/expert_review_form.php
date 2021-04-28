<?php
/**
 * Created by PhpStorm.
 * User: vladimir
 * Date: 01.10.15
 * Time: 22:59
 */
use Clewed\Insights\InsightEntity;
if (isset($_REQUEST['type']) && $_REQUEST['type'] == 'getReviewForm') require_once "../../includes/dbcon.php";

/**
 * @param $toUserId
 * @param bool $openDialog
 * @param InsightEntity|null $toInsight
 */
function displayExpertReviewForm($toUserId, $openDialog = false, $toInsight = null, $toProjectServiceId = null) {

    if (isset($_REQUEST['type']) && $_REQUEST['type'] == 'getReviewForm'){

        $AccessObj = (object) array("uid" => $_REQUEST['editorId'],"user_type" => $_REQUEST['utype']);

    }
    else global $AccessObj;


    $toUser = new \Clewed\User\User();
    $toUserName = $toUser->getUserName($toUserId);

    //if (canRateUser($_REQUEST['id'], $AccessObj->uid)) $content .= "<div class='add_review'>Add review</div>";
    ob_start();
?>

<style>

    #rate_user_dialog .rateit {
        float: right;
    }

    .rateit {
        /*float:right;*/
    }
    .rate_hint {
        font-size:12px;
        font-style:italic;
        line-height:normal;
        border: 1px solid #c0c0c0;
        width: 300px;
        float: left;
        margin-left: 20px;
        height: 75px;
        margin-top: 20px;
        padding-left:50px;
        padding-top:60px;
    }
    .add_review {
        border-left: 1px solid #006274;
        color: #00a2bf;
        cursor: pointer;
        float: left;
        font-size: 12px;
        line-height: 11px;
        margin-left: 6px;
        margin-top: 8px;
        padding-left: 4px;
    }
    .add_review:hover {color:#00a2bf;}
    .get_reviews {
        cursor:pointer;
        /*float:left;*/
        margin-top:2px;
        vertical-align: top;
    }
    .get_reviews:hover {color:#00a2bf;}

    #rate_user_dialog textarea::-webkit-input-placeholder { font-style:italic;font-family: "LatoRegular"; }
    #rate_user_dialog textarea::-moz-placeholder { font-style:italic;font-family: "LatoRegular"; } /* firefox 19+ */
    #rate_user_dialog textarea:-ms-input-placeholder { font-style:italic;font-family: "LatoRegular" ;} /* ie */
    #rate_user_dialog textareainput:-moz-placeholder { font-style:italic;font-family: "LatoRegular"; }

    textarea.invalid {border-color: red !important;}
    </style>

    <script type="text/javascript">

    var reviewBFormTitle = "<?= $toInsight ? ($toInsight->isGroupInsight() ? 'Rate Insight' : 'Rate Service') : 'Rate '.$toUserName; ?>";
    var openDialog = <?= $openDialog ? 'true' : 'false'; ?>;
    var duplicateReviewCallback;

    $(document).ready(function(){
        init_rate();
        init_follow();
        $("#get_reviews_dialog").dialog({
            modal: true,
            autoOpen: false,
            width:700,
            height:500,
            resizable: true,
            title: "User Ratings",
            buttons: {
                Close: function() {
                    $(this).dialog("close");
                }
            }
        });

        $("#rate_user_dialog").dialog({
            modal: true,
            autoOpen: openDialog,
            width:700,
            height:430,
            resizable: true,
            title: reviewBFormTitle,
            buttons: {
                Rate: function () {

                    var dialog = $("#rate_user_dialog");
                    var rate_str = {};
                    var rate_overall = 0;
                    var editor = dialog.find("#editor_uid").val();
                    var target = dialog.find("#target_uid").val();
                    var targetInsightId = dialog.find("#target_insight_id").val();
                    var comment = dialog.find("textarea").val();
                    var if_admin = dialog.find("#if_admin").val();
                    var refresh =  '<?php if (isset($_REQUEST['type']) && $_REQUEST['type'] == 'getReviewForm') echo false; else echo true;?>';
                    var ifAllSel = true;



                    $(this).find("input[id^=\'rate_\']").each (function() {
                        if ($(this).val() == 0) {
                            ifAllSel = false;
                        }
                        rate_str[$(this).attr("id")] = $(this).val();
                        rate_overall = rate_overall + parseInt($(this).val());
                    });
                    if (!ifAllSel) {
                        alert("Please rate all sections in order to add review.");
                        return false;
                    }

                    if(!comment.trim().length && !dialog.data('empty-comment-warning')) {
                        dialog.data('empty-comment-warning', 1);
                        dialog.find("textarea").addClass('invalid');
                        alert("Please help the community by sharing your experience in the comment section");
                        return false;
                    }
                    else {
                        dialog.find("textarea").removeClass('invalid');
                    }

                    rate_overall = rate_overall / 5;

                    $.post("/themes/maennaco/includes/like.php?type=rate_user",
                        {
                            rate_str: JSON.stringify(rate_str),
                            rate_overall:rate_overall,
                            target_uid: target,
                            target_insight_id: targetInsightId,
                            target_project_service_id: '<?= $toProjectServiceId ?>',
                            editor_uid: editor,
                            comment: comment,
                            if_admin:if_admin
                        },
                        function(response) {
                            if (response == "success") {
                                alert("Your rating was posted successfully.");
                                $(".waitingRate").append("(Review Added)");
                                $(".waitingRate").removeClass('reviewUser');
                                $(".waitingRate").removeClass('waitingRate');

                                if ( $("a[data-id='" + target +"']").length ) {

                                    $("a[data-id='" + target +"']").html("Review added");
                                    $("a[data-id='" + target +"']").prop("onclick", null);
                                    $("#rate_user_dialog").hide();

                                }
                                else $("#rate_user_dialog").remove();
                            }
                            dialog.dialog("close");
                            if (refresh)
                                location.href = location.href.replace('&review=1', '');
                            else

                            return false;
//                            if (duplicateReviewCallback !== undefined) {
//                                duplicateReviewCallback();
//                            } else {
//                                location.reload();
//                            }

                        }
                    );
                },
                Close: function() {
                    $(this).dialog("close");
                    $("#rate_user_dialog").hide();
                    $(".waitingRate").removeClass('waitingRate');
                }

            }
        });
    });
    </script>
    <div id="get_reviews_dialog" style="display:none"></div>

    <div id="rate_user_dialog" style="display:none">

        <div class="stars_container" style="width:280px;float:left;font-size:15px;margin-top:20px;">
        <span>Overall</span> <div class="rateit overall" style="margin-left:20px;" data-rateit-step="1" data-rateit-readonly="true" data-rateit-value="0" data-rateit-starwidth="12" data-rateit-starheight="12" data-rateit-resetable="false"></div>
        <br><hr style="margin-top:10px;margin-bottom:5px;background:#c0c0c0;">
        <span>Subject Matter Knowledge</span> <div class="rateit individual" style="margin-left:20px;" data-rate-type="1" data-rateit-step="1" data-rateit-starwidth="12" data-rateit-starheight="12" data-rateit-resetable="false"></div>
        <br>
        <span>Thoughtful, Actionable Advice </span> <div class="rateit individual" style="margin-left:20px;" data-rate-type="2" data-rateit-step="1" data-rateit-starwidth="12" data-rateit-starheight="12" data-rateit-resetable="false"></div>
        <br>
        Communication <div class="rateit individual" style="margin-left:20px;" data-rateit-step="1" data-rateit-starwidth="12" data-rate-type="3" data-rateit-starheight="12" data-rateit-resetable="false"></div><br>
        Judgement <div class="rateit individual" style="margin-left:20px;" data-rateit-step="1" data-rateit-starwidth="12" data-rate-type="4" data-rateit-starheight="12" data-rateit-resetable="false"></div>
        <br>
        Resource / contact sharing <div class="rateit individual" style="margin-left:20px;" data-rateit-step="1" data-rateit-starwidth="12" data-rate-type="5" data-rateit-starheight="12" data-rateit-resetable="false"></div>
        <br>
        </div>

        <div class="rate_hint">Roll over stars. Then click to rate</div>
        <div style="clear:both;"></div>

        <textarea placeholder="Your comments help the profesional and others learn about areas this expert is great at. Please provide constructive feedback. Maximum 300 characters." maxlength="300" style="line-height:normal;font-weight:normal;float:left;font-family:'LatoRegular' !important;font-size:14px !important;margin-top:30px;height:100px;width:648px;border-width:1px !important;border-color:#c0c0c0;-moz-border-top-colors: #C0C0C0;-moz-border-left-colors: #C0C0C0;-moz-border-bottom-colors:#C0C0C0;-moz-border-right-colors:#C0C0C0;" class="mceNoEditor" id="rate_comment"></textarea>
        <div style="clear:both;"></div>
        <input type="hidden" id="rate_1" name="rate_1" value=0>
        <input type="hidden" id="rate_2" name="rate_2" value=0>
        <input type="hidden" id="rate_3" name="rate_3" value=0>
        <input type="hidden" id="rate_4" name="rate_4" value=0>
        <input type="hidden" id="rate_5" name="rate_5" value=0>
        <input type="hidden" id="target_uid" name="target_uid" value="<?= (int)$toUserId; ?>">
        <input type="hidden" id="target_insight_id" name="target_insight_id" value="<?= $toInsight ? $toInsight->id : 0; ?>">
        <input type="hidden" id="editor_uid" name="editor_uid" value="<?= $AccessObj->uid; ?>">
        <input type="hidden" id="if_admin" name="if_admin" value="<?= (in_array($AccessObj->user_type, array("admin", "super")) ? "1" : "0"); ?>">
    </div>

<?php
return ob_get_clean();
}

if (isset($_REQUEST['type']) && $_REQUEST['type'] == 'getReviewForm') {

    if (!md5($_REQUEST['targetId'] . "kyarata75") === $_REQUEST['hash'])
        die(json_encode(array(
        'success' => false,
        'error' => "Authentication problem"
    )));

    $targetId = $_REQUEST['targetId'];
    $serviceId = $_REQUEST['serviceId'];

    die(json_encode(array(
        'success' => true,
        'data' => array(
            'html' => displayExpertReviewForm($targetId,true,null,$serviceId)
        )
    )));

}

?>