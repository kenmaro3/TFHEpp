#pragma once

#include <array>
#include <params.hpp>

namespace TFHEpp {
using namespace std;

void IdentityKeySwitchlvl10(TLWElvl0 &res, const TLWElvl1 &tlwe,
                            const KeySwitchingKey &ksk);
void PrivKeySwitchlvl21(TRLWElvl1 &res, const TLWElvl2 &tlwe, const int u,
                        const PrivKeySwitchKey &privksk);
void PackingKeySwitchlvl01(TRLWElvl1 &res, const array<TLWElvl0,DEF_N> &tlwearray, const PackingKeySwitchKey &packksk);
}  // namespace TFHEpp