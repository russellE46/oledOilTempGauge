/*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*   halDisplay.hpp
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
*   Declarations for control of the SSD1306 OLED display
*
*/

#ifndef HAL_DISPLAY_HPP
#define HAL_DISPLAY_HPP

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "wiring_private.h"

#define SCREEN_WIDTH      128
#define SCREEN_HEIGHT     64

#define OLED_RESET -1

extern Adafruit_SSD1306 display;

void displayInit();

void displayPrintHappyChibi();

void displayBlinkChibi(int timeSeconds);

#endif