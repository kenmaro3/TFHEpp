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
    double max;
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
        this->max = b;

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
        assert(x >= this->a);
        assert(x <= this->b);
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
        double tmp_0_2 = tmp_0_1 * this->d + this->a;
        return tmp_0_2;
    }

    double decode_with_mult_option(const lvl0param::T x, int flag)
    {
        //// NOTE: The decoding algorism is WIP(Working In Progress)
        // printf(
        //     "\n[warn] NOTE: The decoding algorism is WIP(Working In "
        //     "Progress)\n\n");
        double tmp_0_1 = this->txtod(x);
        tmp_0_1 = tmp_0_1 - floor(tmp_0_1);
        double tmp_0_2 = tmp_0_1 * this->d + this->a;
        // return tmp_0_2;

        int reverse = (int)(tmp_0_2 / half_d) % 2;
        double tmp_0_3 = fmod(tmp_0_2, half_d);

        if (flag) {
            printf("decode: %lf %lf %d %lf\n", tmp_0_2, tmp_0_3, reverse, max);
        }

        if (tmp_0_3 > max) {
            return -((max - a) - tmp_0_3);
        }
            

        return tmp_0_3;
    }
};
}  // namespace TFHEpp
