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

#define TIMER_NUM 3

#define TICKER_TIM1 0
#define TICKER_TIM2 1
#define TICKER_TIM4 2

#define Tick2hz    TICKER_TIM4   
#define Tick10hz   TICKER_TIM1    
#define Tick20khz  TICKER_TIM2  

#define PWM_PERIOD 1000
#define VREF_A TIM_CHANNEL_1
#define VREF_B TIM_CHANNEL_2

#ifdef __cplusplus
}
#endif

#endif /* __PERIPHERAL_CONFIG_H */