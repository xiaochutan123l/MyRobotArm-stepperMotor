// #include "main.h"
// #include "sysInit.h"
// #include "utils.h"

// #include "adc.hpp"
// #include "generic_gpio.hpp"
// #include "spi.hpp"
// #include "uart_dma.hpp"
// #include "timer.hpp"
// #include "pwm.hpp"

// uint16_t MT6816Read();
// void stepMotor(int step);
// void stepMotorStop();

// SPI spi;

// Generic_GPIO ain1(AIN1_Pin);
// Generic_GPIO ain2(AIN2_Pin);
// Generic_GPIO bin1(BIN1_Pin);
// Generic_GPIO bin2(BIN2_Pin);

// PWM vref_a(VREF_A);
// PWM vref_b(VREF_B);

// UartDMA uart;

// int main() {
//     System_Init();

//     spi.init();

//     ain1.init();
//     ain2.init();
//     bin1.init();
//     bin2.init();

//     vref_a.init();
//     vref_b.init();
//     vref_a.setVoltage(100);
//     vref_b.setVoltage(100);

//     uart.init();
//     HAL_Delay(100);
    
//     while (1) {
//         int rotations = 100;
//         for(int i = 0; i < rotations * 8; i++)
//         {
//             stepMotor(i);
//             HAL_Delay(1); // 控制步进速度
//             //printf(" 7MT6816 value: %u\n", MT6816Read());
//         }
//         printf(" 7MT6816 value: %u\n", MT6816Read());
//         stepMotorStop();
//         HAL_Delay(1000);
        
//         for(int i = rotations * 8 - 1; i >= 0; i--)
//         {
//             stepMotor(i);
//             HAL_Delay(1); // 控制步进速度
//             //printf(" 7MT6816 value: %u\n", MT6816Read());
//         }
//         printf(" 7MT6816 value: %u\n", MT6816Read());
//         stepMotorStop();
//         HAL_Delay(1000);
//     }

//     return 0;
// }

// uint16_t MT6816Read(void) {
//   uint16_t data_t[2];
//   uint16_t data_r[2];
//   uint16_t rawData;
//   data_t[0] = (0x80 | 0x03) << 8;
//   data_t[1] = (0x80 | 0x04) << 8;
//   //dataTx[0] = (0x80 | 0x03) << 8;
//   //dataTx[1] = (0x80 | 0x04) << 8;
//   uint16_t rawAngle;
//   //uint16_t rectifiedAngle;
//   bool checksumFlag = false;

//   for (uint8_t i = 0; i < 3; i++) {
//     // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
//     // //GPIOA->BRR = GPIO_PIN_4;
//     // HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) &data_t[0], (uint8_t*) &data_r[0], 1, HAL_MAX_DELAY);
//     // //GPIOA->BSRR = GPIO_PIN_4;
//     // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

//     // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
//     // //GPIOA->BRR = GPIO_PIN_4;
//     // HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) &data_t[1], (uint8_t*) &data_r[1], 1, HAL_MAX_DELAY);
//     // //GPIOA->BSRR = GPIO_PIN_4;
//     // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
//     data_r[0] = spi.read(data_t[0]);
//     data_r[1] = spi.read(data_t[1]);

//     rawData = ((data_r[0] & 0x00FF) << 8) | (data_r[1] & 0x00FF);
    
//     uint16_t hCount = 0;
//         for (uint8_t j = 0; j < 16; j++)
//         {
//             if (rawData & (0x0001 << j))
//                 hCount++;
//         }
//         if (hCount & 0x01)
//         {
//             checksumFlag = false;
//         } else
//         {
//             checksumFlag = true;
//             break;
//         }
//   }

//   //printf(" 0 rawData%u\n", rawData);

//   if (checksumFlag) {
//     rawAngle = rawData >> 2;
//     //printf(" 8MT6816 %u, %u\n", data_r[0], data_r[1]);
//     //no_mag_flag = (bool) (rawData & 0x0010);
//     //printf(" 8MT6816 read ok\n");
//   }
//   else {
//     //printf(" 8MT6816 read not ok\n");
//   }
//   return rawAngle;
// }

// void stepMotor(int step)
// {
//     switch(step % 8)
//     {
//         case 0:
//             ain1.setHigh();
//             ain2.setLow();
//             bin1.setLow();
//             bin2.setLow();

//             // HAL_GPIO_WritePin(GPIOB, AIN1_Pin, GPIO_PIN_SET);
//             // HAL_GPIO_WritePin(GPIOB, AIN2_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, BIN1_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, BIN2_Pin, GPIO_PIN_RESET);
//             break;
//         case 1:
//             ain1.setHigh();
//             ain2.setLow();
//             bin1.setHigh();
//             bin2.setLow();
//             // HAL_GPIO_WritePin(GPIOB, AIN1_Pin, GPIO_PIN_SET);
//             // HAL_GPIO_WritePin(GPIOB, AIN2_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, BIN1_Pin, GPIO_PIN_SET);
//             // HAL_GPIO_WritePin(GPIOB, BIN2_Pin, GPIO_PIN_RESET);
//             break;
//         case 2:
//             ain1.setLow();
//             ain2.setLow();
//             bin1.setHigh();
//             bin2.setLow();
//             // HAL_GPIO_WritePin(GPIOB, AIN1_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, AIN2_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, BIN1_Pin, GPIO_PIN_SET);
//             // HAL_GPIO_WritePin(GPIOB, BIN2_Pin, GPIO_PIN_RESET);
//             break;
//         case 3:
//             ain1.setLow();
//             ain2.setHigh();
//             bin1.setHigh();
//             bin2.setLow();
//             // HAL_GPIO_WritePin(GPIOB, AIN1_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, AIN2_Pin, GPIO_PIN_SET);
//             // HAL_GPIO_WritePin(GPIOB, BIN1_Pin, GPIO_PIN_SET);
//             // HAL_GPIO_WritePin(GPIOB, BIN2_Pin, GPIO_PIN_RESET);
//             break;
//         case 4:
//             ain1.setLow();
//             ain2.setHigh();
//             bin1.setLow();
//             bin2.setLow();
//             // HAL_GPIO_WritePin(GPIOB, AIN1_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, AIN2_Pin, GPIO_PIN_SET);
//             // HAL_GPIO_WritePin(GPIOB, BIN1_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, BIN2_Pin, GPIO_PIN_RESET);
//             break;
//         case 5:
//             ain1.setLow();
//             ain2.setHigh();
//             bin1.setLow();
//             bin2.setHigh();
//             // HAL_GPIO_WritePin(GPIOB, AIN1_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, AIN2_Pin, GPIO_PIN_SET);
//             // HAL_GPIO_WritePin(GPIOB, BIN1_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, BIN2_Pin, GPIO_PIN_SET);
//             break;
//         case 6:
//             ain1.setLow();
//             ain2.setLow();
//             bin1.setLow();
//             bin2.setHigh();
//             // HAL_GPIO_WritePin(GPIOB, AIN1_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, AIN2_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, BIN1_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, BIN2_Pin, GPIO_PIN_SET);
//             break;
//         case 7:
//             ain1.setHigh();
//             ain2.setLow();
//             bin1.setLow();
//             bin2.setHigh();
//             // HAL_GPIO_WritePin(GPIOB, AIN1_Pin, GPIO_PIN_SET);
//             // HAL_GPIO_WritePin(GPIOB, AIN2_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, BIN1_Pin, GPIO_PIN_RESET);
//             // HAL_GPIO_WritePin(GPIOB, BIN2_Pin, GPIO_PIN_SET);
//             break;
//     }
// }

// void stepMotorStop() {
//     ain1.setLow();
//     ain2.setLow();
//     bin1.setLow();
//     bin2.setLow();
// //   HAL_GPIO_WritePin(GPIOB, AIN1_Pin, GPIO_PIN_RESET);
// //   HAL_GPIO_WritePin(GPIOB, AIN2_Pin, GPIO_PIN_RESET);
// //   HAL_GPIO_WritePin(GPIOB, BIN1_Pin, GPIO_PIN_RESET);
// //   HAL_GPIO_WritePin(GPIOB, BIN2_Pin, GPIO_PIN_RESET);
// }