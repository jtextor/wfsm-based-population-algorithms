#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "trim.hpp"
#include "wfsms_fwd.hpp"

static ratfa learn(int first, int last, long r, long n) {
    std::string line;
    std::vector<ratfa> automata;
    for (long i = 0; i<n/*  && std::getline(std::cin, line) */; ++i) {
        std::getline(std::cin, line);
        trim(line);
        automata.push_back(contiguous<ratfa>(line, -r, first, last));
    }
    biguplus(&automata);
    return automata[0];
}

static void classify(const ratfa &rep, int first, int last, long r) {

    std::string line;
    while (std::getline(std::cin, line)) {
        trim(line);
        ratfa intersection = capdot(rep, contiguous<ratfa>(line, -r, first, last));
        std::cout << count(intersection) << "," << weigh(intersection).as_float() << std::endl;
    }
}

int main(int argc, char **argv) {
    if (5 != argc) {
        std::cerr << "usage: " << argv[0] << " <first char> <nchars> <r> <n>" << std::endl;
        exit(1);
    }
    int first = static_cast<int>(*argv[1]);
    long nchars = strtol(argv[2], nullptr, 10);
    int last = first + nchars - 1;
    long r = strtol(argv[3], nullptr, 10);
    long n = strtol(argv[4], nullptr, 10);

    ratfa repertoire = learn(first, last, r, n);
    classify(repertoire, first, last, r);

    return 0;
}