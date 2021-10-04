#pragma once

#include <bits/stdint-uintn.h>

#include <cmath>
#include <limits>

#include "cloudkey.hpp"
#include "detwfa.hpp"
#include "encoder.hpp"
#include "keyswitch.hpp"
#include "params.hpp"
#include "trlwe.hpp"
#include "utils.hpp"

namespace TFHEpp {

template <class P, uint32_t num_out = 1>
void BlindRotate(TRLWE<typename P::targetP> &res,
                 const TLWE<typename P::domainP> &tlwe,
                 const BootstrappingKeyFFT<P> &bkfft,
                 const Polynomial<typename P::targetP> &testvector)
{
    constexpr uint32_t bitwidth = bits_needed<num_out - 1>();
    const uint32_t bbar =
        2 * P::targetP::n - (tlwe[P::domainP::n] >>(std::numeric_limits<typename P::domainP::T>::digits - 1 - P::targetP::nbit + bitwidth) );
    res[0] = {};
    PolynomialMulByXai<typename P::targetP>(res[1], testvector, bbar);
    for (int i = 0; i < P::domainP::n; i++) {
        constexpr typename P::domainP::T roundoffset = 1ULL << (std::numeric_limits<typename P::domainP::T>::digits - 2 - P::targetP::nbit + bitwidth);
        const uint32_t abar = (tlwe[i]+roundoffset)>>(std::numeric_limits<typename P::domainP::T>::digits - 1 - P::targetP::nbit + bitwidth);
        if (abar == 0) continue;
        // Do not use CMUXFFT to avoid unnecessary copy.
        CMUXFFTwithPolynomialMulByXaiMinusOne<typename P::targetP>(
            res, bkfft[i], abar);
    }
}

template <class P, uint32_t num_out = 1>
void BlindRotate(TRLWE<typename P::targetP> &res,
                 const TLWE<typename P::domainP> &tlwe,
                 const BootstrappingKeyFFT<P> &bkfft,
                 const TRLWE<typename P::targetP> &testvector)
{
    constexpr uint32_t bitwidth = bits_needed<num_out - 1>();
    const uint32_t bbar =
        2 * P::targetP::n - (tlwe[P::domainP::n] >>(std::numeric_limits<typename P::domainP::T>::digits - 1 - P::targetP::nbit + bitwidth) );
    PolynomialMulByXai<typename P::targetP>(res[0], testvector[0], bbar);
    PolynomialMulByXai<typename P::targetP>(res[1], testvector[1], bbar);
    for (int i = 0; i < P::domainP::n; i++) {
        constexpr typename P::domainP::T roundoffset = 1ULL << (std::numeric_limits<typename P::domainP::T>::digits - 2 - P::targetP::nbit + bitwidth);
        const uint32_t abar = (tlwe[i]+roundoffset)>>(std::numeric_limits<typename P::domainP::T>::digits - 1 - P::targetP::nbit + bitwidth);
        if (abar == 0) continue;
        // Do not use CMUXFFT to avoid unnecessary copy.
        CMUXFFTwithPolynomialMulByXaiMinusOne<typename P::targetP>(
            res, bkfft[i], abar);
    }
}

template <class P>
void ProgrammableBootstrappingTLWE2TRLWEFFTWITHKEY(
    TRLWE<typename P::targetP> &acc, const TLWE<typename P::domainP> &tlwe,
    const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder, Key<lvl0param> sk);

template <class P>
void ProgrammableBootstrappingTLWE2TRLWEFFT(
    TRLWE<typename P::targetP> &acc, const TLWE<typename P::domainP> &tlwe,
    const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder_domain,
    Encoder &encoder_target);

template <class P>
void ProgrammableBootstrappingTLWE2TLWEFFT(
    TLWE<typename P::targetP> &res, const TLWE<typename P::domainP> &tlwe,
    const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder_domain,
    Encoder &encoder_target);

template <class P>
void ProgrammableBootstrappingTLWE2TLWEFFTDEBUG(
    TLWE<typename P::targetP> &res, const TLWE<typename P::domainP> &tlwe,
    const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder_domain,
    Encoder &encoder_target);

template <class P>
void GateBootstrappingTLWE2TRLWEFFT(TRLWE<typename P::targetP> &acc,
                                    const TLWE<typename P::domainP> &tlwe,
                                    const BootstrappingKeyFFT<P> &bkfft);
template <class P, uint32_t num_out = 1>
void BlindRotate(TRLWE<typename P::targetP> &res,
                 const TLWE<typename P::domainP> &tlwe,
                 const BootstrappingKeyNTT<P> &bkntt,
                 const Polynomial<typename P::targetP> &testvector)
{
    constexpr uint32_t bitwidth = bits_needed<num_out - 1>();
    const uint32_t bbar =
        2 * P::targetP::n - (tlwe[P::domainP::n] >>(std::numeric_limits<typename P::domainP::T>::digits - 1 - P::targetP::nbit + bitwidth) );
    res[0] = {};
    PolynomialMulByXai<typename P::targetP>(res[1], testvector, bbar);
    for (int i = 0; i < P::domainP::n; i++) {
        constexpr typename P::domainP::T roundoffset = 1ULL << (std::numeric_limits<typename P::domainP::T>::digits - 2 - P::targetP::nbit + bitwidth);
        const uint32_t abar = (tlwe[i]+roundoffset)>>(std::numeric_limits<typename P::domainP::T>::digits - 1 - P::targetP::nbit + bitwidth);
        if (abar == 0) continue;
        // Do not use CMUXNTT to avoid unnecessary copy.
        CMUXNTTwithPolynomialMulByXaiMinusOne<typename P::targetP>(
            res, bkntt[i], abar);
    }
}

template <class P>
void GateBootstrappingTLWE2TLWEFFT(
    TLWE<typename P::targetP> &res, const TLWE<typename P::domainP> &tlwe,
    const BootstrappingKeyFFT<P> &bkfft,
    const Polynomial<typename P::targetP> &testvector);

template <class P, uint32_t num_out>
void GateBootstrappingManyLUT(
    std::array<TLWE<typename P::targetP>, num_out> &res,
    const TLWE<typename P::domainP> &tlwe, const BootstrappingKeyFFT<P> &bkfft,
    const Polynomial<typename P::targetP> &testvector)
{
    TRLWE<typename P::targetP> acc;
    BlindRotate<P, num_out>(acc, tlwe, bkfft, testvector);
    for (int i = 0; i < num_out; i++)
        SampleExtractIndex<typename P::targetP>(res[i], acc, i);
}

template <class P>
void GateBootstrappingTLWE2TLWEFFTvariableMu(
    TLWE<typename P::targetP> &res, const TLWE<typename P::domainP> &tlwe,
    const BootstrappingKeyFFT<P> &bkfft, const typename P::targetP::T mus2);

void ProgrammableBootstrapping(TLWE<lvl0param> &res,
                               const TLWE<lvl0param> &tlwe, const GateKey &gk,
                               Encoder &encoder_domain, Encoder &encoder_target,
                               CustomTestVector<lvl1param> &function);

void ProgrammableBootstrappingWithoutKS(TLWE<lvl1param> &res,
                                        const TLWE<lvl0param> &tlwe,
                                        const GateKey &gk,
                                        Encoder &encoder_domain,
                                        Encoder &encoder_target,
                                        CustomTestVector<lvl1param> &function);
template <class P, typename P::T mu>
constexpr Polynomial<P> mupolygen()
{
    Polynomial<P> poly;
    for (typename P::T &p : poly) p = mu;
    return poly;
}

template <typename lvl1param::T mu = lvl1param::mu>
void GateBootstrapping(TLWE<lvl0param> &res, const TLWE<lvl0param> &tlwe,
                       const GateKey &gk)
{
    TLWE<lvl1param> tlwelvl1;
    GateBootstrappingTLWE2TLWEFFT<lvl01param>(tlwelvl1, tlwe, gk.bkfftlvl01,
                                              mupolygen<lvl1param, mu>());
    IdentityKeySwitch<lvl10param>(res, tlwelvl1, gk.ksk);
}

template <class P>
void ProgrammableBootstrappingTLWE2TRLWEFFT(
    TRLWE<typename P::targetP> &acc, const TLWE<typename P::domainP> &tlwe,
    const BootstrappingKeyFFT<P> &bkfft, Encoder &encoder_domain,
    Encoder &encoder_target);

}  // namespace TFHEpp