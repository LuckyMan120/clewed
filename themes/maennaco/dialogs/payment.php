<div id="payment" style="display: none" data-id="<?= $id ?>">
    <script type="text/javascript" src="/js/paypal-checkout.js"></script>
    <table style="width: 100%">
        <tr>
            <td colspan="2">To Join, Proceed with Payment</td>
        </tr>
        <tr>
            <td class="poptit">Cost:</td>
            <td class="poptit">$&thinsp;<span id="total-cost"><?= $cost ?></span></td>
        </tr>
        <?php
        global $user;
        $discountModel = new \Clewed\Insights\DiscountModel();
        $discountModel->getInsightDiscount($_REQUEST['pro_id']);
        if ($discountModel) :
            ?>
            <tr>
                <td>
                    <label for="discount-code">Enter discount code:</label>
                </td>
                <td>
                    <input type="text" id="discount-code" maxlength="7" value="" data-event="<?= $_REQUEST['pro_id'] ?>"/>
                    <input type="button" id="payment-apply-discount" class="small button" value="Apply"/>
                    <input type="hidden" id="buyer_uid" data-uid="<?=$user->uid;?>">
                </td>
            </tr>
        <?php endif; ?>
        <tr>
            <td id="pp-form-container"></td>
            <td>
                <input type="submit" id="payment-submit" class="small button"
                       style="margin-left:0px!important; width: 240px !important;margin-top: 10px;" value="Proceed With Payment"/>
            </td>
        </tr>
    </table>
    <div id="eveditdlg"></div>
</div>
