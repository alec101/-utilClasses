#pragma once

/*
USAGE ==========================================================================
  * just use error.simple() to print an error message * - fastest way

  useConsoleFlag:					use the ConsoleClass, it wont create any window
  useWindowsFlag:					use OS windows (MessageBox/ XBlaBla/ etc)
  both flags are false (DEFAULT): print to OS console / terminal / watever - just printf something
  
  using window(...) will force a OS window only msg
  using console(...) will force a ConsoleClass only print
  using terminal(...) will force a OS terminal/ console / watever printf only
 ===============================================================================
*/



//OPEN GL SPECIFIC ERRORZ:
//glErrNr-GL_INVALID_ENUM+GLENUM


class ErrorHandling {
  #ifdef OS_LINUX
  void messageBox(string8 text);   // linux specific messageBox window
  #endif /// OS_LINUX
    
public:
// USAGE FLAGS
  bool useConsoleFlag;    // use the ConsoleClass (only)
  bool useWindowsFlag;    // use OS windows (MessageBox/ XBlaBla/ etc)

// main call funcs
  void simple(string8 txt, bool exit= false, void (*exitFunc)(void)= null);  // exitFunc: func to call before exit program
  
// these funcs will force a type of window/ print to something
  void window(string8 txt, bool exit= false, void (*exitFunc)(void)= null);
  void console(string8 txt, bool exit= false, void (*exitFunc)(void)= null);
  void terminal(string8 txt, bool exit= false, void (*exitFunc)(void)= null);
  
  #ifdef USING_DIRECTINPUT
  void dinput(long nr);   // direct input error nr as text (msgbox etc)
  #endif
  
  ErrorHandling();
  ~ErrorHandling();
  void delData();
};

extern ErrorHandling error; // only 1 global class




