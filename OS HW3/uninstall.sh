#!/bin/bash
rm -f /dev/repeated
rmmod repeated
make clean
dmesg -c
clear
echo uninstalling done.
