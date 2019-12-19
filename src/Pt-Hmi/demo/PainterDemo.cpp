
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/TabView.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Label.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/ImagePainter2.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/System/Logger.h>


class BasicStylesWidget : public Pt::Hmi::Control
{
    public:
        BasicStylesWidget()
        {}

        virtual ~BasicStylesWidget()
        {}

    protected:
        virtual void drawShapes(Pt::Gfx::Painter& painter, const Pt::String& text) = 0;

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

            drawShapes(painter,       "Native Painter");
            drawShapes(imagePainter,  "ImagePainter"  );
            drawShapes(imagePainter2, "ImagePainter2" );

            painter.drawImage(PointF(210, 0), image1);
            painter.drawImage(PointF(420, 0), image2);
        }
};


class LineStylesWidget : public BasicStylesWidget
{
    private:
        bool _userStyle;

    public:
        LineStylesWidget(bool userStyle)
        : _userStyle(userStyle)
        {}

        virtual ~LineStylesWidget()
        {}

    protected:
        virtual void drawShapes(Pt::Gfx::Painter& painter, const Pt::String& text)
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

            // User-defined style
            const Pt::uint8_t  userStyle[]  = { 1, 1, 3, 3 };
            const Pt::uint8_t* userStyleBeg = userStyle;
            const Pt::uint8_t* userStyleEnd = userStyle + sizeof(userStyle);


            if(1) {
                painter.setPen( lightBlue );
                painter.drawText( PointF(20, y + 10), "ButtCap");
                y += 20;

                Pen pen1Solid(red, 1, Pen::Solid, Pen::ButtCap);
                Pen pen2Solid(red, 4, Pen::Solid, Pen::ButtCap);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::ButtCap);

                if(_userStyle) {
                    pen1Solid = Pen(red, 1, userStyleBeg, userStyleEnd, Pen::ButtCap);
                    pen2Solid = Pen(red, 4, userStyleBeg, userStyleEnd, Pen::ButtCap);
                    pen3Solid = Pen(red, 9, userStyleBeg, userStyleEnd, Pen::ButtCap);
                }

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
                painter.drawText( PointF(20, y + 10), "SquareCap");
                y += 20;

                Pen pen2Solid(red, 4, Pen::Solid, Pen::SquareCap);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::SquareCap);

                if(_userStyle) {
                    pen2Solid = Pen(red, 4, userStyleBeg, userStyleEnd, Pen::SquareCap);
                    pen3Solid = Pen(red, 9, userStyleBeg, userStyleEnd, Pen::SquareCap);
                }

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
                painter.drawText( PointF(20, y + 10), "RoundCap");
                y += 20;

                Pen pen2Solid(red, 4, Pen::Solid, Pen::SquareCap);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::SquareCap);

                if(_userStyle) {
                    pen2Solid = Pen(red, 4, userStyleBeg, userStyleEnd, Pen::SquareCap);
                    pen3Solid = Pen(red, 9, userStyleBeg, userStyleEnd, Pen::SquareCap);
                }

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
        }
};


class CapStylesWidget : public BasicStylesWidget
{
    public:
        CapStylesWidget()
        {}

        virtual ~CapStylesWidget()
        {}

    protected:

        virtual void drawShapes(Pt::Gfx::Painter& painter, const Pt::String& text)
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
                painter.drawText( PointF(20, y + 10), "ButtCap");
                y += 20;

                Pen pen1Solid(red,  1, Pen::Solid, Pen::ButtCap);
                Pen pen2Solid(red,  4, Pen::Solid, Pen::ButtCap);
                Pen pen3Solid(red,  9, Pen::Solid, Pen::ButtCap);
                Pen pen4Solid(red, 14, Pen::Solid, Pen::ButtCap);

                painter.setPen(pen1Solid); painter.drawLine( PointF(10, y), PointF(180, y) ); y += 15;
                painter.setPen(pen2Solid); painter.drawLine( PointF(10, y), PointF(180, y) ); y += 15;
                painter.setPen(pen3Solid); painter.drawLine( PointF(10, y), PointF(180, y) ); y += 15;
                painter.setPen(pen4Solid); painter.drawLine( PointF(10, y), PointF(180, y) ); y += 15;

                y += 5;
            }

            if(1) {
                painter.setPen( lightBlue );
                painter.drawText( PointF(20, y + 10), "SquareCap");
                y += 20;

                Pen pen1Solid(red,  1, Pen::Solid, Pen::SquareCap);
                Pen pen2Solid(red,  4, Pen::Solid, Pen::SquareCap);
                Pen pen3Solid(red,  9, Pen::Solid, Pen::SquareCap);
                Pen pen4Solid(red, 14, Pen::Solid, Pen::SquareCap);

                painter.setPen(pen1Solid); painter.drawLine( PointF(10, y), PointF(180, y) ); y += 15;
                painter.setPen(pen2Solid); painter.drawLine( PointF(10, y), PointF(180, y) ); y += 15;
                painter.setPen(pen3Solid); painter.drawLine( PointF(10, y), PointF(180, y) ); y += 15;
                painter.setPen(pen4Solid); painter.drawLine( PointF(10, y), PointF(180, y) ); y += 15;

                y += 5;
            }

            if(1) {
                painter.setPen( lightBlue );
                painter.drawText( PointF(20, y + 10), "RoundCap");
                y += 20;

                Pen pen1Solid(red,  1, Pen::Solid, Pen::RoundCap);
                Pen pen2Solid(red,  4, Pen::Solid, Pen::RoundCap);
                Pen pen3Solid(red,  9, Pen::Solid, Pen::RoundCap);
                Pen pen4Solid(red, 14, Pen::Solid, Pen::RoundCap);

                painter.setPen(pen1Solid); painter.drawLine( PointF(10, y), PointF(180, y) ); y += 15;
                painter.setPen(pen2Solid); painter.drawLine( PointF(10, y), PointF(180, y) ); y += 15;
                painter.setPen(pen3Solid); painter.drawLine( PointF(10, y), PointF(180, y) ); y += 15;
                painter.setPen(pen4Solid); painter.drawLine( PointF(10, y), PointF(180, y) ); y += 15;

                y += 5;
            }
        }
};


class JoinStylesWidget : public BasicStylesWidget
{
    public:
        JoinStylesWidget()
        {}

        virtual ~JoinStylesWidget()
        {}

    protected:

        virtual void drawShapes(Pt::Gfx::Painter& painter, const Pt::String& text)
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
                painter.drawText( PointF(20, y + 10), "ButtCap - BevelJoin");
                y += 20;

                Pen pen1Solid(red, 1, Pen::Solid, Pen::ButtCap, Pen::BevelJoin);
                Pen pen2Solid(red, 4, Pen::Solid, Pen::ButtCap, Pen::BevelJoin);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::ButtCap, Pen::BevelJoin);

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
                painter.drawText( PointF(20, y + 10), "ButtCap - MiterJoin");
                y += 20;

                Pen pen1Solid(red, 1, Pen::Solid, Pen::ButtCap, Pen::MiterJoin);
                Pen pen2Solid(red, 4, Pen::Solid, Pen::ButtCap, Pen::MiterJoin);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::ButtCap, Pen::MiterJoin);

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
                painter.drawText( PointF(20, y + 10), "ButtCap - RoundJoin");
                y += 20;

                Pen pen1Solid(red, 1, Pen::Solid, Pen::ButtCap, Pen::RoundJoin);
                Pen pen2Solid(red, 4, Pen::Solid, Pen::ButtCap, Pen::RoundJoin);
                Pen pen3Solid(red, 9, Pen::Solid, Pen::ButtCap, Pen::RoundJoin);

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
        : _tabLineStyles1(false), _tabLineStyles2(true)
        {
            _tabLineStyles1.setMargin(2);
            _tabLineStyles2.setMargin(2);
            _tabCapStyles  .setMargin(2);
            _tabJoinStyles .setMargin(2);

            _tabView.addTab(_tabLineStyles1, "Line Styles 1");
            _tabView.addTab(_tabLineStyles2, "Line Styles 2");
            _tabView.addTab(_tabCapStyles,   "Cap Styles"   );
            _tabView.addTab(_tabJoinStyles,  "Join Styles"  );
            _tabView.setPadding(8);
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
        LineStylesWidget _tabLineStyles1;
        LineStylesWidget _tabLineStyles2;
        CapStylesWidget  _tabCapStyles;
        JoinStylesWidget _tabJoinStyles;
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

