#ifndef RATFA_HPP
#define RATFA_HPP

#include <string>

#include <fst/vector-fst.h>

#include "rational_weight.hpp"


using ratweight = fsacs::RationalWeight;
using ratarc = fst::ArcTpl<ratweight>;
using ratfa = fst::VectorFst<ratarc>;



/* Convention for parameters that are passed by reference to functions
 * in this file:
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
ratfa all(int, int, int);

/* Pairwise weighted intersect (see function capdot(...)) of all automata
 * in vector until only one automaton remains. */
void bigcapdot(std::vector<ratfa>*, bool=false);

/* Pairwise weighted union (see function uplus(...)) of all automata
 * in vector until only one automaton remains. */
void biguplus(std::vector<ratfa>*, bool=false);

/* Return minimized weighted intersection between two weighted automata.
 * This performs effective multiset multiplication on their contents.
 * Pass true for the optional bool parameter to push weights before
 * minimization. */
ratfa capdot(const ratfa&, const ratfa&, bool=false);

/* Return automaton containing all strings that share a stretch of
 * contiguous bits with the passed center string.
 * First integer parameter is radius -- maximum number of non-matching
 * bits.  A negative value for this integer parameter is
 *  added to center string length, obtaining minimum number of contiguously
 *  matching bits.
 * Second and third integer parameters are the start and end of the
 * character range, inclusive on both ends.
 *
 * Output automaton is deterministic and sorted. */
ratfa contiguous(const std::string&, int, int, int);

/* Return number of distinct strings in automaton. */
unsigned long count(const ratfa&);

/* Return deterministic version of parameter automaton.
 * Any epsilons in the input automaton are removed in-place. */
ratfa determinize(ratfa*);

/* Is the automaton empty? */
bool empty(const ratfa&);

/* Return the weighted automaton that recognizes the input string with
 * weight 1. */
ratfa exact(const std::string&);

/* Write automaton to file in binary format.  Wrapper around
 * ratfa.Write(...) that is callable from gdb. */
void fawrite(const ratfa&, std::ostream&);
void fawrite(const ratfa&, const std::string&);
void fawrite(const ratfa&, const char*);

/* Return automaton containing the Hamming ball around center string.
 * First integer parameter is Hamming ball radius -- maximum Hamming
 *  distance wrt center.  A negative value for this integer parameter is
 *  added to center string length to obtain the Hamming ball radius.
 * Second and third integer parameters are the start and end of the
 * character range, inclusive on both ends.
 *
 * Output automaton is deterministic and sorted. */
ratfa hamming(const std::string&, int, int, int);

/* Insert strings and weights of right automaton into left automaton,
 * replacing weights in left automaton by weights from right automaton
 * for strings that were already present in the left automaton.  Pass
 * true for the optional bool parameter to push weights before minimization.
 *
 * Assumes the left automaton to be much larger than the right. */
ratfa insert(const ratfa&, const ratfa&, bool=false);

/* Make the parameter automaton smaller in-place, while retaining
 * its contents.  Pass true for the optional bool parameter to push
 * weights before minimization. */
void minimize(ratfa*, bool=false);

/* Print contents of automaton in CSV to stream. */
void print(const ratfa&, std::ostream&);

/* Remove strings from automaton.  Pass true for the optional bool
 * parameter to push weights before minimization. */
ratfa prune(const ratfa&, float, bool=false);

/* Sample a string from the weighted automaton with frequency
 * proportional to string weight in automaton. */
std::string sample(const ratfa&);

/* Remove strings from left automaton that are contained in the right
 * automaton.  Weights from the remaining strings in the left automaton
 * retain their weight.  Right automaton should be deterministic and
 * have its arcs sorted.
 * Pass true for the first optional bool parameter to sort arcs of right
 * automaton.  Pass true for the second optional bool parameter to push
 * weights of result before minimization. */
ratfa setminus(const ratfa&, const ratfa&, bool=false, bool=false);

/* Sort arcs in-place.  "Arcs" is a short synonym for transitions. */
void sortarcs(ratfa*);

/* Return minimized weighted union of the two parameter automata.
 * This performs effective multiset sum of their contents.
 * Right automaton is mutated to nondeterministic automaton containing this
 * weighted union.
 * Pass true for the optional bool parameter to push weights before
 * minimization. */
ratfa uplus(const ratfa&, ratfa*, bool=false);

/* Returns weight of a string in automaton. */
ratweight weight(const ratfa&, const std::string&);


#endif /* !defined(RATFA_HPP) */