#include "termiter.hpp"


termiter::termiter()
    : s_{}
    , w_{}
    , f_{*static_cast<const ratfa*>(nullptr)}
    , t_{"", ratweight::NoWeight()}
{}

termiter::termiter(const ratfa &f)
    : s_{}
    , w_{ratweight::One()}
    , f_(f)
    , t_{"", ratweight::One()} {

    if (fst::kNoStateId == f.Start()) {
        t_.w = ratweight::NoWeight();
    } else {
        s_.push_back(aiter(f, f.Start()));
    }

    ++*this;
}

termiter::reference termiter::operator*() const {
    return t_;
}

termiter::pointer termiter::operator->() const {
    return &t_;
}

termiter& termiter::operator++() {
    using fsacs::Times;
    std::string &word = t_.s;
    bool found = false;
    while (!s_.empty()) {
        auto &it = s_.back();
        if (it.Done()) {
            word.pop_back();
            w_.pop_back();
            s_.pop_back();
            continue;
        }
        const auto &a = it.Value();
        it.Next();
        word.push_back(a.ilabel);
        w_.push_back(Times(w_.back(), a.weight));
        s_.push_back(aiter(f_, a.nextstate));

        if (ratweight::Zero() != f_.Final(a.nextstate)) {
            t_.w = Times(w_.back(), f_.Final(a.nextstate));
            found = true;
            break;
        }
    }
    if (!found) {
        t_ = {"", ratweight::NoWeight()};
    }
    return *this;
}

termiter termiter::operator++(int) {
    termiter result = *this;
    ++*this;
    return result;
}

bool termiter::operator==(const termiter &o) const {
    if (!t_.w.Member() && !o.t_.w.Member()) {
        return true;
    }
    return &f_ == &o.f_ && t_.s == o.t_.s;
}

bool termiter::operator!=(const termiter &o) const {
    return !(*this == o);
}

namespace fst {

termiter begin(const ratfa &f) {
    return termiter(f);
}

termiter begin(ratfa &f) {
    return termiter(f);
}

termiter end(const ratfa &f) {
    return termiter();
}

termiter end(ratfa &f) {
    return termiter();
}

} // namespace fst