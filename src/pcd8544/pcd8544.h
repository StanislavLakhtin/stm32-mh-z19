//
// Created by sl on 18.02.17.
//

#ifndef STM32_MH_Z19_PCD8544_H
#define STM32_MH_Z19_PCD8544_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#define PCD8544_RST GPIO1
#define PCD8544_RST_PORT GPIOA

#define PCD8544_DC GPIO1
#define PCD8544_DC_PORT GPIOB
#define PCD8544_SCE GPIO2
#define PCD8544_SCE_PORT PCD8544_DC_PORT

#define PCD8544_SPI_PORT SPI1
#define PCD8544_SPI_MOSI GPIO7
#define PCD8544_SPI_SCK  GPIO5
#define PCD8544_SPI_SS   GPIO4

void pcd8544_init(void);

#endif //STM32_MH_Z19_PCD8544_H
