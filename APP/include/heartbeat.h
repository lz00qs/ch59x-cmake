/********************************** (C) COPYRIGHT *******************************
 * File Name          : heartbeat.h
 * Author             :
 * Version            : V1.0
 * Date               : 2026/04/01
 * Description        : MCU 心跳灯任务
 *******************************************************************************/

#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#ifdef __cplusplus
extern "C" {
#endif

#define HEARTBEAT_BLINK_EVT    0x0001

void Heartbeat_Init(void);
uint16_t Heartbeat_ProcessEvent(uint8_t task_id, uint16_t events);

#ifdef __cplusplus
}
#endif

#endif /* HEARTBEAT_H */
