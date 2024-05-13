#ifndef __ADC_H
#define __ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern ADC_HandleTypeDef hadc1;

void MX_ADC1_Init(void);

void ADC_Read_Chanlles(uint16_t *readValue);
// int ADC_Read(uint8_t count);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H */