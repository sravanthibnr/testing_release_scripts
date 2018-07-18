/*******************************************************************************
* FILE: slave_impl.c
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
*     This module implements the CAN slave SDO routines
*
*******************************************************************************/

#include "cytypes.h"
#include "string.h"

#include "i2c_psoc.h"
#include "node.h"
#include "slave_framework.h"
#include "usr_impl.h"



#define IDAC_ID_ALL 0
#define IDAC_ID_1 1
#define IDAC_ID_2 2
#define IDAC_ID_3 3
#define IDAC_ID_4 4

#define ACN_BASE_INDEX              (0x2600)
#define ACN_NODE_ID                 (0x17)

#if (TAR_k_ENABLE_LED == 1)
  
void USR_InitLeds( void )
{
	LED_RUN_Write(LED_OFF);	 // Green   
	LED_ERR_Write(LED_OFF);	 // Red	 
}

void USR_SwitchGrnLed(BOOLEAN state)
{
	if (state)
		LED_RUN_Write(LED_ON);
	else
		LED_RUN_Write(LED_OFF);
}


void USR_SwitchRedLed(BOOLEAN state)
{
	if (state)
		LED_ERR_Write(LED_ON);
	else
		LED_ERR_Write(LED_OFF);
}

#endif




/*************************************************************************
**
** Function    : USR_GetNodeId
**
** Description : Gets the node ID from the addressable bit field
**
** Parameters  : void
**
** Returnvalue : 8 bit Node ID
**
*************************************************************************/

UINT8 USR_GetNodeId(void)
{
  return (23);                  
}


/*************************************************************************
**
** Function    : USR_IntSyncIndication
**
** Description : Indication functions  if sync received. This function
**               from interrupt function
**
** Parameters  : pb_data     (IN) - Pointer to received CAN Message
**               pb_len      (IN) - Length of received message
**
** Returnvalue : -
**
*************************************************************************/
void USR_IntSyncIndication(UINT8* pb_data, UINT8* pb_len)
{
	pb_data = pb_data; /* only to suppress compiler warning */
	pb_len  = pb_len;  /* only to suppress compiler warning */

}
/*************************************************************************
**
** Function    : USR_SyncIndication
**
** Description : Indication functions if sync received. The SYNC
**               is get from receive queue
**
** Parameters  : -
**
** Returnvalue : -
**
*************************************************************************/
void USR_SyncIndication(void)
{
	PRINTF_ARG0("sync received\r\n");
}

uint32_t uid[2] = { 0 };

/*******************************************************************************
 * 
 ******************************************************************************/
void USR_Start(void)
{
    I2C_Start();

    // WS_LED_cisr_StartEx
    // to set new interrupt controllable by us
}

/*************************************************************************
**
** Function    : slave_framework_objcb
**
** Description : Callback function for SDO access and dynamic PDO mapping
**
** Parameters  : Idx         (IN) - Memory index of object
**               srvc        (IN) - Indicator which service is performed
**                                  possible values:
**                                  - COP_k_SDO_READ
**                                  - COP_k_SDO_BEFORE_WRITE
**                                  - COP_k_SDO_AFTER_WRITE
**                                  - COP_k_SDO_READ_OBJLEN
**                                  - COP_k_SDO_READ_MAX_OBJLEN
**                                  - COP_k_SDO_WRITE_SEGMENT
**                                  - COP_k_PDO_WRITE
**
** Returnvalue : COP_k_OK         - Success
**               COP_k_NO         - Failure
**               actual length    - Actual length of the object,
**                                  if srvc COP_k_SDO_READ_OBJLEN
**               max length       - Max length of the object,
**                                  if srvc COP_k_SDO_READ_MAX_OBJLEN
**
*************************************************************************/
COP_t_OBJ_LEN slave_framework_objcb(PDO_t_Idx Idx, UINT8 srvc)
{    
	Idx = Idx; /* avoid compiler warning */
    
    PRINTF_ARG0("Received Object\r\n");

	if ( srvc == COP_k_SDO_READ_OBJLEN )
	{
		PRINTF_ARG2("COP_k_SDO_READ_OBJLEN : Index %xh Subindex %xh\r\n", OBD_s_ObjectInfo.index, OBD_s_ObjectInfo.subindex);
		/* length of the object (not used in demo, here always 0x10) */
		return (0x10);
	}
	else if ( srvc == COP_k_SDO_READ_MAX_OBJLEN )
	{
		PRINTF_ARG2("COP_k_SDO_READ_MAX_OBJLEN : Index %xh Subindex %xh\r\n", OBD_s_ObjectInfo.index, OBD_s_ObjectInfo.subindex);
		/* max. length of the object (not used in demo, here always 0x10) */
		return (0x10);
	}
	else if ( srvc == COP_k_SDO_BEFORE_WRITE )
	{
		PRINTF_ARG2("COP_k_SDO_BEFORE_WRITE : Index %xh Subindex %xh\r\n", OBD_s_ObjectInfo.index, OBD_s_ObjectInfo.subindex);

		/* OBD_s_ObjectInfo.p_sdobuf is pointer to "new value"          */
		/* if value is not allowed, an return value != COP_k_OK         */
		/* aborts the transfer and the actual value is not overwritten  */
		/* Remark: The pointer is of type *UINT8, so be aware on 16-Bit */
		/*         microcontrollers concerning the alignment            */

		return (COP_k_OK);
	}
#if (SDO_FLASH_SUPPORT == 1)
	else if ( srvc == COP_k_SDO_WRITE_SEGMENT )
	{
		/* example of writing a testobject defined with attributes ATTR_FLASH|NO_SDO_BUF */
		/* OBD_s_ObjectInfo.datalength => offset in destination  */
		/* Idx => datalength in actual segment                   */
		/* OBD_s_ObjectInfo.p_sdobuf => pointer to source buffer */
		PRINTF_ARG4("COP_k_SDO_WRITE_SEGMENT: index %xh Subindex %xh SegNo %d NumBytes %d\r\n",
					OBD_s_ObjectInfo.index,
					OBD_s_ObjectInfo.subindex,
					OBD_s_ObjectInfo.datalength / 7, /* max 7 bytes per segment */
					Idx
				   );
	}
#endif
	else if ( srvc == COP_k_SDO_AFTER_WRITE )
	{
        uint16_t index = OBD_s_ObjectInfo.index;
		PRINTF_ARG2("COP_k_SDO_AFTER_WRITE : Index %xh Subindex %xh\r\n", OBD_s_ObjectInfo.index, OBD_s_ObjectInfo.subindex);
		
        if (index == ACN_BASE_INDEX )
        {
            uint8_t* dataPtr = OBD_s_ObjectInfo.p_object;
            if ((*dataPtr) == 1)
                USR_SPKR_Enable();
            else
                USR_SPKR_Disable();
        }
        
        return (COP_k_OK);
	}
	else if ( srvc == COP_k_SDO_READ )
	{
		PRINTF_ARG2("COP_k_SDO_READ : Index %xh Subindex %xh\r\n", OBD_s_ObjectInfo.index, OBD_s_ObjectInfo.subindex);
    

        
		return (COP_k_OK);
	}
	else if ( srvc == COP_k_PDO_WRITE )
	{
		PRINTF_ARG1("COP_k_PDO_WRITE : memindex %xh\r\n", Idx);
		return (COP_k_OK);
	}

	return (COP_k_OK);
}



/* [] END OF FILE */
