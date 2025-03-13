/*
 * setup.h
 *
 *  Created on: 1 de jul de 2022
 *      Author: Ever_
 */

#ifndef APPLICATION_SETUP_H_
#define APPLICATION_SETUP_H_
#include "F28x_Project.h"
#include "F2837xD_GlobalPrototypes.h"

#include "../inc/hw_types.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_can.h"
#include "../driverlib/can.h"

void setup_GPIO(void);
void setup_ePWM(void);
void setup_ADC_A(void);
void setup_ADC_B(void);
void Setup_UART(void);
void Setup_CAN(void);

#define CPU_FREQ        200E6
#define LSPCLK_FREQ     CPU_FREQ/4
#define SCI_FREQ        115200
#define SCI_PRD         (LSPCLK_FREQ/(SCI_FREQ*8))-1

#endif /* APPLICATION_SETUP_H_ */
