# pal1-ram-rom
Configurable RAM ROM for the PAL-1 (kim-1 clone)

This project is still under development to replace the RAM/ROM and IO expansion board with a Pi Pico.


Current state:  The prototype worked after some manual board rework.

Trying to take over the DEN line via software so all ICs except the Pi Pico can be removed from the board.



KIM-1 Memory Map:

K0 $0000 - $03FF 1024 bytes of RAM (8*6102)
K1 $0400 - $07FF free
K2 $0800 - $0BFF free
K3 $0C00 - $0FFF free
K4 $1000 - $13FF free
K5 $1400 - $16FF free
$1700 - $173F I/O, timer of 6530-003
$1740 - $177F I/O, timer of 6530-002
$1780 - $17BF 64 bytes RAM of 6530-003
$17C0 - $17FF 64 bytes RAM of 6530-002
K6 $1800 - $1BFF 1024 bytes ROM of 6530-003
K7 $1C00 - $1FFF 1024 bytes ROM of 6530-002
