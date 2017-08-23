#!/bin/bash
echo " Lets make another Paradox "

export ARCH=arm
echo " Initializing ............. "
export CROSS_COMPILE=~/arm-eabi-4.9/bin/arm-eabi-
make paradox_condor_defconfig
echo " Getting ready........ "
export KBUILD_BUILD_USER=Akshay7273
export KBUILD_BUILD_HOST=JARVIS
export CONFIG_NO_ERROR_ON_MISMATCH=y
echo " Building for device condor .......... "

rm -rf Paradox-anykernel/zImage-dtb
rm -rf Paradox-anykernel/modules/*.ko
rm -rf arch/arm/boot/zImage
echo " Starting build ..........."
make -j5 zImage-dtb
echo " Kernel is successfully compiled ......"
mv arch/arm/boot/zImage-dtb Paradox-anykernel/zImage-dtb

find -name "*.ko" -exec cp {} /Paradox-anykernel/modules \;
cd /Paradox-anykernel
zip -r9 Paradox-R-$(date +'20%y%m%d').zip * -x README Paradox.zip

echo " Kernel is successully packed !! "
echo " Name of Package : Paradox-R-$(date +'20%y%m%d').zip "
echo " Flashable zip is present in Paradox-anykernel folder"
echo " #####Enjoy##### "
echo " ========================================================================================================"
echo "                                                                                                         "
echo "                   ____ ____ ____ ____ ____ ____ ____ _________ ____ ____ ____ ____ ____ ____            "
echo "                  ||P |||A |||R |||A |||D |||O |||X |||       |||K |||E |||R |||N |||E |||L ||           "
echo "                  ||__|||__|||__|||__|||__|||__|||__|||_______|||__|||__|||__|||__|||__|||__||           "
echo "                  |/__\|/__\|/__\|/__\|/__\|/__\|/__\|/_______\|/__\|/__\|/__\|/__\|/__\|/__\|           "
echo "                                                                                                         "
echo "---------------------------------------------------------------------------------------------------------"
echo "                                      Developed  By - Akshay Kumar                                     "
echo " ========================================================================================================"
