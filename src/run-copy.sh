#!/bin/sh

rm -rf build
mkdir build
cd build
cmake ..
cd picoROM
make
cp *.uf2 /media/jjs/RPI-RP2/

sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program pico_rom.elf verify reset exit"
