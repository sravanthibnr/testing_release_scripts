
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "canopen_bootloader.h"
#include <project.h>
#include "timer.h"


#define BOOTLOADER_MAX_CMD_LEN Bootloader_SIZEOF_COMMAND_BUFFER


static uint8_t bootloader_cmd_buff[BOOTLOADER_MAX_CMD_LEN];
static uint8_t bootloader_resp_buff[BOOTLOADER_MAX_CMD_LEN];
static int     bootloader_cmd_len = 0;
static uint8_t bootloader_resp_len = 0;


void ClearResponse()
{
  bootloader_resp_buff[0] = 0xFF;
  bootloader_resp_len = 1;
}


/**********************************************************************************************************************
 * PSoC Bootloader Communication Callbacks
 * --------------------------------------------------------------------------------------------------------------------
 * These function definitions and descriptions were taken from Section 10 of the  PSoC® Creator™ Component Author Guide
 * Document # 001-42697 Rev. *S available at http://www.cypress.com/file/137436/download.
 **********************************************************************************************************************/

/** void CyBtldrCommStart(void)
    --------------------------------------------------------------------------------------------------------------
    Description:  This function will start the selected communications component. In many cases, this is just a
                  call to the existing function `@INSTANCE_NAME`_Start()
    Parameters:   None
    Return Value: None
    Side Effects: Starts up the communications component and does any configuration necessary to allow
                  data to be read and/or written by the PSoC. */
void CyBtldrCommStart(void)
{
  TAR_InitHardware();
  TAR_AppInit();
  ClearResponse();
}

/** void CyBtldrCommStop(void)
    --------------------------------------------------------------------------------------------------------------
    Description:  This function will stop the selected communications component. In many cases, this is just a
                  call to the existing function `@INSTANCE_NAME`_Stop()
    Parameters:   None
    Return Value: None
    Side Effects: Stops up the communications component and does any tear down necessary to disable the
                  communications component. */
void CyBtldrCommStop(void)
{
  DLL_UsrCanIntDisable();
  TAR_TimerIntDisable();
}

/** void CyBtldrCommReset(void)
    --------------------------------------------------------------------------------------------------------------
    Description:  Forces the selected communications component to remove stale data. This is used when a
                  command has been interrupted or is corrupt to clear the component’s state and begin again.
    Parameters:   None
    Return Value: None
    Side Effects: Clears any cached data in the communications component and sets the component back to a
                  state to read/write a fresh command. */
void CyBtldrCommReset(void)
{
  bootloader_resp_len = 0;
}

/** cystatus CyBtldrCommWrite(uint8 *data, uint16 size, uint16 *count, uint8 timeOut)
    --------------------------------------------------------------------------------------------------------------
    Description:  Requests that the provided size number of bytes are written from the input data buffer to the
                  host device. Once the write is done count is updated with the number of bytes written. The
                  timeOut parameter is used to provide an upper bound on the time that the function is allowed
                  to operate. If the write completes early it should return a success code as soon as possible.
                  If the write was not successful before the allotted time has expired it should return an error.
    Parameters:   uint8 *data   – pointer to the buffer containing data to be written
                  uint16 size   – the number of bytes from the data buffer to write
                  uint16 *count – pointer to where the comm. component will write the count of the number of
                                   bytes actually written
                  uint8 timeOut – amount of time (in units of 10 milliseconds) the comm. component should
                                  wait before indicating communications timed out
    Return Value: CYRET_SUCCESS if one or more bytes were successfully written. CYRET_TIMEOUT if the
                  host controller did not respond to the write in 10 milliseconds * timeOut milliseconds.
    Side Effects: None */
cystatus CyBtldrCommWrite(uint8 *data, uint16 size, uint16 *count, uint8 timeOut)
{
  memcpy(bootloader_resp_buff, data, size);
  *count = size;
  bootloader_resp_len = size;
  return CYRET_SUCCESS;
}

/** cystatus CyBtldrCommRead(uint8 *data, uint16 size, uint16 *count, uint8 timeOut)
    --------------------------------------------------------------------------------------------------------------
    Description:  Requests that the provided size number of bytes are read from the host device and stored in
                  the provided data buffer. Once the write is done count is updated with the number of bytes
                  written. The timeOut parameter is used to provide an upper bound on the time that the
                  function is allowed to operate. If the read completes early it should return a success code as
                  soon as possible. If the read was not successful before the allotted time has expired it should
                  return an error.
    Parameters:   uint8 *data   – pointer to the buffer to store data from the host controller
                  uint16 size   – the number of bytes to read into the data buffer
                  uint16 *count – pointer to where the comm. component will write the count of the number of
                                  bytes actually read
                  uint8 timeOut – amount of time (in units of 10 milliseconds) the comm. component should
                                  wait before indicating communications timed out
    Return Value: CYRET_SUCCESS if one or more bytes were successfully read. CYRET_TIMEOUT if the
                  host controller did not respond to the read in 10 milliseconds * timeOut milliseconds.
    Side Effects: None */
cystatus CyBtldrCommRead(uint8 *data, uint16 size, uint16 *count, uint8 timeOut)
{
  COP_t_Timer start_time = SysTick_GetTicks();
  while((bootloader_cmd_len == 0) && (timeOut == 0xFF || SysTick_GetTicks() < start_time + (timeOut*10)))
    TAR_AppRun();
  
  if (bootloader_cmd_len > 0)
  {
    memcpy(data, bootloader_cmd_buff, bootloader_cmd_len);
    *count = bootloader_cmd_len;
    bootloader_cmd_len = 0;
    ClearResponse();
    return CYRET_SUCCESS;
  }
  
  return CYRET_TIMEOUT;
}

/*********************************************************************************************************************/


/**********************************************************************************************************************
 * Object Dictionary Callbacks
 * --------------------------------------------------------------------------------------------------------------------
 * These functions are used to communicate with the CANopen stack/
 **********************************************************************************************************************/

/*************************************************************************
**
** Function    : BLR_MsgReceive
**
** Description : User extention of CANopen Bootloader stack.
**               Callback on the segmented SDO download of the 
**               application.
**                
** Parameters  : pdw_len     (IN/OUT)  - pointer to the length of SDO
**               pdw_pos     (IN/OUT)  - pointer to the current SDO position
**               pb_data     (IN/OUT)  - pointer to the next data segment
**               pb_len      (IN/OUT)  - pointer to the length of segment
**                                       NULL => request object length,
**                                       returned in pdw_len
**               b_type      (IN)      - SDO type (upload,download,abort) 
**                
** Returnvalue : SDO_k_ABORT_OK - continue SDO communication
**               SDO_k_ABORT_GEN_ERROR or other abort codes - abort SDO
**
*************************************************************************/
UINT8 Program_ObjReceive(UINT32  *pdw_len, 
                         UINT32  *pdw_pos, 
                         UINT8   *pb_data, 
                         UINT8   *pb_len, 
                         UINT8   b_type)
{
  
  /* Object 1F50 is WO, only download is possible */
  switch(b_type)
  {
    case SDO_k_DOWNLOAD:
    {
      /* check if data size requested */
      if (pb_len == NULL)
      {
        /* SDO module shall accept maximum command size. */
        *pdw_len = BOOTLOADER_MAX_CMD_LEN;
        return SDO_k_ABORT_OK;
      }
      
      // Copy segment into command buffer and move the SDO position forward
      memcpy((void*)(bootloader_cmd_buff+*pdw_pos), (void*)pb_data, *pb_len);
      *pdw_pos += *pb_len;
      bootloader_cmd_len = 0;
      
      /* last data received */
      if( *pdw_pos == *pdw_len )  
        bootloader_cmd_len = *pdw_len;
      
      break;
    }
    case SDO_k_UPLOAD:
    {
      /* check if data size requested */
      if (pb_len == NULL)
      {
        /* SDO module shall accept maximum command size. */
        *pdw_len = bootloader_resp_len;
        return SDO_k_ABORT_OK;
      }
      
      // Calculate the length of the current segment (max 7 bytes)
      if(((*pdw_len) - (*pdw_pos)) > 7)
        *pb_len = 7;
      else
        *pb_len = (*pdw_len) - (*pdw_pos);
      
      // Copy segment into command buffer and move the SDO position forward
      memcpy((void*)pb_data, (void*)(bootloader_resp_buff+*pdw_pos), *pb_len);
      *pdw_pos += *pb_len;
      
      break;
    }
    case SDO_k_ABORT:
    {
      return SDO_k_ABORT_NO;
    }
  }
  return SDO_k_ABORT_OK;
}

/*********************************************************************************************************************/


/**********************************************************************************************************************
 * Target Specific LED Control Callbacks
 **********************************************************************************************************************/

#if (TAR_k_ENABLE_LED == 1)
  
void TAR_InitLeds( void )
{
    LED_RUN_Write(LED_OFF);	 // Green   
    LED_ERR_Write(LED_OFF);	 // Red	 
    LED_SYS_Write(LED_ON);
    LED_DBG_Write(LED_ON);
}

void TAR_SwitchGrnLed(BOOLEAN state)
{
	if (state)
		LED_RUN_Write(LED_ON);
	else
		LED_RUN_Write(LED_OFF);
}

void TAR_SwitchRedLed(BOOLEAN state)
{
	if (state)
		LED_ERR_Write(LED_ON);
	else
		LED_ERR_Write(LED_OFF);
}

#endif

UINT8 USR_GetNodeId(void)
{
  return ((NODE_ADDR_0_Read() ? 0 : 1)  |
          (NODE_ADDR_1_Read() ? 0 : 2)  |
          (NODE_ADDR_2_Read() ? 0 : 4)  |
          (NODE_ADDR_3_Read() ? 0 : 8)  |
          (NODE_ADDR_4_Read() ? 0 : 16) |
          (NODE_ADDR_5_Read() ? 0 : 32) |
          (NODE_ADDR_6_Read() ? 0 : 64));                  
}

void USR_GetUI(void)
{
    Bootloader_Start();
}

/*********************************************************************************************************************/


