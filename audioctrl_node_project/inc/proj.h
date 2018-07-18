
 /******************************************************************************
 * File 			        : project.h
 * Component          : free_rtos_example_stm32f417
 * Repository         : fw_sandbox
 * Last Updated       : 6/9/2016
 * Author							: Brian Eccles <brian.e@bnrobotics.com>
 * Description        : Standard project header file included by all components
 *                      when USE_PROJECT_HEADER is defined. This file can be 
 *                      used to configure components. See each component's 
 *                      readme.md file for configuration options.
 *
 * PROPRIETARY NOTICE:
 * The information contained herein is confidential, is submitted in confidence,
 * and is proprietary information of Bossa Nova Robotics and its subsidiaries,
 * and shall only be used in the furtherance of the contract of which this file
 * forms a part, and shall not, without Bossa Nova Robotics prior written 
 * approval, be reproduced, distributed, or in any way used in whole or in part 
 * in connection with services or equipment offered for sale or furnished to 
 * others. The information contained herein may not be disclosed to a third
 * party without consent of Bossa Nova Robotics, and then, only pursuant to a 
 * Bossa Nova Robotics approved non-disclosure agreement. Bossa Nova Robotics 
 * assumes no liability for incidental or consequential damages arising from 
 * the use of the information contained herein, and reserve the right to update,
 * revise, or change any information in this file without notice.
 *
 * COPYRIGHT(c) 2015 Bossa Nova Robotics. All Rights Reserved.
 ******************************************************************************/
 
#ifndef __PROJECT_H__
#define __PROJECT_H__


/******************************************************************************/
/* fw_common/slave_framework                                                  */
/******************************************************************************/
#define SLAVE_FRAMEWORK_USE_SYNC         0
#define SLAVE_FRAMEWORK_USE_SYNC_CB      0
#define SLAVE_FRAMEWORK_USE_SYNC_INT_CB  0   
#define SLAVE_FRAMEWORK_USE_TICK_CB      1   
#define SLAVE_FRAMEWORK_USE_MAIN_CB      1
#define SLAVE_FRAMEWORK_USE_PRINTF       1
#define SLAVE_FRAMEWORK_USE_BOOTLOADER   1
/******************************************************************************/


#endif
