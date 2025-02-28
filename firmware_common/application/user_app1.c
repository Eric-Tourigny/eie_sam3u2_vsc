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
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
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
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */
static u8 UserApp1_u8cactusBitmaps[12][8] = {{}, {}, {}, {}, {}, {}, CACTUS_PATTERN, {}, {}, {}, {}, {}};
static u8 UserAPP1_u8dino_pattern[8] = DINO_PATTERN;
static u8 UserApp1_u8MillisecondCount = 0;
static u8 UserApp1_u8CactusPositions[21];
static u8 UserApp1_u8SubframeCount = 0;
static s16 UserApp1_s16DinoHeight = 0x0000;
static s16 UserApp1_s16DinoVelocity = 0;
static u8 (*UserApp1_u8DinoBottomMask)[8] = UserApp1_u8cactusBitmaps + 11;
static u8 (*UserApp1_u8DinoTopMask)[8] = UserApp1_u8cactusBitmaps + 11;
static u8 UserApp1_u8FramesToNextCactus = 5;

bool (*UserApp1_checkInputFunction)();

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/



enum { 
  STATE_INIT = 0,
  STATE_RUN_GAME,
  STATE_CHECK_MENU,
  STATE_CRASH_ANIMATION,
  STATE_WAIT_ANT_READY,
  STATE_WAIT_ANT_OPEN,
} typedef State_t;

State_t currentState = STATE_INIT;

void enterInit(State_t prevState) {}

void enterRunGame(State_t prevState) {
  LcdClearChars(LINE1_START_ADDR, 20);
  LcdClearChars(LINE2_START_ADDR, 20);
  LcdPutChar(LINE1_START_ADDR, DINO_TOP_NUM);
  LcdPutChar(LINE2_START_ADDR, DINO_BOTTOM_NUM);

  UserApp1_u8MillisecondCount = 0;
  for (u8 u8Index = 0; u8Index < 20; u8Index++)
    UserApp1_u8CactusPositions[u8Index] = ' ';
  UserApp1_u8CactusPositions[20] = '\0';  
  UserApp1_u8SubframeCount = 0;
  UserApp1_s16DinoHeight = 0x0000;
  UserApp1_s16DinoVelocity = 0;
  UserApp1_u8DinoBottomMask = UserApp1_u8cactusBitmaps + 11;
  UserApp1_u8DinoTopMask = UserApp1_u8cactusBitmaps + 11;
  UserApp1_u8FramesToNextCactus = 5;
}

void enterCheckMenu(State_t prevState) {
  LcdClearChars(LINE1_START_ADDR, 20);
  LcdClearChars(LINE2_START_ADDR, 20);
  LcdMessage(LINE1_START_ADDR, "Press BUTTON 0 to");
  LcdMessage(LINE2_START_ADDR, "begin");
}

void enterCrashAnimation(State_t prevState) {}

void enterWaitANTReady(State_t prevState) {
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
  }

  LedOn(RED);
}

void enterWaitANTOpen(State_t prevState) {
  AntOpenChannelNumber(U8_ANT_CHANNEL_USERAPP);
  LedOff(RED);
  LedOn(YELLOW);
}

void (*stateTransition[])(State_t) = {
  enterInit,
  enterRunGame,
  enterCheckMenu,
  enterCrashAnimation,
  enterWaitANTReady,
};

void (*stateFunctionArray[])(void) = {
  UserApp1Initialize,
  UserApp1SM_RunGame,
  UserApp1SM_CheckMenu,
  UserApp1SM_CrashAnimation,
};

void gotoState(State_t targetState) {
  stateTransition[targetState](currentState);
  currentState = targetState;
  UserApp1_pfStateMachine = stateFunctionArray[currentState];
}




void shiftCactuses() {
  for(u8 u8Index = 0; u8Index < 19; u8Index++) {
    UserApp1_u8CactusPositions[u8Index] = UserApp1_u8CactusPositions[u8Index + 1];
  }

  if (--UserApp1_u8FramesToNextCactus == 1) {
    UserApp1_u8CactusPositions[19] = CACTUS_FRONT_NUM;
  } else if (UserApp1_u8FramesToNextCactus == 0)
  {
    UserApp1_u8CactusPositions[19] = CACTUS_BACK_NUM;
    UserApp1_u8FramesToNextCactus = 2;
  } else {
    UserApp1_u8CactusPositions[19] = ' ';
  }

  LcdMessage(LINE2_START_ADDR + 1, UserApp1_u8CactusPositions + 1);
}

void updateCactusCustomCharacters() {
  u8 (*u8cactusFrontBitPattern)[8] = UserApp1_u8cactusBitmaps + 5 - UserApp1_u8SubframeCount;
  LcdModifyCustomChar(CACTUS_FRONT_NUM, *u8cactusFrontBitPattern);
  if (UserApp1_u8CactusPositions[0] == CACTUS_FRONT_NUM) UserApp1_u8DinoBottomMask = u8cactusFrontBitPattern;
  u8 (*u8cactusBackBitPattern)[8] = UserApp1_u8cactusBitmaps + 11 - UserApp1_u8SubframeCount;
  LcdModifyCustomChar(CACTUS_BACK_NUM, *u8cactusBackBitPattern);
  if (UserApp1_u8CactusPositions[0] == CACTUS_BACK_NUM) UserApp1_u8DinoBottomMask = u8cactusBackBitPattern;
}


bool getButtonInput() {
  bool buttonPressed = WasButtonPressed(BUTTON0);
  if (buttonPressed) {
    ButtonAcknowledge(BUTTON0);
  }
  return buttonPressed;
}

bool getANTInput() {

}


/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{

  LcdCommand(LCD_CLEAR_CMD);
  LcdCommand(LCD_FUNCTION_CMD);     //required for custom characters to function

  for (u8 u8BitmapN = 5; u8BitmapN != 0; u8BitmapN--)
    for (u8 u8InnerPos = 0; u8InnerPos < 8; u8InnerPos++)
      UserApp1_u8cactusBitmaps[u8BitmapN][u8InnerPos] = UserApp1_u8cactusBitmaps[u8BitmapN + 1][u8InnerPos] >> 1;

  for (u8 u8BitmapN = 7; u8BitmapN <= 11; u8BitmapN++)
    for (u8 u8InnerPos = 0; u8InnerPos < 8; u8InnerPos++)
      UserApp1_u8cactusBitmaps[u8BitmapN][u8InnerPos] = (UserApp1_u8cactusBitmaps[u8BitmapN - 1][u8InnerPos] << 1) & 0x1F;


  /* If good initialization, set state to Idle */
  if( 1 )
  {
    gotoState(STATE_CHECK_MENU);
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void UserApp1SM_RunGame(void)
{
  if(UserApp1_u8MillisecondCount-- == 0)
  {
    if (UserApp1_u8SubframeCount-- == 0)
    {
      shiftCactuses();
      UserApp1_u8SubframeCount = U8_FRAME_SUBFRAMES;
    }

    updateCactusCustomCharacters();


    /* Updates dino height and sees if its reached the ground*/
    UserApp1_s16DinoHeight += UserApp1_s16DinoVelocity;
    UserApp1_s16DinoVelocity -= 50;
    if (UserApp1_s16DinoHeight <= 0)
    {
      UserApp1_s16DinoHeight = 0;
      UserApp1_s16DinoVelocity = 0;

      /* Dino can jump if its on the ground */
      if (UserApp1_checkInputFunction())
      {
        UserApp1_s16DinoVelocity = 500;
      }
    }

    u8 dino_pattern[8] = {};

    /* Dino height in pixels is dino height divided by 256 */
    u8 pixel_height = ((u16)UserApp1_s16DinoHeight) >> 8;
    

    /* Determines custom char pattern for bottom leftmost tile */
    for (u8 u8Index = 0; u8Index < 8; u8Index++)
    {
      if ((s8)(7 - pixel_height - u8Index) >= 0) {
        dino_pattern[u8Index] = UserAPP1_u8dino_pattern[u8Index + pixel_height] | (*UserApp1_u8DinoBottomMask)[u8Index];
        if (UserAPP1_u8dino_pattern[u8Index + pixel_height] & (*UserApp1_u8DinoBottomMask)[u8Index]) {
          gotoState(STATE_CRASH_ANIMATION);
        }
      }
      else
        dino_pattern[u8Index] = (*UserApp1_u8DinoBottomMask)[u8Index];
    }

    LcdModifyCustomChar(DINO_BOTTOM_NUM, dino_pattern);

    /* Determines custom char pattern for top leftmost tile */
    for (u8 u8Index = 0; u8Index < 8; u8Index++)
    {
      if (((s8)(16 - pixel_height - u8Index) >= 0 && (pixel_height + u8Index) >= 9))
        dino_pattern[u8Index] = UserAPP1_u8dino_pattern[pixel_height + u8Index - 9];
      else
        dino_pattern[u8Index] = 0;
    }

    LcdModifyCustomChar(DINO_TOP_NUM, dino_pattern);

    UserApp1_u8MillisecondCount = U8_SUBFRAME_MILLISECONDS;
  } /* end of subframe */
} /* end UserApp1SM_Idle() */
     

void UserApp1SM_CheckMenu() {
  if (WasButtonPressed(BUTTON0)) {
    ButtonAcknowledge(BUTTON0);
    UserApp1_checkInputFunction = getButtonInput;
    gotoState(STATE_RUN_GAME);
  }
  if (WasButtonPressed(BUTTON1)) {
    ButtonAcknowledge(BUTTON1);
    UserApp1_checkInputFunction = getANTInput;
    gotoState(STATE_WAIT_ANT_READY);
  }
}

void UserApp1SM_CrashAnimation() {
  gotoState(STATE_CHECK_MENU);
}


void UserApp1SM_WaitANTReady() {
  if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_CONFIGURED) {
    gotoState(STATE_WAIT_ANT_OPEN);
  }
}

void UserApp1SM_WaitANTOpen() {
  if (AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    LedOff(YELLOW);
    LedOn(GREEN);

    gotoState(STATE_RUN_GAME);
  }
}


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                         */
/*--------------------------------------------------------------------------------------------------------------------*/
