#!/bin/sh

rm -rf build
mkdir build
cd build
cmake ../pal-expansion
cd pal-expansion 
make  -j5

# Set the baud rate to 1200 and other settings for the serial port /dev/ttyS0
# This is a "magic connection" that causes the device to go into programming mode
# https://github.com/raspberrypi/pico-sdk/pull/197

# can be disabled by adding 
# add_compile_definitions( PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE=0 ) to CMakeLists.txt.

stty -F /dev/ttyACM0 1200 cs8 -cstopb -parenb

# This is the Window command to do the same thing:
# Mode Com14: Baud=1200 Parity=N Data=8 Stop=1

sleep 5

cp *.uf2 /media/jjs/RPI-RP2/

#sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program pico_rom.elf verify reset exit"
