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

#warning "Just for debugging ;)"
#include <stdio.h>

#include <Pt/Xml/StartDocument.h>
#include <Pt/Xml/DocType.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Xml/Characters.h>

#include <Pt/Gfx/Path.h>

#include "SvgRasterizer.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Private Member Structure Definitions ===========================================
// ======================================================================================

struct SvgRasterizer::RasterState {
    bool               gotStart; // State flags
    bool               gotEnd;   // ---

    Image&             image;    // Target image
    ImagePainter2      painter;  // Target painter

    Pen                pen;      // Active pen
    Brush              brush;    // Active brush
    AffineTransform    atrans;   // Active affine transform
    Path               path;     // Working path

    std::vector<Pen  > psStack;  // Pen stack
    std::vector<Brush> bsStack;  // Brush stack

    inline RasterState(Image& image_, const AffineTransform& initialTransform)
    : gotStart(false)
    , gotEnd  (false)
    , image   (image_)
    , painter (image_)
    , pen     (Color::fromRgb8(0, 0, 0, 255), 1, Pen::Solid, Pen::ButtCap, Pen::MiterJoin)
    , brush   (Color::fromRgb8(0, 0, 0, 255))
    , atrans  (initialTransform)
    {}
};


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

SvgRasterizer::SvgRasterizer(std::istream& is, Image& image, const AffineTransform& worldTransform)
: _rstate           (new RasterState(image, worldTransform))
, _binaryInputSource(is)
, _xmlReader        (_binaryInputSource)
{
    _xmlReader.reportStartDocument(true);
    _xmlReader.reportDocType(true);

    std::vector<std::string> tokens;

    //const std::string str = "M0,0 L10,10l10 10zl200,200z";
    //lexPathData(tokens, str);

    const std::string str = "stroke:green; stroke-width:8";
    lexStyleData(tokens, str);

    for(size_t i = 0; i < tokens.size(); ++i) {
        lprintf("%s\n", tokens[i].c_str());
    }
    throw 0;
}

SvgRasterizer::~SvgRasterizer()
{ delete _rstate; }

Image& SvgRasterizer::image()
{ return _rstate->image; }

bool SvgRasterizer::advance()
{
    // Get the next node
    Xml::Node* node = _xmlReader.advance();
    if(!node) {
        // Check if we have got a complete SVG body
        if(!_rstate->gotStart || !_rstate->gotEnd) throw IOError("svg error: premature end of document");
        // All done!
        lprintf("DONE!\n");
        return true;
    }

    // Process the node
    switch(node->type()) {
        case Xml::Node::StartDocument: {
            // Get the XML declaration
            const Xml::XmlDeclaration* elem = _xmlReader.input()->declaration();
            // Dump the XML declaration
            if(elem && !elem->version().empty()) {
                lprintf("StartDocument : %s\n", lcaseStdStr(elem->version ()).c_str());
                lprintf("                %s\n", lcaseStdStr(elem->encoding()).c_str());
            }
            break;
        }

        case Xml::Node::EndDocument: {
            // Check if we have got a complete SVG body
            if(!_rstate->gotStart || !_rstate->gotEnd) throw IOError("svg error: premature end of document");
            // Dump the element
            lprintf("EndDocument   :\n");
            break;
        }

        case Xml::Node::DocType: {
            // Convert the element
            const Xml::DocType& elem = Xml::nodeCast<Xml::DocType>(*node);
            // Dump the element
            lprintf("DocType       : %s\n", lcaseStdStr(elem.rootName().local()).c_str());
            lprintf("                %s\n", lcaseStdStr(elem.publicId()        ).c_str());
            lprintf("                %s\n", lcaseStdStr(elem.systemId()        ).c_str());
            break;
        }

        case Xml::Node::StartElement: {
            // Convert the element
            const Xml::StartElement& elem = Xml::nodeCast<Xml::StartElement>(*node);
            // Check for the SVG opening element
            if(lcasePtStr(elem.name().local()) == L"svg") {
                // Check if we have got the opening element
                if(_rstate->gotStart) throw IOError("svg error: multiple main body");
                // Set flag
                _rstate->gotStart = true;
            }
            // Dump the element
            if(lcaseStdStr(elem.name().local()) == "svg") {
                lprintf("StartElement  : %s\n", lcaseStdStr(elem.name().local()).c_str());
                lprintf("                %s\n", lcaseStdStr(elem.namespaceUri()).c_str());
            }
            else {
                lprintf("StartElement  : %s\n", lcaseStdStr(elem.name().local()).c_str());
            }
            const Xml::AttributeList& alist = elem.attributes();
            for(Xml::AttributeList::ConstIterator it = alist.begin(); it != alist.end(); ++it) {
                const Xml::Attribute& a = *it;
                lprintf("    Attribute : %s = %s\n", lcaseStdStr(a.name().local()).c_str(), a.value().narrow().c_str());
            }
            break;
        }

        case Xml::Node::EndElement: {
            // Convert the element
            const Xml::EndElement& elem = Xml::nodeCast<Xml::EndElement>(*node);
            // Check for the SVG closing element
            if(lcasePtStr(elem.name().local()) == L"svg") _rstate->gotEnd = true;
            // Dump the element
            lprintf("EndElement    : %s\n", lcaseStdStr(elem.name().local()).c_str());
            break;
        }

        case Xml::Node::Characters: {
            // Convert the element
            //const Xml::Characters& elem = Xml::nodeCast<Xml::Characters>(*node);
            // Dump the element
            //lprintf("Characters    : %s\n", elem.content().narrow().c_str());
            break;
        }

        default:
            // Dump the element
            lprintf("node->type()  : %d\n", node->type());
            break;
    }

    // Not done yet
    return false;
}


} // namespace
} // namespace
