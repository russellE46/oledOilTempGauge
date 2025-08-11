
#include "baseChibis.hpp"
#include "halDisplay.hpp"
#include "halThermistor.hpp"

#define INIT_DELAY_SEC    2

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const bool DEBUG = false;
const bool NUMBERS_DEBUG = false;
const bool THERMIST_DATA_COLLECTION = true;

/***************************************************************************************
 * Need to delay for a bit to ensure that voltages have stabilized
 * after power-on. Not sure if this is necessary, but doesn't hurt.
 ***************************************************************************************/
void startupDelay()
{
  displayBlinkChibi(INIT_DELAY_SEC);
}

void numbersDebug()
{
  for (int i = 0; i < 20; i++)
  {
    display.clearDisplay();
    display.setTextSize(5);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);     // Top-left corner
    display.println(String(i));
    display.display();
    delay(100);
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/***********************************
 *     MAIN FIRMWARE SECTION       *
 ***********************************/

// Initialization code. Runs once on power-on.
void setup() {
  Serial.begin(9600);

  displayInit();

  startupDelay();

  thermistorMonInit();
}

// Main code that continuously loops forever
void loop() {
  display.clearDisplay();

  if (DEBUG)
  {
    // If you suspect something is wrong with the screen or 
    // any connections, set DEBUG = true at the top of this file.
    // Chibi blink to show that firmware is alive
    displayBlinkChibi(5);

    if (NUMBERS_DEBUG)
    {
      numbersDebug();
    }

    displaySerialDebugPrint(HAPPY_CHIBI);
  }
  else 
  {
    if (THERMIST_DATA_COLLECTION)
    {
      display.setTextSize(2);  // 2x scale
    }
    else
    {
      display.setTextSize(4); // 4x
    }

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);     // Top-left corner
    display.println(String(getTempAvg()) + "F");

    if (THERMIST_DATA_COLLECTION)
    {
      display.println(String(getResAvg()) + " Ohms");
      #ifdef __AVR__
      display.println(String(readVcc()) + " mV");
      #endif
    }
    display.display();           // Push to screen
  }

  Serial.println(F("I'm alive!\r\n"));
  delay(10);
}