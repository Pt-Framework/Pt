// Uncomment this to benchmark SDL
// #define BENCHMARK_SDL

static int writePNG(const char* filename, int width, int height, const Pt::uint8_t* argb8888Buff)
{
#if defined(PT_GFX_USE_ARM_CPU)
    // To minimize SVN pollution, do not save the resulting image when
    // running on secondary development/testing system
    (void) filename;
    (void) width;
    (void) height;
    (void) argb8888Buff;
    return 0;
#endif

    int         code     = 0;
    FILE*       fp       = 0;
    png_structp png_ptr  = 0;
    png_infop   info_ptr = 0;
    png_bytep   row      = 0;

    // Open file for writing (binary mode)
    fp = fopen(filename, "wb");
    if(!fp) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        code = -1;
        goto finalise;
    }

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if(!png_ptr) {
        fprintf(stderr, "Could not allocate write struct\n");
        code = -1;
        goto finalise;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = -1;
        goto finalise;
    }

    // Setup Exception handling
    if(setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png creation\n");
        code = -1;
        goto finalise;
    }

    png_init_io(png_ptr, fp);

    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    // Write information
    png_write_info(png_ptr, info_ptr);

    // Allocate memory for one row (3 bytes per pixel - RGB)
    row = (png_bytep) malloc(3 * width * sizeof(png_byte));

    // Write image data
    for(int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            row[x * 3 + 2] = *argb8888Buff++;
            row[x * 3 + 1] = *argb8888Buff++;
            row[x * 3 + 0] = *argb8888Buff++;
            ++argb8888Buff;
        }
        png_write_row(png_ptr, row);
    }

    // End write
    png_write_end(png_ptr, 0);

finalise:
    if(fp      ) fclose(fp);
    if(info_ptr) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if(png_ptr ) png_destroy_write_struct(&png_ptr, 0);
    if(row     ) free(row);

    return code;
}

static void resetImage(Image& image)
{
    const Size origSize = image.size();
    image.reset(image.format(), Size(0, 0));
    image.reset(image.format(), origSize);
}

static const std::string formatCaption(const std::string& className, CompositionMode cm, const char* funcName)
{
    // Get the composition mode string
    std::string cmStr;
    switch(cm) {
        case CompositionMode::SourceCopy : cmStr = "SourceCopy"; break;
        case CompositionMode::SourceOver : cmStr = "SourceOver"; break;
        default                          : cmStr = "<unknown>";  break;
    }

    // Generate the information text
    std::stringstream ss;
    ss << funcName << "() - " << className << " [" << cmStr << "]";

    // Return the text
    return ss.str();
}

static const std::string formatCaption(const Painter& painter, CompositionMode cm, const char* funcName)
{
    // Get the class name string
    std::string className = typeid(painter).name();
#if defined(PT_GFX_USE_GNU_STYLE_COMPILER)
    int s;
    char* demangled = abi::__cxa_demangle(className.c_str(), 0, 0, &s);
    className = demangled;
    free(demangled);
#endif

    // Call the overload function
    return formatCaption(className, cm, funcName);
}



static void sdlPreviewRGB888Buffer(const std::string& title, const Pt::uint8_t* argb8888Buff, int sizeX, int sizeY, bool saveImageAsPNG)
{
#if defined(WITH_EXPERIMENTAL_GFX)
    // Save the image as a PNG file
    if(saveImageAsPNG) {
        std::string eraseStr = " - ImagePainter2";
        std::string fileName = std::string(sfileDirXPrefix) + "../src/Pt-Gfx/TEMPORARY/IPT2 - " + title + ".png";
        fileName.erase(fileName.find(eraseStr), eraseStr.length());
        if(writePNG(fileName.c_str(), sizeX, sizeY, argb8888Buff) < 0) return;
    }

    // Initialise SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) return;

    // Create window, renderer, and texture objects
#ifdef BENCHMARK_SDL
    bool              firstFrame = true;
    size_t            etime      = 0;
    size_t            sum        = 0;
    Pt::System::Clock clock;
    clock.start();
#endif
    SDL_Window*   sdlWindow     = SDL_CreateWindow  (title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sizeX, sizeY, 0);
    SDL_Renderer* sdlRenderer   = SDL_CreateRenderer(sdlWindow, -1, 0);
    SDL_Texture*  sdlTexture    = SDL_CreateTexture (sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, sizeX, sizeY);
#ifdef BENCHMARK_SDL
    std::clog << std::endl;
    etime  = clock.stop().toUSecs();
    sum   += etime;
    std::clog << "SDL main system initialization     = " << std::setw(8) << etime << " uS" << std::endl;
#endif

    // Bring the window on top by force
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(sdlWindow, &info);

    XClientMessageEvent xclient;
    memset(&xclient, 0, sizeof (xclient));

    xclient.type         = ClientMessage;
    xclient.window       = info.info.x11.window;
    xclient.message_type = XInternAtom(info.info.x11.display, "_NET_WM_STATE", 1);
    xclient.format       = 32;
    xclient.data.l[0]    = 1; // 0 = remove; 1 = add; 2 = toggle
    xclient.data.l[1]    = XInternAtom(info.info.x11.display, "_NET_WM_STATE_ABOVE", 1);

    XSendEvent(info.info.x11.display, DefaultRootWindow(info.info.x11.display), False, SubstructureRedirectMask | SubstructureNotifyMask, (XEvent*) &xclient);

    // Render the image data
#ifdef BENCHMARK_SDL
    clock.start();
#endif
    SDL_UpdateTexture(sdlTexture, 0, argb8888Buff, sizeX * 4);
    SDL_RenderClear  (sdlRenderer);
    SDL_RenderCopy   (sdlRenderer, sdlTexture,    0, 0);
#ifdef BENCHMARK_SDL
    etime  = clock.stop().toUSecs();
    sum   += etime;
    std::clog << "SDL texture creation and rendering = " << std::setw(8) << etime << " uS" << std::endl;
#endif

    // Display the image
    for(;;) {
        // Check for event
        SDL_Event event;
        SDL_PollEvent(&event);
        if( (event.type == SDL_QUIT                                                    ) ||
            (event.type == SDL_KEYUP         && event.key.keysym.sym == SDLK_ESCAPE    ) ||
            (event.type == SDL_MOUSEBUTTONUP && event.button.button  == SDL_BUTTON_LEFT)
        ) break;
        // Display the frame and wait for a while
#ifdef BENCHMARK_SDL
        if(firstFrame) clock.start();
#endif
        SDL_RenderPresent(sdlRenderer);
#ifdef BENCHMARK_SDL
        if(firstFrame) {
            etime  = clock.stop().toUSecs();
            sum   += etime;
            std::clog << "SDL displaying rendered texture    = " << std::setw(8) << etime << " uS" << std::endl;
            firstFrame = false;
        }
#endif
        usleep(10000);
    }

    // Done
#ifdef BENCHMARK_SDL
    clock.start();
#endif
    SDL_DestroyTexture (sdlTexture   );
    SDL_DestroyRenderer(sdlRenderer  );
    SDL_DestroyWindow  (sdlWindow    );
    SDL_Quit           (             );
#ifdef BENCHMARK_SDL
    etime  = clock.stop().toUSecs();
    sum   += etime;
    std::clog << "SDL system shutdown and clean-up   = " << std::setw(8) << etime << " uS" << std::endl;
    std::clog << "SDL total consumed time            = " << std::setw(8) << sum << " uS" << std::endl;
    std::clog << std::endl;
#endif

#endif // WITH_EXPERIMENTAL_GFX
}



////////////////////////////////////////////////////////////////////////////////

static volatile float dummyMathValue;

template<int LOOP_COUNT, int MIN, int MAX, int DIV, typename F>
static double benchMathFunction(F f)
{
    const int loopCount = LOOP_COUNT;

    if(DIV) {
        Pt::System::Clock clock;
        clock.start();

        for(int i = 0; i < loopCount ; ++i) {
            for(int j = MIN; j <= MAX; ++j) {
                dummyMathValue = f((float) j / ((float) DIV / 100000.0f));
            }
        }

        return (double) clock.stop().toUSecs() / ( loopCount * (MAX - MIN + 1) );
    }

    Pt::System::Clock clock;
    clock.start();

    for(int i = 0; i < loopCount ; ++i) {
        for(int j = MIN; j <= MAX; ++j) {
            dummyMathValue = f(j);
        }
    }

    return (double) clock.stop().toUSecs() / ( loopCount * (MAX - MIN + 1) );
}

struct F_sqrtf           { float operator() (float x) { return ::sqrtf (x); } };
struct F_fastSqrt        { float operator() (float x) { return ::sqrtf(x); } };
struct F_fastSqrt_SIMD   { float operator() (float x) { return ::sqrtf(x); } };

struct F_isqrtf          { float operator() (float x) { return 1.0f / ::sqrtf(x); } };
struct F_fastInvSqrt     { float operator() (float x) { return 1.0f / ::sqrtf(x); } };
struct F_fastInvSqrt_SIMD{ float operator() (float x) { return 1.0f / ::sqrtf(x); } };

struct F_sinf            { float operator() (float x) { return ::sinf (x); } };
struct F_fastSin         { float operator() (float x) { return ::sinf(x); } };

struct F_cosf            { float operator() (float x) { return ::cosf (x); } };
struct F_fastCos         { float operator() (float x) { return ::cosf(x); } };

struct F_atan2f          { float operator() (float x) { return ::atan2f (100.0f, x);} };
struct F_fastAtan2       { float operator() (float x) { return ::atan2f(100.0f, x);  } };

static void benchMathFunctions()
{
    double time1, time2, time3;

    std::clog << "                    Time    Factor " << std::endl;
    std::clog << "                   ------- --------" << std::endl << std::endl;

    time1 = benchMathFunction<100000, 0, 100, 0>(F_sqrtf        ());
    time2 = benchMathFunction<100000, 0, 100, 0>(F_fastSqrt     ());
    time3 = benchMathFunction<100000, 0, 100, 0>(F_fastSqrt_SIMD());
    printf("sqrtf            = %6.5f\n",           time1               );
    printf("fastSqrt         = %6.5f (%6.3f)\n",   time2, time2 / time1);
    printf("fastSqrt SIMD    = %6.5f (%6.3f)\n\n", time3, time3 / time1);

    time1 = benchMathFunction<100000, 0, 100, 0>(F_isqrtf          ());
    time2 = benchMathFunction<100000, 0, 100, 0>(F_fastInvSqrt     ());
    time3 = benchMathFunction<100000, 0, 100, 0>(F_fastInvSqrt_SIMD());
    printf("1.0f / sqrtf     = %6.5f\n",           time1               );
    printf("fastInvSqrt      = %6.5f (%6.3f)\n",   time2, time2 / time1);
    printf("fastInvSqrt SIMD = %6.5f (%6.3f)\n\n", time3, time3 / time1);

    time1 = benchMathFunction<10000, -360, 360, 1745>(F_sinf   ());
    time2 = benchMathFunction<10000, -360, 360, 1745>(F_fastSin());
    printf("sinf             = %6.5f\n",           time1               );
    printf("fastSin          = %6.5f (%6.3f)\n\n", time2, time2 / time1);

    time1 = benchMathFunction<10000, -360, 360, 1745>(F_cosf   ());
    time2 = benchMathFunction<10000, -360, 360, 1745>(F_fastCos());
    printf("cosf             = %6.5f\n",           time1               );
    printf("fastCos          = %6.5f (%6.3f)\n\n", time2, time2 / time1);

    time1 = benchMathFunction<10000, 0, 100, 0>(F_atan2f   ());
    time2 = benchMathFunction<10000, 0, 100, 0>(F_fastAtan2());
    printf("atan2f           = %6.5f\n",           time1               );
    printf("fastAtan2        = %6.5f (%6.3f)\n\n", time2, time2 / time1);

    std::clog << std::endl;

    /*
    ---------------------------------------
    Result on x86_64 (i5-4460; 64-Bit Mode)
    ---------------------------------------

                        Time    Factor
                       ------- --------

    sqrtf            = 0.00402
    fastSqrt         = 0.00888 ( 2.207)
    fastSqrt SIMD    = 0.00413 ( 1.027)

    1.0f / sqrtf     = 0.00774
    fastInvSqrt      = 0.00868 ( 1.122)
    fastInvSqrt SIMD = 0.00266 ( 0.344)

    sinf             = 0.02739
    fastSin          = 0.02052 ( 0.749)

    cosf             = 0.02719
    fastCos          = 0.02126 ( 0.782)

    atan2f           = 0.03134
    fastAtan2        = 0.00823 ( 0.263)


    --------------------------------------------------------
    Result on v7l (A53; BCM2709; RaspberryPi 3; 32-bit Mode)
    --------------------------------------------------------

                    Time    Factor
                   ------- --------

    sqrtf            = 0.01774
    fastSqrt         = 0.02690 ( 1.516)
    fastSqrt SIMD    = 0.01517 ( 0.855)

    1.0f / sqrtf     = 0.02775
    fastInvSqrt      = 0.02532 ( 0.913)
    fastInvSqrt SIMD = 0.01183 ( 0.426)

    sinf             = 1.72777
    fastSin          = 0.06160 ( 0.036)

    cosf             = 1.72768
    fastCos          = 0.06870 ( 0.040)

    atan2f           = 0.14729
    fastAtan2        = 0.04413 ( 0.300)
    */
}

////////////////////////////////////////////////////////////////////////////////

static void dumpTransformMatrix(const Transform& t)
{
    printf("    | %7.3f %7.3f %7.3f |\n", t.m11(),  t.m12(), t.dx());
    printf("    | %7.3f %7.3f %7.3f |\n",  t.m21(),  t.m22(), t.dy());
}

//template <typename T>
//static void bench2DTransOps()
//{
//#if defined(PT_GFX_USE_ARM_CPU)
//    const int loopCount = 2048;
//#else
//    const int loopCount = 8192;
//#endif
//
//    // Reinitialize the random number generator here, so it will produce
//    // the same sequence at the start of every benchmark
//    srand(13579);
//
//    // Do not use constants or repeating values to avoid loop unroll optimizations
//    T a[3][3];
//    T b[3][3];
//    for(int i = 0; i < 3; ++i) {
//        for(int j = 0; j < 3; ++j) {
//            a[i][j] = 2.0f * rand() / RAND_MAX - 1.0f;
//            b[i][j] = 2.0f * rand() / RAND_MAX - 1.0f;
//        }
//    }
//
//    // Correctness check
//    Transform transform;
//
//    printf("Initial value\n");
//    dumpTransformMatrix(transform);
//
//    printf("After operations\n");
//    transform.translate(10.0f, 20.0f);
//    transform.scale    ( 0.5f,  2.0f);
//    dumpTransformMatrix(transform);
//
//    volatile T x = 8.0f, y = 8.0f;
//             T xx, yy;
//    transform.transformPoint(xx, yy, x, y);
//    printf("A: Point (%6.3f, %6.3f) -> (%6.3f, %6.3f)\n", x, y, xx, yy);
//
//    T xya  [10] = { 11.0f, 12.0f, 13.0f, 24.0f, 25.0f, 16.0f, 27.0f, 28.0f, 5.0f, 5.0f };
//    T xxyya[10];
//    transform.transformPoints(xxyya, xya, 5);
//    printf("B: Point (%6.3f, %6.3f) -> (%6.3f, %6.3f)\n", xya[0], xya[1], xxyya[0], xxyya[1]);
//    printf("B: Point (%6.3f, %6.3f) -> (%6.3f, %6.3f)\n", xya[2], xya[3], xxyya[2], xxyya[3]);
//    printf("B: Point (%6.3f, %6.3f) -> (%6.3f, %6.3f)\n", xya[4], xya[5], xxyya[4], xxyya[5]);
//    printf("B: Point (%6.3f, %6.3f) -> (%6.3f, %6.3f)\n", xya[6], xya[7], xxyya[6], xxyya[7]);
//    printf("B: Point (%6.3f, %6.3f) -> (%6.3f, %6.3f)\n", xya[8], xya[9], xxyya[8], xxyya[9]);
//
//    transform.transformPoints(xya, 5);
//    printf("C: Point                  -> (%6.3f, %6.3f)\n", xya[0], xya[1]);
//    printf("C: Point                  -> (%6.3f, %6.3f)\n", xya[2], xya[3]);
//    printf("C: Point                  -> (%6.3f, %6.3f)\n", xya[4], xya[5]);
//    printf("C: Point                  -> (%6.3f, %6.3f)\n", xya[6], xya[7]);
//    printf("C: Point                  -> (%6.3f, %6.3f)\n", xya[8], xya[9]);
//
//    PointF pf[5] = { PointF(11.0f, 12.0f), PointF(13.0f, 24.0f), PointF(25.0f, 16.0f), PointF(27.0f, 28.0f), PointF(5.0f, 5.0f) };
//    transform.transformPoints(pf, 5);
//    printf("D: Point                  -> (%6.3f, %6.3f)\n", pf[0].x(), pf[0].y());
//    printf("D: Point                  -> (%6.3f, %6.3f)\n", pf[1].x(), pf[1].y());
//    printf("D: Point                  -> (%6.3f, %6.3f)\n", pf[2].x(), pf[2].y());
//    printf("D: Point                  -> (%6.3f, %6.3f)\n", pf[3].x(), pf[3].y());
//    printf("D: Point                  -> (%6.3f, %6.3f)\n", pf[4].x(), pf[4].y());
//
//    // Performance check
//    Pt::System::Clock clock;
//
//    double totalTime = 0;
//    for(int i = 0; i < loopCount ; ++i) {
//        // Reset the transformation
//        if(i % 1) transform.setRaw(a);
//        else      transform.setRaw(b);
//        // Perform benchmark
//        clock.start();
//        for(int j = 0; j < loopCount ; ++j) {
//            transform.setRaw(b);
//        }
//        totalTime += clock.stop().toUSecs();
//    }
//    totalTime /= (double) loopCount;
//    printf("Time M    = M * M    : %9.6f nS\n", totalTime / (double) loopCount * 1000.0);
//
//    totalTime = 0;
//    T dxya[loopCount * 2], sxya[loopCount * 2];
//    volatile T dmyx, dmyy;
//    for(int i = 0; i < loopCount ; ++i) {
//        // Reset the transformation
//        if(i % 1) transform.setRaw(a);
//        else      transform.setRaw(b);
//        // Reset the source vectors
//        for(int j = 0; j < loopCount * 2; j += 2) {
//            sxya[j    ] = 2.0f * rand() / RAND_MAX - 1.0f;
//            sxya[j + 1] = 2.0f * rand() / RAND_MAX - 1.0f;
//        }
//        // Perform benchmark
//        clock.start();
//        transform.transformPoints(dxya, sxya, loopCount);
//        totalTime += clock.stop().toUSecs();
//        // Copy the destination vectors to a volatile memory location
//        for(int j = 0; j < loopCount * 2; j += 2) {
//            dmyx += dxya[j    ];
//            dmyy += dxya[j + 1];
//        }
//    }
//    totalTime /= (double) loopCount;
//    printf("Time VD[] = M * VS[] : %9.6f nS\n", totalTime / (double) loopCount * 1000.0);
//
//    totalTime = 0;
//    for(int i = 0; i < loopCount ; ++i) {
//        // Reset the transformation
//        if(i % 1) transform.setRaw(a);
//        else      transform.setRaw(b);
//        // Reset the source vectors
//        for(int j = 0; j < loopCount * 2; j += 2) {
//            sxya[j    ] = 2.0f * rand() / RAND_MAX - 1.0f;
//            sxya[j + 1] = 2.0f * rand() / RAND_MAX - 1.0f;
//        }
//        // Perform benchmark
//        clock.start();
//        transform.transformPoints(sxya, loopCount);
//        totalTime += clock.stop().toUSecs();
//        // Copy the result vectors to a volatile memory location
//        for(int j = 0; j < loopCount * 2; j += 2) {
//            dmyx += sxya[j    ];
//            dmyy += sxya[j + 1];
//        }
//    }
//    totalTime /= (double) loopCount;
//    printf("Time VS[] = M * VS[] : %9.6f nS\n", totalTime / (double) loopCount * 1000.0);
//
//    totalTime = 0;
//    PointF pointsF[loopCount];
//    for(int i = 0; i < loopCount ; ++i) {
//        // Reset the transformation
//        if(i % 1) transform.setRaw(a);
//        else      transform.setRaw(b);
//        // Reset the source vectors
//        for(int j = 0; j < loopCount; ++j) {
//            pointsF[j].set( 2.0f * rand() / RAND_MAX - 1.0f, 2.0f * rand() / RAND_MAX - 1.0f );
//        }
//        // Perform benchmark
//        clock.start();
//        transform.transformPoints(pointsF, loopCount);
//        totalTime += clock.stop().toUSecs();
//        // Copy the result vectors to a volatile memory location
//        for(int j = 0; j < loopCount; ++j) {
//            dmyx += pointsF[j].x();
//            dmyy += pointsF[j].y();
//        }
//    }
//    totalTime /= (double) loopCount;
//    printf("Time VS[] = M * VS[] : %9.6f nS\n",  totalTime / (double) loopCount * 1000.0);
//
//    printf("\n");
//
//    /*
//    -----------------------------------
//    Plain x86_64 (i5-4460; 64-Bit Mode)
//    -----------------------------------
//    <float>
//    Initial value
//        |   1.000   0.000   0.000 |
//        |   0.000   1.000   0.000 |
//        |   0.000   0.000   1.000 |
//    After operations
//        |   0.500   0.000   5.000 |
//        |   0.000   2.000  40.000 |
//        |   0.000   0.000   1.000 |
//    A: Point ( 8.000,  8.000) -> ( 9.000, 56.000)
//    B: Point (11.000, 12.000) -> (10.500, 64.000)
//    B: Point (13.000, 24.000) -> (11.500, 88.000)
//    B: Point (25.000, 16.000) -> (17.500, 72.000)
//    B: Point (27.000, 28.000) -> (18.500, 96.000)
//    B: Point ( 5.000,  5.000) -> ( 7.500, 50.000)
//    C: Point                  -> (10.500, 64.000)
//    C: Point                  -> (11.500, 88.000)
//    C: Point                  -> (17.500, 72.000)
//    C: Point                  -> (18.500, 96.000)
//    C: Point                  -> ( 7.500, 50.000)
//    D: Point                  -> (10.500, 64.000)
//    D: Point                  -> (11.500, 88.000)
//    D: Point                  -> (17.500, 72.000)
//    D: Point                  -> (18.500, 96.000)
//    D: Point                  -> ( 7.500, 50.000)
//    Time M    = M * M    :  1.616523 nS
//    Time VD[] = M * VS[] :  1.496449 nS
//    Time VS[] = M * VS[] :  1.483902 nS
//    Time VS[] = M * VS[] :  3.305390 nS
//
//    <double>
//    Initial value
//        |   1.000   0.000   0.000 |
//        |   0.000   1.000   0.000 |
//        |   0.000   0.000   1.000 |
//    After operations
//        |   0.500   0.000   5.000 |
//        |   0.000   2.000  40.000 |
//        |   0.000   0.000   1.000 |
//    A: Point ( 8.000,  8.000) -> ( 9.000, 56.000)
//    B: Point (11.000, 12.000) -> (10.500, 64.000)
//    B: Point (13.000, 24.000) -> (11.500, 88.000)
//    B: Point (25.000, 16.000) -> (17.500, 72.000)
//    B: Point (27.000, 28.000) -> (18.500, 96.000)
//    B: Point ( 5.000,  5.000) -> ( 7.500, 50.000)
//    C: Point                  -> (10.500, 64.000)
//    C: Point                  -> (11.500, 88.000)
//    C: Point                  -> (17.500, 72.000)
//    C: Point                  -> (18.500, 96.000)
//    C: Point                  -> ( 7.500, 50.000)
//    D: Point                  -> (10.500, 64.000)
//    D: Point                  -> (11.500, 88.000)
//    D: Point                  -> (17.500, 72.000)
//    D: Point                  -> (18.500, 96.000)
//    D: Point                  -> ( 7.500, 50.000)
//    Time M    = M * M    :  1.483798 nS
//    Time VD[] = M * VS[] :  2.525777 nS
//    Time VS[] = M * VS[] :  2.575859 nS
//    Time VS[] = M * VS[] :  2.751425 nS
//
//    --------------------------------------------------------------
//    With Partial AVX and Auto-Vectorization (i5-4460; 64-Bit Mode)
//    --------------------------------------------------------------
//    <float>
//    Initial value
//        |   1.000   0.000   0.000 |
//        |   0.000   1.000   0.000 |
//        |   0.000   0.000   1.000 |
//    After operations
//        |   0.500   0.000   5.000 |
//        |   0.000   2.000  40.000 |
//        |   0.000   0.000   1.000 |
//    A: Point ( 8.000,  8.000) -> ( 9.000, 56.000)
//    B: Point (11.000, 12.000) -> (10.500, 64.000)
//    B: Point (13.000, 24.000) -> (11.500, 88.000)
//    B: Point (25.000, 16.000) -> (17.500, 72.000)
//    B: Point (27.000, 28.000) -> (18.500, 96.000)
//    B: Point ( 5.000,  5.000) -> ( 7.500, 50.000)
//    C: Point                  -> (10.500, 64.000)
//    C: Point                  -> (11.500, 88.000)
//    C: Point                  -> (17.500, 72.000)
//    C: Point                  -> (18.500, 96.000)
//    C: Point                  -> ( 7.500, 50.000)
//    D: Point                  -> (10.500, 64.000)
//    D: Point                  -> (11.500, 88.000)
//    D: Point                  -> (17.500, 72.000)
//    D: Point                  -> (18.500, 96.000)
//    D: Point                  -> ( 7.500, 50.000)
//    Time M    = M * M    :  1.548439 nS
//    Time VD[] = M * VS[] :  0.755504 nS
//    Time VS[] = M * VS[] :  0.747219 nS
//    Time VS[] = M * VS[] :  2.614081 nS
//
//    <double>
//    Initial value
//        |   1.000   0.000   0.000 |
//        |   0.000   1.000   0.000 |
//        |   0.000   0.000   1.000 |
//    After operations
//        |   0.500   0.000   5.000 |
//        |   0.000   2.000  40.000 |
//        |   0.000   0.000   1.000 |
//    A: Point ( 8.000,  8.000) -> ( 9.000, 56.000)
//    B: Point (11.000, 12.000) -> (10.500, 64.000)
//    B: Point (13.000, 24.000) -> (11.500, 88.000)
//    B: Point (25.000, 16.000) -> (17.500, 72.000)
//    B: Point (27.000, 28.000) -> (18.500, 96.000)
//    B: Point ( 5.000,  5.000) -> ( 7.500, 50.000)
//    C: Point                  -> (10.500, 64.000)
//    C: Point                  -> (11.500, 88.000)
//    C: Point                  -> (17.500, 72.000)
//    C: Point                  -> (18.500, 96.000)
//    C: Point                  -> ( 7.500, 50.000)
//    D: Point                  -> (10.500, 64.000)
//    D: Point                  -> (11.500, 88.000)
//    D: Point                  -> (17.500, 72.000)
//    D: Point                  -> (18.500, 96.000)
//    D: Point                  -> ( 7.500, 50.000)
//    Time M    = M * M    :  1.485005 nS
//    Time VD[] = M * VS[] :  1.269802 nS
//    Time VS[] = M * VS[] :  1.320288 nS
//    Time VS[] = M * VS[] :  1.324102 nS
//    */
//
//    /*
//    ---------------------------------------------------------
//    Plain Arm (v7l; A53; BCM2709; RaspberryPi 3; 32-bit Mode)
//    ---------------------------------------------------------
//    <float>
//    Initial value
//        |   1.000   0.000   0.000 |
//        |   0.000   1.000   0.000 |
//        |   0.000   0.000   1.000 |
//    After operations
//        |   0.500   0.000   5.000 |
//        |   0.000   2.000  40.000 |
//        |   0.000   0.000   1.000 |
//    A: Point ( 8.000,  8.000) -> ( 9.000, 56.000)
//    B: Point (11.000, 12.000) -> (10.500, 64.000)
//    B: Point (13.000, 24.000) -> (11.500, 88.000)
//    B: Point (25.000, 16.000) -> (17.500, 72.000)
//    B: Point (27.000, 28.000) -> (18.500, 96.000)
//    B: Point ( 5.000,  5.000) -> ( 7.500, 50.000)
//    C: Point                  -> (10.500, 64.000)
//    C: Point                  -> (11.500, 88.000)
//    C: Point                  -> (17.500, 72.000)
//    C: Point                  -> (18.500, 96.000)
//    C: Point                  -> ( 7.500, 50.000)
//    D: Point                  -> (10.500, 64.000)
//    D: Point                  -> (11.500, 88.000)
//    D: Point                  -> (17.500, 72.000)
//    D: Point                  -> (18.500, 96.000)
//    D: Point                  -> ( 7.500, 50.000)
//    Time M    = M * M    :  9.586573 nS
//    Time VD[] = M * VS[] : 18.969774 nS
//    Time VS[] = M * VS[] : 19.458532 nS
//    Time VS[] = M * VS[] : 36.407232 nS
//
//    <double>
//    Initial value
//        |   1.000   0.000   0.000 |
//        |   0.000   1.000   0.000 |
//        |   0.000   0.000   1.000 |
//    After operations
//        |   0.500   0.000   5.000 |
//        |   0.000   2.000  40.000 |
//        |   0.000   0.000   1.000 |
//    A: Point ( 8.000,  8.000) -> ( 9.000, 56.000)
//    B: Point (11.000, 12.000) -> (10.500, 64.000)
//    B: Point (13.000, 24.000) -> (11.500, 88.000)
//    B: Point (25.000, 16.000) -> (17.500, 72.000)
//    B: Point (27.000, 28.000) -> (18.500, 96.000)
//    B: Point ( 5.000,  5.000) -> ( 7.500, 50.000)
//    C: Point                  -> (10.500, 64.000)
//    C: Point                  -> (11.500, 88.000)
//    C: Point                  -> (17.500, 72.000)
//    C: Point                  -> (18.500, 96.000)
//    C: Point                  -> ( 7.500, 50.000)
//    D: Point                  -> (10.500, 64.000)
//    D: Point                  -> (11.500, 88.000)
//    D: Point                  -> (17.500, 72.000)
//    D: Point                  -> (18.500, 96.000)
//    D: Point                  -> ( 7.500, 50.000)
//    Time M    = M * M    :  9.413958 nS
//    Time VD[] = M * VS[] : 19.264221 nS
//    Time VS[] = M * VS[] : 19.632339 nS
//    Time VS[] = M * VS[] : 21.365404 nS
//
//    ----------------------------------------------------------------------------------------
//    With Partial NEON and Auto-Vectorization (v7l; A53; BCM2709; RaspberryPi 3; 32-bit Mode)
//    ----------------------------------------------------------------------------------------
//    <float>
//    Initial value
//        |   1.000   0.000   0.000 |
//        |   0.000   1.000   0.000 |
//        |   0.000   0.000   1.000 |
//    After operations
//        |   0.500   0.000   5.000 |
//        |   0.000   2.000  40.000 |
//        |   0.000   0.000   1.000 |
//    A: Point ( 8.000,  8.000) -> ( 9.000, 56.000)
//    B: Point (11.000, 12.000) -> (10.500, 64.000)
//    B: Point (13.000, 24.000) -> (11.500, 88.000)
//    B: Point (25.000, 16.000) -> (17.500, 72.000)
//    B: Point (27.000, 28.000) -> (18.500, 96.000)
//    B: Point ( 5.000,  5.000) -> ( 7.500, 50.000)
//    C: Point                  -> (10.500, 64.000)
//    C: Point                  -> (11.500, 88.000)
//    C: Point                  -> (17.500, 72.000)
//    C: Point                  -> (18.500, 96.000)
//    C: Point                  -> ( 7.500, 50.000)
//    D: Point                  -> (10.500, 64.000)
//    D: Point                  -> (11.500, 88.000)
//    D: Point                  -> (17.500, 72.000)
//    D: Point                  -> (18.500, 96.000)
//    D: Point                  -> ( 7.500, 50.000)
//    Time M    = M * M    :  8.731365 nS
//    Time VD[] = M * VS[] : 16.457558 nS
//    Time VS[] = M * VS[] : 16.906023 nS
//    Time VS[] = M * VS[] : 33.613920 nS
//
//    <double>
//    Initial value
//        |   1.000   0.000   0.000 |
//        |   0.000   1.000   0.000 |
//        |   0.000   0.000   1.000 |
//    After operations
//        |   0.500   0.000   5.000 |
//        |   0.000   2.000  40.000 |
//        |   0.000   0.000   1.000 |
//    A: Point ( 8.000,  8.000) -> ( 9.000, 56.000)
//    B: Point (11.000, 12.000) -> (10.500, 64.000)
//    B: Point (13.000, 24.000) -> (11.500, 88.000)
//    B: Point (25.000, 16.000) -> (17.500, 72.000)
//    B: Point (27.000, 28.000) -> (18.500, 96.000)
//    B: Point ( 5.000,  5.000) -> ( 7.500, 50.000)
//    C: Point                  -> (10.500, 64.000)
//    C: Point                  -> (11.500, 88.000)
//    C: Point                  -> (17.500, 72.000)
//    C: Point                  -> (18.500, 96.000)
//    C: Point                  -> ( 7.500, 50.000)
//    D: Point                  -> (10.500, 64.000)
//    D: Point                  -> (11.500, 88.000)
//    D: Point                  -> (17.500, 72.000)
//    D: Point                  -> (18.500, 96.000)
//    D: Point                  -> ( 7.500, 50.000)
//    Time M    = M * M    :  8.566141 nS
//    Time VD[] = M * VS[] : 19.327879 nS
//    Time VS[] = M * VS[] : 19.709587 nS
//    Time VS[] = M * VS[] : 21.282434 nS
//    */
//}
