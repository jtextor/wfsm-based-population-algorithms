#include <cassert>
#include <cstdlib>
#include <iostream>

#include "dimacs.hpp"
#include "trim.hpp"


sat read_cnf(std::istream &cnf) {
    enum { Comment, Problem, Clauses } state = Comment;
    int nclauses = -1;
    int clauses_read = 0;
    sat result;
    for (std::string line; std::getline(cnf, line);) {
        trim(line);
        if (Comment == state) {
            if (line[0] == 'p') state = Problem;
            else if (line[0] == 'c') continue;
            else assert(false);
        }
        if (Problem == state) {
            assert('p' == line[0]);
            size_t p = line.find("cnf ", 1);
            if (std::string::npos == p) {
                std::cerr << "Expected 'cnf' in problem line of CNF file. Got: " << line << std::endl;
                exit(1);
            }
            char *linep = line.data() + p + 4;
            char *nextp = nullptr;
            result.nvars = strtol(linep, &nextp, 10);
            if (linep == nextp) {
                std::cerr << "Expected an integer in problem line of CNF file. Got: " << linep << std::endl;
                exit(1);
            }
            linep = nextp;
            nclauses = strtol(linep, &nextp, 10);
            if (linep == nextp) {
                std::cerr << "Expected an integer in problem line of CNF file. Got: " << linep << std::endl;
                exit(1);
            }
            state = Clauses;
            continue;
        }
        if (Clauses == state) {
            clause c;
            char *linep = line.data();
            for (;;) {
                char *nextp = nullptr;
                int var = strtol(linep, &nextp, 10);
                if (nextp == linep) {
                    std::cerr << "Expected an integer while reading CNF file. Got: " << linep << std::endl;
                    exit(1);
                }
                if (!var) {
                    break;
                }
                c.push_back(var);
                linep = nextp;
            }
            result.clauses.push_back(c);
            if (++clauses_read == nclauses) {
                break;
            }
        }
    }
    return result;
}