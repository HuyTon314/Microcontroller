/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Lab 2 - Exercise 10: Shift Left LED Matrix (Fixed Compiler Errors)
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
// --- Cấu hình Software Timers (1 tick = 10ms) ---
int timer1_counter = 0;
int timer1_flag = 0;

int timer2_counter = 0;
int timer2_flag = 0;

int timer3_counter = 0;
int timer3_flag = 0;

// ĐÃ THÊM: Khai báo biến cho Timer 4
int timer4_counter = 0;
int timer4_flag = 0;

// --- Biến lưu trữ thời gian đồng hồ ---
int hour = 15;
int minute = 8;
int second = 50;

// --- Biến hiển thị LED 7-đoạn ---
int index_led = 0;
int led_buffer[4] = {1, 5, 0, 8}; // Khởi tạo (15:08)

// --- Biến hiển thị LED Matrix 8x8 (Chạy từ COL7 -> COL0) ---
const int MAX_LED_MATRIX = 8;
int index_led_matrix = 7; // Khởi tạo bắt đầu từ cột 7

// Mã nhị phân hiển thị chữ 'A' trên LED Matrix (Xuất ra PB8 - PB15)
uint8_t matrix_buffer[8] = {
    0b00000000, // Cột 0 (ENM0 - PA2)
    0b01111100, // Cột 1 (ENM1 - PA3): Nét đứng trái
    0b00010010, // Cột 2 (ENM2 - PA10): Nét ngang + đỉnh
    0b00010001, // Cột 3 (ENM3 - PA11): Nét ngang + đỉnh
    0b00010010, // Cột 4 (ENM4 - PA12): Nét ngang + đỉnh
    0b01111100, // Cột 5 (ENM5 - PA13): Nét đứng phải
    0b00000000, // Cột 6 (ENM6 - PA14)
    0b00000000  // Cột 7 (ENM7 - PA15)
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

void setTimer1(int duration);
void setTimer2(int duration);
void setTimer3(int duration);
void setTimer4(int duration); // ĐÃ THÊM: Prototype cho setTimer4
void timerRun(void);

void display7SEG(int num);
void update7SEG(int index);
void updateClockBuffer(void);

void displayMatrixRow(uint8_t row_data);
void disableAllColumns(void);
void updateLEDMatrix(int index);
void shiftLeftLEDMatrix(void);

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// --- QUẢN LÝ SOFTWARE TIMERS ---
void setTimer1(int duration) {
    timer1_counter = duration;
    timer1_flag = 0;
}

void setTimer2(int duration) {
    timer2_counter = duration;
    timer2_flag = 0;
}

void setTimer3(int duration) {
    timer3_counter = duration;
    timer3_flag = 0;
}

// ĐÃ THÊM: Hàm setTimer4
void setTimer4(int duration) {
    timer4_counter = duration;
    timer4_flag = 0;
}

void timerRun(void) {
    if (timer1_counter > 0) {
        timer1_counter--;
        if (timer1_counter <= 0) timer1_flag = 1;
    }

    if (timer2_counter > 0) {
        timer2_counter--;
        if (timer2_counter <= 0) timer2_flag = 1;
    }

    if (timer3_counter > 0) {
        timer3_counter--;
        if (timer3_counter <= 0) timer3_flag = 1;
    }

    // ĐÃ THÊM: Xử lý bộ đếm Timer 4
    if (timer4_counter > 0) {
        timer4_counter--;
        if (timer4_counter <= 0) timer4_flag = 1;
    }
}

// --- CẬP NHẬT TIẾN TRÌNH ĐỒNG HỒ ---
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

    led_buffer[0] = hour / 10;
    led_buffer[1] = hour % 10;
    led_buffer[2] = minute / 10;
    led_buffer[3] = minute % 10;
}

// --- XUẤT MÃ 7 ĐOẠN (PB0 - PB6) ANODE CHUNG ---
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
        uint8_t data = ~segmentNumber[num];
        for (int i = 0; i < 7; i++) {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 << i, (data >> i) & 0b00000001);
        }
    }
}

// --- QUÉT LED 7-ĐOẠN (PA6 - PA9) ---
void update7SEG(int index) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);

    switch (index) {
        case 0:
            display7SEG(led_buffer[0]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
            break;
        case 1:
            display7SEG(led_buffer[1]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
            break;
        case 2:
            display7SEG(led_buffer[2]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
            break;
        case 3:
            display7SEG(led_buffer[3]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

// --- XUẤT DỮ LIỆU HÀNG LED MATRIX RA PB8 - PB15 (ROW0 - ROW7) ---
void displayMatrixRow(uint8_t row_data) {
    for (int i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 << i, (row_data >> i) & 0b00000001);
    }
}

// --- TẮT TẤT CẢ CÁC CỘT CỦA LED MATRIX ---
void disableAllColumns(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_10 | GPIO_PIN_11 |
                             GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
}

// --- QUÉT CỘT LED MATRIX ---
void updateLEDMatrix(int index) {
    disableAllColumns();                    // 1. Tắt hết cột cũ
    displayMatrixRow(matrix_buffer[index]); // 2. Xuất dữ liệu hàng mới

    // 3. Bật đúng cột hiện tại
    switch (index) {
        case 0: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2,  GPIO_PIN_SET); break;
        case 1: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3,  GPIO_PIN_SET); break;
        case 2: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET); break;
        case 3: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); break;
        case 4: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); break;
        case 5: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET); break;
        case 6: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET); break;
        case 7: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); break;
        default: break;
    }
}

// --- HÀM CUỘN CHỮ 'A' SANG TRÁI ---
void shiftLeftLEDMatrix(void) {
    uint8_t temp = matrix_buffer[0];
    for (int i = 0; i < MAX_LED_MATRIX - 1; i++) {
        matrix_buffer[i] = matrix_buffer[i + 1];
    }
    matrix_buffer[MAX_LED_MATRIX - 1] = temp;
}

// --- NGẮT HARDWARE TIMER 2 ---
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

  HAL_TIM_Base_Start_IT(&htim2);

  /* Khởi tạo Software Timers */
  setTimer1(25);  // Timer 1: 250ms (Quét 1 LED 7-đoạn)
  setTimer2(100); // Timer 2: 1000ms (1s - Cập nhật đồng hồ)
  setTimer3(1);   // Timer 3: 10ms (Quét 1 cột LED Matrix)
  setTimer4(30);  // ĐÃ THÊM: Timer 4: 300ms (Hiệu ứng cuộn chữ A)

  while (1)
  {
      // Task 1: Quét 4 LED 7-đoạn
      if (timer1_flag == 1) {
          setTimer1(25);
          update7SEG(index_led);
          index_led = (index_led + 1) % 4;
      }

      // Task 2: Cập nhật thời gian & Nháy LED DOT
      if (timer2_flag == 1) {
          setTimer2(100);
          updateClockBuffer();
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
      }

      // Task 3: Quét Ma trận LED chạy lùi từ COL7 về COL0
      if (timer3_flag == 1) {
          setTimer3(1); // 10ms
          updateLEDMatrix(index_led_matrix);

          index_led_matrix--;
          if (index_led_matrix < 0) {
              index_led_matrix = MAX_LED_MATRIX - 1; // Reset về 7 khi về dưới 0
          }
      }

      // Task 4: Cuộn chữ 'A' sang trái 1 cột mỗi 300ms
      if (timer4_flag == 1) {
          setTimer4(30);
          shiftLeftLEDMatrix();
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
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
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

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();

  __HAL_AFIO_REMAP_SWJ_DISABLE();

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                           GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
                           GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                           GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8 |
                           GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |
                           GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                        GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |
                        GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
                        GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                        GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8 |
                        GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |
                        GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
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
