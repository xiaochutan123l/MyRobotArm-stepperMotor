#ifndef __PERIPHERAL_CONFIG_H
#define __PERIPHERAL_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <cstdint>

// ADC
#define TEMP_SENSOR 0
#define VOLTAGE 1

// Generic gpio

#define GPIOB_PIN_6 6
#define GPIOB_PIN_7 7
#define GPIOB_PIN_8 8
#define GPIOB_PIN_9 9

#define AIN1_Pin GPIOB_PIN_6
#define AIN2_Pin GPIOB_PIN_7
#define BIN1_Pin GPIOB_PIN_8
#define BIN2_Pin GPIOB_PIN_9

#define Tick2hz    4
#define Tick100hz  1
#define Tick20khz  2

#define PWM_PERIOD 1000
#define VREF_A TIM_CHANNEL_1
#define VREF_B TIM_CHANNEL_2

#ifdef __cplusplus
}
#endif

#endif /* __PERIPHERAL_CONFIG_H */