#ifndef _TIMER_H_
#define _TIMER_H_
/*******************************************************************************
* FILE: sio.h
*
* Version: 1.0
*
* Copyright 2016, Bossa Nova Robotics. All rights reserved.
* This software is owned by Bossa Nova Robotics and is protected by and subject
* to worldwide patent and copyright laws and treaties.
*
********************************************************************************
*
* DESCRIPTION:
*   The SysTick timer is an integral part of the ARM® Cortex™-M0 processor that
*   powers the PSoC 4 family. The timer is a down counter with a 24-bit reload/tick
*   value and clocked by the system clock (SysClk) reaching the Cortex-M0 from the
*   PSoC 4 clocking system. The timer has the capability to generate an interrupt
*   when the set number of ticks expires and the counter reloads. This interrupt
*   is available as part of the Nested Vectored Interrupt Controller (NVIC) for
*   service by the CPU and can be used for general purpose timing control in the
*   user code. Since the timer is independent of the CPU (except for the clock), 
*   it can be handy in applications requiring precise timing but not having a
*   dedicated timer/counter available for the job.
*******************************************************************************/
#include <project.h>

#define TIMER_ID_500      0
#define TIMER_ID_250      1
#define TIMER_ID_100      2
#define TIMER_ID_050      3
#define TIMER_ID_SENSOR   4
#define TIMER_ID_SECONDS  5    
#define TIMER_ID_LAST     6 
    
/* Function prototypes */
void SysTick_Callback(void);     
uint16 SysTick_GetMicroseconds(void);
uint32 SysTick_GetMinutes(void);
uint32 SysTick_GetSeconds(void);
uint32 SysTick_GetStatus(void);
uint32 SysTick_GetTicks(void);
uint32 SysTick_GetTimestamp(void);
void SysTick_Refresh(void);
void SysTick_Start(void);
uint8 SysTick_TimerCheck(uint8 index);
void SysTick_TimerSet(uint8 index, uint32 milliseconds);

#endif

/* [] END OF FILE */
