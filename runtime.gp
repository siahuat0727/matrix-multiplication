reset
set style fill solid
set style data histogram
# set style func linespoints
set term png enhanced font 'Verdana,10'
set output 'runtime.png'
# set xlabel 'stride'
set ylabel 'time ( s )'
# set ylabel 'count'
set title 'Matrix multiplication algorithms'
set xtics rotate by -20
# set grid xtics

plot [:][0:]'runtime.txt' using 2:xtic(1) title 'runtime', \
'' using ($0):($2+0.1):2 with labels title ' '

# plot [:][0:]'results.txt' using 2:xtic(1) with line title 'cache-misses', \
# '' using 5:xtic(1) with line title 'branch-misses'
