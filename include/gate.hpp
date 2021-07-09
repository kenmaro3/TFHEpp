#pragma once

#include "./cloudkey.hpp"
#include "./tlwe.hpp"

namespace TFHEpp {
using namespace std;
void HomCONSTANTONE(TLWE<lvl0param> &res);
void HomCONSTANTZERO(TLWE<lvl0param> &res);
void HomNOT(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca);
void HomCOPY(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca);
void adjust_bp(TLWE<lvl0param> &c, Encoder &encoder, int bp_diff);
void HomADD(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const TLWE<lvl0param> &cb, Encoder &encoder1, Encoder &encoder2);
void HomADDFixedEncoder(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const TLWE<lvl0param> &cb, Encoder &encoder1, Encoder &encoder2);
void HomADD(TRLWE<lvl1param> &res, const TRLWE<lvl1param> &ca, const TRLWE<lvl1param> &cb);
void HomADDCONST(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const double &b, Encoder &encoder);
void HomADDCONST(TRLWE<lvl1param> &res, const TRLWE<lvl1param> &ca, const array<double, lvl1param::n> &b, Encoder &encoder);
void HomSUB(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const TLWE<lvl0param> &cb, Encoder &encoder1, Encoder &encoder2);
void HomSUBFixedEncoder(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const TLWE<lvl0param> &cb, Encoder &encoder1, Encoder &encoder2);
void HomSUB(TRLWE<lvl1param> &res, const TRLWE<lvl1param> &ca, const TRLWE<lvl1param> &cb);
//void HomMULTCONSTINT(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const int &b);
void HomMULTCONSTINT(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const int &b, Encoder &encoder);
void HomMULTCONSTINT(array<uint64_t, lvl0param::n+1> &res, const array<uint64_t, lvl0param::n+1> &ca, const int &b, Encoder &encoder);
void HomMULTCONSTREAL(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const double &b, Encoder &encoder, int mult_bp, double mult_max);
void HomMULTCONSTREAL2(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const double &b, Encoder &encoder);
void HomMULTCONSTREAL3(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const double &b, Encoder &encoder, int mult_bp, double mult_max);
void HomMULTCONSTREAL4(array<uint64_t, lvl0param::n+1> &res, const array<uint64_t, lvl0param::n+1> &ca, const double &b, Encoder &encoder, int mult_bp, double mult_max);
void HomMULTCONSTREAL5(array<uint64_t, lvl0param::n+1> &res, const array<uint64_t, lvl0param::n+1> &ca, const double &b, Encoder &encoder, int mult_bp, double mult_max);
void convert_t_to_t2(array<uint64_t, lvl0param::n+1> &res, const TLWE<lvl0param> &ca);
void HomMULTCONST01(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const double &b, Encoder &encoder, int mult_bp);
void HomMAX(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca, const TLWE<lvl0param> &cb, Encoder &encoder1, Encoder &encoder2, Encoder &encoder_bs, GateKey &gk);
void HomNAND(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
             const TLWE<lvl0param> &cb, const GateKey &gk);
void HomNOR(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
            const TLWE<lvl0param> &cb, const GateKey &gk);
void HomXNOR(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
             const TLWE<lvl0param> &cb, const GateKey &gk);
void HomAND(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
            const TLWE<lvl0param> &cb, const GateKey &gk);
void HomOR(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
           const TLWE<lvl0param> &cb, const GateKey &gk);
void HomXOR(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
            const TLWE<lvl0param> &cb, const GateKey &gk);
void HomANDNY(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
              const TLWE<lvl0param> &cb, const GateKey &gk);
void HomANDYN(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
              const TLWE<lvl0param> &cb, const GateKey &gk);
void HomORNY(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
             const TLWE<lvl0param> &cb, const GateKey &gk);
void HomORYN(TLWE<lvl0param> &res, const TLWE<lvl0param> &ca,
             const TLWE<lvl0param> &cb, const GateKey &gk);
void HomMUX(TLWE<lvl0param> &res, const TLWE<lvl0param> &cs,
            const TLWE<lvl0param> &c1, const TLWE<lvl0param> &c0,
            const GateKey &gk);
void HomNMUX(TLWE<lvl0param> &res, const TLWE<lvl0param> &cs,
            const TLWE<lvl0param> &c1, const TLWE<lvl0param> &c0,
            const GateKey &gk);
template <class P>
void HomMUXwoSE(TRLWE<typename P::targetP> &res,
                const TLWE<typename P::domainP> &cs,
                const TLWE<typename P::domainP> &c1,
                const TLWE<typename P::domainP> &c0,
                const BootstrappingKeyFFT<P> &bkfft);
void ExtractSwitchAndHomMUX(TRLWE<lvl1param> &res, const TRLWE<lvl1param> &csr,
                            const TRLWE<lvl1param> &c1r,
                            const TRLWE<lvl1param> &c0r, const GateKey &gk);
}  // namespace TFHEpp