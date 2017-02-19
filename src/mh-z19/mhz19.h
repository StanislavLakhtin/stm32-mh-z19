//
// Created by sl on 19.02.17.
//

#ifndef STM32_MH_Z19_MHZ19_H
#define STM32_MH_Z19_MHZ19_H

#include <libopencm3/stm32/usart.h>

#ifndef MH_Z19_USART
#define MH_Z19_USART USART3
#endif

static uint8_t GAS_CONCENTRATION_READ[] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
static uint8_t CALIBRATE_ZERO[] = {0xff, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};

void mhz19_init(void);
void mhz19_readConcentrationCmd(void);
void mhz19_isrHandler(uint8_t data);
uint16_t mhz19_lastConcentration();
uint8_t * mhz19_lastResp();
uint8_t mhz19_checkLastCrc();
void mhz19_calibrateZero();

#endif //STM32_MH_Z19_MHZ19_H
