/*!*********************************************************************************************************************
@file dino_game.h                                                                
@brief Header file for dino_game

----------------------------------------------------------------------------------------------------------------------
To start a new task using this dino_game as a template:
1. Follow the instructions at the top of dino_game.c
2. Use ctrl-h to find and replace all instances of "dino_game" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "DinoGame" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "DINO_GAME" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

**********************************************************************************************************************/

#ifndef __DINO_GAME_H
#define __DINO_GAME_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/
enum { 
    STATE_INIT = 0,
    STATE_RUN_GAME,
    STATE_CHECK_MENU,
    STATE_CRASH_ANIMATION,
    STATE_WAIT_ANT_READY,
  } typedef State_t;


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void DinoGameInitialize(void);
void DinoGameRunActiveState(void);
int linearFeedbackShiftRegister(void);
int intializeLinearFeedbackShiftRegister(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void DinoGameSM_RunGame(void);
static void DinoGameSM_CheckMenu(void);
static void DinoGameSM_Error(void);         
static void DinoGameSM_CrashAnimation(void);
static void DinoGameSM_WaitANTReady(void);
static void DinoGameSM_WaitANTOpen(void);



/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
#define   U8_SUBFRAME_MILLISECONDS      (u8)200 /*How many milliseconds between each subframe*/
#define   U8_FRAME_SUBFRAMES            (u8)5   /*How many subframes in each frame*/

#define   DINO_TOP_NUM          (u8)1   /*Custom character number for the top of the dino (top leftmost tile)*/
#define   DINO_BOTTOM_NUM       (u8)2   /*Custom character number for the bottom of the dino (top rightmost tile)*/
#define   CACTUS_FRONT_NUM      (u8)3   /*Custom character number for the front of a cactus*/
#define   CACTUS_BACK_NUM       (u8)4   /*Custom character number for the back of a cactus*/

#define   CACTUS_PATTERN        {0x04, 0x05, 0x15, 0x15, 0x17, 0x1C, 0x04, 0x04}    /*Custom character pattern for the cactus*/
#define   DINO_PATTERN          {0x00, 0x03, 0x05, 0x17, 0x1E, 0x1F, 0x0E, 0x0A}    /*Custom character patterm for the dino*/

#define   MINIMUM_CACTUS_SPACES     2


#endif /* __DINO_GAME_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
