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


//
// Comparators
//

struct SvgRasterizer::ComparePen
{
    inline bool operator()(const Pen& a, const Pen&b) const
    {
        const Pt::uint64_t aCol = ( Pt::uint64_t(a.color().alpha()) << 48 ) |
                                  ( Pt::uint64_t(a.color().red  ()) << 32 ) |
                                  ( Pt::uint64_t(a.color().green()) << 16 ) |
                                  ( Pt::uint64_t(a.color().blue ()) <<  0 );
        const Pt::uint64_t bCol = ( Pt::uint64_t(b.color().alpha()) << 48 ) |
                                  ( Pt::uint64_t(b.color().red  ()) << 32 ) |
                                  ( Pt::uint64_t(b.color().green()) << 16 ) |
                                  ( Pt::uint64_t(b.color().blue ()) <<  0 );
        if(aCol < bCol) return true;
        if(aCol > bCol) return false;

        if(a.size() < b.size()) return true;
        if(a.size() > b.size()) return false;

        if(a.style() < b.style()) return true;
        if(a.style() > b.style()) return false;

        if(a.capStyle() < b.capStyle()) return true;
        if(a.capStyle() > b.capStyle()) return false;

        if(a.joinStyle() < b.joinStyle()) return true;
        if(a.joinStyle() > b.joinStyle()) return false;

        return a.styleUserPattern() < b.styleUserPattern();
    }
};

struct SvgRasterizer::CompareBrush {
    inline bool operator()(const Brush& a, const Brush&b) const
    {
        const Pt::uint64_t aCol1 = ( Pt::uint64_t(a.color().alpha()) << 48 ) |
                                   ( Pt::uint64_t(a.color().red  ()) << 32 ) |
                                   ( Pt::uint64_t(a.color().green()) << 16 ) |
                                   ( Pt::uint64_t(a.color().blue ()) <<  0 );
        const Pt::uint64_t bCol1 = ( Pt::uint64_t(b.color().alpha()) << 48 ) |
                                   ( Pt::uint64_t(b.color().red  ()) << 32 ) |
                                   ( Pt::uint64_t(b.color().green()) << 16 ) |
                                   ( Pt::uint64_t(b.color().blue ()) <<  0 );
        if(aCol1 < bCol1) return true;
        if(aCol1 > bCol1) return false;

        const Pt::uint64_t aCol2 = ( Pt::uint64_t(a.gradientColor().alpha()) << 48 ) |
                                   ( Pt::uint64_t(a.gradientColor().red  ()) << 32 ) |
                                   ( Pt::uint64_t(a.gradientColor().green()) << 16 ) |
                                   ( Pt::uint64_t(a.gradientColor().blue ()) <<  0 );
        const Pt::uint64_t bCol2 = ( Pt::uint64_t(b.gradientColor().alpha()) << 48 ) |
                                   ( Pt::uint64_t(b.gradientColor().red  ()) << 32 ) |
                                   ( Pt::uint64_t(b.gradientColor().green()) << 16 ) |
                                   ( Pt::uint64_t(b.gradientColor().blue ()) <<  0 );
        if(aCol2 < bCol2) return true;
        if(aCol2 > bCol2) return false;

        if(a.fillStyle() < b.fillStyle()) return true;
        if(a.fillStyle() > b.fillStyle()) return false;

        if(a.rotation() < b.rotation()) return true;
        if(a.rotation() > b.rotation()) return false;

        if(a.scale() < b.scale()) return true;
        if(a.scale() > b.scale()) return false;

        if(a.offsetX() < b.offsetX()) return true;
        if(a.offsetX() > b.offsetX()) return false;

        if(a.offsetY() < b.offsetY()) return true;
        if(a.offsetY() > b.offsetY()) return false;

        if(a.texture().width() < b.texture().width()) return true;
        if(a.texture().width() > b.texture().width()) return false;

        if(a.texture().height() < b.texture().height()) return true;
        if(a.texture().height() > b.texture().height()) return false;

        if(a.texture().padding() < b.texture().padding()) return true;
        if(a.texture().padding() > b.texture().padding()) return false;

        if(a.texture().view().pixelStride() < b.texture().view().pixelStride()) return true;
        if(a.texture().view().pixelStride() > b.texture().view().pixelStride()) return false;

        return memcmp( a.texture().data(),
                       b.texture().data(),
                       a.texture().view().stride() * a.texture().height()
                     ) < 0;
    }
};


//
// SVG inherit flags
//

struct SvgRasterizer::SvgInheritSpec : public SGNode::BasicExtendedData {
    bool penColor;
    bool brushColor;

    inline SvgInheritSpec()
    : penColor  (false)
    , brushColor(false)
    {}

    virtual ~SvgInheritSpec()
    {}

    virtual BasicExtendedData* clone() const
    { return new SvgInheritSpec(*this); }

    inline void combineWith(const SvgInheritSpec* sis)
    {
        if(!sis) return;

        penColor   |= sis->penColor;
        brushColor |= sis->brushColor;
    }

    inline bool isNull() const
    { return !(penColor || brushColor); }
};


//
// SVG style data
//

struct SvgRasterizer::SvgStyleData {
    // Inherit specifiers
    SvgInheritSpec inheritSpec;

    // Pen data
    bool           penSpecified;
    Color          penColor;
    Pt::size_t     penSize;
    Pen::Style     penStyle;
    Pen::CapStyle  penCapStyle;
    Pen::JoinStyle penJoinStyle;
    Pt::uint64_t   penStylePattern;

    // Brush data
    // ### TODO ###

    inline SvgStyleData()
    : penSpecified   (false)
    , penColor       (0, 0, 0, 255)
    , penSize        (1)
    , penStyle       (Pen::Solid)
    , penCapStyle    (Pen::FlatCap)
    , penJoinStyle   (Pen::MiterJoin)
    , penStylePattern(0)
    {}
};


//
// Svg object
//

struct SvgRasterizer::SvgObject {
    const SGNode* sgn;

    inline SvgObject()
    : sgn(0)
    {}

    inline SvgObject(const SGNode* sgn_)
    : sgn(sgn_)
    {}
};


//
// Raster state
//

struct SvgRasterizer::RasterState {
    // Typedefs
    typedef std::map<Pt::String, SvgObject*> SvgObjects;

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
