/*
 * accelerometer.h
 *
 *  Created on: Jan 19, 2015
 *      Author: HerrHorst
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

enum axis {X, Y, Z};

void initAcc();
uint16_t getAccValue(enum axis direction);

#endif /* ACCELEROMETER_H_ */
