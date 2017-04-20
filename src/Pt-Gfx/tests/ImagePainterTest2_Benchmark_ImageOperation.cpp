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
            ImageOperation2::blockScale(textureWithWhiteBackground, srImage);
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

        ImageOperation2::bilinearScale(textureWithWhiteBackground, srImage);

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}

static size_t benchImageRotationBlock(int loopCount, ImageOperation2::ImageRotateMode irm)
{
    size_t sum = 0;

    Image srImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        ImageOperation2::blockRotate(textureWithWhiteBackground, srImage, 30, Color::fromRgb8(0, 0, 0, 255), irm);

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}

static size_t benchImageRotationBilinear(int loopCount, ImageOperation2::ImageRotateMode irm)
{
    size_t sum = 0;

    Image srImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        ImageOperation2::bilinearRotate(textureWithWhiteBackground, srImage, 30, Color::fromRgb8(0, 0, 0, 255), irm);

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
Image scaling    (block    - generic )           =    174
Image scaling    (block    - argb32  )           =     39 ( 0.224)
Image scaling  4 (block              )           =     10 ( 0.057)
Image scaling  4 (bilinear           )           =    125 ( 0.718)

Image rotation 4 (block    - crop    )           =     40
Image rotation 4 (block    - no-crop )           =     40 ( 1.000)
Image rotation 4 (block    - fit     )           =     40 ( 1.000)
Image rotation 4 (bilinear - crop    )           =    206 ( 5.150)
Image rotation 4 (bilinear - no-crop )           =    208 ( 5.200)
Image rotation 4 (bilinear - fit     )           =    208 ( 5.200)
*/

/*
---------------------------------------------------
Arm (v7l; A53; BCM2709; RaspberryPi 3; 32-bit Mode)
---------------------------------------------------
*/
