#pragma once

//#define CHAINLIST_SAFECHECKS 1 //to check for bad calls to chainList

// DERIVE classes from this one & DEFINE what vars are in the node inside it
class chainData {
public:
  chainData *next, *prev;

  /// don't create a constructor, it won't be called by derived class... big source of errors
  virtual ~chainData();       /// being virtual, it seems delete[] knows to dealloc the derived parts... it never crashed/ no garbage remains in memory
};

/// manipulator class, no derives necesary, just make a var from this one
class chainList {
public:
  chainData *first, *last;    // last e important pt add(), altfel trebe un while care sa treaca prin toata lista (daca e lista de 1000000?)
  int nrNodes;                // it's not good to modify the variable type (_dunno why what i was thinking 11.jan.2014_) better to create a special int64 class or, if needed

  void add(chainData *);      // alloc mem, then pass pointer to add(). _better/faster function_
  void addFirst(chainData *); // adds the node to the front of the list not the back (makes it first)
  void del(chainData *);      // NO searches involved - VERY FAST
  void deli(int);             /// searches involved - SLOW - good to have tho

  chainData *get(int);        /// searches involved - SLOW
  int search(chainData *);    /// returns -1 if failed to find; nu e rapida - la 50000 iteme, vor fi tot atatea instructiuni minim

  chainList();
  ~chainList();
  void delData();             // teh real destructor (tm)
};




