StepMania-RaveItOut
=========

This is a custom version of StepMania with various features. The theme is available [here](https://github.com/raveitoutofficial/raveitout)

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
* ~~Card Reader support~~ Need to redo, doesn't flush the buffer correctly right now and only works on Linux

## Compiling

Do this because it has libUSB: `apt-get install libusb-dev libusb-1.0.0 libusb-0.1-4`

Then follow the rest like normal: https://github.com/stepmania/stepmania/wiki/Compiling-StepMania

## Linux PacDrive
In preferences.ini: `LightsDriver=Linux_PacDrive`

Check product ID with lsusb. Replace idProduct with the Product ID of your PacDrive and the OWNER="raveitout" with the name of your linux account. Then run this command.
`sudo echo SUBSYSTEMS=="usb", ATTRS{idVendor}=="d209", ATTRS{idProduct}=="1500", SYMLINK+="pacdrive", OWNER="raveitout" > /etc/udev/rules.d/90-pacdrive.rules`

## Resources

Hit us up on Facebook!

The wiki has information about some SM-RIO features and theming help.

## Licensing, legal stuff

Rave It Out is 100% open source. There is no encryption. There are no binary blobs. What you get here is what we use to test.

For specific information/legalese:

* Arduino card reader code written by Alex123123
* All of the StepMania source code is under the [MIT license](http://opensource.org/licenses/MIT).
* The wiki is [CC BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0/).
* The [MAD library](http://www.underbit.com/products/mad/) and [FFmpeg codecs](https://www.ffmpeg.org/) when built with our code use the [GPL license](http://www.gnu.org).

