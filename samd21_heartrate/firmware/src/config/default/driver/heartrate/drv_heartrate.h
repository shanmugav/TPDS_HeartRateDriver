/*******************************************************************************
  HEARTRATE Driver Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_heartrate.h

  Summary:
    HEARTRATE Driver Interface Header File

  Description:
    The HEARTRATE device driver provides a simple interface to manage the HEARTRATE or
    on Microchip microcontrollers.  This file provides the
    interface definition for the HEARTRATE driver.
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

#ifndef DRV_HEARTRATE_H
#define DRV_HEARTRATE_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <stddef.h>
#include <stdbool.h>
#include "system/system.h"
#include "driver/driver.h"
#include "drv_heartrate_definitions.h"

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
/* HEARTRATE Driver Initialization Data

  Summary:
    Defines the data required to initialize the HEARTRATE driver

  Description:
    This data type defines the data required to initialize the
    HEARTRATE driver.

  Remarks:
    This structure is implementation specific. It is fully defined in
    drv_heartrate_definitions.h.
*/

typedef struct _DRV_HEARTRATE_INIT DRV_HEARTRATE_INIT;

typedef void (*DRV_HEARTRATE_APP_CALLBACK )(DRV_HANDLE handle, int heartrate );
// *****************************************************************************
// *****************************************************************************
// Section: HEARTRATE Driver System Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_HEARTRATE_Initialize
    (
        const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the HEARTRATE instance for the specified driver index.

  Description:
    This routine initializes the HEARTRATE driver instance for the specified driver
    index, making it ready for clients to open and use it. The initialization
    data is specified by the init parameter. The initialization may fail if the
    number of driver objects allocated are insufficient or if the specified
    driver instance is already initialized. The driver instance index is
    independent of the HEARTRATE module ID. For example, driver instance 0 can be
    assigned to HEARTRATE2.

  Precondition:
    None.

  Parameters:
    index - Identifier for the instance to be initialized

    init - Pointer to the init data structure containing any data necessary to
    initialize the driver.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    // The following code snippet shows an example HEARTRATE driver initialization.

    SYS_MODULE_OBJ              objectHandle;

    const DRV_HEARTRATE_PLIB_INTERFACE drvUsart0PlibAPI = {
        .readCallbackRegister = (DRV_HEARTRATE_PLIB_READ_CALLBACK_REG)HEARTRATE1_ReadCallbackRegister,
        .read = (DRV_HEARTRATE_PLIB_READ)HEARTRATE1_Read,
        .readIsBusy = (DRV_HEARTRATE_PLIB_READ_IS_BUSY)HEARTRATE1_ReadIsBusy,
        .readCountGet = (DRV_HEARTRATE_PLIB_READ_COUNT_GET)HEARTRATE1_ReadCountGet,
        .writeCallbackRegister = (DRV_HEARTRATE_PLIB_WRITE_CALLBACK_REG)HEARTRATE1_WriteCallbackRegister,
        .write = (DRV_HEARTRATE_PLIB_WRITE)HEARTRATE1_Write,
        .writeIsBusy = (DRV_HEARTRATE_PLIB_WRITE_IS_BUSY)HEARTRATE1_WriteIsBusy,
        .writeCountGet = (DRV_HEARTRATE_PLIB_WRITE_COUNT_GET)HEARTRATE1_WriteCountGet,
        .errorGet = (DRV_HEARTRATE_PLIB_ERROR_GET)HEARTRATE1_ErrorGet,
        .serialSetup = (DRV_HEARTRATE_PLIB_SERIAL_SETUP)HEARTRATE1_SerialSetup
    };

    const DRV_HEARTRATE_INIT drvUsart0InitData = {
        .heartratePlib = &drvUsart0PlibAPI,
        .numClients = DRV_HEARTRATE_CLIENTS_NUMBER_IDX0,
        .clientObjPool = (uintptr_t)&drvHEARTRATE0ClientObjPool[0],
        .dmaChannelTransmit = DRV_HEARTRATE_XMIT_DMA_CH_IDX0,
        .heartrateTransmitAddress = (void *)&(HEARTRATE1_REGS->US_THR),
        .dmaChannelReceive = DRV_HEARTRATE_RCV_DMA_CH_IDX0,
        .heartrateReceiveAddress = (void *)&(HEARTRATE1_REGS->US_RHR),
        .bufferObjPoolSize = DRV_HEARTRATE_QUEUE_SIZE_IDX0,
        .bufferObjPool = (uintptr_t)&drvHEARTRATE0BufferObjPool[0],
        .interruptHEARTRATE = HEARTRATE1_IRQn,
        .interruptDMA = XDMAC_IRQn,
        .remapDataWidth = drvUsart0remapDataWidth,
        .remapParity = drvUsart0remapParity,
        .remapStopBits = drvUsart0remapStopBits,
        .remapError = drvUsart0remapError,
    };

    objectHandle = DRV_HEARTRATE_Initialize(DRV_HEARTRATE_INDEX_1, (SYS_MODULE_INIT*)&drvUsart0InitData);

    if (objectHandle == SYS_MODULE_OBJ_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other HEARTRATE routine is called.

    This routine should only be called once during system initialization.
    This routine will NEVER block for hardware access.
*/

SYS_MODULE_OBJ DRV_HEARTRATE_Initialize( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT* const init );

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_HEARTRATE_Status( SYS_MODULE_OBJ object )

  Summary:
    Gets the current status of the HEARTRATE driver module.

  Description:
    This routine provides the current status of the HEARTRATE driver module.

  Precondition:
    Function DRV_HEARTRATE_Initialize should have been called before calling this
    function.

  Parameters:
    object - Driver object handle, returned from the
    DRV_HEARTRATE_Initialize routine

  Returns:
    SYS_STATUS_READY - Initialization have succeeded and the HEARTRATE is ready for
    additional operations

    SYS_STATUS_UNINITIALIZED - Indicates that the driver has not been initialized

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_HEARTRATE_Initialize
    SYS_STATUS          heartrateStatus;

    heartrateStatus = DRV_HEARTRATE _Status(object);
    if (heartrateStatus == SYS_STATUS_READY)
    {
        // This means the driver can be opened using the
        // DRV_HEARTRATE_Open() function.
    }
    </code>

  Remarks:
    A driver can opened only when its status is SYS_STATUS_READY.
*/

SYS_STATUS DRV_HEARTRATE_Status( SYS_MODULE_OBJ object);

int DRV_HEARTRATE_Read(DRV_HANDLE handle);
// *****************************************************************************
// *****************************************************************************
// Section: HEARTRATE Driver Common Client Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_HEARTRATE_Open
    (
        const SYS_MODULE_INDEX index,
        const DRV_IO_INTENT ioIntent
    )

  Summary:
    Opens the specified HEARTRATE driver instance and returns a handle to it.

  Description:
    This routine opens the specified HEARTRATE driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver. The ioIntent
    parameter defines how the client interacts with this driver instance.

    Specifying a DRV_IO_INTENT_EXCLUSIVE will cause the driver to provide
    exclusive access to this client. The driver cannot be opened by any
    other client.

  Precondition:
    Function DRV_HEARTRATE_Initialize must have been called before calling this
    function.

  Parameters:
    index - Identifier for the object instance to be opened

    intent - Zero or more of the values from the enumeration DRV_IO_INTENT
    "ORed" together to indicate the intended use of the driver.
    See function description for details.

  Returns:
    If successful, the routine returns a valid open-instance handle (a number
    identifying both the caller and the module instance).

    If an error occurs, the return value is DRV_HANDLE_INVALID. Error can occur
    - if the number of client objects allocated via DRV_HEARTRATE_CLIENTS_NUMBER is
      insufficient.
    - if the client is trying to open the driver but driver has been opened
      exclusively by another client.
    - if the driver peripheral instance being opened is not initialized or is
      invalid.
    - if the client is trying to open the driver exclusively, but has already
      been opened in a non exclusive mode by another client.
    - if the driver is not ready to be opened, typically when the initialize
      routine has not completed execution.

  Example:
    <code>
    DRV_HANDLE handle;

    handle = DRV_HEARTRATE_Open(DRV_HEARTRATE_INDEX_0, DRV_IO_INTENT_EXCLUSIVE);
    if (handle == DRV_HANDLE_INVALID)
    {
        // Unable to open the driver
        // May be the driver is not initialized or the initialization
        // is not complete.
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_HEARTRATE_Close routine is called.
    This routine will NEVER block waiting for hardware. If the requested intent
    flags are not supported, the routine will return DRV_HANDLE_INVALID. This
    function is thread safe in a RTOS application.
*/

DRV_HANDLE DRV_HEARTRATE_Open( const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent );

// *****************************************************************************
/* Function:
    void DRV_HEARTRATE_Close( DRV_Handle handle )

  Summary:
    Closes an opened-instance of the HEARTRATE driver.

  Description:
    This routine closes an opened-instance of the HEARTRATE driver, invalidating the
    handle. Any buffers in the driver queue that were submitted by this client
    will be removed. A new handle must be obtained by calling DRV_HEARTRATE_Open
    before the caller may use the driver again.

  Precondition:
    DRV_HEARTRATE_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's
    open routine

  Returns:
    None.

  Example:
    <code>
    // 'handle', returned from the DRV_HEARTRATE_Open

    DRV_HEARTRATE_Close(handle);

    </code>

  Remarks:
    None.
*/

void DRV_HEARTRATE_Close( const DRV_HANDLE handle);

void DRV_HEARTRATE_TASKS( SYS_MODULE_OBJ object);

bool DRV_HEARTRATE_GetHeartrate( const DRV_HANDLE handle, int * heartRate);

void DRV_HEARTRATE_TASKS( SYS_MODULE_OBJ object);

void DRV_HEARTRATE_ClientEventHandlerSet(const DRV_HANDLE handle, const DRV_HEARTRATE_APP_CALLBACK callback);





#include "driver/heartrate/src/drv_heartrate_local.h"

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // #ifndef DRV_HEARTRATE_H