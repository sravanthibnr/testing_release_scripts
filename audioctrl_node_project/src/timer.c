/*******************************************************************************
* FILE: sio.c
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
#include "target.h"
#include "timer.h"

//#define DEBUG_TIMERS

#define TIME_MIN_IN_HR      (60u)
#define TIME_SECS_IN_MIN     (60u)
#define TIME_MS_IN_SEC      (1000u)

/* SysClk = 24 MHz, therefore each clock cycle is 41.667 nanoseconds */
#define SYS_TICK_MSEC (24000u) /* Number of cycles per millisecond */
#define SYS_TICK_USEC (2400u)  /* Number of cycles per microsecond */

/* Global Variables */
volatile uint32 tick_seconds; 
volatile uint32 tick_milliseconds;
volatile uint32 tick_status;
uint32 timer_interval[TIMER_ID_LAST];
uint8 timer_check[TIMER_ID_LAST];

/* Function prototypes */
void SysTick_Callback(void);

/*******************************************************************************
* Function Name: SysTick_Callback
********************************************************************************
*
* Summary:
*  This API is called from SysTick timer interrupt handler to update the
*  millisecond counter.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SysTick_Callback(void)
{
	tick_status = 1u;
    ++tick_milliseconds;
    
   	/* Also count the number of milliseconds in one second */
    if ((tick_milliseconds % TIME_MS_IN_SEC) == 0u)
    {
        ++tick_seconds;
    }
}

/*******************************************************************************
* Function Name: SysTickRefresh
********************************************************************************
*
* Summary:
*  This API is called from main program to update the millisecond counter.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SysTick_Refresh(void)
{
    uint32 tick;
#ifdef DEBUG_TIMERS    
	char message[32u];
#endif    
	/* Tick status fires only once every millisecond. */
	if ( tick_status != 0u )
	{
		tick_status = 0u;  		/* Reset tick status. */
        tick = tick_milliseconds;
                
        if ((tick % 500u) == 0u)
        {
            timer_check[TIMER_ID_500] = 1u;
        }
        
        if ((tick % 250u) == 0u)
        {
            timer_check[TIMER_ID_250] = 1u;
            timer_check[TIMER_ID_SENSOR] = 1u;
        }
        
        if ((tick % 100u) == 0u)
        {
            timer_check[TIMER_ID_100] = 1u;
        }

        if ((tick % 50u) == 0u)
        {
            timer_check[TIMER_ID_050] = 1u;
        }        
	}
}

/*******************************************************************************
* Function Name: SysTick_Start
********************************************************************************
*
* Summary:
*  Configure the SysTick timer to generate interruupt every 1 millisecond and
*  start its operation.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SysTick_Start(void)
{
	uint32 i;
    
	/* Initialize global variables. */  
	tick_status = 0u;
    tick_seconds = 0u;
    tick_milliseconds = 0u;   
    memset(timer_interval, 0, sizeof(timer_interval));
    memset(timer_check, 0, sizeof(timer_check));
   
	CySysTickStart();
    
	/* Find unused callback slot. */
	for ( i = 0u; i < CY_SYS_SYST_NUM_OF_CALLBACKS; ++i )
	{
		if ( CySysTickGetCallback(i) == NULL )
		{
			/* Set callback */
			CySysTickSetCallback(i, SysTick_Callback);
			break;
		}
	}
}

uint32 SysTick_GetMinutes(void)
{
    uint32 t = tick_seconds / TIME_SECS_IN_MIN;
    return (t);
}

uint32 SysTick_GetSeconds(void)
{
    uint32 t = tick_seconds;
    return (t);
}

uint32 SysTick_GetStatus(void)
{
    uint32 t = tick_status;
    return (t);
}

uint32 SysTick_GetTicks(void)
{
    uint32 t = tick_milliseconds;
    return (t);
}

/* This takes advantage of the SysTick timer to obtain a resolution greater
 * than one millisecond */

uint16 SysTick_GetMicroseconds(void)
{
    uint16 usecs;
    uint32 reload = (uint32)(SYS_TICK_MSEC - (CY_SYS_SYST_CVR_REG & CY_SYS_SYST_CVR_CNT_MASK));
    usecs = (uint16)(reload % SYS_TICK_USEC);
    return (usecs);
}

uint32 SysTick_GetTimestamp(void)
{
    uint32 t = (tick_milliseconds);
    //t |= SysTick_GetMicroseconds();    
    return (t);
}

uint8 SysTick_TimerCheck(uint8 index)
{
    uint8 status = 0u;

    if (index < TIMER_ID_LAST)
    {
        if (timer_check[index] != 0u)
        {
            status = 1u;
            timer_check[index] = 0u;
        }
    }
    
    return (status);
}

void SysTick_TimerSet(uint8 index, uint32 milliseconds)
{
    if (index < TIMER_ID_LAST)
    {
        timer_interval[index] = milliseconds;
    }
}
/* [] END OF FILE */
