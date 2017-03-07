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

    double t1, td;

    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v = v0 << 4;
        v = v2 << 4;
        v = v4 << 4;
        v = v6 << 4;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s sal  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);

    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v = v1 >> 4;
        v = v3 >> 4;
        v = v5 >> 4;
        v = v7 >> 4;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s sar  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);


    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v <<= v0;
        v <<= v2;
        v <<= v4;
        v <<= v6;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s salx = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);

    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v >>= v1;
        v >>= v3;
        v >>= v5;
        v >>= v7;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s sarx = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);
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
         int8_t sal  =  0.040 uS (25.054 MIPS)
         int8_t sar  =  0.031 uS (32.694 MIPS)
         int8_t salx =  0.209 uS ( 4.790 MIPS)
         int8_t sarx =  0.207 uS ( 4.839 MIPS)
         int8_t add  =  0.193 uS ( 5.183 MIPS)
         int8_t sub  =  0.202 uS ( 4.955 MIPS)
         int8_t mul  =  0.237 uS ( 4.223 MIPS)
         int8_t div  =  0.804 uS ( 1.244 MIPS)
        uint8_t sal  =  0.043 uS (23.424 MIPS)
        uint8_t sar  =  0.040 uS (25.020 MIPS)
        uint8_t salx =  0.207 uS ( 4.831 MIPS)
        uint8_t sarx =  0.207 uS ( 4.838 MIPS)
        uint8_t add  =  0.198 uS ( 5.051 MIPS)
        uint8_t sub  =  0.203 uS ( 4.919 MIPS)
        uint8_t mul  =  0.236 uS ( 4.234 MIPS)
        uint8_t div  =  0.739 uS ( 1.352 MIPS)

        int16_t sal  =  0.030 uS (33.796 MIPS)
        int16_t sar  =  0.030 uS (33.820 MIPS)
        int16_t salx =  0.207 uS ( 4.838 MIPS)
        int16_t sarx =  0.207 uS ( 4.840 MIPS)
        int16_t add  =  0.198 uS ( 5.051 MIPS)
        int16_t sub  =  0.198 uS ( 5.042 MIPS)
        int16_t mul  =  0.236 uS ( 4.234 MIPS)
        int16_t div  =  0.804 uS ( 1.244 MIPS)
       uint16_t sal  =  0.040 uS (25.053 MIPS)
       uint16_t sar  =  0.030 uS (33.821 MIPS)
       uint16_t salx =  0.207 uS ( 4.836 MIPS)
       uint16_t sarx =  0.207 uS ( 4.837 MIPS)
       uint16_t add  =  0.198 uS ( 5.045 MIPS)
       uint16_t sub  =  0.198 uS ( 5.053 MIPS)
       uint16_t mul  =  0.236 uS ( 4.234 MIPS)
       uint16_t div  =  0.774 uS ( 1.293 MIPS)

        int32_t sal  =  0.030 uS (33.816 MIPS)
        int32_t sar  =  0.030 uS (33.790 MIPS)
        int32_t salx =  0.207 uS ( 4.838 MIPS)
        int32_t sarx =  0.207 uS ( 4.840 MIPS)
        int32_t add  =  0.198 uS ( 5.041 MIPS)
        int32_t sub  =  0.204 uS ( 4.896 MIPS)
        int32_t mul  =  0.236 uS ( 4.232 MIPS)
        int32_t div  =  0.804 uS ( 1.244 MIPS)
       uint32_t sal  =  0.030 uS (33.765 MIPS)
       uint32_t sar  =  0.030 uS (33.820 MIPS)
       uint32_t salx =  0.207 uS ( 4.837 MIPS)
       uint32_t sarx =  0.207 uS ( 4.836 MIPS)
       uint32_t add  =  0.192 uS ( 5.202 MIPS)
       uint32_t sub  =  0.193 uS ( 5.187 MIPS)
       uint32_t mul  =  0.237 uS ( 4.224 MIPS)
       uint32_t div  =  0.775 uS ( 1.290 MIPS)

        int64_t sal  =  0.030 uS (33.720 MIPS)
        int64_t sar  =  0.030 uS (33.754 MIPS)
        int64_t salx =  0.207 uS ( 4.829 MIPS)
        int64_t sarx =  0.207 uS ( 4.834 MIPS)
        int64_t add  =  0.204 uS ( 4.899 MIPS)
        int64_t sub  =  0.194 uS ( 5.147 MIPS)
        int64_t mul  =  0.236 uS ( 4.233 MIPS)
        int64_t div  =  1.318 uS ( 0.759 MIPS)
       uint64_t sal  =  0.030 uS (33.772 MIPS)
       uint64_t sar  =  0.030 uS (33.758 MIPS)
       uint64_t salx =  0.207 uS ( 4.839 MIPS)
       uint64_t sarx =  0.207 uS ( 4.839 MIPS)
       uint64_t add  =  0.204 uS ( 4.899 MIPS)
       uint64_t sub  =  0.185 uS ( 5.396 MIPS)
       uint64_t mul  =  0.237 uS ( 4.226 MIPS)
       uint64_t div  =  1.063 uS ( 0.941 MIPS)

          float add  =  0.266 uS ( 3.764 MIPS)
          float sub  =  0.266 uS ( 3.763 MIPS)
          float mul  =  0.325 uS ( 3.078 MIPS)
          float div  =  0.487 uS ( 2.055 MIPS)
          float sqrt =  0.033 uS (29.981 MIPS)

         double add  =  0.266 uS ( 3.763 MIPS)
         double sub  =  0.266 uS ( 3.763 MIPS)
         double mul  =  0.325 uS ( 3.079 MIPS)
         double div  =  0.483 uS ( 2.070 MIPS)
         double sqrt =  0.033 uS (30.030 MIPS)

    long double add  =  0.354 uS ( 2.823 MIPS)
    long double sub  =  0.354 uS ( 2.823 MIPS)
    long double mul  =  9.777 uS ( 0.102 MIPS)
    long double div  = 10.152 uS ( 0.099 MIPS)
    long double sqrt =  0.214 uS ( 4.670 MIPS)

    ---------------------------
    RaspberryPi 3 - 32-bit Mode
    ---------------------------
         int8_t sal  =  0.419 uS ( 2.385 MIPS)
         int8_t sar  =  0.503 uS ( 1.988 MIPS)
         int8_t salx =  0.587 uS ( 1.705 MIPS)
         int8_t sarx =  0.587 uS ( 1.705 MIPS)
         int8_t add  =  0.419 uS ( 2.387 MIPS)
         int8_t sub  =  0.419 uS ( 2.386 MIPS)
         int8_t mul  =  0.587 uS ( 1.704 MIPS)
         int8_t div  =  1.655 uS ( 0.604 MIPS)
        uint8_t sal  =  0.419 uS ( 2.388 MIPS)
        uint8_t sar  =  0.335 uS ( 2.984 MIPS)
        uint8_t salx =  0.503 uS ( 1.990 MIPS)
        uint8_t sarx =  0.503 uS ( 1.990 MIPS)
        uint8_t add  =  0.419 uS ( 2.387 MIPS)
        uint8_t sub  =  0.419 uS ( 2.388 MIPS)
        uint8_t mul  =  0.586 uS ( 1.705 MIPS)
        uint8_t div  =  1.669 uS ( 0.599 MIPS)

        int16_t sal  =  0.503 uS ( 1.990 MIPS)
        int16_t sar  =  0.503 uS ( 1.990 MIPS)
        int16_t salx =  0.587 uS ( 1.704 MIPS)
        int16_t sarx =  0.586 uS ( 1.706 MIPS)
        int16_t add  =  0.419 uS ( 2.387 MIPS)
        int16_t sub  =  0.419 uS ( 2.388 MIPS)
        int16_t mul  =  0.586 uS ( 1.705 MIPS)
        int16_t div  =  1.654 uS ( 0.605 MIPS)
       uint16_t sal  =  0.419 uS ( 2.387 MIPS)
       uint16_t sar  =  0.335 uS ( 2.984 MIPS)
       uint16_t salx =  0.502 uS ( 1.990 MIPS)
       uint16_t sarx =  0.502 uS ( 1.990 MIPS)
       uint16_t add  =  0.419 uS ( 2.387 MIPS)
       uint16_t sub  =  0.419 uS ( 2.388 MIPS)
       uint16_t mul  =  0.670 uS ( 1.492 MIPS)
       uint16_t div  =  1.487 uS ( 0.673 MIPS)

        int32_t sal  =  0.335 uS ( 2.985 MIPS)
        int32_t sar  =  0.335 uS ( 2.985 MIPS)
        int32_t salx =  0.419 uS ( 2.388 MIPS)
        int32_t sarx =  0.419 uS ( 2.388 MIPS)
        int32_t add  =  0.335 uS ( 2.985 MIPS)
        int32_t sub  =  0.335 uS ( 2.983 MIPS)
        int32_t mul  =  0.503 uS ( 1.990 MIPS)
        int32_t div  =  1.675 uS ( 0.597 MIPS)
       uint32_t sal  =  0.335 uS ( 2.983 MIPS)
       uint32_t sar  =  0.335 uS ( 2.985 MIPS)
       uint32_t salx =  0.419 uS ( 2.388 MIPS)
       uint32_t sarx =  0.419 uS ( 2.388 MIPS)
       uint32_t add  =  0.335 uS ( 2.984 MIPS)
       uint32_t sub  =  0.336 uS ( 2.978 MIPS)
       uint32_t mul  =  0.503 uS ( 1.990 MIPS)
       uint32_t div  =  1.624 uS ( 0.616 MIPS)

        int64_t sal  =  0.440 uS ( 2.274 MIPS)
        int64_t sar  =  0.440 uS ( 2.274 MIPS)
        int64_t salx =  0.754 uS ( 1.326 MIPS)
        int64_t sarx =  0.880 uS ( 1.137 MIPS)
        int64_t add  =  0.440 uS ( 2.274 MIPS)
        int64_t sub  =  0.440 uS ( 2.274 MIPS)
        int64_t mul  =  0.838 uS ( 1.194 MIPS)
        int64_t div  =  5.382 uS ( 0.186 MIPS)
       uint64_t sal  =  0.440 uS ( 2.274 MIPS)
       uint64_t sar  =  0.440 uS ( 2.274 MIPS)
       uint64_t salx =  0.754 uS ( 1.327 MIPS)
       uint64_t sarx =  0.754 uS ( 1.327 MIPS)
       uint64_t add  =  0.440 uS ( 2.274 MIPS)
       uint64_t sub  =  0.440 uS ( 2.271 MIPS)
       uint64_t mul  =  0.838 uS ( 1.194 MIPS)
       uint64_t div  =  4.376 uS ( 0.229 MIPS)

          float add  =  0.670 uS ( 1.492 MIPS)
          float sub  =  0.670 uS ( 1.493 MIPS)
          float mul  =  0.670 uS ( 1.493 MIPS)
          float div  =  1.424 uS ( 0.702 MIPS)
          float sqrt =  0.272 uS ( 3.674 MIPS)

         double add  =  0.670 uS ( 1.493 MIPS)
         double sub  =  0.670 uS ( 1.493 MIPS)
         double mul  =  0.670 uS ( 1.492 MIPS)
         double div  =  2.177 uS ( 0.459 MIPS)
         double sqrt =  0.272 uS ( 3.674 MIPS)

    long double add  =  0.670 uS ( 1.492 MIPS)
    long double sub  =  0.670 uS ( 1.492 MIPS)
    long double mul  =  0.671 uS ( 1.491 MIPS)
    long double div  =  2.178 uS ( 0.459 MIPS)
    long double sqrt =  0.272 uS ( 3.674 MIPS)
 */
