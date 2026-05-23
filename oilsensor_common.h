#ifndef OILSENSOR_COMMON_H
#define OILSENSOR_COMMON_H
#include <Preferences.h>

#define New_sensor_OilLevelEmpty      0x1011
#define New_sensor_OilLevel_10        0x1213
#define New_sensor_OilLevel_20        0x1415
#define New_sensor_OilLevel_30        0x1617
#define New_sensor_OilLevel_40        0x1819
#define New_sensor_OilLevel_50        0x1A1B
#define New_sensor_OilLevel_60        0x1C1D
#define New_sensor_OilLevel_70        0x1E1F
#define New_sensor_OilLevel_80        0x2021
#define New_sensor_OilLevel_90        0x2223
#define New_sensor_OilLevelFull       0x2425


#define New_sensor_Temperature_30     0x3031
#define New_sensor_Temperature_40     0x3233
#define New_sensor_Temperature_50     0x3435
#define New_sensor_Temperature_55     0x3637
#define New_sensor_Temperature_60     0x3839
#define New_sensor_Temperature_65     0x3a3b
#define New_sensor_Temperature_70     0x3c3d
#define New_sensor_Temperature_75     0x3e3f
#define New_sensor_Temperature_80     0x4041
#define New_sensor_Temperature_85     0x4243
#define New_sensor_Temperature_90     0x4445
#define New_sensor_Temperature_95     0x4647
#define New_sensor_Temperature_100    0x4849
#define New_sensor_Temperature_105    0x4a4b
#define New_sensor_Temperature_110    0x4c4d
#define New_sensor_Temperature_115    0x4e4f


#define Old_sensor_OilLevelEmpty      0x4041
#define Old_sensor_OilLevel_10        0x4243
#define Old_sensor_OilLevel_20        0x4445
#define Old_sensor_OilLevel_30        0x4647
#define Old_sensor_OilLevel_40        0x4849
#define Old_sensor_OilLevel_50        0x4A4B
#define Old_sensor_OilLevel_60        0x4C4D
#define Old_sensor_OilLevel_70        0x4E4F
#define Old_sensor_OilLevel_80        0x5051
#define Old_sensor_OilLevel_90        0x5253
#define Old_sensor_OilLevelFull       0x5455


#define Old_sensor_Temperature_30     0x5051
#define Old_sensor_Temperature_40     0x5253
#define Old_sensor_Temperature_50     0x5455
#define Old_sensor_Temperature_55     0x5657
#define Old_sensor_Temperature_60     0x5859
#define Old_sensor_Temperature_65     0x5A5B
#define Old_sensor_Temperature_70     0x5C5D
#define Old_sensor_Temperature_75     0x5e5f
#define Old_sensor_Temperature_80     0x6061
#define Old_sensor_Temperature_85     0x6263
#define Old_sensor_Temperature_90     0x6465
#define Old_sensor_Temperature_95     0x6667
#define Old_sensor_Temperature_100    0x6869
#define Old_sensor_Temperature_105    0x7071
#define Old_sensor_Temperature_110    0x7273
#define Old_sensor_Temperature_115    0x7475
                    
                                                                                        
                                        

#define OilLevelPercentageErrorValue                  0xFF
#define OilTemperaturePercentageErrorValue      0xFF

#define OilLevelPercentageInitValue                   0xFE
#define OilTemperaturePercentageInitValue       0xFE


#define OilLevelPercentageDebugValue                0xFD
#define OilTemperaturePercentageDebugValue      0xFD


extern bool TimeoutSensorDetected;
extern bool NewOilSensorEquipped; 

extern uint8_t oilTemperature;
extern uint8_t oilLevelPercentage ;



extern uint8_t  testValue_oilTemperature;
extern uint8_t  testValue_oilLevelPercentage;
extern uint16_t OldOilTempCompValues[16];
extern uint16_t OldOilLevelCompValues[11];
extern uint16_t NewOilTempCompValues[16];
extern uint16_t NewOilLevelCompValues[11];
void convertImpulseToPercentage(uint16_t cntRawData_Temp, uint16_t cntRawData_Level,uint8_t session);

#endif /* OILSENSOR_COMMON_H */