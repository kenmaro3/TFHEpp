#pragma once

#include <cstdint>
#include <cmath>

struct lvl0param {
    static constexpr std::uint32_t n = 635; //dimension
    static const inline double alpha = std::pow(2.0, -15); //fresh noise
    using T = uint32_t; //Torus representation
    static constexpr T mu = 1U << 29;
};

struct lvl1param {
    static constexpr std::uint32_t nbit = 13; //dimension must be a power of 2 for ease of polynomial multiplication.
    static constexpr std::uint32_t n = 1<<nbit; //dimension
    static constexpr std::uint32_t l = 15;
    static constexpr std::uint32_t Bgbit = 2;
    static constexpr std::uint32_t Bg = 1<<Bgbit;
    static const inline double alpha = std::pow(2.0, -25); //fresh noise
    using T = uint32_t; //Torus representation
    static constexpr T mu = 1U << 29;
};

struct lvl2param {
    static const std::uint32_t nbit = 11; //dimension must be a power of 2 for ease of polynomial multiplication.
    static constexpr std::uint32_t n = 1<<nbit; //dimension
    static constexpr std::uint32_t l = 3;
    static constexpr std::uint32_t Bgbit = 11;
    static constexpr std::uint32_t Bg = 1<<Bgbit;
    static const inline double alpha = std::pow(2.0, -44); //fresh noise
    using T = uint64_t; //Torus representation
    static constexpr T mu = 1ULL << 61;
};

//Key Switching parameters
struct lvl10param {
    static constexpr std::uint32_t t = 15; //number of addition in keyswitching
    static constexpr std::uint32_t basebit = 2; //how many bit should be encrypted in keyswitching key
    static const inline double alpha = lvl0param::alpha; //key noise
    using domainP = lvl1param;
    using targetP = lvl0param;
};

struct lvl20param{
    static constexpr std::uint32_t t = 7; //number of addition in keyswitching
    static constexpr std::uint32_t basebit = 2; //how many bit should be encrypted in keyswitching key
    static const inline double alpha = lvl0param::alpha; //key noise
    using domainP = lvl2param;
    using targetP = lvl0param;
};

struct lvl21param{
    static constexpr std::uint32_t t = 10; //number of addition in keyswitching
    static constexpr std::uint32_t basebit = 3; //how many bit should be encrypted in keyswitching key
    static const inline double alpha = lvl1param::alpha; //key noise
    using domainP = lvl2param;
    using targetP = lvl1param;
};

struct lvl22param{
    static constexpr std::uint32_t t = 8; //number of addition in keyswitching
    static constexpr std::uint32_t basebit = 4; //how many bit should be encrypted in keyswitching key
    static const inline double alpha = lvl2param::alpha; //key noise
    using domainP = lvl2param;
    using targetP = lvl2param;
};