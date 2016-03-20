/* photosensor.cpp
 *
 * Exports helper functions for line following and color detection Assumes pins
 * have been properly setup for IN/OUTPUT
 */

#include "pinmap.h"
#include "photo_sensor.h"

void setupPhotosensor(void) 
{
    pinMode(PHOTOLEFT,        INPUT);
    pinMode(PHOTORIGHT,       INPUT);

    /* red can be on at all times */
    pinMode(PHOTO_BLUE,       OUTPUT);
    digitalWrite(PHOTO_BLUE,  HIGH);
}

static inline bool isBlue(const int red, const bool left) 
{
    if (left) return (816 < red && red < 856);
    else      return (617 < red && red < 657);
}

static inline bool isRed(const int red, const bool left) 
{
    if (left) return (165 < red && red < 245);
    else      return (133 < red && red < 173);
}

static inline bool isYellow(const int red, const bool left) 
{
    if (left) return (480 < red && red < 555);
    else      return (366 < red && red < 416);
}  

/* Read sensor value for red, blue and both */
void readSensors(Colors &left, Colors &right) 
{
    int rR, rL;

    rL = analogRead(PHOTOLEFT);
    rR = analogRead(PHOTORIGHT);

    if      (isBlue(rR,   false)) right = BLUE;
    else if (isRed(rR,    false)) right = RED;
    else if (isYellow(rR, false)) right = YELLOW;
    else                          right = BLACK;

    if      (isBlue(rL,   true)) left = BLUE;
    else if (isRed(rL,    true)) left = RED;
    else if (isYellow(rL, true)) left = YELLOW;
    else                         left = BLACK;
}
