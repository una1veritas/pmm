#!/bin/sh

echo "starting tests..."

for wc in 10000 15000 25000 40000 65000 100000 150000 250000 400000 
do 
    for wshuf in 1 2 3 4 5
    do 
	unzip -c words.txt.zip | gshuf > words_shuf.txt
	for tries in 1 2 3 4
	do 
	    echo "$wc words in shuffle $wshuf trial $tries \c"
    	    cat words_shuf.txt | ./acm.exe -n $wc -p - 
	done
    done
done
echo "done."