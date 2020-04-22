// https://www.lemoda.net/c/cairo-to-png
// https://www.cairographics.org/FAQ

#include <stdlib.h>
#include <cairo.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <sys/time.h>

int main ()
{
    int SIZEX = 800;
    int SIZEY = 800;

    cairo_surface_t* cs;
    cairo_t*         c;

    cs = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, SIZEX, SIZEY);
    c  = cairo_create (cs);

    cairo_set_operator  (c, CAIRO_OPERATOR_SOURCE);
    cairo_set_antialias (c, CAIRO_ANTIALIAS_DEFAULT);
    cairo_set_line_width(c, 1.0);

#if 1

    /* Draw something and write to PNG */
    cairo_set_source_rgba(c, 0.0, 0.0, 0.0, 1.0);
    cairo_paint          (c);

    cairo_set_source_rgba(c, 1.0, 1.0, 1.0, 1.0);

    cairo_move_to(c, 11.0, 240.5);
    cairo_line_to(c, 16.5, 246.0);
    cairo_line_to(c, 11.0, 251.5);
    cairo_line_to(c,  5.5, 246.0);
    cairo_line_to(c, 11.0, 240.5);
    cairo_stroke (c);
    cairo_surface_write_to_png (cs, "CairoTest1.png");

    /* Draw something and write to PNG */
    cairo_set_source_rgba(c, 0.0, 0.0, 0.0, 1.0);
    cairo_paint          (c);

    cairo_set_source_rgba(c, 1.0, 1.0, 1.0, 1.0);

    cairo_move_to(c, 11.0, 240.0);
    cairo_line_to(c, 16.0, 246.0);
    cairo_line_to(c, 11.0, 251.0);
    cairo_line_to(c,  5.0, 246.0);
    cairo_line_to(c, 11.0, 240.0);
    cairo_stroke (c);

    cairo_surface_write_to_png (cs, "CairoTest2.png");


#else

    /* Draw something and write to PNG */
    cairo_set_source_rgba(c, 0.0, 0.0, 0.0, 1.0);
    cairo_paint          (c);

    cairo_set_source_rgba(c, 1.0, 1.0, 1.0, 1.0);

    cairo_move_to(c, 1.5, 1.5);
    cairo_line_to(c, 3.5, 1.5);
    cairo_stroke (c);

    cairo_move_to(c, 11.5, 1.5);
    cairo_line_to(c, 11.5, 3.5);
    cairo_stroke (c);

    cairo_move_to(c, 1.5, 21.5);
    cairo_line_to(c, 3.5, 21.5);
    cairo_stroke (c);

    cairo_move_to(c, 11.5, 21.5);
    cairo_line_to(c, 11.5, 23.5);
    cairo_stroke (c);

    cairo_move_to(c, 1.5, 41.0);
    cairo_line_to(c, 3.5, 41.0);
    cairo_stroke (c);

    cairo_move_to(c, 11.0, 41.5);
    cairo_line_to(c, 11.0, 43.5);
    cairo_stroke (c);

    cairo_surface_write_to_png (cs, "CairoTest1.png");

    /* Draw something and write to PNG */
    cairo_set_source_rgba(c, 0.0, 0.0, 0.0, 1.0);
    cairo_paint          (c);

    cairo_set_source_rgba(c, 1.0, 1.0, 1.0, 1.0);

    cairo_move_to(c, 1.0, 1.5);
    cairo_line_to(c, 4.0, 1.5);
    cairo_stroke (c);

    cairo_move_to(c, 11.5, 1.0);
    cairo_line_to(c, 11.5, 4.0);
    cairo_stroke (c);

    cairo_move_to(c, 1.0, 21.5);
    cairo_line_to(c, 4.0, 21.5);
    cairo_stroke (c);

    cairo_move_to(c, 11.5, 21.0);
    cairo_line_to(c, 11.5, 24.0);
    cairo_stroke (c);

    cairo_move_to(c, 1.0, 41.0);
    cairo_line_to(c, 4.0, 41.0);
    cairo_stroke (c);

    cairo_move_to(c, 11.0, 41.0);
    cairo_line_to(c, 11.0, 44.0);
    cairo_stroke (c);

    cairo_surface_write_to_png (cs, "CairoTest2.png");

#endif

#if 1
    int x =  50;
    int y = 100;

    cairo_set_line_width (c, 2.0);

    cairo_set_source_rgba(c, 0.0, 0.0, 0.0, 1.0);
    cairo_paint          (c);

    cairo_set_source_rgba(c, 1.0, 1.0, 1.0, 1.0);

    struct timeval _startTime;
    struct timeval _stopTime;


    const int          loopCount       = 250;
    unsigned long long benchmarkResult = 0;

    for(int i = 0; i < loopCount; ++i) {
        gettimeofday(&_startTime, 0);

        cairo_move_to(c, x +   0.00000000000000, y + 293.18028817003403);
        cairo_line_to(c, x +   5.49606026084621, y + 296.71994152999889);
        cairo_line_to(c, x +  10.99212052169241, y + 294.88495331543334);
        cairo_line_to(c, x +  16.48818078253862, y + 293.85795572653115);
        cairo_line_to(c, x +  21.98424104338483, y + 294.77169869430020);
        cairo_line_to(c, x +  27.48030130423103, y + 295.63640623817560);
        cairo_line_to(c, x +  32.97636156507679, y + 299.94476639380139);
        cairo_line_to(c, x +  38.47242182592299, y + 307.43775473151447);
        cairo_line_to(c, x +  43.96848208676920, y + 311.35468073639333);
        cairo_line_to(c, x +  49.46454234761541, y + 308.29371922974519);
        cairo_line_to(c, x +  54.96060260846161, y + 305.13807465432672);
        cairo_line_to(c, x +  60.45666286930782, y + 312.12728297894137);
        cairo_line_to(c, x +  65.95272313015403, y + 315.88024704895599);
        cairo_line_to(c, x +  71.44878339100023, y + 313.61666528143485);
        cairo_line_to(c, x +  76.94484365184644, y + 316.63708129073359);
        cairo_line_to(c, x +  82.44090391269265, y + 319.33056785380506);
        cairo_line_to(c, x +  87.93696417353840, y + 320.50421474339430);
        cairo_line_to(c, x +  93.43302443438461, y + 316.88405867519225);
        cairo_line_to(c, x +  98.92908469523081, y + 308.31392271469053);
        cairo_line_to(c, x + 104.42514495607702, y + 304.17712384300364);
        cairo_line_to(c, x + 109.92120521692323, y + 298.66413733514435);
        cairo_line_to(c, x + 115.41726547776943, y + 291.06152098985854);
        cairo_line_to(c, x + 120.91332573861564, y + 288.18752791329371);
        cairo_line_to(c, x + 126.40938599946185, y + 292.36096221727917);
        cairo_line_to(c, x + 131.90544626030805, y + 287.79965815403534);
        cairo_line_to(c, x + 137.40150652115381, y + 275.51343924622313);
        cairo_line_to(c, x + 142.89756678200001, y + 253.44732823497196);
        cairo_line_to(c, x + 148.39362704284622, y + 209.36818852641358);
        cairo_line_to(c, x + 153.88968730369243, y + 154.44726435976219);
        cairo_line_to(c, x + 159.38574756453863, y +  88.83280076573152);
        cairo_line_to(c, x + 164.88180782538484, y +  26.70221359818009);
        cairo_line_to(c, x + 170.37786808623105, y +   0.00000000000000);
        cairo_line_to(c, x + 175.87392834707725, y +  25.57442687308429);
        cairo_line_to(c, x + 181.36998860792346, y +  89.96279403730892);
        cairo_line_to(c, x + 186.86604886876967, y + 166.77451615040542);
        cairo_line_to(c, x + 192.36210912961542, y + 231.59291168000124);
        cairo_line_to(c, x + 197.85816939046163, y + 271.34438004706749);
        cairo_line_to(c, x + 203.35422965130783, y + 290.75922306897894);
        cairo_line_to(c, x + 208.85028991215404, y + 301.93771630345861);
        cairo_line_to(c, x + 214.34635017300025, y + 311.53547167043700);
        cairo_line_to(c, x + 219.84241043384645, y + 315.21780730165176);
        cairo_line_to(c, x + 225.33847069469266, y + 310.57481729234723);
        cairo_line_to(c, x + 230.83453095553887, y + 310.07928050870373);
        cairo_line_to(c, x + 236.33059121638507, y + 314.73906633672925);
        cairo_line_to(c, x + 241.82665147723083, y + 309.00463564002473);
        cairo_line_to(c, x + 247.32271173807703, y + 302.00508070317835);
        cairo_line_to(c, x + 252.81877199892324, y + 292.82949950620491);
        cairo_line_to(c, x + 258.31483225976945, y + 280.16296883454959);
        cairo_line_to(c, x + 263.81089252061565, y + 267.55077164027193);
        cairo_line_to(c, x + 269.30695278146186, y + 253.55863462711631);
        cairo_line_to(c, x + 274.80301304230807, y + 239.95043844896890);
        cairo_line_to(c, x + 280.29907330315427, y + 226.67345504684153);
        cairo_line_to(c, x + 285.79513356400048, y + 212.79190301599357);
        cairo_line_to(c, x + 291.29119382484669, y + 200.64338976811786);
        cairo_line_to(c, x + 296.78725408569244, y + 189.62993437097606);
        cairo_line_to(c, x + 302.28331434653865, y + 187.77393255581893);
        cairo_line_to(c, x + 307.77937460738485, y + 198.64777771403254);
        cairo_line_to(c, x + 313.27543486823106, y + 212.00237588470242);
        cairo_line_to(c, x + 318.77149512907727, y + 233.07558611164899);
        cairo_line_to(c, x + 324.26755538992347, y + 253.20107507081201);
        cairo_line_to(c, x + 329.76361565076968, y + 265.89410084468932);
        cairo_line_to(c, x + 335.25967591161589, y + 277.75305178614258);
        cairo_line_to(c, x + 340.75573617246209, y + 283.73482812085831);
        cairo_line_to(c, x + 346.25179643330785, y + 282.12945737877351);
        cairo_line_to(c, x + 351.74785669415405, y + 279.51258306926866);
        cairo_line_to(c, x + 357.24391695500026, y + 283.28820513334279);
        cairo_line_to(c, x + 362.73997721584647, y + 288.66057303150501);
        cairo_line_to(c, x + 368.23603747669267, y + 292.57799599434281);
        cairo_line_to(c, x + 373.73209773753888, y + 297.38764005293865);
        cairo_line_to(c, x + 379.22815799838509, y + 303.99640498972462);
        cairo_line_to(c, x + 384.72421825923129, y + 306.15499690381034);
        cairo_line_to(c, x + 390.22027852007750, y + 310.63776201923861);
        cairo_line_to(c, x + 395.71633878092371, y + 317.16660927334021);
        cairo_line_to(c, x + 401.21239904176946, y + 320.44260432987210);
        cairo_line_to(c, x + 406.70845930261567, y + 321.38224927455792);
        cairo_line_to(c, x + 412.20451956346187, y + 322.44791380375693);
        cairo_line_to(c, x + 417.70057982430808, y + 322.56611190981005);
        cairo_line_to(c, x + 423.19664008515429, y + 320.48178340397078);
        cairo_line_to(c, x + 428.69270034600049, y + 318.51815088303044);
        cairo_line_to(c, x + 434.18876060684670, y + 320.44256819487316);
        cairo_line_to(c, x + 439.68482086769291, y + 323.47304135761181);
        cairo_line_to(c, x + 445.18088112853911, y + 322.33013678363216);
        cairo_line_to(c, x + 450.67694138938487, y + 316.20438688734214);
        cairo_line_to(c, x + 456.17300165023107, y + 312.53018371215342);
        cairo_line_to(c, x + 461.66906191107728, y + 315.32087076419123);
        cairo_line_to(c, x + 467.16512217192349, y + 319.07964117121048);
        cairo_line_to(c, x + 472.66118243276969, y + 316.82842839347961);
        cairo_line_to(c, x + 478.15724269361590, y + 312.38343161565456);
        cairo_line_to(c, x + 483.65330295446211, y + 307.52853939864599);
        cairo_line_to(c, x + 489.14936321530831, y + 304.56751960755446);
        cairo_line_to(c, x + 494.64542347615452, y + 302.61963889685705);
        cairo_line_to(c, x + 500.14148373700073, y + 295.21561600070913);
        cairo_line_to(c, x + 505.63754399784648, y + 283.12461689562474);
        cairo_line_to(c, x + 511.13360425869269, y + 266.91006333116093);
        cairo_line_to(c, x + 516.62966451953889, y + 245.60877543691657);
        cairo_line_to(c, x + 522.12572478038510, y + 217.14577492173402);
        cairo_line_to(c, x + 527.62178504123131, y + 177.81952090951057);
        cairo_line_to(c, x + 533.11784530207751, y + 135.82336436972963);
        cairo_line_to(c, x + 538.61390556292372, y + 104.67846518353480);
        cairo_line_to(c, x + 544.10996582376993, y +  94.79123548543788);
        cairo_line_to(c, x + 549.60602608461613, y + 109.86485354272332);
        cairo_line_to(c, x + 555.10208634546188, y + 144.29613553118935);
        cairo_line_to(c, x + 560.59814660630809, y + 192.67547210597508);
        cairo_line_to(c, x + 566.09420686715430, y + 242.63196986775466);
        cairo_line_to(c, x + 571.59026712800051, y + 267.98371522067123);
        cairo_line_to(c, x + 577.08632738884671, y + 282.61867969083994);
        cairo_line_to(c, x + 582.58238764969292, y + 291.99061658640949);
        cairo_line_to(c, x + 588.07844791053913, y + 293.14396649183470);
        cairo_line_to(c, x + 593.57450817138533, y + 295.38218434800319);
        cairo_line_to(c, x + 599.07056843223154, y + 295.24565163464376);
        cairo_line_to(c, x + 604.56662869307775, y + 298.17827908936977);
        cairo_line_to(c, x + 610.06268895392350, y + 306.82261544500398);
        cairo_line_to(c, x + 615.55874921476970, y + 312.14458836278754);
        cairo_line_to(c, x + 621.05480947561591, y + 316.93172753735087);
        cairo_line_to(c, x + 626.55086973646212, y + 321.06684038878245);
        cairo_line_to(c, x + 632.04692999730833, y + 322.00025823107728);
        cairo_line_to(c, x + 637.54299025815453, y + 318.63822224819762);
        cairo_line_to(c, x + 643.03905051900074, y + 312.18435044679256);
        cairo_line_to(c, x + 648.53511077984695, y + 304.38166533295680);
        cairo_line_to(c, x + 654.03117104069315, y + 296.66870163770000);
        cairo_line_to(c, x + 659.52723130153890, y + 289.62022334615619);
        cairo_stroke (c);

        gettimeofday(&_stopTime, 0);
        time_t      secs  = _stopTime.tv_sec - _startTime.tv_sec;
        suseconds_t usecs = _stopTime.tv_usec - _startTime.tv_usec;

        benchmarkResult += ( secs * 1000000 + usecs );

        if(!i) cairo_surface_write_to_png (cs, "CairoTest3.png");
    }

    benchmarkResult /= 1000;

    printf("%zd\n", (size_t) benchmarkResult);


#endif

    /* Done */
    cairo_destroy        (c);
    cairo_surface_destroy(cs);
    return 0;
}
