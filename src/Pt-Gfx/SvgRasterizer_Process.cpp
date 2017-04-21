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

#include <Pt/Xml/StartElement.h>

#include <Pt/Gfx/SGNodeLine.h>
#include <Pt/Gfx/SGNodeRectangle.h>
#include <Pt/Gfx/SGNodeEllipse.h>
#include <Pt/Gfx/SGNodeArc.h>
#include <Pt/Gfx/SGNodeProxy.h>

#include "SvgRasterizer.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void SvgRasterizer::processSvgElementAttributes(const Xml::StartElement& elem)
{
    // Get the attributes
    const Xml::AttributeList& attrList = elem.attributes();

    // Walk through the attributes
    for(Xml::AttributeList::ConstIterator it = attrList.begin(); it != attrList.end(); ++it) {
        const std::string& snam = lcaseStdStr( it->name ().local() );
        const std::string& sval = lcaseStdStr( it->value()         );
        if(snam == "width") {
            _rstate->vpWidth = cnvUnitStrToPixels(sval);
        }
        else if(snam == "height") {
            _rstate->vpHeight = cnvUnitStrToPixels(sval);
        }
        else if(snam == "viewbox") {
            // Tokenize
            const std::vector<std::string>& tok = tokenizeWS(sval);
            if(tok.size() != 4)
                throw IOError("svg error: invalid viewBox specifier '" + sval + "'");
            // Convert to doubles
            const double x = cnvStrToDbl(tok[0], "viewBox");
            const double y = cnvStrToDbl(tok[1], "viewBox");
            const double w = cnvStrToDbl(tok[2], "viewBox");
            const double h = cnvStrToDbl(tok[3], "viewBox");
            _rstate->vbX = (x <= 0) ? 0 : x;
            _rstate->vbY = (y <= 0) ? 0 : y;
            _rstate->vbW = (w <= 0) ? 0 : w;
            _rstate->vbH = (h <= 0) ? 0 : h;
        }
        else if(snam == "preserveaspectratio") {
            // Tokenize
            const std::vector<std::string>& tok = tokenizeWS(sval);
            // CHeck the number of tokens
            if(tok.size() < 1 || tok.size() > 2)
                throw IOError("svg error: invalid preserveAspectRatio specifier '" + sval + "'");
            // Check for "meet" or "slice"
            bool meet = true;
            if(tok.size() == 2) {
                     if(tok.back() == "meet" ) { /* Does nothing */ }
                else if(tok.back() == "slice") { meet = false; }
                else
                    throw IOError("svg error: invalid preserveAspectRatio specifier '" + sval + "'");
            }
            // Process the directive
                 if(tok[0] == "none"    ) _rstate->arMode = None;
            else if(tok[0] == "xminymin") _rstate->arMode = meet ? XMinYMinMeet : XMinYMinSlice;
            else if(tok[0] == "xminymid") _rstate->arMode = meet ? XMinYMidMeet : XMinYMidSlice;
            else if(tok[0] == "xminymax") _rstate->arMode = meet ? XMinYMaxMeet : XMinYMaxSlice;
            else if(tok[0] == "xmidymin") _rstate->arMode = meet ? XMidYMinMeet : XMidYMinSlice;
            else if(tok[0] == "xmidymid") _rstate->arMode = meet ? XMidYMidMeet : XMidYMidSlice;
            else if(tok[0] == "xmidymax") _rstate->arMode = meet ? XMidYMaxMeet : XMidYMaxSlice;
            else if(tok[0] == "xmaxymin") _rstate->arMode = meet ? XMaxYMinMeet : XMaxYMinSlice;
            else if(tok[0] == "xmaxymid") _rstate->arMode = meet ? XMaxYMidMeet : XMaxYMidSlice;
            else if(tok[0] == "xmaxymax") _rstate->arMode = meet ? XMaxYMaxMeet : XMaxYMaxSlice;
            else                          throw IOError("svg error: invalid preserveAspectRatio specifier '" + sval + "'");
        }
    }
}

SGNode* SvgRasterizer::processDrawingElement(const Xml::StartElement& elem)
{
    // Get the parent node
    SGNode& parent = *_rstate->sgStack.top();

    // Get the element type
    const std::string& elemType = lcaseStdStr(elem.name().local());

    // Get the attributes
    const Xml::AttributeList& attrList = elem.attributes();

    // Extract the object ID
    const Pt::String& objId = attrList.has("id") ? attrList.get("id") : Pt::String();

    // Extract the style data
    SvgStyleData ssd;
    extractStyleData(ssd, parent, attrList, elemType);

    // Process based on the element type
    SGNode* sgn = 0;

    // ### TODO: Complete them ! ###

         if(elemType == "g"   ) sgn = processDrawingElement_g   (ssd, attrList, objId);
    else if(elemType == "defs") sgn = processDrawingElement_defs(ssd, attrList       );
    else if(elemType == "use" ) sgn = processDrawingElement_use (ssd, attrList       );
    else if(elemType == "line") sgn = processDrawingElement_line(ssd, attrList, objId);

    if(!sgn) return 0;

    // Add the newly created node to the parent and return it
    return &parent.addChild(sgn);
}

SGNode* SvgRasterizer::processDrawingElement_g(const SvgStyleData& ssd, const Xml::AttributeList& attrList, const Pt::String& id)
{
    // Create a new node and apply the style
    SGNode* sgn = new SGNode();
    applyStyleData(*sgn, ssd);

    // Store the node in a list of objects as needed
    if(!id.empty()) storeSvgObject(id, sgn, "g");

    // Return the newly created node
    return sgn;
}

SGNode* SvgRasterizer::processDrawingElement_defs(const SvgStyleData& ssd, const Xml::AttributeList& attrList)
{
    // ### TODO ###

    return new SGNode();
}

SGNode* SvgRasterizer::processDrawingElement_use(const SvgStyleData& ssd, const Xml::AttributeList& attrList)
{
    // ### TODO ###

    return new SGNode();
}

SGNode* SvgRasterizer::processDrawingElement_line(const SvgStyleData& ssd, const Xml::AttributeList& attrList, const Pt::String& id)
{
    // Extract the coordinates
    if( !attrList.has("x1") || !attrList.has("y1") || !attrList.has("x2") || !attrList.has("y2") )
        throw IOError("svg error: line: missing attribute");

    const double x1 = cnvStrToDbl(attrList.get("x1"), "line");
    const double y1 = cnvStrToDbl(attrList.get("y1"), "line");
    const double x2 = cnvStrToDbl(attrList.get("x2"), "line");
    const double y2 = cnvStrToDbl(attrList.get("y2"), "line");

    // Create a new node and apply the style
    SGNodeLine* sgn = new SGNodeLine( SGNode::RenderStroke, PointF(x1, y1), PointF(x2, y2) );
    applyStyleData(*sgn, ssd);

    // Store the node in a list of objects as needed
    if(!id.empty()) storeSvgObject(id, sgn, "line");

    // Return the newly created node
    return sgn;
}


} // namespace
} // namespace
