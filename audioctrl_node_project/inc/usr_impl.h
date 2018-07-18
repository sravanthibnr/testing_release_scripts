/*******************************************************************************
* FILE: usr_impl.h
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

typedef enum {
    SUBINDEX_ENABLE     = 0x01,
    SUBINDEX_PWR        = 0x02,
    SUBINDEX_INTENSITY  = 0x03,
    SUBINDEX_MOD        = 0x04,
    SUBINDEX_FCOL       = 0x05,
    SUBINDEX_BCOL       = 0x06,
    SUBINDEX_MASK       = 0x07,
    SUBINDEX_SYNC       = 0x08,
    SUBINDEX_END        = 0x09
} subindex_e ;

typedef enum {
    STRIP_OPTMOD_NONE               = 0,
    STRIP_OPTMOD_BLINK_SLOW         = 1,
    STRIP_OPTMOD_BLINK_QUICK        = 2,
    STRIP_OPTMOD_BLINK_FAST         = 3,
    STRIP_OPTMOD_HEARTBEAT_SLOW     = 4,
    STRIP_OPTMOD_HEARTBEAT_QUICK    = 5,
    STRIP_OPTMOD_HEARTBEAT_FAST     = 6,
    STRIP_OPTMOD_END                = 7,
} STRIP_OPTMOD;

#define MOD_PERIOD                  (1024u)
#define MOD_FREQ                    (10u)

#ifndef SLAVE_FRAMEWORK_USE_MAIN_CB
    #define SLAVE_FRAMEWORK_USE_MAIN_CB         0
#endif

// Function declarations



void USR_Tick(void);
void USR_SPKR_Enable(void);
void USR_SPKR_Disable(void);

