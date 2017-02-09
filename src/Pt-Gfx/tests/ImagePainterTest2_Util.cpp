static void sdlPreviewRGB888Buffer(const std::string& title, const uint8_t* argb8888Buff, int sizeX, int sizeY, bool saveImageAsBMP)
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

    // Save the image as a BMP file
    if(saveImageAsBMP) {
        std::string eraseStr = " - ImagePainter2";
        std::string fileName = std::string("IPT2 - ") + title + ".bmp";
        fileName.erase(fileName.find(eraseStr), eraseStr.length());
         SDL_Surface* sdlSurface = SDL_CreateRGBSurfaceFrom((void*) argb8888Buff, sizeX, sizeY, 32, sizeX * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_SaveBMP(sdlSurface, fileName.c_str());
        SDL_FreeSurface(sdlSurface);
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
