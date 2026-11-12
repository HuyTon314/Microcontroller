#include "scheduler.h"

sTask SCH_tasks_G[SCH_MAX_TASKS];
uint8_t current_index_task = 0;
volatile uint32_t system_ticks = 0;
static uint32_t g_next_task_id = 1; // Bộ đếm ID duy nhất toàn cục

void SCH_Init(void) {
    unsigned char i;
    for (i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_G[i].pTask = 0;
        SCH_tasks_G[i].Delay = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].RunMe = 0;
        SCH_tasks_G[i].TaskID = 0;
    }
    current_index_task = 0;
    system_ticks = 0;
    g_next_task_id = 1;

}

uint32_t SCH_Add_Task(void (*pFunction)(void), int32_t DELAY, uint32_t PERIOD) {
    if (current_index_task >= SCH_MAX_TASKS) {
        return SCH_MAX_TASKS;
    }

    uint32_t new_id = g_next_task_id++; // Cấp ID duy nhất không trùng
    int i = 0;
    int32_t new_delay = DELAY;

    // Tìm vị trí chèn trong Delta Queue
    while (i < current_index_task && new_delay >= SCH_tasks_G[i].Delay) {
        new_delay -= SCH_tasks_G[i].Delay;
        i++;
    }

    // Dời các task phía sau sang phải
    for (int j = current_index_task; j > i; j--) {
        SCH_tasks_G[j] = SCH_tasks_G[j - 1];
    }

    // Trừ chênh lệch delay cho task đứng liền sau
    if (i < current_index_task) {
        SCH_tasks_G[i + 1].Delay -= new_delay;
    }

    // Gán dữ liệu task
    SCH_tasks_G[i].pTask  = pFunction;
    SCH_tasks_G[i].Delay  = new_delay;
    SCH_tasks_G[i].Period = PERIOD;
    SCH_tasks_G[i].RunMe  = 0;
    SCH_tasks_G[i].TaskID = new_id;

    if (new_delay <= 0) {
            SCH_tasks_G[i].RunMe = 1;
        } else {
            SCH_tasks_G[i].RunMe = 0;
        }

    current_index_task++;
    return new_id;
}

/* SCH_Update đạt độ phức tạp O(1) tuyệt đối - Không chứa vòng lặp */
void SCH_Update(void) {
    system_ticks++;
    if (current_index_task > 0) {
        SCH_tasks_G[0].Delay--;

        if (SCH_tasks_G[0].Delay <= 0) {
            SCH_tasks_G[0].RunMe = 1;
        }
    }
}

void SCH_Dispatch_Tasks(void)
{
    // Điều kiện vòng lặp sử dụng cờ RunMe
    while (current_index_task > 0 && SCH_tasks_G[0].RunMe > 0)
    {
        void (*pTask)(void) = SCH_tasks_G[0].pTask;
        uint32_t period = SCH_tasks_G[0].Period;
        uint32_t taskID = SCH_tasks_G[0].TaskID;

        // Lưu lại độ trễ (nếu <= 0 nghĩa là task đã bị lố nhịp)
        int32_t overrun = SCH_tasks_G[0].Delay;

        __disable_irq(); // Khóa ngắt Timer bảo vệ mảng

        // Lấy ra khỏi hàng đợi
        SCH_Delete_Task(taskID);

        // Chèn lại vào Delta Queue nếu là task định kỳ
        if (period > 0)
        {
            // Bù trừ thời gian chạy lố
            int32_t next_delay = period + overrun;
            if (next_delay < 0) next_delay = 0; // Chống lỗi âm sâu
            SCH_Add_Task(pTask, next_delay, period);
        }

        __enable_irq(); // Mở lại ngắt Timer

        // Thực thi task ngoài vùng cấm ngắt
        if (pTask != 0)
        {
            pTask();
        }
    }
}

uint8_t SCH_Delete_Task(uint32_t taskID) {
    if (current_index_task == 0) {
        return 0;
    }

    int i = 0;
    while (i < current_index_task && SCH_tasks_G[i].TaskID != taskID) {
        i++;
    }

    if (i == current_index_task) {
        return 0; // Không tìm thấy ID
    }

    // Cộng trả Delay của task bị xóa cho task đứng liền sau
    if (i + 1 < current_index_task) {
        SCH_tasks_G[i + 1].Delay += SCH_tasks_G[i].Delay;
    }

    // Dồn hàng đợi sang trái
    for (int j = i; j < current_index_task - 1; j++) {
        SCH_tasks_G[j] = SCH_tasks_G[j + 1];
    }

    current_index_task--;
    SCH_tasks_G[current_index_task].pTask = 0;
    SCH_tasks_G[current_index_task].Delay = 0;
    SCH_tasks_G[current_index_task].Period = 0;
    SCH_tasks_G[current_index_task].RunMe = 0;
    SCH_tasks_G[current_index_task].TaskID = 0;

    return 1;
}

uint32_t get_time(void) {
    return system_ticks * 10;
}
