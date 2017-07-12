// https://en.wikipedia.org/wiki/Lab_color_space
#define D65_WHITEPOINT_X 0.950456
#define D65_WHITEPOINT_Y 1.0
#define D65_WHITEPOINT_Z 1.088754

template <typename T>
static inline T srgbGammaCorrection(T v)
{
    // https://en.wikipedia.org/wiki/SRGB
    return (v <= (T) 0.0031306684425005883) ? ( (T) 12.92 * v ) : ( (T) 1.055 * std::pow(v, (T) 0.416666666666666667) - (T) 0.055 );
}

template <typename T>
static inline T srgbInverseGammaCorrection(T v)
{
    // https://en.wikipedia.org/wiki/SRGB
    return (v <= (T) 0.0404482362771076) ? ( v / (T) 12.92 ) : std::pow( (v + (T) 0.055) / (T) 1.055, (T) 2.4 );
}

template <typename T>
static inline T labFunc(T v)
{
    // https://en.wikipedia.org/wiki/Lab_color_space
    return (v >= (T) 8.85645167903563082e-3) ? std::pow( v, (T) 0.333333333333333 ) : ( ((T) 841.0 / (T) 108.0) * v + ((T) 4.0 / (T) 29.0) );
}

template <typename T>
static inline T labInvFunc(T v)
{
    return (v >= (T) 0.206896551724137931) ? ( v * v * v ) : ( ((T) 108.0 / (T) 841.0) * (v - ((T) 4.0 / (T) 29.0)) );
}

template <typename T>
static inline void cnvRgbToXyz(T* x, T* y, T* z, T r, T g, T b)
{
    // https://en.wikipedia.org/wiki/SRGB
    // https://en.wikipedia.org/wiki/CIE_1931_color_space

    r = srgbInverseGammaCorrection(r);
    g = srgbInverseGammaCorrection(g);
    b = srgbInverseGammaCorrection(b);

    *x = (T) 0.41239558896741421610 * r + (T) 0.3575834307637148171 * g + (T) 0.18049264738170157350 * b;
    *y = (T) 0.21258623078559555160 * r + (T) 0.7151703037034108499 * g + (T) 0.07220049864333622685 * b;
    *z = (T) 0.01929721549174694484 * r + (T) 0.1191838645808485318 * g + (T) 0.95049712513157976600 * b;
}

template <typename T>
static inline void cnvXyzToRgb(T* r, T* g, T* b, T x, T y, T z)
{
    // https://en.wikipedia.org/wiki/SRGB
    // https://en.wikipedia.org/wiki/CIE_1931_color_space

    T rs = (T)  3.2406 * x - (T) 1.5372 * y - (T) 0.4986 * z;
    T gs = (T) -0.9689 * x + (T) 1.8758 * y + (T) 0.0415 * z;
    T bs = (T)  0.0557 * x - (T) 0.2040 * y + (T) 1.0570 * z;

    T mins = std::min( std::min( rs, gs ), bs );

    if(mins < (T) 0.0) {
        rs -= mins;
        gs -= mins;
        bs -= mins;
    }

    *r = srgbGammaCorrection(rs);
    *g = srgbGammaCorrection(gs);
    *b = srgbGammaCorrection(bs);
}

template <typename T>
static inline void cnvXyzToLab(T* l, T* a, T* b, T x, T y, T z)
{
    // https://en.wikipedia.org/wiki/Lab_color_space

    x = labFunc(x / (T) D65_WHITEPOINT_X);
    y = labFunc(y / (T) D65_WHITEPOINT_Y);
    z = labFunc(z / (T) D65_WHITEPOINT_Z);

    *l = (T) 116.0 *  y - (T) 16.0;
    *a = (T) 500.0 * (x - y);
    *b = (T) 200.0 * (y - z);
}

template <typename T>
static inline void cnvLabToXyz(T* x, T* y, T* z, T l, T a, T b)
{
    // https://en.wikipedia.org/wiki/Lab_color_space

    l = (l + (T) 16.0) / (T) 116.0;

    *y = (T) D65_WHITEPOINT_Y * labInvFunc(l            );
    *x = (T) D65_WHITEPOINT_X * labInvFunc(l + a / (T) 500.0);
    *z = (T) D65_WHITEPOINT_Z * labInvFunc(l - b / (T) 200.0);
}

template <typename T>
static inline void cnvRgbToLab(T* l, T* a, T* b, Pt::uint8_t r_, Pt::uint8_t g_, Pt::uint8_t b_)
{
    T x, y, z;
    cnvRgbToXyz(&x, &y, &z, (T) r_ / (T) 255.0, (T) g_ / (T) 255.0, (T) b_ / (T) 255.0);

    cnvXyzToLab(l, a, b, x, y, z);
}

template <typename T>
static inline void cnvLabToRgb(Pt::uint8_t* r_, Pt::uint8_t* g_, Pt::uint8_t* b_, T l, T a, T b)
{
    T x, y, z;
    cnvLabToXyz(&x, &y, &z, l, a, b);

    T rf, gf, bf;
    cnvXyzToRgb(&rf, &gf, &bf, x, y, z);
    *r_ = rf * (T) 255.0;
    *g_ = gf * (T) 255.0;
    *b_ = bf * (T) 255.0;
}


template <Pt::uint8_t F, typename T>
static inline T logisticSigmoid(T v)
{
    // https://en.wikipedia.org/wiki/Sigmoid_function
    v *= (T) F * (T) 2.0;
    v -= (T) F;

    return (T) 1.0 / ( (T) 1.0 + std::exp(-v) );
}
