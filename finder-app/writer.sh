#!/bin/bash

writefile=$1
writestr=$2

if [[ $# -lt 2 ]]; then
	        echo "missing arguments"
		        exit 1
fi

dirname=$(dirname $writefile)
filename=$(basename $writefile)

if [[ "$dirname" != "$filename" ]]; then
	        if [[ ! -d $dirname ]]; then
			                mkdir -p $dirname
					        fi
fi

echo $writestr > $writefile

if [[ $? -ne 0 ]]; then
	        echo "File could not be created"
fi
