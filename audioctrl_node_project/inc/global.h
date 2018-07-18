#ifndef _GLOBAL_H_
#define _GLOBAL_H_
/*******************************************************************************
* File: global.h
*
* Version: 1.0
*
* Copyright 2016, Bossa Nova Robotics. All rights reserved.
* This software is owned by Bossa Nova Robotics and is protected by and subject
* to worldwide patent and copyright laws and treaties.
********************************************************************************
*
* Description:
*     This file contains variable considered relevant to ALL subsystems.
*
*******************************************************************************/  
#include "target.h"
    
#define ADC_MUX_SIZE 9    
GLOBAL uint16 adc_data[ADC_MUX_SIZE];
GLOBAL uint8  adc_channel;

#endif

//[] END OF FILE
