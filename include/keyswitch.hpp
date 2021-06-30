#pragma once

#include <array>

#include "./params.hpp"
#include "./tlwe.hpp"

namespace TFHEpp {
using namespace std;

template <class P>
void IdentityKeySwitchWITHEncoder(TLWE<typename P::targetP> &res,
                       const TLWE<typename P::domainP> &tlwe,
                       const KeySwitchingKey<P> &ksk, Encoder &encoder_domain, Encoder &encoder_target);

template <class P>
void IdentityKeySwitch(TLWE<typename P::targetP> &res,
                       const TLWE<typename P::domainP> &tlwe,
                       const KeySwitchingKey<P> &ksk);

template <class P>
void PrivKeySwitch(TRLWE<typename P::targetP> &res,
                   const TLWE<typename P::domainP> &tlwe,
                   const PrivKeySwitchKey<P> &privksk);
}  // namespace TFHEpp