/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2017-2017 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_SVGRASTERIZER_H
#define PT_GFX_SVGRASTERIZER_H

#include <algorithm>
#include <iostream>

#include <Pt/IOError.h>

#include <Pt/Xml/InputSource.h>
#include <Pt/Xml/XmlReader.h>

#include <Pt/Gfx/TransformStack.h>
#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Gfx {


class SvgRasterizer
{
    public:
        SvgRasterizer(std::istream& is, Image& image, const PointF& topLeft);

        ~SvgRasterizer();

        Image& image();

        bool advance();

    private:
        static inline const std::string lcaseStdStr(const std::string & str);
        static inline const std::string lcaseStdStr(const Pt::String& str);
        static inline const Pt::String lcasePtStr(const Pt::String& str);

        static inline const std::string ltrimStdStr(const std::string & str);
        static inline const std::string rtrimStdStr(const std::string & str);
        static inline const std::string lrtrimStdStr(const std::string & str);
        static inline const std::string removeAllSpacesStdStr(const std::string & str);

        static inline double cnvStrToDbl(const std::string& s, const std::string& sectionInfo);
        static inline const std::string& passValidNumber(const std::string& s, const std::string& sectionInfo);

        static inline const std::vector<std::string> tokenizeBySpace(const std::string& str);

    private:
        enum AspectRatioMode {
            None,
            XMinYMinMeet,  XMinYMidMeet,  XMinYMaxMeet,
            XMidYMinMeet,  XMidYMidMeet,  XMidYMaxMeet,
            XMaxYMinMeet,  XMaxYMidMeet,  XMaxYMaxMeet,
            XMinYMinSlice, XMinYMidSlice, XMinYMaxSlice,
            XMidYMinSlice, XMidYMidSlice, XMidYMaxSlice,
            XMaxYMinSlice, XMaxYMidSlice, XMaxYMaxSlice
        };

        struct SvgObject;
        struct RasterState;

    private:
        RasterState*           _rstate;

        Xml::BinaryInputSource _binaryInputSource;
        Xml::XmlReader         _xmlReader;

    private:
        // Defined in "SvgRasterizer.cpp"
        void renderNextFrame();

        // Defined in "SvgRasterizer_Color.cpp"
        static const Color fromHtmlColor(const std::string& colStr);

        // Defined in "SvgRasterizer_Lexer.cpp"
        static void lexPathData(std::vector<std::string>& tokens, const std::string& str);
        static void lexStyleData(std::vector<std::string>& tokens, const std::string& str);
        static void lexTransformData(std::vector<std::string>& tokens, const std::string& str);

        // Defined in "SvgRasterizer_Util.cpp"
        static double cnvUnitStrToPixels(const std::string& str);

        static void processSvgElementParameters(RasterState& rs, const Xml::StartElement& elem);

        static void processDrawingElement(RasterState& rs, const Xml::StartElement& elem);
};


// ======================================================================================
// ===== Private Member Structure Definitions ===========================================
// ======================================================================================

struct SvgRasterizer::RasterState {
    // Typedefs
    typedef std::map<std::string, SvgObject*> SvgObjects;

    // State flags
    bool gotStart;   // A flag that indicates that we have got the SVG opening tag
    bool gotEnd;     // A flag that indicates that we have got the SVG closing tag
    bool renderInit; // A flag that indicates if the rendering-related data has been initialized

    // Rendering target
    Image&        image;    // Target image
    ImagePainter2 painter;  // Target painter
    PointF        topLeft;  // Starting (top-left) coordinate for rendering the SVG

    // Viewport and viewbox
    double          vpWidth;      // Width  of the viewport (negative: relative percentage; positive: absolute pixels)
    double          vpHeight;     // Height of the viewport (negative: relative percentage; positive: absolute pixels)
    double          vbX;          // Viewbox top-left X coordinate
    double          vbY;          // Viewbox top-left Y coordinate
    double          vbW;          // Viewbox width
    double          vbH;          // Viewbox height
    AspectRatioMode arMode;       // Aspect ratio mode
    Transform       vpbTransform; // The viewport-viewbox transform (projection-view matrix in OpenGL worlds ;)

    // Caches
    std::set<Pen>   penSet;     // A set of pens
    std::set<Brush> brushSet;   // A set of brushes
    SvgObjects      svgObjects; // A map between reference names and their corresponding SVG objects

    // Construct a raster state object
    RasterState(Image& image, const PointF& topLeft);

    // Destruct a raster state object
    ~RasterState();
};


// ======================================================================================
// ===== Inlined Private Member Functions ===============================================
// ======================================================================================

inline const std::string SvgRasterizer::lcaseStdStr(const std::string & str_)
{
    std::string  str = str_;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}

inline const std::string SvgRasterizer::lcaseStdStr(const Pt::String& str_)
{
    Pt::String str = str_;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str.narrow();
}

inline const Pt::String SvgRasterizer::lcasePtStr(const Pt::String& str_)
{
    Pt::String str = str_;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}

inline const std::string SvgRasterizer::ltrimStdStr(const std::string & str)
{
    const size_t idx = str.find_first_not_of(" \t\v\n\r\f");

    if(idx != std::string::npos) return str.substr(idx);
    return str;
}

inline const std::string SvgRasterizer::rtrimStdStr(const std::string & str)
{
    const size_t idx = str.find_last_not_of(" \t\v\n\r\f");

    if(idx != std::string::npos) return str.substr(0, idx + 1);
    return str;
}

inline const std::string SvgRasterizer::lrtrimStdStr(const std::string & str)
{ return rtrimStdStr(ltrimStdStr(str)); }

inline const std::string SvgRasterizer::removeAllSpacesStdStr(const std::string & str_)
{
    std::string str = str_;
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());

    return str;
}

inline double SvgRasterizer::cnvStrToDbl(const std::string& s, const std::string& sectionInfo)
{
    char*  end = 0;
    double val = strtod(s.c_str(), &end);

    if(*end || val == HUGE_VAL)
        throw IOError("svg error: " + sectionInfo + ": invalid number '" + s + "'");

    return val;
}

inline const std::string& SvgRasterizer::passValidNumber(const std::string& s, const std::string& sectionInfo)
{
    cnvStrToDbl(s, sectionInfo);
    return s;
}

inline const std::vector<std::string> SvgRasterizer::tokenizeBySpace(const std::string& str_)
{
    std::vector<std::string> result;

    const char* str = str_.c_str();
    do {
        const char *begin = str;
        while(*str && !::isspace(*str)) ++str;
        result.push_back(std::string(begin, str));

    } while(*str++);

    return result;
}


} // namespace
} // namespace

#endif
