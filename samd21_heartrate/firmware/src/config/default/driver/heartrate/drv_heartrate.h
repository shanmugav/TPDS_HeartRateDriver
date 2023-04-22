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
    UART modules on Microchip PIC32 microcontrollers.  This file provides the
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
/* HEARTRATE Driver Buffer Handle

  Summary:
    Handle identifying a read or write buffer passed to the driver.

  Description:


  Remarks:
    None
*/

typedef uintptr_t DRV_HEARTRATE_BUFFER_HANDLE;

// *****************************************************************************
/* HEARTRATE Driver Invalid Buffer Handle

  Summary:
    Definition of an invalid buffer handle.

  Description:
    This is the definition of an invalid buffer handle. An invalid buffer handle
    is returned by DRV_HEARTRATE_ReadBufferAdd and DRV_HEARTRATE_WriteBufferAdd
    functions if the buffer add request was not successful.

  Remarks:
    None
*/

#define DRV_HEARTRATE_BUFFER_HANDLE_INVALID  ((DRV_HEARTRATE_BUFFER_HANDLE)(-1))

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
    /* The buffer is pending to be serviced */
    DRV_HEARTRATE_BUFFER_EVENT_PENDING = 0,

    /* All data from or to the buffer was transferred successfully. */
    DRV_HEARTRATE_BUFFER_EVENT_COMPLETE = 1,

    /* Transfer Handle given is expired. It means transfer
    is completed but with or without error is not known. */
    DRV_HEARTRATE_BUFFER_EVENT_HANDLE_EXPIRED = 2,

    /* There was an error while processing the buffer transfer request. */
    DRV_HEARTRATE_BUFFER_EVENT_ERROR = -1,

    /* Transfer Handle given is invalid */
    DRV_HEARTRATE_BUFFER_EVENT_HANDLE_INVALID = -2

} DRV_HEARTRATE_BUFFER_EVENT;

// *****************************************************************************
/* HEARTRATE Driver Errors

  Summary:
    Defines the different types of errors for HEARTRATE driver

  Description:
    This data type defines the different types of errors for HEARTRATE driver.
    DRV_HEARTRATE_ERROR_NONE : No errors
    DRV_HEARTRATE_ERROR_OVERRUN : Receive Overflow error
    DRV_HEARTRATE_ERROR_PARITY : Parity error
    DRV_HEARTRATE_ERROR_FRAMING : Framing error

  Remarks:
    This structure is implementation specific. It is fully defined in
    drv_heartrate_definitions.h.
*/

typedef enum _DRV_HEARTRATE_ERROR DRV_HEARTRATE_ERROR;

// *****************************************************************************
/* HEARTRATE Driver Serial Setup Data

  Summary:
    Defines the data required to dynamically set the serial settings.

  Description:
    This data type defines the data required to dynamically set the serial
    settings for the specific HEARTRATE driver instance.

  Remarks:
    This structure is implementation specific. It is fully defined in
    drv_heartrate_definitions.h.
*/

typedef struct _DRV_HEARTRATE_SERIAL_SETUP DRV_HEARTRATE_SERIAL_SETUP;

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

// *****************************************************************************
/* HEARTRATE Driver Buffer Event Handler Function Pointer

   Summary
    Pointer to a HEARTRATE Driver Buffer Event handler function

   Description
    This data type defines the required function signature for the HEARTRATE driver
    buffer event handling callback function. A client must register a pointer
    using the buffer event handling function whose function signature (parameter
    and return value types) match the types specified by this function pointer
    in order to receive buffer related event calls back from the driver.

    The parameters and return values are described here and
    a partial example implementation is provided.

  Parameters:
    event - Identifies the type of event

    bufferHandle - Handle identifying the buffer to which the vent relates

    context - Value identifying the context of the application that
    registered the event handling function.

  Returns:
    None.

  Example:
    <code>
    void APP_HEARTRATEBufferEventHandler(
        DRV_HEARTRATE_BUFFER_EVENT event,
        DRV_HEARTRATE_BUFFER_HANDLE handle,
        uintptr_t context
    )
    {
        // The context handle was set to an application specific
        // object. It is now retrievable easily in the event handler.
        MY_APP_OBJ* myAppObj = (MY_APP_OBJ *) context;

        switch(event)
        {
            case DRV_HEARTRATE_BUFFER_EVENT_COMPLETE:
            {
                // This means the data was transferred.
                break;
            }

            case DRV_HEARTRATE_BUFFER_EVENT_ERROR:
            {
                // Error handling here.
                break;
            }

            default:
            {
                break;
            }
        }
    }
    </code>

  Remarks:
    If the event is DRV_HEARTRATE_BUFFER_EVENT_COMPLETE, it means that the data was
    transferred successfully.

    If the event is DRV_HEARTRATE_BUFFER_EVENT_ERROR, it means that the data was not
    transferred successfully. The DRV_HEARTRATE_BufferCompletedBytesGet function can
    be called to find out how many bytes were processed.

    The bufferHandle parameter contains the buffer handle of the buffer that
    associated with the event. And bufferHandle will be valid while the buffer
    request is in the queue and during callback, unless an error occurred.
    After callback returns, the driver will retire the buffer handle.

    The context parameter contains the a handle to the client context,
    provided at the time the event handling function was registered using the
    DRV_HEARTRATE_BufferEventHandlerSet function.  This context handle value is
    passed back to the client as the "context" parameter.  It can be any value
    necessary to identify the client context or instance (such as a pointer to
    the client's data) instance of the client that made the buffer add request.

    The event handler function executes in the peripheral's interrupt
    context. It is recommended of the application to not perform process
    intensive or blocking operations within this function.

    The DRV_HEARTRATE_ReadBufferAdd and DRV_HEARTRATE_WriteBufferAdd functions can
    be called in the event handler to add a buffer to the driver queue. These
    functions can only be called to add buffers to the driver whose event
    handler is running. For example, HEARTRATE2 driver buffers cannot be added in
    HEARTRATE1 driver event handler.
*/

typedef void (*DRV_HEARTRATE_BUFFER_EVENT_HANDLER )( DRV_HEARTRATE_BUFFER_EVENT event, DRV_HEARTRATE_BUFFER_HANDLE bufferHandle, uintptr_t context );

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

bool DRV_HEARTRATE_Read(DRV_HANDLE handle);
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

typedef void (*DRV_HEARTRATE_APP_CALLBACK )(DRV_HANDLE handle, uintptr_t context );



#include "driver/heartrate/src/drv_heartrate_local.h"

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // #ifndef DRV_HEARTRATE_H