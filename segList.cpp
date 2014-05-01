#ifdef OS_WIN
#include <Windows.h>
#endif /// OS_WIN

#ifdef OS_LINUX
#include <time.h>
#endif /// OS_LINUX

#ifdef OS_MAC
#include <mach/mach.h>                  // high resolution clock funcs
#include <mach/mach_time.h>             // high resolution clock funcs
#endif /// OS_MAC

#include "typeShortcuts.h"
#include "chainList.h"
#include "segList.h"


// !!!!!!!!!!!
// there shouldn't be many segments, else there are too many allocs/ loops get bigger = adios speed.
// TRY AND MAKE THE SEGMENT SIZE AS OPTIMAL AS POSSIBLE WITH THAT IN MIND

//#define CHAINLIST_SAFECHECKS 1 //to check for bad calls to chainList

///-------------///
// segData class //
///-------------///

segData::~segData() {
}

///-------------///
// segment class //
///-------------///

segment::segment() {
  next= prev= null;
  data= null;
  freeSpc= null;
  freeSpcPeak= 0;
  timeIdle= 0;					/// 0= segment in use
}

segment::~segment() {
  if(data)
    delete[] data;
  if(freeSpc)
    delete[] freeSpc;
}


///-------------///
// segList class //
///-------------///

segList::segList(int segmentSize, int uSize, int idleTime) {
  unitSize= uSize;
  segSize= segmentSize;
  timeMaxIdle= idleTime;

  first= last= null;
  nrNodes= 0;

  addSegment();         // create the first memory segment - there is always 1, never 0
}

segList::~segList() {
  delData();
}

void segList::delData() {
  while(seg.nrNodes)
    removeSegment();

  nrNodes= 0;
  first= last= null;
  unitSize= 0;
  segSize= 0;
}

// MEMORY ALLOC/ DEALLOC FUNCS
void segList::addSegment() {
  segment *p= new segment;
  p->data= new char[unitSize* segSize];
  p->freeSpc= new void*[segSize];

/// set free space to point to the data allocated
  for(int a= 0; a< segSize; a++)
     p->freeSpc[a]= (char*)p->data+ a* unitSize;

  p->freeSpcPeak= segSize;
  p->timeIdle= 0;
  seg.add((chainData*)p);
}

void segList::removeSegment() {
  seg.del(seg.last);
}


// ADD NODE funcs
segData *segList::add() {
  segData *p= last;
  segData *ref= null;
  segment *b= (segment*)seg.first;

/// find a free segment unit(segData) space
  for(int a= 0; b; a++, b= (segment*)b->next) /// pass thru all the segments (should be few segments, else the search time gets big)
    if(b->freeSpcPeak) {
      ref= (segData*)b->freeSpc[b->freeSpcPeak- 1];
      ref->segment= a;
      b->freeSpcPeak--;
      b->timeIdle= 0;                         /// set timeIdle to 0, signifying segment is used
      break;
    }

/// not found? alloc mem for another segment
  if(!ref) {
    addSegment();
    b= (segment*)seg.last;
    ref= (segData*) b->freeSpc[b->freeSpcPeak- 1];
    ref->segment= seg.nrNodes- 1; /// last segment
    b->freeSpcPeak--;
  }

/// do the next/prev link
  if(p) {
    p->next= ref;
    ref->prev= p;
    last= ref;
  } else {
    first= last= ref;
    ref->next= null;    /// keep this init here, if a constructor is used, it wont be called by derived class
    ref->prev= null;    /// ^^^
  }

  nrNodes++;
  return ref;
}

segData *segList::addFirst() {
  segData *p= first;
  segData *ret= null;
  segment *b= (segment*)seg.first;

/// find a free segment unit(segData) space
  for(int a= 0; b; a++, b= (segment*)b->next) /// pass thru all the segments (should be few segments, else the search time gets big)
    if(b->freeSpcPeak) {
      ret= (segData*)b->freeSpc[b->freeSpcPeak- 1];
      ret->segment= a;
      b->freeSpcPeak--;
      b->timeIdle= 0;                         /// set timeIdle to 0, signifying segment is used
      break;
    }

/// not found? alloc mem for another segment
  if(!ret) {
    addSegment();
    b= (segment*)seg.last;
    ret= (segData*) b->freeSpc[b->freeSpcPeak- 1];
    ret->segment= seg.nrNodes- 1;
    b->freeSpcPeak--;
  }

/// do the next/prev linking
  if(p) {
    p->prev= ret;
    ret->next= p;
    first= ret;
    ret->prev= null;
  } else {
    first= last= ret;
    ret->next= null;
    ret->prev= null;
  }

  nrNodes++;
  return ret;
}

// DEL NODE FUNCS

/// fast function
void segList::del(segData *p) {	
#ifdef CHAINLIST_SAFECHECKS
  if((!nrNodes) || (!p))
    return;
#endif
/// set the next / prev link
  if(p->prev) p->prev->next= p->next;
  if(p->next) p->next->prev= p->prev;
  if(p== first) first= p->next;
  if(p== last) last= p->prev;

 /// find the segment it belongs to
  segment *b= (segment *)seg.first;

  for(int a= 0; a< p->segment; a++)
    b= (segment *)b->next;

/// 'free' the space it used
  b->freeSpc[b->freeSpcPeak]= p;
  b->freeSpcPeak++;

  nrNodes--;
}

/// SLOW - goes thru the list, with many instructions per cicle (in a small list is ok)
void segList::deli(int nr) {
#ifdef CHAINLIST_SAFECHECKS
  if(!nrNodes) return;
  if(nr> nrNodes) return;
#endif

/// find the segment
  segData *p= first;
  for(int a= 0; a< nr; a++)
    p= p->next;
/// set the next-prev link
  if(p->prev) p->prev->next= p->next;
  if(p->next) p->next->prev= p->prev;
  if(p== first) first= p->next;
  if(p== last) last= p->prev;

/// find segment it belongs to
  segment *b= (segment *)seg.first;
  for(short a= 0; a< p->segment; a++)
    b= (segment *)b->next;
/// 'deallocate' itself from the segment
  b->freeSpc[b->freeSpcPeak]= p;
  b->freeSpcPeak++;

  nrNodes--;
}


///---------------------------------------------------------------///
// GET - SEARCH list funcs - not to be used regulary or in a cycle //  
///---------------------------------------------------------------///

/// get must be RARELY used: if one makes a for(a) { get(b)} it gets thru a*b times thru the list. if the list is 100000units.... do the math
segData *segList::get(int nr) {
#ifdef CHAINLIST_SAFECHECKS
  if(!nrNodes) return null;
  if(nr> nrNodes) return null;
#endif

  segData *p= first;
  for(int a= 0; a< nr; a++)
    p= p->next;

  return p;
}

/// same as the get func, should be RARELY used
int segList::search(segData *e) {
  segData *p= first;
  for(int a= 0; a< nrNodes; a++, p= p->next)
    if(p== e) return a;

  return -1;
}


// call this func from time to time to clear idle allocated segments... from time to time as in very rarely... no need to include it in every frame
void segList::checkIdle() {
  if(seg.nrNodes == 1)    // it wont dealloc the first segment
    return;
  
/// getting the present time
  int present;
  
  #ifdef OS_WIN
  present= GetTickCount();
  #endif /// OS_WIN

  #ifdef OS_LINUX
  timespec out;
  clock_gettime(CLOCK_MONOTONIC, &out);
  present= (out.tv_sec* 1000)+ (out.tv_nsec/ 1000000);
  #endif /// OS_LINUX

  #ifdef OS_MAC
  //uint64_t clocks;
  uint64_t time;
  static mach_timebase_info_data_t machInfo;
  if(machInfo.denom== 0)
    mach_timebase_info(&machInfo);

  //clocks= mach_absolute_time();

  time= (mach_absolute_time()* machInfo.numer/ machInfo.denom)/ 1000000;
  present= (int)time;
  #endif /// OS_MAC
  
  segment *p= (segment*)seg.first->next;              /// ignore the first
  segment *t;

  while(p)
    if(p->freeSpcPeak == segSize) {                   // if all space is free
      if(p->timeIdle) {                               /// check if the idle timer was started
        if((present- p->timeIdle)> timeMaxIdle) {     /// check difference between present-idle start time
          t= p;
          p= (segment *)p->next;
          seg.del(t);
        }
      } else {                                        /// if all space is free, start the idle timer
        p->timeIdle= present;
        p= (segment*)p->next;		
      }
    }	else {                                          // else the segment is in use / check next
      p->timeIdle= 0;                                 /// assure the idle timer is not used
      p= (segment*)p->next;
    }
}




