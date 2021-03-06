/* communication.cpp
 *
 * Contains interface for communciation of bots 
 */

/* data constants */
#define CARRIER      5
#define DATA_DELAY   416  /* 1/(1.2*10^3)/2 = 416 aka 1.2 kHz */
#define BIT_SIZE     25
#define MSG_LEN      128
#define TIMEOUT      1000

#include "Arduino.h"
#include "communication.h"
#include "pinmap.h"

static bool receivingBot = true;  /* COM 9 receiving --> true; COM 12
                                   * transmitting --> false 
                                   */
static bool receiving    = false;

void setup() 
{
    pinMode(RECEIVE_PIN,  INPUT);
    pinMode(TRANSMIT_PIN, OUTPUT);
    pinMode(CARRIER,      OUTPUT);

    /* use interrupts on the input comm pin to manage state of communication
     * subsystem
     */
    attachInterrupt(COMM_INTERR, setReceiving, RISING);

    Serial.begin(9600);
    /* sets COM Output Mode to FastPWM with toggle of OC3A on compare match
     * with OCR3A also sets WGM to mode 15: FastPWM with top set by OCR3A
     */
    TCCR3A = _BV(COM3A0) | _BV(COM3B0) | _BV(WGM30) | _BV(WGM31);

    /* sets WGM as stated above; sets clock scaling to "divide by 8"
     */
    TCCR3B = _BV(WGM32) | _BV(WGM33) |  _BV(CS31);

    /* Used to be 63 above sets the counter value at which register resets
     * to 0x0000; generate 20kHz when OCR3A=50
     */
    OCR3A = 55; 
}

void loop() 
{
    if (receivingBot) {
        /* execute code to anticipate and respond to messages */
        while (!receiving) { delayMicroseconds(1); } /* idle until we see a
                                                      * handshake */

        /* if here, we've seen a waiting falling edge AND a receiving rising
         * edge
         */
        receive(recMsg, MSG_LEN);

        /* testing only */
        for (unsigned i(0); i < 2 * MSG_LEN; ++i) Serial.print(recMsg[i]);

        Serial.print("\n"); 
    }
    else { /* TRANSMITTING BOT */
        unsigned long currTime(millis());

        transmit(transMsg, MSG_LEN);
        while (!receiving) {
            if (millis() - currTime > TIMEOUT) { /* execute code to transmit a
                                                  * regular message 
                                                  */
                transmit(transMsg, MSG_LEN);
                currTime = millis();
            } /* wait until acknowledgement */
            delayMicroseconds(1);
        }
        receive(recMsg, MSG_LEN);

        if (checkMsg(recMsg, ack, MSG_LEN)) {
            /* testing only */
            for (unsigned i(0); i < 2 * MSG_LEN; ++i) Serial.print(recMsg[i]);
            Serial.print("\n");

            delay(2000);
        }
    }
}

void setReceiving(void) { receiving = true; }

void transmit(unsigned msg[], const unsigned len) 
{
    sendBits(hsMsg, MSG_LEN); /* sending handshake so other bot knows message
                               * is coming */
    sendBits(msg, len);
    digitalWrite(TRANSMIT_PIN, LOW); /* ensure low afterwards */
}

void sendBits(unsigned bits[], const unsigned len) 
{
    for (unsigned i(0); i < len; ++i) {
        for (unsigned j(0); j < BIT_SIZE; ++j) {
            digitalWrite(TRANSMIT_PIN, (bits[i]) ? HIGH : LOW);
            delayMicroseconds(DATA_DELAY); 
        }    
    }
}

void receive(unsigned bits[], const unsigned len) 
{
    unsigned sum;
    for (unsigned i(0); i < 2 * len; ++i) {
        sum = 0;
        for (unsigned j(0); j < BIT_SIZE; ++j) {
            /* read all 25 bits, recording sum */
            int x = digitalRead(RECEIVE_PIN);
            sum += x;
            delayMicroseconds(DATA_DELAY);
        }
        /* average sum of 25 bits, adding value to bit[i] */
        bits[i] = ((float)sum / BIT_SIZE > 0.5);
    }
    receiving = false; /* we are receiving, not waiting */
    if (receivingBot) transmit(ack, MSG_LEN);
}

bool checkMsg(unsigned msgToCheck[], unsigned correctMsg[], const unsigned len) 
{
    for (unsigned i(0); i < len; ++i) {
        if (hsMsg[i] != msgToCheck[i] || msgToCheck[i+len] != correctMsg[i]) {
            return false;
        }
    }
    return true;
}

#undef CARRIER
#undef DATA_DELAY
#undef BIT_SIZE
#undef MSG_LEN
#undef TIMEOUT
