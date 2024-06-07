#ifndef TERMITER_HPP
#define TERMITER_HPP

#include "ratfa.hpp"


/* A pair of string and weight.  Called 'term' for the analogy between
 * weighed automata and formal power series. */
struct term {
    std::string s;
    ratweight w;
};

class termiter {
    using aiter = fst::ArcIterator<ratfa>;
    std::vector<aiter> s_;
    std::vector<ratweight> w_;
    const ratfa &f_;
    term t_;
public:
    using difference_type = void;
    using value_type = term;
    using pointer = const value_type*;
    using reference = const value_type&;
    using category = std::input_iterator_tag;

    termiter();
    termiter(const ratfa&);
    reference operator*() const;
    pointer operator->() const;
    termiter& operator++();
    termiter operator++(int);
    bool operator==(const termiter&) const;
    bool operator!=(const termiter&) const;
};


namespace fst {

termiter begin(ratfa&);
termiter begin(const ratfa&);
termiter end(ratfa&);
termiter end(const ratfa&);

} // namespace fst


#endif /* !defined(TERMITER_HPP) */