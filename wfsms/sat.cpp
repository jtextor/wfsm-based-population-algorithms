#include "sat.hpp"

ratfa satisfy(const std::string &s) {
    const int l = s.size();
    ratfa f = hamming<ratfa>(s, -3, '0', '2');
    ratfa l2 = all<ratfa>(l, '0', '1');
    ratfa x = setminus(l2, f);
    sortarcs(&x);
    return x;
}

ratfa falsify(const std::string &s) {
    const int l = s.size();
    ratfa f = hamming<ratfa>(s, -1, '0', '2');
    ratfa l2 = all<ratfa>(l, '0', '1');
    return capdot(l2, f);
}