template <int mode>
static size_t benchImageScalingBlock(int loopCount)
{
    size_t sum = 0;

    Image scaledImage( textureWithWhiteBackground.format(), Size(120, 120) );

    Argb32Image srcArgb32(textureWithWhiteBackground.size());
    Argb32Image dstArgb32(scaledImage               .size());
    memcpy(
        srcArgb32.data(),
        textureWithWhiteBackground.data(),
        textureWithWhiteBackground.width() * textureWithWhiteBackground.height() * textureWithWhiteBackground.format().pixelStride()
    );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        if(mode == -1) {
            blockScale(
                textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
                scaledImage               .begin(), scaledImage               .width(), scaledImage               .height()
            );
        }
        else if(mode == -2) {
            blockScale(
                srcArgb32.begin(), srcArgb32.width(), srcArgb32.height(),
                dstArgb32.begin(), dstArgb32.width(), dstArgb32.height()
            );
            memcpy(
                scaledImage.data(),
                dstArgb32.data(),
                scaledImage.width() * scaledImage.height() * scaledImage.format().pixelStride()
            );
        }
        else {
            blockScale(scaledImage, textureWithWhiteBackground);
        }

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}


static size_t benchImageScalingBilinear(int loopCount)
{
    size_t sum = 0;

    Image scaledImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        bilinearScale(scaledImage, textureWithWhiteBackground);

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}

template <bool fullscale>
static size_t benchImageRotationBlock(int loopCount)
{
    size_t sum = 0;

    Image scaledImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        blockRotate(scaledImage, textureWithWhiteBackground, 30, Color::fromRgb8(0, 0, 0, 255), fullscale);

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}

template <bool fullscale>
static size_t benchImageRotationBilinear(int loopCount)
{
    size_t sum = 0;

    Image scaledImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        bilinearRotate(scaledImage, textureWithWhiteBackground, 30, Color::fromRgb8(0, 0, 0, 255), fullscale);

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}

/*
-----------------------------
x86_64 (i5-4460; 64-Bit Mode)
-----------------------------
                                                   (Time) (Factor)
                                                   ------ --------
Image scaling    (block - generic     )          =    157
Image scaling    (block - argb32      )          =     40 ( 0.255)
Image scaling  4 (block               )          =      9 ( 0.057)
Image scaling  4 (bilinear            )          =    121 ( 0.771)

Image rotation 4 (block    - normal   )          =     36
Image rotation 4 (block    - fullscale)          =     75 ( 2.083)
Image rotation 4 (bilinear - normal   )          =    197 ( 5.472)
Image rotation 4 (bilinear - fullscale)          =    292 ( 8.111)
*/

/*
---------------------------------------------------
Arm (v7l; A53; BCM2709; RaspberryPi 3; 32-bit Mode)
---------------------------------------------------
Image scaling    (block - generic     )          =   2431
Image scaling    (block - argb32      )          =    522 ( 0.215)
Image scaling  4 (block               )          =    141 ( 0.058)
Image scaling  4 (bilinear            )          =   1639 ( 0.674)

Image rotation 4 (block    - normal   )          =    625
Image rotation 4 (block    - fullscale)          =   2121 ( 3.394)
Image rotation 4 (bilinear - normal   )          =   2685 ( 4.296)
Image rotation 4 (bilinear - fullscale)          =   4323 ( 6.917)
*/
