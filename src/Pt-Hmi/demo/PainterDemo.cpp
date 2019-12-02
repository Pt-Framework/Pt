
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
            Pt::Hmi::Painter painter(surface);
            painter.setClip(rect);

            Pt::Gfx::Image image1( painter.format(), Pt::Gfx::Size(200, 400) );
            Pt::Gfx::ImagePainter imagePainter(image1);

            Pt::Gfx::Image image2( painter.format(), Pt::Gfx::Size(200, 400) );
            Pt::Gfx::ImagePainter2 imagePainter2(image2);
            imagePainter2.setAntiAliasing(true);

            drawLinesLines(painter, "Native Painter");
            drawLinesLines(imagePainter, "ImagePainter");
            drawLinesLines(imagePainter2, "ImagePainter2");

            painter.drawImage(Pt::Gfx::PointF(210, 0), image1);
            painter.drawImage(Pt::Gfx::PointF(420, 0), image2);
        }

        void drawLinesLines(Pt::Gfx::Painter& painter, const Pt::String& text)
        {
            using namespace Pt::Gfx;

            Color red = Color::fromRgb8(255, 0, 0);
            Color lightBlue = Color::fromRgb8(100, 100, 255);

            painter.setPen(lightBlue);
            painter.setFont( Font("", 12) );
            painter.drawText( PointF(20, 22), text);

            Pen pen1Solid(red, 1);
            Pen pen2Solid(red, 4);
            Pen pen3Solid(red, 9);

            painter.setPen(pen1Solid);
            painter.drawLine( PointF(10, 40),
                              PointF(180, 40) );

            painter.setPen(pen2Solid);
            painter.drawLine( PointF(10, 60),
                              PointF(180, 60) );

            painter.setPen(pen3Solid);
            painter.drawLine( PointF(10, 80),
                              PointF(180, 80) );

            Pen pen1Dash(red, 1, Pen::Dash);
            Pen pen2Dash(red, 4, Pen::Dash);
            Pen pen3Dash(red, 9, Pen::Dash);

            painter.setPen(pen1Dash);
            painter.drawLine( PointF(10, 140),
                              PointF(180, 140) );

            painter.setPen(pen2Dash);
            painter.drawLine( PointF(10, 160),
                              PointF(180, 160) );

            painter.setPen(pen3Dash);
            painter.drawLine( PointF(10, 180),
                              PointF(180, 180) );

            Pen pen1Dot(red, 1, Pen::Dot);
            Pen pen2Dot(red, 4, Pen::Dot);
            Pen pen3Dot(red, 9, Pen::Dot);

            painter.setPen(pen1Dot);
            painter.drawLine( PointF(10, 240),
                              PointF(180, 240) );

            painter.setPen(pen2Dot);
            painter.drawLine( PointF(10, 260),
                              PointF(180, 260) );

            painter.setPen(pen3Dot);
            painter.drawLine( PointF(10, 280),
                              PointF(180, 280) );
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

