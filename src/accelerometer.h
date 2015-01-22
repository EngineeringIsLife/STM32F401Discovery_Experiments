/*
 * accelerometer.h
 *
 *  Created on: Jan 19, 2015
 *      Author: HerrHorst
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

enum direction { RIGHT, LEFT, UP, DOWN, HORIZ_MID, VERTIC_MID };
enum axis {X, Y, Z};

void initAcc(void);
void initMagnet(void);
uint16_t getAccValue(enum axis direction);
uint16_t getMagValue(enum axis direction);

uint8_t getMRregister(void);

#endif /* ACCELEROMETER_H_ */
