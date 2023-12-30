#!/bin/bash

filesdir=$1
searchstr=$2

if [[ $# -lt 2 ]]; then
	        echo "missing arguments"
		        exit 1
fi

if [[ ! -d $filesdir ]]; then
	        echo "$filesdir is not a directory"
		        exit 1
fi

x=$(ls -1 $filesdir | wc -l)
y=$(grep "$searchstr" $filesdir/* | wc -l)

echo "The number of files are $x and the number of matching lines are $y"
