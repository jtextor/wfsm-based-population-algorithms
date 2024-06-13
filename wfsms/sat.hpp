#ifndef SAT_HPP
#define SAT_HPP

#include <string>

#include "wfsms_fwd.hpp"


/* Functions below take a string that encodes a clause from a 3-CNF
 * problem as a constraint string for a farthest string problem.  These
 * encoded strings have an alphabet of three characters: 0, 1, and 2.
 * Each position in the string corresponds to one variable from the 3-CNF
 * problem and the character in that position indicates the presence and
 * sign of the corresponding variable:
 *
 * - A 0 in the nth position means that the nth variable occurs
 *   positively in the clause.
 * - A 1 in the nth position means that the nth variable occurs
 *   negatively in the clause.
 * - A 2 in the nth position means that the nth variable does not occur
 *   in the clause.
 *
 * Example:
 * (position)         :   1       2   3   4  5   6
 * Constraint string  :   0       1   2   2  0   2
 * 3-CNF clause       : (x1 or  ~x2    or   x5    ) */

/* Construct FSA containing all variable assignments that satisfy the
 * clause encoded by the input string. */
ratfa satisfy(const std::string&);

/* Construct FSA containing all variable assignments that falsify the
 * clause encoded by the input string. */
ratfa falsify(const std::string&);

#endif /* !defined(SAT_HPP) */