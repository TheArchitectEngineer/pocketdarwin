<div align="center">
  <img src="logo.png" alt="PocketDarwin Logo" width="240" />
  <h1>PocketDarwin</h1>
  <p>I'm the crazy guy who's gonna clone iOS! — OakyMacintosh</p>
</div>

## What This Repo Is
PocketDarwin is an Darwin port for Android hardware.

>[!NEW] Discord server now available on https://discord.gg/bthwXwh8vP

## Installation instructions

>[!NOTE] PocketDarwin will never ships pre-built linux interludes!! Only system bundles are released.

```sh
# build the xnu kernel
$ cd System/darwinKernel/xnu-arm
$ make TARGET_CONFIGS="debug arm ARMPBA8"
# build the interlude linux kernel
# you will need to get your device's android linux kernel.
# for example: android_kernel_moto_msm9816
$ cd System/Boot/linuxTemp/<the device kernel dir>
$ cp ../configs/<device codename>/.config .config
$ ARCH="arm" make
# create the System bundle
$ cd System/SystemBundle
# for macOS
$ make darwinApp
# for linux
$ make linuxapp
$ ./dist/pdbundle -kernel ../darwinKernel/xnu-arm/BUILD/DEBUG_ARM/kernel -sysroot $(./pdbundle -makesysroot -s 30G) -relname PocketDarwin.System -buildsys -buildframeworks="../../Frameworks"
```

## Supported devices
- harpia (Moto G4 Play, 2016, Linux kernel 3.10) **official development device**
- Any android device thats aarch64 and has the linux kernel source tree publicaly available
