/*!*********************************************************************************************************************
@file dino_game.c                                                                
@brief Controls lcd screen and gets information from buttons or form ant_radio.c to run the dino game
------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- State_t - represents current state machine state for the game

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
extern volatile u32 G_u32AntRadioANTInfo;


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


static bool (*DinoGame_checkInputFunction)();
static State_t DinoGame_currentState = STATE_INIT;

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

void enterInit(State_t prevState) {}

void enterRunGame(State_t prevState) {
  intializeLinearFeedbackShiftRegister();
  
  LcdClearChars(LINE1_START_ADDR, 20);
  LcdClearChars(LINE2_START_ADDR, 20);
  LcdPutChar(LINE1_START_ADDR, DINO_TOP_NUM);
  LcdPutChar(LINE2_START_ADDR, DINO_BOTTOM_NUM);

  for (u8 u8Index = 0; u8Index < 20; u8Index++)
    DinoGame_u8CactusPositions[u8Index] = ' ';
  DinoGame_u8CactusPositions[20] = '\0';  
  DinoGame_u8FramesToNextCactus = 2;
  for (u8 u8Index = 0; u8Index < 10; u8Index++)
    shiftCactuses();
  DinoGame_u8MillisecondCount = 0;
  DinoGame_u8SubframeCount = 0;
  DinoGame_s16DinoHeight = 0x0000;
  DinoGame_s16DinoVelocity = 0;
  DinoGame_u8DinoBottomMask = DinoGame_u8cactusBitmaps + 11;
  DinoGame_u8DinoTopMask = DinoGame_u8cactusBitmaps + 11;
}

void enterCheckMenu(State_t prevState) {
  LcdClearChars(LINE1_START_ADDR, 20);
  LcdClearChars(LINE2_START_ADDR, 20);
  LcdMessage(LINE1_START_ADDR, "Press BUTTON 0 to");
  LcdMessage(LINE2_START_ADDR, "begin");
}

void enterCrashAnimation(State_t prevState) {}

void enterWaitANTReady(State_t prevState) {
  AntRadio_IntializeANT();
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
  stateTransition[targetState](DinoGame_currentState);
  DinoGame_currentState = targetState;
  DinoGame_pfStateMachine = stateFunctionArray[DinoGame_currentState];
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
    DinoGame_u8FramesToNextCactus = MINIMUM_CACTUS_SPACES + 1 + linearFeedbackShiftRegister() + linearFeedbackShiftRegister();
  } else {
    DinoGame_u8CactusPositions[19] = ' ';
  }
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
  // Repeatedly call to give similar starting values time to diverge
  for (u8 u8Index = 100; u8Index > 0; u8Index--) {
    linearFeedbackShiftRegister();
  }
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
      LcdMessage(LINE2_START_ADDR + 1, DinoGame_u8CactusPositions + 1);
      DinoGame_u8SubframeCount = U8_FRAME_SUBFRAMES;
    }

    updateCactusCustomCharacters();

    /* Dino jumps if it has reached the grouned*/
    if (DinoGame_checkInputFunction() && DinoGame_s16DinoHeight <= 0)
      DinoGame_s16DinoVelocity = 500;

    /* Updates dino height and sees if its reached the ground*/
    DinoGame_s16DinoHeight += DinoGame_s16DinoVelocity;
    DinoGame_s16DinoVelocity -= 50;
    if (DinoGame_s16DinoHeight <= 0)
    {
      DinoGame_s16DinoHeight = 0;
      DinoGame_s16DinoVelocity = 0;
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
