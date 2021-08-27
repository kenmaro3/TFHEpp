#pragma once

#include <stdio.h>

#include <array>
#include <cassert>
#include <cstdint>
#include <vector>

#include "encoder.hpp"
#include "key.hpp"
#include "params.hpp"

namespace TFHEpp {
using namespace std;

template <class P>
array<typename P::T, P::n + 1> tlweSymEncodeEncrypt(
    const double &x, const double alpha, const array<typename P::T, P::n> &key,
    const Encoder &encoder);

template <class P>
double tlweSymDecryptDecode(const TLWE<P> &c, const Key<P> &key,
                            const Encoder &encoder);

template <class P>
array<typename P::T, P::n + 1> tlweSymEncrypt(
    const typename P::T p, const double alpha,
    const array<typename P::T, P::n> &key);

template <class P>
bool tlweSymDecrypt(const TLWE<P> &c, const Key<P> &key);

template <class P = lvl0param>
vector<TLWE<P>> bootsSymEncrypt(const vector<uint8_t> &p, const SecretKey &sk);
template <class P = lvl0param>
vector<uint8_t> bootsSymDecrypt(const vector<TLWE<P>> &c, const SecretKey &sk);
}  // namespace TFHEpp
