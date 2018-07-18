#ifndef _NODE_H_
#define _NODE_H_
/*******************************************************************************
* FILE: node.h
*
* Version: 1.0
*
* Copyright 2016, Bossa Nova Robotics. All rights reserved.
*
* This software is owned by Bossa Nova Robotics and is protected by and subject
* to worldwide patent and copyright laws and treaties.
*
********************************************************************************
*
* DESCRIPTION:
*     This is the public interface to routines known collectively as the device
* node subsystem.  The routines implemented here constitute the board level API
* used for developing the specific CAN node slave application.
*******************************************************************************/
#include <project.h>

/*******************************************************************************
 * Function prototypes.
 ******************************************************************************/    
uint32 NODE_LED_GetColor(int row, uint8 option);
void NODE_LED_Test(uint8 option);
void NODE_LED_SetRowColor(uint8_t row, uint32_t color);
void NODE_LED_SetAllColor(uint32_t color);
void NONE_LED_SetPixel(uint8_t column, uint8_t row, uint32_t color);
void NODE_LED_SetRowDim(uint8_t row, uint32_t dim);
void NODE_LED_SetAllDim(uint32_t dim);

/* Baseline methods common to all nodes*/
uint8 NODE_GetAddress(void);
uint8 NODE_GetOptions(void);
void NODE_Start(void);
void NODE_ReadEE(uint16 addr, uint8* buffer, size_t size);
void NODE_WriteEE(uint16 addr, const uint8* buffer, size_t size);

void NODE_Test(void);
#endif  // _CARDS_H_
/* [] END OF FILE */
