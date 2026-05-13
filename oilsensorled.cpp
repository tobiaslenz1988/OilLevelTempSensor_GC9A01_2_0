#include <stdio.h>
#include <string.h>
#include <Arduino.h>

#define OilLevelLED                   13
#define OilTemperatureLED             15


bool showLevelAndTempAtLED(bool OldToggleValue, uint8_t oilLevelPercentage,uint8_t oilTemperature)
{
  bool toggle = OldToggleValue;
  if(oilLevelPercentage>30)
  {
    digitalWrite(OilLevelLED, false);
    if(oilTemperature<65)
    { /* Engine is cold Hot -> led on */
      digitalWrite(OilTemperatureLED, true);
    } else if(oilTemperature>100)
    { /* Engine is too Hot -> toggle led*/
      if(toggle == false){
        digitalWrite(OilTemperatureLED, true);
        toggle = true;
      }else{
        digitalWrite(OilTemperatureLED, false);
        toggle = false;
      }
    }
    else 
    { /* Engine is in Normal range -> led off */
      digitalWrite(OilTemperatureLED, false);
    }
  }else{
    digitalWrite(OilLevelLED, true);
  }
  return toggle;
}