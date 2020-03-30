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

                std::vector<std::string> result1, result2;
                onPaintContent(ip1, "IP1", &result1);
                onPaintContent(ip2, "IP2", &result2);

                if(ip2.isAntiAliasing()) fprintf(stderr, "IP2 WITH AA\n\n");
                else                     fprintf(stderr, "IP2 WITHOUT AA\n\n");

#if 0
                for(size_t i = 0; i < result1.size(); ++i) {
                    fprintf(stderr, "%s\n", result1[i].c_str());
                }
                for(size_t i = 0; i < result2.size(); ++i) {
                    fprintf(stderr, "%s\n", result2[i].c_str());
                }
                fprintf(stderr, "\n");
#else
                for(size_t i = 0; i < result1.size(); ++i) {
                    std::string r1 = result1[i];
                    std::string r2 = result2[i];
                    std::string r3;
                    std::string r4;
                    if(r1[0] == 0 || r2[0] == 0) continue;
                    if(r1[r1.length() - 1] == 'A' && r2[r2.length() - 1] == 'A') {
                        const char idx = r1[r1.length() - 2];
                        for(size_t j = 0; j < result1.size(); ++j) {
                            r3 = result1[j];
                            r4 = result2[j];
                            if(r3[r3.length() - 1] == 'B' && r3[r3.length() - 2] == idx &&
                               r4[r4.length() - 1] == 'B' && r4[r4.length() - 2] == idx
                            ) {
                                result1[i][0] = 0;
                                result1[j][0] = 0;
                                result2[i][0] = 0;
                                result2[j][0] = 0;
                                r1[r1.length() - 3] = 0;
                                r2[r2.length() - 3] = 0;
                                r3[r3.length() - 3] = 0;
                                r4[r4.length() - 3] = 0;
                                break;
                            }
                        }
                    }
                                    fprintf(stderr, "%s\n", r1.c_str());
                    if(!r3.empty()) fprintf(stderr, "%s\n", r3.c_str());
                                    fprintf(stderr, "%s\n", r2.c_str());
                    if(!r4.empty()) fprintf(stderr, "%s\n", r4.c_str());
                                    fprintf(stderr, "\n");
                }
#endif
            }

            ip1.setBrush(background);
            ip1.fillRect(imageRect);

            ip2.setBrush(background);
            ip2.fillRect(imageRect);

//#define SOURCE_OVER

#ifdef SOURCE_OVER
            ip1.setCompositionMode(CompositionMode::SourceOver);
            ip2.setCompositionMode(CompositionMode::SourceOver);
#endif

            onPaintContent(ip1, "IP1", 0);
            onPaintContent(ip2, "IP2", 0);

            painter.drawImage(PointF(2, 2), image1);
            painter.drawImage(PointF(317, 2), image2);
        }

        virtual void onPaintContent(Pt::Gfx::Painter& painter, const char* text, std::vector<std::string>* brBuff)
        {
            using namespace Pt::Gfx;

            painter.setPen  ( Color::fromRgb8(164, 100, 255)  );
            painter.setFont ( Font("", 12) );
            painter.drawText( PointF(10, 20), Pt::String(text) );

#ifdef SOURCE_OVER
            const Pt::uint8_t alpha = 175;
#else
            const Pt::uint8_t alpha = 255;
#endif

            Pt::Gfx::Pen   green1( Color::fromRgb8(  0, 255,   0, alpha), 1, Pen::Solid, Pen::RoundCap, Pen::RoundJoin );
            Pt::Gfx::Pen   green2( Color::fromRgb8(  0, 255,   0, alpha), 3, Pen::Solid, Pen::RoundCap, Pen::RoundJoin );
            Pt::Gfx::Pen   green9( Color::fromRgb8(  0, 255,   0, alpha), 9, Pen::Solid, Pen::RoundCap, Pen::RoundJoin );

            Pt::Gfx::Pen   cyan1 ( Color::fromRgb8(  0, 255, 255, alpha), 1, Pen::Solid, Pen::RoundCap, Pen::NoJoin );
            Pt::Gfx::Pen   cyan2 ( Color::fromRgb8(  0, 255, 255, alpha), 3, Pen::Solid, Pen::RoundCap, Pen::NoJoin );

            Pt::Gfx::Brush redb  ( Color::fromRgb8(255,   0,   0, alpha) );

            int    x     = 10;
            int    y     = 30;
            double scale = 1.0;
            std::vector<Pt::Gfx::PointF> shape;

            // Get ImagePainter2
            ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(&painter);

            // Benchmark loop count and flag
            const int  loopCount = 100;
                  bool fill      = false;

#define BENCHMARK_CODE(DESC, INFO, SIZE, SCALE)                                 \
                do {                                                            \
                    char buff[128];                                             \
                    Pt::int64_t sum = 0;                                        \
                    for(int i = 0; i < loopCount; ++i) {                        \
                        Pt::System::Clock clock;                                \
                        if(fill) {                                              \
                            clock.start();                                      \
                            if(ip2)                                             \
                                ip2->fillPolygon_NR( &shape[0], shape.size() ); \
                            else                                                \
                                painter.fillPolygon( &shape[0], shape.size() ); \
                            sum += clock.stop().toUSecs();                      \
                        }                                                       \
                        else {                                                  \
                            clock.start();                                      \
                            painter.drawPolyline( &shape[0], shape.size() );    \
                            sum += clock.stop().toUSecs();                      \
                        }                                                       \
                    }                                                           \
                    sum /= loopCount;                                           \
                    sprintf(buff, "%s [%s] [SCALE %4.1f SIZE %d] %3zd %s",      \
                                   text, DESC, SCALE, SIZE, sum, INFO);         \
                    brBuff->push_back(buff);                                    \
                } while(false)

            // Polyline simple - scale 10x
            scale = 10.0;
            if(brBuff) {
                shape = makeLineSimple(0, 0, scale);
                painter.setPen(green1); BENCHMARK_CODE("Polyline Simple ", "",   1, scale);
                painter.setPen(green2); BENCHMARK_CODE("Polyline Simple ", "2A", 2, scale);
                painter.setPen(green9); BENCHMARK_CODE("Polyline Simple ", "9A", 9, scale);
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
            if(brBuff) {
                shape = makeLineComplex(0, 0, scale);
                painter.setPen(cyan1); BENCHMARK_CODE("Polyline Complex", "",   1, scale);
                painter.setPen(cyan2); BENCHMARK_CODE("Polyline Complex", "1A", 2, scale);
            }
            else {
                shape = makeLineComplex(x, y, scale);
                painter.setPen(cyan1);
                painter.drawPolyline( &shape[0], shape.size() );
                y += 35;
                shape = makeLineComplex(x, y, scale);
                painter.setPen(cyan2);
                painter.drawPolyline( &shape[0], shape.size() );
                y += 35;
            }
            y -= 35 * 2;
            x += 50;

            // Polyline complex - scale 5x
            scale = 5.0;
            if(brBuff) {
                shape = makeLineComplex(0, 0, scale);
                painter.setPen(cyan1); BENCHMARK_CODE("Polyline Complex", "",   1, scale);
                painter.setPen(cyan2); BENCHMARK_CODE("Polyline Complex", "5A", 2, scale);
            }
            else {
                shape = makeLineComplex(x, y, scale);
                painter.setPen(cyan1);
                painter.drawPolyline( &shape[0], shape.size() );
                y += 35;
                shape = makeLineComplex(x, y, scale);
                painter.setPen(cyan2);
                painter.drawPolyline( &shape[0], shape.size() );
                y += 35;
            }
            y += 15;
            x -= 50;

            // Set flag
            fill = true;

            // Polygon - simple
            if(brBuff) {
                painter.setBrush(redb);
                shape = makePolygonSimple_S10_P2_RC_RJ(0, 0); BENCHMARK_CODE("Polygon  Simple ", "2B", 2, 10.0);
                shape = makePolygonSimple_S10_P9_RC_RJ(0, 0); BENCHMARK_CODE("Polygon  Simple ", "9B", 9, 10.0);
            }
            else {
                shape = makePolygonSimple_S10_P2_RC_RJ(x, y);
                painter.setBrush(redb);
                if(ip2) ip2->fillPolygon_NR( &shape[0], shape.size() );
                else    painter.fillPolygon( &shape[0], shape.size() );
                y += 35;
                shape = makePolygonSimple_S10_P9_RC_RJ(x, y);
                painter.setBrush(redb);
                if(ip2) ip2->fillPolygon_NR( &shape[0], shape.size() );
                else    painter.fillPolygon( &shape[0], shape.size() );
                y += 35;
            }
            y += 15;

            // Polygon - complex
            if(brBuff) {
                painter.setBrush(redb);
                shape = makePolygonComplex_S1_P2_RC_NJ(0, 0); BENCHMARK_CODE("Polygon  Complex", "1B", 2, 1.0);
                shape = makePolygonComplex_S5_P2_RC_NJ(0, 0); BENCHMARK_CODE("Polygon  Complex", "5B", 2, 5.0);
            }
            else {
                shape = makePolygonComplex_S1_P2_RC_NJ(x, y);
                painter.setBrush(redb);
                if(ip2) ip2->fillPolygon_NR( &shape[0], shape.size() );
                else    painter.fillPolygon( &shape[0], shape.size() );
                x += 50;
                shape = makePolygonComplex_S5_P2_RC_NJ(x, y);
                painter.setBrush(redb);
                if(ip2) ip2->fillPolygon_NR( &shape[0], shape.size() );
                else    painter.fillPolygon( &shape[0], shape.size() );
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

        std::vector<Pt::Gfx::PointF> makePolygonSimple_S10_P2_RC_RJ(double x, double y)
        {
            std::vector<Pt::Gfx::PointF> points;
            points.push_back( Pt::Gfx::PointF( x +   1.77263683080673, y +  0.08085495233536 ) );
            points.push_back( Pt::Gfx::PointF( x +   0.00000000000000, y +  0.40912103652954 ) );
            points.push_back( Pt::Gfx::PointF( x +   0.98479825258255, y +  1.91914504766464 ) );
            points.push_back( Pt::Gfx::PointF( x +  71.37871754169464, y + 32.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x + 160.74607288837433, y +  2.26439464092255 ) );
            points.push_back( Pt::Gfx::PointF( x + 170.43003284931183, y + 31.31622695922852 ) );
            points.push_back( Pt::Gfx::PointF( x + 171.85305225849152, y + 32.42302513122559 ) );
            points.push_back( Pt::Gfx::PointF( x + 172.32740223407745, y + 30.68377304077148 ) );
            points.push_back( Pt::Gfx::PointF( x + 162.37871754169464, y +  1.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x + 162.26760303974152, y +  0.44444447755814 ) );
            points.push_back( Pt::Gfx::PointF( x + 161.93425953388214, y +  0.11111116409302 ) );
            points.push_back( Pt::Gfx::PointF( x + 161.37871754169464, y +  0.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  71.42318165302277, y + 29.93108749389648 ) );
            return points;
        }

        std::vector<Pt::Gfx::PointF> makePolygonSimple_S10_P9_RC_RJ(double x, double y)
        {
            std::vector<Pt::Gfx::PointF> points;
            points.push_back( Pt::Gfx::PointF( x +   5.42222225666046, y +  0.50000023841858 ) );
            points.push_back( Pt::Gfx::PointF( x +   1.06666684150696, y +  1.54444503784180 ) );
            points.push_back( Pt::Gfx::PointF( x +   0.00000000000000, y +  4.01111161708832 ) );
            points.push_back( Pt::Gfx::PointF( x +   2.22222220897675, y +  7.90000033378601 ) );
            points.push_back( Pt::Gfx::PointF( x +  71.82222223281860, y + 38.20000028610229 ) );
            points.push_back( Pt::Gfx::PointF( x +  74.82222223281860, y + 38.20000028610229 ) );
            points.push_back( Pt::Gfx::PointF( x + 161.29196405410767, y +  9.26470613479614 ) );
            points.push_back( Pt::Gfx::PointF( x + 170.02221918106079, y + 35.50000143051147 ) );
            points.push_back( Pt::Gfx::PointF( x + 173.66665887832642, y + 38.01111173629761 ) );
            points.push_back( Pt::Gfx::PointF( x + 176.19999933242798, y + 37.14444684982300 ) );
            points.push_back( Pt::Gfx::PointF( x + 177.62222528457642, y + 32.90000104904175 ) );
            points.push_back( Pt::Gfx::PointF( x + 167.82222223281860, y +  3.20000028610229 ) );
            points.push_back( Pt::Gfx::PointF( x + 167.30223321914673, y +  1.80000042915344 ) );
            points.push_back( Pt::Gfx::PointF( x + 166.54222345352173, y +  0.80000066757202 ) );
            points.push_back( Pt::Gfx::PointF( x + 165.54223871231079, y +  0.20000028610229 ) );
            points.push_back( Pt::Gfx::PointF( x + 164.30223321914673, y +  0.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x + 162.82222223281860, y +  0.20000028610229 ) );
            points.push_back( Pt::Gfx::PointF( x +  74.00096368789673, y + 29.92405462265015 ) );
            return points;
        }

        std::vector<Pt::Gfx::PointF> makePolygonComplex_S1_P2_RC_NJ(double x, double y)
        {
            std::vector<Pt::Gfx::PointF> points;
            points.push_back( Pt::Gfx::PointF( x +   1.50000000000000, y +  6.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   0.00000000000000, y +  7.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   1.50000000000000, y +  8.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   2.08578634262085, y +  8.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   2.79289317131042, y +  8.70710659027100 ) );
            points.push_back( Pt::Gfx::PointF( x +   3.50000000000000, y +  8.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   4.20710682868958, y +  8.70710659027100 ) );
            points.push_back( Pt::Gfx::PointF( x +   4.91421318054199, y +  8.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   5.50000000000000, y +  8.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   5.50000000000000, y +  7.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   6.50000000000000, y +  7.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   6.50000000000000, y +  6.41421127319336 ) );
            points.push_back( Pt::Gfx::PointF( x +   7.20710659027100, y +  5.70710659027100 ) );
            points.push_back( Pt::Gfx::PointF( x +   6.50000000000000, y +  5.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   7.50000000000000, y +  5.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   7.50000000000000, y +  4.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   7.50000000000000, y +  3.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   7.50000000000000, y +  3.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   7.50000000000000, y +  2.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   8.50000000000000, y +  2.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   8.50000000000000, y +  2.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   8.50000000000000, y +  2.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   8.50000000000000, y +  2.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   8.50000000000000, y +  2.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   9.50000000000000, y +  2.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   8.60557270050049, y +  2.44721364974976 ) );
            points.push_back( Pt::Gfx::PointF( x +   9.50000000000000, y +  4.23606872558594 ) );
            points.push_back( Pt::Gfx::PointF( x +   9.50000000000000, y +  5.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  10.50000000000000, y +  5.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   9.79289340972900, y +  5.70710659027100 ) );
            points.push_back( Pt::Gfx::PointF( x +  10.50000000000000, y +  6.41421508789062 ) );
            points.push_back( Pt::Gfx::PointF( x +  10.50000000000000, y +  7.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  11.50000000000000, y +  7.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  10.79289340972900, y +  7.70710659027100 ) );
            points.push_back( Pt::Gfx::PointF( x +  11.79289340972900, y +  8.70710659027100 ) );
            points.push_back( Pt::Gfx::PointF( x +  13.56066036224365, y +  9.06066036224365 ) );
            points.push_back( Pt::Gfx::PointF( x +  13.20710659027100, y +  7.29289340972900 ) );
            points.push_back( Pt::Gfx::PointF( x +  12.50000000000000, y +  6.58578872680664 ) );
            points.push_back( Pt::Gfx::PointF( x +  12.50000000000000, y +  6.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  11.50000000000000, y +  6.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  12.20710659027100, y +  5.29289340972900 ) );
            points.push_back( Pt::Gfx::PointF( x +  11.50000000000000, y +  4.58578681945801 ) );
            points.push_back( Pt::Gfx::PointF( x +  11.50000000000000, y +  4.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  10.50000000000000, y +  4.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  11.39442729949951, y +  3.55278635025024 ) );
            points.push_back( Pt::Gfx::PointF( x +  10.50000000000000, y +  1.76393318176270 ) );
            points.push_back( Pt::Gfx::PointF( x +  10.50000000000000, y +  1.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   9.50000000000000, y +  1.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   9.50000000000000, y +  0.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   8.50000000000000, y +  0.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   7.50000000000000, y +  0.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   7.50000000000000, y +  1.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   6.50000000000000, y +  1.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   6.50000000000000, y +  1.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   6.50000000000000, y +  1.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   6.50000000000000, y +  2.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   5.50000000000000, y +  2.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   5.50000000000000, y +  4.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   5.50000000000000, y +  4.58578681945801 ) );
            points.push_back( Pt::Gfx::PointF( x +   4.79289317131042, y +  5.29289340972900 ) );
            points.push_back( Pt::Gfx::PointF( x +   5.50000000000000, y +  6.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   4.50000000000000, y +  6.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   4.50000000000000, y +  6.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   4.50000000000000, y +  6.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   4.50000000000000, y +  7.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   3.79289317131042, y +  6.29289340972900 ) );
            points.push_back( Pt::Gfx::PointF( x +   3.49999976158142, y +  6.58578681945801 ) );
            points.push_back( Pt::Gfx::PointF( x +   3.20710682868958, y +  6.29289340972900 ) );
            points.push_back( Pt::Gfx::PointF( x +   2.50000000000000, y +  7.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   2.50000000000000, y +  6.00000000000000 ) );
            return points;
        }

        std::vector<Pt::Gfx::PointF> makePolygonComplex_S5_P2_RC_NJ(double x, double y)
        {
            std::vector<Pt::Gfx::PointF> points;
            points.push_back( Pt::Gfx::PointF( x +   1.76776689291000, y + 32.29289245605469 ) );
            points.push_back( Pt::Gfx::PointF( x +   0.00000000000000, y + 31.93934059143066 ) );
            points.push_back( Pt::Gfx::PointF( x +   0.35355335474014, y + 33.70710754394531 ) );
            points.push_back( Pt::Gfx::PointF( x +   2.35355329513550, y + 35.70710754394531 ) );
            points.push_back( Pt::Gfx::PointF( x +   3.06066012382507, y + 35.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   3.06066012382507, y + 36.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   5.06066012382507, y + 36.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   7.06066012382507, y + 36.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   9.82460665702820, y + 36.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  11.61344695091248, y + 36.89442825317383 ) );
            points.push_back( Pt::Gfx::PointF( x +  12.06066012382507, y + 36.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  12.06066012382507, y + 37.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  14.06066012382507, y + 37.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  14.06066012382507, y + 36.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  14.50787329673767, y + 36.89442825317383 ) );
            points.push_back( Pt::Gfx::PointF( x +  16.44429564476013, y + 35.92621231079102 ) );
            points.push_back( Pt::Gfx::PointF( x +  19.37688708305359, y + 34.94868469238281 ) );
            points.push_back( Pt::Gfx::PointF( x +  19.06066012382507, y + 34.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  19.89271044731140, y + 34.55469894409180 ) );
            points.push_back( Pt::Gfx::PointF( x +  21.89271044731140, y + 31.55470085144043 ) );
            points.push_back( Pt::Gfx::PointF( x +  21.06066012382507, y + 31.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  21.95508646965027, y + 31.44721412658691 ) );
            points.push_back( Pt::Gfx::PointF( x +  23.95508646965027, y + 27.44721412658691 ) );
            points.push_back( Pt::Gfx::PointF( x +  23.06066012382507, y + 27.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  23.95508646965027, y + 27.44721412658691 ) );
            points.push_back( Pt::Gfx::PointF( x +  26.95508646965027, y + 21.44721412658691 ) );
            points.push_back( Pt::Gfx::PointF( x +  26.06066012382507, y + 21.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  26.98913645744324, y + 21.37139129638672 ) );
            points.push_back( Pt::Gfx::PointF( x +  28.98913645744324, y + 16.37139034271240 ) );
            points.push_back( Pt::Gfx::PointF( x +  28.06066012382507, y + 16.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  29.02218317985535, y + 16.27472114562988 ) );
            points.push_back( Pt::Gfx::PointF( x +  30.97929644584656, y +  9.42481899261475 ) );
            points.push_back( Pt::Gfx::PointF( x +  32.70239329338074, y +  6.84018230438232 ) );
            points.push_back( Pt::Gfx::PointF( x +  35.37688708305359, y +  5.94868326187134 ) );
            points.push_back( Pt::Gfx::PointF( x +  35.06066012382507, y +  5.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  35.89271044731140, y +  5.55470037460327 ) );
            points.push_back( Pt::Gfx::PointF( x +  37.74540209770203, y +  2.77566123008728 ) );
            points.push_back( Pt::Gfx::PointF( x +  39.29673266410828, y +  2.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  41.38362765312195, y +  2.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  43.09808611869812, y +  6.28613948822021 ) );
            points.push_back( Pt::Gfx::PointF( x +  45.08008074760437, y + 16.19611644744873 ) );
            points.push_back( Pt::Gfx::PointF( x +  46.06066012382507, y + 16.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  45.09051775932312, y + 16.24253559112549 ) );
            points.push_back( Pt::Gfx::PointF( x +  47.09051775932312, y + 24.24253654479980 ) );
            points.push_back( Pt::Gfx::PointF( x +  48.06066012382507, y + 24.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  47.16623187065125, y + 24.44721412658691 ) );
            points.push_back( Pt::Gfx::PointF( x +  50.16623187065125, y + 30.44721412658691 ) );
            points.push_back( Pt::Gfx::PointF( x +  51.06066012382507, y + 30.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  50.22860980033875, y + 30.55470085144043 ) );
            points.push_back( Pt::Gfx::PointF( x +  52.22860980033875, y + 33.55469894409180 ) );
            points.push_back( Pt::Gfx::PointF( x +  53.06066012382507, y + 33.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  52.61344790458679, y + 33.89442825317383 ) );
            points.push_back( Pt::Gfx::PointF( x +  54.55773043632507, y + 34.86657714843750 ) );
            points.push_back( Pt::Gfx::PointF( x +  57.50596117973328, y + 36.83205032348633 ) );
            points.push_back( Pt::Gfx::PointF( x +  59.30873751640320, y + 36.83205032348633 ) );
            points.push_back( Pt::Gfx::PointF( x +  58.61535906791687, y + 35.16794967651367 ) );
            points.push_back( Pt::Gfx::PointF( x +  55.61535906791687, y + 33.16794967651367 ) );
            points.push_back( Pt::Gfx::PointF( x +  55.06066012382507, y + 34.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  55.50787234306335, y + 33.10557174682617 ) );
            points.push_back( Pt::Gfx::PointF( x +  53.74540591239929, y + 32.22434425354004 ) );
            points.push_back( Pt::Gfx::PointF( x +  51.92726397514343, y + 29.49711227416992 ) );
            points.push_back( Pt::Gfx::PointF( x +  49.00416064262390, y + 23.65092658996582 ) );
            points.push_back( Pt::Gfx::PointF( x +  47.03655886650085, y + 15.78051090240479 ) );
            points.push_back( Pt::Gfx::PointF( x +  45.04123950004578, y +  5.80388402938843 ) );
            points.push_back( Pt::Gfx::PointF( x +  44.06066012382507, y +  6.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  44.98913836479187, y +  5.62860918045044 ) );
            points.push_back( Pt::Gfx::PointF( x +  42.98913836479187, y +  0.62860932946205 ) );
            points.push_back( Pt::Gfx::PointF( x +  42.06066012382507, y +  1.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  42.06066012382507, y +  0.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  39.06066012382507, y +  0.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  39.06066012382507, y +  1.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  38.61344790458679, y +  0.10557281970978 ) );
            points.push_back( Pt::Gfx::PointF( x +  36.61344790458679, y +  1.10557281970978 ) );
            points.push_back( Pt::Gfx::PointF( x +  37.06066012382507, y +  2.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  36.22860980033875, y +  1.44529980421066 ) );
            points.push_back( Pt::Gfx::PointF( x +  34.41893267631531, y +  4.15981960296631 ) );
            points.push_back( Pt::Gfx::PointF( x +  31.74443316459656, y +  5.05131673812866 ) );
            points.push_back( Pt::Gfx::PointF( x +  32.06066012382507, y +  6.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  31.22860980033875, y +  5.44529962539673 ) );
            points.push_back( Pt::Gfx::PointF( x +  29.22860980033875, y +  8.44529962539673 ) );
            points.push_back( Pt::Gfx::PointF( x +  30.06066012382507, y +  9.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  29.09913706779480, y +  8.72527885437012 ) );
            points.push_back( Pt::Gfx::PointF( x +  27.11319041252136, y + 15.67609786987305 ) );
            points.push_back( Pt::Gfx::PointF( x +  25.15500903129578, y + 20.57154655456543 ) );
            points.push_back( Pt::Gfx::PointF( x +  23.46066164970398, y + 23.40000152587891 ) );
            points.push_back( Pt::Gfx::PointF( x +  20.19411540031433, y + 30.49704170227051 ) );
            points.push_back( Pt::Gfx::PointF( x +  18.41893649101257, y + 33.15982055664062 ) );
            points.push_back( Pt::Gfx::PointF( x +  15.74443221092224, y + 34.05131530761719 ) );
            points.push_back( Pt::Gfx::PointF( x +  16.06066012382507, y + 35.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  15.61344695091248, y + 34.10557174682617 ) );
            points.push_back( Pt::Gfx::PointF( x +  13.82457804679871, y + 35.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  12.29674410820007, y + 35.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  10.50787329673767, y + 34.10557174682617 ) );
            points.push_back( Pt::Gfx::PointF( x +  10.06066012382507, y + 35.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +  10.06066012382507, y + 34.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   7.06066012382507, y + 34.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   5.06066012382507, y + 34.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x +   3.47487330436707, y + 34.00000000000000 ) );
            return points;
        }
};
