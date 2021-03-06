StepAMWorks
=========

This is a fork of StepMania with various features.

## Features
* OptionsList implements more features that the regular options menu supports.
* ~~Update checker~~ Eventually
* ~~Windows PIUIO ported from OpenITG (thx to pk for the libUSB shit)~~ It doesn't work
* ~~BedrockSolid's PIUIO2Key for windows lights which may or may not work, I don't know, I don't have a PIUIO~~ It's not even in the CMakeLists right now
* PacDrive lights for both Windows and Linux.
* ^ Including the correct mappings for pump mode
* Barely functional co-op x4 that happens to be very buggy
* Support StepF2's note visibility and fake attributes.
* Bonus note for modcharts/missions that does not affect life (Check the wiki)
* All level, all difficulty sort
* Ability to show SortOrders next to folders if you are using the built in group select. Check MusicWheel metrics for more information.
* Hide charts with #SELECTABLE:EASY when not in Easy Mode.
* ~~Card Reader support~~ Need to redo, doesn't flush the buffer correctly right now and only works on Linux
* Windows XP support, since this is based on chrispable's "starworlds" StepMania fork.

## Compiling

Linux users, do this because it has libUSB: `apt-get install libusb-dev libusb-1.0.0 libusb-0.1-4`

Windows users, you need to use Visual Studio 2013 build tools if you want Windows XP support.

Then follow the rest like normal: https://github.com/stepmania/stepmania/wiki/Compiling-StepMania

## Linux PacDrive
In preferences.ini: `LightsDriver=Linux_PacDrive`

Check product ID with lsusb. Replace idProduct with the Product ID of your PacDrive and the OWNER="raveitout" with the name of your linux account. Then run this command.
`sudo echo SUBSYSTEMS=="usb", ATTRS{idVendor}=="d209", ATTRS{idProduct}=="1500", SYMLINK+="pacdrive", OWNER="raveitout" > /etc/udev/rules.d/90-pacdrive.rules`

## Debugging Lights
In preferences.ini: `DebugLights=1`, `LightsDriver=Broadcast`.

Use the RIO theme to show the lights debug.

## Resources

Check the wiki.

## Licensing, legal stuff

StepAMW is GPLv3.

For specific information/legalese:
* Arduino card reader code written by Alex123123, used with permission.
* The original MIT licensed code for StepAMW, if you would like to use it, is before this commit https://github.com/RhythmLunatic/stepmania/commit/476ecaed98b47ba4c9b12ee22c6f0d4de0434af3
* The wiki is [CC BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0/).
* The [MAD library](http://www.underbit.com/products/mad/) and [FFmpeg codecs](https://www.ffmpeg.org/) when built with our code use the [GPL license](http://www.gnu.org).

