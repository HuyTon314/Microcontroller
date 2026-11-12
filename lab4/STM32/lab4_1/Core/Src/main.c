/* USER CODE BEGIN Header */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "scheduler.h"
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
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);

/* USER CODE BEGIN PFP */
// Hàm đè để xuất printf qua UART1
int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

// Định nghĩa 5 Task định kỳ
void Task_0_5s(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
    printf("[%lu ms] Task 0.5s (LED1) executed\r\n", get_time());
}

void Task_1_0s(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
    printf("[%lu ms] Task 1.0s (LED2) executed\r\n", get_time());
}

void Task_1_5s(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
    printf("[%lu ms] Task 1.5s (LED3) executed\r\n", get_time());
}

void Task_2_0s(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
    printf("[%lu ms] Task 2.0s (LED4) executed\r\n", get_time());
}

void Task_2_5s(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
    printf("[%lu ms] Task 2.5s (LED5) executed\r\n", get_time());
}

// Task Demo phục vụ chấm điểm
void Task_10ms_Callback(void) {
    printf("[%lu ms] 10ms Tick Callback\r\n", get_time());
}

void Task_Temp_OneShot(void) {
    printf("[%lu ms] Single-shot Task executed!\r\n", get_time());
}

// Callback ngắt Timer 2
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        SCH_Update();

    }
}
/* USER CODE END PFP */

/* Private user code --------------------------------------------------------- */
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    /* USER CODE BEGIN 2 */
    SCH_Init();
    __disable_irq();

    // Đăng ký 5 Task định kỳ theo đề bài
    SCH_Add_Task(Task_0_5s, 50, 50);
    SCH_Add_Task(Task_1_0s, 100, 100);
    SCH_Add_Task(Task_1_5s, 150, 150);
    SCH_Add_Task(Task_2_0s, 200, 200);
    SCH_Add_Task(Task_2_5s, 250, 250);

    SCH_Add_Task(Task_Temp_OneShot, 200, 0);
    __enable_irq();
    /* USER CODE END 2 */
    MX_TIM2_Init();
    HAL_TIM_Base_Start_IT(&htim2);

    while (1) {
        SCH_Dispatch_Tasks();
    }
}

/**
  * @brief System Clock Configuration (HSI 8MHz)
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief TIM2 Initialization Function (10ms Interrupt @ 8MHz)
  */
static void MX_TIM2_Init(void) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7999;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 9;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief USART1 Initialization Function (9600 Baud)
  */
static void MX_USART1_UART_Init(void) {
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOA, D1_Pin|D2_Pin|D3_Pin|D4_Pin|D5_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = D1_Pin|D2_Pin|D3_Pin|D4_Pin|D5_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}
