#pragma once

#include <array>

#include "./utils.hpp"
#include "./tlwe.hpp"

namespace TFHEpp {
using namespace std;

template <class P>
TRLWE<P> trlweSymEncryptZero(const double alpha, const Key<P> &key);

template <class P>
TRLWE<P> trlweSymEncodeEncrypt(const array<double, P::n> &p, const double alpha,
                         const Key<P> &key, Encoder &encoder);

template <class P>
TRLWE<P> trlweSymEncrypt(const array<typename P::T, P::n> &p, const double alpha,
                         const Key<P> &key);

template <class P>
array<double, P::n> trlweSymDecryptDecode(const TRLWE<P> &c, const Key<P> &key, Encoder &encoder);

template <class P>
array<bool, P::n> trlweSymDecrypt(const TRLWE<P> &c, const Key<P> &key);

template <class P>
void InverseSampleExtractIndex(TRLWE<P> &trlwe, const TLWE<P> &tlwe, const int index);

template <class P>
void SampleExtractIndex(TLWE<P> &tlwe, const TRLWE<P> &trlwe, const int index);
}  // namespace TFHEpp