
#include <BluetoothSerial.h>
#include "bus_common.h"
#include "nrc_uds_protocol_common.h"


extern BluetoothSerial SerialBT;
extern bool NewData;
extern uint8_t BT_rx_buffer[];

void BUS_output(uint8_t data )
{
	SerialBT.write(data);	
}
/*
void initBtConnection(String name)
{
	  SerialBT.begin(name);
}

*/


