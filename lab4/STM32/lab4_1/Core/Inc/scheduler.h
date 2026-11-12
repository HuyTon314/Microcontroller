#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include "main.h"

// Cấu trúc dữ liệu quản lý Task
typedef struct {
    void (*pTask)(void); // Con trỏ hàm trỏ tới nhiệm vụ
    int32_t Delay;      // Thời gian trễ (tính bằng số tick)
    uint32_t Period;     // Chu kỳ lặp lại (tính bằng số tick)
    uint8_t  RunMe;      // Cờ báo số lần task sẵn sàng chạy
    uint32_t TaskID;     // ID định danh duy nhất của task
} sTask;

#define SCH_MAX_TASKS 40
#define NO_TASK_ID     0

// Khai báo các hàm quản lý Scheduler
void     SCH_Init(void);
void     SCH_Update(void);
void     SCH_Dispatch_Tasks(void);
uint32_t SCH_Add_Task(void (*pFunction)(void), int32_t DELAY, uint32_t PERIOD);
uint8_t  SCH_Delete_Task(uint32_t taskID);
uint32_t get_time(void);

#endif /* INC_SCHEDULER_H_ */
