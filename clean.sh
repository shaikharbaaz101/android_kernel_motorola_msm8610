#!/bin/bash


make clean && make mrproper
rm -rf ~/motoe/Paradox-anykernel/boot/zImage-dtb
rm -rf ~/motoe/Paradox-anykernel/system/lib/modules/*.ko

