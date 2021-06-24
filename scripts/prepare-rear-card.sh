#!/bin/bash

# examples:
# > prepare-rear-card
# > prepare-rear-card 0.6.12
# > prepare-rear-card 0.6.12 /dev/mmcblk0 /dev/mmcblk0p1

# Defaults
DEFAULT_VERSION="0.6.13"
#DEFAULT_CARD_DEVICE="/dev/sda"
#DEFAULT_CARD_PARTITION="/dev/sda1"
DEFAULT_CARD_DEVICE="/dev/mmcblk0"
DEFAULT_CARD_PARTITION="/dev/mmcblk0p1"

########
FIRMWARE_VERSION=${1:-$(DEFAULT_VERSION)}
FIRMWARE_PATH="release/am335x/er-301-v${FIRMWARE_VERSION}-stable.zip"
CARD_DEVICE=${2:-$(DEFAULT_CARD_DEVICE)}
CARD_PARTITION=${3:-$(DEFAULT_CARD_PARTITION)1}
CARD_PATH=`findmnt --source ${CARD_PARTITION} --noheadings --output TARGET`
LABEL="ER301-REAR"

echo "The $CARD_PARTITION is mounted at $CARD_PATH."
echo "Firmware Source: $FIRMWARE_PATH"
echo "Performing filesystem check"
sudo fsck.fat -a ${CARD_PARTITION}
unzip $FIRMWARE_PATH -d $CARD_PATH
sync
echo "Setting volume label to $LABEL."
sudo fatlabel ${CARD_PARTITION} $LABEL
echo "Setting boot flag on partition 1."
sudo parted --script $CARD_DEVICE set 1 boot on
sync
echo "Done."
