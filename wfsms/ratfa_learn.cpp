#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "trim.hpp"
#include "wfsms_fwd.hpp"


int main(int argc, char **argv) {
    if (4 != argc) {
        std::cerr << "usage: " << argv[0] << " <first char> <nchars> <r>" << std::endl;
        exit(1);
    }
    int first = static_cast<int>(*argv[1]);
    long nchars = strtol(argv[2], nullptr, 10);
    int last = first + nchars - 1;
    long r = strtol(argv[3], nullptr, 10);

    std::vector<ratfa> automata;
    for (std::string line; std::getline(std::cin, line);) {
        trim(line);
        automata.push_back(contiguous<ratfa>(line, -r, first, last));
    }

    if (0 == automata.size()) {
        ratfa().Write(std::cout, fst::FstWriteOptions());
    } else {
        biguplus(&automata);
        automata[0].Write(std::cout, fst::FstWriteOptions());
    }
    return 0;
}