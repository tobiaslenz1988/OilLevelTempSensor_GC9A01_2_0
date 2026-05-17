#include "esp32-hal.h"
#include <Arduino.h>
#include <stdio.h>

#include <string.h>
#include <BluetoothSerial.h>
#include <Preferences.h>


#include "uds_statemachine_common.h"
#include "bus_common.h"
#include "oilsensor_common.h"
#include "nrc_uds_protocol_common.h"
#include "brand_defines_common.h"
#include "softwareversion.h"

extern Preferences preferences;
extern BluetoothSerial SerialBT;
extern uint8_t session;
//extern


void delete_BT_buffer()
{
  for(int z=0;z<Buffersize;z++)
  {
    BT_rx_buffer[z]=0xFF;  
  }
}

void analyse_BT_Protocol(uint8_t receive_BT_Array[])
{   
  if(NewData == true){
    /*Command to read something*/
    if (receive_BT_Array[0]==UDS_READ_DATA_BY_IDENTIFIER)
    {
	    uint8_t posResponse = (UDS_READ_DATA_BY_IDENTIFIER + 0x40);
      /* 0x22 0xF1 0xAB */
      /* Get SW Version*/
      if((receive_BT_Array[1]==0xF1) && (receive_BT_Array[2]==0xAB))
      {
        BUS_output(posResponse);
        BUS_output(0xF1);
        BUS_output(0xAB); 
        BUS_output(SOFTWAREVERSION[0]);
        BUS_output(SOFTWAREVERSION[1]);
        BUS_output(SOFTWAREVERSION[2]);
        BUS_output(SOFTWAREVERSION[3]);
      }else

      /* 0x22 0xF1 0x86 */
      /* Get actual Sesion */
      if((receive_BT_Array[1]==0xF1) && (receive_BT_Array[2]==0x86))
      {
        BUS_output(posResponse);
        BUS_output(0xF1);
        BUS_output(0x86); 
        BUS_output(session);
      }else


      
      /* 0x22 0xF1 0x91 */
      /* Get actual Sesion */
      if((receive_BT_Array[1]==0xF1) && (receive_BT_Array[2]==0x91))
      {
        BUS_output(posResponse);
        BUS_output(0xF1);
        BUS_output(0x91); 
        BUS_output(session);
      }else




      /* 0x22 0xF1 0x97 */
      /* get Systemname */
      if((receive_BT_Array[1]==0xF1) && (receive_BT_Array[2]==0x97))
      {
        BUS_output(posResponse);
        BUS_output(0xF1);
        BUS_output(0x97);
  
        //temp.toCharArray(Modulename, leng);
        //BUS_output(temp);
        uint8_t sizeOfArr = sizeof(Modulename) / sizeof(Modulename[0]);
        uint8_t i;
        while(Modulename[i] != 0x00)
        {
          BUS_output(Modulename[i]);
          i=i+1;
        }
        
      }else
      
      /* This part should read the coding
      /* 0x22 0x06 0x00 ......*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x00))
      {
          BUS_output(posResponse);
          BUS_output(0x06);
          BUS_output(0x00); 
          BUS_output(brand);
          BUS_output(NewOilSensorEquipped);

      }else





      /* 0x22 0x06 0x01 */
      /* This part should get the Debugvalue for the OilTemperature */
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x01))
      {
        BUS_output(posResponse);
        BUS_output(0x06);
        BUS_output(0x01);                  
        BUS_output(testValue_oilTemperature);
        
      }else

      /* 0x22 0x06 0x02 */
       /* This part should get the Debugvalue for the OilLevel */
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x02))
      {
        BUS_output(posResponse);
        BUS_output(0x06);
        BUS_output(0x02);                  
        BUS_output(testValue_oilLevelPercentage);
        
      }else
      
      
      /* 0x22 0x06 0x03 */
      /* Returns the OilTemperature  in Degree Celsius which is used by the SW*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x03))
      {
        BUS_output(posResponse);
        BUS_output(0x06);
        BUS_output(0x03);                  
        BUS_output(oilTemperature);
        
      }else
      
      /* 0x22 0x06 0x04 */
      /* Returns the Oillevel in percent which is used by the SW*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x04))
      {
        BUS_output(posResponse);
        BUS_output(0x06);
        BUS_output(0x04);                  
        BUS_output(oilLevelPercentage);
        
      }else
      
      /* 0x22 0x06 0x05 */
      /* get Oiltemperature compare values for OldSensor */
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x05))
      {
        BUS_output(posResponse);
        BUS_output(0x06);
        BUS_output(0x05); 
        uint8_t i = 0;
        uint8_t tempvar;
        uint8_t sizeOfArr = sizeof(OldOilTempCompValues) / sizeof(OldOilTempCompValues[0]);
        for(i=0;i<sizeOfArr;i++)
        {

            /* As example testval =         500 == 0x01F4    */
            /* BUS_output(testval >> 8);         -> 0x01 */
            /* BUS_output(testval & 0xFF);       -> 0xF4 */
          tempvar = (OldOilTempCompValues[i] >> 8) ;
          BUS_output(tempvar);
          tempvar = (OldOilTempCompValues[i] & 0xFF);
          BUS_output(tempvar);
        }                   
      }else
      
      /* 0x22 0x06 0x06 */
      /* get Oillevel compare values for OldSensor*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x06))
      {
        BUS_output(posResponse);
        BUS_output(0x06);
        BUS_output(0x06);                  
        uint8_t i = 0;
        uint8_t tempvar;
        uint8_t sizeOfArr = sizeof(OldOilLevelCompValues) / sizeof(OldOilLevelCompValues[0]);
        for(i=0;i<sizeOfArr;i++)
        {
            /* As example testval =         500 == 0x01F4    */
            /* BUS_output(testval >> 8);         -> 0x01 */
            /* BUS_output(testval & 0xFF);       -> 0xF4 */
          tempvar = (OldOilLevelCompValues[i] >> 8) ;
          BUS_output(tempvar);
          tempvar = (OldOilLevelCompValues[i] & 0xFF);
          BUS_output(tempvar);
        } 
        
      }else

      /* 0x22 0x06 0x0A */
      /* get the Brandvalue */
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x0A))
      {
        BUS_output(posResponse);
        BUS_output(0x06);
        BUS_output(0x0A);                  
        BUS_output(brand);
      }
      else



      /* 0x22 0x06 0x0B */
      /* get ExtraOutputPin flag*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x0B))
      {
        BUS_output(posResponse);
        BUS_output(0x06);
        BUS_output(0x0B);                  
        BUS_output(statusOfExtraOutputPin);
        
      }else


      /* 0x22 0x06 0x0C */
      /* get OldSensorNewSensor flag*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x0C))
      {
        BUS_output(posResponse);
        BUS_output(0x06);
        BUS_output(0x0C);                  
        BUS_output(NewOilSensorEquipped);
        
      }
      else
      {
        BUS_output(0x7F);
        BUS_output(UDS_READ_DATA_BY_IDENTIFIER);
        BUS_output(UDS_NRC_requestOutOfRange);
      }
  
    
    }else


    

    /*Command to write something*/
    if(receive_BT_Array[0] == UDS_WRITE_DATA_BY_IDENTIFIER)
    {
      uint8_t posResponse = (UDS_WRITE_DATA_BY_IDENTIFIER + 0x40);


      /* This part should change the name of the BT Module*/
      /* 0x2E 0xF1 0x97 0x?? 0x!!  0x!! 0x!! 0x!!......*/
      if((receive_BT_Array[1]==0xF1) && (receive_BT_Array[2]==0x97))
      {
        uint8_t length_of_name = receive_BT_Array[3];
        if((0<length_of_name) && (length_of_name<=20))
        {
          uint8_t i; 
          String tempStr;
          Modulename =    {0,0,0,0,0, 0,0,0,0,0 ,0,0,0,0,0, 0,0,0,0,0};
          for (i=0;i<length_of_name;i++)
          {
            //Modulename[i] =  (char) receive_BT_Array[4+i];
            tempStr.concat((char) receive_BT_Array[4+i]);
            Modulename[i] = receive_BT_Array[4+i];
          }
      
         /*  
          for (i=0;i<length_of_name;i++)
          {
            Modulename[i] = Temparr[i];
          }
          */
       
          preferences.begin(EEPROMNameSpace, false); 
          preferences.putString("Modulename",tempStr);
          preferences.end();
          //Modulename =  Temparray;
          SerialBT.begin(tempStr);
         // Modulename=Temparr;
          BUS_output(posResponse);
          BUS_output(0xF1);
          BUS_output(0x97); 
        }else{
          BUS_output(0x7F);
          BUS_output(UDS_WRITE_DATA_BY_IDENTIFIER);
          BUS_output(UDS_NRC_incorrectMessageLengthOrInvalidFormat);
        }
      }
      else

      /* This part should set the coding
      /* 0x2E 0x06 0x00 0x??......*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x00))
      {

        if((receive_BT_Array[3]!=NULL)&&(receive_BT_Array[4]!=NULL)&&(receive_BT_Array[5]!=NULL))
        {
          brand = receive_BT_Array[3];
          NewOilSensorEquipped = (bool) receive_BT_Array[4];
          
          BUS_output(posResponse);
          BUS_output(0x06);
          BUS_output(0x00); 
        }else{
          BUS_output(0x7F);
          BUS_output(0x2E);
          BUS_output(UDS_NRC_incorrectMessageLengthOrInvalidFormat);         
        }
      }else


      /* This part should set the Debugvalue for the OilTemperature
      /* 0x2E 0x06 0x01 0x??......*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x01))
      {
        uint8_t val  = NULL;
        val = receive_BT_Array[3];
        if(val == NULL)
        {
          BUS_output(0x7f);
          BUS_output(0x2e);
          BUS_output(UDS_NRC_incorrectMessageLengthOrInvalidFormat);
        }
        else
        {
          testValue_oilTemperature = val;
          BUS_output(posResponse);
          BUS_output(0x06);
          BUS_output(0x01); 
        }
      }else

      /* This part should set the Debugvalue for the OilLevel
      /* 0x2E 0x06 0x02 0x!! 0x!!  0x!! 0x!! 0x!!......*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x02))
      {
        uint8_t val  = NULL;
        val = receive_BT_Array[3];
        if(val == NULL)
        {
          BUS_output(0x7f);
          BUS_output(0x2e);
          BUS_output(UDS_NRC_incorrectMessageLengthOrInvalidFormat);
        }
        else
        {
          testValue_oilLevelPercentage =val;
          BUS_output(posResponse);
          BUS_output(0x06);
          BUS_output(0x02); 
        }
      }else


      /* This part should set the compare values for OilTemperature of old Sensor
      /* 0x2E 0x06 0x05 0x!! 0x!!  0x!! 0x!! 0x!!......*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x05))
      {
        uint8_t val  = NULL;
        val = receive_BT_Array[3];
        if(val == NULL)
        {
          BUS_output(0x7f);
          BUS_output(0x2e);
          BUS_output(UDS_NRC_incorrectMessageLengthOrInvalidFormat);
        }
        else
        {
          preferences.begin(EEPROMNameSpace, false); 
          uint16_t temp = ((uint16_t) receive_BT_Array[3]<<8)|receive_BT_Array[4];
          preferences.putShort("Old_sensor_Temperature_30",temp);
          temp = ((uint16_t) receive_BT_Array[5]<<8)|receive_BT_Array[6];
          preferences.putShort("Old_sensor_Temperature_40",temp);
          temp = ((uint16_t) receive_BT_Array[7]<<8)|receive_BT_Array[8];
          preferences.putShort("Old_sensor_Temperature_50",temp);
          temp = ((uint16_t) receive_BT_Array[9]<<8)|receive_BT_Array[10];
          preferences.putShort("Old_sensor_Temperature_55",temp);
          temp = ((uint16_t) receive_BT_Array[11]<<8)|receive_BT_Array[12];
          preferences.putShort("Old_sensor_Temperature_60",temp);
          temp = ((uint16_t) receive_BT_Array[13]<<8)|receive_BT_Array[14];
          preferences.putShort("Old_sensor_Temperature_65",temp);
          temp = ((uint16_t) receive_BT_Array[15]<<8)|receive_BT_Array[16];
          preferences.putShort("Old_sensor_Temperature_70",temp);
          temp = ((uint16_t) receive_BT_Array[17]<<8)|receive_BT_Array[18];
          preferences.putShort("Old_sensor_Temperature_75",temp);
          temp = ((uint16_t) receive_BT_Array[19]<<8)|receive_BT_Array[20];
          preferences.putShort("Old_sensor_Temperature_80",temp);
          temp = ((uint16_t) receive_BT_Array[21]<<8)|receive_BT_Array[22];
          preferences.putShort("Old_sensor_Temperature_85",temp);
          temp = ((uint16_t) receive_BT_Array[23]<<8)|receive_BT_Array[24];
          preferences.putShort("Old_sensor_Temperature_90",temp);
          temp = ((uint16_t) receive_BT_Array[25]<<8)|receive_BT_Array[26];
          preferences.putShort("Old_sensor_Temperature_95",temp);
          temp = ((uint16_t) receive_BT_Array[27]<<8)|receive_BT_Array[28];
          preferences.putShort("Old_sensor_Temperature_100",temp);
          temp = ((uint16_t) receive_BT_Array[29]<<8)|receive_BT_Array[30];
          preferences.putShort("Old_sensor_Temperature_105",temp);
          temp = ((uint16_t) receive_BT_Array[31]<<8)|receive_BT_Array[32];
          preferences.putShort("Old_sensor_Temperature_110",temp);
          temp = ((uint16_t) receive_BT_Array[33]<<8)|receive_BT_Array[34];
          preferences.putShort("Old_sensor_Temperature_115",temp);
          preferences.end();
          

          BUS_output(posResponse);
          BUS_output(0x06);
          BUS_output(0x05); 
        }


      }else
       /* This part should set the compare values for Oillevel old sensor
      /* 0x2E 0x06 0x06 0x!! 0x!!  0x!! 0x!! 0x!!......*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x06))
      {
        uint8_t val  = NULL;
        val = receive_BT_Array[3];
        if(val == NULL)
        {
          BUS_output(0x7f);
          BUS_output(0x2e);
          BUS_output(UDS_NRC_incorrectMessageLengthOrInvalidFormat);
        }
        else
        {
          preferences.begin(EEPROMNameSpace, false); 
          uint16_t temp = ((uint16_t) receive_BT_Array[3]<<8)|receive_BT_Array[4];
          preferences.putShort("Old_sensor_OilLevelEmpty",temp);
          temp = ((uint16_t) receive_BT_Array[5]<<8)|receive_BT_Array[6];
          preferences.putShort("Old_sensor_OilLevel_10",temp);
          temp = ((uint16_t) receive_BT_Array[7]<<8)|receive_BT_Array[8];
          preferences.putShort("Old_sensor_OilLevel_20",temp);
          temp = ((uint16_t) receive_BT_Array[9]<<8)|receive_BT_Array[10];
          preferences.putShort("Old_sensor_OilLevel_30",temp);
          temp = ((uint16_t) receive_BT_Array[11]<<8)|receive_BT_Array[12];
          preferences.putShort("Old_sensor_OilLevel_40",temp);
          temp = ((uint16_t) receive_BT_Array[13]<<8)|receive_BT_Array[14];
          preferences.putShort("Old_sensor_OilLevel_50",temp);
          temp = ((uint16_t) receive_BT_Array[15]<<8)|receive_BT_Array[16];
          preferences.putShort("Old_sensor_OilLevel_60",temp);
          temp = ((uint16_t) receive_BT_Array[17]<<8)|receive_BT_Array[18];
          preferences.putShort("Old_sensor_OilLevel_70",temp);
          temp = ((uint16_t) receive_BT_Array[19]<<8)|receive_BT_Array[20];
          preferences.putShort("Old_sensor_OilLevel_80",temp);
          temp = ((uint16_t) receive_BT_Array[21]<<8)|receive_BT_Array[22];
          preferences.putShort("Old_sensor_OilLevel_90",temp);
          temp = ((uint16_t) receive_BT_Array[23]<<8)|receive_BT_Array[24];
          preferences.putShort("Old_sensor_OilLevelFull",temp);

          preferences.end();

          BUS_output(posResponse);
          BUS_output(0x06);
          BUS_output(0x06); 
        }
      }else

      /* This part should set the compare values for Oiltemperatur New sensor
      /* 0x2E 0x06 0x07 0x!! 0x!!  0x!! 0x!! 0x!!......*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x07))
      {
        uint8_t val  = NULL;
        val = receive_BT_Array[3];
        if(val == NULL)
        {
          BUS_output(0x7f);
          BUS_output(0x2e);
          BUS_output(UDS_NRC_incorrectMessageLengthOrInvalidFormat);
        }
        else
        {
          preferences.begin(EEPROMNameSpace, false); 
          uint16_t temp = ((uint16_t) receive_BT_Array[3]<<8)|receive_BT_Array[4];
          preferences.putShort("New_sensor_Temperature_30",temp);
          temp = ((uint16_t) receive_BT_Array[5]<<8)|receive_BT_Array[6];
          preferences.putShort("New_sensor_Temperature_40",temp);
          temp = ((uint16_t) receive_BT_Array[7]<<8)|receive_BT_Array[8];
          preferences.putShort("New_sensor_Temperature_50",temp);
          temp = ((uint16_t) receive_BT_Array[9]<<8)|receive_BT_Array[10];
          preferences.putShort("New_sensor_Temperature_55",temp);
          temp = ((uint16_t) receive_BT_Array[11]<<8)|receive_BT_Array[12];
          preferences.putShort("New_sensor_Temperature_60",temp);
          temp = ((uint16_t) receive_BT_Array[13]<<8)|receive_BT_Array[14];
          preferences.putShort("New_sensor_Temperature_65",temp);
          temp = ((uint16_t) receive_BT_Array[15]<<8)|receive_BT_Array[16];
          preferences.putShort("New_sensor_Temperature_70",temp);
          temp = ((uint16_t) receive_BT_Array[17]<<8)|receive_BT_Array[18];
          preferences.putShort("New_sensor_Temperature_75",temp);
          temp = ((uint16_t) receive_BT_Array[19]<<8)|receive_BT_Array[20];
          preferences.putShort("New_sensor_Temperature_80",temp);
          temp = ((uint16_t) receive_BT_Array[21]<<8)|receive_BT_Array[22];
          preferences.putShort("New_sensor_Temperature_85",temp);
          temp = ((uint16_t) receive_BT_Array[23]<<8)|receive_BT_Array[24];
          preferences.putShort("New_sensor_Temperature_90",temp);
          temp = ((uint16_t) receive_BT_Array[25]<<8)|receive_BT_Array[26];
          preferences.putShort("New_sensor_Temperature_95",temp);
          temp = ((uint16_t) receive_BT_Array[27]<<8)|receive_BT_Array[28];
          preferences.putShort("New_sensor_Temperature_100",temp);
          temp = ((uint16_t) receive_BT_Array[29]<<8)|receive_BT_Array[30];
          preferences.putShort("New_sensor_Temperature_105",temp);
          temp = ((uint16_t) receive_BT_Array[31]<<8)|receive_BT_Array[32];
          preferences.putShort("New_sensor_Temperature_110",temp);
          temp = ((uint16_t) receive_BT_Array[33]<<8)|receive_BT_Array[34];
          preferences.putShort("New_sensor_Temperature_115",temp);

          preferences.end();
        
          BUS_output(posResponse);
          BUS_output(0x06);
          BUS_output(0x07); 
        }
      }else

      /* This part should set the compare values for Oillevel new sensor
      /* 0x2E 0x06 0x08 0x!! 0x!!  0x!! 0x!! 0x!!......*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x08))
      {
        uint8_t val  = NULL;
        val = receive_BT_Array[3];
        if(val == NULL)
        {
          BUS_output(0x7f);
          BUS_output(0x2e);
          BUS_output(UDS_NRC_incorrectMessageLengthOrInvalidFormat);
        }
        else
        {
          preferences.begin(EEPROMNameSpace, false); 
          uint16_t temp = ((uint16_t) receive_BT_Array[3]<<8)|receive_BT_Array[4];
          preferences.putShort("New_sensor_OilLevelEmpty",temp);
          temp = ((uint16_t) receive_BT_Array[5]<<8)|receive_BT_Array[6];
          preferences.putShort("New_sensor_OilLevel_10",temp);
          temp = ((uint16_t) receive_BT_Array[7]<<8)|receive_BT_Array[8];
          preferences.putShort("New_sensor_OilLevel_20",temp);
          temp = ((uint16_t) receive_BT_Array[9]<<8)|receive_BT_Array[10];
          preferences.putShort("New_sensor_OilLevel_30",temp);
          temp = ((uint16_t) receive_BT_Array[11]<<8)|receive_BT_Array[12];
          preferences.putShort("New_sensor_OilLevel_40",temp);
          temp = ((uint16_t) receive_BT_Array[13]<<8)|receive_BT_Array[14];
          preferences.putShort("New_sensor_OilLevel_50",temp);
          temp = ((uint16_t) receive_BT_Array[15]<<8)|receive_BT_Array[16];
          preferences.putShort("New_sensor_OilLevel_60",temp);
          temp = ((uint16_t) receive_BT_Array[17]<<8)|receive_BT_Array[18];
          preferences.putShort("New_sensor_OilLevel_70",temp);
          temp = ((uint16_t) receive_BT_Array[19]<<8)|receive_BT_Array[20];
          preferences.putShort("New_sensor_OilLevel_80",temp);
          temp = ((uint16_t) receive_BT_Array[21]<<8)|receive_BT_Array[22];
          preferences.putShort("New_sensor_OilLevel_90",temp);
          temp = ((uint16_t) receive_BT_Array[23]<<8)|receive_BT_Array[24];
          preferences.putShort("New_sensor_OilLevelFull",temp);
          preferences.end();
          
          BUS_output(posResponse);
          BUS_output(0x06);
          BUS_output(0x08); 
        }
      }
      else 
      /* This part should set brand for the startuplogo
      /* 0x2E 0x06 0x0A 0x!! */
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x0A))
      {
        uint8_t val  = NULL;
        val = receive_BT_Array[3];
        if(val == NULL)
        {
          BUS_output(0x7f);
          BUS_output(0x2e);
          BUS_output(UDS_NRC_incorrectMessageLengthOrInvalidFormat);
        }
        else
        {
          if((val==BRAND_AUDI_ALT)||(val==BRAND_VW)||(val==BRAND_AUDI_NEU)||(val==BRAND_CHEVY)||(val==BRAND_DODGE)||(val==BRAND_NISSAN_GTT))
          {
            brand = val;
            preferences.begin(EEPROMNameSpace, false); 
            preferences.putUChar("Brand",val);
            preferences.end();
            
            BUS_output(posResponse);
            BUS_output(0x06);
            BUS_output(0x0A); 
          }else{
            BUS_output(0x7f);
            BUS_output(0x2e);
            BUS_output(UDS_NRC_requestOutOfRange);
          }
        }
      }
      else
      
      /* This part should set the value of an Output Pin
      /* 0x2E 0x06 0x0B 0x!! 0x!!  0x!! 0x!! 0x!!......*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x0B))
      {
        uint8_t val  = NULL;
        val = receive_BT_Array[3];
        if(val == NULL)
        {
          BUS_output(0x7f);
          BUS_output(0x2e);
          BUS_output(UDS_NRC_incorrectMessageLengthOrInvalidFormat);
        }
        else
        {
          statusOfExtraOutputPin = val;
          digitalWrite(OutputPin, statusOfExtraOutputPin);
          BUS_output(posResponse);
          BUS_output(0x06);
          BUS_output(0x0B); 
        }
      }
      else


      /* This part should set the old sensor new sensor flag
      /* 0x2E 0x06 0x0C 0x!!......*/
      if((receive_BT_Array[1]==0x06) && (receive_BT_Array[2]==0x0C))
      {

        bool val = NULL;
        val = (bool)receive_BT_Array[3];
        if(val == NULL)
        {
          BUS_output(0x7f);
          BUS_output(0x2e);
          BUS_output(UDS_NRC_incorrectMessageLengthOrInvalidFormat);
        }
        else
        {
          if(NewOilSensorEquipped != val)
          {
            NewOilSensorEquipped = val;
            preferences.begin(EEPROMNameSpace, false); 
            preferences.putBool("NewSensorflag",val);
            preferences.end();
          }

          BUS_output(posResponse);
          BUS_output(0x06);
          BUS_output(0x0C); 
        }
      }else


      {
        BUS_output(0x7F);
        BUS_output(UDS_WRITE_DATA_BY_IDENTIFIER);
        BUS_output(UDS_NRC_requestOutOfRange);
      }
    }else

    /* Session Control */
    if(receive_BT_Array[0] == UDS_Session_Control)
    {
      if((receive_BT_Array[1] == UDS_Session_Control_Default_Session)||(receive_BT_Array[1] == UDS_Session_Control_Extended_Session)||(receive_BT_Array[1] == UDS_Session_Control_Development_Session))
      {
          
          if(session != receive_BT_Array[1])
          {
            session = receive_BT_Array[1];
            preferences.begin(EEPROMNameSpace, false); 
            preferences.putUChar("session",receive_BT_Array[1]);
            preferences.end();
          }
          
          BUS_output(0x50);
      }else{
        BUS_output(0x7F);
        BUS_output(UDS_Session_Control);
        BUS_output(UDS_NRC_subFunctionNotSupported);
      }
    }else
    
    if(receive_BT_Array[0] == UDS_ECU_Reset)
    {
      if(receive_BT_Array[1] == UDS_ECU_Reset_KeyOnOffReset)
      {
        BUS_output(0x51);
        delay(1500);
        ESP.restart();
      }
    }
    else
  
    {
        BUS_output(0x7F);
        BUS_output(receive_BT_Array[0]);
        BUS_output(UDS_NRC_serviceNotSupported);
    }
    delete_BT_buffer();
  }
  NewData = false;
}






