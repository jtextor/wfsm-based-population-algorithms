#ifndef NEGSEL_COMMON_HPP
#define NEGSEL_COMMON_HPP


#include <iosfwd>
#include <optional>
#include <unordered_map>


namespace fsacs {
    class Population;
} // namespace fsacs


class Alphabet
{
public:
    Alphabet(std::fstream &, bool);

    std::optional<unsigned char> index(char);
    unsigned char nchars() const;
    bool update() const;
private:
    std::fstream &strm_;
    std::unordered_map<char, unsigned char> chars_;
    bool update_;
};


using fsabuilder = bool (*) (const char*, Alphabet&, fsacs::Population&, unsigned, unsigned);

extern fsabuilder construct_hamming;
extern fsabuilder construct_rcont;

extern void print(const fsacs::Population &pop);

class OpenFile
{
public:
    constexpr OpenFile()
        : ofs_{nullptr}
    { }

    OpenFile(const std::string &, std::fstream &, std::ios::openmode);

    ~OpenFile();

private:
    std::fstream *ofs_;
};


#endif /* ndef NEGSEL_COMMON_HPP */
