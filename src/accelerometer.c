/*
 * accelerometer.c
 *
 *  Created on: Jan 19, 2015
 *      Author: EngineeringIsLife
 */

#include "stm32f401_discovery.h"
#include "stm32f401_discovery_lsm303dlhc.h"
#include "accelerometer.h"
#include "main.h"

uint8_t readstream[4];
uint8_t accInitDone = 0;

void initAcc()
{
	LSM303DLHCAcc_InitTypeDef AccInitStruct;
	LSM303DLHCAcc_FilterConfigTypeDef AccInitFilterStruct;

	const uint8_t dev_addr = ACC_I2C_ADDRESS;
	const uint8_t reg_addr = LSM303DLHC_STATUS_REG_A;

	// Initialize LSM303
	AccInitStruct.Power_Mode = LSM303DLHC_NORMAL_MODE;
	AccInitStruct.AccOutput_DataRate = LSM303DLHC_ODR_100_HZ;
	AccInitStruct.Axes_Enable = LSM303DLHC_X_ENABLE | LSM303DLHC_Y_ENABLE;
	AccInitStruct.AccFull_Scale = LSM303DLHC_FULLSCALE_4G;
	AccInitStruct.Endianness = LSM303DLHC_BLE_MSB;
	AccInitStruct.BlockData_Update = LSM303DLHC_BlockUpdate_Continous;
	AccInitStruct.High_Resolution = LSM303DLHC_HR_ENABLE;
	LSM303DLHC_AccInit(&AccInitStruct);

	// Configure internal highpass
	AccInitFilterStruct.HighPassFilter_Mode_Selection = LSM303DLHC_HPM_NORMAL_MODE;
	AccInitFilterStruct.HighPassFilter_CutOff_Frequency = LSM303DLHC_HPFCF_8;
	AccInitFilterStruct.HighPassFilter_AOI1 = LSM303DLHC_HPF_AOI1_DISABLE;
	AccInitFilterStruct.HighPassFilter_AOI2 = LSM303DLHC_HPF_AOI2_DISABLE;
	LSM303DLHC_AccFilterConfig(&AccInitFilterStruct);

	// Wait for end of configuration
	delay(0x100);

	// Try to read status register
	if (LSM303DLHC_Read(dev_addr|0x01, reg_addr, 1, readstream) == ERROR)
	{
		STM_EVAL_LEDToggle(LED3);
		STM_EVAL_LEDToggle(LED4);
		STM_EVAL_LEDToggle(LED5);
		STM_EVAL_LEDToggle(LED6);
	}
	else
		STM_EVAL_LEDToggle(LED3);

	accInitDone = 1;
}

uint16_t getAccValue(enum axis direction)
{
	const uint8_t dev_addr = ACC_I2C_ADDRESS;
	uint8_t reg_addr_H, reg_addr_L;
	uint8_t ret_msb = 0, ret_lsb = 0;

	if (!accInitDone) return 0;

	if (direction == X) 	 {	reg_addr_L = LSM303DLHC_OUT_X_L_A; reg_addr_H = LSM303DLHC_OUT_X_H_A; }
	else if (direction == Y) {	reg_addr_L = LSM303DLHC_OUT_Y_L_A; reg_addr_H = LSM303DLHC_OUT_Y_H_A; }
	else if (direction == Z) {	reg_addr_L = LSM303DLHC_OUT_Z_L_A; reg_addr_H = LSM303DLHC_OUT_Z_H_A; }
	else return 0;

	LSM303DLHC_Read(dev_addr|0x01, reg_addr_L, 1, &ret_msb);	// Read low-value
	LSM303DLHC_Read(dev_addr|0x01, reg_addr_H, 1, &ret_lsb);	// Read high-value

	return ret_msb << 8 | ret_lsb;
}
