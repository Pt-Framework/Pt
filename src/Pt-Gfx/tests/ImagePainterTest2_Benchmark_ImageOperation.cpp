template <int mode>
static size_t benchImageScalingBlock(int loopCount)
{
    size_t sum = 0;

    Image srImage( textureWithWhiteBackground.format(), Size(120, 120) );

    Argb32Image srcArgb32(textureWithWhiteBackground.size());
    Argb32Image dstArgb32(srImage               .size());
    memcpy(
        srcArgb32.data(),
        textureWithWhiteBackground.data(),
        textureWithWhiteBackground.width() * textureWithWhiteBackground.height() * textureWithWhiteBackground.format().pixelStride()
    );

    ImageOperation2::blockScale(textureWithWhiteBackground, srImage);
    ImageOperation2::blockScale(srcArgb32, dstArgb32);

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        if(mode == -1) {
            blockScale(
                textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
                srImage                   .begin(), srImage                   .width(), srImage                   .height()
            );
        }
        else if(mode == -2) {
            blockScale(
                srcArgb32.begin(), srcArgb32.width(), srcArgb32.height(),
                dstArgb32.begin(), dstArgb32.width(), dstArgb32.height()
            );
            /*
            memcpy(
                srImage.data(),
                dstArgb32.data(),
                srImage.width() * srImage.height() * srImage.format().pixelStride()
            );
            */
        }
        else {
            blockScaleImage(textureWithWhiteBackground, srImage);
        }

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}


static size_t benchImageScalingBilinear(int loopCount)
{
    size_t sum = 0;

    Image srImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        bilinearScaleImage(textureWithWhiteBackground, srImage);

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}

template <bool fullscale>
static size_t benchImageRotationBlock(int loopCount)
{
    size_t sum = 0;

    Image srImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        blockRotateImage(textureWithWhiteBackground, srImage, 30, Color::fromRgb8(0, 0, 0, 255), fullscale);

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}

template <bool fullscale>
static size_t benchImageRotationBilinear(int loopCount)
{
    size_t sum = 0;

    Image srImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        bilinearRotateImage(textureWithWhiteBackground, srImage, 30, Color::fromRgb8(0, 0, 0, 255), fullscale);

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
Image scaling    (block    - generic )           =    158
Image scaling    (block    - argb32  )           =     45 ( 0.285)
Image scaling  4 (block              )           =     12 ( 0.076)
Image scaling  4 (bilinear           )           =    136 ( 0.861)

Image rotation 4 (block    - normal  )           =     40
Image rotation 4 (block    - full-fit)           =     40 ( 1.000)
Image rotation 4 (bilinear - normal  )           =    203 ( 5.075)
Image rotation 4 (bilinear - full-fit)           =    204 ( 5.100)
*/

/*
---------------------------------------------------
Arm (v7l; A53; BCM2709; RaspberryPi 3; 32-bit Mode)
---------------------------------------------------
                                                   (Time) (Factor)
                                                   ------ --------
Image scaling    (block    - generic )           =   2440
Image scaling    (block    - argb32  )           =    511 ( 0.209)
Image scaling  4 (block              )           =    142 ( 0.058)
Image scaling  4 (bilinear           )           =   1621 ( 0.664)

Image rotation 4 (block    - normal  )           =    651
Image rotation 4 (block    - full-fit)           =    655 ( 1.006)
Image rotation 4 (bilinear - normal  )           =   3004 ( 4.614)
Image rotation 4 (bilinear - full-fit)           =   3092 ( 4.750)
*/
