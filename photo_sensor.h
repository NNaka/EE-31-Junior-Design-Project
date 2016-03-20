/* photosensor.h
 *
 * Helper macros and function delcarations and enums defined
 */

#ifndef PHOTO_SENSOR_H
#define PHOTO_SENSOR_H

enum Colors { RED, BLUE, YELLOW, BLACK };

void readSensors      (Colors &left, Colors &right);
void setupPhotosensor (void);

#endif // PHOTO_SENSOR_H
