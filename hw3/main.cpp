#include <iostream>
#include <math.h>
#include <vector>
#include <array>
#include "libs/Cache.h"
#include <fstream>
#include <string>

const unsigned short MAX = 10000;

int main(int argc, char const *argv[])
{
    if(argc < 4){
        std::cout << "./.exe  .. args- ERROR not enough arguments inputed" << std::endl;
        return 1;
    }

    Cache myCache;
    std::ifstream input;
    std::ofstream output;
    std::string file = "cache_sim_output.txt";
    char deliminator = ' ';
    std::string strAddr, charDigit = "";
    int addrs[MAX];
    int indx = 0;
    int hit_miss[MAX];

    input.open(argv[3]);
    if(!input.is_open()){
        std::cout << "The file did not open, exiting program!" << std::endl;
        return -1;
    }

    std::cout << "argv[2] " << atoi(argv[2]) << " argv[1] " << atoi(argv[1]) \
    << " The file: " << argv[3] << std::endl;

    myCache = Cache(atoi(argv[2]), atoi(argv[1]));
    int i = 0;
    while(!input.eof())
    {
        getline(input, strAddr);
    }
    input.close();
    std::cout << strAddr << std::endl;
    
    for(int i = 0; i < strAddr.length(); i++)
    {
        if(strAddr[i] != deliminator)
        {
            charDigit += strAddr[i];
        }
        else{
            addrs[indx] = stoi(charDigit);
            charDigit = "";
            indx++;
        }
        
    }
    for(int i = 0; i < indx; i++)
    {
        hit_miss[i] = myCache.running(addrs[i]);
    }

    output.open(file);
    if(!output.is_open())
    {
        std::cout << "error didn't open output file!!" << std::endl;
        return -2;
    }
    for (int i = 0; i < indx; i++)
    {
        switch (hit_miss[i])
        {
        case 0:
            output << "HIT:  "  << addrs[i] << " | " << std::endl;
            break;
        case 1:
            output << "MISS: " << addrs[i] << " | misstype: " << std::endl;
            break;
        case -1:
            output << "Error: unrecognized addr" << addrs[i];
        default:
            break;
        }
    }
    output.close();
    


    
    return 0;
}
