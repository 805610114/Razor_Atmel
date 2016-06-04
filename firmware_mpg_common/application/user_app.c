/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserAppFlags;                       /* Global state flags */
volatile u8 u8CongratulationIndex=0;                     


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[];                     /* From debug.c */

u8 au8MyName[] = "A3.JunFengZhang";
u8 au8MyName2[] = "junfengzhang";

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */

static u8 au8UserInputBuffer[];

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserAppInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserAppInitialize(void)
{
  
  /*Request2:Choose the LCD backlight.*/
  LedPWM(LCD_RED, LED_PWM_0);
  LedPWM(LCD_GREEN, LED_PWM_30);
  LedPWM(LCD_BLUE, LED_PWM_70);
  
  /*Request1:Show my name in the line1.*/
  LCDMessage(LINE1_START_ADDR, au8MyName);
  LCDClearChars(LINE1_START_ADDR + 15, 7);
  
  for(u8 i = 0; i < USER_INPUT_BUFFER_SIZE; i++)
  {
    au8UserInputBuffer[i] = 0;
  }
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp_StateMachine = UserAppSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp_StateMachine = UserAppSM_FailedInit;
  }

} /* end UserAppInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserAppRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserAppRunActiveState(void)
{
  UserApp_StateMachine();

} /* end UserAppRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserAppSM_Idle(void)
{
  static u8 u8InputTime =0;
  static u8 u8InputCount=0;
  static u8 u8CharCount;
  static u32 u32InputNumber =0;
  static u8 u8CurrentBuffer[12];
  static u8 u8BufferCompareCount=0;
  static u32 u32NameCount=0;
  
  u8 u8String[] = "\n\rTotal characters received:";
  u8 u8String2[] = "\n\rCharacter count cleared!";
  u8 u8String3[] = "\n\r";
  u8 u8String4[] = "\n\rJunFengZhang:";
  
  /*Request3&4:When I use the keyboard input, real-time display 
               on the LCD screen*/
  if(u8InputTime==10)
  { 
    u8CharCount = DebugScanf(au8UserInputBuffer);
    au8UserInputBuffer[u8CharCount] = '\0';
    if(u8CharCount)
    { 
      /*Request5:Keep track of the total number of characters 
               that have been received*/
      u32InputNumber++;
   
      /*Request9:Monitor each character as it arrives
          and compare it to the letters of your name.*/
      if(((au8MyName2[u8BufferCompareCount]-au8UserInputBuffer[0])==0)||((au8MyName2[u8BufferCompareCount]-au8UserInputBuffer[0])==32))
      {
        if(u8BufferCompareCount==0)
        {
          for(u8 j=0;j<12;j++)
          {
            u8CurrentBuffer[j]='\0';
          }
        }
        u8CurrentBuffer[u8BufferCompareCount]=au8UserInputBuffer[0];
        u8BufferCompareCount++;
      }
      if(u8BufferCompareCount==12)
      {
        u8BufferCompareCount=0;
        /*Request10:Count the number of my name*/
        u32NameCount++;
        u8CongratulationIndex=1;
        DebugPrintf(u8String4);
        DebugPrintNumber(u32NameCount);
        DebugPrintf(u8String3);
      }
 

    
      /*Every input 20 characters displayed from the very beginning.*/
      if(u8InputCount==20)
      {
        LCDClearChars(LINE2_START_ADDR, 20);
        u8InputCount=0;    
      }
      
      LCDMessage(LINE2_START_ADDR+u8InputCount,au8UserInputBuffer); 
      u8InputCount++; 
    }   
    u8InputTime=0;
  }
  u8InputTime++;
  
  /*Request6:Clear line of text*/
  if( WasButtonPressed(BUTTON0) )
  {
    /* Be sure to acknowledge the button press */
    ButtonAcknowledge(BUTTON0);
    
    /*Clear the characters on the LCD screen*/
    LCDClearChars(LINE2_START_ADDR, 20);
    u8InputCount=0;  
  }
  
  /*Request7:Print the total number of characters.*/
  if( WasButtonPressed(BUTTON1) )
  {
    /* Be sure to acknowledge the button press */
    ButtonAcknowledge(BUTTON1);
    
    DebugPrintf(u8String3);
    DebugPrintf(u8String);
    DebugPrintNumber(u32InputNumber);
    
      
  }
  
  /*Request8:Clears the total character count and reports a message .*/
  if( WasButtonPressed(BUTTON2) )
  {
    /* Be sure to acknowledge the button press */
    ButtonAcknowledge(BUTTON2);
    
    DebugPrintf(u8String3);
    DebugPrintf(u8String2);
    u32InputNumber=0;     
  }
  
  /*Request10: Prints the current letter buffer storing my name.*/
  if( WasButtonPressed(BUTTON3) )
  {
    /* Be sure to acknowledge the button press */
    ButtonAcknowledge(BUTTON3);
    
    DebugPrintf(u8String3);
    DebugPrintf(u8CurrentBuffer);    
    
  }
} /* end UserAppSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{
  
} /* end UserAppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
