/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : 观察应用主函数及任务系统初始化
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "HAL.h"
#include "heartbeat.h"
#include "observer.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4))) uint32_t MEM_BUF[BLE_MEMHEAP_SIZE / 4];

#if(defined(BLE_MAC)) && (BLE_MAC == TRUE)
const uint8_t MacAddr[6] = {0x84, 0xC2, 0xE4, 0x03, 0x02, 0x02};
#endif

static void PrintUuidDerivedKey(void)
{
    uint8_t uid[8] __attribute__((aligned(4)));
    uint8_t key[16];
    uint8_t i;

    GET_UNIQUE_ID(uid);

    for(i = 0; i < sizeof(uid); i++)
    {
        key[i] = uid[i];
        key[i + sizeof(uid)] = uid[i];
    }

    PRINT("UUID: ");
    for(i = 0; i < sizeof(uid); i++)
    {
        PRINT("%02x", uid[i]);
    }
    PRINT("\n");

    PRINT("AES-128 key: ");
    for(i = 0; i < sizeof(key); i++)
    {
        PRINT("%02x", key[i]);
    }
    PRINT("\n");
}

/*********************************************************************
 * @fn      Main_Circulation
 *
 * @brief   主循环
 *
 * @return  none
 */
__HIGH_CODE
__attribute__((noinline))
void Main_Circulation()
{
    while(1)
    {
        TMOS_SystemProcess();
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   主函数
 *
 * @return  none
 */
int main(void)
{
#if(defined(DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
    PWR_DCDCCfg(ENABLE);
#endif
    SetSysClock(CLK_SOURCE_PLL_60MHz);
#if(defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
    GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
    GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
#endif
#ifdef DEBUG
    GPIOPinRemap(ENABLE, RB_PIN_UART0);
    GPIOA_SetBits(GPIO_Pin_14);
    GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeIN_PU);
    UART0_DefInit();
#endif
    PRINT("%s\n", VER_LIB);
    PrintUuidDerivedKey();
    CH59x_BLEInit();
    HAL_Init();
    Heartbeat_Init();
    GAPRole_ObserverInit();
    Observer_Init();
    Main_Circulation();
}

/******************************** endfile @ main ******************************/
