StepMania-RaveItOut
=========

This is a custom version of StepMania with various features. The theme is available [here](https://github.com/raveitoutofficial/raveitout)

Features:
* OptionsList implements more features that the regular options menu supports.
* Update checker
* Windows PIUIO ported from OpenITG (thx to pk for the libUSB shit)
* PacDrive lights because it's been a year and it still hasn't been merged

## Compiling

Do this because it has libUSB: `apt-get install libusb-dev libusb-1.0.0 libusb-0.1-4`

Then follow the rest like normal: https://github.com/stepmania/stepmania/wiki/Compiling-StepMania

## Linux PacDrive
In preferences.ini: `LightsDriver=Linux_PacDrive`

Check product ID with lsusb. Replace idProduct with the Product ID of your PacDrive and the OWNER="raveitout" with the name of your linux account. Then run this command.
`sudo echo SUBSYSTEMS=="usb", ATTRS{idVendor}=="d209", ATTRS{idProduct}=="1500", SYMLINK+="pacdrive", OWNER="raveitout" > /etc/udev/rules.d/90-pacdrive.rules`

## Resources

Hit us up on Facebook!

## Licensing, legal stuff

Rave It Out is 99% open source. The other 1% is because we need to encrypt songs, and thus the .dll/.so for encryption is closed source.

To follow the StepMania license we do not provide builds with mp3 support.

For specific information/legalese:

* All of the StepMania source code is under the [MIT license](http://opensource.org/licenses/MIT).
* The [MAD library](http://www.underbit.com/products/mad/) and [FFmpeg codecs](https://www.ffmpeg.org/) when built with our code use the [GPL license](http://www.gnu.org).

