#include <deque>
#include <functional>

#include <fst/fstlib.h>

#include "ratfa.hpp"
#include "termiter.hpp"


using logarc = fst::LogArc;
using logweight = typename logarc::Weight;
using logfa = fst::VectorFst<logarc>;

using trpweight = fst::TropicalWeight;
using trparc = fst::ArcTpl<trpweight>;
using trpfa = fst::VectorFst<trparc>;


fst::LogProbArcSelector<fst::LogArc> logselect;

template <typename T1, typename T2>
using convert = fst::WeightConvertMapper<T1, T2>;

template <typename T1, typename T2>
using convertfa = fst::ArcMapFst<T1, T2, convert<T1, T2>>;


static void pairwise_reduce(std::vector<ratfa> *f,
    std::function<ratfa(const ratfa&, ratfa*)> fn) {

    std::vector<ratfa> g;
    std::vector<ratfa> &a = *f, &b = g;

    while (1 < a.size()) {
        b.resize(1 + ((a.size() - 1) >> 1));
        for (int i=0; i < (a.size() & (~1ULL)); i += 2) {
            int j = i >> 1;
            b[j] = fn(a[i], &a[i+1]);
        }
        if (a.size() & 1) {
            b.back() = a.back();
        }
        std::swap(a, b);
    }
}

ratfa all(int l, int cf, int ct) {
    using state = typename ratfa::StateId;
    const int nchars = ct - cf + 1;
    ratfa result;
    result.ReserveStates(1 + l);
    state s = result.AddState();
    result.SetStart(s);
    for (int i=0; i<l; ++i) {
        state t = result.AddState();
        result.ReserveArcs(s, nchars);
        for (int c=cf; c<=ct; ++c) {
            result.AddArc(s, ratarc(c, c, ratweight::One(), t));
        }
        s = t;
    }
    result.SetFinal(s, ratweight::One());
    return result;
}

void bigcapdot(std::vector<ratfa> *f, bool push) {
    pairwise_reduce(f, [push](const ratfa &g, ratfa *h) -> ratfa {
        return capdot(g, *h, push);
    });
}

void biguplus(std::vector<ratfa> *f, bool push) {
    pairwise_reduce(f, [push](const ratfa &g, ratfa *h) -> ratfa {
        return uplus(g, h, push);
    });
}

ratfa capdot(const ratfa &f, const ratfa &g, bool push) {
    ratfa h;
    if (!(f.Properties(fst::kILabelSorted, false) & fst::kILabelSorted) &&
        !(g.Properties(fst::kILabelSorted, false) & fst::kILabelSorted)) {
            if (f.NumStates() <= g.NumStates()) {
                fst::ArcSort(const_cast<ratfa*>(&f), fst::ILabelCompare<ratarc>());
            } else {
                fst::ArcSort(const_cast<ratfa*>(&g), fst::ILabelCompare<ratarc>());
            }
    }
    fst::Intersect(f, g, &h);
    minimize(&h, push);
    return h;
}

ratfa contiguous(const std::string &word, int d, int cf, int ct) {
    const int l = word.size();
    const int r = 0 <= d ? l - d : -d;
    d = abs(d);
    const int a = (d + 1)*(r + 1) - 1;
    const int nchars = ct - cf + 1;
    const ratweight one = ratweight::One();
    ratfa f;
    f.ReserveStates(1 + a);
    for (int i=0; i <= a; ++i) {
        f.AddState();
    }
    f.SetStart(0);
    // Loop over number of hits, minus succesful layer.
    for (int h = 0; h < r; ++h) {
        // Loop over positions in current layer
        int p = h;
        for (; p < d; ++p) {
            const int ff = d*h + p;
            const int t0 = p + 1;
            const int t1 = ff + d + 1;

            f.ReserveArcs(ff, nchars);
            for (int c=cf; c<=ct; ++c) {
                const int t = c == word[p] ? t1 : t0;
                f.AddArc(ff, ratarc(c, c, one, t));
            }
        }
        for (; p <= d+h; ++p) {
            const int ff = d*h + p;
            const int t1 = ff + d + 1;
            f.AddArc(ff, ratarc(word[d+h], word[d+h], one, t1));
        }
    }
    // Loop over positions in succesful layer
    for (int p = r; p < d+r; ++p) {
        const int ff = d*r + p;
        const int tx = ff + 1;
        // Succesful layer always proceeds to next position.
        f.ReserveArcs(ff, nchars);
        for (int c=cf; c<=ct; ++c) {
            f.AddArc(ff, ratarc(c, c, one, tx));
        }
    }
    f.SetFinal(a, one);
    return f;
}

unsigned long count(const ratfa &f) {
    using aiter = fst::ArcIterator<ratfa>;
    using stateid = typename ratfa::StateId;
    if (fst::kNoStateId == f.Start()) {
        return 0;
    }
    std::vector<unsigned long> nprefixes(f.NumStates(), 0);
    std::vector<bool> enqueued(f.NumStates(), false);
    std::deque<stateid> queue{f.Start()};
    nprefixes[f.Start()] = 1;
    enqueued[f.Start()] = true;
    unsigned long result = 0;
    while (!queue.empty()) {
        stateid s = queue.front();
        queue.pop_front();
        for (aiter it(f, s); !it.Done(); it.Next()) {
            const auto &a = it.Value();
            nprefixes[a.nextstate] += nprefixes[s];
            if (!enqueued[a.nextstate]) {
                queue.push_back(a.nextstate);
                enqueued[a.nextstate] = true;
            }
        }
        if (ratweight::Zero() != f.Final(s)) {
            result += nprefixes[s];
        }
    }
    return result;
}

ratfa determinize(ratfa *f) {
    const fst::DeterminizeOptions<typename ratfa::Arc> opts(0.0f);
    ratfa g;
    fst::RmEpsilon(f);
    fst::Determinize(*f, &g, opts);
    return g;
}

bool empty(const ratfa &f) {
    return begin(f) == end(f);
}

ratfa exact(const std::string &w) {
    using state = typename ratfa::StateId;
    ratfa result;
    result.ReserveStates(1 + w.size());
    state s = result.AddState();
    result.SetStart(s);
    for (auto c : w) {
        state t = result.AddState();
        result.AddArc(s, ratarc(c, c, ratweight::One(), t));
        s = t;
    }
    result.SetFinal(s, ratweight::One());
    return result;
}

void fawrite(const ratfa &f, std::ostream &os) {
    f.Write(os, fst::FstWriteOptions());
}

void fawrite(const ratfa &f, const std::string &fnm) {
    std::ofstream fout(fnm, std::ios::out);
    fawrite(f, fout);
    fout.close();
}

void fawrite(const ratfa &f, const char *fnm) {
    std::ofstream fout(fnm, std::ios::out);
    fawrite(f, fout);
    fout.close();
}

ratfa hamming(const std::string &word, int hd, int cf, int ct) {
    const int l = word.size();
    const int r = 0 <= hd ? l - hd : -hd;
    const int d = l - r + 1;
    const int a = d*(r + 1) - 1;
    const int nchars = ct - cf + 1;
    const ratweight one = ratweight::One();
    ratfa f;
    f.ReserveStates(1 + a);
    for (int i=0; i <= a; ++i) {
        f.AddState();
    }
    f.SetStart(0);
    // Loop over number of hits, minus succesful layer.
    for (int m = 0; m < r; ++m) {
        // Loop over positions in current layer.
        for (int p = m; p < d+m-1; ++p) {
            const int c = word[p];
            const int o = m*(d - 1) + p;

            f.ReserveArcs(o, nchars);
            for (int c=cf; c<=ct; ++c) {
                const int t = c == word[p] ? o + d : o + 1;
                f.AddArc(o, ratarc(c, c, one, t));
            }
        }
        // The only transition out of a rightmost position in layer is a hit.
        const int o = d*(m + 1) - 1;
        const int t = o + d;
        const int c = word[m+d-1];
        f.AddArc(o, ratarc(c, c, one, t));
    }
    // Loop over positions in succesful layer
    for (int p = r; p < l; ++p) {
        const int o = r*(d - 1) + p;
        const int t = o + 1;
        f.ReserveArcs(o, nchars);
        // Succesful layer always proceeds to next position.
        for (int c = cf; c <= ct; ++c) {
            f.AddArc(o, ratarc(c, c, one, t));
        }
    }
    f.SetFinal(a, one);
    return f;
}

ratfa insert(const ratfa &f1, const ratfa &f2, bool push) {
    using rwm = fst::RmWeightMapper<ratarc>;
    sortarcs(const_cast<ratfa*>(&f1));
    fst::ArcMapFst<ratarc, ratarc, rwm> f0(f1, rwm());
    ratfa intermediate;
    fst::Difference(f2, f0, &intermediate);
    return uplus(f1, &intermediate, push);
}

void minimize(ratfa *f, bool do_push) {
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
}

void print(const ratfa &f, std::ostream &os) {
    for (const term &t : f) {
        os << t.s << "," << t.w.as_float() << "\n";
    }
}

ratfa prune(const ratfa &f, float factor, bool push) {
    /** TODO: roll our own that does not require conversions? */
    using namespace fst;
    using iwm = fst::InvertWeightMapper<logarc>;
    using rwm = fst::RmWeightMapper<trparc>;
    logfa g;
    trpfa h;
    ArcMap(f, &g, convert<ratarc, logarc>());
    ArcMap(g, &h, convert<logarc, trparc>());
    fst::ArcSort(&h, fst::ILabelCompare<trparc>());
    Prune(&h, -logf(factor));
    ratfa i;
    ArcMap(h, &i, RmWeightConvert<trparc, ratarc>());
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

ratfa setminus(const ratfa &f, const ratfa &g, bool sort, bool push) {
    if (sort) {
        sortarcs(const_cast<ratfa*>(&g));
    }
    ratfa h;
    fst::Difference(f, g, &h);
    minimize(&h, push);
    return h;
}

void sortarcs(ratfa *f) {
    bool sorted = f->Properties(fst::kILabelSorted, false) & fst::kILabelSorted;
    if (!sorted) {
        fst::ArcSort(f, fst::ILabelCompare<ratarc>());
        f->SetProperties(fst::kILabelSorted, fst::kILabelSorted);
    }
}

ratfa uplus(const ratfa &f, ratfa *g, bool push) {
    fst::Union(g, f);
    ratfa h = determinize(g);
    minimize(&h, push);
    return h;
}

ratweight weight(const ratfa &f, const std::string &w) {
    using namespace fsacs;
    using state = typename ratfa::StateId;

    state s = f.Start();
    if (fst::kNoStateId == s) {
        return ratweight::Zero();
    }
    auto result = ratweight::One();
    for (auto c : w) {
        bool found = false;
        for (fst::ArcIterator<ratfa> aiter(f, s); !aiter.Done(); aiter.Next()) {
            const auto &a = aiter.Value();
            if (a.ilabel == c) {
                s = a.nextstate;
                result = Times(result, a.weight);
                found = true;
                break;
            }
        }
        if (!found) {
            return ratweight::Zero();
        }
    }
    return Times(result, f.Final(s));
}