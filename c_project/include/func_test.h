#include <stdbool.h>
#include <stdio.h>

void getFECDataLengths(void);
void softwareDACandADC(void);
void testSyncEdgeCases(void);
bool fullSendTest(void);
void testMLSAutoCorrelation(void);
bool fullSendTest_FileIO(void);
float imageSendTest(char * filename);
float checkEfficacy(void);