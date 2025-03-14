/*!*********************************************************************************************************************
@file ant_radio.c                                                                
@brief Handles connection with the ANT radio, placing new messages into G_u32AntRadioANTInfo
------------------------------------------------------------------------------------------------------------------------
GLOBALS
- G_u32AntRadioFlags
- G_u32AntRadioANTInfo

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void AntRadioInitialize(void)
- void AntRadioRunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>AntRadio"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32AntRadioFlags;                          /*!< @brief Global state flags */
volatile u32 G_u32AntRadioANTInfo;


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
Variable names shall start with "AntRadio_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type AntRadio_pfStateMachine;               /*!< @brief The state machine function pointer */
static u32 AntRadio_u32Timeout;

//static u32 AntRadio_u32Timeout;                           /*!< @brief Timeout counter used across states */


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
@fn void AntRadioInitialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void AntRadioInitialize(void)
{
  AntRadio_pfStateMachine = AntRadioSM_Idle;
}


void AntRadio_IntializeANT(void) {
   AntAssignChannelInfoType sChannelInfo;

  if(AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_UNCONFIGURED)
  {
    sChannelInfo.AntChannel = U8_ANT_CHANNEL_USERAPP;
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
    
    LcdMessage(LINE1_START_ADDR, INTIALIZE_ANT_MESSAGE_1);
    LcdMessage(LINE2_START_ADDR, INTIALIZE_ANT_MESSAGE_2);
    /* If good initialization, set state to Idle */
    if( AntAssignChannel(&sChannelInfo) )
    {
      AntRadio_pfStateMachine = AntRadioSM_WaitAntReady;
    }
    else
    {
      /* The task isn't properly initialized, so shut it down and don't run */
      AntRadio_pfStateMachine = AntRadioSM_Error;
    }
  } else {
    AntRadio_pfStateMachine = AntRadioSM_ChannelAwaitConnection;
  }


} /* end AntRadioInitialize() */

void AntRadio_CloseANT() {
  AntCloseChannelNumber(U8_ANT_CHANNEL_USERAPP);
}

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void AntRadioRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void AntRadioRunActiveState(void)
{
  AntRadio_pfStateMachine();

} /* end AntRadioRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* Idle state, for when radio not in use */
static void AntRadioSM_Idle(void)
{

} /* end AntRadioSM_Idle() */



/*!--------------------------------------------------------------------------------------------------------------------*/
/* Wait for ANT channel to be configured */
void AntRadioSM_WaitAntReady(void)
{
  if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_CONFIGURED) 
  {
    AntOpenChannelNumber(U8_ANT_CHANNEL_USERAPP);
    AntRadio_u32Timeout = G_u32SystemTime1ms;
    AntRadio_pfStateMachine = AntRadioSM_WaitChannelOpen;
  }

} /* end AntRadioRunActiveState */


/*!--------------------------------------------------------------------------------------------------------------------*/
/* Wait for ANT channel to be open */
void AntRadioSM_WaitChannelOpen(void)
{
  if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    LcdMessage(LINE1_START_ADDR, WAIT_CHANNEL_OPEN_MESSAGE_1);
    LcdMessage(LINE2_START_ADDR, WAIT_CHANNEL_OPEN_MESSAGE_2);
    AntRadio_pfStateMachine = AntRadioSM_ChannelAwaitConnection;
  }

  if (IsTimeUp(&AntRadio_u32Timeout, U32_TIMEOUT_OPEN_CHANNEL)) 
  {
    LcdMessage(LINE1_START_ADDR, UNABLE_TO_OPEN_ANT_MESSAGE_1);
    LcdMessage(LINE2_START_ADDR, UNABLE_TO_OPEN_ANT_MESSAGE_2);
    AntCloseChannelNumber(U8_ANT_CHANNEL_USERAPP);
    AntRadio_pfStateMachine = AntRadioSM_Idle;
  }
} /* end AntRadioSM_WaitChannelOpen */  

void AntRadioSM_ChannelAwaitConnection(void) {
  if (AntReadAppMessageBuffer()) {
    if ( G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX] == EVENT_RX_SEARCH_TIMEOUT ) {
      LcdMessage(LINE1_START_ADDR, SEARCH_TIMEOUT_ANT_MESSAGE_1);
      LcdMessage(LINE2_START_ADDR, SEARCH_TIMEOUT_ANT_MESSAGE_2);
      AntRadio_u32Timeout = G_u32SystemTime1ms;
      AntRadio_pfStateMachine = AntRadioSM_WaitChannelClose;
    } else {
      G_u32AntRadioANTInfo |= 1;
      AntRadio_pfStateMachine = AntRadioSM_ChannelOpen;
    }
  }
}

void AntRadioSM_ChannelOpen(void) {
  static u8 u8LastState = 0xff;
  static u8 au8TickMessage[] = "EVENT x\n\r";
  static u8 au8DataContent[] = "xxxxxxxxxxxxxxxx";
  static u8 au8LastAntData[ ANT_APPLICATION_MESSAGE_BYTES ] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) != ANT_OPEN) 
  {
    u8LastState = 0xff;
    
    AntRadio_u32Timeout = G_u32SystemTime1ms;
    AntRadio_pfStateMachine = AntRadioSM_WaitChannelClose;
  }

  if (AntReadAppMessageBuffer())
  {
    //DebugPrintf("Message");
    if (G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      LedOff(YELLOW);
      u8LastState = 0xff;
      static bool bGotNewData = FALSE;

      
      for(u8 i = 0; i < ANT_APPLICATION_MESSAGE_BYTES; i++) {
        if (G_au8AntApiCurrentMessageBytes[i] != au8LastAntData[i]){
          bGotNewData = TRUE;
          au8LastAntData[i] = G_au8AntApiCurrentMessageBytes[i];
          //au8DataContent[2 * i] = HexToASCIICharLower(G_au8AntApiCurrentMessageBytes[i] >> 4);
          //au8DataContent[2*i + 1] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[i] & 0xf);
        }
      }
      if (bGotNewData) {
        bGotNewData = FALSE;
        //DebugPrintf(au8DataContent);
        G_u32AntRadioANTInfo |= 0x2;
      }
    }
    else if (G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      if (u8LastState != G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX])
      {
        if (G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX] == EVENT_RX_FAIL)
        {
          LedOn(YELLOW);
        }
      }
    }
  }
}

static void AntRadioSM_DisplayClose(void) {
  if ( IsTimeUp(&AntRadio_u32Timeout, U32_TIMEOUT_DISPLAY_FAIL) ) {
    G_u32AntRadioANTInfo |= 2;
    AntRadio_pfStateMachine = AntRadioSM_Idle;
  }
}

static void AntRadioSM_WaitChannelClose(void) {
  if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
    AntRadio_u32Timeout = G_u32SystemTime1ms;
    AntRadio_pfStateMachine = AntRadioSM_DisplayClose;
  }

  if ( IsTimeUp(&AntRadio_u32Timeout, U32_TIMEOUT_CLOSE_CHANNEL))
  {
    LedBlink(RED, LED_4HZ);

    AntRadio_pfStateMachine = AntRadioSM_Error;
  }
}


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void AntRadioSM_Error(void)          
{
  
} /* end AntRadioSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
