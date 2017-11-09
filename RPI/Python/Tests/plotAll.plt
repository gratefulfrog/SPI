#!/usr/bin/gnuplot
# plotAll.plt
# tell it that it is a comma separated file
set datafile separator ","
# tell it that there is a header row
set key autotitle columnhead
# set the output to png format
set term png
# enable time stamping
set timestamp
# list <- all the csv filenames in the current directory
list = system("ls -1 *.csv")
# create a counter
#i = 1
# iterate over the list of filenames
do for [file in list] {
   # set the output file name to the input filename in the plot directory with
   # .png appended
   set output sprintf('plot/%s.png', file)
   # create a title for the plot
   set title sprintf("%s", file) noenhanced
   # plot it, to the output as above
   plot file
   # inc the counter
   #i = i + 1
}
