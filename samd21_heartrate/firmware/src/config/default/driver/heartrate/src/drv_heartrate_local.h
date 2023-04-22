/*******************************************************************************
  HEARTRATE Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_heartrate_local.h

  Summary:
    HEARTRATE Driver Local Data Structures

  Description:
    Driver Local Data Structures
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

#ifndef DRV_HEARTRATE_LOCAL_H
#define DRV_HEARTRATE_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "driver/heartrate/drv_heartrate.h"
#include "osal/osal.h"

#define DRV_RECEIVE_HEARTRATE_DATA_SIZE     (128)
#define DRV_HEARTRATE_RECEIVE_BUFFER_SIZE  (DRV_RECEIVE_HEARTRATE_DATA_SIZE * 10)
// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* HEARTRATE Driver client Handle Macros

  Summary:
    HEARTRATE driver client Handle Macros

  Description:
    Client handle related utility macros. HEARTRATE client handle is a combination
    of client index (8-bit), instance index (8-bit) and token (16-bit). The token
    is incremented for every new driver open request.

  Remarks:
    None
*/

#define DRV_HEARTRATE_CLIENT_INDEX_MASK               (0x000000FF)

#define DRV_HEARTRATE_INSTANCE_INDEX_MASK             (0x0000FF00)

#define DRV_HEARTRATE_TOKEN_MASK                      (0xFFFF0000)

#define DRV_HEARTRATE_TOKEN_MAX                       (DRV_HEARTRATE_TOKEN_MASK >> 16)


// *****************************************************************************
/* HEARTRATE Driver Buffer Events

  Summary
    Identifies the possible events that can result from a buffer add request.

  Description
    This enumeration identifies the possible events that can result from a
    buffer add request caused by the client calling either the
    DRV_HEARTRATE_ReadBufferAdd or DRV_HEARTRATE_WriteBufferAdd functions.

  Remarks:
    One of these values is passed in the "event" parameter of the event
    handling callback function that the client registered with the driver by
    calling the DRV_HEARTRATE_BufferEventHandlerSet function when a buffer
    transfer request is completed.
*/

typedef enum
{
    /* All data from or to the buffer was transferred successfully. */
    DRV_HEARTRATE_REQUEST_STATUS_COMPLETE,

    /* There was an error while processing the buffer transfer request. */
    DRV_HEARTRATE_REQUEST_STATUS_ERROR,

} DRV_HEARTRATE_REQUEST_STATUS;
typedef enum
{
    HEADER_BYTE1 = 0,
    HEADER_BYTE2,
    HEADER_BYTE3,
    HEART_RATE_BYTE_POS_FIND,
    HEART_RATE_READ,
}HEART_RATE_DATA_BUF_PROCESS_STATE;
// *****************************************************************************
/* HEARTRATE Driver Instance Object

  Summary:
    Object used to keep any data required for an instance of the HEARTRATE driver.

  Description:
    None.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag to indicate this object is in use  */
    bool inUse;

    /* Flag to indicate that driver has been opened Exclusively */
    bool isExclusive;

    /* Keep track of the number of clients
     * that have opened this driver
     */
    size_t nClients;

    /* Maximum number of clients */
    size_t nClientsMax;

    /* The status of the driver */
    SYS_STATUS status;

    /* PLIB API list that will be used by the driver to access the hardware */
    const DRV_HEARTRATE_PLIB_INTERFACE* heartratePlib;

    /* Memory pool for Client Objects */
    uintptr_t clientObjPool;

    /* This is an instance specific token counter used to generate unique
     * client handles
     */
    uint16_t heartrateTokenCount;

    /* Active receive client allows reporting errors directly to the client */
    uintptr_t currentClient;

    /* Indicates transmit requests status */
    volatile DRV_HEARTRATE_REQUEST_STATUS txRequestStatus;

    /* Indicates receive requests status */
    volatile DRV_HEARTRATE_REQUEST_STATUS rxRequestStatus;

    /* Transmit mutex */
    OSAL_MUTEX_DECLARE(clientMutex);
   
    int heartRate;
    uint8_t rx_buff[DRV_RECEIVE_HEARTRATE_DATA_SIZE];
    uint8_t rx_data[DRV_HEARTRATE_RECEIVE_BUFFER_SIZE];
     uint32_t no_of_bytes_rcvd;
     bool hr9_data_rx_flag;
     //HEART_RATE_DATA_BUF_PROCESS_STATE    heartrate9_read_byte_state = HEADER_BYTE1;
    HEART_RATE_DATA_BUF_PROCESS_STATE    heartrate9_read_byte_state;
    volatile uint16_t rx_buff_in_index;
    uint16_t rx_buff_out_index;
     bool hr9_click_intrfce_initialized;
     bool hr9_rx_data_buf_clr_state;
     uint16_t heartrate_pross_cnt;
    uint8_t current_heartrate;
    uint8_t semicoln_cnt;

} DRV_HEARTRATE_OBJ;

typedef struct
{
    /* The hardware instance object associated with the client */
    DRV_HEARTRATE_OBJ                   *hDriver;

    /* The IO intent with which the client was opened */
    DRV_IO_INTENT                   ioIntent;

    /* Client handle that was assigned to this client object when it was
     * opened by the user.
     */
    DRV_HANDLE                      clientHandle;

    /* This flags indicates if the object is in use or is
     * available
     */
    bool                            inUse;
    
    DRV_HEARTRATE_APP_CALLBACK     callback;

} DRV_HEARTRATE_CLIENT_OBJ;

#endif //#ifndef DRV_HEARTRATE_LOCAL_H
