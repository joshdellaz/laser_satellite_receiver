#include <stdbool.h>
#include <stdio.h>

void setPacketDataLengthInclFEC(void);
void softwareDACandADC(void);
void testSyncEdgeCases(void);
bool sendAndReceiveRandomlyGeneratedPacket(void);
bool fullSendTest_FileIO(void);
float imageSendTest(char * filename);
float checkEfficacy(void);