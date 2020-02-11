# InuReader

Created by Alex123123, adapted by Rhythm Lunatic for SM

This code essentially supports any Arduino since it's just sending the card data through serial.

Yes the code has keypad support, DO NOT attach a keypad since StepMania doesn't support it.

## Requirements

- Arduino. Pretty much any will work if they have the right pins. If you don't know what to buy then buy a Leonardo. If you only need MFRC522 then buy a Nano.
- MFRC522 Module, HSJ632TR Module, or LMRF 3060 module.

Note: MFRC522 only supports MIFARE, so only Nesica, Aime, Banapass will work. AM Pass and eAmusement pass will not work.

## Setup
### For MFRC522:
| Signal | MFRC522 Reader/<br>PCD Pin | Arduino Leonardo/<br>Micro Pin | Arduino Pro Micro Pin | Arduino Nano |
| ------ | ------------------------ | ---------------------------- | --------------------- | ------------ |
| RST/Reset | RST | RESET/ICSP-5 | RST | RST |
| SPI SS | SDA(SS) | A0 | A0 | A0 |
| SPI MOSI | MOSI | ICSP-4 | 16 | D11 |
| SPI MISO | MISO | ICSP-1 | 14 | D12 |
| SPI SCK | SCK | ICSP-3| 15 | D13 |

### For HSJ632TR and LMRF 3060
Connect the reader module's RX to TX(on Pro Micro) ,TX to RX (on Pro Micro).
Then connect USB cable.
Nothing else.

Then flash the arduino code for your reader, of course.

## Testing
### Linux/Unix:
Compile readerTest3.cpp. Edit source code to point to your serial port.

`g++ readerTest3.cpp -o readerTest3`

Run in terminal, card reader should print to console.

Alternatively you can run `od --endian=big -x < /dev/ttyUSB0` where ttyUSB0 is the correct port and it will print to terminal. Note that it sends 11 bytes at a time and the first four bytes should be ignored ("aaaa").

### Windows:
Coming soon...

## Configuration in StepMania
TODO...

You must implement card reader support in YOUR THEME. The card reader will only trigger CardScannedMessageCommand with params `{player=PlayerNumber,card=cardid}`. PlayerNumber = PlayerNumber enum, cardid = string.

That means you can hide local profiles and create local profiles based on the scanned card, or whatever else you want really. This is mostly for Rave It Out after all, you gotta do it yourself if you want something else.

Use `PROFILEMAN:CreateLocalProfileByID(String name,String id)` to create profiles.

If you want a complete example, start from here:

https://github.com/RhythmLunatic/raveitout/blob/master/Graphics/ScreenAttract%20cardInserted.lua

https://github.com/RhythmLunatic/raveitout/blob/master/BGAnimations/ScreenSelectProfile%20overlay/default.lua#L813-L866
