#ifndef DIMACS_HPP
#define DIMACS_HPP

#include <vector>

using clause = std::vector<int>;

struct csp {
    int nvars;
    std::vector<clause> clauses;
};

csp read_cnf(std::istream&);

#endif /* !defined(DIMACS_HPP) */