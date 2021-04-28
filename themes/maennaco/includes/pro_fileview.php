<link href="<?php echo $base_url; ?>/themes/maennaco/jui/comments/css/screen.css?as" type="text/css" rel="stylesheet" />
<link href="<?php echo $base_url; ?>/themes/maennaco/jui/comments/css/SpryTabbedPanels.css" type="text/css" rel="stylesheet" />
<style type="text/css">
div.content_box .box_title { margin-top:14px; }
.text_button { border: none !important; background-color: transparent!important; color:#0fabc4!important; cursor: pointer; font-family: 'LatoRegular'!important; font-size: 14px!important; 
	font-style:normal !important; }


div.content_box .box_content{
	/*padding-bottom:15px;*/
	padding-top:2px;
	font-size:14px;
	text-align:left;
	font-family:'LatoRegular';
	float:left !important;
}
div.content_box{ padding-left:10px; position:relative; width:100%; padding-bottom:10px; float:left !important; }
</style>
<?php pro_tabs(); ?>
<div id = "docprev">
    <script type="text/javascript" src="/themes/maennaco/jui/comments/js/jquery.livequery.js"></script>
    <script type='text/javascript'>
        $(document).ready(function(){

            $("#watermark").focus(function () {

                    $("#submitDiv").show();

                    }

            );

            $("textarea[name='commentMark']").focus(function () {

                        $(this).parent().parent().children('a').show();

                    }

            );

            $('#shareButton').click(function(event) {

                event.preventDefault();

                var a =$("#watermark").val();
                var m =  $("#documenturl").attr("m");
                var u =  $("#documenturl").attr("name");
                var fid = $("#documenturl").attr("fid");

                if(a != "")
                {

                    $.post("/themes/maennaco/includes/pro_comments_posts.php", {

                        value: a,
                        m: m,
                        uid: u,
                        fid: fid

                    }, function(response){

                        $('#posting').prepend($(response).show());
                        $("#watermark").val("");

                    });
                }
            });

            $(".SubmitSubComment").livequery('click', function(event) {
                        event.preventDefault();

                         pid = $(this).attr("pid");
                         a =$("#commentMark-"+pid).val();
                         m =  $(this).attr("m");
                         u =  $(this).attr("uid");
                        sel_obj = $(this);


                        if(a != "")
                        {

                            $.post("/themes/maennaco/includes/add_comment.php?type=pro_file_comment", {

                                value: a,
                                m: m,
                                uid: u,
                                pid: pid

                            }, function(response){

                                   //alert(sel_obj.parent().parent().attr('id'));
                                sel_obj.parent().parent().prev().append(($(response).show()));
                                $("#commentMark-"+pid).val("");

                            });
                        }




                    });
					

                    $("a[id^='remove_id']").livequery('click', function() {

                        m = $(this).attr('m');
                        pid = $(this).attr('pid');

                        $.post("/themes/maennaco/includes/delete_comment.php?type=pro_file_post", {

                            pid: pid,
                            m: m

                        }, function(response){

                           if (response.status == 'success') {

                               alert(response.display);
                               $("#record-"+pid).hide();

                           }
                            else alert(response.display);
                        },"json");

        });
            $("a[id^='cid-']").livequery('click', function(event) {

                event.preventDefault();

                m = $(this).attr('m');
                cid = $(this).attr('cid');

                $.post("/themes/maennaco/includes/delete_comment.php?type=pro_file_comment", {

                    cid: cid,
                    m: m

                }, function(response){

                    if (response.status == 'success') {

                        alert(response.display);
                        $("#comment-"+cid).hide();

                    }
                    else alert(response.display);
                },"json");

            });

        });
    </script>

    <div align="center">

    <div id="documenturl" title="<?php echo $host ?>" name="<?php echo $editor ?>" fid="<?=$fid;?>" m="<?php  echo md5($editor.$fid."kyarata75") ?>" class="UIComposer_Box" align="center" width="0" height="0" >

    </div>
    <?php if (!empty($file_path)) { ?>
    <div class="TabbedPanelsContent TabbedPanelsContentVisible">
    <iframe src="https://docs.google.com/viewer?url=http://<?=$host;?>/sites/default/files/<?=empty($_REQUEST['root']) ? 'events_tmp/' . $file_path : $file_path;?>&embedded=true" width="600" height="670" style="border: none;"></iframe>
    <br>
    <br>
    <div style="position: relative;margin-bottom:10px; ">
        <form action="" method="post" name="postsForm">

            <div class="UIComposer_Box">
	
			<span class="w" style="margin-bottom: -11px;">
			<textarea style="font-size:14px;font-family:'LatoRegular';border: 1px solid #CCCCCC !important;height:25px; margin-left:5px;width:593px;" class="input" id="watermark" placeholder = 'Enter your comment here' name="watermark"></textarea>
			</span>

                <div style="clear:both"></div>

                <div id="submitDiv" align="right" style="display:none; margin-top:10px;">
                    <a style="cursor:pointer;color: #0fabc4!important;" id="shareButton"  class="text-button"> Submit </a>

                </div>
            </div>

        </form>
        <div style="clear: both;">&nbsp;</div>
        <!--<div style="position: relative;border-bottom:dotted 1px #ccc;bottom: 4px;z-index:-1">
            <div class="discussions">
                Discussions
            </div>
         </div>-->

    </div>
    <div class="tabtags cmtloop" id="posting" align="center">

                 <?php
                 include_once('pro_comments_posts.php');

                 ?>

    </div>
    </div>
    <?php } else { echo '<div style="margin-top:50px;">Please select a file to view here.</div>'; }?>


</div>
</div>