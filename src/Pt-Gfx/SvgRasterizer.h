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
        SvgRasterizer(std::istream& is, Image& image, const Transform& worldTransform);

        ~SvgRasterizer();

        Image& image();

        bool advance();

    private:
        struct SvgInst;
        struct RasterState;

    private:
        RasterState*           _rstate;

        Xml::BinaryInputSource _binaryInputSource;
        Xml::XmlReader         _xmlReader;

    private:
        static inline const std::string lcaseStdStr(const std::string & str);
        static inline const std::string lcaseStdStr(const Pt::String& str);
        static inline const Pt::String lcasePtStr(const Pt::String& str);

        static inline const std::string ltrimStdStr(const std::string & str);
        static inline const std::string rtrimStdStr(const std::string & str);
        static inline const std::string lrtrimStdStr(const std::string & str);
        static inline const std::string removeAllSpacesStdStr(const std::string & str);

        static inline const std::string& passValidNumber(const std::string& s, const std::string& sectionInfo);

    private:
        static const Color fromHtmlColor(const std::string& colStr);

        static void lexPathData(std::vector<std::string>& tokens, const std::string& str);
        static void lexStyleData(std::vector<std::string>& tokens, const std::string& str);
        static void lexTransformData(std::vector<std::string>& tokens, const std::string& str);
};


// ======================================================================================
// ===== Inlined Private Member Functions ===============================================
// ======================================================================================

const std::string SvgRasterizer::lcaseStdStr(const std::string & str_)
{
    std::string  str = str_;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}

const std::string SvgRasterizer::lcaseStdStr(const Pt::String& str_)
{
    Pt::String str = str_;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str.narrow();
}

const Pt::String SvgRasterizer::lcasePtStr(const Pt::String& str_)
{
    Pt::String str = str_;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}

const std::string SvgRasterizer::ltrimStdStr(const std::string & str)
{
    const size_t idx = str.find_first_not_of(" \t\v\n\r\f");

    if(idx != std::string::npos) return str.substr(idx);
    return str;
}

const std::string SvgRasterizer::rtrimStdStr(const std::string & str)
{
    const size_t idx = str.find_last_not_of(" \t\v\n\r\f");

    if(idx != std::string::npos) return str.substr(0, idx + 1);
    return str;
}

const std::string SvgRasterizer::lrtrimStdStr(const std::string & str)
{ return rtrimStdStr(ltrimStdStr(str)); }

const std::string SvgRasterizer::removeAllSpacesStdStr(const std::string & str_)
{
    std::string str = str_;
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());

    return str;
}

const std::string& SvgRasterizer::passValidNumber(const std::string& s, const std::string& sectionInfo)
{
    char*  end = 0;
    double val = strtod(s.c_str(), &end);

    if(*end || val == HUGE_VAL)
        throw IOError("svg error: " + sectionInfo + ": invalid number '" + s + "'");

    return s;
}


} // namespace
} // namespace

#endif
