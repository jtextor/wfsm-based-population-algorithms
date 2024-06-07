#include <cassert>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

#include <fsacs/fsa.hpp>
#include <fsacs/population.hpp>
#include <fsacs/types.hpp>

#include "include/negsel_common.hpp"


using namespace fsacs;


template <class C>
static bool not_eof(C c)
{
    return std::char_traits<C>::not_eof(c);
}

void print(const fsacs::Population &pop)
{
    for (auto piter = pop.extract(); !piter->is_done(); piter->advance()) {
        const auto c = piter->value();
        std::cout << c << std::endl;
    }
}

Alphabet::Alphabet(std::fstream &strm, bool update)
    : strm_{strm}
    , chars_{}
    , update_{update}
{
    while (!strm_.eof()) {
        if (!strm_) {
            throw std::runtime_error("Error while reading alphabet file");
        }
        char c;
        strm_ >> c;
        if ('\n' != c && not_eof(c)) {
            chars_.insert({c, chars_.size()});
        } else {
            strm_.unget();
            break;
        }
    }
    strm_.clear();
}

std::optional<unsigned char> Alphabet::index(char c)
{
    using std::cend;
    auto it = chars_.find(c);

    if (cend(chars_) == it) {
        if (update_) {
            const unsigned char idx = chars_.size();
            chars_.insert({c, idx});
            strm_ << c;
            strm_.flush();
            return {idx};
        } else {
            return {};
        }
    } else {
        return {it->second};
    }
}

unsigned char Alphabet::nchars() const
{
    return chars_.size();
}

bool Alphabet::update() const
{
    return update_;
}


static inline constexpr unsigned long long
nr(unsigned long long pos, unsigned long long hits, unsigned length) { return hits*(1 + length) + pos; }


static bool
impl_construct_rcont(
    const char *word,
    Alphabet &alpha,
    Population &pop,
    unsigned length,
    unsigned threshold
)
{
    using ull = unsigned long long;

    FSA fsa{};

    for (unsigned pos = 0; pos < length; ++pos) {
        const auto maybe_current_symbol = alpha.index(word[pos]);
        if (!maybe_current_symbol.has_value()) {
            return false;
        }
        const auto current_symbol = *maybe_current_symbol;

        const unsigned hits_s = pos + threshold >= length ? pos + threshold - length : 0U;
        const unsigned hits_t = std::min(1 + pos, threshold);

        for (unsigned hits=hits_s; hits < hits_t; ++hits) {
            ull nr_from = nr(pos, hits, length);
            ull match_to = nr(1 + pos, 1 + hits, length);
            ull mismatch_to = nr(1 + pos, 0, length);

            for (char c = 0; c < alpha.nchars(); ++c) {
                if (current_symbol == c) {
                    fsa.add_edge(nr_from, match_to, c);

                } else {
                    fsa.add_edge(nr_from, mismatch_to, c);
                }
            }
        }
        if (threshold <= pos) {
            ull succeeded_from = nr(pos, threshold, length);
            ull succeeded_to = nr(1 + pos, threshold, length);

            for (char c = 0; c < alpha.nchars(); ++c) {
                fsa.add_edge(succeeded_from, succeeded_to, c);
            }
        }
    }
    ull final = nr(length, threshold, length);
    fsa.set_final(final);

    pop = fsa.to_population().value();
    return true;
}


static bool
impl_construct_hamming(
    const char *word,
    Alphabet &alpha,
    Population &pop,
    unsigned length,
    unsigned threshold
)
{
    using ull = unsigned long long;

    FSA fsa{};

    for (unsigned pos = 0; pos < length; ++pos) {
        const auto maybe_current_symbol = alpha.index(word[pos]);
        if (!maybe_current_symbol.has_value()) {
            return false;
        }
        const auto current_symbol = *maybe_current_symbol;

        const unsigned hits_s = pos + threshold >= length ? pos + threshold - length : 0U;
        const unsigned hits_t = std::min(1 + pos, threshold);

        for (unsigned hits=hits_s; hits < hits_t; ++hits) {
            ull nr_from = nr(pos, hits, length);
            ull match_to = nr(1 + pos, 1 + hits, length);
            ull mismatch_to = nr(1 + pos, 0, length);

            for (char c = 0; c < alpha.nchars(); ++c) {
                if (current_symbol == c) {
                    fsa.add_edge(nr_from, match_to, c);

                } else {
                    fsa.add_edge(nr_from, mismatch_to, c);
                }
            }
        }
        if (threshold <= pos) {
            ull succeeded_from = nr(pos, threshold, length);
            ull succeeded_to = nr(1 + pos, threshold, length);

            for (char c = 0; c < alpha.nchars(); ++c) {
                fsa.add_edge(succeeded_from, succeeded_to, c);
            }
        }
    }
    ull final = nr(length, threshold, length);
    fsa.set_final(final);

    pop = fsa.to_population().value();
    return true;
}


fsabuilder construct_rcont = impl_construct_rcont;

fsabuilder construct_hamming = impl_construct_hamming;


OpenFile::OpenFile(const std::string &fnm, std::fstream &ofs, std::ios::openmode mode)
    : ofs_{&ofs}
{
    ofs_->open(fnm, mode);
    if (!ofs_->is_open()) {
        throw std::runtime_error("Failed to open file");
    } else if (!ofs_->good()) {
        throw std::runtime_error("Failure after opening file");
    }
}

OpenFile::~OpenFile()
{
    if (ofs_) {
        ofs_->close();
    }
}
