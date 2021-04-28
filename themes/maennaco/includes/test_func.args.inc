<?php
$numargs = func_num_args();
echo "Number of arguments: $numargs<br />\n";
for($i = 0; $i<= $numargs-1; $i++)
{
    echo "$i argument is: " . func_get_arg($i) . "<br />\n";
}

/* EOF */