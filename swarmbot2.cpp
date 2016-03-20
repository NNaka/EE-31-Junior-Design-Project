/* swarmbot2.cpp
 *
 * Simulating the actions of swarmbot 2
 */


#include "collision.h"
#include "communication.h"
#include "motor_control.h"
#include "photo_sensor.h"
#include "pinmap.h"

/* State variables */
static bool hitWall = false;

#define PUSH_CONST 300

void bot2(void);

/* Helper routines / functions */
inline void flashLed              (int ledPin);
inline void actionUntilColor      (Colors c, void (*action)(void));
inline bool followColorUntilColor (Colors c1, Colors c2);

void setup() 
{
    /* set up pins */
    pinMode(CALIB_SWITCH, INPUT);
    pinMode(BOT_SWITCH,   INPUT);

    pinMode(RED_LED,      OUTPUT);
    pinMode(YELLOW_LED,   OUTPUT);
    pinMode(GREEN_LED,    OUTPUT);
    pinMode(BLUE_LED,     OUTPUT);

    Serial.begin(9600);

    setupPhotosensor();
    setupMotorControl();
    setupCollision();
    setupCommunication();
}

void loop() {
    delay(1000);
    isBot1 = digitalRead(BOT_SWITCH);

    /* GO state */
    bot2();

    digitalWrite(TRANSMIT_PIN, LOW);
    while (1); /* stop forever */
}

void bot2(void) 
{
    /* Waits for `START` */
    Serial.println("Waiting for start");
    while (!receive(recMsg, MSG_LEN)) {}

    delay(500);

    /* Flash green LED twice */
    Serial.println("Lit green led");
    flashLed(GREEN_LED); /* pauses for 1 second to flash */
    flashLed(GREEN_LED); /* pauses for 1 second to flash */

    /* Puts bot in motion (`forward()`) */
    forward();

    /* Loops until collision (boolean is set in ISR) */
    while (!hitWall) {
        for (int i = 0; i < NUM_BUMPERS; ++i) {
            if (digitalRead(bumpers[i])) {
                hitWall = true;
            }
        }
        delay(1);
    }
    Serial.println("Hit wall");

    /* Moves backward until blue and stops */
    actionUntilColor(BLUE, backward);
    turn(80); /* positive to turn left */
    actionUntilColor(BLUE, turnLeft);

    /* Stops on blue, flashing a blue LED */
    flashLed(BLUE_LED);

    /* Turns a predetermined angle to the right, and follows blue */
    while (!followColorUntilColor(BLUE, YELLOW)) {}
    Serial.println("Found yellow");
    forward();
    delay(PUSH_CONST);
    stop();

    /* Stops on yellow, flashing yellow LED twice */
    flashLed(YELLOW_LED);
    flashLed(YELLOW_LED);

    /* Follows yellow */
    Serial.println("Following yellow until blue");
    while (!followColorUntilColor(YELLOW, BLUE)) {}
    forward();
    delay(PUSH_CONST);
    stop();

    /* Follows blue */
    Serial.println("Following blue until yellow");
    while (!followColorUntilColor(BLUE, YELLOW)) {}

    /* Stops on yellow, turns on yellow LED, turns 180 degrees */
    Serial.println("Turning until blue");
    stop();
    digitalWrite(YELLOW_LED, HIGH);
    turn(-180);
    actionUntilColor(BLUE, turnRight);

    /* Communicates to Bot 1: `TOXIC` */
    transmit(startMsg, MSG_LEN, TIMEOUT);

    /* Flash yellow continuously */
    flashLed(YELLOW_LED);
    digitalWrite(YELLOW_LED, HIGH);

    /* Follows blue, stops on yellow */
    while (!followColorUntilColor(BLUE, YELLOW)) {}
    forward();
    delay(PUSH_CONST);
    stop();

    /* Communicates to Bot 1: `STOP_YELLOW` */
    transmit(startMsg, MSG_LEN, TIMEOUT);

    /* Stop flashing yellow LED */
    digitalWrite(YELLOW_LED, LOW);

    /* Follows yellow until blue */
    while (!followColorUntilColor(YELLOW, BLUE)) {}
    forward();
    delay(PUSH_CONST);
    stop();

    /* Follows blue, stops on yellow */
    while (!followColorUntilColor(BLUE, YELLOW)) {}
    stop();

    /* Communicates to Bot 1: `DONE` */
    transmit(doneMsg, MSG_LEN, TIMEOUT);

    /* Flashes green LED */
    while (1) { flashLed(GREEN_LED); }
}

/* Helper Routines */

inline void flashLed(int ledPin) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
}

inline void actionUntilColor(Colors c, void (*action)(void)) {
    Colors left, right;

    do {
        readSensors(left, right);
        action();
    } while (left != c && right != c);

    stop();
}

inline bool followColorUntilColor(Colors c1, Colors c2) {
    Colors left, right; readSensors(left, right);

    if (left == c2 && right == c2)      { stop(); delay(50); return true; }
    else if (left == c1 && right == c2) turnRight();
    else if (left == c2 && right == c1) turnLeft();
    else if (left == c2 && right == BLACK) forward();
    else if (left == BLACK && right == c2) forward();
    else if (left == c2 && right != c2) turnLeft();
    else if (left != c2 && right == c2) turnRight();
    else if (left == c1 && right == c1) forward();
    else if (left == c1 && right != c1) turnLeft();
    else if (left != c1 && right == c1) turnRight();
    else if (left != c1 && right != c1) {
        Serial.println("Both off c1");
        bool turning = false, turningLeft = !isBot1;
        int i = 0, prevMillis;

        do {
            if (!turning) { 
                prevMillis = millis();
                (turningLeft) ? turnLeft() : turnRight();
                turning = true; turningLeft = !turningLeft;
            }

            /* turn other way after 100*i milliseconds */
            if (millis() > (200 * i) + prevMillis) {
                turning = false; ++i;
            }

            readSensors(left, right);
            if (left == c2 || right == c2) return true;
        } while (left != c1 && right != c1);
        stop();
    }

    delay(150);
    return false;
}

#undef PUSH_CONST
