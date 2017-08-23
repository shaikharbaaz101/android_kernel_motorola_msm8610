#!/bin/bash
echo " Lets make another Paradox "
cd ~/MotoE
export ARCH=arm
echo " Initializing ............. "
export CROSS_COMPILE=~/arm-eabi-4.9/bin/arm-eabi-
make paradox_condor_defconfig
echo " Getting ready........ "
export KBUILD_BUILD_USER=Akshay7273
export KBUILD_BUILD_HOST=JARVIS
export CONFIG_NO_ERROR_ON_MISMATCH=y
echo " Building for device condor .......... "

rm -rf ~/MotoE/Paradox-anykernel/zImage-dtb
rm -rf ~/MotoE/Paradox-anykernel/modules/*.ko
rm -rf ~/MotoE/arch/arm/boot/zImage
echo " Starting build ..........."
make all -j5 zImage-dtb
echo " Kernel is successfully compiled ......"
mv ~/MotoE/arch/arm/boot/zImage-dtb ~/MotoE/Paradox-anykernel/zImage-dtb

find -name "*.ko" -exec cp {} ~/MotoE/Paradox-anykernel/modules \;
cd ~/MotoE/Paradox-anykernel
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
