<style type="text/css">
    #register-dialog .password-hint {
        margin-left: 30px;
        font-size: 11px;
        color: #0a2bff;
        font-family: 'Lato Italic', sans-serif;
    }
    #pro-agree, #cmp-agree {
        margin-left: 30px;
        margin-top: 7px;
    }
</style>
<div id="register-dialog" style="display:none;">
    <div align="center" class="reg-top">JOIN CLEWED, IT'S FREE TO START</div>
    <div id="tabs">
        <div align="center" class="reg-choose option-label">First, choose your option:</div>
        <div style="margin: 0; text-align: center;">
            <input type="button" class="reg-but1" id="b-1"
                   onclick="show_hide('tabs-1','tabs-2','b-1','b-2');"
                   value="I am a Company1"/>
            <div class="reg-choose-btn-divider">or</div>
            <input type="button" class="reg-but2" id="b-2"
                   onclick="show_hide('tabs-2','tabs-1','b-2','b-1');"
                   value="I am a Professional"/>
        </div>
        <div id="tabs-1" style="display: block;">
            <div align="center" class="reg-img"><img src="/themes/maennaco/images/reg-comp.png"/></div>
            <div id="reg-form" style="color:#00a1be;">
                <input class="required" type="text" id="company-firstname" style="width: 131px !important;"/>
                <input class="required" type="text" id="company-lastname"  style="width:131px !important; margin-left:-7px !important;"/><br/>
                <input class="required" type="text" id="company-name"/><br/>
                <input class="required" type="text" id="company-email"/><br/>
                <input class="required" type="password" id="company-password"/><br/>
                <span class="password-hint">
                    Eight characters minimum, at least one digit
                </span>
                <br/>
                <select name="company-revenue" id="company-revenue">
                    <option value="revenue">Revenue less than</option>
                    <?php
                    $Revenus = RevenuesData();
                    foreach ($Revenus as $key => $val) {
                        echo "\n<option value='$key'>$val</option>";
                    }
                    ?>
                </select>
                <select name="company-industry" id="company-industry">
                    <option value="industry">Industry</option>
                    <?php
                    $Sectors = _INDUSTRY();
                    foreach ($Sectors as $key => $value) {
                        echo "<optgroup label=\"$key\">";
                        foreach ($value as $key1 => $value1) echo "<option value=\"$key1\">$value1</option>";
                        echo '</optgroup>';
                    }
                    ?>
                </select>
                <br/>
                <input type="checkbox" id="cmp-agree">
                <label style="font-family:'Lato Bold', sans-serif; font-size:11px; color:#898B8E; margin:15px 0;"
                       for="cmp-agree">
                    I agree with
                    Clewed's
                    <a class="show_terms" type="terms" target="_blank"
                       style="color:#00a1be; font-family:'Lato Italic', sans-serif;">
                        Terms
                    </a>
                    and
                    <a target="_blank" class="show_terms" type="privacy"
                       style="color:#00a1be; font-family:'Lato Italic', sans-serif;">
                        Privacy Policy
                    </a>
                </label>
                <div></div>
            </div>
        </div>
        <div id="tabs-2" style="display:none;">
            <div align="center" class="reg-img"><img
                    src="/themes/maennaco/images/reg-prof.png"/></div>
            <div id="reg-form" style="color:#00a1be;">
                <select name="pro-type" id="pro-type">
                    <option value="iam">I am&hellip;</option>
                    <?php
                    $ProType = _pro_types();
                    foreach ($ProType as $key => $value) {
                        if ($key == 'investor') {
                            echo "<option disabled=\"disabled\" value=\"$key\">$value (coming soon)</option>";
                        } else {
                            echo "<option value=\"$key\">$value</option>";
                        }
                    }
                    ?>
                </select>
                <input class="required" type="text" id="pro-firstname"><br>
                <input class="required" type="text" id="pro-lastname"><br>
                <input class="required" type="text" id="pro-email"><br>
                <input class="required" type="password" id="pro-password"><br>
                <span class="password-hint">
                    Eight characters minimum, at least one digit
                </span>
                <br/>
                <select name="pro-experties" id="pro-experties">
                    <option value="experties">Expertise&hellip;</option>
                    <?php
                    define('__ACCOUNT__', 1);
                    require_once 'themes/maennaco/includes/new_functions.inc';
                    print Options_experties();
                    ?>
                </select>
                <br/>
                <span class="agree_terms">
                <input type="checkbox" id="pro-agree">
                <label style="font-family:'Lato Bold', sans-serif; font-size:11px; color:#898B8E; margin:15px 0;"
                       for="cmp-agree">I agree with Clewed's
                    <a class="show_terms" type="terms" target="_blank"
                       style="color:#00a1be; font-family:'Lato Italic', sans-serif;">Terms</a> and <a target="_blank"
                                                                                          class="show_terms"
                                                                                          type="privacy"
                                                                                          style="color:#00a1be; font-family:'Lato Italic', sans-serif;">Privacy
                                                                                                                                            Policy</a></label>
                </span>
                <br>
                <div></div>
            </div>
        </div>
    </div>
</div>
