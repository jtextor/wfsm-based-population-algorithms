#include <cassert>
#include <iostream>

#include "trim.hpp"
#include "wfsms_fwd.hpp"

#ifndef FA_TYPE
#define FA_TYPE ratfa
#endif /* defined(FA_TYPE) */
using fatype = FA_TYPE;
#define EXPAND_(M) #M
#define EXPAND(M) EXPAND_(M)

static void report(const fatype &fa) {
    int nstrings = count(fa);
    int nstates = fa.NumStates();
    int narcs = 0;
    for (int s=0; s<nstates; ++s) {
        narcs += fa.NumArcs(s);
    }
    std::cout << nstrings << "," << nstates << "," << narcs << "\n";
}

static void rollover(std::string *s) {
    std::string &sr = *s;
    int i = sr.size();
    do {
        --i;
        if (sr[i] != '2') {
            ++sr[i];
            break;
        } else {
            sr[i] = '0';
        }
    } while (0 < i);
}

int main(int argc, char **argv) {
    if (2 != argc || !strcmp(argv[1], "-h")) {
        // Provide type information for double-checking type.
        std::cerr << "usage: " << argv[0] << " <length>\n"
            "automaton type: " EXPAND(FA_TYPE) "\n"
            "weight type: " << fatype::Weight::Type()
            << std::endl;
        exit(1);
    }
    int length = strtol(argv[1], nullptr, 10);
    assert(0 < length);

    std::vector<fatype> automata;
    std::string s(length, '0');
    std::string t(length, '2');
    std::cout << "nstrings,nstates,narcs\n";
    for (;;) {
        automata.push_back(singleton<fatype>(s));
        report(automata.back());
        if (s != t) {
            rollover(&s);
        } else {
            break;
        }
    }

    std::vector<fatype> folded;
    std::vector<fatype> &a = automata, &b = folded;
    while (1 < a.size()) {
        b.resize(1 + ((a.size() - 1) >> 1));
        for (int i=0; i < (a.size() & (~1ULL)); i += 2) {
            int j = i >> 1;
            b[j] = uplus(a[i], &a[i+1], true);
            report(b.at(j));
        }
        if (a.size() & 1) {
            b.back() = a.back();
        }
        std::swap(a, b);
    }
    return 0;
}