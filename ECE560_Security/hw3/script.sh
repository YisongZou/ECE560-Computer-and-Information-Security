#!/bin/bash
echo -n > out.csv
while read NAME;do
    if [[ $NAME =~ ^[0-9]+ ]];then
	host $NAME | awk '{print $5}' | sed 's/.$//' | tr -d '\n' >> out.csv
	echo -n , >> out.csv
	echo -n $NAME, >> out.csv
	sudo scanssh -s ssh $NAME | awk '{print $2 " "$3}' >> out.csv
    else
	echo -n $NAME, >> out.csv
	nslookup $NAME | grep ^Name -A1 | grep ^Address | awk '{print $2}' | tr -d '\n' >> out.csv
	echo -n , >> out.csv
	sudo scanssh -s ssh $NAME | awk '{print $2 " "$3}' >> out.csv
    fi
done < hostList.txt
