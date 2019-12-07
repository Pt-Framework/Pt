// Uncomment this to benchmark SDL
// #define BENCHMARK_SDL

static int writePNG(const char* filename, int width, int height, const Pt::uint8_t* argb8888Buff)
{
#if defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || defined(_M_ARMT) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM) || defined(__arm)
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
    // !!! DISABLE PNG GENERATION !!!
    saveImageAsPNG = true;

    // Save the image as a PNG file
    if(saveImageAsPNG) {
        std::string eraseStr = " - ImagePainter2";
        std::string fileName = std::string("IPT2 - ") + title + ".png";
        fileName.erase(fileName.find(eraseStr), eraseStr.length());

        Pt::System::Path pp = buildDir;
        pp /= Pt::System::Path::updir();
        pp /= "src";
        pp /= "Pt-Gfx";
        pp /= "TEMPORARY";
        pp /= fileName.c_str();

        if(writePNG(pp.toLocal().c_str(), sizeX, sizeY, argb8888Buff) < 0) return;
    }

#if defined(WITH_EXPERIMENTAL_GFX)

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
