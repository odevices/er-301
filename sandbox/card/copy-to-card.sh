#!/bin/bash

CARD_DEVICE="/dev/mmcblk0"
CARD_PATH=`findmnt --source ${CARD_DEVICE}p1 --noheadings --output TARGET`
echo "The $CARD_DEVICE is mounted at $CARD_PATH."

FILE=$1
echo "Copying $FILE to $CARD_PATH"
cp "$FILE" $CARD_PATH
sync