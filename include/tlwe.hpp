#pragma once

#include <array>
#include <cstdint>
#include <vector>
#include <stdio.h>
#include <cassert>

#include "./key.hpp"
#include "./params.hpp"
#include "./utils.hpp"

namespace TFHEpp {
using namespace std;


class Encoder
{
    public:
        double a;
        double b;
        double d;
        double d2;
        double half;
        double half_d;
        int bp;


        void print(){
            printf("\nEncoder Print\n");
            printf("a     : %f\n", this->a);
            printf("b     : %f\n", this->b);
            printf("d     : %f\n", this->d);
            printf("half  : %f\n", this->half);
            printf("half_d: %f\n", this->half_d);
            printf("bp    : %d\n", this->bp);

        }

        Encoder(){
            this->a = -10.;
            this->b = 10.;
            this->d = b-a;
            this->d2 = b-a;
            this->half_d = (b-a)/2.;
            this->half = (b+a)/2.;

            this->bp = 16;
        };

        Encoder(double a, double b, int bp){
            this->a = a;
            this->b = b;
            this->d = b-a;
            this->d2 = b-a;
            this->half_d = (b-a)/2.;
            this->half = (b+a)/2.;

            this->bp = bp;

        }

        double encode_0_1(double x) const{
            return (x-this->a)/this->d;

        }

        uint32_t dtotx(double d) const{
            return uint32_t(int64_t((d - int64_t(d)) * (1LL << this->bp)));
        }

        uint32_t encode(double x) const{
            assert(x >= this->a);
            assert(x <= this->b);
            if (x == this->a) x = encode_sanitize(x);
            return dtotx((x-this->a)/this->d);
        }

        double txtod(uint32_t x) const{
            double tmp_0_1 = static_cast<double>(x) / pow(2, this->bp);
            return tmp_0_1;
        }

        double t32tod(uint32_t x) const{
            double tmp_0_1 = static_cast<double>(x) / pow(2, 32);
            return tmp_0_1;
        }

        double decode(const uint32_t x){
            double tmp_0_1 = this->txtod(x);
            tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
            return tmp_0_1 * this->d + this->a;
        }

};



template <class P>
array<typename P::T, P::n + 1> tlweSymEncodeEncrypt(
    const double &x, const double alpha,
    const array<typename P::T, P::n> &key, const Encoder &encoder);

template <class P>
double tlweSymDecryptDecode(const TLWE<P> &c, const Key<P> &key, Encoder &encoder);

template <class P>
array<typename P::T, P::n + 1> tlweSymEncrypt(
    const typename P::T p, const double alpha,
    const array<typename P::T, P::n> &key);

template <class P>
bool tlweSymDecrypt(const TLWE<P> &c, const Key<P> &key);

template <class P>
typename P::T tlweSymDecryptRaw(const TLWE<P> &c, const Key<P> &key);

vector<TLWE<lvl0param>> bootsSymEncrypt(const vector<uint8_t> &p,
                                        const SecretKey &sk);
vector<uint8_t> bootsSymDecrypt(const vector<TLWE<lvl0param>> &c,
                                const SecretKey &sk);
}  // namespace TFHEpp