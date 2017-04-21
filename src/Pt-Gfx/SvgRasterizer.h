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

#include <stack>

#include <Pt/IOError.h>

#include <Pt/Xml/InputSource.h>
#include <Pt/Xml/XmlReader.h>

#include <Pt/Gfx/SGNodeProxy.h>
#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Xml {
    class AttributeList;
}
}


namespace Pt {
namespace Gfx {


class SGNode;


class SvgRasterizer
{
    public:
        SvgRasterizer(std::istream& is, Image& image, const PointF& topLeft);

        ~SvgRasterizer();

        Image& image();

        bool advance();

        // ### TODO: Add support for animated SVG! ###

    private:
        static inline const std::string lcaseStdStr(const std::string & str);
        static inline const std::string lcaseStdStr(const Pt::String& str);
        static inline const Pt::String lcasePtStr(const Pt::String& str);

        static inline const std::string ltrimStdStr(const std::string & str);
        static inline const std::string rtrimStdStr(const std::string & str);
        static inline const std::string lrtrimStdStr(const std::string & str);
        static inline const std::string removeAllSpacesStdStr(const std::string & str);

        static inline double cnvStrToDbl(const std::string& s, const std::string& sectionInfo);
        static inline double cnvStrToDbl(const Pt::String& s, const std::string& sectionInfo);
        static inline const std::string& passValidNumber(const std::string& s, const std::string& sectionInfo);

        static inline const std::vector<std::string> tokenizeWS(const std::string& str);

    private:
        enum AspectRatioMode {
            None,
            XMinYMinMeet,  XMinYMidMeet,  XMinYMaxMeet,
            XMidYMinMeet,  XMidYMidMeet,  XMidYMaxMeet,
            XMaxYMinMeet,  XMaxYMidMeet,  XMaxYMaxMeet,
            XMinYMinSlice, XMinYMidSlice, XMinYMaxSlice,
            XMidYMinSlice, XMidYMidSlice, XMidYMaxSlice,
            XMaxYMinSlice, XMaxYMidSlice, XMaxYMaxSlice
        };

        struct ComparePen;
        struct CompareBrush;

        struct SvgInheritSpec;
        struct SvgStyleData;
        struct SvgObject;

        struct RasterState;

    private:
        RasterState*           _rstate;

        Xml::BinaryInputSource _binaryInputSource;
        Xml::XmlReader         _xmlReader;

    private:
        // Defined in "SvgRasterizer.cpp"
        void renderNextFrame();

        // Defined in "SvgRasterizer_Color.cpp"
        static const Color fromHtmlColor(const std::string& colStr);
        static inline const Color fromHtmlColor(const Pt::String& colStr);

        // Defined in "SvgRasterizer_Lexer.cpp"
        static void lexPathData(std::vector<std::string>& tokens, const std::string& str);
        static void lexStyleData(std::vector<std::string>& tokens, const std::string& str);
        static void lexTransformData(std::vector<std::string>& tokens, const std::string& str);

        static inline void lexPathData(std::vector<std::string>& tokens, const Pt::String& str);
        static inline void lexStyleData(std::vector<std::string>& tokens, const Pt::String& str);
        static inline void lexTransformData(std::vector<std::string>& tokens, const Pt::String& str);

        static inline void copyPenData(SvgStyleData& ssd, const Pen& pen);
        static inline void copyBrushData(SvgStyleData& ssd, const Brush& brush);

        // Defined in "SvgRasterizer_Process.cpp"
        void processSvgElementAttributes(const Xml::StartElement& elem);

        SGNode* processDrawingElement(const Xml::StartElement& elem);
        SGNode* processDrawingElement_g(const SvgStyleData& ssd, const Xml::AttributeList& attrList, const Pt::String& objId);
        SGNode* processDrawingElement_defs(const SvgStyleData& ssd, const Xml::AttributeList& attrList);
        SGNode* processDrawingElement_use(const SvgStyleData& ssd, const Xml::AttributeList& attrList);
        SGNode* processDrawingElement_line(const SvgStyleData& ssd, const Xml::AttributeList& attrList, const Pt::String& objId);

        // Defined in "SvgRasterizer_Util.cpp"
        static const std::string cnvUtf32ToUtf8(const Pt::String& str);
        static double cnvUnitStrToPixels(const std::string& str);

        static void extractStyleData(SvgStyleData& ssd, const SGNode& parent, const Xml::AttributeList& attrList, const std::string& sectionInfo);
        inline void applyStyleData(SGNode& sgn, const SvgStyleData& ssd);

        inline void applyPenOverride(SGNodeProxy* sgn, Pen* pen);
        inline void applyBrushOverride(SGNodeProxy* sgn, Brush* brush);

        void storeSvgObject(const Pt::String& objId, SGNode* sgn, const SvgStyleData& ssd, const std::string& sectionInfo);
        const SGNode* getSvgObject_SGNode(const Pt::String& objId, const std::string& sectionInfo);
};


//
// Include the inline function implementation
//
#include "SvgRasterizer_PrivateStruct.h"
#include "SvgRasterizer_PrivateFunc.h"


} // namespace
} // namespace

#endif
