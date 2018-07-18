/*******************************************************************************
* FILE: node.c
*
* Version: 1.2
*
* Copyright 2016, Bossa Nova Robotics. All rights reserved.
* This software is owned by Bossa Nova Robotics and is protected by and subject
* to worldwide patent and copyright laws and treaties.
*
********************************************************************************
*
* DESCRIPTION:
*     Implements the API used by the specific CAN node slave application.
*******************************************************************************/
#include "global.h"
#include "i2c_psoc.h"
#include "node.h"
#include "sio.h"

extern const uint32 WS_LED_CLUT[ ];

/*******************************************************************************
 * Used to report mask of DIP switch positions.
 ******************************************************************************/
uint8 NODE_GetAddress(void)
{
	return (uint8)(NODE_GetOptions() & 0x0F);
}
/*******************************************************************************
 * Used to report mask of DIP switch positions.
 ******************************************************************************/
uint8 NODE_GetOptions(void)
{
	uint8 mask = 0u;
	mask += NODE_ADDR_6_Read() ? 64 : 0;
	mask += NODE_ADDR_5_Read() ? 32 : 0;
	mask += NODE_ADDR_4_Read() ? 16 : 0;    
	mask += NODE_ADDR_3_Read() ?  8 : 0;
	mask += NODE_ADDR_2_Read() ?  4 : 0;
	mask += NODE_ADDR_1_Read() ?  2 : 0;
	mask += NODE_ADDR_0_Read() ?  1 : 0;   
	return(mask);
}

void NODE_Start(void)
{   
    LED_SYS_Write(LED_OFF);
    LED_DBG_Write(LED_OFF);
    LED_ERR_Write(LED_OFF);
    LED_RUN_Write(LED_OFF);      
    I2C_Start();
    SIO_Start();
    CyDelay(10u);
}


void NODE_ReadEE(uint16 addr, uint8* buffer, size_t size)
{
    I2C_Write (I2C_ADDR_EEPROM, addr, buffer, size);    
}

void NODE_WriteEE(uint16 addr, const uint8* buffer, size_t size)
{
	I2C_Clear();    
    I2C_Write (I2C_ADDR_EEPROM, addr, (uint8*)buffer, size);    
}

/* [] END OF FILE */
