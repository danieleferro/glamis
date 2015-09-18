#!/bin/bash

if [ "$1" = "on" ]; then
	AT_ID_FILE=/tmp/glamis_on_id

elif [ "$1" = "off" ]; then
	AT_ID_FILE=/tmp/glamis_off_id
else
	echo "Usage: $0 [on|off]"
	exit 1
fi	

if [ ! -f ${AT_ID_FILE} ]; then
        exit 1
fi


ID=$(cat ${AT_ID_FILE} 2>/dev/null | grep job | awk '{print $2 }')

echo "Removing at task with id" $ID

atrm $ID

rm -f ${AT_ID_FILE}

exit 0

