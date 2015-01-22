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
uint8_t magInitDone = 0;

void initAcc()
{
	LSM303DLHCAcc_InitTypeDef AccInitStruct;
	LSM303DLHCAcc_FilterConfigTypeDef AccInitFilterStruct;

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
	if (LSM303DLHC_AccGetDataStatus() == ERROR)
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

void initMagnet(void)
{
	LSM303DLHCMag_InitTypeDef MagnetInitStruct;
	MagnetInitStruct.Temperature_Sensor = LSM303DLHC_TEMPSENSOR_DISABLE;
	MagnetInitStruct.MagOutput_DataRate = LSM303DLHC_ODR_1_5_HZ;
	MagnetInitStruct.Working_Mode = LSM303DLHC_CONTINUOS_CONVERSION;
	MagnetInitStruct.MagFull_Scale = LSM303DLHC_FS_1_3_GA;
	LSM303DLHC_MagInit(&MagnetInitStruct);

	delay(0x100);
	magInitDone = 1;
}

uint16_t getMagValue(enum axis direction)
{
	const uint8_t dev_addr = MAG_I2C_ADDRESS; // | 0x01;
	uint8_t reg_addr_H;
	uint8_t ret[2];

	if (!magInitDone) return 0;

	if (direction == X)		 {	reg_addr_H = LSM303DLHC_OUT_X_H_M; }
	else if (direction == Y) {	reg_addr_H = LSM303DLHC_OUT_Y_H_M; }
	else if (direction == Z) {	reg_addr_H = LSM303DLHC_OUT_Z_H_M; }
	else return 0;

	LSM303DLHC_Read(dev_addr, reg_addr_H, 2, ret);

	return (uint16_t)ret[0] << 8 | ret[1];
}

uint16_t getAccValue(enum axis direction)
{
	const uint8_t dev_addr = ACC_I2C_ADDRESS | 0x01;
	uint8_t reg_addr_H, reg_addr_L;
	uint8_t ret_msb = 0, ret_lsb = 0;

	if (!accInitDone) return 0;

	if (direction == X) 	 {	reg_addr_L = LSM303DLHC_OUT_X_L_A; reg_addr_H = LSM303DLHC_OUT_X_H_A; }
	else if (direction == Y) {	reg_addr_L = LSM303DLHC_OUT_Y_L_A; reg_addr_H = LSM303DLHC_OUT_Y_H_A; }
	else if (direction == Z) {	reg_addr_L = LSM303DLHC_OUT_Z_L_A; reg_addr_H = LSM303DLHC_OUT_Z_H_A; }
	else return 0;

	LSM303DLHC_Read(dev_addr, reg_addr_L, 1, &ret_msb);	// Read low-value
	LSM303DLHC_Read(dev_addr, reg_addr_H, 1, &ret_lsb);	// Read high-value

	return ret_msb << 8 | ret_lsb;
}


uint8_t getMRregister(void)
{
	uint8_t ret;
	LSM303DLHC_Read(ACC_I2C_ADDRESS | 0x01, LSM303DLHC_CRA_REG_M, 1, &ret);
	return ret;
}
