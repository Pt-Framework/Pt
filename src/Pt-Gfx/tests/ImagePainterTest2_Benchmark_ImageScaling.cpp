static size_t benchImageScalingBlock(int loopCount)
{
    size_t sum = 0;

    Image scaledImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        blockScale(
            textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
            scaledImage               .begin(), scaledImage               .width(), scaledImage               .height()
        );

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}

template <typename GetPixelT>
static size_t benchImageScalingBilinear(int loopCount)
{
    size_t sum = 0;

    Image scaledImage( textureWithWhiteBackground.format(), Size(120, 120) );

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        bilinearScale<GetPixelT>(
            textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
            scaledImage               .begin(), scaledImage               .width(), scaledImage               .height()
        );

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}
