#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>

#include <fsacs/fsa.hpp>
#include <fsacs/language.hpp>
#include <fsacs/population.hpp>

#include "include/negsel_common.hpp"


using namespace fsacs;


constexpr unsigned nmeasurements = 5;


static void measure_union_duration(Population&, const Population&, unsigned);
static void read_chunks(std::istream &, std::vector<std::string> &, unsigned, unsigned);
static void read_sliding(std::istream &, std::vector<std::string> &, unsigned, unsigned);
static bool visited(unsigned);


template <class T>
static constexpr T max_of(const T& x) { return std::numeric_limits<T>::max(); }


class Config
{
public:
    bool sliding; /* as opposed to chunk */
    unsigned length;
    bool measure;
    unsigned threshold;
    unsigned n_training_strings;
    bool dont_invert;
    bool enumerate;
    bool push;
    bool quantize;
    bool encode;

    const char *alphabet_file;
    const char *input_words;

    fsabuilder build_fsa; /* presently Hamming or r-contiguous */

    static constexpr char usage[] =
        "Usage: negsel_train"
        " [-h]"
        " [-c|-s] [-d|-r]"
        " [-b]"
        " [-e]"
        " [-m]"
        " [-p]"
        " [-q]"
        " [-v]"
        " -a <alphabet_file>"
        " -l <length>"
        " -n <number>"
        " -i <input_words>"
        " -t <threshold>";

    static constexpr char help[] =
        "negsel_train\n\n"
        "Flags:\n"
        "\t-h: Display this message.\n"
        "\t-c or -s: (c)hunk or (s)liding window. Default: -c\n"
        "\t-d or -r: Hamming (d)istance or (r)-contiguous. Default: -r\n"
        "\t-b: Whether to sequentialize encoded weights.\n"
        "\t-a: Use this alphabet if the file exists. Otherwise, write the alphabet to here.\n"
        "\t-e: Enumerate (=count) nstates and nedges at every union.\n"
        "\t-i <input_words>: Self set in lines. Processed in chunks or sliding window.\n"
        "\t-l <length>: Word length. Number of characters in a word.\n"
        "\t-m: Measure folding time and save highest fold.\n"
        "\t-n: Number of strings to read from stdin for training.\n"
        "\t-p: Do not Push before minimizing.\n"
        "\t-q: Do not Quantize before minimizing.\n"
        "\t-t <threshold>: r in r-contiguous or max Hamming distance.\n"
        "\t-v: Don't invert -- thereby performing positive selection.\n";

    static Config
    from_args(int argc, const char *argv[])
    {
        bool b = false;
        bool c = false;
        bool d = false;
        bool e = false;
        bool m = false;
        unsigned l = max_of(l);
        bool p = true;
        bool q = false;
        bool r = false;
        bool s = false;
        unsigned t = max_of(t);
	unsigned n = 1;
        bool v = true;
        const char *a = nullptr;
        const char *i = nullptr;

        int flag;
        opterr = 0;

        while ((flag = getopt(argc, (char**)argv, "bcdehmpqrsva:l:r:i:t:n:")) != -1)
        {
            switch (flag)
            {
                case 'a':
                    a = optarg;
                    break;
                case 'b':
                    b = true;
                    break;
                case 'c':
                    c = true;
                    break;
                case 'e':
                    e = true;
                    break;
                case 'h':
                    std::cout << help << std::endl;
                    exit(0);
                case 'i':
                    i = optarg;
                    break;
                case 'l':
                    l = std::stoul(optarg);
                    break;
                case 'm':
                    m = true;
                    break;
                case 'n':
                    n = std::stoul(optarg);
                    break;
                case 'p':
                    p = false;
                    break;
                case 'q':
                    q = false;
                    break;
                case 'r':
                    r = true;
                    break;
                case 's':
                    s = true;
                    break;
                case 't':
                    t = std::stoull(optarg);
                    break;
                case 'v':
                    v = false;
                    break;
                case '?':
                    std::cerr
                        << "Unkown option or requires argument: "
                        << static_cast<char>(optopt)
                        << std::endl;
                    std::cerr << usage << std::endl;
                    exit(1);
                default:
                    std::cerr
                        << "Reached a branch that should be unreachable."
                        << " Please file a bug report."
                        << std::endl;
                    abort();
            }
        }
        if (optind < argc) {
            std::cerr << "Too many arguments. Arguments left:" << std::endl;
            for (int i=optind; i < argc; ++i) {
                std::cerr << argv[i] << " ";
            }
            std::cerr << std::endl;
            std::cerr << usage << std::endl;
            exit(1);
        }
        if (max_of(l) == l || max_of(t) == t ) {
            std::cerr << "Missing one or more required arguments" << std::endl;
            std::cerr << usage << std::endl;
            exit(1);
        }
        if ((c && s) || (d && r)) {
            std::cerr
                << "Mutually exclusive options given."
                << " The -c and -s flags are mutually exlusive."
                << " So are -d and -r. "
                << " Please give at most one of both pairs."
                << std::endl;
            exit(1);
        }

        return Config{s, d, l, b, e, m, p, q, t, v, a, i, n};
    }

private:
    Config(
        bool s, bool d, unsigned l, bool b, bool e, bool m, bool p, bool q,
        unsigned t, bool v, const char *a, const char *i, unsigned n
    )
        : sliding{s}
        , build_fsa{construct_rcont}
        , length{l}
        , encode{b}
        , enumerate{e}
        , measure{m}
        , push{p}
        , quantize{q}
        , threshold{t}
        , dont_invert{v}
        , alphabet_file{a}
        , input_words{i}
        , n_training_strings{n}
    {
        Diagnostics::set_determinize_encode(b);
        Diagnostics::set_do_push(p);
        Diagnostics::set_do_quantize(q);
    }
};


static std::optional<Config> conf{};

static std::optional<Alphabet> alpha{};


static const Config& config() { return conf.value(); }


static void construct(const std::string &word, Population &pop)
{
    const Config& c = config();

    bool all_legal_characters = c.build_fsa(
        word.c_str(),
        *alpha,
        pop,
        c.length,
        c.threshold
    );

    if (!alpha->update() && !all_legal_characters) {
        std::cerr
            << "This word contains a character that is not in the alphabet file: "
            << word
            << std::endl;
        exit(1);
    }
}


/** Ensures a substitution error in fold if the passed iterator tag isn't random access. */
static inline void require_random_access_iterator(std::random_access_iterator_tag){}


template< class RAIt >
static void
fold(RAIt first, RAIt last, Population &pop, unsigned depth=0)
{
    require_random_access_iterator(typename std::iterator_traits<RAIt>::iterator_category{});
    using diff_t = typename std::iterator_traits<RAIt>::difference_type;

    diff_t nwords = last - first;

    switch (nwords)
    {
    case 0:
        pop = Population{};
        break;

    case 1:
        construct(*first, pop);
        break;

    default:
        RAIt mid = first + (nwords / 2);

        Population left{};
        fold(first, mid, left, 1+depth);

        Population right{};
        fold(mid, last, right, 1+depth);


        left += right;
        pop = std::move(left);
    }
}


static Population 
process(std::istream &f_in, unsigned nstrings)
{
    using std::cbegin;
    using std::cend;

    std::vector< std::string > words{};
    Population pos{};
    const Config& c = config();

    if (c.sliding) {
        read_sliding(f_in, words, c.length, config().n_training_strings);
    } else {
        read_chunks(f_in, words, c.length, config().n_training_strings);
    }

    fold(cbegin(words), cend(words), pos);

    return pos;
}

static void
process_classify(std::istream &f_inw, Population repertoire, std::ostream &f_out)
{
    using std::cbegin;
    using std::cend;

    unsigned increment;
    const Config& c = config();
    //const Population repertoire{read_pop(f_int)};

    if (c.sliding) {
        increment = 1;
    } else {
        increment = c.length;
    }

    Population negativerepertoire;
    if( !c.dont_invert ){
        const Language lan{c.length, static_cast<chartype>(alpha->nchars())};
	negativerepertoire = Population::whole_repertoire(lan) - repertoire;
    }

    unsigned linenr = 0;
    for (std::string line{}; std::getline(f_inw, line); ++linenr) {

        for (unsigned i=0; i < (1 + line.length() - c.length); i += increment) {
            std::string word = line.substr(i, c.length);
            Population matching{};

            construct(word, matching);
            matching *= repertoire;

            Stats s{matching.compute_stats()};

            f_out
                //<< linenr
                //<< ","
                << s.n()
                << "," << s.sumweights();

	    if( !c.dont_invert ){
		construct(word, matching);
		matching *= negativerepertoire;
		f_out << "," << matching.compute_stats().n();
	    }

	    f_out << std::endl;
        }
    }
}

static void
read_chunks(std::istream &f_in, std::vector<std::string> &words, unsigned wlen, unsigned nstrings)
{
    for (std::string line{}; (nstrings-- > 0) && std::getline(f_in, line);) {
        for (unsigned i = 0; i < (1 + line.length() - wlen); i += wlen) {
            std::string word = line.substr(i, wlen);
            words.push_back(std::move(word));
        }
    }
}


static void
read_sliding(std::istream &f_in, std::vector<std::string> &words, unsigned wlen, unsigned nstrings)
{
    for (std::string line{}; (nstrings-- > 0) && std::getline(f_in, line);) {

        for (unsigned i = 0; i < (1 + line.length() - wlen); ++i) {
            std::string word = line.substr(i, wlen);
            words.push_back(std::move(word));
        }
    }
}

static bool
visited(unsigned depth)
{
    static std::vector<bool> visited{};

    if (visited.size() <= depth) {
        visited.resize(1 + depth, false);
    }

    bool result = visited[depth];
    visited[depth] = true;

    return result;
}


int main(int argc, const char *argv[])
{
    using std::ios;

    conf.emplace(Config::from_args(argc, argv));
    std::fstream f_alpha;

    ios::openmode alpha_mode;
    bool update;
    struct stat buffer;

    const Config& c = config();
    if (0 == stat(c.alphabet_file, &buffer)) {
        alpha_mode = ios::in;
        update = false;
    } else if (ENOENT == errno) {
        alpha_mode = ios::out;
        update = true;
    } else {
        perror("Error while finding alphabet file:");
        return 1;
    }

    // Read alphabet file
    OpenFile f_alpha_guard{c.alphabet_file, f_alpha, alpha_mode};


    // Train classifier
    alpha.emplace(f_alpha, update);
    Population p = process( std::cin , 2 );

    //std::cout << p.compute_stats().n() << std::endl;
    //print(p);

    //OpenFile f_class_guard{"test.txt", f_class, ios::in}; 

    // Classify
    process_classify( std::cin, p, std::cout ); 

    return 0;
}
