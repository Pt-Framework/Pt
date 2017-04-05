/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2017-2017 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline fuelemtions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Ielem., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

//#include <algorithm>
#include <stdio.h>

#include "SvgRasterizer.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Internal Helper Functions ======================================================
// ======================================================================================

inline float cssHueToRgb(float t1, float t2, float hue)
{
  if(hue <  0.0f) hue += 6.0f;
  if(hue >= 6.0f) hue -= 6.0f;

  if(hue <  1.0f) return (t2 - t1) * hue + t1;
  if(hue <  3.0f) return t2;
  if(hue <  4.0f) return (t2 - t1) * (4.0f - hue) + t1;

  return t1;
}

inline void cssHslToRgb(int& r, int& g, int& b, int h_, int s_, int l_)
{
    const float h = (float) h_ /  60.0f;
    const float s = (float) s_ / 100.0f;
    const float l = (float) l_ / 100.0f;

    // Achromatic color?
    if(s == 0.0f) {
        r = g = b = Gfx::Math::lrint(l * 255.0f);
        return;
    }

    // Normal color
    const float t2 = (l <= 0.5f) ? ( l * (s + 1.0f) ) : ( l + s - (l * s) );
    const float t1 = l * 2.0f - t2;

    r = Gfx::Math::lrint( cssHueToRgb(t1, t2, h + 2.0f) * 255.0f );
    g = Gfx::Math::lrint( cssHueToRgb(t1, t2, h       ) * 255.0f );
    b = Gfx::Math::lrint( cssHueToRgb(t1, t2, h - 2.0f) * 255.0f );
}

inline const Color fromCssHsl(int h, int s, int l)
{
    int r, g, b;
    cssHslToRgb(r, g, b, h, s, l);

    return Color::fromRgb8(r, g, b, 255);
}

inline const Color fromCssHwb(int h, int w_, int b_)
{
    int r, g, b;
    cssHslToRgb(r, g, b, h, 100, 50);

    const float wh = (float) w_ / 100.0f;
    const float bl = (float) b_ / 100.0f;

    float fr = (float) r / 255.0f;
    float fg = (float) g / 255.0f;
    float fb = (float) b / 255.0f;

    fr *= (1.0f - wh - bl); fr += wh;
    fg *= (1.0f - wh - bl); fg += wh;
    fb *= (1.0f - wh - bl); fb += wh;

    r = Gfx::Math::lrint(fr * 255.0f);
    g = Gfx::Math::lrint(fg * 255.0f);
    b = Gfx::Math::lrint(fb * 255.0f);

    return Color::fromRgb8(r, g, b, 255);
}

inline const Color fromCssCmyk(int c_, int m_, int y_, int k_)
{
    const float c = (float) c_ / 100.0f;
    const float m = (float) m_ / 100.0f;
    const float y = (float) y_ / 100.0f;
    const float k = (float) k_ / 100.0f;

    const int r = 255 - Gfx::Math::lrint( ( std::min( 1.0f, c * (1.0f - k) + k ) ) * 255.0f );
    const int g = 255 - Gfx::Math::lrint( ( std::min( 1.0f, m * (1.0f - k) + k ) ) * 255.0f );
    const int b = 255 - Gfx::Math::lrint( ( std::min( 1.0f, y * (1.0f - k) + k ) ) * 255.0f );

    return Color::fromRgb8(r, g, b, 255);
}

inline const Color fromCssNCol(char n, int h, int w, int b)
{
    const float percent = (h / 10) * 10;

    switch(::toupper(n)) {
        case 'R': h = Gfx::Math::lrint(  0 + (percent * 0.6f)); break;
        case 'Y': h = Gfx::Math::lrint( 60 + (percent * 0.6f)); break;
        case 'G': h = Gfx::Math::lrint(120 + (percent * 0.6f)); break;
        case 'C': h = Gfx::Math::lrint(180 + (percent * 0.6f)); break;
        case 'B': h = Gfx::Math::lrint(240 + (percent * 0.6f)); break;
        case 'M': h = Gfx::Math::lrint(300 + (percent * 0.6f)); break;
        case 'W': h = 0;
                  b = Gfx::Math::lrint(percent);
                  w = 100 - b;
                  break;
        default : return Color::fromRgb8(0, 0, 0, 255);
    }

    return fromCssHwb(h, w, b);
}

inline const Color& fromCssNamedColor(const std::string& colStr)
{
    /*
    cat html_color.txt | awk --non-decimal-data '
    BEGIN {OFS = FS}
    {
        $3 = sprintf("%d", "0x" substr($2, 2, 2))
        $4 = sprintf("%d", "0x" substr($2, 4, 2))
        $5 = sprintf("%d", "0x" substr($2, 6, 2))
        printf "    static const Color& c_%-20s = Color::fromRgb8(%3d, %3d, %3d, 255);\n", $1, $3, $4, $5
    }'

    cat html_color.txt | awk '
    BEGIN {OFS = FS}
    {
        $2 = sprintf("\"%s\"", $1)
        printf "    if(colStr == %-22s) return c_%s;\n", $2, $1
    }'
    */

    // Color constants
    static const Color& c_black                = Color::fromRgb8(  0,   0,   0, 255); // CSS Level 1
    static const Color& c_silver               = Color::fromRgb8(192, 192, 192, 255);
    static const Color& c_gray                 = Color::fromRgb8(128, 128, 128, 255);
    static const Color& c_white                = Color::fromRgb8(255, 255, 255, 255);
    static const Color& c_maroon               = Color::fromRgb8(128,   0,   0, 255);
    static const Color& c_red                  = Color::fromRgb8(255,   0,   0, 255);
    static const Color& c_purple               = Color::fromRgb8(128,   0, 128, 255);
    static const Color& c_fuchsia              = Color::fromRgb8(255,   0, 255, 255);
    static const Color& c_green                = Color::fromRgb8(  0, 128,   0, 255);
    static const Color& c_lime                 = Color::fromRgb8(  0, 255,   0, 255);
    static const Color& c_olive                = Color::fromRgb8(128, 128,   0, 255);
    static const Color& c_yellow               = Color::fromRgb8(255, 255,   0, 255);
    static const Color& c_navy                 = Color::fromRgb8(  0,   0, 128, 255);
    static const Color& c_blue                 = Color::fromRgb8(  0,   0, 255, 255);
    static const Color& c_teal                 = Color::fromRgb8(  0, 128, 128, 255);
    static const Color& c_aqua                 = Color::fromRgb8(  0, 255, 255, 255);
    static const Color& c_orange               = Color::fromRgb8(255, 165,   0, 255); // CSS Level 2 Revision 1
    static const Color& c_aliceblue            = Color::fromRgb8(240, 248, 255, 255); // CSS Color Module Level 3
    static const Color& c_antiquewhite         = Color::fromRgb8(250, 235, 215, 255);
    static const Color& c_aquamarine           = Color::fromRgb8(127, 255, 212, 255);
    static const Color& c_azure                = Color::fromRgb8(240, 255, 255, 255);
    static const Color& c_beige                = Color::fromRgb8(245, 245, 220, 255);
    static const Color& c_bisque               = Color::fromRgb8(255, 228, 196, 255);
    static const Color& c_blanchedalmond       = Color::fromRgb8(255, 235, 205, 255);
    static const Color& c_blueviolet           = Color::fromRgb8(138,  43, 226, 255);
    static const Color& c_brown                = Color::fromRgb8(165,  42,  42, 255);
    static const Color& c_burlywood            = Color::fromRgb8(222, 184, 135, 255);
    static const Color& c_cadetblue            = Color::fromRgb8( 95, 158, 160, 255);
    static const Color& c_chartreuse           = Color::fromRgb8(127, 255,   0, 255);
    static const Color& c_chocolate            = Color::fromRgb8(210, 105,  30, 255);
    static const Color& c_coral                = Color::fromRgb8(255, 127,  80, 255);
    static const Color& c_cornflowerblue       = Color::fromRgb8(100, 149, 237, 255);
    static const Color& c_cornsilk             = Color::fromRgb8(255, 248, 220, 255);
    static const Color& c_crimson              = Color::fromRgb8(220,  20,  60, 255);
    static const Color& c_cyan                 = Color::fromRgb8(  0, 255, 255, 255);
    static const Color& c_darkblue             = Color::fromRgb8(  0,   0, 139, 255);
    static const Color& c_darkcyan             = Color::fromRgb8(  0, 139, 139, 255);
    static const Color& c_darkgoldenrod        = Color::fromRgb8(184, 134,  11, 255);
    static const Color& c_darkgray             = Color::fromRgb8(169, 169, 169, 255);
    static const Color& c_darkgreen            = Color::fromRgb8(  0, 100,   0, 255);
    static const Color& c_darkgrey             = Color::fromRgb8(169, 169, 169, 255);
    static const Color& c_darkkhaki            = Color::fromRgb8(189, 183, 107, 255);
    static const Color& c_darkmagenta          = Color::fromRgb8(139,   0, 139, 255);
    static const Color& c_darkolivegreen       = Color::fromRgb8( 85, 107,  47, 255);
    static const Color& c_darkorange           = Color::fromRgb8(255, 140,   0, 255);
    static const Color& c_darkorchid           = Color::fromRgb8(153,  50, 204, 255);
    static const Color& c_darkred              = Color::fromRgb8(139,   0,   0, 255);
    static const Color& c_darksalmon           = Color::fromRgb8(233, 150, 122, 255);
    static const Color& c_darkseagreen         = Color::fromRgb8(143, 188, 143, 255);
    static const Color& c_darkslateblue        = Color::fromRgb8( 72,  61, 139, 255);
    static const Color& c_darkslategray        = Color::fromRgb8( 47,  79,  79, 255);
    static const Color& c_darkslategrey        = Color::fromRgb8( 47,  79,  79, 255);
    static const Color& c_darkturquoise        = Color::fromRgb8(  0, 206, 209, 255);
    static const Color& c_darkviolet           = Color::fromRgb8(148,   0, 211, 255);
    static const Color& c_deeppink             = Color::fromRgb8(255,  20, 147, 255);
    static const Color& c_deepskyblue          = Color::fromRgb8(  0, 191, 255, 255);
    static const Color& c_dimgray              = Color::fromRgb8(105, 105, 105, 255);
    static const Color& c_dimgrey              = Color::fromRgb8(105, 105, 105, 255);
    static const Color& c_dodgerblue           = Color::fromRgb8( 30, 144, 255, 255);
    static const Color& c_firebrick            = Color::fromRgb8(178,  34,  34, 255);
    static const Color& c_floralwhite          = Color::fromRgb8(255, 250, 240, 255);
    static const Color& c_forestgreen          = Color::fromRgb8( 34, 139,  34, 255);
    static const Color& c_gainsboro            = Color::fromRgb8(220, 220, 220, 255);
    static const Color& c_ghostwhite           = Color::fromRgb8(248, 248, 255, 255);
    static const Color& c_gold                 = Color::fromRgb8(255, 215,   0, 255);
    static const Color& c_goldenrod            = Color::fromRgb8(218, 165,  32, 255);
    static const Color& c_greenyellow          = Color::fromRgb8(173, 255,  47, 255);
    static const Color& c_grey                 = Color::fromRgb8(128, 128, 128, 255);
    static const Color& c_honeydew             = Color::fromRgb8(240, 255, 240, 255);
    static const Color& c_hotpink              = Color::fromRgb8(255, 105, 180, 255);
    static const Color& c_indianred            = Color::fromRgb8(205,  92,  92, 255);
    static const Color& c_indigo               = Color::fromRgb8( 75,   0, 130, 255);
    static const Color& c_ivory                = Color::fromRgb8(255, 255, 240, 255);
    static const Color& c_khaki                = Color::fromRgb8(240, 230, 140, 255);
    static const Color& c_lavender             = Color::fromRgb8(230, 230, 250, 255);
    static const Color& c_lavenderblush        = Color::fromRgb8(255, 240, 245, 255);
    static const Color& c_lawngreen            = Color::fromRgb8(124, 252,   0, 255);
    static const Color& c_lemonchiffon         = Color::fromRgb8(255, 250, 205, 255);
    static const Color& c_lightblue            = Color::fromRgb8(173, 216, 230, 255);
    static const Color& c_lightcoral           = Color::fromRgb8(240, 128, 128, 255);
    static const Color& c_lightcyan            = Color::fromRgb8(224, 255, 255, 255);
    static const Color& c_lightgoldenrodyellow = Color::fromRgb8(250, 250, 210, 255);
    static const Color& c_lightgray            = Color::fromRgb8(211, 211, 211, 255);
    static const Color& c_lightgreen           = Color::fromRgb8(144, 238, 144, 255);
    static const Color& c_lightgrey            = Color::fromRgb8(211, 211, 211, 255);
    static const Color& c_lightpink            = Color::fromRgb8(255, 182, 193, 255);
    static const Color& c_lightsalmon          = Color::fromRgb8(255, 160, 122, 255);
    static const Color& c_lightseagreen        = Color::fromRgb8( 32, 178, 170, 255);
    static const Color& c_lightskyblue         = Color::fromRgb8(135, 206, 250, 255);
    static const Color& c_lightslategray       = Color::fromRgb8(119, 136, 153, 255);
    static const Color& c_lightslategrey       = Color::fromRgb8(119, 136, 153, 255);
    static const Color& c_lightsteelblue       = Color::fromRgb8(176, 196, 222, 255);
    static const Color& c_lightyellow          = Color::fromRgb8(255, 255, 224, 255);
    static const Color& c_limegreen            = Color::fromRgb8( 50, 205,  50, 255);
    static const Color& c_linen                = Color::fromRgb8(250, 240, 230, 255);
    static const Color& c_mediumaquamarine     = Color::fromRgb8(102, 205, 170, 255);
    static const Color& c_mediumblue           = Color::fromRgb8(  0,   0, 205, 255);
    static const Color& c_mediumorchid         = Color::fromRgb8(186,  85, 211, 255);
    static const Color& c_mediumpurple         = Color::fromRgb8(147, 112, 219, 255);
    static const Color& c_mediumseagreen       = Color::fromRgb8( 60, 179, 113, 255);
    static const Color& c_mediumslateblue      = Color::fromRgb8(123, 104, 238, 255);
    static const Color& c_mediumspringgreen    = Color::fromRgb8(  0, 250, 154, 255);
    static const Color& c_mediumturquoise      = Color::fromRgb8( 72, 209, 204, 255);
    static const Color& c_mediumvioletred      = Color::fromRgb8(199,  21, 133, 255);
    static const Color& c_midnightblue         = Color::fromRgb8( 25,  25, 112, 255);
    static const Color& c_mintcream            = Color::fromRgb8(245, 255, 250, 255);
    static const Color& c_mistyrose            = Color::fromRgb8(255, 228, 225, 255);
    static const Color& c_moccasin             = Color::fromRgb8(255, 228, 181, 255);
    static const Color& c_navajowhite          = Color::fromRgb8(255, 222, 173, 255);
    static const Color& c_oldlace              = Color::fromRgb8(253, 245, 230, 255);
    static const Color& c_olivedrab            = Color::fromRgb8(107, 142,  35, 255);
    static const Color& c_orangered            = Color::fromRgb8(255,  69,   0, 255);
    static const Color& c_orchid               = Color::fromRgb8(218, 112, 214, 255);
    static const Color& c_palegoldenrod        = Color::fromRgb8(238, 232, 170, 255);
    static const Color& c_palegreen            = Color::fromRgb8(152, 251, 152, 255);
    static const Color& c_paleturquoise        = Color::fromRgb8(175, 238, 238, 255);
    static const Color& c_palevioletred        = Color::fromRgb8(219, 112, 147, 255);
    static const Color& c_papayawhip           = Color::fromRgb8(255, 239, 213, 255);
    static const Color& c_peachpuff            = Color::fromRgb8(255, 218, 185, 255);
    static const Color& c_peru                 = Color::fromRgb8(205, 133,  63, 255);
    static const Color& c_pink                 = Color::fromRgb8(255, 192, 203, 255);
    static const Color& c_plum                 = Color::fromRgb8(221, 160, 221, 255);
    static const Color& c_powderblue           = Color::fromRgb8(176, 224, 230, 255);
    static const Color& c_rosybrown            = Color::fromRgb8(188, 143, 143, 255);
    static const Color& c_royalblue            = Color::fromRgb8( 65, 105, 225, 255);
    static const Color& c_saddlebrown          = Color::fromRgb8(139,  69,  19, 255);
    static const Color& c_salmon               = Color::fromRgb8(250, 128, 114, 255);
    static const Color& c_sandybrown           = Color::fromRgb8(244, 164,  96, 255);
    static const Color& c_seagreen             = Color::fromRgb8( 46, 139,  87, 255);
    static const Color& c_seashell             = Color::fromRgb8(255, 245, 238, 255);
    static const Color& c_sienna               = Color::fromRgb8(160,  82,  45, 255);
    static const Color& c_skyblue              = Color::fromRgb8(135, 206, 235, 255);
    static const Color& c_slateblue            = Color::fromRgb8(106,  90, 205, 255);
    static const Color& c_slategray            = Color::fromRgb8(112, 128, 144, 255);
    static const Color& c_slategrey            = Color::fromRgb8(112, 128, 144, 255);
    static const Color& c_snow                 = Color::fromRgb8(255, 250, 250, 255);
    static const Color& c_springgreen          = Color::fromRgb8(  0, 255, 127, 255);
    static const Color& c_steelblue            = Color::fromRgb8( 70, 130, 180, 255);
    static const Color& c_tan                  = Color::fromRgb8(210, 180, 140, 255);
    static const Color& c_thistle              = Color::fromRgb8(216, 191, 216, 255);
    static const Color& c_tomato               = Color::fromRgb8(255,  99,  71, 255);
    static const Color& c_turquoise            = Color::fromRgb8( 64, 224, 208, 255);
    static const Color& c_violet               = Color::fromRgb8(238, 130, 238, 255);
    static const Color& c_wheat                = Color::fromRgb8(245, 222, 179, 255);
    static const Color& c_whitesmoke           = Color::fromRgb8(245, 245, 245, 255);
    static const Color& c_yellowgreen          = Color::fromRgb8(154, 205,  50, 255);
    static const Color& c_rebeccapurple        = Color::fromRgb8(102,  51, 153, 255); // CSS Color Module Level 4

    // Compare the names
    if(colStr == "black"               ) return c_black;
    if(colStr == "silver"              ) return c_silver;
    if(colStr == "gray"                ) return c_gray;
    if(colStr == "white"               ) return c_white;
    if(colStr == "maroon"              ) return c_maroon;
    if(colStr == "red"                 ) return c_red;
    if(colStr == "purple"              ) return c_purple;
    if(colStr == "fuchsia"             ) return c_fuchsia;
    if(colStr == "green"               ) return c_green;
    if(colStr == "lime"                ) return c_lime;
    if(colStr == "olive"               ) return c_olive;
    if(colStr == "yellow"              ) return c_yellow;
    if(colStr == "navy"                ) return c_navy;
    if(colStr == "blue"                ) return c_blue;
    if(colStr == "teal"                ) return c_teal;
    if(colStr == "aqua"                ) return c_aqua;
    if(colStr == "orange"              ) return c_orange;
    if(colStr == "aliceblue"           ) return c_aliceblue;
    if(colStr == "antiquewhite"        ) return c_antiquewhite;
    if(colStr == "aquamarine"          ) return c_aquamarine;
    if(colStr == "azure"               ) return c_azure;
    if(colStr == "beige"               ) return c_beige;
    if(colStr == "bisque"              ) return c_bisque;
    if(colStr == "blanchedalmond"      ) return c_blanchedalmond;
    if(colStr == "blueviolet"          ) return c_blueviolet;
    if(colStr == "brown"               ) return c_brown;
    if(colStr == "burlywood"           ) return c_burlywood;
    if(colStr == "cadetblue"           ) return c_cadetblue;
    if(colStr == "chartreuse"          ) return c_chartreuse;
    if(colStr == "chocolate"           ) return c_chocolate;
    if(colStr == "coral"               ) return c_coral;
    if(colStr == "cornflowerblue"      ) return c_cornflowerblue;
    if(colStr == "cornsilk"            ) return c_cornsilk;
    if(colStr == "crimson"             ) return c_crimson;
    if(colStr == "cyan"                ) return c_cyan;
    if(colStr == "darkblue"            ) return c_darkblue;
    if(colStr == "darkcyan"            ) return c_darkcyan;
    if(colStr == "darkgoldenrod"       ) return c_darkgoldenrod;
    if(colStr == "darkgray"            ) return c_darkgray;
    if(colStr == "darkgreen"           ) return c_darkgreen;
    if(colStr == "darkgrey"            ) return c_darkgrey;
    if(colStr == "darkkhaki"           ) return c_darkkhaki;
    if(colStr == "darkmagenta"         ) return c_darkmagenta;
    if(colStr == "darkolivegreen"      ) return c_darkolivegreen;
    if(colStr == "darkorange"          ) return c_darkorange;
    if(colStr == "darkorchid"          ) return c_darkorchid;
    if(colStr == "darkred"             ) return c_darkred;
    if(colStr == "darksalmon"          ) return c_darksalmon;
    if(colStr == "darkseagreen"        ) return c_darkseagreen;
    if(colStr == "darkslateblue"       ) return c_darkslateblue;
    if(colStr == "darkslategray"       ) return c_darkslategray;
    if(colStr == "darkslategrey"       ) return c_darkslategrey;
    if(colStr == "darkturquoise"       ) return c_darkturquoise;
    if(colStr == "darkviolet"          ) return c_darkviolet;
    if(colStr == "deeppink"            ) return c_deeppink;
    if(colStr == "deepskyblue"         ) return c_deepskyblue;
    if(colStr == "dimgray"             ) return c_dimgray;
    if(colStr == "dimgrey"             ) return c_dimgrey;
    if(colStr == "dodgerblue"          ) return c_dodgerblue;
    if(colStr == "firebrick"           ) return c_firebrick;
    if(colStr == "floralwhite"         ) return c_floralwhite;
    if(colStr == "forestgreen"         ) return c_forestgreen;
    if(colStr == "gainsboro"           ) return c_gainsboro;
    if(colStr == "ghostwhite"          ) return c_ghostwhite;
    if(colStr == "gold"                ) return c_gold;
    if(colStr == "goldenrod"           ) return c_goldenrod;
    if(colStr == "greenyellow"         ) return c_greenyellow;
    if(colStr == "grey"                ) return c_grey;
    if(colStr == "honeydew"            ) return c_honeydew;
    if(colStr == "hotpink"             ) return c_hotpink;
    if(colStr == "indianred"           ) return c_indianred;
    if(colStr == "indigo"              ) return c_indigo;
    if(colStr == "ivory"               ) return c_ivory;
    if(colStr == "khaki"               ) return c_khaki;
    if(colStr == "lavender"            ) return c_lavender;
    if(colStr == "lavenderblush"       ) return c_lavenderblush;
    if(colStr == "lawngreen"           ) return c_lawngreen;
    if(colStr == "lemonchiffon"        ) return c_lemonchiffon;
    if(colStr == "lightblue"           ) return c_lightblue;
    if(colStr == "lightcoral"          ) return c_lightcoral;
    if(colStr == "lightcyan"           ) return c_lightcyan;
    if(colStr == "lightgoldenrodyellow") return c_lightgoldenrodyellow;
    if(colStr == "lightgray"           ) return c_lightgray;
    if(colStr == "lightgreen"          ) return c_lightgreen;
    if(colStr == "lightgrey"           ) return c_lightgrey;
    if(colStr == "lightpink"           ) return c_lightpink;
    if(colStr == "lightsalmon"         ) return c_lightsalmon;
    if(colStr == "lightseagreen"       ) return c_lightseagreen;
    if(colStr == "lightskyblue"        ) return c_lightskyblue;
    if(colStr == "lightslategray"      ) return c_lightslategray;
    if(colStr == "lightslategrey"      ) return c_lightslategrey;
    if(colStr == "lightsteelblue"      ) return c_lightsteelblue;
    if(colStr == "lightyellow"         ) return c_lightyellow;
    if(colStr == "limegreen"           ) return c_limegreen;
    if(colStr == "linen"               ) return c_linen;
    if(colStr == "mediumaquamarine"    ) return c_mediumaquamarine;
    if(colStr == "mediumblue"          ) return c_mediumblue;
    if(colStr == "mediumorchid"        ) return c_mediumorchid;
    if(colStr == "mediumpurple"        ) return c_mediumpurple;
    if(colStr == "mediumseagreen"      ) return c_mediumseagreen;
    if(colStr == "mediumslateblue"     ) return c_mediumslateblue;
    if(colStr == "mediumspringgreen"   ) return c_mediumspringgreen;
    if(colStr == "mediumturquoise"     ) return c_mediumturquoise;
    if(colStr == "mediumvioletred"     ) return c_mediumvioletred;
    if(colStr == "midnightblue"        ) return c_midnightblue;
    if(colStr == "mintcream"           ) return c_mintcream;
    if(colStr == "mistyrose"           ) return c_mistyrose;
    if(colStr == "moccasin"            ) return c_moccasin;
    if(colStr == "navajowhite"         ) return c_navajowhite;
    if(colStr == "oldlace"             ) return c_oldlace;
    if(colStr == "olivedrab"           ) return c_olivedrab;
    if(colStr == "orangered"           ) return c_orangered;
    if(colStr == "orchid"              ) return c_orchid;
    if(colStr == "palegoldenrod"       ) return c_palegoldenrod;
    if(colStr == "palegreen"           ) return c_palegreen;
    if(colStr == "paleturquoise"       ) return c_paleturquoise;
    if(colStr == "palevioletred"       ) return c_palevioletred;
    if(colStr == "papayawhip"          ) return c_papayawhip;
    if(colStr == "peachpuff"           ) return c_peachpuff;
    if(colStr == "peru"                ) return c_peru;
    if(colStr == "pink"                ) return c_pink;
    if(colStr == "plum"                ) return c_plum;
    if(colStr == "powderblue"          ) return c_powderblue;
    if(colStr == "rosybrown"           ) return c_rosybrown;
    if(colStr == "royalblue"           ) return c_royalblue;
    if(colStr == "saddlebrown"         ) return c_saddlebrown;
    if(colStr == "salmon"              ) return c_salmon;
    if(colStr == "sandybrown"          ) return c_sandybrown;
    if(colStr == "seagreen"            ) return c_seagreen;
    if(colStr == "seashell"            ) return c_seashell;
    if(colStr == "sienna"              ) return c_sienna;
    if(colStr == "skyblue"             ) return c_skyblue;
    if(colStr == "slateblue"           ) return c_slateblue;
    if(colStr == "slategray"           ) return c_slategray;
    if(colStr == "slategrey"           ) return c_slategrey;
    if(colStr == "snow"                ) return c_snow;
    if(colStr == "springgreen"         ) return c_springgreen;
    if(colStr == "steelblue"           ) return c_steelblue;
    if(colStr == "tan"                 ) return c_tan;
    if(colStr == "thistle"             ) return c_thistle;
    if(colStr == "tomato"              ) return c_tomato;
    if(colStr == "turquoise"           ) return c_turquoise;
    if(colStr == "violet"              ) return c_violet;
    if(colStr == "wheat"               ) return c_wheat;
    if(colStr == "whitesmoke"          ) return c_whitesmoke;
    if(colStr == "yellowgreen"         ) return c_yellowgreen;
    if(colStr == "rebeccapurple"       ) return c_rebeccapurple;

    // Unknown/unsupported color
    return c_black;
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

const Color SvgRasterizer::fromHtmlColor(const std::string& colStr_)
{
    // Remove spaces and convert to lower case
    std::string colStr = colStr_;
    colStr.erase(remove_if(colStr.begin(), colStr.end(), ::isspace), colStr.end());
    std::transform(colStr.begin(), colStr.end(), colStr.begin(), ::tolower);

    // Get the length and C-string
    const size_t clen = colStr.length();
    const char*  cstr = colStr.c_str();

    // Hex RGB/RGBA color?
    // #RGB    / #RGBA     : R/G/B/A =  0 -  F
    // #RRGGBB / #RRGGBBAA : R/G/B/A = 00 - FF
    if(cstr[0] == '#') {
        int r = 0;
        int g = 0;
        int b = 0;
        int a = 255;
             if(clen == 3 + 1) sscanf(cstr + 1, "%1x%1x%1x",        &r, &g, &b    );
        else if(clen == 4 + 1) sscanf(cstr + 1, "%1x%1x%1x%1x",     &r, &g, &b, &a);
        else if(clen == 6 + 1) sscanf(cstr + 1, "%02x%02x%02x",     &r, &g, &b    );
        else if(clen == 8 + 1) sscanf(cstr + 1, "%02x%02x%02x%02x", &r, &g, &b, &a);
        return Color::fromRgb8(r, g, b, a);
    }

    // RGBA color?
    // rgba(RRR, GGG, BBB, A.A) : RRR/GGG/BBB = 0   - 255
    //                            A.A         = 0.0 - 1.0
    if(clen >= 13 && cstr[0] == 'r' && cstr[1] == 'g' && cstr[2] == 'b' && cstr[3] == 'a' && cstr[4] == '(' &&  cstr[clen - 1] == ')') {
        char  c1, c2, c3;
        int   r = 0;
        int   g = 0;
        int   b = 0;
        float a = 1.0f;
        sscanf(cstr + 5, "%3d%1c%3d%1c%3d%1c%f", &r, &c1, &g, &c2, &b, &c3, &a);
        if(c1 != ',' || c2 != ',' || c3 != ',') {
            r = g = b = 0;
            a = 1.0f;
        }
        return Color::fromRgb8(r, g, b, a * 255.0f);
    }

    // RGB color?
    // rgb(RRR, GGG, BBB) : RRR/GGG/BBB = 0 - 255
    if(clen >= 10 && cstr[0] == 'r' && cstr[1] == 'g' && cstr[2] == 'b' && cstr[3] == '(' &&  cstr[clen - 1] == ')') {
        char c1, c2;
        int  r = 0;
        int  g = 0;
        int  b = 0;
        sscanf(cstr + 4, "%3d%1c%3d%1c%3d", &r, &c1, &g, &c2, &b);
        if(c1 != ',' || c2 != ',') {
            r = g = b = 0;
        }
        return Color::fromRgb8(r, g, b, 255);
    }

    // HSL color?
    // hsl(HHH, SSS%, LLL%) : HHH     = 0  - 360
    //                        SSS/LLL = 0% - 100%
    if(clen >= 12 && cstr[0] == 'h' && cstr[1] == 's' && cstr[2] == 'l' && cstr[3] == '(' &&  cstr[clen - 1] == ')') {
        char c1, c2, p1, p2;
        int  h = 0;
        int  s = 0;
        int  l = 0;
        sscanf(cstr + 4, "%3d%c%3d%c%c%3d%c", &h, &c1, &s, &p1, &c2, &l, &p2);
        if(c1 != ',' || c2 != ',' || p1 != '%' || p2 != '%') {
            h = s = l = 0;
        }
        return fromCssHsl(h, s, l);
    }

    // HWB color?
    // hsl(HHH, WWW%, BBB%) : HHH     = 0  - 360
    //                        WWW/HHH = 0% - 100%
    if(clen >= 12 && cstr[0] == 'h' && cstr[1] == 'w' && cstr[2] == 'b' && cstr[3] == '(' &&  cstr[clen - 1] == ')') {
        char c1, c2, p1, p2;
        int  h = 0;
        int  w = 0;
        int  b = 100;
        sscanf(cstr + 4, "%3d%c%3d%c%c%3d%c", &h, &c1, &w, &p1, &c2, &b, &p2);
        if(c1 != ',' || c2 != ',' || p1 != '%' || p2 != '%') {
            h = w = 0;
            b = 100;
        }
        return fromCssHwb(h, w, b);
    }

    // CMYK color?
    // cmyk(CCC%, YYY%, MMM%, KKK%) : CCC/YYY/MMM/KKK = 0 - 100%
    if(clen >= 17 && cstr[0] == 'c' && cstr[1] == 'm' && cstr[2] == 'y' && cstr[3] == 'k' && cstr[4] == '(' &&  cstr[clen - 1] == ')') {
        char c1, c2, c3, p1, p2, p3, p4;
        int  c = 0;
        int  m = 0;
        int  y = 0;
        int  k = 100;
        sscanf(cstr + 5, "%3d%c%c%3d%c%c%3d%c%c%3d%c", &c, &p1, &c1, &m, &p2, &c2, &y, &p3, &c3, &k, &p4);
        if(c1 != ',' || c2 != ',' || c3 != ','  || p1 != '%' || p2 != '%' || p3 != '%' || p4 != '%') {
            c = m = y = 0;
            k = 100;
        }
        return fromCssCmyk(c, m, y, k);
    }

    // Natural color?
    // N[VV], WWW%, HHH% :  N       = R, Y, G, C, B, M, W
    //                      VV      = 10, 20, 30, 40, 50, 60, 90
    //                      WWW/HHH = 0 - 100%
    if(clen >= 8 && cstr[clen - 1] == '%') {
        char c1, c2, p1, p2;
        char n = 'W';
        int  h = 0;
        int  w = 0;
        int  b = 100;
        // N..,..%,..%
        sscanf(cstr, "%c%2d%c%3d%c%c%3d%c", &n, &h, &c1, &w, &p1, &c2, &b, &p2);
        if(c1 != ',' || c2 != ',' || p1 != '%' || p2 != '%') {
            // N,..%,..%
            h = 0;
            sscanf(cstr, "%c%c%3d%c%c%3d%c", &n, &c1, &w, &p1, &c2, &b, &p2);
            if(c1 != ',' || c2 != ',' || p1 != '%' || p2 != '%') {
                n = 'W';
                h = w = 0;
                b = 100;
            }
        }
        return fromCssNCol(n, h, w, b);
    }

    // Check against named colors
    // Please refer to: https://www.w3schools.com/colors/colors_names.asp
    //                  https://developer.mozilla.org/en-US/docs/Web/CSS/color_value
    return fromCssNamedColor(colStr);
}


} // namespace
} // namespace
