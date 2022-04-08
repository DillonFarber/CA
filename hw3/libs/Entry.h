/*
    Class: Entry
    Parameter(s):   int tag
                    int counter;
    Method(s): 
                    Entry()
                    bool checkEntry()
                    void setTag()

        
    Class Function:
                    This is a class that is a entry for a cache sim. 
                    It acts as the stacking blocks for the cache system. 
        
    Associated Classes:
        Class: Cache.h
        REQUIRED Class: WordBlocks (WordBlocks.h)
 */

#pragma once
#include <cmath>


class Entry
{
private:
    int tag = -1;
    int counter = 0;

public:
    Entry();
    bool checkEntry(int tag);
    void counterUp();
    void setTag(int tag);
    int getCounter();
    ~Entry();
};

 /* 
    Constructor
  */
Entry::Entry(){}

/* 
    Method: checkEntry()
    Parameter(s): int tag
    Functioin: The method takes in the tag of an address and checks
        it agains the tag of the object.
    Return: boolean
            true if match
            false if not
 */
bool Entry::checkEntry(int tag)
{
    this->counter++;
    if(this->tag == tag)
        return true;
    else
        return false;
}

/* 
    Method: counterUp()
    Parameter(s): none
    Functioin: The method checks the valid parameter and sends
        it back to the caller.
    Return: boolean
 */
void Entry::counterUp()
{
    this->counter++;
}

/* 
    Method: counterUp()
    Parameter(s): none
    Functioin: The method checks the valid parameter and sends
        it back to the caller.
    Return: boolean
 */
int Entry::getCounter()
{
    return this->counter;
}

/* 
    Method: setTag()
    Parameter(s): int tag
    Functioin: This sets the tag to the new tag and makes the 
        valid boolean true as this tag has been recently visited
    Return:
 */
void Entry::setTag(int tag)
{
    this->tag = tag;
    this->counter = 0;
}

/* 
    Deconstructor
 */
Entry::~Entry()
{
}
