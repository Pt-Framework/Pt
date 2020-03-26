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

#if 1
            // RoundCap, RoundJoin
            Pt::Gfx::Pen rcrj1( Color::fromRgb8(255, 255, 255), 1, Pen::Solid, Pen::RoundCap, Pen::RoundJoin );
            Pt::Gfx::Pen rcrj2( Color::fromRgb8(0,   255,   0), 2, Pen::Solid, Pen::RoundCap, Pen::RoundJoin );
            Pt::Gfx::Pen rcrj3( Color::fromRgb8(0,   255,   0), 3, Pen::Solid, Pen::RoundCap, Pen::RoundJoin );

            // FlatCap, BevelJoin
            Pt::Gfx::Pen fcbj1( Color::fromRgb8(255, 255, 255), 1, Pen::Solid, Pen::FlatCap, Pen::BevelJoin );
            Pt::Gfx::Pen fcbj2( Color::fromRgb8(0,   255,   0), 2, Pen::Solid, Pen::FlatCap, Pen::BevelJoin );
            Pt::Gfx::Pen fcbj3( Color::fromRgb8(0,   255,   0), 3, Pen::Solid, Pen::FlatCap, Pen::BevelJoin );
#else
            Pt::Gfx::Pen rcrj1( Color::fromRgb8(255, 255, 255), 1, Pen::Solid, Pen::RoundCap, Pen::NoJoin );
            Pt::Gfx::Pen rcrj2( Color::fromRgb8(0,   255,   0), 2, Pen::Solid, Pen::RoundCap, Pen::NoJoin );
            Pt::Gfx::Pen rcrj3( Color::fromRgb8(0,   255,   0), 3, Pen::Solid, Pen::RoundCap, Pen::NoJoin );

            Pt::Gfx::Pen fcbj1( Color::fromRgb8(255, 255, 255), 1, Pen::Solid, Pen::RoundCap, Pen::NoJoin );
            Pt::Gfx::Pen fcbj2( Color::fromRgb8(0,   255,   0), 2, Pen::Solid, Pen::RoundCap, Pen::NoJoin );
            Pt::Gfx::Pen fcbj3( Color::fromRgb8(0,   255,   0), 3, Pen::Solid, Pen::RoundCap, Pen::NoJoin );
#endif

            std::vector<Pt::Gfx::PointF> shape;

            //shape = makeTestShape1(-300, -380);
            //painter.setPen(rcrj1);
            //painter.drawPolyline( &shape[0], shape.size() );
            shape = makeTestShape1(-300, -300);
            painter.setPen(rcrj2);
            painter.drawPolyline( &shape[0], shape.size() );
            //shape = makeTestShape1(-300, -220);
            //painter.setPen(rcrj3);
            //painter.drawPolyline( &shape[0], shape.size() );

            //shape = makeTestShape2(0, -580);
            //painter.setPen(fcbj1);
            //painter.drawPolyline( &shape[0], shape.size() );
            shape = makeTestShape2(0, -500);
            painter.setPen(fcbj2);
            painter.drawPolyline( &shape[0], shape.size() );
            //shape = makeTestShape2(0, -420);
            //painter.setPen(fcbj3);
            //painter.drawPolyline( &shape[0], shape.size() );
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
            //points.push_back(Pt::Gfx::PointF(xOfs + 135.00000000000000, yOfs + 613.00000000000000 ) );
            //points.push_back(Pt::Gfx::PointF(xOfs + 142.00000000000000, yOfs + 624.00000000000000 ) );
            //points.push_back(Pt::Gfx::PointF(xOfs + 148.00000000000000, yOfs + 630.00000000000000 ) );
            //points.push_back(Pt::Gfx::PointF(xOfs + 155.00000000000000, yOfs + 637.00000000000000 ) );
            //points.push_back(Pt::Gfx::PointF(xOfs + 161.00000000000000, yOfs + 641.00000000000000 ) );
            //points.push_back(Pt::Gfx::PointF(xOfs + 168.00000000000000, yOfs + 641.00000000000000 ) );
            //points.push_back(Pt::Gfx::PointF(xOfs + 174.00000000000000, yOfs + 638.00000000000000 ) );
            //points.push_back(Pt::Gfx::PointF(xOfs + 181.00000000000000, yOfs + 643.00000000000000 ) );

            points.push_back(Pt::Gfx::PointF( 49.478425706441250, 580.56902103648815 ) );
            points.push_back(Pt::Gfx::PointF( 49.934681602422991, 579.65288910389404 ) );
            points.push_back(Pt::Gfx::PointF( 50.390937498404710, 579.10761753369900 ) );
            points.push_back(Pt::Gfx::PointF( 50.847193394386430, 577.84638915106393 ) );
            points.push_back(Pt::Gfx::PointF( 51.303449290368150, 577.05764688413342 ) );
            points.push_back(Pt::Gfx::PointF( 51.759705186349869, 575.85544930487856 ) );
            points.push_back(Pt::Gfx::PointF( 52.215961082331610, 573.93217131660333 ) );
            points.push_back(Pt::Gfx::PointF( 52.672216978313330, 572.29433840440970 ) );
            points.push_back(Pt::Gfx::PointF( 53.128472874295049, 571.54499273420242 ) );
            points.push_back(Pt::Gfx::PointF( 53.584728770276769, 572.52301928652059 ) );
            points.push_back(Pt::Gfx::PointF( 54.040984666258488, 574.39608921078184 ) );
            points.push_back(Pt::Gfx::PointF( 54.497240562240229, 575.48845293601244 ) );
            points.push_back(Pt::Gfx::PointF( 54.953496458221927, 574.98782539189699 ) );
            points.push_back(Pt::Gfx::PointF( 55.409752354203668, 571.83974870248971 ) );
            points.push_back(Pt::Gfx::PointF( 55.866008250185409, 565.96768054425047 ) );
            points.push_back(Pt::Gfx::PointF( 56.322264146167107, 556.98093645869517 ) );
            points.push_back(Pt::Gfx::PointF( 56.778520042148848, 543.67210577523065 ) );
            points.push_back(Pt::Gfx::PointF( 57.234775938130547, 527.10937664183268 ) );
            points.push_back(Pt::Gfx::PointF( 57.691031834112287, 510.56418149526905 ) );
            points.push_back(Pt::Gfx::PointF( 58.147287730094028, 499.07836062042190 ) );
            points.push_back(Pt::Gfx::PointF( 58.603543626075727, 497.78069257045604 ) );
            points.push_back(Pt::Gfx::PointF( 59.059799522057467, 508.36675052067523 ) );
            points.push_back(Pt::Gfx::PointF( 59.516055418039166, 526.07820392215194 ) );
            points.push_back(Pt::Gfx::PointF( 59.972311314020907, 544.14048209649104 ) );
            points.push_back(Pt::Gfx::PointF( 60.428567210002647, 558.10990974126969 ) );
            points.push_back(Pt::Gfx::PointF( 60.884823105984346, 567.72187642465826 ) );
            points.push_back(Pt::Gfx::PointF( 61.341079001966087, 573.90936857742929 ) );
            points.push_back(Pt::Gfx::PointF( 61.797334897947785, 577.72862195536572 ) );
            points.push_back(Pt::Gfx::PointF( 62.253590793929526, 580.17171704743294 ) );
            points.push_back(Pt::Gfx::PointF( 62.709846689911267, 582.05073606587928 ) );
            points.push_back(Pt::Gfx::PointF( 63.166102585892965, 583.29970982394514 ) );
            points.push_back(Pt::Gfx::PointF( 63.622358481874706, 584.06213477319352 ) );
            points.push_back(Pt::Gfx::PointF( 64.078614377856411, 584.64641697106231 ) );
            points.push_back(Pt::Gfx::PointF( 64.534870273838152, 585.13199219526757 ) );
            points.push_back(Pt::Gfx::PointF( 64.991126169819836, 585.47793846330819 ) );
            points.push_back(Pt::Gfx::PointF( 65.447382065801577, 585.66067341007897 ) );
            points.push_back(Pt::Gfx::PointF( 65.903637961783318, 585.70168193706490 ) );
            points.push_back(Pt::Gfx::PointF( 66.359893857765030, 585.70004195799140 ) );
            points.push_back(Pt::Gfx::PointF( 66.816149753746771, 585.87094466876044 ) );
            points.push_back(Pt::Gfx::PointF( 67.272405649728455, 586.26651629522598 ) );
            points.push_back(Pt::Gfx::PointF( 67.728661545710196, 586.38193011054068 ) );
            points.push_back(Pt::Gfx::PointF( 68.184917441691937, 585.98268519938119 ) );
            points.push_back(Pt::Gfx::PointF( 68.641173337673649, 586.14805740248767 ) );
            points.push_back(Pt::Gfx::PointF( 69.097429233655390, 586.16534251093162 ) );
            points.push_back(Pt::Gfx::PointF( 69.553685129637074, 586.02558183673204 ) );
            points.push_back(Pt::Gfx::PointF( 70.009941025618815, 586.19552392310959 ) );
            points.push_back(Pt::Gfx::PointF( 70.466196921600556, 586.10742441336993 ) );
            points.push_back(Pt::Gfx::PointF( 70.922452817582268, 586.05811729337438 ) );
            points.push_back(Pt::Gfx::PointF( 71.378708713564009, 586.10198695250870 ) );
            points.push_back(Pt::Gfx::PointF( 71.834964609545693, 586.14350237483063 ) );
            points.push_back(Pt::Gfx::PointF( 72.291220505527434, 586.35035079733257 ) );
            points.push_back(Pt::Gfx::PointF( 72.747476401509175, 586.71009621356882 ) );
            points.push_back(Pt::Gfx::PointF( 73.203732297490888, 586.89815151440257 ) );
            points.push_back(Pt::Gfx::PointF( 73.659988193472628, 586.75119187404778 ) );
            points.push_back(Pt::Gfx::PointF( 74.116244089454312, 586.59968641052478 ) );
            points.push_back(Pt::Gfx::PointF( 74.572499985436053, 586.93524487459524 ) );
            points.push_back(Pt::Gfx::PointF( 75.028755881417794, 587.11542820875206 ) );
            points.push_back(Pt::Gfx::PointF( 75.485011777399507, 587.00675151950065 ) );
            points.push_back(Pt::Gfx::PointF( 75.941267673381248, 587.15176453240008 ) );
            points.push_back(Pt::Gfx::PointF( 76.397523569362932, 587.28108135515311 ) );
            points.push_back(Pt::Gfx::PointF( 76.853779465344672, 587.33742924597857 ) );
            points.push_back(Pt::Gfx::PointF( 77.310035361326413, 587.16362214887670 ) );
            points.push_back(Pt::Gfx::PointF( 77.766291257308126, 586.75216186235866 ) );
            points.push_back(Pt::Gfx::PointF( 78.222547153289867, 586.55355025842880 ) );
            points.push_back(Pt::Gfx::PointF( 78.678803049271551, 586.28886659196792 ) );
            points.push_back(Pt::Gfx::PointF( 79.135058945253292, 585.92385783199779 ) );
            points.push_back(Pt::Gfx::PointF( 79.591314841235032, 585.78587472178992 ) );
            points.push_back(Pt::Gfx::PointF( 80.047570737216745, 585.98624522727505 ) );
            points.push_back(Pt::Gfx::PointF( 80.503826633198486, 585.76725272990882 ) );
            points.push_back(Pt::Gfx::PointF( 80.960082529180170, 585.17737980661673 ) );
            points.push_back(Pt::Gfx::PointF( 81.416338425161911, 584.11796506677194 ) );
            points.push_back(Pt::Gfx::PointF( 81.872594321143652, 582.00168411889160 ) );
            points.push_back(Pt::Gfx::PointF( 82.328850217125364, 579.36487890394540 ) );
            points.push_back(Pt::Gfx::PointF( 82.785106113107105, 576.21466680526248 ) );
            points.push_back(Pt::Gfx::PointF( 83.241362009088789, 573.23171888992965 ) );
            points.push_back(Pt::Gfx::PointF( 83.697617905070530, 571.94972050525871 ) );
            points.push_back(Pt::Gfx::PointF( 84.153873801052271, 573.17757278872693 ) );
            points.push_back(Pt::Gfx::PointF( 84.610129697033983, 576.26891884483689 ) );
            points.push_back(Pt::Gfx::PointF( 85.066385593015724, 579.95672185452827 ) );
            points.push_back(Pt::Gfx::PointF( 85.522641488997408, 583.06871397684313 ) );
            points.push_back(Pt::Gfx::PointF( 85.978897384979149, 584.97721935402865 ) );
            points.push_back(Pt::Gfx::PointF( 86.435153280960861, 585.90934422454598 ) );
            points.push_back(Pt::Gfx::PointF( 86.891409176942602, 586.44603419659518 ) );
            points.push_back(Pt::Gfx::PointF( 87.347665072924343, 586.90683145715536 ) );
            points.push_back(Pt::Gfx::PointF( 87.803920968906027, 587.08362385354894 ) );
            points.push_back(Pt::Gfx::PointF( 88.260176864887768, 586.86070953709770 ) );
            points.push_back(Pt::Gfx::PointF( 88.716432760869480, 586.83691835012962 ) );
            points.push_back(Pt::Gfx::PointF( 89.172688656851221, 587.06063904963185 ) );
            points.push_back(Pt::Gfx::PointF( 89.628944552832962, 586.78532363942611 ) );
            points.push_back(Pt::Gfx::PointF( 90.085200448814646, 586.44926842477287 ) );
            points.push_back(Pt::Gfx::PointF( 90.541456344796387, 586.00874014311239 ) );
            points.push_back(Pt::Gfx::PointF( 90.997712240778100, 585.40060809440627 ) );
            points.push_back(Pt::Gfx::PointF( 91.453968136759840, 584.79508464704236 ) );
            points.push_back(Pt::Gfx::PointF( 91.910224032741581, 584.12330899132735 ) );
            points.push_back(Pt::Gfx::PointF( 92.366479928723265, 583.46996669614555 ) );
            points.push_back(Pt::Gfx::PointF( 92.822735824705006, 582.83252623780118 ) );
            points.push_back(Pt::Gfx::PointF( 93.278991720686719, 582.16605987107698 ) );
            points.push_back(Pt::Gfx::PointF( 93.735247616668460, 581.58279832601193 ) );
            points.push_back(Pt::Gfx::PointF( 94.191503512650200, 581.05403197572912 ) );
            points.push_back(Pt::Gfx::PointF( 94.647759408631885, 580.96492358326464 ) );
            points.push_back(Pt::Gfx::PointF( 95.104015304613625, 581.48698711469183 ) );
            points.push_back(Pt::Gfx::PointF( 95.560271200595338, 582.12815393106018 ) );
            points.push_back(Pt::Gfx::PointF( 96.016527096577079, 583.13989857056151 ) );
            points.push_back(Pt::Gfx::PointF( 96.472782992558820, 584.10614222079278 ) );
            points.push_back(Pt::Gfx::PointF( 96.929038888540504, 584.71554632427160 ) );
            points.push_back(Pt::Gfx::PointF( 97.385294784522245, 585.28490571071109 ) );
            points.push_back(Pt::Gfx::PointF( 97.841550680503957, 585.57209641759289 ) );
            points.push_back(Pt::Gfx::PointF( 98.297806576485698, 585.49502105863110 ) );
            points.push_back(Pt::Gfx::PointF( 98.754062472467439, 585.36938246221530 ) );
            points.push_back(Pt::Gfx::PointF( 99.210318368449123, 585.55065362797359 ) );
            points.push_back(Pt::Gfx::PointF( 99.666574264430864, 585.80858606275024 ) );
            points.push_back(Pt::Gfx::PointF( 100.12283016041258, 585.99666522300288 ) );
            points.push_back(Pt::Gfx::PointF( 100.57908605639432, 586.22758075707611 ) );
            points.push_back(Pt::Gfx::PointF( 101.03534195237606, 586.54487377634121 ) );
            points.push_back(Pt::Gfx::PointF( 101.49159784835774, 586.64850980400047 ) );
            points.push_back(Pt::Gfx::PointF( 101.94785374433948, 586.86373157262676 ) );
            points.push_back(Pt::Gfx::PointF( 102.40410964032120, 587.17718766879455 ) );
            points.push_back(Pt::Gfx::PointF( 102.86036553630294, 587.33447127208910 ) );
            points.push_back(Pt::Gfx::PointF( 103.31662143228468, 587.37958450949259 ) );
            points.push_back(Pt::Gfx::PointF( 103.77287732826636, 587.43074806565278 ) );
            points.push_back(Pt::Gfx::PointF( 104.22913322424810, 587.43642286787372 ) );
            points.push_back(Pt::Gfx::PointF( 104.68538912022981, 587.33635229627919 ) );
            points.push_back(Pt::Gfx::PointF( 105.14164501621156, 587.24207645241790 ) );
            points.push_back(Pt::Gfx::PointF( 105.59790091219330, 587.33446953721375 ) );
            points.push_back(Pt::Gfx::PointF( 106.05415680817498, 587.47996540350732 ) );
            points.push_back(Pt::Gfx::PointF( 106.51041270415672, 587.42509348016324 ) );
            points.push_back(Pt::Gfx::PointF( 106.96666860013843, 587.13099046720254 ) );
            points.push_back(Pt::Gfx::PointF( 107.42292449612017, 586.95458851767035 ) );
            points.push_back(Pt::Gfx::PointF( 107.87918039210186, 587.08857202730269 ) );
            points.push_back(Pt::Gfx::PointF( 108.33543628808360, 587.26903412915908 ) );
            points.push_back(Pt::Gfx::PointF( 108.79169218406534, 587.16095128673896 ) );
            points.push_back(Pt::Gfx::PointF( 109.24794808004705, 586.94754281151700 ) );
            points.push_back(Pt::Gfx::PointF( 109.70420397602879, 586.71445487080859 ) );
            points.push_back(Pt::Gfx::PointF( 110.16045987201048, 586.57229352619936 ) );
            points.push_back(Pt::Gfx::PointF( 110.61671576799222, 586.47877394156023 ) );
            points.push_back(Pt::Gfx::PointF( 111.07297166397396, 586.12329983983352 ) );
            points.push_back(Pt::Gfx::PointF( 111.52922755995567, 585.54279960284816 ) );
            points.push_back(Pt::Gfx::PointF( 111.98548345593741, 584.76432363862250 ) );
            points.push_back(Pt::Gfx::PointF( 112.44173935191910, 583.74162877583785 ) );
            points.push_back(Pt::Gfx::PointF( 112.89799524790084, 582.37509335549953 ) );
            points.push_back(Pt::Gfx::PointF( 113.35425114388258, 580.48700291934676 ) );
            points.push_back(Pt::Gfx::PointF( 113.81050703986429, 578.47072795216354 ) );
            points.push_back(Pt::Gfx::PointF( 114.26676293584603, 576.97543205466673 ) );
            points.push_back(Pt::Gfx::PointF( 114.72301883182772, 576.50073685925804 ) );
            points.push_back(Pt::Gfx::PointF( 115.17927472780946, 577.22443543688848 ) );
            points.push_back(Pt::Gfx::PointF( 115.63553062379120, 578.87751366312114 ) );
            points.push_back(Pt::Gfx::PointF( 116.09178651977291, 581.20025110630297 ) );
            points.push_back(Pt::Gfx::PointF( 116.54804241575465, 583.59870954117923 ) );
            points.push_back(Pt::Gfx::PointF( 117.00429831173633, 584.81587067546229 ) );
            points.push_back(Pt::Gfx::PointF( 117.46055420771808, 585.51850908188089 ) );
            points.push_back(Pt::Gfx::PointF( 117.91681010369982, 585.96846458527693 ) );
            points.push_back(Pt::Gfx::PointF( 118.37306599968153, 586.02383799880613 ) );
            points.push_back(Pt::Gfx::PointF( 118.82932189566327, 586.13129694282213 ) );
            points.push_back(Pt::Gfx::PointF( 119.28557779164495, 586.12474187886323 ) );
            points.push_back(Pt::Gfx::PointF( 119.74183368762669, 586.26554008070423 ) );
            points.push_back(Pt::Gfx::PointF( 120.19808958360844, 586.68056279796815 ) );
            points.push_back(Pt::Gfx::PointF( 120.65434547959015, 586.93607572234703 ) );
            points.push_back(Pt::Gfx::PointF( 121.11060137557189, 587.16591077577516 ) );
            points.push_back(Pt::Gfx::PointF( 121.56685727155357, 587.36444143228698 ) );
            points.push_back(Pt::Gfx::PointF( 122.02311316753531, 587.40925570064894 ) );
            points.push_back(Pt::Gfx::PointF( 122.47936906351705, 587.24784119157039 ) );
            points.push_back(Pt::Gfx::PointF( 122.93562495949877, 586.93798473739093 ) );
            points.push_back(Pt::Gfx::PointF( 123.39188085548051, 586.56337048770513 ) );
            points.push_back(Pt::Gfx::PointF( 123.84813675146219, 586.19306384769607 ) );
            points.push_back(Pt::Gfx::PointF( 124.30439264744393, 585.85465977677802 ) );
            points.push_back(Pt::Gfx::PointF( 124.76064854342567, 585.45889581081906 ) );
            points.push_back(Pt::Gfx::PointF( 125.21690443940739, 584.60637464514252 ) );
            points.push_back(Pt::Gfx::PointF( 125.67316033538913, 583.42849108382688 ) );
            points.push_back(Pt::Gfx::PointF( 126.12941623137081, 582.00495387260855 ) );
            points.push_back(Pt::Gfx::PointF( 126.58567212735255, 579.58525933730380 ) );
            points.push_back(Pt::Gfx::PointF( 127.04192802333429, 575.91070488695141 ) );
            points.push_back(Pt::Gfx::PointF( 127.49818391931601, 569.54808938442284 ) );
            points.push_back(Pt::Gfx::PointF( 127.95443981529775, 559.59077789675439 ) );
            points.push_back(Pt::Gfx::PointF( 128.41069571127943, 547.94847698072283 ) );
            points.push_back(Pt::Gfx::PointF( 128.86695160726117, 539.23461651082800 ) );
            points.push_back(Pt::Gfx::PointF( 129.32320750324288, 538.05425642893022 ) );
            points.push_back(Pt::Gfx::PointF( 129.77946339922462, 546.20612251540535 ) );
            points.push_back(Pt::Gfx::PointF( 130.23571929520637, 560.16329864121167 ) );
            points.push_back(Pt::Gfx::PointF( 130.69197519118805, 571.21904786572509 ) );
            points.push_back(Pt::Gfx::PointF( 131.14823108716979, 578.85102778334044 ) );
            points.push_back(Pt::Gfx::PointF( 131.60448698315150, 583.64218154572723 ) );
            points.push_back(Pt::Gfx::PointF( 132.06074287913324, 585.21766361214713 ) );
            points.push_back(Pt::Gfx::PointF( 132.51699877511498, 586.61930921296903 ) );
            points.push_back(Pt::Gfx::PointF( 132.97325467109667, 587.33620468868469 ) );
            points.push_back(Pt::Gfx::PointF( 133.42951056707841, 587.28397711314165 ) );
            points.push_back(Pt::Gfx::PointF( 133.88576646306012, 587.32475065589654 ) );
            points.push_back(Pt::Gfx::PointF( 134.34202235904186, 587.32496027901857 ) );
            points.push_back(Pt::Gfx::PointF( 134.79827825502360, 587.22244011300859 ) );
            points.push_back(Pt::Gfx::PointF( 135.25453415100529, 587.03927685990857 ) );
            points.push_back(Pt::Gfx::PointF( 135.71079004698703, 586.75825793073648 ) );
            points.push_back(Pt::Gfx::PointF( 136.16704594296874, 586.34041033862115 ) );
            points.push_back(Pt::Gfx::PointF( 136.62330183895048, 585.92137826139447 ) );
            points.push_back(Pt::Gfx::PointF( 137.07955773493222, 585.08167293249721 ) );
            points.push_back(Pt::Gfx::PointF( 137.53581363091391, 583.72855279677060 ) );
            points.push_back(Pt::Gfx::PointF( 137.99206952689565, 582.27012475272943 ) );
            points.push_back(Pt::Gfx::PointF( 138.44832542287736, 580.36448211601953 ) );
            points.push_back(Pt::Gfx::PointF( 138.90458131885910, 578.16399887947364 ) );
            points.push_back(Pt::Gfx::PointF( 139.36083721484084, 575.66859336032894 ) );
            points.push_back(Pt::Gfx::PointF( 139.81709311082253, 572.78694024186746 ) );
            points.push_back(Pt::Gfx::PointF( 140.27334900680427, 570.02378224606207 ) );
            points.push_back(Pt::Gfx::PointF( 140.72960490278598, 566.84601816560109 ) );
            points.push_back(Pt::Gfx::PointF( 141.18586079876772, 563.18273485837767 ) );
            points.push_back(Pt::Gfx::PointF( 141.64211669474946, 559.24438868792288 ) );
            points.push_back(Pt::Gfx::PointF( 142.09837259073115, 554.49805547427502 ) );
            points.push_back(Pt::Gfx::PointF( 142.55462848671289, 548.65410109561594 ) );
            points.push_back(Pt::Gfx::PointF( 143.01088438269460, 540.56719290642991 ) );
            points.push_back(Pt::Gfx::PointF( 143.46714027867634, 529.68284747517021 ) );
            points.push_back(Pt::Gfx::PointF( 143.92339617465808, 517.83236834530805 ) );
            points.push_back(Pt::Gfx::PointF( 144.37965207063976, 508.76068823300426 ) );
            points.push_back(Pt::Gfx::PointF( 144.83590796662151, 507.05558618426937 ) );
            points.push_back(Pt::Gfx::PointF( 145.29216386260322, 514.86174328543711 ) );
            points.push_back(Pt::Gfx::PointF( 145.74841975858496, 529.39214848769916 ) );
            points.push_back(Pt::Gfx::PointF( 146.20467565456670, 543.92856712521643 ) );
            points.push_back(Pt::Gfx::PointF( 146.66093155054838, 556.04573807494535 ) );
            points.push_back(Pt::Gfx::PointF( 147.11718744653012, 564.21998613581786 ) );
            points.push_back(Pt::Gfx::PointF( 147.57344334251184, 567.40587937707483 ) );
            points.push_back(Pt::Gfx::PointF( 148.02969923849358, 568.44246024048141 ) );
            points.push_back(Pt::Gfx::PointF( 148.48595513447532, 567.35068840191275 ) );
            points.push_back(Pt::Gfx::PointF( 148.94221103045700, 566.08706832514008 ) );
            points.push_back(Pt::Gfx::PointF( 149.39846692643874, 567.06644086940344 ) );
            points.push_back(Pt::Gfx::PointF( 149.85472282242046, 569.02461238998603 ) );
            points.push_back(Pt::Gfx::PointF( 150.31097871840220, 573.08771731221873 ) );
            points.push_back(Pt::Gfx::PointF( 150.76723461438388, 577.10940019180464 ) );
            points.push_back(Pt::Gfx::PointF( 151.22349051036562, 578.51967564552774 ) );
            points.push_back(Pt::Gfx::PointF( 151.67974640634736, 578.07401224698629 ) );
            points.push_back(Pt::Gfx::PointF( 152.13600230232908, 574.91121446725685 ) );
            points.push_back(Pt::Gfx::PointF( 152.59225819831082, 569.61967364192174 ) );
            points.push_back(Pt::Gfx::PointF( 153.04851409429250, 563.27976923805102 ) );
            points.push_back(Pt::Gfx::PointF( 153.50476999027424, 556.89724280277790 ) );
            points.push_back(Pt::Gfx::PointF( 153.96102588625598, 553.39492524666127 ) );
            points.push_back(Pt::Gfx::PointF( 154.41728178223769, 554.58374644468768 ) );
            points.push_back(Pt::Gfx::PointF( 154.87353767821944, 560.07705285106249 ) );
            points.push_back(Pt::Gfx::PointF( 155.32979357420112, 567.62936974660431 ) );
            points.push_back(Pt::Gfx::PointF( 155.78604947018286, 574.59389627962196 ) );
            points.push_back(Pt::Gfx::PointF( 156.24230536616460, 579.95110248748279 ) );
            points.push_back(Pt::Gfx::PointF( 156.69856126214631, 583.64189892824425 ) );
            points.push_back(Pt::Gfx::PointF( 157.15481715812805, 585.86343854859160 ) );
            points.push_back(Pt::Gfx::PointF( 157.61107305410974, 586.85350336844624 ) );
            points.push_back(Pt::Gfx::PointF( 158.06732895009148, 587.20642896126105 ) );
            points.push_back(Pt::Gfx::PointF( 158.52358484607322, 587.17350712896791 ) );
            points.push_back(Pt::Gfx::PointF( 158.97984074205493, 586.70249271942612 ) );
            points.push_back(Pt::Gfx::PointF( 159.43609663803667, 585.84923914598608 ) );
            points.push_back(Pt::Gfx::PointF( 159.89235253401836, 584.87728694239479 ) );
            points.push_back(Pt::Gfx::PointF( 160.34860843000010, 583.31911333838218 ) );
            points.push_back(Pt::Gfx::PointF( 160.80486432598184, 580.68262655614910 ) );
            points.push_back(Pt::Gfx::PointF( 161.26112022196355, 576.92488685106332 ) );
            points.push_back(Pt::Gfx::PointF( 161.71737611794529, 571.55189568860260 ) );
            points.push_back(Pt::Gfx::PointF( 162.17363201392698, 563.35976357429388 ) );
            points.push_back(Pt::Gfx::PointF( 162.62988790990872, 551.55191892623156 ) );
            points.push_back(Pt::Gfx::PointF( 163.08614380589046, 536.35285916332373 ) );
            points.push_back(Pt::Gfx::PointF( 163.54239970187217, 519.14230509543427 ) );
            points.push_back(Pt::Gfx::PointF( 163.99865559785391, 504.43380818721164 ) );
            points.push_back(Pt::Gfx::PointF( 164.45491149383560, 496.16949376045591 ) );
            points.push_back(Pt::Gfx::PointF( 164.91116738981734, 496.18193550710885 ) );
            points.push_back(Pt::Gfx::PointF( 165.36742328579908, 504.41821533846291 ) );
            points.push_back(Pt::Gfx::PointF( 165.82367918178079, 517.14545213311897 ) );
            points.push_back(Pt::Gfx::PointF( 166.27993507776253, 531.34896811880560 ) );
            points.push_back(Pt::Gfx::PointF( 166.73619097374421, 545.07081725980345 ) );
            points.push_back(Pt::Gfx::PointF( 167.19244686972596, 556.38561978766302 ) );
            points.push_back(Pt::Gfx::PointF( 167.64870276570770, 565.20693023216472 ) );
            points.push_back(Pt::Gfx::PointF( 168.10495866168941, 571.37330863630984 ) );
            points.push_back(Pt::Gfx::PointF( 168.56121455767115, 575.36507813158642 ) );
            points.push_back(Pt::Gfx::PointF( 169.01747045365283, 578.46861185659816 ) );
            points.push_back(Pt::Gfx::PointF( 169.47372634963457, 580.48217195256552 ) );
            points.push_back(Pt::Gfx::PointF( 169.92998224561632, 581.26196751593466 ) );
            points.push_back(Pt::Gfx::PointF( 170.38623814159803, 581.31915831465744 ) );
            points.push_back(Pt::Gfx::PointF( 170.84249403757977, 581.06093599255973 ) );
            points.push_back(Pt::Gfx::PointF( 171.29874993356145, 579.57004905874578 ) );
            points.push_back(Pt::Gfx::PointF( 171.75500582954319, 577.31877713043525 ) );
            points.push_back(Pt::Gfx::PointF( 172.21126172552491, 574.42682251848066 ) );
            points.push_back(Pt::Gfx::PointF( 172.66751762150665, 570.53117947337773 ) );
            points.push_back(Pt::Gfx::PointF( 173.12377351748839, 565.98066636653175 ) );
            points.push_back(Pt::Gfx::PointF( 173.58002941347007, 560.44864223163495 ) );
            points.push_back(Pt::Gfx::PointF( 174.03628530945181, 554.45427246987515 ) );
            points.push_back(Pt::Gfx::PointF( 174.49254120543353, 548.68865389663813 ) );
            points.push_back(Pt::Gfx::PointF( 174.94879710141527, 543.51420102301199 ) );
            points.push_back(Pt::Gfx::PointF( 175.40505299739701, 541.98156663551356 ) );
            points.push_back(Pt::Gfx::PointF( 175.86130889337869, 544.10363884481194 ) );
            points.push_back(Pt::Gfx::PointF( 176.31756478936043, 548.70160036523055 ) );
            points.push_back(Pt::Gfx::PointF( 176.77382068534214, 555.42942991612483 ) );
            points.push_back(Pt::Gfx::PointF( 177.23007658132389, 561.92930272511990 ) );
            points.push_back(Pt::Gfx::PointF( 177.68633247730563, 567.67479618929610 ) );
            points.push_back(Pt::Gfx::PointF( 178.14258837328731, 572.48484166977642 ) );
            points.push_back(Pt::Gfx::PointF( 178.59884426926905, 575.93259308505594 ) );
            points.push_back(Pt::Gfx::PointF( 179.05510016525076, 579.02926533393975 ) );
            points.push_back(Pt::Gfx::PointF( 179.51135606123250, 581.27720917097884 ) );
            points.push_back(Pt::Gfx::PointF( 179.96761195721425, 582.90619604403355 ) );
            points.push_back(Pt::Gfx::PointF( 180.42386785319593, 584.51694885340805 ) );
            points.push_back(Pt::Gfx::PointF( 180.88012374917767, 585.11883557417241 ) );
            points.push_back(Pt::Gfx::PointF( 181.33637964515938, 585.86662429649039 ) );
            points.push_back(Pt::Gfx::PointF( 181.79263554114112, 586.78258627142554 ) );
            points.push_back(Pt::Gfx::PointF( 182.24889143712286, 587.04927804473982 ) );
            points.push_back(Pt::Gfx::PointF( 182.70514733310455, 586.98904552806209 ) );
            points.push_back(Pt::Gfx::PointF( 183.16140322908629, 587.08931589556755 ) );
            points.push_back(Pt::Gfx::PointF( 183.61765912506800, 586.85442269569774 ) );
            points.push_back(Pt::Gfx::PointF( 184.07391502104974, 586.53634161361742 ) );
            points.push_back(Pt::Gfx::PointF( 184.53017091703148, 587.15822612008560 ) );
            points.push_back(Pt::Gfx::PointF( 184.98642681301317, 586.44288846992401 ) );
            points.push_back(Pt::Gfx::PointF( 185.44268270899491, 585.84502539445111 ) );
            points.push_back(Pt::Gfx::PointF( 185.89893860497662, 585.79764714844043 ) );
            points.push_back(Pt::Gfx::PointF( 186.35519450095836, 584.60959865712221 ) );
            points.push_back(Pt::Gfx::PointF( 186.81145039694010, 584.47865518909032 ) );
            points.push_back(Pt::Gfx::PointF( 187.26770629292179, 583.74345267889748 ) );
            points.push_back(Pt::Gfx::PointF( 187.72396218890353, 582.75039592990720 ) );
            points.push_back(Pt::Gfx::PointF( 188.18021808488524, 583.01770966392098 ) );
            points.push_back(Pt::Gfx::PointF( 188.63647398086698, 581.63440498132297 ) );
            points.push_back(Pt::Gfx::PointF( 189.09272987684872, 580.89682953427848 ) );
            points.push_back(Pt::Gfx::PointF( 189.54898577283041, 580.50600566791286 ) );
            points.push_back(Pt::Gfx::PointF( 190.00524166881215, 579.09588279891057 ) );
            points.push_back(Pt::Gfx::PointF( 190.46149756479386, 578.79038258127855 ) );
            points.push_back(Pt::Gfx::PointF( 190.91775346077560, 578.06158659382288 ) );
            points.push_back(Pt::Gfx::PointF( 191.37400935675734, 577.37094999696149 ) );
            points.push_back(Pt::Gfx::PointF( 191.83026525273903, 577.45760017114742 ) );
            points.push_back(Pt::Gfx::PointF( 192.28652114872079, 577.26442940493553 ) );
            points.push_back(Pt::Gfx::PointF( 192.74277704470251, 577.80591024248611 ) );
            points.push_back(Pt::Gfx::PointF( 193.19903294068422, 578.67856190831503 ) );
            points.push_back(Pt::Gfx::PointF( 193.65528883666593, 579.50209696618003 ) );
            points.push_back(Pt::Gfx::PointF( 194.11154473264767, 580.71300113447126 ) );
            points.push_back(Pt::Gfx::PointF( 194.56780062862941, 581.75038918436076 ) );
            points.push_back(Pt::Gfx::PointF( 195.02405652461113, 582.64648726637961 ) );
            points.push_back(Pt::Gfx::PointF( 195.48031242059284, 583.54718372842285 ) );
            points.push_back(Pt::Gfx::PointF( 195.93656831657455, 584.27945496209668 ) );
            points.push_back(Pt::Gfx::PointF( 196.39282421255629, 584.56112933944473 ) );
            points.push_back(Pt::Gfx::PointF( 196.84908010853803, 584.72692433042880 ) );
            points.push_back(Pt::Gfx::PointF( 197.30533600451975, 585.28976467034909 ) );
            points.push_back(Pt::Gfx::PointF( 197.76159190050146, 585.89985627740543 ) );
            points.push_back(Pt::Gfx::PointF( 198.21784779648317, 585.47922122461193 ) );
            points.push_back(Pt::Gfx::PointF( 198.67410369246491, 585.91770369203562 ) );
            points.push_back(Pt::Gfx::PointF( 199.13035958844665, 586.52190299032168 ) );
            points.push_back(Pt::Gfx::PointF( 199.58661548442836, 586.32401666051635 ) );
            points.push_back(Pt::Gfx::PointF( 200.04287138041008, 586.71004820396047 ) );
            points.push_back(Pt::Gfx::PointF( 200.49912727639179, 587.09713039419933 ) );
            points.push_back(Pt::Gfx::PointF( 200.95538317237353, 587.14491976184195 ) );
            points.push_back(Pt::Gfx::PointF( 201.41163906835527, 587.06236973221769 ) );
            points.push_back(Pt::Gfx::PointF( 201.86789496433698, 587.14328427272108 ) );
            points.push_back(Pt::Gfx::PointF( 202.32415086031870, 587.35944317395581 ) );
            points.push_back(Pt::Gfx::PointF( 202.78040675630041, 587.33688018323755 ) );
            points.push_back(Pt::Gfx::PointF( 203.23666265228215, 587.06855211079812 ) );
            points.push_back(Pt::Gfx::PointF( 203.69291854826389, 586.81770235767578 ) );
            points.push_back(Pt::Gfx::PointF( 204.14917444424560, 586.34153385773061 ) );
            points.push_back(Pt::Gfx::PointF( 204.60543034022731, 585.38229968091991 ) );
            points.push_back(Pt::Gfx::PointF( 205.06168623620903, 584.32761411477202 ) );
            points.push_back(Pt::Gfx::PointF( 205.51794213219077, 583.34996804920286 ) );
            points.push_back(Pt::Gfx::PointF( 205.97419802817251, 581.81590048140788 ) );
            points.push_back(Pt::Gfx::PointF( 206.43045392415422, 581.22712949756612 ) );
            points.push_back(Pt::Gfx::PointF( 206.88670982013593, 581.10834525424411 ) );
            points.push_back(Pt::Gfx::PointF( 207.34296571611765, 580.50872918254674 ) );
            points.push_back(Pt::Gfx::PointF( 207.79922161209939, 580.19319242330948 ) );
            points.push_back(Pt::Gfx::PointF( 208.25547750808113, 580.18038204659888 ) );
            points.push_back(Pt::Gfx::PointF( 208.71173340406284, 581.20957233906279 ) );
            points.push_back(Pt::Gfx::PointF( 209.16798930004455, 583.21187839564334 ) );
            points.push_back(Pt::Gfx::PointF( 209.62424519602627, 584.87984718126211 ) );
            points.push_back(Pt::Gfx::PointF( 210.08050109200801, 586.03687522046437 ) );
            points.push_back(Pt::Gfx::PointF( 210.53675698798975, 586.63168745407006 ) );
            points.push_back(Pt::Gfx::PointF( 210.99301288397146, 586.96522243684171 ) );
            points.push_back(Pt::Gfx::PointF( 211.44926877995317, 587.33749410772862 ) );
            points.push_back(Pt::Gfx::PointF( 211.90552467593488, 587.27476386684737 ) );
            points.push_back(Pt::Gfx::PointF( 212.36178057191663, 587.13840615964887 ) );
            points.push_back(Pt::Gfx::PointF( 212.81803646789837, 586.90918022498613 ) );
            points.push_back(Pt::Gfx::PointF( 213.27429236388008, 586.08969719699166 ) );
            points.push_back(Pt::Gfx::PointF( 213.73054825986179, 584.69895770772655 ) );
            points.push_back(Pt::Gfx::PointF( 214.18680415584350, 583.75461680743558 ) );
            points.push_back(Pt::Gfx::PointF( 214.64306005182524, 582.05264549167612 ) );
            points.push_back(Pt::Gfx::PointF( 215.09931594780693, 579.42477093339835 ) );
            points.push_back(Pt::Gfx::PointF( 215.55557184378870, 577.95387585481888 ) );
            points.push_back(Pt::Gfx::PointF( 216.01182773977041, 573.28052295135330 ) );
            points.push_back(Pt::Gfx::PointF( 216.46808363575212, 568.25997531312032 ) );
            points.push_back(Pt::Gfx::PointF( 216.92433953173386, 564.06016557976261 ) );
            points.push_back(Pt::Gfx::PointF( 217.38059542771555, 557.03063199660312 ) );
            points.push_back(Pt::Gfx::PointF( 217.83685132369732, 553.03331266107375 ) );
            points.push_back(Pt::Gfx::PointF( 218.29310721967903, 550.17403939051064 ) );
            points.push_back(Pt::Gfx::PointF( 218.74936311566074, 548.43219183743156 ) );
            points.push_back(Pt::Gfx::PointF( 219.20561901164248, 551.13437443951761 ) );
            points.push_back(Pt::Gfx::PointF( 219.66187490762417, 554.65024672840480 ) );
            points.push_back(Pt::Gfx::PointF( 220.11813080360594, 559.62570251765283 ) );
            points.push_back(Pt::Gfx::PointF( 220.57438669958765, 565.25430801353195 ) );
            points.push_back(Pt::Gfx::PointF( 221.03064259556936, 569.84822656269603 ) );
            points.push_back(Pt::Gfx::PointF( 221.48689849155110, 574.97298041214390 ) );
            points.push_back(Pt::Gfx::PointF( 221.94315438753279, 578.70885215887165 ) );
            points.push_back(Pt::Gfx::PointF( 222.39941028351456, 581.21505216440812 ) );
            points.push_back(Pt::Gfx::PointF( 222.85566617949627, 583.37065331589235 ) );
            points.push_back(Pt::Gfx::PointF( 223.31192207547798, 584.16560983414490 ) );
            points.push_back(Pt::Gfx::PointF( 223.76817797145972, 584.69531264853094 ) );
            points.push_back(Pt::Gfx::PointF( 224.22443386744141, 584.33585532530708 ) );
            points.push_back(Pt::Gfx::PointF( 224.68068976342317, 582.99302184916826 ) );
            points.push_back(Pt::Gfx::PointF( 225.13694565940489, 581.89491264959179 ) );
            points.push_back(Pt::Gfx::PointF( 225.59320155538660, 579.39064675794998 ) );
            points.push_back(Pt::Gfx::PointF( 226.04945745136834, 576.28522580287415 ) );
            points.push_back(Pt::Gfx::PointF( 226.50571334735002, 572.05576094592607 ) );
            points.push_back(Pt::Gfx::PointF( 226.96196924333179, 564.93613948607231 ) );
            points.push_back(Pt::Gfx::PointF( 227.41822513931351, 556.72375660712555 ) );
            points.push_back(Pt::Gfx::PointF( 227.87448103529522, 545.88411554678601 ) );
            points.push_back(Pt::Gfx::PointF( 228.33073693127696, 532.38143954280815 ) );
            points.push_back(Pt::Gfx::PointF( 228.78699282725864, 518.26177181020807 ) );
            points.push_back(Pt::Gfx::PointF( 229.24324872324041, 504.95507993369898 ) );
            points.push_back(Pt::Gfx::PointF( 229.69950461922213, 496.13292051807809 ) );
            points.push_back(Pt::Gfx::PointF( 230.15576051520384, 493.08872697709296 ) );
            points.push_back(Pt::Gfx::PointF( 230.61201641118558, 494.39686246984604 ) );
            points.push_back(Pt::Gfx::PointF( 231.06827230716726, 497.99990131679829 ) );
            points.push_back(Pt::Gfx::PointF( 231.52452820314903, 502.55285443495177 ) );
            points.push_back(Pt::Gfx::PointF( 231.98078409913074, 510.08454509577803 ) );
            points.push_back(Pt::Gfx::PointF( 232.43703999511246, 521.68934617823470 ) );
            points.push_back(Pt::Gfx::PointF( 232.89329589109420, 534.80976991520015 ) );
            points.push_back(Pt::Gfx::PointF( 233.34955178707588, 547.16499678271362 ) );
            points.push_back(Pt::Gfx::PointF( 233.80580768305765, 556.33073972171519 ) );
            points.push_back(Pt::Gfx::PointF( 234.26206357903934, 561.24599263708535 ) );
            points.push_back(Pt::Gfx::PointF( 234.71831947502108, 563.01737075005644 ) );
            points.push_back(Pt::Gfx::PointF( 235.17457537100282, 563.41571358993133 ) );
            points.push_back(Pt::Gfx::PointF( 235.63083126698450, 560.14360734584659 ) );
            points.push_back(Pt::Gfx::PointF( 236.08708716296627, 554.07456694555390 ) );
            points.push_back(Pt::Gfx::PointF( 236.54334305894795, 547.33213205570382 ) );
            points.push_back(Pt::Gfx::PointF( 236.99959895492970, 536.99363646106121 ) );
            points.push_back(Pt::Gfx::PointF( 237.45585485091144, 525.83879380503106 ) );
            points.push_back(Pt::Gfx::PointF( 237.91211074689312, 516.09285174976458 ) );
            points.push_back(Pt::Gfx::PointF( 238.36836664287489, 509.28919978141596 ) );
            points.push_back(Pt::Gfx::PointF( 238.82462253885657, 508.30426074390698 ) );
            points.push_back(Pt::Gfx::PointF( 239.28087843483831, 512.96510968192661 ) );
            points.push_back(Pt::Gfx::PointF( 239.73713433082006, 519.40008749230731 ) );
            points.push_back(Pt::Gfx::PointF( 240.19339022680174, 526.01640865973081 ) );
            points.push_back(Pt::Gfx::PointF( 240.64964612278351, 535.07667420776227 ) );
            points.push_back(Pt::Gfx::PointF( 241.10590201876519, 544.12074028144332 ) );
            points.push_back(Pt::Gfx::PointF( 241.56215791474693, 553.24004829579440 ) );
            points.push_back(Pt::Gfx::PointF( 242.01841381072867, 562.12623921325053 ) );
            points.push_back(Pt::Gfx::PointF( 242.47466970671036, 568.67587382348177 ) );
            points.push_back(Pt::Gfx::PointF( 242.93092560269213, 572.74636619559749 ) );
            points.push_back(Pt::Gfx::PointF( 243.38718149867381, 576.06272762725575 ) );
            points.push_back(Pt::Gfx::PointF( 243.84343739465555, 578.94517366911964 ) );
            points.push_back(Pt::Gfx::PointF( 244.29969329063729, 580.78066511044415 ) );
            points.push_back(Pt::Gfx::PointF( 244.75594918661898, 582.56753688078743 ) );
            points.push_back(Pt::Gfx::PointF( 245.21220508260075, 583.74913127120226 ) );
            points.push_back(Pt::Gfx::PointF( 245.66846097858243, 584.56602903930536 ) );
            points.push_back(Pt::Gfx::PointF( 246.12471687456417, 585.35394368400216 ) );
            points.push_back(Pt::Gfx::PointF( 246.58097277054591, 585.34782060904865 ) );
            points.push_back(Pt::Gfx::PointF( 247.03722866652765, 585.69862852656047 ) );
            points.push_back(Pt::Gfx::PointF( 247.49348456250931, 586.28877322447204 ) );
            points.push_back(Pt::Gfx::PointF( 247.94974045849105, 586.59016529238193 ) );
            points.push_back(Pt::Gfx::PointF( 248.40599635447279, 586.93031243486882 ) );
            points.push_back(Pt::Gfx::PointF( 248.86225225045453, 587.17193437312551 ) );
            points.push_back(Pt::Gfx::PointF( 249.31850814643627, 587.12975711052263 ) );
            points.push_back(Pt::Gfx::PointF( 249.77476404241793, 587.02303616776010 ) );
            points.push_back(Pt::Gfx::PointF( 250.23101993839967, 587.24225276679340 ) );
            points.push_back(Pt::Gfx::PointF( 250.68727583438141, 587.49052229017946 ) );
            points.push_back(Pt::Gfx::PointF( 251.14353173036315, 587.29499879705850 ) );
            points.push_back(Pt::Gfx::PointF( 251.59978762634489, 586.99280313490419 ) );
            points.push_back(Pt::Gfx::PointF( 252.05604352232655, 587.19304036032031 ) );
            points.push_back(Pt::Gfx::PointF( 252.51229941830829, 587.41639886914891 ) );
            points.push_back(Pt::Gfx::PointF( 252.96855531429003, 587.21439566273580 ) );
            points.push_back(Pt::Gfx::PointF( 253.42481121027177, 586.98051424185724 ) );
            points.push_back(Pt::Gfx::PointF( 253.88106710625351, 587.11560239248172 ) );
            points.push_back(Pt::Gfx::PointF( 254.33732300223517, 587.16321216083600 ) );
            points.push_back(Pt::Gfx::PointF( 254.79357889821691, 587.35360491327071 ) );
            points.push_back(Pt::Gfx::PointF( 255.24983479419865, 587.33367297325685 ) );
            points.push_back(Pt::Gfx::PointF( 255.70609069018039, 586.95491929036461 ) );
            points.push_back(Pt::Gfx::PointF( 256.16234658616213, 586.86400344952563 ) );
            points.push_back(Pt::Gfx::PointF( 256.61860248214379, 587.01218995010754 ) );
            points.push_back(Pt::Gfx::PointF( 257.07485837812555, 587.44723373479667 ) );
            points.push_back(Pt::Gfx::PointF( 257.53111427410727, 587.36095627299801 ) );
            points.push_back(Pt::Gfx::PointF( 257.98737017008898, 586.27338389173701 ) );
            points.push_back(Pt::Gfx::PointF( 258.44362606607075, 586.90113157912913 ) );
            points.push_back(Pt::Gfx::PointF( 258.89988196205240, 586.98569414261749 ) );
            points.push_back(Pt::Gfx::PointF( 259.35613785803412, 586.20555624326630 ) );
            points.push_back(Pt::Gfx::PointF( 259.81239375401589, 586.63188741605836 ) );
            points.push_back(Pt::Gfx::PointF( 260.26864964999766, 586.77636257959045 ) );
            points.push_back(Pt::Gfx::PointF( 260.72490554597937, 586.52355838072720 ) );
            points.push_back(Pt::Gfx::PointF( 261.18116144196102, 586.64207126580004 ) );
            points.push_back(Pt::Gfx::PointF( 261.63741733794279, 587.02951466395905 ) );
            points.push_back(Pt::Gfx::PointF( 262.09367323392451, 586.48771695452888 ) );
            points.push_back(Pt::Gfx::PointF( 262.54992912990622, 586.81325081659543 ) );
            points.push_back(Pt::Gfx::PointF( 263.00618502588799, 586.83267081176632 ) );
            points.push_back(Pt::Gfx::PointF( 263.46244092186964, 586.00747962535615 ) );
            points.push_back(Pt::Gfx::PointF( 263.91869681785136, 586.64068800642258 ) );
            points.push_back(Pt::Gfx::PointF( 264.37495271383312, 585.80859379145181 ) );
            points.push_back(Pt::Gfx::PointF( 264.83120860981489, 586.10542146391924 ) );
            points.push_back(Pt::Gfx::PointF( 265.28746450579661, 587.43238742695553 ) );
            points.push_back(Pt::Gfx::PointF( 265.74372040177826, 585.94202280248339 ) );
            points.push_back(Pt::Gfx::PointF( 266.19997629776003, 586.90473991257102 ) );
            points.push_back(Pt::Gfx::PointF( 266.65623219374174, 587.05847284593574 ) );
            points.push_back(Pt::Gfx::PointF( 267.11248808972346, 585.95147411842891 ) );
            points.push_back(Pt::Gfx::PointF( 267.56874398570517, 586.85013598983460 ) );
            points.push_back(Pt::Gfx::PointF( 268.02499988168688, 586.32475907176763 ) );
            points.push_back(Pt::Gfx::PointF( 268.48125577766859, 586.30029213185958 ) );
            points.push_back(Pt::Gfx::PointF( 268.93751167365036, 587.01253131389353 ) );
            points.push_back(Pt::Gfx::PointF( 269.39376756963213, 586.77451983221545 ) );
            points.push_back(Pt::Gfx::PointF( 269.85002346561373, 586.38939869673209 ) );
            points.push_back(Pt::Gfx::PointF( 270.30627936159550, 587.09506489195417 ) );
            points.push_back(Pt::Gfx::PointF( 270.76253525757727, 586.94817319901074 ) );
            points.push_back(Pt::Gfx::PointF( 271.21879115355898, 585.68903487910484 ) );
            points.push_back(Pt::Gfx::PointF( 271.67504704954069, 586.73192963452857 ) );
            points.push_back(Pt::Gfx::PointF( 272.13130294552241, 586.85174555482081 ) );
            points.push_back(Pt::Gfx::PointF( 272.58755884150412, 586.86496893882043 ) );
            points.push_back(Pt::Gfx::PointF( 273.04381473748583, 587.47119910627146 ) );
            points.push_back(Pt::Gfx::PointF( 273.50007063346760, 586.79490046916567 ) );
            points.push_back(Pt::Gfx::PointF( 273.95632652944937, 586.53457656971932 ) );
            points.push_back(Pt::Gfx::PointF( 274.41258242543097, 586.58505411689816 ) );
            points.push_back(Pt::Gfx::PointF( 274.86883832141274, 586.60789541360748 ) );
            points.push_back(Pt::Gfx::PointF( 275.32509421739451, 586.81335152704025 ) );
            points.push_back(Pt::Gfx::PointF( 275.78135011337622, 586.79957616029287 ) );
            points.push_back(Pt::Gfx::PointF( 276.23760600935793, 587.05728595781341 ) );
            points.push_back(Pt::Gfx::PointF( 276.69386190533965, 586.57225149205385 ) );
            points.push_back(Pt::Gfx::PointF( 277.15011780132136, 585.56460194626379 ) );
            points.push_back(Pt::Gfx::PointF( 277.60637369730307, 587.28137057508775 ) );
            points.push_back(Pt::Gfx::PointF( 278.06262959328484, 586.16509332506041 ) );
            points.push_back(Pt::Gfx::PointF( 278.51888548926661, 585.38824135327445 ) );
            points.push_back(Pt::Gfx::PointF( 278.97514138524821, 587.48018940653060 ) );
            points.push_back(Pt::Gfx::PointF( 279.43139728122998, 585.70022743624531 ) );
            points.push_back(Pt::Gfx::PointF( 279.88765317721175, 585.56310529088785 ) );
            points.push_back(Pt::Gfx::PointF( 280.34390907319346, 586.08354970760342 ) );
            points.push_back(Pt::Gfx::PointF( 280.80016496917517, 585.58064726831594 ) );
            points.push_back(Pt::Gfx::PointF( 281.25642086515688, 586.46073099443186 ) );
            points.push_back(Pt::Gfx::PointF( 281.71267676113860, 586.38321757943629 ) );
            points.push_back(Pt::Gfx::PointF( 282.16893265712031, 586.24413705056566 ) );
            points.push_back(Pt::Gfx::PointF( 282.62518855310208, 586.47661712292324 ) );
            points.push_back(Pt::Gfx::PointF( 283.08144444908385, 586.03576624893515 ) );
            points.push_back(Pt::Gfx::PointF( 283.53770034506545, 585.95125755789854 ) );
            points.push_back(Pt::Gfx::PointF( 283.99395624104721, 585.91118330481549 ) );
            points.push_back(Pt::Gfx::PointF( 284.45021213702898, 585.54583587542993 ) );
            points.push_back(Pt::Gfx::PointF( 284.90646803301070, 585.26386092021369 ) );
            points.push_back(Pt::Gfx::PointF( 285.36272392899241, 585.51288452101005 ) );
            points.push_back(Pt::Gfx::PointF( 285.81897982497412, 584.65368672402451 ) );
            points.push_back(Pt::Gfx::PointF( 286.27523572095583, 584.97843563049742 ) );
            points.push_back(Pt::Gfx::PointF( 286.73149161693755, 586.78669101701632 ) );
            points.push_back(Pt::Gfx::PointF( 287.18774751291932, 585.29438804655649 ) );
            points.push_back(Pt::Gfx::PointF( 287.64400340890109, 585.91419057042378 ) );
            points.push_back(Pt::Gfx::PointF( 288.10025930488268, 586.38055654365928 ) );
            points.push_back(Pt::Gfx::PointF( 288.55651520086445, 585.19905014810183 ) );
            points.push_back(Pt::Gfx::PointF( 289.01277109684622, 586.26882389973991 ) );
            points.push_back(Pt::Gfx::PointF( 289.46902699282793, 585.12809231955634 ) );
            points.push_back(Pt::Gfx::PointF( 289.92528288880965, 584.83728758591394 ) );
            points.push_back(Pt::Gfx::PointF( 290.38153878479136, 585.78618721775058 ) );
            points.push_back(Pt::Gfx::PointF( 290.83779468077307, 584.52206874118019 ) );
            points.push_back(Pt::Gfx::PointF( 291.29405057675478, 585.03483124628190 ) );
            points.push_back(Pt::Gfx::PointF( 291.75030647273655, 585.02096786355651 ) );
            points.push_back(Pt::Gfx::PointF( 292.20656236871832, 584.50508808662801 ) );
            points.push_back(Pt::Gfx::PointF( 292.66281826469992, 585.53878561074907 ) );
            points.push_back(Pt::Gfx::PointF( 293.11907416068169, 585.66800796504697 ) );
            points.push_back(Pt::Gfx::PointF( 293.57533005666346, 584.39002697565820 ) );
            points.push_back(Pt::Gfx::PointF( 294.03158595264517, 584.57928808994734 ) );
            points.push_back(Pt::Gfx::PointF( 294.48784184862689, 586.07559412587807 ) );
            points.push_back(Pt::Gfx::PointF( 294.94409774460860, 584.11040602932053 ) );
            points.push_back(Pt::Gfx::PointF( 295.40035364059031, 585.00891908736992 ) );
            points.push_back(Pt::Gfx::PointF( 295.85660953657202, 585.75771944452572 ) );
            points.push_back(Pt::Gfx::PointF( 296.31286543255379, 584.32981907585133 ) );
            points.push_back(Pt::Gfx::PointF( 296.76912132853556, 585.86728678688269 ) );
            points.push_back(Pt::Gfx::PointF( 297.22537722451716, 585.51037807681882 ) );
            points.push_back(Pt::Gfx::PointF( 297.68163312049893, 584.74559108878520 ) );
            points.push_back(Pt::Gfx::PointF( 298.13788901648070, 584.75052883637238 ) );
            points.push_back(Pt::Gfx::PointF( 298.59414491246241, 584.13950306511970 ) );
            points.push_back(Pt::Gfx::PointF( 299.05040080844412, 585.62860274620664 ) );
            points.push_back(Pt::Gfx::PointF( 299.50665670442584, 584.72976935107158 ) );
            points.push_back(Pt::Gfx::PointF( 299.96291260040755, 583.58318170971279 ) );
            points.push_back(Pt::Gfx::PointF( 300.41916849638926, 585.93357173763297 ) );
            points.push_back(Pt::Gfx::PointF( 300.87542439237103, 584.36093550731027 ) );
            points.push_back(Pt::Gfx::PointF( 301.33168028835280, 584.94649687411368 ) );
            points.push_back(Pt::Gfx::PointF( 301.78793618433440, 585.95851122747217 ) );
            points.push_back(Pt::Gfx::PointF( 302.24419208031617, 584.30931338548919 ) );
            points.push_back(Pt::Gfx::PointF( 302.70044797629794, 586.13513992098922 ) );
            points.push_back(Pt::Gfx::PointF( 303.15670387227965, 584.68188467423727 ) );
            points.push_back(Pt::Gfx::PointF( 303.61295976826136, 584.66350930136082 ) );
            points.push_back(Pt::Gfx::PointF( 304.06921566424307, 586.70997129755381 ) );
            points.push_back(Pt::Gfx::PointF( 304.52547156022479, 582.03120433495894 ) );
            points.push_back(Pt::Gfx::PointF( 304.98172745620650, 584.51877588081504 ) );
            points.push_back(Pt::Gfx::PointF( 305.43798335218827, 585.97762401054842 ) );
            points.push_back(Pt::Gfx::PointF( 305.89423924817004, 583.24986839472786 ) );
            points.push_back(Pt::Gfx::PointF( 306.35049514415164, 585.51759535266308 ) );
            points.push_back(Pt::Gfx::PointF( 306.80675104013341, 584.18323076023239 ) );
            points.push_back(Pt::Gfx::PointF( 307.26300693611518, 584.04849073013361 ) );
            points.push_back(Pt::Gfx::PointF( 307.71926283209689, 585.11359076966653 ) );
            points.push_back(Pt::Gfx::PointF( 308.17551872807860, 583.42903529590421 ) );
            points.push_back(Pt::Gfx::PointF( 308.63177462406031, 585.52372080021780 ) );
            points.push_back(Pt::Gfx::PointF( 309.08803052004203, 584.96564211820657 ) );
            points.push_back(Pt::Gfx::PointF( 309.54428641602374, 583.37246047406916 ) );
            points.push_back(Pt::Gfx::PointF( 310.00054231200551, 584.74619071608595 ) );
            points.push_back(Pt::Gfx::PointF( 310.45679820798716, 583.65825534023179 ) );
            points.push_back(Pt::Gfx::PointF( 310.91305410396888, 584.34136848538969 ) );
            points.push_back(Pt::Gfx::PointF( 311.36930999995064, 584.67628768186273 ) );
            points.push_back(Pt::Gfx::PointF( 311.82556589593241, 583.45147585648169 ) );
            points.push_back(Pt::Gfx::PointF( 312.28182179191413, 585.21211822978785 ) );
            points.push_back(Pt::Gfx::PointF( 312.73807768789578, 585.00976034348787 ) );
            points.push_back(Pt::Gfx::PointF( 313.19433358387755, 584.85344291387605 ) );
            points.push_back(Pt::Gfx::PointF( 313.65058947985926, 585.78475131840503 ) );
            points.push_back(Pt::Gfx::PointF( 314.10684537584098, 584.96083626055213 ) );
            points.push_back(Pt::Gfx::PointF( 314.56310127182275, 585.22861339407564 ) );
            points.push_back(Pt::Gfx::PointF( 315.01935716780440, 586.20965924356960 ) );
            points.push_back(Pt::Gfx::PointF( 315.47561306378611, 586.29605090556515 ) );
            points.push_back(Pt::Gfx::PointF( 315.93186895976788, 585.61624281691195 ) );
            points.push_back(Pt::Gfx::PointF( 316.38812485574965, 586.28124950626773 ) );
            points.push_back(Pt::Gfx::PointF( 316.84438075173136, 587.16860933840792 ) );
            points.push_back(Pt::Gfx::PointF( 317.30063664771302, 587.00418805001618 ) );
            points.push_back(Pt::Gfx::PointF( 317.75689254369479, 586.46636824022198 ) );
            points.push_back(Pt::Gfx::PointF( 318.21314843967650, 587.26090516052000 ) );
            points.push_back(Pt::Gfx::PointF( 318.66940433565821, 586.96805520311864 ) );
            points.push_back(Pt::Gfx::PointF( 319.12566023163998, 586.58895374115218 ) );
            points.push_back(Pt::Gfx::PointF( 319.58191612762164, 586.74278908223062 ) );
            points.push_back(Pt::Gfx::PointF( 320.03817202360335, 586.62405780573886 ) );
            points.push_back(Pt::Gfx::PointF( 320.49442791958512, 586.29777062945300 ) );
            points.push_back(Pt::Gfx::PointF( 320.95068381556689, 586.30231626772706 ) );
            points.push_back(Pt::Gfx::PointF( 321.40693971154860, 586.55144852828528 ) );
            points.push_back(Pt::Gfx::PointF( 321.86319560753026, 586.64562934918445 ) );
            points.push_back(Pt::Gfx::PointF( 322.31945150351203, 586.36858672030075 ) );
            points.push_back(Pt::Gfx::PointF( 322.77570739949374, 586.23023214249690 ) );
            points.push_back(Pt::Gfx::PointF( 323.23196329547545, 586.79434442787215 ) );
            points.push_back(Pt::Gfx::PointF( 323.68821919145722, 586.67905309366915 ) );
            points.push_back(Pt::Gfx::PointF( 324.14447508743888, 586.60015332561989 ) );
            points.push_back(Pt::Gfx::PointF( 324.60073098342059, 586.79528449714542 ) );
            points.push_back(Pt::Gfx::PointF( 325.05698687940236, 586.41610844294348 ) );
            points.push_back(Pt::Gfx::PointF( 325.51324277538413, 586.66639850236356 ) );
            points.push_back(Pt::Gfx::PointF( 325.96949867136584, 587.01860820573620 ) );
            points.push_back(Pt::Gfx::PointF( 326.42575456734750, 586.95550527466185 ) );
            points.push_back(Pt::Gfx::PointF( 326.88201046332927, 586.88337705170079 ) );
            points.push_back(Pt::Gfx::PointF( 327.33826635931098, 587.12177226118490 ) );
            points.push_back(Pt::Gfx::PointF( 327.79452225529269, 587.09548746452936 ) );
            points.push_back(Pt::Gfx::PointF( 328.25077815127446, 586.86161601870026 ) );
            points.push_back(Pt::Gfx::PointF( 328.70703404725612, 586.90661109332689 ) );
            points.push_back(Pt::Gfx::PointF( 329.16328994323783, 587.00104132561569 ) );
            points.push_back(Pt::Gfx::PointF( 329.61954583921960, 586.91080679990148 ) );
            points.push_back(Pt::Gfx::PointF( 330.07580173520137, 586.97517494791703 ) );
            points.push_back(Pt::Gfx::PointF( 330.53205763118308, 587.35190345351282 ) );
            points.push_back(Pt::Gfx::PointF( 330.98831352716473, 587.41847429186919 ) );
            points.push_back(Pt::Gfx::PointF( 331.44456942314650, 587.19355463573390 ) );
            points.push_back(Pt::Gfx::PointF( 331.90082531912822, 586.86558386770685 ) );
            points.push_back(Pt::Gfx::PointF( 332.35708121510993, 586.71677845136742 ) );
            return points;
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


