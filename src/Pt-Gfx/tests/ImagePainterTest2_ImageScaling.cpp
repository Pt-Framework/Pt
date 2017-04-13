// FastC++: Coding Cpp Efficiently
// Bilinear Pixel Interpolation using SSE
// Blog by theowl84, 2011
// http://fastcpp.blogspot.co.id/2011/06/bilinear-pixel-interpolation-using-sse.html

//
// Normal C
//

inline Pt::int32_t getPixel_C(const Pt::int32_t* img, Pt::ssize_t imageWidth, float x, float y)
{
    // Used for processing the pixels
    union Pixel4 {
        Pt::uint8_t c[4];
        Pt::int32_t i;
    };

    // Floor the coordinate
    const Pt::int32_t px = Pt::Gfx::Math::zfint(x);
    const Pt::int32_t py = Pt::Gfx::Math::zfint(y);

    // Pointer to the first pixel
    const Pixel4* p0 = (const Pixel4*) img + py * imageWidth + px;

    // Load the four neighboring pixels
    const Pixel4& p1 = p0[0 + 0 * imageWidth];
    const Pixel4& p2 = p0[1 + 0 * imageWidth];
    const Pixel4& p3 = p0[0 + 1 * imageWidth];
    const Pixel4& p4 = p0[1 + 1 * imageWidth];

    // Calculate the weights for each pixel
    const float fx  = x    - px;
    const float fy  = y    - py;
    const float fx1 = 1.0f - fx;
    const float fy1 = 1.0f - fy;

    const float w1 = fx1 * fy1;
    const float w2 = fx  * fy1;
    const float w3 = fx1 * fy ;
    const float w4 = fx  * fy ;

    // Calculate the weighted sum of pixels
    Pixel4 r;
    r.c[0] = p1.c[0] * w1 + p2.c[0] * w2 + p3.c[0] * w3 + p4.c[0] * w4;
    r.c[1] = p1.c[1] * w1 + p2.c[1] * w2 + p3.c[1] * w3 + p4.c[1] * w4;
    r.c[2] = p1.c[2] * w1 + p2.c[2] * w2 + p3.c[2] * w3 + p4.c[2] * w4;
    r.c[3] = p1.c[3] * w1 + p2.c[3] * w2 + p3.c[3] * w3 + p4.c[3] * w4;

    // Return the resulting pixel
    return r.i;
}

struct GetPixel_C {
    static inline Pt::int32_t getPixel(const Pt::int32_t* img, Pt::ssize_t imageWidth, float x, float y)
    { return getPixel_C(img, imageWidth, x, y); }
};


//
// SSE2
//

// Constant
static const __m128 sseFour001 = _mm_set1_ps(  1);
static const __m128 sseFour256 = _mm_set1_ps(256);

inline __m128 calcWeight_SSE2(float x, float y)
{
     _MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

     const __m128  ssx       = _mm_set_ss     (x                                            );
     const __m128  ssy       = _mm_set_ss     (y                                            );
     const __m128  psXY      = _mm_unpacklo_ps(ssx,        ssy                              ); // 0 0 Y X

     const __m128i psXYi     = _mm_cvtps_epi32(psXY                                         );
     const __m128  psXYfloor = _mm_cvtepi32_ps(psXYi                                        );
     const __m128  psXYfrac  = _mm_sub_ps     (psXY,       psXYfloor                        );

     const __m128  psXYfrac1 = _mm_sub_ps     (sseFour001, psXYfrac                         ); // ? ?     (1-Y) (1-X)
     const __m128  wxh       = _mm_unpacklo_ps(psXYfrac1,  psXYfrac                         ); // ? ?     X     (1-X)
     const __m128  wx        = _mm_movelh_ps  (wxh,        wxh                              ); // X (1-X) X     (1-X)
     const __m128  wy        = _mm_shuffle_ps (psXYfrac1,  psXYfrac, _MM_SHUFFLE(1, 1, 1, 1)); // Y Y     (1-Y) (1-Y)

     _MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);

     // Return the complete weight vector
     return _mm_mul_ps(wx, wy);
}

inline __m128 calcWeight_SSE4(float x, float y)
{
     const __m128  ssx       = _mm_set_ss     (x                                            );
     const __m128  ssy       = _mm_set_ss     (y                                            );
     const __m128  psXY      = _mm_unpacklo_ps(ssx,        ssy                              ); // 0 0 Y X

     const __m128  psXYfloor = _mm_floor_ps   (psXY                                         );
     const __m128  psXYfrac  = _mm_sub_ps     (psXY,       psXYfloor                        );

     const __m128  psXYfrac1 = _mm_sub_ps     (sseFour001, psXYfrac                         ); // ? ?     (1-Y) (1-X)
     const __m128  wxh       = _mm_unpacklo_ps(psXYfrac1,  psXYfrac                         ); // ? ?     X     (1-X)
     const __m128  wx        = _mm_movelh_ps  (wxh,        wxh                              ); // X (1-X) X     (1-X)
     const __m128  wy        = _mm_shuffle_ps (psXYfrac1,  psXYfrac, _MM_SHUFFLE(1, 1, 1, 1)); // Y Y     (1-Y) (1-Y)
     // Return the complete weight vector
     return _mm_mul_ps(wx, wy);
}

inline Pt::int32_t getPixel_SSE2(const Pt::int32_t* img, Pt::ssize_t imageWidth, float x, float y)
{
    // Floor the coordinate
    const Pt::int32_t px = Pt::Gfx::Math::zfint(x);
    const Pt::int32_t py = Pt::Gfx::Math::zfint(y);

    // Pointer to the first pixel
    const Pt::int32_t* p0 = img + py * imageWidth + px;

    // Load the four neighboring pixels
    const __m128i p12h    = _mm_loadl_epi64    ( (const __m128i*) &p0[0 * imageWidth] );
    const __m128i p34h    = _mm_loadl_epi64    ( (const __m128i*) &p0[1 * imageWidth] );

    // Extend to 16-bit
    const __m128i p12     = _mm_unpacklo_epi8  (p12h,    _mm_setzero_si128()    );
    const __m128i p34     = _mm_unpacklo_epi8  (p34h,    _mm_setzero_si128()    );

    // Calculate weight
    const __m128  weight  = calcWeight_SSE2(x, y);

    // Convert floating point weights to 16-bit integer
    const __m128  weights = _mm_mul_ps         (weight,  sseFour256             );
    const __m128i weighti = _mm_cvtps_epi32    (weights                         ); // W4 W3 W2 W1
    const __m128i weighth = _mm_packs_epi32    (weighti, _mm_setzero_si128()    ); // 32-bit to 16-bit

     // Prepare the weights
    const __m128i w12h    = _mm_shufflelo_epi16(weighth, _MM_SHUFFLE(1, 1, 0, 0));
    const __m128i w12     = _mm_unpacklo_epi16 (w12h,    w12h                   ); // W2 W2 W2 W2 W1 W1 W1 W1

    const __m128i w34h    = _mm_shufflelo_epi16(weighth, _MM_SHUFFLE(3, 3, 2, 2));
    const __m128i w34     = _mm_unpacklo_epi16 (w34h,    w34h                   ); // W4 W4 W4 W4 W3 W3 W3 W3

    // Multiply each pixel with its weight
    const __m128i L12     = _mm_mullo_epi16    (p12,     w12                    );
    const __m128i L34     = _mm_mullo_epi16    (p34,     w34                    );

    // Add the results
    const __m128i L1234   = _mm_add_epi16      (L12,     L34                    );
    const __m128i Lhi     = _mm_shuffle_epi32  (L1234,   _MM_SHUFFLE(3, 2, 3, 2));
    const __m128i L       = _mm_add_epi16      (L1234,   Lhi                    );

    // Convert back to 8-bit
    const __m128i L8h     = _mm_srli_epi16     (L,       8                      ); // Divide by 256
    const __m128i L8      = _mm_packus_epi16   (L8h,     _mm_setzero_si128()    );

    // Return the result
    return _mm_cvtsi128_si32(L8);
}

struct GetPixel_SSE2 {
    static inline Pt::int32_t getPixel(const Pt::int32_t* img, Pt::ssize_t imageWidth, float x, float y)
    { return getPixel_SSE2(img, imageWidth, x, y); }
};


//
// Wrapper
//

template <typename GetPixelT, typename InIterT, typename OutIterT>
void bilinearScale(
    InIterT  from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
    OutIterT to,   Pt::ssize_t toWidth,   Pt::ssize_t toHeight
)
{
    const Pt::int32_t* src = (const Pt::int32_t*) from->base();
          Pt::int32_t* dst = (      Pt::int32_t*) to  ->base();

    const float incX = (float) fromWidth  / (float) toWidth;
    const float incY = (float) fromHeight / (float) toHeight;

    float itrY = 0;
    for(Pt::ssize_t y = 0; y < toHeight; ++y) {
        float itrX = 0;
        for(Pt::ssize_t x = 0; x < toWidth; ++x) {
            *dst++ = GetPixelT::getPixel(src, fromWidth, itrX, itrY);
            itrX += incX;
        }
        itrY += incY;
    }
}


//
// Test
//

static void testImageScaling(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    painter.setFont( Pt::Gfx::Font(FONT_SPEC_S) );
    painter.setPen( Color::fromRgb8(255, 255, 0, 255) );

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    // Reference image
    Pt::int32_t x = 0;
    Pt::int32_t y = 0;

    painter.drawImage(PointF(x, y), textureWithWhiteBackground);
    x += textureWithWhiteBackground.width() + 20;

    // Scaled image (block scale)
    Image scaledImage( image.format(), Size(120, 120) );

    blockScale(
        textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
        scaledImage               .begin(), scaledImage               .width(), scaledImage               .height()
    );
    painter.drawImage(PointF(x, y), scaledImage);
    painter.drawText( PointF(x, y + scaledImage.height() + 20), "Block - Plain C" );
    x += scaledImage.width() + 20;

    // Scaled image (bilinear scale)
    bilinearScale<GetPixel_C>(
        textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
        scaledImage               .begin(), scaledImage               .width(), scaledImage               .height()
    );
    painter.drawImage(PointF(x, y), scaledImage);
    painter.drawText( PointF(x, y + scaledImage.height() + 20), "Bilinear - Plain C" );
    x += scaledImage.width() + 20;

    // Scaled image (bilinear scale)
    bilinearScale<GetPixel_SSE2>(
        textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
        scaledImage               .begin(), scaledImage               .width(), scaledImage               .height()
    );
    painter.drawImage(PointF(x, y), scaledImage);
    painter.drawText( PointF(x, y + scaledImage.height() + 20), "Bilinear - SSE2" );
    x += scaledImage.width() + 20;

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
