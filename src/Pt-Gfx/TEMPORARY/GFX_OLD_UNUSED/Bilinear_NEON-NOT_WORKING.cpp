inline Pt::int32_t bsGetPixel(const Pt::int32_t* img, Pt::ssize_t imgW, float x, float y)
{
    // Floor the coordinate
    const Pt::int32_t px = Pt::Gfx::Math::zfint(x);
    const Pt::int32_t py = Pt::Gfx::Math::zfint(y);

    // Pointer to the first pixel
    const Pt::int32_t* p0 = img + py * imgW + px;

    const Pt::uint64_t p1 = static_cast<Pt::uint64_t>(p0[1]) << 32 & p0[0];
    const Pt::uint64_t p3 = static_cast<Pt::uint64_t>(p0[3]) << 32 & p0[2];

    // Load the four neighboring pixels
    const int32x4_t     p12      = (int32x4_t ) vdupq_n_u64     (             p1                                    );
    const int32x4_t     p34      = (int32x4_t ) vdupq_n_u64     (             p3                                    );

    // Extend to 16-bit integer
    const int8x8_t      p12a1    = (  int8x8_t) vget_low_s16    ( (int16x8_t) p12                                   );
    const int8x8_t      p12b1    = (  int8x8_t) vget_low_s16    ( (int16x8_t) vdupq_n_s32(0)                        );
    const int8x8x2_t    p12x2    =              vzip_s8         (             p12a1,                    p12b1       );
    const int16x8_t     p12ex    = ( int16x8_t) vcombine_s8     (             p12x2.val[0],             p12x2.val[1]);
    const int8x8_t      p34a1    = (  int8x8_t) vget_low_s16    ( (int16x8_t) p34                       );
    const int8x8_t      p34b1    = (  int8x8_t) vget_low_s16    ( (int16x8_t) vdupq_n_s32(0)            );
    const int8x8x2_t    p34x2    =              vzip_s8         (             p34a1,                    p34b1       );
    const int16x8_t     p34ex    = ( int16x8_t) vcombine_s8     (             p34x2.val[0],             p34x2.val[1]);

    // Calculate weights
    const float32x4_t   zzyx     =              NEON_SET_FLT32X4(0, 0, y, x                                         ); // 0  0      Y      X
    const int32x4_t     zzyxi    =              vcvtq_s32_f32   (zzyx                                               ); // 0  0      Yi     Xi
    const float32x4_t   zzyxflor =              vcvtq_f32_s32   (zzyxi                                              ); // 0  0      Yi     Xi

    const float32x4_t   zzyxfrac =              vsubq_f32       (             zzyx,                     zzyxflor    ); // 0  0      Yr     Xr
    const float32x4_t   ooyxfrac =              vsubq_f32       (             neonFour001,              zzyxfrac    ); // ?  ?      (1-Yr) (1-Xr)
    const float32x2x2_t wxhx2    =              vzip_f32        (
                                                    vget_low_f32(ooyxfrac),
                                                    vget_low_f32(zzyxfrac)
                                                );
    const float32x4_t   wxh      =              vcombine_f32    (             wxhx2.val[0],             wxhx2.val[1]); // ?  ?      Xr     (1-Xr)

    const float32x4_t   wx       =              vcombine_f32    (                                                      // Xr (1-Xr) Xr     (1-Xr)
                                                    vget_low_f32(wxh),
                                                    vget_low_f32(wxh)
                                                );
    const float32x4_t   wy       =              vcombine_f32    (                                                      // Yr Yr     (1-Yr) (1-Yr)
                                                    vdup_n_f32(vgetq_lane_f32(ooyxfrac, 1)),
                                                    vdup_n_f32(vgetq_lane_f32(zzyxfrac, 1))
                                                );
    const float32x4_t   weight   =              vmulq_f32       (             wx,                       wy          );

    // Convert the weights to 16-bit integer
    const float32x4_t   sweight  =              vmulq_f32       (             weight,                   neonFour256 ); // W4  .   W3  .   W2  .   W1  .
    const int32x4_t     sweighti =              vcvtq_s32_f32   (             sweight                               ); // W4i .   W3i .   W2i .   W1i .
    const int16x8_t     weighth  =              vcombine_s16    (                                                      // 0   0   0   0   W4i W3i W2i W1i
                                                    vqmovn_s32(sweighti      ),
                                                    vqmovn_s32(vdupq_n_s32(0))
                                                );

    // Shuffle the weights
    const int32x4_t    w12       =              vcombine_s32    (                                                      // W2i W2i W2i W2i W1i W1i W1i W1i
                                                    vdup_n_s32(vgetq_lane_s32((int32x4_t) weighth, 1)),
                                                    vdup_n_s32(vgetq_lane_s32((int32x4_t) weighth, 0))
                                                );
    const int32x4_t    w34       =              vcombine_s32    (                                                      // W4i W4i W4i W4i W3i W3i W3i W3i
                                                    vdup_n_s32(vgetq_lane_s32((int32x4_t) weighth, 3)),
                                                    vdup_n_s32(vgetq_lane_s32((int32x4_t) weighth, 2))
                                                );

    // Multiply each pixel with the corresponding weight
    const int16x8_t    r1r       =              vmulq_s16       ( (int16x8_t) p12ex,        (int16x8_t) w12         );
    const int16x8_t    r34       =              vmulq_s16       ( (int16x8_t) p34ex,        (int16x8_t) w34         );

    // Add the results
    const int16x8_t    r1234     =              vaddq_s16       (             r1r,          r34                     );
    const int16x8_t    r1234h    = ( int16x8_t) vcombine_s32    (
                                                    vget_high_s32((int32x4_t) r1234),
                                                    vget_high_s32((int32x4_t) r1234)
                                                );
    const int16x8_t    r         =              vaddq_s16       (             r1234,        r1234h                  );

    // Divide the results by 256
    const int16x8_t    rdiv256   =              vshrq_n_s16     (             r,            8                       );

    // Convert back the results to 8-bit integer and pack
    const uint8x16_t   rfin      =              vcombine_u8     (
                                                    vqmovun_s16((int16x8_t) rdiv256       ),
                                                    vqmovun_s16((int16x8_t) vdupq_n_s32(0))
                                                );

    // Return the result as a 32-bit integer
    return vgetq_lane_s32((int32x4_t) rfin, 0);
}
