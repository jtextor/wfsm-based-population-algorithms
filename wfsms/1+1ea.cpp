#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

#include "trim.hpp"

using u64 = uint64_t;

struct clause {
    u64 mask;
    u64 constraint;

    constexpr clause() : mask(0), constraint(0) {}
};

static std::mt19937 rng;
static float mu;
static int nbits;
static int indexes[std::numeric_limits<u64>::digits];
static std::ofstream fout;
static std::vector<clause> problem;
static unsigned long long max_seconds;


static u64 random_model();

static inline void clause_add_var(clause *c, int variable, bool positive) {
    c->mask |= 1 << variable;
    c->constraint |= (!positive) << variable;
}

static inline bool clause_eval(const clause &c, u64 model) {
    return c.constraint ^ (c.mask & model);
}

static u64 clause_random_satisfying_model(const clause &c) {
    u64 result;
    do {
        result = random_model();
    } while (!clause_eval(c, result)); // This averages 8/7 repetitions.
    return result;
}

static int eval(u64 canditate_solution) {
    int nsatisfied = 0;
    for (const auto &c : problem) {
        nsatisfied += clause_eval(c, canditate_solution);
    }
    return nsatisfied;
}

static void initialize() {
    std::random_device rd;
    rng.seed(rd());
    for (int i=0; i < std::size(indexes); ++i) {
        indexes[i] = i;
    }
    timespec test;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &test)) {
        if (EINVAL == errno) {
            std::cerr << "Unfortunately your system does not support a per-process CPU-time clock." << std::endl;
        } else {
            perror("A call to clock_gettime(...) was unsuccesful.");
        }
        return exit(1);
    }
}

static u64 initial_model() {
    std::uniform_int_distribution<int> clauseidxgen(0, problem.size()-1);
    int idx = clauseidxgen(rng);
    return clause_random_satisfying_model(problem[idx]);
}

static u64 offspring(u64 parent) {
    std::binomial_distribution<int> nflipgen(nbits, mu);
    int nflips = nflipgen(rng);
    u64 flipbits = 0;
    for (int i = 0; i<nflips; ++i) {
        std::uniform_int_distribution<int> indexgen(i, nbits-1);
        int index = indexgen(rng);
        std::swap(indexes[i], indexes[index]);
        flipbits |= 1 << indexes[i];
    }
    return parent ^ flipbits;
}

static std::vector<clause> read_csv(std::istream &csv) {
    std::vector<clause> result;
    std::string line;
    std::getline(csv, line);
    if (!csv) {
        std::cerr << "Cannot find problem CSV header line." << std::endl;
        std::abort();
    }
    trim(line);
    if (line != "type,string") {
        std::cerr << "Expected problem CSV to have fields 'type' and 'string' in that order." << std::endl;
        std::abort();
    }
    while (std::getline(csv, line)) {
        trim(line);
        std::string type = line.substr(0, 2);
        std::string string = line.substr(2);
        if (type[1] != ',') {
            std::cerr << "Expected type field to be '1', '2', or '3'" << std::endl;
            std::abort();
        }
        if (type[0] == '1') {
            nbits = string.size();
            if (std::numeric_limits<u64>::digits < nbits) {
                std::terminate();
            }
            result.push_back({});
            for (int i=0; i<string.size(); ++i) {
                char l = string[i];
                if ('0' <= l && l <= '1') {
                    clause_add_var(&result.back(), string.size()-i-1, '0' == l);
                }
            }
        } else if (type[0] == '2' || type[0] == '3') {
        } else {
            std::cerr << "Expected type field to be '1', '2', or '3'" << std::endl;
            std::abort();
        }
    }
    return result;
}

static u64 random_model() {
    const int maxbits = std::numeric_limits<u64>::digits;
    u64 result;
    if (maxbits > nbits) {
        std::uniform_int_distribution<u64> dist(0, (1<<nbits)-1);
        result = dist(rng);
    } else if (maxbits == nbits) {
        std::uniform_int_distribution<u64> dist(0, ~0ULL);
        result = dist(rng);
    } else {
        std::terminate();
    }
    return result;
}

static void record_data() {
    std::flush(fout);
    fout.close();
}

static void report(unsigned long long step, u64 model, int eval, std::ostream &os) {
    timespec now;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
    unsigned long long msduration = (1000ULL*now.tv_sec) + (now.tv_nsec/1000000ULL);
    os << step
        << "," << msduration
        << "," << model
        << "," << eval
        << "\n";
    if (max_seconds <= now.tv_sec) {
        exit(0);
    }
}

int main(int argc, char **argv) {
    if (5 > argc || 6 < argc) {
        std::cerr << "usage: " << argv[0] << " <problem csv> <mutation rate> <max user seconds> <out csv> [report modulo]" << std::endl;
        return 1;
    }
    std::ifstream fin(argv[1]);
    mu = std::stof(argv[2]);
    max_seconds = std::strtoull(argv[3], nullptr, 10);
    problem = read_csv(fin);
    fin.close();
    fout.open(argv[4], std::ios::out);
    const unsigned long long report_modulo = 6 <= argc ? std::strtoull(argv[5], nullptr, 10) : 25000000;
    initialize();
    atexit(record_data);

    u64 current_model = initial_model();
    int current_eval = eval(current_model);
    fout << "step,duration_ms,fittest,fitness\n";
    unsigned long long step = 0;
    for (;; ++step) {
        if (0 == (step % report_modulo)) {
            report(step, current_model, current_eval, fout);
        }
        u64 proposed_model = offspring(current_model);
        int proposed_eval = eval(proposed_model);
        if (proposed_eval >= current_eval) {
            current_model = proposed_model;
            current_eval = proposed_eval;
            if (proposed_eval >= problem.size()) {
                break;
            }
        }
    }
    report(step, current_model, current_eval, fout);
    return 0;
}