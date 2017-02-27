static void sdlPreviewRGB888Buffer(const std::string& title, const uint8_t* argb8888Buff, int sizeX, int sizeY, bool saveImageAsPNG)
{
    // Initialise SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) return;

    // Create window, renderer, and texture objects
    SDL_Window*   sdlWindow     = SDL_CreateWindow  (title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sizeX, sizeY, 0);
    SDL_Renderer* sdlRenderer   = SDL_CreateRenderer(sdlWindow, -1, 0);
    SDL_Texture*  sdlTexture    = SDL_CreateTexture (sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, sizeX, sizeY);

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
    SDL_UpdateTexture(sdlTexture, 0, argb8888Buff, sizeX * 4);
    SDL_RenderClear  (sdlRenderer);
    SDL_RenderCopy   (sdlRenderer, sdlTexture,    0, 0);

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
        SDL_RenderPresent(sdlRenderer);
        usleep(10000);
    }

    // Save the image as a PNG file
    if(saveImageAsPNG) {
        std::string eraseStr = " - ImagePainter2";
        std::string fileName = std::string("../src/Pt-Gfx/TEMPORARY/IPT2 - ") + title + ".png";
        fileName.erase(fileName.find(eraseStr), eraseStr.length());
        SDL_Surface* imageS = SDL_CreateRGBSurfaceFrom((void*) argb8888Buff, sizeX, sizeY, 32, sizeX * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000);
        SDL_Surface* imageD = SDL_PNGFormatAlpha(imageS);
        SDL_SavePNG(imageD, fileName.c_str());
        SDL_FreeSurface(imageS);
        SDL_FreeSurface(imageD);
    }

    // Done
    SDL_DestroyTexture (sdlTexture   );
    SDL_DestroyRenderer(sdlRenderer  );
    SDL_DestroyWindow  (sdlWindow    );
    SDL_Quit           (             );
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

inline float fastSqrt(float x)
{
    // https://en.wikipedia.org/wiki/Methods_of_computing_square_roots

    union {
        float       f;
        Pt::int32_t i;
    } u;

    u.f = x;
    u.i = (1 << 29) + (u.i >> 1) - (1 << 22) - 0x0004C000;
    u.f = (u.f + x / u.f) * 0.5;

    return u.f;
}

inline float fastInvSqrt(float x)
{
    // https://en.wikipedia.org/wiki/Fast_inverse_square_root

    const float x2 = x * 0.5f;

    union {
        float       f;
        Pt::int32_t i;
    } u;

    u.f = x;
    u.i = 0x5F3759DF - ( u.i >> 1 );
    u.f = u.f * ( 1.5f - ( x2 * u.f * u.f ) );

    return u.f;
}

inline float fastSin(float x)
{
    if (x > Pt::Pi) x -= Pt::PiDouble;

    const float b =  4 / Pt::Pi;
    const float c = -4 / Pt::PiSqr;
    const float p = 0.225;
    const float y = b * x + c * x * ::fabs(x);

    return p * (y * ::fabs(y) - y) + y;
}

inline float fastCos(float x)
{
    x += Pt::PiHalf;
    if(x > Pt::PiDouble) x -= Pt::PiDouble;

    return fastSin(x);
}

inline float fastAtan2(float y, float x)
{
    // From https://gist.github.com/volkansalma/2972237
    // Original code by Volkan SALMA, 2012

    if(x == 0.0f) {
        if(y >  0.0f) return Pt::PiHalf;
        if(y == 0.0f) return 0.0f;
        return -Pt::PiHalf;
    }

    const float z = y / x;
          float atan;

    if(fabs(z) < 1.0f) {
        atan = z / (1.0f + 0.28f * z * z);
        if(x < 0.0f) {
            if(y < 0.0f) return atan - Pt::Pi;
            return atan + Pt::Pi;
        }
    }

    else {
        atan = Pt::PiHalf - z / (z * z + 0.28f);
        if(y < 0.0f) return atan - Pt::Pi;
    }

    return atan;
}

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

struct F_sqrtf       { float operator() (float x) { return ::sqrtf (x);         } };
struct F_fastSqrt    { float operator() (float x) { return fastSqrt(x);         } };

struct F_isqrtf      { float operator() (float x) { return 1.0f / ::sqrtf(x);   } };
struct F_fastInvSqrt { float operator() (float x) { return fastInvSqrt(x);      } };

struct F_sinf        { float operator() (float x) { return ::sinf (x);          } };
struct F_fastSin     { float operator() (float x) { return fastSin(x);          } };

struct F_cosf        { float operator() (float x) { return ::cosf (x);          } };
struct F_fastCos     { float operator() (float x) { return fastCos(x);          } };

struct F_atan2f      { float operator() (float x) { return ::atan2f (100.0f, x); } };
struct F_fastAtan2   { float operator() (float x) { return fastAtan2(100.0f, x); } };

static void benchMarkMathFunctions()
{
    double time1, time2;

    std::clog << "               (Time) (Factor)" << std::endl;
    std::clog << "               ------ --------" << std::endl << std::endl;

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
    Result on x86_64 (i5-4460; 64-Bit Mode)
    ---------------------------------------

                   (Time) (Factor)
                   ------ --------

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


    Result on v7l (A53; BCM2709; RaspberryPi 3; 32-bit Mode)
    --------------------------------------------------------
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
