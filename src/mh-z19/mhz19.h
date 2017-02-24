//
// Created by sl on 19.02.17.
//

#ifndef STM32_MH_Z19_MHZ19_H
#define STM32_MH_Z19_MHZ19_H

#include <libopencm3/stm32/usart.h>
#include <stddef.h>

#ifndef MH_Z19_USART
#define MH_Z19_USART USART3
#endif

typedef struct {
  uint8_t CMD;
  uint8_t CM;
  uint8_t HH;
  uint8_t LL;
  uint8_t TT;
  uint8_t SS;
  uint8_t Uh;
  uint8_t Ul;
  uint8_t CS;
} MHZ19_RESPONSE;

#define NOTVALIDREADING 0x01

static uint8_t GAS_CONCENTRATION_READ[] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
static uint8_t CALIBRATE_ZERO[] = {0xff, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};

void mhz19_init(void);
void mhz19_readConcentrationCmd(void);
void mhz19_isrHandler(uint8_t data);
uint16_t mhz19_lastConcentration(uint16_t calibrated);
uint16_t  mhz19_lastTempCelsius();
uint8_t * mhz19_lastResp();
uint16_t  mhz19_lastTempCelsius();
wchar_t * mhz19_lastStatus();
uint8_t mhz19_calcLastCrc();
void mhz19_calibrateZero();

#endif //STM32_MH_Z19_MHZ19_H
