
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


class PolylinesView : public PaintView
{
    public:
        PolylinesView()
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
            painter.drawText( PointF(20, y + 14), "BevelJoin");
            y += 20;

            y = drawLines(y, painter, Pen::BevelJoin);

            painter.setPen(lightBlue);
            painter.drawText( PointF(20, y + 14), "MiterJoin");
            y += 20;

            y = drawLines(y, painter, Pen::MiterJoin);

            painter.setPen(lightBlue);
            painter.drawText( PointF(20, y + 14), "RoundJoin");
            y += 20;

            y = drawLines(y, painter, Pen::RoundJoin);
        }

        int drawLines(int y, Pt::Gfx::Painter& painter,
                      Pt::Gfx::Pen::JoinStyle join)
        {
            using namespace Pt::Gfx;

            Color red = Color::fromRgb8(255,   0,   0);

            Pen pen(red, 1, Pen::Solid, Pen::FlatCap, join);

            PointF polygon[] = { // CCW
                PointF(20, y),
                PointF(90, y + 30),
                PointF(180, y),
                PointF(190, y + 30)
            };

            pen.setSize(1);
            painter.setPen(pen);
            painter.drawPolyline(polygon, 4);

            y += 20;

            PointF polygon2[] = { // CCW
                PointF(15, y),
                PointF(85, y + 30),
                PointF(175, y),
                PointF(185, y + 30)
            };

            pen.setSize(4);
            painter.setPen(pen);
            painter.drawPolyline(polygon2, 4);

            y += 30;

            PointF polygon3[] = { // CCW
                PointF(10, y),
                PointF(80, y + 30),
                PointF(170, y),
                PointF(180, y + 30)
            };

            pen.setSize(11);
            painter.setPen(pen);
            painter.drawPolyline(polygon3, 4);

            y += 50;

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
            painter.drawText(PointF(20, y + 12), "Circles");
            y += 20;

            y = drawCircles(y, painter);

            painter.setFont(Font("", 10));
            painter.setPen(lightBlue);
            painter.drawText(PointF(20, y + 12), "Ellipses");
            y += 20;

            y = drawEllipses(y, painter);

            painter.setPen(lightBlue);
            painter.drawText( PointF(20, y + 12), "Rectangles");
            y += 20;

            y = drawRects(y, painter);

            painter.setPen(lightBlue);
            painter.drawText( PointF(20, y + 12), "Polygons");
            y += 20;

            y = drawPolygons(y, painter);
            y = drawDiamonds(y, painter);
        }

        int drawCircles(int y, Pt::Gfx::Painter& painter)
        {
            using namespace Pt::Gfx;

            Color lightPurple = Color::fromRgb8(164, 100, 255);
            Color lightBlue = Color::fromRgb8(100, 100, 255);

            painter.setPen(lightBlue);
            painter.setBrush(lightPurple);

            double x = 5;
            double size = 20;

            painter.fillCircle(PointF(x, y), size);

            x += size + 1;

            painter.drawCircle(PointF(x, y), size);

            x += size + 1;

            painter.fillCircle(PointF(x, y), size);
            painter.drawCircle(PointF(x, y), size);

            y += size + 20;

            return y;
        }

        int drawEllipses(int y, Pt::Gfx::Painter& painter)
        {
            using namespace Pt::Gfx;

            Color lightPurple = Color::fromRgb8(164, 100, 255);
            Color lightBlue = Color::fromRgb8(100, 100, 255);

            painter.setPen(lightBlue);
            painter.setBrush(lightPurple);

            double x = 5;
            double width = 50;
            double height = 20;

            painter.fillEllipse(PointF(x, y), SizeF(width, height));

            x += width + 1;

            painter.drawEllipse(PointF(x, y), SizeF(width, height));

            x += width + 1;
            painter.fillEllipse(PointF(x, y), SizeF(width, height));
            painter.drawEllipse(PointF(x, y), SizeF(width, height));

            y += height + 20;

            return y;
        }

        int drawRects(int y, Pt::Gfx::Painter& painter)
        {
            using namespace Pt::Gfx;

            Color lightPurple = Color::fromRgb8(164, 100, 255);
            Color lightBlue = Color::fromRgb8(100, 100, 255);

            painter.setPen(lightBlue);
            painter.setBrush(lightPurple);

            double x = 5;
            double width = 5;
            double height = 5;
            double inset = 0;

            painter.fillRect(RectF(PointF(x + inset, y + inset),
                                   SizeF(width - 2*inset, height- 2*inset)));

            inset = 0.5;
            x += width + 1;
            painter.drawRect(RectF(PointF(x + inset, y + inset),
                                   SizeF(width - 2*inset, height - 2*inset)));

            x += 60;

            painter.fillRect(RectF(PointF(x + inset, y + inset),
                                   SizeF(width - 2*inset, height - 2*inset)));
            painter.drawRect(RectF(PointF(x + inset, y + inset),
                                   SizeF(width - 2*inset, height - 2*inset)));

            y += height + 20;

            return y;
        }

        int drawPolygons(int y, Pt::Gfx::Painter& painter)
        {
            using namespace Pt::Gfx;

            Color lightPurple = Color::fromRgb8(164, 100, 255);
            Color lightBlue = Color::fromRgb8(100, 100, 255);

            painter.setPen(lightBlue);
            painter.setBrush(lightPurple);

            double x = 5;
            double width = 5;
            double height = 5;
            double inset = 0.5;

            Pt::Gfx::PointF polyline[5];
            polyline[0].set(x + inset, y + inset);
            polyline[1].set(x + width - inset, y + inset);
            polyline[2].set(x + width - inset, y + height - inset);
            polyline[3].set(x + inset, y + height - inset);
            polyline[4] = polyline[0];
            painter.drawPolyline(polyline, 5);

            x += width + 1;
            inset = 0;
            Pt::Gfx::PointF polygon[5];
            polygon[0].set(x + inset, y + inset);
            polygon[1].set(x + width - inset, y + inset);
            polygon[2].set(x + width - inset, y + height - inset);
            polygon[3].set(x + inset, y + height - inset);
            polygon[4] = polygon[0];
            painter.fillPolygon(polygon, 5);



            x += 60;
            inset = 0;
            Pt::Gfx::PointF polygon2[5];
            polygon2[0].set(x + inset, y + inset);
            polygon2[1].set(x + width - inset, y + inset);
            polygon2[2].set(x + width - inset, y + height - inset);
            polygon2[3].set(x + inset, y + height - inset);
            polygon2[4] = polygon2[0];
            painter.fillPolygon(polygon2, 5);

            inset = 0.5;
            Pt::Gfx::PointF polyline2[5];
            polyline2[0].set(x + inset, y + inset);
            polyline2[1].set(x + width - inset, y + inset);
            polyline2[2].set(x + width - inset, y + height - inset);
            polyline2[3].set(x + inset, y + height - inset);
            polyline2[4] = polyline2[0];
            painter.drawPolyline(polyline2, 5);

            y += height + 20;

            return y;
        }

        int drawDiamonds(int y, Pt::Gfx::Painter& painter)
        {
            using namespace Pt::Gfx;

            Color lightPurple = Color::fromRgb8(164, 100, 255);
            Color lightBlue = Color::fromRgb8(100, 100, 255);

            painter.setPen(lightBlue);
            painter.setBrush(lightPurple);

            double x = 5;
            double width = 12;
            double height = 12;
            double insetD = 0;
            double insetF = 0;
            std::vector<Pt::Gfx::PointF> shape;

            shape = makeDiamond(x, y, width, height, insetD);
            painter.drawPolyline(&shape[0], shape.size());

            x += width + 2;

            shape = makeDiamond(x, y, width, height, insetF);
            painter.fillPolygon(&shape[0], shape.size());

            x += width + 2;

            shape = makeDiamond(x, y, width, height, insetF);
            painter.fillPolygon(&shape[0], shape.size());
            shape = makeDiamond(x, y, width, height, insetD);
            painter.drawPolyline(&shape[0], shape.size());

            x += width + 2;

            shape = makeFlag(x, y, width, height, insetD);
            painter.drawPolyline(&shape[0], shape.size());

            x += width + 2;

            shape = makeFlag(x, y, width, height, insetF);
            painter.fillPolygon(&shape[0], shape.size());

            x += width + 2;

            shape = makeFlag(x, y, width, height, insetF);
            painter.fillPolygon(&shape[0], shape.size());
            shape = makeFlag(x, y, width, height, insetD);
            painter.drawPolyline(&shape[0], shape.size());

            y += height + 20;

            return y;
        }

        std::vector<Pt::Gfx::PointF> makeDiamond(double x, double y,
                                                 double width, double height,
                                                 double inset)
        {
            std::vector<Pt::Gfx::PointF> polygon(5);
            polygon[0].set(x + width/2.0,     y + inset);
            polygon[1].set(x + width - inset, y + height/2.0);
            polygon[2].set(x + width/2.0,     y + height - inset);
            polygon[3].set(x + inset,         y + height/2.0);
            polygon[4] = polygon[0];
            return polygon;
        }


        std::vector<Pt::Gfx::PointF> makeFlag(double x, double y,
                                              double width, double height,
                                              double inset)
        {
            std::vector<Pt::Gfx::PointF> polygon(6);
            polygon[0].set(x + inset,     y + inset);
            polygon[1].set(x + width - inset,  y + inset);
            polygon[2].set(x + width/2.0 - inset, y + height/2.0);
            polygon[3].set(x + width - inset, y + height - inset);
            polygon[4].set(x + inset, y + height - inset);
            polygon[5] = polygon[0];
            return polygon;
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


