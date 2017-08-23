#!/bin/bash


make clean && make mrproper
rm -rf ~/MotoE/Paradox-anykernel/boot/zImage-dtb
rm -rf ~/MotoE/Paradox-anykernel/system/lib/modules/*.ko

