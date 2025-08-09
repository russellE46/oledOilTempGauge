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

#define SERIAL_PAD_LINES  3
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/***************************************************************************************
 * @brief - displayInit()
 *  Will hang firmware if display init fails.
 *      If you suspect this, check the serial monitor.
 *      Set "Port" to /dev/tty.usbmodem##### - Seeed Studio.
 * 
 * @return - None
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


/***********************************************************************************
 * @brief - displayPrintHappyChibi()
 *  Just prints happy chibi to the OLED.
 * 
 * @return - None
 ***********************************************************************************/
void displayPrintHappyChibi()
{
    display.clearDisplay();
    display.drawBitmap(0, 0, BLANK_CHIBI, 128, 60, WHITE);
    display.display();
}


/***********************************************************************************
 * @brief - displayBlinkChibi()
 *  Blinks happy chibi on the OLED screen on and off every 500 ms.
 * 
 * @param - TimeSeconds: Int representing the amount of time to blink for 
 * 
 * @return - None
 ***********************************************************************************/
void displayBlinkChibi(int TimeSeconds)
{
    for (int i = 0; i < TimeSeconds; i++)
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


/***********************************************************************************
 * @brief - displaySerialDebugPrint()
 *  Prints the contents of an image byte array to the serial port
 * 
 * @param - const uint8_t * PtrImage: Ptr to the first byte in the image byte array
 * 
 * @return - None
 ***********************************************************************************/
void displaySerialDebugPrint(const unsigned char * PtrImage)
{
    String line = "";
    int imageSize = SCREEN_HEIGHT * SCREEN_WIDTH / 8; // One bit per pixel, 8 bits per byte

    for (int i = 0; i < SERIAL_PAD_LINES; i++)
    {
        Serial.println("\n");
    }

    for (int i = 0; i < imageSize; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (PtrImage[i] & (1 << (7 - j)))
            {
                line += "**";
            }
            else
            {
                line += "  ";
            }
        }

        if (((i + 1) % (SCREEN_WIDTH / 8)) == 0)
        {
            line += "\r\n";
            Serial.println(line);
            line = "";
        }
    }
}