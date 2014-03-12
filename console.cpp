#include "precompiled.h"
//#include "c:/alec/dev/Calendatorizator/source/stdafx.h"

#include <stdio.h>
//#include <string8.h>
#include <stdarg.h>

#include "typeShortcuts.h"
#include "chainList.h""
#include "stringClass8.h"


#ifdef USING_CONSOLE_OPENGL
  #include "c:/alec/dev/!openGL/Texture.h"
  #include "c:/alec/dev/!opengl/print.h"
#endif



#include "console.h"

// Init / constructors / destructor
///--------------------------------


#ifdef USING_CONSOLE_WINDOWS
IMPLEMENT_DYNAMIC(Console, CDialog)

BEGIN_MESSAGE_MAP(Console, CDialog)
END_MESSAGE_MAP()

Console::Console(CWnd* pParent /*=NULL*/)
  : CDialog(Console::IDD, pParent) {
#else
Console::Console() {
#endif

/// backup current console.txt if it exists
  FILE *f1, *f2;
  if( f1= fopen("console.txt", "rb") ) {
    char p;
    f2= fopen("console.bak", "wb");
      while(fread(&p, 1, 1, f1))
        fwrite(&p, 1, 1, f2);

    fclose(f2);
    fclose(f1);

    f1= fopen("console.txt", "wb");     ///wipe console.txt
    fclose(f1);
  }

  init();
}

/// optional constructor, not used in MFC, just use normal constr+init (carefull to copy changes between constructors)
#ifndef USING_CONSOLE_WINDOWS
Console::Console(short w, short h, short posX, short posY, char *fontFile, char *backFile) {
/// backup current console.txt if it exists
  FILE *f1, *f2;
  if( f1= fopen("console.txt", "rb") ) {
    char p;
    f2= fopen("console.bak", "wb");
      while(fread(&p, 1, 1, f1))
        fwrite(&p, 1, 1, f2);

    fclose(f2);
    fclose(f1);

    f1=fopen("console.txt", "wb");			///wipe console.txt
    fclose(f1);
  }

#ifdef USING_CONSOLE_OPENGL
  if(fontFile)
    loadFont(fontFile);

  if(backFile)
    loadBackground(backFile);
#endif 
  init(w, h, posX, posY);
}
#endif

///teh real constructor
void Console::init(short w, short h, short posX, short posY) {
  width= w;
  height= h;
  x0= posX;
  y0= posY;

  scroll= 0;
  cursorPos= 0;

  savedLines= 0;
  lines= 0;

  show= false;
  commandWaiting= false;
  widthInChars= 80;                       /// in case opengl is not used, there should be a line width

#ifdef USING_CONSOLE_OPENGL
  heightInChars= font.cHei? (height/ font.cHei): 0;
  widthInChars= font.cWidFix? (width/ font.cWidFix): 0;   // fixed width fonts only, pls, no headaches
  textureLoaded= false;
#endif

  consoleData *p= null;

  data.delData();
  for(register short a= 0; a <CON_MAX_LINES; a++) {       /// creates text buffers, 300
    p= new consoleData;
    data.add(p);
  }

  clearBuffers();
}


Console::~Console() {
  save();
  delData();
}

void Console::delData() {
  data.delData();
#ifdef USING_CONSOLE_OPENGL
  font.delData();
  texture.delData();
#endif
}

void Console::clearBuffers() {
  command= "";

  consoleData *p= (consoleData*)data.first;
  for(register short a= 0; a< CON_MAX_LINES; a++) {
    p->line= "";
    p= (consoleData*)p->next;
  }
}

/// save to console.txt
void Console::save() {
  if(dontSave) return;
  if(!(lines- savedLines)) return;
  if(lines- savedLines> 300) {
    print("Some strange error ocured. check void Console::save()");
    return;
  }

  short a;
  consoleData *p= (consoleData*)data.get(lines- savedLines- 1);   /// gets first unsaved line

  FILE *f= fopen("console.txt", "ab");
    for(a= 0; a< lines- savedLines; a++) {
      fprintf(f, "%d> %s\n", savedLines+ a, p->line.buf);
      p= (consoleData*)p->prev;                         /// jumps back from node to node till the first one
    }
    savedLines+= a- 1;
  fclose(f);
}

// position, size funcs
void Console::setPos(short x, short y) {
  x0= x;
  y0= y;
}

void Console::setSize(short w, short h) {
  width= w;
  height= h;
#ifdef USING_CONSOLE_OPENGL
  heightInChars= font.cHei? (height/ font.cHei): 0;
  widthInChars= font.cWidFix? (width/ font.cWidFix): 0;		// fixed width fonts only, pls, no headaches
#endif
}

void Console::resize(short w, short h, short posX, short posY) {
  setPos(posX, posY);
  setSize(w, h);
}



// PRINT FUNCS
///-----------
void Console::print(const char *text) {
  short a, b;
  short len= strlen(text);
/// safecheck, WILL SET LINE LENGTH TO 80
  if((widthInChars- 2)< 1)
    widthInChars= 80;
/// text is divided in as many lines as it takes to fit the console
  if(len> widthInChars- 2) {
    char *buff= new char[widthInChars];
    char *t= (char *)text;
    for(a= 0; a<= len/ widthInChars; a++) {
      for(b= 0; (b< widthInChars- 2) && (*t!= 0); b++, t++)   // strncpy... faster i think, i checked MS code, it's shit, full with templates, asm is a wet dead dream
        buff[b]= *t;
      buff[b]= '\0';

      print(buff);
    }
    delete[] buff;
  } else {
/// safecheck
    if(lines> 2000000) {
      save();
      lines= 0;
      savedLines= 0;
    }
/// save to file every 100 printed lines
    if( (lines- savedLines)> 99)
      save();

/// actual adding to the data list
    lines++;
    consoleData *p= new consoleData;
    p->line= text;
    data.del(data.last);
    data.addFirst(p);
  }
#ifdef USING_CONSOLE_WINDOWS
  this->setData();
#endif
}

///printf e limitat la 511 caractere afisabile
void Console::printf(const char *format, ...) {
  short len= strlen(format);
  if(len> 511) {
    print("Line too large to use printf; use \'print\'");
    return;
  }
  va_list args;
    char b[512];
    va_start(args, format);
    vsprintf(b, format, args);
    va_end(args);
  print(b);
}


// COMMAND LINE processing funcs
///-----------------------------
bool Console::getCommand(string &text) {
  if(commandWaiting) {
    text= command;
    print(command);
    cursorPos= 0;
    command= "";
    commandWaiting= false;
    return true;
  }
  return false;

}

void Console::processCommand() {
  if(commandWaiting) {
    command.down();

    if(command == "quit") {
      print("Quitting program");
      command= "";
      commandWaiting= false;
      PostQuitMessage(0);
    } else if((command == "help") || (command == "h") || (command == "-h") || (command == "/h")) {
// HELP / LIST OF COMMANDS FOR CORE CONSOLE COMMANDS ONLY HERE
      print("quit - exit program");
      
      command= "";
      commandWaiting= false;
    } else {
      print("unknown command /h -h h help for help");
      command= "";
      commandWaiting= false;
    }
  }
}

void Console::in(char c) {
  if(commandWaiting) return;            // it will IGNORE every input if there is a command waiting to be processed
  switch(c) {
    case '\n': {
      commandWaiting= true;
      break;
    }
    case '\b': {
      if(cursorPos> 0)
        command.buf[--cursorPos]= 0;
      break;
    }
    default: {
      if(!(cursorPos>= (widthInChars- 2))) {      ///it will ignore input if cursor is at max line length
        command+= c;
        cursorPos++;
        break;
      }
    }
  }
}

/// COMMAND registration funcs
void Console::regCommand(string &name, void (*func)()) {
  conRegCom *p= new conRegCom;
  p->name= name;
  p->nrParams= 0;
  p->func= func;
  regCommands.add((conRegCom*)p);
}

void Console::regCommand1p(string &name, void (*func)(string &p1)) {
  conRegCom *p= new conRegCom;
  p->name= name;
  p->nrParams= 1;
  p->func1p= func;
  regCommands.add((conRegCom*)p);
}

void Console::regCommand2p(string &name, void (*func)(string &p1, string &p2)) {
  conRegCom *p= new conRegCom;
  p->name= name;
  p->nrParams= 2;
  p->func2p= func;
  regCommands.add((conRegCom*)p);
}

void Console::regCommand3p(string &name, void (*func)(string &p1, string &p2, string &p3)) {
  conRegCom *p= new conRegCom;
  p->name= name;
  p->nrParams= 3;
  p->func3p= func;
  regCommands.add((conRegCom*)p);
}

void Console::regCommand4p(string &name, void (*func)(string &p1, string &p2, string &p3, string &p4)) {
  conRegCom *p= new conRegCom;
  p->name= name;
  p->nrParams= 4;
  p->func4p= func;
  regCommands.add((conRegCom*)p);
}

/// VARIABLE registration funcs

void Console::regClass(const string &name) {
  if((!name) || (name == "")) {
    print("Error: Console::regClass: name cant be null or empty");
    return;
  }

  chainList *l= getParentClass(name); /// parent class (first is global)
  if(!l)
    return;
  
  conRegVar *var= (conRegVar*)new conRegVar;
  var->type= 0;
  var->name= getVarName(name);
  
  l->add((conRegVar*)var);
}

void Console::regVar(void *var, const string &name, short size, bool sig, bool pointer) {
  if((!name) || (name == "")) {
    print("Error: Console::regVar: name cant be null or empty");
    return;
  }

  chainList *l= getParentClass(name); /// parent class (first is global)
  if(!l)
    return;
  
  conRegVar *v= (conRegVar*)new conRegVar;

  v->type= 1;
  v->name= getVarName(name);
  v->pointer= pointer;
  v->sign= sig;
  v->size= size;
  v->var= var;	
  l->add((conRegVar*)v);
}

void Console::regString(string *var, const string &name) {
  if((!name) || (name == "")) {
    print("Error: Console::regString: name cant be null or empty");
    return;
  }

  chainList *l= getParentClass(name); /// parent class (first is global)
  if(!l)
    return;
  
  conRegVar *v= (conRegVar*)new conRegVar;

  v->type= 2;
  v->name= getVarName(name);
  v->var= var;	
  l->add((conRegVar*)v);
}

/// private TOOLS funcs used mainly in registering vars/classes
chainList *Console::getParentClass(const string &name) {
  string s;
  conRegVar *v;
  chainList *l= &regVars;

  for(int a= 0; a< getNrParentClasses(name); a++) {
    s= getParentClassName(name, a);
    for(int b= 0; b< l->nrNodes; b++) { /// search for parent class a
      v= (conRegVar*)l->get(b);
      if(v->name== s)
        break;                          /// found
      
      if(b== l->nrNodes- 1) {           /// cant find class (last var+name didnt check)
        printf("Error: Console::getParentClass: can't find %s", name);
        return null;
      }
    }
  }
  return l= &v->vars;                   /// l points to next parent class (first is global)
}

int Console::getNrParentClasses(const string &name) {
  int ret= 0;
  for(int a= 0; a< (int)name.getLen(); a++)
    if(name.buf[a] == '.')
      ret++;
  return ret;
}

string Console::getParentClassName(const string &name, int nr) {
  if(getNrParentClasses(name) == 0)
    return string("global");
  if(getNrParentClasses(name)< (nr+ 1)) {
    print("Console::getParentClassName: nr class to be returned is out of bounds");
    return string("");
  }
  string ret;
  char *p= name.buf;

  int t= (int)name.getLen();
  int l= 0;

/// skip classNames till nr
  for(short a= 0; a< nr; a++)
    for(int b= 0; b< t; b++, p++)
      if(*p == '.') {
        p++;
        break;
      }
/// check how big the name of the class is
  for(int a= 0; a< t; a++, l++)
    if(p[a] == '.')
      break;
/// build return string
  ret.buf= new char[l+ 1];
  for(short a= 0; a< l; a++)
    ret.buf[a]= p[a];
  ret.buf[l]= 0;

  return ret;
}

string Console::getVarName(const string &name){
  char *p= name.buf;
  int n= getNrParentClasses(name);
  int l= (int)name.getLen();
  int l2= 0;

/// skip parent class(es)
  for(int a= 0; a< n; a++)
    for(int b= 0; b< l; b++)
      if(*p == '.') {
        p++;
        break;
      }
/// name length
  for(int a= 0; a< l; a++, l2++)
    if( ! (((p[a] >= 'a') && (p[a] <= 'z')) || ((p[a] >= 'A') && (p[a] <= 'Z')) || ((p[a] >= '0') && (p[a] <= '9'))) )
      break;
/// build return string
  string ret;
  ret.buf= new char[l2+ 1];
  for(short a= 0; a< l2; a++)
    ret.buf[a]= p[a];
  ret.buf[l2]= 0;

  return ret;
}






// scroll funcs
void Console::scrollUp(short n) {
  scroll+= n;
  if(scroll> CON_MAX_LINES- heightInChars- 3)     // 3: statusline, there's text up line, there's text down line
    scroll= CON_MAX_LINES- heightInChars- 3;
}

void Console::scrollDown(short n) {
  scroll-= n;
  if(scroll< 0)
    scroll= 0;
}

char *Console::getLine(short nr) {
  if(nr<0 || nr>CON_MAX_LINES- 1) return NULL;

  consoleData *p= (consoleData *)data.get(nr);

  return p->line.buf;
}



// OpenGL specific
///---------------

#ifdef USING_CONSOLE_OPENGL
// MAIN DRAW FUNC, use in a loop or watever
void Console::draw() {
  if(show) {
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    
    ///
    if(textureLoaded) {
      glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
      texture.bind();
    }
    else
      glColor4f(0.0f, 0.0f, 0.0f, 0.5f);

    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f); glVertex3f((float)x0,				(float)y0,				90.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f((float)(x0+ width),	(float)y0,				90.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f((float)(x0+ width),	(float)(y0+ height),	90.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f((float)x0,				(float)(y0+ height),	90.0f);
    glEnd();

///bulk text print
    glColor4f(0.5f, 1.0f, 0.7f, 1.0f);

    consoleData *p= (consoleData*)data.first;
    for(short a=0; a< scroll; a++)
      p= (consoleData*)p->next;

    for(short a=0; a< heightInChars- 3; a++) {
      font.printXY((float)x0, (float)(y0+ (a+ 2)* font.cHei), p->line );
      p= p->next;
    }

///"up" line print
    if(lines> heightInChars+ scroll- 3) {
      glColor4f(1.0, 0.8f, 0.8f, 1.0f);
      font.setPos((float)x0, (float)(y0+ (heightInChars- 1)* font.cHei), 90);
      for(short a= 0; a< widthInChars; a++)
        font.print("^");
    }

///"down" line print
    if(scroll) {
      glColor4f(1.0, 0.8f, 0.8f, 1.0f);
      font.setPos((float)(x0+ (widthInChars+ 1)* font.cWidFix+ 1), (float)(y0+ font.cHei* 1+ 1), 90);			
      glRotatef(180, 0.0f, 0.0f, 1.0f);
      for(short a= 0; a< widthInChars; a++)
        font.print("^");
      glRotatef(180, 0.0f, 0.0f, 1.0f);
    }


    ///status line draw
    if(statusText) {
      glColor4f(0.6f, 0.6f, 0.6f, 1.0f);
      font.setPos(x0+ widthInChars- (statusText.getLen()* font.cWidFix), y0+ font.cHei, 90);
      font.print(statusText);
    }

    ///command line draw
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    font.setPos(x0, y0, 91);
    font.print(command);
    font.showCursor();
  }///show
}

bool Console::loadBackground(char *fileName) {
  texture.load(fileName);
  
  if(texture.checkValid()) {
    textureLoaded= true;
    return true;
  }
  return false;
}

bool Console::loadFont(char *fileName) {
  p.setFont(fileName);
}

void Console::deleteTexture() {
  texture.delTexture();
  p.delTexture();
  textureLoaded= false;
}

#endif


///----------------
// Windows specific
///----------------

#ifdef USING_CONSOLE_WINDOWS

void Console::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

/// window, window control init
BOOL Console::OnInitDialog() {
  CDialog::OnInitDialog();

  int scr1, scr2;
  setData();
  CEdit *e= (CEdit *)GetDlgItem(IDC_CONSOLE1);
  e->GetScrollRange(SB_VERT, &scr1, &scr2);
  e->SetScrollPos(SB_VERT, scr2- scr1);
  scroll= e->GetScrollPos(SB_VERT);
  setData();

  return TRUE;
}

void Console::setData() {

  string s;
  CEdit *e= (CEdit *)GetDlgItem(IDC_CONSOLE1);
  consoleData *p= (consoleData*)data.last;

  for(short a=0; a< CON_MAX_LINES; a++) {
    s+= p->line; s+= "\r\n";
    p= (consoleData*)p->prev;
  }

  scroll= e->GetScrollPos(SB_VERT);

  SetDlgItemTextA(IDC_CONSOLE1, s);

  e->SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, scroll));
  e->UpdateWindow();
}



void Console::OnOK() {
  CString s;

  GetDlgItemTextA(IDC_CONSOLE2, s);
  command= s;
  commandWaiting= true;
  SetDlgItemTextA(IDC_CONSOLE2, "");    /// clear command line
  GetDlgItem(IDC_CONSOLE2)->SetFocus(); /// re-set focus on the command line after return - focus is set on somthin else, dunno why

  print(command);
  command.down();

// windows specific, program independent commands
  if(command == "windows specific command") {
    command= "";
    commandWaiting= false;
    // do the stuff here

  } else if( (command == "help") || (command == "h") || (command == "/h") || (command == "-h")){
/// windows help specific

    print("help / list of commands not defined");
    processCommand();         /// process non-windows specific help stuff too
  } else
    processCommand();


  ///CDialog::OnOK();           /// default action disabled
}

void Console::OnCancel() {
  CDialog::OnCancel();
}

#endif




