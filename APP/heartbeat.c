/********************************** (C) COPYRIGHT *******************************
 * File Name          : heartbeat.c
 * Author             :
 * Version            : V1.0
 * Date               : 2026/04/01
 * Description        : MCU 心跳灯任务，使用 PA12 以 500ms 周期翻转
 *******************************************************************************/

#include "CONFIG.h"
#include "heartbeat.h"

#define HEARTBEAT_GPIO_PIN         GPIO_Pin_12
#define HEARTBEAT_BLINK_PERIOD_MS  500

static tmosTaskID HeartbeatTaskId = INVALID_TASK_ID;

void Heartbeat_Init(void)
{
    HeartbeatTaskId = TMOS_ProcessEventRegister(Heartbeat_ProcessEvent);

    GPIOA_ResetBits(HEARTBEAT_GPIO_PIN);
    GPIOA_ModeCfg(HEARTBEAT_GPIO_PIN, GPIO_ModeOut_PP_5mA);

    tmos_start_task(HeartbeatTaskId,
                    HEARTBEAT_BLINK_EVT,
                    MS1_TO_SYSTEM_TIME(HEARTBEAT_BLINK_PERIOD_MS));
}

uint16_t Heartbeat_ProcessEvent(uint8_t task_id, uint16_t events)
{
    (void)task_id;

    if(events & HEARTBEAT_BLINK_EVT)
    {
        GPIOA_InverseBits(HEARTBEAT_GPIO_PIN);
        tmos_start_task(HeartbeatTaskId,
                        HEARTBEAT_BLINK_EVT,
                        MS1_TO_SYSTEM_TIME(HEARTBEAT_BLINK_PERIOD_MS));
        
        PRINT("Hellow World!\r\n");

        return events ^ HEARTBEAT_BLINK_EVT;
    }

    return 0;
}
