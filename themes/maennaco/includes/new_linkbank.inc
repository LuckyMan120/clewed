<?php

function linkbank()
{
    global $AccessObj;
    $uid = $AccessObj->uid;
    $get_user_email_sql = "SELECT mail FROM users WHERE uid = '" . $uid . "'";
    $user_email_res = mysql_query($get_user_email_sql);
    $user_email_data = mysql_fetch_object($user_email_res);
    $user_email = $user_email_data->mail;
    $companyid = $_REQUEST['companyid'];
    $flag = $_REQUEST['flag'];
    if ($companyid == null)
        $redirect_invest_now = 'account?tab=investments';
    else
        $redirect_invest_now = 'account?tab=companies&page=company_detail&id=' . $companyid . '&section=company_name&panel=invest_now';
    $content = '';

    if ($flag == 'edit') {
        $customerid = sget($_REQUEST, 'customerid');
        $user_data_sql = "SELECT * FROM users_bank_info WHERE uid = '" . $uid . "'";
        $user_data_res = db_query($user_data_sql, array($uid));
        $user_data = mysql_fetch_object($user_data_res);
        if ($user_data == null) {
            $insert_sql = "INSERT INTO users_bank_info SET uid='%s',customerid='%s'";
            if (db_query($insert_sql, array($uid, $customerid))) {
                drupal_set_message("Success insert bank link");
                if ($companyid != null)
                    drupal_goto('account', 'tab=companies&page=company_detail&id=' . $companyid . '&section=company_name&panel=invest_now');
                else drupal_goto('account', 'tab=linkbank');
            } else {
                drupal_set_message("Faild insert bank link");
            };
        } else {
            $update_sql = "UPDATE users_bank_info SET customerid='%s' WHERE uid='%s'";
            if (db_query($update_sql, array($customerid, $uid))) {
                drupal_set_message("Success update bank link");
                if ($companyid != null)
                    drupal_goto('account', 'tab=companies&page=company_detail&id=' . $companyid . '&section=company_name&panel=invest_now');
                else drupal_goto('account', 'tab=linkbank');
            } else {
                drupal_set_message("Faild update bank link");
            }
        }
    }
    $bank_sql = "SELECT * FROM users_bank_info WHERE uid = '" . $uid . "'";
    $bank_data_res = mysql_query($bank_sql);
    $flag = mysql_num_rows($bank_data_res);
    $Block['title'] = "LINK BANK ACCOUNTS";  
    if ($flag < 1) {
        $content .= '<div class="edit-bank" style="margin-top: 20px;">';
        $content .= '<font style="font-size:20pt; font-famly:Latoregular;">You have not linked an external bank account</font><br/>';
        $content .= '<p style="margin-top:30px;"><font style="font-size:14pt; font-famly:Latoregular;">In order to transfer funds to and from Clewed, you’ll need to link an external account.</font></p>';
        $content .= '<div class="setting-label bottomcontent" style="margin-top:30px;font-famly:Latoregular;">
                            <input type="button"  id="linkbank_btn" value="Add external bank account" class="button" style="width:250px; margin-bottom: 30px">
                    </div>';
        $content .= '</div>';
        $content .= '<div class="setting-label bottomcontent" >
                            <a href= "' . $redirect_invest_now . '" class="button" style="width: 71px;height:24px;  margin-top: 30px; border-radius: 6px; font-family: Lato Bold 14Px !important;" >Skip</a>
                    </div>';
        $content .= '</div>';
        
    } else {
        $Block['title'] = "Your bank infomation";
        $content .= '<link type="text/css" rel="stylesheet" media="all" href="/bank_card.css">';
        $content .= '<div class="edit-bank" style="font-famly:Latoregular; margin-top: 20px;">';
        $content .= '<font style="font-size:20px">Linked Banks:</font><br/>';
        $content .= '<div class="bank_div">';
        while($data = mysql_fetch_object($bank_data_res)){
            $status = $data->status;
            if($status == 1) $class = 'is_active';
            else $class = null;
            $content .= "<div class='card-wrap link-wrapper ".$class."'>";
            $content .= "<div class='card'>
                            <div class='flag'><img src='/themes/maennaco/images/bank-account.png' alt='mastercard'></div>
                            <div class='bank_name'> $data->bankname •••• $data->banknumber </div>
                        </div>";
            $content .= '</div>';
        }
        $content .= '</div>';
        $content .= '<div class="bank-button-wrapper"><input type="button"  id="linkbank_btn" value="ADD NEW ACCOUNT" class="custom-bank-button" style="width:250px">';
        $content .= '<a href= "' . $redirect_invest_now . '" class="custom-bank-button">INVEST NOW</a>';
        $content .= '</div>';
    }
    $content .= '<script src="https://cdn.plaid.com/link/v2/stable/link-initialize.js"></script>';
    $content .= '<script>';
    $content .= "
            $('.card-wrap.link-wrapper').click(function(){
                var bank_name = $(this).find('.bank_name').html();
                $('.bank_div').find('.link-wrapper').removeClass('is_active');
                $(this).addClass('is_active');
                userid = '$uid'
                $.post( '/setbank.php', {bank_name,userid}, function( data ) {                        
                    if (data != null || data != ''){
                        alert('Bank was reselected!');
                    }else{
                        alert('Failed');
                    }
                });                
            });
            (async function() {
                async function fetchLinkToken() {
                    let response = await fetch('/payment_create_link_token.php',{
                        method: 'POST',
                        headers: {
                            'Content-Type': 'application/json',
                        },
                        body: JSON.stringify({'customerinfo':'".$user_email."'}),
                    });
                    let responseJSON  = await response.json();
                    return responseJSON.link_token;
                }
        
                const configs = {
                    token: await fetchLinkToken(),
                    env: 'sandbox',
                    onSuccess: async function(public_token, metadata){
                        account_id = metadata.accounts[0].id
                        user_email = '$user_email'
                        prouserid = '$uid'
                        description = 'Clewed Professional Client';
                        $.post( '/payment_process_plaid_token.php', {public_token,account_id,prouserid,user_email,description}, function( data ) {                        
                            if (data != null || data != ''){
                                // console.log(data);
                                // window.location = data;
                                window.location.replace('/account?tab=linkbank');
                            }else{
                                window.location.replace('/account?tab=linkbank');
                            }
                        });
                    },
                    onExit: async function(err, metadata) {
                        if (err != null) {
                            console.log(err);
                            console.log(metadata);   
                        }
                    }
                }
        
                var linkHandler = Plaid.create(configs);
                document.getElementById('linkbank_btn').onclick = function() {
                    linkHandler.open();
                };
            })();
        ";
    $content .= '</script>';

    $Block['body'] = content_box($Block['title'], $content);
    return $Block;

    
}
