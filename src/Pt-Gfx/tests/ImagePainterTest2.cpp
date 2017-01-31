// ./jam.sh configure --with-hmi -sGUI=linux-fb --with-rasterizer2

#include <Pt/Gfx/ImagePainter2.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <unistd.h>

using namespace Pt::Gfx;

static void sdlPreviewRGB888Buffer(const char* title, const uint8_t* argb8888Buff, int sizeX, int sizeY)
{
    // Initialise SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) return;

    // Create window, renderer, and texture objects
    SDL_Window*   sdlWindow     = SDL_CreateWindow  (title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sizeX, sizeY, 0);
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

static void testLines(const char* title, Image& image, ImagePainter2& painter)
{
    resetImage(image);

#if 1
    painter.setClip( RectF (20, image.width() - 20, 20, image.height() - 20) );
    painter.setPen( Color::fromRgb8(0, 255, 0, 175) );
    painter.drawLine( PointF(  0,   0), PointF(799, 599) );
    painter.setClip( RectF (0, image.width() - 1, 0, image.height() - 1) );
#endif

    painter.setPen( Color::fromRgb8(255, 0, 0, 175) );

    painter.drawLine( PointF(  0,   0), PointF(799,   0) );
    painter.drawLine( PointF(  0, 599), PointF(799, 599) );
    painter.drawLine( PointF(  0,   0), PointF(  0, 599) );
    painter.drawLine( PointF(799,   0), PointF(799, 599) );

    painter.setPen( Color::fromRgb8(255, 255, 255, 175) );

    painter.drawLine( PointF( 10,  10), PointF(100,  50) );
    painter.drawLine( PointF( 10, 150), PointF(100, 110) );

    painter.drawLine( PointF(100,  10), PointF(110,  20) );

    painter.setPen( Pen(Color::fromRgb8( 63,  63,  63, 175)) ); painter.drawLine( PointF(500-50, 500), PointF(200-50, 200) );
    painter.setPen( Pen(Color::fromRgb8(127, 127, 127, 175)) ); painter.drawLine( PointF(500   , 500), PointF(200   , 200) );
    painter.setPen( Pen(Color::fromRgb8(255, 255, 255, 175)) ); painter.drawLine( PointF(500+50, 500), PointF(200+50, 200) );
                                                                painter.drawLine( PointF(300   , 200), PointF(700   , 100) );

    painter.drawLine( PointF(770,  11), PointF(770, 500) );
    painter.drawLine( PointF(780,  11), PointF(782, 500) );

    painter.drawLine( PointF( 10, 540), PointF(781, 540) );
    painter.drawLine( PointF( 10, 550), PointF(781, 551) );

    painter.setPen( Pen(Color::fromRgb8(0, 255, 255, 175)) ); painter.drawText( PointF(100, 100), "Hello world!" );
    painter.setPen( Pen(Color::fromRgb8(0, 255, 255, 255)) ); painter.drawText( PointF(100, 150), "Hello world!" );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height());
}

static void testFillPolygon(const char* title, Image& image, ImagePainter2& painter)
{
    resetImage(image);

    painter.setBrush( Color::fromRgb8(255, 255, 255, 175) );

    const PointF poly1[5] = { PointF( 10, 10), PointF( 50, 20), PointF( 90, 50), PointF( 70, 70), PointF( 30, 20) };
    painter.fillPolygon(poly1, sizeof(poly1) / sizeof(poly1[0]));

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height());
}

int main(int argc, char* args[])
{
    Image         image( ImageFormat::argb32(), Size(800, 600) );
    ImagePainter2 painter(image);

    painter.setFontDir( Pt::System::Path("../src/Pt-Gfx/fonts") );
    painter.setFont( Pt::Gfx::Font("DejaVu Serif", 24, Pt::Gfx::Font::BoldItalic) );

    if(0) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testLines("Test Lines - CompositionMode::SourceCopy", image, painter);
    }

    if(0) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testLines("Test Lines - CompositionMode::SourceOver", image, painter);
    }

    if(1) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testFillPolygon("Test Fill Polygons - CompositionMode::SourceCopy", image, painter);
    }

    return 0;
}

