<?php

        chdir('../../../');
        require_once './includes/bootstrap.inc';
        drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

        $return = menu_execute_active_handler();

        if (is_int($return)) {
            switch ($return) {
                case MENU_NOT_FOUND:
                    drupal_not_found();
                    break;
                case MENU_ACCESS_DENIED:
                    drupal_access_denied();
                    break;
                case MENU_SITE_OFFLINE:
                    drupal_site_offline();
                    break;
            }
        }

    if ($_REQUEST['type'] == 'new') {

        $time = time();
        $from = sget($_REQUEST,'sender');

        $to_array = explode(',',sget($_REQUEST,'toArray'));
        $body = sget($_REQUEST,'body');

        //Starts a new conversation
        $sql = "INSERT INTO maenna_conversation (started_by, started) VALUES (%d, '%s')";
        db_query($sql,array($from,$time)) or die(mysql_error());

        $conv_id = db_last_insert_id('maenna_conversation','id');

        //Insert text message into messages table
        $sql = "INSERT INTO maenna_conversation_messages (conv_id,body,sent) VALUES (%d,'%s','%s')";
        db_query($sql,array($conv_id,$body,$time));

        $message_id = db_last_insert_id('maenna_conversation_messages','id');

        foreach($to_array as $key=>$value) {

            $sql = "INSERT INTO maenna_messages_recepients (message_id,sender,recepient) VALUES (%d,%d,%d)";
            db_query($sql, array($message_id,$from,$value));

        }


        die('uspeh');

    }


?>