#ifndef WFSMS_FWD_HPP
#define WFSMS_FWD_HPP


/* Forward declarations for WFSMs.  This header does not provide
 * definitions, but the wfsm object does contain definitions for
 * templates contained herein with WA={boolfa,logfa,ratfa}.
 * Therefore this header suffices for when only these three types are
 * used.  Generic definitions can be found in the wfsms.hpp header file.
 *
 * In general, default to only using this header file.  If you get
 * linker errors, use this header file in your own header files and use
 * the wfsm.hpp header file for your source files -- where your
 * implementation lives.
 */


#include <string>

#include <fst/vector-fst.h>

#include "rational_weight.hpp"


/* For weighted automata over the two-element Boolean algebra.  In fact
 * uses the Tropical semiring, by which we perform pruning. */
using boolfa = fst::StdVectorFst;
using boolarc = typename boolfa::Arc;
using boolweight = typename boolfa::Weight;

/* For weighted automata over positive reals.  Log transformation makes
 * it so semiring multiplication is implemented by float addition, which
 * makes operations on automata significantly faster in practice.  Using
 * floats instead with float addition and multiplication makes no impactful
 * difference in automata size explosion due to rounding errors.  Such
 * non-logtransformed weights are only present in newer OpenFST releases
 * anyway. */
using logarc = fst::LogArc;
using logfa = fst::VectorFst<logarc>;
using logweight = typename logarc::Weight;

/* For weighted automata over rationals.  Throws on integer overflow. */
using ratweight = fsacs::RationalWeight;
using ratarc = fst::ArcTpl<ratweight>;
using ratfa = fst::VectorFst<ratarc>;


template <class WA> using arc = typename WA::Arc;
template <class WA> using aiter = fst::ArcIterator<WA>;
template <class WA> using state = typename WA::StateId;
template <class WA> using weight = typename WA::Weight;


/* Convention for parameters that are passed by reference to functions:
 *
 * - If "const type&", then the function does not mutate the referred-to
 *   object externally (though in several cases we may sortarcs the
 *   object anyway).
 * - If "type*" (pointer, not const), then the referred-to object may be
 *   mutated by the function. */

/* Return the weighted automaton recognizing all strings of length $l$,
 * with characters in a range.
 * First parameter: Length; $l$.
 * Second parameter: First character in range, inclusive.
 * Third parameter: Last character in range, inclusive. */
template <class WA> WA all(int, int, int);

/* Pairwise weighted intersect (see function capdot(...)) of all automata
 * in vector until only one automaton remains. */
template <class WA> void bigcapdot(std::vector<WA>*, bool=false);

/* Pairwise weighted union (see function uplus(...)) of all automata
 * in vector until only one automaton remains. */
template <class WA> void biguplus(std::vector<WA>*, bool=false);

/* Return minimized weighted intersection between two weighted automata.
 * This performs effective multiset multiplication on their contents.
 * Pass true for the optional bool parameter to push weights before
 * minimization. */
template <class WA> WA capdot(const WA&, const WA&, bool=false);

/* Return automaton containing all strings that share a stretch of
 * contiguous bits with the passed center string.
 * First integer parameter is radius -- maximum number of non-matching
 * characters.  A negative value for this integer parameter is
 *  added to center string length, obtaining minimum number of contiguously
 *  matching bits.
 * Second and third integer parameters are the start and end of the
 * character range, inclusive on both ends.
 *
 * Output automaton is deterministic and sorted. */
template <class WA> WA contiguous(const std::string&, int, int, int);

/* Return number of distinct strings in deterministic automaton. */
template <class WA> unsigned long count(const WA&);

/* Return deterministic version of parameter automaton.
 * Any epsilons in the input automaton are removed in-place. */
template <class WA> WA determinize(WA*);

/* Return automaton containing the Hamming ball around center string.
 * First integer parameter is Hamming ball radius -- maximum Hamming
 *  distance wrt center.  A negative value for this integer parameter is
 *  added to center string length to obtain the Hamming ball radius.
 * Second and third integer parameters are the start and end of the
 * character range, inclusive on both ends.
 *
 * Output automaton is deterministic and sorted. */
template <class WA> WA hamming(const std::string&, int, int, int);

/* Insert strings and weights of right automaton into left automaton,
 * replacing weights in left automaton by weights from right automaton
 * for strings that were already present in the left automaton.  Pass
 * true for the optional bool parameter to push weights before minimization.
 *
 * Assumes the left automaton to be much larger than the right. */
template <class WA> WA insert(const WA&, const WA&, bool=false);

/* Make the parameter automaton smaller in-place, while retaining
 * its contents.  Pass true for the optional bool parameter to push
 * weights before minimization. */
template <class WA> void minimize(WA*, bool=false);

/* Remove strings from rational-weighted automaton.  Pass true for the
 * optional bool parameter to push weights before minimization. */
ratfa prune(const ratfa&, float, bool=false);

/* Sample a string from the rational-weighted automaton with frequency
 * proportional to string weight in automaton. */
std::string sample(const ratfa&);

/* Remove strings from left automaton that are contained in the right
 * automaton.  Weights from the remaining strings in the left automaton
 * retain their weight.  Right automaton should be deterministic and
 * have its arcs sorted.
 * Pass true for the first optional bool parameter to sort arcs of right
 * automaton.  Pass true for the second optional bool parameter to push
 * weights of result before minimization. */
template <class WA> WA setminus(const WA&, const WA&, bool=false, bool=false);

/* Return the weighted automaton that recognizes the input string with
 * weight 1. */
template <class WA> WA singleton(const std::string&);

/* Sort arcs in-place.  "Arcs" is a short synonym for transitions. */
template <class WA> void sortarcs(WA*);

/* Return minimized weighted union of the two parameter automata.
 * This performs effective multiset sum of their contents.
 * Right automaton is mutated to nondeterministic automaton containing this
 * weighted union.
 * Pass true for the optional bool parameter to push weights before
 * minimization. */
template <class WA> WA uplus(const WA&, WA*, bool=false);

/* Compute the sum -- according to the applicable semiring's addition --
 * of all the string weights contained in the deterministic parameter
 * automaton. */
template <class WA> weight<WA> weigh(const WA&);

#endif /* !defined(WFSMS_FWD_HPP) */