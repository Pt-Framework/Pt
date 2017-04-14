static size_t benchImageScalingBlock(int loopCount)
{
    size_t sum = 0;

    Image scaledImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        blockScale4(
            textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
            scaledImage               .begin(), scaledImage               .width(), scaledImage               .height()
        );

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

        bilinearScale4(
            textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
            scaledImage               .begin(), scaledImage               .width(), scaledImage               .height()
        );

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}

static size_t benchImageRotationBlock(int loopCount)
{
    size_t sum = 0;

    Image scaledImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        blockRotate4(
            textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
            scaledImage               .begin(), scaledImage               .width(), scaledImage               .height(),
            30
        );

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}

static size_t benchImageRotationBilinear(int loopCount)
{
    size_t sum = 0;

    Image scaledImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        bilinearRotate4(
            textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
            scaledImage               .begin(), scaledImage               .width(), scaledImage               .height(),
            30
        );

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}
