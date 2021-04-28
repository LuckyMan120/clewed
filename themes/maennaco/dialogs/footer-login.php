<div id="footer-login-dialog" style="display:none;">
    <form action="/" accept-charset="UTF-8" method="post" id="maenna-login-form-home" class="home_login">
        <div align="center" class="reg-top">LOG IN</div>
        <div align="center" class="reg-choose">You need an account?
            <span onclick="showRegDlg()" style="cursor:pointer;color:#00A2BF; font: 15px Lato Italic;">Register</span>
        </div>
        <div id="reg-form" style="color:#00a1be;">
            <input type="text" name="email" id="login-email" class="required login-email-response"  >
            <input type="password" name="password" id="login-password"  maxlength="30"
                   size="60" class="required register-password-response" ><br><br>
            <input type="hidden" name="page" value="companies">
            <input type="hidden" name="uri" value="<?php echo $_SERVER['REQUEST_URI'];?>">
            <input type="hidden" name="ref" value="">
        <span style="margin-left:34px; font-family: 'Lato Regular', sans-serif; color:grey;" class="rembeber">
			<label><input type="checkbox" name="rembeber">&nbsp;Remember password</label>
            <a style="float:right;margin-right:35px;color:#808080; text-decoration: underline;font-family:'Lato Regular', sans-serif;" href="/user/password">Forgot?</a></span>
        </div>
        <input type="hidden" name="form_build_id" id="form-c7277b72a9156d39377562276b6a943e" value="form-c7277b72a9156d39377562276b6a943e"/>
        <input type="hidden" name="form_id" id="edit-maenna-login-form-home" value="maenna_login_form_home"/>
    </form>
</div>