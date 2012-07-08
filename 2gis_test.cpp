// 2gis_test.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <stdint.h>


void usage()
{
   std::cerr << "Test -- 2gis test utility"                                                 << std::endl;
   std::cerr                                                                                << std::endl;
   std::cerr << "Usage:"                                                                    << std::endl;
   std::cerr << "  test -f <filename> -m words -v <word>"                                   << std::endl;
   std::cerr << "  test -f <filename> -m checksum"                                          << std::endl;
   std::cerr << "  test -h"                                                                 << std::endl;
   std::cerr                                                                                << std::endl;
   std::cerr << "Options:"                                                                  << std::endl;
   std::cerr << "-h                   show this screen"                                     << std::endl;
   std::cerr << "-f <filename>        filename for processing"                              << std::endl;
   std::cerr << "-m (words|checksum)  mode (word occurence count or checksum generation)"   << std::endl;
   std::cerr << "-v <word>            word to count its occurences the input file"          << std::endl;
   std::cerr                                                                                << std::endl;
}

int checksum(std::ifstream &ifile)
{
    uint32_t checksum = 0;
    const int BUF_SIZE = 4;
    while(ifile)
    {
        uint32_t buf = 0;
        ifile.read(reinterpret_cast<char*>(&buf),BUF_SIZE);
        checksum += buf;
    }
    return checksum;
}

int wordcount(std::ifstream &ifile, const char* s)
{
    size_t len = std::strlen(s);
    int_fast64_t cnt = 0;
    assert(len > 0);
    std::streamsize buf_size = (len < 4096)?4096:len;
    char* buf = new char[buf_size+len];
    char* match = buf - 1;
    
    ifile.read(buf,len-1);
    buf[ifile.gcount()] = 0;
    while(ifile) {
        ifile.read(buf+len-1,buf_size);
        buf[len-1 + ifile.gcount()] = 0;
        do {
            match = std::strstr(match + 1,s);
            cnt++;
        } while (match != NULL);
        cnt--;
    }
    return cnt;
}

int main(int argc, char* argv[])
{
    //usage();
    std::ifstream ifile;
    ifile.open("input.txt",std::ios::binary);
    std::cout << std::hex << std::setw(8) << std::setfill('0') << checksum(ifile) << std::endl;
    ifile.close();

    ifile.open("input2.txt",std::ios::binary);
    std::cout << wordcount(ifile,"test") << std::endl;
    ifile.close();
    
    return 0;
}
