/*!*********************************************************************************************************************
@file ant_radio.h                                                                
@brief Header file for ant_radio.c
**********************************************************************************************************************/

#ifndef __ANT_RADIO_H
#define __ANT_RADIO_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void AntRadio_IntializeANT(void);

/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void AntRadioInitialize(void);
void AntRadioRunActiveState(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void AntRadioSM_Idle(void);    
static void AntRadioSM_Error(void);         
static void AntRadioSM_WaitAntReady(void);
static void AntRadioSM_WaitChannelOpen(void);
static void AntRadioSM_ChannelOpen(void);
static void AntRadioSM_DisplayClose(void);
static void AntRadioSM_WaitChannelClose(void);
static void AntRadioSM_ChannelAwaitConnection(void);


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
#define U8_ANT_CHANNEL_USERAPP (u8)ANT_CHANNEL_0 /* Channel 0 – 7 */
#define U8_ANT_DEVICE_LO_USERAPP (u8)0x22 /* Low byte of two-byte Device # */
#define U8_ANT_DEVICE_HI_USERAPP (u8)0x16 /* High byte of two-byte Device # */
#define U8_ANT_DEVICE_TYPE_USERAPP (u8)1 /* 1 – 255 */
#define U8_ANT_TRANSMISSION_TYPE_USERAPP (u8)1 /* 1-127 (MSB is pairing bit) */
#define U8_ANT_CHANNEL_PERIOD_LO_USERAPP (u8)0x00 /* Low byte of two-byte channel period */
#define U8_ANT_CHANNEL_PERIOD_HI_USERAPP (u8)0x10 /* High byte of two-byte channel period */
#define U8_ANT_FREQUENCY_USERAPP (u8)50 /* 2400MHz + this number 0 – 99 */
#define U8_ANT_TX_POWER_USERAPP RADIO_TX_POWER_4DBM /* RADIO_TX_POWER_xxx */

#define U32_TIMEOUT_OPEN_CHANNEL 5000
#define U32_TIMEOUT_CLOSE_CHANNEL 5000
#define U32_TIMEOUT_DISPLAY_FAIL 2000

#define INTIALIZE_ANT_MESSAGE_1             "Waiting for ANT to "
#define INTIALIZE_ANT_MESSAGE_2             "configure...       "
#define UNABLE_TO_OPEN_ANT_MESSAGE_1        "Unable to open ANT "
#define UNABLE_TO_OPEN_ANT_MESSAGE_2        "channel            "
#define WAIT_CHANNEL_OPEN_MESSAGE_1         "Searching for ANT  "
#define WAIT_CHANNEL_OPEN_MESSAGE_2         "signal...          "
#define SEARCH_TIMEOUT_ANT_MESSAGE_1        "Search timeout...  "
#define SEARCH_TIMEOUT_ANT_MESSAGE_2        "Unable to connect. "


#endif /* __ANT_RADIO_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
