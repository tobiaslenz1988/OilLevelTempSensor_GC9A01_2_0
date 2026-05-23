#include "dtc_oilsensor.h"
#include <Preferences.h>

extern uint32_t DTC_Storage;
extern Preferences preferences;

void deleteDTC (void)
{
    DTC_Storage = 0x0;
}

void setDTC(uint32_t DTCBit)
{
    DTC_Storage = preferences.getUInt("DTC_Storage",0x0);
    DTC_Storage |= DTCBit;
    preferences.putUInt("DTC_Storage",DTC_Storage);
}


bool readDTC(uint32_t DTCBit)
{
    bool returnval = false;
    if((DTC_Storage & DTCBit) == 1)
    {
        returnval = true;
    }
    return returnval;
}

uint32_t readFullDTC(void)
{
    return DTC_Storage;
}

void update_DTCStorage(void)
{
    DTC_Storage = preferences.getUInt("DTC_Storage",0x0);
}