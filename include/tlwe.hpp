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
        double half;
        double half_d;
        bool is_include_negative;

        Encoder(double a, double b){
            if(a<0){
                assert(abs(a) == abs(b));
                this->is_include_negative = true;
            }else{
                this->is_include_negative = false;
            }
            this->a = a;
            this->b = b;
            this->d = b-a;
            this->half_d = (b-a)/2.;
            this->half = (b+a)/2.;
        }

        uint32_t encode(double x) const{
            if(this->is_include_negative){
                if(x-this->a < this->half_d){
                    x = this->b - abs(x);
                    return dtot30((x-this->a)/this->d);
                }else{
                    x = x - this->half_d;
                    return dtot30((x-this->a)/this->d);
                }
            }
            else{
                return dtot30((x-this->a)/this->d);
            } 
        }

        double decode(const uint32_t x){
            if(this->is_include_negative){
                double tmp_0_1 = static_cast<double>(x) / pow(2, 30);
                assert(tmp_0_1 >= 0.);
                tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
                if(tmp_0_1 > 0.5){
                    double tmp2 = tmp_0_1 - 0.5;
                    return this->a + tmp2 * this->d;
                }else{
                    return tmp_0_1 * this->d;

                }

            }else{
                uint32_t x1 = static_cast<uint32_t>(x);
                double tmp_0_1 = static_cast<double>(x1) / pow(2, 30);
                tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
                return tmp_0_1 * this->d;
            }
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