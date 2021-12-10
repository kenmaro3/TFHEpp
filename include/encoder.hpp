#pragma once

#include <stdio.h>

#include <cmath>

#include "key.hpp"
#include "params.hpp"

namespace TFHEpp {
class Encoder {
public:
    double a;  // input lower bound
    double b;  // input upper bound

    double d;
    double half;
    double half_d;
    int bp;
    // bit precision including noise bit (lvl0param::T - bp is padding bit)
    // bp = (noise bit + plaintext precision bit)

    double get_a(){return this->a;}
    double get_b(){return this->b;}
    double get_d(){return this->d;}
    double get_half_d(){return this->half_d;}
    double get_half(){return this->half;}
    int get_bp(){return this->bp;}


    void print()
    {
        printf("=========================================\n");
        printf("\nEncoder Print\n");
        printf("a     : %f\n", this->a);
        printf("b     : %f\n", this->b);
        printf("d     : %f\n", this->d);
        printf("half  : %f\n", this->half);
        printf("half_d: %f\n", this->half_d);
        printf("bp    : %d\n", this->bp);
    }

    static Encoder copy(Encoder &encoder)
    {
        Encoder tmp(encoder.a, abs(encoder.a), encoder.bp);
        return tmp;
    }

    Encoder(){};

    Encoder(double a, double b, int bp)
    {
        this->a = a;
        double tmp = b - a;
        this->b = b + tmp;
        this->d = this->b - this->a;
        this->half_d = (this->b - this->a) / 2.;
        this->half = (this->b + this->a) / 2.;
        this->bp = bp;
    }

    void update(double a, double b, int bp)
    {
        this->a = a;
        this->b = b;
        this->d = this->b - this->a;
        this->half_d = (this->b - this->a) / 2.;
        this->half = (this->b + this->a) / 2.;

        this->bp = bp;
    }

    void update(double x)
    {
        this->a = this->a * x;
        this->b = this->b * x;
        this->d = this->b - this->a;
        this->half_d = (this->b - this->a) / 2.;
        this->half = (this->b + this->a) / 2.;
    }

    double encode_0_1(double x) const { return (x - this->a) / this->d; }

    static lvl0param::T dtotx(double d, int bpx)
    {
        double tmp = d - floor(d);
        tmp = tmp * pow(2., bpx);

        return static_cast<lvl0param::T>(round(tmp));
    }

    static lvl0param::T dtotx(double d, double max, int bpx)
    {
        d = d / max;
        double tmp = d - floor(d);
        tmp = tmp * pow(2., bpx);

        return static_cast<lvl0param::T>(round(tmp));
    }

    lvl0param::T dtotx(double d) const
    {
        double tmp = d - floor(d);
        tmp = tmp * pow(2., this->bp);

        return static_cast<lvl0param::T>(round(tmp));
    }

    lvl0param::T encode(double x) const
    {
        //assert(x >= this->a);
        //assert(x <= this->b);
        if (x < this->a){
            //printf("Warning:  x=%f, encoder (%f, %f)\n", x, this->a, this->b);
            x = 0;
        }else if (x > b){
            //printf("Warning:  x=%f, encoder (%f, %f)\n", x, this->a, this->b);
            x = 0;
        }
        return dtotx((x - this->a) / this->d);
    }

    double txtod(lvl0param::T x) const
    {
        double tmp_0_1 = static_cast<double>(x) / pow(2, this->bp);
        return tmp_0_1;
    }

    double decode(const lvl0param::T x)
    {
        double tmp_0_1 = this->txtod(x);
        tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
        return tmp_0_1 * this->d + this->a;
    }
};
}  // namespace TFHEpp
