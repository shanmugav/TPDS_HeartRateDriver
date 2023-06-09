/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_usart_echo.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
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

#include "app_usart_echo.h"
#include "user.h"
#include <string.h>
#include "definitions.h"     
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

static const char startMessage[] = "*** HEART RATE Driver Demo Application ***\r\n"
"*** Continuously prints Heart Rate for configured Time  ***\r\n";

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_HEART_RATE9_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/
APP_DATA appHeartRateData;

#define CLICK_HEARTRATE9_USART_READ_BUFFER_SIZE 128
#define RECEIVE_BUFFER_SIZE     (CLICK_HEARTRATE9_USART_READ_BUFFER_SIZE * 10)
APP_HEART_RATE9_DATA app_heartrate_Data;
void APP_HeartRateCallback (DRV_HANDLE handle, int heartrate );


/**
  Section: Variable Definitions
 */

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_HEART_RATE9_Initialize ( void )

  Remarks:
    See prototype in app_usart_echo.h.
 */

void APP_HEART_RATE9_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_heartrate_Data.state = APP_HEART_RATE9_STATE_INIT;
    SYS_CONSOLE_Print(SYS_CONSOLE_INDEX_0, startMessage);
}



/******************************************************************************
  Function:
    void APP_HEART_RATE9_Tasks ( void )

  Remarks:
    See prototype in app_usart_echo.h.
 */

void APP_HEART_RATE9_Tasks ( void )
{
   /* Check the application's current state. */
    switch ( appHeartRateData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;


            if (appInitialized)
            {
                
                appHeartRateData.drvHeartRatehandle= DRV_HEARTRATE_Open(DRV_HEARTRATE_INDEX_0,DRV_IO_INTENT_READWRITE);
                 if(appHeartRateData.drvHeartRatehandle == DRV_HANDLE_INVALID)
                {
                    //appHeartRateData.state = APP_STATE_ERROR;
                }
                 else
                 {
                      appHeartRateData.state = APP_STATE_SERVICE_TASKS;
                 }
                
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
          
           DRV_HEARTRATE_ClientEventHandlerSet(appHeartRateData.drvHeartRatehandle, APP_HeartRateCallback);
           break;
        }
        case APP_STATE_ERROR:

        {
            SYS_CONSOLE_Print(SYS_CONSOLE_INDEX_0, "Un Error state !!! exiting !!!\r\n");
            break;
        }


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
    
}

void APP_HeartRateCallback (DRV_HANDLE handle, int heartrate )
{
    if(handle == appHeartRateData.drvHeartRatehandle)
     SYS_CONSOLE_Print(SYS_CONSOLE_INDEX_0, "Received new heartrate %d \r\n", heartrate);
    else
        SYS_CONSOLE_Print(SYS_CONSOLE_INDEX_0, "Received new bug!!! %d \r\n", heartrate);
}

/*******************************************************************************
 End of File
 */
