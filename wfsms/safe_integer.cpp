#include <iostream>

#include "safe_integer.hpp"


void SafeInt::read(std::istream &is) {
    integer n;
    is >> n;
    val_ = n;
}


void SafeInt::write(std::ostream &os) const {
    os << val_;
}


std::istream& operator>>(std::istream &is, SafeInt &s) {
    s.read(is);
    return is;
}


std::ostream& operator<<(std::ostream &os, const SafeInt &s) {
    s.write(os);
    return os;
}