#ifndef __PERIPHERAL_CONFIG_H
#define __PERIPHERAL_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <cstdint>

// ADC
#define TEMP_SENSOR 0
#define VOLTAGE 0

// Generic gpio

#define GPIOB_PIN_6 6
#define GPIOB_PIN_7 7
#define GPIOB_PIN_8 8
#define GPIOB_PIN_9 9

#define AIN1_Pin GPIOB_PIN_6
#define AIN2_Pin GPIOB_PIN_7
#define BIN1_Pin GPIOB_PIN_8
#define BIN2_Pin GPIOB_PIN_9

#ifdef __cplusplus
}
#endif

#endif /* __PERIPHERAL_CONFIG_H */