<?php

include_once( 'open-flash-chart.php' );
srand((double)microtime()*1000000);


$bar_red = new bar_3d( 75, '#D54C78' );
$bar_red->key( '2006', 10 );

// add random height bars:
for( $i=0; $i<10; $i++ )
  $bar_red->data[] = rand(2,5);

//
// create a 2nd set of bars:
//
$bar_blue = new bar_3d( 75, '#3334AD' );
$bar_blue->key( '2007', 10 );

// add random height bars:
for( $i=0; $i<10; $i++ )
  $bar_blue->data[] = rand(5,9);

// create the graph object:
$g = new graph();
$g->title( $_GET['uid'], '{font-size:20px; color: #FFFFFF; margin: 5px; background-color: #505050; padding:5px; padding-left: 20px; padding-right: 20px;}' );

//$g->set_data( $data_1 );
//$g->bar_3D( 75, '#D54C78', '2006', 10 );

//$g->set_data( $data_2 );
//$g->bar_3D( 75, '#3334AD', '2007', 10 );

$g->data_sets[] = $bar_red;
$g->data_sets[] = $bar_blue;

$g->set_x_axis_3d( 12 );
$g->x_axis_colour( '#909090', '#ADB5C7' );
$g->y_axis_colour( '#909090', '#ADB5C7' );

$g->set_x_labels( array( 'January','February','March','April','May','June','July','August','September','October' ) );
$g->set_y_max( 10 );
$g->y_label_steps( 5 );
$g->set_y_legend( 'Open Flash Chart', 12, '#736AFF' );
echo $g->render();
?>