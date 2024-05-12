// utils.h
#ifndef __UTILS_H__
#define __UTILS_H__

#include "usart.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// redirect printf to uart
int __io_putchar(int ch);

#ifdef __cplusplus
}
#endif

#endif // __PRINTF_REDIRECTION_H__
