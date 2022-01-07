#pragma once

#include <cmath>
#include <cstdint>

struct lvl0param {
    static constexpr std::uint32_t n = 500;
    //static constexpr std::uint32_t n = 4;
    static const inline double alpha = 2.44e-5;
    //static const inline double alpha = 0.0;
    using T = uint32_t;
    static constexpr T mu = 1U << 29;
};

// FMT_LVL1PARAM 


struct lvl2param {
    static const std::uint32_t nbit = 11;
    static constexpr std::uint32_t n = 1 << nbit;
    static constexpr std::uint32_t l = 4;
    static constexpr std::uint32_t Bgbit = 9;
    static constexpr std::uint32_t Bg = 1 << Bgbit;
    static const inline double alpha = std::pow(2.0, -44);
    using T = uint64_t;
    static constexpr T mu = 1ULL << 61;
    static constexpr uint32_t plain_modulus = 8;
    static constexpr double delta = mu;
};

// FMT_LVL01PARAM



// dummy
struct lvl11param {
    static constexpr std::uint32_t t = 0;  // number of addition in keyswitching
    static constexpr std::uint32_t basebit =
        0;  // how many bit should be encrypted in keyswitching key
    static const inline double alpha = lvl1param::alpha;  // key noise
    using domainP = lvl1param;
    using targetP = lvl1param;
};

struct lvl21param {
    static constexpr std::uint32_t t = 10;
    static constexpr std::uint32_t basebit = 3;
    static const inline double alpha = std::pow(2, -31);
    using domainP = lvl2param;
    using targetP = lvl1param;
};

// Dummy
struct lvl20param {
    static constexpr std::uint32_t t = 0;  // number of addition in keyswitching
    static constexpr std::uint32_t basebit =
        0;  // how many bit should be encrypted in keyswitching key
    static const inline double alpha = lvl0param::alpha;  // key noise
    using domainP = lvl2param;
    using targetP = lvl0param;
};

// Dummy
struct lvl22param {
    static constexpr std::uint32_t t = 0;
    static constexpr std::uint32_t basebit = 0;
    static const inline double alpha = lvl2param::alpha;
    using domainP = lvl2param;
    using targetP = lvl2param;
};
