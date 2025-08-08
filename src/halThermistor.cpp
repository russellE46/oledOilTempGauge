/*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*   halThermistor.hpp
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
*   Definitions for interactions with the thermistor voltage divider circuit
*
*/

#include "halThermistor.hpp"
#include <Arduino.h>

const unsigned int RESISTANCE_VALS[] =
  {
    1050,    // 80f  | 19    Ohms/deg f
    2250,    // 120f | 33    Ohms/deg f
    3100,    // 140f | 31    Ohms/deg f
    4500,    // 160f | 53    Ohms/deg f
    5500,   // 170f | 55    Ohms/deg f
    6750,   // 180f | 60    Ohms/deg f
    7750,   // 190f | 60    Ohms/deg f
    8400,   // 195f | 72    Ohms/deg f
    9300,   // 200f | 80    Ohms/deg f
    10350,   // 205f | 76    Ohms/deg f
    11400,   // 210f | 80    Ohms/deg f
    12600,   // 215f | 84    Ohms/deg f
    14250,   // 220f | 55    Ohms/deg f
    17250,   // 230f | 75    Ohms/deg f
    21000,   // 240f | 174   Ohms/deg f
    30000,   // 260f | 197   Ohms/deg f
    42000,   // 280f | 232.5 Ohms/deg f
    60000   // 300f
  };

const unsigned int TEMP_VALS[] =
  {
    80,
    120,
    140,
    160,  // -10
    170,
    180,
    190,
    195,
    200,
    205,
    210, // -10
    215,
    220,
    230,
    240,
    260,
    280,
    300
  };

unsigned int tempSamples[NUM_SAMPLES];
float resSamples[NUM_SAMPLES];

int tempIt = 0;
int resIt = 0;
unsigned int currTempSum = 0;
double currResSum = 0;

// Vout 4.76, 4.67
const float SERIES_RESISTOR = 2200.0; // 10kΩ

/***************************************************************************************
 * Initializes "rolling averages" with the first temp and resistance
 * values read from hardware.
 ***************************************************************************************/
void thermistorMonInit()
{
    float temp = getTemp(false);
    float res = getRes();
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        // Populate entire
        tempSamples[i] = temp;
        currTempSum += temp;

        resSamples[i] = res;
        currResSum += res;
    }
}

float getResAvg()
{
  float newRes = getRes();
  resIt = resIt % NUM_SAMPLES;

  // crash zone
  currResSum -= resSamples[resIt];
  // store new sample
  resSamples[resIt] = newRes;
  currResSum += newRes;
  // end crash zone
  resIt++;

  return currResSum / NUM_SAMPLES;
}

int getTempAvg()
{
  int newTemp = getTemp(true);
  tempIt = tempIt % NUM_SAMPLES;
  
  currTempSum -= tempSamples[tempIt];
  // store new sample
  tempSamples[tempIt] = newTemp;
  currTempSum += newTemp;
  
  tempIt++;
  return currTempSum / NUM_SAMPLES;
}

float getRes()
{
  int adcValue = analogRead(SENSOR_PIN);
  float refV, voltage, resistance;

  #ifdef __AVR__
  refV = readVcc();
  #else
  refV = REF_mV;
  #endif

  voltage = adcValue * refV / 1000.0 / ADC_RES;
  resistance = SERIES_RESISTOR * (refV / 1000.0 / voltage - 1.0);

  return resistance;
}

int getTemp(bool print)
{
  return int(resToTemp(getRes(), print));
}

unsigned long int getScaledRefRes(unsigned char a)
{
  return (unsigned long int)(RESISTANCE_VALS[a]) * RES_SCALE_FACTOR;
}

float resToTemp(float Res, bool print)
{
    float lookupRes = Res;
    float temp = 0;
    float slope;

    if (lookupRes < getScaledRefRes(0))
    {
      slope = getSlope(0, 1);
      temp = TEMP_VALS[0] - (slope * (getScaledRefRes(0) - lookupRes));
    }

    else if (lookupRes >= getScaledRefRes(NUM_RES_VALUES - 1))
    {
      slope = getSlope(NUM_RES_VALUES - 2, NUM_RES_VALUES - 1);
      temp = TEMP_VALS[NUM_RES_VALUES - 1] + (slope * (lookupRes - getScaledRefRes(NUM_RES_VALUES - 1)));
    }
    else
    {
      for (int i  = 0; i < NUM_RES_VALUES - 1; i++)
      {
        if (lookupRes >= getScaledRefRes(i) &&
            lookupRes < getScaledRefRes(i + 1))
        {
          if (print)
          {
            Serial.println(String(lookupRes) + " " + getScaledRefRes(i) + " " + getScaledRefRes(i + 1) + " " + String(i) + "\n");
          }
          slope = getSlope(i, i + 1);
          temp = TEMP_VALS[i] + (slope * (lookupRes - getScaledRefRes(i)));
          break;
        }
      }
    }

    return temp;
}

float getSlope(unsigned char a, unsigned char b)
{
   return (float(TEMP_VALS[b]) - float(TEMP_VALS[a])) / (float(getScaledRefRes(b)) - float(getScaledRefRes(a)));
}

#ifdef __AVR__
long readVcc()
{
  // Set the reference to Vcc and the measurement to the internal 1.1V reference
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(200); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // Wait until done
  uint16_t result = ADC;

  // 1.1V * 1023 / result = Vcc in millivolts
  return 1125300L / result; // ~1.1V * 1023 * 1000
}
#endif