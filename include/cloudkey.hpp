#pragma once

#include <params.hpp>
#include <tlwe.hpp>
#include <trgsw.hpp>
#include <trlwe.hpp>

namespace TFHEpp {
struct GateKey {
    alignas(32) KeySwitchingKey ksk;
    alignas(32) BootStrappingKeyFFTlvl01 bkfftlvl01;
    GateKey(SecretKey sk);
    GateKey() {}
};

struct CircuitKey {
    alignas(32) PrivKeySwitchKey privksk;
    alignas(32) BootStrappingKeyFFTlvl02 bkfftlvl02;
    CircuitKey(SecretKey sk);
    CircuitKey() {}
};

struct CloudKey {
    GateKey gk;
    CircuitKey ck;
    lweParams params;
    CloudKey(SecretKey sk) : gk(sk), ck(sk) {}
};
}  // namespace TFHEpp