/* motor_control.h
 *
 * This file exposes the motor control interface to implement motion control
 */

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "pinmap.h"

void setupMotorControl(void);

/* sets/gets angular speed of bot tire */
void set_speed(const int s);
int  get_speed(void);

/* move the bot forward until another call is made */
void forward(void);

/* move the bot backward until another call is made */
void backward(void);

/* turn the bot angle degrees, and stops */
void turn(const int angle);

// turn bot left until stopped
void turnLeft(void);
void turnLeft(const int speed);

// turn bot right until stopped
void turnRight(void);
void turnRight(const int speed);

/* stops the bot */
void stop(void);

#endif // MOTOR_CONTROL_H
