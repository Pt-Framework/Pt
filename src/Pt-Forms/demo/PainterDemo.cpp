//
// X11       : ./jam.sh configure --debug --optimize --with-forms -sGUI=x11
// XPutImage : ./jam.sh configure --debug --optimize --with-forms -sGUI=xorg
//

#include <stdio.h>

#include <sstream>
#include <iomanip>

//#include <Pt/Format.h>

#include <Pt/Forms/Application.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/TabView.h>
#include <Pt/Forms/Control.h>
#include <Pt/Forms/Label.h>
#include <Pt/Gfx/Painter.h>
//#include <Pt/Gfx/ImagePainter2.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/System/Logger.h>


class PaintView : public Pt::Forms::Control
{
    public:
        PaintView()
        {}

    protected:
        virtual void onPaint(Pt::Forms::PaintSurface& surface,
                             const Pt::Gfx::RectF& rect)
        {
            using namespace Pt::Gfx;

            const int   imageWidth  = 200;
            const int   imageHeight = 440;
            const RectF imageRect   = RectF( PointF(0, 0), SizeF(imageWidth, imageHeight) );
            const Color background  = Color::fromRgb8(0, 0, 0);

            Painter painter(surface);
            painter.setClip(rect);
            painter.setBrush(background);
            painter.fillRect(imageRect);

            Image image1( painter.format(), Size(imageWidth, imageHeight) );
            Bitmap imageSurface(image1);
            Painter imagePainter(imageSurface);
            imagePainter.setBrush(background);
            imagePainter.fillRect(imageRect);

            //Image image2( painter.format(), Size(imageWidth, imageHeight) );
            //ImagePainter2 imagePainter2(image2);
            //imagePainter2.setAntiAliasing(true);
            //imagePainter2.setBrush(background);
            //imagePainter2.fillRect(imageRect);

            onPaintContent(painter,       "Native Painter");
            onPaintContent(imagePainter,  "ImagePainter"  );
            //onPaintContent(imagePainter2, "ImagePainter2" );

            painter.drawImage(PointF(210, 0), image1);
            //painter.drawImage(PointF(420, 0), image2);
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

            painter.setFont( Font("", 12) );
            painter.setPen( lightPurple );
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

            painter.setFont( Font("", 12) );
            painter.setPen( lightPurple );
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
        int doPaint(int y, Pt::Gfx::Painter& painter, const Pt::String& text)
        {
            using namespace Pt::Gfx;

            Color lightPurple = Color::fromRgb8(164, 100, 255);
            Color lightBlue = Color::fromRgb8(100, 100, 255);

            painter.setFont(Font("", 12));
            painter.setPen(lightPurple);
            painter.drawText(PointF(10, 20), text);

            painter.setFont(Font("", 10));
            painter.setPen(lightBlue);
            painter.drawText(PointF(20, y + 12), "Circles");
            y += 20;

            y = drawCircles(y, painter);

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
            y += 20;

            return y;
        }

        virtual void onPaintContent(Pt::Gfx::Painter& painter, const Pt::String& text)
        {
            int y = 30;
            y = doPaint(y, painter, text);

            //Pt::Gfx::ImagePainter2* ip2 = dynamic_cast<Pt::Gfx::ImagePainter2*>(&painter);
            //if(ip2) {
            //    ip2->setAntiAliasing(false);
            //    y = doPaint(y + 20, painter, text);
            //    ip2->setAntiAliasing(true);
            //};
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

            y += size + 10;

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

            y += height + 10;

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
            double insetDraw = 0.0;
            double insetFill = 0.0;
            double offsetDraw = 0.0;
            double offsetFill = 0.0;

            painter.drawRect( makeRect(x, y, width, height, insetDraw, offsetDraw) );

            x += width + 2;

            painter.fillRect( makeRect(x, y, width, height, insetFill, offsetFill) );

            x += width + 2;

            painter.fillRect( makeRect(x, y, width, height, insetFill, offsetFill) );
            painter.drawRect( makeRect(x, y, width, height, insetDraw, offsetDraw) );

            x += width + 20;

            std::vector<Pt::Gfx::PointF> shape;

            shape = makeRectangle(x, y, width, height, insetDraw, offsetDraw);
            painter.drawPolyline(&shape[0], shape.size());

            x += width + 2;

            shape = makeRectangle(x, y, width, height, insetFill, offsetFill);
            painter.fillPolygon(&shape[0], shape.size());

            x += width + 2;

            shape = makeRectangle(x, y, width, height, insetFill, offsetFill);
            painter.fillPolygon(&shape[0], shape.size());

            shape = makeRectangle(x, y, width, height, insetDraw, offsetDraw);
            painter.drawPolyline(&shape[0], shape.size());

            y += height + 10;

            return y;
        }

        Pt::Gfx::RectF makeRect(double x, double y, double width, double height,
                                double inset = 0.0, double offset = 0.0)
        {
          using namespace Pt::Gfx;

          return RectF( PointF(x + offset + inset,
                               y + offset + inset),
                        SizeF(width - 2 * inset,
                              height - 2 * inset) );
        }

        std::vector<Pt::Gfx::PointF> makeRectangle(double x, double y,
                                                   double width, double height,
                                                   double inset = 0.0, double offset = 0.0)
        {
            std::vector<Pt::Gfx::PointF> polygon(5);
            polygon[0].set(x + inset + offset,         y + inset + offset);
            polygon[1].set(x + width - inset + offset, y + inset + offset);
            polygon[2].set(x + width - inset + offset, y + height - inset + offset);
            polygon[3].set(x + inset + offset,         y + height - inset + offset);
            polygon[4] = polygon[0];
            return polygon;
        }

        int drawPolygons(int y, Pt::Gfx::Painter& painter)
        {
            using namespace Pt::Gfx;

            Color lightPurple = Color::fromRgb8(164, 100, 255);
            Color lightBlue = Color::fromRgb8(100, 100, 255);

            painter.setPen(lightBlue);
            painter.setBrush(lightPurple);

            double x = 5;
            double width = 12;
            double height = 12;
            double offsetDraw = 0.0; // pen size / 2
            double offsetFill = 0.0; // pen size / 2
            std::vector<Pt::Gfx::PointF> shape;

            // convex diamond shape

            shape = makeDiamond(x, y, width, height, offsetDraw);
            painter.drawPolyline(&shape[0], shape.size());

            x += width + 2;

            shape = makeDiamond(x, y, width, height, offsetFill);
            painter.fillPolygon(&shape[0], shape.size());

            x += width + 2;

            shape = makeDiamond(x, y, width, height, offsetFill);
            painter.fillPolygon(&shape[0], shape.size());

            shape = makeDiamond(x, y, width, height, offsetDraw);
            painter.drawPolyline(&shape[0], shape.size());

            x += width + 20;

            // concave diamond shape

            shape = makeFlag(x, y, width, height, offsetDraw);
            painter.drawPolyline(&shape[0], shape.size());

            x += width + 2;

            shape = makeFlag(x, y, width, height, offsetFill);
            painter.fillPolygon(&shape[0], shape.size());

            x += width + 2;

            shape = makeFlag(x, y, width, height, offsetFill);
            painter.fillPolygon(&shape[0], shape.size());

            shape = makeFlag(x, y, width, height, offsetDraw);
            painter.drawPolyline(&shape[0], shape.size());

            y += height + 10;

            return y;
        }

        std::vector<Pt::Gfx::PointF> makeDiamond(double x, double y,
                                                 double width, double height,
                                                 double offset = 0.0)
        {
            std::vector<Pt::Gfx::PointF> polygon(5);
            polygon[0].set(x + width/2.0 + offset,     y + offset);
            polygon[1].set(x + width + offset,         y + height/2.0 + offset);
            polygon[2].set(x + width/2.0 + offset,     y + height + offset);
            polygon[3].set(x + offset,                 y + height/2.0 + offset);
            polygon[4] = polygon[0];
            return polygon;
        }

        std::vector<Pt::Gfx::PointF> makeFlag(double x, double y,
                                              double width, double height,
                                              double offset = 0.0)
        {
            std::vector<Pt::Gfx::PointF> polygon(6);
            polygon[0].set(x + offset,             y + offset);
            polygon[1].set(x + width + offset,     y + offset);
            polygon[2].set(x + width/2.0 + offset, y + height/2.0 + offset);
            polygon[3].set(x + width + offset,     y + height + offset);
            polygon[4].set(x + offset,             y + height + offset);
            polygon[5] = polygon[0];
            return polygon;
        }
};


#include "PainterDemo_Test1.cpp"
#include "PainterDemo_Test2.cpp"
#include "PainterDemo_Benchmark1.cpp"
#include "PainterDemo_Benchmark2.cpp"


class PainterDemoWindow : public Pt::Forms::Window
{
    public:
        PainterDemoWindow()
        {
            _linesView.setMargin(2);
            _polylinesView.setMargin(2);
            _shapesView.setMargin(2);

            _tabView.setPadding(8);

            _tabView.addTab(_linesView,     "Lines"       ); // 0
            _tabView.addTab(_polylinesView, "Polylines"   ); // 1
            _tabView.addTab(_shapesView,    "Shapes"      ); // 2
            _tabView.setCurrent(2);

            _tabView.addTab(_test1View,      "Test 1"     ); // 3
            _tabView.addTab(_test2View,      "Test 2"     ); // 4
            _tabView.addTab(_benchmark1View, "Benchmark 1"); // 5
            _tabView.addTab(_benchmark2View, "Benchmark 2"); // 6
            _tabView.setCurrent(6);

            this->setContent(&_tabView);
        }

    protected:
        void onCloseEvent(const Pt::Forms::CloseEvent& ev)
        {
           Pt::Forms::Window::onCloseEvent(ev);
           Pt::Forms::Application::instance().exit();
        }

    private:
        Pt::Forms::TabView _tabView;
        LinesView        _linesView;
        PolylinesView    _polylinesView;
        ShapesView       _shapesView;

        Test1View         _test1View;
        Test2View         _test2View;
        Benchmark1View    _benchmark1View;
        Benchmark2View    _benchmark2View;
};


int main(int argc, char* args[])
{
    try
    {
        Pt::System::Logger::setLogLevel( "Pt.Forms", Pt::System::Info );
        Pt::System::Logger::setLogLevel( "Pt.Gfx", Pt::System::Info );

        Pt::Forms::Application app(argc, args);
        app.setScaleFactor(1.0);

        PainterDemoWindow window;
        window.setTitle("Painter Demo");
        window.resize( Pt::Gfx::SizeF(690, 705) );
        window.show();
        window.move( Pt::Gfx::PointF(50, 0) );
        window.activate();

        app.run();
    }
    catch(const std::exception& ex)
    {
        std::clog << "ERROR: " << ex.what() << std::endl;
    }
}


