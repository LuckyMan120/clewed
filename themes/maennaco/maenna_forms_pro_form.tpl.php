<table cellspacing="0">
	<tr>
    	<td class="container-inline"><strong><?php print t('Search');?></strong> <?php print drupal_render($form['pro_type']); ?></td>
        <td class="container-inline"><strong><?php print t('Category');?></strong> <?php print drupal_render($form['first_name']); ?></td>
        <td><?php print drupal_render($form['submit']); ?></td>
    </tr>
</table>
<?php
print drupal_render($form);
?>