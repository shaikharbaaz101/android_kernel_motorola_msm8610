#!/bin/bash


make clean && make mrproper
rm -rf ~/MotoE/Paradox-template/boot/zImage-dtb
rm -rf ~/MotoE/Paradox-template/system/lib/modules/*.ko

