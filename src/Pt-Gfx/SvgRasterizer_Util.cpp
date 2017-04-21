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

#include "SvgRasterizer.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

double SvgRasterizer::cnvUnitStrToPixels(const std::string& str_)
{
    // Monitor's DPI
    const double MONITOR_DPI = 96; // https://en.wikipedia.org/wiki/Dots_per_inch#Computer_monitor_DPI_standards

    // Remove all white-spaces and convert to lower case
    const std::string& str = lcaseStdStr(removeAllSpacesStdStr(str_));

    // Convert to double and check for invalid value
    char*  end = 0;
    double val = strtod(str.c_str(), &end);

    if(val == HUGE_VAL)
        throw IOError("svg error: invalid number unit specifier '" + str + "'");

    // If there is no unit or the unit is "px", simply return the value
    if(!*end || strcmp(end, "px") == 0) return val;

    if(strcmp(end, "%" ) == 0) return -val;

    if(strcmp(end, "pt") == 0) return MONITOR_DPI * val / 72.00; // (1 / 72 of an inch)
    if(strcmp(end, "pc") == 0) return MONITOR_DPI * val /  6.00; // (1 /  6 of an inch)
    if(strcmp(end, "mm") == 0) return MONITOR_DPI * val / 25.40;
    if(strcmp(end, "cm") == 0) return MONITOR_DPI * val /  2.54;
    if(strcmp(end, "in") == 0) return MONITOR_DPI * val;
    if(strcmp(end, "em") == 0) return val * (10.0 + 1.0 / 15.0); // http://kb.mozillazine.org/Em_units_versus_ex_units
    if(strcmp(end, "ex") == 0) return val *   6.0;               // http://kb.mozillazine.org/Em_units_versus_ex_units

    throw IOError("svg error: invalid number unit specifier '" + str + "'");
}

const std::string SvgRasterizer::cnvUtf32ToUtf8(const Pt::String& str)
{
    std::string utf8;
    utf8.reserve(str.length() * 4);

    // UTF-32 Bytes                 UTF-8 Byte #1   UTF-8 Byte #2   UTF-8 Byte #3   UTF-8 Byte #4
    //                   0AAAAAAA   0AAAAAAA
    //          00000BBB BAAAAAAA   110BBBBA        10AAAAAA
    //          CCCCBBBB BAAAAAAA   1110CCCC        10BBBBBA        10AAAAAA
    // 000DDDDD CCCCBBBB BAAAAAAA   11110DDD        10DDCCCC        10BBBBBA        10AAAAAA

    for(Pt::String::const_iterator it = str.begin(); it != str.end(); ++it) {
        //                                     Max. UTF-32   Replacement
        const Pt::uint32_t ch = (it->value() > 0x0010FFFF) ? 0x0000FFFD : it->value();
        if(ch < 0x00000080) {
            utf8 += static_cast<char>( 0x00 | ( ( ch & 0x0000007F ) >>  0 ) );
        }
        else if(ch < 0x00000800) {
            utf8 += static_cast<char>( 0xC0 | ( ( ch & 0x000007C0 ) >>  6 ) );
            utf8 += static_cast<char>( 0x80 | ( ( ch & 0x0000003F ) >>  0 ) );
        }
        else if(ch < 0x00010000) {
            utf8 += static_cast<char>( 0xE0 | ( ( ch & 0x0000F000 ) >> 12 ) );
            utf8 += static_cast<char>( 0x80 | ( ( ch & 0x00000FC0 ) >>  6 ) );
            utf8 += static_cast<char>( 0x80 | ( ( ch & 0x0000003F ) >>  0 ) );
        }
        else if(ch < 0x00200000) {
            utf8 += static_cast<char>( 0xF0 | ( ( ch & 0x001C0000 ) >> 18 ) );
            utf8 += static_cast<char>( 0x80 | ( ( ch & 0x0003F000 ) >> 12 ) );
            utf8 += static_cast<char>( 0x80 | ( ( ch & 0x00000FC0 ) >>  6 ) );
            utf8 += static_cast<char>( 0x80 | ( ( ch & 0x0000003F ) >>  0 ) );
        }
    }

    return utf8;
}

void SvgRasterizer::storeSvgObject(const Pt::String& objId, const SGNode* sgn, const std::string& sectionInfo)
{
    // Check if an object with the same ID already exists
    RasterState::SvgObjects::iterator it = _rstate->svgObjects.find(objId);

    if(it != _rstate->svgObjects.end()) {
        // Check if the object already has an associated SGNode
        if(it->second->sgn)
            throw IOError("svg error: " + sectionInfo + ": duplicated ID '" + cnvUtf32ToUtf8(objId) + "'");
        // Store the SGNode
        it->second->sgn = sgn;
        return;
    }

    // Create a new object and store it
    _rstate->svgObjects[objId] = new SvgObject(sgn);
}

const SGNode* SvgRasterizer::getSvgObject_SGNode(const Pt::String& objId_, const std::string& sectionInfo)
{
    // Remove the '#' prefix
    const Pt::String& objId = (objId_[0] == '#') ? objId_.substr(1) : objId_;

    // Check if an object with the given ID does exist
    RasterState::SvgObjects::iterator it = _rstate->svgObjects.find(objId);

    if(it == _rstate->svgObjects.end() || !it->second->sgn)
        throw IOError("svg error: " + sectionInfo + ": undefined ID '" + cnvUtf32ToUtf8(objId) + "'");

    // Return the object
    return it->second->sgn;
}


} // namespace
} // namespace
