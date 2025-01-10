
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/ImageSurface.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/BlockScale.h>
#include <Pt/System/Logger.h>
#include <Pt/System/Clock.h>

using namespace Pt::Gfx;

void PaintBenchmark()
{
    try
    {
        for(int n = 0; n < 10 ; ++n)
        {
            ImageSurface surface;

            Pt::System::Clock clock;
            clock.start();

            Painter painter(surface);
            painter.setCompositionMode(CompositionMode::SourceOver);

            Brush brush( Color(1, 1, 1) );
            painter.setBrush(brush);

            Pen pen( Color(1, 0, 0) );
            painter.setPen(pen);

            for(int n = 0; n < 10; ++n)
            {
                painter.fillRect(RectF(PointF(0,0), SizeF(1280,800)));

               for(int i = 0; i < 10; i++)
                    painter.fillRect( RectF(PointF(0,0), SizeF(100,100)));

                for(int x = 0; x < 50; x++)
                    painter.drawLine( PointF(x, 0), PointF(x, 200) );

                for(int y = 0; y < 20; y++)
                    painter.drawLine( PointF(0, y), PointF(200, y) );
            }

            std::clog << "drawing: " << clock.stop().toUSecs() << std::endl;

            std::vector<Pt::uint32_t> frameBuffer(1280 * 800);
            clock.start();

            for(int n = 0; n < 10; ++n)
                std::memcpy( &frameBuffer[0], surface.image().data(), frameBuffer.size() );

            std::clog << "copy: " << clock.stop().toUSecs() << std::endl;
            std::clog << std::endl;
        }
    }
    catch(const std::exception& ex)
    {
        std::clog << "ERROR: " << ex.what() << std::endl;
    }
}

void BlockScale()
{
    try
    {
        for(int n = 0; n < 10 ; ++n)
        {
            Pt::Gfx::Image image(ImageFormat::argb32(), 1000, 1000 );
            Pt::Gfx::Image image2(ImageFormat::argb32(), 900, 900 );

            Pt::System::Clock clock;
            clock.start();
            Pt::Gfx::blockScale(image.begin(), image.width(), image.height(),
                                image2.begin(), image2.width(), image2.height());

            std::clog << "image scale: " << clock.stop().toUSecs() << std::endl;
        }

        std::clog << std::endl;

        for(int n = 0; n < 10 ; ++n)
        {
            std::vector<Pt::uint32_t> image(1000 * 1000);
            std::vector<Pt::uint32_t> image2(900 * 900);

            std::vector<Pt::uint16_t> frameBuffer( image2.size() );

            Pt::System::Clock clock;
            clock.start();
            Pt::Gfx::blockScale(image.begin(), 1000, 1000,
                                image2.begin(), 900, 900);

            for(unsigned y = 0; y < 900; ++y)
            {
                for(unsigned x = 0; x < 900; ++x)
                {
                    unsigned n = (y * 900) + x;
                    Pt::uint8_t* from = reinterpret_cast<Pt::uint8_t*>( &image2[n] );

                    Pt::uint8_t r = from[2];
                    Pt::uint8_t g = from[1];
                    Pt::uint8_t b = from[0];

                    Pt::uint16_t* to = reinterpret_cast<Pt::uint16_t*>( &frameBuffer[n] );

                    *to  = (Pt::uint16_t) (r / 8);
                    *to  |= ((Pt::uint16_t) (g / 4)) << 5;
                    *to  |= ((Pt::uint16_t) (b / 8)) << 11;
                }
            }

            std::clog << "vector scale: " << clock.stop().toUSecs() << std::endl;
        }
    }
    catch(const std::exception& ex)
    {
        std::clog << "ERROR: " << ex.what() << std::endl;
    }
}


int main(int argc, char* args[])
{
    BlockScale();

    std::clog << std::endl;

    PaintBenchmark();

    return 0;
}

