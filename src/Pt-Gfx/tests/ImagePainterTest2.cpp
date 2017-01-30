// --with-hmi -sGUI=linux-fb

#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/ImagePainter2.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/BlockScale.h>
#include <Pt/System/Logger.h>
#include <Pt/System/Clock.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <unistd.h>

using namespace Pt::Gfx;

void sdlPreviewRGB888Buffer(const uint8_t* argb8888Buff, int sizeX, int sizeY)
{
    // Initialise SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) return;

    // Create window, renderer, and texture objects
    SDL_Window*   sdlWindow     = SDL_CreateWindow  ("ImagePainterTest2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sizeX, sizeY, 0);
    SDL_Renderer* sdlRenderer   = SDL_CreateRenderer(sdlWindow, -1, 0);
    SDL_Texture*  sdlTexture    = SDL_CreateTexture (sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, sizeX, sizeY);

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

    // Done
    SDL_DestroyTexture (sdlTexture   );
    SDL_DestroyRenderer(sdlRenderer  );
    SDL_DestroyWindow  (sdlWindow    );
    SDL_Quit           (             );
}

int main(int argc, char* args[])
{
    Image image( ImageFormat::argb32(), Size(800, 600) );

    ImagePainter2 painter(image);
    painter.setCompositionMode(CompositionMode::SourceOver);

    Brush brush( Color(1, 1, 1) );
    painter.setBrush(brush);

    Pen pen( Color(1, 0, 0) );
    painter.setPen(pen);

    painter.drawLine( PointF(0, 0), PointF(200, 200) );

    sdlPreviewRGB888Buffer(image.data(), image.width(), image.height());

    return 0;
}

