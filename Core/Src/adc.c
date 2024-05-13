#include "adc.h"

ADC_HandleTypeDef hadc1;


/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

void ADC_Read_Channels(uint16_t *readValue) {
  HAL_ADC_Start(&hadc1);  // 只需启动一次
  for (uint8_t i = 0; i < 2; i++) {
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
      readValue[i] = HAL_ADC_GetValue(&hadc1);
    } else { 
      readValue[i] = 0;
    }
  }
  HAL_ADC_Stop(&hadc1); 
}

// TODO: read value for multiple times and return average value
// int ADC_Read(uint8_t count, int *readValueAverage) {
//   *readValueAverage = 0;
//   HAL_ADC_Start(&hadc1);  // 只需启动一次
//   for (uint8_t i = 0; i < count; ++i) {
//     if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
//       *readValueAverage += HAL_ADC_GetValue(&hadc1);
//     } else {
      
//     }
//   }
//   HAL_ADC_Stop(&hadc1); 
//   return adcValue;