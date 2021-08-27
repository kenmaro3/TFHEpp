#pragma once

#include <cstdint>
#include <limits>

#include "params.hpp"

namespace TFHEpp {
struct portablelvl0param {
    std::uint32_t n;           // dimension
    double alpha;              // fresh noise
    std::uint32_t approx_bit;  // Torus representation bit size

    portablelvl0param()
        : n(lvl0param::n),
          alpha(lvl0param::alpha),
          approx_bit(std::numeric_limits<typename lvl0param::T>::digits)
    {
    }
    portablelvl0param(const portablelvl0param& P)
        : n(P.n), alpha(P.alpha), approx_bit(P.approx_bit)
    {
    }

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(n, alpha, approx_bit);
    }

    bool operator==(const portablelvl0param& in) const
    {
        return (n == in.n) && (alpha == in.alpha) &&
               (approx_bit == in.approx_bit);
    };
};

struct portablelvl1param {
    std::uint32_t nbit;  // dimension must be a power of 2 for ease of
                         // polynomial multiplication.
    std::uint32_t n;     // dimension
    std::uint32_t l;
    std::uint32_t Bgbit;
    std::uint32_t Bg;
    double alpha;              // fresh noise
    std::uint32_t approx_bit;  // Torus representation bit size

    portablelvl1param()
        : nbit(lvl1param::nbit),
          n(lvl1param::n),
          l(lvl1param::l),
          Bgbit(lvl1param::Bgbit),
          Bg(lvl1param::Bg),
          alpha(lvl1param::alpha),
          approx_bit(std::numeric_limits<typename lvl1param::T>::digits)
    {
    }
    portablelvl1param(const portablelvl1param& P)
        : nbit(P.nbit),
          n(P.n),
          l(P.l),
          Bgbit(P.Bgbit),
          Bg(P.Bg),
          alpha(P.alpha),
          approx_bit(P.approx_bit)
    {
    }

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(nbit, n, l, Bgbit, Bg, alpha, approx_bit);
    }

    bool operator==(const portablelvl1param& in) const
    {
        return (nbit == in.nbit) && (n == in.n) && (l == in.l) &&
               (Bgbit == in.Bgbit) && (Bg == in.Bg) && (alpha == in.alpha) &&
               (approx_bit == in.approx_bit);
    };
};

struct portablelvl2param {
    std::uint32_t nbit;  // dimension must be a power of 2 for ease of
                         // polynomial multiplication.
    std::uint32_t n;     // dimension
    std::uint32_t l;
    std::uint32_t Bgbit;
    std::uint32_t Bg;
    double alpha;              // fresh noise
    std::uint32_t approx_bit;  // Torus representation bit size

    portablelvl2param()
        : nbit(lvl2param::nbit),
          n(lvl2param::n),
          l(lvl2param::l),
          Bgbit(lvl2param::Bgbit),
          Bg(lvl2param::Bg),
          alpha(lvl2param::alpha),
          approx_bit(std::numeric_limits<typename lvl2param::T>::digits)
    {
    }
    portablelvl2param(const portablelvl1param& P)
        : nbit(P.nbit),
          n(P.n),
          l(P.l),
          Bgbit(P.Bgbit),
          Bg(P.Bg),
          alpha(P.alpha),
          approx_bit(P.approx_bit)
    {
    }

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(nbit, n, l, Bgbit, Bg, alpha, approx_bit);
    }

    bool operator==(const portablelvl2param& in) const
    {
        return (nbit == in.nbit) && (n == in.n) && (l == in.l) &&
               (Bgbit == in.Bgbit) && (Bg == in.Bg) && (alpha == in.alpha) &&
               (approx_bit == in.approx_bit);
    };
};

// Key Switching parameters
struct portablelvl10param {
    std::uint32_t t;  // number of addition in keyswitching
    std::uint32_t
        basebit;   // how many bit should be encrypted in keyswitching key
    double alpha;  // key noise

    portablelvl10param()
        : t(lvl10param::t),
          basebit(lvl10param::basebit),
          alpha(lvl10param::alpha)
    {
    }
    portablelvl10param(const portablelvl10param& P)
        : t(P.t), basebit(P.basebit), alpha(P.alpha)
    {
    }

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(t, basebit, alpha);
    }

    bool operator==(const portablelvl10param& in) const
    {
        return (t == in.t) && (basebit == in.basebit) && (alpha == in.alpha);
    };
};

struct portablelvl20param {
    std::uint32_t t;  // number of addition in keyswitching
    std::uint32_t
        basebit;   // how many bit should be encrypted in keyswitching key
    double alpha;  // key noise

    portablelvl20param()
        : t(lvl20param::t),
          basebit(lvl20param::basebit),
          alpha(lvl20param::alpha)
    {
    }
    portablelvl20param(const portablelvl20param& P)
        : t(P.t), basebit(P.basebit), alpha(P.alpha)
    {
    }

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(t, basebit, alpha);
    }

    bool operator==(const portablelvl20param& in) const
    {
        return (t == in.t) && (basebit == in.basebit) && (alpha == in.alpha);
    }
};

struct portablelvl21param {
    std::uint32_t t;  // number of addition in keyswitching
    std::uint32_t
        basebit;   // how many bit should be encrypted in keyswitching key
    double alpha;  // key noise

    portablelvl21param()
        : t(lvl21param::t),
          basebit(lvl21param::basebit),
          alpha(lvl21param::alpha)
    {
    }
    portablelvl21param(const portablelvl21param& P)
        : t(P.t), basebit(P.basebit), alpha(P.alpha)
    {
    }

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(t, basebit, alpha);
    }

    bool operator==(const portablelvl21param& in) const
    {
        return (t == in.t) && (basebit == in.basebit) && (alpha == in.alpha);
    }
};

struct portablelvl22param {
    std::uint32_t t;  // number of addition in keyswitching
    std::uint32_t
        basebit;   // how many bit should be encrypted in keyswitching key
    double alpha;  // key noise

    portablelvl22param()
        : t(lvl22param::t),
          basebit(lvl22param::basebit),
          alpha(lvl22param::alpha)
    {
    }
    portablelvl22param(const portablelvl22param& P)
        : t(P.t), basebit(P.basebit), alpha(P.alpha)
    {
    }

    bool operator==(const portablelvl22param& in) const
    {
        return (t == in.t) && (basebit == in.basebit) && (alpha == in.alpha);
    }
};

struct lweParams {
    portablelvl0param lvl0;
    portablelvl1param lvl1;
    portablelvl2param lvl2;
    portablelvl10param lvl10;
    portablelvl20param lvl20;
    portablelvl21param lvl21;
    portablelvl22param lvl22;

    lweParams() : lvl0(), lvl1(), lvl2(), lvl10(), lvl20(), lvl21(), lvl22() {}

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(lvl0.n, lvl0.alpha, lvl0.approx_bit, lvl1.nbit, lvl1.n, lvl1.l,
                lvl1.Bgbit, lvl1.alpha, lvl1.approx_bit, lvl2.nbit, lvl2.n,
                lvl2.l, lvl2.Bgbit, lvl2.alpha, lvl2.approx_bit, lvl10.t,
                lvl10.basebit, lvl10.alpha, lvl20.t, lvl20.basebit, lvl20.alpha,
                lvl21.t, lvl21.basebit, lvl21.alpha, lvl22.t, lvl22.basebit,
                lvl22.alpha);
    }

    // https://cpprefjp.github.io/lang/cpp20/consistent_comparison.html
    bool operator==(const lweParams& in) const
    {
        return (lvl0 == in.lvl0) && (lvl1 == in.lvl1) && (lvl2 == in.lvl2) &&
               (lvl10 == in.lvl10) && (lvl20 == in.lvl20) &&
               (lvl21 == in.lvl21) && (lvl22 == in.lvl22);
    }
};
}  // namespace TFHEpp