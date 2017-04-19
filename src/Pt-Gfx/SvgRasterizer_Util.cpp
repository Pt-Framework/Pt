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

#include "SvgRasterizer.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void SvgRasterizer::processSvgElemParams(RasterState& rs, const Xml::StartElement& elem)
{
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

}


} // namespace
} // namespace
