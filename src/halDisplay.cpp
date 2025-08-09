/*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*   halDisplay.cpp
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
*   Definitions for control of the SSD1306 OLED display
*
*/

#include "halDisplay.hpp"
#include "baseChibis.hpp"

#ifndef __AVR__
//TwoWire myWire(&sercom2, 9, 10);  // SDA = A9, SCL = A10
#endif

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/***************************************************************************************
 * Will hang firmware if display init fails
 * If you suspect this, check the serial monitor
 * Set "Port" to /dev/tty.usbmodem##### - Seeed Studio
 ***************************************************************************************/
void displayInit()
{
    // I2C address 0x3C is typical for 0.96" OLEDs
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        // Hang firmware and continuously send serial output
        // if display init fails
        while (true)
        {
            Serial.println(F("SSD1306 allocation failed"));
            delay(1000);
        }
    }
}

/* @brief displayPrintHappyChibi()
 *  Just prints happy chibi to the OLED.
 *
 */

void displayPrintHappyChibi()
{
    display.clearDisplay();
    display.drawBitmap(0, 0, HAPPY_CHIBI, 128, 60, WHITE);
    display.display();
}

// 0xf8 = 0b 1111 0100

/* @brief displayBlinkChibi()
 * Blinks happy chibi on the OLED screen on and off every 500 ms.
 * 
 * @param - timeSeconds: Int representing the amount of time to blink for 
 */

void displayBlinkChibi(int timeSeconds)
{
    int halfIntervalMs = timeSeconds * 1000 / 2;
    for (int i = 0; i < timeSeconds; i++)
    {
        // Blink screen while waiting so that we can show that
        // firmware is alive.
        displayPrintHappyChibi();
        delay(500);
        display.clearDisplay();
        display.display();
        delay(500);
    }
}