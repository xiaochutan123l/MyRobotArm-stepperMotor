#ifndef __CAN_H
#define __CAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern CAN_HandleTypeDef hcan;

void MX_CAN_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H */