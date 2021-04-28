<div id="login-dialog" style="display:none;">
    <form action="/" accept-charset="UTF-8" method="post" id="maenna-login-form-home" class="home_login">
        <div align="center" class="reg-top">LOG IN</div>
        <div align="center" class="reg-choose">
            You need an account?
            <span onclick="showFullRegistrationDialog()" style="cursor:pointer;color:#00A2BF; font: 15px Lato Italic;">Register</span>
        </div>
        <div id="reg-form" style="color:#00a1be;">
            <input class="required" type="text" name="email" id="login-email" style="width:280px !important;">
            <input type="password" name="password" id="login-password" style="width:283px !important;"><br><br>
        <span style="margin-left:34px; font-family: 'Lato Regular'; color:grey;" class="rembeber">
            <label><input type="checkbox" name="rembeber">&nbsp;Remember password</label>
            <a style="float:right;margin-right:35px;color:#808080; text-decoration: underline;font-family:'Lato Regular';"
               href="/user/password">Forgot?</a></span>
        </div>
        <input type="hidden" name="form_build_id"
               id="form-c7277b72a9156d39377562276b6a943e"
               value="form-c7277b72a9156d39377562276b6a943e"/>
        <input type="hidden" name="form_id" id="edit-maenna-login-form-home"
               value="maenna_login_form_home"/>
        <input type="hidden" name="page" value="insights">
        <input type="hidden" name="uri" value="<?php echo $_SERVER['REQUEST_URI'];?>">
        <input type="hidden" name="ref" value="<?= $_REQUEST['id'] ?>">
        <input type="hidden" name="ref_pro" value="<?= $row1['postedby'] ?>">
    </form>
</div>
