void fillOnePixelGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const std::map<Pt::int32_t, Pt::int32_t>* exclusionZone, DrawLineMask* maskInOut);


// Xiaolin Wu's Anti-Aliased Line Algorithm
// https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
void Rasterizer2::fillOnePixelGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const std::map<Pt::int32_t, Pt::int32_t>* exclusionZone, DrawLineMask* maskInOut)
{
    // Get the mask's coordinate as needed
    Pt::int32_t mx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t my[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    if(maskInOut) {
        for(Pt::int32_t i = 0; i < 4; ++i) {
            mx[i] = (*maskInOut)[i].x();
            my[i] = (*maskInOut)[i].y();
        }
    }

    // Convert the coordinates to fixed-points
    Pt::int32_t fx1 = FIXED_POINT_FROM_INT(x1);
    Pt::int32_t fy1 = FIXED_POINT_FROM_INT(y1);
    Pt::int32_t fx2 = FIXED_POINT_FROM_INT(x2);
    Pt::int32_t fy2 = FIXED_POINT_FROM_INT(y2);

    // A helper macro to fill pixel
    #define XW_FILL_PIXEL(IMG, X, Y, A)                                                         \
        do {                                                                                    \
            /* Check the boundary limit, just in case */                                        \
            if( (X) < 0 || (X) >= IMG->width() || (Y) < 0 || (Y) >= IMG->height() ) break;      \
            /* Check if we should skip drawing the pixel */                                     \
            bool skipDrawing = false;                                                           \
            if(exclusionZone) {                                                                 \
                std::map<Pt::int32_t, Pt::int32_t>::const_iterator xt = exclusionZone->find(Y); \
                skipDrawing = ( xt != exclusionZone->end() ) && ( xt->second == X );            \
            }                                                                                   \
            for(Pt::int32_t j = 0; !skipDrawing && j < 4; ++j) {                                \
                if( (X) != mx[j] || (Y) != my[j] ) continue;                                    \
                skipDrawing = true;                                                             \
                break;                                                                          \
            }                                                                                   \
            if(skipDrawing) break;                                                              \
            /* Fill the pixel */                                                                \
            if(_isTexture || _isGradient) {                                                     \
                const Pt::int32_t bw = _brushImage->width();                                    \
                const Pt::int32_t bh = _brushImage->height();                                   \
                ConstPixel srcPixel(_brushImage->view(), (X - minX) % bw, (Y - minY) % bh);     \
                Pixel      dstPixel(IMG->view(), X, Y);                                         \
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, A);             \
            }                                                                                   \
            else {                                                                              \
                Pixel pixel(IMG->view(), X, Y);                                                 \
                _image->format().setPixel(pixel, _brush.color(), _compositionMode, A);          \
            }                                                                                   \
        } while(false)

    // Swap the values as needed
    const Pt::int32_t deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
    const Pt::int32_t deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);
    const bool        steep  = deltaY > deltaX;

    if(steep) {
        std::swap(fx1, fy1);
        std::swap(fx2, fy2);
    }

    if(fx1 > fx2) {
        std::swap(fx1, fx2);
        std::swap(fy1, fy2);
    }

    // Handle the gradient, starting point, and ending point
    const Pt::int32_t grad = (fy2 - fy1) / FIXED_POINT_TO_INT(fx2 - fx1);
    const Pt::int32_t xpxl1 = FIXED_POINT_ROUND(fx1);
    const Pt::int32_t xpxl2 = FIXED_POINT_ROUND(fx2);
    const Pt::int32_t ypxl  = fy1 + grad * FIXED_POINT_TO_INT(xpxl1 - fx1);

    // Draw the pixels
    Pt::int32_t from  = FIXED_POINT_TO_INT(FIXED_POINT_ROUND(fx1));
    Pt::int32_t to    = FIXED_POINT_TO_INT(xpxl2);
    Pt::int32_t ypxli = ypxl;

    if(steep) {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            XW_FILL_PIXEL(_image, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), i, a1);
            XW_FILL_PIXEL(_image, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), i, a2);
            ypxli += grad;
        }
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[0].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        )                           ), from);
            (*maskInOut)[1].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        ) + FIXED_POINT_CONSTANT_ONE), from);
            (*maskInOut)[2].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad)                           ), to  );
            (*maskInOut)[3].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad) + FIXED_POINT_CONSTANT_ONE), to  );
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            XW_FILL_PIXEL(_image, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), a1);
            XW_FILL_PIXEL(_image, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), a2);
            ypxli += grad;
        }
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[0].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        )                           ));
            (*maskInOut)[1].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        ) + FIXED_POINT_CONSTANT_ONE));
            (*maskInOut)[2].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad)                           ));
            (*maskInOut)[3].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad) + FIXED_POINT_CONSTANT_ONE));
        }
    }

    // Undefine the helper macro
    #undef XW_FILL_PIXEL
}
#endif
