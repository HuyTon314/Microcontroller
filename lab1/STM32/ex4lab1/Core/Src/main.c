/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Bài 4 - Traffic Light Countdown from 9 (Common Anode)
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* LED 7 đoạn (a -> g) kết nối PB0 -> PB6 */
#define Seg1_Pin GPIO_PIN_0   // a
#define Seg2_Pin GPIO_PIN_1   // b
#define Seg3_Pin GPIO_PIN_2   // c
#define Seg4_Pin GPIO_PIN_3   // d
#define Seg5_Pin GPIO_PIN_4   // e
#define Seg6_Pin GPIO_PIN_5   // f
#define Seg7_Pin GPIO_PIN_6   // g
#define Seg_GPIO_Port GPIOB

/* Đèn giao thông Làn 0 (PA4, PA5, PA6) */
#define Led_Green0_Pin GPIO_PIN_4
#define Led_Green0_GPIO_Port GPIOA

#define Led_Yellow0_Pin GPIO_PIN_5
#define Led_Yellow0_GPIO_Port GPIOA

#define Led_Red0_Pin GPIO_PIN_6
#define Led_Red0_GPIO_Port GPIOA

/* Đèn giao thông Làn 1 (PA7, PA8, PA9) */
#define Led_Green1_Pin GPIO_PIN_7
#define Led_Green1_GPIO_Port GPIOA

#define Led_Yellow1_Pin GPIO_PIN_8
#define Led_Yellow1_GPIO_Port GPIOA

#define Led_Red1_Pin GPIO_PIN_9
#define Led_Red1_GPIO_Port GPIOA
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void display7SEG(int num);
void setTrafficLeds(GPIO_PinState r0, GPIO_PinState y0, GPIO_PinState g0,
                    GPIO_PinState r1, GPIO_PinState y1, GPIO_PinState g1);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Bảng mã Binary Anode Chung (Mức 0 = SÁNG, Mức 1 = TẮT)
 * Thứ tự Bit từ PB6 -> PB0: 0b0gfedcba
 */
const uint8_t seg_code[10] = {
    0b01000000, // 0: Sáng a,b,c,d,e,f (g Tắt)
    0b01111001, // 1: Sáng b,c
    0b00100100, // 2: Sáng a,b,d,e,g
    0b00110000, // 3: Sáng a,b,c,d,g
    0b00011001, // 4: Sáng b,c,f,g
    0b00010010, // 5: Sáng a,c,d,f,g
    0b00000010, // 6: Sáng a,c,d,e,f,g
    0b01111000, // 7: Sáng a,b,c
    0b00000000, // 8: Sáng tất cả
    0b00010000  // 9: Sáng a,b,c,d,f,g
};

/**
  * @brief Hiển thị số 0 - 9 lên LED 7 đoạn Anode Chung
  */
void display7SEG(int num)
{
    if (num < 0 || num > 9) return;

    uint8_t code = seg_code[num];

    // Ghi dữ liệu ra các chân PB0..PB6 tương ứng với các bit 0..6
    for (int i = 0; i < 7; i++)
    {
        GPIO_PinState bit_state = (code & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        HAL_GPIO_WritePin(Seg_GPIO_Port, (uint16_t)(1 << i), bit_state);
    }
}

/**
  * @brief Điều khiển 6 đèn giao thông Làn 0 và Làn 1
  */
void setTrafficLeds(GPIO_PinState r0, GPIO_PinState y0, GPIO_PinState g0,
                    GPIO_PinState r1, GPIO_PinState y1, GPIO_PinState g1)
{
    HAL_GPIO_WritePin(Led_Red0_GPIO_Port, Led_Red0_Pin, r0);
    HAL_GPIO_WritePin(Led_Yellow0_GPIO_Port, Led_Yellow0_Pin, y0);
    HAL_GPIO_WritePin(Led_Green0_GPIO_Port, Led_Green0_Pin, g0);

    HAL_GPIO_WritePin(Led_Red1_GPIO_Port, Led_Red1_Pin, r1);
    HAL_GPIO_WritePin(Led_Yellow1_GPIO_Port, Led_Yellow1_Pin, y1);
    HAL_GPIO_WritePin(Led_Green1_GPIO_Port, Led_Green1_Pin, g1);
}
/* USER CODE END 0 */

/**
  * @brief Application entry point
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  /* USER CODE BEGIN WHILE */
  while (1)
  {
      // ===================================================================
      // BÀI 4: ĐẾM NGƯỢC TỪ 9 VỀ 1
      // Đỏ = 9 giây | Xanh = 6 giây | Vàng = 3 giây (6 + 3 = 9)
      // ===================================================================

      // -------------------------------------------------------------------
      // PHA 1: Làn 0 ĐỎ | Làn 1 XANH -> Đếm ngược từ 9 về 4 (6 giây)
      // -------------------------------------------------------------------
      setTrafficLeds(GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET,  // Red0 ON
                     GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET); // Green1 ON

      for (int counter = 9; counter >= 4; counter--)
      {
          display7SEG(counter);
          HAL_Delay(1000);
      }

      // -------------------------------------------------------------------
      // PHA 2: Làn 0 ĐỎ | Làn 1 VÀNG -> Đếm ngược từ 3 về 1 (3 giây)
      // (Tổng thời gian Đỏ Làn 0 = 6s + 3s = 9s)
      // -------------------------------------------------------------------
      setTrafficLeds(GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET,  // Red0 ON
                     GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET); // Yellow1 ON

      for (int counter = 3; counter >= 1; counter--)
      {
          display7SEG(counter);
          HAL_Delay(1000);
      }

      // -------------------------------------------------------------------
      // PHA 3: Làn 0 XANH | Làn 1 ĐỎ -> Đếm ngược từ 9 về 4 (6 giây)
      // -------------------------------------------------------------------
      setTrafficLeds(GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET, // Green0 ON
                     GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET); // Red1 ON

      for (int counter = 9; counter >= 4; counter--)
      {
          display7SEG(counter);
          HAL_Delay(1000);
      }

      // -------------------------------------------------------------------
      // PHA 4: Làn 0 VÀNG | Làn 1 ĐỎ -> Đếm ngược từ 3 về 1 (3 giây)
      // (Tổng thời gian Đỏ Làn 1 = 6s + 3s = 9s)
      // -------------------------------------------------------------------
      setTrafficLeds(GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET, // Yellow0 ON
                     GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET); // Red1 ON

      for (int counter = 3; counter >= 1; counter--)
      {
          display7SEG(counter);
          HAL_Delay(1000);
      }
  }
  /* USER CODE END WHILE */
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

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Enable Clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Mức xuất mặc định: Tắt hết các đèn PA4 -> PA9 */
  HAL_GPIO_WritePin(GPIOA, Led_Green0_Pin|Led_Yellow0_Pin|Led_Red0_Pin
                          |Led_Green1_Pin|Led_Yellow1_Pin|Led_Red1_Pin, GPIO_PIN_RESET);

  /* Mức xuất mặc định: Tắt hết các thanh LED (Anode chung -> xuất HIGH để TẮT) */
  HAL_GPIO_WritePin(GPIOB, Seg1_Pin|Seg2_Pin|Seg3_Pin|Seg4_Pin
                          |Seg5_Pin|Seg6_Pin|Seg7_Pin, GPIO_PIN_SET);

  /* Cấu hình Output GPIOA (PA4 - PA9 cho Đèn giao thông) */
  GPIO_InitStruct.Pin = Led_Green0_Pin|Led_Yellow0_Pin|Led_Red0_Pin
                      |Led_Green1_Pin|Led_Yellow1_Pin|Led_Red1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Cấu hình Output GPIOB (PB0 - PB6 cho LED 7 Đoạn) */
  GPIO_InitStruct.Pin = Seg1_Pin|Seg2_Pin|Seg3_Pin|Seg4_Pin
                      |Seg5_Pin|Seg6_Pin|Seg7_Pin;
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

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
