#!/bin/bash

CARD_DEVICE="/dev/mmcblk0"
CARD_PATH=`findmnt --source ${CARD_DEVICE}p1 --noheadings --output TARGET`
echo "The $CARD_DEVICE is mounted at $CARD_PATH."

MLO_PATH="/home/clarkson/projects/er-301/bin/pbl/release/MLO"
echo "Copying MLO"
cp "$MLO_PATH" $CARD_PATH

SBL_PATH="/home/clarkson/projects/er-301/bin/sbl/release/SBL"
echo "Copying MLO"
cp "$SBL_PATH" $CARD_PATH

KERNEL_PATH="/home/clarkson/projects/er-301/bin/firmware/release/kernel.bin"
echo "Copying kernel.bin"
cp "$KERNEL_PATH" $CARD_PATH

echo "Setting boot flag on partition 1."
sudo parted --script $CARD_DEVICE set 1 boot on
sync