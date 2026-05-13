#include "nrc_uds_protocol.h"
#include <BluetoothSerial.h>
#include "bus.h"

extern BluetoothSerial SerialBT;

void BUS_output(uint8_t data )
{
	SerialBT.write(data);	
};



