/*
    Class: Entry
    Parameter(s):   int tag
                    bool valid
    Method(s): 
                    Entry()
                    bool checkEntry()
                    bool checkValid()
                    bool oldValid()
                    void setTag()
                    void validTrue();
        
    Class Function:
                    This is a class that is a entry for a cache sim. 
                    It acts as the stacking blocks for the cache system. 
        
    Associated Classes:
        Class: Cache.h
        REQUIRED Class: WordBlocks (WordBlocks.h)
 */

#pragma once
#include <cmath>
#include "WordBlocks.h"

class Entry
{
private:
    int tag = -1;
    bool valid = false;

public:
    Entry();
    bool checkEntry(int tag);
    bool checkValid();
    void oldValid();
    void setTag(int tag);
    void validTrue();
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
bool Entry::checkEntry(int tag){
    if(this->tag == tag)
        return true;
    else
        return false;
}

/* 
    Method: checkvalid()
    Parameter(s): none
    Functioin: The method checks the valid parameter and sends
        it back to the caller.
    Return: boolean
 */
bool Entry::checkValid(){
    return this->valid;
    
}

/* 
    Method: oldValid()
    Parameter(s): none 
    Functioin: this function makes the valid parameter false.
    Return: none 
 */
void Entry::oldValid(){
    this->valid = false;
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
    this->valid = true;
}

/* 
    Method: validTrue()
    Parameter(s): none 
    Functioin: This method makes the valid boolean true when it is 
        checked. 
    Return: none 
 */
void Entry::validTrue()
{
    this->valid = true;
}

/* 
    Deconstructor
 */
Entry::~Entry()
{
}
