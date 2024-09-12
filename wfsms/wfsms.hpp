#ifndef WFSMS_HPP
#define WFSMS_HPP

/* Exports the generic definitions for WFSMs.  The wfsms_fwd.hpp header
 * file should suffice for a lot of use cases, but if you want to define
 * your own weighted automata type then this header helps with implementation
 */

#include "wfsms_fwd.hpp"
#include "wfsms_generic.hpp"

/* forward declarations for the explicit instantiations and
 * specialized template definitions that exist in wfsms.cpp */
template <> boolfa all(int, int, int);
template <> logfa all(int, int, int);
template <> ratfa all(int, int, int);

template <> void bigcapdot(std::vector<boolfa>*, bool);
template <> void bigcapdot(std::vector<logfa>*, bool);
template <> void bigcapdot(std::vector<ratfa>*, bool);

template <> void biguplus(std::vector<boolfa>*, bool);
template <> void biguplus(std::vector<logfa>*, bool);
template <> void biguplus(std::vector<ratfa>*, bool);

template <> boolfa capdot(const boolfa&, const boolfa&, bool);
template <> logfa capdot(const logfa&, const logfa&, bool);
template <> ratfa capdot(const ratfa&, const ratfa&, bool);

template <> boolfa contiguous(const std::string&, int, int, int);
template <> logfa contiguous(const std::string&, int, int, int);
template <> ratfa contiguous(const std::string&, int, int, int);

template <> unsigned long count(const boolfa&);
template <> unsigned long count(const logfa&);
template <> unsigned long count(const ratfa&);

template <> boolfa determinize(boolfa*);
template <> logfa determinize(logfa*);
template <> ratfa determinize(ratfa*);

template <> boolfa hamming(const std::string&, int, int, int);
template <> logfa hamming(const std::string&, int, int, int);
template <> ratfa hamming(const std::string&, int, int, int);

template <> boolfa insert(const boolfa&, const boolfa&, bool);
template <> logfa insert(const logfa&, const logfa&, bool);
template <> ratfa insert(const ratfa&, const ratfa&, bool);

template <> void minimize(boolfa*, bool);
template <> void minimize(logfa*, bool);
template <> void minimize(ratfa*, bool);

template <> boolfa setminus(const boolfa&, const boolfa&, bool, bool);
template <> logfa setminus(const logfa&, const logfa&, bool, bool);
template <> ratfa setminus(const ratfa&, const ratfa&, bool, bool);

template <> void sortarcs(boolfa*);
template <> void sortarcs(logfa*);
template <> void sortarcs(ratfa*);

template <> boolfa singleton(const std::string&);
template <> logfa singleton(const std::string&);
template <> ratfa singleton(const std::string&);

template <> boolfa uplus(const boolfa&, boolfa*, bool);
template <> logfa uplus(const logfa&, logfa*, bool);
template <> ratfa uplus(const ratfa&, ratfa*, bool);

template <> boolweight weigh(const boolfa&);
template <> logweight weigh(const logfa&);
template <> ratweight weigh(const ratfa&);

#endif /* !defined(WFSMS_HPP) */