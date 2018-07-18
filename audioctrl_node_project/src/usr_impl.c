/*******************************************************************************
* FILE: usr_impl.c
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
*     This module implements the USR CAN slave routines
*
*******************************************************************************/

#include "cytypes.h"
#include "string.h"

#include "i2c_psoc.h"
#include "node.h"
#include "slave_framework.h"
#include "usr_impl.h"
#include <project.h>

// ****** Globals ********

static bool reset = false;

// **** timers ****
static uint16_t timer = 0;

static uint32_t mod_index = 0;
static uint32_t mod_timer = 0;
static uint32_t update_timer = 0;

static volatile uint8_t b_DisplayIsBusy = 0;
static volatile uint8_t b_mod_timer_update = 0;

static uint8_t b_ACN_Init = 1;
uint32 I2CA = 0x20;
uint8 I2CData[1];

/*************************************************************************
**
** Function    : USR_Main
**
** Description : Main loop usr function
**
** Parameters  : -
**
** Returnvalue : -
**
*************************************************************************/
void USR_SPKR_Enable(void)
{
    I2CData[0] = 0xFF;
    //I2CData[1] = 0xFF;
   // I2CData_on[2] = 0xFF;
    I2C_Write(I2CA,0xFF,I2CData,1);
}
void USR_SPKR_Disable(void)
{
    I2CData[0] = 0x00;
  //  I2CData[1] = 0x00;
  //  I2CData_on[2] = 0x00;
    I2C_Write(I2CA,0,I2CData,1);
}
void USR_Main(void)
{
    bool tx_pend;
    COP_CheckTransmissionInProgress(&tx_pend);
    if (reset && !tx_pend)
    {
        Bootloadable_Load();
    }
    if (b_ACN_Init)
    {
        USR_SPKR_Enable();
        Str_Mon_Write(1);
        Amp_Shtdn_Write(1);
        b_ACN_Init = 0;
    }
}

/*************************************************************************
**
** Function    : USR_Tick
**
** Description : User level system callback for millisecond timer
**
** Parameters  : None
**
** Returnvalue : -
**
*************************************************************************/

void USR_Tick(void)
{ 
  timer++;
  if (timer == 500)
  {
    LED_SYS_Write(~LED_SYS_Read());
    timer = 0;
  }

  mod_timer++;
  if( mod_timer >= MOD_FREQ )
  {
    if( !b_DisplayIsBusy )
    {
        b_mod_timer_update = 1;
    }
    mod_timer = 0;
    mod_index++;
    if( mod_index == MOD_PERIOD )
    {
        mod_index = 0;
    }
  }
   
  update_timer++;
  if( update_timer >= 80 )
  {
    update_timer = 0;
    //USR_UpdateDisplay();
  }
   
}

bool USR_StartBootloader(void)
{
    reset = true;
    
    return true;
}