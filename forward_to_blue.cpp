#ifndef _INCL_GUARD
#define _INCL_GUARD

#include "Arduino.h"

// Photosensor pins

#define RED_LED      3
#define BLUE_LED     2
#define SENSOR_PORT  A0

// Motor Control pins

#define ENABLE_1     12
#define ENABLE_2     7
#define INPUT_1      11
#define INPUT_2      9
#define INPUT_3      6
#define INPUT_4      5

#endif // _INCL_GUARD

// Arduino constants
static const unsigned int NUM_READINGS (5);
static const unsigned int POWER        (35);
static const unsigned int MAX_POWER    (255);
static const unsigned int DELAY_TIME   (15);

// necessary globals due to Arduino loop structure
static int index        (0);   // the index of the current reading
static int blue_total   (0);   // the running total
static int red_total    (0);
static int blue_average (0);   // the average
static int red_average  (0);
static int red[NUM_READINGS];  // readings
static int blue[NUM_READINGS];

// Boolean values / flags
unsigned int b (0), r (0), y (0);

// the setup function runs once when you press reset or power the board
void setup()
{
    // Color detection pins
    Serial.begin(9600);
    pinMode(RED_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(SENSOR_PORT, INPUT);

    // Motor pins going to H-bridge
    pinMode(ENABLE_1, OUTPUT);
    pinMode(INPUT_1, OUTPUT);
    pinMode(INPUT_2, OUTPUT);
    pinMode(ENABLE_2, OUTPUT);
    pinMode(INPUT_3, OUTPUT);
    pinMode(INPUT_4, OUTPUT);

    // initialize array values to 0
    for (unsigned int thisReading(0); thisReading < NUM_READINGS;
            ++thisReading) { 
        red[thisReading]  (0);
        blue[thisReading] (0);
    }
}

// the loop function runs over and over again forever
void loop()
{
    read();

    forward();  

    // if bot hits blue, then stop
    if (b) stop();
}

void read()
{
    blue_total -= blue[index];
    red_total  -= red[index];

    digitalWrite(RED_LED, HIGH);   // turn the LED off by making the voltage LOW
    digitalWrite(BLUE_LED, LOW);

    delay(DELAY_TIME); 

    red[index] = analogRead(SENSOR_PORT);
    red_total += red[index];

    delay(DELAY_TIME);    

    digitalWrite(RED_LED, LOW);   // turn the LED off by making the voltage LOW
    digitalWrite(BLUE_LED, HIGH);

    delay(DELAY_TIME);

    blue[index] = analogRead(SENSOR_PORT);
    blue_total += blue[index];
    ++index;

    if (index >= NUM_READINGS) {
        index = 0;
        Serial.print("Red: ");
        Serial.println(red_average);

        Serial.print("Blue: ");
        Serial.println(blue_average);

        if (620 <= red_average && red_average <= 720 && 
            760 <= blue_average && blue_average <= 860) {
            b = 0; r = 1; y = 0;
        }
        else if (800 <= red_average && red_average <= 900 && 
                 650 <= blue_average && blue_average <=  800) {
            b = 1; r = 0; y = 0;
        }
        else if (460 <= red_average && red_average <= 560  && 
                 590 <= blue_average && blue_average <= 690) {
            b = 0; r = 0; y = 1;
        }
        else  b = r = y = 0;

        Serial.print("b, r, y: ");
        Serial.print(b);
        Serial.print(r);
        Serial.println(y);
    }

    delay(DELAY_TIME);

    blue_average /= NUM_READINGS;
    red_average  /= NUM_READINGS; 
}

// note: HIGH / LOW constant key words seem to only work with LED output pins

void forward()
{
    analogWrite(ENABLE_1, 0);
    analogWrite(ENABLE_2, 0);

    analogWrite(INPUT_1, 0);
    analogWrite(INPUT_3, 0);

    analogWrite(INPUT_2, POWER);
    analogWrite(INPUT_4, POWER);

    analogWrite(ENABLE_1, MAX_POWER);
    analogWrite(ENABLE_2, MAX_POWER);
}

void backward()
{
    analogWrite(ENABLE_1, 0);
    analogWrite(ENABLE_2, 0);

    analogWrite(INPUT_2, 0);
    analogWrite(INPUT_4, 0);

    analogWrite(INPUT_1, POWER);
    analogWrite(INPUT_3, POWER);

    analogWrite(ENABLE_1, MAX_POWER);
    analogWrite(ENABLE_2, MAX_POWER);
}

void rotate(const int angle)
{
    analogWrite(ENABLE_1, 0);
    analogWrite(ENABLE_2, 0);

    if (angle > 0) {
        analogWrite(INPUT_2, 0);
        analogWrite(INPUT_3, 0);

        analogWrite(INPUT_1, POWER);
        analogWrite(INPUT_4, POWER);
    }
    else {
        analogWrite(INPUT_1, 0);
        analogWrite(INPUT_4, 0);

        analogWrite(INPUT_2, POWER);
        analogWrite(INPUT_3, POWER);
    }

    analogWrite(ENABLE_1, MAX_POWER);
    analogWrite(ENABLE_2, MAX_POWER);
}

void stop()
{
    analogWrite(ENABLE_1, 0);
    analogWrite(ENABLE_2, 0);

    analogWrite(INPUT_1, 0);
    analogWrite(INPUT_3, 0);

    analogWrite(INPUT_2, 0);
    analogWrite(INPUT_4, 0);
}

#undef RED_LED
#undef BLUE_LED
#undef SENSOR_PORT
#undef ENABLE_1
#undef ENABLE_2
#undef INPUT_1
#undef INPUT_2
#undef INPUT_3
#undef INPUT_4
