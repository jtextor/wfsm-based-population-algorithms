#ifndef WFSMS_GENERIC_HPP
#define WFSMS_GENERIC_HPP

#include <deque>

#include "wfsms_fwd.hpp"

/* Contains all of the generic definitions.  Include the wfsms.hpp header
 * instead of this one unless you have a good reason not to. */

template<class WA>
WA all(int l, int cf, int ct) {
    const int nchars = ct - cf + 1;
    WA result;
    result.ReserveStates(1 + l);
    state<WA> s = result.AddState();
    result.SetStart(s);
    for (int i=0; i<l; ++i) {
        state<WA> t = result.AddState();
        result.ReserveArcs(s, nchars);
        for (int c=cf; c<=ct; ++c) {
            result.AddArc(s, arc<WA>(c, c, weight<WA>::One(), t));
        }
        s = t;
    }
    result.SetFinal(s, weight<WA>::One());
    return result;
};

template<class WA>
void bigcapdot(std::vector<WA> *fs, bool do_push) {
    std::vector<WA> gs;
    std::vector<WA> &a = *fs, &b = gs;

    while (1 < a.size()) {
        b.resize(1 + ((a.size() - 1) >> 1));
        for (int i=0; i < (a.size() & (~1ULL)); i += 2) {
            int j = i >> 1;
            b[j] = capdot(a[i], a[i+1], do_push);
        }
        if (a.size() & 1) {
            b.back() = a.back();
        }
        std::swap(a, b);
    }
};

template<class WA>
void biguplus(std::vector<WA> *fs, bool do_push) {
    std::vector<WA> gs;
    std::vector<WA> &a = *fs, &b = gs;

    while (1 < a.size()) {
        b.resize(1 + ((a.size() - 1) >> 1));
        for (int i=0; i < (a.size() & (~1ULL)); i += 2) {
            int j = i >> 1;
            b[j] = uplus(a[i], &a[i+1], do_push);
        }
        if (a.size() & 1) {
            b.back() = a.back();
        }
        std::swap(a, b);
    }
};

template <class WA>
WA capdot(const WA &f, const WA &g, bool push) {
    using a = arc<WA>;
    WA h;
    if (!(f.Properties(fst::kILabelSorted, false) & fst::kILabelSorted) &&
        !(g.Properties(fst::kILabelSorted, false) & fst::kILabelSorted)) {
            if (f.NumStates() <= g.NumStates()) {
                fst::ArcSort(const_cast<WA*>(&f), fst::ILabelCompare<a>());
            } else {
                fst::ArcSort(const_cast<WA*>(&g), fst::ILabelCompare<a>());
            }
    }
    fst::Intersect(f, g, &h);
    minimize(&h, push);
    return h;
}

template <class WA>
WA contiguous(const std::string &word, int d, int cf, int ct) {
    const int l = word.size();
    const int r = 0 <= d ? l - d : -d;
    d = l - r;
    const int a = (d + 1)*(r + 1) - 1;
    const int nchars = ct - cf + 1;
    const weight<WA> one = weight<WA>::One();
    WA f;
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
                f.AddArc(ff, arc<WA>(c, c, one, t));
            }
        }
        for (; p <= d+h; ++p) {
            const int ff = d*h + p;
            const int t1 = ff + d + 1;
            f.AddArc(ff, arc<WA>(word[d+h], word[d+h], one, t1));
        }
    }
    // Loop over positions in succesful layer
    for (int p = r; p < d+r; ++p) {
        const int ff = d*r + p;
        const int tx = ff + 1;
        // Succesful layer always proceeds to next position.
        f.ReserveArcs(ff, nchars);
        for (int c=cf; c<=ct; ++c) {
            f.AddArc(ff, arc<WA>(c, c, one, tx));
        }
    }
    f.SetFinal(a, one);
    return f;
};

template <class WA>
unsigned long count(const WA &f) {
    if (fst::kNoStateId == f.Start()) {
        return 0;
    }
    std::vector<unsigned long> nprefixes(f.NumStates(), 0);
    std::vector<bool> enqueued(f.NumStates(), false);
    std::deque<state<WA>> queue{f.Start()};
    nprefixes[f.Start()] = 1;
    enqueued[f.Start()] = true;
    unsigned long result = 0;
    while (!queue.empty()) {
        state<WA> s = queue.front();
        queue.pop_front();
        for (aiter<WA> it(f, s); !it.Done(); it.Next()) {
            const auto &a = it.Value();
            nprefixes[a.nextstate] += nprefixes[s];
            if (!enqueued[a.nextstate]) {
                queue.push_back(a.nextstate);
                enqueued[a.nextstate] = true;
            }
        }
        if (weight<WA>::Zero() != f.Final(s)) {
            result += nprefixes[s];
        }
    }
    return result;
};

template <class WA>
WA determinize(WA *f) {
    WA g;
    fst::RmEpsilon(f);
    fst::Determinize(*f, &g);
    return g;
};

template <class WA>
WA hamming(const std::string &word, int hd, int cf, int ct) {
    const int l = word.size();
    const int r = 0 <= hd ? l - hd : -hd;
    const int d = l - r + 1;
    const int a = d*(r + 1) - 1;
    const int nchars = ct - cf + 1;
    const weight<WA> one = weight<WA>::One();
    WA f;
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
                f.AddArc(o, arc<WA>(c, c, one, t));
            }
        }
        // The only transition out of a rightmost position in layer is a hit.
        const int o = d*(m + 1) - 1;
        const int t = o + d;
        const int c = word[m+d-1];
        f.AddArc(o, arc<WA>(c, c, one, t));
    }
    // Loop over positions in succesful layer
    for (int p = r; p < l; ++p) {
        const int o = r*(d - 1) + p;
        const int t = o + 1;
        f.ReserveArcs(o, nchars);
        // Succesful layer always proceeds to next position.
        for (int c = cf; c <= ct; ++c) {
            f.AddArc(o, arc<WA>(c, c, one, t));
        }
    }
    f.SetFinal(a, one);
    return f;
};

template <class WA>
WA insert(const WA &f1, const WA &f2, bool do_push) {
    using a = arc<WA>;
    using rwm = fst::RmWeightMapper<a>;
    sortarcs(const_cast<WA*>(&f1));
    fst::ArcMapFst<a, a, rwm> f0(f1, rwm());
    WA intermediate;
    fst::Difference(f2, f0, &intermediate);
    return uplus(f1, &intermediate, do_push);
};

template <class WA>
void minimize(WA *f, bool do_push) {
    fst::Minimize(f);
};

template <class WA>
WA setminus(const WA &f, const WA &g, bool do_sort, bool do_push) {
    if (do_sort) {
        sortarcs(const_cast<WA*>(&g));
    }
    WA h;
    fst::Difference(f, g, &h);
    minimize(&h, do_push);
    return h;
};

template <class WA>
WA singleton(const std::string &w) {
    WA result;
    result.ReserveStates(1 + w.size());
    state<WA> s = result.AddState();
    result.SetStart(s);
    for (auto c : w) {
        state<WA> t = result.AddState();
        result.AddArc(s, arc<WA>(c, c, weight<WA>::One(), t));
        s = t;
    }
    result.SetFinal(s, weight<WA>::One());
    return result;
};

template <class WA>
void sortarcs(WA *f) {
    using a = arc<WA>;
    bool sorted = f->Properties(fst::kILabelSorted, false) & fst::kILabelSorted;
    if (!sorted) {
        fst::ArcSort(f, fst::ILabelCompare<a>());
        f->SetProperties(fst::kILabelSorted, fst::kILabelSorted);
    }
};

template <class WA>
WA uplus(const WA &f, WA *g, bool do_push) {
    fst::Union(g, f);
    WA h = determinize(g);
    minimize(&h, do_push);
    return h;
}

template <class WA>
weight<WA> weigh(const WA &f) {
    return fst::ShortestDistance(f);
};

#endif /* !defined(WFSMS_GENERIC_HPP) */