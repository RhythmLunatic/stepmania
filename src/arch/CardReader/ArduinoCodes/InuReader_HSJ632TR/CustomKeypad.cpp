#include "CustomKeypad.h"
#include "Keypad.h"
#include "Key.h"
#include "Keyboard.h"

#define CUSTOM_KEY1 'A'
#define CUSTOM_KEY2 'B'

const byte ROWS = 4; 
const byte COLS = 3; 
//define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'7','8','9'},
  {'4','5','6'},
  {'1','2','3'},
  {'0',CUSTOM_KEY1,CUSTOM_KEY2}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


void CustomKeypadClass::DetactInput()
{
	char customKey = customKeypad.getKey();
  
    if (customKey){
		
		//add 100ms delay is enough
		//if you're a perfectionist ,edit these coed to EventKeypad
		if(customKey==CUSTOM_KEY1)
		{
		  Keyboard.press('0');
		  delay(100);
		  Keyboard.release('0');
		  delay(10);
		  Keyboard.press('0');
		  delay(100);
		  Keyboard.release('0');
		  delay(10);
		}
		else if(customKey==CUSTOM_KEY2)
		{
			//Do nothing :P
		}
		else
		{
		   Keyboard.press(customKey);
		   delay(100);
		   Keyboard.release(customKey);
		   delay(10);
		}
   
    }
}

CustomKeypadClass CustomKeypad;

