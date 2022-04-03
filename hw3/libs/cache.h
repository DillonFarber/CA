#pragma once
#include <vector>
#include <array>


class cache
{
private:
    int entries, set;
public:
    cache(int s, int e);
 
    ~cache();
};


cache::cache(int set, int ent)
{
    this->entries = ent/set;
    this->set = set;
}

cache::~cache()
{
}
