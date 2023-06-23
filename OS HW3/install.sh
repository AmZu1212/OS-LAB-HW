#!/bin/bash
make
insmod ./repeated.o
mknod /dev/repeated c 254 0
clear
echo installing done.
