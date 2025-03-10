/*!*********************************************************************************************************************
@file dino_game.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this dino_game as a template:
 1. Copy both dino_game.c and dino_game.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "dino_game" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "DinoGame" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "DINO_GAME" with "YOUR_NEW_TASK_NAME"
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
- void DinoGameInitialize(void)
- void DinoGameRunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>DinoGame"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32DinoGameFlags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */

/* ANT Radio */
extern volatile G_u32AntRadioANTInfo;


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "DinoGame_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type DinoGame_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 DinoGame_u32Timeout;                           /*!< @brief Timeout counter used across states */
static u8 DinoGame_u8cactusBitmaps[12][8] = {{}, {}, {}, {}, {}, {}, CACTUS_PATTERN, {}, {}, {}, {}, {}};
static u8 DinoGame_u8dino_pattern[8] = DINO_PATTERN;
static u8 DinoGame_u8MillisecondCount = 0;
static u8 DinoGame_u8CactusPositions[21];
static u8 DinoGame_u8SubframeCount = 0;
static s16 DinoGame_s16DinoHeight = 0x0000;
static s16 DinoGame_s16DinoVelocity = 0;
static u8 (*DinoGame_u8DinoBottomMask)[8] = DinoGame_u8cactusBitmaps + 11;
static u8 (*DinoGame_u8DinoTopMask)[8] = DinoGame_u8cactusBitmaps + 11;
static u8 DinoGame_u8FramesToNextCactus = 5;

static u32 DinoGame_u32LSFRValue;

bool (*DinoGame_checkInputFunction)();

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
} typedef State_t;

State_t currentState = STATE_INIT;

void enterInit(State_t prevState) {}

void enterRunGame(State_t prevState) {
  intializeLinearFeedbackShiftRegister();
  
  LcdClearChars(LINE1_START_ADDR, 20);
  LcdClearChars(LINE2_START_ADDR, 20);
  LcdPutChar(LINE1_START_ADDR, DINO_TOP_NUM);
  LcdPutChar(LINE2_START_ADDR, DINO_BOTTOM_NUM);

  DinoGame_u8MillisecondCount = 0;
  for (u8 u8Index = 0; u8Index < 20; u8Index++)
    DinoGame_u8CactusPositions[u8Index] = ' ';
  DinoGame_u8CactusPositions[20] = '\0';  
  DinoGame_u8SubframeCount = 0;
  DinoGame_s16DinoHeight = 0x0000;
  DinoGame_s16DinoVelocity = 0;
  DinoGame_u8DinoBottomMask = DinoGame_u8cactusBitmaps + 11;
  DinoGame_u8DinoTopMask = DinoGame_u8cactusBitmaps + 11;
  DinoGame_u8FramesToNextCactus = 5;
}

void enterCheckMenu(State_t prevState) {
  LcdClearChars(LINE1_START_ADDR, 20);
  LcdClearChars(LINE2_START_ADDR, 20);
  LcdMessage(LINE1_START_ADDR, "Press BUTTON 0 to");
  LcdMessage(LINE2_START_ADDR, "begin");
}

void enterCrashAnimation(State_t prevState) {}

void enterWaitANTReady(State_t prevState) {
  AntRadioInitializeANT();
}

void (*stateTransition[])(State_t) = {
  enterInit,
  enterRunGame,
  enterCheckMenu,
  enterCrashAnimation,
  enterWaitANTReady,
};

void (*stateFunctionArray[])(void) = {
  DinoGameInitialize,
  DinoGameSM_RunGame,
  DinoGameSM_CheckMenu,
  DinoGameSM_CrashAnimation,
  DinoGameSM_WaitANTReady,
};

void gotoState(State_t targetState) {
  stateTransition[targetState](currentState);
  currentState = targetState;
  DinoGame_pfStateMachine = stateFunctionArray[currentState];
}




void shiftCactuses() {
  for(u8 u8Index = 0; u8Index < 19; u8Index++) {
    DinoGame_u8CactusPositions[u8Index] = DinoGame_u8CactusPositions[u8Index + 1];
  }

  if (--DinoGame_u8FramesToNextCactus == 1) {
    DinoGame_u8CactusPositions[19] = CACTUS_FRONT_NUM;
  } else if (DinoGame_u8FramesToNextCactus == 0)
  {
    DinoGame_u8CactusPositions[19] = CACTUS_BACK_NUM;
    DinoGame_u8FramesToNextCactus = 2 + linearFeedbackShiftRegister() + linearFeedbackShiftRegister();
  } else {
    DinoGame_u8CactusPositions[19] = ' ';
  }

  LcdMessage(LINE2_START_ADDR + 1, DinoGame_u8CactusPositions + 1);
}

void updateCactusCustomCharacters() {
  u8 (*u8cactusFrontBitPattern)[8] = DinoGame_u8cactusBitmaps + 5 - DinoGame_u8SubframeCount;
  LcdModifyCustomChar(CACTUS_FRONT_NUM, *u8cactusFrontBitPattern);
  if (DinoGame_u8CactusPositions[0] == CACTUS_FRONT_NUM) DinoGame_u8DinoBottomMask = u8cactusFrontBitPattern;
  u8 (*u8cactusBackBitPattern)[8] = DinoGame_u8cactusBitmaps + 11 - DinoGame_u8SubframeCount;
  LcdModifyCustomChar(CACTUS_BACK_NUM, *u8cactusBackBitPattern);
  if (DinoGame_u8CactusPositions[0] == CACTUS_BACK_NUM) DinoGame_u8DinoBottomMask = u8cactusBackBitPattern;
}


bool getButtonInput() {
  bool buttonPressed = WasButtonPressed(BUTTON0);
  if (buttonPressed) {
    ButtonAcknowledge(BUTTON0);
  }
  return buttonPressed;
}

bool getANTInput() {
  bool ANTJumpValue = G_u32AntRadioANTInfo & 0x2;
  G_u32AntRadioANTInfo &= ~0x2;
  return ANTJumpValue;
}

int linearFeedbackShiftRegister() {
  bool newBit = (DinoGame_u32LSFRValue ^ (DinoGame_u32LSFRValue >> 1) ^ (DinoGame_u32LSFRValue >> 22) ^ (DinoGame_u32LSFRValue >> 31)) & 1;
  DinoGame_u32LSFRValue >>= 1;
  DinoGame_u32LSFRValue |= (u32)newBit << 31;

  return newBit;
}

int intializeLinearFeedbackShiftRegister() {
  DinoGame_u32LSFRValue = G_u32SystemTime1ms;
}


/*!--------------------------------------------------------------------------------------------------------------------
@fn void DinoGameInitialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void DinoGameInitialize(void)
{
  LcdCommand(LCD_CLEAR_CMD);
  LcdCommand(LCD_FUNCTION_CMD);     //required for custom characters to function

  for (u8 u8BitmapN = 5; u8BitmapN != 0; u8BitmapN--)
    for (u8 u8InnerPos = 0; u8InnerPos < 8; u8InnerPos++)
      DinoGame_u8cactusBitmaps[u8BitmapN][u8InnerPos] = DinoGame_u8cactusBitmaps[u8BitmapN + 1][u8InnerPos] >> 1;

  for (u8 u8BitmapN = 7; u8BitmapN <= 11; u8BitmapN++)
    for (u8 u8InnerPos = 0; u8InnerPos < 8; u8InnerPos++)
      DinoGame_u8cactusBitmaps[u8BitmapN][u8InnerPos] = (DinoGame_u8cactusBitmaps[u8BitmapN - 1][u8InnerPos] << 1) & 0x1F;


  /* If good initialization, set state to Idle */
  if( 1 )
  {
    gotoState(STATE_CHECK_MENU);
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    DinoGame_pfStateMachine = DinoGameSM_Error;
  }

} /* end DinoGameInitialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void DinoGameRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void DinoGameRunActiveState(void)
{
  DinoGame_pfStateMachine();

} /* end DinoGameRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void DinoGameSM_RunGame(void)
{
  if(DinoGame_u8MillisecondCount-- == 0)
  {
    if (DinoGame_u8SubframeCount-- == 0)
    {
      shiftCactuses();
      DinoGame_u8SubframeCount = U8_FRAME_SUBFRAMES;
    }

    updateCactusCustomCharacters();


    /* Updates dino height and sees if its reached the ground*/
    DinoGame_s16DinoHeight += DinoGame_s16DinoVelocity;
    DinoGame_s16DinoVelocity -= 50;
    if (DinoGame_s16DinoHeight <= 0)
    {
      DinoGame_s16DinoHeight = 0;
      DinoGame_s16DinoVelocity = 0;

      /* Dino can jump if its on the ground */
      if (DinoGame_checkInputFunction())
      {
        DinoGame_s16DinoVelocity = 500;
      }
    }

    u8 dino_pattern[8] = {};

    /* Dino height in pixels is dino height divided by 256 */
    u8 pixel_height = ((u16)DinoGame_s16DinoHeight) >> 8;
    

    /* Determines custom char pattern for bottom leftmost tile */
    for (u8 u8Index = 0; u8Index < 8; u8Index++)
    {
      if ((s8)(7 - pixel_height - u8Index) >= 0) {
        dino_pattern[u8Index] = DinoGame_u8dino_pattern[u8Index + pixel_height] | (*DinoGame_u8DinoBottomMask)[u8Index];
        if (DinoGame_u8dino_pattern[u8Index + pixel_height] & (*DinoGame_u8DinoBottomMask)[u8Index]) {
          gotoState(STATE_CRASH_ANIMATION);
        }
      }
      else
        dino_pattern[u8Index] = (*DinoGame_u8DinoBottomMask)[u8Index];
    }

    LcdModifyCustomChar(DINO_BOTTOM_NUM, dino_pattern);

    /* Determines custom char pattern for top leftmost tile */
    for (u8 u8Index = 0; u8Index < 8; u8Index++)
    {
      if (((s8)(16 - pixel_height - u8Index) >= 0 && (pixel_height + u8Index) >= 9))
        dino_pattern[u8Index] = DinoGame_u8dino_pattern[pixel_height + u8Index - 9];
      else
        dino_pattern[u8Index] = 0;
    }

    LcdModifyCustomChar(DINO_TOP_NUM, dino_pattern);

    DinoGame_u8MillisecondCount = U8_SUBFRAME_MILLISECONDS;
  } /* end of subframe */
} /* end DinoGameSM_Idle() */
     

void DinoGameSM_CheckMenu() {
  if (WasButtonPressed(BUTTON0)) {
    ButtonAcknowledge(BUTTON0);
    DinoGame_checkInputFunction = getButtonInput;
    gotoState(STATE_RUN_GAME);
  }
  if (WasButtonPressed(BUTTON1)) {
    ButtonAcknowledge(BUTTON1);
    DinoGame_checkInputFunction = getANTInput;
    gotoState(STATE_WAIT_ANT_READY);
  }
}

void DinoGameSM_CrashAnimation() {
  gotoState(STATE_CHECK_MENU);
}


void DinoGameSM_WaitANTReady() {
  if (G_u32AntRadioANTInfo & 0x1) {
    gotoState(STATE_RUN_GAME);
  }
}


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void DinoGameSM_Error(void)          
{
  
} /* end DinoGameSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                         */
/*--------------------------------------------------------------------------------------------------------------------*/
