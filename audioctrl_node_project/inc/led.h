/***************************************************************************************************
**    Copyright (C) 1996-2015 HMS Technology Center Ravensburg GmbH, all rights reserved
****************************************************************************************************
**
**       File: LED.h
**    Summary: Interface to the LED module
**  $Revision: 4137 $
**      $Date: 2015-10-26 13:12:49 +0100 (Mo, 26 Okt 2015) $
**
**************************************************************************
**************************************************************************
**
**  Functions: LED_Init
**             LED_Handler
**             LED_Switch
**
**
**************************************************************************
**    all rights reserved
*************************************************************************/

#ifndef _LED_H
#define _LED_H

/*************************************************************************
**    constants and macros
*************************************************************************/

/* identifiers for the LEDs */
#define LED_k_RED  0x01
#define LED_k_GRN  0x02

/* states of the LEDs */
#define LED_k_OFF             0x00
#define LED_k_NOCHANGE        0x00
#define LED_k_ON              0x01
#define LED_k_FLICKERING      0x02
#define LED_k_BLINKING        0x04
#define LED_k_SINGLE_FLASH    0x08
#define LED_k_DOUBLE_FLASH    0x10
#define LED_k_TRIPLE_FLASH    0x20
#define LED_k_QUADRUPL_FLASH  0x40



/* for better readability inside the CANopen code these defines: */
#define LED_ARGS_BUSOFF           LED_k_RED, LED_k_ON           , LED_k_NOCHANGE
#define LED_ARGS_NOTBUSOFF        LED_k_RED, LED_k_NOCHANGE     , LED_k_ON
#define LED_ARGS_ESTATSET         LED_k_RED, LED_k_SINGLE_FLASH , LED_k_NOCHANGE
#define LED_ARGS_ESTATRESET       LED_k_RED, LED_k_NOCHANGE     , LED_k_SINGLE_FLASH
#define LED_ARGS_STARTED          LED_k_GRN, LED_k_ON           , LED_k_SINGLE_FLASH+LED_k_BLINKING
#define LED_ARGS_STOPPED          LED_k_GRN, LED_k_SINGLE_FLASH , LED_k_ON+LED_k_BLINKING
#define LED_ARGS_PREOP            LED_k_GRN, LED_k_BLINKING     , LED_k_ON+LED_k_SINGLE_FLASH
#define LED_ARGS_GUARDAGAIN       LED_k_RED, LED_k_NOCHANGE     , LED_k_DOUBLE_FLASH
#define LED_ARGS_GUARDFAIL        LED_k_RED, LED_k_DOUBLE_FLASH , LED_k_NOCHANGE
#define LED_ARGS_LSS_NON_CFG_SLV  LED_k_RED, LED_k_BLINKING, LED_k_NOCHANGE
#define LED_ARGS_LSS_CONFIG       LED_k_RED|LED_k_GRN, LED_k_FLICKERING,LED_k_NOCHANGE
#define LED_ARGS_LSS_OPERATION    LED_k_RED|LED_k_GRN, LED_k_NOCHANGE,LED_k_FLICKERING
#define LED_ARGS_CLRALL           LED_k_RED+LED_k_GRN, LED_k_NOCHANGE, (LED_k_ON  | \
                                                            LED_k_FLICKERING      | \
                                                            LED_k_BLINKING        | \
                                                            LED_k_SINGLE_FLASH    | \
                                                            LED_k_DOUBLE_FLASH    | \
                                                            LED_k_TRIPLE_FLASH    | \
                                                            LED_k_QUADRUPL_FLASH)


/*************************************************************************
**    datatypes
*************************************************************************/


/*************************************************************************
**    global variables
*************************************************************************/

/*************************************************************************
**    functionprototypes
*************************************************************************/

/*************************************************************************
**
** Function    : LED_Init
**
** Description : Initialize the LED module
**
** Parameters  : -
**
** Returnvalue : -
**
*************************************************************************/
extern void LED_Init(void);

/*************************************************************************
**
** Function    : LED_Handler
**
** Description : This is called cyclically and performs the updating
**               of the LEDs depending on the current LED states.
**
** Parameters  : -
**
** Returnvalue : -
**
*************************************************************************/
extern void LED_Handler(void);

/*************************************************************************
**
** Function    : LED_Switch
**
** Description : This is called from the stack to change a LED
**
** Parameters  : led             (IN) - Select LED, one of
**                                      - LED_k_RED
**                                      - LED_k_GRN
**                                      - LED_k_RED+LED_k_GRN
**               setState        (IN) - state bits that should be set to on
**               resetState      (IN) - state bits that should be set to off
**                                      - LED_k_NOCHANGE
**                                      - LED_k_ON
**                                      - LED_k_FLICKERING
**                                      - LED_k_BLINKING
**                                      - LED_k_SINGLE_FLASH
**                                      - LED_k_DOUBLE_FLASH
**                                      - LED_k_TRIPLE_FLASH
**                                      - LED_k_QUADRUPL_FLASH
**
** Returnvalue : -
**
*************************************************************************/
extern void LED_Switch(UINT8 led, UINT8 setState, UINT8 resetState);



#endif
