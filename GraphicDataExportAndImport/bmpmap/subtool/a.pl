for (1..200) {
	my $a = $_*8;
	`BitMapExporter hexgrp.002 tmp$_.bmp 0 $a 64`;
	print($a, "\n");
}