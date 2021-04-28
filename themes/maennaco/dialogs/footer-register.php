<div id="footer-register-dialog" style="display:none;">
    <div align="center" class="reg-top">JOIN CLEWED, IT'S FREE TO START</div>
    <div id="footer-tabs">
<!--        <div align="center" class="tab-title-buttons-caption reg-choose option-label">First, choose your option:</div>-->
        <div class="register-step-1-buttons" style="margin: 0; text-align: center;">
            <div class="register-step-1-section">
                <div class="register-step-1-image"><img src="/themes/maennaco/images/cmp-avatar-service.png"/></div>
                <div class="register-step-1-caption">Get help on your company or project</div>
                <div class="register-step-1-description">Work with a private team of experts<br> to capitalize on your opportunity or project</div>
                <button class="reg-but1" style="padding:0 !important;" id="b-1" onclick="show_hide_new('footer-tabs-1','footer-tabs-2');">
                    Create a company or project account
                </button>
            </div>
            <div class="register-step-1-divider"></div>
            <div class="register-step-1-divider-text">or</div>
            <div class="register-step-1-section">
                <div class="register-step-1-image"><img src="/themes/maennaco/images/Prof_Other_M.jpg"/></div>
                <div class="register-step-1-caption">Join as an individual</div>
                <div class="register-step-1-description">Participate as an expert<br>or join your company's account</div>
                <button class="reg-but2" id="b-2" onclick="show_hide_new('footer-tabs-2','footer-tabs-1');">
                    Create a professional account
                </button>
            </div>
        </div>
        <div id="footer-tabs-1" style="display: none;">
            <div align="center" class="reg-img"><img src="/themes/maennaco/images/reg-comp.png"/></div>
            <div id="reg-form" style="color:#00a1be;">
                <input class="required" type="text" placeholder="First Name" id="company-firstname" />
                <input class="required" type="text" placeholder="Last Name" id="company-lastname" /><br/>
                <input class="required" type="text" placeholder="Company Name" id="company-name"/><br/>
                <input class="required" type="text" placeholder="Email" id="company-email"/><br/>
                <input class="required" type="password" placeholder="Password" id="company-password"/><br/>
                <span class="password-hint">
                    Eight characters minimum, at least one digit
                </span>
                <br/>
                <input class="required" type="text" placeholder="Invitation code (If any)" id="company-referral"><br>
                <select name="company-revenue" id="company-revenue">
                    <option value="revenue">Revenue (Optional)</option>
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
                <span class="agree_terms agree_terms-1">
                <input type="checkbox" id="cmp-agree">
                <label style="font-family:'Lato Bold', sans-serif; font-size:11px; color:#898B8E; margin:15px 0;" for="cmp-agree">I agree with Clewed's
                    <a href="#" class="show_terms" type="terms" style="color:#00a1be; font-family:'Lato Italic', sans-serif;">Terms</a> and
                    <a href="#" class="show_terms" type="privacy" style="color:#00a1be; font-family:'Lato Italic', sans-serif;">Privacy Policy</a>
                </label>
                </span>
                <div></div>
            </div>
        </div>
        <div id="footer-tabs-2" style="display:none;">
            <div align="center" class="reg-img"><img
                    src="/themes/maennaco/images/reg-prof.png"/></div>
            <div id="reg-form" style="color:#00a1be;">
                <select name="pro-type" id="pro-type">
                    <option value="iam">I am&hellip;</option>
                    <?php
                    $ProType = _pro_types();
                    foreach ($ProType as $key => $value) {
                       if ($key == 'client') {
                            continue;
                        } else {
                            echo "<option value=\"$key\">$value</option>";
                        }
                    }
                    ?>
                </select>
                <input class="required" type="text" placeholder="First Name" id="pro-firstname" />
                <input class="required" type="text" placeholder="Last Name" id="pro-lastname" /><br/>
                <input class="required" type="text" placeholder="Primary Email" id="pro-email"><br>
                <input class="required" type="password" placeholder="Password" id="pro-password"><br>
                <span class="password-hint">
                    Eight characters minimum, at least one digit
                </span>
                <br/>
                <input type="text" placeholder="Invitation code (If any)" id="pro-referral"><br>
                <select name="pro-experties" id="pro-experties">
                    <option value="experties">Expertise&hellip;</option>
                    <?php
                    define('__ACCOUNT__', 1);
                    require_once 'themes/maennaco/includes/new_functions.inc';
                    print Options_experties();
                    ?>
                </select>
                <br/>
                <span style="  margin-top: 10px;display: block;height: 5px;" class="agree_terms">
                <label style="padding-left:29px;font-family:'Lato Bold', sans-serif; font-size:11px; color:#898B8E; margin:15px 0;">Add me as a member of my company`s team &nbsp;&nbsp;
                    Yes
                    <input style="" type="checkbox" data-group="cmp-member" id="pro-member" data-box="pro-cmp-email" name="pro-member">&nbsp;&nbsp; No
                    <input data-group="cmp-member" style="" data-box="pro-cmp-email" type="checkbox" id="pro-no-member" name="pro-no-member">
                </label>
                </span><br>
                <input style="display:none;" type="text" placeholder="Enter or verify your company`s email" id="pro-cmp-email"/>
                <span class="agree_terms agree_terms-2">
                <input type="checkbox" id="pro-agree">
                <label style="font-family:'Lato Bold', sans-serif; font-size:11px; color:#898B8E; margin:15px 0;" for="pro-agree">I agree with Clewed's
                    <a href="#" class="show_terms" type="terms" style="color:#00a1be; font-family:'Lato Italic', sans-serif;">Terms</a> and
                    <a href="#" class="show_terms" type="privacy" style="color:#00a1be; font-family:'Lato Italic', sans-serif;">Privacy Policy</a>
                </label>
                </span>
                <br>
                <div></div>
            </div>
        </div>
    </div>
    <div id="policy" style="display:none;"></div>
</div>
