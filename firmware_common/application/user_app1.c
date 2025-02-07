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
static u8 UserApp1_u8cactusBitmaps[11][8] = {{}, {}, {}, {}, {}, CACTUS_PATTERN, {}, {}, {}, {}, {}};
static u8 UserAPP1_u8dino_pattern[8] = DINO_PATTERN;


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

  for (u8 u8BitmapN = 4; u8BitmapN != 0xFF; u8BitmapN--)
    for (u8 u8InnerPos = 0; u8InnerPos < 8; u8InnerPos++)
      UserApp1_u8cactusBitmaps[u8BitmapN][u8InnerPos] = UserApp1_u8cactusBitmaps[u8BitmapN + 1][u8InnerPos] >> 1;

  for (u8 u8BitmapN = 6; u8BitmapN <= 10; u8BitmapN++)
    for (u8 u8InnerPos = 0; u8InnerPos < 8; u8InnerPos++)
      UserApp1_u8cactusBitmaps[u8BitmapN][u8InnerPos] = (UserApp1_u8cactusBitmaps[u8BitmapN - 1][u8InnerPos] << 1) & 0x1F;


  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_MenuSetup;
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
  static u8 u8millisecondCount = 0;
  static u32 u32cactusPositions = 0x08801FFF;
  static u8 u8subframeCount = 0;
  static s16 s16DinoHeight = 0x0000;
  static s16 s16DinoVelocity = 0;
  static u8 (*u8DinoBottomMask)[8] = UserApp1_u8cactusBitmaps + 10;
  static u8 (*u8DinoTopMask)[8] = UserApp1_u8cactusBitmaps + 10;
  static u8 u8framesToNextCactus = 5;


  if(u8millisecondCount-- == 0)
  {
    //DebugPrintf("1");

    if (u8subframeCount-- == 0)
    {
      //DebugPrintf("2");

      /* Removes the backs of the cactuses, which are now empty characters */
      for (u8 u8Index = 1; u8Index < 20; u8Index++)
        if (u32cactusPositions & (0x80000000 >> u8Index))
          LcdPutChar(LINE2_START_ADDR | u8Index, ' ');

      /* Shifts tracked cactus locations one to the left */
      u32cactusPositions = u32cactusPositions << 1;
      /* Changes the cactus back to a full cactus character */
      LcdModifyCustomChar(CACTUS_BACK_NUM, UserApp1_u8cactusBitmaps[5]);
      
      /* Replaces old cactus fronts with cactus backs */
      for (u8 u8Index = 1; u8Index < 20; u8Index++)
        if (u32cactusPositions & (0x80000000 >> u8Index))
          LcdPutChar(LINE2_START_ADDR | u8Index, CACTUS_BACK_NUM);

      /* Changes the cactus front to an empty character*/
      LcdModifyCustomChar(CACTUS_FRONT_NUM, ' ');

      /* Places cactus fronts in front of tracked locations */
      for (u8 u8Index = 2; u8Index < 20; u8Index++)
        if (u32cactusPositions & (0x80000000 >> u8Index))
          LcdPutChar(LINE2_START_ADDR | u8Index - 1, CACTUS_FRONT_NUM);

      if (u32cactusPositions & 0x40000000)
      {
        u8DinoBottomMask = UserApp1_u8cactusBitmaps;
      }
      
      /*
      if (--u8framesToNextCactus == 0)
      {
        u32cactusPositions |= 0x800;
        u8framesToNextCactus = 4;
      }
      */
      
      



      u8subframeCount = U8_FRAME_SUBFRAMES;
    }
    else
    {
      LcdModifyCustomChar(CACTUS_FRONT_NUM, UserApp1_u8cactusBitmaps[4 - u8subframeCount]);
      LcdModifyCustomChar(CACTUS_BACK_NUM, UserApp1_u8cactusBitmaps[10 - u8subframeCount]);
    }

    /* Updates dino height and sees if its reached the ground*/
    s16DinoHeight += s16DinoVelocity;
    s16DinoVelocity -= 50;
    if (s16DinoHeight <= 0)
    {
      s16DinoHeight = 0;
      s16DinoVelocity = 0;

      /* Dino can jump if its on the ground */
      if (WasButtonPressed(BUTTON0))
      {
        ButtonAcknowledge(BUTTON0);
        s16DinoVelocity = 500;
      }
    }

    u8 dino_pattern[8] = {};

    /* Dino height in pixels is dino height divided by 256 */
    u8 pixel_height = ((u16)s16DinoHeight) >> 8;

    /* Determines custom char pattern for bottom leftmost tile */
    for (u8 u8Index = 0; u8Index < 8; u8Index++)
    {
      if ((s8)(7 - pixel_height - u8Index) >= 0) {
        dino_pattern[u8Index] = UserAPP1_u8dino_pattern[u8Index + pixel_height] | (*u8DinoBottomMask)[u8Index];
        if (UserAPP1_u8dino_pattern[u8Index + pixel_height] & (*u8DinoBottomMask)[u8Index]) {
          UserApp1_pfStateMachine = UserApp1SM_MenuSetup;
          u32cactusPositions = 0x08801100;
        }
      }
      else
        dino_pattern[u8Index] = (*u8DinoBottomMask)[u8Index];
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

    /* Updates cactus in the bottom leftmost tile */
    if (u8DinoBottomMask != UserApp1_u8cactusBitmaps + 10)
      u8DinoBottomMask++;

    




    u8millisecondCount = U8_SUBFRAME_MILLISECONDS;
  } /* end of subframe */
} /* end UserApp1SM_Idle() */
     

static void UserApp1SM_MenuSetup() {
  LcdClearChars(LINE1_START_ADDR, 20);
  LcdClearChars(LINE2_START_ADDR, 20);
  LcdMessage(LINE1_START_ADDR, "Press BUTTON 0 to");
  LcdMessage(LINE2_START_ADDR, "begin");
  UserApp1_pfStateMachine = UserApp1SM_CheckMenu;
}

static void UserApp1SM_CheckMenu() {
  if(WasButtonPressed(BUTTON0)) {
    ButtonAcknowledge(BUTTON0);
    LcdClearChars(LINE1_START_ADDR, 20);
    LcdClearChars(LINE2_START_ADDR, 20);
    LcdPutChar(LINE1_START_ADDR, DINO_TOP_NUM);
    LcdPutChar(LINE2_START_ADDR, DINO_BOTTOM_NUM);

    UserApp1_pfStateMachine = UserApp1SM_RunGame;
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
