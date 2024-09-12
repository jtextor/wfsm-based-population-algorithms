#ifndef DIMACS_HPP
#define DIMACS_HPP

#include <vector>

using clause = std::vector<int>;

struct sat {
    int nvars;
    std::vector<clause> clauses;
};

sat read_cnf(std::istream&);

#endif /* !defined(DIMACS_HPP) */