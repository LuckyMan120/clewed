<?php 
/**
 * MODERATOR DB UPDATE
 * CREATE  TABLE `maennaco_dev`.`maenna_discussion_moderator` (
  `id` INT NOT NULL ,
  `discussion_id` INT NOT NULL ,
  `user_id` INT NOT NULL ,
  PRIMARY KEY (`id`) ,
  UNIQUE INDEX `disc_mod` (`discussion_id` ASC, `user_id` ASC) );
 * ALTER TABLE `maennaco_dev`.`maenna_discussion_moderator` CHANGE COLUMN `id` `id` INT(11) NOT NULL AUTO_INCREMENT  ;
 * ALTER TABLE `maennaco_dev`.`maenna_discussion_moderator` ADD COLUMN `moderator_name` VARCHAR(255) NULL  AFTER `user_id` ;
 */
?>
<?php require_once './dbcon.php';
error_reporting(0);

function getProId($id)
{
    if(empty($id)) return 'invalid id';
    $sql = "select users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname from users_roles, maenna_people where users_roles.uid = ".$id." and maenna_people.pid = ".$id." limit 1";
    $result = mysql_query($sql);
    $Row = mysql_fetch_object($result);
    if(empty($Row)) return "invalid user role setting - $id";
    $output = strtoupper($Row->firstname);

    return $output;
}?>

<?php
if ($_REQUEST['type'] == 'invitation') {

    if ($_REQUEST['status'] == 'reject')
        mysql_query("DELETE FROM maenna_discussion_moderator where id = ".mysql_real_escape_string($_REQUEST['inv_id'])) or die('error');
    elseif ($_REQUEST['status'] == 'accept')
        mysql_query("UPDATE maenna_discussion_moderator SET status = 'active' where id = ".mysql_real_escape_string($_REQUEST['inv_id'])) or die('error');

    die("OK");

}

if(empty($_POST) && isset($_GET['discid'])): ?>
  <script type="text/javascript">
    $(document).ready(function(){
/*      $("input[name=modname]").Watermark("Add moderator name");
      $("input[name=modurl]").Watermark("Add link to moderator's Clewed profile");*/
        var availableTags = [
            <?php
            $q = mysql_query ("SELECT DISTINCT pid,real_first_name,real_last_name,profile FROM maenna_people mp JOIN users u on mp.pid = u.uid where firstname is not null and u.status = 1") or die(mysql_error());
            while ($row = mysql_fetch_array($q)) {

                echo '{value: "'.$row['pid'].'", label: "'.mysql_real_escape_string(ucfirst($row['real_first_name']).' '.ucfirst($row['real_last_name'])).'",profile:"'.mysql_real_escape_string($row['profile']).'"},';
            }
            ?>
        ];

        $( document ).tooltip({
            items: "ul.ui-autocomplete li.ui-menu-item a.summary-tooltip",
            content: function () {
                var profile = $(this).attr('data-profile');
                return profile;
            }
        });

        //$("#modname").autoSuggest(availableTags.items, {selectedItemProp: "name", searchObjProps: "name"});
        $( "#modname" ).autocomplete({
           source: availableTags,
            focus: function( event, ui ) {
                $( "#modname" ).val( ui.item.label );
                return false;
            },
            select: function( event, ui ) {
                $( "#modname" ).val( ui.item.label );
                $( "input[name='proid']" ).val( ui.item.value );
                return false;
            }
        }).data('autocomplete')._renderItem = function( ul, item ) {
            return $( "<li class=\"ui-menu-item\">")
                .append( "<a class=\"ui-corner-all summary-tooltip\" data-profile='"+item.profile+"' data-id=\""+item.value+"\">" + item.label + "</a>" )
                .appendTo( ul );
        };;
    });
  </script>
    <?php

    if ($_REQUEST['type'] == 'edit') $edit_input = '<input type="hidden" name="edit" value="edit">';

    ?>
  <form method="post">
    <div style="margin-bottom: 10px;">
      <input type="text" id="modname" name="modname" placeholder="Add colloaborator name" style="width: 250px !important">
        <div class="chkbox">
            <input type="checkbox" value="1" id="notify" name="notify" />
            <label for="notify"></label>
        </div>
        <p style="font-family: Lato Italic; font-size:14pt;">Delete moderator.</p>
        <style>.chkbox {
                width: 25px;
                position: relative;
                float:left;
                margin-left:10px;
                margin-top:11px;
            }
            .chkbox label {
                cursor: pointer;
                position: absolute;
                width: 15px;
                height: 15px;
                top: 0;
                left: 0;
                background: white;
                border:1px solid gray;
                border-radius:2px;
            }
            .chkbox label:after {
                opacity: 0;
                content: "";
                position: absolute;
                width: 9px;
                height: 5px;
                background: transparent;
                top: 2px;
                left: 2px;
                border: 3px solid #333;
                border-top: none;
                border-right: none;
                -webkit-transform: rotate(-45deg);
                -moz-transform: rotate(-45deg);
                -o-transform: rotate(-45deg);
                -ms-transform: rotate(-45deg);
                transform: rotate(-45deg);
            }
            .chkbox label:hover::after {
                opacity: 0.5;
            }
            .chkbox input[type=checkbox]:checked + label:after {
                opacity: 1;
            }
        </style>
      <input type="hidden" name="proid">
    </div>

      <input type="hidden" name="_token" value="<?php print md5($_GET['discid'].'kyarata75'); ?>">
      <input type="hidden" name="discid" value="<?php print $_GET['discid'];

          ?>">
      <?=$edit_input;?>

  </form>
<?php elseif(!empty($_POST['m'])): ?>
  <?php

    if ($_POST['notify'] == '1') {

        mysql_query("DELETE FROM maenna_discussion_moderator WHERE discussion_id = ". (int) $_POST['discid']) or die('error');
        die("OK");

    }


    if($_POST['m'] === md5($_POST['discid'].'kyarata75') && !empty($_POST['modname'])){




        $modname = mysql_real_escape_string($_POST['modname']);
        $itopic = $_REQUEST["itopic"];
        $id = mysql_real_escape_string($_POST['proid']);
        if ($_REQUEST['type'] == 'invite') $status = 'invited'; else $status = 'active';

        //Get insights originator id for url

        $q = mysql_query("SELECT postedby,IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) as firstname from maenna_professional mpr left JOIN maenna_people on mpr.postedby = maenna_people.pid WHERE id =".mysql_real_escape_string($_POST['discid']));
        $posted_by = mysql_fetch_object($q);

        $email = mysql_query("SELECT mp.email,IF (mp.username_type = 1,mp.firstname,CONCAT(mp.firstname,' ', mp.lastname)) as firstname FROM maenna_people mp where pid = ".$id);
        $email_data = mysql_fetch_object($email);

        if ($_POST['bEdit'] != 'edit') {

        $result = mysql_query("INSERT INTO maenna_discussion_moderator(discussion_id, user_id, moderator_name,status)
          VALUES(".mysql_real_escape_string($_POST['discid']).",".
                mysql_real_escape_string($id).",'".$id."','".$status."')");

           /*Here you should send email to participant notifying him that he has been invited to participate to Insight. You have all the needed data in variables:
            participant name: $email_data->firstname
            email: $email_data->email
            Insight topic from which invitation expired: $itopic
           Link to answer invitation: https://www.clewed.com/account?tab=professionals&id=$posted_by&page=pro_detail&section=pro_industry_view&type=details&pro_id=$_POST['discid']

*/
            $to = $email_data->email;

            $subject = "Your participation in upcoming Insight or Service";
            $headers = "From: clewed@clewed.com\r\n";
            $headers .= 'Bcc: clewed@clewed.com' . "\r\n";
            $headers .= "MIME-Version: 1.0\r\n";
            $headers .= "Content-Type: text/html\r\n";
            $link = "https://www.clewed.com/account?tab=professionals&id=".$posted_by->postedby."&page=pro_detail&type=details&pro_id=".$_POST['discid'];
            $content = <<< END
            Hi $email_data->firstname,
            <br><br>
            $posted_by->firstname has invited you to participate in topic $itopic.
            <br>
            Please visit <a href=$link>www.clewed.com</a> to respond as soon as possible.<br><br>

            The Clewed team.
END;

            mail($to,$subject, $content, $headers);

        }
        else {

            //Test if this user is already invited to this insight
            $q = mysql_query("select user_id from maenna_discussion_moderator WHERE discussion_id = ".(int) $_POST['discid']." and status = 'invited'");
            $inv_id = mysql_fetch_object($q);

            if ($id == $inv_id->user_id) die('duplicate');

/*            $prev = mysql_query("  SELECT mdm.*,IF (mp.username_type = 1,mp.firstname,CONCAT(mp.firstname,' ', mp.lastname)) as firstname,mp.email from maenna_discussion_moderator mdm left join maenna_people mp on mdm.user_id = mp.pid WHERE discussion_id = ".mysql_real_escape_string($_POST['discid'])." and mdm.status = 'invited'");

            $prev_data = mysql_fetch_object($prev);*/

            $result = mysql_query("UPDATE maenna_discussion_moderator SET user_id = ".
                mysql_real_escape_string($id).", moderator_name = '".mysql_real_escape_string($id)."', status = '".$status."'   WHERE discussion_id = ".mysql_real_escape_string($_POST['discid']));



/*            if ($prev_data) {

                /*Here you should send email to participant notifying him that his invitation has expired. You have all the needed data in variables:
                participant name: $prev_data->firstname
                email: $prev_data->email
                Insight topic from which invitation expired: $itopic

                $to = $prev_data->email;
                $subject = "Your invitation has expired!";
                $headers = "From: clewed@clewed.com\r\n";
                $headers .= 'Bcc: clewed@clewed.com' . "\r\n";
                $headers .= "MIME-Version: 1.0\r\n";
                $headers .= "Content-Type: text/html\r\n";
                $content = <<< END
                Hi $prev_data->firstname,
                <br><br>
                Your Invitation to $itopic has expired!
                <br>

                The Clewed team.
END;
                mail($to,$subject, $content, $headers);
           } */

            $to = $email_data->email;

            $subject = "Your participation in upcoming Insight or Service";

            $headers = "From: clewed@clewed.com\r\n";
            $headers .= 'Bcc: clewed@clewed.com' . "\r\n";
            $headers .= "MIME-Version: 1.0\r\n";
            $headers .= "Content-Type: text/html\r\n";
            $link = "https://www.clewed.com/account?tab=professionals&id=".$posted_by->postedby."&page=pro_detail&type=details&pro_id=".$_POST['discid'];
            $content = <<< END
            Hi $email_data->firstname,
            <br><br>
            $posted_by->firstname has invited you to participate in topic $itopic.
            <br>
            Please visit <a href=$link>www.clewed.com</a> to respond as soon as possible.<br><br>

            The Clewed team.
END;

            mail($to,$subject, $content, $headers);

        }
        if(!$result){
          print "There was a problem editing moderator. Please try again";
        } else {
          print "OK";
        }
    }
  ?>
<?php endif; ?>