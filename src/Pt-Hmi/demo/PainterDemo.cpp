
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/TabView.h>
#include <Pt/Hmi/Widget.h>
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

            paintSolidLines(painter);
            paintSolidLines(imagePainter);
            paintSolidLines(imagePainter2);

            paintDashedLines(painter);
            paintDashedLines(imagePainter);
            paintDashedLines(imagePainter2);
   
            paintDottedLines(painter);
            paintDottedLines(imagePainter);
            paintDottedLines(imagePainter2);

            painter.drawImage(Pt::Gfx::PointF(200, 0), image1);
            painter.drawImage(Pt::Gfx::PointF(400, 0), image2);
        }

        void paintSolidLines(Pt::Gfx::Painter& painter)
        {
            Pt::Gfx::Pen pen1( Pt::Gfx::Color::fromRgb8(255, 0, 0), 1);
            Pt::Gfx::Pen pen2( Pt::Gfx::Color::fromRgb8(255, 0, 0), 4);
            Pt::Gfx::Pen pen3( Pt::Gfx::Color::fromRgb8(255, 0, 0), 9);

            painter.setPen(pen1);
            painter.drawLine( Pt::Gfx::PointF(10, 20),
                              Pt::Gfx::PointF(180, 20) );

            painter.setPen(pen2);
            painter.drawLine( Pt::Gfx::PointF(10, 40),
                              Pt::Gfx::PointF(180, 40) );

            painter.setPen(pen3);
            painter.drawLine( Pt::Gfx::PointF(10, 60),
                              Pt::Gfx::PointF(180, 60) );
        }

        void paintDashedLines(Pt::Gfx::Painter& painter)
        {
            Pt::Gfx::Pen pen1( Pt::Gfx::Color::fromRgb8(255, 0, 0), 1, Pt::Gfx::Pen::Dash);
            Pt::Gfx::Pen pen2( Pt::Gfx::Color::fromRgb8(255, 0, 0), 4, Pt::Gfx::Pen::Dash);
            Pt::Gfx::Pen pen3( Pt::Gfx::Color::fromRgb8(255, 0, 0), 9, Pt::Gfx::Pen::Dash);

            painter.setPen(pen1);
            painter.drawLine( Pt::Gfx::PointF(10, 120),
                              Pt::Gfx::PointF(180, 120) );

            painter.setPen(pen2);
            painter.drawLine( Pt::Gfx::PointF(10, 140),
                              Pt::Gfx::PointF(180, 140) );

            painter.setPen(pen3);
            painter.drawLine( Pt::Gfx::PointF(10, 160),
                              Pt::Gfx::PointF(180, 160) );
        }

        void paintDottedLines(Pt::Gfx::Painter& painter)
        {
            Pt::Gfx::Pen pen1( Pt::Gfx::Color::fromRgb8(255, 0, 0), 1, Pt::Gfx::Pen::Dot);
            Pt::Gfx::Pen pen2( Pt::Gfx::Color::fromRgb8(255, 0, 0), 4, Pt::Gfx::Pen::Dot);
            Pt::Gfx::Pen pen3( Pt::Gfx::Color::fromRgb8(255, 0, 0), 9, Pt::Gfx::Pen::Dot);

            painter.setPen(pen1);
            painter.drawLine( Pt::Gfx::PointF(10, 220),
                              Pt::Gfx::PointF(180, 220) );

            painter.setPen(pen2);
            painter.drawLine( Pt::Gfx::PointF(10, 240),
                              Pt::Gfx::PointF(180, 240) );

            painter.setPen(pen3);
            painter.drawLine( Pt::Gfx::PointF(10, 260),
                              Pt::Gfx::PointF(180, 260) );
        }
};

class PainterDemoWindow : public Pt::Hmi::Window
{
    public:
        PainterDemoWindow()
        {
            _tabView.addTab(_tabLineStyles, "Line Styles");
            _tabView.setCurrent(0);

            this->setContent(&_tabView);
        }

    private:
        Pt::Hmi::TabView _tabView;
        LineStylesWidget _tabLineStyles;
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
        window.move( Pt::Gfx::PointF(50, 50) );
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

