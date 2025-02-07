#pragma once

#include <bits/stdint-uintn.h>
#include <randen.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <limits>
#include <random>

#include "encoder.hpp"
#include "params.hpp"

namespace TFHEpp {
static thread_local std::random_device trng;
static thread_local randen::Randen<uint64_t> generator(trng());

// https://qiita.com/saka1_p/items/e8c4dfdbfa88449190c5
template <typename T>
constexpr bool false_v = false;

template <class P>
class CustomTestVector {
public:
    virtual void custom_test_vector(
        std::array<std::array<typename P::T, P::n>, 2> &testvector,
        const uint32_t bara, Encoder &encoder_domain,
        Encoder &encoder_target) = 0;
};

template <class P>
class FunctionCustomTestVector : public CustomTestVector<P> {
public:
    virtual double run(double arg) = 0;

    void custom_test_vector(
        std::array<std::array<typename P::T, P::n>, 2> &testvector,
        const uint32_t bara, Encoder &encoder_domain, Encoder &encoder_target)
    {
        testvector[0] = {};
        for (int i = 0; i < P::n; i++) {
            double tmp = encoder_domain.a +
                         encoder_domain.d / 2. * double(i) / double(P::n);
            testvector[1][i] = encoder_target.encode(run(tmp));
        }
    }
};

template <class P>
class IdentityFunction : public FunctionCustomTestVector<P> {
public:
    IdentityFunction() {}
    double run(double x) { return x; }
};

template <class P>
class ReLUFunction : public FunctionCustomTestVector<P> {
public:
    ReLUFunction() {}
    double run(double x) { return x >= 0 ? x : 0.; }
};

template <class P>
class SigmoidFunction : public FunctionCustomTestVector<P> {
public:
    SigmoidFunction() {}
    double run(double x) { return 1. / (1. + pow(std::exp(1.0), x * (-1.))); }
};

template <class P>
class SquareDividedByFourFunction : public FunctionCustomTestVector<P> {
public:
    double run(double x) { return pow(x, 2) / 4.; }
};

template <class P>
class DirectCustomTestVector : public CustomTestVector<P> {
public:
    std::array<std::array<typename P::T, P::n>, 2> testvector;

    DirectCustomTestVector(
        std::array<std::array<typename P::T, P::n>, 2> &testvector)
    {
        this->testvector = testvector;
    }

    void custom_test_vector(
        std::array<std::array<typename P::T, P::n>, 2> &testvector,
        const uint32_t bara, Encoder &encoder_domain, Encoder &encoder_target)
    {
        testvector = this->testvector;
    }

    static void basic_custom_test_vector(std::array<double, P::n> &args, Encoder &encoder_domain){
        for (int i = 0; i < P::n; i++) 
            args[i] = encoder_domain.a +
                         encoder_domain.d / 2. * double(i) / double(P::n);
    }

    static DirectCustomTestVector from_unencoded(std::array<std::array<typename P::T, P::n>, 2> &testvector, std::array<double, P::n> &unencoded, Encoder &encoder_target){
        testvector[0] = {};

        for (int i = 0; i < P::n; i++) 
            testvector[1][i] = encoder_target.encode(unencoded[i]);

        return DirectCustomTestVector(testvector);
    }
};


inline double frand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

inline double encode_sanitize(double x) { return x + frand(0.00001, 0.001); }

inline double decode_sanitize(double x, double b)
{
    if (abs(x - b) < 0.001) {
        return 0.;
    }
    else {
        return x;
    }
}

// Double to Torus(32bit fixed-point number)
inline uint32_t dtot32(double d)
{
    return int32_t(int64_t((d - int64_t(d)) * (1LL << 32)));
}

inline uint32_t dtot30(double d)
{
    return int32_t(int64_t((d - int64_t(d)) * (1LL << 30)));
}

inline double t32tod(uint32_t x)
{
    double tmp_0_1 = static_cast<double>(x) / pow(2, 32);
    return tmp_0_1;
}

inline double t30tod(uint32_t x)
{
    double tmp_0_1 = static_cast<double>(x) / pow(2, 30);
    return tmp_0_1;
}

// Modular Gaussian Distribution over Torus
template <class P>
inline typename P::T ModularGaussian(typename P::T center, double stdev)
{
    if constexpr (std::is_same_v<typename P::T, uint32_t>) {
        // 32bit fixed-point number version
        std::normal_distribution<double> distribution(0., stdev);
        double err = distribution(generator);
        return center + dtot32(err);
    }
    else if constexpr (std::is_same_v<typename P::T, uint64_t>) {
        // 64bit fixed-point number version
        static const double _2p64 = std::pow(2., 64);
        std::normal_distribution<double> distribution(0., 1.0);
        const double val = stdev * distribution(generator) * _2p64;
        const uint64_t ival = static_cast<typename P::T>(val);
        return ival + center;
    }
    else
        static_assert(false_v<typename P::T>, "Undefined Modular Gaussian!");
}

template <class P, uint32_t bitwidth = 0>
inline typename P::T modSwitchFromTorus(uint32_t phase)
{
    constexpr uint32_t Mbit = P::nbit + 1 - bitwidth;
    static_assert(32 >= Mbit, "Undefined modSwitchFromTorus!");
    return ((phase + (1U << (31 - Mbit))) >> (32 - Mbit)) << bitwidth;
}

template <class P>
inline typename P::targetP::T modSwitchFromTorusSpecificTwoBP(
    typename P::domainP::T phase, int domain_bp, int target_bp)
{
    uint32_t tmp = domain_bp - P::targetP::nbit - 1;
    return ((phase + (1U << (tmp - 1))) >> tmp) %
           (1UL << (P::targetP::nbit + 1));
}

// https://stackoverflow.com/questions/21191307/minimum-number-of-bits-to-represent-a-given-int
template <uint32_t data>
constexpr int bits_needed()
{
    uint32_t value = data;
    int bits = 0;
    for (int bit_test = 16; bit_test > 0; bit_test >>= 1) {
        if (value >> bit_test != 0) {
            bits += bit_test;
            value >>= bit_test;
        }
    }
    return bits + value;
}

template <uint32_t N>
inline void MulInFD(std::array<double, N> &res, const std::array<double, N> &a,
                    const std::array<double, N> &b)
{
    for (int i = 0; i < N / 2; i++) {
        double aimbim = a[i + N / 2] * b[i + N / 2];
        double arebim = a[i] * b[i + N / 2];
        res[i] = std::fma(a[i], b[i], -aimbim);
        res[i + N / 2] = std::fma(a[i + N / 2], b[i], arebim);
    }
}

// removing inline seems to be faster in my environment.
template <uint32_t N>
void FMAInFD(std::array<double, N> &res, const std::array<double, N> &a,
             const std::array<double, N> &b)
{
    for (int i = 0; i < N / 2; i++) {
        res[i] = std::fma(a[i + N / 2], b[i + N / 2], -res[i]);
        res[i] = std::fma(a[i], b[i], -res[i]);
        res[i + N / 2] = std::fma(a[i], b[i + N / 2], res[i + N / 2]);
        res[i + N / 2] = std::fma(a[i + N / 2], b[i], res[i + N / 2]);
    }
}

template <class P>
inline void PolynomialMulByXai(Polynomial<P> &res, const Polynomial<P> &poly,
                               const typename P::T a)
{
    if (a == 0)
        res = poly;
    else if (a < P::n) {
        for (int i = 0; i < a; i++) res[i] = -poly[i - a + P::n];
        for (int i = a; i < P::n; i++) res[i] = poly[i - a];
    }
    else {
        const typename P::T aa = a - P::n;
        for (int i = 0; i < aa; i++) res[i] = poly[i - aa + P::n];
        for (int i = aa; i < P::n; i++) res[i] = -poly[i - aa];
    }
}

template <class P>
inline void PolynomialMulByXaiMinusOne(Polynomial<P> &res,
                                       const Polynomial<P> &poly,
                                       const typename P::T a)
{
    if (a < P::n) {
        for (int i = 0; i < a; i++) res[i] = -poly[i - a + P::n] - poly[i];
        for (int i = a; i < P::n; i++) res[i] = poly[i - a] - poly[i];
    }
    else {
        const typename P::T aa = a - P::n;
        for (int i = 0; i < aa; i++) res[i] = poly[i - aa + P::n] - poly[i];
        for (int i = aa; i < P::n; i++) res[i] = -poly[i - aa] - poly[i];
    }
}

// calcurate τ_d
template <class P>
inline void Automorphism(Polynomial<P> &res, const Polynomial<P> &poly,
                         const uint d)
{
    res = {};
    constexpr uint Nmask = (1ULL << (P::nbit)) - 1;
    constexpr uint signmask = 1ULL << (P::nbit);
    for (uint i = 0; i < P::n; i++) {
        const uint index = i * d;
        if (index & signmask)
            res[index & Nmask] -= poly[i];
        else
            res[index & Nmask] += poly[i];
    }
}
}  // namespace TFHEpp
