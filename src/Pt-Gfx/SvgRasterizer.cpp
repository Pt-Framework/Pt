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
//#warning "Just for debugging ;)"
//#include <stdio.h>

#include <Pt/Xml/StartDocument.h>
#include <Pt/Xml/EndDocument.h>
#include <Pt/Xml/DocType.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/EndElement.h>

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
, sgParent  (new SGNode)
{}

SvgRasterizer::RasterState::~RasterState()
{
    delete sgParent;

    clearAllCaches();
}

void SvgRasterizer::RasterState::clearAllCaches()
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
            const std::string&       enam = lcaseStdStr(elem.name().local());
            // Check for the SVG opening element
            if(enam == "svg") {
                // Check if we have already got the opening element
                if(_rstate->gotStart) throw IOError("svg error: multiple SVG elements in one document");
                // Check the namespace URI
                if(lcaseStdStr(elem.namespaceUri()) != "http://www.w3.org/2000/svg")
                    throw IOError("svg error: invalid SVG namespace URI");
                // Process the parameters
                processSvgElementParameters(elem);
                // Store the parent node to the stack
                _rstate->sgStack.push(_rstate->sgParent);
                // Set flag
                _rstate->gotStart = true;
            }
            // Process the element as a drawing element
            else {
                _rstate->sgStack.push( processDrawingElement(elem) );
            }
            break;
        }

        case Xml::Node::EndElement: {
            // Convert the element
            const Xml::EndElement& elem = Xml::nodeCast<Xml::EndElement>(*node);
            // Check for the SVG closing element
            if(lcaseStdStr(elem.name().local()) == "svg") {
                // The only node left in the node stack must be the parent node
                if(_rstate->sgStack.size() != 1 && _rstate->sgStack.top() != _rstate->sgParent)
                    throw IOError("svg error: document structure error");
                // Clear the node stack and caches
                _rstate->sgStack.pop();
                _rstate->clearAllCaches();
                // Set flag
                _rstate->gotEnd = true;
            }
            // Process the element as a drawing element
            else {
                _rstate->sgStack.pop();
            }
            break;
        }

        default:
            // Ignore other elements
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
    // Initialize the rendering-related data as needed
    if(!_rstate->renderInit) {
        // Determine the viewport width and height
             if(_rstate->vpWidth  == 0.0) _rstate->vpWidth  =                             _rstate->image.width ();
        else if(_rstate->vpWidth  <  0  ) _rstate->vpWidth  = -_rstate->vpWidth  * 0.01 * _rstate->image.width ();
             if(_rstate->vpHeight == 0.0) _rstate->vpHeight =                             _rstate->image.height();
        else if(_rstate->vpHeight <  0  ) _rstate->vpHeight = -_rstate->vpHeight * 0.01 * _rstate->image.height();
        // Determine the viewbox width and height
        if(_rstate->vbW <= 0.0) _rstate->vbW = _rstate->vpWidth;
        if(_rstate->vbH <= 0.0) _rstate->vbH = _rstate->vpHeight;
        // Calculate the delta between the viewport and viewbox
        const double pbDltW = _rstate->vpWidth  - _rstate->vbW;
        const double pbDltH = _rstate->vpHeight - _rstate->vbH;
        // Calculate the ratio between the viewport and viewbox
        const double pbRatW = _rstate->vpWidth  / _rstate->vbW;
        const double pbRatH = _rstate->vpHeight / _rstate->vbH;
        // Determine the viewport-viewbox scaling
        double xs = 1;
        double ys = 1;
        switch(_rstate->arMode) {
            // Non-uniform scaling
            case None :
                xs = pbRatW;
                ys = pbRatH;
                break;
            // The entire viewbox is visible within the viewport
            case XMinYMinMeet : /* Fallthrough */
            case XMinYMidMeet : /* Fallthrough */
            case XMinYMaxMeet : /* Fallthrough */
            case XMidYMinMeet : /* Fallthrough */
            case XMidYMidMeet : /* Fallthrough */
            case XMidYMaxMeet : /* Fallthrough */
            case XMaxYMinMeet : /* Fallthrough */
            case XMaxYMidMeet : /* Fallthrough */
            case XMaxYMaxMeet :
                xs = std::min(pbRatW, pbRatH);
                ys = xs;
                break;
            // The entire viewport is covered by the viewbox
            case XMinYMinSlice : /* Fallthrough */
            case XMinYMidSlice : /* Fallthrough */
            case XMinYMaxSlice : /* Fallthrough */
            case XMidYMinSlice : /* Fallthrough */
            case XMidYMidSlice : /* Fallthrough */
            case XMidYMaxSlice : /* Fallthrough */
            case XMaxYMinSlice : /* Fallthrough */
            case XMaxYMidSlice : /* Fallthrough */
            case XMaxYMaxSlice :
                xs = std::max(pbRatW, pbRatH);
                ys = xs;
                break;
        }
        // Determine the viewport-viewbox translation
        double xt = 0;
        double yt = 0;
        switch(_rstate->arMode) {
            // Top-left
            case None          : /* Fallthrough */
            case XMinYMinMeet  : /* Fallthrough */
            case XMinYMinSlice :                                       break;
            // Middle-left
            case XMinYMidMeet  : /* Fallthrough */
            case XMinYMidSlice :                    yt = pbDltH * 0.5; break;
            // Bottom-left
            case XMinYMaxMeet  : /* Fallthrough */
            case XMinYMaxSlice :                    yt = pbDltH * 1.0; break;
            // Top-center
            case XMidYMinMeet  : /* Fallthrough */
            case XMidYMinSlice : xt = pbDltW * 0.5;                    break;
            // Middle-center
            case XMidYMidMeet  : /* Fallthrough */
            case XMidYMidSlice : xt = pbDltW * 0.5; yt = pbDltH * 0.5; break;
            // Bottom-center
            case XMidYMaxMeet  : /* Fallthrough */
            case XMidYMaxSlice : xt = pbDltW * 0.5; yt = pbDltH * 1.0; break;
            // Top-right
            case XMaxYMinMeet  : /* Fallthrough */
            case XMaxYMinSlice : xt = pbDltW * 1.0;                    break;
            // Middle-right
            case XMaxYMidMeet  : /* Fallthrough */
            case XMaxYMidSlice : xt = pbDltW * 1.0; yt = pbDltH * 0.5; break;
            // Bottom-right
            case XMaxYMaxMeet  : /* Fallthrough */
            case XMaxYMaxSlice : xt = pbDltW * 1.0; yt = pbDltH * 1.0; break;
        }
        // Initialize the viewport-viewbox transform
        _rstate->vpbTransform.identity();
        _rstate->vpbTransform.translate(-_rstate->vbX, -_rstate->vbY);
        _rstate->vpbTransform.scale(xs, ys);
        _rstate->vpbTransform.translate(xt, yt);
        // Translate to the user-specified top-left coordinate
        _rstate->vpbTransform.translate(_rstate->topLeft.x(), _rstate->topLeft.y());
        // Set flag
        _rstate->renderInit = true;
    }

    // ### TODO: for animated SVG, update the scene graph nodes here! ###

    // Render the scene graph
    _rstate->sgParent->draw(_rstate->painter, &_rstate->vpbTransform);
}


} // namespace
} // namespace
