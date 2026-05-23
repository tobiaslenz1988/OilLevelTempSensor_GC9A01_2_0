#ifndef DTC_OILSENSOR_H
#define DTC_OILSENSOR_H

#include <stdio.h>

void update_DTCStorage(void);
void deleteDTC (void);
void setDTC(uint32_t DTCBit);
bool readDTC(uint32_t DTCBit);
uint32_t readFullDTC(void);



#endif /* DTC_OILSENSOR_H */