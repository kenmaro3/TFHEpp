#pragma once

#include "trgsw.hpp"

namespace TFHEpp {
template <class P>
void CMUXFFT(TRLWE<P> &res, const TRGSWFFT<P> &cs, const TRLWE<P> &c1,
             const TRLWE<P> &c0);

template <class P>
void CMUXFFTwithPolynomialMulByXaiMinusOne(TRLWE<P> &acc, const TRGSWFFT<P> &cs,
                                           const typename P::T a);

template <class P>
void CMUXFFTwithPolynomialMulByXai(TRLWE<P> &acc, const TRGSWFFT<P> &cs,
                                           const typename P::T a);

template <class P>
void CMUXFFTwithPolynomialMulByXaiWITHKEY(TRLWE<P> &acc, const TRGSWFFT<P> &cs,
                                           const typename P::T a, Key<lvl0param> sk, int index);

}  // namespace TFHEpp