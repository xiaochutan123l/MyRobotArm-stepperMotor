#ifndef __GPIO_H
#define __GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

// void MX_GPIO_Init(void);

void MX_GPIO_Input_Init(void);
void MX_GPIO_Output_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H */