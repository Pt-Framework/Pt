class BenchmarkView : public Pt::Hmi::Control
{
    public:
        BenchmarkView()
        {}

    protected:
        virtual void onPaint(Pt::Hmi::PaintSurface& surface,
                             const Pt::Gfx::RectF& rect)
        {
            using namespace Pt::Gfx;


            int imageWidth = 305, imageHeight = 440;
            RectF imageRect = RectF( PointF(0, 0), SizeF(imageWidth, imageHeight) );
            Color background = Color::fromRgb8(0, 0, 0);

            Pt::Hmi::Painter painter(surface);

            Image image1( painter.format(), Size(imageWidth, imageHeight) );
            ImagePainter ip1(image1);

            Image image2( painter.format(), Size(imageWidth, imageHeight) );
            ImagePainter2 ip2(image2);
            ip2.setAntiAliasing(true);

            static bool doBenchmark = true;
            if(doBenchmark) {
                doBenchmark = false;
                onPaintContent(ip1, "IP1", true);
                onPaintContent(ip2, "IP2", true);
            }

            ip1.setBrush(background);
            ip1.fillRect(imageRect);

            ip2.setBrush(background);
            ip2.fillRect(imageRect);

            onPaintContent(ip1, "IP1", false);
            onPaintContent(ip2, "IP2", false);

            painter.drawImage(PointF(2, 2), image1);
            painter.drawImage(PointF(317, 2), image2);
        }

        virtual void onPaintContent(Pt::Gfx::Painter& painter, const Pt::String& text, bool benchmark)
        {

            using namespace Pt::Gfx;

            painter.setPen  ( Color::fromRgb8(164, 100, 255)  );
            painter.setFont ( Font("", 12) );
            painter.drawText( PointF(10, 20), text );

            Pt::Gfx::Pen   green1( Color::fromRgb8(0, 255, 0), 1, Pen::Solid, Pen::RoundCap, Pen::RoundJoin );
            Pt::Gfx::Pen   green2( Color::fromRgb8(0, 255, 0), 3, Pen::Solid, Pen::RoundCap, Pen::RoundJoin );
            Pt::Gfx::Pen   green9( Color::fromRgb8(0, 255, 0), 9, Pen::Solid, Pen::RoundCap, Pen::RoundJoin );

            Pt::Gfx::Brush redb  ( Color::fromRgb8(255, 0, 0) );

            int    x     = 10;
            int    y     = 30;
            double scale = 1.0;
            std::vector<Pt::Gfx::PointF> shape;

            // Polyline simple - scale 10x
            scale = 10.0;
            if(benchmark) {
            }
            else {
                shape = makeLineSimple(x, y, scale);
                painter.setPen(green1);
                painter.drawPolyline( &shape[0], shape.size() );
                y += 35;
                shape = makeLineSimple(x, y, scale);
                painter.setPen(green2);
                painter.drawPolyline( &shape[0], shape.size() );
                y += 35;
                shape = makeLineSimple(x, y, scale);
                painter.setPen(green9);
                painter.drawPolyline( &shape[0], shape.size() );
                y += 35;
            }
            y += 15;

            // Polyline complex - scale 1x
            scale = 1.0;
            if(benchmark) {
            }
            else {
                shape = makeLineComplex(x, y, scale);
                painter.setPen(green1);
                painter.drawPolyline( &shape[0], shape.size() );
                y += 35;
                shape = makeLineComplex(x, y, scale);
                painter.setPen(green2);
                painter.drawPolyline( &shape[0], shape.size() );
                y += 35;
            }
            y -= 35 * 2;
            x += 50;

            // Polyline complex - scale 5x
            scale = 5.0;
            if(benchmark) {
            }
            else {
                shape = makeLineComplex(x, y, scale);
                painter.setPen(green1);
                painter.drawPolyline( &shape[0], shape.size() );
                y += 35;
                shape = makeLineComplex(x, y, scale);
                painter.setPen(green2);
                painter.drawPolyline( &shape[0], shape.size() );
                y += 35;
            }
            x -= 50;

        }

        std::vector<Pt::Gfx::PointF> makeLineSimple(double x, double y, double scale)
        {
            std::vector<Pt::Gfx::PointF> points(4);
            points[0].set(x +  0 * scale, y + 0 * scale);
            points[1].set(x +  7 * scale, y + 3 * scale);
            points[2].set(x + 16 * scale, y + 0 * scale);
            points[3].set(x + 17 * scale, y + 3 * scale);
            return points;
        }

        std::vector<Pt::Gfx::PointF> makeLineComplex(double x, double y, double scale)
        {
            std::vector<Pt::Gfx::PointF> points;
            points.push_back( Pt::Gfx::PointF( x +  0.00000000000000 * scale, y + 6.52966615736159 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  0.45625589598171 * scale, y + 6.91674834760045 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  0.91251179196348 * scale, y + 6.96453771524307 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  1.36876768794519 * scale, y + 6.88198768561881 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  1.82502358392691 * scale, y + 6.96290222612220 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  2.28127947990862 * scale, y + 7.17906112735693 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  2.73753537589033 * scale, y + 7.15649813663867 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  3.19379127187204 * scale, y + 6.88817006419924 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  3.65004716785381 * scale, y + 6.63732031107691 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  4.10630306383553 * scale, y + 6.16115181113173 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  4.56255895981724 * scale, y + 5.20191763432103 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  5.01881485579895 * scale, y + 4.14723206817314 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  5.47507075178072 * scale, y + 3.16958600260398 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  5.93132664776243 * scale, y + 1.63551843480900 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  6.38758254374414 * scale, y + 1.04674745096725 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  6.84383843972586 * scale, y + 0.92796320764523 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  7.30009433570757 * scale, y + 0.32834713594787 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  7.75635023168928 * scale, y + 0.01281037671060 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  8.21260612767105 * scale, y + 0.00000000000000 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  8.66886202365276 * scale, y + 1.02919029246391 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  9.12511791963448 * scale, y + 3.03149634904446 * scale ) );
            points.push_back( Pt::Gfx::PointF( x +  9.58137381561619 * scale, y + 4.69946513466323 * scale ) );
            points.push_back( Pt::Gfx::PointF( x + 10.03762971159796 * scale, y + 5.85649317386549 * scale ) );
            points.push_back( Pt::Gfx::PointF( x + 10.49388560757967 * scale, y + 6.45130540747118 * scale ) );
            points.push_back( Pt::Gfx::PointF( x + 10.95014150356138 * scale, y + 6.78484039024283 * scale ) );
            points.push_back( Pt::Gfx::PointF( x + 11.40639739954310 * scale, y + 7.15711206112974 * scale ) );
            return points;
        }
};
