/*******************************************************************************
  HEARTRATE Driver Implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_heartrate.c

  Summary:
    Source code for the HEARTRATE driver dynamic implementation.

  Description:
    This file contains the source code for the dynamic implementation of the
    HEARTRATE driver.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "driver/heartrate/drv_heartrate.h"
#include "drv_heartrate_local.h"
#include "peripheral/systick/plib_systick.h"
#include "definitions.h"
#include "string.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************
static void heartrate9_initialize(void);
/* This is the driver instance object array. */
static bool  bInitialized=false;
static int iInitializeDelay = 5000;
static DRV_HEARTRATE_OBJ gDrvHEARTRATEObj[DRV_HEARTRATE_INSTANCES_NUMBER] ;

static void _DRV_HEARTRATE_PLIB_CallbackHandler(int event, uintptr_t context);
static void heartrate9_set_rst(uint8_t state);
static int8_t heartrate9_read_byte(DRV_HEARTRATE_OBJ *dObj);
static void hr9_rx_buff_update(DRV_HEARTRATE_OBJ *dObj);
static bool is_heartrate9_byte_ready(DRV_HEARTRATE_OBJ *dObj);

static inline uint32_t  _DRV_HEARTRATE_MAKE_HANDLE(uint16_t token, uint8_t drvIndex, uint8_t clientIndex)
{
    return ((token << 16) | (drvIndex << 8) | clientIndex);
}

static inline uint16_t _DRV_HEARTRATE_UPDATE_TOKEN(uint16_t token)
{
    token++;

    if (token >= DRV_HEARTRATE_TOKEN_MAX)
    {
        token = 1;
    }

    return token;
}



static DRV_HEARTRATE_CLIENT_OBJ* _DRV_HEARTRATE_DriverHandleValidate(DRV_HANDLE handle)
{
    /* This function returns the pointer to the client object that is
       associated with this handle if the handle is valid. Returns NULL
       otherwise.
    */
    uint32_t drvInstance = 0;
    DRV_HEARTRATE_CLIENT_OBJ* clientObj = NULL;

    if((handle != DRV_HANDLE_INVALID) && (handle != 0))
    {
        /* Extract the instance value from the handle */
        drvInstance = ((handle & DRV_HEARTRATE_INSTANCE_INDEX_MASK) >> 8);
        if (drvInstance >= DRV_HEARTRATE_INSTANCES_NUMBER)
        {
            return (NULL);
        }
        if ((handle & DRV_HEARTRATE_CLIENT_INDEX_MASK) >= gDrvHEARTRATEObj[drvInstance].nClientsMax)
        {
            return (NULL);
        }

        clientObj = &((DRV_HEARTRATE_CLIENT_OBJ *)gDrvHEARTRATEObj[drvInstance].clientObjPool)[handle & DRV_HEARTRATE_CLIENT_INDEX_MASK];

        if ((handle != clientObj->clientHandle) || (clientObj->inUse == false))
        {
            return (NULL);
        }
    }

    return(clientObj);
}

SYS_MODULE_OBJ DRV_HEARTRATE_Initialize( const SYS_MODULE_INDEX drvIndex, const SYS_MODULE_INIT* const init )
{
    DRV_HEARTRATE_OBJ *dObj = NULL;
    DRV_HEARTRATE_INIT *heartrateInit = (DRV_HEARTRATE_INIT *)init ;

    /* Validate the request */
    if(drvIndex >= DRV_HEARTRATE_INSTANCES_NUMBER)
    {
        //SYS_DEBUG(SYS_ERROR_ERROR, "Invalid driver instance");
        return SYS_MODULE_OBJ_INVALID;
    }
    
    heartrate9_initialize();

    if(gDrvHEARTRATEObj[drvIndex].inUse != false)
    {
        //SYS_DEBUG(SYS_ERROR_ERROR, "Instance already in use");
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Allocate the driver object */
    dObj                        = &gDrvHEARTRATEObj[drvIndex];
    dObj->inUse                 = true;
    dObj->heartratePlib          = heartrateInit->heartratePlib;
    dObj->clientObjPool         = heartrateInit->clientObjPool;
    dObj->nClientsMax           = heartrateInit->numClients;
    dObj->nClients              = 0;
    dObj->isExclusive           = false;
    dObj->heartrateTokenCount       = 1;
    


    if (OSAL_MUTEX_Create(&dObj->clientMutex) == OSAL_RESULT_FALSE)
    {
        /*  If the mutex was not created because the memory required to
            hold the mutex could not be allocated then NULL is returned. */
        return SYS_MODULE_OBJ_INVALID;
    }

  

    dObj->heartratePlib->setReadThreshold(10); 
    dObj->heartratePlib->readNotificationEnable(true,false);
    dObj->heartratePlib->readCallbackRegister((DRV_HEARTRATE_PLIB_READ_CALLBACK_REG)_DRV_HEARTRATE_PLIB_CallbackHandler, (uintptr_t)dObj);
    dObj->heartratePlib->read(dObj->rx_buff,DRV_RECEIVE_HEARTRATE_DATA_SIZE);
   
    /* Update the status */
    dObj->status = SYS_STATUS_READY;

    /* Return the object structure */
    return ( (SYS_MODULE_OBJ)drvIndex );
}

static void _DRV_HEARTRATE_PLIB_CallbackHandler(int event, uintptr_t context)
{
    uint32_t nBytesAvailable = 0;
    DRV_HEARTRATE_OBJ *dObj = (DRV_HEARTRATE_OBJ*)context;
    if (event == HEARTRATE_EVENT_READ_THRESHOLD_REACHED)
    {
        /* Receiver should atleast have the thershold number of bytes in the ring buffer */
        nBytesAvailable = dObj->heartratePlib->readCountGet();
        
        dObj->no_of_bytes_rcvd = dObj->heartratePlib->read((uint8_t*)&dObj->rx_data[0], nBytesAvailable);
        dObj->hr9_data_rx_flag = true;
    }
}

SYS_STATUS DRV_HEARTRATE_Status( SYS_MODULE_OBJ object)
{
    /* Validate the request */
    if( (object == SYS_MODULE_OBJ_INVALID) || (object >= DRV_HEARTRATE_INSTANCES_NUMBER) )
    {
        //SYS_DEBUG(SYS_ERROR_ERROR, "Invalid system object handle");
        return SYS_STATUS_UNINITIALIZED;
    }

    return (gDrvHEARTRATEObj[object].status);
}

DRV_HANDLE DRV_HEARTRATE_Open( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent )
{
    DRV_HEARTRATE_OBJ *dObj = NULL;
    DRV_HEARTRATE_CLIENT_OBJ *clientObj = NULL;
    uint32_t iClient;

    /* Validate the request */
    if (drvIndex >= DRV_HEARTRATE_INSTANCES_NUMBER || (!bInitialized))
    {
        //SYS_DEBUG(SYS_ERROR_ERROR, "Invalid Driver Instance");
        return DRV_HANDLE_INVALID;
    }

    dObj = &gDrvHEARTRATEObj[drvIndex];

    if(dObj->status != SYS_STATUS_READY)
    {
        //SYS_DEBUG(SYS_ERROR_ERROR, "Was the driver initialized?");
        return DRV_HANDLE_INVALID;
    }

    /* Acquire the instance specific mutex to protect the instance specific
     * client pool
     */
    if (OSAL_MUTEX_Lock(&dObj->clientMutex , OSAL_WAIT_FOREVER ) == OSAL_RESULT_FALSE)
    {
        return DRV_HANDLE_INVALID;
    }

    if(dObj->isExclusive)
    {
        /* This means the another client has opened the driver in exclusive
           mode. The driver cannot be opened again */
        OSAL_MUTEX_Unlock( &dObj->clientMutex);
        return DRV_HANDLE_INVALID;
    }

    if((dObj->nClients > 0) && (ioIntent & DRV_IO_INTENT_EXCLUSIVE))
    {
        /* This means the driver was already opened and another driver was
           trying to open it exclusively.  We cannot give exclusive access in
           this case */
        OSAL_MUTEX_Unlock( &dObj->clientMutex);
        return(DRV_HANDLE_INVALID);
    }

    /* Enter here only if the lock was obtained */
    for(iClient = 0; iClient != dObj->nClientsMax; iClient++)
    {
        if(false == ((DRV_HEARTRATE_CLIENT_OBJ *)dObj->clientObjPool)[iClient].inUse)
        {
            /* This means we have a free client object to use */

            clientObj = &((DRV_HEARTRATE_CLIENT_OBJ *)dObj->clientObjPool)[iClient];

            clientObj->inUse        = true;

            clientObj->hDriver      = dObj;

            clientObj->ioIntent     = ioIntent;
            
            clientObj->callback     = NULL;

            if(ioIntent & DRV_IO_INTENT_EXCLUSIVE)
            {
                /* Set the driver exclusive flag */
                dObj->isExclusive = true;
            }

            dObj->nClients ++;

            /* Generate and save client handle in the client object, which will
             * be then used to verify the validity of the client handle.
             */
            clientObj->clientHandle = _DRV_HEARTRATE_MAKE_HANDLE(dObj->heartrateTokenCount, drvIndex, iClient);

            /* Increment the instance specific token counter */
            dObj->heartrateTokenCount = _DRV_HEARTRATE_UPDATE_TOKEN(dObj->heartrateTokenCount);
            
            

            break;
        }
    }

    OSAL_MUTEX_Unlock(&dObj->clientMutex);

    /* Driver index is the handle */
    return clientObj ? ((DRV_HANDLE)clientObj->clientHandle) : DRV_HANDLE_INVALID;
}

void DRV_HEARTRATE_Close( DRV_HANDLE handle )
{
    DRV_HEARTRATE_CLIENT_OBJ* clientObj;
    DRV_HEARTRATE_OBJ* dObj;

    /* Validate the handle */
    clientObj = _DRV_HEARTRATE_DriverHandleValidate(handle);

    if(clientObj != NULL)
    {
        dObj = (DRV_HEARTRATE_OBJ *)clientObj->hDriver;

        /* Acquire the instance specifc mutex to protect the instance specific
         * client pool
         */
        if (OSAL_MUTEX_Lock(&dObj->clientMutex , OSAL_WAIT_FOREVER ) == OSAL_RESULT_TRUE)
        {
            /* Reduce the number of clients */
            dObj->nClients --;

            /* Reset the exclusive flag */
            dObj->isExclusive = false;

            /* De-allocate the object */
            clientObj->inUse = false;

            /* Release the instance specific mutex */
            OSAL_MUTEX_Unlock( &dObj->clientMutex );
        }
    }
}


void DRV_HEARTRATE_TASKS( SYS_MODULE_OBJ object)
{
    int drvIndex = (int)object;
    
     DRV_HEARTRATE_OBJ *dObj = NULL;
     DRV_HEARTRATE_CLIENT_OBJ *clientObj=NULL;
    /* Allocate the driver object */
     int heartrate_data=-1;
    //wait for 5 seconds 
    if(!bInitialized)
    {
        
        iInitializeDelay--; 
        SYSTICK_DelayMs(1);
        if(!iInitializeDelay)   bInitialized=true;
        return;
    }
  
    // perform a read and provide callback
    if(DRV_HEARTRATE_Status(object)  == SYS_STATUS_READY)
    {
       dObj= &gDrvHEARTRATEObj[drvIndex];
       
      
        for(int iClient = 0; iClient != dObj->nClientsMax; iClient++)
        {
            if(true == ((DRV_HEARTRATE_CLIENT_OBJ *)dObj->clientObjPool)[iClient].inUse)
            {
                /* This means we have a free client object to use */

                clientObj = &((DRV_HEARTRATE_CLIENT_OBJ *)dObj->clientObjPool)[iClient];

                if(clientObj->clientHandle && clientObj->callback)
                {
                        heartrate_data = DRV_HEARTRATE_Read(clientObj->clientHandle);
                        if(heartrate_data > 0)
                            (clientObj->callback)(clientObj->clientHandle, dObj->heartRate);
                   //code for callback and cloud to be added.
                }
            }
        }
    }

}


bool DRV_HEARTRATE_GetHeartrate( const DRV_HANDLE handle, int * heartRate)
{
    DRV_HEARTRATE_CLIENT_OBJ* clientObj = (DRV_HEARTRATE_CLIENT_OBJ *)NULL;
    DRV_HEARTRATE_OBJ* dObj = NULL;

    /* Validate the driver handle */
    clientObj = _DRV_HEARTRATE_DriverHandleValidate(handle);

    if(clientObj != NULL)
    {
        dObj = clientObj->hDriver;
        if(dObj->heartRate <= 0) return false;
        *heartRate = dObj->heartRate;
    }
     return true;
}

/* trigger an asynchronous read of the BME280, once complete the registered callback will be called and the Get functions can then be used to access the data */
int DRV_HEARTRATE_Read(DRV_HANDLE handle)
{
    DRV_HEARTRATE_CLIENT_OBJ* clientObj = (DRV_HEARTRATE_CLIENT_OBJ *)NULL;
    int heartrate_data=-1;
    /* Validate the driver handle */
    clientObj = _DRV_HEARTRATE_DriverHandleValidate(handle);
    
     //perform a read!!!.
        //DRV_HEARTRATE_Read();
         // Return the Heartrate 
        if(true == is_heartrate9_byte_ready(clientObj->hDriver))
        {
            heartrate_data      = heartrate9_read_byte(clientObj->hDriver);
            if(heartrate_data !=-1)
            {
                 
                 clientObj->hDriver->heartRate = heartrate_data;
                 SYS_CONSOLE_Print(SYS_CONSOLE_INDEX_0, "DRV new heartrate %d \r\n", heartrate_data);
                 return heartrate_data;
            }
        }
    
    return heartrate_data;
}


/* register a client callback handler to be notified when the read command has returned with new data */
void DRV_HEARTRATE_ClientEventHandlerSet(const DRV_HANDLE handle, const DRV_HEARTRATE_APP_CALLBACK callback)
{
    DRV_HEARTRATE_CLIENT_OBJ* clientObj = (DRV_HEARTRATE_CLIENT_OBJ *)NULL;

    /* Validate the driver handle */
    clientObj = _DRV_HEARTRATE_DriverHandleValidate(handle);
    
    clientObj->callback = callback;
   
    return ;
}

////// static methods to support reading multi objects///////

static void hr9_rx_buff_update(DRV_HEARTRATE_OBJ *dObj)
{
    if(true == dObj->hr9_data_rx_flag)
    {
        dObj->hr9_data_rx_flag = false;
        if((dObj->rx_buff_in_index + dObj->no_of_bytes_rcvd) >= DRV_RECEIVE_HEARTRATE_DATA_SIZE)
        {
            memcpy((char *)&dObj->rx_buff[dObj->rx_buff_in_index], (char *)dObj->rx_data, (DRV_RECEIVE_HEARTRATE_DATA_SIZE - dObj->rx_buff_in_index));
            memcpy((char *)dObj->rx_buff, (char *)&dObj->rx_data[DRV_RECEIVE_HEARTRATE_DATA_SIZE - dObj->rx_buff_in_index], (dObj->rx_buff_in_index + dObj->no_of_bytes_rcvd - DRV_RECEIVE_HEARTRATE_DATA_SIZE));
        }
        else
        {
            memcpy((char *)&dObj->rx_buff[dObj->rx_buff_in_index], (char *)dObj->rx_data, dObj->no_of_bytes_rcvd);
        }
        
        dObj->rx_buff_in_index += dObj->no_of_bytes_rcvd;
        if(dObj->rx_buff_in_index >= DRV_RECEIVE_HEARTRATE_DATA_SIZE)
        {
            dObj->rx_buff_in_index = dObj->rx_buff_in_index - DRV_RECEIVE_HEARTRATE_DATA_SIZE;
        }
    }
}

static bool is_heartrate9_byte_ready(DRV_HEARTRATE_OBJ *dObj)
{
   
    if (false == dObj->hr9_rx_data_buf_clr_state)
    {
        memset((char*)dObj->rx_buff, 0x00, DRV_HEARTRATE_RECEIVE_BUFFER_SIZE);
       dObj->hr9_rx_data_buf_clr_state  = true;
        dObj->rx_buff_in_index     = 0;
        dObj->rx_buff_out_index    = 0;
        dObj->hr9_data_rx_flag     = false;
    }
    if((dObj->rx_buff_in_index == dObj->rx_buff_out_index) && (false == dObj->hr9_data_rx_flag)) return false;

    return true;
}

static int8_t heartrate9_read_byte(DRV_HEARTRATE_OBJ *dObj)
{

    uint16_t lcl_rx_buff_in_index = dObj->rx_buff_in_index;

    hr9_rx_buff_update(dObj);
    if(dObj->rx_buff_out_index > lcl_rx_buff_in_index)
    {
        lcl_rx_buff_in_index    = DRV_RECEIVE_HEARTRATE_DATA_SIZE;
    }
    while(dObj->rx_buff_out_index < lcl_rx_buff_in_index)
    {
        switch(dObj->heartrate9_read_byte_state)
        {
            case HEADER_BYTE1:
                for(; dObj->rx_buff_out_index < lcl_rx_buff_in_index; dObj->rx_buff_out_index++)
                {
                    if(dObj->rx_buff[dObj->rx_buff_out_index] == '1')
                    {
                        dObj->heartrate9_read_byte_state  = HEADER_BYTE2;
                        dObj->rx_buff_out_index++;
                        break;
                    }
                }
                break;
            case HEADER_BYTE2:
                if(dObj->rx_buff[dObj->rx_buff_out_index] == '5')
                {
                    dObj->heartrate9_read_byte_state  = HEADER_BYTE3;
                }
                else
                {
                    dObj->heartrate9_read_byte_state  = HEADER_BYTE1;
                }
                dObj->rx_buff_out_index++;
                break;
            case HEADER_BYTE3:
                if(dObj->rx_buff[dObj->rx_buff_out_index] == ';')
                {
                    dObj->heartrate9_read_byte_state  = HEART_RATE_BYTE_POS_FIND;
                }
                else
                {
                    dObj->heartrate9_read_byte_state  = HEADER_BYTE1;
                }
                dObj->rx_buff_out_index++;
                break;
            case HEART_RATE_BYTE_POS_FIND:
                for(; dObj->rx_buff_out_index < lcl_rx_buff_in_index; dObj->rx_buff_out_index++)
                {
                    if(dObj->rx_buff[dObj->rx_buff_out_index] == ';')
                    {
                        dObj->semicoln_cnt++;
                        if(dObj->semicoln_cnt == 3)
                        {
                            dObj->semicoln_cnt                = 0;
                            dObj->rx_buff_out_index++;
                            dObj->current_heartrate           = 0;
                            dObj->heartrate9_read_byte_state  = HEART_RATE_READ;
                            break;
                        }
                    }
                }
                break;
            case HEART_RATE_READ:
                for(; dObj->rx_buff_out_index < lcl_rx_buff_in_index; dObj->rx_buff_out_index++)
                {
                    if(dObj->rx_buff[dObj->rx_buff_out_index] != ';')
                    {
                        dObj->current_heartrate   = (dObj->current_heartrate * 10) + (dObj->rx_buff[dObj->rx_buff_out_index] - 0x30);
                    }
                    else
                    {
                        dObj->heartrate9_read_byte_state  = HEADER_BYTE1;
                        dObj->heartrate_pross_cnt++;
                        if(dObj->heartrate_pross_cnt == 100)
                        {
                            dObj->heartrate_pross_cnt         = 0;
                            dObj->hr9_rx_data_buf_clr_state   = false;
                            return((int8_t)dObj->current_heartrate);
                        }
                        break;
                    }
                }
                break;
            default:
                break;
        }
    }
    if(dObj->rx_buff_out_index == DRV_RECEIVE_HEARTRATE_DATA_SIZE)
    {
        dObj->rx_buff_out_index = 0;
    }

    return(-1);
}


static void heartrate9_initialize(void)
{
    SYSTICK_TimerStart();
    heartrate9_set_rst(0);
    SYSTICK_DelayMs(1);
    heartrate9_set_rst(1);
}


static void heartrate9_set_rst(uint8_t state)
{
    if ( state > 0 )
        WDRV_WINC_RESETN_Set();
    else
        WDRV_WINC_RESETN_Clear();
}