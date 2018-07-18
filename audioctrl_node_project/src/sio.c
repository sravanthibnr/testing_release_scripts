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
*     This is the public interface to routines known collectively as the serial
* input output subsystem.  These routines are essentially wrappers for UART
* communications that utilize the serial debug port.
*******************************************************************************/
#include "target.h"
#include "sio.h"

// Size of the circular receive buffer, must be power of 2
#ifndef SIO_RX_BUFFER_SIZE
	#define SIO_RX_BUFFER_SIZE 128
#endif

//#define SIO_DEBUG_LED
// Size of the circular transmit buffer, must be power of 2
#ifndef SIO_TX_BUFFER_SIZE
	#define SIO_TX_BUFFER_SIZE 128
#endif

#define SIO_RX_BUFFER_MASK (SIO_RX_BUFFER_SIZE - 1)
#define SIO_TX_BUFFER_MASK (SIO_TX_BUFFER_SIZE - 1)

uint8 sio_rx_buffer[SIO_RX_BUFFER_SIZE];
uint8 sio_rx_head = 0;	// Index of oldest element
uint8 sio_rx_tail = 0;	// Index at which to write new element
uint8 sio_rx_error = 0;	// 

char sio_tx_buffer[SIO_TX_BUFFER_SIZE];
uint8 sio_tx_head;	// Index of oldest element
uint8 sio_tx_tail;	// Index at which to write new element
uint8 sio_tx_error;	// 

/*******************************************************************************
 * Helper function to transmit bad command string.
 *******************************************************************************/
void SIO_BadCommand(void)
{
    UART_PUT_STRING("BAD COMMAND\r\n");
    SIO_Clear();  
}

/*******************************************************************************
 * Transfers contents of UART to RX buffer.
 *******************************************************************************/
uint8 SIO_CheckHost (void)
{
    uint8 count = SIO_GetRxBufferSize();
    uint8 i;    
    
    if (count > 0)
    {        
        for (i=0; i < count; i++)
        {
            sio_rx_buffer[sio_rx_tail++] = SIO_ReadRxData();
        }
        
        SIO_ClearRxBuffer();
    }

	return(sio_rx_tail);
}

/*******************************************************************************
 * Checks contents of RX buffer for end-of-command character.
 *******************************************************************************/
uint8 SIO_CheckReady(void)
{    
    if ( strchr((char*)sio_rx_buffer, CR) != NULL )
    {
        return (1u);
    }
    
    if ( strchr((char*)sio_rx_buffer, LF) != NULL )
    {
        return (1u);
    }
    
    if ( strchr((char*)sio_rx_buffer, ETX) != NULL )
    {
        return (1u);
    }
    
    return (0u);
}

/*******************************************************************************
 * Remove all characters not considered valid for command language interpreter
 * particularly when manually entered using terminal emulator.
*******************************************************************************/
void SIO_CleanBuffer(void)
{
	size_t i;
	size_t j = 0;
	size_t k;
	size_t length = sio_rx_tail;
	char* buffer = (char*)sio_rx_buffer;
	char c;

#ifdef SIO_DEBUG    
	SIO_HexDump(buffer, length);
#endif

	for ( i = 0; i < length; i++ )
	{
		if ( i == 0 )
		{
			j = 0;
		}

		c = buffer[i];

		if ( (c == '#') || (c == ETX) || (c == EOT) || (c == LF) || (c == CR) )
		{
			c = 0;
			buffer[j] = c;
			break;
		}

		if ( (c == BS) || (c == DEL) )
		{
			for ( k = i; k < length; k++ )
			{
				c = buffer[k+1];
				buffer[k-1] = c;
			}

			buffer[k-1] = 0;
			c = 0;
			i = -1;
		}

		if ( (c > SPACE) && (c < DEL) )
		{
			if ( (c >= 'a') && (c <= 'z') )
			{
				c &= 0xDF;
			}

			buffer[j++] = c;
		}
	}

    sio_rx_tail = j;
#ifdef SIO_DEBUG     
	SIO_HexDump(buffer, length);
#endif
}

/*******************************************************************************
 * Resets contents and pointers to SIO buffers
 *******************************************************************************/
void SIO_Clear(void)
{
    sio_rx_tail = 0;
    sio_rx_head = 0;
    sio_tx_head = 0;
    sio_tx_tail = 0;   
    memset(sio_rx_buffer, 0, sizeof(sio_rx_buffer));
    memset(sio_tx_buffer, 0, sizeof(sio_tx_buffer));
}

/*******************************************************************************
 * Wrapper for ClearRxBuffer function.
 *******************************************************************************/
void SIO_ClearRxBuffer(void)
{
#if (CY_PSOC3 || CY_PSOC5)
    SIOU_ClearRxBuffer();    
#else    
    SIOU_SpiUartClearRxBuffer();
#endif    
}

/*******************************************************************************
 * Wrapper for GetRxBufferSize function.
 *******************************************************************************/
uint8 SIO_GetRxBufferSize(void)
{
#if (CY_PSOC3 || CY_PSOC5)
   return SIOU_GetRxBufferSize();   
#else    
   return SIOU_SpiUartGetRxBufferSize();
#endif       
}

/*******************************************************************************
 * This routine is used to dump the hex values of the specified buffer during
 * debug sessions.
 *******************************************************************************/
void SIO_HexDump(uint8* buffer, size_t length)
{
	size_t i;

	if ( (buffer != NULL) && (length > 0) )
	{
		SIO_SendReturn();

		for ( i=0; i < length; i++ )
		{
			memset(sio_tx_buffer, 0, sizeof(sio_tx_buffer));       
			sprintf(sio_tx_buffer, "%02x, ", buffer[i]);
            UART_PUT_STRING(sio_tx_buffer);                    
		}

		SIO_SendReturn(); 
	}
}

/*******************************************************************************
 * 
 *******************************************************************************/
int SIO_Printf(const char* format, ...)
{
	int ret;
	int offset;
	va_list args;

	va_start(args, format);
	memset(sio_tx_buffer, 0, sizeof(sio_tx_buffer));
	sio_tx_buffer[0] = '>';
    sio_tx_buffer[1] = ' ';
	ret = vsprintf((char*)&sio_tx_buffer[2], format, args);
	va_end(args);
	offset = strlen(sio_tx_buffer);
	sio_tx_buffer[offset++] = LF;
	sio_tx_buffer[offset++] = CR;
	//sio_tx_buffer[offset++] = '$';
	//while ( USB2UART_CDCIsReady() == 0u );
    UART_PUT_STRING(sio_tx_buffer); 
    CyDelay(100);
	return(ret);
}

/*******************************************************************************
 * Wrapper for ReadRxData function.
 *******************************************************************************/
uint8 SIO_ReadRxData(void)
{
#if (CY_PSOC3 || CY_PSOC5)
   return SIOU_ReadRxData();
#else    
   return SIOU_SpiUartReadRxData();
#endif      
}

/*******************************************************************************
 * Transmits end-of-line character(s).
 *******************************************************************************/
void SIO_SendReturn(void)
{
	memset(sio_tx_buffer, 0, sizeof(sio_tx_buffer));
	sio_tx_buffer[0] = CR;
    sio_tx_buffer[1] = LF;
    UART_PUT_STRING(sio_tx_buffer);  
}

/*******************************************************************************
 * Used to initialize the SIO subsystem
 *******************************************************************************/
void SIO_Start(void)
{
    SIO_Clear();
    SIOU_Start();
    CyDelay(10u);
    UART_PUT_STRING("SIO Started\r\n");
    CyDelay(500u);
}

/* [] END OF FILE */
