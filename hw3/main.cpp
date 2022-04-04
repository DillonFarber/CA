#include <iostream>
#include <math.h>
#include <vector>
#include <array>
#include "libs/Cache.h"


int main(int argc, char const *argv[])
{
    Cache myCache;
    if(argc < 4){
        std::cout << "./.exe  .. args- ERROR not enough arguments inputed" << std::endl;
        return 1;
    }

    std::cout << "argv[2] " << atoi(argv[2]) << " argv[1] " << atoi(argv[1]) << std::endl;

    myCache = Cache(atoi(argv[2]), atoi(argv[1]));
    


    std::cout << "Thats awesome!!!!!" << std::endl;

    return 0;
}
