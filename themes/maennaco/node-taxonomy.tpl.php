<?php
// $Id: node.tpl.php,v 1.5 2007/10/11 09:51:29 goba Exp $
?>
<div id="node-<?php print $node->nid; ?>" class="node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?>">

<?php  print $picture ?>

<?php if ($page == 0): ?>
  <h2><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
<?php endif; ?>
<?php if ($page > 0): ?>
  <h2 style='color:#6792d0'><?php print $title ?></h2>
<?php endif; ?>

  <?php if ($submitted): ?>
    <span class="submitted">Posted on <?php print date("M, d Y",$node->created); ?></span>
  <?php endif; ?>

  <div class="content clear-block">
    <?php print $content ?>
  </div>

  <div class="clear-block">
    <div class="meta">
    <?php if (($taxonomy) && 0){ ?>
      <div class="terms"><?php print $terms ?></div>
    <?php } ?>
    </div>

    <?php if (($links) && 0) { ?>
      <div class="links"><?php print $links; ?></div>
    <?php } ?>
  </div>

  <?php if ($page > 0): ?>
    <br>
    <a href='/blog'><u>back</u></a>
  <?php endif; ?>
</div>
<?php if ($page == 0): ?>
<div class="divider" style='height:4px;'></div>
<?php endif; ?>
