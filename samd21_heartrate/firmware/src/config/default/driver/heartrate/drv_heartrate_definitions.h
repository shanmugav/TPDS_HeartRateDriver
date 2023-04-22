/*******************************************************************************
  HEARTRATE Driver Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_heartrate_definitions.h

  Summary:
    HEARTRATE Driver Definitions Header File

  Description:
    This file provides implementation-specific definitions for the HEARTRATE
    driver's system interface.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef DRV_HEARTRATE_DEFINITIONS_H
#define DRV_HEARTRATE_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "system/int/sys_int.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************


// *****************************************************************************
/* HEARTRATE PLIB API Set needed by the driver */

typedef enum
{
    /* Threshold number of bytes are available in the receive ring buffer */
    HEARTRATE_EVENT_READ_THRESHOLD_REACHED = 0,

    /* ReceSERCOM_ive ring buffer is full. Application must read the data out to avoid missing data on the next RX interrupt. */
    HEARTRATE_EVENT_READ_BUFFER_FULL,

    /* HEARTRATE error. Application must call the SERCOMx_HEARTRATE_ErrorGet API to get the type of error and clear the error. */
    HEARTRATE_EVENT_READ_ERROR,

    /* Threshold number of free space is available in the transmit ring buffer */
    HEARTRATE_EVENT_WRITE_THRESHOLD_REACHED,

    /* Recevie break signal is detected */
    HEARTRATE_EVENT_BREAK_SIGNAL_DETECTED,
}HEARTRATE_EVENT;

typedef void (* DRV_HEARTRATE_PLIB_CALLBACK)( uintptr_t context);
typedef bool(*DRV_HEARTRATE_PLIB_READ)(void *buffer, const size_t size);
typedef size_t(*DRV_HEARTRATE_PLIB_READ_COUNT_GET)(void);
typedef void (* DRV_HEARTRATE_PLIB_READ_CALLBACK_REG)( int event, uintptr_t context);
typedef bool(*DRV_HEARTRATE_PLIB_SET_READ_THRESHOLD_SET)(uint32_t nBytesThreshold);
typedef bool(*DRV_HEARTRATE_PLIB_READ_NOTIFICATION_ENABLE)(bool isEnabled, bool isPersistent);

    

typedef struct
{
    DRV_HEARTRATE_PLIB_READ read;
    DRV_HEARTRATE_PLIB_READ_COUNT_GET readCountGet;
    DRV_HEARTRATE_PLIB_READ_CALLBACK_REG readCallbackRegister;
    DRV_HEARTRATE_PLIB_SET_READ_THRESHOLD_SET setReadThreshold;
    DRV_HEARTRATE_PLIB_READ_NOTIFICATION_ENABLE readNotificationEnable;
} DRV_HEARTRATE_PLIB_INTERFACE;





// *****************************************************************************
/* HEARTRATE Driver Initialization Data Declaration */

struct _DRV_HEARTRATE_INIT
{
    /* Identifies the PLIB API set to be used by the driver to access the
     * peripheral. */
    const DRV_HEARTRATE_PLIB_INTERFACE*         heartratePlib;

    /* Number of clients */
    uint32_t                                numClients;

    /* Memory Pool for Client Objects */
    uintptr_t                               clientObjPool;


};

//DOM-IGNORE-BEGIN
#ifdef __cplusplus

    }

#endif
//DOM-IGNORE-END

#endif // #ifndef DRV_HEARTRATE_DEFINITIONS_H
