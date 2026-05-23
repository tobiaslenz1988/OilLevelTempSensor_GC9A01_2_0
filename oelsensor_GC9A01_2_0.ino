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
#include <TJpg_Decoder.h>
#include <SPI.h>
#include <TFT_eSPI.h>


#include "display/Oil_LevelGraphic_GCA901_240_240/oil_level_graphic.h"
#include "display/display_selector.h"
#include "display/logos/logos.h"

//#include "oilsensorled.h"

#include "nrc_uds_protocol_common.h"
#include "uds_statemachine_common.h"
#include "oilsensor_common.h"
#include "brand_defines_common.h"
#include "bus_common.h"
#include "softwareversion.h"
#include "sensors/sensors.h"
#include "dtc_oilsensor.h"


Preferences preferences;
BluetoothSerial SerialBT;
TFT_eSPI tft = TFT_eSPI();


bool Impuls_1_High                      = false;
bool Impuls_1_Low                       = false;
bool Impuls_2_High                      = false;
bool Impuls_2_Low                       = false;

uint8_t session                         = UDS_Session_Control_Default_Session;
uint8_t displayselector                 = DISPLAY_SELECTOR_GC9A01;

uint8_t signalinput                     = 0;
uint16_t cnt                            = 0;
uint8_t oilTemperature                  = OilTemperaturePercentageInitValue; /*init value 254*/
uint8_t oilLevelPercentage              = OilLevelPercentageInitValue; /*init value 254*/
uint8_t testValue_oilTemperature        = 85; /* Debugvalue 255 */
uint8_t testValue_oilLevelPercentage    = 60; /* Debugvalue 255 */
uint8_t lastOilTemp                     = 0;
uint8_t lastOilLevel                    = 0;
uint16_t startUpCounter                 = 0;
uint8_t brandSelector                   = 0;

hw_timer_t *timer                       = NULL;
static uint16_t                         cntRawArr[4];
static uint16_t                         returnArray[4];

      
portMUX_TYPE timerMux                           =  portMUX_INITIALIZER_UNLOCKED;
String  Modulename                              =  {0,0,0,0,0, 0,0,0,0,0 ,0,0,0,0,0, 0,0,0,0,0};
String  HWModuleName                            =  {'-','-','-','-','-', '-','-','-','-','-', '-','-','-','-','-'};// ESP32
String  oemPartNumberOilTempSensor              =  DEFAULT_OEM_PARTNUMBER_OIL_TEMPSENSOR;
String  supplierPartNumberOilTempSensor         =  DEFAULT_SUPPLIER_PARTNUMBER_OIL_TEMPSENSOR;
String  oemPartNumberWaterTempSensor            =  DEFAULT_OEM_PARTNUMBER_WATER_TEMPSENSOR;
String  supplierPartNumberWaterTempSensor       =  DEFAULT_SUPPLIER_PARTNUMBER_WATER_TEMPSENSOR;


uint16_t OldOilTempCompValues[]         = {Old_sensor_Temperature_30,Old_sensor_Temperature_40,Old_sensor_Temperature_50,Old_sensor_Temperature_55,Old_sensor_Temperature_60,Old_sensor_Temperature_65,Old_sensor_Temperature_70,Old_sensor_Temperature_75,Old_sensor_Temperature_80,Old_sensor_Temperature_85,Old_sensor_Temperature_90,Old_sensor_Temperature_95,Old_sensor_Temperature_100,Old_sensor_Temperature_105,Old_sensor_Temperature_110,Old_sensor_Temperature_115};
uint16_t OldOilLevelCompValues[]        = {Old_sensor_OilLevelEmpty,Old_sensor_OilLevel_10,Old_sensor_OilLevel_20,Old_sensor_OilLevel_30,Old_sensor_OilLevel_40,Old_sensor_OilLevel_50,Old_sensor_OilLevel_60,Old_sensor_OilLevel_70,Old_sensor_OilLevel_80,Old_sensor_OilLevel_90,Old_sensor_OilLevelFull};

uint32_t DTC_Storage                    = 0;

bool NewData                            = false;
bool TimeoutSensorDetected              = true;
bool toggleInvertDisplayFlag            = false;
bool NewOilSensorEquipped               = false;
bool statusOfExtraOutputPin             = false;


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
#define time10s_ShowBrandLogo         400

uint8_t BT_rx_buffer[Buffersize];

    

void ARDUINO_ISR_ATTR onTimer() {
  static bool toggleflag;
  /* This toggle is for debug purpose only.. */
  /* You could verify at Pin ISRDebugTogglePin how often the ISR is called. */
  if (toggleflag) {
    digitalWrite(ISRDebugTogglePin, LOW);
    toggleflag =  false;
  } else {
    digitalWrite(ISRDebugTogglePin, HIGH);
    toggleflag = true;
  }
  signalinput = digitalRead(SignalInputPin);
  
  if ((signalinput == 0x01) && (Impuls_1_High == false) && (Impuls_1_Low == false) && (Impuls_2_High == false) && (Impuls_2_Low == false)) {
    //  first high signal
    // T1 
    Impuls_1_High = true;
    TimeoutSensorDetected = false;

  } else if ((signalinput == 0x00) && (Impuls_1_High == true) && (Impuls_1_Low == false) && (Impuls_2_High == false) && (Impuls_2_Low == false)) {
    //  first low signal 
    // T2 
    Impuls_1_Low = true;
    portENTER_CRITICAL(&timerMux);
    cntRawArr[0] = cnt;
    portEXIT_CRITICAL(&timerMux);

  } else if ((signalinput == 0x01) && (Impuls_1_High == true) && (Impuls_1_Low == true) && (Impuls_2_High == false) && (Impuls_2_Low == false)) {
    // 2nd high signal 
    // T3 
    Impuls_2_High = true;
    portENTER_CRITICAL(&timerMux);
    cntRawArr[1] = cnt;
    portEXIT_CRITICAL(&timerMux);

  } else if ((signalinput == 0x00) && (Impuls_1_High == true) && (Impuls_1_Low == true) && (Impuls_2_High == true) && (Impuls_2_Low == false)) {
    // T4 
    Impuls_2_Low = true;
    portENTER_CRITICAL(&timerMux);
    cntRawArr[2] = cnt;
    portEXIT_CRITICAL(&timerMux);

  } else if ((signalinput == 0x01) && (Impuls_1_High == true) && (Impuls_1_Low == true) && (Impuls_2_High == true) && (Impuls_2_Low == true)) {
    // T5 
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
    //String stri = SoftwareVersion;
    SerialBT.println(SOFTWAREVERSION);
  
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
  if((lastOilLevel!=oilLevelPercentage) || (lastOilTemp != oilTemperature))
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
    tft.fillScreen(GC9A01A_BLACK);
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
  if ((startUpCounter>=time10s_ShowBrandLogo) && (startUpCounter<=1000) )
  {
    if(startUpCounter==time10s_ShowBrandLogo){
      tft.fillScreen(GC9A01A_BLACK);
      tft.setTextColor(GC9A01A_WHITE);
      startUpCounter = startUpCounter+1;
    }
    showOilLevelAtDisplay(oilLevelPercentage,false);
  } else
  /* short initialization sequence*/
  if((startUpCounter>=0) && (startUpCounter<time10s_ShowBrandLogo))
  {  
    if(startUpCounter==0)
    {
      showBrandLogo(brandSelector);
    }
    // Position of Text "Check" measured from the Top left corner (0,0) in Pixel 
    tft.setCursor(110, 15);
    tft.print(SOFTWAREVERSION);
    tft.setCursor(90, 205);
    tft.print("created by");
    tft.setCursor(100, 220);
    tft.print("Der Arzt");
    
    startUpCounter = startUpCounter+1;
  }
}
void showBrandLogo(uint8_t brandvalue)
{  
  uint16_t w = 0, h = 0;
  if(brandvalue == BRAND_VW){
    tft.setTextColor(GC9A01A_BLACK);
    TJpgDec.getJpgSize(&w, &h, vw_logo, sizeof(vw_logo));
    TJpgDec.drawJpg(0, 0, vw_logo, sizeof(vw_logo));

  }else if(brandvalue == BRAND_AUDI_ALT){
    tft.fillScreen(GC9A01A_WHITE);
    tft.setTextColor(GC9A01A_BLACK);
    TJpgDec.getJpgSize(&w, &h, audi_alt_logo, sizeof(audi_alt_logo));
    TJpgDec.drawJpg(0, 50, audi_alt_logo, sizeof(audi_alt_logo));

  }else if(brandvalue == BRAND_AUDI_NEU){

  }else if(brandvalue == BRAND_DODGE){
    tft.fillScreen(GC9A01A_BLACK);
    tft.setTextColor(GC9A01A_WHITE);
    TJpgDec.getJpgSize(&w, &h, dodge_logo, sizeof(dodge_logo));
    TJpgDec.drawJpg(0, 115, dodge_logo, sizeof(dodge_logo));

  }else if(brandvalue == BRAND_NISSAN_GTT){
    tft.fillScreen(GC9A01A_WHITE);
    tft.setTextColor(GC9A01A_BLACK);
    TJpgDec.getJpgSize(&w, &h, gtt_logo, sizeof(gtt_logo));
    TJpgDec.drawJpg(5, 50, gtt_logo, sizeof(gtt_logo));

  }else if(brandvalue == BRAND_CHEVY){
    tft.fillScreen(GC9A01A_BLACK);
    tft.setTextColor(GC9A01A_WHITE);
    TJpgDec.getJpgSize(&w, &h, chevy_logo, sizeof(chevy_logo));
    TJpgDec.drawJpg(0, 80, chevy_logo, sizeof(chevy_logo));
    
  }else if(brandvalue == BRAND_BMW){
    tft.fillScreen(GC9A01A_BLACK);
    tft.setTextColor(GC9A01A_WHITE);
    TJpgDec.getJpgSize(&w, &h, bmw_logo, sizeof(bmw_logo));
    TJpgDec.drawJpg(0, 0, bmw_logo, sizeof(bmw_logo));
  }else if(brandvalue == BRAND_MERCEDES_OLD){
    tft.fillScreen(GC9A01A_BLACK);
    tft.setTextColor(GC9A01A_BLACK);
    TJpgDec.getJpgSize(&w, &h, mercedes_old_logo, sizeof(mercedes_old_logo));
    TJpgDec.drawJpg(0, 0, mercedes_old_logo, sizeof(mercedes_old_logo));
  }else if(brandvalue == BRAND_Init){
    
  }
  
}


void readEepromValues()
{
  preferences.clear();
  preferences.begin(EEPROMNameSpace, false);
    
  session                                 = (uint8_t) preferences.getUChar("session",UDS_Session_Control_Default_Session);
  NewOilSensorEquipped                    = preferences.getBool("NewSensorflag",false);
  brandSelector                           = preferences.getUChar("Brand",BRAND_VW);

  Modulename                              =  preferences.getString("Modulename","OilSensor");
  HWModuleName                            =  preferences.getString("HWModuleName",{'-','-','-','-','-',    '-','-','-','-','-',    '-','-','-','-','-' }); 
  oemPartNumberOilTempSensor              =  preferences.getString("PartNumberOilTempSensor",DEFAULT_OEM_PARTNUMBER_OIL_TEMPSENSOR);
  supplierPartNumberOilTempSensor         =  preferences.getString("supplierPartNumberOilTempSensor",DEFAULT_SUPPLIER_PARTNUMBER_OIL_TEMPSENSOR);
  oemPartNumberWaterTempSensor            =  preferences.getString("PartNumberWaterTempSensor", DEFAULT_OEM_PARTNUMBER_WATER_TEMPSENSOR); 
  supplierPartNumberWaterTempSensor       =  preferences.getString("supplierPartNumberWaterTempSensor", DEFAULT_SUPPLIER_PARTNUMBER_WATER_TEMPSENSOR);
  update_DTCStorage();
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

  preferences.end();
}

// This next function will be called during decoding of the jpeg file to
// render each block to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}

void setup() {
  readEepromValues();
  tft.init();

  tft.initDMA();
  //tft.initSPI();
  delay(300);
  
  //start serial connection
  initBtConnection(Modulename);

  //SerialBT = blueSerial(Serial1, true);
  
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
  


    // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);

  // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setSwapBytes(true);

  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);
  
  tft.fillScreen(GC9A01A_BLACK);
  tft.setTextColor(GC9A01A_WHITE);

}


void loop() {
  //ArduinoOTA.handle();
  portENTER_CRITICAL(&timerMux);
  orderImpulse(cntRawArr);
  portEXIT_CRITICAL(&timerMux);

  convertImpulseToPercentage((uint16_t)returnArray[1], (uint16_t)returnArray[3],(uint8_t)session);
  sendInfosToBT(oilTemperature, oilLevelPercentage,returnArray);
  //TempToggle = showLevelAndTempAtLED(TempToggle,oilLevelPercentage,oilTemperature);
  analyse_BT_Protocol(BT_rx_buffer);
 
  controlOfDisplay();
  lastOilTemp = oilTemperature;
  lastOilLevel = oilLevelPercentage;
  delay(10);
  }