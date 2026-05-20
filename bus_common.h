#ifndef BUS_COMMON_H
#define BUS_COMMON_H
#include <cstdint>
#include <BluetoothSerial.h>

void BUS_output(uint8_t data);

void initBtConnection(String name);


#endif /* BUS_COMMON_H */