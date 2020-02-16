
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/TabView.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Label.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/ImagePainter2.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/System/Logger.h>


class PaintView : public Pt::Hmi::Control
{
    public:
        PaintView()
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

            onPaintContent(painter,       "Native Painter");
            onPaintContent(imagePainter,  "ImagePainter"  );
            onPaintContent(imagePainter2, "ImagePainter2" );

            painter.drawImage(PointF(210, 0), image1);
            painter.drawImage(PointF(420, 0), image2);
        }

        virtual void onPaintContent(Pt::Gfx::Painter& painter, 
                                    const Pt::String& text) = 0;
};


class LinesView : public PaintView
{
    public:
        LinesView()
        {}

    protected:
        virtual void onPaintContent(Pt::Gfx::Painter& painter, 
                                const Pt::String& text)
        {
            using namespace Pt::Gfx;

            Color lightPurple = Color::fromRgb8(164, 100, 255);
            Color lightBlue   = Color::fromRgb8(100, 100, 255);

            painter.setPen( lightPurple );
            painter.setFont( Font("", 12) );
            painter.drawText( PointF(10, 20), text );

            int y = 30;

            painter.setFont( Font("", 10) );
            painter.setPen(lightBlue);
            painter.drawText( PointF(20, y + 12), "FlatCap");
            y += 18;

            y = drawLines(y, painter, Pen::Solid, Pen::FlatCap);
            y = drawLines(y, painter, Pen::Dash, Pen::FlatCap);
            y = drawLines(y, painter, Pen::Dot, Pen::FlatCap);
            y = drawLines(y, painter, Pen::DashPattern, Pen::FlatCap);

            painter.setPen(lightBlue);
            painter.drawText( PointF(20, y + 12), "SquareCap");
            y += 18;

            y = drawLines(y, painter, Pen::Solid, Pen::SquareCap);
            y = drawLines(y, painter, Pen::Dash, Pen::SquareCap);
            y = drawLines(y, painter, Pen::Dot, Pen::SquareCap);
            y = drawLines(y, painter, Pen::DashPattern, Pen::SquareCap);

            painter.setPen(lightBlue);
            painter.drawText( PointF(20, y + 12), "RoundCap");
            y += 18;

            y = drawLines(y, painter, Pen::Solid, Pen::RoundCap);
            y = drawLines(y, painter, Pen::Dash, Pen::RoundCap);
            y = drawLines(y, painter, Pen::Dot, Pen::RoundCap);
            y = drawLines(y, painter, Pen::DashPattern, Pen::RoundCap);
        }

        int drawLines(int y, Pt::Gfx::Painter& painter,
                      Pt::Gfx::Pen::Style style,
                      Pt::Gfx::Pen::CapStyle cap)
        {
            using namespace Pt::Gfx;
            
            Pt::uint8_t dashes[] = { 1, 1, 3, 1 };
            std::vector<Pt::uint8_t> dashPattern( dashes, dashes + sizeof(dashes) );

            Color red = Color::fromRgb8(255, 0, 0);

            Pen pen(red, 1, style, cap);
            
            if(style == Pen::DashPattern)
                pen.setDashPattern(dashPattern);

            painter.setPen(pen);
            painter.drawLine( PointF(10, y), PointF(190, y) ); 
            y += 6;

            pen.setSize(4);
            painter.setPen(pen);
            painter.drawLine( PointF(10, y), PointF(190, y) ); 
            y += 10;

            pen.setSize(10);
            painter.setPen(pen);
            painter.drawLine( PointF(10, y), PointF(190, y) ); 
            y += 12;

            return y;
        }
};



class ShapesView : public PaintView
{
public:
    ShapesView()
    {}

protected:
    virtual void onPaintContent(Pt::Gfx::Painter& painter, const Pt::String& text)
    {
        using namespace Pt::Gfx;

        Color lightPurple = Color::fromRgb8(164, 100, 255);
        Color lightBlue = Color::fromRgb8(100, 100, 255);

        painter.setPen(lightPurple);
        painter.setFont(Font("", 12));
        painter.drawText(PointF(10, 20), text);

        int y = 30;

        painter.setFont(Font("", 10));
        painter.setPen(lightBlue);
        painter.drawText(PointF(20, y + 12), "Ellipse and Circle");
        y += 18;

        painter.setBrush(lightPurple);

        painter.fillCircle(PointF(5, y), 20);
        painter.drawCircle(PointF(35, y), 20);

        y += 25;

        painter.fillCircle(PointF(5, y), 20);
        painter.drawCircle(PointF(5, y), 20);


        painter.fillEllipse(PointF(70, y), SizeF(60, 20));
        painter.drawEllipse(PointF(70, y), SizeF(60, 20));

        //painter.drawEllipse(PointF(20, 120), SizeF(50, 100));
        //painter.fillEllipse(PointF(30, 130), SizeF(30, 80));

        y += 30;

        painter.fillRect(RectF(PointF(20, y), SizeF(20, 30)));
        painter.drawRect(RectF(PointF(20.5, y + 0.5), SizeF(19, 29)));
    }
};


class PolylinesView : public PaintView
{
    public:
        PolylinesView()
        {}

    protected:

        virtual void onPaintContent(Pt::Gfx::Painter& painter, const Pt::String& text)
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

            if(1) {
                painter.setPen( lightBlue );
                painter.drawText( PointF(20, y + 10), "FlatCap - BevelJoin");
                y += 20;

                Pen pen1Solid(red, 1, Pen::Solid, Pen::FlatCap, Pen::BevelJoin);
                Pen pen2Solid(red, 4, Pen::Solid, Pen::FlatCap, Pen::BevelJoin);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::FlatCap, Pen::BevelJoin);

                int x = 0;
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen1Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen2Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen3Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                y += 45;
            }

            if(1) {
                painter.setPen( lightBlue );
                painter.drawText( PointF(20, y + 10), "FlatCap - MiterJoin");
                y += 20;

                Pen pen1Solid(red, 1, Pen::Solid, Pen::FlatCap, Pen::MiterJoin);
                Pen pen2Solid(red, 4, Pen::Solid, Pen::FlatCap, Pen::MiterJoin);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::FlatCap, Pen::MiterJoin);

                int x = 0;
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen1Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen2Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen3Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                y += 45;
            }

            if(1) {
                painter.setPen( lightBlue );
                painter.drawText( PointF(20, y + 10), "FlatCap - RoundJoin");
                y += 20;

                Pen pen1Solid(red, 1, Pen::Solid, Pen::FlatCap, Pen::RoundJoin);
                Pen pen2Solid(red, 4, Pen::Solid, Pen::FlatCap, Pen::RoundJoin);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::FlatCap, Pen::RoundJoin);

                int x = 0;
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen1Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen2Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen3Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                y += 45;
            }

            y += 10;

            /////

            if(1) {
                painter.setPen( lightBlue );
                painter.drawText( PointF(20, y + 10), "RoundCap - BevelJoin");
                y += 20;

                Pen pen1Solid(red, 1, Pen::Solid, Pen::RoundCap, Pen::BevelJoin);
                Pen pen2Solid(red, 4, Pen::Solid, Pen::RoundCap, Pen::BevelJoin);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::RoundCap, Pen::BevelJoin);

                int x = 0;
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen1Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen2Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen3Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                y += 45;
            }

            if(1) {
                painter.setPen( lightBlue );
                painter.drawText( PointF(20, y + 10), "RoundCap - MiterJoin");
                y += 20;

                Pen pen1Solid(red, 1, Pen::Solid, Pen::RoundCap, Pen::MiterJoin);
                Pen pen2Solid(red, 4, Pen::Solid, Pen::RoundCap, Pen::MiterJoin);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::RoundCap, Pen::MiterJoin);

                int x = 0;
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen1Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen2Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen3Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                y += 45;
            }

            if(1) {
                painter.setPen( lightBlue );
                painter.drawText( PointF(20, y + 10), "RoundCap - RoundJoin");
                y += 20;

                Pen pen1Solid(red, 1, Pen::Solid, Pen::RoundCap, Pen::RoundJoin);
                Pen pen2Solid(red, 4, Pen::Solid, Pen::RoundCap, Pen::RoundJoin);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::RoundCap, Pen::RoundJoin);

                int x = 0;
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen1Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen2Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                if(1) {
                    const PointF poly[] = { // CCW
                        PointF(x + 10, y),
                        PointF(x + 30, y + 30),
                        PointF(x + 50, y + 10)
                    };
                    painter.setPen(pen3Solid);
                    painter.drawPolyline(poly, sizeof(poly) / sizeof(poly[0]) );
                    x += 65;
                }
                y += 45;
            }
        }
};


class PainterDemoWindow : public Pt::Hmi::Window
{
    public:
        PainterDemoWindow()
        {
            _linesView.setMargin(2);
            _polylinesView.setMargin(2);
            _shapesView.setMargin(2);

            _tabView.addTab(_linesView, "Lines");
            _tabView.addTab(_polylinesView, "Polylines"  );
            _tabView.addTab(_shapesView, "Shapes");
            _tabView.setPadding(8);
            _tabView.setCurrent(2);

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
        LinesView        _linesView;
        PolylinesView    _polylinesView;
        ShapesView       _shapesView;
};


int main(int argc, char* args[])
{
    try
    {
        Pt::System::Logger::setLogLevel( "Pt.Hmi", Pt::System::Info );
        Pt::System::Logger::setLogLevel( "Pt.Gfx", Pt::System::Info );

        Pt::Hmi::Application app(argc, args);
        app.setScaleFactor(1.0);

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

