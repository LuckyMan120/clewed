<form action="<?= PAYPAL_URL ?>" method="post" style="margin: 0; padding: 0;">

    <?php

    if (isset($_REQUEST['type']) && $_REQUEST['type'] == 'insight-page') {
        if ($this->response['cost'] == 0)
            echo "<input type='button' onclick='joinnow2(" . $this->response['event-id'] . ",$user->uid, \"false\", \"false\");' class='join' value='Join free'>";
        else
            echo '<input type="submit" id="joinnow" class="join" value="Join $' . $discountAmount . '"/>';
    }
    ?>



    <input type="hidden" name="cmd" value="_xclick"/>
    <input type="hidden" name="business" value="<?= PAYPAL_ACCOUNT ?>"/>
    <input type="hidden" name="no_note" value="1"/>
    <input type="hidden" name="no_shipping" value="1"/>
    <input type="hidden" name="bn" value="Clewed_Insight_<?= $itemNumber ?>_US"/>
    <input type="hidden" name="hosted_button_id" value="58JBKVTL2D9JW"/>

    <input type="hidden" name="notify_url" value="<?= PROTO . HOST ?>/paypal-callback.php"/>
    <input type="hidden" name="return" value="<?= PROTO . HOST . $returnUrl ?>"/>
    <input type="hidden" name="cancel_return" value="<?= PROTO . HOST . $returnUrl ?>"/>

    <input type="hidden" name="email" value="<?= $email ?>"/>
    <input type="hidden" name="first_name" value="<?= $firstname ?>"/>
    <input type="hidden" name="last_name" value="<?= $lastname ?>"/>

    <input type="hidden" name="item_name" value="<?= $itemName ?>"/>
    <input type="hidden" name="item_number" value="<?= $itemNumber ?>"/>
    <input type="hidden" name="amount" value="<?= $amount ?>"/>
    <?php if (!empty($discountAmount)) : ?>
    <input type="hidden" name="discount_rate" value="<?= $rate ?>"/>
    <?php endif; ?>
    <input type="hidden" name="currency_code" value="USD"/>
    <input type="hidden" name="lc" value="US"/>

    <input type="hidden" name="custom" value="<?= $custom ?>"/>
</form>
