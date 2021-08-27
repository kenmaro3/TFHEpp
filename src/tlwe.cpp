#include <randen.h>

#include <array>
#include <cstdint>
#include <key.hpp>
#include <limits>
#include <params.hpp>
#include <random>
#include <tlwe.hpp>
#include <type_traits>
#include <utils.hpp>
#include <vector>

namespace TFHEpp {

template <class P>
array<typename P::T, P::n + 1> tlweSymEncodeEncrypt(
    const double &x, const double alpha, const array<typename P::T, P::n> &key,
    const Encoder &encoder)
{
    uniform_int_distribution<typename P::T> Torusdist(
        0, numeric_limits<typename P::T>::max());
    array<typename P::T, P::n + 1> res = {};
    lvl0param::T p = encoder.encode(x);
    res[P::n] = ModularGaussian<P>(p, alpha);
    for (int i = 0; i < P::n; i++) {
        res[i] = Torusdist(generator);
        res[P::n] += res[i] * key[i];
    }
    return res;
}
#define INST(P)                                                      \
    template array<typename P::T, P::n + 1> tlweSymEncodeEncrypt<P>( \
        const double &x, const double alpha,                         \
        const array<typename P::T, P::n> &key, const Encoder &encoder)
TFHEPP_EXPLICIT_INSTANTIATION_TLWE(INST)
#undef INST

template <class P>
TLWE<P> tlweSymEncrypt(const typename P::T p, const double alpha,
                       const std::array<typename P::T, P::n> &key)
{
    std::uniform_int_distribution<typename P::T> Torusdist(
        0, std::numeric_limits<typename P::T>::max());
    TLWE<P> res = {};
    res[P::n] = ModularGaussian<P>(p, alpha);
    for (int i = 0; i < P::n; i++) {
        res[i] = Torusdist(generator);
        res[P::n] += res[i] * key[i];
    }
    return res;
}
#define INST(P)                                    \
    template TLWE<P> tlweSymEncrypt<P>(            \
        const typename P::T p, const double alpha, \
        const std::array<typename P::T, P::n> &key)
TFHEPP_EXPLICIT_INSTANTIATION_TLWE(INST)
#undef INST

template <class P>
double tlweSymDecryptDecode(const TLWE<P> &c, const Key<P> &key,
                            const Encoder &encoder)
{
    typename P::T phase = c[P::n];
    for (int i = 0; i < P::n; i++) {
        phase = (phase - c[i] * key[i]);
    }
    double res = encoder.decode(phase);
    return res;
}
#define INST(P)                              \
    template double tlweSymDecryptDecode<P>( \
        const TLWE<P> &c, const Key<P> &key, const Encoder &encoder)
TFHEPP_EXPLICIT_INSTANTIATION_TLWE(INST)
#undef INST

template <class P>
bool tlweSymDecrypt(const TLWE<P> &c, const Key<P> &key)
{
    typename P::T phase = c[P::n];
    for (int i = 0; i < P::n; i++) phase -= c[i] * key[i];
    bool res =
        static_cast<typename std::make_signed<typename P::T>::type>(phase) > 0;
    return res;
}
#define INST(P) \
    template bool tlweSymDecrypt<P>(const TLWE<P> &c, const Key<P> &key)
TFHEPP_EXPLICIT_INSTANTIATION_TLWE(INST)
#undef INST

template <class P>
std::vector<TLWE<P>> bootsSymEncrypt(const std::vector<uint8_t> &p,
                                     const SecretKey &sk)
{
    vector<TLWE<P>> c(p.size());
    for (int i = 0; i < p.size(); i++)
        c[i] = tlweSymEncrypt<P>(p[i] ? lvl0param::mu : -lvl0param::mu,
                                 lvl0param::alpha, sk.key.get<P>());
    return c;
}
#define INST(P)                                       \
    template std::vector<TLWE<P>> bootsSymEncrypt<P>( \
        const std::vector<uint8_t> &p, const SecretKey &sk)
TFHEPP_EXPLICIT_INSTANTIATION_TLWE(INST)
#undef INST

template <class P>
std::vector<uint8_t> bootsSymDecrypt(const std::vector<TLWE<P>> &c,
                                     const SecretKey &sk)
{
    vector<uint8_t> p(c.size());
    for (int i = 0; i < c.size(); i++)
        p[i] = tlweSymDecrypt<P>(c[i], sk.key.get<P>());
    return p;
}
#define INST(P)                                       \
    template std::vector<uint8_t> bootsSymDecrypt<P>( \
        const std::vector<TLWE<P>> &c, const SecretKey &sk)
TFHEPP_EXPLICIT_INSTANTIATION_TLWE(INST)
#undef INST
}  // namespace TFHEpp
