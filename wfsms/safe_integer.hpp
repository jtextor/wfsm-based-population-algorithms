#ifndef SAFE_INTEGER_HPP
#define SAFE_INTEGER_HPP


#include <iosfwd>
#include <limits>
#include <stdexcept>


/*
 * Implement enough operations for this class to work with Boost
 * rationals, but throw on operations that would overflow.
 */
class SafeInt
{
public:
    using integer = long long;

    static constexpr long long intmax() { return std::numeric_limits<integer>::max(); }
    static constexpr long long intmin() { return std::numeric_limits<integer>::min(); }

    constexpr SafeInt(integer n)
        : val_{n}
    { }
    constexpr SafeInt(const SafeInt &s)
        : SafeInt{s.val_}
    { }
    constexpr SafeInt()
        : SafeInt{0}
    { }

    constexpr integer val() const {
        return val_;
    }

    constexpr SafeInt operator+(const integer &n) const {
        const integer limit = intmax() - n;
        if (limit < val_) {
            throw std::overflow_error("Integer overflow");
        }
        return {val_ + n};
    }
    constexpr SafeInt& operator+=(const integer &n) {
        *this = *this + n;
        return *this;
    }

    constexpr SafeInt operator-(const integer &n) const {
        const integer limit = intmin() + n;
        if (limit > val_) {
            throw std::overflow_error("Integer overflow");
        }
        return {val_ - n};
    }
    constexpr SafeInt& operator-=(const integer &n) {
        *this = *this - n;
        return *this;
    }

    constexpr SafeInt operator*(const integer &n) const {
        if (!val_ || !n) {
            return {0};
        }
        const integer limit = intmax() / n;
        if (limit < val_) {
            throw std::overflow_error("Integer overflow");
        }
        return {val_ * n};
    }
    constexpr SafeInt& operator*=(const integer &n) {
        *this = *this * n;
        return *this;
    }

    constexpr SafeInt operator/(const integer &n) const {
        if (intmin() == val_ && -1 == n) {
            throw std::overflow_error("Integer overflow");
        }
        return {val_ / n};
    }
    constexpr SafeInt& operator/=(const integer &n) {
        *this = *this / n;
        return *this;
    }

    constexpr SafeInt operator%(const integer &n) const {
        if (intmin() == val_ && -1 == n) {
            throw std::overflow_error("Integer overflow");
        }
        return {val_ % n};
    }
    constexpr SafeInt& operator%=(const integer &n) {
        *this = *this % n;
        return *this;
    }

    constexpr SafeInt operator+(const SafeInt &s) const {
        return {*this + s.val_};
    }
    constexpr SafeInt& operator+=(const SafeInt &s) {
        *this = *this + s.val_;
        return *this;
    }

    constexpr SafeInt operator-(const SafeInt &s) const {
        return {*this - s.val_};
    }
    constexpr SafeInt& operator-=(const SafeInt &s) {
        *this = *this - s;
        return *this;
    }

    constexpr SafeInt operator*(const SafeInt &s) const {
        return {*this * s.val_};
    }
    constexpr SafeInt& operator*=(const SafeInt &s) {
        *this = *this * s;
        return *this;
    }

    constexpr SafeInt operator/(const SafeInt &s) const {
        return {*this / s.val_};
    }
    constexpr SafeInt& operator/=(const SafeInt &s) {
        *this = *this / s;
        return *this;
    }

    constexpr SafeInt operator%(const SafeInt &s) const {
        return {*this % s.val_};
    }
    constexpr SafeInt& operator%=(const SafeInt &s) {
        *this = *this % s;
        return *this;
    }

    constexpr SafeInt operator+() const {
        return {+val_};
    }
    constexpr SafeInt operator-() const {
        return {-val_};
    }

    constexpr bool operator!() const {
        return !val_;
    }

    constexpr bool operator==(const SafeInt &s) const {
        return val_ == s.val_;
    }
    constexpr bool operator!=(const SafeInt &s) const {
        return val_ != s.val_;
    }
    constexpr bool operator<(const SafeInt &s) const {
        return val_ < s.val_;
    }
    constexpr bool operator<=(const SafeInt &s) const {
        return val_ <= s.val_;
    }
    constexpr bool operator>(const SafeInt &s) const {
        return val_ > s.val_;
    }
    constexpr bool operator>=(const SafeInt &s) const {
        return val_ >= s.val_;
    }

    void read(std::istream&);
    void write(std::ostream&) const;

private:
    long long val_;
};


constexpr SafeInt operator+(const SafeInt::integer &n, const SafeInt &s) {
    return {s + n};
}
constexpr SafeInt operator-(const SafeInt::integer &n, const SafeInt &s) {
    return {n - s.val()};
}
constexpr SafeInt operator*(const SafeInt::integer &n, const SafeInt &s) {
    return {s * n};
}
constexpr SafeInt operator/(const SafeInt::integer &n, const SafeInt &s) {
    if (SafeInt::intmin() == n && -1 == s.val()) {
        throw std::overflow_error("Integer overflow");
    }
    return {n / s.val()};
}
constexpr SafeInt operator%(const SafeInt::integer &n, const SafeInt &s) {
    if (SafeInt::intmin() == n && -1 == s.val()) {
        throw std::overflow_error("Integer overflow");
    }
    return {n % s.val()};
}


std::istream& operator>>(std::istream&, SafeInt&);
std::ostream& operator<<(std::ostream&, const SafeInt&);


template<> class std::numeric_limits<SafeInt> : public std::numeric_limits<SafeInt::integer>{};


#endif /* ndef SAFE_INTEGER_HPP */