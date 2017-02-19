//
// Created by sl on 19.02.17.
//

#include "mhz19.h"

void usart_setup(uint32_t usart, uint32_t baud, uint32_t bits, uint32_t stopbits, uint32_t mode, uint32_t parity,
                 uint32_t flowcontrol) {
  usart_disable(usart);

  // Настраиваем
  usart_set_baudrate(usart, baud);
  usart_set_databits(usart, bits);
  usart_set_stopbits(usart, stopbits);
  usart_set_mode(usart, mode);
  usart_set_parity(usart, parity);
  usart_set_flow_control(usart, flowcontrol);

  /* Enable USART Receive interrupt. */
  USART_CR1(usart) |= USART_CR1_RXNEIE;

  usart_enable(usart);
}

volatile uint8_t mhz19_cmdIndex = 0;
volatile uint8_t mhz19_lastResponse[9];

void mhz19_init(void) {
  usart_setup(MH_Z19_USART, 9600, 8, USART_STOPBITS_1, USART_MODE_TX_RX, USART_PARITY_NONE, USART_FLOWCONTROL_NONE);
}

void mhz19_readConcentrationCmd(void) {
  for (int i = 0; i < 9; i++)
    usart_send_blocking(MH_Z19_USART, GAS_CONCENTRATION_READ[i]);
  mhz19_cmdIndex = 0;
}

void mhz19_calibrateZero() {
  for (int i = 0; i < 9; i++)
    usart_send_blocking(MH_Z19_USART, CALIBRATE_ZERO[i]);
}

void mhz19_isrHandler(uint8_t data) {
  if (mhz19_cmdIndex >= 9)
    return;
  mhz19_lastResponse[mhz19_cmdIndex] = data;
  mhz19_cmdIndex++;
}

uint16_t mhz19_lastConcentration() {
  return mhz19_lastResponse[2] * 256 + mhz19_lastResponse[3];
}

uint8_t *mhz19_lastResp() {
  return &mhz19_lastResponse;
}

uint8_t mhz19_checkLastCrc() {
  return 0xff - (mhz19_lastResponse[1] + mhz19_lastResponse[2] + mhz19_lastResponse[3] +
                 mhz19_lastResponse[4] + mhz19_lastResponse[5] + mhz19_lastResponse[6] + mhz19_lastResponse[7])
         + 0x01;
}
