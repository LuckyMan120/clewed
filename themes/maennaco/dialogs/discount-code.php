<div id="discount-code-container" style="display: none">
    <form style="text-align: center;">
        <div style="width: 100%; padding: 10px;">
            <input id="discount-code-value" data-insight="<?= $insight['id'] ?>" data-uid="<?= $userId ?>" type="text" name="code" style="width: 90px !important; margin: 0;"/>
            <button id="check-discount-code" style="background-color: #00a1be; border-radius: 4px; color: white; font-size: 14px;padding: 4px 8px;">Apply</button>
            <div style="color: red;" id="discount-code-error"></div>
            <div style="color: green;" id="discount-code-ok"></div>
        </div>
    </form>
</div>
