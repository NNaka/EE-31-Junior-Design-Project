/* communication.h */
/* Contains macro definitions to map pins */

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include "Arduino.h"

/* pin set up */
#define COMM_INTERR  0    /* pin 2 */
#define RECEIVE_PIN  4
#define TRANSMIT_PIN 12
/* TODO: move appropriately to pinmap.h */

/* message formats */
static const unsigned hsMsg[]    =   { 1,1,1,1,0,0,0,0 };
static const unsigned transMsg[] =   { 1,1,0,0,1,1,1,0 };
static const unsigned ack[]      =   { 0,0,0,0,1,1,1,1 };
static unsigned recMsg[]         =   { 0,0,0,0,0,0,0,0,
                                       0,0,0,0,0,0,0,0 };

/* function declarations */
void setReceiving(void);
void transmit(unsigned msg[], unsigned len);
void sendBits(unsigned bits[], unsigned len);
void receive(unsigned bits[], unsigned len);
bool checkMsg(unsigned msgToCheck[], unsigned correctMsg[], unsigned len);

#undef COMM_INTERR
#undef RECEIVE_PIN
#undef TRANSMIT_PIN
#endif // _COMMUNICATION_H
