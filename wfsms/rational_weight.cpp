#include "rational_weight.hpp"



namespace fsacs {


RationalWeight::RationalWeight(I n, I d, bool b)
    : value_{n, d}
    , bad_{b}
{}


RationalWeight::RationalWeight(I n, I d)
    : RationalWeight{n, d, false}
{}


RationalWeight::RationalWeight(const R &r)
    : RationalWeight{r.numerator(), r.denominator(), false}
{}


RationalWeight::RationalWeight(I n)
    : RationalWeight{n, 1}
{}


RationalWeight::RationalWeight() noexcept
    : RationalWeight{I(0)}
{}


typename RationalWeight::I::integer
RationalWeight::numerator() const
{
    return value_.numerator().val();
}

typename RationalWeight::I::integer
RationalWeight::denominator() const
{
    return value_.denominator().val();
}

float RationalWeight::as_float() const
{
    return static_cast<float>(numerator()) / denominator();
}


RationalWeight RationalWeight::Zero() { return {I(0)}; }
RationalWeight RationalWeight::One() { return {I(1)}; }
RationalWeight RationalWeight::NoWeight() { return {1, 1, true}; }


RationalWeight RationalWeight::Quantize(float _) const
{
    /** TODO: Maybe this can be done more nicely. */
    return *this;
}


typename RationalWeight::ReverseWeight
RationalWeight::Reverse() const
{
    return *this;
}


std::istream &RationalWeight::Read(std::istream &strm)
{
    /** NOTE: Fst binary format explicitly has no defined endianness:
     *        https://www.openfst.org/twiki/bin/view/FST/FstAdvancedUsage#FST%20Input/Output
     *        therefore we don't need to care either.
     */
    I num, denom;
    strm.read(reinterpret_cast<char*>(&num), sizeof(num));
    strm.read(reinterpret_cast<char*>(&denom), sizeof(denom));
    if (I(1) != boost::gcd(num, denom)) {
        /* Non-normalized is how we encode badness */
        bad_ = true;
    } else {
        value_ = {num, denom};
        bad_ = false;
    }
    return strm;
}


const std::string &RationalWeight::Type()
{
    static const std::string *const type = new std::string("rational");
    return *type;
}


std::ostream &RationalWeight::Write(std::ostream &strm) const
{
    /** NOTE: Fst binary format explicitly has no defined endianness:
     *        https://www.openfst.org/twiki/bin/view/FST/FstAdvancedUsage#FST%20Input/Output
     *        therefore we don't need to care either.
     */
    const auto &num = value_.numerator();
    const auto &denom = value_.denominator();
    if (!bad_) {
        strm.write(reinterpret_cast<const char*>(&num), sizeof(num));
        strm.write(reinterpret_cast<const char*>(&denom), sizeof(denom));
    } else {
        /* Write non-normalized form; detect this in Read. */
        I two = 2;
        strm.write(reinterpret_cast<const char*>(&two), sizeof(two));
        strm.write(reinterpret_cast<const char*>(&two), sizeof(two));
    }
    return strm;
}


size_t RationalWeight::Hash() const
{
    using i = I::integer;
    if (bad_) {
        return 0;
    }
    i n = value_.numerator().val();
    i d = value_.denominator().val();
    // don't overflow if we don't need to:
    i paired = n;
    if (1 == (n&1) + (d&1)) {
        paired += ((n+d-1)/2) * (n+d-2);
    } else {
        paired += ((n+d-2)/2) * (n+d-1);
    }
    return paired;
}


const typename RationalWeight::R&
RationalWeight::Value() const
{
    return value_;
}

void RationalWeight::SetValue(const R& x)
{
    value_ = x;
}


bool operator==(
    const RationalWeight &w1,
    const RationalWeight &w2)
{
    return w1.Member() && w2.Member() && w1.Value() == w2.Value();
}


bool operator!=(
    const RationalWeight &w1,
    const RationalWeight &w2
) {
    return w1.Member() && w2.Member() && w1.Value() != w2.Value();
}


RationalWeight Plus(
    const RationalWeight &w1,
    const RationalWeight &w2
) {
    return !(w1.Member() && w2.Member())
        ? RationalWeight::NoWeight()
        : RationalWeight{w1.Value() + w2.Value()};
}


RationalWeight Minus(
    const RationalWeight &w1,
    const RationalWeight &w2
) {
    return !(w1.Member() && w2.Member())
        ? RationalWeight::NoWeight()
        : RationalWeight{w1.Value() - w2.Value()};
}


RationalWeight Times(
    const RationalWeight &w1,
    const RationalWeight &w2
) {
    return !(w1.Member() && w2.Member())
        ? RationalWeight::NoWeight()
        : RationalWeight{w1.Value() * w2.Value()};
}


RationalWeight Divide(
    const RationalWeight &w1,
    const RationalWeight &w2,
    fst::DivideType typ
) {
    return !(w1.Member() && w2.Member())
        ? RationalWeight::NoWeight()
        : RationalWeight{w1.Value() / w2.Value()};
}


bool ApproxEqual(
    const RationalWeight &w1,
    const RationalWeight &w2,
    float delta)
{
    if (0 == delta) {
        return w1 == w2;
    }
    const RationalWeight::R d = Minus(w1, w2).Value();

    const float df = static_cast<float>(d.numerator().val()) / d.denominator().val();

    return delta < df;
}


} /* namespace fsacs */