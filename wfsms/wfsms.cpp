#include <deque>

#include "wfsms_fwd.hpp"
#include "wfsms_generic.hpp"


template <typename T1, typename T2>
using convert = fst::WeightConvertMapper<T1, T2>;

template <typename T1, typename T2>
using convertfa = fst::ArcMapFst<T1, T2, convert<T1, T2>>;


/* This is required for sample(...) */
static fst::LogProbArcSelector<logarc> logselect;


template boolfa all(int, int, int);
template logfa all(int, int, int);
template ratfa all(int, int, int);

template void bigcapdot(std::vector<boolfa>*, bool);
template void bigcapdot(std::vector<logfa>*, bool);
template void bigcapdot(std::vector<ratfa>*, bool);

template void biguplus(std::vector<boolfa>*, bool);
template void biguplus(std::vector<logfa>*, bool);
template void biguplus(std::vector<ratfa>*, bool);

template boolfa capdot(const boolfa&, const boolfa&, bool);
template logfa capdot(const logfa&, const logfa&, bool);
template ratfa capdot(const ratfa&, const ratfa&, bool);

template boolfa contiguous(const std::string&, int, int, int);
template logfa contiguous(const std::string&, int, int, int);
template ratfa contiguous(const std::string&, int, int, int);

template unsigned long count(const boolfa&);
template unsigned long count(const logfa&);
template unsigned long count(const ratfa&);

template boolfa determinize(boolfa*);
template logfa determinize(logfa*);
template <>
ratfa determinize(ratfa *f) {
    const fst::DeterminizeOptions<typename ratfa::Arc> opts(0.0f);
    ratfa g;
    fst::RmEpsilon(f);
    fst::Determinize(*f, &g, opts);
    return g;
};

template boolfa hamming(const std::string&, int, int, int);
template logfa hamming(const std::string&, int, int, int);
template ratfa hamming(const std::string&, int, int, int);

template boolfa insert(const boolfa&, const boolfa&, bool);
template logfa insert(const logfa&, const logfa&, bool);
template ratfa insert(const ratfa&, const ratfa&, bool);

template void minimize(boolfa*, bool);
template void minimize(logfa*, bool);
template <>
void minimize<ratfa>(ratfa *f, bool do_push) {
    using namespace fst;
    using mutfa = MutableFst<ratarc>;

    if (do_push) {
        std::vector<ratweight> potentials;
        ShortestDistance(*f, &potentials, true, 0.0f);
        Reweight(f, potentials, fst::REWEIGHT_TO_INITIAL);
    }
    EncodeMapper<ratarc> encoder(kEncodeLabels | kEncodeWeights, fst::ENCODE);
    Encode(f, &encoder);
    Minimize(f, static_cast<mutfa*>(nullptr), 0.0f);
    Decode(f, encoder);
};

ratfa prune(const ratfa &f, float factor, bool push) {
    /** TODO: roll our own that does not require conversions? */
    using namespace fst;
    using iwm = fst::InvertWeightMapper<logarc>;
    using rwm = fst::RmWeightMapper<boolarc>;
    logfa g;
    boolfa h;
    ArcMap(f, &g, convert<ratarc, logarc>());
    ArcMap(g, &h, convert<logarc, boolarc>());
    fst::ArcSort(&h, fst::ILabelCompare<boolarc>());
    Prune(&h, -logf(factor));
    ratfa i;
    ArcMap(h, &i, RmWeightConvert<boolarc, ratarc>());
    return capdot(f, i, push);
}

std::string sample(const ratfa &f) {
    using namespace fst;
    using aiter = ArcIterator<logfa>;

    RandGenOptions options(logselect);
    logfa g, h;
    ArcMap(f, &g, convert<ratarc, logarc>());
    std::vector<logweight> potentials;
    ShortestDistance(g, &potentials, true);
    Reweight(&g, potentials, fst::REWEIGHT_TO_INITIAL);
    RandGen(g, &h, options);
    std::string out;
    auto s = h.Start();
    for (;;) {
        if (logweight::Zero() != h.Final(s)) {
            break;
        }
        aiter it(h, s);
        const auto &a = it.Value();
        out.push_back(a.ilabel);
        s = a.nextstate;
    }
    return out;
}

template boolfa setminus(const boolfa&, const boolfa&, bool, bool);
template logfa setminus(const logfa&, const logfa&, bool, bool);
template ratfa setminus(const ratfa&, const ratfa&, bool, bool);

template boolfa singleton(const std::string&);
template logfa singleton(const std::string&);
template ratfa singleton(const std::string&);

template void sortarcs(boolfa*);
template void sortarcs(logfa*);
template void sortarcs(ratfa*);

template boolfa uplus(const boolfa&, boolfa*, bool);
template logfa uplus(const logfa&, logfa*, bool);
template ratfa uplus(const ratfa&, ratfa*, bool);

template boolweight weigh(const boolfa&);
template logweight weigh(const logfa&);
template <>
ratweight weigh(const ratfa &f) {
    return fst::ShortestDistance(f, 0.0f);
};