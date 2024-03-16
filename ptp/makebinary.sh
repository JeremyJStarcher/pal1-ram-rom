#!/bin/bash

# Using printf to directly print bytes into a file
printf "\x00\x01\x02\x03\x04" > binaryfile.bin
