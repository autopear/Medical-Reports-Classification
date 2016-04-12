#!/usr/bin/env bash 

# This script renames the .edf files contained in the specified location.
#  The renamed files are named in sequential numbering, starting from 
#  0 to the number of files in the specified location. 
#  

# Define directory that contains the series of directories/files to be renamed
#

direct="/home/tud22978/_projects/CIS5538-Project/test_data/edf";

# For safety, the directory variable was not made a command line argument. 
#  Uncomment the line to make it a command line argument (not recommended).
#
# direct=$1;
i=0;
for dir in $(find $direct -type d)

do
    
    for file in $(find "$dir" -name '*.txt')
    do 
	a=$(printf "$dir_%06d" $i)
	new=${file%/*}
	mv $file $new/$a.txt

	let i=i+1
     done
done
