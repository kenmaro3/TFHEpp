#pragma once

#include <params.hpp>
#include <tlwe.hpp>
#include <trgsw.hpp>
#include <trlwe.hpp>
#include <unicodeparams.hpp>

namespace TFHEpp {
struct GateKey {
    KeySwitchingKey ksk;
    BootStrappingKeyFFTlvl01 bkfftlvl01;
    GateKey(SecretKey sk);
    GateKey() {}
};

struct CircuitKey {
    PrivKeySwitchKey privksk;
    BootStrappingKeyFFTlvl02 bkfftlvl02;
    CircuitKey(SecretKey sk);
    CircuitKey() {}
};

struct PackingKey{
    PackingKeylvl1 pack;
    PackingKey(SecretKey sk);
    PackingKey();
};

struct CloudKey {
    GateKey gk;
    CircuitKey ck;
    PackingKey pack;
    lweParams params;
    CloudKey(SecretKey sk) : gk(sk), ck(sk), pack(sk) {}
};
}  // namespace TFHEpp