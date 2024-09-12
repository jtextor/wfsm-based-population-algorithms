#ifndef RATIONAL_WEIGHT_HPP
#define RATIONAL_WEIGHT_HPP


#include <cmath>

#include <boost/rational.hpp>
#include <fst/fstlib.h>

#include "safe_integer.hpp"

namespace fsacs {


class RationalWeight
{
public:
    using I = SafeInt;
    using R = boost::rational<I>;
    using ValueType = R;
    using ReverseWeight = RationalWeight;

    static constexpr I default_lim() { return {1 << 14}; }

    /**
     * NOTE:
     * We can't constexpr anything if we intend to support Boost < 1.69,
     * which we do, because RedHat Linux in manylinux2014 supplies 1.53 in
     * its boost-devel package.
     */
    RationalWeight(I, I, bool);
    RationalWeight(I, I);
    RationalWeight(const R&);
    RationalWeight(I);
    RationalWeight() noexcept;

    /* For obtaining useful representations of the value */
    I::integer numerator() const;
    I::integer denominator() const;
    float as_float() const;

    /* All other methods in the class are for OpenFST's benefit. */

    static RationalWeight Zero();
    static RationalWeight One();
    static RationalWeight NoWeight();

    static constexpr uint64_t Properties() {
        using namespace fst;
        return kLeftSemiring | kRightSemiring | kCommutative;
    }

    constexpr bool Member() const { return !bad_; }

    RationalWeight Quantize(float) const;

    ReverseWeight Reverse() const;

    std::istream &Read(std::istream&);

    static const std::string &Type();

    std::ostream &Write(std::ostream &strm) const;

    size_t Hash() const;

    const ValueType &Value() const;

protected:
    void SetValue(const R&);

private:
    R value_;
    bool bad_;
};


bool operator==(const RationalWeight&, const RationalWeight&);
bool operator!=(const RationalWeight&, const RationalWeight&);


inline std::ostream& operator<<(
    std::ostream &strm,
    const RationalWeight &w) {

    return w.Write(strm);
}


inline std::istream& operator>>(
    std::istream &strm,
    RationalWeight &w) {

    RationalWeight::R x;
    strm >> x;
    w = RationalWeight{x};
    return strm;
}


RationalWeight Plus(const RationalWeight&, const RationalWeight &);
RationalWeight Minus(const RationalWeight&, const RationalWeight&);
RationalWeight Times(const RationalWeight&, const RationalWeight&);
RationalWeight Divide(
    const RationalWeight&,
    const RationalWeight&,
    fst::DivideType = fst::DIVIDE_ANY);

bool ApproxEqual(const RationalWeight&, const RationalWeight&, float);


template <class F>
RationalWeight
convert_farey(F x, const typename RationalWeight::I& ilim)
{
    using R = typename RationalWeight::R;
    using I = typename RationalWeight::I;

    I::integer lim = ilim.val();
    bool spot_on = false;
    I ln = 0;
    I ld = 1;
    I rn = 1;
    I rd = 1;
    I yn = 1;
    I yd = 2;

    // Detect bad values
    if (std::isnan(x) || std::isinf(x)) {
        return RationalWeight::NoWeight();
    }

    const F mod = fmod(x, 1.0);
    const I::integer quot = std::llround(x - mod);
    // The distance between neighbors in the n-th order Farey sequence
    // is at least 1 / (n * (n-1)).
    const F eps = 0.5 / (lim * (lim-1.0));

    // Exclude values closer to zero and one than we can approximate
    // otherwise.
    if (mod <= (0.5/lim)) {
        return R(quot);
    } else if ((1.0 - (0.5/lim)) <= mod) {
        return R(1 + quot);
    }

    while (yd <= lim) {
        F yf = static_cast<F>(yn.val()) / yd.val();

        // Since eps may be zero, we need to test for equality on both sides.
        if ((-eps <= (mod-yf)) && ((mod-yf) <= eps)) {
            spot_on = true;
            break;
        } else if (mod < yf) {
            rn = yn;
            rd = yd;
            yn += ln;
            yd += ld;
        } else {
            ln = yn;
            ld = yd;
            yn += rn;
            yd += rd;
        }
    }
    if (!spot_on) {
        const F err_left = x - (static_cast<F>(ln.val()) / ld.val());
        const F err_right = x - (static_cast<F>(rn.val()) / rd.val());

        if (err_left <= err_right) {
            yn = ln;
            yd = ld;
        } else {
            yn = rn;
            yd = rd;
        }
    }

    return {quot + R(yn, yd)};
}


} // namespace fsacs


namespace fst {


template <>
struct WeightConvert<fsacs::RationalWeight, Log64Weight>
{
    Log64Weight operator()(const fsacs::RationalWeight &w) const
    {
        /** NOTE: LogWeights are the negative log of a number, so the
         *        following is correct even if it looks incorrect at a
         *        glance.  */
        return log(w.denominator()) - log(w.numerator());
    }
};


template <>
struct WeightConvert<Log64Weight, fsacs::RationalWeight>
{
    fsacs::RationalWeight operator()(const Log64Weight &w) const
    {
        const auto f = w.Value();
        return fsacs::convert_farey(exp(-f), fsacs::RationalWeight::default_lim());
    }
};


template <>
struct WeightConvert<fsacs::RationalWeight, LogWeight>
{
    LogWeight operator()(const fsacs::RationalWeight &w) const
    {
        /** NOTE: LogWeights are the negative log of a number, so the
         *        following is correct even if it looks incorrect at a
         *        glance.  */
        return logf(w.denominator()) - log(w.numerator());
    }
};


template <>
struct WeightConvert<LogWeight, fsacs::RationalWeight>
{
    fsacs::RationalWeight operator()(const LogWeight &w) const
    {
        const auto f = w.Value();
        return fsacs::convert_farey(exp(-f), fsacs::RationalWeight::default_lim());
    }
};


} // namespace fst


// struct SampleConvert {
//     using FromArc = fst::ArcTpl<fsacs::RationalWeight>;
//     using ToArc = fst::LogArc;

//     float beta;

//     SampleConvert(float temp)
//         : beta{1/temp} {}

//     ToArc operator()(const FromArc &a) const {
//         return ToArc(a.ilabel, a.olabel, beta*a.weight.as_float(), a.nextstate);
//     }

//     fst::MapFinalAction FinalAction() const { return fst::MAP_NO_SUPERFINAL; }
//     fst::MapSymbolsAction InputSymbolsAction() const { return fst::MAP_COPY_SYMBOLS; }
//     fst::MapSymbolsAction OutputSymbolsAction() const { return fst::MAP_COPY_SYMBOLS; }

//     uint64_t Properties(uint64_t p) const { return p; }
// };

template <class A, class B>
struct RmWeightConvert {
    using FromArc = A;
    using ToArc = B;
    using FromWeight = typename A::Weight;
    using ToWeight = typename B::Weight;

    RmWeightConvert<A, B>() {}

    B operator()(const A &a) const {
        ToWeight w = FromWeight::Zero() == a.weight ? ToWeight::Zero() : ToWeight::One();
        return B(a.ilabel, a.olabel, w, a.nextstate);
    }

    fst::MapFinalAction FinalAction() const { return fst::MAP_NO_SUPERFINAL; }
    fst::MapSymbolsAction InputSymbolsAction() const { return fst::MAP_COPY_SYMBOLS; }
    fst::MapSymbolsAction OutputSymbolsAction() const { return fst::MAP_COPY_SYMBOLS; }
    uint64 Properties(uint64 p) const { return (p & fst::kWeightInvariantProperties) | fst::kUnweighted; }
};


#endif /* ndef RATIONAL_WEIGHT_HPP */