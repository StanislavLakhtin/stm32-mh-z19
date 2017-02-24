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
volatile uint8_t mhz19LastResponse[9];
wchar_t *statusString = L"неизвестен";

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
  mhz19LastResponse[mhz19_cmdIndex] = data;
  mhz19_cmdIndex++;
}

uint16_t mhz19_lastConcentration(uint16_t calibrated) {
  MHZ19_RESPONSE * r = (MHZ19_RESPONSE *) mhz19LastResponse;
  return r->HH * 256 + r->LL -calibrated;
}

uint16_t  mhz19_lastTempCelsius() {
  MHZ19_RESPONSE * r = (MHZ19_RESPONSE *) mhz19LastResponse;
  return r->TT - 40;
}

uint8_t *mhz19_lastResp() {
  return &mhz19LastResponse;
}

wchar_t * mhz19_lastStatus() {
  MHZ19_RESPONSE * r = (MHZ19_RESPONSE *) mhz19LastResponse;
  statusString = L"........";
  uint8_t ss = r->SS;
  for (uint8_t i=0; i<8;i++) {
    if (ss & 0x01)
      statusString[7-i] = L'l';
    else
      statusString[7-i] = L'0';
    ss >>= 1;
  }
  return statusString;
}


uint8_t mhz19_calcLastCrc() {
  return 0xff - (mhz19LastResponse[1] + mhz19LastResponse[2] + mhz19LastResponse[3] +
                 mhz19LastResponse[4] + mhz19LastResponse[5] + mhz19LastResponse[6] + mhz19LastResponse[7])
         + 0x01;
}
