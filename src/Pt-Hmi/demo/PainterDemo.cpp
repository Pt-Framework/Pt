
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
            painter.drawLine( PointF(10, 55),
                              PointF(180, 55) );

            painter.setPen(pen3Solid);
            painter.drawLine( PointF(10, 70),
                              PointF(180, 70) );

            Pen pen1Dash(red, 1, Pen::Dash);
            Pen pen2Dash(red, 4, Pen::Dash);
            Pen pen3Dash(red, 9, Pen::Dash);

            painter.setPen(pen1Dash);
            painter.drawLine( PointF(10, 100),
                              PointF(180, 100) );

            painter.setPen(pen2Dash);
            painter.drawLine( PointF(10, 115),
                              PointF(180, 115) );

            painter.setPen(pen3Dash);
            painter.drawLine( PointF(10, 130),
                              PointF(180, 130) );

            Pen pen1Dot(red, 1, Pen::Dot);
            Pen pen2Dot(red, 4, Pen::Dot);
            Pen pen3Dot(red, 9, Pen::Dot);

            painter.setPen(pen1Dot);
            painter.drawLine( PointF(10, 160),
                              PointF(180, 160) );

            painter.setPen(pen2Dot);
            painter.drawLine( PointF(10, 175),
                              PointF(180, 175) );

            painter.setPen(pen3Dot);
            painter.drawLine( PointF(10, 190),
                              PointF(180, 190) );

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

