#!/bin/sh

# Abort if any command fails
set -e

# rm -rf build
# mkdir build
# cd build
# cmake ..
# cd pal-expansion
# make
# #cmake ../pal-expansion
# ##cd pal-expansion 
# #make -j5

rm -rf build
mkdir build
cd build
cmake ../pal-expansion
make VERBOSE=TRUE


UF2_FILE=$(find . -name "*.uf2")

# Check if the .uf2 file exists
if [ -z "$UF2_FILE" ]; then
    echo ".uf2 file does not exist. Aborting."
    exit 1
fi

# Check if the COM port exists
if [ -c /dev/ttyACM0 ]; then
    # Set the baud rate to 1200 and other settings for the serial port /dev/ttyACM0
    # This is a "magic connection" that causes the device to go into programming mode
    stty -F /dev/ttyACM0 1200 cs8 -cstopb -parenb || true
    #echo "COM port /dev/ttyACM0 configured."
    echo "Using COM port to reset PICO"
else
    echo "No com port found. Skipping"
    # echo "COM port /dev/ttyACM0 does not exist. Aborting."
    # exit 1
fi

# Check for the mount point to be available
MOUNT_POINT="/media/jjs/RPI-RP2/"
i=0
while [ ! -d "$MOUNT_POINT" ]; do
    sleep 1
    i=$((i+1))
    if [ $i -ge 600 ]; then
        echo "Timeout waiting for $MOUNT_POINT to be mounted. Aborting."
        exit 1
    fi
    echo "Waiting for $MOUNT_POINT to be mounted..."
done



# Attempt to copy the .uf2 file to the mount point with retries
MAX_RETRIES=5
RETRY_INTERVAL=2
retry_count=0
echo "Copying $UF2_FILE to $MOUNT_POINT"
until cp "$UF2_FILE" "$MOUNT_POINT"; do
    retry_count=$((retry_count+1))
    if [ $retry_count -eq $MAX_RETRIES ]; then
        echo "Failed to copy file after $MAX_RETRIES attempts. Aborting."
        exit 1
    fi
    echo "Attempt $retry_count to copy file failed. Retrying in $RETRY_INTERVAL seconds..."
    sleep $RETRY_INTERVAL
done


echo "Operation completed successfully."



