plot 'deadlayerThickness.txt' using 1:2:3 with yerrorbars
set xlabel 'deadlayer thickness [mm]'
set terminal png
set output 'deadlayerThickness.png'
replot
