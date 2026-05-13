#include <Arduino.h>
#include <stdio.h>
#include "oilsensor.h"
#include "NRC_UDS_protocol.h"

extern bool TimeoutSensorDetected;
extern bool NewOilSensorEquipped; 

extern uint8_t oilTemperature;
extern uint8_t oilLevelPercentage ;


extern uint8_t testValue_oilTemperature;
extern uint8_t testValue_oilLevelPercentage;

extern uint16_t OldOilTempCompValues[16];
extern uint16_t OldOilLevelCompValues[11];
/*
extern uint16_t NewOilTempCompValues[16];
extern uint16_t NewOilLevelCompValues[11];
*/


/* This methods converts the received impulses to an Percentage for OilLevel and Oil Temperature*/
void convertImpulseToPercentage(uint16_t cntRawData_Temp, uint16_t cntRawData_Level,uint8_t session) {
  if (session == UDS_Session_Control_Default_Session) 
  { /* Debug of... normal Operation*/
    if(TimeoutSensorDetected == false)
	{
      if(NewOilSensorEquipped==true)
	  {
      /*
        if (cntRawData_Level > NewOilLevelCompValues[10]) { oilLevelPercentage = 253; }else
        if (cntRawData_Level = NewOilLevelCompValues[10]) { oilLevelPercentage = 100; }else
        if (cntRawData_Level >= NewOilLevelCompValues[9]) { oilLevelPercentage = 90; }else
        if (cntRawData_Level >= NewOilLevelCompValues[8]) { oilLevelPercentage = 80; }else
        if (cntRawData_Level >= NewOilLevelCompValues[7]) { oilLevelPercentage = 70; }else
        if (cntRawData_Level >= NewOilLevelCompValues[6]) { oilLevelPercentage = 60; }else
        if (cntRawData_Level >= NewOilLevelCompValues[5]) { oilLevelPercentage = 50; }else
        if (cntRawData_Level >= NewOilLevelCompValues[4]) { oilLevelPercentage = 40; }else
        if (cntRawData_Level >= NewOilLevelCompValues[3]) { oilLevelPercentage = 30; }else
        if (cntRawData_Level >= NewOilLevelCompValues[2]) { oilLevelPercentage = 20; }else
        if (cntRawData_Level >= NewOilLevelCompValues[1]) { oilLevelPercentage = 10; }else
        if (cntRawData_Level >= NewOilLevelCompValues[0]) { oilLevelPercentage = 0; }
    */
      }else{
        if (cntRawData_Level > OldOilLevelCompValues[10]) { oilLevelPercentage = 253; }else
        if (cntRawData_Level = OldOilLevelCompValues[10]) { oilLevelPercentage = 100; }else
        if (cntRawData_Level >= OldOilLevelCompValues[9]) { oilLevelPercentage = 90; }else
        if (cntRawData_Level >= OldOilLevelCompValues[8]) { oilLevelPercentage = 80; }else
        if (cntRawData_Level >= OldOilLevelCompValues[7]) { oilLevelPercentage = 70; }else
        if (cntRawData_Level >= OldOilLevelCompValues[6]) { oilLevelPercentage = 60; }else
        if (cntRawData_Level >= OldOilLevelCompValues[5]) { oilLevelPercentage = 50; }else
        if (cntRawData_Level >= OldOilLevelCompValues[4]) { oilLevelPercentage = 40; }else
        if (cntRawData_Level >= OldOilLevelCompValues[3]) { oilLevelPercentage = 30; }else
        if (cntRawData_Level >= OldOilLevelCompValues[2]) { oilLevelPercentage = 20; }else
        if (cntRawData_Level >= OldOilLevelCompValues[1]) { oilLevelPercentage = 10; }else
        if (cntRawData_Level >= OldOilLevelCompValues[0]) {oilLevelPercentage = 0; }
      }


      if(NewOilSensorEquipped==true){
        if (oilLevelPercentage > 0) {
          /*
          if (cntRawData_Temp > NewOilTempCompValues[15]) { oilTemperature = 253; }else
          if (cntRawData_Temp = NewOilTempCompValues[15]) { oilTemperature = 115; }else
          if (cntRawData_Temp >= NewOilTempCompValues[14]) { oilTemperature = 110; }else
          if (cntRawData_Temp >= NewOilTempCompValues[13]) { oilTemperature = 105; }else
          if (cntRawData_Temp >= NewOilTempCompValues[12]) { oilTemperature = 100; }else
          if (cntRawData_Temp >= NewOilTempCompValues[11]) { oilTemperature = 95; }else
          if (cntRawData_Temp >= NewOilTempCompValues[10]) { oilTemperature = 90; }else
          if (cntRawData_Temp >= NewOilTempCompValues[9]) { oilTemperature = 85; }else
          if (cntRawData_Temp >= NewOilTempCompValues[8]) { oilTemperature = 80; }else
          if (cntRawData_Temp >= NewOilTempCompValues[7]) { oilTemperature = 75; }else
          if (cntRawData_Temp >= NewOilTempCompValues[6]) { oilTemperature = 70; }else
          if (cntRawData_Temp >= NewOilTempCompValues[5]) { oilTemperature = 65; }else
          if (cntRawData_Temp >= NewOilTempCompValues[4]) { oilTemperature = 60; }else
          if (cntRawData_Temp >= NewOilTempCompValues[3]) { oilTemperature = 55; }else
          if (cntRawData_Temp >= NewOilTempCompValues[2]) { oilTemperature = 50; }else
          if (cntRawData_Temp >= NewOilTempCompValues[1]) { oilTemperature = 40; }else
		      if (cntRawData_Temp >= NewOilTempCompValues[0]) { oilTemperature = 30; }
      */
        }else{ oilTemperature = OilTemperaturePercentageErrorValue;}
      }else{
        if (oilLevelPercentage > 0) {
          
          if (cntRawData_Temp > OldOilTempCompValues[15]) { oilTemperature = 253; }else
          if (cntRawData_Temp =  OldOilTempCompValues[15]) { oilTemperature = 115; }else
          if (cntRawData_Temp >= OldOilTempCompValues[14]) { oilTemperature = 110; }else
          if (cntRawData_Temp >= OldOilTempCompValues[13]) { oilTemperature = 105; }else
          if (cntRawData_Temp >= OldOilTempCompValues[12]) { oilTemperature = 100; }else
          if (cntRawData_Temp >= OldOilTempCompValues[11]) { oilTemperature = 95; }else
          if (cntRawData_Temp >= OldOilTempCompValues[10]) { oilTemperature = 90; }else
          if (cntRawData_Temp >= OldOilTempCompValues[9]) { oilTemperature = 85; }else
          if (cntRawData_Temp >= OldOilTempCompValues[8]) { oilTemperature = 80; }else
          if (cntRawData_Temp >= OldOilTempCompValues[7]) { oilTemperature = 75; }else
          if (cntRawData_Temp >= OldOilTempCompValues[6]) { oilTemperature = 70; }else
          if (cntRawData_Temp >= OldOilTempCompValues[5]) { oilTemperature = 65; }else
          if (cntRawData_Temp >= OldOilTempCompValues[4]) { oilTemperature = 60; }else
          if (cntRawData_Temp >= OldOilTempCompValues[3]) { oilTemperature = 55; }else
          if (cntRawData_Temp >= OldOilTempCompValues[2]) { oilTemperature = 50; }else
          if (cntRawData_Temp >= OldOilTempCompValues[1]) { oilTemperature = 40; }else
          if (cntRawData_Temp >= OldOilTempCompValues[0]) { oilTemperature = 30; }
        }else{ oilTemperature = OilLevelPercentageErrorValue;}
      }


    }else{
      /* if Oil Sensor is disconnected  */
      oilTemperature      = OilTemperaturePercentageErrorValue;
      oilLevelPercentage  = OilLevelPercentageErrorValue;
    }
  } else {
    /*Debug On*/
    oilTemperature      = testValue_oilTemperature;
    oilLevelPercentage  = testValue_oilLevelPercentage;
  }
}
