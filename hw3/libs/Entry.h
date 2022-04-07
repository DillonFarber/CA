/*
    Class: Entry
    Parameter(s): int tag
                  bool valid
    Method(s): 
        
    Class Function:
        
    Associated Classes:

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

Entry::Entry(){}

bool Entry::checkEntry(int tag){
    if(this->tag == tag)
        return true;
    else
        return false;
}
bool Entry::checkValid(){
    return this->valid;
    
}

void Entry::oldValid(){
    this->valid = false;
}

void Entry::setTag(int tag)
{
    this->tag = tag;
    this->valid = true;
}

void Entry::validTrue()
{
    this->valid = true;
}

Entry::~Entry()
{
}
