#include "sat.hpp"

static int find_var(const std::vector<int> &lits, int var) {
    for (const auto &lit : lits) {
        if (lit == var) {
            return 1;
        } else if (lit == -var) {
            return -1;
        }
    }
    return 0;
}

ratfa satisfy(const std::vector<int> &lits, int nvars) {
    /** MAYBE: Construct these directly instead? */
    ratfa all_truth_assignments = all<ratfa>(nvars, '0', '1');
    ratfa falsifiers = falsify(lits, nvars);
    return setminus(all_truth_assignments, falsifiers);
}

ratfa falsify(const std::vector<int> &lits, int nvars) {
    ratfa result;
    result.ReserveStates(1 + nvars);
    state<ratfa> s = result.AddState();
    result.SetStart(s);
    for (int var=1; var<=nvars; ++var) {
        state<ratfa> t = result.AddState();
        int sign = find_var(lits, var);
        if (!sign) {
            // If the variable is not in the literals, then it does not
            // constrain the truth assigments.  Therefore it does not matter
            // which value the variable takes for satisfying/falsifying the
            // clause.
            result.ReserveArcs(s, 2);
            result.AddArc(s, ratarc('0', '0', ratweight::One(), t));
            result.AddArc(s, ratarc('1', '1', ratweight::One(), t));
        } else {
            // If the variable does occur in the literals, then for
            // falsifying the clause it HAS to take the negated value of
            // the literal by De Morgan's laws.
            int isnegative = 0 > sign ? 1 : 0;
            char label = '0' + isnegative;
            result.AddArc(s, ratarc(label, label, ratweight::One(), t));
        }
        s = t;
    }
    result.SetFinal(s, ratweight::One());
    return result;
}