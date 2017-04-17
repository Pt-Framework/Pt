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
Image scaling    (block    - generic )           =    155
Image scaling    (block    - argb32  )           =     40 ( 0.258)
Image scaling  4 (block              )           =      9 ( 0.058)
Image scaling  4 (bilinear           )           =    128 ( 0.826)

Image rotation 4 (block    - normal  )           =     37
Image rotation 4 (block    - full-fit)           =     37 ( 1.000)
Image rotation 4 (bilinear - normal  )           =    201 ( 5.432)
Image rotation 4 (bilinear - full-fit)           =    204 ( 5.514)
*/

/*
---------------------------------------------------
Arm (v7l; A53; BCM2709; RaspberryPi 3; 32-bit Mode)
---------------------------------------------------
                                                   (Time) (Factor)
                                                   ------ --------
Image scaling    (block    - generic )           =   2441
Image scaling    (block    - argb32  )           =    526 ( 0.215)
Image scaling  4 (block              )           =    142 ( 0.058)
Image scaling  4 (bilinear           )           =   1646 ( 0.674)

Image rotation 4 (block    - normal  )           =    650
Image rotation 4 (block    - full-fit)           =    660 ( 1.015)
Image rotation 4 (bilinear - normal  )           =   2764 ( 4.252)
Image rotation 4 (bilinear - full-fit)           =   3057 ( 4.703)
*/
