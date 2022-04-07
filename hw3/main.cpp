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
    // error checking for the number of args in the input at compile time
    if(argc < 4){
        std::cout << "./.exe  .. args- ERROR not enough arguments inputed" << std::endl;
        return 1;
    }

    Cache myCache;
    std::ifstream input;
    std::ofstream output;
    std::string file = "cache_sim_output.txt";
    char deliminator = ' ';
    std::string strAddr = "";
    std::string str;
    std::string charDigit = "";
    int addrs[MAX];
    int indx = 0;
    int hit_miss[MAX];

    // opening and checking if it has opened the file. 
    input.open(argv[3]);
    if(!input.is_open()){
        std::cout << "The file did not open, exiting program!" << std::endl;
        return -1;
    }

    // creating my cache with the specified arguements 
    myCache = Cache(atoi(argv[2]), atoi(argv[1]));

    // getting the information from the input file. 
    while(!input.eof())
    {
        getline(input, str);
        strAddr += str + " ";

    }
    input.close();

    std::cout << strAddr << std::endl;
    
    // the string of addresses and breaking them down into integers and adding to array
    for(int i = 0; i < strAddr.length(); i++)
    {
        if(strAddr[i] != deliminator)
        {
            charDigit += strAddr[i];
        }
        else{
            std::cout << charDigit << std::endl;
            addrs[indx] = stoi(charDigit);
            charDigit = "";
            indx++;
        }
        
    }
    for(int i = 0; i < indx; i++)
    {
        hit_miss[i] = myCache.running(addrs[i]);
    }


    // opening the file for output.
    output.open(file);
    // error checking
    if(!output.is_open())
    {
        std::cout << "error didn't open output file!!" << std::endl;
        return -2;
    }

    // checking for hit or miss in the cache. then formating and putting them in the output file
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
