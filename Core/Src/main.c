/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t Rx_data[10];
int warning = 0;
char Reply[150];
char GsmTest[10]="AT\r";
char ActivateTxtMode[20]="AT+CMGF=1\r";
char clearmem[100]="AT+CMGD=1,4\r";
char Readsms[150]= "AT+CMGL=\"ALL\"\r";
char memory[100] = "AT+CPMS=\"SM\",\"SM\",\"SM\"\r";
//char PhoneNumber[30]="AT+CMGS=\"+37062303250\"\r\n";
char PhoneNumber[30]="AT+CMGS=\"+37061169629\"\r";
//char Call[30] = "ATD+ +37061169629;";
char Message[100]="Virsyta temperatura.";
//char Message2[100]="Duomenu perdavimo klaida.";
int CtrlZ=26;
char ch[1];

static const uint8_t TMP102_ADDR = 0x48 << 1; // Use 8-bit address
static const uint8_t REG_TEMP = 0x00;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* 
Kai Tx ir Rx klaidos yra noriu issiusti zinute kad neiseina perduoti duomenu
bet problema ta kad jie pastoviai siuncia ta klaida ir man zinute pastoviai sius
tai ideja viena yra tokia kad ch turi "26" kintamaji kuris naudojamas issiusti zinute
tai jei yra Tx arba Rx klaida galima ta ch i 0 nustatyti arba memset isvalyti masyva
ir nesius zinutes kitas variantas yra sukurti while cikla viduje (while error = 1) tarkim
ir kai bus pradedami skaityti duomenys tada galetu error kintamaji i 0 nustatyti ideja tokia

*/
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
		ch[0]=CtrlZ;
	  HAL_StatusTypeDef ret;
	  uint8_t buf[12];
	char klaida[] = "Virsyta temperatura. Siunciama zinute... || Temperatura: ";
//	char klaida2[] = "Duomenu perdavimo klaida. Siunciama zinute...";
	char klaida3[] = "Zinute issiusta";
	char krauna1[] = "Kraunama programa. Palaukite.\r\n";
	char krauna2[] = "Programa pakrauta.\r\n";
	char stop[] = "Programa sustabdyta\r\n";
	char pagebreak[] = "\r\n";
	  int16_t val;
	  float temp_c;
		char Message2[100];
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Transmit(&huart2, (uint8_t *)krauna1, strlen(krauna1), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart1,(uint8_t *)GsmTest, strlen(GsmTest), HAL_MAX_DELAY);
  HAL_UART_Receive(&huart1,(uint8_t *)Reply, 15, 500);
  HAL_Delay(1000);
	HAL_UART_Transmit(&huart2,(uint8_t *)Reply, strlen(Reply), HAL_MAX_DELAY);
	memset(Reply,0,sizeof(Reply));

	HAL_UART_Transmit(&huart1,(uint8_t *)ActivateTxtMode, strlen(ActivateTxtMode), HAL_MAX_DELAY);
  HAL_UART_Receive(&huart1,(uint8_t *)Reply, 15, 500);
  HAL_Delay(1000);
	HAL_UART_Transmit(&huart2,(uint8_t *)Reply, strlen(Reply), HAL_MAX_DELAY);
	memset(Reply,0,sizeof(Reply));
	
	HAL_UART_Transmit(&huart1,(uint8_t *)clearmem, strlen(clearmem), HAL_MAX_DELAY);
  HAL_UART_Receive(&huart1,(uint8_t *)Reply, 80, 500);
  HAL_Delay(1000);
	HAL_UART_Transmit(&huart2,(uint8_t *)Reply, strlen(Reply), HAL_MAX_DELAY);
	memset(Reply,0,sizeof(Reply));

	HAL_UART_Transmit(&huart1,(uint8_t *)memory, strlen(memory), HAL_MAX_DELAY);
  HAL_UART_Receive(&huart1,(uint8_t *)Reply, 80, 500);
  HAL_Delay(1000);
	HAL_UART_Transmit(&huart2,(uint8_t *)Reply, strlen(Reply), HAL_MAX_DELAY);
	memset(Reply,0,sizeof(Reply));
	HAL_UART_Transmit(&huart2, (uint8_t *)krauna2, strlen(krauna1), HAL_MAX_DELAY);
	uint16_t pass = 0;
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		while (pass < 1)
		{
		/*	if (Rx_data[0] == 's' && Rx_data[1] == 't' && Rx_data[2] == 'o' && Rx_data[3] == 'p')
			{
				HAL_UART_Transmit(&huart2, (uint8_t *)stop, strlen(stop), 100);
				memset(Rx_data,0,sizeof(Reply));
			}*/
			if(Reply[102] == 'S' && Reply[103] == 't' && Reply[104] == 'o' && Reply[105] == 'p')
			{
				HAL_UART_Transmit(&huart2, (uint8_t *)stop, strlen(stop), 100);
				memset(Reply,0,sizeof(Reply));
				
			}
			else
			{
	//		HAL_UART_Receive(&huart2, Rx_data, 5, 2500); // get rx data from terminal
			HAL_UART_Transmit(&huart1,(uint8_t *)Readsms, strlen(Readsms), 100);
			HAL_UART_Receive(&huart1,(uint8_t *)Reply, 150, 1500);
		//	HAL_UART_Transmit(&huart2,(uint8_t *)Reply, strlen(Reply), 500);
			if(Reply[102] == 'S' && Reply[103] == 't' && Reply[104] == 'a' && Reply[105] == 'r' && Reply[106] == 't')
			{
				pass = 1;
				memset(Reply,0,sizeof(Reply));
		//		memset(Rx_data,0,sizeof(Rx_data));
			}
	/*		else if (Rx_data[0] == 's' && Rx_data[1] == 't' && Rx_data[2] == 'a' && Rx_data[3] == 'r' && Rx_data[4] == 't')
			{
				pass = 1;
				memset(Reply,0,sizeof(Reply));
				memset(Rx_data,0,sizeof(Rx_data));
			}*/
			else
			{
				memset(Reply,0,sizeof(Reply));
		//		memset(Rx_data,0,sizeof(Rx_data));
			}
		}
		}
		HAL_UART_Receive(&huart2, Rx_data, 5, 1000);
		if (pass > 0)
	{
		memset(Rx_data,0,sizeof(Rx_data));
			//					HAL_UART_Transmit(&huart1,(uint8_t *)Readsms, strlen(Readsms), 100);
			//			HAL_UART_Receive(&huart1,(uint8_t *)Reply, 150, 1500);
		// Tell TMP102 that we want to read from the temperature register
	      buf[0] = REG_TEMP;
	      ret = HAL_I2C_Master_Transmit(&hi2c1, TMP102_ADDR, buf, 1, 100);
	      if ( ret != HAL_OK ) 
					{
	        strcpy((char*)buf, "Error Tx\r\n");
						//Rx_data[0] = 's'; Rx_data[1] = 't'; Rx_data[2] = 'o'; Rx_data[3] = 'p';
						pass = 0;
						HAL_UART_Transmit(&huart2, (uint8_t *)stop, strlen(stop), 100);
						//galima issiust zinute su "Tx klaida"
	      } 
					else 
						{

	        // Read 2 bytes from the temperature register
	        ret = HAL_I2C_Master_Receive(&hi2c1, TMP102_ADDR, buf, 2, 100);
	        if ( ret != HAL_OK ) {
	          strcpy((char*)buf, "Error Rx\r\n");
						//Rx_data[0] = 's'; Rx_data[1] = 't'; Rx_data[2] = 'o'; Rx_data[3] = 'p';
						pass = 0;
						HAL_UART_Transmit(&huart2, (uint8_t *)stop, strlen(stop), 100);
						//galima issiust zinute su "Rx klaida"
	        } else 
					{
			//			HAL_UART_Transmit(&huart1,(uint8_t *)Readsms, strlen(Readsms), 100);
			//			HAL_UART_Receive(&huart1,(uint8_t *)Reply, 150, 1500);
						Rx_data[0] = 's'; Rx_data[1] = 't'; Rx_data[2] = 'a'; Rx_data[3] = 'r'; Rx_data[4] = 't';
	          //Combine the bytes
	          val = ((int16_t)buf[0] << 4) | (buf[1] >> 4);

	          // Convert to 2's complement, since temperature can be negative
	          if ( val > 0x7FF ) 
							{
			//					HAL_UART_Transmit(&huart1,(uint8_t *)Readsms, strlen(Readsms), 100);
			//			HAL_UART_Receive(&huart1,(uint8_t *)Reply, 150, 1500);
	            val |= 0xF000;
	          }

	          // Convert to float temperature value (Celsius)
	          temp_c = val * 0.0625;

	          // Convert temperature to decimal format
	          temp_c *= 100;
	          sprintf((char*)buf,
	                "%u.%u C\r\n",
	                ((unsigned int)temp_c / 100),
	                ((unsigned int)temp_c % 100));
					memcpy(Message2,&buf,sizeof(Message2));
				//		HAL_UART_Transmit(&huart1,(uint8_t *)Readsms, strlen(Readsms), 100);
				//		HAL_UART_Receive(&huart1,(uint8_t *)Reply, 150, 1500);
	        }
	      }

	      // Send out buffer (temperature or error message)
	      if (temp_c > 2450)
	      {
					warning = warning + 1;
					if (warning <= 1)
					{
					HAL_UART_Transmit(&huart2, (uint8_t *)klaida, strlen(klaida), HAL_MAX_DELAY);
	    	  HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
			//		HAL_UART_Transmit(&huart1,(uint8_t *)ActivateTxtMode, strlen(ActivateTxtMode), HAL_MAX_DELAY);
			//		HAL_Delay(1000);
					HAL_UART_Transmit(&huart1,(uint8_t *)PhoneNumber, strlen(PhoneNumber), HAL_MAX_DELAY);
					HAL_Delay(1000);
					HAL_UART_Transmit(&huart1,(uint8_t *)Message, strlen(Message), HAL_MAX_DELAY);
					HAL_Delay(1000);
					HAL_UART_Transmit(&huart1,(uint8_t *)ch, strlen(ch), HAL_MAX_DELAY);
					HAL_Delay(1000);
					HAL_UART_Transmit(&huart2, (uint8_t *)klaida3, strlen(klaida3), HAL_MAX_DELAY);
					HAL_UART_Transmit(&huart2, (uint8_t *)pagebreak, strlen(pagebreak), HAL_MAX_DELAY);
					HAL_Delay(1000);
					}
					else
					{
						//Rx_data[0] = 's'; Rx_data[1] = 't'; Rx_data[2] = 'o'; Rx_data[3] = 'p';
						pass = 0;
						warning = 0;
					}
	      }
	      else
	      {
				//	HAL_UART_Transmit(&huart1,(uint8_t *)Readsms, strlen(Readsms), 100);
			//		HAL_UART_Receive(&huart1,(uint8_t *)Reply, 150, 1500);
			//		HAL_UART_Receive(&huart2, Rx_data, 5, 1500);

			//	HAL_UART_Transmit(&huart2,(uint8_t *)Reply, strlen(Reply), 500);
					if(Reply[102] == 'C' && Reply[103] == 'h' && Reply[104] == 'e' && Reply[105] == 'c' && Reply[106] == 'k')
					{

						HAL_UART_Transmit(&huart1,(uint8_t *)PhoneNumber, strlen(PhoneNumber), HAL_MAX_DELAY);
						HAL_Delay(1000);
						HAL_UART_Transmit(&huart1,(uint8_t *)Message2, strlen(Message), HAL_MAX_DELAY);
						HAL_Delay(1000);
						HAL_UART_Transmit(&huart1,(uint8_t *)ch, strlen(ch), HAL_MAX_DELAY);
						HAL_Delay(1000);
						memset(Reply,0,sizeof(Reply));
						HAL_UART_Transmit(&huart2, (uint8_t *)klaida3, strlen(klaida3), HAL_MAX_DELAY);
						HAL_UART_Transmit(&huart2, (uint8_t *)pagebreak, strlen(pagebreak), HAL_MAX_DELAY);
					}
					else if(Reply[102] == 'S' && Reply[103] == 't' && Reply[104] == 'o' && Reply[105] == 'p')
						{
						pass = 0;
						}
					else
					{
						HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
						HAL_UART_Transmit(&huart1,(uint8_t *)Readsms, strlen(Readsms), 100);
						HAL_UART_Receive(&huart1,(uint8_t *)Reply, 150, 1500);
					}
			/*		else if (Rx_data[0] == 's' && Rx_data[1] == 't' && Rx_data[2] == 'o' && Rx_data[3] == 'p')
					{
						pass = 0;
					}*/
	      }
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10909CEC;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

