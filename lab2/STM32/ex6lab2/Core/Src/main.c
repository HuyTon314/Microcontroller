/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Lab 2 - Exercise 6: Digital Clock with Software Timers
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
// --- Khai báo Software Timers (1 tick = 10ms) ---
int timer1_counter = 0;
int timer1_flag = 0;

int timer2_counter = 0;
int timer2_flag = 0;

// --- Biến lưu trữ thời gian đồng hồ ---
int hour = 15;
int minute = 8;
int second = 50;

// --- Biến hiển thị LED 7-đoạn ---
int index_led = 0;
int led_buffer[4] = {1, 5, 0, 8}; // Giá trị khởi tạo ban đầu (15:08)
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

void setTimer1(int duration);
void setTimer2(int duration);
void timerRun(void);

void display7SEG(int num);
void update7SEG(int index);
void updateClockBuffer(void);

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// --- HÀM KHỞI TẠO SOFTWARE TIMERS ---
void setTimer1(int duration) {
    timer1_counter = duration; // duration x 10ms
    timer1_flag = 0;
}

void setTimer2(int duration) {
    timer2_counter = duration; // duration x 10ms
    timer2_flag = 0;
}

// --- HÀM ĐẾM SOFTWARE TIMER (Gọi trong ngắt Hardware TIM2) ---
void timerRun(void) {
    if (timer1_counter > 0) {
        timer1_counter--;
        if (timer1_counter <= 0) {
            timer1_flag = 1;
        }
    }

    if (timer2_counter > 0) {
        timer2_counter--;
        if (timer2_counter <= 0) {
            timer2_flag = 1;
        }
    }
}

// --- CẬP NHẬT ĐỒNG HỒ & TÁCH SỐ CHO LED BUFFER ---
void updateClockBuffer(void) {
    second++;
    if (second >= 60) {
        second = 0;
        minute++;
        if (minute >= 60) {
            minute = 0;
            hour++;
            if (hour >= 24) {
                hour = 0;
            }
        }
    }

    // Tách chữ số hàng chục và hàng đơn vị cho Giờ & Phút
    led_buffer[0] = hour / 10;
    led_buffer[1] = hour % 10;
    led_buffer[2] = minute / 10;
    led_buffer[3] = minute % 10;
}

// --- HÀM HIỂN THỊ MÃ 7 ĐOẠN (BINARY ANODE CHUNG) ---
void display7SEG(int num) {
    uint8_t segmentNumber[10] = {
        0b00111111, // 0
        0b00000110, // 1
        0b01011011, // 2
        0b01001111, // 3
        0b01100110, // 4
        0b01101101, // 5
        0b01111101, // 6
        0b00000111, // 7
        0b01111111, // 8
        0b01101111  // 9
    };

    if (num >= 0 && num <= 9) {
        uint8_t data = ~segmentNumber[num]; // Đảo bit cho Anode chung (0 là SÁNG)
        for (int i = 0; i < 7; i++) {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 << i, (data >> i) & 0b00000001);
        }
    }
}

// --- QUÉT LED THEO INDEX (PA6, PA7, PA8, PA9) ---
void update7SEG(int index) {
    // Tắt tất cả 4 Transistor PNP trước (Khóa mức HIGH)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);

    // Kích mở LED tương ứng (Mức LOW)
    switch (index) {
        case 0:
            display7SEG(led_buffer[0]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // EN0 - PA6
            break;
        case 1:
            display7SEG(led_buffer[1]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // EN1 - PA7
            break;
        case 2:
            display7SEG(led_buffer[2]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // EN2 - PA8
            break;
        case 3:
            display7SEG(led_buffer[3]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET); // EN3 - PA9
            break;
        default:
            break;
    }
}

// --- CALLBACK NGẮT HARDWARE TIM2 (XẢY RA MỖI 10MS) ---
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        timerRun();
    }
}
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_TIM2_Init();

  /* Kích hoạt ngắt Hardware Timer 2 */
  HAL_TIM_Base_Start_IT(&htim2);

  /* Khởi tạo 2 Software Timers */
  setTimer1(25);  // Software Timer 1: 250ms (Quét LED 7-đoạn)
  setTimer2(100); // Software Timer 2: 1000ms (1 giây - Cập nhật đồng hồ)

  while (1)
  {
    /* Task 1: Quét LED 7-đoạn khi Timer 1 hết giờ (250ms/lần) */
    if (timer1_flag == 1) {
        setTimer1(25); // Re-arm timer 1
        update7SEG(index_led);
        index_led = (index_led + 1) % 4;
    }

    /* Task 2: Cập nhật Đồng hồ & Nháy LED DOT khi Timer 2 hết giờ (1000ms/lần) */
    if (timer2_flag == 1) {
        setTimer2(100); // Re-arm timer 2
        updateClockBuffer();                    // Cập nhật thời gian vào led_buffer
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4); // Nháy 2 LED DOT nối ở chân PA4
    }
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

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

static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999; // 8MHz / 8000 = 1000Hz (1ms)
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;       // Tần số ngắt 10ms/lần
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Enable Clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();

  /* Tắt JTAG để giải phóng chân PB3, PB4 */
  __HAL_AFIO_REMAP_SWJ_NOJTAG();

  /* Trạng thái ban đầu GPIOA: Tắt 4 Transistor (HIGH), Bật LED DOT (LOW) */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);

  /* Trạng thái ban đầu GPIOB (Bus 7 đoạn) */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
                          | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_RESET);

  /* Cấu hình chân Output GPIOA (PA4 -> PA9) */
  GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Cấu hình chân Output GPIOB (PB0 -> PB6) */
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
                          | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
