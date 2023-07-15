# Smart Mirror : Customized Yocto Image

This repo is for HTI-Students graduation project
 
## Table of Contents
1. [ Building the image using Yocto ](#gettingStarted)
	1. [ Setting up environment ](#settingEnvYocto)  
	2. [ Configuring network settings ](#networkSettings)
	3. [ Adding VNC server ](#addingVNC)  
	4. [ Adding Qt ](#addingQt)     
	5. [ Baking and flashing the image ](#baking) 
2. [ Creating UI  ](#creatingUI)  
	1. [ Setting up environment ](#settingEnv)  
	2. [ Configuring the cross compiling and remote deployment settings on Qt creator ](#qtCreatorDeploy)
	3. [ Creating Qt project with C++ ](#qtCreator)  
	4. [ Running multimedia Mp3 and Mp4 ](#multimedia)
3. [ References ](#references)

## General setup  
**Host machine:** Ubuntu 20.04.2 LTS   
**Target machine:** Raspberry Pi 4   
 
---
<a name="gettingStarted"></a>
# Building the image using Yocto

<a name="settingEnvYocto"></a>
## Setting up the environment

1. Download the Poky build system (dunfell branch)  
```
$ git clone -b dunfell git://git.yoctoproject.org/poky
``` 
2. Download RPI BSP (dunfell branch) 
```
$ git clone -b dunfell https://github.com/agherzan/meta-raspberrypi.git 
``` 
3. Download openembedded (dunfell branch)
```
$ git clone -b dunfell https://github.com/openembedded/meta-openembedded.git
```
Note: for my steps, both poky, meta-raspberrypi and meta-openembedded repos are in the same path   
  
4. Source “oe-init-build-env” script 
```
$ source poky/oe-init-build-env rpi-build
```
5. Edit rpi-build/bblayers.conf and add layers to BBLAYERS variable  
```
BBLAYERS ?= " \
/ABSOLUTE/PATH/poky/meta \
/ABSOLUTE/PATH/poky/meta-poky \
/ABSOLUTE/PATH/poky/meta-yocto-bsp \
/ABSOLUTE/PATH/meta-raspberrypi \
/ABSOLUTE/PATH/meta-openembedded/meta-oe \
/ABSOLUTE/PATH/meta-openembedded/meta-python \
/ABSOLUTE/PATH/meta-openembedded/meta-networking \
/ABSOLUTE/PATH/meta-openembedded/meta-multimedia \
"
```  
6. Edit rpi-build/local.conf by changing ```MACHINE ??= "qemux86-64"``` to be ```MACHINE ?= "raspberrypi4-64"```  

7. Edit rpi-build/local.conf and add the following line  
```
LICENSE_FLAGS_WHITELIST_append = " commercial_faad2"
VIRTUAL-RUNTIME_init_manager = "systemd"
DISTRO_FEATURES_append = " systemd"
DISTRO_FEATURES_BACKFILL_CONSIDERED = "sysvinit"
```   
8. For developing you might need rootfs extra space, to add additional space as 5G edit rpi-build/local.conf and add the following line 
```
IMAGE_ROOTFS_EXTRA_SPACE = "5242880"
```

<a name="addingVNC"></a>
## Adding VNC server

Connect to your target through VNC server, then edit rpi-build/local.conf and add x11vnc to  IMAGE_INSTALL_append variable  
```
IMAGE_INSTALL_append = " x11vnc"
```  

<a name="networkSettings"></a>
## Configuring Wifi settings  

Edit rpi-build/local.conf to add some configurations
```
DISTRO_FEATURES_append = " wifi"
IMAGE_INSTALL_append = " kernel-modules"
WIRELESS_DEV = "wlan0"
```

<a name="addingQt"></a>
## Adding Qt 

1. Download the qt5 layer ( dunfell branch)  
```
$ git clone -b dunfell https://github.com/meta-qt5/meta-qt5
``` 
2. Edit rpi-build/bblayers.conf and add the layer to BBLAYERS variable  
```
BBLAYERS ?= " \
....
/ABSOLUTE/PATH/meta-qt5 \
"
``` 
3.  To support Qt5 on image, edit rpi-build/local.conf and add
``` 
IMAGE_INSTALL_append = " make cmake"
IMAGE_INSTALL_append = " qtbase-tools qtbase qtdeclarative qtimageformats qtmultimedia qtquickcontrols2 qtquickcontrols qtbase-plugins cinematicexperience liberation-fonts"
PACKAGECONFIG_FONTS_append_pn-qtbase = " fontconfig"
```  
4. To enable remote deployment on RPI using Qt platform, you need to add extra network configuration to rpi-build/local.conf  
```
IMAGE_INSTALL_append = " openssh-sftp-server rsync"
```

<a name="baking"></a>
## Baking and flashing the image 

1. Build the image using the build engine **BitBake**  
It may take many hours to finish the build process
```
$ bitbake core-image-sato
```  

2. If the build process was successful, the raspberry pi image will be under ```rpi-build/tmp/deploy/images/raspberrypi4-64/core-image-sato-raspberrypi4-64.rpi-sdimg```   

3. Flash the image on the SD card and make sure that it's formatted as free space  
my SD card is /dev/mmcblk0  
```
$ sudo dd if=tmp/deploy/images/raspberrypi4-64/core-image-sato-raspberrypi4-64.rpi-sdimg of=/dev/mmcblk0 status=progress conv=fsync bs=4M
```
4. Now your image is ready, connect RPI with HDMI and explore your sato image  

<p align="center">
  <img  width=30% src="../Media/readme/desktop.png">
</p>

---
<a name="creatingUI"></a>
# Deploy Qt UI   

<a name="settingEnv"></a>
## Setting up environment

1. Install Qt5 Creator command line launcher, my Qt version is 5.9.5  
```
$ sudo apt-get install qtcreator  
```
2. Install Qt5 toolchain for cross compilation. The installation path may differ, just check your terminal output  
```
$ bitbake meta-toolchain-qt5  
$ cd tmp/deploy/sdk
$ ./poky-glibc-x86_64-meta-toolchain-qt5-aarch64-raspberrypi4-64-toolchain-3.0.2.sh 
```   

<a name="qtCreatorDeploy"></a>
## Configuring the cross compiling and remote deployment settings on Qt creator

1. First, you need to source the SDK toolchain. The source path may differ depending on the output of your SDK installation  
```
$ source rpi-build/tmp/deploy/sdk/environment-setup-aarch64-poky-linux  
```  
2. From the same terminal launch qtcreator 
```
$ qtcreator 
```  
3. After Qt creator launches, you need to configure the device from Tools -> Devices  
	1. Add new Generic Linux Device providing the name you want, the hostname/IP address of your device and the username  
	2. Make sure that the device is connected on the same ethernet network to your device and that you did step 4 in [Adding Qt](#addingQt)  
	3. Do the device Test, if anything goes wrong you need the review your network configurations  

4. Then configure your build and run options from Tools -> Build & Run
	1. Qt Versions -> add the path of the SDK qmake  `rpi-build/tmp/deploy/sdk/sysroots/x86_64-pokysdk-linux/usr/bin/qmake` with any name  

	2. Compilers -> add the path of C and C++ compilers `rpi-build/tmp/deploy/sdk/yes/sysroots/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-g++` with any name    

	3. Debuggers -> add the remote debugger path `rpi-build/tmp/deploy/sdk/yes/sysroots/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-gdb`  with any name  

	4. CMake -> it might be auto detected, in case if not add the path `rpi-build/tmp/deploy/sdk/yes/sysroots/x86_64-pokysdk-linux/usr/bin/cmake` 

	5. Kits -> add the previous setups with:  
		**Device type** -> Generic Linux Device  
		**sysroot** -> path `rpi-build/tmp/deploy/sdk/yes/sysroots/aarch64-poky-linux`  
		**QT mkspec** -> linux-oe-g++    


<a name="qtCreator"></a>
## Creating Qt project with C++

1. Create new project as **Application** -> Qt Widgets Application  
2. When it comes to **Kit Selection** choose your pc and your device, if your pc is not listed add it from devices window  
3. Following the steps will create main.cpp which is the whole project main function, source/header and ui files for your main class and .pro file which configure your project  
4. The pro file is updated automatically on each file creation/deletion. You will only need to add the remote executable path where your application will be deployed in your image as  
```
	target.path = /home/root/app
	INSTALLS += target
```  
5. Executing system commands may require root permissions on pc such as mount, mkdir, date.. etc. You can launch your application as sudoer to avoid this problem in testing your application ```sudo qtcreator```    
6. In my Qt project there is a configuration file `targetconfiguration.h` to specify on each target the application is going to run because of some commands that don't run on both  

---
# Refernces
<a name="references"></a>
Finally, we would love to recommend to you and show some apperciation for the following online guides. 
Their insights were incredibly helpful in finishing our project. 
1. [Infotainment System](https://github.com/alzahraaelsallakh/Infotainment-System-Yocto)
2. [Deploy QT application on Raspberry Pi ](https://youtu.be/LU6pLW_S1wg) 
