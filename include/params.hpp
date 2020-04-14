#pragma once
#include <array>
#include <cmath>
#include <cstdint>

namespace TFHEpp {
using namespace std;

// Use old 80bit security parameters. It is faster, but not recommended.
#define USE_80BIT_SECURITY

#ifdef USE_80BIT_SECURITY
constexpr uint32_t DEF_n = 500;
constexpr uint32_t DEF_Nbit = 10;
constexpr uint32_t DEF_N = 1 << DEF_Nbit;
constexpr uint32_t DEF_l = 2;
constexpr uint32_t DEF_Bgbit = 10;
constexpr uint32_t DEF_Bg = 1 << DEF_Bgbit;
constexpr uint32_t DEF_t = 8;
constexpr uint32_t DEF_basebit = 2;

constexpr uint32_t DEF_nbarbit = 11;
constexpr uint32_t DEF_nbar = 1 << DEF_nbarbit;
constexpr uint32_t DEF_lbar = 4;
constexpr uint32_t DEF_Bgbitbar = 9;
constexpr uint32_t DEF_Bgbar = 1 << DEF_Bgbitbar;
constexpr uint32_t DEF_tbar = 10;
constexpr uint32_t DEF_basebitlvl21 = 3;

constexpr uint32_t DEF_lpk = 3;
constexpr uint32_t DEF_Bgpkbit = 7;
constexpr uint32_t DEF_Bgpk = 1<<DEF_Bgpkbit;
#else
constexpr uint32_t DEF_n = 630;
constexpr uint32_t DEF_Nbit = 10;
constexpr uint32_t DEF_N = 1 << DEF_Nbit;
constexpr uint32_t DEF_l = 3;
constexpr uint32_t DEF_Bgbit = 6;
constexpr uint32_t DEF_Bg = 1 << DEF_Bgbit;
constexpr uint32_t DEF_t = 8;
constexpr uint32_t DEF_basebit = 2;

constexpr uint32_t DEF_nbarbit = 11;
constexpr uint32_t DEF_nbar = 1 << DEF_nbarbit;
constexpr uint32_t DEF_lbar = 4;
constexpr uint32_t DEF_Bgbitbar = 9;
constexpr uint32_t DEF_Bgbar = 1 << DEF_Bgbitbar;
constexpr uint32_t DEF_tbar = 10;
constexpr uint32_t DEF_basebitlvl21 = 3;

constexpr uint32_t DEF_lpk = 3;
constexpr uint32_t DEF_Bgpkbit = 7;
constexpr uint32_t DEF_Bgpk = 1<<DEF_Bgpkbit;
#endif

constexpr uint32_t offsetgenlvl1()
{
    uint32_t offset = 0;
    for (int i = 1; i <= DEF_l; i++)
        offset += DEF_Bg / 2 * (1U << (32 - i * DEF_Bgbit));
    return offset;
}

constexpr uint64_t offsetgenlvl2()
{
    uint64_t offset = 0;
    for (int i = 1; i <= DEF_lbar; i++)
        offset += DEF_Bgbar / 2 * (1UL << (64 - i * DEF_Bgbitbar));
    return offset;
}

constexpr uint32_t offsetpkgenlvl1()
{
    uint32_t offset = 0;
    for (int i = 1; i <= DEF_lpk; i++)
        offset += DEF_Bgpk / 2 * (1U << (32 - i * DEF_Bgpkbit));
    return offset;
}

constexpr uint32_t DEF_offsetlvl1 = offsetgenlvl1();
constexpr uint64_t DEF_offsetlvl2 = offsetgenlvl2();
constexpr uint32_t DEF_offsetpklvl1 = offsetpkgenlvl1();

using Keylvl0 = array<uint32_t, DEF_n>;
using Keylvl1 = array<uint32_t, DEF_N>;
using Keylvl2 = array<uint64_t, DEF_nbar>;

using TLWElvl0 = array<uint32_t, DEF_n + 1>;
using TLWElvl1 = array<uint32_t, DEF_N + 1>;
using TLWElvl2 = array<uint64_t, DEF_nbar + 1>;
using PrepackTLWElvl1 = array<TLWElvl1,DEF_N>;

using Polynomiallvl1 = array<uint32_t, DEF_N>;
using Polynomiallvl2 = array<uint64_t, DEF_nbar>;
using DecomposedPolynomiallvl1 = array<Polynomiallvl1 ,DEF_lpk>;
using PolynomialInFDlvl1 = array<double, DEF_N>;
using PolynomialInFDlvl2 = array<double, DEF_nbar>;
using DecomposedPolynomialInFDlvl1 = array<PolynomialInFDlvl1 ,DEF_lpk>;

using TRLWElvl1 = array<Polynomiallvl1, 2>;
using TRLWElvl2 = array<Polynomiallvl2, 2>;
using TRLWEInFDlvl1 = array<PolynomialInFDlvl1, 2>;
using TRLWEInFDlvl2 = array<PolynomialInFDlvl2, 2>;
using DecomposedTRLWElvl1 = array<Polynomiallvl1, 2 * DEF_l>;
using DecomposedTRLWElvl2 = array<Polynomiallvl2, 2 * DEF_lbar>;
using DecomposedTRLWEInFDlvl1 = array<PolynomialInFDlvl1, 2 * DEF_l>;
using DecomposedTRLWEInFDlvl2 = array<PolynomialInFDlvl2, 2 * DEF_lbar>;

using TRGSWlvl1 = array<TRLWElvl1, 2 * DEF_l>;
using TRGSWlvl2 = array<TRLWElvl2, 2 * DEF_lbar>;
using TRGSWFFTlvl1 = array<TRLWEInFDlvl1, 2 * DEF_l>;
using TRGSWFFTlvl2 = array<TRLWEInFDlvl2, 2 * DEF_lbar>;

using HalfTRGSWlvl1 = array<TRLWElvl1, DEF_lpk>;
using HalfTRGSWFFTlvl1 = array<TRLWEInFDlvl1, DEF_lpk>;

using BootStrappingKeyFFTlvl01 = array<TRGSWFFTlvl1, DEF_n>;
using BootStrappingKeyFFTlvl02 = array<TRGSWFFTlvl2, DEF_n>;

using KeySwitchingKey =
    array<array<array<TLWElvl0, (1 << DEF_basebit) - 1>, DEF_t>, DEF_N>;
using PrivKeySwitchKey =
    array<array<array<array<TRLWElvl1, (1 << DEF_basebitlvl21) - 1>, DEF_tbar>,
                DEF_nbar + 1>,
          2>;

using PackingKeylvl1 =
    array<HalfTRGSWFFTlvl1, DEF_N>;
}  // namespace TFHEpp