#pragma once

/*
USE ONE OF THE FOLLOWING to setup how the console will compile & work
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
 USING_CONSOLE_OPENGL - se compileaza & utilizeaza cod & print in OPENGL
 USING_CONSOLE_WINDOWS - se compileaza parte de cod pt print/use in WINDOWS (MFC STUFF)
  WINDOWS ONLY
    add in <<programName>>.rc2 -> #include "..\..\!utilClasses\console.rc" 
    carefull, idiot microsoft code needs another \n char in <<programName>>.rc2, so include line has a follow-up line or something - makes no sense

 If none is selected, it will just print to file everything

  WARNINGS
 -if u dont process commands, console might stop functioning until you do
 -if the console width is too small, it will set line length to 80

neimplementat
 USING_CONSOLE_LINUX
 USING_CONSOLE_APPLE
*/



#ifndef USING_CONSOLE
  #define USING_CONSOLE
#endif
/// should be fine to set bigger buffer
#define CON_MAX_LINES 300

/// chainlist with all lines
class consoleData: public chainData {
public:
  string line;
};
/// chainlist with all registered commands (funcs)
class conRegCom: public chainData {
public:
  string name;
  short nrParams;
  void (*func)();
  void (*func1p)(string8 &p1);
  void (*func2p)(string8 &p1, string8 &p2);
  void (*func3p)(string8 &p1, string8 &p2, string8 &p3);
  void (*func4p)(string8 &p1, string8 &p2, string8 &p3, string8 &p4);
};
///chainlist with all registered vars
class conRegVar: public chainData {
public:
  void *var;            // pointer to the actual var
  string8 name;          // var/class name
  short type;           // 0= class; 1= var; 2= string
  short size;           // size in bytes
  bool sign;            // is it signed
  bool pointer;         // is it a pointer
  chainList vars;       // if it's a class, it's vars
};

#ifdef USING_CONSOLE_WINDOWS    // WINDOWS DEFINITION & constructor/destructor

#ifndef IDD_CONSOLE
  #define IDD_CONSOLE                     990
#endif
#ifndef IDC_CONSOLE1
  #define IDC_CONSOLE1                    30990
#endif
#ifndef IDC_CONSOLE2
  #define IDC_CONSOLE2                    30991
#endif


class Console : public CDialog {
public:
  Console(CWnd* pParent = NULL);
  virtual ~Console();
  DECLARE_DYNAMIC(Console)

#else               // NORMAL DEFINITION & constructor/destructor
class Console {
public:
  Console();                                // load background/font then use init
  Console(short w, short h, short posX= 0, short posY= 0, char *fontFile= null, char *backFile= null);    // constructor, all needed vars
  ~Console();                               // destructor

#endif


private:
  int savedLines;       // nr of lines saved (lines- savedLines= how many lines it has to save @ program end)
  int lines;            // nr of printed lines in all console activity

  int scroll;           // use scrollUp() + scrollDown()

  short x0, y0;         // console origin position
  short height, width;  // console size in pixels
  short heightInChars;  // console height in font chars
  short widthInChars;   // console width in font chars

  short cursorPos;      // cursor position in command line (x axis)
  string8 command;       // command text that is waiting to be processed

  void clearBuffers();  // wipes command & console buffer (CON_MAX_LINES= 300 usually)
  void save();          // saves data strings to console.txt

/// register stuff - private tool funcs
  int getNrParentClasses(const string8 &name);      // returns nr of parent classes in string name ( parent.parent.parent.classOrVar = 3 parents)
  chainList *getParentClass(const string8 &name);   // returns pointer to parent class that is found in 'name'
  string8 getParentClassName(const string8 &name, int nr); // returns parent class name number nr in given string name ( name1.name2.name3.classOrVar )
  string8 getVarName(const string8 &name);          // returns var name, ignores parent classes if any ( parent.parent._varOrClassName_ )

/// default command help/tool/funcs - internal console stuff
  //print all win messages!!!!!!!!!!!!!!!!! MUST BE IMPLEMENTED


public:
  chainList data;         // string matrix with all lines (300)
  chainList regCommands;  // list with all registered commands
  chainList regVars;      // list with all registered vars/classes

  string statusText;      // 1 line above command line

  bool show;              // show console or not
  bool commandWaiting;    // if there is a command waiting flag
  bool dontSave;          // it can be forced not to write the buffers to disk

/// initialisation & setup & resetup
  void init(short w= 800, short h= 300, short posX= 0, short posY= 300);  // constructor basically can be used as a realloc (load background/font first)

  void delData();                     // destructor, dont use to clear buffers, it wipes everything

  void resize(short w, short h, short posX= 0, short posY= 0);    // resizes the console
  void setPos(short x, short y);      // sets position
  void setSize(short w, short h);     // sets size
  
/// command line funcs
  void in(char);                  // send char to console command line (MAIN INPUT FUNC)
  bool getCommand(string8 &text); // if there's a command waiting to be processed, it is put into "text" string
  void processCommand();          // process command, then call this as the last procesor
                                  //-case is irrelevant
                                  //-will print unknown command if command is still not recognized
                                  //

  char *getLine(short nr);      //returns pointer to selected line nr

/// scroll
  void scrollUp(short n= 3);
  void scrollDown(short n= 3);

// register commands funcs
  void regCommand(string8 &name, void (*func)());                          // command with no param's
  void regCommand1p(string8 &name, void (*func)(string8 &p1));                    // command with 1 param's
  void regCommand2p(string8 &name, void (*func)(string8 &p1, string8 &p2));              // command with 2 param's
  void regCommand3p(string8 &name, void (*func)(string8 &p1, string8 &p2, string8 &p3));        // command with 3 param's
  void regCommand4p(string8 &name, void (*func)(string8 &p1, string8 &p2, string8 &p3, string8 &p4));  // command with 4 param's

// register vars/classes 
  void regClass(const string8 &name);                // register a class name (parent+name in 'name')
  void regVar(void *var, const string8 &name, short size= 4, bool sig= true, bool pointer= false);	// reg a var: size is in bytes, sig= signed or not, pointer= if it is a pointer
  void regString(string8 *var, const string8 &name);  // register a string (class+name in 'name')
  
// print to console funcs
///----------------------
  void print(const char *text);
  void printf(const char *text, ...); // printf is limited to 511 characters (buffer[512])

  
///------------
// OPENGL Stuff
///------------
#ifdef USING_CONSOLE_OPENGL
private:
  string texFile, fontFile; // texture + font file names
  Print font;               // font
  Texture texture;          // background texture
  bool textureLoaded;			  // Texture loaded?

public:
  bool loadBackground(char *fileName);  // loads a background texture from file
  bool loadFont(char *fileName);        // load font from file
  bool loadTextures();          // loads textures (again if not loaded)
  void deleteTexture();         // deletes all textures loaded
//main draw thing
  void draw();                  // updates & draws consola (MAIN FUNC USED FOR A INF LOOP)
#endif


///-------------------
// WINDOWS (MFC) Stuff
///-------------------
#ifdef USING_CONSOLE_WINDOWS

  void setData();               // updates window text with console data text

// Dialog Data
  enum { IDD = IDD_CONSOLE };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  virtual BOOL Console::OnInitDialog();
  virtual void OnOK();								// command process, default action disabled
  virtual void OnCancel();
  DECLARE_MESSAGE_MAP()
#endif
};


