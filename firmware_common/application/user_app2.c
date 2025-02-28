/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp2" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP2" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp2Initialize(void)
- void UserApp2RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp2"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp2Flags;                          /*!< @brief Global state flags */
volatile u32 G_u32UserApp2ANTInfo;


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */

extern volatile u32 G_u32AntApiCurrentMessageTimeStamp;
extern volatile AntApplicationMessageType G_eAntApiCurrentMessageClass;
extern volatile u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];
extern volatile AntExtendedDataType G_sAntApiCurrentMessageExtData;

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp2_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp2_pfStateMachine;               /*!< @brief The state machine function pointer */
static u32 UserApp2_u32Timeout;
static u32 UserApp2_u32TickMsgCount = 0;
static u32 UserApp2_u32DataMsgCount = 0;

//static u32 UserApp2_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp2Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp2Initialize(void)
{
  UserApp2_pfStateMachine = UserApp2SM_Idle;
}


void UserApp2InitializeANT(void) {
   AntAssignChannelInfoType sChannelInfo;

  if(AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_UNCONFIGURED)
  {
    sChannelInfo.AntChannel = U8_ANT_CHANNEL_PERIOD_HI_USERAPP;
    sChannelInfo.AntChannelType = CHANNEL_TYPE_SLAVE;
    sChannelInfo.AntChannelPeriodHi = U8_ANT_CHANNEL_PERIOD_HI_USERAPP;
    sChannelInfo.AntChannelPeriodLo = U8_ANT_CHANNEL_PERIOD_LO_USERAPP;
    
    sChannelInfo.AntDeviceIdHi = U8_ANT_DEVICE_HI_USERAPP;
    sChannelInfo.AntDeviceIdLo = U8_ANT_DEVICE_LO_USERAPP;
    sChannelInfo.AntDeviceType = U8_ANT_DEVICE_TYPE_USERAPP;
    sChannelInfo.AntTransmissionType = U8_ANT_TRANSMISSION_TYPE_USERAPP;
    
    sChannelInfo.AntFrequency = U8_ANT_FREQUENCY_USERAPP;
    sChannelInfo.AntTxPower = U8_ANT_TX_POWER_USERAPP;
    
    sChannelInfo.AntNetwork = ANT_NETWORK_DEFAULT;
    for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
    {
      sChannelInfo.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
    }
    
    LedOn(RED);
    LcdClearChars(LINE1_START_ADDR, 20);
    LcdClearChars(LINE2_START_ADDR, 20);
    LcdMessage(LINE1_START_ADDR, "ANT Slave test");
  }



  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sChannelInfo) )
  {
    UserApp2_pfStateMachine = UserApp2SM_WaitAntReady;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp2_pfStateMachine = UserApp2SM_Error;
  }

} /* end UserApp2Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp2RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp2RunActiveState(void)
{
  UserApp2_pfStateMachine();

} /* end UserApp2RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void UserApp2SM_Idle(void)
{

} /* end UserApp2SM_Idle() */



/*!--------------------------------------------------------------------------------------------------------------------*/
/* Wait for ANT channel to be configured */
void UserApp2SM_WaitAntReady(void)
{
  if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_CONFIGURED) 
  {
    LedOff(RED);
    LedOn(YELLOW);
    AntOpenChannelNumber(U8_ANT_CHANNEL_USERAPP);
    UserApp2_u32Timeout = G_u32SystemTime1ms;
    UserApp2_pfStateMachine = UserApp2SM_WaitChannelOpen;
  }

} /* end UserApp2RunActiveState */


/*!--------------------------------------------------------------------------------------------------------------------*/
/* Wait for ANT channel to be open */
void UserApp2SM_WaitChannelOpen(void)
{
  if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    LedOff(YELLOW);
    LedOn(GREEN);
    UserApp2_pfStateMachine = UserApp2SM_ChannelAwaitConnection;
  }

  if (IsTimeUp(&UserApp2_u32Timeout, U32_TIMEOUT_OPEN_CHANNEL)) 
  {
    AntCloseChannelNumber(U8_ANT_CHANNEL_USERAPP);
    LedOff(GREEN);
    LedOn(YELLOW);
    UserApp2_pfStateMachine = UserApp2SM_Idle;
  }
} /* end UserApp2SM_WaitChannelOpen */  

void UserApp2SM_ChannelAwaitConnection(void) {
  if (AntReadAppMessageBuffer()) {
    LedOff(GREEN);
    LedOn(BLUE);
    G_u32UserApp2ANTInfo |= 1;
    UserApp2_pfStateMachine = UserApp2SM_ChannelOpen;
  }
}

void UserApp2SM_ChannelOpen(void) {
  static u8 u8LastState = 0xff;
  static u8 au8TickMessage[] = "EVENT x\n\r";
  static u8 au8DataContent[] = "xxxxxxxxxxxxxxxx";
  static u8 au8LastAntData[ ANT_APPLICATION_MESSAGE_BYTES ] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  static u8 au8TestMessage[] = {0, 0, 0, 0, 0xa5, 0, 0, 0};
  bool bGotNewData;




  if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) != ANT_OPEN) 
  {
    u8LastState = 0xff;
    LedOff(BLUE);
    LedOn(GREEN);
    
    UserApp2_u32Timeout = G_u32SystemTime1ms;
    UserApp2_pfStateMachine = UserApp2SM_WaitChannelClose;
  }

  if (AntReadAppMessageBuffer())
  {
    //DebugPrintf("Message");
    if (G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      LedOff(PURPLE);
      u8LastState = 0xff;
      static bool bGotNewData = FALSE;

      
      for(u8 i = 0; i < ANT_APPLICATION_MESSAGE_BYTES; i++) {
        if (G_au8AntApiCurrentMessageBytes[i] != au8LastAntData[i]){
          bGotNewData = TRUE;
          au8LastAntData[i] = G_au8AntApiCurrentMessageBytes[i];
          au8DataContent[2 * i] = HexToASCIICharLower(G_au8AntApiCurrentMessageBytes[i] >> 4);
          au8DataContent[2*i + 1] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] & 0xf);
        }
      }
      

      if (bGotNewData) {
        bGotNewData = FALSE;
        DebugPrintf(au8DataContent);
        
        G_u32UserApp2ANTInfo |= 0x2;
      }
    }
    else if (G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      UserApp2_u32TickMsgCount++;
      if (u8LastState != G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX])
      {
        u8LastState = G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX];
        au8TickMessage[6] = HexToASCIICharLower(u8LastState);
        DebugPrintf(au8TickMessage);

        switch (u8LastState)
        {
          case EVENT_RX_FAIL:
          {
            LedOn(PURPLE);
            break;
          }
          case EVENT_RX_FAIL_GO_TO_SEARCH:
          {
            LedOff(BLUE);
            LedOn(GREEN);
            break;
          }
          case EVENT_RX_SEARCH_TIMEOUT:
          {
            DebugPrintf("Search timeout\r\n");
            break;
          }
          default:
          {
            DebugPrintf("Unexpected Event\r\n");
            break;
          }
        }
      }
    }
  }
}

static void UserApp2SM_WaitChannelClose(void) {
  if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
    LedOff(GREEN);
    LedOn(YELLOW);

    UserApp2_pfStateMachine = UserApp2SM_Idle;
  }

  if ( IsTimeUp(&UserApp2_u32Timeout, U32_TIMEOUT_CLOSE_CHANNEL))
  {
    LedOff(GREEN);
    LedBlink(RED, LED_4HZ);

    UserApp2_pfStateMachine = UserApp2SM_Error;
  }
}


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp2SM_Error(void)          
{
  
} /* end UserApp2SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
