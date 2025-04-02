class Test1View : public Pt::Forms::Control
{
    public:
        Test1View()
        {}

    protected:
        virtual void onPaint(Pt::Forms::PaintSurface& surface,
                             const Pt::Gfx::RectF& rect)
        {
          using namespace Pt::Gfx;

            const int   imageWidth  = 620;
            const int   imageHeight = 680;
            const RectF imageRect   = RectF( PointF(0, 0), SizeF(imageWidth, imageHeight) );
            const Color background  = Color::fromRgb8(0, 0, 0);

            //Pt::Forms::Painter painter(surface);
            //painter.setClip(rect);

            //Image image2( painter.format(), Size(imageWidth, imageHeight) );
            //ImagePainter2 imagePainter2(image2);
            //imagePainter2.setAntiAliasing(true);
            //imagePainter2.setBrush(background);
            //imagePainter2.fillRect(imageRect);

            ////imagePainter2.setCompositionMode(CompositionMode::SourceOver);

            ////onPaintContent(imagePainter2);

            //painter.drawImage(PointF(2, 2), image2);
        }

//        virtual void onPaintContent(Pt::Gfx::ImagePainter2& painter)
//        {
//            using namespace Pt::Gfx;
//
//            Pt::Gfx::Pen   green1( Color::fromRgb8(0, 255, 0, 175), 1, Pen::Solid, Pen::RoundCap, Pen::NoJoin );
//            Pt::Gfx::Pen   green2( Color::fromRgb8(0, 255, 0, 175), 2, Pen::Solid, Pen::RoundCap, Pen::NoJoin );
//            Pt::Gfx::Pen   green3( Color::fromRgb8(0, 255, 0, 175), 3, Pen::Solid, Pen::RoundCap, Pen::NoJoin );
//
//            Pt::Gfx::Brush greenb( Color::fromRgb8(0, 255, 0, 175) );
//
//            std::vector<Pt::Gfx::PointF> shape;
//
//#if 1
//            shape = makeTestShape1(-300, -380);
//            painter.setPen(green1);
//            painter.drawPolyline( &shape[0], shape.size() );
//            shape = makeTestShape1(-300, -300);
//            painter.setPen(green2);
//            painter.drawPolyline( &shape[0], shape.size() );
//            shape = makeTestShape1(-300, -220);
//            painter.setPen(green3);
//            painter.drawPolyline( &shape[0], shape.size() );
//#endif
//
//#if 1
//            shape = makeTestShape2(0, -580);
//            painter.setPen(green1);
//            painter.drawPolyline( &shape[0], shape.size() );
//            shape = makeTestShape2(0, -500);
//            painter.setPen(green2);
//            painter.drawPolyline( &shape[0], shape.size() );
//            shape = makeTestShape2(0, -420);
//            painter.setPen(green3);
//            painter.drawPolyline( &shape[0], shape.size() );
//#endif
//
//#if 1
//            shape = makeTestShape3(50, -530);
//            painter.setPen(green1);
//            painter.drawPolyline( &shape[0], shape.size() );
//            shape = makeTestShape3(50, -450);
//            painter.setPen(green2);
//            painter.drawPolyline( &shape[0], shape.size() );
//            shape = makeTestShape3(50, -370);
//            painter.setPen(green3);
//            painter.drawPolyline( &shape[0], shape.size() );
//            shape = makeTestShape3(50, -290);
//            painter.setBrush(greenb);
//            painter.fillPolygon( &shape[0], shape.size() );
//
//            shape = makeTestShape3(150, -530); scaleShape(shape, 5);
//            painter.setPen(green1);
//            painter.drawPolyline( &shape[0], shape.size() );
//            shape = makeTestShape3(150, -450); scaleShape(shape, 5);
//            painter.setPen(green2);
//            painter.drawPolyline( &shape[0], shape.size() );
//            shape = makeTestShape3(150, -370); scaleShape(shape, 5);
//            painter.setPen(green3);
//            painter.drawPolyline( &shape[0], shape.size() );
//            shape = makeTestShape3(150, -290); scaleShape(shape, 5);
//            painter.setBrush(greenb);
//            painter.fillPolygon( &shape[0], shape.size() );
//#endif
//        }

        std::vector<Pt::Gfx::PointF> makeTestShape1(double xOfs, double yOfs)
        {
            std::vector<Pt::Gfx::PointF> points;
            points.push_back( Pt::Gfx::PointF(xOfs + 364.25000000000000, yOfs + 444.50000000000000) );
            points.push_back( Pt::Gfx::PointF(xOfs + 365.75000000000000, yOfs + 446.00000000000000) );
            points.push_back( Pt::Gfx::PointF(xOfs + 366.50000000000000, yOfs + 446.00000000000000) );
            points.push_back( Pt::Gfx::PointF(xOfs + 368.00000000000000, yOfs + 445.25000000000000) );
            return points;
        }

        std::vector<Pt::Gfx::PointF> makeTestShape2(double xOfs, double yOfs)
        {
            std::vector<Pt::Gfx::PointF> points;
            points.push_back( Pt::Gfx::PointF(xOfs + 135.00000000000000, yOfs + 613.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF(xOfs + 142.00000000000000, yOfs + 624.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF(xOfs + 148.00000000000000, yOfs + 630.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF(xOfs + 155.00000000000000, yOfs + 637.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF(xOfs + 161.00000000000000, yOfs + 641.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF(xOfs + 168.00000000000000, yOfs + 641.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF(xOfs + 174.00000000000000, yOfs + 638.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF(xOfs + 181.00000000000000, yOfs + 643.00000000000000 ) );
            return points;
        }

        std::vector<Pt::Gfx::PointF> makeTestShape3(double xOfs, double yOfs)
        {
            std::vector<Pt::Gfx::PointF> points;
            points.push_back( Pt::Gfx::PointF( xOfs + 200.04287138041008, yOfs + 586.71004820396047 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 200.49912727639179, yOfs + 587.09713039419933 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 200.95538317237353, yOfs + 587.14491976184195 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 201.41163906835527, yOfs + 587.06236973221769 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 201.86789496433698, yOfs + 587.14328427272108 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 202.32415086031870, yOfs + 587.35944317395581 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 202.78040675630041, yOfs + 587.33688018323755 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 203.23666265228215, yOfs + 587.06855211079812 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 203.69291854826389, yOfs + 586.81770235767578 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 204.14917444424560, yOfs + 586.34153385773061 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 204.60543034022731, yOfs + 585.38229968091991 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 205.06168623620903, yOfs + 584.32761411477202 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 205.51794213219077, yOfs + 583.34996804920286 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 205.97419802817251, yOfs + 581.81590048140788 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 206.43045392415422, yOfs + 581.22712949756612 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 206.88670982013593, yOfs + 581.10834525424411 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 207.34296571611765, yOfs + 580.50872918254674 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 207.79922161209939, yOfs + 580.19319242330948 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 208.25547750808113, yOfs + 580.18038204659888 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 208.71173340406284, yOfs + 581.20957233906279 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 209.16798930004455, yOfs + 583.21187839564334 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 209.62424519602627, yOfs + 584.87984718126211 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 210.08050109200801, yOfs + 586.03687522046437 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 210.53675698798975, yOfs + 586.63168745407006 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 210.99301288397146, yOfs + 586.96522243684171 ) );
            points.push_back( Pt::Gfx::PointF( xOfs + 211.44926877995317, yOfs + 587.33749410772862 ) );
            return points;
        }

        void scaleShape(std::vector<Pt::Gfx::PointF>& points, double scale, double xOfs = 0.0, double yOfs = 0.0)
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

            //fprintf(stderr, "###\n");
            for(size_t i = 0; i < points.size(); ++i)
            {
                const double x = (points[i].x() - minX) * scale;
                const double y = (points[i].y() - minY) * scale;
                points[i].set( x + minX + xOfs, y + minY + yOfs );
                //fprintf(stderr, "            points.push_back( Pt::Gfx::PointF( x + %17.14f * scale, y + %16.14f * scale ) );\n", points[i].x() - minX, points[i].y() - minY);
            }
        }
};
