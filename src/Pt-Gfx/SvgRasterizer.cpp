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

// Just for debugging ;)
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
// ===== Internal Helper Functions ======================================================
// ======================================================================================

inline const std::string cnvLowerCase(const std::string & str_)
{
    std::string  str = str_;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}

inline const std::string cnvLowerCase(const Pt::String& str_)
{
    Pt::String str = str_;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str.narrow();
}


// ======================================================================================
// ===== Private Member Structure Definitions ===========================================
// ======================================================================================

struct SvgRasterizer::RasterState {
    Image&             image;
    ImagePainter2      painter;

    AffineTransform    atrans;
    Path               path;

    std::vector<Pen  > psStack;
    std::vector<Brush> bsStack;

    inline RasterState(Image& image_, const AffineTransform& initialTransform)
    : image  (image_)
    , painter(image_)
    , atrans (initialTransform)
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
}

SvgRasterizer::~SvgRasterizer()
{ delete _rstate; }

Image& SvgRasterizer::image()
{ return _rstate->image; }

bool SvgRasterizer::advance()
{
    // Get the next node
    Xml::Node* node = _xmlReader.advance();
    if(!node) return true;

    // Process the node
    switch(node->type()) {
        case Xml::Node::StartDocument: {
            const Xml::XmlDeclaration* nc = _xmlReader.input()->declaration();
            if(nc && !nc->version().empty()) {
                lprintf("StartDocument : %s\n", cnvLowerCase(nc->version()).c_str());
                lprintf("                %s\n", cnvLowerCase(nc->encoding()).c_str());
            }
            break;
        }

        case Xml::Node::DocType: {
            const Xml::DocType& nc = Xml::nodeCast<Xml::DocType>(*node);
            lprintf("DocType       : %s\n", cnvLowerCase(nc.rootName().local()).c_str());
            lprintf("                %s\n", cnvLowerCase(nc.publicId()        ).c_str());
            lprintf("                %s\n", cnvLowerCase(nc.systemId()        ).c_str());
            break;
        }

        case Xml::Node::StartElement: {
            const Xml::StartElement& nc = Xml::nodeCast<Xml::StartElement>(*node);
            if(cnvLowerCase(nc.name().local()) == "svg") {
                lprintf("StartElement  : %s\n", cnvLowerCase(nc.name().local()).c_str());
                lprintf("                %s\n", cnvLowerCase(nc.namespaceUri()).c_str());
            }
            else {
                lprintf("StartElement  : %s\n", cnvLowerCase(nc.name().local()).c_str());
            }
            const Xml::AttributeList& alist = nc.attributes();
            for(Xml::AttributeList::ConstIterator it = alist.begin(); it != alist.end(); ++it) {
                const Xml::Attribute& a = *it;
                lprintf("    Attribute : %s = %s\n", cnvLowerCase(a.name().local()).c_str(), a.value().narrow().c_str());
            }
            break;
        }

        case Xml::Node::EndElement: {
            const Xml::EndElement& nc = Xml::nodeCast<Xml::EndElement>(*node);
            lprintf("EndElement    : %s\n", cnvLowerCase(nc.name().local()).c_str());
            break;
        }

        case Xml::Node::Characters: {
            //const Xml::Characters& nc = Xml::nodeCast<Xml::Characters>(*node);
            //lprintf("Characters    : %s\n", nc.content().narrow().c_str());
            break;
        }

        default:
            break;
    }

    // Not done yet
    return false;
}


} // namespace
} // namespace
