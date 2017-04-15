static size_t benchImageScalingBlock(int loopCount)
{
    size_t sum = 0;

    Image scaledImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        blockScale(scaledImage, textureWithWhiteBackground);

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
Image scaling  4 (block               )          =     10
Image scaling  4 (bilinear            )          =    120 (12.000)

Image rotation 4 (block    - normal   )          =     36
Image rotation 4 (block    - fullscale)          =     73 ( 2.028)
Image rotation 4 (bilinear - normal   )          =    198 ( 5.500)
Image rotation 4 (bilinear - fullscale)          =    291 ( 8.083)
*/

/*
---------------------------------------------------
Arm (v7l; A53; BCM2709; RaspberryPi 3; 32-bit Mode)
---------------------------------------------------
Image scaling  4 (block               )          =    144
Image scaling  4 (bilinear            )          =   1425 ( 9.896)

Image rotation 4 (block    - normal   )          =    650
Image rotation 4 (block    - fullscale)          =   2148 ( 3.305)
Image rotation 4 (bilinear - normal   )          =   2887 ( 4.442)
Image rotation 4 (bilinear - fullscale)          =   3569 ( 5.491)
*/
