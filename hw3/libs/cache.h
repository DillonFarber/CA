/*
    Class: Cache
    Parameter(s):   int assoc
                    int entries
                    int words
                    Entry **cacheBlock
    Method(s): 
        private: void enter()
        private: bool check()
        private: void createCacheBlocks()
        private: void createEntriesPerBlock()
        public: int running()
        void incrementCounter()
    Class Function:
        The purpose of this class is to simulate the cache of a cpu
        in the fashion of taking in a generic integer value, the 
        associtivity of the cache, and the words per block, to 
        simulate a true cache. 
    Associated Classes:
        REQUIRED Class: Entry (Entry.h)
 */


#include "Entry.h"

class Cache
{
private:

    Entry **cacheBlock;
    int assoc, entries, words;

    void enter(int addr);
    bool check(int addr);
    void createCacheBlocks(int assoc, int entries);
    void incrementCounter();
    
public:
    Cache();
    Cache(int assoc, int entries);
    Cache(int assoc, int entries, int words);
    int running(int addr);
    ~Cache();
}; 

/* Default Constructor */
Cache::Cache(){}

/* 
    Constructor
    Parameters: int assoc entries
    Takes in the amount of associtivity and the number entries that
    the cache will have.
*/
Cache::Cache(int assoc, int entries){
    this->assoc = assoc;
    this->entries = entries;
    createCacheBlocks(assoc, entries);
    
}
/* 
    Constructor
    Parameter(s): int assoc, entries, words
    Takes in the amount of associtivity and the number entries that
    the cache will have. Along with how many words that each entry 
    will contain. 
 */
Cache::Cache(int assoc, int entries, int words){
    this->assoc = assoc;
    this->entries = entries;
    this->words = words;
    createCacheBlocks(assoc, entries);
}

/* 
    Method: createCacheBlocks()
    Parameter(s): int assoc, entries
    Function: Takes a associtivity parameter and creates an array
    of Entry objects for the number of ways. The second demension
    is the number of entries per way. 
 */
void Cache::createCacheBlocks(int assoc, int entries)
{
    this->cacheBlock = new Entry*[assoc];
    for(int i = 0; i < assoc; i++)
        this->cacheBlock[i] = new Entry[entries/assoc];
}

/* 
    Method: check()
    Parameter(s): int addr
    Function: The check takes the address and finds the tag and its
    index, then searches the number of ways there are for that index.
    Return: boolean
            true if found
            false if not found
 */

bool Cache::check(int addr)
{
    int tag = addr / (this->entries/this->assoc);
    int ind = addr % (this->entries/this->assoc);
    int fset;
    bool found = false;
    
    for(int set = 0; set < this->assoc; set++)
    {
        found = this->cacheBlock[set][ind].checkEntry(tag);
        if(found)
        {
            incrementCounter();
            this->cacheBlock[set][ind].setTag(tag);
            fset = set;
            break;
        }
    }
    return found;
    
}
/*
    Method: enter()
    Parameter: addr - address in integer form
    Function: enter() method uses the addr parameter to break down
    into a tag and index. Then it checks each way if has an opening 
    to input into the cach. when the method finds a place to put the 
    address, it calls the the incrementCounter() to increment each of
    the cache.
*/
void Cache::enter(int addr)
{
    int tag = addr / (this->entries/this->assoc);
    int ind = addr % (this->entries/this->assoc);
    int findOld[this->assoc]; 
    int oldest, oldestby;
    for(int set = 0; set < this->assoc; set++)
    {       
        findOld[set] = this->cacheBlock[set][ind].getCounter();
    }
    oldest = 0;
    oldestby = findOld[0];
    for (int i = 1; i < this->assoc; i++)
    {
        if(findOld[i] > oldestby)
        {
            oldest = i;
            oldestby = findOld[i];
        }
    }
    incrementCounter();
    cacheBlock[oldest][ind].setTag(tag);
    
    
}

/*
    Method: running()
    Parameter: addr - this is the address integer to be broken down
    Function: running() takes an address integer, and checks if it 
    is in the cache by calling check(). If returned false, we enter
    it with the enter(). This inputs it into the cache.
    Return: 
        This method returns three options as integers back to calling
        program.
        0 - means that it was found in the cache
        1 - means that it was entered into the cache
        -1 - means that some sort of error occured
*/
int Cache::running(int addr)
{
    bool found = false;
    found = check(addr);
    if(found)
        return 0;
    else
    {
        enter(addr);
        return 1;
    }

    return -1;
    
}

/* 
    Method: incrementCounter()
    Parameters: none
    Function: This increments the counter of each object in the entry
    class object. 
    Return: N/A
 */
void Cache::incrementCounter()
{
    for (int i = 0; i < this->assoc; i++)
    {
        for (int j = 0; j < (this->entries/this->assoc); j++)
        {
            cacheBlock[i][j].counterUp();
        }
        
    }
    
}

/* Destructor 
    Function: Deletes the allocated space of the dynamically 
    allocated array, then decunstructs the object. 
*/
Cache::~Cache()
{
    for(int i = 0; i < this->assoc; i++)
        delete [] this->cacheBlock[i];
    delete [] this->cacheBlock;
}
