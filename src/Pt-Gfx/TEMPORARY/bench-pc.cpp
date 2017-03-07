// clear && g++ -O2 -fno-loop-optimize -fno-rerun-loop-opt bench-pc.cpp -o bench-pc && ./bench-pc && rm -f bench-pc
//
// Results are below the program (TOPS = tera operations per second)
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
static void benchSALR(const char* name)
{
    volatile TYPE v = 0;

    // Do not use constants or repeating values to avoid loop unroll optimizations
    // All values > 0 to avoid division by 0
    volatile TYPE v0 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v1 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v2 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v3 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v4 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v5 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v6 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v7 = (TYPE) (rand() % 128) / 16 + 1;

    volatile size_t i;
    volatile double t1, td;

    // Loop and assignment overhead
    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v0;
        v = v2;
        v = v4;
        v = v6;
    }
    td = (getUTime() - t1);

    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v1;
        v = v3;
        v = v5;
        v = v7;
    }
    td += (getUTime() - t1);

    td /= (LOOP_COUNT * 8.0);
    printf("%s LOOP = %12.10f nS (%8.3f TOPS)\n", name, td * 1000, 1.0 / td / 1000000.0);

    // Shift-arithmetic-left by a constant
    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v0 << 4;
        v = v2 << 4;
        v = v4 << 4;
        v = v6 << 4;
    }
    td = (getUTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s sal  = %12.10f nS (%8.3f TOPS)\n", name, td * 1000, 1.0 / td / 1000000.0);

    // Shift-arithmetic-right by a constant
    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v1 >> 4;
        v = v3 >> 4;
        v = v5 >> 4;
        v = v7 >> 4;
    }
    td = (getUTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s sar  = %12.10f nS (%8.3f TOPS)\n", name, td * 1000, 1.0 / td / 1000000.0);

    // Shift-arithmetic-left by a variable
    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v <<= v0;
        v <<= v2;
        v <<= v4;
        v <<= v6;
    }
    td = (getUTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s salx = %12.10f nS (%8.3f TOPS)\n", name, td * 1000, 1.0 / td / 1000000.0);

    // Shift-arithmetic-right by a variable
    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v >>= v1;
        v >>= v3;
        v >>= v5;
        v >>= v7;
    }
    td = (getUTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s sarx = %12.10f nS (%8.3f TOPS)\n", name, td * 1000, 1.0 / td / 1000000.0);
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

    volatile size_t i;
    volatile double t1, td;

    // Loop and assignment overhead
    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v0;
        v = v2;
        v = v4;
        v = v6;
    }
    td = (getUTime() - t1);

    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v1;
        v = v3;
        v = v5;
        v = v7;
    }
    td += (getUTime() - t1);

    td /= (LOOP_COUNT * 8.0);
    printf("%s LOOP = %12.10f nS (%8.3f TOPS)\n", name, td * 1000, 1.0 / td / 1000000.0);

    // Addition
    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v += v0;
        v += v2;
        v += v4;
        v += v6;
    }
    td = (getUTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s add  = %12.10f nS (%8.3f TOPS)\n", name, td * 1000, 1.0 / td / 1000000.0);

    // Subtraction
    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v -= v1;
        v -= v3;
        v -= v5;
        v -= v7;
    }
    td = (getUTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s sub  = %12.10f nS (%8.3f TOPS)\n", name, td * 1000, 1.0 / td / 1000000.0);

    // Multiplication
    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v *= v0;
        v *= v2;
        v *= v4;
        v *= v6;
    }
    td = (getUTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s mul  = %12.10f nS (%8.3f TOPS)\n", name, td * 1000, 1.0 / td / 1000000.0);

    // Division
    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v /= v1;
        v /= v3;
        v /= v5;
        v /= v7;
    }
    td = (getUTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s div  = %12.10f nS (%8.3f TOPS)\n", name, td * 1000, 1.0 / td / 1000000.0);
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

    volatile size_t i;
    volatile double t1, td;

    // Loop and assignment overhead
    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v0;
        v = v2;
        v = v4;
        v = v6;
    }
    td = (getUTime() - t1);

    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v1;
        v = v3;
        v = v5;
        v = v7;
    }
    td += (getUTime() - t1);

    td /= (LOOP_COUNT * 8.0);
    printf("%s LOOP = %12.10f nS (%8.3f TOPS)\n", name, td * 1000, 1.0 / td / 1000000.0);

    // Square-root
    t1 = getUTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = xsqrt(v0);
        v = xsqrt(v2);
        v = xsqrt(v4);
        v = xsqrt(v6);
    }
    td = (getUTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s sqrt = %12.10f nS (%8.3f TOPS)\n", name, td * 1000, 1.0 / td / 1000000.0);
}

int main()
{
    printf("sizeof(      float) = %2zd\n", sizeof(      float));
    printf("sizeof(     double) = %2zd\n", sizeof(     double));
    printf("sizeof(long double) = %2zd\n", sizeof(long double));
    printf("\n");

    benchSALR<     int8_t>("     int8_t");
    benchASMD<     int8_t>("     int8_t");
    benchSALR<    uint8_t>("    uint8_t");
    benchASMD<    uint8_t>("    uint8_t");
    printf("\n");

    benchSALR<    int16_t>("    int16_t");
    benchASMD<    int16_t>("    int16_t");
    benchSALR<   uint16_t>("   uint16_t");
    benchASMD<   uint16_t>("   uint16_t");
    printf("\n");

    benchSALR<    int32_t>("    int32_t");
    benchASMD<    int32_t>("    int32_t");
    benchSALR<   uint32_t>("   uint32_t");
    benchASMD<   uint32_t>("   uint32_t");
    printf("\n");

    benchSALR<    int64_t>("    int64_t");
    benchASMD<    int64_t>("    int64_t");
    benchSALR<   uint64_t>("   uint64_t");
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
    ---------------------
    Core i5 - 64-bit Mode
    ---------------------
    sizeof(      float) =  4
    sizeof(     double) =  8
    sizeof(long double) = 16

         int8_t LOOP = 0.0000004571 nS (2187.490 TOPS)
         int8_t sal  = 0.0000004449 nS (2247.772 TOPS)
         int8_t sar  = 0.0000004439 nS (2252.595 TOPS)
         int8_t salx = 0.0000020752 nS ( 481.883 TOPS)
         int8_t sarx = 0.0000020772 nS ( 481.419 TOPS)
         int8_t LOOP = 0.0000004431 nS (2256.661 TOPS)
         int8_t add  = 0.0000021073 nS ( 474.530 TOPS)
         int8_t sub  = 0.0000019960 nS ( 500.996 TOPS)
         int8_t mul  = 0.0000023735 nS ( 421.310 TOPS)
         int8_t div  = 0.0000080387 nS ( 124.398 TOPS)
        uint8_t LOOP = 0.0000004432 nS (2256.566 TOPS)
        uint8_t sal  = 0.0000004433 nS (2255.605 TOPS)
        uint8_t sar  = 0.0000004432 nS (2256.139 TOPS)
        uint8_t salx = 0.0000020756 nS ( 481.788 TOPS)
        uint8_t sarx = 0.0000020772 nS ( 481.417 TOPS)
        uint8_t LOOP = 0.0000004429 nS (2257.592 TOPS)
        uint8_t add  = 0.0000021193 nS ( 471.845 TOPS)
        uint8_t sub  = 0.0000021199 nS ( 471.717 TOPS)
        uint8_t mul  = 0.0000023667 nS ( 422.528 TOPS)
        uint8_t div  = 0.0000074030 nS ( 135.080 TOPS)

        int16_t LOOP = 0.0000004431 nS (2256.936 TOPS)
        int16_t sal  = 0.0000004435 nS (2254.893 TOPS)
        int16_t sar  = 0.0000004433 nS (2255.948 TOPS)
        int16_t salx = 0.0000020898 nS ( 478.504 TOPS)
        int16_t sarx = 0.0000020785 nS ( 481.109 TOPS)
        int16_t LOOP = 0.0000004431 nS (2256.859 TOPS)
        int16_t add  = 0.0000021230 nS ( 471.035 TOPS)
        int16_t sub  = 0.0000021262 nS ( 470.317 TOPS)
        int16_t mul  = 0.0000023618 nS ( 423.400 TOPS)
        int16_t div  = 0.0000080365 nS ( 124.433 TOPS)
       uint16_t LOOP = 0.0000004431 nS (2256.783 TOPS)
       uint16_t sal  = 0.0000004434 nS (2255.236 TOPS)
       uint16_t sar  = 0.0000004434 nS (2255.236 TOPS)
       uint16_t salx = 0.0000020905 nS ( 478.352 TOPS)
       uint16_t sarx = 0.0000020771 nS ( 481.452 TOPS)
       uint16_t LOOP = 0.0000004429 nS (2257.598 TOPS)
       uint16_t add  = 0.0000020596 nS ( 485.535 TOPS)
       uint16_t sub  = 0.0000020586 nS ( 485.759 TOPS)
       uint16_t mul  = 0.0000023624 nS ( 423.289 TOPS)
       uint16_t div  = 0.0000077488 nS ( 129.052 TOPS)

        int32_t LOOP = 0.0000004431 nS (2256.928 TOPS)
        int32_t sal  = 0.0000004434 nS (2255.402 TOPS)
        int32_t sar  = 0.0000004438 nS (2253.521 TOPS)
        int32_t salx = 0.0000020761 nS ( 481.680 TOPS)
        int32_t sarx = 0.0000020759 nS ( 481.728 TOPS)
        int32_t LOOP = 0.0000004434 nS (2255.536 TOPS)
        int32_t add  = 0.0000021274 nS ( 470.060 TOPS)
        int32_t sub  = 0.0000021274 nS ( 470.050 TOPS)
        int32_t mul  = 0.0000023627 nS ( 423.247 TOPS)
        int32_t div  = 0.0000080386 nS ( 124.399 TOPS)
       uint32_t LOOP = 0.0000004434 nS (2255.319 TOPS)
       uint32_t sal  = 0.0000004439 nS (2252.607 TOPS)
       uint32_t sar  = 0.0000004437 nS (2253.700 TOPS)
       uint32_t salx = 0.0000020810 nS ( 480.540 TOPS)
       uint32_t sarx = 0.0000020814 nS ( 480.446 TOPS)
       uint32_t LOOP = 0.0000004433 nS (2255.663 TOPS)
       uint32_t add  = 0.0000021115 nS ( 473.605 TOPS)
       uint32_t sub  = 0.0000020587 nS ( 485.736 TOPS)
       uint32_t mul  = 0.0000023624 nS ( 423.301 TOPS)
       uint32_t div  = 0.0000077588 nS ( 128.886 TOPS)

        int64_t LOOP = 0.0000004432 nS (2256.350 TOPS)
        int64_t sal  = 0.0000004438 nS (2253.294 TOPS)
        int64_t sar  = 0.0000004438 nS (2253.521 TOPS)
        int64_t salx = 0.0000020756 nS ( 481.788 TOPS)
        int64_t sarx = 0.0000020808 nS ( 480.595 TOPS)
        int64_t LOOP = 0.0000004432 nS (2256.076 TOPS)
        int64_t add  = 0.0000020527 nS ( 487.172 TOPS)
        int64_t sub  = 0.0000021225 nS ( 471.146 TOPS)
        int64_t mul  = 0.0000023620 nS ( 423.365 TOPS)
        int64_t div  = 0.0000131813 nS (  75.865 TOPS)
       uint64_t LOOP = 0.0000004433 nS (2255.765 TOPS)
       uint64_t sal  = 0.0000004438 nS (2253.342 TOPS)
       uint64_t sar  = 0.0000004437 nS (2253.803 TOPS)
       uint64_t salx = 0.0000020777 nS ( 481.305 TOPS)
       uint64_t sarx = 0.0000020820 nS ( 480.301 TOPS)
       uint64_t LOOP = 0.0000004432 nS (2256.458 TOPS)
       uint64_t add  = 0.0000021220 nS ( 471.259 TOPS)
       uint64_t sub  = 0.0000020593 nS ( 485.608 TOPS)
       uint64_t mul  = 0.0000023625 nS ( 423.288 TOPS)
       uint64_t div  = 0.0000106814 nS (  93.621 TOPS)

          float LOOP = 0.0000004432 nS (2256.145 TOPS)
          float add  = 0.0000026572 nS ( 376.340 TOPS)
          float sub  = 0.0000026576 nS ( 376.284 TOPS)
          float mul  = 0.0000032477 nS ( 307.907 TOPS)
          float div  = 0.0000049048 nS ( 203.884 TOPS)
          float LOOP = 0.0000004434 nS (2255.502 TOPS)
          float sqrt = 0.0000004096 nS (2441.182 TOPS)

         double LOOP = 0.0000004432 nS (2256.114 TOPS)
         double add  = 0.0000026570 nS ( 376.360 TOPS)
         double sub  = 0.0000026582 nS ( 376.188 TOPS)
         double mul  = 0.0000032476 nS ( 307.924 TOPS)
         double div  = 0.0000048782 nS ( 204.993 TOPS)
         double LOOP = 0.0000004433 nS (2256.000 TOPS)
         double sqrt = 0.0000004697 nS (2128.869 TOPS)

    long double LOOP = 0.0000018454 nS ( 541.894 TOPS)
    long double add  = 0.0000035446 nS ( 282.119 TOPS)
    long double sub  = 0.0000035458 nS ( 282.026 TOPS)
    long double mul  = 0.0000974729 nS (  10.259 TOPS)
    long double div  = 0.0001016053 nS (   9.842 TOPS)
    long double LOOP = 0.0000017717 nS ( 564.414 TOPS)
    long double sqrt = 0.0000022148 nS ( 451.518 TOPS)

    ---------------------------
    RaspberryPi 3 - 32-bit Mode
    ---------------------------
    sizeof(      float) =  4
    sizeof(     double) =  8
    sizeof(long double) =  8

         int8_t LOOP = 0.0000039911 nS ( 250.555 TOPS)
         int8_t sal  = 0.0000056778 nS ( 176.125 TOPS)
         int8_t sar  = 0.0000065080 nS ( 153.658 TOPS)
         int8_t salx = 0.0000073538 nS ( 135.985 TOPS)
         int8_t sarx = 0.0000073336 nS ( 136.358 TOPS)
         int8_t LOOP = 0.0000022995 nS ( 434.879 TOPS)
         int8_t add  = 0.0000054298 nS ( 184.169 TOPS)
         int8_t sub  = 0.0000054358 nS ( 183.965 TOPS)
         int8_t mul  = 0.0000071073 nS ( 140.701 TOPS)
         int8_t div  = 0.0000179638 nS (  55.667 TOPS)
        uint8_t LOOP = 0.0000039675 nS ( 252.049 TOPS)
        uint8_t sal  = 0.0000056387 nS ( 177.345 TOPS)
        uint8_t sar  = 0.0000048024 nS ( 208.231 TOPS)
        uint8_t salx = 0.0000064694 nS ( 154.573 TOPS)
        uint8_t sarx = 0.0000064666 nS ( 154.642 TOPS)
        uint8_t LOOP = 0.0000022955 nS ( 435.634 TOPS)
        uint8_t add  = 0.0000054243 nS ( 184.355 TOPS)
        uint8_t sub  = 0.0000054268 nS ( 184.269 TOPS)
        uint8_t mul  = 0.0000070931 nS ( 140.983 TOPS)
        uint8_t div  = 0.0000165128 nS (  60.559 TOPS)

        int16_t LOOP = 0.0000039652 nS ( 252.196 TOPS)
        int16_t sal  = 0.0000064701 nS ( 154.557 TOPS)
        int16_t sar  = 0.0000064658 nS ( 154.661 TOPS)
        int16_t salx = 0.0000073030 nS ( 136.931 TOPS)
        int16_t sarx = 0.0000073003 nS ( 136.980 TOPS)
        int16_t LOOP = 0.0000022944 nS ( 435.853 TOPS)
        int16_t add  = 0.0000054213 nS ( 184.459 TOPS)
        int16_t sub  = 0.0000054211 nS ( 184.464 TOPS)
        int16_t mul  = 0.0000070917 nS ( 141.010 TOPS)
        int16_t div  = 0.0000179331 nS (  55.763 TOPS)
       uint16_t LOOP = 0.0000039632 nS ( 252.318 TOPS)
       uint16_t sal  = 0.0000056308 nS ( 177.596 TOPS)
       uint16_t sar  = 0.0000047961 nS ( 208.503 TOPS)
       uint16_t salx = 0.0000064663 nS ( 154.648 TOPS)
       uint16_t sarx = 0.0000064636 nS ( 154.713 TOPS)
       uint16_t LOOP = 0.0000022935 nS ( 436.006 TOPS)
       uint16_t add  = 0.0000054211 nS ( 184.465 TOPS)
       uint16_t sub  = 0.0000054210 nS ( 184.468 TOPS)
       uint16_t mul  = 0.0000079241 nS ( 126.198 TOPS)
       uint16_t div  = 0.0000152206 nS (  65.700 TOPS)

        int32_t LOOP = 0.0000031275 nS ( 319.749 TOPS)
        int32_t sal  = 0.0000047960 nS ( 208.508 TOPS)
        int32_t sar  = 0.0000047955 nS ( 208.530 TOPS)
        int32_t salx = 0.0000056305 nS ( 177.603 TOPS)
        int32_t sarx = 0.0000056296 nS ( 177.631 TOPS)
        int32_t LOOP = 0.0000022935 nS ( 436.017 TOPS)
        int32_t add  = 0.0000045880 nS ( 217.959 TOPS)
        int32_t sub  = 0.0000045870 nS ( 218.006 TOPS)
        int32_t mul  = 0.0000062551 nS ( 159.871 TOPS)
        int32_t div  = 0.0000176134 nS (  56.775 TOPS)
       uint32_t LOOP = 0.0000031275 nS ( 319.747 TOPS)
       uint32_t sal  = 0.0000047956 nS ( 208.525 TOPS)
       uint32_t sar  = 0.0000047955 nS ( 208.529 TOPS)
       uint32_t salx = 0.0000056296 nS ( 177.633 TOPS)
       uint32_t sarx = 0.0000056295 nS ( 177.637 TOPS)
       uint32_t LOOP = 0.0000022940 nS ( 435.911 TOPS)
       uint32_t add  = 0.0000045872 nS ( 218.000 TOPS)
       uint32_t sub  = 0.0000045870 nS ( 218.008 TOPS)
       uint32_t mul  = 0.0000062551 nS ( 159.871 TOPS)
       uint32_t div  = 0.0000166395 nS (  60.098 TOPS)

        int64_t LOOP = 0.0000031283 nS ( 319.664 TOPS)
        int64_t sal  = 0.0000056296 nS ( 177.633 TOPS)
        int64_t sar  = 0.0000056295 nS ( 177.636 TOPS)
        int64_t salx = 0.0000089654 nS ( 111.539 TOPS)
        int64_t sarx = 0.0000102170 nS (  97.876 TOPS)
        int64_t LOOP = 0.0000022935 nS ( 436.017 TOPS)
        int64_t add  = 0.0000054221 nS ( 184.431 TOPS)
        int64_t sub  = 0.0000054211 nS ( 184.465 TOPS)
        int64_t mul  = 0.0000095910 nS ( 104.265 TOPS)
        int64_t div  = 0.0000548386 nS (  18.235 TOPS)
       uint64_t LOOP = 0.0000031281 nS ( 319.680 TOPS)
       uint64_t sal  = 0.0000056295 nS ( 177.637 TOPS)
       uint64_t sar  = 0.0000056297 nS ( 177.630 TOPS)
       uint64_t salx = 0.0000089656 nS ( 111.538 TOPS)
       uint64_t sarx = 0.0000089654 nS ( 111.540 TOPS)
       uint64_t LOOP = 0.0000022940 nS ( 435.912 TOPS)
       uint64_t add  = 0.0000054211 nS ( 184.464 TOPS)
       uint64_t sub  = 0.0000054209 nS ( 184.470 TOPS)
       uint64_t mul  = 0.0000095912 nS ( 104.263 TOPS)
       uint64_t div  = 0.0000448307 nS (  22.306 TOPS)

          float LOOP = 0.0000022935 nS ( 436.011 TOPS)
          float add  = 0.0000079240 nS ( 126.199 TOPS)
          float sub  = 0.0000079230 nS ( 126.215 TOPS)
          float mul  = 0.0000079231 nS ( 126.214 TOPS)
          float div  = 0.0000154290 nS (  64.813 TOPS)
          float LOOP = 0.0000022941 nS ( 435.904 TOPS)
          float sqrt = 0.0000039616 nS ( 252.424 TOPS)

         double LOOP = 0.0000022934 nS ( 436.026 TOPS)
         double add  = 0.0000079232 nS ( 126.212 TOPS)
         double sub  = 0.0000079231 nS ( 126.214 TOPS)
         double mul  = 0.0000079230 nS ( 126.215 TOPS)
         double div  = 0.0000229369 nS (  43.598 TOPS)
         double LOOP = 0.0000022935 nS ( 436.009 TOPS)
         double sqrt = 0.0000039619 nS ( 252.404 TOPS)

    long double LOOP = 0.0000022935 nS ( 436.012 TOPS)
    long double add  = 0.0000079241 nS ( 126.197 TOPS)
    long double sub  = 0.0000079230 nS ( 126.215 TOPS)
    long double mul  = 0.0000079231 nS ( 126.213 TOPS)
    long double div  = 0.0000229361 nS (  43.599 TOPS)
    long double LOOP = 0.0000022935 nS ( 436.010 TOPS)
    long double sqrt = 0.0000039616 nS ( 252.423 TOPS)
 */
