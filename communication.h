/* communication.h */
/* Contains macro definitions to map pins */

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include "Arduino.h"
#include "pinmap.h"

/* function declarations */
void setReceiving(void);
void transmit(unsigned msg[], unsigned len);
void sendBits(unsigned bits[], unsigned len);
void receive(unsigned bits[], unsigned len);
bool checkMsg(unsigned msgToCheck[], unsigned correctMsg[], unsigned len);

#endif // _COMMUNICATION_H
