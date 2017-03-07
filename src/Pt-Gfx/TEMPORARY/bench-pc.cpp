// clear && g++ -O2 bench-pc.cpp -o bench-pc && ./bench-pc && rm -f bench-pc
//
// Results are below the program
//

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <typeinfo>

#define LOOP_COUNT 100000000

static inline double getUTime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (double) tv.tv_sec + ((double) tv.tv_usec * 0.000001);
}

static inline       float xsqrt(      float v) { return sqrtf(v); }
static inline      double xsqrt(     double v) { return sqrt (v); }
static inline long double xsqrt(long double v) { return sqrtl(v); }

template<typename TYPE>
static void benchSHIF(const char* name)
{
    volatile TYPE v = 0;

    // Do not use constants or repeating values to avoid loop unroll optimizations
    // All values > 0 to avoid division by 0
    TYPE v0 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v1 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v2 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v3 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v4 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v5 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v6 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v7 = (TYPE) (rand() % 128) / 16 + 1;

    double t1, td;

    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v = v0 * 16; // These will be converted to (<< 4) by the compiler
        v = v2 * 16; // ---
        v = v4 * 16; // ---
        v = v6 * 16; // ---
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s shl  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);

    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v = v1 / 16; // These will be converted to (>> 4) by the compiler
        v = v3 / 16; // ---
        v = v5 / 16; // ---
        v = v7 / 16; // ---
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s shr  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);


    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v <<= v0;
        v <<= v2;
        v <<= v4;
        v <<= v6;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s shlx = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);

    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v >>= v1;
        v >>= v3;
        v >>= v5;
        v >>= v7;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s shrx = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);
}

template<typename TYPE>
static void benchASMD(const char* name)
{
    volatile TYPE v = 0;

    // Do not use constants or repeating values to avoid loop unroll optimizations
    // All values > 0 to avoid division by 0
    TYPE v0 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v1 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v2 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v3 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v4 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v5 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v6 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v7 = (TYPE) (rand() % 128) / 16 + 1;

    double t1, td;

    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v += v0;
        v += v2;
        v += v4;
        v += v6;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s add  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);

    t1 = getUTime();
    for(size_t i = 0; i < LOOP_COUNT; ++i) {
        v -= v1;
        v -= v3;
        v -= v5;
        v -= v7;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s sub  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);

    t1 = getUTime();
    for(size_t i = 0; i < LOOP_COUNT; ++i) {
        v *= v0;
        v *= v2;
        v *= v4;
        v *= v6;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s mul  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);

    t1 = getUTime();
    for(size_t i = 0; i < LOOP_COUNT; ++i) {
        v /= v1;
        v /= v3;
        v /= v5;
        v /= v7;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s div  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);
}

template<typename TYPE>
static void benchSQRT(const char* name)
{
    volatile TYPE v = 0;

    // Do not use constants or repeating values to avoid loop unroll optimizations
    // All values > 0 to avoid division by 0
    TYPE v0 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v1 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v2 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v3 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v4 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v5 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v6 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v7 = (TYPE) (rand() % 128) / 16 + 1;

    double t1, td;

    t1 = getUTime();
    for(size_t i = 0; i < LOOP_COUNT; ++i) {
        v = xsqrt(v0);
        v = xsqrt(v2);
        v = xsqrt(v4);
        v = xsqrt(v6);
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s sqrt = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);
}


int main()
{
    benchSHIF<     int8_t>("     int8_t");
    benchASMD<     int8_t>("     int8_t");
    benchSHIF<    uint8_t>("    uint8_t");
    benchASMD<    uint8_t>("    uint8_t");
    printf("\n");

    benchSHIF<    int16_t>("    int16_t");
    benchASMD<    int16_t>("    int16_t");
    benchSHIF<   uint16_t>("   uint16_t");
    benchASMD<   uint16_t>("   uint16_t");
    printf("\n");

    benchSHIF<    int32_t>("    int32_t");
    benchASMD<    int32_t>("    int32_t");
    benchSHIF<   uint32_t>("   uint32_t");
    benchASMD<   uint32_t>("   uint32_t");
    printf("\n");

    benchSHIF<    int64_t>("    int64_t");
    benchASMD<    int64_t>("    int64_t");
    benchSHIF<   uint64_t>("   uint64_t");
    benchASMD<   uint64_t>("   uint64_t");
    printf("\n");

    benchASMD<      float>("      float");
    benchSQRT<      float>("      float");
    printf("\n");

    benchASMD<     double>("     double");
    benchSQRT<     double>("     double");
    printf("\n");

    benchASMD<long double>("long double");
    benchSQRT<long double>("long double");
    printf("\n");

    return 0;
}

/*
    Core i5 - 64-bit Mode
    -------------------
         int8_t shl  =  0.030 uS (33.815 MIPS)
         int8_t shr  =  0.031 uS (32.620 MIPS)
         int8_t shlx =  0.180 uS ( 5.570 MIPS)
         int8_t shrx =  0.178 uS ( 5.633 MIPS)
         int8_t add  =  0.197 uS ( 5.082 MIPS)
         int8_t sub  =  0.203 uS ( 4.929 MIPS)
         int8_t mul  =  0.236 uS ( 4.234 MIPS)
         int8_t div  =  0.804 uS ( 1.244 MIPS)
        uint8_t shl  =  0.030 uS (33.794 MIPS)
        uint8_t shr  =  0.030 uS (33.820 MIPS)
        uint8_t shlx =  0.178 uS ( 5.632 MIPS)
        uint8_t shrx =  0.178 uS ( 5.632 MIPS)
        uint8_t add  =  0.205 uS ( 4.874 MIPS)
        uint8_t sub  =  0.198 uS ( 5.040 MIPS)
        uint8_t mul  =  0.236 uS ( 4.234 MIPS)
        uint8_t div  =  0.740 uS ( 1.352 MIPS)

        int16_t shl  =  0.030 uS (33.802 MIPS)
        int16_t shr  =  0.030 uS (33.806 MIPS)
        int16_t shlx =  0.178 uS ( 5.630 MIPS)
        int16_t shrx =  0.177 uS ( 5.639 MIPS)
        int16_t add  =  0.198 uS ( 5.050 MIPS)
        int16_t sub  =  0.204 uS ( 4.904 MIPS)
        int16_t mul  =  0.236 uS ( 4.235 MIPS)
        int16_t div  =  0.804 uS ( 1.244 MIPS)
       uint16_t shl  =  0.030 uS (33.795 MIPS)
       uint16_t shr  =  0.030 uS (33.823 MIPS)
       uint16_t shlx =  0.178 uS ( 5.632 MIPS)
       uint16_t shrx =  0.178 uS ( 5.632 MIPS)
       uint16_t add  =  0.198 uS ( 5.052 MIPS)
       uint16_t sub  =  0.199 uS ( 5.023 MIPS)
       uint16_t mul  =  0.236 uS ( 4.235 MIPS)
       uint16_t div  =  0.774 uS ( 1.293 MIPS)

        int32_t shl  =  0.030 uS (33.824 MIPS)
        int32_t shr  =  0.030 uS (33.796 MIPS)
        int32_t shlx =  0.178 uS ( 5.631 MIPS)
        int32_t shrx =  0.178 uS ( 5.631 MIPS)
        int32_t add  =  0.203 uS ( 4.929 MIPS)
        int32_t sub  =  0.204 uS ( 4.902 MIPS)
        int32_t mul  =  0.236 uS ( 4.234 MIPS)
        int32_t div  =  0.804 uS ( 1.244 MIPS)
       uint32_t shl  =  0.030 uS (33.703 MIPS)
       uint32_t shr  =  0.030 uS (33.773 MIPS)
       uint32_t shlx =  0.178 uS ( 5.632 MIPS)
       uint32_t shrx =  0.178 uS ( 5.630 MIPS)
       uint32_t add  =  0.192 uS ( 5.205 MIPS)
       uint32_t sub  =  0.193 uS ( 5.194 MIPS)
       uint32_t mul  =  0.236 uS ( 4.233 MIPS)
       uint32_t div  =  0.776 uS ( 1.289 MIPS)

        int64_t shl  =  0.030 uS (33.768 MIPS)
        int64_t shr  =  0.030 uS (33.770 MIPS)
        int64_t shlx =  0.177 uS ( 5.634 MIPS)
        int64_t shrx =  0.177 uS ( 5.638 MIPS)
        int64_t add  =  0.195 uS ( 5.119 MIPS)
        int64_t sub  =  0.205 uS ( 4.878 MIPS)
        int64_t mul  =  0.236 uS ( 4.234 MIPS)
        int64_t div  =  1.318 uS ( 0.759 MIPS)
       uint64_t shl  =  0.030 uS (33.754 MIPS)
       uint64_t shr  =  0.030 uS (33.755 MIPS)
       uint64_t shlx =  0.178 uS ( 5.630 MIPS)
       uint64_t shrx =  0.177 uS ( 5.637 MIPS)
       uint64_t add  =  0.204 uS ( 4.896 MIPS)
       uint64_t sub  =  0.196 uS ( 5.112 MIPS)
       uint64_t mul  =  0.236 uS ( 4.232 MIPS)
       uint64_t div  =  1.063 uS ( 0.940 MIPS)

          float add  =  0.266 uS ( 3.764 MIPS)
          float sub  =  0.266 uS ( 3.763 MIPS)
          float mul  =  0.325 uS ( 3.079 MIPS)
          float div  =  0.486 uS ( 2.058 MIPS)
          float sqrt =  0.033 uS (30.027 MIPS)

         double add  =  0.266 uS ( 3.764 MIPS)
         double sub  =  0.266 uS ( 3.763 MIPS)
         double mul  =  0.325 uS ( 3.079 MIPS)
         double div  =  0.483 uS ( 2.070 MIPS)
         double sqrt =  0.033 uS (29.982 MIPS)

    long double add  =  0.354 uS ( 2.822 MIPS)
    long double sub  =  0.354 uS ( 2.822 MIPS)
    long double mul  =  9.776 uS ( 0.102 MIPS)
    long double div  = 10.153 uS ( 0.098 MIPS)
    long double sqrt =  0.207 uS ( 4.838 MIPS)

    RaspberryPi 3 - 32-bit Mode
    -------------------------
     int8_t shl  =  0.084 uS (11.921 MIPS)
     int8_t shr  =  0.084 uS (11.931 MIPS)
     int8_t shlx =  0.587 uS ( 1.704 MIPS)
     int8_t shrx =  0.587 uS ( 1.702 MIPS)
     int8_t add  =  0.419 uS ( 2.386 MIPS)
     int8_t sub  =  0.419 uS ( 2.387 MIPS)
     int8_t mul  =  0.586 uS ( 1.705 MIPS)
     int8_t div  =  1.655 uS ( 0.604 MIPS)
     int8_t shl  =  0.084 uS (11.929 MIPS)
     int8_t shr  =  0.084 uS (11.935 MIPS)
     int8_t shlx =  0.503 uS ( 1.990 MIPS)
     int8_t shrx =  0.503 uS ( 1.990 MIPS)
    uint8_t add  =  0.419 uS ( 2.388 MIPS)
    uint8_t sub  =  0.419 uS ( 2.388 MIPS)
    uint8_t mul  =  0.586 uS ( 1.705 MIPS)
    uint8_t div  =  1.658 uS ( 0.603 MIPS)

    int16_t shl  =  0.084 uS (11.936 MIPS)
    int16_t shr  =  0.084 uS (11.939 MIPS)
    int16_t shlx =  0.586 uS ( 1.706 MIPS)
    int16_t shrx =  0.587 uS ( 1.705 MIPS)
    int16_t add  =  0.419 uS ( 2.386 MIPS)
    int16_t sub  =  0.419 uS ( 2.388 MIPS)
    int16_t mul  =  0.586 uS ( 1.706 MIPS)
    int16_t div  =  1.759 uS ( 0.569 MIPS)
   uint16_t shl  =  0.084 uS (11.939 MIPS)
   uint16_t shr  =  0.084 uS (11.940 MIPS)
   uint16_t shlx =  0.503 uS ( 1.990 MIPS)
   uint16_t shrx =  0.503 uS ( 1.990 MIPS)
   uint16_t add  =  0.422 uS ( 2.371 MIPS)
   uint16_t sub  =  0.419 uS ( 2.388 MIPS)
   uint16_t mul  =  0.670 uS ( 1.492 MIPS)
   uint16_t div  =  1.487 uS ( 0.673 MIPS)

    int32_t shl  =  0.084 uS (11.935 MIPS)
    int32_t shr  =  0.084 uS (11.939 MIPS)
    int32_t shlx =  0.419 uS ( 2.387 MIPS)
    int32_t shrx =  0.419 uS ( 2.388 MIPS)
    int32_t add  =  0.335 uS ( 2.985 MIPS)
    int32_t sub  =  0.335 uS ( 2.985 MIPS)
    int32_t mul  =  0.503 uS ( 1.990 MIPS)
    int32_t div  =  1.722 uS ( 0.581 MIPS)
   uint32_t shl  =  0.084 uS (11.938 MIPS)
   uint32_t shr  =  0.084 uS (11.939 MIPS)
   uint32_t shlx =  0.419 uS ( 2.388 MIPS)
   uint32_t shrx =  0.419 uS ( 2.388 MIPS)
   uint32_t add  =  0.335 uS ( 2.985 MIPS)
   uint32_t sub  =  0.335 uS ( 2.985 MIPS)
   uint32_t mul  =  0.503 uS ( 1.990 MIPS)
   uint32_t div  =  1.647 uS ( 0.607 MIPS)

    int64_t shl  =  0.084 uS (11.939 MIPS)
    int64_t shr  =  0.084 uS (11.937 MIPS)
    int64_t shlx =  0.963 uS ( 1.038 MIPS)
    int64_t shrx =  0.900 uS ( 1.111 MIPS)
    int64_t add  =  0.440 uS ( 2.274 MIPS)
    int64_t sub  =  0.440 uS ( 2.274 MIPS)
    int64_t mul  =  0.838 uS ( 1.194 MIPS)
    int64_t div  =  5.382 uS ( 0.186 MIPS)
   uint64_t shl  =  0.084 uS (11.939 MIPS)
   uint64_t shr  =  0.084 uS (11.935 MIPS)
   uint64_t shlx =  0.963 uS ( 1.038 MIPS)
   uint64_t shrx =  0.963 uS ( 1.038 MIPS)
   uint64_t add  =  0.440 uS ( 2.274 MIPS)
   uint64_t sub  =  0.440 uS ( 2.274 MIPS)
   uint64_t mul  =  0.837 uS ( 1.194 MIPS)
   uint64_t div  =  4.377 uS ( 0.228 MIPS)

      float add  =  0.670 uS ( 1.493 MIPS)
      float sub  =  0.670 uS ( 1.493 MIPS)
      float mul  =  0.670 uS ( 1.492 MIPS)
      float div  =  1.424 uS ( 0.702 MIPS)
      float sqrt =  0.272 uS ( 3.673 MIPS)

     double add  =  0.670 uS ( 1.493 MIPS)
     double sub  =  0.670 uS ( 1.493 MIPS)
     double mul  =  0.670 uS ( 1.492 MIPS)
     double div  =  2.177 uS ( 0.459 MIPS)
     double sqrt =  0.314 uS ( 3.184 MIPS)

long double add  =  0.670 uS ( 1.492 MIPS)
long double sub  =  0.670 uS ( 1.493 MIPS)
long double mul  =  0.670 uS ( 1.493 MIPS)
long double div  =  2.178 uS ( 0.459 MIPS)
long double sqrt =  0.272 uS ( 3.672 MIPS)
 */
