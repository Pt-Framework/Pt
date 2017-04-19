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

#include <stack>

#include <Pt/IOError.h>

#include <Pt/Xml/InputSource.h>
#include <Pt/Xml/XmlReader.h>

#include <Pt/Gfx/SGNode.h>
#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Xml {
    class AttributeList;
}
}


namespace Pt {
namespace Gfx {


class SGNode;


class SvgRasterizer
{
    public:
        SvgRasterizer(std::istream& is, Image& image, const PointF& topLeft);

        ~SvgRasterizer();

        Image& image();

        bool advance();

        // ### TODO: Add support for animated SVG! ###

    private:
        static inline const std::string lcaseStdStr(const std::string & str);
        static inline const std::string lcaseStdStr(const Pt::String& str);
        static inline const Pt::String lcasePtStr(const Pt::String& str);

        static inline const std::string ltrimStdStr(const std::string & str);
        static inline const std::string rtrimStdStr(const std::string & str);
        static inline const std::string lrtrimStdStr(const std::string & str);
        static inline const std::string removeAllSpacesStdStr(const std::string & str);

        static inline double cnvStrToDbl(const std::string& s, const std::string& sectionInfo);
        static inline double cnvStrToDbl(const Pt::String& s, const std::string& sectionInfo);
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
        struct SvgStyleData;

        struct ComparePen;
        struct CompareBrush;

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
        static inline const Color fromHtmlColor(const Pt::String& colStr);

        // Defined in "SvgRasterizer_Lexer.cpp"
        static void lexPathData(std::vector<std::string>& tokens, const std::string& str);
        static void lexStyleData(std::vector<std::string>& tokens, const std::string& str);
        static void lexTransformData(std::vector<std::string>& tokens, const std::string& str);

        static inline void lexPathData(std::vector<std::string>& tokens, const Pt::String& str);
        static inline void lexStyleData(std::vector<std::string>& tokens, const Pt::String& str);
        static inline void lexTransformData(std::vector<std::string>& tokens, const Pt::String& str);

        static inline void copyPenData(SvgStyleData& ssd, const Pen& pen);
        static inline void copyBrushData(SvgStyleData& ssd, const Brush& brush);

        static void extractStyleData(SvgStyleData& ssd, const SGNode& parent, const Xml::AttributeList& alist, const std::string& sectionInfo);
        inline void applyStyleData(SGNode& sgn, const SvgStyleData& ssd);

        // Defined in "SvgRasterizer_Util.cpp"
        static double cnvUnitStrToPixels(const std::string& str);

        void processSvgElementAttributes(const Xml::StartElement& elem);

        SGNode* processDrawingElement(const Xml::StartElement& elem);
        SGNode* processDrawingElement_g(SGNode& parent, const Xml::AttributeList& alist);
        SGNode* processDrawingElement_line(SGNode& parent, const Xml::AttributeList& alist);
};


// ======================================================================================
// ===== Private Member Structure Definitions ===========================================
// ======================================================================================

struct SvgRasterizer::SvgStyleData {
    bool           penSpecified;
    Color          penColor;
    Pt::size_t     penSize;
    Pen::Style     penStyle;
    Pen::CapStyle  penCapStyle;
    Pen::JoinStyle penJoinStyle;
    Pt::uint64_t   penStylePattern;

    inline SvgStyleData()
    : penSpecified   (false)
    , penSize        (1)
    , penStyle       (Pen::Solid)
    , penCapStyle    (Pen::FlatCap)
    , penJoinStyle   (Pen::MiterJoin)
    , penStylePattern(0)
    {}
};

struct SvgRasterizer::ComparePen {
    bool operator()(const Pen& a, const Pen&b) const
    {
        const Pt::uint64_t aCol = ( Pt::uint64_t(a.color().alpha()) << 48 ) |
                                  ( Pt::uint64_t(a.color().red  ()) << 32 ) |
                                  ( Pt::uint64_t(a.color().green()) << 16 ) |
                                  ( Pt::uint64_t(a.color().blue ()) <<  0 );
        const Pt::uint64_t bCol = ( Pt::uint64_t(a.color().alpha()) << 48 ) |
                                  ( Pt::uint64_t(a.color().red  ()) << 32 ) |
                                  ( Pt::uint64_t(a.color().green()) << 16 ) |
                                  ( Pt::uint64_t(a.color().blue ()) <<  0 );

/*
     if(a._name < b._name)
        return true;

    if(a._name > b._name)
        return false;

    return a._style < b._style;*
        const Color& color() const;
        std::size_t size() const;
        Style style() const;
        Pt::uint64_t styleUserPattern() const;
        void setCapStyle(CapStyle cap = FlatCap);
        JoinStyle joinStyle() const;
*/

        return false;
    }
};

struct SvgRasterizer::CompareBrush {
    bool operator()(const Brush& a, const Brush&b) const
    {
        // ### TODO ###
        return false;
    }
};

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
    double             vpWidth;      // Width  of the viewport (negative: relative percentage; positive: absolute pixels)
    double             vpHeight;     // Height of the viewport (negative: relative percentage; positive: absolute pixels)
    double             vbX;          // Viewbox top-left X coordinate
    double             vbY;          // Viewbox top-left Y coordinate
    double             vbW;          // Viewbox width
    double             vbH;          // Viewbox height
    AspectRatioMode    arMode;       // Aspect ratio mode
    SGNode::TransformT vpbTransform; // The viewport-viewbox transform (projection-view matrix in OpenGL worlds ;)

    // Scene graph objects
    // ### TODO: Add support for animated SVG! ###
    SGNode*             sgParent; // Parent node
    std::stack<SGNode*> sgStack;  // Node stack

    // Caches
    std::set<Pen,   ComparePen  > penSet;     // A set of pens
    std::set<Brush, CompareBrush> brushSet;   // A set of brushes
    SvgObjects                    svgObjects; // A map between reference names and their corresponding SVG objects

    // Construct a raster state object
    RasterState(Image& image, const PointF& topLeft);

    // Destruct a raster state object
    ~RasterState();

    // Clear all caches
    void clearAllCaches();
};


// ======================================================================================
// ===== Inlined Private Member Functions ===============================================
// ======================================================================================

//
// Character case converters
//

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


//
// White-space removers
//

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


//
// Converters and checkers
//

inline double SvgRasterizer::cnvStrToDbl(const std::string& s, const std::string& sectionInfo)
{
    char*  end = 0;
    double val = strtod(s.c_str(), &end);

    if(*end || val == HUGE_VAL)
        throw IOError("svg error: " + sectionInfo + ": invalid number '" + s + "'");

    return val;
}

inline double SvgRasterizer::cnvStrToDbl(const Pt::String& s, const std::string& sectionInfo)
{ return cnvStrToDbl(s.narrow(), sectionInfo); }

inline const std::string& SvgRasterizer::passValidNumber(const std::string& s, const std::string& sectionInfo)
{
    cnvStrToDbl(s, sectionInfo);
    return s;
}

inline const Color SvgRasterizer::fromHtmlColor(const Pt::String& colStr)
{ return fromHtmlColor(colStr.narrow()); }


//
// Lexers
//

inline void SvgRasterizer::lexPathData(std::vector<std::string>& tokens, const Pt::String& str)
{ lexPathData(tokens, str.narrow()); }

inline void SvgRasterizer::lexStyleData(std::vector<std::string>& tokens, const Pt::String& str)
{ lexStyleData(tokens, str.narrow()); }

inline void SvgRasterizer::lexTransformData(std::vector<std::string>& tokens, const Pt::String& str)
{ lexTransformData(tokens, str.narrow()); }


//
// Other helper functions
//

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

inline void SvgRasterizer::copyPenData(SvgStyleData& ssd, const Pen& pen)
{
    if(pen.isNull()) return;

    ssd.penColor        = pen.color();
    ssd.penSize         = pen.size();
    ssd.penStyle        = pen.style();
    ssd.penCapStyle     = pen.capStyle();
    ssd.penJoinStyle    = pen.joinStyle();
    ssd.penStylePattern = pen.styleUserPattern();
}

inline void SvgRasterizer::copyBrushData(SvgStyleData& ssd, const Brush& brush)
{
    // ### TODO ###
}

inline void SvgRasterizer::applyStyleData(SGNode& sgn, const SvgRasterizer::SvgStyleData& ssd)
{
    // Set the pen as needed
    if(ssd.penSpecified) {
        // Generate the pen
        const Pen pen = (ssd.penStyle == Pen::UserDefined)
                       ? Pen(ssd.penColor, ssd.penSize, ssd.penStylePattern, ssd.penCapStyle, ssd.penJoinStyle)
                       : Pen(ssd.penColor, ssd.penSize, ssd.penStyle,        ssd.penCapStyle, ssd.penJoinStyle);
        // If a pen with the same parameters already exists, assign it
        std::set<Pen>::const_iterator it = _rstate->penSet.find(pen);
        if(it != _rstate->penSet.end()) {
            sgn.setPen(*it);
        }
        // Assign and store the new pen
        else {
            sgn.setPen(pen);
            _rstate->penSet.insert(pen);
        }

    }

    // Set the brush as needed
    // ### TODO ###
    // _rstate->brushSet;
}


} // namespace
} // namespace

#endif
