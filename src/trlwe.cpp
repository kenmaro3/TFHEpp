#include "trlwe.hpp"

#include "mulfft.hpp"
#include "tlwe.hpp"

namespace TFHEpp {
using namespace std;

template <class P>
TRLWE<P> trlweSymEncryptZero(const double alpha, const Key<P> &key)
{
    uniform_int_distribution<typename P::T> Torusdist(
        0, std::numeric_limits<typename P::T>::max());
    TRLWE<P> c;
    for (typename P::T &i : c[0]) i = Torusdist(generator);
    PolyMul<P>(c[1], c[0], key);
    for (typename P::T &i : c[1]) i += ModularGaussian<P>(0, alpha);
    return c;
}
#define INST(P)                                                  \
    template TRLWE<P> trlweSymEncryptZero<P>(const double alpha, \
                                             const Key<P> &key)
TFHEPP_EXPLICIT_INSTANTIATION_TRLWE(INST)
#undef INST

template <class P>
TRLWE<P> trlweSymEncodeEncrypt(const array<double, P::n> &x, const double alpha,
                               const Key<P> &key, Encoder &encoder)
{
    TRLWE<P> c;
    c = trlweSymEncryptZero<P>(alpha, key);
    for (int i = 0; i < P::n; i++) c[1][i] += encoder.encode(x[i]);
    return c;
}
#define INST(P)                                                              \
    template TRLWE<P> trlweSymEncodeEncrypt<P>(                              \
        const array<double, P::n> &p, const double alpha, const Key<P> &key, \
        Encoder &encoder)
TFHEPP_EXPLICIT_INSTANTIATION_TRLWE(INST)
#undef INST

template <class P>
TRLWE<P> trlweSymEncrypt(const array<typename P::T, P::n> &p,
                         const double alpha, const Key<P> &key)
{
    TRLWE<P> c = trlweSymEncryptZero<P>(alpha, key);
    for (int i = 0; i < P::n; i++) c[1][i] += p[i];
    return c;
}
#define INST(P)                                                               \
    template TRLWE<P> trlweSymEncrypt<P>(const array<typename P::T, P::n> &p, \
                                         const double alpha,                  \
                                         const Key<P> &key)
TFHEPP_EXPLICIT_INSTANTIATION_TRLWE(INST)
#undef INST

template <class P>
array<double, P::n> trlweSymDecryptDecode(const TRLWE<P> &c, const Key<P> &key,
                                          Encoder &encoder)
{
    Polynomial<P> mulres;
    PolyMul<P>(mulres, c[0], key);
    Polynomial<P> phase = c[1];
    for (int i = 0; i < P::n; i++) phase[i] -= mulres[i];

    array<double, P::n> p;
    for (int i = 0; i < P::n; i++) p[i] = encoder.decode(phase[i]);
    return p;
}
#define INST(P)                                            \
    template array<double, P::n> trlweSymDecryptDecode<P>( \
        const TRLWE<P> &c, const Key<P> &key, Encoder &encoder)
TFHEPP_EXPLICIT_INSTANTIATION_TRLWE(INST)
#undef INST

template <class P>
TRLWE<P> trlweSymIntEncrypt(const array<typename P::T, P::n> &p,
                            const double alpha, const Key<P> &key)
{
    TRLWE<P> c = trlweSymEncryptZero<P>(alpha, key);
    for (int i = 0; i < P::n; i++)
        c[1][i] += static_cast<typename P::T>(P::delta * p[i]);
    return c;
}
#define INST(P)                                                  \
    template TRLWE<P> trlweSymIntEncrypt<P>(                     \
        const array<typename P::T, P::n> &p, const double alpha, \
        const Key<P> &key)
TFHEPP_EXPLICIT_INSTANTIATION_TRLWE(INST)
#undef INST

template <class P>
array<bool, P::n> trlweSymDecrypt(const TRLWE<P> &c, const Key<P> &key)
{
    Polynomial<P> mulres;
    PolyMul<P>(mulres, c[0], key);
    Polynomial<P> phase = c[1];
    for (int i = 0; i < P::n; i++) phase[i] -= mulres[i];

    array<bool, P::n> p;
    for (int i = 0; i < P::n; i++)
        p[i] = static_cast<typename make_signed<typename P::T>::type>(
                   phase[i]) > 0;
    return p;
}
#define INST(P)                                                      \
    template array<bool, P::n> trlweSymDecrypt<P>(const TRLWE<P> &c, \
                                                  const Key<P> &key)
TFHEPP_EXPLICIT_INSTANTIATION_TRLWE(INST)
#undef INST

template <class P>
void InverseSampleExtractIndex(TRLWE<P> &trlwe, const TLWE<P> &tlwe,
                               const int index)
{
    for (int i = 0; i <= index; i++) trlwe[0][index - i] = tlwe[i];
    for (int i = index + 1; i < P::n; i++)
        trlwe[0][P::n + index - i] = -tlwe[i];
    trlwe[1][index] = tlwe[P::n];
}
#define INST(P)                                 \
    template void InverseSampleExtractIndex<P>( \
        TRLWE<P> & trlwe, const TLWE<P> &tlwe, const int index)
TFHEPP_EXPLICIT_INSTANTIATION_TRLWE(INST)
#undef INST

template <class P>
Polynomial<P> trlweSymIntDecrypt(const TRLWE<P> &c, const Key<P> &key)
{
    Polynomial<P> mulres;
    PolyMul<P>(mulres, c[0], key);
    Polynomial<P> phase = c[1];
    for (int i = 0; i < P::n; i++) phase[i] -= mulres[i];

    Polynomial<P> p;
    for (int i = 0; i < P::n; i++)
        p[i] = static_cast<typename P::T>(std::round(phase[i] / P::delta)) %
               P::plain_modulus;
    return p;
}
#define INST(P)                                                     \
    template Polynomial<P> trlweSymIntDecrypt<P>(const TRLWE<P> &c, \
                                                 const Key<P> &key)
TFHEPP_EXPLICIT_INSTANTIATION_TRLWE(INST)
#undef INST

template <class P>
void SampleExtractIndex(TLWE<P> &tlwe, const TRLWE<P> &trlwe, const int index)
{
    for (int i = 0; i <= index; i++) tlwe[i] = trlwe[0][index - i];
    for (int i = index + 1; i < P::n; i++)
        tlwe[i] = -trlwe[0][P::n + index - i];
    tlwe[P::n] = trlwe[1][index];
}
#define INST(P)                                                                \
    template void SampleExtractIndex<P>(TLWE<P> & tlwe, const TRLWE<P> &trlwe, \
                                        const int index)
TFHEPP_EXPLICIT_INSTANTIATION_TRLWE(INST)
#undef INST
}  // namespace TFHEpp
