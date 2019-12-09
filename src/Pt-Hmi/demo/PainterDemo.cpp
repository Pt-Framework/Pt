
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/TabView.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Label.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/ImagePainter2.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/System/Logger.h>

class LineStylesWidget : public Pt::Hmi::Control
{
    public:
        LineStylesWidget()
        {}

    protected:
        virtual void onPaint(Pt::Hmi::PaintSurface& surface,
                             const Pt::Gfx::RectF& rect)
        {
            using namespace Pt::Gfx;

            int imageWidth = 200, imageHeight = 440;
            RectF imageRect = RectF( PointF(0, 0), SizeF(imageWidth, imageHeight) );
            Color background = Color::fromRgb8(0, 0, 0);

            Pt::Hmi::Painter painter(surface);
            painter.setClip(rect);
            painter.setBrush(background);
            painter.fillRect(imageRect);

            Image image1( painter.format(), Size(imageWidth, imageHeight) );
            ImagePainter imagePainter(image1);
            imagePainter.setBrush(background);
            imagePainter.fillRect(imageRect);

            Image image2( painter.format(), Size(imageWidth, imageHeight) );
            ImagePainter2 imagePainter2(image2);
            imagePainter2.setAntiAliasing(true);
            imagePainter2.setBrush(background);
            imagePainter2.fillRect(imageRect);

            drawLinesLines(painter, "Native Painter");
            drawLinesLines(imagePainter, "ImagePainter");
            drawLinesLines(imagePainter2, "ImagePainter2");

            painter.drawImage(PointF(210, 0), image1);
            painter.drawImage(PointF(420, 0), image2);
        }

        void drawLinesLines(Pt::Gfx::Painter& painter, const Pt::String& text)
        {
            using namespace Pt::Gfx;

            Color red         = Color::fromRgb8(255,   0,   0);
            Color lightPurple = Color::fromRgb8(164, 100, 255);
            Color lightBlue   = Color::fromRgb8(100, 100, 255);

            painter.setPen( lightPurple );
            painter.setFont( Font("", 12) );
            painter.drawText( PointF(20, 22), text);

            painter.setFont( Font("", 10) );

            int y = 40;

//#define USER_STYLE

#ifdef USER_STYLE
            std::vector<Pt::uint8_t> userStyle;
            userStyle.resize(4);
            userStyle[0] = 1;
            userStyle[1] = 1;
            userStyle[2] = 3;
            userStyle[3] = 1;
#endif

            if(1) {
                painter.setPen( lightBlue );
                painter.drawText( PointF(20, y + 10), "ButtCap");
                y += 20;

#ifdef USER_STYLE

                Pen pen1Solid(red, 1, userStyle, Pen::ButtCap);
                Pen pen2Solid(red, 4, userStyle, Pen::ButtCap);
                Pen pen3Solid(red, 9, userStyle, Pen::ButtCap);
#else
                Pen pen1Solid(red, 1, Pen::Solid, Pen::ButtCap);
                Pen pen2Solid(red, 4, Pen::Solid, Pen::ButtCap);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::ButtCap);
#endif
                painter.setPen(pen1Solid);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;
                painter.setPen(pen2Solid);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;
                painter.setPen(pen3Solid);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;

                Pen pen1Dot(red, 1, Pen::Dot, Pen::ButtCap);
                Pen pen2Dot(red, 4, Pen::Dot, Pen::ButtCap);
                Pen pen3Dot(red, 9, Pen::Dot, Pen::ButtCap);
                painter.setPen(pen1Dot);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;
                painter.setPen(pen2Dot);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;
                painter.setPen(pen3Dot);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;

                Pen pen1Dash(red, 1, Pen::Dash, Pen::ButtCap);
                Pen pen2Dash(red, 4, Pen::Dash, Pen::ButtCap);
                Pen pen3Dash(red, 9, Pen::Dash, Pen::ButtCap);
                painter.setPen(pen1Dash);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;
                painter.setPen(pen2Dash);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;
                painter.setPen(pen3Dash);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;

                y += 5;
            }

            if(1) {
                painter.setPen( lightBlue );
                painter.drawText( PointF(20, y +10), "SquareCap");
                y += 20;

#ifdef USER_STYLE
                Pen pen2Solid(red, 4, userStyle, Pen::SquareCap);
                Pen pen3Solid(red, 9, userStyle, Pen::SquareCap);
#else
                Pen pen2Solid(red, 4, Pen::Solid, Pen::SquareCap);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::SquareCap);
#endif
                painter.setPen(pen2Solid);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;
                painter.setPen(pen3Solid);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;

                Pen pen2Dot(red, 4, Pen::Dot, Pen::SquareCap);
                Pen pen3Dot(red, 9, Pen::Dot, Pen::SquareCap);
                painter.setPen(pen2Dot);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;
                painter.setPen(pen3Dot);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;

                Pen pen2Dash(red, 4, Pen::Dash, Pen::SquareCap);
                Pen pen3Dash(red, 9, Pen::Dash, Pen::SquareCap);
                painter.setPen(pen2Dash);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;
                painter.setPen(pen3Dash);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;

                y += 5;
            }

            if(1) {
                painter.setPen( lightBlue );
                painter.drawText( PointF(20, y +10), "RoundCap");
                y += 20;

#ifdef USER_STYLE
                Pen pen2Solid(red, 4, userStyle, Pen::RoundCap);
                Pen pen3Solid(red, 9, userStyle, Pen::RoundCap);
#else
                Pen pen2Solid(red, 4, Pen::Solid, Pen::RoundCap);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::RoundCap);
#endif
                painter.setPen(pen2Solid);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;
                painter.setPen(pen3Solid);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;

                Pen pen2Dot(red, 4, Pen::Dot, Pen::RoundCap);
                Pen pen3Dot(red, 9, Pen::Dot, Pen::RoundCap);
                painter.setPen(pen2Dot);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;
                painter.setPen(pen3Dot);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;

                Pen pen2Dash(red, 4, Pen::Dash, Pen::RoundCap);
                Pen pen3Dash(red, 9, Pen::Dash, Pen::RoundCap);
                painter.setPen(pen2Dash);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;
                painter.setPen(pen3Dash);
                painter.drawLine( PointF(10, y),
                                  PointF(180, y) ); y += 15;

                y += 5;
            }
#if 0
            ///////////////////////////////////////////////////

            Pen pen1DoubleDot(red, 1, Pen::DoubleDot);
            Pen pen2DoubleDot(red, 4, Pen::DoubleDot);
            Pen pen3DoubleDot(red, 9, Pen::DoubleDot);
            painter.setPen(pen1DoubleDot);
            painter.drawLine( PointF(10, 220+20),
                              PointF(180, 220+20) );
            painter.setPen(pen2DoubleDot);
            painter.drawLine( PointF(10, 235+20),
                              PointF(180, 235+20) );
            painter.setPen(pen3DoubleDot);
            painter.drawLine( PointF(10, 250+20),
                              PointF(180, 250+20) );

            Pen pen1DoubleDash(red, 1, Pen::DoubleDash);
            Pen pen2DoubleDash(red, 4, Pen::DoubleDash);
            Pen pen3DoubleDash(red, 9, Pen::DoubleDash);
            painter.setPen(pen1DoubleDash);
            painter.drawLine( PointF(10, 280+20),
                              PointF(180, 280+20) );
            painter.setPen(pen2DoubleDash);
            painter.drawLine( PointF(10, 295+20),
                              PointF(180, 295+20) );
            painter.setPen(pen3DoubleDash);
            painter.drawLine( PointF(10, 310+20),
                              PointF(180, 310+20) );

            Pen pen1DotDash(red, 1, Pen::DotDash);
            Pen pen2DotDash(red, 4, Pen::DotDash);
            Pen pen3DotDash(red, 9, Pen::DotDash);
            painter.setPen(pen1DotDash);
            painter.drawLine( PointF(10, 340+20),
                              PointF(180, 340+20) );
            painter.setPen(pen2DotDash);
            painter.drawLine( PointF(10, 355+20),
                              PointF(180, 355+20) );
            painter.setPen(pen3DotDash);
            painter.drawLine( PointF(10, 370+20),
                              PointF(180, 370+20) );
#endif
        }

};

class PainterDemoWindow : public Pt::Hmi::Window
{
    public:
        PainterDemoWindow()
        {
            _tabLineStyles.setMargin(2);
            _tabCapStyles.setPadding(2);
            _tabView.setPadding(2);

            _tabView.addTab(_tabLineStyles, "Line Styles");
            _tabView.addTab(_tabCapStyles, "Cap Styles");
            _tabView.addTab(_tabJoinStyles, "Join Styles");
            _tabView.setCurrent(0);

            this->setContent(&_tabView);
        }

    protected:
        void onCloseEvent(const Pt::Hmi::CloseEvent& ev)
        {
           Pt::Hmi::Window::onCloseEvent(ev);
           Pt::Hmi::Application::instance().exit();
        }

    private:
        Pt::Hmi::TabView _tabView;
        LineStylesWidget _tabLineStyles;
        Pt::Hmi::Label   _tabCapStyles;
        Pt::Hmi::Label   _tabJoinStyles;
};

int main(int argc, char* args[])
{
    try
    {
        Pt::System::Logger::setLogLevel( "Pt.Hmi", Pt::System::Info );

        Pt::Hmi::Application app(argc, args);
        app.screen().setScaleFactor(1.0);

        PainterDemoWindow window;
        window.setTitle("Painter Demo");
        window.move( Pt::Gfx::PointF(100, 50) );
        window.resize( Pt::Gfx::SizeF(640, 480) );
        window.show();
        window.activate();

        app.run();
    }
    catch(const std::exception& ex)
    {
        std::clog << "ERROR: " << ex.what() << std::endl;
    }
}

