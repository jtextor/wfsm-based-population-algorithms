#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>

#include "ratfa.hpp"
#include "sat.hpp"
#include "termiter.hpp"
#include "trim.hpp"


static constexpr float nearly_one = 1 - 1e-3;
static int max_user_seconds;
static std::vector<ratfa> problem;
static std::ofstream fout;


static ratfa eval(const std::vector<ratfa> &problem, const ratfa &f) {
    std::vector<ratfa> result;
    std::transform(begin(problem), end(problem), std::back_inserter(result),
        [&](const ratfa &p) -> ratfa { return capdot(p, f); });

    biguplus(&result);
    return result.back();
}

static term fittest(const ratfa &f) {
    ratfa g = prune(f, nearly_one);
    return *begin(g);
}

static std::vector<ratfa> read_csv(std::istream &csv) {
    std::vector<ratfa> result;
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
            result.push_back(satisfy(string));
        } else if (!(type[0] == '2' || type[0] == '3')) {
            std::cerr << "Expected type field to be '1', '2', or '3'" << std::endl;
            std::abort();
        }
    }
    return result;
}

static void record_data() {
    std::flush(fout);
    fout.close();
}

static void report(const ratfa &f, std::ostream &os) {
    timespec now;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
    unsigned long long msduration = (1000ULL*now.tv_sec) + (now.tv_nsec/1000000ULL);
    int nstates = f.NumStates();
    unsigned long nstrings = count(f);
    term t = fittest(f);
    os << nstrings
        << "," << nstates
        << "," << msduration
        << "," << t.s
        << "," << t.w.as_float()
        << "\n";
    if (problem.size() <= round(t.w.as_float())) {
        // Exit early, since the problem is solved.
        exit(0);
    }
    if (max_user_seconds <= now.tv_sec) {
        // Hit the time limit.
        exit(0);
    }
}

static ratfa sample_initial(const std::vector<ratfa> &p) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> ranidx(0, p.size()-1);
    int idx = ranidx(rng);
    return eval(p, exact(sample(p[idx])));
}

int main(int argc, char **argv) {
    timespec test;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &test)) {
        if (EINVAL == errno) {
            std::cerr << "Unfortunately your system does not support a per-process CPU-time clock." << std::endl;
        } else {
            perror("A call to clock_gettime(...) was unsuccesful.");
        }
        return 1;
    }
    if (6 != argc) {
        std::cerr << "usage: waga <problem csv> <max user seconds> <radius> <prunefract> <outcsv>" << std::endl;
        return 1;
    }
    std::ifstream fprob(argv[1]);
    max_user_seconds = std::stoi(argv[2]);
    int radius = std::stoi(argv[3]);
    float prunefract = std::stof(argv[4]);
    problem = read_csv(fprob);
    fprob.close();
    fout.open(argv[5], std::ios::out);

    if (1 <= prunefract || 0 > prunefract) {
        std::cerr << "Pruning fraction should be in [0, 1). Was: " << prunefract << std::endl;
        return 1;
    }
    ratfa population = sample_initial(problem);
    int push_modulo = 20;
    atexit(record_data);
    fout << "nstrings,nstates,duration_ms,fittest,fitness\n";
    report(population, fout);
    for (int nsteps=0;; ++nsteps) {
        std::string s = sample(population);
        ratfa ball = hamming(s, radius, '0', '1'); /* contiguous(s, radius, '0', '1'); */
        ratfa neig = eval(problem, ball);
        population = insert(population, neig);
        population = prune(population, prunefract, 0 == (nsteps % push_modulo));
        report(population, fout);
    }
    return 0;
}