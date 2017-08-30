#!/bin/bash
echo " Lets make another Paradox "
cd ~/MotoE
echo " Initializing ............. "
make paradox_condor_defconfig
echo " Getting ready........ "
export KBUILD_BUILD_USER=Akshay7273
export KBUILD_BUILD_HOST=JARVIS
echo " Building for device condor .......... "

rm -rf ~/MotoE/Paradox-template/boot/zImage-dtb
rm -rf ~/MotoE/Paradox-anykernel/system/lib/modules/*.ko
rm -rf ~/MotoE/arch/arm/boot/zImage
echo " Starting build ..........."
make -j5 zImage-dtb
make modules
echo " Kernel is successfully compiled ......"
mv ~/MotoE/arch/arm/boot/zImage-dtb ~/MotoE/Paradox-template/boot/zImage-dtb

find -name "*.ko" -exec cp {} ~/MotoE/Paradox-template/system/lib/modules \;
cd ~/MotoE/Paradox-template
zip -r Paradox-R-$(date +'20%y%m%d').zip *

echo " Kernel is successully packed !! "
echo " Name of Package : Paradox-R-$(date +'20%y%m%d').zip "
echo " Flashable zip is present in Paradox-template folder"
echo " #####Enjoy##### "

