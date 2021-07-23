#pragma once

#include <array>
#include <cstdint>
#include <vector>
#include <stdio.h>
#include <cassert>

#include "key.hpp"
#include "params.hpp"

namespace TFHEpp {
using namespace std;


class Encoder
{
    public:
        double a;  // input lower bound
        double b;  // input upper bound
        double effective_b;
        double d;
        double d2;
        double half;
        double half_d;
        int bp;    // bit precision including noise bit (lvl0param::T - bp is padding bit)
        // bp = (noise bit + plaintext precision bit)
        bool is_type_second;


        void print(){
            printf("=========================================\n");
            printf("\nEncoder Print\n");
            printf("a     : %f\n", this->a);
            printf("b     : %f\n", this->b);
            printf("effective_b     : %f\n", this->effective_b);
            printf("d     : %f\n", this->d);
            printf("half  : %f\n", this->half);
            printf("half_d: %f\n", this->half_d);
            printf("bp    : %d\n", this->bp);
            printf("type    : %d\n", this->is_type_second);

        }

        static Encoder copy(Encoder &encoder){
            
            if(encoder.is_type_second){
                Encoder tmp(encoder.a, abs(encoder.a), encoder.bp, encoder.is_type_second);
                return tmp;
            }else{
                Encoder tmp(encoder.a, encoder.b, encoder.bp, encoder.is_type_second);
                return tmp;
            }
        }

        Encoder(){
        };

        Encoder(double a, double b, int bp, bool is_type_second=true){

            if(is_type_second){
                this->a = a;
                this->effective_b = b;
                double tmp = b-a;
                this->b = b + tmp;
                this->d = this->b-this->a;
                this->half_d = (this->b-this->a)/2.;
                this->half = (this->b+this->a)/2.;
                this->bp = bp;
                this->is_type_second = true;
            }else{
                this->a = a;
                this->b = b;
                this->d = b-a;
                this->d2 = b-a;
                this->half_d = (b-a)/2.;
                this->half = (b+a)/2.;
                this->bp = bp;
                this->is_type_second = false;
            }


        }

        void update(double a, double b, int bp){
            //this->a = a;
            //this->b = b;
            //this->d = b-a;
            //this->d2 = b-a;
            //this->half_d = (b-a)/2.;
            //this->half = (b+a)/2.;
            //this->bp = bp;

            if(this->is_type_second){
                this->a = a;
                this->b = b;
                this->effective_b = abs(this->a);
                this->d = this->b-this->a;
                this->half_d = (this->b-this->a)/2.;
                this->half = (this->b+this->a)/2.;

                this->bp = bp;
            }
        }

        void update(double x){
            if(this->is_type_second){
                this->a = this->a*x;
                this->b = this->b*x;
                this->effective_b = abs(this->a);
                this->d = this->b-this->a;
                this->half_d = (this->b-this->a)/2.;
                this->half = (this->b+this->a)/2.;
            }
        }

        double encode_0_1(double x) const{
            return (x-this->a)/this->d;

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

        lvl0param::T encode(double x) const{
            //printf("here: %f\n", x);
            assert(x >= this->a);
            assert(x <= this->b);
            return dtotx((x-this->a)/this->d);
        }

        double txtod(lvl0param::T x) const{
            double tmp_0_1 = static_cast<double>(x) / pow(2, this->bp);
            return tmp_0_1;
        }


        double t32tod(lvl0param::T x) const{
            double tmp_0_1 = static_cast<double>(x) / pow(2, std::numeric_limits<lvl0param::T>::digits);
            return tmp_0_1;
        }

        double decode(const lvl0param::T x){
            double tmp_0_1 = this->txtod(x);
            //printf("tmp_0_1: %f\n", tmp_0_1);
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
double tlweSymDecryptDecode64(const array<uint64_t, lvl0param::n+1> &c, const Key<P> &key, Encoder &encoder);

template <class P>
array<typename P::T, P::n + 1> tlweSymEncrypt(
    const typename P::T p, const double alpha,
    const array<typename P::T, P::n> &key);

template <class P>
bool tlweSymDecrypt(const TLWE<P> &c, const Key<P> &key);

template <class P = lvl0param>
vector<TLWE<P>> bootsSymEncrypt(const vector<uint8_t> &p, const SecretKey &sk);
template <class P = lvl0param>
vector<uint8_t> bootsSymDecrypt(const vector<TLWE<P>> &c, const SecretKey &sk);
}  // namespace TFHEpp