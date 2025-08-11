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

// Resistance values that have been experimentally colelcted at specified temperatures.
// True values have been scaled down by a factor of RES_SCALE_FACTOR.
// Muliply values in this array by RES_SCALE_FACTOR to get the real resistance.

#ifdef __AVR__
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

#define SERIES_RESISTOR   2200.0       // 2.2kΩ resistor is used in the voltage divider circuit
#define RES_SCALE_FACTOR  2       // Factor that RESISTANCE_VALS values are scaled down by
#else

const unsigned int RESISTANCE_VALS[] =
  {
    600,    //   14440  | 80f  | 19    Ohms/deg f
    950,    //   19000  | 120f | 33    Ohms/deg f
    1215,   //   24300  | 140f | 31    Ohms/deg f
    1600,   //   32000  | 160f | 53    Ohms/deg f
    1850,   //   37000  | 170f | 55    Ohms/deg f
    2175,   //   43500  | 180f | 60    Ohms/deg f
    2535,   //   50700  | 190f | 60    Ohms/deg f
    2750,   //   55000  | 195f | 72    Ohms/deg f
    3000,   //   60000  | 200f | 80    Ohms/deg f
    3285,   //   65700  | 205f | 76    Ohms/deg f
    3500,   //   70000  | 210f | 80    Ohms/deg f
    3850,   //   77000  | 215f | 84    Ohms/deg f
    4150,   //   83000  | 220f | 55    Ohms/deg f
    4875,   //   97500  | 230f | 75    Ohms/deg f
    5700,   //   114000 | 240f | 174   Ohms/deg f
    7750,   //   155000 | 260f | 197   Ohms/deg f
    10600,  //   212000 | 280f | 232.5 Ohms/deg f
    14000   //   280000 | 300f
  };

#define SERIES_RESISTOR     2200.0
#define RES_SCALE_FACTOR    20       // Factor that RESISTANCE_VALS values are scaled down by
#endif


// Temperatures used to experimentally collect resistance values.
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

unsigned int tempSamples[NUM_SAMPLES];      // Stores the last NUM_SAMPLES temperature samples
float resSamples[NUM_SAMPLES];              // Stores the last NUM_SAMPLES resistances samples

int tempIt = 0;                             // Iterator used to move through tempSamples arr
int resIt = 0;                              // Iterator used ot move through the resSamples arr

unsigned int currTempSum = 0;               // Current sum of all values in tempSamples arr
double currResSum = 0;                      // Current sum of all values in resSamples arr


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


/***********************************************************************************
 * @brief - getResAvg()
 *  Stores a new resistance value in the array of stored samples. Discards the oldest
 *    resistance value. Gets the current average resistance based on the stored samples.
 * 
 * @return - float: The current average resistance based on the stored samples.
 ***********************************************************************************/
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


/***********************************************************************************
 * @brief - getTempAvg()
 *  Stores a new temperature value in the array of stored samples. Discards the oldest
 *    temperature value. Gets the current average temperature based on the stored samples.
 * 
 * @return - float: The current average temperature based on the stored samples.
 ***********************************************************************************/
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


/***********************************************************************************
 * @brief - getRes()
 *  Calculates the current resistance of the thermistor based on the series resistor
 *    and the voltage sensed on the SENSOR_PIN
 * 
 * @return - float: The current resistance value of the thermistor
 ***********************************************************************************/
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


/***********************************************************************************
 * @brief - getTemp()
 *  Returns the current temperature of the thermistor.
 * 
 * @param - bool Print: boolean that makes FW print debug info to the serial port if true.
 * 
 * @return - float: The current temperature of the thermistor.
 ***********************************************************************************/
int getTemp(bool Print)
{
  return int(resToTemp(getRes(), Print));
}


/***********************************************************************************
 * @brief - getScaledRefRes()
 *  Gets the actual resistance value at the requested index of the RESISTANCE_VALS array.
 *    Resistance values in the RESISTANCE_VALS array are scaled down by RES_SCALE_FACTOR
 *    in order to save memory. This is more important on the nano platform where memory
 *    is extremely limited.
 * 
 * @param - uint8_t index: index of RESISTANCE_VALS to get actual value of
 * 
 * @return - float: The current average resistance based on the stored samples.
 ***********************************************************************************/
unsigned long int getScaledRefRes(unsigned char index)
{
  return (unsigned long int)(RESISTANCE_VALS[index]) * RES_SCALE_FACTOR;
}


/***********************************************************************************
 * @brief - resToTemp()
 *  Calculates/estimates the current temperature of the thermistor based on the
 *    provided resistance value. Performs linear interpolation to estimate values.
 * 
 * @param - float Res: Resistance value to return corresponding temperature of
 * @param - bool Print: boolean that makes FW print debug info to the serial port if true.
 * 
 * @return - float: The current average resistance based on the stored samples.
 ***********************************************************************************/
float resToTemp(float Res, bool Print)
{
    float lookupRes = Res;
    float temp = 0;
    float slope;

    // Resistance is less than 
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
          if (Print)
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


/***********************************************************************************
 * @brief - resToTemp()
 *  Calculates/estimates the current temperature of the thermistor based on the
 *    provided resistance value. Performs linear interpolation to estimate values.
 * 
 * @param - uint8_t ind0: Lower index
 * @param - uint8_t ind1: Upper index
 * 
 * @return - float: Change in temp per change in resistance between ind0 and ind1
 ***********************************************************************************/
float getSlope(unsigned char ind0, unsigned char ind1)
{
   return (float(TEMP_VALS[ind1]) - float(TEMP_VALS[ind0])) / (float(getScaledRefRes(ind1)) - float(getScaledRefRes(ind0)));
}


#ifdef __AVR__
/***********************************************************************************
 * @brief - readVcc()
 *  Reads internal reference voltage in mV. Only works on arduino, not seeed.
 *    I measured voltage with a multimeter and saw values between 4.67 and 4.76.
 *    This function will probably return values in or near that range.
 * 
 * @return - long: Internal reference voltage in mV
 ***********************************************************************************/
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