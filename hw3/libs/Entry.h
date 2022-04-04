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
#include "WordBlocks.h"

class Entry
{
private:
    int tag = -1;
    bool valid = false;

public:
    Entry();
    Entry(int tag, bool valid);
    bool checkEntry(int tag);
    bool checkValid();
    void oldValid();
    ~Entry();
};

Entry::Entry(){}

Entry::Entry(int tag, bool valid)
{
    this->tag = tag;
    this->valid = valid;
}

bool Entry::checkEntry(int tag){
    if(tag == this->tag)
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

Entry::~Entry()
{
}
