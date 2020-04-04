class Benchmark2View : public Pt::Hmi::Control
{
    public:
        Benchmark2View()
        {}

    protected:
        virtual void onPaint(Pt::Hmi::PaintSurface& surface,
                             const Pt::Gfx::RectF& rect)
        {
            using namespace Pt::Gfx;


            const int   imageWidth  = 672;
            const int   imageHeight = 327;
            const RectF imageRect   = RectF( PointF(0, 0), SizeF(imageWidth, imageHeight) );
            const Color background  = Color::fromRgb8(0, 0, 0);

            Pt::Hmi::Painter painter(surface);
            painter.setClip(rect);

            Image image1( painter.format(), Size(imageWidth, imageHeight) );
            ImagePainter ip1(image1);

            Image image2( painter.format(), Size(imageWidth, imageHeight) );
            ImagePainter2 ip2(image2);
            ip2.setAntiAliasing(true);

//#define SOURCE_OVER

#ifdef SOURCE_OVER
            ip1.setCompositionMode(CompositionMode::SourceOver);
            ip2.setCompositionMode(CompositionMode::SourceOver);
#endif

            static Pt::uint64_t resIP1 = 0;
            static Pt::uint64_t resIP2 = 0;

            static bool doBenchmark = true;
            if(doBenchmark) {
                doBenchmark = false;
                resIP1 = onPaintContent(ip1, "IP1", 0, 0.0f);
                resIP2 = onPaintContent(ip2, "IP2", 0, 0.0f);
            }

            ip1.setBrush(background);
            ip1.fillRect(imageRect);

            ip2.setBrush(background);
            ip2.fillRect(imageRect);

            onPaintContent(ip1, "IP1", resIP1, 1.0f);
            onPaintContent(ip2, "IP2", resIP2, (float) resIP2 / (float) resIP1);

            painter.drawImage(PointF(2, 2), image1);
            painter.drawImage(PointF(2, 338), image2);
        }

        virtual Pt::uint64_t onPaintContent(Pt::Gfx::Painter& painter, const char* text, Pt::uint64_t benchmarkResult, float benchmarkRatio)
        {
            using namespace Pt::Gfx;

            ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(&painter);
            const char*    aai = (ip2 && ip2->isAntiAliasing()) ? "WITH AA" : "WITHOUT AA";

            const int loopCount = 250;

            //char buff[128];
            //sprintf(buff, "%s [%s] - %zd mS (%.1f x)", text, aai, (size_t) benchmarkResult, benchmarkRatio);

            std::ostringstream oss;
            oss << text << " [" << aai << "] - " << benchmarkResult << " mS ("
                << std::fixed << std::setprecision(1) << benchmarkRatio << " x)"
                << " [" << loopCount << " polylines]";

            painter.setFont ( Font("", 12) );
            painter.setPen  ( Color::fromRgb8(164, 100, 255)  );
            painter.drawText( PointF(250, 50), Pt::String(oss.str().c_str()));

#ifdef SOURCE_OVER
            const Pt::uint8_t alpha = 175;
#else
            const Pt::uint8_t alpha = 255;
#endif

            Pt::Gfx::Pen green2( Color::fromRgb8(0, 255, 0, alpha), 2, Pen::Solid, Pen::RoundCap, Pen::NoJoin );

            std::vector<Pt::Gfx::PointF> shape = makeLineComplex(6, 1);

            if(!benchmarkResult) {
                painter.setPen(green2);

                Pt::System::Clock clock;

                for(int i = 0; i < loopCount; ++i) {
                    clock.start();
                    painter.drawPolyline( &shape[0], shape.size() );
                    benchmarkResult += clock.stop().toUSecs();
                }

                benchmarkResult /= 1000;
            }
            else {
                painter.setPen(green2);
                painter.drawPolyline( &shape[0], shape.size() );
            }

            return benchmarkResult;
        }

        std::vector<Pt::Gfx::PointF> makeLineComplex(double x, double y)
        {
            std::vector<Pt::Gfx::PointF> points;
            points.push_back( Pt::Gfx::PointF( x +   0.00000000000000, y + 293.18028817003403 ) );
            points.push_back( Pt::Gfx::PointF( x +   5.49606026084621, y + 296.71994152999889 ) );
            points.push_back( Pt::Gfx::PointF( x +  10.99212052169241, y + 294.88495331543334 ) );
            points.push_back( Pt::Gfx::PointF( x +  16.48818078253862, y + 293.85795572653115 ) );
            points.push_back( Pt::Gfx::PointF( x +  21.98424104338483, y + 294.77169869430020 ) );
            points.push_back( Pt::Gfx::PointF( x +  27.48030130423103, y + 295.63640623817560 ) );
            points.push_back( Pt::Gfx::PointF( x +  32.97636156507679, y + 299.94476639380139 ) );
            points.push_back( Pt::Gfx::PointF( x +  38.47242182592299, y + 307.43775473151447 ) );
            points.push_back( Pt::Gfx::PointF( x +  43.96848208676920, y + 311.35468073639333 ) );
            points.push_back( Pt::Gfx::PointF( x +  49.46454234761541, y + 308.29371922974519 ) );
            points.push_back( Pt::Gfx::PointF( x +  54.96060260846161, y + 305.13807465432672 ) );
            points.push_back( Pt::Gfx::PointF( x +  60.45666286930782, y + 312.12728297894137 ) );
            points.push_back( Pt::Gfx::PointF( x +  65.95272313015403, y + 315.88024704895599 ) );
            points.push_back( Pt::Gfx::PointF( x +  71.44878339100023, y + 313.61666528143485 ) );
            points.push_back( Pt::Gfx::PointF( x +  76.94484365184644, y + 316.63708129073359 ) );
            points.push_back( Pt::Gfx::PointF( x +  82.44090391269265, y + 319.33056785380506 ) );
            points.push_back( Pt::Gfx::PointF( x +  87.93696417353840, y + 320.50421474339430 ) );
            points.push_back( Pt::Gfx::PointF( x +  93.43302443438461, y + 316.88405867519225 ) );
            points.push_back( Pt::Gfx::PointF( x +  98.92908469523081, y + 308.31392271469053 ) );
            points.push_back( Pt::Gfx::PointF( x + 104.42514495607702, y + 304.17712384300364 ) );
            points.push_back( Pt::Gfx::PointF( x + 109.92120521692323, y + 298.66413733514435 ) );
            points.push_back( Pt::Gfx::PointF( x + 115.41726547776943, y + 291.06152098985854 ) );
            points.push_back( Pt::Gfx::PointF( x + 120.91332573861564, y + 288.18752791329371 ) );
            points.push_back( Pt::Gfx::PointF( x + 126.40938599946185, y + 292.36096221727917 ) );
            points.push_back( Pt::Gfx::PointF( x + 131.90544626030805, y + 287.79965815403534 ) );
            points.push_back( Pt::Gfx::PointF( x + 137.40150652115381, y + 275.51343924622313 ) );
            points.push_back( Pt::Gfx::PointF( x + 142.89756678200001, y + 253.44732823497196 ) );
            points.push_back( Pt::Gfx::PointF( x + 148.39362704284622, y + 209.36818852641358 ) );
            points.push_back( Pt::Gfx::PointF( x + 153.88968730369243, y + 154.44726435976219 ) );
            points.push_back( Pt::Gfx::PointF( x + 159.38574756453863, y +  88.83280076573152 ) );
            points.push_back( Pt::Gfx::PointF( x + 164.88180782538484, y +  26.70221359818009 ) );
            points.push_back( Pt::Gfx::PointF( x + 170.37786808623105, y +   0.00000000000000 ) );
            points.push_back( Pt::Gfx::PointF( x + 175.87392834707725, y +  25.57442687308429 ) );
            points.push_back( Pt::Gfx::PointF( x + 181.36998860792346, y +  89.96279403730892 ) );
            points.push_back( Pt::Gfx::PointF( x + 186.86604886876967, y + 166.77451615040542 ) );
            points.push_back( Pt::Gfx::PointF( x + 192.36210912961542, y + 231.59291168000124 ) );
            points.push_back( Pt::Gfx::PointF( x + 197.85816939046163, y + 271.34438004706749 ) );
            points.push_back( Pt::Gfx::PointF( x + 203.35422965130783, y + 290.75922306897894 ) );
            points.push_back( Pt::Gfx::PointF( x + 208.85028991215404, y + 301.93771630345861 ) );
            points.push_back( Pt::Gfx::PointF( x + 214.34635017300025, y + 311.53547167043700 ) );
            points.push_back( Pt::Gfx::PointF( x + 219.84241043384645, y + 315.21780730165176 ) );
            points.push_back( Pt::Gfx::PointF( x + 225.33847069469266, y + 310.57481729234723 ) );
            points.push_back( Pt::Gfx::PointF( x + 230.83453095553887, y + 310.07928050870373 ) );
            points.push_back( Pt::Gfx::PointF( x + 236.33059121638507, y + 314.73906633672925 ) );
            points.push_back( Pt::Gfx::PointF( x + 241.82665147723083, y + 309.00463564002473 ) );
            points.push_back( Pt::Gfx::PointF( x + 247.32271173807703, y + 302.00508070317835 ) );
            points.push_back( Pt::Gfx::PointF( x + 252.81877199892324, y + 292.82949950620491 ) );
            points.push_back( Pt::Gfx::PointF( x + 258.31483225976945, y + 280.16296883454959 ) );
            points.push_back( Pt::Gfx::PointF( x + 263.81089252061565, y + 267.55077164027193 ) );
            points.push_back( Pt::Gfx::PointF( x + 269.30695278146186, y + 253.55863462711631 ) );
            points.push_back( Pt::Gfx::PointF( x + 274.80301304230807, y + 239.95043844896890 ) );
            points.push_back( Pt::Gfx::PointF( x + 280.29907330315427, y + 226.67345504684153 ) );
            points.push_back( Pt::Gfx::PointF( x + 285.79513356400048, y + 212.79190301599357 ) );
            points.push_back( Pt::Gfx::PointF( x + 291.29119382484669, y + 200.64338976811786 ) );
            points.push_back( Pt::Gfx::PointF( x + 296.78725408569244, y + 189.62993437097606 ) );
            points.push_back( Pt::Gfx::PointF( x + 302.28331434653865, y + 187.77393255581893 ) );
            points.push_back( Pt::Gfx::PointF( x + 307.77937460738485, y + 198.64777771403254 ) );
            points.push_back( Pt::Gfx::PointF( x + 313.27543486823106, y + 212.00237588470242 ) );
            points.push_back( Pt::Gfx::PointF( x + 318.77149512907727, y + 233.07558611164899 ) );
            points.push_back( Pt::Gfx::PointF( x + 324.26755538992347, y + 253.20107507081201 ) );
            points.push_back( Pt::Gfx::PointF( x + 329.76361565076968, y + 265.89410084468932 ) );
            points.push_back( Pt::Gfx::PointF( x + 335.25967591161589, y + 277.75305178614258 ) );
            points.push_back( Pt::Gfx::PointF( x + 340.75573617246209, y + 283.73482812085831 ) );
            points.push_back( Pt::Gfx::PointF( x + 346.25179643330785, y + 282.12945737877351 ) );
            points.push_back( Pt::Gfx::PointF( x + 351.74785669415405, y + 279.51258306926866 ) );
            points.push_back( Pt::Gfx::PointF( x + 357.24391695500026, y + 283.28820513334279 ) );
            points.push_back( Pt::Gfx::PointF( x + 362.73997721584647, y + 288.66057303150501 ) );
            points.push_back( Pt::Gfx::PointF( x + 368.23603747669267, y + 292.57799599434281 ) );
            points.push_back( Pt::Gfx::PointF( x + 373.73209773753888, y + 297.38764005293865 ) );
            points.push_back( Pt::Gfx::PointF( x + 379.22815799838509, y + 303.99640498972462 ) );
            points.push_back( Pt::Gfx::PointF( x + 384.72421825923129, y + 306.15499690381034 ) );
            points.push_back( Pt::Gfx::PointF( x + 390.22027852007750, y + 310.63776201923861 ) );
            points.push_back( Pt::Gfx::PointF( x + 395.71633878092371, y + 317.16660927334021 ) );
            points.push_back( Pt::Gfx::PointF( x + 401.21239904176946, y + 320.44260432987210 ) );
            points.push_back( Pt::Gfx::PointF( x + 406.70845930261567, y + 321.38224927455792 ) );
            points.push_back( Pt::Gfx::PointF( x + 412.20451956346187, y + 322.44791380375693 ) );
            points.push_back( Pt::Gfx::PointF( x + 417.70057982430808, y + 322.56611190981005 ) );
            points.push_back( Pt::Gfx::PointF( x + 423.19664008515429, y + 320.48178340397078 ) );
            points.push_back( Pt::Gfx::PointF( x + 428.69270034600049, y + 318.51815088303044 ) );
            points.push_back( Pt::Gfx::PointF( x + 434.18876060684670, y + 320.44256819487316 ) );
            points.push_back( Pt::Gfx::PointF( x + 439.68482086769291, y + 323.47304135761181 ) );
            points.push_back( Pt::Gfx::PointF( x + 445.18088112853911, y + 322.33013678363216 ) );
            points.push_back( Pt::Gfx::PointF( x + 450.67694138938487, y + 316.20438688734214 ) );
            points.push_back( Pt::Gfx::PointF( x + 456.17300165023107, y + 312.53018371215342 ) );
            points.push_back( Pt::Gfx::PointF( x + 461.66906191107728, y + 315.32087076419123 ) );
            points.push_back( Pt::Gfx::PointF( x + 467.16512217192349, y + 319.07964117121048 ) );
            points.push_back( Pt::Gfx::PointF( x + 472.66118243276969, y + 316.82842839347961 ) );
            points.push_back( Pt::Gfx::PointF( x + 478.15724269361590, y + 312.38343161565456 ) );
            points.push_back( Pt::Gfx::PointF( x + 483.65330295446211, y + 307.52853939864599 ) );
            points.push_back( Pt::Gfx::PointF( x + 489.14936321530831, y + 304.56751960755446 ) );
            points.push_back( Pt::Gfx::PointF( x + 494.64542347615452, y + 302.61963889685705 ) );
            points.push_back( Pt::Gfx::PointF( x + 500.14148373700073, y + 295.21561600070913 ) );
            points.push_back( Pt::Gfx::PointF( x + 505.63754399784648, y + 283.12461689562474 ) );
            points.push_back( Pt::Gfx::PointF( x + 511.13360425869269, y + 266.91006333116093 ) );
            points.push_back( Pt::Gfx::PointF( x + 516.62966451953889, y + 245.60877543691657 ) );
            points.push_back( Pt::Gfx::PointF( x + 522.12572478038510, y + 217.14577492173402 ) );
            points.push_back( Pt::Gfx::PointF( x + 527.62178504123131, y + 177.81952090951057 ) );
            points.push_back( Pt::Gfx::PointF( x + 533.11784530207751, y + 135.82336436972963 ) );
            points.push_back( Pt::Gfx::PointF( x + 538.61390556292372, y + 104.67846518353480 ) );
            points.push_back( Pt::Gfx::PointF( x + 544.10996582376993, y +  94.79123548543788 ) );
            points.push_back( Pt::Gfx::PointF( x + 549.60602608461613, y + 109.86485354272332 ) );
            points.push_back( Pt::Gfx::PointF( x + 555.10208634546188, y + 144.29613553118935 ) );
            points.push_back( Pt::Gfx::PointF( x + 560.59814660630809, y + 192.67547210597508 ) );
            points.push_back( Pt::Gfx::PointF( x + 566.09420686715430, y + 242.63196986775466 ) );
            points.push_back( Pt::Gfx::PointF( x + 571.59026712800051, y + 267.98371522067123 ) );
            points.push_back( Pt::Gfx::PointF( x + 577.08632738884671, y + 282.61867969083994 ) );
            points.push_back( Pt::Gfx::PointF( x + 582.58238764969292, y + 291.99061658640949 ) );
            points.push_back( Pt::Gfx::PointF( x + 588.07844791053913, y + 293.14396649183470 ) );
            points.push_back( Pt::Gfx::PointF( x + 593.57450817138533, y + 295.38218434800319 ) );
            points.push_back( Pt::Gfx::PointF( x + 599.07056843223154, y + 295.24565163464376 ) );
            points.push_back( Pt::Gfx::PointF( x + 604.56662869307775, y + 298.17827908936977 ) );
            points.push_back( Pt::Gfx::PointF( x + 610.06268895392350, y + 306.82261544500398 ) );
            points.push_back( Pt::Gfx::PointF( x + 615.55874921476970, y + 312.14458836278754 ) );
            points.push_back( Pt::Gfx::PointF( x + 621.05480947561591, y + 316.93172753735087 ) );
            points.push_back( Pt::Gfx::PointF( x + 626.55086973646212, y + 321.06684038878245 ) );
            points.push_back( Pt::Gfx::PointF( x + 632.04692999730833, y + 322.00025823107728 ) );
            points.push_back( Pt::Gfx::PointF( x + 637.54299025815453, y + 318.63822224819762 ) );
            points.push_back( Pt::Gfx::PointF( x + 643.03905051900074, y + 312.18435044679256 ) );
            points.push_back( Pt::Gfx::PointF( x + 648.53511077984695, y + 304.38166533295680 ) );
            points.push_back( Pt::Gfx::PointF( x + 654.03117104069315, y + 296.66870163770000 ) );
            points.push_back( Pt::Gfx::PointF( x + 659.52723130153890, y + 289.62022334615619 ) );
            return points;
        }
};
