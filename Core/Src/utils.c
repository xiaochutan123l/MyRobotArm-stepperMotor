#include "utils.h"

// 声明外部 UART 句柄，假设您已经在某处（如 main.c 或类似文件）初始化了这个句柄
extern UART_HandleTypeDef huart1;  // 修改为您使用的 UART 句柄

// 重写 __io_putchar 函数来重定向 printf 的输出
int __io_putchar(int ch) {
    // 检查是否为新行符号，若是则添加回车符（CR），因为一些终端需要CR-LF
    if (ch == '\n') {
        uint8_t retChar = '\r';
        HAL_UART_Transmit(&huart1, &retChar, 1, HAL_MAX_DELAY);
    }
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}