/* USER CODE BEGIN Header */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t pipe[4];
uint8_t pwm[4];
uint8_t pipe_num;
uint8_t start_time[4];
uint8_t stop_time[4];
uint8_t pwm_num;
uint16_t clock_hours = 0;
uint16_t clock_secs = 0;
uint8_t last_hour = 0;
uint16_t min_distance = 100; /* minimum distance = 100mm */
uint16_t max_distance = 250; /* maximum distance = 250mm */
uint8_t cmd_dist = 0x55;
uint8_t us100_Rx_flag = 00;
uint8_t us100_buffer[2] = {0};
volatile uint16_t distance = 0;
volatile uint16_t depth = 0;
volatile uint8_t rcv_intpt_flag = 0;
uint16_t rpm_tick_count = 0;
uint8_t buffer[64] = {0};
uint8_t double_byte[2];
int empty_zone = 0;
int single_digit = 1;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM5_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART6_UART_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void DIGITS_Display()
{
	 uint8_t DIGIT_A = depth/10;
	 uint8_t DIGIT_B = depth%10;

//	 sprintf((char *)buffer, "\r\n%d%d", DIGIT_A, DIGIT_B);
//	 HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
//	 memset(buffer, 0, sizeof(buffer));

	 uint8_t DIGITA_VAL = 0x0F & DIGIT_A; //mask off higher4 bits
	 int Abit0 = (DIGITA_VAL ) & 1;  	// extract Abit0 of the 4-bit value
	 int Abit1 = (DIGITA_VAL >> 1) & 1;  // extract Abit1 of the 4-bit value
	 int Abit2 = (DIGITA_VAL >> 2) & 1;  // extract Abit2 of the 4-bit value
	 int Abit3 = (DIGITA_VAL >> 3) & 1;  // extract Abit3 of the 4-bit value

	 uint8_t DIGITB_VAL = 0x0F & DIGIT_B; //mask off higher4 bits
	 int Bbit0 = (DIGITB_VAL ) & 1;  	// extract Bbit0 of the 4-bit value
	 int Bbit1 = (DIGITB_VAL >> 1) & 1;  // extract Bbit1 of the 4-bit value
	 int Bbit2 = (DIGITB_VAL >> 2) & 1;  // extract Bbit2 of the 4-bit value
	 int Bbit3 = (DIGITB_VAL >> 3) & 1;  // extract Bbit3 of the 4-bit value

	 if (Abit0 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOB, DIGIT_A0_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOB, DIGIT_A0_Pin, GPIO_PIN_SET);

	 }
	 if (Abit1 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOB, DIGIT_A1_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOB, DIGIT_A1_Pin, GPIO_PIN_SET);

	 }
	 if (Abit2 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOB, DIGIT_A2_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOB, DIGIT_A2_Pin, GPIO_PIN_SET);

	 }
	 if (Abit3 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOB, DIGIT_A3_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOB, DIGIT_A3_Pin, GPIO_PIN_SET);

	 }


	 if (Bbit0 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B0_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B0_Pin, GPIO_PIN_SET);

	 }
	 if (Bbit1 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B1_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B1_Pin, GPIO_PIN_SET);

	 }
	 if (Bbit2 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B2_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B2_Pin, GPIO_PIN_SET);

	 }
	 if (Bbit3 == (0))
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B3_Pin, GPIO_PIN_RESET);
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOC, DIGIT_B3_Pin, GPIO_PIN_SET);

	 }
}

void ADC_Select_CH(int CH) {
	ADC_ChannelConfTypeDef sConfig = {0};
	switch(CH)
	{
	case 0:
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 1:
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 2:
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 3:
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 4:
	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 5:
	sConfig.Channel = ADC_CHANNEL_5;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 6:
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 7:
	sConfig.Channel = ADC_CHANNEL_7;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 8:
	sConfig.Channel = ADC_CHANNEL_8;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 9:
	sConfig.Channel = ADC_CHANNEL_9;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 10:
	sConfig.Channel = ADC_CHANNEL_10;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 11:
	sConfig.Channel = ADC_CHANNEL_11;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 12:
	sConfig.Channel = ADC_CHANNEL_12;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 13:
	sConfig.Channel = ADC_CHANNEL_13;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 14:
	sConfig.Channel = ADC_CHANNEL_14;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	case 15:
	sConfig.Channel = ADC_CHANNEL_15;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
	break;
	}
}

void user_input () {
		sprintf((char*)buffer, "\r\nSETUP Mode");
		HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
		memset(buffer, 0, sizeof(buffer));
		for (int i = 0; i < 4; i++) {
			single_digit = 1;
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, double_byte, 1);
			sprintf((char*)buffer, "\n\r\nEnter Pipeline Choice (0 = INLET, 1-3 = ZONES): ");
			HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
			while(rcv_intpt_flag == (00)) {};
			pipe[i] = atoi((char *)&double_byte);
			memset(buffer, 0, sizeof(buffer));
			memset(double_byte, 0, sizeof(double_byte));

			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, double_byte, 1);
			sprintf((char*)buffer, "\r\nEnter Pump PWM (0 = INLET, 1-3 = ZONES): ");
			HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
			while(rcv_intpt_flag == (00)) {};
			pwm[i] = atoi((char *)&double_byte);
			memset(buffer, 0, sizeof(buffer));
			memset(double_byte, 0, sizeof(double_byte));
		}

		for (int i = 0; i < 4; i ++) {
			single_digit = 0;
			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, double_byte, 2);
			sprintf((char*)buffer, "\n\r\nEnter Start Hour (00 - 23): ");
			HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
			while(rcv_intpt_flag == (00)) {};
			start_time[i] = atoi((char *)&double_byte);
			memset(buffer, 0, sizeof(buffer));
			memset(double_byte, 0, sizeof(double_byte));

			rcv_intpt_flag = 00;
			HAL_UART_Receive_IT(&huart6, double_byte, 2);
			sprintf((char*)buffer, "\r\nEnter Stop Hour (00 - 23): ");
			HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
			rcv_intpt_flag = 00;
			while(rcv_intpt_flag == (00)) {};
			stop_time[i] = atoi((char *)&double_byte);
			memset(buffer, 0, sizeof(buffer));
			memset(double_byte, 0, sizeof(double_byte));
		}

			rcv_intpt_flag = 00;

			while(rcv_intpt_flag) {};
			sprintf((char*)buffer, "\n\r\nPrinting SETUP Parameters");
			HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
			memset(buffer, 0, sizeof(buffer));
			sprintf((char*)buffer, "\r\nCurrent Wall Clock Hour: 00");
			HAL_UART_Transmit(&huart6, buffer, strlen((char*)buffer), HAL_MAX_DELAY);
			memset(buffer, 0, sizeof(buffer));

			for (int i = 0; i < 4; i++) {
				sprintf((char *)buffer, "\r\nPipeline: %u, Pump PWM: %u, Start Hour: %u, Stop Hour: %u", pipe[i], pwm[i], start_time[i], stop_time[i]);
				HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
				memset(buffer, 0, sizeof(buffer));
			}

			sprintf((char*)buffer, "\n\r\nSETUP is done. Press Blue Button for RUN Mode");
			HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
			memset(buffer, 0, sizeof(buffer));
}

uint32_t get_potentio_num() {
	ADC_Select_CH(9);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 1000);
	uint32_t ADC_CH9 = HAL_ADC_GetValue(&hadc1);
//	sprintf((char*)buffer, "\n\r\n ADC_CH9: %d", ADC_CH9);
//				HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
//				memset(buffer, 0, sizeof(buffer));
	HAL_ADC_Stop(&hadc1);

	return ADC_CH9/2.55;
}

void servo_num(uint16_t pipe_num) {
	if (pipe_num == 0) {
		TIM2->CCR1 = 500;
	} else if (pipe_num == 1) {
		TIM2->CCR1 = 800;
	} else if (pipe_num == 2) {
		TIM2->CCR1 = 1100;
	} else if (pipe_num == 3) {
		TIM2->CCR1 = 1400;
	}
}

void DC_motor_speed(uint16_t pipe_num, uint16_t pwm_num) {
	if(pipe_num == 0) {
		TIM3->CCR3 = 0;
		if (pwm_num == 0) {
//			sprintf((char *)buffer, "\r\n val = %lu ", get_potentio_num()*20);
//				HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
//				memset(buffer, 0, sizeof(buffer));
			TIM3->CCR1 = get_potentio_num()*20;
		} else if (pwm_num == 1) {
			TIM3->CCR1 = 0.7*2000;
		} else if (pwm_num == 2) {
			TIM3->CCR1 = 0.85*2000;
		} else if (pwm_num == 3) {
			TIM3->CCR1 = 0.99*2000;
		}
	} else {
		TIM3->CCR3 = 0;
		if (pwm_num == 0) {
			TIM3->CCR1 = get_potentio_num()*20;
		} else if (pwm_num == 1) {
			TIM3->CCR1 = 0.7*2000;
		} else if (pwm_num == 2) {
			TIM3->CCR1 = 0.85*2000;
		} else if (pwm_num == 3) {
			TIM3->CCR1 = 0.99*2000;
		}
	}
}

void DC_motor_reset() {
	TIM3->CCR1 = 0;
	TIM3->CCR3 = 0;
}

void water_level() {
	us100_Rx_flag = 00;
	HAL_UART_Receive_IT(&huart1, us100_buffer, 2);
	HAL_UART_Transmit(&huart1, &cmd_dist, 1, HAL_MAX_DELAY);

	while (us100_Rx_flag == (00)) {};

	distance = (us100_buffer[0] << 8) | us100_buffer[1];

	if (distance >= max_distance) {
		depth = 0;
	} else if (distance <= min_distance) {
		depth = 100;
	} else {
		depth = ((max_distance - distance)*100/(max_distance - min_distance)); // water level in percentage
	}

	memset(us100_buffer, 0, sizeof(us100_buffer));
}

uint16_t get_rpm() {
	uint16_t rpm = rpm_tick_count/4;
	rpm_tick_count = 0;

	return rpm;
}

void run_output() {
	uint16_t rpm = get_rpm();
	uint16_t current_hour = clock_hours - 1;

	if(empty_zone)
	{
		sprintf((char *)buffer, "\r\n%d :   :   : %d : %d", current_hour, rpm, depth);
		HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
		memset(buffer, 0, sizeof(buffer));
	} else {
		sprintf((char *)buffer, "\r\n%d : %d : %d : %d : %d", current_hour, pipe_num, pwm_num, rpm, depth);
		HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
		memset(buffer, 0, sizeof(buffer));
	}
}

void set_led(uint16_t pipe_num) {
	HAL_GPIO_WritePin(GPIOA, LED_Blue_Pin|LED_Red_Pin|LED_Green_Pin, GPIO_PIN_RESET);

	if (pipe_num == 0) {
		HAL_GPIO_WritePin(GPIOA, LED_Blue_Pin|LED_Red_Pin, GPIO_PIN_SET);
	} else if (pipe_num == 1) {
		HAL_GPIO_WritePin(GPIOA, LED_Red_Pin, GPIO_PIN_SET);
	} else if (pipe_num == 2) {
		HAL_GPIO_WritePin(GPIOA, LED_Green_Pin, GPIO_PIN_SET);
	} else if (pipe_num == 3) {
		HAL_GPIO_WritePin(GPIOA, LED_Blue_Pin, GPIO_PIN_SET);
	}

	if (pipe_num == 4) {
		for (int i = 0; i < 20; i++) {
			HAL_GPIO_WritePin(GPIOA, LED_Blue_Pin|LED_Red_Pin|LED_Green_Pin, GPIO_PIN_SET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(GPIOA, LED_Blue_Pin|LED_Red_Pin|LED_Green_Pin, GPIO_PIN_RESET);
			HAL_Delay(100);
		}
	}
}

void reset_led() {
	HAL_GPIO_WritePin(GPIOA, LED_Blue_Pin|LED_Red_Pin|LED_Green_Pin, GPIO_PIN_RESET);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
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
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_USART1_UART_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Init(&htim2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  TIM2->PSC = 16;
  TIM2->ARR = 20000;
  TIM2->CCR1 = 500;

  HAL_TIM_Base_Init(&htim3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  TIM3->PSC = 16;
  TIM3->ARR = 2000;
  TIM3->CCR1 = 0;
  TIM3->CCR3 = 0;

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
  user_input();

  while(HAL_GPIO_ReadPin(GPIOC, USER_Button_Pin) == GPIO_PIN_SET) {
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
  }

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

  // RUN Mode
  sprintf((char *)buffer, "\n\r\nRUN MODE");
  HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
  memset(buffer, 0, sizeof(buffer));
  sprintf((char *)buffer, "\r\nHOUR : PIPE : PWM : RPM : DEPTH");
  HAL_UART_Transmit(&huart6, buffer, strlen((char*) buffer), HAL_MAX_DELAY);
  memset(buffer, 0, sizeof(buffer));
  sprintf((char *)buffer, "\r\n---------------------------------------------");
  HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
  memset(buffer, 0, sizeof(buffer));

  clock_hours = 0;
  clock_secs = 0;
  last_hour = 0;
  HAL_TIM_Base_Start_IT(&htim5);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1) {
	  empty_zone = 1;

	  if (clock_hours == start_time[0]) {
		  pipe_num = pipe[0];
		  pwm_num = pwm[0];
	  } else if (clock_hours == start_time[1]) {
		  pipe_num = pipe[1];
		  pwm_num = pwm[1];
	  } else if (clock_hours == start_time[2]) {
		  pipe_num = pipe[2];
		  pwm_num = pwm[2];
	  } else if (clock_hours == start_time[3]) {
		  pipe_num = pipe[3];
		  pwm_num = pwm[3];
	  }

	  for (int i = 0; i < 4; i++) {
		  if(start_time[i] <= clock_hours && stop_time[i] >= clock_hours) {
			  empty_zone = 0;
		  }
	  }

	  if (!empty_zone) {
		  servo_num(pipe_num);
		  set_led(pipe_num);
		  DC_motor_speed(pipe_num, pwm_num);
		  if (pwm_num == 0) {
			  while (clock_hours == last_hour) {
				  DC_motor_speed(pipe_num, pwm_num);
			  }
		  }
	  } else if (empty_zone) {
		  DC_motor_reset();
		  reset_led();
	  }

	  while (clock_hours == last_hour){};

	  last_hour = clock_hours;
	  water_level();

	  if (depth == 0) {
		  DC_motor_reset();
		  TIM2->CCR1 = 0;
		  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		  sprintf((char *)buffer, "\n\r\nReservoir is empty");
		  HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
		  memset(buffer, 0, sizeof(buffer));
		  set_led(4);

		  break;
	  }

	  DIGITS_Display();
	  run_output();

	  if (clock_hours == 24) {
		  DC_motor_reset();
		  reset_led();
		  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		  TIM2->CCR1 = 0;
		  sprintf((char *)buffer, "\n\r\nIrrigation complete");
		  HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
		  memset(buffer, 0, sizeof(buffer));

		  break;
	  }
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	HAL_GPIO_WritePin(GPIOA, LED_Blue_Pin|LED_Red_Pin|LED_Green_Pin, GPIO_PIN_RESET);
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	HAL_TIM_Base_Stop_IT(&htim5);

	return 0;
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */
  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */
  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_8B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */
  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */
  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */
  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 16-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 20000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 500-1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */
  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */
  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 16-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 2000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1200-1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = 0;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */
  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */
  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */
  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 16000-1;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 1000-1;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */
  /* USER CODE END TIM5_Init 2 */

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
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */
  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */
  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */
  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|LED_Blue_Pin|LED_Green_Pin|LED_Red_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DIGIT_A3_Pin|DIGIT_A0_Pin|DIGIT_A1_Pin|DIGIT_A2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, DIGIT_B0_Pin|DIGIT_B1_Pin|DIGIT_B2_Pin|DIGIT_B3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Button_Pin */
  GPIO_InitStruct.Pin = USER_Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin LED_Blue_Pin LED_Green_Pin LED_Red_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|LED_Blue_Pin|LED_Green_Pin|LED_Red_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : RPM_TICK_Pin */
  GPIO_InitStruct.Pin = RPM_TICK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RPM_TICK_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DIGIT_A3_Pin DIGIT_A0_Pin DIGIT_A1_Pin DIGIT_A2_Pin */
  GPIO_InitStruct.Pin = DIGIT_A3_Pin|DIGIT_A0_Pin|DIGIT_A1_Pin|DIGIT_A2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : DIGIT_B0_Pin DIGIT_B1_Pin DIGIT_B2_Pin DIGIT_B3_Pin */
  GPIO_InitStruct.Pin = DIGIT_B0_Pin|DIGIT_B1_Pin|DIGIT_B2_Pin|DIGIT_B3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart -> Instance == USART6) {
		if (single_digit) {
			HAL_UART_Transmit(&huart6, double_byte, 1, HAL_MAX_DELAY);
		} else {
			HAL_UART_Transmit(&huart6, double_byte, 2, HAL_MAX_DELAY);
		}
		rcv_intpt_flag = 1;
	}

	if (huart->Instance == USART1) {
		us100_Rx_flag = 1;
	}
}

void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin) {
	if (GPIO_Pin == RPM_TICK_Pin) {
		rpm_tick_count++;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM5) {
		clock_secs++;
		clock_hours = clock_secs/12;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
