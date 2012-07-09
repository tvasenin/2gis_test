// Test -- 2gis test utility
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <cstring>
#include <algorithm>
//#include <cstdint>
#include <stdint.h>

typedef uint32_t checksum_t ;

void usage()
{
   std::cout << "Test -- 2gis test utility"                                                 << std::endl;
   std::cout                                                                                << std::endl;
   std::cout << "Usage:"                                                                    << std::endl;
   std::cout << "  test -f <filename> -m words -v <word>"                                   << std::endl;
   std::cout << "  test -f <filename> -m checksum"                                          << std::endl;
   std::cout << "  test -h"                                                                 << std::endl;
   std::cout                                                                                << std::endl;
   std::cout << "Options:"                                                                  << std::endl;
   std::cout << "  -f <filename>        filename for processing"                            << std::endl;
   std::cout << "  -m (words|checksum)  mode (word occurence count or checksum generation)" << std::endl;
   std::cout << "  -v <word>            word to count its occurences (case-sensitive)"      << std::endl;
   std::cout << "  -h                   show this screen"                                   << std::endl;
}

checksum_t checksum(std::ifstream &ifile)
{
    checksum_t checksum = 0;
    const std::streamsize BUF_SIZE = sizeof(checksum_t);
    while(ifile)
    {
        checksum_t buf = 0;
        ifile.read(reinterpret_cast<char*>(&buf), BUF_SIZE);
        checksum += buf;
    }
    return checksum;
}

int_fast64_t wordcount(std::ifstream &ifile, const char* s)
{
    size_t len = std::strlen(s);
    assert(len > 0);
    int_fast64_t cnt = 0;
    size_t buf_size = (len < 4096)?4096:len;
    char *buf = new char[buf_size+len];
    char *match = buf - 1;
    
    ifile.read(buf, len-1);
    buf[ifile.gcount()] = 0;
    while(ifile) {
        ifile.read(buf+len-1, buf_size);
        buf[len-1 + ifile.gcount()] = 0;
        do {
            match = std::strstr(match + 1, s);
            cnt++;
        } while (match != NULL);
        cnt--;
    }
    return cnt;
}

int main(int argc, char* argv[])
{
    std::ifstream ifile;
    char *ifname     = NULL;
    char *search_str = NULL;
    char *mode       = NULL;
    
    if (argc < 2){
        std::cerr << "No input arguments specified, use -h for help." << std::endl;
        return 1;
    }

    for (int loop = 1 ; loop < argc ; loop++) {
        if      (0 == std::strcmp(argv[loop], "-h")) {
            usage();
            return 0;
        }
        else if (0 == std::strcmp(argv[loop], "-f")) {
            if (argc <= loop + 1) {
                std::cerr << "No argument specified for "<< argv[loop] << std::endl;
                return 1;
            }
            else {
                if (ifname) {
                    std::cerr << "Duplicate "<< argv[loop] << " options are not allowed." << std::endl;
                    return 1;
                }
                else
                    ifname = argv[++loop];
                    if (!*ifname) {
                        std::cerr << "Empty filename is not allowed." << std::endl;
                        return 1;
                    }
            }
        }
        else if (0 == std::strcmp(argv[loop], "-m")) {
            if (argc <= loop + 1) {
                std::cerr << "No argument specified for "<< argv[loop] << std::endl;
                return 1;
            }
            else {
                if (mode) {
                    std::cerr << "Duplicate "<< argv[loop] << " options are not allowed." << std::endl;
                    return 1;
                }
                else
                    mode = argv[++loop];
                    if (!*mode) {
                        std::cerr << "Empty mode is not allowed." << std::endl;
                        return 1;
                    }
                    if ( (0 != std::strcmp(mode, "checksum")) && (0 != std::strcmp(mode, "words")) ) {
                        std::cerr << "Incorrect mode specified, use -h for help." << std::endl;
                        return 1;
                    }
            }
        }
        else if (0 == std::strcmp(argv[loop], "-v")) {
            if (argc <= loop + 1) {
                std::cerr << "No argument specified for "<< argv[loop] << std::endl;
                return 1;
            }
            else {
                if (search_str) {
                    std::cerr << "Duplicate "<< argv[loop] << " options are not allowed." << std::endl;
                    return 1;
                }
                else {
                    search_str = argv[++loop];
                    if (!*search_str) {
                        std::cerr << "Empty word is not allowed." << std::endl;
                        return 1;
                    }
                }
            }
        }
        else {
            std::cerr << "Unknown option " << argv[loop] << ", use -h for help." << std::endl;
            return 1;
        }
    }

    if  (!mode) {
        std::cerr << "No mode specified, use -h for help." << std::endl;
        return 1;
    }
    if  (!ifname) {
        std::cerr << "No input file specified, use -h for help." << std::endl;
        return 1;
    }
    if  (!search_str && (0 == std::strcmp(mode, "words"))) {
        std::cerr << "Option -v is required in 'words' mode, use -h for help." << std::endl;
        return 1;
    }
    if  (search_str && (0 == std::strcmp(mode, "checksum"))) {
        std::cerr << "Option -v is not allowed in 'checksum' mode, use -h for help." << std::endl;
        return 1;
    }
    
    ifile.open(ifname, std::ios::binary);
    
    if (!ifile.is_open()) {
        std::cerr << "Couldn't open input file." << std::endl;
        return 1;
    }

    if      (0 == std::strcmp(mode, "checksum"))
        std::cout << std::hex << std::setw(sizeof(checksum_t)*2) << std::setfill('0') << checksum(ifile) << std::endl;
    else if (0 == std::strcmp(mode, "words"))
        std::cout << wordcount(ifile, search_str) << std::endl;
    
    ifile.close();
    
    return 0;
}
