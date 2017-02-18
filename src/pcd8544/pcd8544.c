#include "pcd8544.h"

void delay(uint32_t delay){
  while (delay--){
    __asm__("nop");
  }
}

void setCmdMode(bool v){
  if (v)
    gpio_clear(PCD8544_DC_PORT, PCD8544_DC);
  else
    gpio_set(PCD8544_DC_PORT, PCD8544_DC);
}

void pcd8544_init(void) {
  // SCE is going LOW
  gpio_clear(PCD8544_SCE_PORT, PCD8544_SCE);
  delay(80);
  gpio_set(PCD8544_SCE_PORT, PCD8544_SCE);
  setCmdMode(true);
  spi_send(PCD8544_SPI_PORT, 0x21); // function set PD = 0 and V = 0, select extended instruction set (H = 1 mode)
  spi_send(PCD8544_SPI_PORT, 0x90); // set VOP; VOP is set to a +16 × b [V]
  spi_send(PCD8544_SPI_PORT, 0x20); // function set PD = 0 and V = 0, select normal instruction set (H = 0 mode)
  spi_send(PCD8544_SPI_PORT, 0x0c); // display control set normal mode (D = 1 and E = 0)
  setCmdMode(false);
  for (int i=0; i<84; i++)
    spi_send(PCD8544_SPI_PORT, i);
}