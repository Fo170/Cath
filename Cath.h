/*Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.

IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Simple cooperative multitask helper for Arduino

See my video on YouTube for more explanation https://youtu.be/aGwHYCcQ3Io
https://github.com/Phildem/Cath

................................................................................................................
Release history
................................................................................................................
Version Date        Author    Comment
1.0     30/08/2021  Phildem   First version tested ok
1.1     05/09/2021  Phildem   Misc fixes, better comments and presentation
1.2     06/09/2021  Phildem   Remove unnecessary Cath:: in Cath class definition, (Warning removed)
1.3     08/09/2021  Phildem   Misc comments/name fixes, Memory optimised, __CathOpt_SmallCounter__ option added
*/


//____________________________________________________________________________________________________________
// Start of Cath definition__________________________________________________________________________________
 
//#define kMaxCathTask    9         // Number max of task instance MUST BE >= at task instancied
//#define __CathOpt_SmallCounter__  // Comment this line to allow 32 bit delay, if not period max is 65536 ms

#ifdef __CathOpt_SmallCounter__
typedef uint16_t CathCnt;
#else
typedef uint32_t CathCnt;
#endif

class Cath
{

  public:

// Derived class MUST implement these 2 methodes
  virtual void          SetUp() = 0;                 // Called at setup
  virtual void          Loop()  = 0;                 // Called periodically

  CathCnt               m_CurCounter;               // Curent number of ms before next Loop call
  CathCnt               m_LoopDelay;                // Default period of Loop call in ms

  static uint8_t        S_NbTask;                   // Actual number of task instances
  static Cath*          S_CathTasks[kMaxCathTask];  // Array of task object pointers
  static uint8_t        S_LastMilli;                // Used to call every ms (a byte is enought to detect change)

  //..............................................................
  // Must be called in task constructors to register in the task list
  // WARNING : think to set kMaxCathTask as needed
  // Task :   Ptr on the derivated task to register
  // Period : Period of loop call in ms WARNING do not pass 0!
  // Offset : Delay of first call in ms (1 def) WARNING do not pass 0!
  static void S_Register(Cath* Task,CathCnt Period,CathCnt Offset=1)
  {
	if(Period < 1) Period = 1;
	if(Offset < 1) Offset = 1;
	
    Task->m_LoopDelay = Period;
    Task->m_CurCounter = Offset;
    Cath::S_CathTasks[Cath::S_NbTask++] = Task;
  }

  //..............................................................
  // Must be called once in Arduino setup to call all the task setups
  static void S_SetUp()
  {
    for(int T = 0; T < S_NbTask; T++)
      Cath::S_CathTasks[T]->SetUp();
  }

   //..............................................................
  // Must be called once in Arduino Loop to call all the task loop if needed
  static void S_Loop()
  {
    uint8_t CurMilli = millis();
    if(CurMilli != S_LastMilli) 
	{
      S_LastMilli = CurMilli;
      for(int T = 0; T < S_NbTask; T++) 
	  {
		if( Cath::S_CathTasks[T]->m_CurCounter-- == 0 ) 
		{
          Cath::S_CathTasks[T]->m_CurCounter = Cath::S_CathTasks[T]->m_LoopDelay;
          Cath::S_CathTasks[T]->Loop();
        } 
	  }
        
    }
  }

};

//Cath static var def 
//(Note set to 0 for code clarity but done by default anyway because they are static)
uint8_t       Cath::S_NbTask = 0;
Cath*         Cath::S_CathTasks[kMaxCathTask];
uint8_t       Cath::S_LastMilli = 0;    

#define  _1_MINUTES_        60000     // 60000ms
#define  _10_SECONDES_      10000     // 10000ms
#define  _1_SECONDES_        1000     //  1000ms
#define  _100ms_              100     //   100ms
#define  _10ms_                10     //    10ms
#define  _1ms_                  1     //     1ms   

// End of Cath definition