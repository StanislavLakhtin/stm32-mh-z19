//
// Created by sl on 18.02.17.
//

#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/f1/nvic.h>
#include <pcd8544.h>
#include <mhz19.h>

static void clock_setup(void) {
  rcc_clock_setup_in_hse_8mhz_out_72mhz();

  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);

  /* Enable SPI2 Periph and gpio clocks */
  rcc_periph_clock_enable(RCC_SPI2);

  rcc_periph_clock_enable(RCC_USART3);
}

static void pcd8544_setup(void) {
  /* Configure GPIOs: SS=PA4, SCK=PA5, MISO=PA6 and MOSI=PA7 */
  gpio_set_mode(PCD8544_SPI_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                PCD8544_SPI_MOSI | PCD8544_SPI_SCK | PCD8544_SPI_SS);
  /* Reset SPI, SPI_CR1 register cleared, SPI is disabled */
  spi_reset(PCD8544_SPI);
  /* Set up SPI in Master mode with:
   * Clock baud rate: 1/64 of peripheral clock frequency
   * Clock: CPOL CPHA (0:0)
   * Data frame format: 8-bit
   * Frame format: MSB First
   */
  spi_init_master(PCD8544_SPI, SPI_CR1_BAUDRATE_FPCLK_DIV_16, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                  SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);

  /*
   * Set NSS management to software.
   *
   * Note:
   * Setting nss high is very important, even if we are controlling the GPIO
   * ourselves this bit needs to be at least set to 1, otherwise the spi
   * peripheral will not send any data out.
   */
  spi_enable_software_slave_management(PCD8544_SPI);
  spi_set_nss_high(PCD8544_SPI);

  /* Enable SPI1 periph. */
  spi_enable(PCD8544_SPI);

  /* Configure GPIOs: DC, SCE, RST */
  gpio_set_mode(PCD8544_RST_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                PCD8544_RST);
  gpio_set_mode(PCD8544_DC_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                PCD8544_DC);
  gpio_set_mode(PCD8544_SCE_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                PCD8544_SCE);

  gpio_set(PCD8544_RST_PORT, PCD8544_RST);
  gpio_set(PCD8544_SCE_PORT, PCD8544_SCE);

  for (int i = 0; i < 200; i++)
      __asm__("nop");

}

static void mhz19_setup(void) {
  nvic_enable_irq(NVIC_USART3_IRQ);

  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART3_TX);
  gpio_set_mode(GPIOB, GPIO_MODE_INPUT,
                GPIO_CNF_INPUT_PULL_UPDOWN, GPIO_USART3_RX);
}

void usart3_isr(void) {
  /* Check if we were called because of RXNE. */
  if (((USART_CR1(MH_Z19_USART) & USART_CR1_RXNEIE) != 0) &&
      ((USART_SR(MH_Z19_USART) & USART_SR_RXNE) != 0)) {

    /* Retrieve the data from the peripheral. */
    mhz19_isrHandler(usart_recv(MH_Z19_USART));
  }
}

int main(void) {

  clock_setup();
  pcd8544_setup();
  mhz19_setup();

  mhz19_init();
  pcd8544_init();
  uint16_t scanPeriod = 0;
  wchar_t buffer[50];

  while (1) {
    if (scanPeriod > 500) {
      scanPeriod = 0;
      mhz19_readConcentrationCmd();
      swprintf(buffer, 50, L"  %d", mhz19_lastConcentration());
      pcd8544_clearDisplay();
      pcd8544_drawText(0, 0, BLACK, L"Концентрация CO2:");
      pcd8544_drawText(0, 8, BLACK, buffer);
      uint8_t *lResp = mhz19_lastResp();
      swprintf(buffer, 50, L"%02x %02x %02x %02x %02x %02x %02x %02x %02x",
               lResp[0],lResp[1],lResp[2],lResp[3],lResp[4],
               lResp[5],lResp[6],lResp[7],lResp[8]);
      pcd8544_drawText(0, 16, BLACK, buffer);
      swprintf(buffer, 50, L"CRC: %02x:%02x", lResp[8],mhz19_checkLastCrc());
      pcd8544_drawText(0, 24, BLACK, buffer);
      pcd8544_display();
    } else {
      scanPeriod++;
    }
    for (int i = 0; i < 200000; i++)
        __asm__("nop");
  }
}