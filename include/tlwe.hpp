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
        double a;  // input lower bound
        double b;  // input upper bound
        double d;
        double d2;
        double half;
        double half_d;
        int bp;    // bit precision including noise bit (lvl0param::T - bp is padding bit)
        // bp = (noise bit + plaintext precision bit)


        void print(){
            printf("\nEncoder Print\n");
            printf("a     : %f\n", this->a);
            printf("b     : %f\n", this->b);
            printf("d     : %f\n", this->d);
            printf("half  : %f\n", this->half);
            printf("half_d: %f\n", this->half_d);
            printf("bp    : %d\n", this->bp);

        }

        static Encoder copy(Encoder &encoder){
            Encoder tmp(encoder.a, encoder.b, encoder.bp);
            return tmp;
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

        void update(double a, double b, int bp){
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

        double interpret_const(double x) const{
            double tmp1 = x - floor(x);
            double tmp2 = tmp1 * pow(2., this->bp);
            double tmp3 = round(tmp2);
            double tmp4 = tmp3 / pow(2., this->bp);
            return tmp4 + floor(x);
        }

        void negate(){
            this->d *= -1.;
            double tmp_a = this->a;
            this->a = this->b;
            this->b = tmp_a;
        }

        static lvl0param::T dtotx(double d, int bpx){
            double tmp = d - floor(d);
            tmp = tmp * pow(2., bpx);
            double tmp2 = tmp - floor(tmp);
            if(tmp2 < 0.5){
                return static_cast<lvl0param::T>(tmp);
            }else{
                return static_cast<lvl0param::T>(tmp+1);
            }

        }

        static lvl0param::T dtotx(double d, double max,  int bpx){
            d = d/max;
            double tmp = d - floor(d);
            tmp = tmp * pow(2., bpx);
            double tmp2 = tmp - floor(tmp);
            if(tmp2 < 0.5){
                return static_cast<lvl0param::T>(tmp);
            }else{
                return static_cast<lvl0param::T>(tmp+1);
            }
        }

        static uint64_t dtotx64(double d, double max,  int bpx){
            d = d/max;
            double tmp = d - floor(d);
            tmp = tmp * pow(2., bpx);
            double tmp2 = tmp - floor(tmp);
            //printf("tmp: %llu\n", tmp);
            if(tmp2 < 0.5){
                return static_cast<uint64_t>(tmp);
            }else{
                return static_cast<uint64_t>(tmp+1);
            }

        }

        lvl0param::T dtotx(double d) const{
            //return static_cast<lvl0param::T>(int64_t((d - int64_t(d)) * (1LL << this->bp)));
            double tmp = d - floor(d);
            tmp = tmp * pow(2., this->bp);
            double tmp2 = tmp - floor(tmp);
            if(tmp2 < 0.5){
                return static_cast<lvl0param::T>(tmp);
            }else{
                return static_cast<lvl0param::T>(tmp+1);
            }
        }

        uint64_t dtotx64(double d) const{
            //return static_cast<lvl0param::T>(int64_t((d - int64_t(d)) * (1LL << this->bp)));
            double tmp = d - floor(d);
            if(this->bp<=32){
                uint64_t tmp2 = uint64_t(tmp * pow(2, 32));
                //printf("tmp2 > dtotx64: %llu\n", tmp2);
                //printf("bp > dtotx64: %d\n", this->bp);
                return tmp2;
            }else{
                uint64_t tmp2 = uint64_t(tmp * pow(2, 32));
                //printf("tmp2 > dtotx64: %llu\n", tmp2);
                //printf("bp > dtotx64: %d\n", this->bp);
                tmp2 = tmp2 << (this->bp-32);
                //printf("tmp2 > dtotx64: %llu\n", tmp2);
                return tmp2;
            }
            //tmp = tmp * pow(2., this->bp);
            //double tmp2 = tmp - floor(tmp);
            //if(tmp2 < 0.5){
            //    return static_cast<uint64_t>(tmp);
            //}else{
            //    return static_cast<uint64_t>(tmp+1);
            //}
        }

        lvl0param::T encode(double x) const{
            assert(x >= this->a);
            assert(x <= this->b);
            if (x == this->a) x = encode_sanitize(x);
            return dtotx((x-this->a)/this->d);
        }

        uint64_t encode64(double x) const{
            assert(x >= this->a);
            assert(x <= this->b);
            if (x == this->a) x = encode_sanitize(x);
            double tmp = (x-this->a)/this->d;
            //printf("tmp encode: %f\n", tmp);
            return dtotx64(tmp);
            //return dtotx64((x-this->a)/this->d);
        }

        //double txtod(lvl0param::T x) const{
        //    double tmp_0_1 = static_cast<double>(x) / pow(2, this->bp);
        //    return tmp_0_1;
        //}

        double txtod(uint64_t x) const{
            if(this->bp >32){
                uint32_t tmp_x = x >> 32;
                double tmp_0_1 = static_cast<double>(tmp_x) / pow(2, this->bp-32);
                return tmp_0_1;
            }else{
                double tmp_0_1 = static_cast<double>(x) / pow(2, this->bp);
                return tmp_0_1;
            }
        }

        double t32tod(lvl0param::T x) const{
            double tmp_0_1 = static_cast<double>(x) / pow(2, std::numeric_limits<lvl0param::T>::digits);
            return tmp_0_1;
        }

        //double decode(const lvl0param::T x){
        //    double tmp_0_1 = this->txtod(x);
        //    printf("tmp_0_1: %f\n", tmp_0_1);
        //    tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
        //    return tmp_0_1 * this->d + this->a;
        //}
        
        double decode(const uint64_t x){
            double tmp_0_1 = this->txtod(x);
            printf("tmp_0_1: %f\n", tmp_0_1);
            tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
            return tmp_0_1 * this->d + this->a;
        }

};



template <class P>
array<typename P::T, P::n + 1> tlweSymEncodeEncrypt(
    const double &x, const double alpha,
    const array<typename P::T, P::n> &key, const Encoder &encoder);

template <class P>
void showPhase(const TLWE<P> &c, const Key<P> &key, Encoder &encoder);

template <class P>
double tlweSymDecryptDecode(const TLWE<P> &c, const Key<P> &key, Encoder &encoder);

template <class P>
double tlweSymDecryptDecode64(const array<uint64_t, lvl0param::n+1> &c, const Key<P> &key, Encoder &encoder);

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