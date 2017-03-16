// Uncomment this to benchmark SDL
// #define BENCHMARK_SDL

int writePNG(const char* filename, int width, int height, const uint8_t* argb8888Buff)
{
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

static void sdlPreviewRGB888Buffer(const std::string& title, const uint8_t* argb8888Buff, int sizeX, int sizeY, bool saveImageAsPNG)
{
    // Save the image as a PNG file
    if(saveImageAsPNG) {
        std::string eraseStr = " - ImagePainter2";
        std::string fileName = std::string("../src/Pt-Gfx/TEMPORARY/IPT2 - ") + title + ".png";
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
}

static void resetImage(Image& image)
{
    const Size origSize = image.size();
    image.reset(image.format(), Size(0, 0));
    image.reset(image.format(), origSize);
}

#ifdef __GNUC__
#include <cxxabi.h>
#endif

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
#ifdef __GNUC__
    int s;
    char* demangled = abi::__cxa_demangle(className.c_str(), 0, 0, &s);
    className = demangled;
    free(demangled);
#endif

    // Call the overload function
    return formatCaption(className, cm, funcName);
}

////////////////////////////////////////////////////////////////////////////////

static volatile float dummyMathValue;

template<int LOOP_COUNT, int MIN, int MAX, int DIV, typename F>
static double benchMarkMathFunction(F f)
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

struct F_sqrtf       { float operator() (float x) { return ::sqrtf (x);                              } };
struct F_fastSqrt    { float operator() (float x) { return Pt::Gfx::Math::fastSqrt_impl(x);          } };

struct F_isqrtf      { float operator() (float x) { return 1.0f / ::sqrtf(x);                        } };
struct F_fastInvSqrt { float operator() (float x) { return Pt::Gfx::Math::fastInvSqrt_impl(x);       } };

struct F_sinf        { float operator() (float x) { return ::sinf (x);                               } };
struct F_fastSin     { float operator() (float x) { return Pt::Gfx::Math::fastSin_impl(x);           } };

struct F_cosf        { float operator() (float x) { return ::cosf (x);                               } };
struct F_fastCos     { float operator() (float x) { return Pt::Gfx::Math::fastCos_impl(x);           } };

struct F_atan2f      { float operator() (float x) { return ::atan2f (100.0f, x);                     } };
struct F_fastAtan2   { float operator() (float x) { return Pt::Gfx::Math::fastAtan2_impl(100.0f, x); } };

static void benchMarkMathFunctions()
{
    double time1, time2;

    std::clog << "                Time    Factor " << std::endl;
    std::clog << "               ------- --------" << std::endl << std::endl;

    time1 = benchMarkMathFunction<100000, 0, 100, 0>(F_sqrtf   ());
    time2 = benchMarkMathFunction<100000, 0, 100, 0>(F_fastSqrt());
    printf("sqrtf        = %6.5f\n", time1);
    printf("fastSqrt     = %6.5f (%6.3f)\n\n", time2, time2 / time1);

    time1 = benchMarkMathFunction<100000, 0, 100, 0>(F_isqrtf   ());
    time2 = benchMarkMathFunction<100000, 0, 100, 0>(F_fastInvSqrt());
    printf("1.0f / sqrtf = %6.5f\n", time1);
    printf("fastInvSqrt  = %6.5f (%6.3f)\n\n", time2, time2 / time1);

    time1 = benchMarkMathFunction<10000, -360, 360, 1745>(F_sinf   ());
    time2 = benchMarkMathFunction<10000, -360, 360, 1745>(F_fastSin());
    printf("sinf         = %6.5f\n", time1);
    printf("fastSin      = %6.5f (%6.3f)\n\n", time2, time2 / time1);

    time1 = benchMarkMathFunction<10000, -360, 360, 1745>(F_cosf   ());
    time2 = benchMarkMathFunction<10000, -360, 360, 1745>(F_fastCos());
    printf("cosf         = %6.5f\n", time1);
    printf("fastCos      = %6.5f (%6.3f)\n\n", time2, time2 / time1);

    time1 = benchMarkMathFunction<10000, 0, 100, 0>(F_atan2f   ());
    time2 = benchMarkMathFunction<10000, 0, 100, 0>(F_fastAtan2());
    printf("atan2f       = %6.5f\n", time1);
    printf("fastAtan2    = %6.5f (%6.3f)\n\n", time2, time2 / time1);

    std::clog << std::endl;

    /*
    ---------------------------------------
    Result on x86_64 (i5-4460; 64-Bit Mode)
    ---------------------------------------

                    Time    Factor
                   ------- --------

    sqrtf        = 0.00389
    fastSqrt     = 0.01103 ( 2.837)

    1.0f / sqrtf = 0.00864
    fastInvSqrt  = 0.01009 ( 1.168)

    sinf         = 0.02746
    fastSin      = 0.01495 ( 0.544)

    cosf         = 0.02766
    fastCos      = 0.01967 ( 0.711)

    atan2f       = 0.03156
    fastAtan2    = 0.00818 ( 0.259)


    --------------------------------------------------------
    Result on v7l (A53; BCM2709; RaspberryPi 3; 32-bit Mode)
    --------------------------------------------------------

                    Time    Factor
                   ------- --------

    sqrtf        = 0.01780
    fastSqrt     = 0.02708 ( 1.522)

    1.0f / sqrtf = 0.02789
    fastInvSqrt  = 0.02522 ( 0.904)

    sinf         = 1.72843
    fastSin      = 0.06808 ( 0.039)

    cosf         = 1.72667
    fastCos      = 0.08795 ( 0.051)

    atan2f       = 0.14623
    fastAtan2    = 0.05048 ( 0.345)
    */
}
