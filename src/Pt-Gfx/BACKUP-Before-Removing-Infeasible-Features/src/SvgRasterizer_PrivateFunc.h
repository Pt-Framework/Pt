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

inline const std::vector<std::string> SvgRasterizer::tokenizeWS(const std::string& str_)
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
        // If a pen with the same parameters already exists, assign it in place of the new pen
        std::set<Pen>::const_iterator it = _rstate->penSet.find(pen);
        if(it != _rstate->penSet.end()) {
            sgn.setPen(*it);
        }
        // Cache the new pen before assigning it
        else {
            _rstate->penSet.insert(pen);
            sgn.setPen(pen);
        }
    }

    // Set the brush as needed
    // ### TODO ###
    // _rstate->brushSet;
}

inline void SvgRasterizer::applyPenOverride(SGNodeProxy* sgn, Pen* pen)
{
    // If a pen with the same parameters already exists, assign it in place of the new pen
    std::set<Pen>::const_iterator it = _rstate->penSet.find(*pen);
    if(it != _rstate->penSet.end()) {
        *pen = *it;
        sgn->setPenOverride(pen);
    }

    // Cache the new pen before assigning it
    else {
        _rstate->penSet.insert(*pen);
        sgn->setPenOverride(pen);
    }
}

inline void SvgRasterizer::applyBrushOverride(SGNodeProxy* sgn, Brush* brush)
{
    // ### TODO ###
    // _rstate->brushSet;
}
