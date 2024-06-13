#include <cstdlib>
#include <iostream>
#include <string>

#include "trim.hpp"
#include "wfsms_fwd.hpp"

int main(int argc, char **argv) {
    if (5 != argc) {
        std::cerr << "usage: " << argv[0] << " <first char> <nchars> <r> <learned repertoire (.fst)>" << std::endl;
        exit(1);
    }
    int first = static_cast<int>(*argv[1]);
    long nchars = strtol(argv[2], nullptr, 10);
    int last = first + nchars - 1;
    long r = strtol(argv[3], nullptr, 10);
    ratfa repertoire = *ratfa::Read(argv[4]);

    std::cout << "count,weight" << std::endl;
    for (std::string line; std::getline(std::cin, line);) {
        trim(line);
        ratfa intersection = capdot(repertoire, contiguous<ratfa>(line, -r, first, last));
        std::cout << count(intersection) << "," << weigh(intersection).as_float() << std::endl;
    }

    return 0;
}