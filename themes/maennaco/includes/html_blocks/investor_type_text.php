<script>
	$(document).ready(function() {
		$('#get-help-investor-type-btn').click(function (e) {
			e.preventDefault();
			$('#get-help-investor-type-dialog').dialog({
				modal: true,
				autoOpen: true,
				resizable: false,
				title: 'Investor Type',
				width: 560,
			});
		});
	});
</script>
<div id="get-help-investor-type-dialog" style="display:none">
	<h4>Accredited Investor:</h4>
	<p>Individuals who have earned $200,000&nbsp;or more in&nbsp;income over the past two years automatically qualify as&nbsp;an&nbsp;accredited investor, as&nbsp;does a&nbsp;person whose income&nbsp;— when combined with a&nbsp;spouse’s&nbsp;— totals $300,000&nbsp;or more or</p>
	<p>An&nbsp;individual can also maintain a&nbsp;net worth of&nbsp;$1&nbsp;million or&nbsp;more, minus the value of&nbsp;a&nbsp;primary residence.</p>
	<h4>Unaccredited Investor:</h4>
	<p>Any investor who does not meet the income or&nbsp;net worth requirements above is&nbsp;unaccredited investor. Unaccredited investor can invest as&nbsp;follows:</p>
	<ol>
		<li>For crowd funding, If&nbsp;you make less than $100,000 per year or&nbsp;your net worth is&nbsp;below that amount, you can invest up&nbsp;to&nbsp;either the greater of&nbsp;a) $2,000&nbsp;or b) the lesser of&nbsp;5% of&nbsp;your income or&nbsp;net worth.</li>
		<li>If&nbsp;your annual income and your net worth exceed $100,000, you can invest up&nbsp;to&nbsp;10% of&nbsp;your income or&nbsp;net worth, whichever is&nbsp;less, up&nbsp;to&nbsp;a&nbsp;total limit of&nbsp;$100,000.</li>
	</ol>
</div>
