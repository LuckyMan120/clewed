<script type="text/javascript">
  $('document').ready(function(){
    $('.filterbg select').change(function(){
      window.location = $(this).val();
    });
  });
</script>

<?php
 $categories = mysql_query("SELECT * FROM maenna_professional group by tags") or die(mysql_error());
?>

<div class="filters" style="padding-left: 270px;margin: -17px 0 -10px 0; width:77.7%;">
  <div class="filterbg ind">
    <select name="categories" id="indSelect">
      <?php $sortmonth = ((isset($_GET['sortmonth']))?"&sortmonth=".$_GET['sortmonth']:"") ;?>
      <option value="<?=$base_url?>/account?tab=insights<?=$sortmonth?>">Categories</option>
      <?php 
      while($resCategories = mysql_fetch_array($categories)) {
          if($resCategories['tags'] == 'Choose a Category') continue; 
          $selected = (($_GET['sort'] == $resCategories['tags'])?"selected=\"selected\"":"");
      ?>
          <option value="<?=$base_url?>/account?tab=insights&sort=<?=$resCategories['tags']?><?=$sortmonth?>"
              <?=$selected?>>
              <?=$resCategories['tags']?>
          </option>
      <?php } ?>

    </select>
  </div>
  <div class="filterbg rev">
    <select name="month" id="revSelect">
      <?php $sortcat = ((isset($_GET['sort']))?"&sort=".$_GET['sort']:"") ;?>
      <option value="<?=$base_url?>/account?tab=insights<?=$sortcat?>">Month</option>
      <?php 
      $months = array('01' => 'January', '02' => 'February', '03' => 'March', 
          '04' => 'April', '05' => 'May', '06' => 'June', '07' => 'July', 
          '08' => 'August', '09' => 'September', '10' => 'October', 
          '11' => 'November', '12' => 'December');
      ?>
      <?php foreach($months as $key=> $value): ?>
      <?php $selected = (($_GET['sortmonth'] == $key)?"selected=\"selected\"":""); ?>
      <option value="<?=$base_url?>/account?tab=insights&sortmonth=<?=$key?><?=$sortcat?>"
            <?=$selected?>>
        <?=$value?>
      </option>
      <?php endforeach; ?>
    </select>
  </div>
</div>