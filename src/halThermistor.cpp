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
#define ADC_RES_NUM_BITS    10
#else
#define ADC_RES_NUM_BITS    12
#endif

#define ADC_RES             float(1 << ADC_RES_NUM_BITS)

#define SERIES_RESISTOR     150.0   // 150 Ohm series resistor is used in voltage divider circuit
#define RES_SCALE_FACTOR    1       // Factor that RESISTANCE_VALS values are scaled down by

// Slightly altered resistance values based on table in
// Amazon listing: https://www.amazon.com/PQY-Temperature-Sensor-Sender-Electric/dp/B08MTJJTFK/ref=sr_1_5?crid=OERZDIYZEP9L&dib=eyJ2IjoiMSJ9.Ax70sMO3h5wLucVKUUK2Bwrb6nxw-Lu6bLvDJpekDGBI4DigFONbrwXPxV-sgw89X6lzM9883L_3LrC8yWnMoEvriPWv3NNzZ4iyQO-4LNvSLSnKYAsUfNtXQv3_lTDQFCRILeHsCVX_bl78Ms5qUjWPhXeWWkkRMH-TlVPSYIoXB5e7FeHGZ1fF0KqCmZFNPYI2JyvKXpjqIFwoegBKk649bYwI64wWf6Y5QYecLKk.b8o_X-dBp_B8-4zhSeJtW5AX77OmJs_QCWHkZqiLxsc&dib_tag=se&keywords=thermistor+1%2F8+npt&qid=1754964195&sprefix=thermistor+1%2F8+npt%2Caps%2C155&sr=8-5
const unsigned int RESISTANCE_VALS[] =
  {
    3200,  //    | 68f
    2150,  //    | 86f
    1420,  //    | 104f 
    895,   //    | 122f *
    620,   //    | 140f *
    428,   //    | 158f *
    304,   //    | 176f *
    224,   //    | 194f *
    160,   //    | 212f *
    124,   //    | 230f *
    95,    //    | 248f *
    73,    //    | 266f *
    59,    //    | 284f *
    45,    //    | 302f *
    37,    //    | 320f *  
    30    //     | 338f *
  };

// Farenheit temperatures based on datasheet in
// Amazon listing for thermistor: 
// https://www.amazon.com/PQY-Temperature-Sensor-Sender-Electric/dp/B08MTJJTFK/ref=sr_1_5?crid=OERZDIYZEP9L&dib=eyJ2IjoiMSJ9.Ax70sMO3h5wLucVKUUK2Bwrb6nxw-Lu6bLvDJpekDGBI4DigFONbrwXPxV-sgw89X6lzM9883L_3LrC8yWnMoEvriPWv3NNzZ4iyQO-4LNvSLSnKYAsUfNtXQv3_lTDQFCRILeHsCVX_bl78Ms5qUjWPhXeWWkkRMH-TlVPSYIoXB5e7FeHGZ1fF0KqCmZFNPYI2JyvKXpjqIFwoegBKk649bYwI64wWf6Y5QYecLKk.b8o_X-dBp_B8-4zhSeJtW5AX77OmJs_QCWHkZqiLxsc&dib_tag=se&keywords=thermistor+1%2F8+npt&qid=1754964195&sprefix=thermistor+1%2F8+npt%2Caps%2C155&sr=8-5
const unsigned int TEMP_VALS[] =
  {
    68,
    86,
    104,
    122,
    140,
    158,
    176,
    194,
    212,
    230,
    248, 
    266,
    284,
    302,
    320,
    338
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
    #ifndef __AVR__
    analogReadResolution(ADC_RES_NUM_BITS);
    #endif

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

  currResSum -= resSamples[resIt];
  resSamples[resIt] = newRes;       // store new sample
  currResSum += newRes;

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
  tempSamples[tempIt] = newTemp;        // store new sample
  currTempSum += newTemp;
  
  tempIt++;
  return currTempSum / NUM_SAMPLES;
}


/***********************************************************************************
 * @brief - getPinVoltage()
 *  Calculates the current voltage sensed on the requested pin
 * 
 * @return - float: The current resistance value of the thermistor
 ***********************************************************************************/
float getPinVoltage(unsigned char Pin)
{
  int adcValue = analogRead(Pin);
  float refV = readVcc();

  return (adcValue / ADC_RES) * refV;
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
  float refV, voltage, resistance;
  voltage = getPinVoltage(SENSOR_PIN);
  refV = readVcc();
  //resistance = SERIES_RESISTOR * (refV / voltage - 1.0);

  resistance = (voltage * SERIES_RESISTOR) / (refV - voltage);

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
    if (lookupRes > getScaledRefRes(0))
    {
      slope = getSlope(0, 1);
      temp = TEMP_VALS[0] - (slope * (getScaledRefRes(0) - lookupRes));
    }

    else if (lookupRes <= getScaledRefRes(NUM_RES_VALUES - 1))
    {
      slope = getSlope(NUM_RES_VALUES - 2, NUM_RES_VALUES - 1);
      temp = TEMP_VALS[NUM_RES_VALUES - 1] + (slope * (lookupRes - getScaledRefRes(NUM_RES_VALUES - 1)));
    }
    else
    {
      for (int i  = 0; i < NUM_RES_VALUES - 1; i++)
      {
        if (lookupRes <= getScaledRefRes(i) &&
            lookupRes > getScaledRefRes(i + 1))
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


/***********************************************************************************
 * @brief - readVcc()
 *  Reads internal reference voltage in mV. Only works on arduino, not seeed.
 *    I measured voltage with a multimeter and saw values between 4.67 and 4.76.
 *    This function will probably return values in or near that range.
 * 
 * @return - long: Internal reference voltage in mV
 ***********************************************************************************/
float readVcc()
{
#ifdef __AVR__
  // Set the reference to Vcc and the measurement to the internal 1.1V reference
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(200); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // Wait until done
  uint16_t result = ADC;

  // 1.1V * 1023 / result = Vcc in millivolts
  return 1125300f / float(result); // ~1.1V * 1023 * 1000

#else
  return REF_mV / 1000.0;
#endif
}