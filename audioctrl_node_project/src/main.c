/*******************************************************************************
* File: main.c
*
* Version: 1.0
*
* Description:
*  TBD
*
********************************************************************************
* Copyright 2016, Bossa Nova Robotics. All rights reserved.
* This software is owned by Bossa Nova Robotics and is protected by and subject
* to worldwide patent and copyright laws and treaties.
*******************************************************************************/
#define _MAIN_ROUTINE_
/*************************************************************************
**    include-files
*************************************************************************/
#include "global.h"
#include "usr.h"

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  Starts the SysTick timer and sets the callback function that will be called
*  on SysTick interrupt. Updates timing variables at one millisecond rate and
*  prints the system time to the UART once every second in the main loop.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
int main()
{  
#if 1
    USR_CANopenApplication();
#else
    USR_SniffTest();
#endif
    return( 0 );    
}

/* [] END OF FILE */
