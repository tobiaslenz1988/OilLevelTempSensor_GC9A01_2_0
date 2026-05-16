/*
  This example is for the combined OilLevel/Temoerature Sensor G266 of the VAG Family.
  There are two types of the Sensor:

  The "old" Version, used in 
  -Audi A4 (B5) 1.8 20V 
  -Audi A2 
  -VW Transporter T4
  -VW Golf 5 1.9 TDI
  -etc...

  The old Version has the Part Numbers 
  1J0 907 660 
  1J0 907 660 A
  1J0 907 660 B
  1J0 907 660 C
  1J0 907 660 F

  The old Version has a metal big Plate at the Bottom and the Tube to measure the oil is ONE PIECE and is formed in a rectangular form and NOT Square.





  The "new" Version, used in 
  - Audi A4, A6 from 2010
  - Audi Q5 from 2010
  - etc

  The new Version has the Part Numbers 
  03C 907 660 M


  In the new Version the Bottom is made of plastic and the Tube to measure the oil is Two PIECES. The Tube itself is formed in a H-Design and at the top of the Tube is a square 2nd Part



  This code is based on the Information from https://www.mikrocontroller.net/topic/459687 (which uses the "old" Version)

  The Sensor has 3 Pins

  Pinning:
  Pin    Pin    Pin 
   1      2      3
  12V    Gnd    Signal (High 200mV)


  - As Default Pin 2 used as an Input for the Signal of the Sensor 
  - As Default Pin 4 kind of Debug Pin to verify how often the Signalinput ISR is triggered
  - As Default Pin 13 is used for an LED for the Oil level which turns on if theres is to low 
  - As Default Pin 15 is used for an LED to show the oil Temperature
    The LED is on if Engine is two cold
    The LED is off if Engine is in normal temperature area
    The LED toggles if the Engine is too Hot

  Circuit:
  Between the 3.3V output from the ESP 32n is a 4.7K Ohm resistor as a Pullup resistor.
  The Ground if the Sensor is connected to Ground of the Sensor
  The 

*/

/*
The Signal looks like the following

  ______      _______                                   __
  |     |     |     |                                  |
  |     |     |     |                                  |
  |     |     |     |                                  |
__|     |_____|     |__________________________________|
T1   T2   T3    T4                  T5                    T1
*/


#include <BluetoothSerial.h>
#include <Preferences.h>
#include <stdio.h>
#include <string.h>

#include <SPI.h>
#include <TFT_eSPI.h>


#include "Oil_LevelGraphic_GCA901_240_240/_0percent_inv8.h"
#include "Oil_LevelGraphic_GCA901_240_240/_10percent_inv8.h"
#include "Oil_LevelGraphic_GCA901_240_240/_20percent_inv8.h"
#include "Oil_LevelGraphic_GCA901_240_240/_30percent_inv8.h"
#include "Oil_LevelGraphic_GCA901_240_240/_40percent_inv8.h"
#include "Oil_LevelGraphic_GCA901_240_240/_50percent_inv8.h"
#include "Oil_LevelGraphic_GCA901_240_240/_60percent_inv8.h"
#include "Oil_LevelGraphic_GCA901_240_240/_70percent_inv8.h"
#include "Oil_LevelGraphic_GCA901_240_240/_80percent_inv8.h"
#include "Oil_LevelGraphic_GCA901_240_240/_90percent_inv8.h"
#include "Oil_LevelGraphic_GCA901_240_240/_100percent_inv8.h"
#include "logos/brand_defines.h"
#include "logos/audi_alt_1.h"
#include "logos/audi_alt_2.h"
#include "logos/vw.h"
#include "oilsensor.h"
//#include "oilsensorled.h"
#include "NRC_UDS_protocol.h"
#include "uds_statemachine.h"


Preferences preferences;
BluetoothSerial SerialBT;


TFT_eSPI tft = TFT_eSPI();


bool Impuls_1_High                      = false;
bool Impuls_1_Low                       = false;
bool Impuls_2_High                      = false;
bool Impuls_2_Low                       = false;
uint8_t session                         = UDS_Session_Control_Default_Session;
bool NewOilSensorEquipped               = false;
bool toggleflag                         = false;
uint8_t signalinput                     = 0;
uint16_t cnt                            = 0;
uint8_t oilTemperature                  = OilTemperaturePercentageInitValue; /*init value 254*/
uint8_t oilLevelPercentage              = OilLevelPercentageInitValue; /*init value 254*/
uint8_t testValue_oilTemperature        = 85; /* Debugvalue 255 */
uint8_t testValue_oilLevelPercentage    = 60; /* Debugvalue 255 */
uint8_t oldOilTemp                      = 0;
uint8_t oldOilLevel                     = 0;
uint32_t startUpCounter                 = 0;
uint8_t brand                           = 0;
uint8_t startingsequence[]              = {0,50,100,50,0};
hw_timer_t *timer                       = NULL;
static uint16_t                         cntRawArr[4];
static uint16_t                         returnArray[4];
bool TempToggle                         = false;
bool TimeoutSensorDetected              = true;
      
portMUX_TYPE timerMux                   = portMUX_INITIALIZER_UNLOCKED;
String  Modulename                      =  {0,0,0,0,0, 0,0,0,0,0 ,0,0,0,0,0, 0,0,0,0,0};
bool NewData                            = false;
bool statusOfExtraOutputPin             = false;
bool toggleInvertDisplayFlag            = false;
uint8_t coding[]                        = {0,0,0,0,0,0,0,0};
uint16_t OldOilTempCompValues[]         = {Old_sensor_Temperature_30,Old_sensor_Temperature_40,Old_sensor_Temperature_50,Old_sensor_Temperature_55,Old_sensor_Temperature_60,Old_sensor_Temperature_65,Old_sensor_Temperature_70,Old_sensor_Temperature_75,Old_sensor_Temperature_80,Old_sensor_Temperature_85,Old_sensor_Temperature_90,Old_sensor_Temperature_95,Old_sensor_Temperature_100,Old_sensor_Temperature_105,Old_sensor_Temperature_110,Old_sensor_Temperature_115};
uint16_t OldOilLevelCompValues[]        = {Old_sensor_OilLevelEmpty,Old_sensor_OilLevel_10,Old_sensor_OilLevel_20,Old_sensor_OilLevel_30,Old_sensor_OilLevel_40,Old_sensor_OilLevel_50,Old_sensor_OilLevel_60,Old_sensor_OilLevel_70,Old_sensor_OilLevel_80,Old_sensor_OilLevel_90,Old_sensor_OilLevelFull};

uint16_t NewOilTempCompValues[]         = {New_sensor_Temperature_30,New_sensor_Temperature_40,New_sensor_Temperature_50,New_sensor_Temperature_55,New_sensor_Temperature_60,New_sensor_Temperature_65,New_sensor_Temperature_70,New_sensor_Temperature_75,New_sensor_Temperature_80,New_sensor_Temperature_85,New_sensor_Temperature_90,New_sensor_Temperature_95,New_sensor_Temperature_100,New_sensor_Temperature_105,New_sensor_Temperature_110,New_sensor_Temperature_115};
uint16_t NewOilLevelCompValues[]        = {New_sensor_OilLevelEmpty,New_sensor_OilLevel_10,New_sensor_OilLevel_20,New_sensor_OilLevel_30,New_sensor_OilLevel_40,New_sensor_OilLevel_50,New_sensor_OilLevel_60,New_sensor_OilLevel_70,New_sensor_OilLevel_80,New_sensor_OilLevel_90,New_sensor_OilLevelFull};

#define SOFTWAREVERSION               "Y006"
//#define EEPROMNameSpace               "my_variables"
#define SignalInputPin                2
#define ISRDebugTogglePin             4


#define timerfrequency                1000000
#define ISRfrequency                  1000
#define TimeoutSignalMS               1500

#define SCREEN_WIDTH                  240
#define SCREEN_HEIGHT                 240
#define IMAGE_WIDTH                   20
#define IMAGE_HEIGHT                  150
/* Because the Display is round we need an offset*/
#define OFFSET_IMAGE_X                42
#define OFFSET_IMAGE_Y                42 
#define GC9A01A_BLACK                 0x0000
#define GC9A01A_WHITE                 0xFFFF
#define GC9A01A_RED                   0xF800
#define GC9A01A_BLUE                  0x001F
uint8_t BT_rx_buffer[Buffersize];
char SoftwareVersion[]                  = SOFTWAREVERSION;
    
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//Adafruit_GC9A01A tft(17, 18,19,15,16,23);

/*
Adafruit_GC9A01A::Adafruit_GC9A01A(int8_t cs, int8_t dc, int8_t DIN/SDA,
                                   int8_t sclk, int8_t rst, int8_t miso)
*/



void ARDUINO_ISR_ATTR onTimer() {
  /* This toggle is for debug purpose only.. */
  /* You could verify at Pin ISRDebugTogglePin how often the ISR is called. */
  if (toggleflag == false) {
    digitalWrite(ISRDebugTogglePin, LOW);
    toggleflag = true;
  } else {
    digitalWrite(ISRDebugTogglePin, HIGH);
    toggleflag = false;
  }
  signalinput = digitalRead(SignalInputPin);


  if ((signalinput == 0x01) && (Impuls_1_High == false) && (Impuls_1_Low == false) && (Impuls_2_High == false) && (Impuls_2_Low == false)) {
    /*  first high signal*/
    /* T1 */
    Impuls_1_High = true;
    TimeoutSensorDetected = false;
  } else if ((signalinput == 0x00) && (Impuls_1_High == true) && (Impuls_1_Low == false) && (Impuls_2_High == false) && (Impuls_2_Low == false)) {
    /*  first low signal*/
    /* T2 */
    Impuls_1_Low = true;
    portENTER_CRITICAL(&timerMux);
    cntRawArr[0] = cnt;
    portEXIT_CRITICAL(&timerMux);

  } else if ((signalinput == 0x01) && (Impuls_1_High == true) && (Impuls_1_Low == true) && (Impuls_2_High == false) && (Impuls_2_Low == false)) {
    /* 2nd high signal*/
    /* T3 */
    Impuls_2_High = true;
    portENTER_CRITICAL(&timerMux);
    cntRawArr[1] = cnt;
    portEXIT_CRITICAL(&timerMux);

  } else if ((signalinput == 0x00) && (Impuls_1_High == true) && (Impuls_1_Low == true) && (Impuls_2_High == true) && (Impuls_2_Low == false)) {
    /* T4 */
    Impuls_2_Low = true;
    portENTER_CRITICAL(&timerMux);
    cntRawArr[2] = cnt;
    portEXIT_CRITICAL(&timerMux);

  } else if ((signalinput == 0x01) && (Impuls_1_High == true) && (Impuls_1_Low == true) && (Impuls_2_High == true) && (Impuls_2_Low == true)) {
    /* T5 */
    Impuls_1_High = false;
    Impuls_2_High = false;
    Impuls_1_Low  = false;
    Impuls_2_Low  = false;
    portENTER_CRITICAL(&timerMux);
    cntRawArr[3] = cnt;
    portEXIT_CRITICAL(&timerMux);
    cnt = 1;
  }

  if (Impuls_1_High == true) {
    cnt = cnt + 1;
  }


  /* if sensor is disconnected -> cnt is higher than TimeoutSignalMS 1500->set to 0xFE*/
  if(cnt>TimeoutSignalMS)
  { 
    cntRawArr[0] = OilLevelPercentageErrorValue;
    cntRawArr[1] = OilLevelPercentageErrorValue;
    cntRawArr[2] = OilLevelPercentageErrorValue;
    cntRawArr[3] = OilLevelPercentageErrorValue;
    TimeoutSensorDetected = true;
    cnt = 1;
  }
}


void  orderImpulse(uint16_t inputArr[]) {
  /* This Method orders the measured impulses into the correct sequence..*/
  /* There might be the case that the Uc starts at the wrong Time and interprets T4 (see at ine 71) as the start of the sequence ..*/
  /* This method returns a sequence of Four measured where the beginning of the Signal is always stored in returnArray[0] */
  /* SerialBT.print(testval >> 8);         -> 0x01 */
  /* SerialBT.print(testval & 0xFF);       -> 0xF4 */
 
  uint16_t tempArray[4];
  tempArray[0] = inputArr[0];
  tempArray[1] = inputArr[1] - inputArr[0];
  tempArray[2] = inputArr[2] - inputArr[1];
  tempArray[3] = inputArr[3] - inputArr[2];

    /*detect Big LOW T5*/
    if (tempArray[0] > 100) {
        returnArray[0] = tempArray[1]; /* 1) 20ms  High   T2 */
        returnArray[1] = tempArray[2]; /* 2) 20ms  Low    T3 */
        returnArray[2] = tempArray[3]; /* 3) 20ms  High   T4 */
        returnArray[3] = tempArray[0]; /* 4) 160ms BigLow T5 */
    }
    if (tempArray[1] > 100) {
        returnArray[0] = tempArray[2]; /* 1) 20ms  High   T2 */
        returnArray[1] = tempArray[3]; /* 2) 20ms  Low    T3 */
        returnArray[2] = tempArray[0]; /* 3) 20ms  High   T4 */
        returnArray[3] = tempArray[1]; /* 4) 160ms BigLow T5 */
    }
    if (tempArray[2] > 100) {
        returnArray[0] = tempArray[3]; /* 1) 20ms  High    T2 */
        returnArray[1] = tempArray[0]; /* 2) 20ms  Low     T3 */
        returnArray[2] = tempArray[1]; /* 3) 20ms  High    T4 */
        returnArray[3] = tempArray[2]; /* 4) 160ms BigLow  T5 */
    }
    if (tempArray[3] > 100) {
        /* Best case only copy */
        returnArray[0] = tempArray[0]; /* 1) 20ms  High    T2 */
        returnArray[1] = tempArray[1]; /* 2) 20ms  Low     T3 */
        returnArray[2] = tempArray[2]; /* 3) 20ms  High    T4 */
        returnArray[3] = tempArray[3]; /* 4) 160ms BigLow  T5 */
    }
}




void sendInfosToBT(uint8_t temperature, uint8_t OilLevel,uint16_t outputarr[]) {
  if (session == UDS_Session_Control_Development_Session)
  {
    uint8_t i;
    //Serial.write(0xFF);
    for (i = 0; i < 4; i++) 
    {
      if (i == 0) {
        SerialBT.print("T2:");
      }
      if (i == 1) {
        SerialBT.print("T3:");
      }
      if (i == 2) {
        SerialBT.print("T4:");
      }
      if (i == 3) {
        SerialBT.print("T5:");
      }
      SerialBT.println(outputarr[i]);
    }
    SerialBT.println("");
  
    SerialBT.print("Softwareversion");
    String stri = SoftwareVersion;
    SerialBT.println(stri);
  
    SerialBT.print("Sensor TO detected: ");
    SerialBT.println(TimeoutSensorDetected);
    SerialBT.print("startUpCounter");
    SerialBT.println(startUpCounter);
 
    SerialBT.print("Temperature: ");
    SerialBT.println(temperature);
    SerialBT.print("OilLevel: ");
    SerialBT.println(OilLevel);
    delay(1000);
  }
}



void getBTData(const uint8_t *buffer, size_t size)
{
  uint8_t i=0;
  for(i=0;i<size;i++){
    BT_rx_buffer[i] = buffer[i];
  }
  NewData = true;
}

void showOilLevelAtDisplay(uint8_t percentageOillevel,bool initflag)
{
  if((oldOilLevel!=oilLevelPercentage) || (oldOilTemp != oilTemperature))
  {
     tft.fillScreen(GC9A01A_BLACK);
     tft.setTextColor(GC9A01A_WHITE);
  }
  if((TimeoutSensorDetected == false) || ( ((TimeoutSensorDetected == true) && (session == UDS_Session_Control_Extended_Session)) || ((TimeoutSensorDetected == true) && (session == UDS_Session_Control_Development_Session))) )
  {  
      //if(percentageOillevel == 00){tft.drawBitmap(0, 0, image_OilLevel_00, 128, 64, 1);} else
      //if(percentageOillevel == 10){ tft.drawBitmap(0, 0, image_OilLevel_10, 128, 64, 1);} else
    /* Oillevel Ok */
    if(percentageOillevel >= 20){
        tft.invertDisplay(true);
      /* Design Reason Background is Dark all Text is White*/
      if(percentageOillevel == 20){tft.drawBitmap(OFFSET_IMAGE_X, OFFSET_IMAGE_Y, image_OilLevel_20, IMAGE_WIDTH, IMAGE_HEIGHT,GC9A01A_RED);} else
      if(percentageOillevel == 30){tft.drawBitmap(OFFSET_IMAGE_X, OFFSET_IMAGE_Y, image_OilLevel_30, IMAGE_WIDTH, IMAGE_HEIGHT,GC9A01A_WHITE);} else
      if(percentageOillevel == 40){tft.drawBitmap(OFFSET_IMAGE_X, OFFSET_IMAGE_Y, image_OilLevel_40, IMAGE_WIDTH, IMAGE_HEIGHT,GC9A01A_WHITE);} else
      if(percentageOillevel == 50){tft.drawBitmap(OFFSET_IMAGE_X, OFFSET_IMAGE_Y, image_OilLevel_50, IMAGE_WIDTH, IMAGE_HEIGHT,GC9A01A_WHITE);} else
      if(percentageOillevel == 60){tft.drawBitmap(OFFSET_IMAGE_X, OFFSET_IMAGE_Y, image_OilLevel_60, IMAGE_WIDTH, IMAGE_HEIGHT,GC9A01A_WHITE);} else
      if(percentageOillevel == 70){tft.drawBitmap(OFFSET_IMAGE_X, OFFSET_IMAGE_Y, image_OilLevel_70, IMAGE_WIDTH, IMAGE_HEIGHT,GC9A01A_WHITE);} else
      if(percentageOillevel == 80){tft.drawBitmap(OFFSET_IMAGE_X, OFFSET_IMAGE_Y, image_OilLevel_80, IMAGE_WIDTH, IMAGE_HEIGHT,GC9A01A_WHITE);} else
      if(percentageOillevel == 90){tft.drawBitmap(OFFSET_IMAGE_X, OFFSET_IMAGE_Y, image_OilLevel_90, IMAGE_WIDTH, IMAGE_HEIGHT,GC9A01A_WHITE);} else
      if(percentageOillevel == 100){tft.drawBitmap(OFFSET_IMAGE_X, OFFSET_IMAGE_Y, image_OilLevel_100, IMAGE_WIDTH, IMAGE_HEIGHT,GC9A01A_WHITE);} 
      tft.setTextSize(2);
      
      tft.setTextColor(GC9A01A_WHITE);
      tft.setCursor(67, 37);
      tft.print("MAX");

      tft.setTextColor(GC9A01A_WHITE);
      tft.setCursor(67, 185);
      tft.print("MIN");

      if(oilTemperature<100)
      {
        tft.setTextColor(GC9A01A_WHITE);
        tft.setTextSize(4);
        tft.setCursor(105, 105);
        tft.print(oilTemperature);
        tft.setCursor(165, 85);
        tft.print(char(248));
        tft.setCursor(180, 105);
        tft.print("C");
      }else{
        tft.setTextColor(GC9A01A_RED);
        tft.setTextSize(4);
        tft.setCursor(105, 105);
        tft.print(oilTemperature);
        tft.setCursor(175, 85);
        tft.print(char(248));
        tft.setCursor(190, 105);
        tft.print("C");
      }


    }else{
      if(initflag==false)
      {
         /* Oillevel not Ok */
        if(percentageOillevel == 00){ tft.drawBitmap(OFFSET_IMAGE_X, OFFSET_IMAGE_Y, image_OilLevel_00, IMAGE_WIDTH, IMAGE_HEIGHT,GC9A01A_RED);} else
        if(percentageOillevel == 10){ tft.drawBitmap(OFFSET_IMAGE_X, OFFSET_IMAGE_Y, image_OilLevel_10, IMAGE_WIDTH, IMAGE_HEIGHT,GC9A01A_RED);} 
        tft.setTextColor(GC9A01A_WHITE);
        tft.setTextSize(2);
      
        tft.setCursor(67, 37);
        tft.print("MAX");
        
        tft.setCursor(67, 185);
        tft.print("MIN");

        tft.setTextSize(4);
        /* Position of Text "Check" measured from the Top left corner (0,0) in Pixel */
        tft.setCursor(80, 90);
        tft.print("Check");
        tft.setCursor(98, 130);
        tft.print("Oil");
        if(toggleInvertDisplayFlag== false)
        {
          toggleInvertDisplayFlag = true;
          tft.invertDisplay(true);
          delay(1000);
        }else{
          toggleInvertDisplayFlag = false;
          tft.invertDisplay(false);
          delay(1000);
        }
      }
    }
  }else{
        /* timeout detected*/
        tft.setTextSize(4);
        /* Position of Text "Check" measured from the Top left corner (0,0) in Pixel */
        tft.setCursor(55, 65);
        tft.print("Sensor");
        tft.setCursor(100, 130);
        tft.print("OL");
        
        if(toggleInvertDisplayFlag == false)
        {
          toggleInvertDisplayFlag = true;
          tft.invertDisplay(true);
          delay(1000);
        }else{
          toggleInvertDisplayFlag = false;
          tft.invertDisplay(false);
          delay(1000);
        }
  }
}

void controlOfDisplay()
{
  //tft.fillScreen(GC9A01A_BLACK);
  if ((startUpCounter>=20) && (startUpCounter<=25) )
  {
    if(startUpCounter==20){
      tft.fillScreen(GC9A01A_BLACK);
      tft.setTextColor(GC9A01A_WHITE);
      startUpCounter = startUpCounter+1;
    }
    showOilLevelAtDisplay(oilLevelPercentage,false);
  } 
  /* short initialization sequence*/
  if((startUpCounter>=0) && (startUpCounter<20))
  {  
    //tft.drawXBitmap(0, 50, audi_logo, logoWidth, logoHeight,GC9A01A_BLACK,GC9A01A_WHITE);
    //tft.drawXBitmap(0, 0, vw_logo, logoWidth, logoHeight,GC9A01A_BLUE,GC9A01A_WHITE);
    showBrandLogo(brand);
    startUpCounter = startUpCounter+1;
  }
}
void showBrandLogo(uint8_t brandvalue)
{
  if(brandvalue == BRAND_VW){
    tft.drawXBitmap(0, 0, vw_logo, vwlogoWidth, vwlogoHeight,GC9A01A_BLUE,GC9A01A_WHITE);
  }else if(brandvalue == BRAND_AUDI_ALT){
    tft.drawXBitmap(0, 60, audi_alt_1, audi_logo_alt_1_Width, audi_logo_alt_1_Height,GC9A01A_WHITE,GC9A01A_BLACK);
    tft.drawXBitmap(0, 160, audi_alt_2, audi_logo_alt_2_Width, audi_logo_alt_2_Height,GC9A01A_RED,GC9A01A_BLACK);
  }else if(brandvalue == BRAND_AUDI_NEU){
    tft.drawXBitmap(0, 60, audi_alt_1, audi_logo_alt_1_Width, audi_logo_alt_1_Height,GC9A01A_WHITE,GC9A01A_BLACK);

  }else if(brandvalue == BRAND_NISSAN_Skyline){

  }else if(brandvalue == BRAND_NISSAN_GTT){
   
  }else if(brandvalue == BRAND_CHEVY){

  }else if(brandvalue == BRAND_Init){

  }
}


void readEepromValues()
{
  preferences.clear();
  preferences.begin(EEPROMNameSpace, false);
    
  session               = preferences.getUChar("session",UDS_Session_Control_Default_Session);
  NewOilSensorEquipped  = preferences.getBool("NewSensorflag",false);

  
  uint8_t leng;
  String temp;
 /*
  temp = preferences.getString("SW_Version","1234");
  leng = temp.length() +1;
  temp.toCharArray(SoftwareVersion, leng);
  */

    
     Modulename =  preferences.getString("Modulename","1111");

      /*
        temp.toCharArray(Modulename, leng);
        SerialBT.write(temp);
        uint8_t sizeOfArr = sizeof(Modulename) / sizeof(Modulename[0]);
      */

  OldOilTempCompValues[0] = preferences.getUShort("Old_sensor_Temperature_30",Old_sensor_Temperature_30);
  OldOilTempCompValues[1] = preferences.getUShort("Old_sensor_Temperature_40",Old_sensor_Temperature_40);
  OldOilTempCompValues[2] = preferences.getUShort("Old_sensor_Temperature_50",Old_sensor_Temperature_50);
  OldOilTempCompValues[3] = preferences.getUShort("Old_sensor_Temperature_55",Old_sensor_Temperature_55);
  OldOilTempCompValues[4] = preferences.getUShort("Old_sensor_Temperature_60",Old_sensor_Temperature_60);
  OldOilTempCompValues[5] = preferences.getUShort("Old_sensor_Temperature_65",Old_sensor_Temperature_65);
  OldOilTempCompValues[6] = preferences.getUShort("Old_sensor_Temperature_70",Old_sensor_Temperature_70);
  OldOilTempCompValues[7] = preferences.getUShort("Old_sensor_Temperature_75",Old_sensor_Temperature_75);
  OldOilTempCompValues[8] = preferences.getUShort("Old_sensor_Temperature_80",Old_sensor_Temperature_80);
  OldOilTempCompValues[9] = preferences.getUShort("Old_sensor_Temperature_85",Old_sensor_Temperature_85);
  OldOilTempCompValues[10] = preferences.getUShort("Old_sensor_Temperature_90",Old_sensor_Temperature_90);
  OldOilTempCompValues[11] = preferences.getUShort("Old_sensor_Temperature_95",Old_sensor_Temperature_95);
  OldOilTempCompValues[12] = preferences.getUShort("Old_sensor_Temperature_100",Old_sensor_Temperature_100);
  OldOilTempCompValues[13] = preferences.getUShort("Old_sensor_Temperature_105",Old_sensor_Temperature_105);
  OldOilTempCompValues[14] = preferences.getUShort("Old_sensor_Temperature_110",Old_sensor_Temperature_110);
  OldOilTempCompValues[15] = preferences.getUShort("Old_sensor_Temperature_115",Old_sensor_Temperature_115);

  OldOilLevelCompValues[0] = preferences.getUShort("Old_sensor_OilLevelEmpty",Old_sensor_OilLevelEmpty);
  OldOilLevelCompValues[1] = preferences.getUShort("Old_sensor_OilLevel_10",Old_sensor_OilLevel_10);
  OldOilLevelCompValues[2] = preferences.getUShort("Old_sensor_OilLevel_20",Old_sensor_OilLevel_20);
  OldOilLevelCompValues[3] = preferences.getUShort("Old_sensor_OilLevel_30",Old_sensor_OilLevel_30);
  OldOilLevelCompValues[4] = preferences.getUShort("Old_sensor_OilLevel_40",Old_sensor_OilLevel_40);
  OldOilLevelCompValues[5] = preferences.getUShort("Old_sensor_OilLevel_50",Old_sensor_OilLevel_50);
  OldOilLevelCompValues[6] = preferences.getUShort("Old_sensor_OilLevel_60",Old_sensor_OilLevel_60);
  OldOilLevelCompValues[7] = preferences.getUShort("Old_sensor_OilLevel_70",Old_sensor_OilLevel_70);
  OldOilLevelCompValues[8] = preferences.getUShort("Old_sensor_OilLevel_80",Old_sensor_OilLevel_80);
  OldOilLevelCompValues[9] = preferences.getUShort("Old_sensor_OilLevel_90",Old_sensor_OilLevel_90);
  OldOilLevelCompValues[10] = preferences.getUShort("Old_sensor_OilLevelFull",Old_sensor_OilLevelFull);

  NewOilTempCompValues[0] = preferences.getUShort("New_sensor_Temperature_30",New_sensor_Temperature_30);
  NewOilTempCompValues[1] = preferences.getUShort("New_sensor_Temperature_40",New_sensor_Temperature_40);
  NewOilTempCompValues[2] = preferences.getUShort("New_sensor_Temperature_50",New_sensor_Temperature_50);
  NewOilTempCompValues[3] = preferences.getUShort("New_sensor_Temperature_55",New_sensor_Temperature_55);
  NewOilTempCompValues[4] = preferences.getUShort("New_sensor_Temperature_60",New_sensor_Temperature_60);
  NewOilTempCompValues[5] = preferences.getUShort("New_sensor_Temperature_65",New_sensor_Temperature_65);
  NewOilTempCompValues[6] = preferences.getUShort("New_sensor_Temperature_70",New_sensor_Temperature_70);
  NewOilTempCompValues[7] = preferences.getUShort("New_sensor_Temperature_75",New_sensor_Temperature_75);
  NewOilTempCompValues[8] = preferences.getUShort("New_sensor_Temperature_80",New_sensor_Temperature_80);
  NewOilTempCompValues[9] = preferences.getUShort("New_sensor_Temperature_85",New_sensor_Temperature_85);
  NewOilTempCompValues[10] = preferences.getUShort("New_sensor_Temperature_90",New_sensor_Temperature_90);
  NewOilTempCompValues[11] = preferences.getUShort("New_sensor_Temperature_95",New_sensor_Temperature_95);
  NewOilTempCompValues[12] = preferences.getUShort("New_sensor_Temperature_100",New_sensor_Temperature_100);
  NewOilTempCompValues[13] = preferences.getUShort("New_sensor_Temperature_105",New_sensor_Temperature_105);
  NewOilTempCompValues[14] = preferences.getUShort("New_sensor_Temperature_110",New_sensor_Temperature_110);
  NewOilTempCompValues[15] = preferences.getUShort("New_sensor_Temperature_115",New_sensor_Temperature_115);

  NewOilLevelCompValues[0] = preferences.getUShort("New_sensor_OilLevelEmpty",New_sensor_OilLevelEmpty);
  NewOilLevelCompValues[1] = preferences.getUShort("New_sensor_OilLevel_10",New_sensor_OilLevel_10);
  NewOilLevelCompValues[2] = preferences.getUShort("New_sensor_OilLevel_20",New_sensor_OilLevel_20);
  NewOilLevelCompValues[3] = preferences.getUShort("New_sensor_OilLevel_30",New_sensor_OilLevel_30);
  NewOilLevelCompValues[4] = preferences.getUShort("New_sensor_OilLevel_40",New_sensor_OilLevel_40);
  NewOilLevelCompValues[5] = preferences.getUShort("New_sensor_OilLevel_50",New_sensor_OilLevel_50);
  NewOilLevelCompValues[6] = preferences.getUShort("New_sensor_OilLevel_60",New_sensor_OilLevel_60);
  NewOilLevelCompValues[7] = preferences.getUShort("New_sensor_OilLevel_70",New_sensor_OilLevel_70);
  NewOilLevelCompValues[8] = preferences.getUShort("New_sensor_OilLevel_80",New_sensor_OilLevel_80);
  NewOilLevelCompValues[9] = preferences.getUShort("New_sensor_OilLevel_90",New_sensor_OilLevel_90);
  NewOilLevelCompValues[10] = preferences.getUShort("New_sensor_OilLevelFull",New_sensor_OilLevelFull);

  
  brand = preferences.getUChar("Brand",BRAND_VW);
  preferences.end();
}



void setup() {
  readEepromValues();
  tft.init();

  tft.initDMA();
  //tft.initSPI();
  delay(300);
  //start serial connection
  SerialBT.begin(Modulename);
  
  /*configure pin SignalInputPin from #define as an input and enable the internal pull-up resistor*/
  pinMode(SignalInputPin, INPUT_PULLUP);

  /*configure pin ISRDebugTogglePin from #define as an Output to check how often ISR is called*/
  pinMode(ISRDebugTogglePin, OUTPUT);

  /*configure pin OilLevelLED from #define as an Output to proof that oil Level is ok*/
  //pinMode(OilLevelLED, OUTPUT);

  /*configure pin OilTemperatureLED from #define as an Output to proof that oil Temperature is ok*/
  //pinMode(OilTemperatureLED, OUTPUT);

  /*configure Pin for extra Output ok*/
  pinMode(OutputPin, OUTPUT);

  // Set timer frequency to 1Mhz
  timer = timerBegin(timerfrequency);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter) with unlimited count = 0 (fourth parameter).
  timerAlarm(timer, 1000, true, 0);

  SerialBT.onData(getBTData);
  /* Design Reason Background is Dark all Text is White*/
  
  tft.fillScreen(GC9A01A_BLACK);
  tft.setTextColor(GC9A01A_WHITE);
}


void loop() {
  portENTER_CRITICAL(&timerMux);
  orderImpulse(cntRawArr);
  portEXIT_CRITICAL(&timerMux);

  convertImpulseToPercentage(returnArray[1], returnArray[3],session);
  sendInfosToBT(oilTemperature, oilLevelPercentage,returnArray);
  //TempToggle = showLevelAndTempAtLED(TempToggle,oilLevelPercentage,oilTemperature);
  analyse_BT_Protocol(BT_rx_buffer);
 
  controlOfDisplay();
  oldOilTemp = oilTemperature;
  oldOilLevel = oilLevelPercentage;
  delay(10);
  }