class Test2View : public Pt::Hmi::Control
{
    public:
        Test2View()
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

            painter.setPen  ( Color::fromRgb8(255, 0, 0)  );
            painter.setFont ( Font("", 16) );
            painter.drawText( PointF(200, 50), Pt::String("--- NOT WORKING PROPERLY ---") );

            Pt::Gfx::Pen green1sf( Color::fromRgb8(0, 255, 0), 1, Pen::Solid, Pen::FlatCap,   Pen::NoJoin );

            Pt::Gfx::Pen green7sf( Color::fromRgb8(0, 255, 0), 7, Pen::Solid, Pen::FlatCap,   Pen::NoJoin );
            Pt::Gfx::Pen green7ss( Color::fromRgb8(0, 255, 0), 7, Pen::Solid, Pen::SquareCap, Pen::NoJoin );
            Pt::Gfx::Pen green7sr( Color::fromRgb8(0, 255, 0), 7, Pen::Solid, Pen::RoundCap,  Pen::NoJoin );

            Pt::Gfx::Pen green7df( Color::fromRgb8(0, 255, 0), 7, Pen::Dot,   Pen::FlatCap,   Pen::NoJoin );
            Pt::Gfx::Pen green7ds( Color::fromRgb8(0, 255, 0), 7, Pen::Dot,   Pen::SquareCap, Pen::NoJoin );
            Pt::Gfx::Pen green7dr( Color::fromRgb8(0, 255, 0), 7, Pen::Dot,   Pen::RoundCap,  Pen::NoJoin );

            Pt::Gfx::Pen green7hf( Color::fromRgb8(0, 255, 0), 7, Pen::Dash,  Pen::FlatCap,   Pen::NoJoin );
            Pt::Gfx::Pen green7hs( Color::fromRgb8(0, 255, 0), 7, Pen::Dash,  Pen::SquareCap, Pen::NoJoin );
            Pt::Gfx::Pen green7hr( Color::fromRgb8(0, 255, 0), 7, Pen::Dash,  Pen::RoundCap,  Pen::NoJoin );

            std::vector<Pt::Gfx::PointF> shape;

            int x = 30;
            int y = 30;

            IP2_DEBUG::TEST_SMOOTH_CURVE_HACK = true;

#if 1
            shape = makeTestShape1(x, y, 1.0);
            painter.setPen(green1sf);
            painter.drawPolyline( &shape[0], shape.size() );
            y += 100;
#endif

#if 1
            shape = makeTestShape1(x, y, 1.0);
            painter.setPen(green7sf);
            painter.drawPolyline( &shape[0], shape.size() );
            x += 100;
            shape = makeTestShape1(x, y, 1.0);
            painter.setPen(green7ss);
            painter.drawPolyline( &shape[0], shape.size() );
            x += 100;
            shape = makeTestShape1(x, y, 1.0);
            painter.setPen(green7sr);
            painter.drawPolyline( &shape[0], shape.size() );
            x -= 100 * 2;
            y += 100;
#endif

#if 1
            shape = makeTestShape1(x, y, 1.0);
            painter.setPen(green7df);
            painter.drawPolyline( &shape[0], shape.size() );
            x += 100;
            shape = makeTestShape1(x, y, 1.0);
            painter.setPen(green7ds);
            painter.drawPolyline( &shape[0], shape.size() );
            x += 100;
            shape = makeTestShape1(x, y, 1.0);
            painter.setPen(green7dr);
            painter.drawPolyline( &shape[0], shape.size() );
            x -= 100 * 2;
            y += 100;
#endif

#if 1
            shape = makeTestShape1(x, y, 1.0);
            painter.setPen(green7hf);
            painter.drawPolyline( &shape[0], shape.size() );
            x += 100;
            shape = makeTestShape1(x, y, 1.0);
            painter.setPen(green7hs);
            painter.drawPolyline( &shape[0], shape.size() );
            x += 100;
            shape = makeTestShape1(x, y, 1.0);
            painter.setPen(green7hr);
            painter.drawPolyline( &shape[0], shape.size() );
            x -= 100 * 2;
            y += 100;
#endif

            IP2_DEBUG::TEST_SMOOTH_CURVE_HACK = false;
        }

        std::vector<Pt::Gfx::PointF> makeTestShape1(double x, double y, double scale)
        {
            std::vector<Pt::Gfx::PointF> points;
            points.push_back( Pt::Gfx::PointF( x + 75.36458587646484 * scale, y + 33.35655212402344 * scale ) );
            points.push_back( Pt::Gfx::PointF( x + 69.13257598876953 * scale, y + 24.35220336914062 * scale ) );
            points.push_back( Pt::Gfx::PointF( x + 61.51705169677734 * scale, y + 16.48329162597656 * scale ) );
            points.push_back( Pt::Gfx::PointF( x + 52.72142791748047 * scale, y +  9.96001052856445 * scale ) );
            points.push_back( Pt::Gfx::PointF( x + 42.98067474365234 * scale, y +  4.95662689208984 * scale ) );
            points.push_back( Pt::Gfx::PointF( x + 32.55498504638672 * scale, y +  1.60679626464844 * scale ) );
            points.push_back( Pt::Gfx::PointF( x + 21.72288513183594 * scale, y +  0.00000000000000 * scale ) );
            points.push_back( Pt::Gfx::PointF( x + 10.77372741699219 * scale, y +  0.17917633056641 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  0.00000000000000 * scale, y +  2.13952636718750 * scale ) );
            return points;
        }
};
