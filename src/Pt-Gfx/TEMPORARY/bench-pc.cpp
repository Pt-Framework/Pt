// g++ -O2 bench-pc.cpp -o bench-pc && ./bench-pc && rm -f bench-pc

// Results are below the program

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

double mygettime()
{
  struct timeval tv;
  if(gettimeofday(&tv, 0) < 0) {
    perror("oops");
  }
  return (double)tv.tv_sec + (0.000001 * (double)tv.tv_usec);
}

template< typename Type >
void my_test(const char* name) {
  volatile Type v  = 0;
  // Do not use constants or repeating values
  //  to avoid loop unroll optimizations.
  // All values >0 to avoid division by 0
  Type v0 = (Type)(rand() % 128)/16 + 1;
  Type v1 = (Type)(rand() % 128)/16 + 1;
  Type v2 = (Type)(rand() % 128)/16 + 1;
  Type v3 = (Type)(rand() % 128)/16 + 1;
  Type v4 = (Type)(rand() % 128)/16 + 1;
  Type v5 = (Type)(rand() % 128)/16 + 1;
  Type v6 = (Type)(rand() % 128)/16 + 1;
  Type v7 = (Type)(rand() % 128)/16 + 1;

  double t1 = mygettime();
  for (size_t i = 0; i < 100000000; ++i) {
    v += v0;
    v += v2;
    v += v4;
    v += v6;
  }
  printf("%s add = %7.3f uS\n", name, mygettime() - t1);

  t1 = mygettime();
  for (size_t i = 0; i < 100000000; ++i) {
    v -= v1;
    v -= v3;
    v -= v5;
    v -= v7;
  }
  printf("%s sub = %7.3f uS\n", name, mygettime() - t1);

  t1 = mygettime();
  for (size_t i = 0; i < 100000000; ++i) {
    v *= v0;
    v *= v2;
    v *= v4;
    v *= v6;
  }
  printf("%s mul = %7.3f uS\n", name, mygettime() - t1);

  t1 = mygettime();
  for (size_t i = 0; i < 100000000; ++i) {
    v /= v1;
    v /= v3;
    v /= v5;
    v /= v7;
  }
  printf("%s div = %7.3f uS\n", name, mygettime() - t1);
}

int main() {
  my_test<     int8_t>("     int8_t");
  my_test<    uint8_t>("    uint8_t");

  my_test<    int16_t>("    int16_t");
  my_test<   uint16_t>("   uint16_t");

  my_test<    int32_t>("    int32_t");
  my_test<   uint32_t>("   uint32_t");

  my_test<    int64_t>("    int64_t");
  my_test<   uint64_t>("   uint64_t");

  my_test<      float>("      float");
  my_test<     double>("     double");
  my_test<long double>("long double");

  return 0;
}

/*
    Core i5 64-bit Mode
    -------------------
         int8_t add =   0.807 uS
         int8_t sub =   0.820 uS
         int8_t mul =   0.945 uS
         int8_t div =   3.216 uS
        uint8_t add =   0.816 uS
        uint8_t sub =   0.816 uS
        uint8_t mul =   0.946 uS
        uint8_t div =   2.960 uS
        int16_t add =   0.802 uS
        int16_t sub =   0.818 uS
        int16_t mul =   0.945 uS
        int16_t div =   3.216 uS
       uint16_t add =   0.816 uS
       uint16_t sub =   0.816 uS
       uint16_t mul =   0.945 uS
       uint16_t div =   3.097 uS
        int32_t add =   0.823 uS
        int32_t sub =   0.792 uS
        int32_t mul =   0.945 uS
        int32_t div =   3.217 uS
       uint32_t add =   0.792 uS
       uint32_t sub =   0.804 uS
       uint32_t mul =   0.945 uS
       uint32_t div =   3.104 uS
        int64_t add =   0.818 uS
        int64_t sub =   0.814 uS
        int64_t mul =   0.945 uS
        int64_t div =   5.275 uS
       uint64_t add =   0.811 uS
       uint64_t sub =   0.781 uS
       uint64_t mul =   0.945 uS
       uint64_t div =   4.255 uS
          float add =   1.064 uS
          float sub =   1.064 uS
          float mul =   1.299 uS
          float div =   1.944 uS
         double add =   1.063 uS
         double sub =   1.063 uS
         double mul =   1.299 uS
         double div =   1.940 uS
    long double add =   1.418 uS
    long double sub =   1.418 uS
    long double mul =  39.092 uS
    long double div =  40.748 uS

    RaspberryPi 3 32-bit Mode
    -------------------------
         int8_t add =   1.677 uS
         int8_t sub =   1.721 uS
         int8_t mul =   2.337 uS
         int8_t div =   6.592 uS
        uint8_t add =   1.669 uS
        uint8_t sub =   1.669 uS
        uint8_t mul =   2.336 uS
        uint8_t div =   6.565 uS
        int16_t add =   1.669 uS
        int16_t sub =   1.669 uS
        int16_t mul =   2.337 uS
        int16_t div =   7.364 uS
       uint16_t add =   1.669 uS
       uint16_t sub =   1.669 uS
       uint16_t mul =   2.670 uS
       uint16_t div =   6.583 uS
        int32_t add =   1.335 uS
        int32_t sub =   1.335 uS
        int32_t mul =   2.003 uS
        int32_t div =   6.592 uS
       uint32_t add =   1.335 uS
       uint32_t sub =   1.335 uS
       uint32_t mul =   2.003 uS
       uint32_t div =   6.524 uS
        int64_t add =   1.752 uS
        int64_t sub =   1.752 uS
        int64_t mul =   3.338 uS
        int64_t div =  21.446 uS
       uint64_t add =   1.752 uS
       uint64_t sub =   1.752 uS
       uint64_t mul =   3.338 uS
       uint64_t div =  17.441 uS
          float add =   2.670 uS
          float sub =   2.670 uS
          float mul =   2.670 uS
          float div =   5.674 uS
         double add =   2.670 uS
         double sub =   2.670 uS
         double mul =   2.670 uS
         double div =   8.678 uS
    long double add =   2.670 uS
    long double sub =   2.670 uS
    long double mul =   2.670 uS
    long double div =   8.678 uS
 */
