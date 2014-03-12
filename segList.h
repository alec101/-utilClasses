#pragma once
//#define CHAINLIST_SAFECHECKS 1 //to check for bad calls to chainList

// memory alloc is automatic, everything you need to think about is the segment size. it must be right, so not too many allocs happen
// segList( segment size, sizeof(segData derived class));

// you can call checkIdle() from time to time, to auto deallocate memory of idle segments
// set timeMaxIdle (milisecs) to a desired idle time (default is 5 mins) after wich a segment is deallocated
// a forced dealloc can happen with timeMaxIdle set to 0 or very low, and call checkIdle() 2 times(first time will mark empty segments- second pass will erase marked)


// TODO:  i think there is a posibility to auto-know the derived class size, so the constructor will only need to know a segment size ( and not a unit size )


class segList;
class segment;

/// DERIVE classes from this one & DEFINE what vars are in the chain (node/blabla)
class segData {
  friend class segList;
  int segment;
public:
  segData *next, *prev;

/// there are no virtual constructors / will not be called by derived class... big source of errors
  virtual ~segData();			/// being virtual, it seems delete[] knows to dealloc the derived parts... it never crashed/ no garbage remains in memory
};


// ------------------================= SEGLIST CLASS ====================------------------

// manipulator class, just make a variable from this one
class segList {
  friend class segData;

// memory alloc private vars, nothing to bother
  int unitSize;                 /// each segData (derived class) size
  int segSize;                  /// segment size
  chainList seg;                /// chainlist with all segments of memory allocated / memory alloc is automatic, nothing to bother
  inline void addSegment();     /// memory allocation
  inline void removeSegment();  /// memory deallocation

public:
  segData *first, *last;      // last is important for add(), else there has to be a loop that passes thru all the list (if the list has 100k nodes... goodbye speed)
  int nrNodes;                  /// nr nodes in list
  
// fast funcs
  segData *add();               // returns pointer to the space allocated (it really allocs mem IF the segment allocated is full)
  segData *addFirst();          // adds the node to the front of the list not the back (makes it first)
  void del(segData *);          // NO searches involved - VERY FAST

// idle time check func
  void checkIdle();             // call this func RARELY to check for idle memory segments that need to dealocate
  int timeMaxIdle;              /// miliseconds: idle time after wich a segment is dealocated default is 5 mins. A FORCE DEALLOC CAN HAPPEN: make this var very low, and call checkIdle() 2 times, put 5 mins back or watever

// slow funcs
  void deli(int);               /// searches involved good to have - SLOW
  segData *get(int);            /// searches involved - SLOW
  int search(segData *);        /// returns -1 if failed to find; SLOW

  segList(int segmentSize, int uSize, int idleTime= 300000);  // initialize by providing a segment size, and a sizeof(derived segData)
  ~segList();
  void delData();               /// teh real destructor (tm)
};


// internal allocation / deallocation segment list, nothing to bother here
class segment: protected chainData {
friend class segList;
  void *data;       /// actual memory allocated
  
  void **freeSpc;   /// list with all free spaces in segment ( unitSize* segSize total space)
  int freeSpcPeak;  /// peak of freeSpc. acts like some kind of stack: u pick a free space from the top/ u put the space taken back

  int timeIdle;     /// if 0, segment is in use. Else it holds the time @ start idling (used internally by segList::checkIdle())

  segment();
  ~segment();
};




