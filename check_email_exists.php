<?php

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

        $domain = explode("@", $_REQUEST['email']);
        $mail_domains = array(
                                "gmail.com",
                                "yahoo.com",
                                "google.com",
                                "ymail.com",
                                "hotmail.com",
                                "aol.com",
                                "outlook.com",
                                "mail.com",
                                "yandex.com",
                                "hushmail.com",
                                "zoho.com",
                                "clewed.com"
                                );

      $q = mysql_query("SELECT * FROM users WHERE mail = '".$_REQUEST['email']."' and status = 1 ") or die(mysql_error());
      if (mysql_num_rows($q) > 0 ) {
		die('false');
		}

        $q = mysql_query("SELECT * FROM rejected_users WHERE user_email = '".$_REQUEST['email']."' ") or die(mysql_error());
        if (mysql_num_rows($q) > 0 ) {
            die('false');
        }


/*        if (!in_array($domain[1],$mail_domains)) {
            $q = mysql_query("SELECT * from users where right(mail, length(mail)-INSTR(mail, '@')) = '".$domain[1]."' and status = 1");
            die("SELECT * from users where right(mail, length(mail)-INSTR(mail, '@')) = '".$domain[1]."' and status = 1");
            if (mysql_num_rows($q) > 0 ) {

                die('duplicate domain');
            }
        }*/
      die('true');
?>