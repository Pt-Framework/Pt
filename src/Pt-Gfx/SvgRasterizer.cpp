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

// Just for debugging ;)
#warning "Just for debugging ;)"
#include <stdio.h>

#include <Pt/Xml/StartDocument.h>
#include <Pt/Xml/EndDocument.h>
#include <Pt/Xml/DocType.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/EndElement.h>

#include <Pt/Gfx/Path.h>

#include "SvgRasterizer.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== SvgRasterizer::SvgObject =======================================================
// ======================================================================================

struct SvgRasterizer::SvgObject {
};


// ======================================================================================
// ===== SvgRasterizer::RasterState =====================================================
// ======================================================================================

SvgRasterizer::RasterState::RasterState(Image& image_, const PointF& topLeft_)
: gotStart  (false)
, gotEnd    (false)
, renderInit(false)
, image     (image_)
, painter   (image_)
, topLeft   (topLeft_)
, vpWidth   (-100) // The default viewport width  is 100% of the target image width
, vpHeight  (-100) // The default viewport height is 100% of the target image height
, vbX       (0)    // The default viewbox top-left coordinate is (0, 0)
, vbY       (0)    // The default viewbox top-left coordinate is (0, 0)
, vbW       (0)    // The default viewbox width  is the the same with the viewport width
, vbH       (0)    // The default viewbox height is the the same with the viewport height
, arMode    (XMidYMidMeet)
{}

SvgRasterizer::RasterState::~RasterState()
{
    for(SvgObjects::iterator it = svgObjects.begin(); it != svgObjects.end(); ++it) {
        delete it->second;
    }
    svgObjects.clear();
}


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

SvgRasterizer::SvgRasterizer(std::istream& is, Image& image, const PointF& topLeft)
: _rstate           (new RasterState(image, topLeft))
, _binaryInputSource(is)
, _xmlReader        (_binaryInputSource)
{
    _xmlReader.reportStartDocument(true);
    _xmlReader.reportDocType(true);

#if 0
    lprintf("10   = %f\n", cnvUnitStrToPixels("10  "));
    lprintf("10px = %f\n", cnvUnitStrToPixels("10px"));
    lprintf("10pt = %f\n", cnvUnitStrToPixels("10pt"));
    lprintf("10pc = %f\n", cnvUnitStrToPixels("10pc"));
    lprintf("10mm = %f\n", cnvUnitStrToPixels("10mm"));
    lprintf("10cm = %f\n", cnvUnitStrToPixels("10cm"));
    lprintf("10in = %f\n", cnvUnitStrToPixels("10in"));
    lprintf("10em = %f\n", cnvUnitStrToPixels("10em"));
    lprintf("10ex = %f\n", cnvUnitStrToPixels("10ex"));
    throw 0
#endif

#if 0
    std::vector<std::string> tokens;

    const std::string str = "M1,0 L10,10l10 10zl200,200z M100,100 l10,20 30,10z";
    lexPathData(tokens, str);

    //const std::string str = "stroke:green; stroke-width:8";
    //lexStyleData(tokens, str);

    //const std::string str = "translate(10 12) translate (10) ; translate(10,10) rotate(20) rotate(20,10,-11) scale(10) scale(10,2) skewX(1) skewY(1) matrix(0,1,2,3,4,5)";
    //lexTransformData(tokens, str);

    for(size_t i = 0; i < tokens.size(); ++i) {
        lprintf("%s\n", tokens[i].c_str());
    }
    throw 0;
#endif
}

SvgRasterizer::~SvgRasterizer()
{ delete _rstate; }

Image& SvgRasterizer::image()
{ return _rstate->image; }

bool SvgRasterizer::advance()
{
    // Render the (next) frame if we have got a complete SVG body
    if(_rstate->gotStart && _rstate->gotEnd) {
        renderNextFrame();
        return true;
    }

    // Get the next node
    Xml::Node* node = _xmlReader.advance();
    if(!node) {
        // Check if we have got a complete SVG body
        if(!_rstate->gotStart || !_rstate->gotEnd) throw IOError("svg error: premature end of document");
        return true;
    }

    // Process the node
    switch(node->type()) {
        case Xml::Node::StartDocument: {
            // Get the XML declaration
            const Xml::XmlDeclaration* elem = _xmlReader.input()->declaration();
            // Check the XML declaration
            if(!elem->version ().empty() && lcaseStdStr(elem->version ()) != "1.0"  ) throw IOError("svg error: invalid XML version" );
            if(!elem->encoding().empty() && lcaseStdStr(elem->encoding()) != "utf-8") throw IOError("svg error: invalid XML encoding");
            break;
        }

        case Xml::Node::EndDocument: {
            // Check if we have got a complete SVG body
            if(!_rstate->gotStart || !_rstate->gotEnd) throw IOError("svg error: premature end of document");
            break;
        }

        case Xml::Node::DocType: {
            // Convert the element
            const Xml::DocType& elem = Xml::nodeCast<Xml::DocType>(*node);
            // Check the document type
            if(lcaseStdStr(elem.rootName().local()) != "svg")
                throw IOError("svg error: invalid SVG DTD root name");
            const std::string& pubId = lcaseStdStr(elem.publicId());
            if( pubId != "-//w3c//dtd svg 1.0//en" &&
                pubId != "-//w3c//dtd svg 1.1//en"
              )
                throw IOError("svg error: invalid SVG DTD public ID");
            const std::string& sysId = lcaseStdStr(elem.systemId());
            if( sysId != "http://www.w3.org/tr/2001/rec-svg-20010904/dtd/svg10.dtd" &&
                sysId != "http://www.w3.org/tr/2001/rec-svg-20010904/dtd/svg11.dtd"
              )
                throw IOError("svg error: invalid SVG DTD public ID");
            break;
        }

        case Xml::Node::StartElement: {
            // Convert the element
            const Xml::StartElement& elem = Xml::nodeCast<Xml::StartElement>(*node);
            // Check for the SVG opening element
            if(lcasePtStr(elem.name().local()) == "svg") {
                // Check if we have already got the opening element
                if(_rstate->gotStart) throw IOError("svg error: multiple SVG elements in one document");
                // Check the namespace URI
                if(lcaseStdStr(elem.namespaceUri()) != "http://www.w3.org/2000/svg")
                    throw IOError("svg error: invalid SVG namespace URI");
                // Process the parameters
                processSvgElemParams(*_rstate, elem);
                // Set flag
                _rstate->gotStart = true;
            }
            break;
        }

        case Xml::Node::EndElement: {
            // Convert the element
            const Xml::EndElement& elem = Xml::nodeCast<Xml::EndElement>(*node);
            // Check for the SVG closing element
            if(lcasePtStr(elem.name().local()) == "svg") _rstate->gotEnd = true;
            break;
        }

        case Xml::Node::Characters: {
            /*
            // Convert the element
            const Xml::Characters& elem = Xml::nodeCast<Xml::Characters>(*node);
            // Dump the element
            lprintf("Characters    : %s\n", elem.content().narrow().c_str());
            //*/
            break;
        }

        default:
            // Ignore unsupported elements
            break;
    }

    // Not done yet
    return false;
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void SvgRasterizer::renderNextFrame()
{
    if(!_rstate->renderInit) {
    }

/*
 *  Auto-determine these values as needed:
    double          vpWidth;  // Width  of the viewport (negative: relative percentage; positive: absolute pixels)
    double          vpHeight; // Height of the viewport (negative: relative percentage; positive: absolute pixels)
    double          vbX;      // Viewbox top-left X coordinate
    double          vbY;      // Viewbox top-left Y coordinate
    double          vbW;      // Viewbox width
    double          vbH;      // Viewbox height
 *  Create vp-vb transform
    AspectRatioMode arMode;   // Aspect ratio mode
    Transform       vpbTransform; // The viewport-viewbox transform (projection-view matrix in OpenGL worlds ;)

*/
}


} // namespace
} // namespace
