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

#define NUM_RES_VALUES    18      // Number of resistance and temperature values stored for reference
#define NUM_SAMPLES       10      // Number of samples stored and used to calculate rolling averages
#define RES_SCALE_FACTOR  2       // Factor that RESISTANCE_VALS values are scaled down by

#ifdef __AVR__
  #define ADC_RES           1023.0
  #define SENSOR_PIN        A0
#else
  #define ADC_RES           4095.0
  #define REF_mV            3300.0
  #define SENSOR_PIN        A0
#endif

// PARALLEL ARRAYS
extern const unsigned int RESISTANCE_VALS[];    // Stored resistance values of thermistor at temps in TEMP_VALS 
extern const unsigned int TEMP_VALS[];          // Stored temperature values at each resistance value in RESISTANCE_VALS

#ifdef __AVR__
long readVcc();
#endif

void thermistorMonInit();

float getResAvg();

int getTempAvg();

float getRes();

int getTemp(bool print);

unsigned long int getScaledRefRes(unsigned char a);

float resToTemp(float Res, bool print);

float getSlope(unsigned char a, unsigned char b);

#endif
