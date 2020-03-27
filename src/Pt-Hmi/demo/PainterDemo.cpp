#include <iomanip>

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
        int doPaint(int y, Pt::Gfx::Painter& painter, const Pt::String& text)
        {
            using namespace Pt::Gfx;

            Color lightPurple = Color::fromRgb8(164, 100, 255);
            Color lightBlue = Color::fromRgb8(100, 100, 255);

            painter.setPen(lightPurple);
            painter.setFont(Font("", 12));
            painter.drawText(PointF(10, 20), text);

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
            y += 20;

            return y;
        }

        virtual void onPaintContent(Pt::Gfx::Painter& painter, const Pt::String& text)
        {
            int y = 30;
            y = doPaint(y, painter, text);

            Pt::Gfx::ImagePainter2* ip2 = dynamic_cast<Pt::Gfx::ImagePainter2*>(&painter);
            if(ip2) {
                ip2->setAntiAliasing(false);
                y = doPaint(y + 20, painter, text);
                ip2->setAntiAliasing(true);
            };

            //y = drawSPECIALTEST(y, painter);
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

            //IP2_DEBUG::DUMP_POLYGON_COORDINATES = true;

            shape = makeRectangle(x, y, width, height, insetFill, offsetFill);
            painter.fillPolygon(&shape[0], shape.size());

            shape = makeRectangle(x, y, width, height, insetDraw, offsetDraw);
            painter.drawPolyline(&shape[0], shape.size());

            IP2_DEBUG::DUMP_POLYGON_COORDINATES = false;

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
#if 0
            Pt::Gfx::ImagePainter2* ip2 = dynamic_cast<Pt::Gfx::ImagePainter2*>(&painter);
            if(ip2 && !ip2->isAntiAliasing()) {
                IP2_DEBUG::DUMP_POLYGON_COORDINATES = true;
                std::cerr << "### 1 ###" << std::endl;
                for (size_t i = 0; i < shape.size(); ++i) {
                    std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                              << shape[i].x() << ", " << shape[i].y() << std::endl;
                }
                std::cerr << std::endl;
            }
#endif
            painter.drawPolyline(&shape[0], shape.size());
            IP2_DEBUG::DUMP_POLYGON_COORDINATES = false;

            x += width + 2;

            shape = makeDiamond(x, y, width, height, offsetFill);
#if 0
            Pt::Gfx::ImagePainter2* ip2 = dynamic_cast<Pt::Gfx::ImagePainter2*>(&painter);
            if(ip2 && !ip2->isAntiAliasing()) {
                IP2_DEBUG::DUMP_POLYGON_COORDINATES = true;
                std::cerr << "### 1 ###" << std::endl;
                for (size_t i = 0; i < shape.size(); ++i) {
                    std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                              << shape[i].x() << ", " << shape[i].y() << std::endl;
                }
                std::cerr << std::endl;
            }
#endif
#if 0
            Pt::Gfx::ImagePainter2* ip2 = dynamic_cast<Pt::Gfx::ImagePainter2*>(&painter);
            if(ip2) {
                if(ip2->isAntiAliasing()) std::cerr << "WITH AA" << std::endl;
                else                      std::cerr << "NO AA" << std::endl;
                IP2_DEBUG::DUMP_SCANLINE_COORDINATES = true;
            }
#endif
            painter.fillPolygon(&shape[0], shape.size());

            IP2_DEBUG::DUMP_SCANLINE_COORDINATES = false;
            IP2_DEBUG::DUMP_POLYGON_COORDINATES  = false;

            x += width + 2;

            //IP2_DEBUG::DUMP_POLYGON_COORDINATES = true;

            shape = makeDiamond(x, y, width, height, offsetFill);
            painter.fillPolygon(&shape[0], shape.size());

            shape = makeDiamond(x, y, width, height, offsetDraw);
            painter.drawPolyline(&shape[0], shape.size());

            IP2_DEBUG::DUMP_POLYGON_COORDINATES = false;

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

#if 0
        int drawSPECIALTEST(int y, Pt::Gfx::Painter& painter)
        {
            using namespace Pt::Gfx;

            Color lightPurple = Color::fromRgb8(164, 100, 255);
            Color lightBlue = Color::fromRgb8(100, 100, 255);

            painter.setPen(lightBlue);
            painter.setBrush(lightPurple);

            double x      = 5;
            double width  = 5;
            double height = 5;
            std::vector<Pt::Gfx::PointF> polygon;

            auto makeSimpleRectangle = [](double x, double y, double width, double height) {
                std::vector<Pt::Gfx::PointF> p(5);
                p[0].set(x,         y         );
                p[1].set(x + width, y         );
                p[2].set(x + width, y + height);
                p[3].set(x,         y + height);
                p[4] = p[0];
                return p;
            };
            polygon = makeSimpleRectangle(x, y, width, height); x += width + 2;
            painter.drawPolyline(&polygon[0], polygon.size());
            polygon = makeSimpleRectangle(x, y, width, height); x += width + 2;
            painter.fillPolygon (&polygon[0], polygon.size());
            //IP2_DEBUG::DUMP_POLYGON_COORDINATES = true;
            polygon = makeSimpleRectangle(x, y, width, height); x += width + 2;
            painter.fillPolygon (&polygon[0], polygon.size());
            painter.drawPolyline(&polygon[0], polygon.size());
            IP2_DEBUG::DUMP_POLYGON_COORDINATES = false;
            y += 20;

            x      = 5;
            width  = 12;
            height = 12;
            auto makeSimpleDiamond = [](double x, double y, double width, double height) {
                std::vector<Pt::Gfx::PointF> p(5);
                p[0].set(x + width/2.0, y             );
                p[1].set(x + width,     y + height/2.0);
                p[2].set(x + width/2.0, y + height    );
                p[3].set(x,             y + height/2.0);
                p[4] = p[0];
                return p;
            };
            polygon = makeSimpleDiamond(x, y, width, height); x += width + 2;
            painter.drawPolyline(&polygon[0], polygon.size());
            polygon = makeSimpleDiamond(x, y, width, height); x += width + 2;
            painter.fillPolygon (&polygon[0], polygon.size());
            //IP2_DEBUG::DUMP_POLYGON_COORDINATES = true;
            polygon = makeSimpleDiamond(x, y, width, height); x += width + 2;
            painter.fillPolygon (&polygon[0], polygon.size());
            painter.drawPolyline(&polygon[0], polygon.size());
            IP2_DEBUG::DUMP_POLYGON_COORDINATES = false;
            y += 20;

            x      = 5;
            width  = 12;
            height = 12;
            auto makeSimpleFlag = [](double x, double y, double width, double height) {
                std::vector<Pt::Gfx::PointF> p(6);
                p[0].set(x,             y             );
                p[1].set(x + width,     y             );
                p[2].set(x + width/2.0, y + height/2.0);
                p[3].set(x + width,     y + height    );
                p[4].set(x,             y + height    );
                p[5] = p[0];
                return p;
            };
            polygon = makeSimpleFlag(x, y, width, height); x += width + 2;
            painter.drawPolyline(&polygon[0], polygon.size());
            polygon = makeSimpleFlag(x, y, width, height); x += width + 2;
            painter.fillPolygon (&polygon[0], polygon.size());
            //IP2_DEBUG::DUMP_POLYGON_COORDINATES = true;
            polygon = makeSimpleFlag(x, y, width, height); x += width + 2;
            painter.fillPolygon (&polygon[0], polygon.size());
            painter.drawPolyline(&polygon[0], polygon.size());
            IP2_DEBUG::DUMP_POLYGON_COORDINATES = false;
            y += 20;

            return y;
        }
#endif
};


class TestView : public Pt::Hmi::Control
{
    public:
        TestView()
        {}

    protected:
        virtual void onPaint(Pt::Hmi::PaintSurface& surface,
                             const Pt::Gfx::RectF& rect)
        {
          using namespace Pt::Gfx;

            int imageWidth = 620, imageHeight = 680;
            RectF imageRect = RectF( PointF(0, 0), SizeF(imageWidth, imageHeight) );
            Color background = Color::fromRgb8(0, 0, 0);

            Pt::Hmi::Painter painter(surface);
            painter.setClip(rect);
            painter.setBrush(background);

            Image image2( painter.format(), Size(imageWidth, imageHeight) );
            ImagePainter2 imagePainter2(image2);
            imagePainter2.setAntiAliasing(true);
            imagePainter2.setBrush(background);
            imagePainter2.fillRect(imageRect);

            onPaintContent(imagePainter2);

            painter.drawImage(PointF(2, 2), image2);
        }

        virtual void onPaintContent(Pt::Gfx::ImagePainter2& painter)
        {
            using namespace Pt::Gfx;

            Pt::Gfx::Pen green1( Color::fromRgb8(0, 255, 0), 1, Pen::Solid, Pen::RoundCap, Pen::NoJoin );
            Pt::Gfx::Pen green2( Color::fromRgb8(0, 255, 0), 2, Pen::Solid, Pen::RoundCap, Pen::NoJoin );
            Pt::Gfx::Pen green3( Color::fromRgb8(0, 255, 0), 3, Pen::Solid, Pen::RoundCap, Pen::NoJoin );

            std::vector<Pt::Gfx::PointF> shape;

#if 1
            shape = makeTestShape1(-300, -380);
            painter.setPen(green1);
            painter.drawPolyline( &shape[0], shape.size() );
            shape = makeTestShape1(-300, -300);
            painter.setPen(green2);
            painter.drawPolyline( &shape[0], shape.size() );
            shape = makeTestShape1(-300, -220);
            painter.setPen(green3);
            painter.drawPolyline( &shape[0], shape.size() );
#endif

#if 1
            shape = makeTestShape2(0, -580);
            painter.setPen(green1);
            painter.drawPolyline( &shape[0], shape.size() );
            shape = makeTestShape2(0, -500);
            painter.setPen(green2);
            painter.drawPolyline( &shape[0], shape.size() );
            shape = makeTestShape2(0, -420);
            painter.setPen(green3);
            painter.drawPolyline( &shape[0], shape.size() );
#endif

#if 1
            shape = makeTestShape3(50, -530);
            painter.setPen(green1);
            painter.drawPolyline( &shape[0], shape.size() );
            shape = makeTestShape3(50, -450);
            painter.setPen(green2);
            painter.drawPolyline( &shape[0], shape.size() );
            shape = makeTestShape3(50, -370);
            painter.setPen(green3);
            painter.drawPolyline( &shape[0], shape.size() );
            shape = makeTestShape3(50, -290);
            painter.setBrush(Color::fromRgb8(0, 255, 0));
            painter.fillPolygon( &shape[0], shape.size() );

            shape = makeTestShape3(150, -530); scaleShape(shape, 5);
            painter.setPen(green1);
            painter.drawPolyline( &shape[0], shape.size() );
            shape = makeTestShape3(150, -450); scaleShape(shape, 5);
            painter.setPen(green2);
            painter.drawPolyline( &shape[0], shape.size() );
            shape = makeTestShape3(150, -370); scaleShape(shape, 5);
            painter.setPen(green3);
            painter.drawPolyline( &shape[0], shape.size() );
            shape = makeTestShape3(150, -290); scaleShape(shape, 5);
            painter.setBrush(Color::fromRgb8(0, 255, 0));
            painter.fillPolygon( &shape[0], shape.size() );
#endif
        }

        std::vector<Pt::Gfx::PointF> makeTestShape1(double xOfs, double yOfs)
        {
            std::vector<Pt::Gfx::PointF> points;
            points.push_back(Pt::Gfx::PointF(xOfs + 364.25000000000000, yOfs + 444.50000000000000) );
            points.push_back(Pt::Gfx::PointF(xOfs + 365.75000000000000, yOfs + 446.00000000000000) );
            points.push_back(Pt::Gfx::PointF(xOfs + 366.50000000000000, yOfs + 446.00000000000000) );
            points.push_back(Pt::Gfx::PointF(xOfs + 368.00000000000000, yOfs + 445.25000000000000) );
            return points;
        }

        std::vector<Pt::Gfx::PointF> makeTestShape2(double xOfs, double yOfs)
        {
            std::vector<Pt::Gfx::PointF> points;
            points.push_back(Pt::Gfx::PointF(xOfs + 135.00000000000000, yOfs + 613.00000000000000 ) );
            points.push_back(Pt::Gfx::PointF(xOfs + 142.00000000000000, yOfs + 624.00000000000000 ) );
            points.push_back(Pt::Gfx::PointF(xOfs + 148.00000000000000, yOfs + 630.00000000000000 ) );
            points.push_back(Pt::Gfx::PointF(xOfs + 155.00000000000000, yOfs + 637.00000000000000 ) );
            points.push_back(Pt::Gfx::PointF(xOfs + 161.00000000000000, yOfs + 641.00000000000000 ) );
            points.push_back(Pt::Gfx::PointF(xOfs + 168.00000000000000, yOfs + 641.00000000000000 ) );
            points.push_back(Pt::Gfx::PointF(xOfs + 174.00000000000000, yOfs + 638.00000000000000 ) );
            points.push_back(Pt::Gfx::PointF(xOfs + 181.00000000000000, yOfs + 643.00000000000000 ) );
            return points;
        }

        std::vector<Pt::Gfx::PointF> makeTestShape3(double xOfs, double yOfs)
        {
            std::vector<Pt::Gfx::PointF> points;
            points.push_back(Pt::Gfx::PointF( xOfs + 200.04287138041008, yOfs + 586.71004820396047 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 200.49912727639179, yOfs + 587.09713039419933 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 200.95538317237353, yOfs + 587.14491976184195 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 201.41163906835527, yOfs + 587.06236973221769 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 201.86789496433698, yOfs + 587.14328427272108 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 202.32415086031870, yOfs + 587.35944317395581 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 202.78040675630041, yOfs + 587.33688018323755 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 203.23666265228215, yOfs + 587.06855211079812 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 203.69291854826389, yOfs + 586.81770235767578 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 204.14917444424560, yOfs + 586.34153385773061 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 204.60543034022731, yOfs + 585.38229968091991 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 205.06168623620903, yOfs + 584.32761411477202 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 205.51794213219077, yOfs + 583.34996804920286 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 205.97419802817251, yOfs + 581.81590048140788 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 206.43045392415422, yOfs + 581.22712949756612 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 206.88670982013593, yOfs + 581.10834525424411 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 207.34296571611765, yOfs + 580.50872918254674 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 207.79922161209939, yOfs + 580.19319242330948 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 208.25547750808113, yOfs + 580.18038204659888 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 208.71173340406284, yOfs + 581.20957233906279 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 209.16798930004455, yOfs + 583.21187839564334 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 209.62424519602627, yOfs + 584.87984718126211 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 210.08050109200801, yOfs + 586.03687522046437 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 210.53675698798975, yOfs + 586.63168745407006 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 210.99301288397146, yOfs + 586.96522243684171 ) );
            points.push_back(Pt::Gfx::PointF( xOfs + 211.44926877995317, yOfs + 587.33749410772862 ) );
            return points;
        }

        void scaleShape(std::vector<Pt::Gfx::PointF>& points, double scale)
        {
            double minX = 99999;
            double minY = 99999;
            for(size_t i = 0; i < points.size(); ++i)
            {
                const double x = points[i].x();
                const double y = points[i].y();
                if(x < minX) minX = x;
                if(y < minY) minY = y;
            }

            for(size_t i = 0; i < points.size(); ++i)
            {
                const double x = (points[i].x() - minX) * scale;
                const double y = (points[i].y() - minY) * scale;
                points[i].set( x + minX, y + minY );
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

            _tabView.addTab(_testView, "Test");
            _tabView.setCurrent(3);

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

        TestView         _testView;
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
        window.resize( Pt::Gfx::SizeF(640, 700) );
        window.show();
        window.activate();

        app.run();
    }
    catch(const std::exception& ex)
    {
        std::clog << "ERROR: " << ex.what() << std::endl;
    }
}


