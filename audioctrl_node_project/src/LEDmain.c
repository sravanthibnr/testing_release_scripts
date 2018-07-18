/***************************************************************************************************
**    Copyright (C) 1996-2015 HMS Technology Center Ravensburg GmbH, all rights reserved
****************************************************************************************************
**
**       File: LEDmain.c
**    Summary: Main module in LED unit. Contains the implementation
**             of status LEDs (Light Emitting Diode) on CANopen devices.
**             CiA 303-3, Part 3 Indicator specification.
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

/*************************************************************************
**    compiler-instructions
*************************************************************************/

/*************************************************************************
**    include-files
*************************************************************************/
#include "Target.h"
#include "Integral.h"
#include "COPcfg.h"
#include "DLL.h"
#include "COP.h"
#include "USR.h"
#include "LED.h"

#if ((LED_ERROR_AND_RUN_LEDS) || (LED_STATUS_LED))


#if ((LED_ERROR_AND_RUN_LEDS) && (LED_STATUS_LED))
  /* either there is a single color red and a single color green LED
  ** (ERROR LED and RUN LED) or a bicolor LED (STATUS LED) */
  #error mismatch in LED defines
#endif

/*************************************************************************
**    global constants, types, functions, macros
*************************************************************************/

/* macros for accessing the global time reference */
#define LED_GetTime()        COP_Timer
#define LED_TIMER_DATA_TYPE  COP_t_TIMER


/*************************************************************************
**    static constants, types, functions, macros
*************************************************************************/

/* bit positions in LED_b_Clocks */
#define LED_REDFLICK    0x01
#define LED_GRNFLICK    0x02
#define LED_GRNBLINK    0x04
#define LED_REDBLINK    0x08

/* the bits of these byte contain the "flicker" and "blink" clocks */
static UINT8 LED_b_Clocks;

/* needed to synchronize the blink cycles to the global timer */
static LED_TIMER_DATA_TYPE LED_i_NextBlink;

/* blink periods */
static UINT8 LED_b_Cnt50ms;
static UINT8 LED_b_Cnt200ms;

/* action flag indicating that an LED might have changed */
static BOOLEAN LED_o_ActionRed;
static BOOLEAN LED_o_ActionGrn;

/* actual LED condition (on or off)*/
static BOOLEAN LED_o_GrnOn;
static BOOLEAN LED_o_RedOn;

/* states of the LEDs (on, off, flickering, single Flash,...*/
static UINT8 LED_b_RedState;
static UINT8 LED_b_GrnState;

/* Flag if the LEDs are initialized */
static BOOLEAN LED_o_Initialized=FALSE;

/*
** blink patterns:
** Each of the following arrays has 120 bits. Each bit equals 200ms (so
** 120 bit equals 24000ms). 120 is the smallest multiple of all the blink
** patterns (periods are 1200ms, 1600ms and 2000ms).
*/
static const UINT8 LED_kba_GrnSngFls[15] = {0x82,0x20,0x08,
                                            0x82,0x20,0x08,
                                            0x82,0x20,0x08,
                                            0x82,0x20,0x08,
                                            0x82,0x20,0x08};

static const UINT8 LED_kba_RedSngFls[15] = {0x41,0x10,0x04,
                                            0x41,0x10,0x04,
                                            0x41,0x10,0x04,
                                            0x41,0x10,0x04,
                                            0x41,0x10,0x04};

static const UINT8 LED_kba_RedDblFls[15] = {0x05,
                                            0x05,
                                            0x05,
                                            0x05,
                                            0x05,
                                            0x05,
                                            0x05,
                                            0x05,
                                            0x05,
                                            0x05,
                                            0x05,
                                            0x05,
                                            0x05,
                                            0x05,
                                            0x05};

static const UINT8 LED_kba_GrnTrpFls[15] = {0x2A,0xA8,0xA0,0x82,0x0A,
                                            0x2A,0xA8,0xA0,0x82,0x0A,
                                            0x2A,0xA8,0xA0,0x82,0x0A};

static const UINT8 LED_kba_RedTrpFls[15] = {0x15,0x54,0x50,0x41,0x05,
                                            0x15,0x54,0x50,0x41,0x05,
                                            0x15,0x54,0x50,0x41,0x05};

static const UINT8 LED_kba_RedQudFls[15] = {0x55,0x50,0x05,
                                            0x55,0x50,0x05,
                                            0x55,0x50,0x05,
                                            0x55,0x50,0x05,
                                            0x55,0x50,0x05};

/* auxilary constant needed to mask out one bit of the patterns */
static const UINT8 LED_kba_BitMask[8] =
                            {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

/*************************************************************************
**    global functions
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
void LED_Init(void)
{
  if(LED_o_Initialized==FALSE)
  {
    LED_o_Initialized=TRUE;


    COP_DISABLE_TIMER_INT;
    LED_i_NextBlink = LED_GetTime();
    COP_ENABLE_TIMER_INT;

    LED_b_Clocks    = LED_REDFLICK | LED_REDBLINK;
    LED_b_Cnt50ms   = 0;
    LED_b_Cnt200ms  = 0;
    LED_b_RedState  = 0;
    LED_b_GrnState  = 0;
    LED_o_ActionRed = TRUE;
    LED_o_ActionGrn = TRUE;
    LED_o_GrnOn     = FALSE;
    LED_o_RedOn     = FALSE;
  }
}


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
void LED_Handler(void)
{
  LED_TIMER_DATA_TYPE now;
  /* index for accessing the pattern arrays */
  static UINT8 bitPos  = 0;
  static UINT8 bytePos = 0;

  BOOLEAN o_grn_on = LED_o_GrnOn; /* store previous state */
  BOOLEAN o_red_on = LED_o_RedOn; /* store previous state */

  /* if the LEDs are not initialized return */
  if(LED_o_Initialized==FALSE)
  {
    return;
  }

  /* Get current system time */
  COP_DISABLE_TIMER_INT;
  now = LED_GetTime();
  COP_ENABLE_TIMER_INT;

  /* synchronize all the following to a 50ms cycle */
  COP_DISABLE_TIMER_INT;
  if (COP_TimeOver(LED_i_NextBlink, now))
  {
    COP_ENABLE_TIMER_INT;

    /* every 50 ms */
    LED_i_NextBlink += 50;

    /* flicker clock for red and green */
    LED_b_Clocks ^= LED_REDFLICK;
    LED_b_Clocks ^= LED_GRNFLICK;

    if (++LED_b_Cnt50ms == 4)
    {
      /* every 200 ms */
      LED_b_Cnt50ms = 0;

      /* reset counter after number of bits in the pattern array is reached */
      if (++LED_b_Cnt200ms == (sizeof(LED_kba_RedTrpFls)*8))
      {
        LED_b_Cnt200ms = 0;
      }

      /* calculate index for accessing the pattern arrays */
      bitPos  = LED_b_Cnt200ms & 0x07;
      bytePos = LED_b_Cnt200ms >> 3;

      /* provide the clock for the state "Blinking" */
      LED_b_Clocks ^= LED_REDBLINK;
      LED_b_Clocks ^= LED_GRNBLINK;
    }

    /* the following needs only to be executed if there is a chance that
       something changed */
    if (LED_o_ActionRed)
    {
      LED_o_RedOn = FALSE;

      /* check the state of the red LED */
      /* the order of the statements represent their priority */
      if (LED_b_RedState & LED_k_ON)
      {
        /* Priority 1, Bus off */
        LED_o_RedOn = TRUE;
        LED_o_ActionRed = FALSE;
      }
      else if (LED_b_RedState & LED_k_FLICKERING)
      {
        /* Priority 2, State AutoBitrate/LSS */
        if(LED_b_Clocks & LED_REDFLICK)
        {
          LED_o_RedOn =  TRUE;
        }
      }
      else if (LED_b_RedState & LED_k_BLINKING)
      {
        /* Priority 3, invalid configuration / non-configured slave */
        if(LED_b_Clocks & LED_REDBLINK)
        {
          LED_o_RedOn = TRUE;
        }
      }
      else if (LED_b_RedState & LED_k_SINGLE_FLASH)
      {
        /* Priority 4, Warning limit reached */
        if(LED_kba_RedSngFls[bytePos] & LED_kba_BitMask[bitPos])
        {
          LED_o_RedOn = TRUE;
        }
      }
      else if (LED_b_RedState & LED_k_DOUBLE_FLASH)
      {
        /* Priority 5, Error control event */
        if(LED_kba_RedDblFls[bytePos] & LED_kba_BitMask[bitPos])
        {
          LED_o_RedOn = TRUE;
        }
      }
      else if (LED_b_RedState & LED_k_TRIPLE_FLASH)
      {
        /* Priority 6, Sync error */
        if(LED_kba_RedTrpFls[bytePos] & LED_kba_BitMask[bitPos])
        {
          LED_o_RedOn = TRUE;
        }
      }
      else if (LED_b_RedState & LED_k_QUADRUPL_FLASH)
      {
        /* Priority 7, Event-timer error */
        if(LED_kba_RedQudFls[bytePos] & LED_kba_BitMask[bitPos])
        {
          LED_o_RedOn = TRUE;
        }
      }
      else
      {
        /* Priority 8, No error */
        LED_o_ActionRed = FALSE;   /* Led off */
      }



#if LED_STATUS_LED
      LED_o_ActionGrn = TRUE;
#endif
    }

    if (LED_o_ActionGrn)
    {
      LED_o_GrnOn = FALSE;

      /* check the states of the green LED */
      /* the order of the statements represent their priority */
      if (LED_b_GrnState & LED_k_FLICKERING)
      {
        /* Priority 1, State AutoBitrate/LSS */
        if(LED_b_Clocks & LED_GRNFLICK)
        {
          LED_o_GrnOn = TRUE;
        }
      }
      else if (LED_b_GrnState & LED_k_SINGLE_FLASH)
      {
        /* Priority 2, State STOPPED */
        if(LED_kba_GrnSngFls[bytePos] & LED_kba_BitMask[bitPos])
        {
          LED_o_GrnOn = TRUE;
        }
      }
      else if (LED_b_GrnState & LED_k_DOUBLE_FLASH)
      {
        /* Priority 3, State RESERVED */
      }
      else if (LED_b_GrnState & LED_k_TRIPLE_FLASH)
      {
        /* Priority 4, State Program / Firmware download */
        if(LED_kba_GrnTrpFls[bytePos] & LED_kba_BitMask[bitPos])
        {
          LED_o_GrnOn = TRUE;
        }
      }
      else if (LED_b_GrnState & LED_k_BLINKING)
      {
        /* Priority 5, State PREOPERATIONAL*/
        if(LED_b_Clocks & LED_GRNBLINK)
        {
          LED_o_GrnOn = TRUE;
        }
      }
      else if (LED_b_GrnState & LED_k_ON)
      {
        /* Priority 6, State OPERATIONAL */
        LED_o_GrnOn = TRUE;
        LED_o_ActionGrn = FALSE;
      }
      else
      {
        /* Priority 7, Non-configured slave */
        LED_o_ActionGrn = FALSE;   /* Led off */
      }

#if LED_STATUS_LED
      /* For bicolor LEDs only one color may be active at a time.
         In doubt this is always red */
      if (LED_o_RedOn && LED_o_GrnOn)
      {
        LED_o_GrnOn = FALSE;
      }
#endif

    }
  }
  else
  {
    COP_ENABLE_TIMER_INT;
  }
}


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
void LED_Switch(UINT8 led, UINT8 setState, UINT8 resetState)
{
  /* we trust that this function is always called with valid states/leds */

  if (led & LED_k_RED)
  {
    LED_b_RedState |= setState;
    LED_b_RedState &= ~resetState;

    /* indicate that the LED has changed */
    LED_o_ActionRed = TRUE;
  }

  if (led & LED_k_GRN)
  {
    LED_b_GrnState |= setState;
    LED_b_GrnState &= ~resetState;

    /* indicate that the LED has changed */
    LED_o_ActionGrn = TRUE;
  }

}



/*************************************************************************
**    static functions
*************************************************************************/

#endif   /* ((LED_ERROR_AND_RUN_LEDS) || (LED_STATUS_LED)) */

/*** End Of File ***/
