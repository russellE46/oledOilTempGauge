/*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*   halThermistor.hpp
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
*   Declarations for interactions with the thermistor voltage divider circuit
*
*/

#ifndef HAL_THERMISTOR_HPP
#define HAL_THERMISTOR_HPP

#include <Wire.h>

#define NUM_RES_VALUES    16      // Number of resistance and temperature values stored for reference
#define NUM_SAMPLES       120      // Number of samples stored and used to calculate rolling averages


#ifdef __AVR__
  #define SENSOR_PIN        A0
#else
  #define REF_mV            3320.0
  #define SENSOR_PIN        7
#endif

// PARALLEL ARRAYS
extern const unsigned int RESISTANCE_VALS[];    // Stored resistance values of thermistor at temps in TEMP_VALS 
extern const unsigned int TEMP_VALS[];          // Stored temperature values at each resistance value in RESISTANCE_VALS

typedef enum _ADC_CTRL_B_RESSEL_NUM
{
  ADC_CTRL_B_RESSEL_12_BIT    = 0,
  ADC_CTRL_B_RESSEL_16_BIT    = 1,
  ADC_CTRL_B_RESSEL_10_BIT    = 2,
  ADC_CTRL_B_RESSEL_8_BIT     = 3

} ADC_CTRL_B_RESSEL_NUM, *PTR_ADC_CTRL_B_RESSEL_NUM;

float readVcc();

void thermistorMonInit();

float getResAvg();

int getTempAvg();

float getPinVoltage(unsigned char Pin);

float getRes();

int getTemp(bool print);

unsigned long int getScaledRefRes(unsigned char a);

float resToTemp(float Res, bool print);

float getSlope(unsigned char a, unsigned char b);

#endif
