#include "main.h"
#include "sysInit.h"
#include "utils.h"

#include "mt6816.hpp"
#include "tb67h450.hpp"
#include "uart_dma.hpp"

//uint16_t MT6816Read();
void stepMotor(int step);
void stepMotorStop();

//SPI spi;
MT6816 mt6816;
TB67H450 tb67h450;

UartDMA uart;

int main() {
    System_Init();

    //spi.init();
    mt6816.init();
    spiRawData_t angleData;
    uart.init();
    HAL_Delay(100);
    // motor driver init
    tb67h450.init();
    
    HAL_Delay(100);
    tb67h450.setVrefA(100);
    tb67h450.setVrefB(100);
    while (1) {
        int rotations = 100;
        for(int i = 0; i < rotations * 8; i++)
        {
            stepMotor(i);
            HAL_Delay(10); // 控制步进速度
            //printf(" 7MT6816 value: %u\n", MT6816Read());
        }
        // printf(" 7MT6816 value: %u\n", angleData.data);
        mt6816.readAngle(angleData);
        printf("%u\n", angleData.data);
        stepMotorStop();
        HAL_Delay(1000);
        
        for(int i = rotations * 8 - 1; i >= 0; i--)
        {
            stepMotor(i);
            HAL_Delay(10); // 控制步进速度
            //printf(" 7MT6816 value: %u\n", MT6816Read());
        }
        mt6816.readAngle(angleData);
        // printf(" 7MT6816 value: %u\n", angleData.data);
        printf("%u\n", angleData.data);
        stepMotorStop();
        HAL_Delay(1000);
    }

    return 0;
}

void stepMotor(int step)
{
    switch(step % 8)
    {
        case 0:
            tb67h450.setOutptA(true, false);
            tb67h450.setOutptB(false, false);
            break;
        case 1:
            tb67h450.setOutptA(true, false);
            tb67h450.setOutptB(true, false);
            break;
        case 2:
            tb67h450.setOutptA(false, false);
            tb67h450.setOutptB(true, false);
            break;
        case 3:
            tb67h450.setOutptA(false, true);
            tb67h450.setOutptB(true, false);
            break;
        case 4:
            tb67h450.setOutptA(false, true);
            tb67h450.setOutptB(false, false);
            break;
        case 5:
            tb67h450.setOutptA(false, true);
            tb67h450.setOutptB(false, true);
            break;
        case 6:
            tb67h450.setOutptA(false, false);
            tb67h450.setOutptB(false, true);
            break;
        case 7:
            tb67h450.setOutptA(true, false);
            tb67h450.setOutptB(false, true);
            break;
    }
}

void stepMotorStop() {
    tb67h450.setOutptA(false, false);
    tb67h450.setOutptB(false, false);
}